/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM server implementation
*
*/

/**
@file mpmserver.cpp
Mobility Policy Manager server implementation.
*/

// INCLUDE FILES
#include <commdb.h>
#include <etel3rdparty.h>                // Voice call notification
#include <mmtsy_names.h>                 // KMmTsyModuleName
#include <centralrepository.h>
#include <es_sock_partner.h>
#include <CoreApplicationUIsSDKCRKeys.h>

#include "mpmserver.h"
#include "mpmserversession.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"
#include "mpmdtmwatcher.h"
#include "mpmroamingwatcher.h"
#include "mpmdisconnectdlg.h"
#include "mpmconfirmdlgroaming.h"
#include "mpmconfirmdlgstarting.h"
#include "mpmdefaultconnection.h"
#include "mpmcommsdataccess.h"
#include "mpmwlanquerydialog.h"
#include "mpmdialog.h"
#include "mpmprivatecrkeys.h"
#include "mpmcsidwatcher.h"
#include "mpmdatausagewatcher.h"
#include "mpmpropertydef.h"
#include "mpmofflinewatcher.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PanicServer 
// Panics the server in case of programming error.
// -----------------------------------------------------------------------------
//
void PanicServer( TInt aPanic )
    {
    User::Panic( KMPMPanicCategory, aPanic );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMServer::NewL
// -----------------------------------------------------------------------------
//
CMPMServer* CMPMServer::NewL()
    {
    CMPMServer* self = new ( ELeave ) CMPMServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMServer::CMPMServer
// -----------------------------------------------------------------------------
//
CMPMServer::CMPMServer()
    : CPolicyServer( CPolicyServer::EPriorityStandard, KMPMPolicy ),
      iEvents( NULL ), 
      iTSYLoaded( EFalse ),
      iPacketServLoaded( EFalse ), 
      iDtmWatcher( NULL ), 
      iWLANScanRequired( EFalse ), 
      iDisconnectQueue( NULL ), 
      iRoamingQueue( NULL ), 
      iStartingQueue( NULL ),
      iWlanQueryQueue( NULL ),
      iConnectDialogQueue( NULL ),
      iDefaultConnection( NULL ), 
      iConnectionCounter( 0 ),
      iOfflineMode( ECoreAppUIsNetworkConnectionAllowed )
    {
    }


// -----------------------------------------------------------------------------
// CMPMServer::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMServer::ConstructL()
    {
    // identify ourselves and open for service
    MPMLOGSTRING( "MPMServer is starting" )
    StartL( KMPMServerName );

    // Connect to ETel server
    // The RTelServer::Connect() might not always work with the first trial,
    // because of a coding error related to using semaphores in the method.
    TInt err( KErrNotReady );
    err = iTelServer.Connect();
    TInt a = 0;
    while( a < KPhoneRetryCount && err != KErrNone )
        {
        User::After( KPhoneRetryTimeout );
        err = iTelServer.Connect();
        a++;       
        }
    User::LeaveIfError( err );
       
    // Try to load phone.tsy
    TBuf< KCommsDbSvrMaxColumnNameLength > tsyName;
    tsyName.Copy( KMmTsyModuleName );
    err = iTelServer.LoadPhoneModule( tsyName );

    if ( err == KErrNone )
        {
        iTSYLoaded = ETrue;
        // Phone info can be retrieved with value 0 if there is only 1 phone
        RTelServer::TPhoneInfo info;
        User::LeaveIfError( iTelServer.GetPhoneInfo( 0, info ) );
        User::LeaveIfError( iMobilePhone.Open( iTelServer, info.iName ) );

        // Open packet service
        err = iPacketService.Open( iMobilePhone );
        if ( err == KErrNone )
            {
            iPacketServLoaded = ETrue;
            iDtmWatcher = CMPMDtmWatcher::NewL( iPacketService );
            }
#ifdef _DEBUG
        else
            {
            MPMLOGSTRING2( 
                "CMPMServer::ConstructL iPacketService.Open error: %d", err )  
            }
#endif // _DEBUG
        }
    
    iRoamingWatcher = CMPMRoamingWatcher::NewL(iMobilePhone);

    iCommsDatAccess = CMPMCommsDatAccess::NewL( );
    
    iDisconnectQueue = new ( ELeave ) CArrayPtrFlat<CMPMDisconnectDlg>( KGranularity );
    iDisconnectQueue->Reset();

    iRoamingQueue = new ( ELeave ) CArrayPtrFlat<CMPMConfirmDlgRoaming>( KGranularity );
    iRoamingQueue->Reset();

    iStartingQueue = new ( ELeave ) CArrayPtrFlat<CMPMConfirmDlgStarting>( KGranularity ); 
    iStartingQueue->Reset();

    iWlanQueryQueue = new ( ELeave ) CArrayPtrFlat<CMPMWlanQueryDialog>( KGranularity );
    iWlanQueryQueue->Reset();

    iConnectDialogQueue = new ( ELeave ) CArrayPtrFlat<CMPMDialog>( KGranularity );
    iConnectDialogQueue->Reset();
    
    iDefaultConnection = CMPMDefaultConnection::NewL( this );    

    // Create central repository watcher and start it
    iMpmCsIdWatcher = CMpmCsIdWatcher::NewL();
    iMpmCsIdWatcher->StartL();

    // Create another central repository watcher and start it
    TRAPD( duwErr, iMpmDataUsageWatcher = CMpmDataUsageWatcher::NewL( this ) );
    if (duwErr == KErrNone)
        {
        iMpmDataUsageWatcher->StartL();
        }
    else
        {
        iMpmDataUsageWatcher = NULL;
        MPMLOGSTRING( "CMPMServer::ConstructL: CMpmDataUsageWatcher::NewL() failed!" )
        }

    // Create another central repository watcher and start it
    TRAPD( owErr, iMpmOfflineWatcher = CMpmOfflineWatcher::NewL( this ) );
    if (owErr == KErrNone)
        {
        iMpmOfflineWatcher->StartL();
        }
    else
        {
        iMpmOfflineWatcher = NULL;
        MPMLOGSTRING( "CMPMServer::ConstructL: CMpmOfflineWatcher::NewL() failed!" )
        }

    // Define P&S keys (snap & iap) for the user connection
    TInt ret = RProperty::Define( KMPMUserConnectionCategory,
                                  KMPMPSKeyUserConnectionSnap,
                                  KMPMUserConnectionSnapType,
                                  KMPMUserConnectionReadPolicy,
                                  KMPMUserConnectionWritePolicy );
    
    if ( (ret != KErrNone) && (ret != KErrAlreadyExists) )
        {
        User::Leave(err);
        }
    
    ret = RProperty::Define( KMPMUserConnectionCategory,
                             KMPMPSKeyUserConnectionIap,
                             KMPMUserConnectionIapType,
                             KMPMUserConnectionReadPolicy,
                             KMPMUserConnectionWritePolicy );
                                          
    if ( (ret != KErrNone) && (ret != KErrAlreadyExists) )
        {
        User::Leave(err);
        }

    // Set initial values for the keys
    User::LeaveIfError(RProperty::Set( KMPMUserConnectionCategory,
                                       KMPMPSKeyUserConnectionSnap,
                                       0 ));

    User::LeaveIfError(RProperty::Set( KMPMUserConnectionCategory,
                                       KMPMPSKeyUserConnectionIap,
                                       0 ));

    // Read dedicated clients from the central repository
    CRepository* repository = CRepository::NewL( KCRUidMPM );
    
    CleanupStack::PushL( repository );
    
    TUint32 baseKey = KMpmDedicatedClientBase;
    TInt value;
    ret = KErrNone;
    
    while ( ret == KErrNone )
        {
        ret = repository->Get ( baseKey, value );

        if ( ret == KErrNone )
            {
            iDedicatedClients.AppendL( value );
            MPMLOGSTRING2( "CMPMServer::ConstructL: Dedicated client id: %d", value)
            }

        baseKey++;
        }
    
    CleanupStack::PopAndDestroy ( repository );
    
    // Define P&S keys for the connection dialogs
    ret = RProperty::Define( KMPMActiveConnectionCategory,
                             KMPMPSKeyActiveConnectionIap,
                             KMPMActiveConnectionIapType,
                             KMPMActiveConnectionReadPolicy,
                             KMPMActiveConnectionWritePolicy );
    
    if ( (ret != KErrNone) && (ret != KErrAlreadyExists) )
        {
        User::Leave(err);
        }
    
    ret = RProperty::Define( KMPMActiveConnectionCategory,
                             KMPMPSKeyActiveConnectionSnap,
                             KMPMActiveConnectionSnapType,
                             KMPMActiveConnectionReadPolicy,
                             KMPMActiveConnectionWritePolicy );
    
    if ( (ret != KErrNone) && (ret != KErrAlreadyExists) )
        {
        User::Leave(err);
        }

    ret = RProperty::Define( KMPMActiveConnectionCategory,
                             KMPMPSKeyActiveConnectionBearer,
                             KMPMActiveConnectionBearerType,
                             KMPMActiveConnectionReadPolicy,
                             KMPMActiveConnectionWritePolicy );
                                          
    if ( (ret != KErrNone) && (ret != KErrAlreadyExists) )
        {
        User::Leave(err);
        }
        
    PublishActiveConnection();
    }


// -----------------------------------------------------------------------------
// CMPMServer::~CMPMServer
// -----------------------------------------------------------------------------
//
CMPMServer::~CMPMServer()
    {
	if ( iRoamingToWlanPeriodic )
        {
        iRoamingToWlanPeriodic->Cancel();
		delete iRoamingToWlanPeriodic;
        }
    if ( iDisconnectQueue )
        {
        iDisconnectQueue->ResetAndDestroy();
        }
    delete iDisconnectQueue;

    if ( iRoamingQueue )
        {
        iRoamingQueue->ResetAndDestroy();
        }
    delete iRoamingQueue;

    if ( iStartingQueue )
        {
        iStartingQueue->ResetAndDestroy();
        }
    delete iStartingQueue;

    while ( iWlanQueryQueue && iWlanQueryQueue->Count() > 0 )
        {
        iWlanQueryQueue->Delete( 0 );
        }
    delete iWlanQueryQueue;
    
    if ( iConnectDialogQueue )
        {
        iConnectDialogQueue->ResetAndDestroy();
        }
    delete iConnectDialogQueue;

    delete iEvents;

    for ( TInt i = 0; i < iBlackListIdList.Count(); i++ )
        {
        iBlackListIdList[i].Close();
        }

    iActiveBMConns.Close();
    iSessions.Close();

    delete iDtmWatcher;
    delete iRoamingWatcher;

    // Close "global" ETEL objects
    if ( iPacketServLoaded )
        {
        iPacketService.Close();
        }    

    iMobilePhone.Close();
    iTelServer.Close();
    
    delete iDefaultConnection;
    
    delete iMpmCsIdWatcher;    
    
    delete iMpmDataUsageWatcher;    

    delete iMpmOfflineWatcher;    

    iDedicatedClients.Close();

    delete iCommsDatAccess;    
    }


// -----------------------------------------------------------------------------
// CMPMServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CMPMServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/) const
    {
    MPMLOGSTRING( "CMPMServer::NewSessionL" )
    // Check we're the right version
    if ( !User::QueryVersionSupported(TVersion(KMPMServerMajorVersionNumber,
                                               KMPMServerMinorVersionNumber,
                                               KMPMServerBuildVersionNumber ),
                                     aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    CSession2* session = CMPMServerSession::NewL( 
                             *const_cast<CMPMServer*>( this ));
    return session;
    }

// -----------------------------------------------------------------------------
// CMPMServer::PanicClient
// -----------------------------------------------------------------------------
//
void CMPMServer::PanicClient(
    const TInt aPanic) const
    {
    // let's have a look before we panic the client
    __DEBUGGER()
    // ok, go for it
    RThread clientThread;

    TInt err = Message().Client( clientThread );

    if ( err == KErrNone )
        {
        clientThread.Panic( KMPMServerName, aPanic );
        clientThread.Close();
        }
    }


// -----------------------------------------------------------------------------
// CMPMServer::AppendBMConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::AppendBMConnection( const TConnectionId aConnId, 
                                     const TUint32       aSnap,
                                     const TUint32       aIapId, 
                                     TConnectionState    aState,
                                     CMPMServerSession&  aSession )
    {
    MPMLOGSTRING3(
        "CMPMServer::AppendBMConnection - aConnId = 0x%x, aSnap = %i",
        aConnId, aSnap )

    // Set the Connection Id, SNAP, Iap Id and connection state
    // 
    TConnectionInfo connInfo;
    connInfo.iConnId = aConnId;
    connInfo.iSnap   = aSnap;
    connInfo.iIapId  = aIapId;
    connInfo.iState  = aState;
    connInfo.iAppUid = aSession.AppUid();

    // Package into TActiveBMConn //TODO Redundant.. remove the other one.
    // 
    TActiveBMConn conn;
    conn.iConnInfo          = connInfo;

    TInt index1 = iActiveBMConns.Find( conn, TActiveBMConn::MatchConnInfo );

    if ( index1 == KErrNotFound )
        {
        // If this connInfo is not yet there, insert it at the end of array
        //
        iActiveBMConns.Append( conn );
        }
    else
        {
        // connInfo already active, check whether Connection Id is there
        //
        if ( index1 < iActiveBMConns.Count() )
            {
            // Set the Iap Id and connection state
            // 
            iActiveBMConns[index1].iConnInfo.iIapId = aIapId;
            iActiveBMConns[index1].iConnInfo.iState = aState;
            iActiveBMConns[index1].iConnInfo.iAppUid = aSession.AppUid();
            }
        }

    if ( aState == EStarted && aSession.ChooseBestIapCalled() )
        {
        UpdateActiveConnection( aSession );
        }

#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    DumpActiveBMConns();
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMServer::ResetBMConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::ResetBMConnection( const TConnectionId aConnId,
                                    const TUint32       aIapId,
                                    CMPMServerSession&  aSession )
    {
    MPMLOGSTRING3(
        "CMPMServer::ResetBMConnection - aConnId = 0x%x, aIapId = %i", 
        aConnId, aIapId )

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 snap = GetBMSnap( aConnId );

    // Set the Connection Id and SNAP
    // 
    TConnectionInfo connInfo;
    connInfo.iConnId = aConnId;
    connInfo.iSnap   = snap;

    // Set the Iap Id 
    // 
    TActiveBMConn conn;
    conn.iConnInfo          = connInfo;
    conn.iConnInfo.iIapId   = aIapId;

    TInt index1 = iActiveBMConns.Find( conn, TActiveBMConn::MatchConnInfo );

    if ( ( index1 != KErrNotFound ) && ( index1 < iActiveBMConns.Count() ) )
//       && ( aIapId == iActiveBMConns[index1].iConnInfo.iIapId ) )
        {
        // If connInfo found, reset the Iap Id as zero and 
        // update connection state as EIdle.
        // 
        // It is expected that MPM keeps the Connection Id and SNAP 
        // relationship in a database entry and that this entry survives 
        // a roaming event (an ApplicationLeavesConnection call).
        //
        iActiveBMConns[index1].iConnInfo.iIapId = 0;
        iActiveBMConns[index1].iConnInfo.iState = EIdle;
        
        // Change state of P&S keys if needed
        if ( aSession.ChooseBestIapCalled() )
            {
            UpdateActiveConnection( aSession );
            }
        }
#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    DumpActiveBMConns();
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMServer::RemoveBMConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::RemoveBMConnection( const TConnectionId aConnId,
                                     CMPMServerSession&  aSession )
    {
    MPMLOGSTRING2( "CMPMServer::RemoveBMConnection - aConnId = 0x%x", 
        aConnId )

    TInt count = iActiveBMConns.Count();
    
    // Decrement by one, because count is n, 
    // but indexes in array are 0 .. n-1.
    // 
    count--;

    // This time we are browsing the array from the end to the beginning, 
    // because removing one element from array affects index numbering.
    // 
    for ( TInt i = count; i >= 0; i-- )
        {
        if ( iActiveBMConns[i].iConnInfo.iConnId == aConnId )
            {
            // If Connection Id found, remove it. 
            //
            iActiveBMConns.Remove( i );

            // Update active connection
            if ( aSession.ChooseBestIapCalled() )
                {
                UpdateActiveConnection( aSession );
                }
            }
        }

#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    DumpActiveBMConns();
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CMPMServer::GetBMIap
// -----------------------------------------------------------------------------
//
TUint32 CMPMServer::GetBMIap( const TConnectionId aConnId )
    {
    MPMLOGSTRING2( "CMPMServer::GetBMIap - aConnId = 0x%x", aConnId )

    TUint32 connectionIapId( 0 );

    // Set the Connection Id and SNAP
    // 
    TConnectionInfo connInfo;
    connInfo.iConnId = aConnId;

    TActiveBMConn conn;
    conn.iConnInfo = connInfo;

    TInt index1 = iActiveBMConns.Find( conn, TActiveBMConn::MatchConnInfo );

    if ( ( index1 != KErrNotFound ) && ( index1 < iActiveBMConns.Count() ) )
        {
        // If connInfo found, set the Iap Id as connectionIapId
        //
        connectionIapId = iActiveBMConns[index1].iConnInfo.iIapId;
        }

    MPMLOGSTRING2( "CMPMServer::GetBMIap - connectionIapId = %i", 
        connectionIapId )
    return connectionIapId;
    }


// -----------------------------------------------------------------------------
// CMPMServer::GetBMSnap
// -----------------------------------------------------------------------------
//
TUint32 CMPMServer::GetBMSnap( const TConnectionId aConnId )
    {
    MPMLOGSTRING2( "CMPMServer::GetBMSnap - aConnId = 0x%x", aConnId )

    TUint32 snap( 0 );
    TBool   snapFound( EFalse );

    for ( TInt i = 0; ( ( i < iActiveBMConns.Count() ) && !snapFound ); i++ )
        {
        if ( iActiveBMConns[i].iConnInfo.iConnId == aConnId )
            {
            snap = iActiveBMConns[i].iConnInfo.iSnap;
            snapFound = ETrue;
            }
        }
    MPMLOGSTRING2( "CMPMServer::GetBMSnap - SNAP = %i", snap )
    return snap;
    }

// -----------------------------------------------------------------------------
// CMPMServer::GetConnectionState
// -----------------------------------------------------------------------------
//
void CMPMServer::GetConnectionState( const TConnectionId    aConnId,
                                     TConnectionState&      aState )
    {
    MPMLOGSTRING2( "CMPMServer::GetConnectionState - aConnId = 0x%x", 
        aConnId )

    TConnectionInfo connInfo;
    connInfo.iConnId = aConnId; 

    TActiveBMConn conn;
    conn.iConnInfo = connInfo;

    TInt index1 = iActiveBMConns.Find( conn, TActiveBMConn::MatchConnInfo );

    if ( ( index1 != KErrNotFound ) && ( index1 < iActiveBMConns.Count() ) )
        {
        // If connInfo found
        //
        aState = iActiveBMConns[index1].iConnInfo.iState;
        }
    else
        {
        // Since connInfo has no entry in iActiveBMConns, then 
        // the state of connection must be EStarting.
        // 
        aState = EStarting;
        }

#ifdef _DEBUG
    switch( aState )
        {
        case EStarting:
            {
            MPMLOGSTRING( "CMPMServer::GetConnectionState - Starting" )
            break;
            }
        case EStarted:
            {
            MPMLOGSTRING( "CMPMServer::GetConnectionState - Started" )
            break;
            }
        case EIdle:
            {
            MPMLOGSTRING( "CMPMServer::GetConnectionState - Idle" )
            break;
            }
        case ERoaming:
            {
            MPMLOGSTRING( "CMPMServer::GetConnectionState - Roaming" )
            break;
            }
        default:
            {
            MPMLOGSTRING( "CMPMServer::GetConnectionState - Unknown" )
            break;
            }
        }
#endif // _DEBUG
    }

// -----------------------------------------------------------------------------
// CMPMServer::CheckUsageOfIap
// -----------------------------------------------------------------------------
//
TConnectionState CMPMServer::CheckUsageOfIap( const TUint32 aIapId , 
                                  const TConnectionId aOwnConnId )
    {
    MPMLOGSTRING3( "CMPMServer::CheckUsageOfIap - aIapId = %i, aOwnConnId = 0x%x", 
                  aIapId, aOwnConnId )

    TConnectionState state = EIdle;

    // Loop all connections until EStarted is found or no more connections
    // 
    for ( TInt i = 0; i < iActiveBMConns.Count(); i++ )
        {
        // Check if IAP Id matches; exclude matching with own connection
        // 
        if ( iActiveBMConns[i].iConnInfo.iIapId == aIapId && 
             iActiveBMConns[i].iConnInfo.iConnId != aOwnConnId)
            {
            state = iActiveBMConns[i].iConnInfo.iState;  

            // Stop looping if EStarted found
            // 
            if ( state == EStarted )
                {
                break; // Breaks the for loop
                }
            }
        }

#ifdef _DEBUG
    switch( state )
        {
        case EStarting:
            {
            MPMLOGSTRING( "CMPMServer::CheckUsageOfIap - Starting" )
            break;
            }
        case EStarted:
            {
            MPMLOGSTRING( "CMPMServer::CheckUsageOfIap - Started" )
            break;
            }
        case EIdle:
            {
            MPMLOGSTRING( "CMPMServer::CheckUsageOfIap - Idle" )
            break;
            }
        case ERoaming:
            {
            MPMLOGSTRING( "CMPMServer::CheckUsageOfIap - Roaming" )
            break;
            }
        default:
            {
            MPMLOGSTRING( "CMPMServer::CheckUsageOfIap - Unknown" )
            break;
            }
        }
#endif // _DEBUG
        
    return state;
    }

// -----------------------------------------------------------------------------
// CMPMServer::IsWlanConnectionStartedL
// -----------------------------------------------------------------------------
//
TUint32 CMPMServer::IsWlanConnectionStartedL( const CMPMCommsDatAccess* aCdbAccess )
    {
    TUint32 iapId( 0 );
    
    MPMLOGSTRING( "CMPMServer::IsWlanConnectionStartedL" )

    TBool stopLoop( EFalse );

    // Loop all connections until EStarted is found or no more connections
    // 
    for ( TInt i = 0; ( ( i < iActiveBMConns.Count() ) && !stopLoop ); i++ )
        {
        // Check if IAP Id matches
        // 
        if ( iActiveBMConns[i].iConnInfo.iState == EStarting ||
             iActiveBMConns[i].iConnInfo.iState == EStarted )
            {
            if ( aCdbAccess->CheckWlanL( iActiveBMConns[i].iConnInfo.iIapId ) != ENotWlanIap )
                {
                stopLoop = ETrue;
                iapId = iActiveBMConns[i].iConnInfo.iIapId;
                MPMLOGSTRING2( "CMPMServer::IsWlanConnectionStartedL, found wlan iap %d", iapId )
                }
            }
        }

    
    return iapId;
    }

// -----------------------------------------------------------------------------
// CMPMServer::AppendBMIAPConnectionL
// -----------------------------------------------------------------------------
//
void CMPMServer::AppendBMIAPConnectionL( const TUint32       aIapId, 
                                         const TConnectionId aConnId,
                                         CMPMServerSession& aSession )
    {
    MPMLOGSTRING3(
        "CMPMServer::AppendBMIAPConnectionL - aIapId = %i, aConnId = 0x%x", 
        aIapId, aConnId )

    TActiveBMConn conn;
    conn.iConnInfo.iIapId = aIapId;
    conn.iConnInfo.iState = EStarted;
    conn.iConnInfo.iAppUid = aSession.AppUid();
    
    TInt index1 = iActiveBMConns.Find( conn, TActiveBMConn::MatchIapId );

    if ( index1 == KErrNotFound )
        {
        // If this Iap is not yet there, insert it at the end of array
        //
        iActiveBMConns.AppendL( conn );
        }
    else
        {
        // Iap already active, update connection state as started. 
        // This Iap could be shared by multiple applications, but 
        // update only the one with matching connection id.
        //
        for ( TInt i = 0; i < iActiveBMConns.Count(); i++ )
            {
            if ( iActiveBMConns[i].iConnInfo.iConnId == aConnId )
                {
                iActiveBMConns[i].iConnInfo.iState = EStarted;
                }
            else if ( iActiveBMConns[i].iConnInfo.iSnap == 0 &&
                    iActiveBMConns[i].iConnInfo.iIapId == aIapId )
                {
                iActiveBMConns[i].iConnInfo.iState = EStarted;
                }
            }
        }

    if ( aSession.ChooseBestIapCalled() )
        {
        UpdateActiveConnection( aSession );
        }

#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    DumpActiveBMConns();
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMServer::RemoveBMIAPConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::RemoveBMIAPConnection( const TUint32       aIapId, 
                                        const TConnectionId aConnId,
                                        CMPMServerSession&  aSession )
    {
    MPMLOGSTRING3(
        "CMPMServer::RemoveBMIAPConnection - aIapId = %i, aConnId = 0x%x", 
        aIapId, aConnId )

    TActiveBMConn conn;
    conn.iConnInfo.iIapId = aIapId;

    // The IAP connection lifetime is determined by the two calls 
    // IAPConnectionStarted and IAPConnectionStopped. 
    //
    TInt count = iActiveBMConns.Count();

    // Decrement by one, because count is n, 
    // but indexes in array are 0 .. n-1.
    // 
    count--;

    // This time we are browsing the array from the end to the beginning, 
    // because removing one element from array affects index numbering.
    // 
    for ( TInt i = count; i >= 0; i-- )
        {
        if ( iActiveBMConns[i].iConnInfo.iIapId == aIapId )
            {
            if ( iActiveBMConns[i].iConnInfo.iSnap == 0 )
                {
                // If IAPConnectionStopped has been called and SNAP is zero,
                // then this entry can be removed from database.
                // 
                iActiveBMConns.Remove( i );
                }
            else
                {
                // If IAP found, reset the Iap Id as zero and 
                // update connection state as idle.
                // This Iap could be shared by multiple applications, 
                // but update only the one with matching connection id.
                // 
                if ( iActiveBMConns[i].iConnInfo.iConnId == aConnId )
                    {
                    iActiveBMConns[i].iConnInfo.iIapId = 0;
                    iActiveBMConns[i].iConnInfo.iState = EIdle;
                    }
                }
            
            // Update active connection
            if ( aSession.ChooseBestIapCalled() )
                {
                UpdateActiveConnection( aSession );
                }
            }
        }

#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    DumpActiveBMConns();
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMServer::AppendSessionL
// -----------------------------------------------------------------------------
//
void CMPMServer::AppendSessionL( const CMPMServerSession* aSession )
    {
    MPMLOGSTRING( "CMPMServer::AppendSession" )
    
    iSessions.AppendL( aSession );
    }


// -----------------------------------------------------------------------------
// CMPMServer::RemoveSession
// -----------------------------------------------------------------------------
//
void CMPMServer::RemoveSession( const CMPMServerSession* aSession )
    {
    MPMLOGSTRING( "CMPMServer::RemoveSession" )

    TInt index = iSessions.Find( aSession );
    if ( index != KErrNotFound )
        {
        if ( Events() )
            {
            // Cancel WLAN scan request if one exists
            TRAP_IGNORE( Events()->CancelScanL( iSessions[index] ) )
            }
        iSessions.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::NotifyBMPrefIapL
// -----------------------------------------------------------------------------
//
void CMPMServer::NotifyBMPrefIapL( const TConnMonIapInfo& aIapInfo, 
                                   const TPrefIAPNotifCaller aCaller )
    {
    MPMLOGSTRING2( "CMPMServer::NotifyBMPrefIapL - IAPs count: %d", 
        aIapInfo.iCount)
    TConnMonIapInfo iapInfo = aIapInfo;
    
#ifdef _DEBUG
    for (TUint i = 0; i < iapInfo.Count(); i++)
        {
        MPMLOGSTRING2( "CMPMServer::NotifyBMPrefIap - IAP: %d", 
            iapInfo.iIap[i].iIapId) 
        }
#endif // _DEBUG

    // Start possible forced roaming
    TCmUsageOfWlan usageOfWlan = CommsDatAccess()->ForcedRoamingL();
    if ( usageOfWlan == ECmUsageOfWlanKnown || usageOfWlan == ECmUsageOfWlanKnownAndNew )
        {
        if ( IsWlanConnectionStartedL( CommsDatAccess() ) )
            {
            iConnMonIapInfo = aIapInfo;
                
            if ( iRoamingToWlanPeriodic )
                {
                iRoamingToWlanPeriodic->Cancel();
                }
            else
                {
                iRoamingToWlanPeriodic = CPeriodic::NewL( 
                                                   CActive::EPriorityStandard );
                }
            // start periodic object that calls StartForcedRoamingToWlanL after 10s. 
            // this handles the case when new wlan connection is 
            // started from e.g. wlan sniffer but IAP is not yet in Internet SNAP 
            iRoamingToWlanPeriodic->Start( 
                   TTimeIntervalMicroSeconds32( KRoamingToWlanUpdateInterval ), 
                   TTimeIntervalMicroSeconds32( KRoamingToWlanUpdateInterval ), 
                   TCallBack( StartForcedRoamingToConnectedWlanL, this ) );
            }
        else
            {
            StartForcedRoamingToWlanL( iapInfo );
            }

        StartForcedRoamingFromWlanL( iapInfo );
        }
    
    MPMLOGSTRING2(
    "CMPMServer::NotifyBMPrefIapL - Send notifications for %d sessions", 
        iSessions.Count() )

    for ( TInt i = 0; i < iSessions.Count(); i++ )
        {
        iapInfo = iSessions[i]->GetAvailableIAPs( );
        iSessions[i]->PrefIAPNotificationL( iapInfo, aCaller );
        }

    // If a session is displaying connection selection dialog 
    // the contents of the dialog should be updated according to the 
    // current iap availability
    //
    for ( TInt i( 0 ); i < iSessions.Count(); i++ )
        {
        iSessions[i]->UpdateConnectionDialogL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::UpdateSessionConnectionDlgL
// -----------------------------------------------------------------------------
//
void CMPMServer::UpdateSessionConnectionDlgL()
    {
    MPMLOGSTRING( "CMPMServer::UpdateSessionConnectionDlgL" )

    // If a session is displaying connection selection dialog 
    // the contents of the dialog should be updated according to the 
    // current iap availability
    //
    for ( TInt i( 0 ); i < iSessions.Count(); i++ )
        {
        iSessions[i]->UpdateConnectionDialogL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::HandleServerBlackListIap
// -----------------------------------------------------------------------------
//
TInt CMPMServer::HandleServerBlackListIap( const TConnectionId  aConnId,
                                           TUint32              aIapId, 
                                           TBlacklistCategory   aCategory )
    {
    MPMLOGSTRING3(
        "CMPMServer::HandleServerBlackListIap - aConnId = 0x%x, iapId = %i",
        aConnId, aIapId )

    MPMLOGSTRING2(
        "CMPMServer::HandleServerBlackListIap - aCategory = %i", aCategory )

    BlackListIap( aConnId, aIapId, aCategory ); 

    TUint32 presumedIap = Events()->PresumedIapId( aConnId, 
                                                   aIapId );
    if ( ( presumedIap != 0 ) && 
         ( presumedIap != aIapId ) )
        {
        MPMLOGSTRING2(
            "CMPMServer::HandleServerBlackListIap - presumedIap = %i",
            presumedIap )
        BlackListIap( aConnId, presumedIap, aCategory ); 
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMServer::BlackListIap
// -----------------------------------------------------------------------------
//
TInt CMPMServer::BlackListIap( const TConnectionId  aConnId,
                               TUint32              aIapId, 
                               TBlacklistCategory   aCategory )
    {
    TInt i;
    TBool found = EFalse;

    found = FindBlacklistedConnIndex( aConnId, i );
    if ( found )
        {
        TMPMBlackListConnId connIdInfo = iBlackListIdList[i];
        connIdInfo.Append( aIapId, aCategory );
        iBlackListIdList.Remove( i );
        iBlackListIdList.Insert( connIdInfo, 0 );
        }
    else
        {
        TMPMBlackListConnId connIdInfo;
        connIdInfo.iConnId = aConnId;
        connIdInfo.Append( aIapId, aCategory );
        iBlackListIdList.Insert( connIdInfo, 0 );
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMServer::HandleServerUnblackListIap
// -----------------------------------------------------------------------------
//
TInt CMPMServer::HandleServerUnblackListIap( 
    const TConnectionId aConnId,
    TUint32             aIapId )
    {
    MPMLOGSTRING3(
        "CMPMServer::HandleServerUnblackListIap - aConnId = 0x%x, iapId = %i"
        ,aConnId, aIapId )

    TInt i;
    TBool found = EFalse;

    found = FindBlacklistedConnIndex( aConnId, i );
    if ( found )
        {
        // found blacklisted Connection Id
        TMPMBlackListConnId connIdInfo = iBlackListIdList[i];
        iBlackListIdList.Remove( i ); // remove from the list 
        
        MPMLOGSTRING2( "CMPMServer::HandleServerUnblackListIap - \
connIdInfo count: %d", connIdInfo.Count() )

        if ( aIapId == 0 )
            { // 0 will reset Connection Id blacklisted iap list 
            MPMLOGSTRING( "CMPMServer::HandleServerUnblackListIap - \
reset Connection Id blacklisted iap list" )

            connIdInfo.Close();
            return KErrNone;
            }
        
        found = EFalse;
        for (TInt j = 0; j < connIdInfo.Count(); j++)
            {
            if ( connIdInfo.Iap( j ) == aIapId )
                {
                // found and remove blacklisted iap
                connIdInfo.Remove( j ); 
                MPMLOGSTRING2( "CMPMServer::HandleServerUnblackListIap - \
removed blacklisted iap in index = %d", j )
                
                if ( connIdInfo.Count() == 0 )
                    {
                    return KErrNone;
                    }

                // reinsert connIdInfo at the beginning to reflect activeness
                iBlackListIdList.Insert( connIdInfo, 0 ); 
                return KErrNone;
                }
            }
        // nothing found and reinsert at the beginning 
        // connIdInfo to reflect activeness
        iBlackListIdList.Insert( connIdInfo, 0 ); 
        return KErrNotFound;
        }
    else
        {
        MPMLOGSTRING( "CMPMServer::HandleServerUnblackListIap - \
not found blacklisted Connection Id" )
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::HandleServerUnblackListIap
// -----------------------------------------------------------------------------
//
void CMPMServer::HandleServerUnblackListIap( 
    TBlacklistCategory  aCategory )
    {
    MPMLOGSTRING3( "CMPMServer::HandleServerUnblackListIap -\
aCategory = %i blacklisted Id count = %d", 
                   aCategory, iBlackListIdList.Count() )

    for( TInt i( 0 ); i < iBlackListIdList.Count(); i++ )
        {
        // found blacklisted Connection Id
        TMPMBlackListConnId connIdInfo = iBlackListIdList[i];
        iBlackListIdList.Remove( i ); // remove from the list 

        MPMLOGSTRING3( "CMPMServer::HandleServerUnblackListIap - \
aConnId = 0x%x, blacklisted IapId count = %d", connIdInfo.iConnId, 
        connIdInfo.Count() )
        
        for (TInt j = 0; j < connIdInfo.Count(); j++)
            {
            if ( connIdInfo.Category( j ) == aCategory ) 
                {
                // found and remove blacklisted iap
                MPMLOGSTRING3( "CMPMServer::HandleServerUnblackListIap - \
removed blacklisted iap id %i in index: %d", connIdInfo.Iap( j ), j )
                connIdInfo.Remove( j ); 
                }
            }
        // If any blacklisted iaps remain reinsert at the 
        // beginning connIdInfo to reflect activeness
        //
        if( connIdInfo.Count() > 0 )
            {
            MPMLOGSTRING( "reinsert connIdInfo to reflect activeness" )
            iBlackListIdList.Insert( connIdInfo, 0 );             
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::GetBlacklistedIAP
// -----------------------------------------------------------------------------
//
TInt CMPMServer::GetBlacklistedIAP( TConnectionId    aConnId, 
                                    RArray<TUint32> &aBlacklistedIAP )
    {
    TInt  i;
    TBool found = EFalse;

    found = FindBlacklistedConnIndex( aConnId, i );
    if ( !found )
        {
        return KErrNotFound;
        }

    TMPMBlackListConnId connIdInfo = iBlackListIdList[i];
    iBlackListIdList.Remove( i );
    iBlackListIdList.Insert( connIdInfo, 0 );

    for (TInt j = 0; j < connIdInfo.Count(); j++)
        {
        aBlacklistedIAP.Append( connIdInfo.Iap( j ) );
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMServer::GetBlacklistedIAP
// -----------------------------------------------------------------------------
//
TInt CMPMServer::GetBlacklistedIAP( RArray<TUint32> &aBlacklistedIAP )
    {
    // Returns all blacklisted IAPs regardless of Connection Id 
    // 
    for ( TInt i( 0 ); i < iBlackListIdList.Count(); i++ )
        {
        for ( TInt j( 0 ); j < iBlackListIdList[i].Count(); 
              j++ )
            {
            // Inserts an object into the array in ascending unsigned 
            // key order. No duplicate entries are permitted. 
            // 
            // The array remains unchanged following an attempt to 
            // insert a duplicate entry.
            // 
            aBlacklistedIAP.InsertInUnsignedKeyOrder( 
                                    iBlackListIdList[i].Iap( j ) );
            }
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMServer::FindId
// -----------------------------------------------------------------------------
//
TBool CMPMServer::FindBlacklistedConnIndex( const TConnectionId aConnId, 
                                            TInt                &aIndex )
    {
    TInt  i;
    TBool found = EFalse;

    for (i = 0;( (i < iBlackListIdList.Count()) && !found ); i++)
        {
        if ( iBlackListIdList[i].iConnId == aConnId )
            {
            found = ETrue;
            }
        }
    i--; // Since i is incremented after finding the correct iConnId
    aIndex = i;
    return found;
    }


// -----------------------------------------------------------------------------
// CMPMServer::IsVoiceCallActiveL
// 
// Checks if voice call is active or not.
// Return ETrue if voice call is active.
// Return EFalse if voice call is not active.
// -----------------------------------------------------------------------------
//
TBool CMPMServer::IsVoiceCallActiveL() const
    {
    MPMLOGSTRING( "CMPMServer::IsVoiceCallActiveL" )

    CTelephony*                   telephony = CTelephony::NewLC();
    CTelephony::TCallStatusV1     callStatusV1;
    CTelephony::TCallStatusV1Pckg callStatusV1Pckg( callStatusV1 );
    CTelephony::TPhoneLine        line = CTelephony::EVoiceLine;

    telephony->GetLineStatus( line, callStatusV1Pckg );
    CTelephony::TCallStatus voiceLineStatus = callStatusV1.iStatus;

    CleanupStack::PopAndDestroy( telephony );

    if ( voiceLineStatus == CTelephony::EStatusDialling         ||
         voiceLineStatus == CTelephony::EStatusRinging          ||
         voiceLineStatus == CTelephony::EStatusAnswering        ||
         voiceLineStatus == CTelephony::EStatusConnecting       ||
         voiceLineStatus == CTelephony::EStatusConnected        ||
         voiceLineStatus == CTelephony::EStatusReconnectPending ||
         voiceLineStatus == CTelephony::EStatusDisconnecting    ||
         voiceLineStatus == CTelephony::EStatusHold             ||
         voiceLineStatus == CTelephony::EStatusTransferring     ||
         voiceLineStatus == CTelephony::EStatusTransferAlerting  )
        {
        MPMLOGSTRING2(
            "CMPMServer::IsVoiceCallActiveL Voice call is active: %d", 
            voiceLineStatus )
        return ETrue;
        }
    else if ( voiceLineStatus == CTelephony::EStatusIdle ||
              voiceLineStatus == CTelephony::EStatusUnknown )
        {
        MPMLOGSTRING2(
            "CMPMServer::IsVoiceCallActiveL Voice call is not active: %d", 
            voiceLineStatus )
        return EFalse;
        }
    else
        {
        MPMLOGSTRING2(
            "CMPMServer::IsVoiceCallActiveL Unknown voice line status: %d", 
            voiceLineStatus )
        return EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CMPMServer::IsModeGSM
// 
// Checks if mode is GSM or not.
// Return ETrue if mode is GSM.
// Return EFalse if mode is something else.
// -----------------------------------------------------------------------------
//
TBool CMPMServer::IsModeGSM() const
    {
    MPMLOGSTRING( "CMPMServer::IsModeGSM" )

    if ( iTSYLoaded )
        {
        RMobilePhone::TMobilePhoneNetworkMode mode( 
            RMobilePhone::ENetworkModeUnknown );

        TInt ret = iMobilePhone.GetCurrentMode( mode );

        if ( ( ret == KErrNone ) && 
             ( mode == RMobilePhone::ENetworkModeGsm ) )
            {
            MPMLOGSTRING( "CMPMServer::IsModeGSM Mode is GSM" )
            return ETrue;
            }
        else
            {
            MPMLOGSTRING( "CMPMServer::IsModeGSM Mode is not GSM" )
            return EFalse;
            }
        }
    MPMLOGSTRING( "CMPMServer::IsModeGSM phone.tsy not loaded" )
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CMPMServer::IsDTMSupported
// 
// Checks if phone supports Dual Transfer Mode or not.
// Return ETrue if phone supports DTM.
// Return EFalse if phone does not support DTM.
// -----------------------------------------------------------------------------
//
TBool CMPMServer::IsDTMSupported() const
    {
    MPMLOGSTRING( "CMPMServer::IsDTMSupported" )

    if ( iPacketServLoaded )
        {
        TBool rv = iDtmWatcher->IsInDualMode();
        if ( rv )
            {
            MPMLOGSTRING( "CMPMServer::IsDTMSupported DTM is supported" )
            }
        else
            {
            MPMLOGSTRING( "CMPMServer::IsDTMSupported DTM is not supported" )
            }
        return rv;
        }
 
    MPMLOGSTRING( "CMPMServer::IsDTMSupported Packet service not loaded" )
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CMPMServer::DumpActiveBMConns
// -----------------------------------------------------------------------------
//
void CMPMServer::DumpActiveBMConns()
    {
#ifdef _DEBUG
    // Dump array of active connections to log in order to support testing.
    // 
    MPMLOGSTRING( "Display array of active connections - Start" )
    MPMLOGSTRING( "" )

    if ( iActiveBMConns.Count() == 0 )
        {
        MPMLOGSTRING( "Array of active connections is empty" )
        MPMLOGSTRING( "" )
        }

    for ( TInt i = 0; i < iActiveBMConns.Count(); i++ )
        {
        MPMLOGSTRING3( "Connection Id = 0x%x Snap = %i", 
            iActiveBMConns[i].iConnInfo.iConnId, 
            iActiveBMConns[i].iConnInfo.iSnap )

        switch( iActiveBMConns[i].iConnInfo.iState )
            {
            case EStarting:
                {
                MPMLOGSTRING2( "IAP %i: Connection state = Starting", 
                    iActiveBMConns[i].iConnInfo.iIapId )
                break;
                }
            case EStarted:
                {
                MPMLOGSTRING2( "IAP %i: Connection state = Started", 
                    iActiveBMConns[i].iConnInfo.iIapId )
                break;
                }
            case EIdle:
                {
                MPMLOGSTRING2( "IAP %i: Connection state = Idle", 
                    iActiveBMConns[i].iConnInfo.iIapId )
                break;
                }
            case ERoaming:
                {
                MPMLOGSTRING2( "IAP %i: Connection state = Roaming", 
                    iActiveBMConns[i].iConnInfo.iIapId )
                break;
                }
            default:
                {
                MPMLOGSTRING2( "IAP %i: Unknown connection state", 
                    iActiveBMConns[i].iConnInfo.iIapId )
                break;
                }
            }

        MPMLOGSTRING( "" )
        }

    MPMLOGSTRING( "Display array of active connections - End" )
#endif // _DEBUG
    }


// -----------------------------------------------------------------------------
// CMPMServer::DefaultConnection
// -----------------------------------------------------------------------------
//
CMPMDefaultConnection* CMPMServer::DefaultConnection()
    {
    MPMLOGSTRING( "CMPMServer::DefaultConnection:\
 Default Connection" )
    return iDefaultConnection;
    }

// -----------------------------------------------------------------------------
// CMPMServer::StartedConnectionExists
// -----------------------------------------------------------------------------
//
TInt CMPMServer::StartedConnectionExists( TInt aIapId )
    {
    MPMLOGSTRING( "CMPMServer::StartedConnectionExists" )

    // Loop all connections until EStarted is found or no more connections
    // 
    for ( TInt i = 0; ( i < iActiveBMConns.Count() ); i++ )
        {
        if ( iActiveBMConns[i].iConnInfo.iState == EStarted &&
                ( aIapId == KErrNotFound || aIapId == iActiveBMConns[i].iConnInfo.iIapId ) )
            {
            MPMLOGSTRING( "CMPMServer::StartedConnectionExists: True" )
            return iActiveBMConns[i].iConnInfo.iIapId;
            }
        }
    // Modem connection may exist, check from connection counter
    // 
    if ( ConnectionCounter() > 0 )
        {
        MPMLOGSTRING( "CMPMServer::StartedConnectionExists: True (modem connection)" )
        return KMaxTInt; // arbitrary high number that is NOT from Iap range[0:255] 
        }
    else
        {
        MPMLOGSTRING( "CMPMServer::StartedConnectionExists: False" )
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::AppendWlanQueryQueueL
// -----------------------------------------------------------------------------
//
void CMPMServer::AppendWlanQueryQueueL( CMPMWlanQueryDialog* aDlg )
    {
    if( aDlg )
        {
        iWlanQueryQueue->AppendL( aDlg );
        }
    else
        {
        MPMLOGSTRING( "CMPMServer::AppendWlanQueryQueueL argument NULL, Error" )
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::StopConnections
// -----------------------------------------------------------------------------
//
void CMPMServer::StopConnections( TInt aIapId )
    {
    MPMLOGSTRING2( "CMPMServer::StopConnections aIapId = %d", aIapId )
    for (TInt index = 0; index < iSessions.Count(); index++)
        {
        CMPMServerSession* session = iSessions[index];
        // Stop connection
        if ( aIapId == 0 )
            {
            session->StopConnection();
            }
        else if ( GetBMIap( session->ConnectionId() ) == aIapId )
            {
            TRAP_IGNORE( session->StopIAPNotificationL( aIapId ));
            }     
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::UpdateActiveConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::UpdateActiveConnection( CMPMServerSession& aSession )
    {
    // Update active connection
    TInt ret = KErrNone;
            
    TRAP ( ret, UpdateActiveConnectionL( aSession ) );
            
    if ( ret != KErrNone )
        {
        iActiveBearerType = EMPMBearerTypeNone;
        iActiveIapId = 0;
        iActiveSnapId = 0;            
        }   
    }

// -----------------------------------------------------------------------------
// CMPMServer::UpdateActiveConnectionL
// -----------------------------------------------------------------------------
//
void CMPMServer::UpdateActiveConnectionL( CMPMServerSession& aSession )
    {
    MPMLOGSTRING( "CMPMServer::UpdateActiveConnectionL" )

    TBool keysUpToDate( ETrue );
    
    // number of active iaps
    TInt numberOfActive = NumberOfActiveConnections( keysUpToDate );
    
    if ( numberOfActive == 0 )
        {
        // If no active connections then just reset keys and publish
        iActiveBearerType = EMPMBearerTypeNone;
        iActiveIapId = 0;
        iActiveSnapId = 0;
        PublishActiveConnection();
        return;
        }
        
    if ( keysUpToDate )
        {
        MPMLOGSTRING( "CMPMServer::UpdateActiveConnectionL: already up-to-date" )
        return;   
        }

    if ( numberOfActive > 1 )
        {
        // Check if all active connections are in same snap
        TUint32 snapId;

        if ( CommsDatAccess()->AreActiveIapsInSameSnapL(
             iActiveBMConns, snapId, *this ) )
            {
            MPMLOGSTRING( "CMPMServer::UpdateActiveConnectionL: Active iaps are in one SNAP" )
            // Select active connection according to priority
            CommsDatAccess()->SelectActiveConnectionL (
                snapId,
                iActiveBMConns,
                iActiveIapId,
                iActiveSnapId,
                iActiveBearerType,
                aSession );

            PublishActiveConnection();
            return;
            }
        
        MPMLOGSTRING( "CMPMServer::UpdateActiveConnectionL: Active iaps are in different SNAPs" )
        }

    // Reset active connections
    iActiveBearerType = EMPMBearerTypeNone;
    iActiveIapId = 0;
    iActiveSnapId = 0;
                
    // Active connections locating in different snaps
    // Use priority order vpn, wlan and packet
    for ( TInt index = 0; index < iActiveBMConns.Count(); index++ )
        {
        CMPMServerSession* serverSession = GetServerSession(
            iActiveBMConns[index].iConnInfo.iConnId );
                     	
        // Do check only for active connections
        if ( iActiveBMConns[index].iConnInfo.iState == EStarted &&
        	   serverSession->ChooseBestIapCalled() )
            {
            TMPMBearerType bearerType = EMPMBearerTypeOther;
        
            TInt ret = iDedicatedClients.Find( iActiveBMConns[index].iConnInfo.iAppUid );
            if ( ret != KErrNotFound )
                {
                // Skip dedicated client
                MPMLOGSTRING2( "Skip dedicated client = %d", iActiveBMConns[index].iConnInfo.iAppUid )
                continue;
                }

            bearerType = CommsDatAccess()->GetBearerTypeL( 
                         iActiveBMConns[index].iConnInfo.iIapId );
        
            if ( bearerType == EMPMBearerTypeOther )
                {
                // Don't publish this connection
                continue;
                }
                    
            // This is true if,
            // bearer type is smaller or different than none
            // or
            // bearer type is same and iap is different.
            if ( ( bearerType < iActiveBearerType ) || 
                 ( iActiveBearerType == EMPMBearerTypeNone ) ||
                 ( ( bearerType == iActiveBearerType) &&
                   ( iActiveIapId != iActiveBMConns[index].iConnInfo.iIapId ) ) )
                {
                iActiveBearerType = bearerType;
                iActiveIapId = iActiveBMConns[index].iConnInfo.iIapId;
                iActiveSnapId = iActiveBMConns[index].iConnInfo.iSnap;
                }
            }
        }
    PublishActiveConnection();
    }

// -----------------------------------------------------------------------------
// CMPMServer::MapBearerType
// -----------------------------------------------------------------------------
//
TUint32 CMPMServer::MapBearerType(TMPMBearerType aBearerType)
    {
    MPMLOGSTRING( "CMPMServer::MapBearerType" )
    
    switch ( aBearerType )
        {
        case EMPMBearerTypeNone:
            return 0;

        case EMPMBearerTypeVpn:
            return KCommDbBearerVirtual;
            
        case EMPMBearerTypeWlan:
            return KCommDbBearerWLAN;
            
        case EMPMBearerTypePacketData:
            return KCommDbBearerWcdma;

        case EMPMBearerTypeOther:
            return KCommDbBearerUnknown;
            
        default:
            MPMLOGSTRING( "CMPMServer::MapBearerType: Unknown bearer type" )
            break;
        }
    
    return KCommDbBearerUnknown;
    }

// -----------------------------------------------------------------------------
// CMPMServer::PublishActiveConnection
// -----------------------------------------------------------------------------
//
void CMPMServer::PublishActiveConnection()
    {
    MPMLOGSTRING( "CMPMServer::PublishActiveConnection" )
    
    // update active connection keys
    RProperty::Set( KMPMActiveConnectionCategory,
                    KMPMPSKeyActiveConnectionIap,
                    iActiveIapId );

    RProperty::Set( KMPMActiveConnectionCategory,
                    KMPMPSKeyActiveConnectionSnap,
                    iActiveSnapId );

    RProperty::Set( KMPMActiveConnectionCategory,
                    KMPMPSKeyActiveConnectionBearer,
                    MapBearerType( iActiveBearerType ) );    

    MPMLOGSTRING4( "CMPMServer::PublishActiveConnection: Set to: %d, %d, %d",
                   iActiveIapId, iActiveSnapId, iActiveBearerType )
    }

// -----------------------------------------------------------------------------
// CMPMServer::NumberOfActiveConnections
// Returns number of active iaps.
// -----------------------------------------------------------------------------
//
TInt CMPMServer::NumberOfActiveConnections( TBool& aKeysUpToDate )
    {
    MPMLOGSTRING( "CMPMServer::NumberOfActiveConnections" )
    
    aKeysUpToDate = ETrue; 
    TInt count( 0 );
    RArray<TUint32> activeIaps;
    
    activeIaps.Reset();
    
    for ( TInt index = 0; index < iActiveBMConns.Count(); index++ )
        {
        CMPMServerSession* serverSession = GetServerSession(
             iActiveBMConns[index].iConnInfo.iConnId );

        if ( iActiveBMConns[index].iConnInfo.iState == EStarted &&
             serverSession->ChooseBestIapCalled() )
            {
            TInt ret = activeIaps.Find( iActiveBMConns[index].iConnInfo.iIapId );
 
            if ( ret == KErrNotFound )
                {        
                activeIaps.Append ( iActiveBMConns[index].iConnInfo.iIapId ); 
                count++;
                }
            
            if ( iActiveIapId != iActiveBMConns[index].iConnInfo.iIapId )
                 {
                 // iap that is different from current active iap was found
                 aKeysUpToDate = EFalse;
                 }
            }
        }
    
    activeIaps.Close();
    MPMLOGSTRING2( "CMPMServer::NumberOfActiveConnections: count = %d", count )
    
    return count;
    }

// -----------------------------------------------------------------------------
// CMPMServer::UserConnectionInInternet
// -----------------------------------------------------------------------------
//
TBool CMPMServer::UserConnectionInInternet() const
    {
    TBool isInternet = EFalse;
    TInt ret = KErrNone;
        
    TRAP( ret, isInternet = iCommsDatAccess->IsInternetSnapL(
          UserConnPref()->IapId(), UserConnPref()->SnapId() ) );

    if ( ret != KErrNone )
        {
        isInternet = EFalse;
        }
    
    return isInternet;
    }

// -----------------------------------------------------------------------------
// CMPMServer::StartForcedRoamingToWlanL
// -----------------------------------------------------------------------------
//
void CMPMServer::StartForcedRoamingToWlanL( const TConnMonIapInfo& aIapInfo )
    {
    MPMLOGSTRING( "CMPMServer::StartForcedRoamingToWlan" )
    
    // cancel the periodic object
    if ( iRoamingToWlanPeriodic != NULL )
        {
        iRoamingToWlanPeriodic->Cancel();
        }

    // Copy all available wlan iap ids to own array
    RArray<TUint32> wlanIapIds;
    CleanupClosePushL( wlanIapIds );
    RAvailableIAPList iapList;
    CleanupClosePushL( iapList );

    for ( TUint index = 0; index < aIapInfo.iCount; index++ )
        {
        if ( CommsDatAccess()->CheckWlanL( aIapInfo.iIap[index].iIapId ) != ENotWlanIap )
            {
            // Accept only wlan iaps in internet snap
            if ( iCommsDatAccess->IsInternetSnapL( aIapInfo.iIap[index].iIapId, 0 ) )
                {
                wlanIapIds.AppendL( aIapInfo.iIap[index].iIapId );
                }
            }
        // Fill iap list to be used later to check best iap
        iapList.AppendL( aIapInfo.iIap[index].iIapId );
        }

    // No wlans available -> no reason to continue
    if ( !wlanIapIds.Count() )
        {
        CleanupStack::PopAndDestroy( &iapList );
        CleanupStack::PopAndDestroy( &wlanIapIds );
        return;
        }

    // Go through all active connections and start roaming for the ones connected
    // to snap containing wlan and not using mobility api
    for ( TInt index = 0; index < iActiveBMConns.Count(); index++ )
        {
        // Check if snap is internet snap
        TBool internetSnap =
            iCommsDatAccess->IsInternetSnapL(
                iActiveBMConns[index].iConnInfo.iIapId,
                iActiveBMConns[index].iConnInfo.iSnap );

        CMPMServerSession* serverSession = GetServerSession(
                iActiveBMConns[index].iConnInfo.iConnId );

        // Check that connection is started, established to snap and
        // choose best iap is called for the connection
        if ( ( iActiveBMConns[index].iConnInfo.iState == EStarted ) &&
             ( iActiveBMConns[index].iConnInfo.iSnap ) && 
             ( serverSession->ChooseBestIapCalled() ) &&
             ( internetSnap ) )
            {
            // Notify client to disconnect
            NotifyDisconnectL( index, wlanIapIds, iapList, ETrue,
                EMPMBearerTypeWlan );
            }
        }
    CleanupStack::PopAndDestroy( &iapList );
    CleanupStack::PopAndDestroy( &wlanIapIds );
    }


// ---------------------------------------------------------------------------
// CMPMServer::StartForcedRoamingToConnectedWlanL
// ---------------------------------------------------------------------------
//    
TInt CMPMServer::StartForcedRoamingToConnectedWlanL( TAny* aUpdater )
    {
    MPMLOGSTRING( "CMPMServer::StartForcedRoamingToConnectedWlanL" );
    static_cast<CMPMServer*>( aUpdater )->StartForcedRoamingToWlanL( 
            static_cast<CMPMServer*>( aUpdater )->iConnMonIapInfo );
    return 0;
    }


// -----------------------------------------------------------------------------
// CMPMServer::StartForcedRoamingFromWlanL
// -----------------------------------------------------------------------------
//
void CMPMServer::StartForcedRoamingFromWlanL( const TConnMonIapInfo& aIapInfo )
    {
    MPMLOGSTRING( "CMPMServer::StartForcedRoamingFromWlan" )

    // Copy all available packet data iap ids to own array
    RArray<TUint32> packetDataIapIds;
    CleanupClosePushL( packetDataIapIds );

    for ( TUint index = 0; index < aIapInfo.iCount; index++ )
        {
        if ( CommsDatAccess()->GetBearerTypeL( aIapInfo.iIap[index].iIapId ) ==
            EMPMBearerTypePacketData )
            {
            // Accept only packet data iaps in internet snap
            if ( iCommsDatAccess->IsInternetSnapL( aIapInfo.iIap[index].iIapId, 0 ) )
                {
                packetDataIapIds.AppendL( aIapInfo.iIap[index].iIapId );
                }
            }
        }

    // No packet data iaps available -> no reason to continue
    if ( !packetDataIapIds.Count() )
        {
        CleanupStack::PopAndDestroy( &packetDataIapIds );
        return;
        }
    
    // Go through all active connections and start roaming for the ones connected
    // to a wlan not anymore listed in available iaps and not using mobility api
    for ( TInt index = 0; index < iActiveBMConns.Count(); index++ )
        {
        if ( iCommsDatAccess->CheckWlanL( iActiveBMConns[index].iConnInfo.iIapId )
            == EWlanIap )
            {
            // Check if used WLAN is still available
            TBool currentWlanIapAvailable = EFalse;
            for ( TUint iapIndex = 0; iapIndex < aIapInfo.iCount; iapIndex++ )
                {
                if ( aIapInfo.iIap[iapIndex].iIapId ==
                    iActiveBMConns[index].iConnInfo.iIapId )
                    {
                    // Current WLAN IAP found from list of available IAPs
                    currentWlanIapAvailable = ETrue;
                    break;
                    }
                }
            
            if ( !currentWlanIapAvailable )
                {
                // Current WLAN not available anymore
                // Check if snap is internet snap
                TBool internetSnap = iCommsDatAccess->IsInternetSnapL(
                    iActiveBMConns[index].iConnInfo.iIapId,
                    iActiveBMConns[index].iConnInfo.iSnap );
    
                CMPMServerSession* serverSession = GetServerSession(
                    iActiveBMConns[index].iConnInfo.iConnId );
                
                // Check that connection is started, established to snap,
                // choose best iap is called for the connection
                if ( ( iActiveBMConns[index].iConnInfo.iState == EStarted ) &&
                    ( iActiveBMConns[index].iConnInfo.iSnap ) && 
                    ( serverSession->ChooseBestIapCalled() ) &&
                    ( internetSnap ) )
                    {
                    // Notify client to disconnect, don't check if current
                    // WLAN IAP is the best because we want to disconnect
                    // it anyway (it was not included in available IAP
                    // list anymore)
                    RAvailableIAPList iapList;
                    CleanupClosePushL( iapList );
                    NotifyDisconnectL( index, packetDataIapIds, iapList, EFalse,
                        EMPMBearerTypePacketData );
                    CleanupStack::PopAndDestroy( &iapList );
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( &packetDataIapIds );
    }

// -----------------------------------------------------------------------------
// CMPMServer::NotifyDisconnectL
// -----------------------------------------------------------------------------
//
void CMPMServer::NotifyDisconnectL( TInt aConnIndex,
                                    RArray<TUint32>& aAvailIapIds,
                                    RAvailableIAPList& aIapList,
                                    TBool aCheckForBestIap,
                                    TMPMBearerType aDestinationBearerType )
    {
    MPMLOGSTRING( "CMPMServer::NotifyDisconnectL" )
    
    // Get iaps in internet snap
    RArray<TNetIap> destNetIds;
    CleanupClosePushL( destNetIds );
    CommsDatAccess()->SearchDNEntriesL( iActiveBMConns[aConnIndex].iConnInfo.iSnap,
                                        destNetIds );

    // Save available iaps in internet snap
    RArray<TUint32> iapIdsInInternet;
    CleanupClosePushL( iapIdsInInternet );
                        
    for ( TInt iIndex = 0; iIndex < destNetIds.Count(); iIndex++ )
        {
        for ( TInt wIndex = 0; wIndex < aAvailIapIds.Count(); wIndex++ )
            {
            if ( destNetIds[iIndex].iIapId == aAvailIapIds[wIndex] )
                {
                iapIdsInInternet.AppendL( destNetIds[iIndex].iIapId );
                break;
                }
            }
                    
        if ( iapIdsInInternet.Count() )
            {
            // Leave loop when count is non-zero
            break;
            }
        }

    // Check if an iap in internet snap is available
    if ( iapIdsInInternet.Count() )
        {
        // Find session and notify error
        for (TInt sIndex = 0; sIndex < iSessions.Count(); sIndex++ )
            {
            // Check that CMPMIapSelection object exists for the session.
            TRAPD( error, iSessions[sIndex]->IapSelectionL() );
            if ( error == KErrNone )
                {
                MPMLOGSTRING( "CMPMServer::NotifyDisconnectL: IapSelectionL() != NULL" )
                // Check the connection preferences for forced roaming
                if ( iSessions[sIndex]->ForcedRoaming() )
                    {
                    MPMLOGSTRING( "CMPMServer::NotifyDisconnectL: ForcedRoaming == ETrue" )
                    // Notify disconnect error for session,
                    // if mobility api is not used
                    if ( ( iSessions[sIndex]->ConnectionId() ==
                        iActiveBMConns[aConnIndex].iConnInfo.iConnId ) &&
                        !iSessions[sIndex]->PreferredIapRequested() &&
                        iSessions[sIndex]->IsBearerAccepted( aDestinationBearerType ) )
                        {
                        if ( aCheckForBestIap )
                            {
                            // Check whether current IAP and the best IAP are the same.
                            // Disconnection not done if IAPs are the same   
                            TMpmConnPref connPref;
                            connPref.SetIapId( 0 );
                            connPref.SetSnapId(
                                iActiveBMConns[aConnIndex].iConnInfo.iSnap );

                            iSessions[sIndex]->IapSelectionL()->ChooseBestIAPL( connPref, aIapList );

                            // if the best iap is the current iap, don't roam, move to next item
                            if ( ( aIapList.Count() > 0 ) && 
                                ( connPref.IapId() == iActiveBMConns[aConnIndex].iConnInfo.iIapId ) )
                                {
                                MPMLOGSTRING( "CMPMServer::NotifyDisconnectL: Same IAP selected. Disconnection not done." )
                                break;
                                }
                            }
    
                        MPMLOGSTRING2( "CMPMServer::NotifyDisconnectL: \
Disconnected Connection Id = 0x%x", iSessions[sIndex]->ConnectionId() )
                        iSessions[sIndex]->ClientErrorNotificationL( KErrForceDisconnected );
                        }
                    }
                }
            }
        }
            
    CleanupStack::PopAndDestroy( &iapIdsInInternet );
    CleanupStack::PopAndDestroy( &destNetIds );
    }

// -----------------------------------------------------------------------------
// CMPMServer::IsVisitorNetwork
// 
// Checks if phone is in visitor network or in home network.
// Return ETrue if phone is in visitor network.
// Return EFalse if phone is not in visitor network.
// -----------------------------------------------------------------------------
//
TBool CMPMServer::IsVisitorNetwork() const
    {
    MPMLOGSTRING( "CMPMServer::IsVisitorNetwork" )
    
    if ( iRoamingWatcher->RoamingStatus()== EMPMInternationalRoaming )
        {
        MPMLOGSTRING( "CMPMServer::IsVisitorNetwork: TRUE" )
        return ETrue;
        } 
    else
        {
        MPMLOGSTRING( "CMPMServer::IsVisitorNetwork: FALSE" )
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServer::GetServerSession
// -----------------------------------------------------------------------------
//
CMPMServerSession* CMPMServer::GetServerSession( TConnectionId aConnId ) const
    {
    MPMLOGSTRING2( "CMPMServer::GetServerSession: \
Connection Id = 0x%x", aConnId );

    CMPMServerSession* serverSession = NULL;
    for ( TInt sIndex = 0; sIndex < iSessions.Count(); sIndex++ )
        {
        if ( iSessions[sIndex]->ConnectionId() == aConnId )
            {
            serverSession = iSessions[sIndex];
            }
        }

    ASSERT( serverSession != NULL );

    return serverSession;
    }

// ---------------------------------------------------------------------------
// CMPMServer::StopCellularConns
// Stop all cellular connections except MMS
// ---------------------------------------------------------------------------
//
void CMPMServer::StopCellularConns()
    {
    MPMLOGSTRING( "CMPMServer::StopCellularConns" )

    TUint32 iapId;
	TMPMBearerType bearerType = EMPMBearerTypeNone;

	// No cleanup stack used cause this function doesn't leave.
    RArray<TUint32> stoppedIaps;

    // Check through active connections
    for (TInt i = 0; i < iActiveBMConns.Count(); i++)
        {
        iapId = iActiveBMConns[i].iConnInfo.iIapId;

        // Don't stop the same IAP twice.
        if (stoppedIaps.Find( iapId ) == KErrNotFound)
            {
            TRAPD( err, bearerType = CommsDatAccess()->GetBearerTypeL( iapId ) );
            if (err == KErrNone &&
                    iapId != 0 &&
                    bearerType == EMPMBearerTypePacketData)
                {
                TInt mmsIap( 0 );
                err = RProperty::Get( KMPMCathegory, KMPMPropertyKeyMMS, mmsIap );
                // Check that it's not MMS IAP.
                if (!(err == KErrNone && iapId == mmsIap))
                    {
                    // Stop the conn / IAP.
                    StopConnections( iapId );
                    stoppedIaps.Append( iapId );
                    }
                }
            }
        }
    stoppedIaps.Close();
    }

// ---------------------------------------------------------------------------
// CMPMServer::UpdateOfflineMode
// Offline watcher listens the offline mode and calls this when it's changed.
// ---------------------------------------------------------------------------
//
void CMPMServer::UpdateOfflineMode( TInt newModeValue )
    {
    MPMLOGSTRING2( "CMPMServer::UpdateOfflineMode: Value %d", newModeValue )

    iOfflineMode = newModeValue;

    if ( iOfflineMode == ECoreAppUIsNetworkConnectionAllowed )
        {
        // Offline mode finished, reset the QueryResponse variable.
        iOfflineWlanQueryResponse = EOfflineResponseUndefined;
        }
    }

// ---------------------------------------------------------------------------
// CMPMServer::IsPhoneOffline
// Returns the current offline mode.
// ---------------------------------------------------------------------------
//
TBool CMPMServer::IsPhoneOffline()
    {
    MPMLOGSTRING( "CMPMServer::IsPhoneOffline" )

    TBool retval = EFalse;
    if ( iOfflineMode == ECoreAppUIsNetworkConnectionNotAllowed)
        {
        retval = ETrue;
        MPMLOGSTRING( "CMPMServer::IsPhoneOffline: Yes." )
        }
    return retval;
    }

// ---------------------------------------------------------------------------
// CMPMServer::OfflineWlanQueryResponse
// Tells the "Use WLAN in offline mode" query's response during the
// current offline mode session.
// ---------------------------------------------------------------------------
//
TOfflineWlanQueryResponse CMPMServer::OfflineWlanQueryResponse()
    {
    MPMLOGSTRING( "CMPMServer::OfflineWlanQueryResponse" )

#ifndef _PLATFORM_SIMULATOR_
    MPMLOGSTRING2( "CMPMServer::IsOfflineWlanQueryAccepted: %d", iOfflineWlanQueryResponse )
    return iOfflineWlanQueryResponse;
#else
    // Platsim simulates WLAN and offline-mode. To ease automated testing,
    // offline connection confirmation is not asked in Platsim-variant
    MPMLOGSTRING( "CMPMServer::OfflineWlanQueryResponse: yes for Platsim" )
    return EOfflineResponseYes;
#endif
    }

// ---------------------------------------------------------------------------
// CMPMServer::SetOfflineWlanQueryResponse
// Called when the "Use WLAN in offline mode" note has been responded.
// ---------------------------------------------------------------------------
//
void CMPMServer::SetOfflineWlanQueryResponse( TOfflineWlanQueryResponse aResponse)
    {
    MPMLOGSTRING( "CMPMServer::SetOfflineWlanQueryResponse" )

    iOfflineWlanQueryResponse = aResponse;
    }

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Append
// -----------------------------------------------------------------------------
//
void TMPMBlackListConnId::Append( TUint32 aIap, TBlacklistCategory aCategory )
    {
    iBlackListIap.Append( aIap );
    iCategory.Append( aCategory );
    }

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Remove
// -----------------------------------------------------------------------------
//
void TMPMBlackListConnId::Remove( TInt aIndex )
    {
    iBlackListIap.Remove( aIndex );
    iCategory.Remove( aIndex );    
    }

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Close
// -----------------------------------------------------------------------------
//
void TMPMBlackListConnId::Close()
    {
    iBlackListIap.Close();
    iCategory.Close();    
    }

// -----------------------------------------------------------------------------
// TMPMBlackListConnId::Count
// -----------------------------------------------------------------------------
//
TInt TMPMBlackListConnId::Count() const
    {
    return iBlackListIap.Count();
    }

// -----------------------------------------------------------------------------
// TConnectionInfo::TConnectionInfo
// -----------------------------------------------------------------------------
//
TConnectionInfo::TConnectionInfo() 
    : iConnId( 0 ),
      iSnap( 0 ),
      iIapId( 0 ), 
      iState( EIdle ),
      iAppUid( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TConnectionInfo::MatchId
// -----------------------------------------------------------------------------
//
TBool TConnectionInfo::MatchId( const TConnectionInfo& aFirst,
                                       const TConnectionInfo& aSecond )
    {
    if ( aFirst.iConnId == aSecond.iConnId )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TConnectionInfo::MatchIdSnap
// -----------------------------------------------------------------------------
//
TBool TConnectionInfo::MatchIdSnap( const TConnectionInfo& aFirst,
                                    const TConnectionInfo& aSecond )
    {
    if ( ( aFirst.iConnId  == aSecond.iConnId ) &&
         ( aFirst.iSnap    == aSecond.iSnap ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// TActiveBMConn::TActiveBMConn
// -----------------------------------------------------------------------------
//
TActiveBMConn::TActiveBMConn() 
    : iConnInfo()
    {
    }


// -----------------------------------------------------------------------------
// TActiveBMConn::MatchIapId
// -----------------------------------------------------------------------------
//
TBool TActiveBMConn::MatchIapId( const TActiveBMConn& aFirst,
                                 const TActiveBMConn& aSecond )
    {
    if ( aFirst.iConnInfo.iIapId == aSecond.iConnInfo.iIapId )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// -----------------------------------------------------------------------------
// TActiveBMConn::MatchConnInfo
// -----------------------------------------------------------------------------
//
TBool TActiveBMConn::MatchConnInfo( const TActiveBMConn& aFirst,
                                    const TActiveBMConn& aSecond )
    { 
    if ( TConnectionInfo::MatchId( aFirst.iConnInfo, aSecond.iConnInfo ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


//  End of File
