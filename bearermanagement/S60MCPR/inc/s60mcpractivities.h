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
* Description: S60 MCPR's activities.
*
*/

/**
@file s60mcpractivities.h
S60 MCPR's activities.
*/

#ifndef S60MCPR_ACTIVITIES_H
#define S60MCPR_ACTIVITIES_H

#include <comms-infras/mobilitymcpractivities.h>

#include "s60mcprstates.h"

// Forward declarations
class CS60MetaConnectionProvider;

enum TS60MCprActivities
    {
    ECFActivityS60McprServiceIdMessage2Handler = ESock::ECFActivityCustom + 10
    };

/**
 * S60MCprActivities namespace contains the 
 * definition of S60MCPR's activity map. 
 */
namespace S60MCprActivities
    {
    DECLARE_ACTIVITY_MAP(S60MCprActivityMap)
    } // namespace S60MCprActivities


/**
 * S60MCprSelectActivity namespace packages the
 * S60 MCPR's connection selection activity related
 * states, transitions and forks.
 * 
 * The activity doesn't need activity class because it
 * reuses the Symbian's core implementation for 
 * selection.
 * 
 */
namespace S60MCprSelectActivity
    {
    /**
     * Execution context definition for meshmachine activities. 
     */
    typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MobilityMCprStates::TContext> TContext;

    /**
     * STATE: Waits for valid select message.
     * @return ETrue if message is accepted.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayer, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayer )

    /**
     * TRANSITION/ACTION: Makes the selection for next layer based on policy or other obtained preferences. 
     */
    DECLARE_SMELEMENT_HEADER( TSelectNextLayer, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSelectNextLayer )

    /**
     * TRANSITION/ACTION: Requests PolicyServer for Best IAP.  
     */
    DECLARE_SMELEMENT_HEADER( TSimpleSelect, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSimpleSelect )

    /**
     * STATE: Waits for PolicyServer response
     * @return ETrue if valid message policyresponse is received.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingSimpleSelectComplete, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingSimpleSelectComplete )
    }

/**
 * S60MCprSimpleSelectActivity namespace packages the
 * S60 MCPR's connection selection activity related
 * states, transitions and forks.
 * 
 * The activity doesn't need activity class because it
 * reuses the Symbian's core implementation for 
 * selection.
 * 
 */
namespace S60MCprSimpleSelectActivity
    {
    /**
     * Execution context definition for meshmachine activities. 
     */
    typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MobilityMCprStates::TContext> TContext;
    /**
     * STATE: Waits for valid select message.
     * @return ETrue if message is accepted.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayer, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayer )
    /**
     * TRANSITION/ACTION: Makes the selection for next layer based on policy or other obtained preferences. 
     */
    DECLARE_SMELEMENT_HEADER( TSelectNextLayer, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSelectNextLayer )
    /**
     * TRANSITION/ACTION: Requests PolicyServer for Best IAP.  
     */
    DECLARE_SMELEMENT_HEADER( TChooseBestIAP, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TChooseBestIAP )
    /**
     * STATE: Waits for PolicyServer response
     * @return ETrue if valid message policyresponse is received.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingChooseBestIAP, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingChooseBestIAP )
    /**
     * FORK/DECISION: Decides between three different activity execution paths: 
     * 1. Request PolicyServer for connection preferences.
     * 2. Proceed and use existing connection preferences.
     * 3. Bypass selection and just mark new AP active.
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TChooseBestIAPRequiredOrNoTagOrSetChosenApActive, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TChooseBestIAPRequiredOrNoTagOrSetChosenApActive )
    /**
     * FORK/DECISION: Decides between two different activity execution path.  
     * 1. Mark new chose AP active.
     * 2. Reverse back to next layer tiermanager selection.
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TSetChosenApActiveOrSelectedProviderBackward, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TSetChosenApActiveOrSelectedProviderBackward )
    /**
     * TRANSITION/ACTION: Marks the chosen AP active.
     * 
     * Transition assumes that the chose AP is already bound to the MCPR 
     * and will be found from the ServiceProvider -list.
     */
    DECLARE_SMELEMENT_HEADER( TSetChosenApActive, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSetChosenApActive )
    /**
     * CChooseBestIAPCb is a callback class that receives the policy server response
     * and converts PolicyResponse -event into meshmachine compatible message. 
     * 
     * PolicyServer wrapper API takes pointer of the class implementing the 
     * MMPMPolicyRequestsUser -interface. The ownership is passed to the wrapper API
     * because the answer may be delayed and live longer than the execution context 
     * of the MCPR.
     */
    class CChooseBestIAPCb 
        :   public CBase, 
            public MMPMPolicyRequestsUser
        {
        public:
            /**
             * C++ constructor.
             * 
             * @param aNode reference to S60 MCPR running the Activity.
             * @param aActivity pointer to the Activity that requests for PolicyResponse. 
             */
            CChooseBestIAPCb( CS60MetaConnectionProvider& aNode, 
                              MeshMachine::CNodeActivityBase* aActivity )
                :   iNode(aNode) 
                { 
                iLastRequestOriginator.Open( aNode.SelfInterface(), 
                                             Messages::TNodeCtxId( aActivity->ActivityId(), 
                                                                   aNode.Id() ));
                }
        public: // From MMPMPolicyRequestsUser
            /**
             * Called when Policy Server responds to the request.
             * @param aCompletedRequest contains reference to answer from Policy Server.
             */
            virtual void PolicyResponse( PolicyRequest& aCompletedRequest );
            /**
             * Interface function that calls CBase-destructor
             */
            virtual void Delete() { delete this; }
        protected: // From CBase
            /**
             * C++ destructor.
             */
            virtual ~CChooseBestIAPCb() {}
        private:
            /**
             * Valid reference to CS60MetaConnectionProvider
             */ 
            CS60MetaConnectionProvider& iNode;
            /**
             * Valid reference to request originator.
             */ 
            Messages::RRequestOriginator iLastRequestOriginator;
        };

    } // namespace S60MCprSimpleSelectActivity

/**
 * S60MCprErrorRecoveryActivity namespace packages the
 * S60 MCPR's error recovery activity related
 * states, transitions and forks.
 */
namespace S60MCprErrorRecoveryActivity
    {
    /**
     * Execution context definition for meshmachine activities. 
     */
    typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MCprStates::TContext> TContext;
    /**
     * Mutex that blocks the activity during the mobility handshake.  
     */
    class THandshakingMobilityMutex
        {
        public:
            /**
             * @param aContext execution context.
             * @return ETrue if mobility handshake is in progress.
             */
            static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
        };
    /**
     * Serialized state. 
     * Idea is to wait in this state until the mobility handshake is done. 
     */
    DECLARE_SERIALIZABLE_STATE( TProcessErrorBlockedByMobilityHandshaking,
                                THandshakingMobilityMutex,
                                MeshMachine::TTag<S60MCprStates::KProcessError> )

    /**
     * CS60ConnectionRecoveryActivity is the S60 version of the ErrorRecovery -activity
     * that is responsible to process connection errors whenever it happens.
     */
    class CS60ConnectionRecoveryActivity : public S60MCprStates::CS60ErrorRecoveryActivity
        {
        public:

            /**
             * Symbian two-phase constructor.
             */
            static MeshMachine::CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, 
                                                         MeshMachine::AMMNodeBase& aNode );
            /**
             * Sends response to request originators.
             */
            void ReplyToOriginators( Messages::TEErrorRecovery::TErrorRecoveryResponse& aCFMessageSig );

        protected:
            /**
             * Context definition
             */
            typedef MeshMachine::TNodeContext<CMobilityMetaConnectionProvider, MCprStates::TContext> TContext;

        public:
            /**
             * STATE: Waits for reconnect activity to complete.
             */
            DECLARE_SMELEMENT_HEADER( TAwaitingReConnectComplete, 
                                      MeshMachine::TState<TContext>, 
                                      NetStateMachine::MState, 
                                      TContext )
            virtual TBool Accept();
            DECLARE_SMELEMENT_FOOTER( TAwaitingReConnectComplete )
            /**
             * TTransitionBase from Symbian error recovery
             */
            class TTransitionBase : public MeshMachine::TStateTransition<TContext>
                {
                public:
                    TTransitionBase( TContext& aContext )
                    :   MeshMachine::TStateTransition<TContext>(aContext) { /*EMPTY*/ }
                    virtual void Error(TInt aError);
                };
            /**
             * TRANSITION/ACTION: Stores error context.
             */
            DECLARE_SMELEMENT_HEADER( TStoreErrorContext, 
                                      CS60ConnectionRecoveryActivity::TTransitionBase, 
                                      NetStateMachine::MStateTransition, 
                                      TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TStoreErrorContext )
            /**
             * TRANSITION/ACTION: Sends RETRY as RecoveryResponse
             */
            DECLARE_SMELEMENT_HEADER( TSendRetryRecoveryResponse, 
                                      CS60ConnectionRecoveryActivity::TTransitionBase, 
                                      NetStateMachine::MStateTransition, 
                                      TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendRetryRecoveryResponse )
            /**
             * TRANSITION/ACTION: Sends PROPAGATE as RecoveryResponse
             */

            DECLARE_SMELEMENT_HEADER( TSendPropagateRecoveryResponse, 
                                      CS60ConnectionRecoveryActivity::TTransitionBase, 
                                      NetStateMachine::MStateTransition, 
                                      TContext)
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendPropagateRecoveryResponse )
            

            DECLARE_SMELEMENT_HEADER( TSendPropagateRecoveryErrContextResponse,
                                      CS60ConnectionRecoveryActivity::TTransitionBase, 
                                      NetStateMachine::MStateTransition, 
                                      TContext)
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendPropagateRecoveryErrContextResponse )

        protected:
            /**
             * C++ constructor.
             */
            CS60ConnectionRecoveryActivity( const MeshMachine::TNodeActivity& aActivitySig, 
                                            MeshMachine::AMMNodeBase& aNode );
        };
    
    /**
     * TRANSITION/ACTION: Sends IGNORE as RecoveryResponse
     */
    DECLARE_SMELEMENT_HEADER( TSendIgnoreRecoveryResponse, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendIgnoreRecoveryResponse )
    
    /**
     * STATE: Wait for valid error recovery request.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingNotGoneDownErrorRecoveryRequest, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingNotGoneDownErrorRecoveryRequest )

    
    /**
     * FORK/DECISION: Decides between three different activity execution paths: 
     * 1. Continue to next transition. ( reselection of AP )
     * 2. Sendiong of IGNORE to originators.
     * 3. Error handling.
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TNoTagOrIgnoreErrorOrErrorTag, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrIgnoreErrorOrErrorTag )
    
    /**
     * FORK/DECISION: Decides between three different activity execution paths: 
     * 1. Continue to next transition. ( selection of IAP )
     * 2. Retrying of currently bound MCPR
     * 3. Error handling
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TNoTagOrRetryConnectionTagOrErrorTag, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrRetryConnectionTagOrErrorTag )
    
    /**
     * FORK/DECISION: Decides between two different activity execution paths: 
     * 1. Retrying of currently bound MCPR
     * 2. Error handling through process error
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TRetryConnectionOrProcessErrorTagBackward, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TRetryConnectionOrProcessErrorTagBackward )

    /**
     * FORK/DECISION: Decides between two different activity execution paths: 
     * 1. Error handling through process error
     * 2. Else continue
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TNoTagOrProcessErrorBackwardTag, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrProcessErrorBackwardTag )

    } // namespace S60MCprErrorRecoveryActivity

/**
 * S60MCprMobilityActivity namespace packages the
 * S60 MCPR's mobility activity related
 * states, transitions and forks.
 */
