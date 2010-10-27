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
* Description: S60 MCPR activities implementation
*
*/

/**
@file s60mcpractivities.cpp
S60 MCPR activities implementation
*/

#include "s60mcpractivities.h"
#include "s60mcpractivityids.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace MCprActivities;

// -----------------------------------------------------------------------------
// S60MCprSelectActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprSelectActivity
    {
	namespace Simple = S60MCprSimpleSelectActivity;
	
	DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivitySelect, 
	                                    S60MCprSelect,
	                                    TCFSelector::TSelect,
	                                    CS60SelectActivity::NewL )
    
    // Reply from TAwaitingSelectNextLayer if no choices, otherwise accept
    FIRST_NODEACTIVITY_ENTRY( TAwaitingSelectNextLayer,
                              Simple::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive )
    
    // Execute ChooseBestIAPL() to MPM to resolve the most preferred IAP.
    NODEACTIVITY_ENTRY( S60MCprStates::KChooseBestIAPRequired,
                        Simple::TChooseBestIAP,
                        Simple::TAwaitingChooseBestIAP,
                        MeshMachine::TNoTag )
    
    // ChooseBestIAP done or not required
    THROUGH_NODEACTIVITY_ENTRY( KNoTag,
                                MeshMachine::TDoNothing,
                                MCprStates::TSelectedProvider )
    
    // Start the selection main loop
    NODEACTIVITY_ENTRY( MCprStates::KSelectedProvider, 
                        CSelectNextLayerActivity::TFindOrCreateTierManager, 
                        MCprStates::TAwaitingTierManagerCreated, 
                        MeshMachine::TNoTag )
    
    // Join the newly created tier manager
    NODEACTIVITY_ENTRY( KNoTag, 
                        CSelectNextLayerActivity::TJoinTierManager, 
                        CoreStates::TAwaitingJoinComplete, 
                        MeshMachine::TNoTag )
    
    // Select next provider and enter the selection internal loop if provider received. Break if SelectComplete(NULL).
    // Then post the selection list to TM.
    NODEACTIVITY_ENTRY( KNoTag,
                        TSelectNextLayer,
                        MCprStates::TAwaitingSelectComplete, 
                        CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull )
    
    // Add provider info
    NODEACTIVITY_ENTRY( KNoTag, 
                        CSelectNextLayerActivity::TAddProviderInfo, 
                        MCprStates::TAwaitingSelectComplete, 
                        CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider )
    
    // Break the selection internal loop if SelectComplete(NULL), otherwise stay in this tripple
    NODEACTIVITY_ENTRY( MCprStates::KJoinServiceProvider,
                        CSelectNextLayerActivity::TJoinServiceProvider,
                        CoreStates::TAwaitingJoinComplete,
                        MeshMachine::TNoTag )
    
    // Send select complete
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                CSelectNextLayerActivity::TSendSelectComplete,
                                CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward )
    
    // Break the selection main loop if no more choices, otherwise go back again
    THROUGH_NODEACTIVITY_ENTRY( MCprStates::KSelectedProviderIsNull,
                                CSelectNextLayerActivity::TLeaveTierManager, 
                                Simple::TSetChosenApActiveOrSelectedProviderBackward )
    
    // Set Selected Node Active
    THROUGH_NODEACTIVITY_ENTRY( S60MCprStates::KSetChosenApActive,
                                Simple::TSetChosenApActive, 
                                MeshMachine::TNoTag )
    
    // Finish the activity
    LAST_NODEACTIVITY_ENTRY( KNoTag,
                             MCprStates::TSendFinalSelectComplete )
    
    NODEACTIVITY_END()
    }

