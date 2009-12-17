/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data Connection Logger.
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logengdurations.h>
#include <logwraplimits.h>
#include <logengevents.h>
#endif
#include <cdbcols.h>
#include <bldvariant.hrh> // Feature IDs
#include <featmgr.h>
#include <utf.h>

#include <centralrepository.h>
#include "DclPrivateCRKeys.h"

#include "dcl.h"
#include "dcl_log.h"

// --------------------------------------------------------------------------
// CEngine::CEngine
// Constructor
// --------------------------------------------------------------------------
//
CEngine::CEngine()
        :
        CActive( EPriorityStandard ),
        iLogClient( NULL ),
        iTimerInterval( KTimerInterval )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CEngine::NewLC
// 1st phase constructor - for stack pointers to object
// --------------------------------------------------------------------------
//
CEngine* CEngine::NewLC( )
    {
    CEngine* self = new( ELeave ) CEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CEngine::NewL
// 1st phase constructor - for heap pointers to object
// --------------------------------------------------------------------------
//
CEngine* CEngine::NewL( )
    {
    CEngine* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CEngine::ConstructL
// 2nd phase constructor
// --------------------------------------------------------------------------
//
void CEngine::ConstructL()
    {
    LOGENTRFN("CEngine::ConstructL()")
    // Renaming the thread with the real process name helps debugging panics.
    RThread::RenameMe( KDCLName );

    FeatureManager::InitializeLibL();

    iConnectionMonitor.ConnectL();

    User::LeaveIfError( iFsEventLog.Connect() );

    iLogWrap = CLogWrapper::NewL( iFsEventLog, CActive::EPriorityStandard );
    iLogEventQueue = new( ELeave ) CArrayPtrFlat<CEvent>( KEventStateMaxCount );
    iLogEventQueue->Reset();

    iConnections = new( ELeave ) CArrayPtrFlat<CEvent>( KEventStateMaxCount );
    iConnections->Reset();

    iCurrentLogEvent = CLogEvent::NewL();

    iLogClient = CLogClient::NewL( iFsEventLog );

    // Connect to ETel server
    // The RTelServer::Connect() might not always work with the first trial,
    // because of a coding error related to using semaphores in the method.

    TInt err( KErrNotReady );
    TInt a = 0;
    while( a < KPhoneRetryCount && err != KErrNone )
        {
        if ( a )
            {
            User::After( KPhoneRetryTimeout ); // Don't wait on first try
            }
        err = iTelServer.Connect();
        LOGIT1("Connecting to ETel server <%d>", err)
        a++;
        }
    User::LeaveIfError( err );

    // Try to load phone.tsy
    TBuf<KCommsDbSvrMaxColumnNameLength> tsyName;
    tsyName.Copy( KMmTsyModuleName );
    err = iTelServer.LoadPhoneModule( tsyName );

    if ( err == KErrNone )
        {
        iTSYLoaded = 1;
        // Phone info can be retrieved with value 0 if there is only 1 phone
        RTelServer::TPhoneInfo info;
        User::LeaveIfError( iTelServer.GetPhoneInfo( 0, info ) );
        User::LeaveIfError( iMobilePhone.Open( iTelServer, info.iName ) );

        // Open packet service
        err = iPacketService.Open( iMobilePhone );
        if ( err == KErrNone )
            {
            iPacketServLoaded = 1;
            }
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdLoggerGprs ) )
        {
        LOGIT0("LoggerGprs supported")

        // Get the Attach Mode setting from CommDb and set ETel to use that
        // value. Must not leave if an error occurs.
        TRAPD( leaveCode, SetAttachModeL() );
        LOGIT1("SetAttachModeL() result: <%d>", leaveCode)

        // Get the default APN from CommDb and set ETel to use that APN.
        // Must not leave if an error occurs.
        TRAP( leaveCode, SetAPNL() );
        LOGIT1("SetAPNL() result: <%d>", leaveCode)
        }
    // Adds the event type Wlan. Otherwise the LogClient
    // will not understand this type of event.
    AddEventTypeL();

    // Read ini file settings
    ReadIniFile();

    // Create timer mode AO
    iDclTimerAO = new( ELeave ) CDclTimerAO ( this, iTimerInterval );
    iDclTimerAO->ConstructL();

    // Create saeObserver for updating P&S variables
    iSaeObserver = CSaeObserver::NewL( iConnectionMonitor );
    LOGEXITFN("CEngine::ConstructL()")
    }

// --------------------------------------------------------------------------
// CEngine::~CEngine
// Destructor
// Frees reserved resources.
// --------------------------------------------------------------------------
//
CEngine::~CEngine()
    {
    LOGENTRFN("CEngine::~CEngine()")
    FeatureManager::UnInitializeLib();

    delete iSaeObserver;
    iSaeObserver = NULL;

    // Disconnect from CM server.
    iConnectionMonitor.Close();

    Cancel();
    // The queue could be not empty if the active object
    // is aborted and destructed.
    if ( iLogEventQueue != 0 )
        {
        iLogEventQueue->ResetAndDestroy();
        }

    if ( iConnections != 0 )
        {
        iConnections->ResetAndDestroy();
        }

    delete iLogWrap;
    iLogWrap = NULL;

    delete iLogEventQueue;
    iLogEventQueue = NULL;

    delete iConnections;
    iConnections = NULL;

    delete iCurrentLogEvent;
    iCurrentLogEvent = NULL;

    delete iLogEventType;
    iLogEventType = NULL;

    delete iLogClient;
    iLogClient = NULL;

    iFsEventLog.Close();

    // Close "global" ETEL objects
    if ( iPacketServLoaded == 1 )
        {
        iPacketService.Close();
        }

    iMobilePhone.Close();
    iTelServer.Close();

    delete iDclTimerAO;
    iDclTimerAO = NULL;
    LOGEXITFN("CEngine::~CEngine()")
    }

// --------------------------------------------------------------------------
// CEngine::NotifyL
// Request Connection Monitor server to send notifications of events.
// --------------------------------------------------------------------------
//
void CEngine::NotifyL()
    {
    iConnectionMonitor.NotifyEventL( *this );
    }

// --------------------------------------------------------------------------
// CEngine::EventL
// Receives notifications of events from Connection Monitor server.
// --------------------------------------------------------------------------
//
void CEngine::EventL( const CConnMonEventBase& aConnMonEvent )
    {
    //LOGENTRFN("CEngine::EventL()")
    LOGIT1("-> CEngine::EventL() entered, event type (%d)", aConnMonEvent.EventType())
    TInt err( KErrNone );
    TUint connectionId( 0 );

    // Update P&S variables first
    if ( iSaeObserver )
        {
        iSaeObserver->EventL( aConnMonEvent );
        }

    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonCreateConnection:
            {
            TInt bearer( 0 );
            TRequestStatus status( KErrNotReady );

            const CConnMonCreateConnection* eventCreate;
            eventCreate  = static_cast<const CConnMonCreateConnection*>( &aConnMonEvent );
            connectionId = eventCreate->ConnectionId();

            LOGIT1("*** EConnMonCreateConnection event, id %d ***", connectionId)

            // Get the bearer info from CM server.
            err = KErrNotReady;
            TInt a = 0;
            while( a < KPhoneRetryCount && err != KErrNone )
                {
                // ConnMon might not be able to provide KBearer with the first trial.
                if ( a )
                    {
                    User::After( KPhoneRetryTimeout ); // Don't wait on first try
                    }
                iConnectionMonitor.GetIntAttribute(
                        connectionId,
                        KSubConnectionId,
                        KBearer,
                        bearer,
                        status );
                User::WaitForRequest( status );
                err = status.Int();
                LOGIT2("Got bearer %d from ConnMon <%d>", bearer, err)
                a++;
                }
            if ( err != KErrNone )
                {
                LOGIT1("FAILED to get KBearer from ConnMon <%d>", err)
                break;
                }

            // Check that it is GPRS or WCDMA.
            if ( bearer == EBearerGPRS     ||
                 bearer == EBearerEdgeGPRS ||
                 bearer == EBearerWCDMA )
                {
                LOGIT0("New internal connection")
                // Set the uplink and downlink thresholds for this particular connection.
                TUint threshold( iGPRSThreshold );
                if ( bearer == EBearerWCDMA )
                    {
                    threshold = iWCDMAThreshold;
                    }

                err = iConnectionMonitor.SetUintAttribute(
                        connectionId,
                        KSubConnectionId,
                        KDownlinkDataThreshold,
                        threshold );
                if ( err != KErrNone )
                    {
                    LOGIT1("FAILED to set KDownlinkDataThreshold to ConnMon <%d>", err)
                    }

                err = iConnectionMonitor.SetUintAttribute(
                        connectionId,
                        KSubConnectionId,
                        KUplinkDataThreshold,
                        threshold );
                if ( err != KErrNone )
                    {
                    LOGIT1("FAILED to set KUplinkDataThreshold to ConnMon <%d>", err)
                    }

                TName iapName;
                iapName.FillZ();
                iapName.Zero();

                // Get String Attribute -> KIAPName.
                // Get IAP name from CM server.
                err = KErrNotReady;
                a = 0;
                while( a < KPhoneRetryCount && err != KErrNone )
                    {
                    // ConnMon might not be able to provide KIAPName with the first trial.
                    if ( a )
                        {
                        User::After( KPhoneRetryTimeout ); // Don't wait on first try
                        }
                    iConnectionMonitor.GetStringAttribute(
                            connectionId,
                            KSubConnectionId,
                            KIAPName,
                            iapName,
                            status );
                    User::WaitForRequest( status );
                    err = status.Int();
                    LOG( TBuf<KMaxName+1> iapName2( iapName ); )
                    LOGIT2("Got KIAPName from ConnMon <%s>, <%d>", iapName2.PtrZ(), err)
                    a++;
                    }

                if ( err == KErrNone )
                    {
                    LogDataAddEventL(
                            connectionId,
                            R_LOG_CON_CONNECTED,
                            iapName,
                            R_LOG_DIR_OUT,
                            KNullDesC,
                            KLogPacketDataEventTypeUid );
                    }
                else
                    {
                    LOGIT1("FAILED to get KIAPName from ConnMon <%d>", err)
                    }
                }
            else if ( bearer == EBearerWLAN )
                // Add EBearerExternalWLAN in order to enable DCL to log
                // external WLAN connections, i.e. then the mobile phone
                // functions as a WLAN modem
                {
                LOGIT0("New WLAN connection")

                // Set the uplink and downlink thresholds for this particular connection.
                err = iConnectionMonitor.SetUintAttribute(
                        connectionId,
                        KSubConnectionId,
                        KDownlinkDataThreshold,
                        iWLANThreshold );
                if ( err != KErrNone )
                    {
                    LOGIT1("FAILED to set KDownlinkDataThreshold to ConnMon <%d>", err)
                    }

                // Even if KDownlinkDataThreshold and KUplinkDataThreshold
                // are used here ( 4096 bytes ), the Connection Monitor
                // server actually sets the threshold to 20480 bytes =
                // 20 kilobytes.
                err = iConnectionMonitor.SetUintAttribute(
                        connectionId,
                        KSubConnectionId,
                        KUplinkDataThreshold,
                        iWLANThreshold );
                if ( err != KErrNone )
                    {
                    LOGIT1("FAILED to set KUplinkDataThreshold to ConnMon <%d>", err)
                    }

                TName iapName;
                iapName.FillZ();
                iapName.Zero();
                // Get Uint Attribute -> KIAPId.
                // Get IAP id from CM server.
                err = KErrNotReady;
                a = 0;
                TUint iapId( 0 );
                while( a < KPhoneRetryCount && err != KErrNone )
                    {
                    // ConnMon might not be able to provide KIAPId with the first trial.
                    if ( a )
                        {
                        User::After( KPhoneRetryTimeout ); // Don't wait on first try
                        }
                    iConnectionMonitor.GetUintAttribute(
                            connectionId,
                            KSubConnectionId,
                            KIAPId,
                            iapId,
                            status );
                    User::WaitForRequest( status );
                    err = status.Int();
                    LOGIT2("Got IAP ID %d from ConnMon <%d>", iapId, err)
                    a++;
                    }
                // For each new WLAN connection, we need to check the used IAP.
                // For a normal IAP, the IAP name is used for logging, but when
                // customer is using the 'Search for WLAN' functionality and
                // chooses a WLAN network that is not found in any current IAP,
                // the Easy WLAN IAP is used instead. This IAP has some internal
                // name, e.g. 'Easy WLAN', that should not be visible anywhere
                // in the UI. So SSID of used network is used in logging instead.
                //
                if ( err == KErrNone )
                    {
                    // Create a session with the latest version CommsDat
                    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
                    db->SetAttributeMask( ECDProtectedWrite );

                    // Load IAP record
                    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
                    CleanupStack::PushL( iapRecord );
                    iapRecord->SetRecordId( iapId );
                    iapRecord->LoadL( *db );
                    LOGIT0("Read IAP record from CommsDat")

                    // Load wlan service record
                    CCDWlanServiceRecord* wlanRecord = new( ELeave )CCDWlanServiceRecord(
                            CCDWlanServiceRecord::TableIdL( *db ) );
                    CleanupStack::PushL( wlanRecord );
                    wlanRecord->iWlanServiceId = iapRecord->iService;

                    if ( wlanRecord->FindL( *db ) )
                        {
                        wlanRecord->LoadL( *db );
                        LOGIT0("Read WLAN record from CommsDat")

                        if ( wlanRecord->iWLanSSID.IsNull() )
                            {
                            // Used IAP is the special 'Easy WLAN' IAP, so we need to
                            // log the used WLAN network SSID instead of 'Easy WLAN'.
                            // This can be read from the 'iWLanUsedSSID'-field.
                            if ( !( wlanRecord->iWLanUsedSSID.IsNull() ) )
                                {
                                iapName = wlanRecord->iWLanUsedSSID.GetL();
                                LOGIT1("Easy WLAN detected, got SSID <%s> from CommsDat", &iapName)
                                }
                            else
                                {
                                LOGIT0("Error, Easy WLAN detected but no SSID found from CommsDat")
                                }
                            }
                        else
                            {
                            // A noraml WLAN IAP, read IAP name and use it for logging
                            iapName = iapRecord->iRecordName.GetL();
                            LOGIT1("Got IAP name <%s> from CommsDat", &iapName)
                            }
                        }
                    else
                        {
                        LOGIT0("Error, WLAN record not found from CommsDat")
                        }
                    LogDataAddEventL(
                            connectionId,
                            R_LOG_CON_CONNECTED,
                            iapName,
                            R_LOG_DIR_OUT,
                            KNullDesC,
                            KLogWlanDataEventTypeUid );

                    CleanupStack::PopAndDestroy( wlanRecord );
                    CleanupStack::PopAndDestroy( iapRecord );
                    CleanupStack::PopAndDestroy( db );
                    }
                else
                    {
                    LOGIT1("FAILED to get KIAPId from ConnMon", err)
                    }
                }
            // Check if it is EXTERNAL GPRS or WCDMA.
            else if ( bearer == EBearerExternalGPRS     ||
                      bearer == EBearerExternalEdgeGPRS ||
                      bearer == EBearerExternalWCDMA )
                {
                LOGIT0("New external connection")

                TName apName;
                apName.FillZ();
                apName.Zero();

                err = KErrNotReady;
                a = 0;
                // We loop until the retry count is reached or
                // we get a name with length > 0
                // This is because the connection monitor asks the PSD Fax
                // for the name and PSD fax sometines returns an empty string
                // without returning an error code (KErrNone is returned)
                // However if no name for the APN has been defined this will
                // loop for KPhoneRetryCount times for vain, but it's better than
                // to use an empty name if one has been defined...
                while( a < KPhoneRetryCount &&
                        ( ( err != KErrNone ) || ( apName.Length() == 0 ) ) )
                    {
                    // ConnMon might not be able to provide KAccessPointName with first trial.
                    if ( a )
                        {
                        User::After( KPhoneRetryTimeout ); // Don't wait on first try
                        }
                    // Get String Attribute -> KAccessPointName.
                    // Get APN from CM server.
                    iConnectionMonitor.GetStringAttribute(
                            connectionId,
                            KSubConnectionId,
                            KAccessPointName,
                            apName,
                            status );
                    User::WaitForRequest( status );
                    err = status.Int();
                    LOG( TBuf<KMaxName+1> apName2( apName ); )
                    LOGIT2("Got KAccessPointName from ConnMon <%s>, <%d>", apName2.PtrZ(), err)
                    a++;
                    }
                if ( err == KErrNone )
                    {
                    LogDataAddEventL(
                            connectionId,
                            R_LOG_CON_CONNECTED,
                            apName,
                            R_LOG_DIR_OUT,
                            KNullDesC,
                            KLogPacketDataEventTypeUid );
                    }
                else
                    {
                    LOGIT1("FAILED to get KAccessPointName from ConnMon <%d>", err)
                    }
                }
            else
                {
                LOGIT0("Bearer neither GPRS nor WCDMA nor WLAN, ignored")
                break;
                }
            break;
            }

        case EConnMonConnectionStatusChange:
            {
            TInt bearer( 0 );
            TRequestStatus status( KErrNotReady );

            const CConnMonConnectionStatusChange* eventStatus;
            eventStatus  = static_cast<const CConnMonConnectionStatusChange*>( &aConnMonEvent );
            connectionId = eventStatus->ConnectionId();

            LOGIT2("*** EConnMonConnectionStatusChange event, id %d, status %d ***",
                    connectionId, eventStatus->ConnectionStatus())

            if ( eventStatus->ConnectionStatus() == KPsdFinishedActivation )
                {
                // Get the bearer info from CM server.
                iConnectionMonitor.GetIntAttribute(
                        connectionId,
                        KSubConnectionId,
                        KBearer,
                        bearer,
                        status );
                User::WaitForRequest( status );
                err = status.Int();
                if ( err != KErrNone )
                    {
                    LOGIT1("FAILED to get bearer from ConnMon <%d>", err)
                    break;
                    }

                // Check that it is external GPRS or WCDMA.
                if ( bearer == EBearerExternalGPRS      ||
                     bearer == EBearerExternalEdgeGPRS  ||
                     bearer == EBearerExternalWCDMA )
                    // Add EBearerLAN in order to enable debugging in wins.
                    {
                    LOGIT1("Status: KPsdFinishedActivation, bearer: %d (valid external), updating AP name", bearer)
                    TName apName;
                    apName.FillZ();
                    apName.Zero();

                    err = KErrNotReady;
                    TInt a = 0;
                    // We loop until the retry count is reached or
                    // we get a name with length > 0
                    // This is because the connection monitor asks the PSD Fax
                    // for the name and PSD fax sometines returns an empty string
                    // without returning an error code (KErrNone is returned)
                    // However if no name for the APN has been defined this will
                    // loop for KPhoneRetryCount times for vain, but it's better than
                    // to use an empty name if one has been defined...
                    while( a < KPhoneRetryCount &&  (
                            ( err != KErrNone ) ||
                            ( apName.Length() == 0 )
                            ) )
                        {
                        // ConnMon might not be able to provide KAccessPointName with first trial.
                        if ( a )
                            {
                            User::After( KPhoneRetryTimeout ); // Don't wait on first try
                            }
                        // Get String Attribute -> KAccessPointName.
                        // Get APN from CM server.
                        iConnectionMonitor.GetStringAttribute(
                                connectionId,
                                KSubConnectionId,
                                KAccessPointName,
                                apName,
                                status );
                        User::WaitForRequest( status );
                        err = status.Int();
                        LOG( TBuf<KMaxName+1> apName2( apName ); )
                        LOGIT2("Got KAccessPointName from ConnMon <%s>, <%d>", apName2.PtrZ(), err)
                        a++;
                        }

                    if ( err == KErrNone )
                        {
                        // We want only to change the existing entry in the log,
                        // not add a new one, thus LogDataChangeEventL instead of
                        // LogDataAddEventL
                        LogDataChangeEventL( connectionId, apName );
                        }
                    else
                        {
                        LOGIT1("FAILED to get KAccessPointName from ConnMon <%d>", err)
                        }
                    }
                else
                    {
                    LOGIT1("Status: KPsdFinishedActivation, bearer: %d (not valid external)", bearer)
                    }
                }
            }
            break;

        case EConnMonDeleteConnection:
            {
            const CConnMonDeleteConnection* eventDelete;
            eventDelete  = static_cast<const CConnMonDeleteConnection*>( &aConnMonEvent );
            connectionId = eventDelete->ConnectionId();

            LOGIT1("*** EConnMonDeleteConnection event, id %d ***", connectionId)
            LOGIT1("sent: %d bytes", eventDelete->UplinkData())
            LOGIT1("recv: %d bytes", eventDelete->DownlinkData())

            // First update SharedData downlink and uplink data
            UpdateSharedDataL(
                    connectionId,
                    eventDelete->UplinkData(),
                    eventDelete->DownlinkData() );

            // Then update connection specific downlink and uplink data
            err = LogDataUpdateEventL(
                    connectionId,
                    R_LOG_CON_DISCONNECTED,
                    TUid::Null(),
                    eventDelete->UplinkData(),
                    eventDelete->DownlinkData() );
            if ( err != KErrNone )
                {
                LOGIT1("FAILED to log connection deletion <%d>", err)
                }

            // Stop timer mode for this connection.
            // It is ok to call this even if the connection was not in timer mode.
            iDclTimerAO->Remove( connectionId );

            }
            break;

        case EConnMonDownlinkDataThreshold:
            {
            TInt oldDuration( 0 );
            TInt newDuration( 0 );

            const CConnMonDownlinkDataThreshold* down;
            down = static_cast<const CConnMonDownlinkDataThreshold*>( &aConnMonEvent );
            connectionId = down->ConnectionId();

            LOGIT1("*** EConnMonDownlinkDataThreshold event, id %d ***", connectionId)

            if ( !GetDuration( connectionId, oldDuration ) )
                {
                return;
                }

            // First update SharedData downlink data
            UpdateSharedDataL( connectionId, 0, down->DownlinkData() );

            // Then update connection specific downlink data
            // Also uplink data has to be updated at the same time,
            // since CEventLog function SetDataL requires both values.
            TRequestStatus status;
            TUint uplinkData( 0 );
            // Get the amount uplink data from CM server
            iConnectionMonitor.GetUintAttribute(
                    connectionId,
                    KSubConnectionId,
                    KUplinkData,
                    uplinkData,
                    status );
            User::WaitForRequest( status );
            if ( status.Int() != KErrNone )
                {
                LOGIT1("FAILED to get KUplinkData from ConnMon <%d>", err)
                }

            // The actual status of connection is R_LOG_CON_CONNECTED, but
            // due to the performance reasons KConnectionStatusIdNotAvailable
            // is used here. There is no need to read LOGWRAP.RSC every time.
            err = LogDataUpdateEventL(
                    connectionId,
                    KConnectionStatusIdNotAvailable,
                    TUid::Null(),
                    uplinkData,
                    down->DownlinkData() );
            if ( err != KErrNone )
                {
                LOGIT1("FAILED to update log event <%d>", err)
                }

            // Switch to timer if we are updating log too often
            if ( GetDuration( connectionId, newDuration ) )
                {
                if ( ( newDuration - oldDuration ) < iTimerInterval )
                    {
                    SwitchToTimerMode( connectionId );
                    }
                }
            }
            break;

        case EConnMonUplinkDataThreshold:
            {
            TInt oldDuration( 0 );
            TInt newDuration( 0 );

            const CConnMonUplinkDataThreshold* up;
            up = static_cast<const CConnMonUplinkDataThreshold*>( &aConnMonEvent );
            connectionId = up->ConnectionId();

            LOGIT1("*** EConnMonUplinkDataThreshold event, id %d ***", connectionId)

            if ( !GetDuration( connectionId, oldDuration ) )
                {
                return;
                }

            // First update SharedData uplink data
            UpdateSharedDataL( connectionId, up->UplinkData(), 0 );

            // Then update connection specific uplink data
            // Also downlink data has to be updated at the same time,
            // since CEventLog function SetDataL requires both values.
            TRequestStatus status;
            TUint downlinkData( 0 );

            // Get the amount of downlink data from CM server
            iConnectionMonitor.GetUintAttribute(
                    connectionId,
                    KSubConnectionId,
                    KDownlinkData,
                    downlinkData,
                    status );
            User::WaitForRequest( status );
            if ( status.Int() != KErrNone )
                {
                LOGIT1("FAILED to get KDownlinkData from ConnMon <%d>", err)
                }

            // The actual status of connection is R_LOG_CON_CONNECTED, but
            // due to the performance reasons KConnectionStatusIdNotAvailable
            // is used here. There is no need to read LOGWRAP.RSC every time.
            err = LogDataUpdateEventL(
                    connectionId,
                    KConnectionStatusIdNotAvailable,
                    TUid::Null(),
                    up->UplinkData(),
                    downlinkData );
            if ( err != KErrNone )
                {
                LOGIT1("FAILED to update log event <%d>", err)
                }

            // Switch to timer if we are updating log too often
            if ( GetDuration( connectionId, newDuration ) )
                {
                if ( ( newDuration - oldDuration ) < iTimerInterval )
                    {
                    SwitchToTimerMode( connectionId );
                    }
                }
            }
            break;

        default:
            /*
             *  By default Connection Monitor server sends all the events to
             *  all the clients, which have subscribed to reveive events. It
             *  is not possible to subscribe to only selected events.
             *
             *  Therefore these events are not written to log.
             */
            break;
        }
    LOGEXITFN("CEngine::EventL()")
    }