namespace S60MCprMobilityActivity
    {
    /**
     * Execution context definition for meshmachine activities. 
     */
    typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MCprStates::TContext> TContext;
    
    /**
     * TRANSITION/ACTION: Does nothing or resends the earlier received notification.
     */
    DECLARE_SMELEMENT_HEADER( TDoNothingOrReSendPendingNotification, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TDoNothingOrReSendPendingNotification )
    
    /**
     * TRANSITION/ACTION: Responds back to IPCPR that mobility session has been started.
     */
    DECLARE_SMELEMENT_HEADER( TReplyMobilityStarted, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TReplyMobilityStarted )
    
    /**
     * TRANSITION/ACTION: Registers to preferred carrier notifications.
     */
    DECLARE_SMELEMENT_HEADER( TSendAvailabilityRequest, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendAvailabilityRequest )

    /**
     * TRANSITION/ACTION: Application rejects active carrier.
     */
    DECLARE_SMELEMENT_HEADER( TSendInitialApplicationReject, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendInitialApplicationReject )

    /**
     * STATE: Waits for TStartMobility message.
     * @return ETrue if message is valid.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingStartMobility, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingStartMobility )

    /**
     * STATE: Waits for TAwaitingMigrationRequestedOrRejectedOrCancel.
     * @return ETrue if message is valid either of those or cancel.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingMigrationRequestedOrRejectedOrCancel, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingMigrationRequestedOrRejectedOrCancel )

    /**
     * STATE: Waits for application response.
     * @return ETrue if Migration Accepted or Rejected or Start-up Error Ignored. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel )

    /**
     * STATE: Waits for IPCPR rejected response that ends the mobility handshake.
     * @return ETrue if Rejected. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingMigrationRejected, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingMigrationRejected )
    
    
    /**
     * FORK/DECISION: Returns the KStartMobilityHandshake backwards or error
     * @return KStartMobilityHandshake backwards or error
     */
    DECLARE_SMELEMENT_HEADER( TStartMobilityHandshakeBackwardsOrError, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TStartMobilityHandshakeBackwardsOrError )

    /**
     * FORK/DECISION: Decides if needs cancel or not.
     */
    DECLARE_SMELEMENT_HEADER( TInformMigrationAvailableOrCancelTag, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TInformMigrationAvailableOrCancelTag )

    /**
     * FORK/DECISION: Decides what happens after IPCPR application completes the 
     * three way handshake.
     * 1. Application accepted, continue to send accept to policy server.
     * 2. Error
     * 3. Application rejected, continue to send reject to policy server.
     * 4. Error process occured, consume the next rejected msg/accept msg.
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected )
    
    /**
     * FORK/DECISION: Decides whether to wait MPM actions or report an error. 
     * 1. Wait for further actions. ReSelect is NOT supported.
     * 2. Error
     * @return TransitionTag of the selected transition.
     */
    DECLARE_SMELEMENT_HEADER( TWaitPolicyServerResponseBackwardOrErrorTag, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TWaitPolicyServerResponseBackwardOrErrorTag )

    /**
     * FORK/DECISION: 
     * 1. Continue with handshake to select new AP for MCPR.
     * 2. Return back to migration-phase with new preferred IAP.
     * 3. Close down the system in case of mobilityerror.
     * @return
     */
    DECLARE_SMELEMENT_HEADER( TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel, 
                              MeshMachine::TStateFork<TContext>, 
                              NetStateMachine::MStateFork, 
                              TContext )
    virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel )

    /**
     * Class that is responsible for managing mobility on MCPR side.
     * CS60MobilityActivity is the S60 version of the CMobilityActivity -class
     */
    class CS60MobilityActivity : public S60MCprStates::CS60ErrorRecoveryActivity
        {
        public:
            /**
             * Symbian two-phase constructor
             */
            static MeshMachine::CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, 
                                                         MeshMachine::AMMNodeBase& aNode );

            /**
             * CS60MobilityActivity -context definition 
             */
            typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MCprStates::TContext> TContext;

            /**
             * Inner mutex class. Takes care that mobility is not started while 
             * activity/node is doing something more important.
             */
            class TConnectionRecoveryOrMobilityPreferenceOrErrorMutex
                {
                public:
                    /**
                     * @return ETrue if mobility activity can't run
                     */
                    static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
                };

        public: // Mesh Machine elements

            /**
             * TRANSITION/ACTION: Sends migration rejected message 
             */
            DECLARE_SMELEMENT_HEADER( TSendMigrationRejected, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendMigrationRejected )

            /**
             * TRANSITION/ACTION: Sends migration rejected message 
             */
            DECLARE_SMELEMENT_HEADER( TSendMigrationAccepted, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendMigrationAccepted )

            /**
             * TRANSITION/ACTION: Clears the handshaking flag
             */
            DECLARE_SMELEMENT_HEADER( TClearHandshakingFlag, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TClearHandshakingFlag )

            // EMPTY
            DECLARE_SMELEMENT_HEADER( TSendAvailabilityRequest, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendAvailabilityRequest )

            /**
             * TRANSITION/ACTION: Sends migration available notification to IPCPR. 
             */
            DECLARE_SMELEMENT_HEADER( TInformMigrationAvailableAndSetHandshakingFlag, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TInformMigrationAvailableAndSetHandshakingFlag )

            // Symbian comment: For the moment it is sufficient to use the re-connect activity, in the future we may want to
            // customise the behavior, for example start the new layer before rebinding it, etc.
            // EMPTY
            DECLARE_SMELEMENT_HEADER(TRequestReConnect, 
                                     MeshMachine::TStateTransition<TContext>, 
                                     NetStateMachine::MStateTransition,
                                     CS60MobilityActivity::TContext)
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER(TRequestReConnect)

            /**
             * TRANSITION/ACTION: Informs IPCPR that MCPR has migrated. 
             */
            DECLARE_SMELEMENT_HEADER( TInformMigrationCompleted, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TInformMigrationCompleted )

            /**
             * TRANSITION/ACTION: Sends Application Accepted Connection 
             */
            DECLARE_SMELEMENT_HEADER( TSendApplicationAccept, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendApplicationAccept )
            
            /**
             * TRANSITION/ACTION: Sends Application Rejected Connection 
             */
            DECLARE_SMELEMENT_HEADER( TSendApplicationReject, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TSendApplicationReject )

			/**
			 * TRANSITION/ACTION: Stop the mcpr's data client (should only ever be one) and error mobility on the cpr
			 */
			DECLARE_SMELEMENT_HEADER( TErrorOriginatorAndStopDataClient, 
				                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
			virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TErrorOriginatorAndStopDataClient )


            /**
             * TRANSITION/ACTION: Unregister preferred carrier notifications.
             */
            DECLARE_SMELEMENT_HEADER( TCancelMobilityActivity, 
                                      MeshMachine::TStateTransition<TContext>, 
                                      NetStateMachine::MStateTransition, 
                                      CS60MobilityActivity::TContext )
            virtual void DoL();
            DECLARE_SMELEMENT_FOOTER( TCancelMobilityActivity )
            
            /**
             * STATE: Waits for response to the MigrationAccepted message.
             * @return ETrue if new PreferredCarrier arrives, new StartIAPNotification arrives or error arrives.
             */
            DECLARE_SMELEMENT_HEADER( TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel, 
                                      MeshMachine::TState<TContext>, 
                                      NetStateMachine::MState, 
                                      CS60MobilityActivity::TContext)
            virtual TBool Accept();
            DECLARE_SMELEMENT_FOOTER( TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel )

            /**
             * STATE: Waits for preferred carrier message.
             * @return ETrue if preferred carrier notification is received.
             */
            DECLARE_SMELEMENT_HEADER( TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification, 
                                      MeshMachine::TState<TContext>, 
                                      NetStateMachine::MState, 
                                      CS60MobilityActivity::TContext)
            virtual TBool Accept();
            DECLARE_SMELEMENT_FOOTER( TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification )
            
            /**
             * FORK/DECISION: Decides what happens after IPCPR responds to migration offer.
             * 1. Proceed to migration if asked.
             * 2. Proceed to cancel.
             * 3. Go back to wait for new preferred connection.
             * @return TransitionTag of the selected transition.
             */
            DECLARE_SMELEMENT_HEADER( TNoTagOrApplicationRejectedMigrationOrCancel, 
                                      MeshMachine::TStateFork<TContext>, 
                                      NetStateMachine::MStateFork, 
                                      CS60MobilityActivity::TContext )
            virtual TInt TransitionTag();
            DECLARE_SMELEMENT_FOOTER( TNoTagOrApplicationRejectedMigrationOrCancel )

            /**
             * Mutex protected TRANSITION.
             * Mopility scenario should wait until the connection recovery finishes.
             */
            DECLARE_SERIALIZABLE_TRANSITION( TInformMigrationAvailableAndSetHandshakingFlagBlockedByConnectionRecoveryOrNoMobilityPreferenceOrError,
                                             CS60MobilityActivity::TConnectionRecoveryOrMobilityPreferenceOrErrorMutex,
                                             CS60MobilityActivity::TInformMigrationAvailableAndSetHandshakingFlag )

        protected: // Methods.
            
            /**
             * C++ constructor
             */
            CS60MobilityActivity( const MeshMachine::TNodeActivity& aActivitySig, 
                                  MeshMachine::AMMNodeBase& aNode );
            
            //TBool Signal( MeshMachine::TNodeContextBase& aContext );

        private: // Private methods.
            
            /**
             * Sets on MCPR wide mobility handshaking flag.  
             */
            void SetHandshakingFlag();

            /**
             * Clears MCPR wide mobility handshaking flag.  
             */
            void ClearHandshakingFlag();

        private: // Member variables.
            
            TUint iCurrentAssumedAPId;  // Current IAP Id
            TUint iPreferredAPId;       // New IAP Id system should roam to.
            TBool iIsUpgrade;           // If the active iPreferredAPId is upgrade to old one.
            TBool iIsSeamless;          // *not used* if the roaming is seamless from the IP point of view 
        
        };

	DECLARE_SMELEMENT_HEADER( TProcessSimpleSelectionPolicy, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TProcessSimpleSelectionPolicy )

    } // namespace S60MCprMobilityActivity

#endif // S60MCPR_ACTIVITIES_H

