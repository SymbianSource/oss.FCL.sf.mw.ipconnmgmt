/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR error recovery activity implementation
*
*/

/**
@file s60mcprerrorrecoveryactivity.cpp
S60 MCPR error recovery activity implementation
*/

#include "s60mcpractivities.h"
#include "s60mcprstates.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

// -----------------------------------------------------------------------------
// S60MCprErrorRecoveryActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprErrorRecoveryActivity
    {
    TBool THandshakingMobilityMutex::IsBlocked(TNodeContextBase& aContext)
        {
        return static_cast<CS60MetaConnectionProvider&>(aContext.iNode).IsHandshakingNow();
        }
    

    // -----------------------------------------------------------------------------
    // TSendIgnoreRecoveryResponse::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TSendIgnoreRecoveryResponse, NetStateMachine::MStateTransition, TContext )
    void TSendIgnoreRecoveryResponse::DoL()  // codescanner::leave
        {
        CS60ConnectionRecoveryActivity* ac = 
            static_cast<CS60ConnectionRecoveryActivity*>(iContext.iNodeActivity);

        S60MCPRLOGSTRING2("S60MCPR<%x>::TSendIgnoreRecoveryResponse::DoL() Ignore Error %d",
                  (TInt*)&iContext.Node(),ac->iOriginalErrContext.iStateChange.iError );

        __ASSERT_DEBUG( iContext.iNodeActivity, User::Panic( KS60MCprPanic, KPanicNoActivity ) );
        TEErrorRecovery::TErrorRecoveryResponse err( TErrResponse( TErrResponse::EIgnore, 
                                                                   KErrNone, 
                                                                   ac->iOriginalErrContext.iMessageId ) );
        // We must clear out the activity error to prevent last automatic error message from the node.
        iContext.iNodeActivity->SetError( KErrNone );
        ac->ReplyToOriginators( err );
        }

    // -----------------------------------------------------------------------------
    // TAwaitingNotGoneDownErrorRecoveryRequest::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TAwaitingNotGoneDownErrorRecoveryRequest, 
                     NetStateMachine::MState, 
                     TContext)
    TBool TAwaitingNotGoneDownErrorRecoveryRequest::Accept()
        {
        TEErrorRecovery::TErrorRecoveryRequest* msg = 
            message_cast<TEErrorRecovery::TErrorRecoveryRequest>(&iContext.iMessage);
        // Validata error recovery request.
        // This state will accept other error recovery request besides the one meant for GoneDown.
        //
        if ( msg )
            {
            TErrContext& errCtx = msg->iErrContext;
/*            if (errCtx.iActivitySigId == ECFActivityNoBearer || errCtx.iActivitySigId == ECFActivityStart
                || errCtx.iStateChange.iError == KErrCouldNotConnect)
*/
            if ( errCtx.iActivitySigId != ECFActivityGoneDown )
                {
                S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingNotGoneDownErrorRecoveryRequest::Accept()",(TInt*)&iContext.Node());
                return ETrue;
                }
            }
        return EFalse;
        }

    
    // -----------------------------------------------------------------------------
    // TNoTagOrIgnoreErrorOrErrorTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TNoTagOrIgnoreErrorOrErrorTag, NetStateMachine::MStateFork, TContext)
    TInt TNoTagOrIgnoreErrorOrErrorTag::TransitionTag()
        {
        TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg* msg = NULL;

        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg>() )
            {
            msg = message_cast<TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg>(&iContext.iMessage);
            }

        // Sanity check.
        if ( iContext.iMessage.IsMessage<TEBase::TError>() && iContext.iNodeActivity->Error() == KErrNone )
            {
            // Should never end up here!!
            __ASSERT_DEBUG( EFalse, User::Panic( KS60MCprPanic, KPanicUnsupportedMCPRState ) );
            iContext.iNodeActivity->SetError( KErrGeneral );
            }

        // Error transition
        //
        if ( !msg )
            {
            return KErrorTag | NetStateMachine::EForward;
            }
        // Transition to ignore error
        //
        else if ( (TBMNeededAction)msg->iValue == EIgnoreError )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrIgnoreErrorOrErrorTag::TransitionTag() KIgnoreError",(TInt*)&iContext.Node());
            return S60MCprStates::KIgnoreError | NetStateMachine::EForward;
            }
        // Transition to reselection
        //
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrIgnoreErrorOrErrorTag::TransitionTag() KNoTag",(TInt*)&iContext.Node());
            return MeshMachine::KNoTag | NetStateMachine::EForward;
            }
        }

    // -----------------------------------------------------------------------------
    // TNoTagOrRetryConnectionTagOrErrorTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TNoTagOrRetryConnectionTagOrErrorTag, NetStateMachine::MStateFork, TContext)
    TInt TNoTagOrRetryConnectionTagOrErrorTag::TransitionTag()
        {
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg* msg = NULL;
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg>() )
            {
            msg = message_cast<TCFS60MCPRMessage::TMPMReselectBestIAPCompletedMsg>(&iContext.iMessage);
            }
        
        if ( !msg )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrRetryConnectionTagOrErrorTag::TransitionTag() KErrorTag",(TInt*)&iContext.Node());
            return KErrorTag | NetStateMachine::EForward;
            }
        else if ( ((RMetaServiceProviderInterface*)node.ServiceProvider())
                    ->ProviderInfo().APId() == node.PolicyPrefs().IapId() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrRetryConnectionTagOrErrorTag::TransitionTag() KRetryConnection",(TInt*)&iContext.Node());
            return S60MCprStates::KRetryConnection | NetStateMachine::EForward;
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrRetryConnectionTagOrErrorTag::TransitionTag() KNoTag",(TInt*)&iContext.Node());
            return KNoTag | NetStateMachine::EForward;
            }
        }
    
    // -----------------------------------------------------------------------------
    // TRetryConnectionOrErrorTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TRetryConnectionOrProcessErrorTagBackward, NetStateMachine::MStateFork, TContext)
    TInt TRetryConnectionOrProcessErrorTagBackward::TransitionTag()
        {
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        TCFMcpr::TReConnectComplete* msg = NULL;
        if ( iContext.iMessage.IsMessage<TCFMcpr::TReConnectComplete>() )
            {
            msg = message_cast<TCFMcpr::TReConnectComplete>(&iContext.iMessage);
            }

        if ( !msg )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TRetryConnectionOrProcessErrorTagBackward::TransitionTag() KProcessError",(TInt*)&iContext.Node());
            return S60MCprStates::KProcessError | NetStateMachine::EBackward;
            //return KErrorTag | NetStateMachine::EForward;
            }
        else// if ( node.ServiceProvider()->ProviderInfo().APId() == node.PolicyPrefs().IapId() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TRetryConnectionOrProcessErrorTagBackward::TransitionTag() KRetryConnection",(TInt*)&iContext.Node());
            return S60MCprStates::KRetryConnection | NetStateMachine::EForward;
            }
        }

    // -----------------------------------------------------------------------------
    // TNoTagOrProcessErrorBackwardTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT(TNoTagOrProcessErrorBackwardTag, NetStateMachine::MStateFork, TContext)
    TInt TNoTagOrProcessErrorBackwardTag::TransitionTag()
        {
        if ( iContext.iMessage.IsMessage<TEBase::TError>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrProcessErrorBackwardTag::TransitionTag() KProcessError",(TInt*)&iContext.Node());
            return S60MCprStates::KProcessError | NetStateMachine::EBackward;
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrProcessErrorBackwardTag::TransitionTag() KNoTag",(TInt*)&iContext.Node());
            return MeshMachine::KNoTag | NetStateMachine::EForward;
            }
        }


    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::NewL
    // -----------------------------------------------------------------------------
    //
    MeshMachine::CNodeActivityBase* CS60ConnectionRecoveryActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig,  // codescanner::leave
                                                                          MeshMachine::AMMNodeBase& aNode )
        {
        return new (ELeave) CS60ConnectionRecoveryActivity( aActivitySig, aNode );  // codescanner::leave
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::CS60ConnectionRecoveryActivity
    // -----------------------------------------------------------------------------
    //
    CS60ConnectionRecoveryActivity::CS60ConnectionRecoveryActivity( const MeshMachine::TNodeActivity& aActivitySig, 
                                                                    MeshMachine::AMMNodeBase& aNode )
        :   S60MCprStates::CS60ErrorRecoveryActivity( aActivitySig, aNode )
        {
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::ReplyToOriginators
    // -----------------------------------------------------------------------------
    //
    void CS60ConnectionRecoveryActivity::ReplyToOriginators( TEErrorRecovery::TErrorRecoveryResponse& aCFMessageSig )
        {
        NM_LOG_START_BLOCK(KESockMeshMachine, _L8("CS60ConnectionRecoveryActivity::ReplyToOriginators"));
        NM_LOG((KESockMeshMachine, _L8("[this=0x%08x] "), this));
        NM_LOG_MESSAGE(KESockMeshMachine, aCFMessageSig);
        NM_LOG_END_BLOCK(KESockMeshMachine, _L8("CS60ConnectionRecoveryActivity::ReplyToOriginators"));
        for ( TInt n = iOriginators.Count() - 1; n >= 0; n-- )
            {
            Messages::TNodePeerId& peerId = iOriginators[n];
            TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(aCFMessageSig, peerId.RecipientId());
            peerId.PostMessage( iNode.Id(), resp );
            }
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TAwaitingReConnectComplete::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60ConnectionRecoveryActivity::TAwaitingReConnectComplete, 
                      NetStateMachine::MState, 
                      CS60ConnectionRecoveryActivity::TContext )
    TBool CS60ConnectionRecoveryActivity::TAwaitingReConnectComplete::Accept()
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
        if ( msg )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingReConnectComplete::TransitionTag() NOT Accept sending TErrorRecoveryResponse",(TInt*)&iContext.Node());
            CS60ConnectionRecoveryActivity& ac = static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);
            TErrResponse propagateResp( TErrResponse::EPropagate,
                                        ac.iOriginalErrContext.iStateChange.iError,
                                        ac.iOriginalErrContext.iMessageId );
            TEErrorRecovery::TErrorRecoveryResponse errResp( propagateResp );
            ac.ReplyToOriginators( errResp );
            ac.SetIdle();
            iContext.iMessage.ClearMessageId();
            return EFalse;
            }
        if ( iContext.iMessage.IsMessage<TCFMcpr::TReConnectComplete>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingReConnectComplete::TransitionTag() Accept",(TInt*)&iContext.Node());
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TTransitionBase::Error
    // -----------------------------------------------------------------------------
    //
    void CS60ConnectionRecoveryActivity::TTransitionBase::Error( TInt /*aError*/ )
        {
        //Reply to the Error Activity and terminate
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60ConnectionRecoveryActivity& ac = static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);
        TEErrorRecovery::TErrorRecoveryResponse errResp( TErrResponse( TErrResponse::EPropagate, 
                                                                       ac.iOriginalErrContext.iStateChange.iError,
                                                                       ac.iOriginalErrContext.iMessageId ));
        ac.ReplyToOriginators( errResp );
        S60MCPRLOGSTRING1("S60MCPR<%x>::TTransitionBase::Error()",(TInt*)&iContext.Node());
        iContext.iNodeActivity->SetIdle();
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TStoreErrorContext::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60ConnectionRecoveryActivity::TStoreErrorContext, 
                      NetStateMachine::MStateTransition, 
                      CS60ConnectionRecoveryActivity::TContext )
    void CS60ConnectionRecoveryActivity::TStoreErrorContext::DoL()  // codescanner::leave
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60ConnectionRecoveryActivity& activity = static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);
        activity.iOriginalErrContext = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(iContext.iMessage).iErrContext;
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TSendRetryRecoveryResponse::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60ConnectionRecoveryActivity::TSendRetryRecoveryResponse, 
                      NetStateMachine::MStateTransition, 
                      CS60ConnectionRecoveryActivity::TContext )
    void CS60ConnectionRecoveryActivity::TSendRetryRecoveryResponse::DoL()  // codescanner::leave
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60ConnectionRecoveryActivity& activity = static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);
        TEErrorRecovery::TErrorRecoveryResponse err( TErrResponse( TErrResponse::ERetry,
                                                                   KErrNone,
                                                                   activity.iOriginalErrContext.iMessageId ) );
        S60MCPRLOGSTRING2("S60MCPR<%x>::TSendRetryRecoveryResponse<%x>::DoL()",(TInt*)&iContext.Node(),(TInt*)this);
        // We must clear out the activity error to prevent last automatic error message from the node.
        iContext.iNodeActivity->SetError( KErrNone );
        activity.ReplyToOriginators( err );
        }

    
    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse, 
                      NetStateMachine::MStateTransition, 
                      CS60ConnectionRecoveryActivity::TContext )
    void CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse::DoL()  // codescanner::leave
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60ConnectionRecoveryActivity& activity = 
            static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);
        // Override original error with activity error.
        TEErrorRecovery::TErrorRecoveryResponse err( TErrResponse( TErrResponse::EPropagate,
                                                                   activity.Error(),
                                                                   activity.iOriginalErrContext.iMessageId ) );
        S60MCPRLOGSTRING2("S60MCPR<%x>::TSendPropagateRecoveryResponse::DoL() %d",(TInt*)&iContext.Node(),(TInt)activity.Error());
        // We must clear out the activity error to prevent last automatic error message from the node.
        iContext.iNodeActivity->SetError( KErrNone );
        activity.ReplyToOriginators( err );
        }

    // -----------------------------------------------------------------------------
    // CS60ConnectionRecoveryActivity::TSendPropagateRecoveryErrContextResponse::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60ConnectionRecoveryActivity::TSendPropagateRecoveryErrContextResponse, 
                      NetStateMachine::MStateTransition, 
                      CS60ConnectionRecoveryActivity::TContext )
    void CS60ConnectionRecoveryActivity::TSendPropagateRecoveryErrContextResponse::DoL()  // codescanner::leave
        {
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
                
        CS60ConnectionRecoveryActivity& activity = 
            static_cast<CS60ConnectionRecoveryActivity&>(*iContext.iNodeActivity);

        TInt error = activity.iOriginalErrContext.iStateChange.iError;
        
        if ( error == KErrNone )
            {
            error = KErrDisconnected;
            S60MCPRLOGSTRING2("S60MCPR<%x>::TSendPropagateRecoveryErrContextResponse::DoL() overriding error code %d",(TInt*)&iContext.Node(), error);
            }

        // Override original error with activity error.
        TEErrorRecovery::TErrorRecoveryResponse err( TErrResponse( TErrResponse::EPropagate,
                                                                   error,
                                                                   activity.iOriginalErrContext.iMessageId ) );
        S60MCPRLOGSTRING2("S60MCPR<%x>::TSendPropagateRecoveryErrContextResponse::DoL() %d",(TInt*)&iContext.Node(), error);
        // We must clear out the activity error to prevent last automatic error message from the node.
        iContext.iNodeActivity->SetError( KErrNone );
        activity.ReplyToOriginators( err );
        }

    } // namespace S60MCprErrorRecoveryActivity

//  End of File