// --------------------------------------------------------------------------
// CEngine::LogDataAddEventL
// Adds log events.
//
// Handles also multiple data connections.
// --------------------------------------------------------------------------
//
void CEngine::LogDataAddEventL(
        TLogId       aConnectionId,
        TInt         aRConnectionStatusId,
        const TDesC& aRemote,
        TInt         aLogDir,
        const TDesC& aTelNum,
        const TUid&  aDataEventType )
    {
    LOGENTRFN("CEngine::LogDataAddEventL()")
    // It is possible to add a new logevent with a new log id for the same
    // connection (reconnect case) assuming that all the next updates will
    // be for the new event and not the old one.
    TTime time;
    time.UniversalTime();
    TBuf<KLogMaxStatusLength> logStatusBuf;
    TBuf<KLogMaxDirectionLength> logDirBuf;

    // Set the parameters for CEvent
    CEvent* eventUpdate = CEvent::NewL();
    CleanupStack::PushL( eventUpdate );
    eventUpdate->iConnectionId = aConnectionId;
    eventUpdate->iLogWrapEvent->SetId( KGenconnLogWaitingForLogId );
    eventUpdate->iLogWrapEvent->SetTime( time );
    // Ignore error - string blank on error which is ok.
    iLogWrap->Log().GetString( logStatusBuf, aRConnectionStatusId );
    eventUpdate->iLogWrapEvent->SetStatus( logStatusBuf );
    eventUpdate->iLogWrapEvent->SetRemoteParty( aRemote );
    // Ignore error - string blank on error which is ok.
    iLogWrap->Log().GetString( logDirBuf, aLogDir );
    eventUpdate->iLogWrapEvent->SetDirection( logDirBuf );
    eventUpdate->iLogWrapEvent->SetNumber( aTelNum );
    eventUpdate->iLogWrapEvent->SetEventType( *CONST_CAST( TUid*, &aDataEventType ) );
    eventUpdate->iLogWrapEvent->SetDurationType( KLogDurationValid );

    // If there are no requests in the queue, then write to log immediately.
    if ( !IsActive() && ( iLogEventQueue->Count() == 0 ) )
        {
        iConnections->AppendL( eventUpdate );
        iStatus = KRequestPending;
        iLogWrap->Log().AddEvent( *eventUpdate->iLogWrapEvent, iStatus );
        SetActive();
        }
    else
        {
        LOGIT0("Adding event to eventqueue")
        // Add the request to the queue, it will be processed ASAP.
        iLogEventQueue->AppendL( eventUpdate );
        }
    CleanupStack::Pop( eventUpdate );
    LOGEXITFN("CEngine::LogDataAddEventL()")
    }