// -----------------------------------------------------------------------------
// S60MCprSimpleSelectActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprSimpleSelectActivity
    {
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivitySelect, 
                                        MCprSimpleSelect, 
		                                TCFSelector::TSimpleSelect,
		                                S60MCprSelectActivity::CS60SelectActivity::NewL )
    // Reply from TAwaitingSelectNextLayer if no choices, otherwise accept
    FIRST_NODEACTIVITY_ENTRY( S60MCprSimpleSelectActivity::TAwaitingSelectNextLayer,
                              S60MCprSimpleSelectActivity::TChooseBestIAPRequiredOrNoTagOrSetChosenApActive )
    // Execute ChooseBestIAPL() to MPM to resolve the most preferred IAP.
    NODEACTIVITY_ENTRY( S60MCprStates::KChooseBestIAPRequired,
                        S60MCprSimpleSelectActivity::TChooseBestIAP,
                        S60MCprSimpleSelectActivity::TAwaitingChooseBestIAP,
                        MeshMachine::TNoTag )
    // ChooseBestIAP done or not required
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
		                        MeshMachine::TDoNothing,
                                MCprStates::TSelectedProvider )
    // Start the selection main loop
    NODEACTIVITY_ENTRY( MCprStates::KSelectedProvider, 
                        CSelectNextLayerActivity::TFindOrCreateTierManager, 
                        MCprStates::TAwaitingTierManagerCreated, 
                        MeshMachine::TNoTag )
    // Join the newly created tier manager
    NODEACTIVITY_ENTRY( KNoTag,
                        CSelectNextLayerActivity::TJoinTierManager, 
                        CoreStates::TAwaitingJoinComplete, 
                        MeshMachine::TNoTag )
    // Select next provider and enter the selection internal loop if provider received. Break if SelectComplete(NULL).
    // Then post the selection list to TM.
    NODEACTIVITY_ENTRY( KNoTag, 
                        S60MCprSimpleSelectActivity::TSelectNextLayer, 
                        MCprStates::TAwaitingSelectComplete, 
                        CSelectNextLayerActivity::TNoTagOrSelectedProviderIsNull )
    // Add provider info
    NODEACTIVITY_ENTRY( KNoTag, 
                        CSelectNextLayerActivity::TAddProviderInfo, 
                        MCprStates::TAwaitingSelectComplete, 
                        CSelectNextLayerActivity::TNoTagBackwardsOrJoinServiceProvider )
    // Break the selection internal loop if SelectComplete(NULL), otherwise stay in this tripple
    NODEACTIVITY_ENTRY( MCprStates::KJoinServiceProvider, 
                        CSelectNextLayerActivity::TJoinServiceProvider, 
                        CoreStates::TAwaitingJoinComplete, 
                        MeshMachine::TNoTag )
    // Send select complete
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                CSelectNextLayerActivity::TSendSelectComplete, 
                                CSelectNextLayerActivity::TSelectedProviderIsNullOrJoinServiceProviderBackward )
    // Break the selection main loop if no more choices, otherwise go back again
    THROUGH_NODEACTIVITY_ENTRY( MCprStates::KSelectedProviderIsNull, 
                                CSelectNextLayerActivity::TLeaveTierManager, 
                                S60MCprSimpleSelectActivity::TSetChosenApActiveOrSelectedProviderBackward )
    // Set Selected Node Active
    THROUGH_NODEACTIVITY_ENTRY( S60MCprStates::KSetChosenApActive,
                                S60MCprSimpleSelectActivity::TSetChosenApActive,
                                MeshMachine::TNoTag )
    // Finish the activity
    LAST_NODEACTIVITY_ENTRY( KNoTag, 
                             MCprStates::TSendFinalSelectComplete )
    NODEACTIVITY_END()
    }

// -----------------------------------------------------------------------------
// Connection Status Notification to MPM.
// -----------------------------------------------------------------------------
//
namespace S60MCprMonitorDataClientStatusActivity
    {
    DECLARE_DEFINE_NODEACTIVITY( ECFActivityMonitorProviderStatus, 
                                 MCprDataClientStatusChange, 
                                 TCFControlProvider::TDataClientStatusChange )
 
    SINGLE_NODEACTIVITY_ENTRY( S60MCprStates::THandleMPMAndCoreDataClientStatusChangeAndDestroyOrphans, 
                               CoreNetStates::TAwaitingDataClientStatusChange )
    NODEACTIVITY_END()
    }

