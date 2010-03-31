/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application UI class
*
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <AknUtils.h>
#include <cmmanagerext.h>
#include <nifvar.h>
#include <ConnectionMonitorUi.rsg>
#include "ConnectionMonitorUi.hrh"
#include "ConnectionMonitorUiAppUi.h"
#include "ConnectionMonitorUiView.h"
#include "ConnectionMonitorUiDetailsView.h"
#include "ConnectionArray.h"
#include "CsdConnectionInfo.h"
#include "GprsConnectionInfo.h"
#include "WlanConnectionInfo.h"
#include "EasyWLANConnectionInfo.h"
#include "FeatureManagerWrapper.h"
#include "ActiveWrapper.h"


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CConnectionMonitorUiAppUi::ConstructL()
// ----------------------------------------------------------
//
void CConnectionMonitorUiAppUi::ConstructL()
    {
    CMUILOGGER_CREATE;

    CMUILOGGER_WRITE( "Constructing CConnectionMonitorUiAppUi" );

    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);

#ifdef RD_CONTROL_PANEL
    
    TBool isEmbeddedApp = IsEmbedded();
    CMUILOGGER_WRITE_F( "IsEmbedded(): %d", isEmbeddedApp );

#endif // RD_CONTROL_PANEL                                  

    iFeatureManagerWrapper = CFeatureManagerWrapper::NewL();
    TBool isWlanSupported = FeatureManager::FeatureSupported( 
                                                    KFeatureIdProtocolWlan );
          
   CMUILOGGER_WRITE_F( "isWlanSupported: %b", isWlanSupported );

    // connect to connection monitor engine
    CMUILOGGER_WRITE( "Connection monitor engine" );
    iMonitor.ConnectL();
    CMUILOGGER_WRITE( "Connected" );

    CMUILOGGER_WRITE( "Creating connection array" );
    iConnectionArray = new ( ELeave ) CConnectionArray();
    iConnectionArray->ConstructL();
    
    iActiveWrapper = CActiveWrapper::NewL(); // start ActiveWrapper

    if ( isWlanSupported )
        {
        GetEasyWlanIAPNameL();
        }

    InitializeConnectionArrayL();

    CMUILOGGER_WRITE( "Created" );

    CMUILOGGER_WRITE( "Creating main view" );

#ifdef RD_CONTROL_PANEL
    CConnectionMonitorUiView* mainView =
        new ( ELeave ) CConnectionMonitorUiView( iConnectionArray,
                                                 &iMonitor,
                                                 isEmbeddedApp,
                                                 iActiveWrapper );
#else
    CConnectionMonitorUiView* mainView =
        new ( ELeave ) CConnectionMonitorUiView( iConnectionArray,
                                                 &iMonitor,
                                                 iActiveWrapper );
#endif // RD_CONTROL_PANEL

    CleanupStack::PushL( mainView );
    CMUILOGGER_WRITE( "Constructing main view" );
    mainView->ConstructL();
    CMUILOGGER_WRITE( "Adding main view" );
    AddViewL( mainView );      // transfer ownership to CAknViewAppUi

    SetDefaultViewL( *mainView );
    iTimerObserver = this;

    CleanupStack::Pop( mainView );
    CMUILOGGER_WRITE( "Done" );

    CMUILOGGER_WRITE( "Creating details view" );
    CConnectionMonitorUiDetailsView* detailsView =
        new ( ELeave ) CConnectionMonitorUiDetailsView( iConnectionArray );
    CleanupStack::PushL( detailsView );
    CMUILOGGER_WRITE( "Constructing details view" );
    detailsView->ConstructL();
    CMUILOGGER_WRITE( "Adding details view" );
    AddViewL( detailsView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( detailsView );
    CMUILOGGER_WRITE( "Done" );
    
    // from this point we can listen to events of RConnectionMonitor server
    iMonitor.NotifyEventL( *this );
    
    activateMainView = EFalse;
    }