// --------------------------------------------------------------------------
// CEngine::LogDataChangeEventL
// Changes log events. Used to update the access point name of the
// external GPRS ie. dial-up connections.
//
// Handles also multiple data connections.
// --------------------------------------------------------------------------
//
void CEngine::LogDataChangeEventL( TLogId aConnectionId, const TDesC& aRemote )
    {
    LOGENTRFN("CEngine::LogDataChangeEventL()")

    // Check event queue first, if a match is found, the connection hasn't been
    // added to iConnections array yet. Update event in queue with correct
    // access point name.
    for ( TInt index = 0; index < iLogEventQueue->Count(); index++ )
        {
        LOGIT2("Checking event queue index %d, id %d", index, iLogEventQueue->At( index )->iConnectionId)
        if ( iLogEventQueue->At( index )->iConnectionId == aConnectionId )
            {
            LOGIT0("Match found, merging updated apn to queued event")
            iLogEventQueue->At( index )->iLogWrapEvent->SetRemoteParty( aRemote );
            LOGEXITFN("CEngine::LogDataChangeEventL()")
            return;
            }
        }

    // Most likely event queue was empty, so next we need to find the relevant
    // connection from iConnections array.
    for ( TInt index = 0; index < iConnections->Count(); index++ )
        {
        LOGIT2("Checking connections index %d, id %d", index, iConnections->At( index )->iConnectionId)

        if ( iConnections->At( index )->iConnectionId == aConnectionId )
            {
            // Can update now, or need add to queue
            if ( !IsActive() && ( iLogEventQueue->Count() == 0 ) )
                {
                LOGIT0("Match found, updating apn immediately")

                // Update apn and initiate LogWrapper update.
                iConnections->At( index )->iLogWrapEvent->SetRemoteParty( aRemote );
                iStatus = KRequestPending;
                iLogWrap->Log().ChangeEvent( *iConnections->At( index )->iLogWrapEvent, iStatus );
                SetActive();
                }
            else
                {
                LOGIT0("Match found, adding apn update event to queue")

                // Make a copy of the CLogEvent, update apn and add the updated
                // copy to event queue.
                CEvent* eventUpdate = CEvent::NewL();
                CleanupStack::PushL( eventUpdate );

                eventUpdate->iConnectionId = aConnectionId;
                eventUpdate->iLogWrapEvent->CopyL( *iConnections->At( index )->iLogWrapEvent );
                eventUpdate->iLogWrapEvent->SetRemoteParty( aRemote );
                iLogEventQueue->AppendL( eventUpdate );

                CleanupStack::Pop( eventUpdate );
                }
            }
        break; // for
        }
    LOGEXITFN("CEngine::LogDataChangeEventL()")
    }

