/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: S60 MCPR states implementation
*
*/

/**
@file s60mcprstates.cpp
S60 MCPR states implementation
*/

#include <cdblen.h>

#include "s60mcprstates.h"

using namespace S60MCprStates;
using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

// -----------------------------------------------------------------------------
// THandleMPMStatusChange::DoL
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT(THandleMPMStatusChange, NetStateMachine::MStateTransition, TContext)
void THandleMPMStatusChange::DoL()
    {
    TCFControlProvider::TDataClientStatusChange& msg = 
        Messages::message_cast<TCFControlProvider::TDataClientStatusChange>(iContext.iMessage);

    // Get MCPR
    CS60MetaConnectionProvider& node = iContext.Node();

    // Get the policy server's selection result 
    TInt iapid = node.PolicyPrefs().IapId();
    
    if ( iapid == 0 )
        {
        // Filter zeros out.
        // We don't need information of IPCPR status, we only care real connections.
        }
    else if ( msg.iValue == TCFControlProvider::TDataClientStatusChange::EStarted && 
              node.RequestPermissionToSendStarted() )
        {
        S60MCPRLOGSTRING2("S60MCPR<%x>::THandleMPMStatusChange::DoL() calling IAPConnectionStartedL IAP %d",(TInt*)&iContext.Node(),iapid)
        node.Policy()->IAPConnectionStartedL( iapid );  // codescanner::leave
        }
    else if ( msg.iValue == TCFControlProvider::TDataClientStatusChange::EStopped &&
              node.RequestPermissionToSendStopped() )
        {
        S60MCPRLOGSTRING2("S60MCPR<%x>::THandleMPMStatusChange::DoL() calling IAPConnectionStoppedL IAP %d",(TInt*)&iContext.Node(),iapid)
        // Send request.
        // The problem with IAPConnectionStoppedL() is that to MPM it means
        // that IAP has gone down but actually the provider has just left.
        // policy->IAPConnectionStoppedL( node.iIapId );
        // Hence we mimic the situtation that application leaves the connection.
        node.Policy()->ApplicationLeavesConnectionL( iapid );  // codescanner::leave
        node.Policy()->ApplicationConnectionEndsL();  // codescanner::leave
        }
    }

// -----------------------------------------------------------------------------
// TReselectBestIAP::DoL
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT(TReselectBestIAP, NetStateMachine::MStateTransition, TContext)
void TReselectBestIAP::DoL()
    {
    S60MCPRLOGSTRING1("S60MCPR<%x>::TReselectBestIAP::DoL()",(TInt*)&iContext.Node())
    
    // Get MCPR
    CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

    // Build up the PolicyRequest.
    TConnPref* polpref = (TConnPref*)new ( ELeave ) TPolicyConnPref();  // codescanner::leave
    CleanupStack::PushL( polpref );  // codescanner::leave

    CReselectBestIAPCb* cb = new( ELeave ) CReselectBestIAPCb( node, iContext.iNodeActivity );  // codescanner::leave
    CleanupStack::PushL( cb );  // codescanner::leave

    // Send request
    node.Policy()->ReselectIAPL( polpref, cb );  // codescanner::leave

    // Pop three objects.
    CleanupStack::Pop( cb );
    CleanupStack::Pop( polpref );
    // Pop pref and cb objects.
    }