// -----------------------------------------------------------------------------
// MCprConnectionStartRecovery -activity is overriden error processing activity 
// based on Symbian's mobilitymcpr implementation.
// -----------------------------------------------------------------------------
//
namespace S60MCprErrorRecoveryActivity
    {
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivityConnectionStartRecovery, 
                                        MCprConnectionStartRecovery,
                                        TEErrorRecovery::TErrorRecoveryRequest, 
                                        S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::NewL )
    // Intercept the reconnection request if appropriate
    FIRST_NODEACTIVITY_ENTRY( //MCprStates::TAwaitingConnectionStartRecoveryRequest,
                              S60MCprErrorRecoveryActivity::TAwaitingNotGoneDownErrorRecoveryRequest,
                              MeshMachine::TNoTag )
    // Store error context but do not do proceed during mobility handshake.
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TStoreErrorContext,
                                MeshMachine::TTag<S60MCprStates::KProcessError|NetStateMachine::EForward> )
                                //S60MCprErrorRecoveryActivity::TProcessErrorBlockedByMobilityHandshaking ) // can't block here.
    // Decide if it it possible/sensible to reconnect and retry
    NODEACTIVITY_ENTRY( S60MCprStates::KProcessError,
                        S60MCprStates::TProcessError,
                        S60MCprStates::TAwaitingProcessErrorCompleteOrError,
                        S60MCprErrorRecoveryActivity::TNoTagOrIgnoreErrorOrErrorTag )
    // ReSelection state, first execute reselect best IAP.
    NODEACTIVITY_ENTRY( KNoTag,
                        S60MCprStates::TReselectBestIAP,
                        S60MCprStates::TAwaitingReselectBestIAPCompleteOrError,
                        S60MCprErrorRecoveryActivity::TNoTagOrRetryConnectionTagOrErrorTag )
    // Then execute simpleselect.            
    NODEACTIVITY_ENTRY( KNoTag, 
                        MCprStates::TSelectNextLayer,
                        S60MCprStates::TAwaitingSelectNextLayerCompletedOrError,
                        S60MCprErrorRecoveryActivity::TNoTagOrProcessErrorBackwardTag ) 
    // This transition will leave if not possible to recover (==TSendPropagateRecoveryResponse from Transition::Error())
    NODEACTIVITY_ENTRY( KNoTag, 
                        S60MCprStates::TRequestReConnect,
                        MCprStates::TAwaitingReConnectCompleteOrError, 
                        S60MCprErrorRecoveryActivity::TRetryConnectionOrProcessErrorTagBackward )
    // Respond with ignore error
    LAST_NODEACTIVITY_ENTRY( S60MCprStates::KIgnoreError, 
                             S60MCprErrorRecoveryActivity::TSendIgnoreRecoveryResponse )
    // Respond with retry
    LAST_NODEACTIVITY_ENTRY( S60MCprStates::KRetryConnection, 
                             S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TSendRetryRecoveryResponse )
    // Respond with propagate - the reconnect failed.
    LAST_NODEACTIVITY_ENTRY( KErrorTag, 
                             S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse )
    NODEACTIVITY_END()
    } // namespace S60MCprErrorRecoveryActivity