// --------------------------------------------------------------------------
// CEngine::LogDataUpdateEventL
// Updates log events.
//
// Handles also multiple data connections.
// --------------------------------------------------------------------------
//
TInt CEngine::LogDataUpdateEventL(
        TLogId        aConnectionId,
        TInt          aRConnectionStatusId,
        const TUid&   aDataEventType,
        const TInt64& aBytesSent,
        const TInt64& aBytesReceived )
    {
    LOGENTRFN("CEngine::LogDataUpdateEventL()")
    TInt ret = KErrNone;
    TLogId connectionId;

    for ( TInt index = 0; index < iConnections->Count(); index++ )
        {
        connectionId = iConnections->At( index )->iConnectionId;

        if ( connectionId == aConnectionId )
            {
            // Check if there is no request pending, then start it. Otherwise
            // wait until the previous request is finished and keep going on.
            if ( !IsActive() && ( iLogEventQueue->Count() == 0 ) )
                {
                // Request update straight on.
                UpdateLogEventParamL(
                        aConnectionId,
                        *iConnections->At( index )->iLogWrapEvent,
                        aRConnectionStatusId,
                        aDataEventType,
                        aBytesSent,
                        aBytesReceived );

                iConnections->At( index )->iPrevDuration =
                        iConnections->At( index )->iLogWrapEvent->Duration();

                iLogWrap->Log().ChangeEvent( *iConnections->At( index )->iLogWrapEvent, iStatus );
                SetActive();
                }
            else
                {
                LOGIT0("Adding event update to queue")
                // Add the request to the queue, it will be processed ASAP.
                CEvent* eventUpdate = CEvent::NewL();
                CleanupStack::PushL( eventUpdate );
                eventUpdate->iConnectionId = aConnectionId;
                eventUpdate->iLogWrapEvent->CopyL( *iConnections->At( index )->iLogWrapEvent );
                ret = UpdateLogEventParamL(
                        aConnectionId,
                        *eventUpdate->iLogWrapEvent,
                        aRConnectionStatusId,
                        aDataEventType,
                        aBytesSent,
                        aBytesReceived );

                iConnections->At( index )->iPrevDuration =
                        eventUpdate->iLogWrapEvent->Duration();

                // Add to the queue.
                iLogEventQueue->AppendL( eventUpdate );
                CleanupStack::Pop( eventUpdate );
                }
            }
        }
    LOGEXITFN1("CEngine::LogDataUpdateEventL()", ret)
    return ret;
    }