// -----------------------------------------------------------------------------
// TRequestReConnect::DoL
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TRequestReConnect, NetStateMachine::MStateTransition, TContext )
void TRequestReConnect::DoL() // codescanner::leave
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
    S60MCPRLOGSTRING1("S60MCPR<%x>::TRequestReConnect::DoL()",(TInt*)&iContext.Node())
    // We have AP that will go down. 
    // And the other AP that will go up.
    RNodeInterface* stoppingSP = NULL;
    RNodeInterface* startingSP = NULL;

    // Get the MCPR
    CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
    
    // At this point MCPR has selected two active IPProtoMCPRs.
    // The one that will be started is the one MPM selected and
    // the one which doesn't match the policy preferences is the
    // old selection and needs to be disconnected.
    //
    TUint32 newAP = node.PolicyPrefs().IapId();
    ASSERT( newAP > 0 );

    // Choose Service Providers to work on
    TClientIter<TDefaultClientMatchPolicy> iter = 
        iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider));
    
    RMetaServiceProviderInterface* itf = NULL;
    
    for ( itf = (RMetaServiceProviderInterface*)iter++;
             ( itf != NULL && ( stoppingSP == NULL || startingSP == NULL ) );
             itf = (RMetaServiceProviderInterface*)iter++ )
        {
        if ( itf->Flags() & TCFClientType::EActive )
            {
            // Select one that shall be stopped.
            //
            if ( itf->ProviderInfo().APId() != newAP )
                {
                stoppingSP = itf;
                itf->ClearFlags( TCFClientType::EActive );
                }
            // Select one that shall be started
            //
            else if ( itf->ProviderInfo().APId() == newAP )
                {
                startingSP = itf;
                itf->ClearFlags( TCFClientType::EActive );
                }
            // no else.
            //
            }
        }
    
    if ( stoppingSP == NULL )
        { 
        // If the above loop did not select a stoppingSP, the only reasonable chance is that
        // this is one of the rare cases where reconnection is done to the same service provider.
        S60MCPRLOGSTRING1("S60MCPR<%x>::TRequestReConnect::DoL() - reconnection to same service provider",(TInt*)&iContext.Node())
        stoppingSP = startingSP;
        }

    //If there is no other Service Provider to try, return KErrNotFound
    if ( startingSP == NULL )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TRequestReConnect::DoL() - no more choices, abandoning recovery.",(TInt*)&iContext.Node())
        User::Leave( KErrNotFound );  // codescanner::leave
        }

    //Diagnostinc - there must be a data client or we cannot be here
    __ASSERT_DEBUG(iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData)),
                   User::Panic(KS60MCprPanic, KPanicNoDataClient));

    iContext.iNodeActivity->PostRequestTo( iContext.NodeId(),
                                           TCFMcpr::TReConnect( stoppingSP->RecipientId(), 
                                                                startingSP->RecipientId()).CRef() );
    }


// -----------------------------------------------------------------------------
// TProcessError::DoL
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TProcessError, NetStateMachine::MStateTransition, TContext )
void TProcessError::DoL() // codescanner::leave
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
    // Get the MCPR
    CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

    TInt error( KErrNone );

    // Get the error from the message or from the activity.
    // Since this transition could be triggered from almost anywhere, 
    // give preference to current Message.
    if ( iContext.iMessage.IsMessage<Messages::TEBase::TError>() )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TProcessError::DoL() Using iMessage as error source",(TInt*)&iContext.Node())
        error = message_cast<TEBase::TError>(&iContext.iMessage)->iValue;
        }
    else
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TProcessError::DoL() Using CS60ErrorRecoveryActivity as error source",(TInt*)&iContext.Node())
        // Get the activity.
        CS60ErrorRecoveryActivity& activity = 
            static_cast<CS60ErrorRecoveryActivity&>(*iContext.iNodeActivity);
        Messages::TErrContext err =  activity.iOriginalErrContext;
        error = err.iStateChange.iError;
        }

    ASSERT( error != KErrNone );
    
    // Create the callback that will eventually create the message that completes this state.
    CProcessErrorCb* cb = new( ELeave ) CProcessErrorCb( node, iContext.iNodeActivity );  // codescanner::leave
    CleanupStack::PushL( cb );  // codescanner::leave
    // We always need to ask MPM to handle the error.
    node.Policy()->ProcessErrorL( error, cb );  // codescanner::leave
    CleanupStack::Pop( cb );
    // After this we wait for cancellation/error/completion.
    }


// -----------------------------------------------------------------------------
// TAwaitingSelectNextLayerCompletedOrError::Accept
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TAwaitingSelectNextLayerCompletedOrError, NetStateMachine::MState, TContext )
TBool TAwaitingSelectNextLayerCompletedOrError::Accept()
    {
    TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
    if( msg )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayerCompletedOrError::Accept() TError",(TInt*)&iContext.Node())
        iContext.iNodeActivity->SetError(msg->iValue);
        return ETrue;
        }
    if (iContext.iMessage.IsMessage<TCFSelector::TSelectComplete>())
        {
        if (message_cast<TCFSelector::TSelectComplete>(iContext.iMessage).iNodeId.IsNull())
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingSelectNextLayerCompletedOrError::Accept() TSelectComplete",(TInt*)&iContext.Node())
            return ETrue;
            }
        //Consume any other TSelectComplete messages
        iContext.iMessage.ClearMessageId();
        }
    return EFalse;
    }