// -----------------------------------------------------------------------------
// S60MCprMobilityActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprMobilityActivity
    {
    // Wait for start mobility message that is the representative of mobility API registration.
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivityMCprMobility, 
                                        MCprMobility, 
                                        TCFMobilityProvider::TStartMobility, 
                                        S60MCprMobilityActivity::CS60MobilityActivity::NewL )
    // The activity only makes sense after the startup sequence completed on this layer
    FIRST_NODEACTIVITY_ENTRY( S60MCprMobilityActivity::TAwaitingStartMobility, 
                              MeshMachine::TNoTag/*BlockedByNoServiceProviderStarted*/ )
    // Report to the client that we have successfully started
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                S60MCprMobilityActivity::TReplyMobilityStarted, 
                                MeshMachine::TNoTag )
    // Register for preferred carrier
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                S60MCprMobilityActivity::TSendAvailabilityRequest, 
                                TTag<MobilityMCprStates::KStartMobilityHandshake> )
    // Clear handshake status and wait for preferred carrier or mobility API close.
    NODEACTIVITY_ENTRY( MobilityMCprStates::KStartMobilityHandshake, 
                        CS60MobilityActivity::TClearHandshakingFlag,
                        CS60MobilityActivity::TAwaitingPreferredCarrierOrCancelOrRejectedOrErrorNotification, // Ok. Error is fatal.
                        S60MCprMobilityActivity::TInformMigrationAvailableOrErrorOrCancelTag )
    // Application rejected active carrier
    THROUGH_NODEACTIVITY_ENTRY( S60MCprStates::KSendInitialApplicationReject, 
                                S60MCprMobilityActivity::TSendInitialApplicationReject, 
                                TTag<S60MCprStates::KWaitPolicyServerResponse> )
    // Start mobility handshake if there is a better carrier available
    NODEACTIVITY_ENTRY( S60MCprStates::KInformMigrationAvailable, 
                        CS60MobilityActivity::TInformMigrationAvailableAndSetHandshakingFlagBlockedByConnectionRecoveryOrNoMobilityPreferenceOrError,
                        S60MCprMobilityActivity::TAwaitingMigrationRequestedOrRejectedOrCancel, // Ok. Error is fatal. 
                        CS60MobilityActivity::TNoTagOrApplicationRejectedMigrationOrCancel )
    // Informs policy server that application does not migrate
    THROUGH_NODEACTIVITY_ENTRY( S60MCprStates::KApplicationRejectedMigration,
                                CS60MobilityActivity::TSendMigrationRejected, // Ok. Error is fatal
                                S60MCprMobilityActivity::TStartMobilityHandshakeBackwardsOrError )
    // Consults policy server if the user wants to migrate or not.
    THROUGH_NODEACTIVITY_ENTRY( KNoTag,
                                CS60MobilityActivity::TSendMigrationAccepted,
                                TTag<S60MCprStates::KWaitPolicyServerResponse> )
    // Wait for Policy Server Response
    NODEACTIVITY_ENTRY( S60MCprStates::KWaitPolicyServerResponse,
                        S60MCprMobilityActivity::TDoNothingOrReSendPendingNotification,
                        CS60MobilityActivity::TAwaitingPreferredCarrierOrStartIAPNotificationOrErrorNotificationOrCancel, // Ok. Error is fatal
                        S60MCprMobilityActivity::TNoTagOrInformMigrationAvailableBackwardsOrErrorOrCancel )
    
    // Select the next layer MCPR.
    NODEACTIVITY_ENTRY( KNoTag, 
                        MCprStates::TSelectNextLayer,
                        MCprStates::TAwaitingSelectNextLayerCompleted, // Ok. Error is fatal.
                        MeshMachine::TNoTagOrErrorTag )       
                        
    // For the moment it is sufficient to use the re-connect activity, in the future we may want to
    // customise the behavior, for example start the new layer before rebinding it, etc.
    NODEACTIVITY_ENTRY( KNoTag, 
                        S60MCprStates::TRequestReConnect,
                        MCprStates::TAwaitingReConnectCompleteOrError,  // Ok. Error is fatal.
                        MeshMachine::TNoTagOrErrorTag ) //Own error handling MPM must be informed when error happens.

    // If the migration was acepted we loop again waiting for a new, preferred one
    NODEACTIVITY_ENTRY( KNoTag, 
                        CS60MobilityActivity::TInformMigrationCompleted, 
                        S60MCprMobilityActivity::TAwaitingMigrationAcceptedOrRejectedOrStartupErrorIgnoredOrCancel,
                        S60MCprMobilityActivity::TNoTagOrErrorTagOrApplicationRejectedOrConsumeRejected )

    // Informs policy server that application has accepted the carrier
    THROUGH_NODEACTIVITY_ENTRY( KNoTag,
                                CS60MobilityActivity::TSendApplicationAccept,
                                TTag<MobilityMCprStates::KStartMobilityHandshake|NetStateMachine::EBackward> )
    // Informs policy server that application has rejected the carrier
    THROUGH_NODEACTIVITY_ENTRY( S60MCprStates::KApplicationRejectedMigration,
                                CS60MobilityActivity::TSendApplicationReject,
                                TTag<S60MCprStates::KWaitPolicyServerResponse|NetStateMachine::EBackward> )
    // Waits for Migration Rejected the carrier -msg and consumes it, because MPM doesn't need it.
    // MPM already received the ProcessError which is enough.
    NODEACTIVITY_ENTRY( S60MCprStates::KConsumeRejectedMsg,
                        CS60MobilityActivity::TClearHandshakingFlag,
                        S60MCprMobilityActivity::TAwaitingMigrationRejected,
                        TTag<S60MCprStates::KWaitPolicyServerResponse|NetStateMachine::EBackward> )
    // On error, we do not terminate the activity.
    // We need to inform the client and propose a new handshake should the client wish to continue
	// Shut down the layer, to release the rejected bearer
	NODEACTIVITY_ENTRY(KErrorTag,
		               CS60MobilityActivity::TErrorOriginatorAndStopDataClient,
                       CoreNetStates::TAwaitingDataClientStopped,
                       TTag<MeshMachine::KCancelTag>)

    // When closing, always continue to cancel the activity.
    LAST_NODEACTIVITY_ENTRY( KCancelTag,
                             CS60MobilityActivity::TCancelMobilityActivity )
    NODEACTIVITY_END()
    } // namespace S60MCprMobilityActivity

