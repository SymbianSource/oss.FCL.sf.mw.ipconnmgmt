/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CAOServer class
*
*/


// INCLUDE FILES
#include <mmtsy_names.h>
#include <cmgenconnsettings.h>

#include "caoserver.h"
#include "caoconnectionmanager.h"
#include "caotimer.h"
#include "caoraumanager.h"
#include "caosettings.h"
#include "caogpds.h"
#include "taostateinit.h"
#include "taostatestarting.h"
#include "taostatedisabled.h"
#include "taostateconnecting.h"
#include "taostateconnected.h"
#include "taostatedisconnected.h"
#include "taostatewaiting.h"
#include "taostateunconnected.h"
#include "taostateresetfactorysettings.h"
#include "taostateexternalconnection.h"
#include "logger.h"
#include "pdpcontextmanagerpskeys.h"

#ifdef LOGGING_ENABLED
#ifdef LOGGING_MODE_FILE
#include <f32file.h>
#endif // LOGGING_MODE_FILE
#endif // LOGGING_ENABLED

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
#ifdef LOGGING_ENABLED

    _LIT( KStateNone,           "None" );
    _LIT( KStateInit,           "Init" );
    _LIT( KStateStarting,       "Starting" );
    _LIT( KStateDisabled,       "Disabled" );
    _LIT( KStateConnecting,     "Connecting" );
    _LIT( KStateConnected,      "Connected" );
    _LIT( KStateDisconnected,   "Disconnected" );
    _LIT( KStateWaiting,        "Waiting" );
    _LIT( KStateUnconnected,    "Unconnected" );
    _LIT( KStateRfs,            "ResetFactorySettings" );
    _LIT( KStateExtConn,        "ExternalConnection" );
    _LIT( KStateUnexpected,     "Unexpected" );

#endif // LOGGING_ENABLED

    const TInt KReactivationDelay = 25000000; // 25 seconds
    const TInt KSetupDelay = 2000000; // 2 seconds
    _LIT_SECURITY_POLICY_PASS( KPDPAlwaysPass );

#ifdef _DEBUG
    _LIT( KPanicCat, "AOServ" );
    
    // DATA TYPES
    enum TPanicReason
        {
        ENoReason,
        EPDPAlreadySubscribed,
        ESetupAlreadyInitiated
        };

    // LOCAL FUNCTIONS
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
#endif // _DEBUG
    
    
#ifdef LOGGING_ENABLED
    LOCAL_C const TDesC& StateToDesC( TAOState::TAOStateName aState )
        {
        switch ( aState )
            {
            case TAOState::EStateInit:
                {
                return KStateInit;
                }
            case TAOState::EStateStarting:
                {
                return KStateStarting;
                }
            case TAOState::EStateDisabled:
                {
                return KStateDisabled;
                }
            case TAOState::EStateConnecting:
                {
                return KStateConnecting;
                }
            case TAOState::EStateConnected:
                {
                return KStateConnected;
                }
            case TAOState::EStateDisconnected:
                {
                return KStateDisconnected;
                }
            case TAOState::EStateWaiting:
                {
                return KStateWaiting;
                }
            case TAOState::EStateUnconnected:
                {
                return KStateUnconnected;
                }
            case TAOState::EStateResetFactorySettings:
                {
                return KStateRfs;
                }
            case TAOState::EStateExternalConnection:
                {
                return KStateExtConn;
                }
            default:
                {
                return KStateUnexpected;
                }
            }
        }
#endif // LOGGING_ENABLED
    }

// METHODS