// -----------------------------------------------------------------------------
// TAwaitingReselectBestIAPCompleteOrError::Accept
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TAwaitingReselectBestIAPCompleteOrError, NetStateMachine::MState, TContext )
TBool TAwaitingReselectBestIAPCompleteOrError::Accept()
    {
    if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg>() )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingReselectBestIAPCompleteOrError::Accept() TMPMReselectBestIAPCompletedMsg Accepted",(TInt*)&iContext.Node())
        
        // Reselect completed.
        return ETrue;
        }
    else if ( iContext.iMessage.IsMessage<Messages::TEBase::TError>() )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingReselectBestIAPCompleteOrError::Accept() TError",(TInt*)&iContext.Node())
        // Some error possibly sent by the MPM. Must be handled by the activity.
        Messages::TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
        iContext.iNodeActivity->SetError(msg->iValue);
        return ETrue;
        }
    else if ( iContext.iMessage.IsMessage<Messages::TEBase::TCancel>() )
        {
        // Cancellation from the RConnection::Start(). 
        // Cancel MPM asynch.
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingReselectBestIAPCompleteOrError::Accept() NOT Accept TCancel, Cancelling ReselectBestIAPL()",(TInt*)&iContext.Node())

        // Get the MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        
        node.Policy()->CancelReselectBestIAP();
        
        return EFalse;
        }
    else
        {
        // else
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TAwaitingProcessErrorCompleteOrError::Accept
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TAwaitingProcessErrorCompleteOrError, NetStateMachine::MState, TContext )
TBool TAwaitingProcessErrorCompleteOrError::Accept()
    {
    if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg>() )
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingProcessErrorCompleteOrError::Accept() TMPMProcessErrorCompletedMsg or Error", (TInt*)&iContext.Node())
        // ProcessErrorComplete completed.
        return ETrue;
        }
    else if ( iContext.iMessage.IsMessage<TEBase::TError>() )
        {
        // Processing continue in error transition.
        TEBase::TError* msg = message_cast<TEBase::TError>( &iContext.iMessage );
        iContext.iNodeActivity->SetError( msg->iValue );
        S60MCPRLOGSTRING2("S60MCPR<%x>::TAwaitingProcessErrorCompleteOrError::Accept() Error %d", (TInt*)&iContext.Node(),msg->iValue)
        return ETrue;
        }
    else if ( iContext.iMessage.IsMessage<TEBase::TCancel>() )
        {
        // Get the MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        node.Policy()->CancelProcessError();
        // should this be the same as ignore?
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingProcessErrorCompleteOrError::Accept() NOT Accepted TCancel Cancelling ProcessError", (TInt*)&iContext.Node())
        return EFalse;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TAwaitingServiceIdRequest::Accept
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TAwaitingServiceIdRequest, NetStateMachine::MState, TContext )
TBool TAwaitingServiceIdRequest::Accept()
    {
    ESock::TMCprGetConnectionSetting* msg = NULL;

    S60MCPRLOGSTRING2("TAwaitingServiceIdRequest: iContext.iMessage( %x %x )", 
            iContext.iMessage.MessageId().MessageId(), 
            iContext.iMessage.MessageId().Realm());
    S60MCPRLOGSTRING2(" == ( %x %x )?\n", 
            ESock::TMCprGetConnectionSetting::EId, 
            ESock::TMCprGetConnectionSetting::ERealm);

    if ( iContext.iMessage.IsMessage<ESock::TMCprGetConnectionSetting>() )  
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingServiceIdRequest::Accept() TMCprGetConnectionSetting", 
            (TInt*)&iContext.Node())
        msg = message_cast<ESock::TMCprGetConnectionSetting>(&iContext.iMessage);

        // Get MCPR
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        _LIT( KIapProxyServiceSetting, "IAP\\IAPService" );
        TBuf<KCommsDbSvrMaxColumnNameLength*2 +1> field;
        if (msg)
            {
            TRAP_IGNORE(msg->iMessage.ReadL(0, field));
            }

        S60MCPRLOGSTRING2("S60MCPR<%x>::TAwaitingServiceIdRequest::Accept() ServiceId = %x", 
            (TInt*)&iContext.Node(), node.PolicyPrefs().ServiceId())

        // Only EIntSetting is supported 
        // 
        if ( ( TMCprGetConnectionSetting::TConnectionSettingType)msg->iSettingType == 
               TMCprGetConnectionSetting::EIntSetting
               && node.PolicyPrefs().ServiceId() != 0 
               && field.Compare( KIapProxyServiceSetting ) == 0  )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingServiceIdRequest::Accept() TMCprGetConnectionSetting EIntSetting", 
                (TInt*)&iContext.Node())
            return ETrue;
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingServiceIdRequest::Accept() TMCprGetConnectionSetting not processing EIntSetting", 
                (TInt*)&iContext.Node())
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TRetrieveServiceId::DoL
// -----------------------------------------------------------------------------
//
DEFINE_SMELEMENT( TRetrieveServiceId, NetStateMachine::MStateTransition, TContext )
void TRetrieveServiceId::DoL()
    {
    S60MCPRLOGSTRING1("S60MCPR<%x>::TRetrieveServiceId::DoL()", (TInt*)&iContext.Node())

    // Get MCPR
    CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

    // Get the policy server's selection result 
    TUint32 serviceId = node.PolicyPrefs().ServiceId();
    S60MCPRLOGSTRING2("S60MCPR<%x>::TRetrieveServiceId::DoL() ServiceId = %d", 
            (TInt*)&iContext.Node(), serviceId )

    TPckg<TUint32> pckg( serviceId );

    S60MCPRLOGSTRING1("S60MCPR<%x>::TRetrieveServiceId::DoL() Write ServiceId to ResponseMsg", 
            (TInt*)&iContext.Node())
    TCFSigLegacyRMessage2Ext& msg = static_cast<TCFSigLegacyRMessage2Ext&>( iContext.iMessage );
    msg.iMessage.WriteL( 1, pckg ); // codescanner::leave

    S60MCPRLOGSTRING1("S60MCPR<%x>::TRetrieveServiceId::DoL() Complete ResponseMsg", 
            (TInt*)&iContext.Node())
    RLegacyResponseMsg responseMsg( iContext, msg.iMessage, msg.iMessage.Int0() );
    responseMsg.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CReselectBestIAPCb::PolicyResponse 
// -----------------------------------------------------------------------------
//
void CReselectBestIAPCb::PolicyResponse( PolicyRequest& aCompletedRequest )
    {
    using namespace ESock;
    TPolicyConnPref* policypref = (TPolicyConnPref*)aCompletedRequest.iPolicyPref;

    // Request to MPM failed for some reason
    if( aCompletedRequest.iStatus != KErrNone )
        {
        S60MCPRLOGSTRING2("S60MCPR<%x>::CReselectBestIAPCb::PolicyResponse() MPM Error %d",(TInt*)&iNode,aCompletedRequest.iStatus)
        Messages::TEBase::TError msg( TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg::Id(), 
                                      aCompletedRequest.iStatus );
        if ( iLastRequestOriginator.IsOpen() )
            {
            iLastRequestOriginator.ReplyTo( iNode.Id(), msg );
            iLastRequestOriginator.Close();
            }
        return;
        }

    S60MCPRLOGSTRING3("S60MCPR<%x>::CReselectBestIAPCb::PolicyResponse() IAP %d NET %d",
            (TInt*)&iNode,policypref->IapId(),policypref->NetId())
    
    iNode.SetPolicyPrefs( *policypref );
    
    TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg msg;
    if ( iLastRequestOriginator.IsOpen() )
        {
        iLastRequestOriginator.ReplyTo( iNode.Id(), msg );
        iLastRequestOriginator.Close();
        }
    }

// -----------------------------------------------------------------------------
// CProcessErrorCb::PolicyResponse
// -----------------------------------------------------------------------------
//
void CProcessErrorCb::PolicyResponse( PolicyRequest& aCompletedRequest )
    {
    S60MCPRLOGSTRING3("S60MCPR<%x>::CProcessErrorCb::PolicyResponse() status=%d action=%d",
              (TInt*)&iNode,aCompletedRequest.iStatus,(TInt)aCompletedRequest.iNeededAction)
    // Validate that we can send a response.
    //
    if ( !iLastRequestOriginator.IsOpen() )
        {
        // response can't be sent.
        return;
        }
    // Request ok and valid one
    //
    if ( aCompletedRequest.iStatus == KErrNone &&
         ( aCompletedRequest.iNeededAction == EIgnoreError ||
           aCompletedRequest.iNeededAction == EDoReselection ))
        {
        // Send error recovery propagated msg to self. This way a potential mobility 
        // handshake knows that there were a problem with migrated IAP.
        // In case there isn't mobility active available this just results as stray msg
        if ( aCompletedRequest.iNeededAction == EIgnoreError )
            {
            iNode.SelfInterface().PostMessage( iNode.Id(), 
                    TCFS60MCPRMessage::TMPMStartupErrorIgnoredMsg().CRef() );
            }
        
        TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg msg( (TInt)aCompletedRequest.iNeededAction );
        iLastRequestOriginator.ReplyTo( iNode.Id(), msg );
        iLastRequestOriginator.Close();
        }
    // Propagate error
    //
    else
        {
        // Initialize
        TInt err = aCompletedRequest.iStatus;
        if ( err == KErrNone )
            {
            ASSERT( aCompletedRequest.iError != KErrNone );
            err = aCompletedRequest.iError != KErrNone ? aCompletedRequest.iError : KErrGeneral;
            }
            
        // Send error msg indicatating the processerror has been completed.
        TEBase::TError msg( TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg::Id(), err );
        iLastRequestOriginator.ReplyTo( iNode.Id(), msg );
        iLastRequestOriginator.Close();
        }
    }

//  End of File