// -----------------------------------------------------------------------------
// S60MCprConnectionGoneDownRecoveryActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprConnectionGoneDownRecoveryActivity
    {
    // MCprConnectionGoneDownRecovery activity belongs to a group of Error Recovery Activities.
    // Error Recovery Activities need to handle their own errors (generated as well as returned).
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivityConnectionGoneDownRecovery,
                                        MCprConnectionGoneDownRecovery,
                                        TEErrorRecovery::TErrorRecoveryRequest,
                                        S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::NewL )
    // Wait for GoneDown recovery request. GoneDown handles connection errors during active connection.
    // S60MCprConnectionGoneDownRecoveryActivity only needs to run if there's a mobility activity.
    FIRST_NODEACTIVITY_ENTRY( MCprStates::TAwaitingConnectionGoneDownRecoveryRequest,
                              MeshMachine::TNoTag )
    // Either TStoreErrorContext 
    THROUGH_NODEACTIVITY_ENTRY( KNoTag, 
                                S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TStoreErrorContext, 
                                CoreStates::TNoTagOrNoPeer )
    LAST_NODEACTIVITY_ENTRY( CoreStates::KNoPeer,
                             S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TSendPropagateRecoveryErrContextResponse ) //Take error codes directly from the request
    THROUGH_NODEACTIVITY_ENTRY( KNoTag,
                                MeshMachine::TDoNothing,
                                S60MCprErrorRecoveryActivity::TProcessErrorBlockedByMobilityHandshaking )
    // TProcessError assumes that errorcontext is stored in CS60ConnectionRecoveryActivity
    // The response from the policy server should always be ignore in this case.
    NODEACTIVITY_ENTRY( S60MCprStates::KProcessError,
                        S60MCprStates::TProcessError,
                        S60MCprStates::TAwaitingProcessErrorCompleteOrError,
                        S60MCprErrorRecoveryActivity::TNoTagOrIgnoreErrorOrErrorTag )
    // We have gone down. PolicyServer response EDoReselect means nothing here and
    // will be converted to propagate error.
    // Ignore is sensible in the case of mobility API where the mobility API can cause error.
    LAST_NODEACTIVITY_ENTRY( S60MCprStates::KIgnoreError, 
                             S60MCprErrorRecoveryActivity::TSendIgnoreRecoveryResponse )
    LAST_NODEACTIVITY_ENTRY( KNoTag, 
                             S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse )
    LAST_NODEACTIVITY_ENTRY( KErrorTag, 
                             S60MCprErrorRecoveryActivity::CS60ConnectionRecoveryActivity::TSendPropagateRecoveryResponse )
    NODEACTIVITY_END()
    } // namespace S60MCprConnectionGoneDownRecoveryActivity

// -----------------------------------------------------------------------------
// S60MCprServiceIdRMessage2HandlerActivity
// -----------------------------------------------------------------------------
//
namespace S60MCprServiceIdRMessage2HandlerActivity
    {
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivityS60McprServiceIdMessage2Handler, 
                                        S60MCprServiceIdLegacyRMessage2Handler, 
                                        TNodeSignal::TNullMessageId, 
                                        MeshMachine::CNodeParallelMessageStoreActivityBase::NewL )

    FIRST_NODEACTIVITY_ENTRY( S60MCprStates::TAwaitingServiceIdRequest, 
                              MeshMachine::TNoTag )

    NODEACTIVITY_ENTRY( KNoTag, 
                        S60MCprStates::TRetrieveServiceId, 
                        CoreNetStates::TAwaitingRMessage2Processed, 
                        MeshMachine::TNoTag )

    LAST_NODEACTIVITY_ENTRY( KNoTag, 
                             CoreStates::TPostToOriginators )

    NODEACTIVITY_END()
    } // S60MCprServiceIdRMessage2HandlerActivity

// -----------------------------------------------------------------------------
// S60MCprStopIAPActivity
// -----------------------------------------------------------------------------
//

namespace S60MCprStopIAPActivity
    {
    DECLARE_DEFINE_NODEACTIVITY( ECFActivityS60McprStopIAP, 
                                 MCprStopIAPActivity, 
                                 TCFS60MCPRMessage::TMPMStopIAPNotificationMsg)

    FIRST_NODEACTIVITY_ENTRY( S60MCprStates::TAwaitingStopIAPNotification, 
                              MeshMachine::TNoTag )
                              
    NODEACTIVITY_ENTRY( KNoTag,
                        S60MCprStates::TSendStop,
                        S60MCprStates::TAwaitingStoppedOrError,
                        MeshMachine::TNoTag )

    LAST_NODEACTIVITY_ENTRY( KNoTag, 
                             MeshMachine::TDoNothing )

    NODEACTIVITY_END()
    } // S60MCprStopIAPActivity

