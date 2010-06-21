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
* Description: MPM server session implementation
*
*/

/**
@file mpmserversession.cpp
Mobility Policy Manager server session implementation.
*/

// INCLUDE FILES
#include <e32svr.h>
#include <gsmerror.h>     // KErrPacketDataTsyMaxPdpContextsReached 
#include <etelpckt.h>     // KErrUmtsMaxNumOfContextExceededByNetwork
#include <bldvariant.hrh>                // For feature flags
#include <centralrepository.h>           // CRepository 
#include <CoreApplicationUIsSDKCRKeys.h> // KCRUidCoreApplicationUIs, 
                                         // KCoreAppUIsNetworkConnectionAllowed
#include <es_sock.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <cmpluginbaseeng.h>
#include <cdblen.h>
#include <cmgenconnsettings.h>

#include "mpmcommsdataccess.h"
#include "mpmserversession.h"
#include "mpmconnmonevents.h"
#include "mpmdialog.h"
#include "mpmdisconnectdlg.h"
#include "mpmconfirmdlgstarting.h"
#include "mpmconfirmdlgroaming.h"
#include "mpmlogger.h"
#include "mpmpropertydef.h"
#include "mpmdefaultconnection.h"
#include "mpmiapselection.h"
#include "mpmcsidwatcher.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMServerSession::NewL
// -----------------------------------------------------------------------------
//
CMPMServerSession* CMPMServerSession::NewL(CMPMServer& aServer)
    {
    CMPMServerSession* self = new ( ELeave ) CMPMServerSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::CMPMServerSession
// -----------------------------------------------------------------------------
//
CMPMServerSession::CMPMServerSession(CMPMServer& aServer)
    : CSession2(), 
      iMyServer( aServer ),
      iDisconnectDlg( NULL ),
      iConfirmDlgRoaming( NULL ),
      iStoredIapInfo(),
      iIapSelection( NULL ),
      iMigrateState( EMigrateNone ),
      iDisconnectDialogShown( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ConstructL()
    {
    MPMLOGSTRING( "CMPMServerSession::ConstructL" )
    if ( !iMyServer.Events() )
        {
        iMyServer.SetEvents( CMPMConnMonEvents::NewL(
            *const_cast<CMPMServer*>( &iMyServer ) ) );
        }

    // Append session pointer to server
    // 
    iMyServer.AppendSessionL( this );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::~CMPMServerSession
// -----------------------------------------------------------------------------
//
CMPMServerSession::~CMPMServerSession()
    {
    delete iDisconnectDlg;
    delete iConfirmDlgRoaming;
    delete iIapSelection;

    // Remove serverside objects for notification session.
    // 
    iMyServer.RemoveSession( this );

    if (UserConnection())
        {
        iMyServer.ClearUserConnection();
        ClearUserConnection();
        
        // Set PS keys to zero
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionSnap,
                        0 );
        
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionIap,
                        0 );
        
        MPMLOGSTRING( "CMPMServerSession::HandleServerApplicationConnectionEnds -\
User connection deactivated" )
        }   

    // Clean up the blacklist table 
    iMyServer.HandleServerUnblackListIap( iConnId, 0 );
    
    // Make sure the connection is removed from server's information array.
    iMyServer.RemoveBMConnection( iConnId, *this );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::ServiceL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EMPMServerChooseIap:
            {
            HandleServerChooseIapL( aMessage );
            break;
            }
        case EMPMServerCancelRequest:
            {
            HandleServerCancelRequest( aMessage );
            break;
            }
        case EMPMServerApplicationLeavesConnection:
            {
            HandleServerApplicationLeavesConnection( aMessage );
            break;
            }
        case EMPMServerIapConnectionStopped:
            {
            HandleServerIapConnectionStopped( aMessage );
            break;
            }
        case EMPMServerProcessError:
            {
            HandleServerProcessErrorL( aMessage );
            break;
            }
        case EMPMServerRegisterPrefIAPNotif:
            {
            HandleServerRegisterPrefIAPNotifL( aMessage ); 
            break;
            }
        case EMPMServerUnregisterPrefIAPNotif:
            {
            HandleServerUnregisterPrefIAPNotif( aMessage );
            break;
            }
        case EMPMServerWaitNotification:
            {
            HandleServerWaitNotificationL( aMessage );
            break;                                              
            }
        case EMPMServerSortSNAP:
            {
            HandleServerSortSNAPL( aMessage ); 
            break;
            }
        case EMPMServerApplicationJoinsConnection:
            {
            HandleServerApplicationJoinsConnection( aMessage );
            break;
            }
        case EMPMServerIapConnectionActivated:
            {
            HandleServerIapConnectionActivatedL( aMessage );
            break;
            }
        case EMPMServerIapConnectionStarted:
            {
            HandleServerIapConnectionStartedL( aMessage );
            break;
            }
        case EMPMServerApplicationConnectionEnds:
            {
            HandleServerApplicationConnectionEnds( aMessage );
            break;
            }
        case EMPMServerApplicationMigratesToCarrier:
            {
            HandleServerApplicationMigratesToCarrierL( aMessage );
            break;
            }
        case EMPMServerApplicationIgnoredTheCarrier:
            {
            HandleServerApplicationIgnoredTheCarrier( aMessage );
            break;
            }
        case EMPMServerApplicationAcceptedTheCarrier:
            {
            HandleServerApplicationAcceptedTheCarrier( aMessage );
            break;
            }
        case EMPMServerApplicationRejectedTheCarrier:
            {
            HandleServerApplicationRejectedTheCarrierL( aMessage );
            break;
            }
        case EMPMServerEasyWlanSsid:
            {
            // Easy WLAN SSID not used anymore as connecting 
            // notes have been removed
            //
            MPMLOGSTRING( "CMPMServerSession::ServiceL \
completing EMPMServerEasyWlanSsid" )
            aMessage.Complete( KErrNone );
            break;
            }
        case EMPMServerReselectIap:
            {
            HandleServerReselectIapL( aMessage );
            break;
            }
#ifdef _DEBUG
        case EMPMDebugGenerateStopIAPNotification:
            {
            HandleDebugGenerateStopIAPNotification( aMessage );
            break;
            }
        case EMPMDebugSwitchConnMonEventsOn:
            {
            HandleDebugFilterConnMonEvents( aMessage, EFalse );
            break;
            }
        case EMPMDebugSwitchConnMonEventsOff:
            {
            HandleDebugFilterConnMonEvents( aMessage, ETrue );
            break;
            }
        case EMPMDebugShutdown:
            {
            HandleDebugShutdown( aMessage, ETrue );
            break;
            }    
#endif //_DEBUG
        default:
            {
            iMyServer.PanicClient( KErrNotSupported );
            }
        }
    }

#ifdef _DEBUG
void CMPMServerSession::HandleDebugGenerateStopIAPNotification( const RMessage2& aMessage )
    {
    MPMLOGSTRING( "CMPMServerSession::HandleDebugGenerateStopIAPNotification" )
    TInt iap = static_cast<TInt>( aMessage.Int0() );
    MyServer().StopConnections( iap );
    aMessage.Complete( KErrNone );
    }
	
void CMPMServerSession::HandleDebugFilterConnMonEvents( const RMessage2& aMessage, const TBool aVal )
    {
    MPMLOGSTRING2( "CMPMServerSession::HandleDebugFilterConnMonEvents FilteringEnabled=%d", aVal )
    TInt iap = static_cast<TInt>( aMessage.Int0() );
    MyServer().Events()->FilterAvailableIAPEvents( aVal );
    aMessage.Complete( KErrNone );
    }
    
void CMPMServerSession::HandleDebugShutdown( const RMessage2& aMessage, const TBool /*aVal*/ )
    {
    MPMLOGSTRING( "CMPMServerSession::HandleDebugShutdown" )
    aMessage.Complete( KErrNone );
    
    TFindProcess processFinder( _L("*MPMServer*") );
    TFullName    name;
    
    if ( processFinder.Next( name ) == KErrNone ) 
        {
        RProcess process;
        
        TInt err = process.Open( name );
        
        if ( err == KErrNone ) 
            {
            process.Kill( 0 );
            }
                
        process.Close();
        }    
    }    
#endif //_DEBUG