// --------------------------------------------------------------------------
// CEngine::UpdateLogEventParamL
// Updates log event parameters.
// --------------------------------------------------------------------------
//
TInt CEngine::UpdateLogEventParamL(
        TLogId        /*aConnectionId*/,
        CLogEvent&    aLogEvent,
        TInt          aRConnectionStatusId,
        const TUid&   aDataEventType,
        const TInt64& aBytesSent,
        const TInt64& aBytesReceived )
    {
    LOGENTRFN("CEngine::UpdateLogEventParamL()")
    TInt ret( KErrNone );
    TTime now;
    TTimeIntervalSeconds seconds( 0 );
    now.UniversalTime();

    if ( now.SecondsFrom( aLogEvent.Time(), seconds ) != KErrNone )
        {
        seconds = 0; // No duration available -> error.
        }
    aLogEvent.SetDuration( seconds.Int() );

    if ( KConnectionStatusIdNotAvailable != aRConnectionStatusId )
        {
        // Status needs to be updated.
        TBuf<KLogMaxStatusLength> logStatusBuf;
        // Ignore error - string blank on error which is ok.
        iLogWrap->Log().GetString( logStatusBuf, aRConnectionStatusId );
        aLogEvent.SetStatus( logStatusBuf );
        }
    if ( aDataEventType != TUid::Null() )
        {
        aLogEvent.SetEventType( *CONST_CAST( TUid*, &aDataEventType ) );
        }
    // Check if data metrics need to be updated.
    TInt64 byteInfoNotAvailable( KBytesInfoNotAvailable );
    if ( ( aBytesReceived != byteInfoNotAvailable ) &&
         ( aBytesSent     != byteInfoNotAvailable ) )
        {
        TBuf8<KDatabufferSize> dataBuffer;
        dataBuffer.Num( aBytesSent );
        dataBuffer.Append( TChar(',') );
        dataBuffer.AppendNum( aBytesReceived );
        TRAP( ret, aLogEvent.SetDataL( dataBuffer ) );
        }
    LOGEXITFN1("CEngine::UpdateLogEventParamL()", ret)
    return ret;
    }