// ----------------------------------------------------
// CConnectionMonitorUiAppUi::~CConnectionMonitorUiAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CConnectionMonitorUiAppUi::~CConnectionMonitorUiAppUi()
    {
    CMUILOGGER_ENTERFN( "~CConnectionMonitorUiAppUi" );

    StopTimer();
    delete iConnectionArray;
    delete iEasyWlanIAPName;
    delete iActiveWrapper;
    delete iFeatureManagerWrapper;

    // it's needed because views are still active here
    // and a call from handleforegroundevent from them
    // can cause exception in StopConnEventNotificationL
    iMonitor.CancelNotifications();
    iMonitor.Close();
    iNewConnectionIdArray.Close();
    iConnectionsWaitingFirstEvent.Close();
    CMUILOGGER_LEAVEFN( "~CConnectionMonitorUiAppUi" );

    CMUILOGGER_DELETE;
    }

// ---------------------------------------------------------------------------
// CConnectionMonitorUiAppUi::InitializeConnectionArrayL
// ---------------------------------------------------------------------------
//
void CConnectionMonitorUiAppUi::InitializeConnectionArrayL()
    {
    CMUILOGGER_ENTERFN( "InitializeConnectionArrayL" );

    CConnectionInfoBase* connectionInfo = NULL;
    TUint connectionNumber( 0 );
    TUint connectionId( 0 );
    TUint subConnectionCount;

    TUint wlanConnectionCounter( 0 );
    TUint wlanConnectionId( 0 );
    TConnMonBearerType bearerType( EBearerUnknown );
    RArray<TUint> connIds;
    RArray<TInt> bearers;
    CleanupClosePushL( connIds );
    CleanupClosePushL( bearers );

    iActiveWrapper->StartGetConnectionCount( connectionNumber, iMonitor );

    if ( !iActiveWrapper->iStatus.Int() )
        {
        CMUILOGGER_WRITE_F( "Number of connections: %d",
                            connectionNumber );

        for ( TUint i = 1; i <= connectionNumber; ++i )
            {
            iMonitor.GetConnectionInfo( i, connectionId, subConnectionCount );
            connIds.AppendL( connectionId );
            
            iActiveWrapper->StartGetBearerType( connectionId,
                                                iMonitor,
                                                KBearer,
                                                ( TInt& )bearerType );                                           
            CMUILOGGER_WRITE_F( "status.Int(): %d", 
                                iActiveWrapper->iStatus.Int() );
            CMUILOGGER_WRITE_F( "bearerType: %d", ( TInt )bearerType );
            bearers.AppendL( (TInt)bearerType );
            
            if ( bearerType == EBearerWLAN )
                {
                wlanConnectionCounter++;
                wlanConnectionId = connectionId;
                }
            }
        
        for ( TInt i = 0; i < connectionNumber; i++ )
            {           
            if ( bearers[i] != EBearerWLAN || wlanConnectionCounter < 2 || connIds[i] == wlanConnectionId )
                {
                TInt err;
                TRAP(err, connectionInfo = CreateConnectionInfoL( connIds[i], (TConnMonBearerType)bearers[i] ));
                
                if ( !err && connectionInfo )
                    {
                    CleanupStack::PushL( connectionInfo );
                    iConnectionArray->AppendL( connectionInfo );
                    CleanupStack::Pop( connectionInfo );
                    }
                else
                    {
                    iNewConnectionIdArray.Append( connIds[i] );
                    }
                }
            else
                {
                CMUILOGGER_WRITE( "Fake WLAN connection, skipping" );
                }
            }
        }
    else
        {
        CMUILOGGER_WRITE_F( "GetConnectionCount status: %d", 
                            iActiveWrapper->iStatus.Int() );
        }

    CleanupStack::Pop( &bearers );
    bearers.Close();
    CleanupStack::Pop( &connIds );
    connIds.Close();

    CMUILOGGER_LEAVEFN( "InitializeConnectionArrayL Done" );
    }