// -----------------------------------------------------------------------------
// CMPMServerSession::ServiceError
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ServiceError( const RMessage2& aMessage, 
                                      TInt             aError )

    {
    // Handles the situation when a call to CSession2::ServiceL(), 
    // which services a client request, leaves.
    // 
    // The default behaviour of this function is to complete the message, 
    // using the leave value, if it has not already been completed.
    // 
    // Servers can re-implement this as appropriate.
    // 
    MPMLOGSTRING3(
        "CMPMServerSession::ServiceError: Service %d failed with error: %d",
         aMessage.Function(),
         aError )
    if( aMessage.Function() == EMPMServerChooseIap ||
        aMessage.Function() == EMPMServerReselectIap )
        {
        // complete locally stored iChooseIapMessage
        //
        ChooseIapComplete( aError, NULL );
        }
    else if( aMessage.Function() == EMPMServerProcessError )
        {
        // complete locally stored iProcessErrorMessage
        //
        ProcessErrorComplete( aError, NULL, NULL );
        }
    else if( !aMessage.IsNull() )
        {
        // Complete message if it hasn't been 
        // already completed
        //
        aMessage.Complete( aError );        
        }
    else
        {
        MPMLOGSTRING(
        "CMPMServerSession::ServiceError: Message has already been completed" )
        }    
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerChooseIapL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerChooseIapL( const RMessage2& aMessage )
    {
    MPMLOGSTRING2( "CMPMServerSession::HandleServerChooseIapL session <0x%x>", this )

    // Read the Connection Id of the application
    //
    iConnId = aMessage.Int1();
    MPMLOGSTRING2( "CMPMServerSession::HandleServerChooseIapL: \
Connection Id = 0x%x", iConnId )

    // Sanity check that client only has one outstanding ChooseBestIAP() or ReselectBestIAP() -call.
    //
    if ( !iChooseIapMessage.IsNull() )
        {
        MPMLOGSTRING2( "CMPMServerSession::HandleServerChooseIapL: - Error \
ChooseBestIAP() or ReselectBestIAP() already exists %d", KErrServerBusy );
        aMessage.Complete( KErrServerBusy );
        }

    // Sanity check that client doesn't call ChooseBestIAPL() twice for the same session.
    //
    if ( ChooseBestIapCalled() )
        {
        MPMLOGSTRING2( "CMPMServerSession::HandleServerChooseIapL: - Error \
ChooseBestIAP() already completed or in progress %d", KErrAlreadyExists )
        aMessage.Complete( KErrAlreadyExists );
        return;
        }

    // We must save the RMessage in order to complete it later.
    //
    iChooseIapMessage = aMessage;
    TConnPref connPref;
    aMessage.ReadL( KFirstArgument, connPref );

    // Extract connection preferences
    //
    TMpmConnPref mpmConnPref;
    TInt error = ExtractConnPref( connPref, mpmConnPref );
    
    if ( error != KErrNone )
        {
        // Connection preferences are not valid. Display
        // an error note and complete with the error code.
        //
        if ( ! ( mpmConnPref.NoteBehaviour() &
            TExtendedConnPref::ENoteBehaviourConnDisableNotes ) )
            {
            CConnectionUiUtilities* connUiUtils = NULL;
        
            TRAP_IGNORE( connUiUtils = CConnectionUiUtilities::NewL() );
            
            if ( connUiUtils )
                {
                connUiUtils->ConnectionErrorDiscreetPopup( error );
                delete connUiUtils;
                connUiUtils = NULL;
                }
            }    	
        	
        MPMLOGSTRING( "CMPMServerSession::HandleServerChooseIapL - Error \
while extracting TCommDbConnPref from TConnPref" )
        aMessage.Complete( error );
        return;
        }

    // Store the Uid of the application to the member variable so 
    // that it can be used to avoid DisconnectDialog popping up when 
    // AlwaysOnline connection is being established.
    // 
    iAppUid = aMessage.Int2();

    MPMLOGSTRING2( "CMPMServerSession::HandleServerChooseIapL: \
Client UID = 0x%x", iAppUid )

    if ( !iIapSelection )
        {
        iIapSelection = CMPMIapSelection::NewL( iMyServer.CommsDatAccess(),
                                                this ); 
        }
    
    MPMLOGSTRING3( "CMPMServerSession::HandleServerChooseIapL - iap %d \
connType %d", mpmConnPref.IapId(), mpmConnPref.ConnType() )

    iIapSelection->ChooseIapL( mpmConnPref );

    if ( iAppUid == iMyServer.CsIdWatcher()->ConnectScreenId() )
        {
        MPMLOGSTRING( "CMPMServerSession::HandleServerChooseIapL -\
 User connection activated" )
        
        // This is user connection
        iMyServer.SetUserConnection();
        SetUserConnection();
        iMyServer.SetUserConnPref( mpmConnPref );
        
        // Set PS keys according to user connection
        // Do not check return values. Can do nothing in case of failing.
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionSnap,
                        mpmConnPref.SnapId() );
        
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionIap,
                        mpmConnPref.IapId() );
        }
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerCancelRequest
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerCancelRequest( const RMessage2& aMessage )
    {
    TInt cancelcode = aMessage.Int0();
    MPMLOGSTRING2(
        "CMPMServerSession::HandleServerCancelRequest - Request code = %i", 
        cancelcode )

    switch ( cancelcode )
        {
        case EMPMReselectBestIAP:
        case EMPMChooseBestIAP:
            {
            // Complete original request
            // 
            ChooseIapComplete( KErrCancel, NULL );
            
            // Complete cancel request 
            //
            MPMLOGSTRING(
            "CMPMServerSession::HandleServerCancelRequest - Complete with KErrNone" )
            aMessage.Complete( KErrNone );
            
            // Cancel WLAN scan request if one exists
            //
            TRAP_IGNORE( iMyServer.Events()->CancelScanL( this ) )
            
            if ( iIapSelection )
                {
                delete iIapSelection;
                iIapSelection = NULL;
                }
            return;
            }
        case EMPMProcessError:
            {
            ProcessErrorComplete( KErrCancel,
                                  NULL,
                                  NULL );
            // Complete cancel request 
            //
            MPMLOGSTRING(
            "CMPMServerSession::HandleServerCancelRequest - Complete with KErrNone" )
            aMessage.Complete( KErrNone );

            // Cancel WLAN scan request if one exists
            //
            TRAP_IGNORE( iMyServer.Events()->CancelScanL( this ) )

            if ( iDisconnectDlg )
                {
                MPMLOGSTRING( "CMPMServerSession::HandleServerCancelRequest: \
removing dconn dlg" )
                delete iDisconnectDlg;
                iDisconnectDlg = NULL;
                }
            return;
            }
        case EMPMWaitNotification:
            {
            if ( iNotifRequested )
                {
                iNotifRequested = EFalse;
                iNotifMessage.Complete( KErrCancel );
                }
            break;
            }
        case EMPMSortSNAP:
            {
            if ( !iServerSortSNAPMessage.IsNull() )
                {
                // TODO Change CancelScanL to non-leaving.
                // Otherwise, nothing clever can be done here.
                // And OOM may risk MPM stability.
                TRAP_IGNORE( iMyServer.Events()->CancelScanL( this ) )
                iServerSortSNAPMessage.Complete( KErrCancel );
                }
            break;
            }
        default:
            {
            MPMLOGSTRING( "CMPMServerSession::HandleServerCancelRequest - \
Unknown cancel request received" )
            }
        }
    MPMLOGSTRING( 
    "CMPMServerSession::HandleServerCancelRequest - Complete with KErrNone" )
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationJoinsConnection
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationJoinsConnection(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    TUint32 joinIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId joinId = aMessage.Int1();
    
    //-jl- Make sure id is known & assert it stays the same.
    ASSERT(iConnId == joinId || iConnId == NULL);
    iConnId = joinId;

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationJoinsConnection \
- IAP Id = %i, Connection Id = 0x%x", joinIap, joinId )

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 snapId = iMyServer.GetBMSnap( joinId );

    
    // Decides the correct state by the IAP status.
    if ( iMyServer.StartedConnectionExists( joinIap ) == joinIap )
        {
        iMyServer.AppendBMConnection( joinId, 
                                  snapId, 
                                  joinIap, 
                                  EStarted,
								  *this );
        }
    else
        {
        iMyServer.AppendBMConnection( joinId, 
                                  snapId, 
                                  joinIap, 
                                  EStarting,
								  *this );
        }

    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationLeavesConnection
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationLeavesConnection(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    TUint32 leaveIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId leaveId = aMessage.Int1();

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationLeavesConnectionL \
- IAP Id = %i, Connection Id = 0x%x", leaveIap, leaveId )

    iMyServer.Events()->ResetConnInfo( leaveId );
    iMyServer.ResetBMConnection( leaveId, leaveIap, *this );

    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerIapConnectionActivatedL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerIapConnectionActivatedL(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    TUint32 activatedIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId activatedId = aMessage.Int1();

    //-jl- Make sure id is known & assert it stays the same.
    ASSERT(iConnId == activatedId || iConnId == NULL);
    iConnId = activatedId;
    
    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerIapConnectionActivatedL \
- IAP Id = %i, Connection Id = 0x%x", activatedIap, activatedId )

    MPMLOGSTRING(
        "CMPMServerSession::HandleServerIapConnectionActivatedL \
- Complete IapConnectionActivated" )
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerIapConnectionStarted
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerIapConnectionStartedL(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    TUint32 startedIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId startedId = aMessage.Int1();

    //-jl- Make sure id is known & assert it stays the same.
    ASSERT(iConnId == startedId || iConnId == NULL);
    iConnId = startedId;
    
    MPMLOGSTRING3( "CMPMServerSession::HandleServerIapConnectionStarted\
 - IAP Id = %i, Connection Id = 0x%x", startedIap, startedId )

    iMyServer.AppendBMIAPConnectionL( startedIap, startedId, *this );

    // Complete the message as soon as possible to avoid latency in BM
    // 
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerIapConnectionStopped
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerIapConnectionStopped(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    TUint32 stoppedIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId stoppedId = aMessage.Int1();

    MPMLOGSTRING3( "CMPMServerSession::HandleServerIapConnectionStopped\
 - IAP Id = %i, Connection Id = 0x%x", stoppedIap, stoppedId )

    iMyServer.RemoveBMIAPConnection( stoppedIap, stoppedId, *this );

    // Complete the message as soon as possible to avoid latency in BM
    // 
    aMessage.Complete( KErrNone );

    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationConnectionEnds
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationConnectionEnds( 
    const RMessage2& aMessage )
    {
    // Read the Connection Id of the application
    //
    TConnectionId endId = aMessage.Int0();

    if (UserConnection())
        {
        iMyServer.ClearUserConnection();
        ClearUserConnection();
        
        // Set PS keys to zero
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionSnap,
                        0 );
        
        RProperty::Set( KMPMUserConnectionCategory,
                        KMPMPSKeyUserConnectionIap,
                        0 );
        
        MPMLOGSTRING( "CMPMServerSession::HandleServerApplicationConnectionEnds -\
User connection deactivated" )
        }   

    MPMLOGSTRING2( "CMPMServerSession::HandleServerApplicationConnectionEnds\
 - Connection Id = 0x%x", endId )

    // Unblacklist all IAPs related to this connection  
    // when connection closes.
    // 
    iMyServer.HandleServerUnblackListIap( endId, 0 );

    // Remove info about this connection
    // 
    iMyServer.Events()->RemoveConnInfo( endId );

    // SNAP lifetime is determined by the two calls ChooseBestIAP 
    // and ApplicationConnectionEnds.
    // 
    iMyServer.RemoveBMConnection( endId, *this );

    // Clear notification registration if one exists for the connection
    //
    iPreferredIAPRequested = EFalse;
    
    // Complete message before calling synchronous Cancel functions
    aMessage.Complete( KErrNone );

    delete iConfirmDlgRoaming;
    iConfirmDlgRoaming = NULL;
    
    if( iIapSelection )
        {
        delete iIapSelection;
        iIapSelection = NULL;        
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::IsConfirmFirstL
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsConfirmFirstL( const TUint32 aIapId )
    {
    TCmGenConnSettings  genConnSettings;
    TBool               isConfirmFirst( EFalse );

    MPMLOGSTRING3( "CMPMServerSession::IsConfirmFirstL - IAP Id = %i Connection Id = 0x%x", 
                   aIapId,
                   iConnId )

    // check whether a started connection exists which already 
    // uses this IAP. If so, it won't need to be confirmed again
    //
    TConnectionState state = iMyServer.CheckUsageOfIap( aIapId, iConnId );

    if ( state == EStarted || state == EStarting || state == ERoaming )
        {
        MPMLOGSTRING(
        "CMPMServerSession::IsConfirmFirstL - IAP already started, \
confirm not necesary - False" )
        return EFalse;
        }

    if ( IsBackgroundApplication( iAppUid ) || IsMMSIap( aIapId ) )
        {
        MPMLOGSTRING(
        "CMPMServerSession::IsConfirmFirstL - background IAP, \
should not be confirmed - False" )
        return EFalse;
        }

    // Read global cellular data usage values from CommsDat's DefConn table
    genConnSettings = iMyServer.CommsDatAccess()->ReadGenConnSettingsL();

    TUint32 currentDataUsage( genConnSettings.iCellularDataUsageHome );
    
    if ( iMyServer.IsVisitorNetwork() )
        {
        currentDataUsage = genConnSettings.iCellularDataUsageVisitor; 
        }

    if ( currentDataUsage == ECmCellularDataUsageConfirm )
        {
        MPMLOGSTRING( "CMPMServerSession::IsConfirmFirstL - True" )
        isConfirmFirst = ETrue;

        // iDisconnectDialogShown is set when disconnect dialog is shown.
        // Before the disconnect dialog is shown, data usage confirmation
        // dialog is already shown for the new PDP context. If you choose
        // to disconnect the active PDP context then data usage dialog is
        // shown again for the new context. So, set isConfirmFirst to
        // False to avoid duplicate cellular confirm dialog
        //
        if ( iDisconnectDialogShown )
            {
            MPMLOGSTRING( "CMPMServerSession::IsConfirmFirstL  - False; Data \
confirmation dialog already shown this IAP" )
            isConfirmFirst = EFalse;
            iDisconnectDialogShown = EFalse;
            }
        }
    else 
        {
        MPMLOGSTRING( "CMPMServerSession::IsConfirmFirstL - False" )
        }

    return isConfirmFirst;
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationMigratesToCarrierL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationMigratesToCarrierL(
    const RMessage2& aMessage )
    {
    // Read Iap Id
    //
    iMigrateIap = static_cast<TUint32>( aMessage.Int0() );

    // Complete the message ApplicationMigratesToCarrier already here, 
    // otherwise MPM would be hanging Esock thread when displaying the 
    // confirmation dialog while roaming.
    // 
    aMessage.Complete( KErrNone );

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationMigratesToCarrierL\
 - IAP Id = %i, Connection Id = 0x%x", iMigrateIap, iConnId )

    delete iConfirmDlgRoaming;
    iConfirmDlgRoaming = NULL;

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 snapId = iMyServer.GetBMSnap( iConnId );

    // We have to find out the current IAP of connection in order 
    // to determine whether this is an upgrade or downgrade.
    // 
    TUint32 currentIap = iMyServer.GetBMIap( iConnId );

    iMyServer.AppendBMConnection( iConnId, 
                                  snapId, 
                                  iMigrateIap, 
                                  ERoaming,
                                  *this );

    iMigrateState = EMigrateUserConfirmation;

    // Confirm only if this IAP is not already in use
    //
    if ( ( currentIap != iMigrateIap ) && 
         IsConfirmFirstL( iMigrateIap ) )
        {
        TBool             nextBestExists( EFalse );
        TMpmConnPref      mpmConnPref;
        RAvailableIAPList availableIAPList;

        CleanupClosePushL( availableIAPList );
        AvailableUnblacklistedIapsL( availableIAPList, 
                                     iConnId );

        mpmConnPref.SetSnapId( snapId );

        IapSelectionL()->ChooseBestIAPL( mpmConnPref, availableIAPList, nextBestExists );

        TBool reconnect ( EFalse );      
        if ( !IsUpgrade( currentIap, iMigrateIap, availableIAPList ) )
                    {
                    reconnect = ETrue;
                    }

        //Display confirm dialog only if we are moving to cellular IAP
        if ( MyServer().CommsDatAccess()->CheckWlanL( iMigrateIap ) == ENotWlanIap )
            {
            if ( !( iIapSelection->MpmConnPref().NoteBehaviour() & TExtendedConnPref::ENoteBehaviourConnDisableQueries ) )
                {
                if ( MyServer().RoamingWatcher()->RoamingStatus() == EMPMInternationalRoaming )
                    {
                    //International roaming
                    iConfirmDlgRoaming = CMPMConfirmDlgRoaming::NewL( 
                            *this, 
                            snapId, 
                            iMigrateIap, 
                            CMPMConfirmDlg::EConfirmDlgVisitorNetwork, 
                            reconnect );
                    }
                else
                    {
                    //Home network
                    iConfirmDlgRoaming = CMPMConfirmDlgRoaming::NewL( 
                            *this, 
                            snapId, 
                            iMigrateIap, 
                            CMPMConfirmDlg::EConfirmDlgHomeNetwork,
                            reconnect );
                    }
                }
            
            else
                {
                RoamingConfirmationCompletedL( KErrCancel, EMsgQueryCancelled, reconnect );
                }
            }
        //IAP was WLAN IAP
        else
            {
            //Handle like user would have answered "Connect this time" to dialog
            RoamingConfirmationCompletedL( KErrNone, EMsgQueryThisTime, reconnect );
            }
                
        // Release memory
        //
        CleanupStack::PopAndDestroy( &availableIAPList );
        }
    else
        {
        // Handle next state in migration
        //
        MigrateCallbackL( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::MigrateCallbackL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::MigrateCallbackL( TInt aError )
	{
	if( iMigrateState == EMigrateUserConfirmation )
	    {
	    MPMLOGSTRING2( "CMPMServerSession<0x%x>::MigrateCallbackL: State user confirmation",
	                   iConnId )
	    if( aError == KErrNone )
	        {
	        iMigrateState = EMigrateOfflineConfirmation;
    	    if( IapSelectionL()->StartWlanQueryIfNeededL( iMigrateIap, ETrue ) )
	            {
	            return;
                }
	        }
	    }
	else if( iMigrateState == EMigrateOfflineConfirmation )
	    {
	    MPMLOGSTRING3( "CMPMServerSession<0x%x>::MigrateCallbackL: State offline confirmation error: %d", 
	                   iConnId,
	                   aError )
        if( aError != KErrNone )
            {
            ErrorNotificationL( aError, EMPMMobilityErrorNotification );
            }
	    }
	else // EMigrateNone
	    {
        MPMLOGSTRING2( "CMPMServerSession<0x%x>::MigrateCallbackL: State none",
                       iConnId )
	    
	    }
    MigrateDoneL( aError );
	}

// -----------------------------------------------------------------------------
// CMPMServerSession::RoamingConfirmationCompletedL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::RoamingConfirmationCompletedL( TInt                   aError, 
                                                      TMsgQueryLinkedResults aResponse,
                                                      TBool                  aReconnect )
    {
    TInt error( KErrNone);
    MPMLOGSTRING5( "CMPMServerSession<0x%x>::RoamingConfirmationCompleted: \
aError %d, aResponse %d, aReconnect %d", 
                   iConnId,
                   aError,
                   aResponse,
                   aReconnect )

    if( aError == KErrNone )
        {
        if( aResponse == EMsgQueryCancelled )
            {
            if( !aReconnect )
                {
                // Send a preferred IAP notification
                // 
                TConnMonIapInfo availableIAPs;
                availableIAPs = GetAvailableIAPs();

                // Get the current and presumed IapId for this connId 
                //
                TUint32 currentIap  = iMyServer.GetBMIap( ConnectionId() );
                TUint32 presumedIap = MyServer().Events()->PresumedIapId( 
                                          ConnectionId(), iMigrateIap );

                MyServer().HandleServerBlackListIap( ConnectionId(), 
                                                     currentIap, 
                                                     EConnectionLifetime );
                if ( ( presumedIap != 0 ) && 
                     ( presumedIap != currentIap ) )
                    {
                    iMyServer.HandleServerBlackListIap( ConnectionId(), 
                                                        presumedIap, 
                                                        EConnectionLifetime );
                    }

                TRAPD( err, PrefIAPNotificationL( availableIAPs, 
                                                  EConfirmDlgRoaming ) );
                if ( err == KErrNotFound )
                    {
                    // We need to send a notification error to BearerMan 
                    // if sending preferred IAP notifications failed. 
                    // 
                    ErrorNotificationL( KErrNotFound,
                                        EMPMMobilityErrorNotification );

                    }
                }
            else // aReconnect
                {
                ErrorNotificationL( KErrCancel,
                                    EMPMMobilityErrorNotification );
                }
      
            error = KErrCancel;
            }
        //User selected Connect automatically
        else if  ( aResponse == EMsgQueryAutomatically ) 
            {
            //Store selected value to commsdat if we are in home network
            if ( MyServer().RoamingWatcher()->RoamingStatus() == EMPMHomenetwork )
                {
                TCmGenConnSettings genConnSettings;

                TRAPD(errorCode,genConnSettings = MyServer().CommsDatAccess()->ReadGenConnSettingsL()); // call a function

                //If reading of database failed we do not write back to the database to prevent random values
                if (errorCode == KErrNone)
                    {
                    genConnSettings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic;        
                    TRAP_IGNORE(MyServer().CommsDatAccess()->WriteGenConnSettingsL( genConnSettings )); 
                    }
                } 
            }
        
        //user selected connect this time
        else
            {
            MPMLOGSTRING3( "CMPMServerSession<0x%x>::RoamingConfirmationCompleted: \
Unknown response: %d", iConnId, aResponse )
            }
        }
    else // error occurred
        {
        ErrorNotificationL( aError, EMPMMobilityErrorNotification );
        error = KErrCancel;
        }
    MigrateCallbackL( error ); 
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::StopConnection
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::StopConnection()
    { 
    TInt ret = KErrNone;
    MPMLOGSTRING( "CMPMServerSession::StopConnection" )
    
    TConnectionState state( EIdle );
       
    iMyServer.GetConnectionState( iConnId, state );
        
    if ( state == EStarted || state == EStarting )
        {
        TRAP(ret, StopIAPNotificationL());
        }
            
    return ret;
    } 

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationIgnoredTheCarrier
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationIgnoredTheCarrier(
    const RMessage2& aMessage )
    {
    TInt error( KErrNotFound );
    
    // MPM has been waiting for the response to preferred.
    // Release the possible new notifications.
    //
    iStoredIapInfo.ResetStoredIapInfo();
    
    // Read Iap Id
    //
    TUint32 ignoredIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId ignoredId = aMessage.Int1();

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationIgnoredTheCarrier \
- IAP Id = %i, Connection Id = 0x%x", ignoredIap, ignoredId )

    // Blacklisting should be done before ResetConnInfo(), 
    // since iPresumedIapId is reset during ResetConnInfo(). 
    // 
    iMyServer.HandleServerBlackListIap( ignoredId, 
                                        ignoredIap, 
                                        EConnectionLifetime );

    iMyServer.Events()->ResetConnInfo( ignoredId );

    TConnMonIapInfo availableIAPs;
    availableIAPs = GetAvailableIAPs();

    if ( availableIAPs.iCount > 0 ) 
        {
        TRAP( error, PrefIAPNotificationL( availableIAPs, EBearerMan ) );
        if ( error == KErrNotFound )
            {
            TRAP( error, ErrorNotificationL( KErrNotFound,
                                             EMPMMobilityErrorNotification ) );
            }
        }
    else
        {
        MPMLOGSTRING(
            "CMPMServerSession::HandleServerApplicationIgnoredTheCarrier - \
No IAPs available, send error notification" )
        TRAP( error, ErrorNotificationL( KErrNotFound, 
                                         EMPMMobilityErrorNotification ) );
        }
    aMessage.Complete( error );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationAcceptedTheCarrier
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationAcceptedTheCarrier(
    const RMessage2& aMessage )
    {    
    // Read Iap Id
    //
    TUint32 acceptedIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId acceptedId = aMessage.Int1();

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationAcceptedTheCarrier \
- IAP Id = %i, Connection Id = 0x%x", acceptedIap, acceptedId )

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 snapId = iMyServer.GetBMSnap( acceptedId );

    iMyServer.AppendBMConnection( acceptedId, 
                                  snapId, 
                                  acceptedIap, 
                                  EStarted,
                                  *this );

    aMessage.Complete( KErrNone );
    
    // If there is saved IAP info, handle it now
    //
    TConnMonIapInfo iapInfo;
    if( iStoredIapInfo.HandleIapInfoWaiting( iapInfo ) )
        {
        iStoredIapInfo.ResetStoredIapInfo();
        // Attempt to send the preferred IAP notification 
        // if notification has arrived during roaming. 
        //
        TRAPD( error, PrefIAPNotificationL( iapInfo, EConnMon ) );
        if( error != KErrNone )
            {
            MPMLOGSTRING2(
            "CMPMServerSession::HandleServerApplicationAcceptedTheCarrier \
PrefIAPNotification error = %i, ", error )            
            }
        }
    else 
        {
        iStoredIapInfo.ResetStoredIapInfo();
        }
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerApplicationRejectedTheCarrierL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerApplicationRejectedTheCarrierL(
    const RMessage2& aMessage )
    {
    // PrefIAPNotif will be attempted with latest available IAPs
    //
    iStoredIapInfo.ResetStoredIapInfo();

    TInt error( KErrNone );

    // Read Iap Id
    //
    TUint32 rejectedIap = static_cast<TUint32>( aMessage.Int0() );

    // Read the Connection Id of the application
    //
    TConnectionId rejectedId = aMessage.Int1();

    MPMLOGSTRING3(
        "CMPMServerSession::HandleServerApplicationRejectedTheCarrierL \
- IAP Id = %i, Connection Id = 0x%x", rejectedIap, rejectedId )

    // Blacklisting should be done before ResetConnInfo(), 
    // since iPresumedIapId is reset during ResetConnInfo(). 
    // 
    iMyServer.HandleServerBlackListIap( rejectedId, 
                                        rejectedIap, 
                                        EConnectionLifetime );

    iMyServer.Events()->ResetConnInfo( rejectedId );
    iMyServer.ResetBMConnection( rejectedId, rejectedIap, *this );

    // If possible to roam from a WLAN IAP to another WLAN IAP
    // perform WLAN scan first.
    // Currently ConnMon can't send notifications during this time as it 
    // handles requests syncronously. So, that shouldn't cause problems now, 
    // but issue should be reviewed if that changes. 
    //
    TUint32 snapId = iMyServer.GetBMSnap( rejectedId );
    RArray<TUint> iapPath;
    CleanupClosePushL( iapPath );
    if( ( iMyServer.CommsDatAccess()->CheckWlanL( rejectedIap ) != ENotWlanIap ) && 
          iMyServer.CommsDatAccess()->SnapContainsWlanL( snapId, iapPath, KMPMNrWlansTwo ) )
        {
            // perform WLAN scan 
            // message is completed in callback function 
            // CompleteCarrierRejected
            // 
            iMyServer.Events()->ScanWLANNetworksL( this, 
                                                   ConnectionId(), 
                                                   EWlanScanCallbackCarrierRejected );
            CleanupStack::PopAndDestroy( &iapPath );
            aMessage.Complete( error );
            return;    
        }
    else 
        {
        CompleteCarrierRejected();
        }
    CleanupStack::PopAndDestroy( &iapPath );
    aMessage.Complete( error );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::GetServiceIdSettingL
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::GetServiceIdSettingL()
    {
    TUint32 iap( 0 );
    TInt id( KErrNotFound );

    MPMLOGSTRING2( "CMPMServerSession::GetServiceIdSettingL - \
Setting name %S", &KIapProxyServiceSetting() )

    iap = GetPresumedIap();
    if( !iap )
        {
        MPMLOGSTRING( "CMPMServerSession::HandleServerGetIntSetting - \
        Iap for the connection not found" )
        id = KErrNotFound;
        }
    else
        {
        id = iMyServer.CommsDatAccess()->GetIapServiceIdL( iap );
        // validate commsdat value.
        id = id > 0 && id <= 256 ? id : KErrNotFound;
        }
    return id;  
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerReselectIapL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerReselectIapL( const RMessage2& aMessage )
    {
    MPMLOGSTRING2( "CMPMServerSession::HandleServerReselectIapL session <0x%x>", this )
    // Sanity check that client only has one outstanding ChooseBestIAP() or ReselectBestIAP() -call.
    //
    if ( !iChooseIapMessage.IsNull() )
        {
        MPMLOGSTRING2( "CMPMServerSession::HandleServerReselectIapL: - Error \
 ChooseBestIAP() or ReselectBestIAP() already exists %d", KErrServerBusy )
        aMessage.Complete( KErrServerBusy );
        }

    // We must save the RMessage in order to complete it later.
    //
    iChooseIapMessage = aMessage;

    if ( !ChooseBestIapCalled() )
        {
        MPMLOGSTRING( "CMPMServerSession::HandleServerReselectIapL - Error: \
ChooseBestIap hasn't been called yet" )
        aMessage.Complete( KErrNotReady );
        return;
        }

    MPMLOGSTRING3( "CMPMServerSession::HandleServerReselectIapL - iap %d \
connType %d", iIapSelection->MpmConnPref().IapId(), iIapSelection->MpmConnPref().ConnType() )

    iIapSelection->ChooseIapL( iIapSelection->MpmConnPref() );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerProcessErrorL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerProcessErrorL(
    const RMessage2& aMessage )
    {
    // Store message. This is used to complete the message later 
    // if asynchronous services are needed first or in Service error
    iProcessErrorMessage = aMessage;
    
    if ( iIapSelection )
        {
        iIapSelection->StopDisplayingStartingDlg();
        }

    // Read Error code
    //
    TInt error( KErrNone );
    TPtr8 errorPtr( reinterpret_cast< TUint8* >( NULL ), 0 );
    errorPtr.Set( reinterpret_cast< TUint8* >( &error ),
                  sizeof( error ),
                  sizeof( error ) );

    // Read the contents of the client pointer into a TPtr8.
    // 
    TInt res = iProcessErrorMessage.Read( KFirstArgument, errorPtr );
    if ( res != KErrNone )
        {
        iMyServer.PanicClient( KErrBadDescriptor );
        return;
        }

    if ( !ChooseBestIapCalled() )
        {
        MPMLOGSTRING( "CMPMServerSession::HandleServerProcessErrorL - \
Warning: ChooseBestIap has not been called yet" )
        TBMNeededAction neededAction( EPropagateError );
        ProcessErrorComplete( KErrNone, &error, &neededAction );
        return;
        }

    // Show error popup if it's allowed per client request
    // Don't show the pop up if error code is for disconnect dialog
    //
    if ( !( iIapSelection->MpmConnPref().NoteBehaviour() &
            TExtendedConnPref::ENoteBehaviourConnDisableNotes ) &&
         !DisconnectDlgErrorCode( error ) )
        {
        CConnectionUiUtilities* connUiUtils = NULL;
        TRAPD( popupCreateError, connUiUtils = CConnectionUiUtilities::NewL() );
        if ( popupCreateError == KErrNone && connUiUtils )
            {
            // Note: Below function shows the discreet popup only if the error code
            // belongs to the set of errors that are shown to the user.
            // Otherwise the popup is not shown.
            connUiUtils->ConnectionErrorDiscreetPopup( error );
            delete connUiUtils;
            connUiUtils = NULL;
            }
        }

    // Read the Connection Id of the application
    // 
    TConnectionId connId = iProcessErrorMessage.Int1();

    MPMLOGSTRING3( "CMPMServerSession::HandleServerProcessErrorL\
 - error code = %i, Connection Id = 0x%x", error, connId )

    // Get the current connection IapId for this connId 
    //
    TUint32 currentIap = iMyServer.GetBMIap( connId );

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 snapId = iMyServer.GetBMSnap( connId );

    TConnMonIapInfo availableIAPs;
    availableIAPs = GetAvailableIAPs();

    RemoveUnavailableIap( availableIAPs, currentIap );

    TBMNeededAction neededAction( EIgnoreError );

    // Get the state of the connection for this Iap Id.
    // 
    TConnectionState state;
    iMyServer.GetConnectionState( connId, state );

    // We need to blacklist the presumed IAP too
    // 
    TUint32 presumedIap = MyServer().Events()->PresumedIapId( connId, 
                                                              currentIap );

    // Reset connection info.
    // 
    iMyServer.Events()->ResetIapConnInfo( currentIap );

    // Check if IAP is reported by MMS
    //
    TBool isMMSIap = IsMMSIap( currentIap );
    if ( isMMSIap )
        {
        MPMLOGSTRING( "CMPMServerSession::HandleServerProcessErrorL\
        - DisconnectDialog is not started because of MMS reported IAP" )
        }

    TInt* returnError( NULL );
    if ( ( state == EStarting ) || ( state == ERoaming ) )
        {
        // Process error according to the fact that the connection 
        // has not yet been started.
        // 
        if ( DisconnectDlgErrorCode( error ) &&
             !IsBackgroundApplication( iAppUid ) &&
             !isMMSIap && 
             iIapSelection->MpmConnPref().DisconnectDialog() &&
             iMyServer.StartedConnectionExists() != KErrNotFound )
            {
            // Start the Disconnect dialog
            // 
            MPMLOGSTRING( "CMPMServerSession::HandleServerProcessErrorL\
 - Start Disconnect dialog" )
            iDisconnectDlg = CMPMDisconnectDlg::NewL( *const_cast<CMPMServerSession*>(this),
                                                      error,
                                                      *MyServer().DisconnectQueue() );
            iDisconnectDialogShown = ETrue;
            return;
            }
        else
            {
            MPMLOGSTRING( "CMPMServerSession::HandleServerProcessErrorL - \
Error not handled with disconnect dialog" )

            if ( state == EStarting ) 
                {
                if ( ( snapId == 0 ) || ( error == KErrCancel ) )
                    {
                    neededAction = EPropagateError;

                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )
                    }
                else
                    {
                    neededAction = EDoReselection;

                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to ignore error and do reselection" )

                    iMyServer.HandleServerBlackListIap( connId, 
                                                        currentIap, 
                                                        ETemporary );
                    if ( ( presumedIap != 0 ) && 
                         ( presumedIap != currentIap ) )
                        {
                        iMyServer.HandleServerBlackListIap( connId, 
                                                            presumedIap, 
                                                            ETemporary );
                        }
                    }
                }
            else if ( state == ERoaming ) 
                {
                // ERoaming means commsfw stack is moving to new IAP and failed.
                // Hence, MPM should mark to current iap as zero. 
                //
                iMyServer.ResetBMConnection( iConnId, currentIap, *this );
                
                // Notification will be sent with latest 
                // availability info
                //
                iStoredIapInfo.ResetStoredIapInfo();

                neededAction = EIgnoreError;

                iMyServer.HandleServerBlackListIap( connId, 
                                                    currentIap, 
                                                    ETemporary );
                if ( ( presumedIap != 0 ) && 
                     ( presumedIap != currentIap ) )
                    {
                    iMyServer.HandleServerBlackListIap( connId, 
                                                        presumedIap, 
                                                        ETemporary );
                    }
                TRAP( error, PrefIAPNotificationL( availableIAPs, 
                                                   EBearerMan ) );
                if ( error == KErrNotFound )
                    {
                    neededAction = EPropagateError;

                    returnError = &error;

                    TRAP_IGNORE( ErrorNotificationL( KErrNotFound, 
                                                     EMPMMobilityErrorNotification ) );
                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )
                    }
                else
                    {
                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to ignore error and let MPM notify application about preferred IAP" )
                    }
                }
            else
                {
                MPMLOGSTRING2(
                    "CMPMServerSession::HandleServerProcessErrorL - \
Unknown state %d", state )
                }

            // Error might be different from KErrNone if there 
            // is no preferred IAP among the available IAPs.
            // 
            ProcessErrorComplete( KErrNone,
                                  returnError,
                                  &neededAction );
            return;
            }
        }
    else if ( state == EStarted )
        {
        // Process error according to the fact that the connection 
        // has already been started.
        //         
        if ( ( error == KErrCancel ) || ( error == KErrTimedOut ) )
            {
            neededAction = EPropagateError;

            MPMLOGSTRING(
                "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )

            // Send error notification. 
            // Not sent if connection not registered
            // 
            TRAP_IGNORE( ErrorNotificationL( error,
                                             EMPMMobilityErrorNotification ) )
            }
        else if ( iPreferredIAPRequested )
            {
            // IAP connection
            //
            if( snapId == 0 )
                {
                neededAction = EPropagateError;
    
                MPMLOGSTRING(
                    "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )

                TRAP_IGNORE( ErrorNotificationL( KErrNotFound,
                                                 EMPMMobilityErrorNotification ) )
                }
            // SNAP connection
            //
            else
                {
                // If this has been WLAN IAP and the SNAP contains 
                // other WLAN IAPs, we need to perform WLAN scan before
                // knowing the availability of those
                //
                RArray<TUint> iapPath;
                CleanupClosePushL( iapPath );

                iMyServer.HandleServerBlackListIap( connId, 
                                                    currentIap, 
                                                    ETemporary );
                if ( ( presumedIap != 0 ) && 
                     ( presumedIap != currentIap ) )
                    {
                    iMyServer.HandleServerBlackListIap( connId, 
                                                        presumedIap, 
                                                        ETemporary );
                    }

                // current iap is either WLAN or EasyWlan
                // 
                if( ( iMyServer.CommsDatAccess()->CheckWlanL( currentIap ) != ENotWlanIap ) && 
                      iMyServer.CommsDatAccess()->SnapContainsWlanL( snapId, iapPath, KMPMNrWlansTwo ) )
                    {
                    // perform WLAN scan 
                    // message is completed in callback function 
                    // ProcessErrorWlanScanCompletedL
                    // 
                    iMyServer.Events()->ScanWLANNetworksL( this, 
                                                           ConnectionId(), 
                                                           EWlanScanCallbackProcessErr );
                    CleanupStack::PopAndDestroy( &iapPath );
                    return;
                    }

                CleanupStack::PopAndDestroy( &iapPath );
                neededAction = EIgnoreError;

                TRAPD( err2, PrefIAPNotificationL( availableIAPs, EBearerMan ) );
                if ( err2 == KErrNotFound )
                    {
                    error = err2;
                    neededAction = EPropagateError;

                    TRAP_IGNORE( ErrorNotificationL( KErrNotFound,
                                                     EMPMMobilityErrorNotification ) )
                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )   
                    }
                else
                    {
                    MPMLOGSTRING(
                        "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to ignore error and let MPM notify application about preferred IAP" )
                    }
                }
            }
        else
            {
            neededAction = EPropagateError;

            MPMLOGSTRING(
                "CMPMServerSession::HandleServerProcessErrorL - \
Tell BM to end the client connection with appropriate error code" )

            }
        ProcessErrorComplete( KErrNone, &error, &neededAction );
        
        }
    else
        {
        MPMLOGSTRING2(
            "CMPMServerSession::HandleServerProcessErrorL Unknown state %d",
            state )
        ProcessErrorComplete( KErrCorrupt,
                              NULL,
                              NULL );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerRegisterPrefIAPNotifL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerRegisterPrefIAPNotifL(
    const RMessage2& aMessage )
    {
    MPMLOGSTRING( "CMPMServerSession::HandleServerRegisterPrefIAPNotifL" )

    // Read the Connection Id of the application
    //
    TConnectionId regId = aMessage.Int0();
    MPMLOGSTRING2( "CMPMServerSession::HandleServerRegisterPrefIAPNotifL \
- regId = 0x%x", regId )

    // Get the current connection SNAP for this Connection Id
    //
    TUint32 regNetId = iMyServer.GetBMSnap( regId );

    // Get the current connection IapId for this Connection Id 
    //
    TUint32 currentIap = iMyServer.GetBMIap( regId );
    MPMLOGSTRING2( "CMPMServerSession::HandleServerRegisterPrefIAPNotifL \
- currentIap = %i", currentIap)

    // Check that there is a connection using 
    // either IAP or SNAP
    // 
    if ( regNetId == 0 && currentIap == 0 )
        {
        MPMLOGSTRING( "CMPMServerSession::\
        HandleServerRegisterPrefIAPNotifL - return KErrNotSupported" )
        aMessage.Complete( KErrNotSupported );
        return;
        }

    // Append info about registration
    iPreferredIAPRequested = ETrue;

    TConnMonIapInfo availableIAPs;
    availableIAPs = GetAvailableIAPs();

    MPMLOGSTRING2( "CMPMServerSession::HandleServerRegisterPrefIAPNotifL \
- IAPs count: %d", availableIAPs.iCount)

#ifdef _DEBUG
    for (TUint i = 0; i < availableIAPs.Count(); i++)
        {
        MPMLOGSTRING2(
                "CMPMServerSession::HandleServerRegisterPrefIAPNotifL \
- IAP: %d", availableIAPs.iIap[i].iIapId)
        }
#endif // _DEBUG

    // Call now the method that handles notifications for
    // checking if notification is already triggered
    // If SNAP is 0, don't try sending notifications 
    //
    if ( availableIAPs.iCount > 0 && regNetId )
        {
        PrefIAPNotificationL( availableIAPs, EBearerMan );
        }

    // In case the mobility application register to preferred IAP notification
    // we have to make sure we get availability every once in a while.
    // 
    RArray<TUint> iapPath;
    CleanupClosePushL( iapPath );
    CleanupStack::PopAndDestroy( &iapPath );
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerUnregisterPrefIAPNotif
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerUnregisterPrefIAPNotif(
    const RMessage2& aMessage )
    {
    // Read the Connection Id of the application
    //
    TConnectionId unregId = aMessage.Int0();
    MPMLOGSTRING2( "CMPMServerSession::HandleServerUnregisterPrefIAPNotif\
 - unregId = 0x%x", unregId )

    // Currently will remove all registration for this Connection Id. 
    // If needed change BM-MPM API to support unregistration for certain SNAP
    //
    iPreferredIAPRequested = EFalse;
    iLastNotifiedIap = 0;
    // Unblacklist all IAPs related to this connection 
    // when unregistering from preferred IAP notification.
    // 
    iMyServer.HandleServerUnblackListIap( unregId, 0 );

    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerWaitNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerWaitNotificationL(
    const RMessage2& aMessage )
    {
    MPMLOGSTRING2( "CMPMServerSession::HandleServerWaitNotification - \
iNotifRequested = %i", iNotifRequested )

    if ( !iNotifRequested )
        {
        // Save message for later completion
        //
        iNotifMessage = aMessage;

        // Set requested flag
        //
        iNotifRequested = ETrue;
        }
    else
        {
        aMessage.Complete( KErrServerBusy );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::HandleServerSortSNAPL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::HandleServerSortSNAPL( const RMessage2& aMessage )
    {
    MPMLOGSTRING( "CMPMServerSession::HandleServerSortSNAPL()" )
    if ( !iServerSortSNAPMessage.IsNull() )
        {
        MPMLOGSTRING2(
        "CMPMServerSession::HandleServerSortSNAPL: Failed with: %d",KErrNotReady )

        // Error, only one request of each type per session.
        aMessage.Complete( KErrNotReady );
        }
    
        // Store message for later usage.
    iServerSortSNAPMessage = aMessage;

    TUint32 seconds = static_cast<TUint32>( iServerSortSNAPMessage.Int2() );
    
    // To display up to date information the WLAN scan should be done first
    iMyServer.Events()->ScanWLANNetworksL( this, 
                                           ConnectionId(), 
                                           EWlanScanCallbackSortSnap, 
                                           seconds );
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::CompleteServerSortSNAPL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::CompleteServerSortSNAP()
    {
    if ( iServerSortSNAPMessage.IsNull() )
        {
        return;
        }

    // Read SNAP
    //
    TUint32 snapId = static_cast<TUint32>( iServerSortSNAPMessage.Int0() );

    MPMLOGSTRING2( "CMPMServerSession::CompleteServerSortSNAPL - SNAP = %i", 
        snapId )

    TMpmSnapBuffer sortedIapList;

    TRAPD( err, SortSnapL( snapId, sortedIapList ) )
    if ( err != KErrNone )
        {
        MPMLOGSTRING2(
            "CMPMServerSession::CompleteServerSortSNAPL: Sorting failed with: %d", err )
        iServerSortSNAPMessage.Complete( err );
        return;
        }

    // Write results to message
    // 
    TPtrC8 e( reinterpret_cast< TUint8* >( &sortedIapList ), 
              sizeof( sortedIapList ) );
    TRAP( err, iServerSortSNAPMessage.WriteL( KSecondArgument, e ) )
    if ( err != KErrNone )
        {
        MPMLOGSTRING2(
            "CMPMServerSession::CompleteServerSortSNAPL: RMessage.WriteL() Failure: %d", err )
        iServerSortSNAPMessage.Complete( err );
        return;
        }

    MPMLOGSTRING(
        "CMPMServerSession::CompleteServerSortSNAPL: Sorting available uncategorised IAPs completed" )
    iServerSortSNAPMessage.Complete( KErrNone );
    return;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ProcessErrorWlanScanCompletedL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ProcessErrorWlanScanCompletedL()
    {
    MPMLOGSTRING( "CMPMServerSession::ProcessErrorWlanScanCompletedL" )
    TBMNeededAction neededAction( EIgnoreError );

    // Read the Connection Id of the application
    // 
    TConnectionId connId = iProcessErrorMessage.Int1();

    // Read Error code
    //
    TInt error( KErrNone);
    
    TPtr8 errorPtr( reinterpret_cast< TUint8* >( NULL ), 0 );
    errorPtr.Set( reinterpret_cast< TUint8* >( &error ),
                  sizeof( error ),
                  sizeof( error ) );

    // Read the contents of the client pointer into a TPtr8.
    // 
    TInt res = iProcessErrorMessage.Read( KFirstArgument, errorPtr );
    if ( res != KErrNone )
        {
        iMyServer.PanicClient( KErrBadDescriptor );
        return;
        }

    // Get the current connection IapId for this connId 
    //
    TUint32 currentIap = iMyServer.GetBMIap( connId );

    // get available iaps
    TConnMonIapInfo availableIAPs;
    availableIAPs = GetAvailableIAPs();

    RemoveUnavailableIap( availableIAPs, currentIap );

    TRAPD( err2, PrefIAPNotificationL( availableIAPs, EBearerMan ) );
    if ( err2 == KErrNotFound )
        {
        error = err2;
        neededAction = EPropagateError;

        TRAP_IGNORE( ErrorNotificationL( KErrNotFound,
                                         EMPMMobilityErrorNotification ) )
       MPMLOGSTRING( "CMPMServerSession::ProcessErrorWlanScanCompletedL - \
Tell BM to end the client connection with appropriate error code" )
       }
    else
       {
       MPMLOGSTRING( "CMPMServerSession::ProcessErrorWlanScanCompletedL - \
Tell BM to ignore error and let MPM notify application about preferred IAP" )
        }
        
    ProcessErrorComplete( KErrNone, &error, &neededAction );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::SortSnapL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::SortSnapL( const TUint32 aSnapId, 
                                   TMpmSnapBuffer& aSortedIaps )
    {
    MPMLOGSTRING2( "CMPMServerSession::SortSnapL - SNAP = %i", 
        aSnapId )

    TBool hideIap( EFalse ); 

    // In case snapId is zero, return the 
    // list of available uncategorised IAPs 
    // 
    if ( aSnapId == KSortUncategorisedIaps )
        {
        RAvailableIAPList availableUncatIAPList;
        CleanupClosePushL( availableUncatIAPList );
        AvailableUnblacklistedIapsL( availableUncatIAPList, iConnId );

        iMyServer.CommsDatAccess()->RemoveCategorisedIapsL( availableUncatIAPList );

        aSortedIaps.Reset();

        for ( TInt i( 0 ); ( (i < availableUncatIAPList.Count()) ); i++ )
            {
            // Check first if metadata EMetaHiddenAgent is enabled.
            // 
            hideIap = iMyServer.CommsDatAccess()->CheckHiddenAgentL( availableUncatIAPList[i] );
            if ( hideIap )
                {
                // In that case IAP can't be shown in Connection Dialog.
                // 
                MPMLOGSTRING2( 
                    "CMPMServerSession::SortSnapL: Remove HiddenAgent IAP = %i", 
                    availableUncatIAPList[i] )
                }
            else
                {
                aSortedIaps.iIapId[aSortedIaps.iCount] = availableUncatIAPList[i];
                aSortedIaps.iCount++;
                }
            }

#ifdef _DEBUG
        // Print info into the log file
        //
        MPMLOGSTRING( "CMPMServerSession::SortSnapL: Sorted IAPs" )
        for ( TInt p = 0; p < aSortedIaps.Count(); p++ )
            {
            MPMLOGSTRING2(
                "CMPMServerSession::SortSnapL: IapId = %i", 
                aSortedIaps.iIapId[p] )
            }
#endif // _DEBUG

        // Release memory
        //
        CleanupStack::PopAndDestroy( &availableUncatIAPList );
        return;
        }

    RArray<TNetIap> destNetIaps, embeddedIaps;
    CleanupClosePushL( destNetIaps );
    CleanupClosePushL( embeddedIaps );

    // Find the valid IAPs belonging to the Snap.
    //
    iMyServer.CommsDatAccess()->SearchDNEntriesWithEmbeddedL( aSnapId, destNetIaps, embeddedIaps );

    RAvailableIAPList availableIAPList;
    CleanupClosePushL( availableIAPList );
    AvailableUnblacklistedIapsL( availableIAPList, iConnId );

    // Remove any unavailable IAP from destNetIaps
    // 
    TInt ret        = KErrNotFound;
    TInt destCount  = destNetIaps.Count();
    
    // Decrement by one, because count is n, 
    // but indexes in array are 0 .. n-1.
    // 
    destCount--;

    // This time we are browsing the array from the end to the beginning, 
    // because removing one element from array affects index numbering.
    // 
    for ( TInt k = destCount; k >= 0; k-- )
        {
        ret = availableIAPList.Find( destNetIaps[k].iIapId );
        if ( ret == KErrNotFound )
            {
            MPMLOGSTRING2( "CMPMServerSession::SortSnapL: \
Remove unavailable IAP = %i", destNetIaps[k].iIapId )
            destNetIaps.Remove( k );
            }
        else
            {
            // Check first if metadata EMetaHiddenAgent is enabled.
            // 
            hideIap = iMyServer.CommsDatAccess()->CheckHiddenAgentL( destNetIaps[k].iIapId );
            if ( hideIap )
                {
                // In that case IAP can't be shown in Connection Dialog.
                // 
                MPMLOGSTRING2( 
                    "CMPMServerSession::SortSnapL: Remove HiddenAgent IAP = %i", 
                    destNetIaps[k].iIapId )
                destNetIaps.Remove( k );
                }
            }
        }

    // Remove any unavailable IAP from embeddedIaps
    // 
    if ( embeddedIaps.Count() > 0 )
        {
        TInt embedCount = embeddedIaps.Count();
        embedCount--;

        for ( TInt m = embedCount; m >= 0; m-- )
            {
            ret = availableIAPList.Find( embeddedIaps[m].iIapId );
            if ( ret == KErrNotFound )
                {
                MPMLOGSTRING2( "CMPMServerSession::SortSnapL: \
Remove unavailable IAP = %i", embeddedIaps[m].iIapId )
                embeddedIaps.Remove( m );
                }
            else
                {
                // Check first if metadata EMetaHiddenAgent is enabled.
                // 
                hideIap = iMyServer.CommsDatAccess()->CheckHiddenAgentL( embeddedIaps[m].iIapId );
                if ( hideIap )
                    {
                    // In that case IAP can't be shown in Connection Dialog.
                    // 
                    MPMLOGSTRING2( 
                        "CMPMServerSession::SortSnapL: Remove HiddenAgent IAP = %i", 
                        embeddedIaps[m].iIapId )
                    embeddedIaps.Remove( m );
                    }
                }
            }
        }

    // Determine the actual priorities for virtual IAPs and embedded Snaps
    // 
    iMyServer.CommsDatAccess()->DeterminePrioritiesL( destNetIaps, availableIAPList,
                                                      *this );
    if ( embeddedIaps.Count() > 0 )
        {
        iMyServer.CommsDatAccess()->DeterminePrioritiesL( embeddedIaps,
                                                          availableIAPList,
                                                          *this );
        }

    aSortedIaps.Reset();

    // Start sorting destNetIaps and embeddedIaps.
    // 
    for( TInt p = 0; ( destNetIaps.Count()  > 0 ) || 
                     ( embeddedIaps.Count() > 0 ); p++ )
        {
        // Go through the destNetIaps-array and check the global bearer 
        // priority for both destNetIaps and embeddedIaps until the best 
        // available IAP is found.
        // 
        TUint32 destNetPriority( KLowestPriority );
        TUint32 embeddedPriority( KLowestPriority );

        if( destNetIaps.Count() > 0 )
            {
            iMyServer.CommsDatAccess()->GlobalBearerPriorityL( destNetIaps[0].iIapId, 
                                   destNetPriority );
            }

        if( embeddedIaps.Count() > 0 )
            {
            iMyServer.CommsDatAccess()->GlobalBearerPriorityL( embeddedIaps[0].iIapId, 
                                   embeddedPriority );
            }

        // Compare the global bearer priorities of the first IAPs and 
        // select the better. 
        // 
        // If the priorities are equal, prioritize the Snap 
        // over the embedded Snap.
        // 
        // When comparing the priorities, smaller value is better.
        // 
        // Finally, append IapId to sortedList.
        //
        if( destNetPriority <= embeddedPriority )
            {
            if( destNetIaps.Count() > 0 )
                {
                aSortedIaps.iIapId[p] = destNetIaps[0].iIapId;
                aSortedIaps.iCount++;
                MPMLOGSTRING2(
                    "CMPMServerSession::SortSnapL: IapId = %i", 
                    destNetIaps[0].iIapId )
                MPMLOGSTRING2(
                    "CMPMServerSession::SortSnapL: Snap  = %i", 
                    destNetIaps[0].iSnap )
                destNetIaps.Remove( 0 );
                }
            }
        else
            {
            if( embeddedIaps.Count() > 0 )
                {
                aSortedIaps.iIapId[p] = embeddedIaps[0].iIapId;
                aSortedIaps.iCount++;
                MPMLOGSTRING2(
                    "CMPMServerSession::SortSnapL: IapId = %i", 
                    embeddedIaps[0].iIapId )
                MPMLOGSTRING2(
                    "CMPMServerSession::SortSnapL: Snap  = %i", 
                    embeddedIaps[0].iSnap )
                embeddedIaps.Remove( 0 );
                }
            }
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( &availableIAPList );
    CleanupStack::PopAndDestroy( &embeddedIaps );
    CleanupStack::PopAndDestroy( &destNetIaps );

#ifdef _DEBUG
    // Print info into the log file
    //
    MPMLOGSTRING( "CMPMServerSession::SortSnapL: Sorted IAPs" )
    for ( TInt n = 0; n < aSortedIaps.Count(); n++ )
        {
        MPMLOGSTRING2(
            "CMPMServerSession::SortSnapL: IapId = %i", 
            aSortedIaps.iIapId[n] )
        }
#endif // _DEBUG

    MPMLOGSTRING(
            "CMPMServerSession::SortSnapL: Sorting completed" )
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::UpdateConnectionDialog
// -----------------------------------------------------------------------------
//
void CMPMServerSession::UpdateConnectionDialogL()
    {
    if( iIapSelection )
        {
        iIapSelection->UpdateConnectionDialogL();
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::CompleteCarrierRejected
// -----------------------------------------------------------------------------
//
void CMPMServerSession::CompleteCarrierRejected()
    {
    TInt error( KErrNotFound );

    MPMLOGSTRING( "CMPMServerSession::CompleteCarrierRejected" )

    TConnMonIapInfo availableIAPs;
    availableIAPs = GetAvailableIAPs();

    if ( availableIAPs.iCount > 0 ) 
        {
        TRAP( error, PrefIAPNotificationL( availableIAPs, EBearerMan ) );
        if ( error == KErrNotFound )
            {
            TRAP( error, ErrorNotificationL( KErrNotFound,
                                             EMPMMobilityErrorNotification ) );
            }
        }
    else
        {
        MPMLOGSTRING(
            "CMPMServerSession::CompleteCarrierRejected - \
No IAPs available, send error notification" )
        TRAP( error, ErrorNotificationL( KErrNotFound,
                                         EMPMMobilityErrorNotification ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ExtractConnPref
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ExtractConnPref(
    const TConnPref& aBasePref,
    TMpmConnPref& aMpmConnPref ) const
    {
    TInt error( KErrNone );

    // Extract connection preferences from TConnPref to TMpmConnPref
    // based on the type of the connection preferences.
    //
    switch ( aBasePref.ExtensionId() )
        {
        case TConnPref::EConnPrefCommDb: // TCommDbConnPref
            {
            MPMLOGSTRING(
                "CMPMServerSession::ExtractConnPref - EConnPrefCommDb" )
            error = ExtractConnPrefCommDb( aBasePref, aMpmConnPref );
            break;
            }
        case TConnPref::EConnPrefCommDbMulti: // TCommDbMultiConnPref
            {
            MPMLOGSTRING(
                "CMPMServerSession::ExtractConnPref - EConnPrefCommDbMulti" )
            error = ExtractConnPrefCommDbMulti( aBasePref, aMpmConnPref );
            break;
            }
        case TConnPref::EConnPrefSnap: // TConnSnapPref
            {
            MPMLOGSTRING(
                "CMPMServerSession::ExtractConnPref - EConnPrefSnap" )
            error = ExtractConnPrefSnap( aBasePref, aMpmConnPref );
            break;
            }
        case TMpmConnPref::EConnPrefMpm: // TMpmConnPref
            {
            MPMLOGSTRING(
                "CMPMServerSession::ExtractConnPref - EConnPrefMpm" )
            error = ExtractConnPrefMpm( aBasePref, aMpmConnPref );
            break;
            }
        case TConnPref::EConnPrefUnknown: // Default connection
            {
            MPMLOGSTRING( "CMPMServerSession::ExtractConnPref - \
EConnPrefUnknown overriding to default conn" )
            break;
            }
        default:
            {
            MPMLOGSTRING( "CMPMServerSession::ExtractConnPref - \
Unrecognised connection preference type" )
            error = KErrArgument;
            }
        }

    ResolveConnType( aMpmConnPref );

    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
BearerSet = %d", aMpmConnPref.BearerSet());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
ConnSelectionDialog = %d", aMpmConnPref.ConnSelectionDialog());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
ConnType = %d", aMpmConnPref.ConnType());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
DisconnectDialog = %d", aMpmConnPref.DisconnectDialog());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
ForcedRoaming = %d", aMpmConnPref.ForcedRoaming() );
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
IapId = %d", aMpmConnPref.IapId());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
MandateIap = %d", aMpmConnPref.MandateIap());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
NetId = %d", aMpmConnPref.NetId());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
NoteBehaviour = %d", aMpmConnPref.NoteBehaviour());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
SnapId = %d", aMpmConnPref.SnapId());
    
    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
SnapPurpose = %d", aMpmConnPref.SnapPurpose());

    MPMLOGSTRING2( "CMPMServerSession::ExtractConnPref - \
Error code in the end: %d", error )
    return error;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ExtractConnPrefCommDb
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ExtractConnPrefCommDb(
    const TConnPref& aBasePref,
    TMpmConnPref& aMpmConnPref ) const
    {
    ASSERT( aBasePref.ExtensionId() == TConnPref::EConnPrefCommDb );
    TInt error( KErrNone );

    // Cast down to a TCommDbConnPref
    TCommDbConnPref commDbPref;
    commDbPref = TCommDbConnPref::Cast( aBasePref );

    // This is an old sanity check which is not complete but we cannot
    // expand it to keep backwards compatibility.
    if ( commDbPref.IapId() == 0 && commDbPref.NetId() != 0 &&
         commDbPref.DialogPreference() != ECommDbDialogPrefPrompt )
        {
        error = KErrArgument;
        }
    else
        {
        // Map dialog preference. Only request for connection selection
        // dialog is needed as other values either match the default values of TMpmConnPref
        // or are ignored (ECommDbDialogPrefWarn and
        // ECommDbDialogPrefPromptIfWrongMode).
        if ( commDbPref.DialogPreference() == ECommDbDialogPrefPrompt )
            {
            aMpmConnPref.SetConnSelectionDialog( ETrue );
            }
        else
            {
            // Map IAP id.
            aMpmConnPref.SetIapId( commDbPref.IapId() );
            }

        // commDbPref.NetId() is ignored as it's retrieved by MPM based on
        // final selection.
        // commDbPref.Direction() is ignored as it's not used in MPM.

        // Transfer bearerset from aCommDbPref to aMpmConnPref
        // in case 'prompt from user' or 'explicit SNAP requested'.
        // If only IAP is given, bearer set is ignored and not set.
        // If bearer set is set for IAP connection here, implementation of
        // "WLAN Only" setting feature will prevent cellular IAP connection
        // from happening
        if ( commDbPref.DialogPreference() != ECommDbDialogPrefUnknown &&
             ( commDbPref.IapId() == 0 ||
               commDbPref.DialogPreference() == ECommDbDialogPrefPrompt ) )
            {
            if ( commDbPref.BearerSet() & ECommDbBearerWLAN )
                {
                aMpmConnPref.SetBearerSet( aMpmConnPref.BearerSet() | 
                        TExtendedConnPref::EExtendedConnBearerWLAN );
                }

            if ( commDbPref.BearerSet() & ECommDbBearerWcdma ||
                 commDbPref.BearerSet() & DeprecatedCDMA2000 )
                {
                aMpmConnPref.SetBearerSet( aMpmConnPref.BearerSet() | 
                        TExtendedConnPref::EExtendedConnBearerCellular );
                }
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ExtractConnPrefCommDbMulti
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ExtractConnPrefCommDbMulti(
    const TConnPref& aBasePref,
    TMpmConnPref& aMpmConnPref ) const
    {
    ASSERT( aBasePref.ExtensionId() == TConnPref::EConnPrefCommDbMulti );
    TInt error( KErrNone );

    // Cast down to a TCommDbMultiConnPref
    TCommDbMultiConnPref multiPrefs;
    multiPrefs = TCommDbMultiConnPref::Cast( aBasePref );

    // Try to retrieve the connection preference for the first attempt
    TCommDbConnPref commDbPref;
    error = multiPrefs.GetPreference( 1, commDbPref );

    if ( error == KErrNone )
        {
        error = ExtractConnPrefCommDb( commDbPref, aMpmConnPref );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ExtractConnPrefSnap
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ExtractConnPrefSnap(
    const TConnPref& aBasePref,
    TMpmConnPref& aMpmConnPref ) const
    {
    ASSERT( aBasePref.ExtensionId() == TConnPref::EConnPrefSnap );

    // Cast down to a TConnSnapPref
    TConnSnapPref snapPref;
    snapPref = *reinterpret_cast<TCommSnapPref*>(
            const_cast<TConnPref*>( &aBasePref ) );

    // Check whether old net id, new snap id, or zero is given.
    // If snap id equals to zero, doing nothing here implies default
    // connection is used. We cannot give error in this case because it has
    // worked as default connection request earlier.
    TInt snapid(0);
    TInt error( KErrNone );
    if ( snapPref.Snap() > 0 &&
         snapPref.Snap() <= 0x100 )
        {
        // Old Destination network id passed. Convert to SNAP id.
        TRAP( error,
              snapid = iMyServer.CommsDatAccess()->MapNetIdtoSnapAPL(
                  snapPref.Snap() ) );
        }
    else if ( snapPref.Snap() != 0 )
        {
        // Snap id passed.
        snapid = snapPref.Snap();
        }

    aMpmConnPref.SetSnapId( snapid );

    return error;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ExtractConnPrefMpm
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ExtractConnPrefMpm(
    const TConnPref& aBasePref,
    TMpmConnPref& aMpmConnPref ) const
    {    
    ASSERT( aBasePref.ExtensionId() == TMpmConnPref::EConnPrefMpm );
    TInt error( KErrNone );

    aMpmConnPref = *reinterpret_cast<TMpmConnPref*>(
        const_cast<TConnPref*>( &aBasePref ) );

    // Validate connection preferences and if they are valid, resolve needed
    // information into more practical format for MPM.
    error = ValidateExtendedConnPref( aMpmConnPref );
    if ( error == KErrNone )
        {
        // Find the SNAP id based on SNAP purpose.
        CMManager::TSnapPurpose aSnapPurpose = aMpmConnPref.SnapPurpose();
        TInt error = KErrNone;
        
        // MPM searches SnapId for requested purpose by going through all
        // destinations in CommsDat.
        if ( aSnapPurpose != CMManager::ESnapPurposeUnknown )
            {
            TUint32 snapId(0);
            TRAP( error, snapId = iMyServer.CommsDatAccess()->DestinationIdL(
                                      aSnapPurpose ) );
            if ( error == KErrNone )
                {
                aMpmConnPref.SetSnapId( snapId );
                }
            }

        // Mark IAP as mandated if IAP id is given. 
        if ( aMpmConnPref.IapId() != 0 )
            {
            aMpmConnPref.SetMandateIap( ETrue );
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ValidateExtendedConnPref
// 
// Makes a sanity check to the custom preferences and return KErrArgument
// if there are illegal combinations in the connection preferences
// -----------------------------------------------------------------------------
//
TInt CMPMServerSession::ValidateExtendedConnPref(
    TMpmConnPref& aMpmConnPref ) const
    {     
    // If iap id is set, neither snap nor bearer set can be defined
    if ( aMpmConnPref.IapId() != 0 )
        {
        if ( ( aMpmConnPref.SnapId() != 0 ) ||
             ( aMpmConnPref.BearerSet() !=
               TExtendedConnPref::EExtendedConnBearerUnknown ) )
            {
            return KErrArgument;
            }
        }
    
    // If snap purpose is set, then neither iap nor snap id should be defined
    if ( aMpmConnPref.SnapPurpose() != CMManager::ESnapPurposeUnknown )
        {
        if ( ( aMpmConnPref.IapId() != 0 )  || 
             ( aMpmConnPref.SnapId() != 0 ) )
            {
            return KErrArgument;
            }
        }
    
    // If selection dialog is enabled, Snap id, iap id and snap purpose
    // should not be set
    if ( aMpmConnPref.ConnSelectionDialog() )
        {
        if ( ( aMpmConnPref.SnapId() != 0 )  || 
             ( aMpmConnPref.IapId() != 0 ) || 
             ( aMpmConnPref.SnapPurpose() != CMManager::ESnapPurposeUnknown ) )
            {
            return KErrArgument;
            }
        }
    
    // Check that the given enumerations are within the enumeration ranges
    if ( ( aMpmConnPref.SnapPurpose() < CMManager::ESnapPurposeUnknown ) ||
         ( aMpmConnPref.SnapPurpose() > CMManager::ESnapPurposeIntranet ) ||
         ( aMpmConnPref.NoteBehaviour() >
           TExtendedConnPref::ENoteBehaviourConnSilent ) ||
         ( aMpmConnPref.BearerSet() > 
               ( TExtendedConnPref::EExtendedConnBearerCellular + 
                 TExtendedConnPref::EExtendedConnBearerWLAN ) ) )
        {
        return KErrArgument;
        }
    
    // Check that one of SNAP purpose, SNAP id, IAP id or Connection selection
    // dialog is given
    if ( ( aMpmConnPref.SnapPurpose() == CMManager::ESnapPurposeUnknown ) &&
         ( aMpmConnPref.SnapId() == 0 ) &&
         ( aMpmConnPref.IapId() == 0 ) &&
         ( aMpmConnPref.ConnSelectionDialog() == EFalse ) )
        {
        return KErrArgument;
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ResolveConnType
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ResolveConnType( TMpmConnPref& aMpmConnPref ) const
    {
    // Define the connection type based on the connection preferences.
    if ( aMpmConnPref.ConnSelectionDialog() )
        {
        // Connection selection dialog was requested.
        aMpmConnPref.SetConnType( TMpmConnPref::EConnTypeImplicit );
        }
    else if ( aMpmConnPref.IapId() != 0 ||
              aMpmConnPref.SnapId() != 0 ||
              aMpmConnPref.SnapPurpose() != CMManager::ESnapPurposeUnknown )
        {
        // Either IAP id, SNAP id or SNAP purpose was given.
        aMpmConnPref.SetConnType( TMpmConnPref::EConnTypeExplicit );
        }
    else
        {
        // Otherwise this is handled as a request to default connection.
        aMpmConnPref.SetConnType( TMpmConnPref::EConnTypeDefault );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::IapSelectionL
// -----------------------------------------------------------------------------
//
CMPMIapSelection* CMPMServerSession::IapSelectionL()
    {
    if( !iIapSelection )
        {
        MPMLOGSTRING( "CMPMServerSession::IapSelectionL error, no connection started" )
        User::Leave( KErrNotFound );
        }
    return iIapSelection;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::PrefIAPNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::PrefIAPNotificationL(
    const TConnMonIapInfo&      aIapInfo,
    const TPrefIAPNotifCaller   aCaller )
    {
    MPMLOGSTRING( "CMPMServerSession::PrefIAPNotificationL" )

    if ( !iNotifRequested || !iPreferredIAPRequested )
        {
        MPMLOGSTRING( "CMPMServerSession::PrefIAPNotificationL - \
No notification requested" )
        return;
        }
    
    // Dig out the state of this connection.
    //
    TConnectionState state;
    iMyServer.GetConnectionState( iConnId, state );
    
    if( iMigrateState != EMigrateUserConfirmation )
        {
        // If session is roaming, notification must be delayed.
        // But, only ConnMon initiated notifications need to be delayed.
        // Required notifications must go through whenever MPM decides
        // to initiate them.
        //
        if( ( aCaller == EConnMon || aCaller == EConnMonEvent ) && 
                ( iStoredIapInfo.HoldPrefIapNotif() || state == ERoaming) )
            {
            MPMLOGSTRING( "CMPMServerSession::PrefIAPNotificationL - \
Mobility ongoing, notification will be handled later" )
            iStoredIapInfo.SetStoredIapInfo( aIapInfo );
            return;
            }
        }

    TInt currentIap = MyServer().GetBMIap( iConnId );
    MPMLOGSTRING3( "CMPMServerSession::PrefIAPNotificationL - \
current iap %d, last notified %d ", currentIap, iLastNotifiedIap )
        
    TBool iapTypeLanOrWlan( EFalse );
    RAvailableIAPList  availableIAPList;
    CleanupClosePushL( availableIAPList );
    TUint32 validateIapId( 0 );
    TUint32 oldIapId( 0 );

    TMpmNotificationPrefIAPAvailable notifInfo;
    notifInfo.iMPMNotificationType = EMPMPreferredIAPAvailable;

    AvailableUnblacklistedIapsL( availableIAPList, aIapInfo, iConnId);
    oldIapId = currentIap;
            
    // If connection is using SNAP
    // 
    TUint32 snap = iMyServer.GetBMSnap( iConnId );
    if( snap != 0 )
        {
        TMpmConnPref tempMpmConnPref;
        tempMpmConnPref.SetIapId( 0 );
        tempMpmConnPref.SetSnapId( snap );
        IapSelectionL()->ChooseBestIAPL( tempMpmConnPref, availableIAPList );
        validateIapId = tempMpmConnPref.IapId();
        if ( ( validateIapId == 0 ) 
                && ( aCaller == EConnMon || aCaller == EConnMonEvent ) )
            {
            // Since Connection Monitor is the only component which 
            // is unaware of Connection Id and SNAP, it can't send 
            // the error notification in case ChooseBestIAPL could 
            // not find any available IAP for this SNAP. 
            // 
            // All the other components take responsibility of 
            // sending the error notification. 
            // 
            // Do not send the error if this call came from ConnMon Event.
            // This will prevent unnecessary mobility errors.
            // For example if there is two WLAN networks in Internet destination
            // and we get weak indication for the one to which we are connected.
            // Then connmon signals us with IAPAvailabilityChange which results
            // to no available IAPs. We should not report error in that case.
            if ( aCaller == EConnMon )
                {
                TRAP_IGNORE( ErrorNotificationL( KErrNotFound,
                                             EMPMMobilityErrorNotification ) );
                }
            CleanupStack::PopAndDestroy( &availableIAPList );
            return;                                                                   
            }

        TUint32 retNetId = 0;
        iMyServer.CommsDatAccess()->ValidateIapL( iConnId, 
                                       validateIapId, 
                                       retNetId, 
                                       iapTypeLanOrWlan,
                                       *this );
        
        if ( CheckNotifNeedL( currentIap,
                              iLastNotifiedIap,
                              validateIapId ) )
            {
            MPMLOGSTRING2( "CMPMServerSession::PrefIAPNotificationL: \
Sending pref iap notification connId: 0x%x", iConnId )

            iLastNotifiedIap = validateIapId; 

            notifInfo.iIsUpgrade = IsUpgrade( oldIapId, 
                                              validateIapId, 
                                              availableIAPList );

            // Put real value of iIsSeamless here for NLR.
            // 
            notifInfo.iIsSeamless = EFalse;
            notifInfo.iConnId = iConnId;

            MPMLOGSTRING3( "CMPMServerSession::PrefIAPNotificationL: \
Roaming from Iap %i to Iap %i", oldIapId, validateIapId )

            notifInfo.iNewIapId = validateIapId;
            notifInfo.iOldIapId = oldIapId;
                    
            if ( aCaller != EConfirmDlgRoaming )
                {
                delete iConfirmDlgRoaming;
                iConfirmDlgRoaming = NULL;
                }

            // Reset migrate state
            if ( iMigrateState == EMigrateUserConfirmation )
                {
                iMigrateState = EMigrateNone;
                }
            
            // Write buffer to BM
            //
            TPtrC8 d( reinterpret_cast< TUint8* >( &notifInfo ),
                      notifInfo.Length() );
            iNotifMessage.WriteL( KFirstArgument, d );

            // Reset flag
            //
            iNotifRequested = EFalse;

            // Hold the preferred IAP already during the preferred IAP notification.
            // In Freeway new PreferredCarrier can only be processed after the
            // response for the previous one is received.
            //
            iStoredIapInfo.SetHoldPrefIapNotif();
            
            // Now complete WaitNotification to BM
            //
            MPMLOGSTRING( "CMPMServerSession::PrefIAPNotificationL - \
Send preferred IAP notification" )
            iNotifMessage.Complete( KErrNone );
            }
        }
        // Connection is using IAP
        // In this case only check if the current IAP is found 
        // in available IAP list.
        // If it's not, then notify error.
        // 
    else 
        {
        TInt err = availableIAPList.Find( oldIapId );
        if( err == KErrNotFound )
            {
            MPMLOGSTRING2( "CMPMServerSession::PrefIAPNotificationL: \
SNAP 0 and IAP %d not available, notifying error", oldIapId )
            TRAP_IGNORE( ErrorNotificationL( KErrNotFound,
                                             EMPMMobilityErrorNotification ) )
            }
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( &availableIAPList );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::StartIAPNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::StartIAPNotificationL( const TUint32 aIapId )
    {
    MPMLOGSTRING3( "CMPMServerSession::StartIAPNotificationL: aConnId = 0x%x,\
 aIapId = %i", iConnId, aIapId )

    iStoredIapInfo.SetHoldPrefIapNotif();
    
    if ( !iNotifRequested )
        {
        MPMLOGSTRING( "CMPMServerSession::StartIAPNotificationL - \
No notification requested" )
        return;
        }
    
    // Show the connecting discreet popup to the user when migrating to another iap
    if ( !( iIapSelection->MpmConnPref().NoteBehaviour() &
            TExtendedConnPref::ENoteBehaviourConnDisableNotes) )
        {
        TConnectionState state = iMyServer.CheckUsageOfIap( aIapId, iConnId );
        TBool connectionAlreadyActive = (state == EStarted || state == EStarting || state == ERoaming);
        CConnectionUiUtilities* connUiUtils = NULL;
        if (!connectionAlreadyActive )
            {
            TRAPD( popupError,
                   connUiUtils = CConnectionUiUtilities::NewL();
                   connUiUtils->ConnectingViaDiscreetPopup(
                   			        aIapId );
                   delete connUiUtils; );
            if ( popupError && connUiUtils )
                {
                delete connUiUtils;
                }
            }
        }

    TMpmNotificationStartIAP notifInfo;
    notifInfo.iMPMNotificationType = EMPMStartIAPNotification;
    notifInfo.iInfo.iIap = aIapId;
    notifInfo.iInfo.iConnId = iConnId;
    notifInfo.iInfo.iServiceId = GetServiceIdSettingL();    

    // Write buffer to BM
    //
    TPtrC8 d(reinterpret_cast< TUint8* >( &notifInfo ),
                notifInfo.Length() );
    iNotifMessage.WriteL( KFirstArgument, d );

    // Reset flag
    //
    iNotifRequested = EFalse;

    // Now complete WaitNotification to BM
    //
    MPMLOGSTRING( "CMPMServerSession::StartIAPNotificationL - \
Send start IAP notification" )
    iNotifMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::StopIAPNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::StopIAPNotificationL( TInt aIapId )
    {
    MPMLOGSTRING2( "CMPMServerSession::StopIAPNotificationL: aConnId = 0x%x", iConnId)
    
    if ( !iNotifRequested )
        {
        MPMLOGSTRING( "CMPMServerSession::StopIAPNotificationL - \
No notification requested" )
        return;
        }

    TMpmNotificationStopIAP notifInfo;
    notifInfo.iMPMNotificationType = EMPMStopIAPNotification;
    notifInfo.iInfo.iConnId = iConnId;
    notifInfo.iInfo.iIap = aIapId;

    // Write buffer to BM
    //
    TPtrC8 d(reinterpret_cast< TUint8* >( &notifInfo ),
                notifInfo.Length() );
    iNotifMessage.WriteL( KFirstArgument, d );

    // Reset flag
    //
    iNotifRequested = EFalse;

    // Now complete WaitNotification to BM
    //
    MPMLOGSTRING( "CMPMServerSession::StopIAPNotificationL - \
Send stop IAP notification" )
    iNotifMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ErrorMobilityNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::MobilityErrorNotificationL( TInt aError )
    {
    MPMLOGSTRING2(
        "CMPMServerSession::MobilityErrorNotificationL: aError = %i ",
        aError )
        
    ErrorNotificationL( aError, EMPMMobilityErrorNotification );
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::ErrorClientNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ClientErrorNotificationL( TInt aError )
    {
    MPMLOGSTRING2(
        "CMPMServerSession::ClientErrorNotificationL: aError = %i ",
        aError )
        
    ErrorNotificationL( aError, EMPMClientErrorNotification );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ErrorNotificationL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ErrorNotificationL( TInt aError, 
                                            TMpmNotifications aNotificationType )
    {
    MPMLOGSTRING3(
        "CMPMServerSession::ErrorNotificationL: aError = %i, aNotificationType = %i",
        aError, aNotificationType )

    if ( !iNotifRequested )
        {
        MPMLOGSTRING( "CMPMServerSession::ErrorNotificationL - \
No notification requested" )
        return;
        }

    TMpmNotificationError errorNotif;
    
    errorNotif.iMPMNotificationType = aNotificationType;
    errorNotif.iError = aError;
    errorNotif.iConnId = iConnId;

    // Write buffer to BM
    TPtrC8 d(reinterpret_cast< TUint8* >( &errorNotif ),
             errorNotif.Length() );
    iNotifMessage.WriteL( KFirstArgument, d );

    // Reset flag
    iNotifRequested = EFalse;

    // Now complete error notification to BM
    MPMLOGSTRING("CMPMServerSession::ErrorNotificationL - Send error notification" )
    iNotifMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::AvailableUnblacklistedIapsL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::AvailableUnblacklistedIapsL( 
    RAvailableIAPList&  aAvailableIAPs, 
    const TConnectionId aConnId )
    {
    MPMLOGSTRING( "CMPMServerSession::AvailableUnblacklistedIapsL" )

    TConnMonIapInfo availableIAPList;
    availableIAPList = GetAvailableIAPs();

    AvailableUnblacklistedIapsL( aAvailableIAPs, 
                                 availableIAPList, 
                                 aConnId );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::AvailableUnblacklistedIapsL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::AvailableUnblacklistedIapsL( 
    RAvailableIAPList&  aAvailableIAPs, 
    const TConnMonIapInfo&    aIapInfo, 
    const TConnectionId aConnId )
    {
    RArray<TUint32> blacklistiaps;
    CleanupClosePushL( blacklistiaps );
    TWlanSsid       ssid;
    TUint32         iapId( 0 );

    aAvailableIAPs.Reset();

    for (TUint i = 0; i < aIapInfo.iCount; i++)
        {
        aAvailableIAPs.AppendL( aIapInfo.iIap[i].iIapId );
        }

    // Get list of blacklisted IAPs.
    // 
    iMyServer.GetBlacklistedIAP( aConnId, blacklistiaps );

    // If there is an active WLAN connection
    // 
    if ( iMyServer.Events()->ActiveWlanConnection( ssid, iapId ) )
        {
        RAvailableIAPList unavailableIAPs;
        CleanupClosePushL( unavailableIAPs );
        TRAP_IGNORE( UnavailableIAPsL( aAvailableIAPs, 
                                   unavailableIAPs ) )
        
        TBool activeAvailable( ETrue );
        if (unavailableIAPs.Find(iapId) != KErrNotFound)
            {
            MPMLOGSTRING( "CMPMServerSession::AvailableUnblacklistedIapsL: Active IAP unavailable" )
            activeAvailable = EFalse;
            }
    
        // If the active WLAN connection is blacklisted, then 
        // there is no need to check if any of the unavailable 
        // WLAN IAPs have same SSID as active WLAN connection.
        //
        // Same goes for unavailable active connection.
        // 
        if ( activeAvailable && 
             blacklistiaps.Find( iapId ) == KErrNotFound )
            {
            for ( TInt i( 0 ); ( (i < unavailableIAPs.Count()) ); i++ )
                {
                // Check if any of the unavailable WLAN IAPs have 
                // the same SSID as the active WLAN connection.
                //
                TBool usesSame( EFalse ); 

                if ( !iMyServer.CommsDatAccess()->CheckEasyWLanL( unavailableIAPs[i] ) )
                    {
                    TRAP_IGNORE( iMyServer.CommsDatAccess()->MatchSSIDL( ssid, 
                                                              unavailableIAPs[i], 
                                                              usesSame,
                                                              *this ) )
                                                              
                    if ( usesSame )
                        {
                        // Append unavailable IAP to list of available IAPs
                        // if it uses same SSID as active WLAN connection.
                        // 
                        MPMLOGSTRING2(
                            "CMPMServerSession::AvailableUnblacklistedIapsL:\
 Append unavailable IapId = %i", unavailableIAPs[i] )
                        aAvailableIAPs.AppendL( unavailableIAPs[i] );
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( &unavailableIAPs );
        }

    TInt index( KErrNotFound );

    // Remove any blacklisted IAP.
    // 
    for (TInt i = 0; i < blacklistiaps.Count(); i++)
        {
        index = aAvailableIAPs.Find( blacklistiaps[i] );
        if (( index != KErrNotFound ) && ( index < aAvailableIAPs.Count() ))
            {
            MPMLOGSTRING2( "CMPMServerSession::AvailableUnblacklistedIapsL: \
Remove blacklisted IapId = %i", aAvailableIAPs[index] )
            aAvailableIAPs.Remove( index );
            }
        }
    CleanupStack::PopAndDestroy( &blacklistiaps );
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::IsUpgrade
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsUpgrade( 
    const TUint32               aIapId, 
    const TUint32               aMigrateIapId,
    const RAvailableIAPList&    aAvailableIAPs ) const
    {
    if ( aIapId == aMigrateIapId )
        {
        // If the current Iap Id and migrate Iap Id are identical, 
        // then it is considered that this a downgrade. 
        // 
        MPMLOGSTRING( "CMPMServerSession::IsUpgrade - Downgrade" )
        return EFalse;
        }

    if ( aAvailableIAPs.Find( aIapId ) == KErrNotFound )
        {
        // If the registered Iap Id is not found in the list of 
        // available IAPs, then it is considered that the new 
        // validated IAP is a downgrade compared to previous.
        // 
        MPMLOGSTRING( "CMPMServerSession::IsUpgrade - Downgrade" )
        return EFalse;
        }
    else
        {
        // If the registered Iap Id is found in the list of 
        // available IAPs, then it is considered that the new 
        // validated IAP is an upgrade compared to previous.
        // 
        MPMLOGSTRING( "CMPMServerSession::IsUpgrade - Upgrade" )
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::CheckNotifNeedL
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::CheckNotifNeedL( const TUint32       aCurrentIap,
                                          const TUint32       aLastNotifiedIap,
                                          const TUint32       aValidatedIap )
    {
    TBool retValue( EFalse );

    // Get the state of the connection for this Connection Id
    // 
    TConnectionState state;
    iMyServer.GetConnectionState( iConnId, 
                                  state );
    // New IAP to be notified is different from last
    if ( aValidatedIap != aLastNotifiedIap )
        {
        if( aCurrentIap == aValidatedIap)
            {
            MPMLOGSTRING( "CMPMServerSession::CheckNotifNeed: current IAP is same preferred IAP, no need to send notification" )
            retValue = EFalse;
            }
        else
            {
            TCmUsageOfWlan usageOfWlan = MyServer().CommsDatAccess()->ForcedRoamingL();
            
            if ( usageOfWlan == ECmUsageOfWlanManual &&
                 MyServer().CommsDatAccess()->CheckWlanL( aValidatedIap ) != ENotWlanIap  )
                {
                MPMLOGSTRING( "CMPMServerSession::CheckNotifNeed: WLAN IAP, Switch to WLAN is Manual, no need to send notification" )
                retValue = EFalse;
                }
            else
                {
                MPMLOGSTRING( "CMPMServerSession::CheckNotifNeed: notif needed" )
                retValue = ETrue;             
                }
            }
       }
    else
        {
        MPMLOGSTRING( "CMPMServerSession::CheckNotifNeed: Last notified IAP is same as preferred IAP, no need to send notification" )
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::UnavailableIAPsL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::UnavailableIAPsL( 
    const RAvailableIAPList aAvailableIAPs, 
    RAvailableIAPList&      aUnavailableIAPs )
    {
    TInt err( KErrNone );

    CCommsDatabase* commsDatabase = CCommsDatabase::NewL();
    CleanupStack::PushL(commsDatabase);

    // Make hidden records visible
    // 
    commsDatabase->ShowHiddenRecords();

    // Iterate the IAP table from CommsDat
    CCommsDbTableView* table = NULL;
    table = commsDatabase->OpenTableLC( TPtrC( IAP ) );

    err = table->GotoFirstRecord();

    while ( !err )
        {
        TUint32 iapId( 0 );

        // Read IAP's ID
        //
        table->ReadUintL( TPtrC( COMMDB_ID ), iapId );
        
        if ( aAvailableIAPs.Find( iapId ) == KErrNotFound )
            {
            aUnavailableIAPs.AppendL( iapId );
            }

        err = table->GotoNextRecord();
        }

    // Release memory
    //
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commsDatabase );
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::RemoveUnavailableIap
// -----------------------------------------------------------------------------
//
void CMPMServerSession::RemoveUnavailableIap( TConnMonIapInfo& aIapInfo, 
                                              const TUint32    aIapId )
    {
    TConnMonIapInfo iapInfo = aIapInfo;
    TBool found( EFalse );

    for ( TUint i( 0 );( ( i < iapInfo.Count() ) && !found ); i++ )
        {
        if ( iapInfo.iIap[i].iIapId == aIapId )
            {
            found = ETrue;

            MPMLOGSTRING2( "CMPMServerSession::RemoveUnavailableIap - IAP: %d", 
                iapInfo.iIap[i].iIapId ) 
            
            // Since iapInfo.Remove( i ) is not supported
            // 
            TUint k( 0 );
            for ( k = i; ( ( k + 1 ) < iapInfo.Count() ); k++ )
                {
                iapInfo.iIap[k] = iapInfo.iIap[k+1];
                }
            iapInfo.iIap[k].iIapId = 0;
            iapInfo.iCount--;
            }
        }

    aIapInfo = iapInfo;
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::IsBackgroundApplication
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsBackgroundApplication( TUint32 aUid ) const
    {
    if( aUid == KUidSimApplicationToolkit || 
        aUid == KUidDVBH ||
        aUid == KUidAlwaysOnlineStarter)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ChooseIapComplete
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ChooseIapComplete( 
    TInt                aError,
    const TMpmConnPref* aPolicyPref )
    {
    MPMLOGSTRING2( "CMPMServerSession::ChooseIapComplete aError = %d", aError )

    // Show error popup if it's allowed per client request.
	// No error popup shown to SNAP.
    if ( ChooseBestIapCalled() && (!( iIapSelection->MpmConnPref().NoteBehaviour() &
            TExtendedConnPref::ENoteBehaviourConnDisableNotes ))
            && ( aError != KErrNone ) 
            && ( iIapSelection->MpmConnPref().SnapId() == 0 ) )
        {
        CConnectionUiUtilities* connUiUtils = NULL;
        TRAPD( error, connUiUtils = CConnectionUiUtilities::NewL() );
        if ( error == KErrNone && connUiUtils )
            {
            // Note: Below function shows the discreet popup only if the error code
            // belongs to the set of errors that are shown to the user.
            // Otherwise the popup is not shown.
            connUiUtils->ConnectionErrorDiscreetPopup( aError );
            delete connUiUtils;
            connUiUtils = NULL;
            }
        }
    
    // Try to write back arguments and complete message.
    // 
    if ( !iChooseIapMessage.IsNull() )
        {
        if ( aError == KErrNone && aPolicyPref )
            {
            MPMLOGSTRING4( "CMPMServerSession::ChooseIapComplete \
IAP Id = %d, SNAP Id = %d, Net Id = %d",
                aPolicyPref->IapId(),
                aPolicyPref->SnapId(),
                aPolicyPref->NetId() )

            TPolicyConnPref pref;
            pref.SetIapId( aPolicyPref->IapId() );
            pref.SetNetId( aPolicyPref->NetId() );
            TInt serviceid(0);
            
            // Resolve the original serviceid for the application.
            //
            TRAPD( err, serviceid = GetServiceIdSettingL() )
            if ( err != KErrNone )
                {
                MPMLOGSTRING2( "CMPMServerSession::ChooseIapComplete GetServiceIdSettingL Leaved %d", err )
                pref.SetServiceId( 0 );
                }
            else
                {
                pref.SetServiceId( serviceid );
                }
            MPMLOGSTRING4( "CMPMServerSession::ChooseIapComplete writing policy pref \
iap %d net id %d service id %d", pref.IapId(), pref.NetId(), pref.ServiceId() )
            switch ( iChooseIapMessage.Function() )
                {
                case EMPMServerChooseIap:
                    {
                    TRAP( aError, iChooseIapMessage.WriteL( KFourthArgument, pref ) )
                    break;
                    }
                case EMPMServerReselectIap:
                    {
                    TRAP( aError, iChooseIapMessage.WriteL( KSecondArgument, pref ) )
                    break;
                    }
                default:
                    {
                    MPMLOGSTRING2( "CMPMServerSession::ChooseIapComplete - Error \
Inconsistent state %d", KErrGeneral )
                    aError = KErrGeneral;
                    }
                }
            }
        MPMLOGSTRING( "CMPMServerSession::ChooseIapComplete Message completed" )
        iChooseIapMessage.Complete( aError );
        }
    }


// -----------------------------------------------------------------------------
// CMPMServerSession::ProcessErrorComplete
// -----------------------------------------------------------------------------
//
void CMPMServerSession::ProcessErrorComplete( TInt             aError, 
                                              TInt*            aErrorReturned,
                                              TBMNeededAction* aNeededAction )
    {
    MPMLOGSTRING2( "CMPMServerSession::ProcessErrorComplete aError = %d", aError )

    delete iDisconnectDlg;
    iDisconnectDlg = NULL;
    
    if ( !iProcessErrorMessage.IsNull() )
        {
        // Try to write back arguments and complete message.
        // Traps are not necesary here. If WriteL functions leave the
        // message is still completed in ServiceError
        //
        if( aErrorReturned )
            {
            MPMLOGSTRING2( "CMPMServerSession::ProcessErrorComplete returned error = %d", 
                           *aErrorReturned )

            TPtrC8 d( reinterpret_cast< TUint8* >( aErrorReturned ), 
                                                   sizeof( aErrorReturned ) );
            TRAP_IGNORE( iProcessErrorMessage.WriteL( KFirstArgument, d ) )
                        }
        if( aNeededAction )
            {
            MPMLOGSTRING2( "CMPMServerSession::ProcessErrorComplete needed action = %d", 
                           *aNeededAction )
            TPtrC8 c( reinterpret_cast< TUint8* >( aNeededAction ), 
                                                   sizeof( aNeededAction ) );
            TRAP_IGNORE( iProcessErrorMessage.WriteL( KThirdArgument, c ) )
            }
        MPMLOGSTRING( "CMPMServerSession::ProcessErrorComplete completing message" )
        iProcessErrorMessage.Complete( aError );    
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::DisconnectDlgErrorCode
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::DisconnectDlgErrorCode( TInt aError ) const
    {
    if ( aError == KErrGprsInsufficientResources               || // -4154
         aError == KErrPacketDataTsyMaxPdpContextsReached      || // -6000
         aError == KErrUmtsMaxNumOfContextExceededByNetwork    || // -4179
         aError == KErrUmtsMaxNumOfContextExceededByPhone )       // -4178
        {
        MPMLOGSTRING2( "CMPMServerSession::DisconnectDlgErrorCode - \
Error %d, is disconnect dlg error code", aError )
        return ETrue;
        }
    else
        {
        MPMLOGSTRING2( "CMPMServerSession::DisconnectDlgErrorCode - \
Error %d, is not disconnect dlg error code", aError )
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::GetPresumedIap
// -----------------------------------------------------------------------------
//
TUint32 CMPMServerSession::GetPresumedIap()
    {
    TUint32 presumedIap( 0 ), realIap = iMyServer.GetBMIap( iConnId );
    if( realIap != 0 )
        {
        presumedIap = iMyServer.Events()->PresumedIapId( iConnId, realIap );
        }
    MPMLOGSTRING3( "CMPMServerSession::GetPresumedIap - \
Real IAP %d, presumed Iap %d", realIap, presumedIap )
    return presumedIap;
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::MigrateDoneL
// -----------------------------------------------------------------------------
//
void CMPMServerSession::MigrateDoneL( TInt aError )
    {
    MPMLOGSTRING3( "CMPMServerSession<0x%x>::MigrateCompleteL: error %d",
	               iConnId,
	               aError )

    if( aError == KErrNone )
        {
        // Send a notification to BearerMan 
        // to start the Iap
        //
        TRAP_IGNORE( StartIAPNotificationL( iMigrateIap ) )        
        }
    iMigrateState = EMigrateNone;
    iMigrateIap = 0;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::UseUserConnPref
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::UseUserConnPref()
    {
    if ((iAppUid != iMyServer.CsIdWatcher()->ConnectScreenId()) &&
        iMyServer.UserConnection())
        {
        MPMLOGSTRING( "CMPMServerSession::UseUserConnPref - User connection active" );
        
        // Check whether default connection will be used
        if ( iIapSelection->MpmConnPref().ConnType() == TMpmConnPref::EConnTypeDefault )
            {
            return ETrue;
            }
        else if ( ( iIapSelection->MpmConnPref().ConnType() ==
                    TMpmConnPref::EConnTypeImplicit ) &&
                  PrefsAllowWlan() )
            {            
            MPMLOGSTRING( "CMPMServerSession::UseUserConnPref -\
 Prompt from the user" );
            // Prompt from the user -> use user connection
            return ETrue;
            }
        else
            {
            MPMLOGSTRING( "CMPMServerSession::UseUserConnPref -\
 Application preferencies" );
            TBool isInternetSnap = EFalse;
            TInt error = KErrNone;
    
            // Check whether iap belongs to internet snap
            TRAP( error, isInternetSnap =
                             iMyServer.CommsDatAccess()->IsInternetSnapL(
                                     iIapSelection->MpmConnPref().IapId(),
                                     iIapSelection->MpmConnPref().SnapId() ) );

            if ( ( error == KErrNone ) && ( isInternetSnap ) && PrefsAllowWlan() )
                {
                // Iap belongs to internet snap -> use user connection
                return ETrue;
                }
            }
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::IsWlanOnlyL
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsWlanOnlyL( TBool& aNewWlansAllowed )
    {
    TCmGenConnSettings  genConnSettings;

    MPMLOGSTRING( "CMPMServerSession::IsWlanOnlyL")

    // Read global cellular data usage values from CommsDat's DefConn table
    genConnSettings = iMyServer.CommsDatAccess()->ReadGenConnSettingsL();

    // Find out if new wlans can be prompted
    if ( genConnSettings.iUsageOfWlan == ECmUsageOfWlanKnownAndNew )
        {
        aNewWlansAllowed = ETrue;
        MPMLOGSTRING( "CMPMServerSession::IsWlanOnlyL: aNewWlansAllowed: True" )
        }
    else
        {
        aNewWlansAllowed = EFalse;
        MPMLOGSTRING( "CMPMServerSession::IsWlanOnlyL: aNewWlansAllowed: False" )
        }
        
    // Find out is only WLAN connection is allowed in current network
    TUint32 currentCellularDataUsage( genConnSettings.iCellularDataUsageHome );
    
    if ( iMyServer.IsVisitorNetwork() )
        {
        currentCellularDataUsage = genConnSettings.iCellularDataUsageVisitor;  
        }
    
    if ( currentCellularDataUsage == ECmCellularDataUsageDisabled )
        {
        MPMLOGSTRING( "CMPMServerSession::IsWlanOnlyL: True" )
        return ETrue;
        }
    else 
        {
        MPMLOGSTRING( "CMPMServerSession::IsWlanOnlyL: False" )
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::ForcedRoaming()
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::ForcedRoaming()
    {
    TBool forcedRoaming( EFalse );
    if ( iIapSelection )
        {
        forcedRoaming = iIapSelection->MpmConnPref().ForcedRoaming();
        }
    return forcedRoaming;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::RemoveIapsAccordingToBearerSetL()
// -----------------------------------------------------------------------------
//
void CMPMServerSession::RemoveIapsAccordingToBearerSetL( TConnMonIapInfo& aIapInfo )
    {
    if (!iIapSelection)
        {
        // Iap selection hasn't done for this session -> just return
        return;
        }
    
    MPMLOGSTRING2( "CMPMServerSession::RemoveIapsAccordingToBearerSetL:\
 bearerset=%d", iIapSelection->MpmConnPref().BearerSet() )
    
    // Drop iaps not according to bearer set
    if ( iIapSelection->MpmConnPref().BearerSet() != 
         TExtendedConnPref::EExtendedConnBearerUnknown )
        {
        TInt index = 0;
        
        // First filter away physical IAPs (WLAN, packet data...)
        while ( index != aIapInfo.iCount )
            {
            // Get bearer type
            TMPMBearerType bearerType = EMPMBearerTypeOther;
            bearerType =
                iMyServer.CommsDatAccess()->GetBearerTypeL ( aIapInfo.iIap[index].iIapId );
            
            if ( (( bearerType == EMPMBearerTypePacketData ) &&
                  ( iIapSelection->MpmConnPref().BearerSet() & 
                    TExtendedConnPref::EExtendedConnBearerCellular )) )
                {
                // Don't remove this iap
                index++;
                continue;
                }
            else if ( (( bearerType == EMPMBearerTypeWlan ) &&
                  ( iIapSelection->MpmConnPref().BearerSet() & 
                    TExtendedConnPref::EExtendedConnBearerWLAN )) )
                {
                // Don't remove this iap
                index++;
                continue;
                }
            else if ( bearerType == EMPMBearerTypeVpn )
                {
                // Don't remove this VPN IAP on this round. 
                // VPN IAPs are filtered after this.
                index++;
                continue;
                }
            else
                {
                MPMLOGSTRING2( "CMPMServerSession::RemoveIapsAccordingToBearerSetL:\
 Filtered IAP ID:%d", aIapInfo.iIap[index].iIapId );
 
                // Remove this iap from the list
                for ( TInt index2 = index; index2 < aIapInfo.iCount; index2++ )
                    {
                    aIapInfo.iIap[index2].iIapId = aIapInfo.iIap[index2 + 1].iIapId;
                    }
                
                aIapInfo.iCount--;
                }
            }
        // Next filter away the VPN IAPs from the remaining IAPs.
        // VPN IAP should survive this filter only if it has at least
        // one unfiltered physical IAP still available. If it does not
        // then it cannot be used to establish connection and should 
        // be filtered.
        index = 0;
        RAvailableIAPList availableIaps; 
        CleanupClosePushL( availableIaps );
		
        for ( TUint i = 0; i < aIapInfo.iCount; i++ )
            {
            availableIaps.AppendL( aIapInfo.iIap[i].iIapId );
            }
        
        MPMLOGSTRING2( "CMPMServerSession::RemoveIapsAccordingToBearerSetL:\
 Starting VPN IAP filtering (iap count: %d)", aIapInfo.iCount );
        
        while ( index != aIapInfo.iCount )
            {
            // Get bearer type
            TMPMBearerType bearerType = EMPMBearerTypeOther;
            TUint32 realIapId( 0 );
            bearerType =
                iMyServer.CommsDatAccess()->GetBearerTypeL ( aIapInfo.iIap[index].iIapId );
            
            if ( bearerType == EMPMBearerTypeVpn )
                {
                iMyServer.CommsDatAccess()->FindRealIapL( aIapInfo.iIap[index].iIapId, 
                                                           realIapId, 
                                                           availableIaps, 
                                                           *this);
                
                MPMLOGSTRING3( "CMPMServerSession::RemoveIapsAccordingToBearerSetL:\
VPN IAP id: %d, real IAP id: %d", aIapInfo.iIap[index].iIapId, realIapId );
                
                if ( realIapId != 0 )
                    {
                    // Actual physical IAP was found for this VPN IAP. Do not filter.
                    index++;    
                    continue;
                    }                
                MPMLOGSTRING2( "CMPMServerSession::RemoveIapsAccordingToBearerSetL:\
Filtered away VPN IAP: %d", aIapInfo.iIap[index].iIapId );
                
                // Remove this iap from the list
                for ( TInt index2 = index; index2 < aIapInfo.iCount; index2++ )
                    {
                    aIapInfo.iIap[index2].iIapId = aIapInfo.iIap[index2 + 1].iIapId;
                    }
                
                aIapInfo.iCount--;                               
                }
            index++;
            }
        CleanupStack::PopAndDestroy( &availableIaps );
        }
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::GetAvailableIAPs()
// -----------------------------------------------------------------------------
//
TConnMonIapInfo CMPMServerSession::GetAvailableIAPs()
    {
    TConnMonIapInfo availableIAPs;
    
    availableIAPs = MyServer().Events()->GetAvailableIAPs();
#ifndef __WINSCW__  
    // Remove iaps not according to bearer set
    TRAP_IGNORE ( RemoveIapsAccordingToBearerSetL ( availableIAPs ) );
#endif
    
    return availableIAPs;
    }
    
// -----------------------------------------------------------------------------
// CMPMServerSession::IsBearerAccepted()
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsBearerAccepted( TMPMBearerType aBearerType )
    {
    TBool returnValue = EFalse;
    
    if ( !iIapSelection ||
        ( iIapSelection->MpmConnPref().BearerSet() == 
        TExtendedConnPref::EExtendedConnBearerUnknown ) )
        {
        // Iap selection hasn't been done for this session or all bearers
        // are accepted -> just return true
        returnValue = ETrue;
        }
    else
        {
        switch ( aBearerType )
            {
            case EMPMBearerTypeWlan:
                if ( iIapSelection->MpmConnPref().BearerSet() & 
                    TExtendedConnPref::EExtendedConnBearerWLAN )
                    {
                    returnValue = ETrue;
                    }
                break;
                
            case EMPMBearerTypePacketData:
                if ( iIapSelection->MpmConnPref().BearerSet() & 
                    TExtendedConnPref::EExtendedConnBearerCellular )
                    {
                    returnValue = ETrue;
                    }
                break;
                
            default:
                break;
            }
        }
    
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::IsMMSIap()
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::IsMMSIap( TUint32 aIap )
    {
    // Check if IAP is reported by MMS
    //
    TBool isMMSIap = EFalse;
    if( aIap != 0 )
        {
        // get property if it's defined 
        TInt err( KErrNone );
        TInt mmsIap( 0 );
        err = RProperty::Get( KMPMCathegory, KMPMPropertyKeyMMS, mmsIap);    
                            
        // If successful compare IAPs
        if( err == KErrNone ) 
            {
            if( aIap == mmsIap )
                {
                isMMSIap = ETrue;
                }
            }
        } 
    MPMLOGSTRING2( "CMPMServerSession::IsMmsIap: Returning bool value: %d",
                   isMMSIap )
    return isMMSIap;
    }

// -----------------------------------------------------------------------------
// CMPMServerSession::PrefsAllowWlan
// -----------------------------------------------------------------------------
//
TBool CMPMServerSession::PrefsAllowWlan()
    {
    // WLAN connection can be used if application has not specified any bearerset
    // or when bearerset has WLAN enabled.
    //
    if ( ( iIapSelection->MpmConnPref().BearerSet() == 
           TExtendedConnPref::EExtendedConnBearerUnknown ) 
         ||
         ( iIapSelection->MpmConnPref().BearerSet() &
           TExtendedConnPref::EExtendedConnBearerWLAN ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
        
// -----------------------------------------------------------------------------
// TNetIap::TNetIap
// -----------------------------------------------------------------------------
//
TNetIap::TNetIap() 
    : iSnap( 0 ),
      iEmbeddedSnap( 0 ),
      iIapId( 0 ),
      iRanking( 0 ),
      iGlobalPriority( 0 )
    {
    }


// -----------------------------------------------------------------------------
// TNetIap::CompareRanking
// -----------------------------------------------------------------------------
//
TInt TNetIap::CompareRanking( const TNetIap& aFirst,
                              const TNetIap& aSecond )
    {
    if ( aFirst.iRanking < aSecond.iRanking )
        {
        return KSmaller;
        }
    else if ( aFirst.iRanking > aSecond.iRanking )
        {
        return KBigger;
        }
    else
        {
        return KEqual;
        }
    }
    
// -----------------------------------------------------------------------------
// TNetIap::CompareGlobalAndLocalPriority
// -----------------------------------------------------------------------------
//
TInt TNetIap::CompareGlobalAndLocalPriority( const TNetIap& aFirst,
                                             const TNetIap& aSecond )
    {
    if ( aFirst.iGlobalPriority < aSecond.iGlobalPriority )
        {
        return KSmaller;
        }
    else if ( aFirst.iGlobalPriority > aSecond.iGlobalPriority )
        {
        return KBigger;
        }
    else
        {
        // If global priorities are equal,
        // order based on ranking
        // 
        return TNetIap::CompareRanking( aFirst, aSecond );
        }
    }    
// -----------------------------------------------------------------------------
// TStoredIapInfo::TStoredIapInfo
// -----------------------------------------------------------------------------
//
TStoredIapInfo::TStoredIapInfo()
    : iHoldPrefIapNotif( EFalse ),
      iIapInfoWaiting( EFalse ),
      iStoredIapInfo()
    {
    }
    
// -----------------------------------------------------------------------------
// TStoredIapInfo::HoldPrefIapNotif
// -----------------------------------------------------------------------------
//      
TBool TStoredIapInfo::HoldPrefIapNotif() const
    {
    MPMLOGSTRING2( "TStoredIapInfo::HoldPrefIapNotif value %d", iHoldPrefIapNotif )
    return iHoldPrefIapNotif;
    }

// -----------------------------------------------------------------------------
// TStoredIapInfo::SetHoldPrefIapNotif
// -----------------------------------------------------------------------------
//
void TStoredIapInfo::SetHoldPrefIapNotif()
    {
    MPMLOGSTRING( "TStoredIapInfo::SetHoldPrefIapNotif" )
    iHoldPrefIapNotif = ETrue;
    }

// -----------------------------------------------------------------------------
// TStoredIapInfo::HandleIapInfoWaiting
// -----------------------------------------------------------------------------
//
TBool TStoredIapInfo::HandleIapInfoWaiting( TConnMonIapInfo& aStoredIapInfo )
    {
    if( iIapInfoWaiting )
        {
        iHoldPrefIapNotif = EFalse;
        aStoredIapInfo = iStoredIapInfo; 
        }
    MPMLOGSTRING2( "TStoredIapInfo::HandleIapInfoWaiting value %d", iIapInfoWaiting )
    return iIapInfoWaiting;
    }

// -----------------------------------------------------------------------------
// TStoredIapInfo::SetStoredIapInfo
// -----------------------------------------------------------------------------
//
void TStoredIapInfo::SetStoredIapInfo( const TConnMonIapInfo& aStoredIapInfo )
    {
    MPMLOGSTRING( "TStoredIapInfo::SetStoredIapInfo" )
    iIapInfoWaiting = ETrue;
    iStoredIapInfo = aStoredIapInfo;
    }

// -----------------------------------------------------------------------------
// TStoredIapInfo::ResetStoredIapInfo
// -----------------------------------------------------------------------------
//
void TStoredIapInfo::ResetStoredIapInfo()
    {
    MPMLOGSTRING( "TStoredIapInfo::ResetStoredIapInfo" )
    iHoldPrefIapNotif = EFalse;
    iIapInfoWaiting = EFalse;
    }


//  End of File