// --------------------------------------------------------------------------
// CEngine::UpdateSharedDataL
// Updates cumulative counters of SharedData.
// --------------------------------------------------------------------------
//
void CEngine::UpdateSharedDataL(
        TLogId aConnectionId,
        const TInt64& aBytesSent,
        const TInt64& aBytesReceived )
    {
    LOGENTRFN("CEngine::UpdateSharedDataL()")
    TInt64   sent( 0 );
    TInt64   received( 0 );
    TInt64   increment( 0 );
    TLogId   connId;
    TBuf<50> receivedBuf,  sentBuf;
    TBuf<50> receivedBuf2, sentBuf2;
    TUid     dataEventType = KLogPacketDataEventTypeUid;
    TInt     err, index;

    // First find out the data event type (GPRS or WLAN)
    for ( index = 0; index < iConnections->Count(); index++ )
        {
        connId = iConnections->At( index )->iConnectionId;
        if ( connId == aConnectionId )
            {
            dataEventType = iConnections->At( index )->iLogWrapEvent->EventType();

            if ( aBytesSent == 0 && aBytesReceived == 0 )
                {
                break;
                }

            CRepository* repository = NULL;
            repository = CRepository::NewL( KCRUidDCLLogs );
            if ( repository )
                {
                err = repository->StartTransaction( CRepository::EConcurrentReadWriteTransaction );
                if ( err )
                    {
                    LOGIT1("UpdateSharedDataL: StartTransaction() returned error: %d", err)
                    delete repository;
                    break;
                    }

                if ( aBytesReceived != 0 )
                    {
                    // Read current value.
                    if ( dataEventType == KLogPacketDataEventTypeUid )
                        {
                        err = repository->Get( KLogsGPRSReceivedCounter, receivedBuf );
                        }
                    else
                        {
                        err = repository->Get( KLogsWLANReceivedCounter, receivedBuf );
                        }
                    if ( err == KErrNone )
                        {
                        TLex lex( receivedBuf );
                        lex.Val( received );
                        }
                    else
                        {
                        LOGIT1("UpdateSharedDataL: aBytesReceived: Get() returned error: %d", err)
                        repository->CancelTransaction();
                        delete repository;
                        break;
                        }

                    // Calculate the increment
                    increment = aBytesReceived - iConnections->At( index )->iPrevDownValue;
                    // And add _only_ the increment
                    received += increment;
                    // Update the previous downlink value
                    iConnections->At( index )->iPrevDownValue = aBytesReceived;
                    receivedBuf2.Num( received );

                    // Store updated value.
                    if ( dataEventType == KLogPacketDataEventTypeUid )
                        {
                        err = repository->Set( KLogsGPRSReceivedCounter, receivedBuf2 );
                        }
                    else
                        {
                        err = repository->Set( KLogsWLANReceivedCounter, receivedBuf2 );
                        }
                    if ( err != KErrNone )
                        {
                        LOGIT1("UpdateSharedDataL: aBytesReceived: Set() returned error: %d", err)
                        repository->CancelTransaction();
                        delete repository;
                        break;
                        }
                    }
                if ( aBytesSent != 0 )
                    {
                    // Read current value.
                    if ( dataEventType == KLogPacketDataEventTypeUid )
                        {
                        err = repository->Get( KLogsGPRSSentCounter, sentBuf );
                        }
                    else
                        {
                        err = repository->Get( KLogsWLANSentCounter, sentBuf );
                        }
                    if ( err == KErrNone )
                        {
                        TLex lex( sentBuf );
                        lex.Val( sent );
                        }
                    else
                        {
                        LOGIT1("UpdateSharedDataL: aBytesSent: Get() returned error: %d", err)
                        repository->CancelTransaction();
                        delete repository;
                        break;
                        }

                    // Calculate the increment
                    increment = aBytesSent - iConnections->At( index )->iPrevUpValue;
                    // And add _only_ the increment
                    sent += increment;
                    // Update the previous uplink value
                    iConnections->At( index )->iPrevUpValue = aBytesSent;
                    sentBuf2.Num( sent );

                    // Store updated value.
                    if ( dataEventType == KLogPacketDataEventTypeUid )
                        {
                        err = repository->Set( KLogsGPRSSentCounter, sentBuf2 );
                        }
                    else
                        {
                        err = repository->Set( KLogsWLANSentCounter, sentBuf2 );
                        }
                    if ( err != KErrNone )
                        {
                        LOGIT1("UpdateSharedDataL: aBytesSent: Set() returned error: %d", err)
                        repository->CancelTransaction();
                        delete repository;
                        break;
                        }
                    }
                TUint32 keyInfo;
                err = repository->CommitTransaction( keyInfo );
                if ( err != KErrNone )
                    {
                    LOGIT2("UpdateSharedDataL: repository->CommitTransaction returned error: %d, %d", err, keyInfo)
                    repository->CancelTransaction();
                    delete repository;
                    break;
                    }
                }
            delete repository;
            break;
            }
        } // for
    LOGEXITFN("CEngine::UpdateSharedDataL()")
    }

// --------------------------------------------------------------------------
// CEngine::RunL
// Handles the completion of an asynchronous request.
// This function is defined as pure virtual in CActive and any class derived
// from CActive must define and implement it.
// --------------------------------------------------------------------------
//
void CEngine::RunL()
    {
    LOGENTRFN("CEngine::RunL()")

    // Request has completed.
    // Delete the disconnected element of iConnections, if it has already been
    // logged and the connection has been disconnected
    if ( iConnections->Count() > 0 )
        {
        LOGIT1("RunL: iConnections->Count() = %d (>0), clearing old connections", iConnections->Count())

        // Ignore error - string blank on error which is ok.
        TBuf<KLogMaxStatusLength> logStatusBuf;
        iLogWrap->Log().GetString( logStatusBuf, R_LOG_CON_DISCONNECTED );

        for ( TInt i = iConnections->Count()-1; i >= 0; i-- )
            {
            CEvent* elemPtr = iConnections->At( i );

            __ASSERT_DEBUG( ( elemPtr != NULL ), User::Panic( KDCLName, KErrNotReady ) );
            LOGIT2("Checking index %d, connection id %d", i, elemPtr->iConnectionId)
            if ( logStatusBuf == elemPtr->iLogWrapEvent->Status() )
                {
                LOGIT0("Status: disconnected, removing")

                // Remove the pointer from the queue.
                iConnections->Delete( i );
                // Delete the completed CEvent
                delete elemPtr;
                }
            }
        }

    // Delete completed event and check if there is a next event pending.
    // If LogEng is not supported, a dummy logeng just returns error straight
    // on, but we carry on doing all the requests.
    if ( iLogEventQueue->Count() > 0 )
        {
        LOGIT1("RunL: iLogEventQueue->Count() = %d (>0), processing next event", iLogEventQueue->Count())

        CEvent* nextEventPtr = iLogEventQueue->At( 0 );
        __ASSERT_DEBUG( ( nextEventPtr != NULL ), User::Panic( KDCLName, KErrNotReady ) );
        // In general, before the elements of this kind of array are deleted,
        // the CBase derived objects to which those elements point should be
        // destroyed. If they are not destroyed, then a separate copy of
        // those elements (i.e. the pointers), must be taken to avoid
        // orphaning the CBase derived objects on the heap.
        iCurrentLogEvent->CopyL( *nextEventPtr->iLogWrapEvent );
        iLogWrap->Log().ChangeEvent( *iCurrentLogEvent, iStatus );
        SetActive();
        // Remove the pointer from the queue.
        iLogEventQueue->Delete( 0 );
        // Delete the completed CEvent
        delete nextEventPtr;
        }
    LOGEXITFN("CEngine::RunL()")
    }

// --------------------------------------------------------------------------
// CEngine::DoCancel
// Handles a cancel request to the service provider.
// This function is defined as pure virtual in CActive and any class derived
// from CActive must define and implement it.
// --------------------------------------------------------------------------
//
void CEngine::DoCancel()
    {
    if ( iLogWrap )
        {
        iLogWrap->Log().Cancel();
        }
    // Usually you do not need to cancel an update on events, just let them
    // go and be removed from the queue when update is done.
    // If we cancel the logger, most likely the whole Logger Object will be
    // destroyed.
}

