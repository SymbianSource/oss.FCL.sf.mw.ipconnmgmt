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
* Description: S60 MCPR's states
*
*/


/**
@file s60mcprstates.h
S60 MCPR's states.
*/

#ifndef S60MCPRSTATES_H
#define S60MCPRSTATES_H

#include <comms-infras/mobilitymcprstates.h>
#include <comms-infras/ss_nodemessages_legacy.h>
#include <elements/nm_messages_errorrecovery.h>

#include "s60mcpr.h"
#include "s60mpmrequests.h"
#include "s60mcpractivityids.h"

/**
 * S60McprState namespace packages all S60 specific state, forks and 
 * transitions which can be utilized from all S60MCPR activities.
 */
namespace S60MCprStates
    {
    /**
     * Network Meta Connection Provider Transition Ids 11000..20000
     */
    //const TInt KAwaitAvailabilityNotification = 11000;
    //const TInt KProcessAvailabilityNotification = 11001;
    // Transition IDs for ChooseBestIAP
    const TInt KReselectBestIAPNeeded = 11002;
    const TInt KChooseBestIAPRequired = 11003;
    // Transition IDs for ErrorRecoveryRequest
    const TInt KIgnoreError = 11020;
    const TInt KRetryConnection = 11021; 
    const TInt KSetChosenApActive = 11022;
    const TInt KProcessError = 11023;
    // Mobility transition ids.
    const TInt KApplicationRejectedMigration = 11050;
    const TInt KWaitPolicyServerResponse = 11051;
    const TInt KInformMigrationAvailable = 11052;
    const TInt KSendInitialApplicationReject = 11053; 
    const TInt KConsumeRejectedMsg = 11054;
    const TInt KInformMigrationCompleted = 11055;
    const TInt KRequestReConnectToCurrentSP = 11056;
    const TInt KStopActivityNotRunning = 11057;
    
    /**
     * Execution context
     */
    typedef MeshMachine::TNodeContext<CS60MetaConnectionProvider, MobilityMCprStates::TContext> TContext;
    /**
     * Status change information.
     * TRANSITION/ACTION: Sends IAP status notification to MPM.
     * 
     * Actually the notification is not directly IAP related but without any other
     * information we must try to interpret the IPCPR status messages. 
     */
    DECLARE_SMELEMENT_HEADER( THandleMPMStatusChange, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    void DoL();
    DECLARE_SMELEMENT_FOOTER( THandleMPMStatusChange )
    /**
     * Combined transition that handles the MPM interaction and core functionality. 
     */
    DECLARE_AGGREGATED_TRANSITION3( THandleMPMAndCoreDataClientStatusChangeAndDestroyOrphans,
                                    S60MCprStates::THandleMPMStatusChange,
                                    PRStates::THandleDataClientStatusChange,
                                    PRStates::TDestroyOrphanedDataClients )

    /**
     * Generic ReselectBestIAP state that can be used before JustInTimeSelection.
     * TRANSITION/ACTION: Sends Reselect IAP request to Policy Server.
     */
    DECLARE_SMELEMENT_HEADER( TReselectBestIAP, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TReselectBestIAP )

    /**
     * TRANSITION/ACTION: Starts ReConnect -activity
     * Checks the IPProto MCPR tree below and picks the current one
     * and the active one and makes a reconnect request for them.
     */
    DECLARE_SMELEMENT_HEADER( TRequestReConnect, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRequestReConnect )
    
    /**
     * TRANSITION/ACTION: Requests Policy Server to process the error.
     * The error is taken from the error context or activity error in that order.
     */
    DECLARE_SMELEMENT_HEADER( TProcessError, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition,
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TProcessError )

    /**
     * TRANSITION/ACTION: Error no bearer request while in gone down recovery.
     */
    DECLARE_SMELEMENT_HEADER( THandleNoBearerDuringGoneDownRecovery, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( THandleNoBearerDuringGoneDownRecovery )

    
    /**
     * STATE: Waits for valid select or error message.
     * @return ETrue if message is accepted or error is observed.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingSelectNextLayerCompletedOrError, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingSelectNextLayerCompletedOrError )
    
    /**
     * STATE: Waits for valid select or error message.
     * @return ETrue if message is accepted or error is observed.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingReselectBestIAPCompleteOrError, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingReselectBestIAPCompleteOrError )
    
    /**
     * STATE: Waits for PolicyServer response for the process error request.
     * @return ETrue if message is accepted.
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingProcessErrorCompleteOrError, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext)
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingProcessErrorCompleteOrError )
    
    /**
     * STATE: Waits for ServiceId request. 
     * @return ETrue if message is accepted. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingServiceIdRequest, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingServiceIdRequest )

    /**
     * STATE: Waits for NoBearer request while in gone down recovery. 
     * @return ETrue if message is accepted. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingNoBearerInGoneDownRecovery, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingNoBearerInGoneDownRecovery )
    

    /**
     * TRANSITION/ACTION: Retrieve ServiceId.
     */
    DECLARE_SMELEMENT_HEADER( TRetrieveServiceId, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRetrieveServiceId )

    /**
     * STATE: Waits for StopIAPNotification message. 
     * @return ETrue if message is accepted. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingStopIAPNotification, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingStopIAPNotification )

    /**
     * TRANSITION/ACTION: Sends Stop to Service Provider
     */
    DECLARE_SMELEMENT_HEADER( TSendStop, 
                              MeshMachine::TStateTransition<TContext>, 
                              NetStateMachine::MStateTransition, 
                              TContext )
    virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendStop )
    
    
    /**
     * STATE: Waits for Stopped or Error
     * @return ETrue if message is accepted. 
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingStoppedOrError, 
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingStoppedOrError )
    
    
    /**
     * STATE: Waits for TIdle
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingDataClientIdle,
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientIdle )

    /**
     * STATE: Waits for TDataClientStatusChange
     */
    DECLARE_SMELEMENT_HEADER( TAwaitingDataClientStatusChange,
                              MeshMachine::TState<TContext>, 
                              NetStateMachine::MState, 
                              TContext )
    virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingDataClientStatusChange )
    
    
    /** 
     * FORK/MUTEX: Waits for StopIAPActivity to complete
     */
    DECLARE_SERIALIZABLE_STATE( TStopActivityNotRunning,
                                MeshMachine::TActivityIdMutex<ECFActivityS60McprStopIAP>,
                                MeshMachine::TTag<S60MCprStates::KStopActivityNotRunning> )
        
    class CS60ErrorRecoveryActivity : public MeshMachine::CNodeRetryActivity
        {
        public:
            CS60ErrorRecoveryActivity( const MeshMachine::TNodeActivity& aActivitySig, 
                                       MeshMachine::AMMNodeBase& aNode )
                :   MeshMachine::CNodeRetryActivity( aActivitySig, aNode )
                {}
            /**
             * Error context on which this activity started
             */
            Messages::TErrContext iOriginalErrContext;
        };
    
    /**
     * CReselectBestIAPCb is a callback class that receives the policy server response
     * and converts PolicyResponse -event into meshmachine compatible message. 
     * 
     * PolicyServer wrapper API takes pointer of the class implementing the 
     * MMPMPolicyRequestsUser -interface. The ownership is passed to the wrapper API
     * because the answer may be delayed and live longer than the execution context 
     * of the MCPR.
     */
    class CReselectBestIAPCb : public CBase, public MMPMPolicyRequestsUser
        {
        public:
            /**
             * C++ constructor.
             * 
             * @param aNode reference to S60 MCPR running the Activity.
             * @param aActivity pointer to the Activity that requests for PolicyResponse. 
             */
            CReselectBestIAPCb( CS60MetaConnectionProvider& aNode, 
                                MeshMachine::CNodeActivityBase* aActivity )
                :   iNode(aNode) 
                { 
                iLastRequestOriginator.Open( aNode.SelfInterface(),
                                             Messages::TNodeCtxId( aActivity->ActivityId(), 
                                                                   aNode.Id() ));
                }
            /**
             * Called when Policy Server responds to the request.
             * @param aCompletedRequest contains reference to answer from Policy Server.
             */
            virtual void PolicyResponse( PolicyRequest& aCompletedRequest );
            /**
             * Interface function that calls CBase-destructor
             */
            virtual void Delete() { delete this; }
        protected:
            /**
             * C++ destructor
             */
            virtual ~CReselectBestIAPCb() {}
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
    
    /**
     * CProcessErrorCb is a callback class that receives the policy server response
     * and converts PolicyResponse -event into meshmachine compatible message. 
     * 
     * PolicyServer wrapper API takes pointer of the class implementing the 
     * MMPMPolicyRequestsUser -interface. The ownership is passed to the wrapper API
     * because the answer may be delayed and live longer than the execution context 
     * of the MCPR.
     */
    class CProcessErrorCb
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
            CProcessErrorCb( CS60MetaConnectionProvider& aNode, 
                             MeshMachine::CNodeActivityBase* aActivity )
                :   iNode(aNode) 
                { 
                iLastRequestOriginator.Open( aNode.SelfInterface(), 
                                             Messages::TNodeCtxId( aActivity->ActivityId(), 
                                                                   aNode.Id() ));
                }
            /**
             * Called when Policy Server responds to the request.
             * @param aCompletedRequest contains reference to answer from Policy Server.
             */
            virtual void PolicyResponse( PolicyRequest& aCompletedRequest );
            /**
             * Interface function that calls CBase-destructor
             */
            virtual void Delete() { delete this; }
        protected:
            /**
             * C++ destructor
             */
            virtual ~CProcessErrorCb() {}
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

    } // namespace S60MCprStates

#endif //S60MCPRSTATES_H