// ---------------------------------------------------------------------------
// CAOServer::NewL
// ---------------------------------------------------------------------------
//
CAOServer* CAOServer::NewL()
    {
    LOG_1( _L("CAOServer::NewL") );
    
    CAOServer* self = new( ELeave ) CAOServer;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CAOServer::~CAOServer
// ---------------------------------------------------------------------------
//
CAOServer::~CAOServer()
    {
    LOG_1( _L("CAOServer::~CAOServer") );

    delete iAsyncSetup;
    ReleasePDPProperty();    
    delete iAsyncReactivation;
    
    if ( iPointerStatePool )
        {
        iPointerStatePool->ResetAndDestroy();
        delete iPointerStatePool;    
        }
    
    delete iCenRepObserver;
    delete iGpds;
    delete iRAUManager;
    delete iTimer;
    delete iConnectionManager;
    delete iSettings;
    
    iCustomAPI.Close();
    iMobilePhone.Close();
    iTelServer.UnloadPhoneModule( KMmTsyModuleName() );
    iTelServer.Close();
    }

// ---------------------------------------------------------------------------
// CAOServer::CAOServer
// ---------------------------------------------------------------------------
//
CAOServer::CAOServer()
    {
    LOG_1( _L("CAOServer::CAOServer") );
    }

// ---------------------------------------------------------------------------
// CAOServer::ConstructL
// ---------------------------------------------------------------------------
//
void CAOServer::ConstructL()
    {
    LOG_1( _L("CAOServer::ConstructL") );
    
    // Configure Custom API
    ConfigureCustomAPIL();
    
    // Create services
    iSettings = CAOSettings::NewL( *this, *this );
    iConnectionManager = CAOConnectionManager::NewL( *this, *iSettings );
    iTimer = CAOTimer::NewL( *this, *iSettings );

#ifdef __ALWAYS_ON_CUSTOM_API

    iRAUManager = CAORAUManager::NewL( *this, iCustomAPI );
    
#else

    iRAUManager = CAORAUManager::NewL( *this );

#endif // __ALWAYS_ON_CUSTOM_API
    
    iGpds = CAOGpds::NewL( iCustomAPI );
    	
    iCenRepObserver = CAOCenRepObserver::NewL( *this );	
    
    InitStatePoolL();
        
    // Set current state to 'init'
    iCurrentState = ChangeState( TAOState::EStateInit );
    
    // Init timers
    iAsyncReactivation = CPeriodic::NewL( CActive::EPriorityStandard );
    iAsyncSetup = CPeriodic::NewL( CActive::EPriorityStandard );
    
    // Create PDP property observer
    CreatePDPPropertyAndSubscribeL();
    
    // Setup Always-On server
    Setup();
    }

// ---------------------------------------------------------------------------
// CAOServer::Setup
// ---------------------------------------------------------------------------
//
void CAOServer::Setup()
    {
    LOG_1( _L("CAOServer::SetupL") );
    
    __ASSERT_DEBUG( !iAsyncSetup->IsActive(), 
                    Panic( ESetupAlreadyInitiated ) );
    
    TCallBack cb( SetupCallBack, this );
    iAsyncSetup->Start( KSetupDelay, KSetupDelay, cb );
    }

// ---------------------------------------------------------------------------
// CAOServer::ReactivationCallBack
// ---------------------------------------------------------------------------
//
TInt CAOServer::ReactivationCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOServer::ReactivationCallBack") );
    
    TRAP_IGNORE( static_cast<CAOServer*>( aSelf )->HandleReactivationL() );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleReactivationL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleReactivationL()
    {
    LOG_1( _L("CAOServer::HandleReactivationL") );
    
    iAsyncReactivation->Cancel();
    
    TAOState* newState =
        iCurrentState->HandleSwitchFromDisconnectedL( iFailure );
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::DoReactivation
// ---------------------------------------------------------------------------
//
void CAOServer::DoReactivation( MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("CAOServer::DoReactivation") );
    
    // If we are not already re-activating
    if ( !iAsyncReactivation->IsActive() )
        {
        iFailure = aReason;
        TCallBack cb( ReactivationCallBack, this );
        iAsyncReactivation->Start(
            KReactivationDelay, KReactivationDelay, cb );
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::IssuePDPPropertySubscription
// ---------------------------------------------------------------------------
//
void CAOServer::IssuePDPPropertySubscription()
    {
    LOG_1( _L("CAOServer::IssuePDPPropertySubscription") );
    
    __ASSERT_DEBUG( !iPDPPropertySubscriber->IsActive(),
        Panic( EPDPAlreadySubscribed ) );
    
    iPDPPropertySubscriber->IssueRequest();
    }
// ---------------------------------------------------------------------------
// CAOServer::PDPPropertySubscriptionIssueRequest
// ---------------------------------------------------------------------------
//
void CAOServer::PDPPropertySubscriptionIssueRequest( TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOServer::PDPPropertySubscriptionIssueRequest") );
    
    iPDPProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOServer::PDPPropertySubscriptionRunL
// ---------------------------------------------------------------------------
//
void CAOServer::PDPPropertySubscriptionRunL( TInt aStatus )
    {
    LOG_2( _L("CAOServer::PDPPropertySubscriptionRunL: aStatus: %d"),
        aStatus );
    
    if ( aStatus == KErrNone )
        {
        TInt val = 0;
        TInt err = iPDPProperty.Get(
            KPDPContextManager2,
            KPDPContextManagerFactorySettingsReset,
            val );
            
        TAOState* newState = NULL;
        if ( err == KErrNone )
            {
            if ( val == EPDPContextManagerFactorySettingsResetStart )
                {
                newState =
                    iCurrentState->HandleResetFactorySettingsL( ETrue );
                }
            else if ( val == EPDPContextManagerFactorySettingsResetStop )
                {
                newState =
                    iCurrentState->HandleResetFactorySettingsL( EFalse );
                }
            }
        if ( newState )
            {
            iCurrentState = newState;
            }
        }
    IssuePDPPropertySubscription();
    }

// ---------------------------------------------------------------------------
// CAOServer::PDPPropertySubscriptionDoCancel
// ---------------------------------------------------------------------------
//
void CAOServer::PDPPropertySubscriptionDoCancel()
    {
    LOG_1( _L("CAOServer::PDPPropertySubscriptionDoCancel") );
    
    iPDPProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOServer::ReleasePDPProperty
// ---------------------------------------------------------------------------
//
void CAOServer::ReleasePDPProperty()
    {
    LOG_1( _L("CAOServer::ReleasePDPProperty") );
    
    delete iPDPPropertySubscriber;
    iPDPProperty.Delete(
        KPDPContextManager2,
        KPDPContextManagerFactorySettingsReset );
    iPDPProperty.Close();
    }

// ---------------------------------------------------------------------------
// CAOServer::CreatePDPPropertyAndSubscribeL
// ---------------------------------------------------------------------------
//
void CAOServer::CreatePDPPropertyAndSubscribeL()
    {
    LOG_1( _L("CAOServer::CreatePDPPropertyAndSubscribeL") );

    // Define PDP property
    TInt err = iPDPProperty.Define(
        KPDPContextManager2,
        KPDPContextManagerFactorySettingsReset,
        RProperty::EInt,
        KPDPAlwaysPass,
        KPDPAlwaysPass,
        0 );
    LOG_2( _L("> Property define: %d"), err );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    
    // Attach to property
    err = iPDPProperty.Attach(
        KPDPContextManager2,
        KPDPContextManagerFactorySettingsReset );
    LOG_2( _L("> Property attach: %d"), err );
    User::LeaveIfError( err );
        
    // Create property subscriber
    iPDPPropertySubscriber = CAOAsyncWrapper<CAOServer>::NewL(
        this,
        &CAOServer::PDPPropertySubscriptionIssueRequest,
        &CAOServer::PDPPropertySubscriptionRunL,
        &CAOServer::PDPPropertySubscriptionDoCancel,
        NULL );
        
    // Subscribe
    IssuePDPPropertySubscription();
    }

// ---------------------------------------------------------------------------
// CAOServer::SetupCallBack
// ---------------------------------------------------------------------------
//
TInt CAOServer::SetupCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOServer::SetupCallBack") );
    
    CAOServer* server = static_cast<CAOServer*>( aSelf );
    TRAPD( err, server->HandleSetup() );
    return err;
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleSetupL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleSetup()
    {
    LOG_1( _L("CAOServer::HandleSetup") );
    
    iAsyncSetup->Cancel();
    
    // Fetch settings and network registration status
    iSettings->FetchSettings();
    iConnectionManager->FetchNetworkRegistrationStatus();
    }

// ---------------------------------------------------------------------------
// CAOServer::ConfigureCustomAPIL
// ---------------------------------------------------------------------------
//
void CAOServer::ConfigureCustomAPIL()
    {
    LOG_1( _L("CAOServer::ConfigureCustomAPIL") );
    
    // Connect tel server
    // If not possible to load module and if it not already loaded
    // no way to recover.
    User::LeaveIfError( iTelServer.Connect() );
    TInt err = iTelServer.LoadPhoneModule( KMmTsyModuleName() );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( KErrGeneral );
        }

    // Open mobile phone and custom API
    User::LeaveIfError( iMobilePhone.Open( iTelServer, KMmTsyPhoneName() ) );
    User::LeaveIfError( iCustomAPI.Open( iMobilePhone ) );
    }

// ---------------------------------------------------------------------------
// CAOServer::ConnectionManager
// ---------------------------------------------------------------------------
//
MAOConnectionManager& CAOServer::ConnectionManager() const
    {
    LOG_1( _L("CAOServer::ConnectionManager") );
    
    return *iConnectionManager;
    }

// ---------------------------------------------------------------------------
// CAOServer::Timer
// ---------------------------------------------------------------------------
//
MAOTimer& CAOServer::Timer() const
    {
    LOG_1( _L("CAOServer::Timer") );
    
    return *iTimer;
    }

// ---------------------------------------------------------------------------
// CAOServer::RAUManager
// ---------------------------------------------------------------------------
//
MAORAUManager& CAOServer::RAUManager() const
    {
    LOG_1( _L("CAOServer::RAUManager") );
    
    return *iRAUManager;
    }

// ---------------------------------------------------------------------------
// CAOServer::Settings
// ---------------------------------------------------------------------------
//
MAOSettings& CAOServer::Settings() const
    {
    LOG_1( _L("CAOServer::Settings") );
    
    return *iSettings;
    }

// ---------------------------------------------------------------------------
// CAOServer::Gpds
// ---------------------------------------------------------------------------
//
MAOGpds& CAOServer::Gpds() const
    {
    LOG_1( _L("CAOServer::Gpds") );
    
    return *iGpds;
    }

// ---------------------------------------------------------------------------
// CAOServer::ChangeState
// ---------------------------------------------------------------------------
//
TAOState* CAOServer::ChangeState( TAOState::TAOStateName aState )
    {
    if ( !iCurrentState )
        {
        LOG_3( _L(
            ">>>> CAOServer::ChangeState: Old state %S > New state %S <<<<" ),
            &KStateNone, &StateToDesC( aState ) );
        }
    else
        {
        LOG_3( _L(
            ">>>> CAOServer::ChangeState: Old state %S > New state %S <<<<" ),
            &StateToDesC( CurrentState()->StateName() ),
            &StateToDesC( aState ) );
        }
    
    return (*iPointerStatePool)[aState];
    }

// ---------------------------------------------------------------------------
// CAOServer::CurrentState
// ---------------------------------------------------------------------------
//
TAOState* CAOServer::CurrentState() const
    {
    LOG_1( _L("CAOServer::CurrentState") );
    
    if( !iCurrentState )
        {
        LOG_1( _L("CAOServer::CurrentState is NULL !") );
        }
    
    return iCurrentState;
    }

// ---------------------------------------------------------------------------
// CAOServer::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandlePDPContextActivatedL()
    {
    LOG_1( _L("CAOServer::HandlePDPContextActivatedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandlePDPContextActivatedL();
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandlePDPContextActivationFailedL(
    MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("CAOServer::HandlePDPContextActivationFailedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
        
    iActivationFailure = ETrue;
    
    TAOState* newState = iCurrentState->HandlePDPContextActivationFailedL();
    if ( newState )
        {
        iCurrentState = newState;
        
        // If new state is disconnected we need synchronously change to new
        // state depending from the failure reason
        if ( CurrentState()->StateName() == TAOState::EStateDisconnected )
            {
            newState = iCurrentState->HandleSwitchFromDisconnectedL( 
                                                                aReason );
            }
            
        // Change state if new state received
        if ( newState )
            {
            iCurrentState = newState;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandlePDPContextDisconnectedL(
    MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("CAOServer::HandlePDPContextDisconnectedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState =
        iCurrentState->HandlePDPContextDisconnectedL( aReason );
    if ( newState )
        {
        iCurrentState = newState;
        
        // If new state is disconnected we need synchronously change to new
        // state depending from the failure reason
        if ( CurrentState()->StateName() == TAOState::EStateDisconnected )
            {
            // Do asnyc re-activation to ensure old connection
            // has really been closed
            DoReactivation( aReason );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandlePDPContextTemporarilyBlockedL()
    {
    LOG_1( _L("CAOServer::HandlePDPContextTemporarilyBlockedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandlePDPContextTemporarilyBlockedL();
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleNWRegistrationStatusChangedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleNWRegistrationStatusChangedL(
    CTelephony::TRegistrationStatus /*aNetworkRegistration*/ )
    {
    LOG_1( _L("CAOServer::HandleNWRegistrationStatusChangedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    // Get current network type from connection manager
    MAOConnectionManager::TNetworkType nwType =
        iConnectionManager->NetworkType();
        
    TAOState* newState = iCurrentState->HandleNetworkChangedL( nwType );
    if ( newState )
        {
        iCurrentState = newState;
        
        // New state is 'starting'
        if ( CurrentState()->StateName() == TAOState::EStateStarting )
            {
            newState = iCurrentState->HandleEnableAlwaysOnL();
            }
        // New state is 'disconnected'
        else if ( CurrentState()->StateName() == TAOState::EStateDisconnected )
            {
            newState = iCurrentState->HandleSwitchFromDisconnectedL(
                MAOConnectionManager::ETemporary );
            }
            
        // Change state if new state received
        if ( newState )
            {
            iCurrentState = newState;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleConnectionDeletedL( const TUint /*aConnectionId*/,
                                          const TBool /*aForward*/ )
    {
    LOG_1( _L("CAOServer::HandleConnectionDeletedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
        
    // If we currently tried to activate PDP context and it has failed
    // due temporary or permanent reason do not do anything with
    // connection deleted notification since notification concerns
    // the same connection that just failed to activate
    if ( iActivationFailure )
        {
        LOG_1( _L("> Activation failure -> ignore") );
        iActivationFailure = EFalse;
        }
    // If we have not already received disconnect message
    else if ( !iAsyncReactivation->IsActive() )
        {
        LOG_1( _L("> Not an activation failure -> process") );
        TAOState* newState = iCurrentState->HandleConnectionDeletedL();
        if ( newState )
            {
            iCurrentState = newState;
            
            // If new state is disconnected
            if ( CurrentState()->StateName() == TAOState::EStateDisconnected )
                {
                // Do async re-activation
                DoReactivation( MAOConnectionManager::EDisconnected );
                }
            }
        }
    // Just to log
    else
        {
        LOG_1( _L("> Already re-activating -> ignore") );
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleExternalConnectionCreatedL()
    {
    LOG_1( _L("CAOServer::HandleExternalConnectionCreatedL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
        
    // End possible re-activation
    iAsyncReactivation->Cancel();
    TAOState* newState = iCurrentState->HandleExternalConnectionCreatedL();
    if( newState )
        {
        iCurrentState = newState;
        }
    }
    
// ---------------------------------------------------------------------------
// CAOServer::HandleInternalConnectionCreatedL
// ---------------------------------------------------------------------------
//    
void CAOServer::HandleInternalConnectionCreatedL( const TUint /*aConnectionId*/,
                                                  const TUint /*aIapId*/, 
                                                  const TUint /*aNetId*/ )
    {
    }    

// ---------------------------------------------------------------------------
// CAOServer::HandleError
// ---------------------------------------------------------------------------
//
void CAOServer::HandleError( TInt /*aError*/ )
    {
    LOG_1( _L("CAOServer::HandleError") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandleGeneralError();
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleRetryTimerExpiredL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleRetryTimerExpiredL()
    {
    LOG_1( _L("CAOServer::HandleRetryTimerExpiredL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandleRetryTimerTriggeredL();
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleConnectionTimerExpiredL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleConnectionTimerExpiredL()
    {
    LOG_1( _L("CAOServer::HandleConnectionTimerExpiredL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandleConnectionTimerTriggeredL();
    if ( newState )
        {
        iCurrentState = newState;
        
        // Connection timer has trigered maximum amount -> start retry timer
        if ( iCurrentState->StateName() == TAOState::EStateDisconnected )
            {
            newState = iCurrentState->HandleSwitchFromDisconnectedL(
                MAOConnectionManager::ETemporary );
            }
            
        // Change state if new state received
        if ( newState )
            {
            iCurrentState = newState;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleUnconnectTimerExpiredL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleUnconnectTimerExpiredL()
    {
    LOG_1( _L("CAOServer::HandleUnconnectTimerExpiredL") );
    
    TAOState* newState = iCurrentState->HandleUnconnectTimerExpiredL();
    if( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleSettingsChangedL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleSettingsChangedL()
    {
    LOG_1( _L("CAOServer::HandleSettingsChangedL") );

    // Update retry timer value
    iTimer->HandleSettingsChangedL();
    
    // Handle notification
    TAOState* newState = iCurrentState->HandleSettingsReceivedL();

    if ( newState )
        {

        LOG_2( _L("> newState1: %d"), newState->StateName() );
    
        iCurrentState = newState;
        
        if ( iCurrentState->StateName() == TAOState::EStateStarting )
            {
            // If new state is starting check if Always On should be enabled
            // in current network
            newState = iCurrentState->HandleEnableAlwaysOnL();
            }
            
        // Change state if new state received
        if ( newState )
            {
            LOG_2( _L("> newState2: %d"), newState->StateName() );
            iCurrentState = newState;
            }
        else
            {
            LOG_1( _L("> New state is null2.") );
            }
        }
     else
        {
        LOG_1( _L("> New state is null1.") );
        }
       
    // Update linger objects    
    iConnectionManager->HandleSettingsChangedL();    
    }

// ---------------------------------------------------------------------------
// CAOServer::HandleSuccesfulRAUL
// ---------------------------------------------------------------------------
//
void CAOServer::HandleSuccesfulRAUL( TRAUType /*aType*/ )
    {
    LOG_1( _L("CAOServer::HandleSuccesfulRAUL") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    TAOState* newState = iCurrentState->HandleSuccesfulRAUEventL();
    if ( newState )
        {
        iCurrentState = newState;
        }
    }

// ---------------------------------------------------------------------------
// CAOServer::CurrentCellularDataUsageChanged
// ---------------------------------------------------------------------------
//
void CAOServer::CurrentCellularDataUsageChangedL( const TInt aValue )
	  {
    LOG_1( _L("CAOServer::CurrentCellularDataUsageChanged") );
    LOG_2( _L("> Current state: %S"),
        &StateToDesC( CurrentState()->StateName() ) );
    
    if ( aValue != ECmCellularDataUsageDisabled )
        {
        TAOState* newState = NULL;
        	
        if ( CurrentState()->StateName() == TAOState::EStateDisabled )
            {
            newState = iCurrentState->HandleEnableAlwaysOnL();
            }
        else
            {
            // Behaviour is the same as if unconnect timer had expired
            iTimer->StopUnconnectTimer();
            newState = iCurrentState->HandleUnconnectTimerExpiredL();

            if( newState )
                {
                iCurrentState = newState;
                }
            }
        }  	
    }
	
// ---------------------------------------------------------------------------
// CAOServer::InitStatePoolL
// ---------------------------------------------------------------------------
//
void CAOServer::InitStatePoolL()
    {
    LOG_1( _L("CAOServer::InitStatePoolL") );

    iPointerStatePool = new (ELeave) CArrayPtrFlat<TAOState>( TAOState::EStateLastItem );
    
    
    TAOStateInit *stateInit = new (ELeave) TAOStateInit( *this, *this );
    CleanupStack::PushL( stateInit );
    iPointerStatePool->InsertL( TAOState::EStateInit, stateInit );
    CleanupStack::Pop( stateInit );

    TAOStateStarting *stateStarting = new (ELeave) TAOStateStarting( *this, *this );
    CleanupStack::PushL( stateStarting );
    iPointerStatePool->InsertL( TAOState::EStateStarting, stateStarting );
    CleanupStack::Pop( stateStarting );

    TAOStateDisabled *stateDisabled = new (ELeave) TAOStateDisabled( *this, *this );
    CleanupStack::PushL( stateDisabled );
    iPointerStatePool->InsertL( TAOState::EStateDisabled, stateDisabled );
    CleanupStack::Pop( stateDisabled );
    
    TAOStateConnecting *stateConnecting = new (ELeave) TAOStateConnecting( *this, *this );
    CleanupStack::PushL( stateConnecting );
    iPointerStatePool->InsertL( TAOState::EStateConnecting, stateConnecting );
    CleanupStack::Pop( stateConnecting );
    
    TAOState *stateConnected = new (ELeave) TAOStateConnected( *this, *this );
    CleanupStack::PushL( stateConnected );
    iPointerStatePool->InsertL( TAOState::EStateConnected, stateConnected );
    CleanupStack::Pop( stateConnected );
    
    TAOState *stateDisconnected = new (ELeave) TAOStateDisconnected( *this, *this );
    CleanupStack::PushL( stateDisconnected );
    iPointerStatePool->InsertL( TAOState::EStateDisconnected, stateDisconnected );
    CleanupStack::Pop( stateDisconnected );
    
    TAOState *stateWaiting = new (ELeave) TAOStateWaiting( *this, *this );
    CleanupStack::PushL( stateWaiting );
    iPointerStatePool->InsertL( TAOState::EStateWaiting, stateWaiting );
    CleanupStack::Pop( stateWaiting );
    
    TAOState *stateUnconnected = new (ELeave) TAOStateUnconnected( *this, *this );
    CleanupStack::PushL( stateUnconnected );
    iPointerStatePool->InsertL( TAOState::EStateUnconnected, stateUnconnected );
    CleanupStack::Pop( stateUnconnected );
    
    TAOState *stateResetFactorySettings = new (ELeave) TAOStateResetFactorySettings( *this, *this );
    CleanupStack::PushL( stateResetFactorySettings );
    iPointerStatePool->InsertL( TAOState::EStateResetFactorySettings, stateResetFactorySettings );
    CleanupStack::Pop( stateResetFactorySettings );
    
    TAOState *stateExternalConnection = new (ELeave) TAOStateExternalConnection( *this, *this );
    CleanupStack::PushL( stateExternalConnection );
    iPointerStatePool->InsertL( TAOState::EStateExternalConnection, stateExternalConnection );
    CleanupStack::Pop( stateExternalConnection );
    
    }


// End of file