// --------------------------------------------------------------------------
// CEngine::SetAttachModeL
// Gets the Attach Mode setting from CommDb and sets ETel to use that value.
// --------------------------------------------------------------------------
//
void CEngine::SetAttachModeL() const
    {
    LOGENTRFN("CEngine::SetAttachModeL()")

    TUint32 commdbAM = RPacketService::EAttachWhenNeeded;
    TRequestStatus status;
    RPacketService::TAttachMode etelAM;
    CCommsDatabase* commDb = NULL;

    if ( iPacketServLoaded != 1 )
        {
        LOGIT0("RPacketService not loaded")
        return;
        }

    commDb = CCommsDatabase::NewL();

    // Get the Attach Mode setting from CommDb
    TRAPD( error, commDb->GetGlobalSettingL( TPtrC( GPRS_ATTACH_MODE ), commdbAM ) );
    switch ( error )
        {
        case KErrNone:
            // Attach Mode setting found in CommDb
            break;
        case KErrNotFound:
            {
            // Attach Mode setting not found in CommDb, it is set as EAttachWhenNeeded
            TRAPD( leaveCode, commDb->SetGlobalSettingL( TPtrC( GPRS_ATTACH_MODE ), commdbAM ) );
            if ( leaveCode )
                {
                LOGIT1("FAILED SetGlobalSettingL() <%d>", leaveCode)
                }
            }
            break;
        default:
            {
            LOGIT1("FAILED to get attach mode from CommDb <%d>", error)
            }
            break;
        }

    // And set ETel to use the Attach Mode value from CommDb
    etelAM = STATIC_CAST( RPacketService::TAttachMode, commdbAM );

    TInt err( KErrNotReady );
    TInt a = 0;
    while ( a < KPhoneRetryCount && err != KErrNone )
        {
        if ( a )
            {
            User::After( KPhoneRetryTimeout ); // Don't wait on first try
            }
        iPacketService.SetAttachMode( status, etelAM );
        User::WaitForRequest( status );
        err = status.Int();
        LOGIT1("Set attach Mode to ETel <%d>", err)
        a++;
        }
    if ( err != KErrNone )
        {
        LOGIT1("FAILED to set attach mode to ETel <%d>", err)
        }
    delete commDb;
    LOGEXITFN("CEngine::SetAttachModeL()")
    }

// --------------------------------------------------------------------------
// CEngine::SetAPNL
// Gets the default Access Point Name from CommDb and
// sets ETel to use that APN.
// --------------------------------------------------------------------------
//
void CEngine::SetAPNL() const
    {
    LOGENTRFN("CEngine::SetAPNL()")

    TBuf<KMaxAPName> apName;
    TRequestStatus status( KErrNotReady );
    CCommsDatabase* commDb = NULL;

    if ( iPacketServLoaded != 1 )
        {
        LOGIT0("RPacketService not loaded")
        return;
        }

    commDb = CCommsDatabase::NewL();
    CleanupStack::PushL( commDb );

    // Get the default Access Point Name from CommDb
    CCommsDbTableView* table = commDb->OpenTableLC( TPtrC( DEFAULT_GPRS ) );

    table->GotoFirstRecord();

    //If APN is not initialized, ReadTextL will leave
    apName.SetLength( 0 );

    table->ReadTextL( TPtrC( DEFAULT_GPRS_APN ), apName );

    CleanupStack::PopAndDestroy( table );

    // Get DefaultContextParams from ETel
    RPacketContext::TContextConfigGPRS* getParams = NULL;
    getParams = new( ELeave ) RPacketContext::TContextConfigGPRS();

    TPckg<RPacketContext::TContextConfigGPRS> getParamsPckg( *getParams );

    TInt err( KErrNotReady );
    TInt a = 0;
    while ( a < KPhoneRetryCount && err != KErrNone )
        {
        // The RPacketService::GetDefaultContextParams() does not always work
        // with the first trial, it might return KErrNotReady.
        if ( a )
            {
            User::After( KPhoneRetryTimeout ); // Don't wait on first try
            }
        iPacketService.GetDefaultContextParams( status, getParamsPckg );
        User::WaitForRequest( status );
        err = status.Int();
        LOGIT1("Got DefaultContextParams from ETel <%d>", err)
        a++;
        }

    if ( err != KErrNone )
        {
        LOGIT1("FAILED to get DefaultContextParams from ETel <%d>", err)
        }

    // Replace ETel APN with CommDb APN
    ReplaceAPN( getParams->iAccessPointName, apName );

    // Set DefaultContextParams to ETel
    iPacketService.SetDefaultContextParams( status, getParamsPckg );
    User::WaitForRequest( status );

    err = status.Int();
    if ( err != KErrNone )
        {
        LOGIT1("FAILED to set DefaultContextParams to ETel <%d>", err)
        }

    delete getParams;
    CleanupStack::PopAndDestroy( commDb );
    LOGEXITFN("CEngine::SetAPNL()")
    }

