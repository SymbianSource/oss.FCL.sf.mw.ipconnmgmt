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
* Description: S60 MCPR mobility activity implementation
*
*/

/**
@file s60mcprmobilityactivity.cpp
S60 MCPR mobility activity implementation
*/

#include "s60mcpractivities.h"
#include "s60mcprstates.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

// -----------------------------------------------------------------------------
// S60MCprMobilityActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprMobilityActivity
    {
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TDoNothingOrReSendPendingNotification::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TDoNothingOrReSendPendingNotification, NetStateMachine::MStateTransition, TContext )
    void TDoNothingOrReSendPendingNotification::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TDoNothingOrReSendPendingNotification::DoL()",(TInt*)&iContext.Node())
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        // Process the pending request first.
        //
        node.ReSendPolicyNotification();
        node.ClearPolicyNotification();
        }

    
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TReplyMobilityStarted::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TReplyMobilityStarted, NetStateMachine::MStateTransition, TContext )
    void TReplyMobilityStarted::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TReplyMobilityStarted::DoL()",(TInt*)&iContext.Node())
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        iContext.iNodeActivity->PostToOriginators( TCFMobilityProvider::TMobilityStarted().CRef() );
        iContext.iNodeActivity->ClearPostedTo();
        }


    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TSendAvailabilityRequest::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TSendAvailabilityRequest, NetStateMachine::MStateTransition, TContext )
    void TSendAvailabilityRequest::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendAvailabilityRequest::DoL()", (TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        node.Policy()->RegisterPreferredIAPNotificationL();
        }
    

    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TSendInitialApplicationReject::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TSendInitialApplicationReject, 
                      NetStateMachine::MStateTransition, 
                      TContext )
    void TSendInitialApplicationReject::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendInitialApplicationReject::DoL()", (TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

        RMetaServiceProviderInterface* sp = NULL;
        // Set service provider to point to active service provider.
        sp = (RMetaServiceProviderInterface*) node.ServiceProvider();
        TInt iapid = sp ? sp->ProviderInfo().APId() : 0;
        
        node.Policy()->ApplicationRejectedTheCarrierL( iapid );
        }
    

    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TAwaitingStartMobility::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TAwaitingStartMobility, NetStateMachine::MState, TContext )
    TBool TAwaitingStartMobility::Accept()
        {
        if (!iContext.iMessage.IsMessage<TCFMobilityProvider::TStartMobility>())
            {
            return EFalse;
            }
        if (iContext.Node().ServiceProvider()==NULL)
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingStartMobility::Accept() no Provider!",(TInt*)&iContext.Node())
            //There is no mobility on the lowest layer of the stack
            iContext.iMessage.ClearMessageId();
            return EFalse;
            }
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingStartMobility::Accept()", (TInt*)&iContext.Node() );
        return ETrue;
        }

    
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TAwaitingMigrationRequestedOrRejectedOrCancel::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TAwaitingMigrationRequestedOrRejectedOrCancel, 
                      NetStateMachine::MState, TContext )
    TBool TAwaitingMigrationRequestedOrRejectedOrCancel::Accept()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingMigrationRequestedOrRejectedOrCancel::Accept()", 
                (TInt*)&iContext.Node())
        return (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRequested>()) ||
               (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>()) ||
               (iContext.iMessage.IsMessage<TEBase::TCancel>());