// ---------------------------------------------------------------------------
// CConnectionMonitorUiAppUi::EventL
// ---------------------------------------------------------------------------
//
void CConnectionMonitorUiAppUi::EventL(
                                       const CConnMonEventBase &aConnMonEvent )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::EventL" );

    CMUILOGGER_WRITE_F( "EventL type: %d", aConnMonEvent.EventType() );

    CConnectionInfoBase* connectionInfo = NULL;
    TInt index( 0 );
    TConnMonEvent event = ( TConnMonEvent ) aConnMonEvent.EventType();
    TInt connectionId = aConnMonEvent.ConnectionId();

    CMUILOGGER_WRITE_F( "Connection Id:  %d", connectionId );

    switch ( event )
        {
        case EConnMonCreateConnection:
            {
            CMUILOGGER_WRITE( "EConnMonCreateConnection" );

            if ( connectionId > 0 )
                {
                iConnectionsWaitingFirstEvent.AppendL( connectionId );
                }
            break;
            }
        case EConnMonDeleteConnection:
            {
            CMUILOGGER_WRITE( "EConnMonDeleteConnection" );
            TInt count = iNewConnectionIdArray.Count();
            TInt i = 0;
            TBool newConn = EFalse;
            
            for ( TInt cindex = 0; cindex < iConnectionsWaitingFirstEvent.Count(); cindex++ )
                {
                if ( iConnectionsWaitingFirstEvent[cindex] == connectionId )
                    {
                    newConn = ETrue;
                    iConnectionsWaitingFirstEvent.Remove(cindex);
                    break;
                    }
                }

            if ( newConn )
                {
                // Job done
                break;
                }
                
            while ( i < count )
                {
                if ( iNewConnectionIdArray[i] == connectionId )
                    {
                    iNewConnectionIdArray.Remove( i );
                    CMUILOGGER_WRITE_F( "Removed id %d from array", ( TUint )connectionId );
                    count--;
                    }
                else 
                    {
                    i++;
                    }
                }
            
            index = iConnectionArray->GetArrayIndex( connectionId );
            if ( index >= 0 )
                {
                connectionInfo = ( *iConnectionArray )[index]; 
            
                if ( iView->Id() == KDetailsViewId )
                    {               
                        // Mark the connection as closed and refresh details view.
                        // It will change the status there.
                        connectionInfo->RefreshConnectionStatus( KConnectionClosed );
                        connectionInfo->RefreshDetailsArrayL();                        
                    }
                else // MainView
                    {
                    TBool deleting = connectionInfo->GetDeletedFromCMUI();
                    if ( deleting )
                        {
                        ShowConnectionSummaryInformationNoteL( connectionInfo );
                        }
                    // Mark the connection as closed and update the status text. 
                    // When the timer ticks the next time the marked connection 
                    // is deleted and removed from the main view.
                    connectionInfo->RefreshConnectionStatus( KConnectionClosed );
                    connectionInfo->RefreshConnectionListBoxItemTextL();
                    }                
                CMUILOGGER_WRITE_F( "Deleted: %d", connectionId );
                }
            break;
            }
        case EConnMonConnectionStatusChange:
            {
            CMUILOGGER_WRITE( "EConnMonConnectionStatusChange" );
            
            TBool newConn = EFalse;
                
            for ( TInt cindex = 0; cindex < iConnectionsWaitingFirstEvent.Count(); cindex++ )
                {
                if ( iConnectionsWaitingFirstEvent[cindex] == connectionId )
                    {
                    newConn = ETrue;
                    iConnectionsWaitingFirstEvent.Remove(cindex);
                    break;
                    }
                }

            index = iConnectionArray->GetArrayIndex( connectionId );

            if ( newConn || ( index < 0 ) )
                {
                if ( connectionId > 0)
                    {
                    TConnMonBearerType bearerType( EBearerUnknown );
                    iActiveWrapper->StartGetBearerType( connectionId,
                                                        iMonitor,
                                                        KBearer,
                                                        ( TInt& )bearerType );  
                    CMUILOGGER_WRITE_F( "status.Int(): %d", 
                                        iActiveWrapper->iStatus.Int() );
                    CMUILOGGER_WRITE_F( "bearerType: %d", ( TInt )bearerType );
                    if ( bearerType == 0 )
                        {
                        if ( connectionId )
                            {
                            iNewConnectionIdArray.AppendL( connectionId );
                            CMUILOGGER_WRITE_F( "id %d added to array", ( TUint )connectionId );
                            break;
                            }
                        }          
                    
                    connectionInfo = CreateConnectionInfoL( connectionId, bearerType );
                    }
                
                if ( connectionInfo )
                    {
                    CleanupStack::PushL( connectionInfo );
                    iConnectionArray->AppendL( connectionInfo );
                    CleanupStack::Pop( connectionInfo );
										
                    if ( iView->Id() != KDetailsViewId )
                        {
                  	  	// Avkon: This is needed due to events' timing issues:
                    		CConnectionMonitorUiView* view =
                        	    ( CConnectionMonitorUiView* )iView;
                    	view->HandleItemAdditionL();
                    	}
                    }
                }
            
            index = iConnectionArray->GetArrayIndex( connectionId );
            CMUILOGGER_WRITE_F( "Found index :  %d", index );

            if ( index >= 0 )
                {
                ( *iConnectionArray )[index]->StatusChangedL();
                }
            break;
            }
        case EConnMonCreateSubConnection:
        case EConnMonDeleteSubConnection:
            {
            CMUILOGGER_WRITE( "SubConnection" );
            index = iConnectionArray->GetArrayIndex( connectionId );
            if ( index >= 0)
                {
                ( *iConnectionArray )[index]->RefreshDetailsL();
                }
            break;
            }
        default:
            {
            CMUILOGGER_WRITE( "On event default" );
            break;
            }
        }

    if ( iObserver && ( index >= 0 ) )
        {
        CMUILOGGER_WRITE( "EventL calls observer" );
        iObserver->OnEventL( aConnMonEvent, index );
        }

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::EventL" );
    }