// --------------------------------------------------------------------------
// CEngine::AddEventTypeL
// Adds the event type Wlan. Otherwise the LogClient
// will not understand this type of event.
// --------------------------------------------------------------------------
//
void CEngine::AddEventTypeL()
    {
    iLogEventType = CLogEventType::NewL();
    iLogEventType->SetUid( KLogWlanDataEventTypeUid );
    iLogEventType->SetDescription( _L("Wlan") );
    iLogEventType->SetLoggingEnabled( ETrue );

    iStatus = KRequestPending;
    iLogClient->AddEventType( *iLogEventType, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CEngine::ReplaceAPN
// This method will be used in platforms where ETel APN (aResult) is UNICODE.
// -----------------------------------------------------------------------------
//
void CEngine::ReplaceAPN( TDes& aResult, const TDesC& aName ) const
    {
    aResult = aName;
    }

// -----------------------------------------------------------------------------
// CEngine::ReplaceAPN
// This method will be used in platforms where ETel APN (aResult) is UTF8.
// -----------------------------------------------------------------------------
//
void CEngine::ReplaceAPN( TDes8& aResult, const TDesC& aName ) const
    {
    CnvUtfConverter::ConvertFromUnicodeToUtf8( aResult, aName );
    }

// -----------------------------------------------------------------------------
// CEngine::LogDataInTimerModeL
// -----------------------------------------------------------------------------
//
TInt CEngine::LogDataInTimerModeL( const TUint aConnectionId )
    {
    LOGENTRFN("CEngine::LogDataInTimerModeL()")
    TInt index( 0 );
    TInt64 prevDown( 0 );
    TInt64 prevUp( 0 );

    TInt count( iConnections->Count() );

    LOGIT1("LogDataInTimerMode, id %d", aConnectionId)

    // Does the connection still exist
    for ( index = 0; index < count; index++ )
        {
        TUint connId( iConnections->At( index )->iConnectionId );

        if ( connId == aConnectionId )
            {
            prevDown = iConnections->At(index)->iPrevDownValue;
            prevUp = iConnections->At(index)->iPrevUpValue;
            break;
            }
        }

    if ( index == count )
        {
        return KErrNotFound;
        }

    // Read data volumes
    TRequestStatus status;
    TUint uplinkData( 0 );
    TUint downlinkData( 0 );

    // Get the amount uplink data from CM server
    iConnectionMonitor.GetUintAttribute(
            aConnectionId,
            KSubConnectionId,
            KUplinkData,
            uplinkData,
            status );
    User::WaitForRequest( status );

    if ( status.Int() != KErrNone )
        {
        LOGIT1("FAILED to get KUplinkData from ConnMon <%d>", status.Int())
        }

    // Get the amount of downlink data from CM server
    iConnectionMonitor.GetUintAttribute(
            aConnectionId,
            KSubConnectionId,
            KDownlinkData,
            downlinkData,
            status );
    User::WaitForRequest( status );

    if ( status.Int() != KErrNone )
        {
        LOGIT1("FAILED to get KDownlinkData from ConnMon <%d>", status.Int())
        }

    // Update SharedData downlink and uplink data
    UpdateSharedDataL( aConnectionId, uplinkData, downlinkData );

    TInt ret = KErrNone;
    // The actual status of connection is R_LOG_CON_CONNECTED, but
    // due to the performance reasons KConnectionStatusIdNotAvailable
    // is used here. There is no need to read LOGWRAP.RSC every time.
    ret = LogDataUpdateEventL(
            aConnectionId,
            KConnectionStatusIdNotAvailable,
            TUid::Null(),
            uplinkData,
            downlinkData );
    if ( ret != KErrNone )
        {
        LOGIT1("FAILED to update log data (timer mode) <%d>", ret)
        }

    // Check data volumes
    TUint threshold( iGPRSThreshold );

    ret = GetDefaultThreshold( aConnectionId, threshold );

    if ( ret != KErrNotFound )
        {
        // If the increase both in uplink and downlink data volume is less than threshold/2
        // switch the connection back to the threshold mode.
        if ( ( ( iConnections->At(index)->iPrevDownValue - prevDown ) < threshold/2 )
               &&
               ( ( iConnections->At(index)->iPrevUpValue - prevUp ) < threshold/2 ))
            {
            SwitchToThresholdMode( aConnectionId, threshold );

            // Return an error to stop the timer mode for this connection
            ret = KErrCancel;
            }
        else
            {
            ret = KErrNone;
            }
        }
    LOGEXITFN1("CEngine::LogDataInTimerModeL()", ret)
    return ret;
    }

// -----------------------------------------------------------------------------
// CEngine::SwitchToThresholdMode
// -----------------------------------------------------------------------------
//
void CEngine::SwitchToThresholdMode( const TUint aConnectionId, const TUint aThreshold  )
    {
    LOGENTRFN("CEngine::SwitchToThresholdMode()")
    // Set thresholds to start the threshold mode
    TUint threshold( aThreshold );

    if ( threshold == 0 )
        {
        TInt ret = GetDefaultThreshold( aConnectionId, threshold );

        if ( ret == KErrNotFound )
            {
            // Connection has closed
            return;
            }
        }

    iConnectionMonitor.SetUintAttribute(
            aConnectionId,
            KSubConnectionId,
            KDownlinkDataThreshold,
            threshold );

    iConnectionMonitor.SetUintAttribute(
            aConnectionId,
            KSubConnectionId,
            KUplinkDataThreshold,
            threshold );

    LOGIT1("Switched to threshold mode, id %d", aConnectionId)

    LOGEXITFN("CEngine::SwitchToThresholdMode()")
    }

// -----------------------------------------------------------------------------
// CEngine::SwitchToTimerMode
// -----------------------------------------------------------------------------
//
void CEngine::SwitchToTimerMode( const TUint aConnectionId )
    {
    LOGENTRFN("CEngine::SwitchToTimerMode()")
    if ( iDclTimerAO != 0 )
        {
        // Set thresholds to zero.
        iConnectionMonitor.SetUintAttribute(
                aConnectionId,
                KSubConnectionId,
                KDownlinkDataThreshold,
                0 );

        iConnectionMonitor.SetUintAttribute(
                aConnectionId,
                KSubConnectionId,
                KUplinkDataThreshold,
                0 );

        iDclTimerAO->Add( aConnectionId );

        LOGIT1("Switched to timer mode, id %d", aConnectionId)
        }
    LOGEXITFN("CEngine::SwitchToTimerMode()")
    }

// -----------------------------------------------------------------------------
// CEngine::GetDuration
// Retrieves iPrevDuration from connection in connection table with matching id.
// -----------------------------------------------------------------------------
//
TBool CEngine::GetDuration( const TUint aConnectionId, TInt& aDuration )
    {
    TInt index( 0 );

    for ( index = 0; index < iConnections->Count(); index++ )
        {
        TUint connId( iConnections->At( index )->iConnectionId );

        if ( aConnectionId == connId )
            {
            aDuration = iConnections->At( index )->iPrevDuration;
            return ETrue;
            }
        }

    LOGIT1("FAILED to get duration for connection id %d", aConnectionId)
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CEngine::GetDefaultThreshold
// -----------------------------------------------------------------------------
//
TInt CEngine::GetDefaultThreshold( const TUint aConnectionId, TUint& aThreshold )
    {
    LOGENTRFN("CEngine::GetDefaultThreshold()")
    TInt bearer( 0 );
    TRequestStatus status( 0 );

    // Initialize to the smallest value
    aThreshold = iGPRSThreshold;

    iConnectionMonitor.GetIntAttribute(
            aConnectionId,
            KSubConnectionId,
            KBearer,
            bearer,
            status );
    User::WaitForRequest( status );

    if ( status.Int() != KErrNotFound )
        {
        if ( bearer == EBearerWCDMA || bearer == EBearerExternalWCDMA )
            {
            aThreshold = iWCDMAThreshold;
            }
        else if ( bearer == EBearerWLAN || bearer == EBearerExternalWLAN )
            {
            aThreshold = iWLANThreshold;
            }
        else
            {
            aThreshold = iGPRSThreshold;
            }
        }

    LOGEXITFN1("CEngine::GetDefaultThreshold()", status.Int())
    return status.Int();
    }

// -----------------------------------------------------------------------------
// CEngine::ReadIniFile
// -----------------------------------------------------------------------------
//
void CEngine::ReadIniFile()
    {
    LOGENTRFN("CEngine::ReadIniFile()")
    TInt err( KErrNone );
    TInt value( 0 );

    // Initialize parameters to defaults because they might not be in SharedData.
    iTimerInterval  = KTimerInterval;
    iGPRSThreshold  = KGPRSDataThreshold;
    iWCDMAThreshold = KWCDMADataThreshold;
    iWLANThreshold  = KWLANDataThreshold;

    // Connecting and initialization:
    CRepository* repository = NULL;

    TRAPD( leaveCode, repository = CRepository::NewL( KCRUidDCL ) );

    if ( leaveCode )
        {
        LOGIT1("CRepository::NewL() FAILED: %d", leaveCode)
        // Go on using defaults
        return;
        }

    err = repository->Get( KDCLGprsThreshold, value );

    if ( err == KErrNone )
        {
        iGPRSThreshold = value;
        }
    else
        {
        LOGIT1("Reading KDCLGprsThreshold from CRepository  FAILED: %d", err)
        }

    err = repository->Get( KDCLWcdmaThreshold, value );

    if ( err == KErrNone )
        {
        iWCDMAThreshold = value;
        }
     else
        {
        LOGIT1("Reading KDCLWcdmaThreshold from CRepository  FAILED: %d", err)
        }

    err = repository->Get( KDCLWlanThreshold, value );

    if ( err == KErrNone )
        {
        iWLANThreshold = value;
        }
    else
        {
        LOGIT1("Reading KDCLWlanThreshold from CRepository  FAILED: %d", err)
        }

    err = repository->Get( KDCLTimerInterval, value );

    if ( err == KErrNone )
        {
        iTimerInterval = value;
        }
    else
        {
        LOGIT1("Reading KDCLTimerInterval from CRepository  FAILED: %d", err)
        }

    // Closing connection:
    delete repository;
    LOGEXITFN("CEngine::ReadIniFile()")
    }

// End of file