//               (iContext.iMessage.IsMessage<TEBase::TError>());
        }


    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel, NetStateMachine::MState, TContext )
    TBool TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel::Accept()
        {
        if ( (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationAccepted>()) ||
             (iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>()) ||
             (iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMStartupErrorIgnoredMsg>()) ||
             (iContext.iMessage.IsMessage<TEBase::TCancel>()) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel::Accept()",(TInt*)&iContext.Node())
            return ETrue;
            }
        return EFalse;
        }
    
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TAwaitingMigrationRejected::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TAwaitingMigrationRejected, NetStateMachine::MState, TContext )
    TBool TAwaitingMigrationRejected::Accept()
        {
        if ( iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingMigrationRejected::Accept()",(TInt*)&iContext.Node())
            return ETrue;
            }
        return EFalse;
        }
    
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel, NetStateMachine::MStateFork, TContext )
    TBool TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel::TransitionTag()
        {
        // Error notification falls into the error -case.
        if ( iContext.iNodeActivity->Error() ) 
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel::TransitionTag() Error",(TInt*)&iContext.Node());    
            return MeshMachine::KErrorTag | NetStateMachine::EForward;
            }
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMStartIAPNotificationMsg>() ) 
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel::TransitionTag() TMPMStartIAPNotificationMsg",(TInt*)&iContext.Node())    
            return KNoTag | NetStateMachine::EForward;
            }
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel::TransitionTag() TMPMPreferredCarrierAvailableMsg",(TInt*)&iContext.Node())    
            return S60MCprStates::KInformMigrationAvailable | NetStateMachine::EBackward;
            }            
        // else cancel
        return KCancelTag | NetStateMachine::EForward;
        }
        
    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TStartMobilityHandshakeBackwardsOrError::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TStartMobilityHandshakeBackwardsOrError, NetStateMachine::MStateFork, TContext )
    TBool TStartMobilityHandshakeBackwardsOrError::TransitionTag()
        {
        if ( iContext.iNodeActivity->Error() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TStartMobilityHandshakeBackwardsOrError::TransitionTag() KErrorTag",(TInt*)&iContext.Node())
            return MeshMachine::KErrorTag | NetStateMachine::EForward;
            }
        S60MCPRLOGSTRING1("S60MCPR<%x>::TStartMobilityHandshakeBackwardsOrError::TransitionTag() KStartMobilityHandshake",(TInt*)&iContext.Node())
        return MobilityMCprStates::KStartMobilityHandshake | NetStateMachine::EBackward;
        }


    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TInformMigrationAvailableOrCancelTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag, NetStateMachine::MStateFork, TContext )
    TBool TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag()
        {        

        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMErrorNotificationMsg>() )
            {
            // Error notification from MPM. See whether we are able to recover from
            // the error situation via other means (by an error recovery activity)
            TInt errorVal = message_cast<TCFS60MCPRMessage::TMPMErrorNotificationMsg>( &iContext.iMessage )->iValue;
            ASSERT( errorVal != KErrNone );
        
            if ( iContext.iNode.CountActivities( ECFActivityConnectionGoneDownRecovery ) != 0 )
                {
                // This error notification is sent by MPM to error clients mobility
                // interface. The error is unrecoverable in a sense that in this state
                // there is no way to get the connection back up and running. However,
                // we can be certain that the error recovery activity cleans up the
                // stack (MPM instructs us to propagate error). So to avoid possible
                // race conditions, just pass the error notification forward.
                S60MCPRLOGSTRING2("S60MCPR<%x>::TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag() KRecoverableError (%d)",(TInt*)&iContext.Node(), errorVal )
                return S60MCprStates::KRecoverableError | NetStateMachine::EForward;
                }
            else
                {
                // No gone down activity running. Hmm, we actually should hit this
                // branch but for future's sake error this activity and clean up
                // the stack.
                iContext.iNodeActivity->SetError( errorVal );                
                S60MCPRLOGSTRING2("S60MCPR<%x>::TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag() KErrorTag (error: %d)",(TInt*)&iContext.Node(), errorVal )
                return MeshMachine::KErrorTag | NetStateMachine::EForward;
                }
            }
        else if ( iContext.iMessage.IsMessage<TEBase::TCancel>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag() KCancelTag",(TInt*)&iContext.Node())
            return KCancelTag | NetStateMachine::EForward;
            }
        else if ( iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag() KSendInitialApplicationReject",
                    (TInt*)&iContext.Node())
            return S60MCprStates::KSendInitialApplicationReject | NetStateMachine::EForward;
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationAvailableOrErrorOrRecoverableErrorOrCancelTag::TransitionTag() KInformMigrationAvailable",(TInt*)&iContext.Node())
            return S60MCprStates::KInformMigrationAvailable | NetStateMachine::EForward;
            }
        }


    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected, NetStateMachine::MStateFork, TContext )
    TBool TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag()
        {
        if ( iContext.iNodeActivity->Error() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag() KErrorTag",(TInt*)&iContext.Node())
            return MeshMachine::KErrorTag | NetStateMachine::EForward;
            }
        if ( message_cast<TCFMobilityProvider::TMigrationAccepted>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag() KNoTag",(TInt*)&iContext.Node())
            return MeshMachine::KNoTag | NetStateMachine::EForward;
            }
        else if ( message_cast<TCFMobilityProvider::TMigrationRejected>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag() KApplicationRejectedMigration",(TInt*)&iContext.Node())
            return S60MCprStates::KApplicationRejectedMigration | NetStateMachine::EForward; 
            }
        else if ( message_cast<TCFS60MCPRMessage::TMPMStartupErrorIgnoredMsg>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag() KSwallowRejectedMsg",(TInt*)&iContext.Node())
            return S60MCprStates::KConsumeRejectedMsg | NetStateMachine::EForward; 
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected::TransitionTag() KCancelTag",(TInt*)&iContext.Node())
            return MeshMachine::KCancelTag | NetStateMachine::EForward;
            }
        }


    // -----------------------------------------------------------------------------
    // S60MCprMobilityActivity::TWaitPolicyServerResponseBackwardOrErrorTag::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( TWaitPolicyServerResponseBackwardOrErrorTag, NetStateMachine::MStateFork, TContext )
    TBool TWaitPolicyServerResponseBackwardOrErrorTag::TransitionTag()
        {
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg>() )
            {
            // Make sure policy server doesn't say, reselect.
            ASSERT( message_cast<TCFS60MCPRMessage::TMPMProcessErrorCompletedMsg>(&iContext.iMessage)->iValue !=
                    (TInt)EDoReselection );
            // Make sure we're are no longer in error state.
            iContext.iNodeActivity->SetError( KErrNone );
            S60MCPRLOGSTRING1("S60MCPR<%x>::TWaitPolicyServerResponseBackwardOrErrorTag::TransitionTag() KWaitPolicyServerResponse",(TInt*)&iContext.Node())
            return S60MCprStates::KWaitPolicyServerResponse | NetStateMachine::EBackward;
            }
        else
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TWaitPolicyServerResponseBackwardOrErrorTag::TransitionTag() KErrorTag",(TInt*)&iContext.Node())
            return MeshMachine::KErrorTag | NetStateMachine::EForward;
            }
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::NewL
    // -----------------------------------------------------------------------------
    //
    MeshMachine::CNodeActivityBase* CS60MobilityActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, 
                                                                       MeshMachine::AMMNodeBase& aNode )
        {
        CS60MobilityActivity* self = new(ELeave) CS60MobilityActivity( aActivitySig, aNode );
        return self;
        }

    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::CS60MobilityActivity
    // -----------------------------------------------------------------------------
    //
    CS60MobilityActivity::CS60MobilityActivity( const MeshMachine::TNodeActivity& aActivitySig, 
                                                MeshMachine::AMMNodeBase& aNode )
        :   S60MCprStates::CS60ErrorRecoveryActivity( aActivitySig, aNode )
        {}


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag, 
                      NetStateMachine::MStateTransition, 
                      CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationAvailableAndSetHandshakingFlag::DoL()",(TInt*)&iContext.Node())
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);

        //Inform the CPR that a potential migration is available. We only support a single data client
        //in this implementation.
        __ASSERT_DEBUG(activity.iPreferredAPId, User::Panic(KS60MCprPanic, KPanicNoServiceProvider));

        // Activity received the necessary information from the policy server earlier 
        // and now holds that information which we'll send to IPCPR.
        TCFMobilityControlClient::TMigrationNotification msg( activity.iCurrentAssumedAPId,
                                                              //activity.iCurrent->ProviderInfo().APId(),
                                                              activity.iPreferredAPId,
                                                              activity.iIsUpgrade, 
                                                              activity.iIsSeamless );

        activity.PostToOriginators(msg);
        activity.ClearPostedTo();
        activity.SetHandshakingFlag();
        
        