// ---------------------------------------------------------------------------
// CConnectionMonitorUiAppUi::Static
// ---------------------------------------------------------------------------
//
CConnectionMonitorUiAppUi* CConnectionMonitorUiAppUi::Static()
    {
    return REINTERPRET_CAST( CConnectionMonitorUiAppUi*,
                             CEikonEnv::Static()->EikAppUi() );
    }

// ---------------------------------------------------------------------------
//  CConnectionMonitorUiAppUi::::DynInitMenuPaneL( TInt aResourceId,
//  CEikMenuPane* aMenuPane )
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ---------------------------------------------------------------------------
//
void CConnectionMonitorUiAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    }

// ----------------------------------------------------
// CConnectionMonitorUiAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent, TEventCode /*aType*/ )
// ----------------------------------------------------
//
TKeyResponse CConnectionMonitorUiAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CConnectionMonitorUiAppUi::HandleCommandL( TInt aCommand )
// ----------------------------------------------------
//
void CConnectionMonitorUiAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::StartConnEventNotification
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::StartConnEventNotification(
                                    MActiveEventObserverInterface* aObserver )
    {
    CMUILOGGER_ENTERFN
        ( "CConnectionMonitorUiAppUi::StartConnEventNotification" );

    iObserver = aObserver;

    CMUILOGGER_LEAVEFN
        ( "CConnectionMonitorUiAppUi::StartConnEventNotification" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::StopConnEventNotification
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::StopConnEventNotification(
                                MActiveEventObserverInterface* aObserver )
    {
    CMUILOGGER_ENTERFN
        ( "CConnectionMonitorUiAppUi::StopConnEventNotification" );

    if ( iObserver == aObserver )
        {
        iObserver = NULL;
        }

    CMUILOGGER_LEAVEFN
        ( "CConnectionMonitorUiAppUi::StopConnEventNotification" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::StartTimerL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::StartTimerL( const TInt aInterval )
    {
    CMUILOGGER_WRITE( "Starting timer" );

    if ( !iPeriodic )
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
	    CMUILOGGER_WRITE_F( "aInterval  :  %d", aInterval );
        
        iPeriodic->Start( aInterval, aInterval,
                          TCallBack( Tick, this ) );
        }

    CMUILOGGER_WRITE( "Timer started" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::StopTimer
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::StopTimer()
    {
    CMUILOGGER_WRITE( "Stopping timer" );
    if ( iPeriodic )
        {
        CMUILOGGER_WRITE( "Timer existing" );
        iPeriodic->Cancel();
        delete iPeriodic;
        iPeriodic = NULL;
        }

    CMUILOGGER_WRITE( "Timer stopped" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::Tick
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiAppUi::Tick( TAny* aObject )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::Tick(TAny* aObject)" );

    CConnectionMonitorUiAppUi* myself =
                        static_cast<CConnectionMonitorUiAppUi*>( aObject );
    myself->Tick();

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::Tick(TAny* aObject)" );
    return 1;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::Tick
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiAppUi::Tick()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::Tick()" );

    if ( iTimerObserver )
        {
        TInt error( KErrNone );

        TRAP( error, iTimerObserver->OnTimerExpiredL() );
        CMUILOGGER_WRITE_F( "OnTimerExpiredL error id  :  %d", error );

        // refresh listboxes
        if ( !error && ( iObserver ) )
            {
            CMUILOGGER_WRITE( "Tick !error && ( iObserver )" );
            TRAP( error, iObserver->OnTimerEventL() );
            }
        CMUILOGGER_WRITE( "after Tick !error && ( iObserver )" );
        }

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::Tick()" );
    return 1;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::OnTimerExpiredL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::OnTimerExpiredL()
    {
    CMUILOGGER_WRITE_F( "MdcaCount: %d", iConnectionArray->MdcaCount());
    // First check if there are connections that need to be deleted.
    // The conditions for deletion are:
    // 1. Connection status must be closed
    // 2. Main view must be active (details view should be shown also for closed connections)
    // 3. There must not be active request ongoing (otherwise deletion might cause crash)    
    for(TUint i = 0; i < iConnectionArray->MdcaCount(); i++)
       {
       CMUILOGGER_WRITE_F( "i: %d", i );
       CMUILOGGER_WRITE_F( "Status: %d", (*iConnectionArray)[i]->GetStatus() );
       CMUILOGGER_WRITE_F( "active: %d", iActiveWrapper->IsActive() );
       if( (*iConnectionArray)[i]->GetStatus() == EConnectionClosed 
            && !iActiveWrapper->IsActive()
            && iView->Id() != KDetailsViewId)
           {
           CMUILOGGER_WRITE_F( "Delete conn id: %d", (*iConnectionArray)[i]->GetConnectionId() );
           iConnectionArray->Delete( (*iConnectionArray)[i]->GetConnectionId() );
           i--;
           }
       }
    
    
    TInt count = iNewConnectionIdArray.Count();
    if ( count > 0 )
        {
        for ( TInt i = 0; i < count; i++ )
            {
            TConnMonBearerType bearerType( EBearerUnknown );
            iActiveWrapper->StartGetBearerType( iNewConnectionIdArray[i],
                                                iMonitor,
                                                KBearer,
                                                ( TInt& )bearerType );  
            CMUILOGGER_WRITE_F( "status.Int(): %d", 
                                iActiveWrapper->iStatus.Int() );
            CMUILOGGER_WRITE_F( "bearerType: %d", ( TInt )bearerType );
            if ( bearerType == 0 )
                {
                continue;
                }
            CConnectionInfoBase* connectionInfo = NULL;
            TInt err;
            TRAP(err, connectionInfo = CreateConnectionInfoL( iNewConnectionIdArray[i], bearerType ));
            if ( err )
                {
                continue;
                }
            
            
            if ( connectionInfo )
                {
                CleanupStack::PushL( connectionInfo );
                iConnectionArray->AppendL( connectionInfo );
                CleanupStack::Pop( connectionInfo );
                
                iNewConnectionIdArray.Remove( i );
                count--;
                i--;
                CMUILOGGER_WRITE_F( "Removed index %d from array", ( TInt )i );
                }
            
            }
        }
    RefreshAllConnectionsL();
    if ( iView )
        {
        CMUILOGGER_WRITE_F( "iView->Id() %d", iView->Id() );
        if ( iView->Id() == KConnectionsViewId )
            {
            CConnectionMonitorUiView* view =
                    ( CConnectionMonitorUiView* )iView;
            view->ConnectionKeyObserver();
            }
        }
    }
    
// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::GetTickInerval
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiAppUi::GetTickInerval() const
	{
	return KTickInterval;
	}    

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::RefreshAllConnectionsL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::RefreshAllConnectionsL()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::RefreshAllConnectionsL" );
    TUint index( 0 );
    CConnectionInfoBase* connection = NULL;
    if ( iConnectionArray )
        {
        TUint count = iConnectionArray->MdcaCount();
        while ( index < count )
            {
            connection = ( *iConnectionArray )[index];
            // MainView Refresh
            if ( ( connection->IsAlive() ) && 
                 ( !connection->IsSuspended() ) )
                 {
                 CMUILOGGER_WRITE( "MainView Refresh" );
                 CMUILOGGER_WRITE_F( "RefreshAllConnectionsL index: %d", 
                                     index );
                 CMUILOGGER_WRITE_F( "RefreshAllConnectionsL connId: %d", 
                                     connection->GetConnectionId() );
                 connection->RefreshDetailsL();
                 }
            // DetailsView Refresh
            if ( ( connection->IsAlive() && 
                 ( iView->Id() == KDetailsViewId ) ) )
                {
                CMUILOGGER_WRITE( "DetailsView Refresh" );
                connection->RefreshDetailsArrayL();
                }
            connection->RefreshConnectionListBoxItemTextL();
            count = iConnectionArray->MdcaCount();
            ++index;
            }
        }
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::RefreshAllConnectionsL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::HandleForegroundEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::HandleForegroundEventL( TBool aForeground )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::HandleForegroundEventL" );

    CAknViewAppUi::HandleForegroundEventL( aForeground );
    if ( aForeground )
        {
        CMUILOGGER_WRITE( "Foreground" );
        if ( activateMainView && iView != NULL && iView->Id() == KDetailsViewId )
            {
            ((CConnectionMonitorUiDetailsView*) iView)->ActivateMainViewL();
            activateMainView = EFalse;
            }
            
        TInt interval( KTickInterval );
        if ( iTimerObserver )
  	        {
	          interval = iTimerObserver->GetTickInerval();	
	          }
        StartTimerL( interval );
        }
    else
        {
        CMUILOGGER_WRITE( "Background" );
        StopTimer();
        activateMainView = EFalse;
        }

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::HandleForegroundEventL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::CreateConnectionInfoL
// ---------------------------------------------------------
//
CConnectionInfoBase* CConnectionMonitorUiAppUi::CreateConnectionInfoL(
                                                TUint aConnectionId,
                                                TConnMonBearerType aBearerType )
    {
    CMUILOGGER_ENTERFN( "CreateConnectionInfoL - start " );

    CConnectionInfoBase* connection = NULL;

    if ( aConnectionId > 0 )
        {

#ifdef  __WINS__
        if ( aBearerType == EBearerLAN )
            {
            aBearerType = EBearerGPRS;
            }
#endif
        switch ( aBearerType )
            {
            case EBearerGPRS:
            case EBearerWCDMA:
            case EBearerEdgeGPRS:
            case EBearerExternalGPRS:
            case EBearerExternalWCDMA:
            case EBearerExternalEdgeGPRS:
                {
                CMUILOGGER_WRITE( "CGprsConnectionInfo" );
                
                connection = CGprsConnectionInfo::NewL(
                        aConnectionId,
                        &iMonitor,
                        aBearerType,
                        iActiveWrapper );
                break;
                }
            case EBearerCSD:
            case EBearerHSCSD:
            case EBearerWcdmaCSD:
            case EBearerExternalCSD:
            case EBearerExternalHSCSD:
            case EBearerExternalWcdmaCSD:
                {
                CMUILOGGER_WRITE( "CCsdConnectionInfo" );

                connection = CCsdConnectionInfo::NewL(
                        aConnectionId,
                        &iMonitor,
                        aBearerType,
                        iActiveWrapper );
                break;
                }
            case EBearerWLAN:
                //case EBearerExternalWLAN:
                {
                CMUILOGGER_WRITE( "CWlanConnectionInfo" );
                
                connection = CWlanConnectionInfo::NewL(
                        aConnectionId,
                        &iMonitor,
                        aBearerType,
                        iEasyWlanIAPName,
                        iActiveWrapper );
                break;
                }
            default :
                {
                CMUILOGGER_WRITE( "CreateConnectionInfoL, default" );
                break;
                }
            }
            CMUILOGGER_WRITE_F( "Connection created: %d", aConnectionId );

        if (  connection && connection->CheckMrouterIap() )
            {
            CMUILOGGER_WRITE_F( "Connection deleted: %d", aConnectionId );
            delete connection;
            connection = NULL;
            }
        }
    else
        {
        CMUILOGGER_WRITE( "Connection id is invalid" );
        }

    CMUILOGGER_LEAVEFN( "CreateConnectionInfoL - end " );

    return connection;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::GetEasyWlanIAPNameL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::GetEasyWlanIAPNameL()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiAppUi::GetEasyWlanIAPNameL" );

    delete iEasyWlanIAPName;
    iEasyWlanIAPName = NULL;

    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL( cmManagerExt );
    
    TUint32 easyWlanId = cmManagerExt.EasyWlanIdL();
    if ( easyWlanId )
            {
            CMUILOGGER_WRITE_F( "easyWlanId %d", easyWlanId );
            iEasyWlanIAPName = cmManagerExt
                                .GetConnectionMethodInfoStringL( 
                                                        easyWlanId, 
                                                        CMManager::ECmName );
            CMUILOGGER_WRITE_F( "iEasyWlanIAPName: %S", iEasyWlanIAPName );
            }

    CleanupStack::PopAndDestroy( &cmManagerExt );

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiAppUi::GetEasyWlanIAPNameL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::ShowConnectionSummaryInformationNoteL
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::ShowConnectionSummaryInformationNoteL( 
										const CConnectionInfoBase* aConnection )
    {
    CMUILOGGER_ENTERFN( 
    	"CConnectionMonitorUiContainer::ShowConnectionSummaryInformationNoteL" );
    	
 	HBufC* total = aConnection->ToStringTotalTransmittedDataLC();
    HBufC* duration = aConnection->ToStringDurationLC();
    
    CMUILOGGER_WRITE_F( "total: %S", total );
    CMUILOGGER_WRITE_F( "duration: %S", duration );
      	
    CDesCArrayFlat* strings = new( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );

    strings->AppendL( *total );
    strings->AppendL( *duration );    	
    	
    HBufC* informationText;
    informationText = StringLoader::LoadLC( R_QTN_CMON_CONNECTION_SUMMARY_NOTE_TEXT, 
    										*strings );
    										        
    CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog( 
    										CAknNoteDialog::ENoTone, 
    										CAknNoteDialog::TTimeout( 5000000 ) );
    dlg->PrepareLC( R_QTN_CMON_CONNECTION_SUMMARY_NOTE );
    dlg->SetTextWrapping( EFalse );
    
    TPtr temp( informationText->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( temp );
    
    dlg->SetCurrentLabelL( EGeneralNote, temp ); //SetTextL
    
    dlg->RunDlgLD();    
        
    CleanupStack::PopAndDestroy( informationText );
    CleanupStack::PopAndDestroy( strings );
	CleanupStack::PopAndDestroy( duration );
	CleanupStack::PopAndDestroy( total );
    
    CMUILOGGER_LEAVEFN( 
    	"CConnectionMonitorUiContainer::ShowConnectionSummaryInformationNoteL" );        
    }

// ---------------------------------------------------------
// CConnectionMonitorUiAppUi::DeleteDetailsView
// ---------------------------------------------------------
//
void CConnectionMonitorUiAppUi::DeleteDetailsView(
                                            TInt aConnId,
                                            CConnectionInfoBase* aConnInfo )
    {
    CMUILOGGER_WRITE( "CConnectionMonitorUiAppUi::DeleteDetailsView Start" );
    CMUILOGGER_WRITE_F( "aConnInfo:  %d", aConnInfo );
    CMUILOGGER_WRITE_F( "aConnId:  %d", aConnId );

	TBool deleting = aConnInfo->GetDeletedFromCMUI();
    iConnectionArray->Delete( aConnId );
    
    CMUILOGGER_WRITE_F( "Deleted: %d", aConnId );
    CMUILOGGER_WRITE( "CConnectionMonitorUiAppUi::DeleteDetailsView End" );
    }

#ifdef RD_CONTROL_PANEL

// -----------------------------------------------------------------------------
// CConnectionMonitorUiAppUi::IsEmbedded
// -----------------------------------------------------------------------------
//
TBool CConnectionMonitorUiAppUi::IsEmbedded() const
    {
    return iEikonEnv->StartedAsServerApp();
    }
    
#endif // RD_CONTROL_PANEL 


// End of File