// -----------------------------------------------------------------------------
// S60McprNoBearerDuringGoneDownErrorRecovery
// -----------------------------------------------------------------------------
//
namespace S60McprNoBearerDuringGoneDownErrorRecovery
    {
    DECLARE_DEFINE_NODEACTIVITY( ECFActivityNoBearer, 
                                 S60MCprNoBearerDuringGoneDown, 
                                 TCFControlProvider::TNoBearer )
    SINGLE_NODEACTIVITY_ENTRY( S60MCprStates::THandleNoBearerDuringGoneDownRecovery, 
                               S60MCprStates::TAwaitingNoBearerInGoneDownRecovery )
    NODEACTIVITY_END()
    }
	
// -----------------------------------------------------------------------------
// S60MCprDataClientIdleActivity
// -----------------------------------------------------------------------------
//

namespace S60MCprDataClientIdleActivity
    {
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY( ECFActivityS60McprDataClientIdle,
                                 MCprDataClientIdleActivity,
                                 TCFControlProvider::TIdle,
                                 MeshMachine::CNodeRetryActivity::NewL)
                                 
    FIRST_NODEACTIVITY_ENTRY( CoreNetStates::TAwaitingDataClientIdle,
                              S60MCprStates::TStopActivityNotRunning )  
    
    LAST_NODEACTIVITY_ENTRY( S60MCprStates::KStopActivityNotRunning,
                             CoreNetStates::THandleDataClientIdle )
                             
    NODEACTIVITY_END()
    } // S60MCprDataClientIdleActivity
                             
// -----------------------------------------------------------------------------
// S60MCprDataClientStatusChangeConsumeActivity
// -----------------------------------------------------------------------------
//

namespace S60MCprDataClientStatusChangeConsumeActivity
    {
    DECLARE_DEFINE_NODEACTIVITY( ECFActivityS60DataClientStatusChangeConsumeActivity,
                                        MCprDataClientStatusChangeConsumeActivity,
                                        TCFControlProvider::TDataClientStatusChange)
                                 
    SINGLE_NODEACTIVITY_ENTRY( MeshMachine::TDoNothing,
                               S60MCprStates::TAwaitingDataClientStatusChange ) 
          
    NODEACTIVITY_END()
    } // S60MCprDataClientStatusChangeConsumeActivity
                             


// -----------------------------------------------------------------------------
// S60MCprActivities - activitymap
// -----------------------------------------------------------------------------
//
namespace S60MCprActivities
    {
    DEFINE_ACTIVITY_MAP(S60MCprActivityMap)
    ACTIVITY_MAP_ENTRY(S60MCprDataClientStatusChangeConsumeActivity, MCprDataClientStatusChangeConsumeActivity)            
    ACTIVITY_MAP_ENTRY(S60MCprMonitorDataClientStatusActivity, MCprDataClientStatusChange)
	ACTIVITY_MAP_ENTRY(S60MCprSelectActivity, S60MCprSelect) // in s60mcprsimpleselectactivity.cpp
    ACTIVITY_MAP_ENTRY(S60MCprSimpleSelectActivity, MCprSimpleSelect) // in s60mcprsimpleselectactivity.cpp
    ACTIVITY_MAP_ENTRY(S60MCprErrorRecoveryActivity, MCprConnectionStartRecovery) // in s60mcprerrorrecoveryactivity.cpp
    ACTIVITY_MAP_ENTRY(S60MCprMobilityActivity, MCprMobility) // in s60mcprmobilityactivity.cpp
    ACTIVITY_MAP_ENTRY(S60MCprConnectionGoneDownRecoveryActivity, MCprConnectionGoneDownRecovery)
    ACTIVITY_MAP_ENTRY(S60MCprServiceIdRMessage2HandlerActivity, S60MCprServiceIdLegacyRMessage2Handler)
    ACTIVITY_MAP_ENTRY(S60McprNoBearerDuringGoneDownErrorRecovery, S60MCprNoBearerDuringGoneDown)
    ACTIVITY_MAP_ENTRY(S60MCprStopIAPActivity, MCprStopIAPActivity)
    ACTIVITY_MAP_ENTRY(S60MCprDataClientIdleActivity, MCprDataClientIdleActivity)
    ACTIVITY_MAP_END_BASE(MobilityMCprActivities, mobilityMCprActivities)
    }

//  End of File