#ifdef _DEBUG
        // Purely for debugging purposes
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        if ( node.IsGoneDownRecoveryOngoing() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationAvailableAndSetHandshakingFlag::DoL() Start mobility handshake while gone down recovery is active",(TInt*)&iContext.Node())
            // During the handshake, there is four different paths that 
            // needs to be considered for reseting the gone down recovery
            // flag. These are:
            // 1) Migration is done successfully
            // 2) Application rejects the migration
            // 3) While migrating, a new preferred carrier is found       
            // 4) Operation is cancelled or an error occurs
            //
            // For 1) and 2) the flag needs to be reset, where as for 3) we
            // can keep the flag set until the "loop" is exited with a valid
            // reselection (essentially this will be same as 1) but only
            // after a couple of retrials). For the last option 4) the
            // connection is tear down and reseting the flag is not needed.
            }
#endif
        
        }

    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TSendMigrationRejected::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TSendMigrationRejected, 
                      NetStateMachine::MStateTransition, 
                      CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TSendMigrationRejected::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendMigrationRejected::DoL()",(TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        node.Policy()->ApplicationIgnoredTheCarrierL( activity.iPreferredAPId );
        if (node.IsGoneDownRecoveryOngoing() )
            {
            // Too bad... Old link has gone down, and application rejected
            // the new one. Nothing but problems ahead, but who cares, if
            // the application doesn't. So reset the gone down recovery flag
            // and assume that everybody is happy with the choise.
            S60MCPRLOGSTRING1("S60MCPR<%x>::TSendMigrationRejected::DoL() Application rejected the carrier during gone down recovery -> reset flag",(TInt*)&iContext.Node())
            node.ClearGoneDownRecoveryOngoing();
            }
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TSendMigrationAccepted::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TSendMigrationAccepted, 
                      NetStateMachine::MStateTransition, 
                      CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TSendMigrationAccepted::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendMigrationAccepted::DoL()",(TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        node.Policy()->ApplicationMigratesToCarrierL( activity.iPreferredAPId );
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TSendApplicationAccept::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TSendApplicationAccept, 
                      NetStateMachine::MStateTransition, 
                      CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TSendApplicationAccept::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendApplicationAccept::DoL()",(TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        node.Policy()->ApplicationAcceptedTheCarrierL( activity.iPreferredAPId );
        }
    
    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TSendApplicationReject::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TSendApplicationReject, 
                      NetStateMachine::MStateTransition, 
                      CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TSendApplicationReject::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TSendApplicationReject::DoL()",(TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        node.Policy()->ApplicationRejectedTheCarrierL( activity.iPreferredAPId );
        }

    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TClearHandshakingFlag::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TClearHandshakingFlag, 
                      NetStateMachine::MStateTransition, CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TClearHandshakingFlag::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TClearHandshakingFlag::DoL()",(TInt*)&iContext.Node())
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        activity.ClearHandshakingFlag();
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TInformMigrationCompleted::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TInformMigrationCompleted, 
                      NetStateMachine::MStateTransition, CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TInformMigrationCompleted::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationCompleted::DoL()",(TInt*)&iContext.Node())
        __ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KS60MCprPanic, KPanicNoActivity));
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        if ( node.IsGoneDownRecoveryOngoing() )
            {
            // Now we are on a safe side, since the reselect has been done
            // IPCPr can and will complete NoBearer requests to the new
            // interface instead of the old one. Reset the flag.
            S60MCPRLOGSTRING1("S60MCPR<%x>::TInformMigrationCompleted::DoL() New bearer selected while in gone down recovery -> reset flag",(TInt*)&iContext.Node())
            node.ClearGoneDownRecoveryOngoing();
            }
        iContext.iNodeActivity->PostToOriginators(TCFMobilityProvider::TMigrationComplete().CRef());
        iContext.iNodeActivity->ClearPostedTo();
        }
    
    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TCancelMobilityActivity::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TCancelMobilityActivity, 
                      NetStateMachine::MStateTransition, CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TCancelMobilityActivity::DoL()
        {
        S60MCPRLOGSTRING1("S60MCPR<%x>::TCancelAvailabilityRequest::DoL()",(TInt*)&iContext.Node())
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        // Unregister from preferred IAP Notifications.
        //
        node.Policy()->UnregisterPreferredIAPNotificationL();
        // Clear the mobility handshaking flag.
        // 
        CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
        activity.ClearHandshakingFlag();
        
        if ( node.IsGoneDownRecoveryOngoing() )
            {
            // Migration either errored or was cancelled while in gone down
            // recovery. Node will be destroyed, so there is no point in
            // such, but reset the flag anyway..
            S60MCPRLOGSTRING1("S60MCPR<%x>::TCancelAvailabilityRequest::DoL() Error in migration while in gone down recovery -> reset flag",(TInt*)&iContext.Node())
            node.ClearGoneDownRecoveryOngoing();
            }
        
        // At last we must set the activity in error since we're ready to go down.
        //
        iContext.iNodeActivity->SetError( KErrCancel );
        }

    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TErrorOriginatorAndStopDataClient::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TErrorOriginatorAndStopDataClient, 
                      NetStateMachine::MStateTransition, CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TErrorOriginatorAndStopDataClient::DoL()
        {
		// Post error to originators
		TEBase::TError errmsg(TCFMobilityProvider::TStartMobility::Id(), iContext.iNodeActivity->Error());
		iContext.iNodeActivity->PostToOriginators(errmsg);

		RNodeInterface* dc = iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData),
			TClientType(TCFClientType::EData, TCFClientType::ELeaving));
		
		// This should never be NULL, but check anyway for making sure...
		if ( dc )
		    {
            // Mark flag in case we happen to modify this in future
            dc->SetFlags(TCFClientType::EStopping);
            iContext.iNodeActivity->PostRequestTo(*dc, TCFDataClient::TStop(iContext.iNodeActivity->Error()).CRef());
		    }
		}

    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TErrorMobilityOriginator::DoL
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TErrorMobilityOriginator, 
                      NetStateMachine::MStateTransition, CS60MobilityActivity::TContext )
    void CS60MobilityActivity::TErrorMobilityOriginator::DoL()
        {
        // Extract the error code from the error notification message
        TCFS60MCPRMessage::TMPMErrorNotificationMsg* msg =
                message_cast<TCFS60MCPRMessage::TMPMErrorNotificationMsg>( &iContext.iMessage );
        // Message needs to be valid here. 
        ASSERT( msg );
                
        // Post error to originators
        TEBase::TError errmsg(TCFMobilityProvider::TStartMobility::Id(), msg->iValue);
        iContext.iNodeActivity->PostToOriginators(errmsg);
        }
    
    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification, 
                      NetStateMachine::MState, TContext )
    TBool CS60MobilityActivity::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept()
        {
        TBool result( EFalse );
        
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg>() )
            {
            CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
            
            TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg* msg = 
                message_cast<TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg>( &iContext.iMessage );

            TMpmNotificationPrefIAPAvailable* notif = (TMpmNotificationPrefIAPAvailable*)msg->iPtr;

            S60MCPRLOGSTRING4("S60MCPR<%x>::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept() TMPMPreferredCarrierAvailableMsg %d -> %d upgrade=%d",
                       (TInt*)&iContext.Node(), notif->iOldIapId, notif->iNewIapId, notif->iIsUpgrade)
            // notif must be there.
            ASSERT(notif);
            
            // Message received ok, clear the pending preferred IAP.
            //
            ((CS60MetaConnectionProvider&)iContext.Node()).ClearPolicyNotification();
            
            // Store the message preferences.
            //
            activity.iPreferredAPId         = notif->iNewIapId;
            activity.iCurrentAssumedAPId    = notif->iOldIapId;
            activity.iIsUpgrade             = notif->iIsUpgrade;
            activity.iIsSeamless            = notif->iIsSeamless;
            
            result = ETrue;
            }
        else if ( iContext.iMessage.IsMessage<TEBase::TCancel>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept() TCancel", 
                    (TInt*)&iContext.Node())
            
            result = ETrue;
            }
        else if ( iContext.iMessage.IsMessage<TCFMobilityProvider::TMigrationRejected>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept() TMigrationRejected", 
                    (TInt*)&iContext.Node())
            
            result = ETrue;
            }
        else if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMErrorNotificationMsg>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification::Accept() TMPMErrorNotificationMsg %d",(TInt*)&iContext.Node())

            result = ETrue;
            }

        return result;
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel, 
                      NetStateMachine::MState, 
                      CS60MobilityActivity::TContext )
    TBool CS60MobilityActivity::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept()
        {
        CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
        // Clear the pending at the end if the msg was accepted.
        //
        TBool result( EFalse );

        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMErrorNotificationMsg>() )
            {
            CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
            TCFS60MCPRMessage::TMPMErrorNotificationMsg* msg = 
                message_cast<TCFS60MCPRMessage::TMPMErrorNotificationMsg>( &iContext.iMessage );
            S60MCPRLOGSTRING2("S60MCPR<%x>::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept() TMPMErrorNotificationMsg %d",(TInt*)&iContext.Node(),msg->iValue)
            ASSERT( msg->iValue != KErrNone );
            activity.SetError( msg->iValue );
            result = ETrue;
            }
                
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg>() )
            {
            CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
            
            TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg* msg = 
                message_cast<TCFS60MCPRMessage::TMPMPreferredCarrierAvailableMsg>( &iContext.iMessage );

            TMpmNotificationPrefIAPAvailable* notif = (TMpmNotificationPrefIAPAvailable*)msg->iPtr;

            S60MCPRLOGSTRING4("S60MCPR<%x>::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept() TMPMPreferredCarrierAvailableMsg %d -> %d upgrade=%d",
                       (TInt*)&iContext.Node(), notif->iOldIapId, notif->iNewIapId, notif->iIsUpgrade)

            // info must be there.
            ASSERT(notif);

            activity.iPreferredAPId         = notif->iNewIapId;
            activity.iCurrentAssumedAPId    = notif->iOldIapId;
            activity.iIsUpgrade             = notif->iIsUpgrade;
            activity.iIsSeamless            = notif->iIsSeamless;
            result = ETrue;
            }
        
        if ( iContext.iMessage.IsMessage<TCFS60MCPRMessage::TMPMStartIAPNotificationMsg>() )
            {
            TCFS60MCPRMessage::TMPMStartIAPNotificationMsg* msg = 
                message_cast<TCFS60MCPRMessage::TMPMStartIAPNotificationMsg>( &iContext.iMessage );

            CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();

            S60MCPRLOGSTRING2("S60MCPR<%x>::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept() TMPMStartIAPNotificationMsg IAP=%d NET=%d",(TInt*)&iContext.Node(), msg->iValue)

            node.PolicyPrefs().SetIapId( msg->iValue );
            result = ETrue;
            }
        
        if ( iContext.iMessage.IsMessage<TEBase::TCancel>() )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel::Accept() TCancel",(TInt*)&iContext.Node())
            // There's nothing to cancel as we're waiting for notification only.
            // Cancelling the notifications in standard cancel-transition is enough.
            result = ETrue;
            }
        
        // Ok, we can clear the msg.
        //
        if ( result )
            {
            // Clear.
            node.ClearPolicyNotification();
            }

        // other messages.
        //
        return result;
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TNoTagOrErrorTagOrApplicationRejectedMigrationOrCancel::TransitionTag
    // -----------------------------------------------------------------------------
    //
    DEFINE_SMELEMENT( CS60MobilityActivity::TNoTagOrApplicationRejectedMigrationOrCancel, 
                      NetStateMachine::MStateFork, CS60MobilityActivity::TContext )
    TInt CS60MobilityActivity::TNoTagOrApplicationRejectedMigrationOrCancel::TransitionTag()
        {
        if ( message_cast<TCFMobilityProvider::TMigrationRequested>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrApplicationRejectedMigrationOrCancel::TransitionTag() KNoTag",(TInt*)&iContext.Node())
            return MeshMachine::KNoTag | NetStateMachine::EForward;
            }
        if ( message_cast<TCFMobilityProvider::TMigrationRejected>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrApplicationRejectedMigrationOrCancel::TransitionTag() KApplicationRejectedMigration",(TInt*)&iContext.Node())
            return S60MCprStates::KApplicationRejectedMigration | NetStateMachine::EForward; 
            }
        if ( message_cast<TEBase::TCancel>(&iContext.iMessage) )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrApplicationRejectedMigrationOrCancel::TransitionTag() KCancelTag Generating ApplicationIgnoredTheCarrier",(TInt*)&iContext.Node())
            
            // Need to put MPM in consistent state and continue.
            CS60MetaConnectionProvider& node = (CS60MetaConnectionProvider&)iContext.Node();
            CS60MobilityActivity& activity = static_cast<CS60MobilityActivity&>(*iContext.iNodeActivity);
            TRAP_IGNORE( node.Policy()->ApplicationIgnoredTheCarrierL( activity.iPreferredAPId ) );

            return MeshMachine::KCancelTag | NetStateMachine::EForward;
            }
        S60MCPRLOGSTRING1("S60MCPR<%x>::TNoTagOrApplicationRejectedMigrationOrCancel::TransitionTag() KErrorTag ESock_IP Panic",(TInt*)&iContext.Node())
        ASSERT(EFalse); // error, IPCPR should not send TError in this case.
        return MeshMachine::KErrorTag | NetStateMachine::EForward;
        }

    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::ClearHandshakingFlag
    // -----------------------------------------------------------------------------
    //
    void CS60MobilityActivity::SetHandshakingFlag()
        {
        static_cast<CS60MetaConnectionProvider&>(iNode).SetHandshakingFlag();
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::ClearHandshakingFlag
    // -----------------------------------------------------------------------------
    //
    void CS60MobilityActivity::ClearHandshakingFlag()
        {
        static_cast<CS60MetaConnectionProvider&>(iNode).ClearHandshakingFlag();
        }


    // -----------------------------------------------------------------------------
    // CS60MobilityActivity::TConnectionRecoveryOrMobilityPreferenceOrErrorMutex::IsBlocked
    // -----------------------------------------------------------------------------
    //
    TBool CS60MobilityActivity::TConnectionRecoveryOrMobilityPreferenceOrErrorMutex::IsBlocked( MeshMachine::TNodeContextBase& aContext ) 
        {
        if ( aContext.iNode.CountActivities( ECFActivityConnectionStartRecovery ) != 0 ||
             aContext.iNode.CountActivities( ECFActivityConnectionGoneDownRecovery ) != 0 )
            {
            S60MCPRLOGSTRING1("S60MCPR<%x>::TConnectionRecoveryOrMobilityPreferenceOrErrorMutex::IsBlocked() BLOCKED!",(TInt*)&aContext.Node())
            return ETrue;
            }
        // In all other cases we have received a preferred carrier available from 
        // policy server and we need to act on it. 
        return EFalse;
        }

    } // namespace S60MCprMobilityActivity

//  End of File

