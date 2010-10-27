/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connection Monitor server.
*
*/

#include <featmgr.h>
#include <rconnmon.h>

#include "ConnMonServ.h"
#include "ConnMonDef.h"
#include "ConnMonIAP.h"
#include "ConnMonSess.h"
#include "CEventQueue.h"
#include "log.h"

#include "ConnMonAvailabilityManager.h"
#include "connmoncommsdatcache.h"
#include "ConnMonBearerCSD.h"
#include "ConnMonBearerGPRS.h"
#include "ConnMonBearerLAN.h"
#include "ConnMonBearerWLAN.h"
#include "ConnMonBearerGroupManager.h"
#include "connmoncommsdatnotifier.h"
#include "cellulardatausagekeyupdater.h"
#include "connmondialupoverridenotifier.h"

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PanicServer
// Panics the server in case of programming error.
// -----------------------------------------------------------------------------
//
void PanicServer( TInt aPanic )
    {
    _LIT( KPanicCategory, "ConnectionMonitor Server" );
    User::Panic( KPanicCategory, aPanic );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonScheduler::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonScheduler::ConstructL()
    {
    // Construct active scheduler
    CConnMonScheduler* self = new( ELeave ) CConnMonScheduler;
    CActiveScheduler::Install( self );

    // Construct server
    self->iServer = new( ELeave ) CConnMonServer;
    self->iServer->ConstructL();
    }

// -----------------------------------------------------------------------------
// CConnMonScheduler::~CConnMonScheduler
// -----------------------------------------------------------------------------
//
CConnMonScheduler::~CConnMonScheduler()
    {
    delete iServer;
    }

// -----------------------------------------------------------------------------
// CConnMonScheduler::LaunchFromClient
// -----------------------------------------------------------------------------
//
TInt CConnMonScheduler::LaunchFromClient()
    {
    LOGENTRFN("CConnMonScheduler::LaunchFromClient()")
    TInt err( KErrNone );

    // Set up waiting apparatus
    TRequestStatus status;
    TSignal signal( status );

    // Create semaphore to allow to signal when server has started
    RSemaphore globStartSignal;

    LOGTIMINGSTART("CConnMonScheduler::LaunchFromClient()")

    err = globStartSignal.CreateGlobal( KConnMonStartupSemaphore, 0 );
    if ( err != KErrNone )
        {
        // If semaphore already exists, open it and wait
        err = globStartSignal.OpenGlobal( KConnMonStartupSemaphore, EOwnerProcess );
        if ( KErrNone == err )
            {
            LOGIT("LaunchFromClient: semaphore in use, waiting release...")
            globStartSignal.Wait();
            globStartSignal.Signal();
            globStartSignal.Close();
            LOGTIMINGEND("CConnMonScheduler::LaunchFromClient() (waiting)")
            LOGIT("LaunchFromClient: semaphore released")
            }
        else
            {
            LOGIT1("LaunchFromClient: semaphore in use, but open failed <%d>", err)
            }
        LOGEXITFN1("CConnMonScheduler::LaunchFromClient()", err)
        return err;
        }

    RProcess server;
    err = server.Create(
            KConnectionMonitorServerExe,
            signal.Get(),
            TUidType( KNullUid, KNullUid, KConnMonServerUid ),
            EOwnerThread );
    if ( err != KErrNone )
        {
        LOGIT1("LaunchFromClient: ERROR, server creation failed <%d>", err)
        LOGEXITFN1("CConnMonScheduler::LaunchFromClient()", err)
        return err;
        }

    server.Rendezvous( status );
    if ( status != KRequestPending )
        {
        server.Kill( 0 ); // Abort startup
        }
    else
        {
        server.Resume(); // Logon OK - start the server
        }

    // Wait for launch to complete
    User::WaitForRequest( status );

    // We can't use the 'exit reason' if the server panicked as this is the
    // panic 'reason' and may be '0' which cannot be distinguished from KErrNone
    if ( server.ExitType() == EExitPanic )
        {
        LOGIT2("LaunchFromClient: ERROR, server start PANIC, reason: %d, status: %d", server.ExitReason(), status.Int())
        err = KErrGeneral;
        }
    else
        {
        err = status.Int();
        }

    server.Close();

    // Signal other clients that server process has started
    globStartSignal.Signal();

    // Delete semaphore
    globStartSignal.Close();

    LOGTIMINGEND("CConnMonScheduler::LaunchFromClient() (real)")
    LOGEXITFN1("CConnMonScheduler::LaunchFromClient()", err)
    return err;
    }


// -----------------------------------------------------------------------------
// CConnMonScheduler::ThreadStart
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonScheduler::ThreadStart( TSignal& /*aSignal*/ )
    {
    __UHEAP_MARK;
    LOGENTRFN("CConnMonScheduler::ThreadStart()")
    TInt err( KErrNone );

    // Get cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    // Initialize all up to and excluding starting scheduler
    if ( cleanup )
        {
        TRAP( err, ConstructL() );
        }
    else
        {
        err = KErrNoMemory;
        }

    // Tell starting thread we've started and start the active scheduler
    // Scheduler will "sit" here while the server is running because
    // Start() will return only when server calls CActiveScheduler::Stop().
    if ( KErrNone == err )
        {
        RProcess::Rendezvous( KErrNone );
        CActiveScheduler::Start();
        }

    // Close things down because server has died
    delete CActiveScheduler::Current();
    delete cleanup;

    __UHEAP_MARKEND;
    LOGEXITFN1("CConnMonScheduler::ThreadStart()", err)
    return err;
    }

// -----------------------------------------------------------------------------
// CConnMonScheduler::Error
// -----------------------------------------------------------------------------
//
void CConnMonScheduler::Error( TInt aError ) const
    {
    // Panic if error didn't arise from server
    if ( iServer->IsActive() )
        {
        PanicServer( EErrorFromNonClientObject );
        }

    // If it's a bad descriptor, panic the client
    if ( aError == KErrBadDescriptor )
        {
        iServer->PanicClient( EBadDescriptor );
        }

    iServer->Error( aError );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonServer::CConnMonServer
// -----------------------------------------------------------------------------
//
CConnMonServer::CConnMonServer()
        :
        CPolicyServer( CActive::EPriorityStandard, KConnMonPolicy ),
        iIap( NULL ),
        iEventQueue( NULL ),
        iSessionCount( 0 ),
        iShutdown( NULL ),
        iContainerIndex( NULL ),
        iCommsDatCache( NULL ),
        iAvailabilityManager( NULL ),
        iIapTableNotifier( NULL ),
        iSnapTableNotifier( NULL ),
        iVirtualTableNotifier( NULL ),
        iBearerGroupManager( NULL ),
        iCellularDataUsageKeyUpdater( NULL ),
        iDialUpOverrideNotifier( NULL ),
        iDialUpOverrideTimer( NULL ),
        iDialUpOverrideStatus( EConnMonDialUpOverrideDisabled )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonServer::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonServer::ConstructL()
    {
    LOGENTRFN("CConnMonServer::ConstructL()")
    LOGIT("ConstructL: Server trying to start...")

    // Naming the server thread after the server helps debugging panics.
    RThread::RenameMe( KConnectionMonitorServerName );

    // Construct IAP wrapper
    iIap = new( ELeave ) CConnMonIAP( this );
    iIap->ConstructL();

    // Construct event queue
    iEventQueue = new( ELeave ) CEventQueue;
    iEventQueue->ConstructL( this );

    // Create the object container index
    iContainerIndex = CObjectConIx::NewL();

    // Construct shutdown timer
    iShutdown = new( ELeave ) CConnMonDelayedShutdown( this );
    iShutdown->ConstructL();


    ////// Availability stuff

    // CommsDat cache
    iCommsDatCache = CConnMonCommsDatCache::NewL();

    // Add bearers to array
    User::LeaveIfError(
            iBearers.Append( new( ELeave ) TConnMonBearerCSD( iIap, iCommsDatCache ) ) );
    User::LeaveIfError(
            iBearers.Append( new( ELeave ) TConnMonBearerGPRS( iIap, iCommsDatCache ) ) );
    User::LeaveIfError(
            iBearers.Append( new( ELeave ) TConnMonBearerLAN( iIap, iCommsDatCache ) ) );

    // Initialize FeatureManager
    FeatureManager::InitializeLibL();

    #ifndef __WINSCW__  // WINS does not support WLAN
    // Check if WLAN is enabled
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        User::LeaveIfError(
                iBearers.Append( new( ELeave ) TConnMonBearerWLAN( iIap, iCommsDatCache ) ) );
        }
    #endif
    LOGIT1("ConstructL: iBearers constructed, count <%d>", iBearers.Count() )

    // Initialize the CommsDat cache. Will read the necessary CommsDat tables
    // and populate the cache tables with current information. Any changes
    // after this init phase will trigger ConnMon events when appropriate.
    iCommsDatCache->Init( this, iIap, &iBearers );

    // IAP / SNAP availability manager
    iAvailabilityManager = CConnMonAvailabilityManager::NewL(
            iCommsDatCache,
            this );

    // Central Repository notifiers
    iIapTableNotifier = CConnMonCommsDatNotifier::NewL(
            iAvailabilityManager,
            iCommsDatCache->GetIapRecordTableId() );
    iSnapTableNotifier = CConnMonCommsDatNotifier::NewL(
            iAvailabilityManager,
            iCommsDatCache->GetSnapRecordTableId() );
    iVirtualTableNotifier = CConnMonCommsDatNotifier::NewL(
            iAvailabilityManager,
            iCommsDatCache->GetVirtualRecordTableId() );
    LOGIT("ConstructL: Availability manager and CenRep notifiers constructed")

    // Bearer group manager
    iBearerGroupManager = CConnMonBearerGroupManager::NewL();
    LOGIT("ConstructL: Bearer group manager constructed")
    
    iCellularDataUsageKeyUpdater = CCellularDataUsageKeyUpdater::NewL( this );
    LOGIT("ConstructL: CCellularDataUsageKeyUpdater constructed")

    // Add P&S listener for dial-up PDP context override, if feature enabled.
    if ( iCellularDataUsageKeyUpdater->DialUpOverrideEnabled() )
        {
        iDialUpOverrideNotifier = CConnMonDialUpOverrideNotifier::NewL( this );
        iDialUpOverrideStatus = EConnMonDialUpOverrideInactive;
        LOGIT("ConstructL: Dial-up override notifier constructed")

        // Construct dial-up PDP context override timer.
        iDialUpOverrideTimer = CConnMonDialUpOverrideTimer::NewL( this );
        }

    // Identify ourselves and open for service
    StartL( KConnectionMonitorServerName );

    //Start Indicator observer process
    _LIT(KIndicatorObserverExeFile,"indicatorobserver.exe");
    _LIT(KIndicatorObserverExeFileCmd,"");
    RProcess proc;
    User::LeaveIfError(proc.Create(KIndicatorObserverExeFile,KIndicatorObserverExeFileCmd));
    proc.Resume();
    proc.Close(); // Closes the handle, not the process.

    LOGIT("ConstructL: Server started successfully")
    LOGEXITFN("CConnMonServer::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonServer::~CConnMonServer
// -----------------------------------------------------------------------------
//
CConnMonServer::~CConnMonServer()
    {
    LOGIT("Server: Running destructor")

    delete iIap;
    delete iEventQueue;
    delete iContainerIndex;
    delete iShutdown;

    // Bearers to array
    iBearers.ResetAndDestroy();
    iBearers.Close();

    // Availability manager
    delete iAvailabilityManager;

    // CenRep notification listeners
    if ( iIapTableNotifier )
        {
        iIapTableNotifier->Cancel();
        }
    if ( iSnapTableNotifier )
        {
        iSnapTableNotifier->Cancel();
        }
    if ( iVirtualTableNotifier )
        {
        iVirtualTableNotifier->Cancel();
        }
    delete iIapTableNotifier;
    delete iSnapTableNotifier;
    delete iVirtualTableNotifier;

    // CommsDat cache
    delete iCommsDatCache;

    // Bearer Group Manager
    delete iBearerGroupManager;

    delete iCellularDataUsageKeyUpdater;

    // Dial-up PDP context override timer.
    delete iDialUpOverrideTimer;

    // P&S listener for dial-up PDP context override.
    delete iDialUpOverrideNotifier;

    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CConnMonServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CConnMonServer::NewSessionL(
        const TVersion& /*aVersion*/,
        const RMessage2& /*aMessage*/ ) const
    {
    CConnMonSession* session = new( ELeave ) CConnMonSession(
            CONST_CAST( CConnMonServer*, this ) );

    CleanupStack::PushL( session );
    session->ConstructL();
    CleanupStack::Pop( session );
    CONST_CAST( CConnMonServer*, this )->IncrementSessions();
    return session;
    }

// -----------------------------------------------------------------------------
// CConnMonServer::IncrementSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::IncrementSessions()
    {
    iSessionCount++;
    iShutdown->Cancel();
    }

// -----------------------------------------------------------------------------
// CConnMonServer::DecrementSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::DecrementSessions()
    {
    iSessionCount--;

    if ( iSessionCount > 0 )
        {
        return;
        }
    if ( !iShutdown->IsActive() )
        {
        iShutdown->Start();
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::SendEventToSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::SendEventToSessions( const TEvent& aEvent, TInt& aNumOffered )
    {
    CSession2* session;

    // Iterate through sessions
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        CConnMonSession* cmsession = static_cast<CConnMonSession*>( session );
        if ( cmsession->CanReceiveEvent() )
            {
            cmsession->SendEventToClient( aEvent );
            ++aNumOffered;
            }
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::SendDataVolumesToSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::SendDataVolumesToSessions(
        const TUint aConnectionId,
        const TUint aDlData,
        const TUint aUlData,
        const TInt  aStatus )
    {
    CSession2* session;

    // Iterate through sessions
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        ( static_cast<CConnMonSession*>( session ) )->CompleteDataVolumeRequests(
                aConnectionId,
                aDlData,
                aUlData,
                aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::SendActivityToSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::SendActivityToSessions(
        const TUint aConnectionId,
        const TBool aActivity,
        const TInt  aStatus )
    {
    CSession2* session;

    // Iterate through sessions
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        ( static_cast<CConnMonSession*>( session ) )->CompleteActivityRequests(
                aConnectionId,
                aActivity,
                aStatus );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::RemoveConnSettingsFromSessions
// -----------------------------------------------------------------------------
//
void CConnMonServer::RemoveConnSettingsFromSessions( const TUint aConnectionId )
    {
    CSession2* session;

    // Iterate through sessions
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        ( static_cast<CConnMonSession*>( session ) )->RemoveConnectionParams(
                aConnectionId );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::NumberOfListeners
// -----------------------------------------------------------------------------
//
TInt CConnMonServer::NumberOfListeners()
    {
    CSession2* session;
    CSession2* original;
    TInt count( 0 );

    original = iSessionIter;

    // Iterate through sessions
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        if ( ( static_cast<CConnMonSession*>( session ) )->IsListening() )
            {
            ++count;
            }
        }

    // Reset session iterator
    if ( original != 0 )
        {
        iSessionIter.Set( *original );
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CConnMonServer::CalculateThreshold
// -----------------------------------------------------------------------------
//
void CConnMonServer::CalculateThreshold(
        const TUint aConnectionId,
        const TInt& aThresholdType,
        TUint& aThreshold )
    {
    CSession2* session;
    CSession2* original;
    TUint th( 0 );

    aThreshold = 0;
    original = iSessionIter;

    // Iterate through sessions to find the smallest threshold
    iSessionIter.SetToFirst();
    for ( session = iSessionIter++; session; session = iSessionIter++ )
        {
        if ( aThresholdType == EBearerAvailabilityThreshold ||
                aThresholdType == ESignalStrengthThreshold )
            {
            // Bearer specific thresholds
            ( static_cast<CConnMonSession*>( session ) )->GetBearerThreshold(
                    aThresholdType,
                    th );

            if ( th != 0 )
                {
                aThreshold = 1;
                }
            }
        else
            {
            // Connection Specific thresholds
            TConnSettings connSettings( 0, 0, 0 );

            ( static_cast<CConnMonSession*>( session ) )->GetConnectionSettings(
                    aConnectionId,
                    connSettings );
            if ( aThresholdType == EDownlinkThreshold )
                {
                th = connSettings.iDLDataThreshold;
                }
            else if ( aThresholdType == EUplinkThreshold )
                {
                th = connSettings.iULDataThreshold;
                }
            else if ( aThresholdType == EActivityTimeThreshold )
                {
                th = connSettings.iActivityTimeThreshold;
                }

            if ( ( th > 0 ) && ( aThreshold == 0 || th < aThreshold ) )
                {
                aThreshold = th;
                }
            }
        }

    // Reset session iterator
    if ( original != 0 )
        {
        iSessionIter.Set( *original );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::NewContainerL
// -----------------------------------------------------------------------------
//
CObjectCon* CConnMonServer::NewContainerL()
    {
    return iContainerIndex->CreateL();
    }

// -----------------------------------------------------------------------------
// CConnMonServer::RemoveContainer
// -----------------------------------------------------------------------------
//
void CConnMonServer::RemoveContainer( CObjectCon* aContainer )
    {
    iContainerIndex->Remove( aContainer );
    }

// -----------------------------------------------------------------------------
// CConnMonServer::SendRequestToPlugin
// -----------------------------------------------------------------------------
//
TInt CConnMonServer::SendRequestToPlugin(
        const TInt aType,
        const RMessage2& aMessage,
        const TBool aToAllPlugins )
    {
    TInt ret( KErrNotSupported );
    TInt err( KErrNotSupported );
    CSession2* session;

    if ( FeatureManager::FeatureSupported( KFeatureIdConnMonExtension ) )
        {
        // Iterate through sessions
        iSessionIter.SetToFirst();

        for ( session = iSessionIter++; session; session = iSessionIter++ )
            {
            err = ( static_cast<CConnMonSession*>( session ) )->SendRequestToPlugin(
                    aType,
                    aMessage );

            if ( ( err == KRequestPending ) || ( err == KErrNone ) )
                {
                // OK because a plugin has handled the request.
                if ( aToAllPlugins )
                    {
                    ret = err;
                    }
                else
                    {
                    return err;
                    }
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CConnMonServer::CancelAttributeRequestsFromPlugins
// -----------------------------------------------------------------------------
//
void CConnMonServer::CancelAttributeRequestsFromPlugins(
        const RThread& aClient,
        const TInt aType )
    {
    CSession2* session;

    if ( FeatureManager::FeatureSupported( KFeatureIdConnMonExtension ) )
        {
        // Iterate through sessions
        iSessionIter.SetToFirst();

        for ( session = iSessionIter++; session; session = iSessionIter++ )
            {
            ( static_cast<CConnMonSession*>( session ) )->CancelAttributeRequestFromPlugin(
                    aClient,
                    aType );
            }
        }
    }

// -----------------------------------------------------------------------------
// CConnMonServer::PanicClient
// -----------------------------------------------------------------------------
//
void CConnMonServer::PanicClient( TInt aPanic ) const
    {
    // Let's have a look before we panic the client
    __DEBUGGER()

    // Ok, go for it
    LOGIT("Server: PanicClient: Sending panic")
    /*_LIT( KConnectionMonitorServer, "ConnMonServ" );*/
    /*Message().Panic( KConnectionMonitorServer, aPanic );*/
    LOGIT1("Server: PanicClient: Panic removed to improve ConnMon robustness, code was <%d>", aPanic )
    aPanic = aPanic; // To remove warning
    }

// -----------------------------------------------------------------------------
// CConnMonServer::PanicClient
// -----------------------------------------------------------------------------
//
void CConnMonServer::PanicClient( const RMessage2& aMsg, const TInt aPanic ) const
    {
    // Let's have a look before we panic the client
    __DEBUGGER()

    // Ok, go for it
    LOGIT("Server: PanicClient: Sending panic")
    /*_LIT( KConnectionMonitorServer, "ConnMonServ" );*/
    /*aMsg.Panic( KConnectionMonitorServer, aPanic );*/
    LOGIT1("Server: PanicClient: Panic removed to improve ConnMon robustness, code was <%d>", aPanic )
    TInt panic = aPanic; // To remove warning
    panic = aMsg.Int0(); // To remove warning
    panic = panic; // To remove warning
    }

// -----------------------------------------------------------------------------
// CConnMonServer::Error
// -----------------------------------------------------------------------------
//
void CConnMonServer::Error( TInt aError )
    {
    if ( aError != KErrBadDescriptor )
        {
        // Anyway, complete the outstanding message
        Message().Complete( aError );
        }

    // Ready to roll again
    ReStart(); // Really means just continue reading client requests
    }

// -----------------------------------------------------------------------------
// Get available IAP IDs for the IAPs with bearer aBearerId
// -----------------------------------------------------------------------------
//
TInt CConnMonServer::GetAvailableIaps( const TUint aBearerId, TConnMonIapInfo& aIapInfo )
    {
    // Currently CommsDat is read only after a change notification is received
    // from CenRep about a relevant CommsDat table. If these events are
    // unreliable, we can increase robustness by reading CommsDat tables again
    // every time a client asks for IAP/SNAP availability information, but this
    // will slow down the requests.
    //
    // Uncomment the following TRAP to force ConnMon to read CommsDat every
    // time a client asks for IAP availability (KIapAvailability-attribute).
    //
    /*TRAPD( err, iCommsDatCache->RefreshCommsDatCacheL( 0 ) );*/
    return iCommsDatCache->GetAvailableIaps( aBearerId, aIapInfo );
    }

// -----------------------------------------------------------------------------
// Get available SANP IDs
// -----------------------------------------------------------------------------
//
void CConnMonServer::GetAvailableSnaps( TConnMonSNAPInfo& aSnapInfo )
    {
    // See comment in CConnMonServer::GetAvailableIaps() (above)
    /*TRAPD( err, iCommsDatCache->RefreshCommsDatCacheL( 0 ) );*/
    iCommsDatCache->GetAvailableSnaps( aSnapInfo );
    }

// -----------------------------------------------------------------------------
// Get available SANP IDs
// -----------------------------------------------------------------------------
//
TInt CConnMonServer::GetAvailableSnaps( RArray<TConnMonId>& aSnapIds )
    {
    // See comment in CConnMonServer::GetAvailableIaps() (above)
    /*TRAPD( err, iCommsDatCache->RefreshCommsDatCacheL( 0 ) );*/
    return iCommsDatCache->GetAvailableSnaps( aSnapIds );
    }

// -----------------------------------------------------------------------------
// Set the dial-up PDP context override feature status. Either activates or
// deactivates it. Does nothing if the feature itself has not been enabled.
// -----------------------------------------------------------------------------
//
void CConnMonServer::SetDialUpOverrideStatus( TInt aStatus )
    {
    LOGENTRFN("CConnMonServer::SetDialUpOverrideStatus()")

    // Do nothing if whole feature is disabled.
    LOGIT2("iDialUpOverrideStatus = %d, aStatus = %d", iDialUpOverrideStatus, aStatus)
    if ( iDialUpOverrideStatus == EConnMonDialUpOverrideInactive &&
            aStatus == EConnMonDialUpOverrideActive )
        {
        LOGIT("Setting dial-up override status from inactive to active")
        // Start the dial-up override timer to ensure packetdata connectivity
        // is restored to original state eventually. Normally it is restored
        // after the dial-up connection has been successfully established.
        iDialUpOverrideTimer->Start();
        // Status must be set before updating cellular data usage key.
        iDialUpOverrideStatus = aStatus;

        // Disable cellular data usage until the expected dial-up
        // connection has been established (or timeout).
        TRAP_IGNORE( iCellularDataUsageKeyUpdater->UpdateKeyL( 0 ) );

        LOGIT("SetDialUpOverrideStatus() triggered HandleAvailabilityChange()")
        AvailabilityManager()->HandleAvailabilityChange();
        }
    else if ( iDialUpOverrideStatus == EConnMonDialUpOverrideActive &&
            aStatus == EConnMonDialUpOverrideInactive )
        {
        LOGIT("Setting dial-up override status from active to inactive")
        // Stop the dial-up override timer.
        iDialUpOverrideTimer->Cancel();
        // Status must be set before updating cellular data usage key.
        iDialUpOverrideStatus = aStatus;

        TInt registration( ENetworkRegistrationExtUnknown );
        TInt err = iIap->GetNetworkRegistration_v2( registration );
        if ( err == KErrNone )
            {
            TRAP_IGNORE( iCellularDataUsageKeyUpdater->UpdateKeyL( registration ) );
            }
        else
            {
            LOGIT1("GetNetworkRegistration_v2 failed <%d>", err)
            }
        iDialUpOverrideNotifier->ResetStatus();

        LOGIT("SetDialUpOverrideStatus() triggered HandleAvailabilityChange()")
        AvailabilityManager()->HandleAvailabilityChange();
        }

    LOGEXITFN("CConnMonServer::SetDialUpOverrideStatus()")
    }

// -----------------------------------------------------------------------------
// Signals that all internal PDP connections have closed through the
// KDialUpConnection P&S-property.
// -----------------------------------------------------------------------------
//
void CConnMonServer::ConnectionsClosedForDialUpOverride()
    {
    LOGENTRFN("CConnMonServer::ConnectionsClosedForDialUpOverride()")
    if ( iDialUpOverrideStatus == EConnMonDialUpOverrideActive )
        {
        iDialUpOverrideNotifier->ResetStatus();
        }
    LOGEXITFN("CConnMonServer::ConnectionsClosedForDialUpOverride()")
    }

// -----------------------------------------------------------------------------
// CConnMonServer::CustomSecurityCheckL
// Implements custom security checking for IPCs marked with
// TSpecialCase::ECustomCheck.
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CConnMonServer::CustomSecurityCheckL(
        const RMessage2& aMsg,
        TInt& /*aAction*/,
        TSecurityInfo& /*aMissing*/ )
    {
    switch ( aMsg.Function() )
        {
        case EReqSetBoolAttribute:
        case EReqSetAsyncBoolAttribute:
            {
            switch ( aMsg.Int2() )
                {
                case KConnectionStop:
                case KConnectionStopAll:
                    {
                    // NetworkServices + NetworkControl
                    return ( KConnMonPolicyElements[4].iPolicy.CheckPolicy( aMsg ) ) ?
                        EPass : EFail;
                    }
                default:
                    return EPass;
                }
            }
        default:
            return EPass;
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonDelayedShutdown::CConnMonDelayedShutdown
// -----------------------------------------------------------------------------
//
CConnMonDelayedShutdown::CConnMonDelayedShutdown( CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonDelayedShutdown::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonDelayedShutdown::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// Destructor
CConnMonDelayedShutdown::~CConnMonDelayedShutdown()
    {
    Cancel();
    iTimer.Close();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CConnMonDelayedShutdown::Start
// -----------------------------------------------------------------------------
//
void CConnMonDelayedShutdown::Start()
    {
    if ( IsActive() )
        {
        return;
        }

    iTimer.After( iStatus, KConnMonShutdownInterval );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CConnMonDelayedShutdown::DoCancel
// -----------------------------------------------------------------------------
//
void CConnMonDelayedShutdown::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CConnMonDelayedShutdown::RunL
// -----------------------------------------------------------------------------
//
void CConnMonDelayedShutdown::RunL()
    {
    LOGIT("Server: Shutting down by timer")
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::NewL
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideTimer* CConnMonDialUpOverrideTimer::NewL(
        CConnMonServer* aServer )
    {
    CConnMonDialUpOverrideTimer* self = CConnMonDialUpOverrideTimer::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::NewLC
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideTimer* CConnMonDialUpOverrideTimer::NewLC(
        CConnMonServer* aServer )
    {
    CConnMonDialUpOverrideTimer* self = new( ELeave ) CConnMonDialUpOverrideTimer( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::~CConnMonDialUpOverrideTimer
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideTimer::~CConnMonDialUpOverrideTimer()
    {
    Cancel();
    iTimer.Close();
    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::CConnMonDialUpOverrideTimer
// -----------------------------------------------------------------------------
//
CConnMonDialUpOverrideTimer::CConnMonDialUpOverrideTimer(
        CConnMonServer* aServer )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideTimer::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::Start
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideTimer::Start()
    {
    if ( !IsActive() )
        {
        iTimer.After( iStatus, KConnMonDialUpOverrideInterval );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::DoCancel
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideTimer::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CConnMonDialUpOverrideTimer::RunL
// -----------------------------------------------------------------------------
//
void CConnMonDialUpOverrideTimer::RunL()
    {
    LOGIT(".")
    LOGIT1("RunL: CConnMonDialUpOverrideTimer <%d>", iStatus.Int())
    
    // Make sure that ETel goes to correct network status
    // in case AT+CGATT=0 has been sent from PC.
    iServer->Iap()->RestoreAttachMode();
    iServer->SetDialUpOverrideStatus( EConnMonDialUpOverrideInactive );
    }

// End of file
