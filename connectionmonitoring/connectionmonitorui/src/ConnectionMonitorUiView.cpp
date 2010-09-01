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
* Description:  view class
*     
*
*/


// INCLUDE FILES
#include  <eikmenup.h>
#include  <akntitle.h>   //for CAknTitlePane
#include  <ConnectionMonitorUi.rsg>
#include  <StringLoader.h>

#include  <hlplch.h>
#include <csxhelp/cmon.hlp.hrh>

    
#include  "ConnectionMonitorUiDetailsView.h"
#include  "ConnectionMonitorUi.hrh"
#include  "ConnectionMonitorUiAppUi.h"
#include  "ConnectionMonitorUiView.h"
#include  "ConnectionMonitorUiContainer.h" 
#include  "ConnectionArray.h"
#include  "FeatureManagerWrapper.h"


// CONSTANTS 

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CConnectionMonitorUiView::CConnectionMonitorUiView
// ---------------------------------------------------------
//
#ifdef RD_CONTROL_PANEL
CConnectionMonitorUiView::CConnectionMonitorUiView( 
                                    CConnectionArray* const aConnectionArray,
                                    RConnectionMonitor* aMonitor,
                                    TBool aIsEmbedded,
                                    CActiveWrapper* aActiveWrapper )
    : CAknView(),
      iConnectionArray( aConnectionArray ),
      iMonitor( aMonitor ),
      isHelpSupported( EFalse ),
      iIsEmbedded( EFalse ),
      iActiveWrapper( aActiveWrapper )
    {
   	iIsEmbedded = aIsEmbedded;
    }
#else
CConnectionMonitorUiView::CConnectionMonitorUiView( 
                                    CConnectionArray* const aConnectionArray,
                                    RConnectionMonitor* aMonitor,
                                    CActiveWrapper* aActiveWrapper )
    : CAknView(),
      iConnectionArray( aConnectionArray ),
      iMonitor( aMonitor ),
      isHelpSupported( EFalse ),
      iActiveWrapper( aActiveWrapper )
    {
    }
#endif // RD_CONTROL_PANEL

// ---------------------------------------------------------
// CConnectionMonitorUiView::ConstructL
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::ConstructL()
    {
    isHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );

    TInt actView( NULL );

#ifdef RD_CONTROL_PANEL
    if ( iIsEmbedded )
        {
        actView = R_CP_CONNECTIONMONITORUI_VIEW;
        }
    else
        {
        actView = R_CONNECTIONMONITORUI_VIEW;
        }
#else
        actView = R_CONNECTIONMONITORUI_VIEW;
#endif // RD_CONTROL_PANEL

    BaseConstructL( actView );        
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::~CConnectionMonitorUiView()
// ---------------------------------------------------------
//
CConnectionMonitorUiView::~CConnectionMonitorUiView()
    {
    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CConnectionMonitorUiView::Id()
// ---------------------------------------------------------
//
TUid CConnectionMonitorUiView::Id() const
    {
    return KConnectionsViewId;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::HandleCommandL( TInt aCommand )
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::HandleCommandL( TInt aCommand )
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyDetails:
            {
            if ( IsSelectedConnectionAlive() )
                {
                BringupDetailsViewL();
                }
            break;
            }
        case EConnectionMonitorUiCmdAppDetails:
            {
            BringupDetailsViewL();
            break;
            }
        case EConnectionMonitorUiCmdAppEndConn:
            {
            iContainer->EndConnectionL();
            break;
            }
        case EConnectionMonitorUiCmdAppEndAll:
            {
            iContainer->EndAllConnectionL();
            break;
            }
       case EAknCmdHelp:
            {
            if ( isHelpSupported )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    CEikonEnv::Static()->WsSession(),
                    CEikonEnv::Static()->EikAppUi()->AppHelpContextL() );
                }
            break;
            }
       default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::DynInitMenuPaneL()
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::DynInitMenuPaneL( TInt aResourceId,
                                                 CEikMenuPane* aMenuPane )
    {
    MEikMenuObserver::DynInitMenuPaneL( aResourceId, aMenuPane );
    // This is a security measure to prevent menu pane updates when
    // the main view is not active. This should not happen but
    // just to be safe it is checked.
    if ( !iContainer )
        {
        return;
        }
    switch ( aResourceId )
        {
        case R_CONNECTIONMONITORUI_VIEW_MENU:
            {
            if ( !iConnectionArray->MdcaCount() )
                {
                aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppDetails );
                aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndConn );
                aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndAll );
                }
            else if ( iConnectionArray->MdcaCount() == 1 )
                {
                aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndAll );
                if ( !IsSelectedConnectionAlive() )
                    {
                    aMenuPane->DeleteMenuItem( 
                                    EConnectionMonitorUiCmdAppDetails );
                    }
                else if ( IsSelectedConnectionSuspended() )
                    {
                    aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndConn );
                    }
                }
            else if ( iConnectionArray->NumberOfSuspendedConnections() >= 1 )
                {
                aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndAll );
                if ( IsSelectedConnectionSuspended() )
                    {
                    aMenuPane->DeleteMenuItem( 
                                EConnectionMonitorUiCmdAppEndConn );
                    }
                }
            else
                {
                if ( !IsSelectedConnectionAlive() )
                    {
                    aMenuPane->DeleteMenuItem( 
                                    EConnectionMonitorUiCmdAppDetails );
                    aMenuPane->DeleteMenuItem( 
                                    EConnectionMonitorUiCmdAppEndConn );
                    }
                }
            break;
            }
        case R_CONNECTIONMONITORUI_APP_MENU:
            {
            if ( !isHelpSupported )
                {
                aMenuPane->DeleteMenuItem( EAknCmdHelp );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::HandleListBoxEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        //case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            if ( IsSelectedConnectionAlive() )
                {
                BringupDetailsViewL();
                }
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::DoActivateL
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::DoActivateL(
                                            const TVwsViewId& /*aPrevViewId*/, 
                                            TUid /*aCustomMessageId*/,
                                            const TDesC8& /*aCustomMessage*/ )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiView::DoActivateL" );
    
    if ( !iContainer )
        {
        iContainer = new( ELeave ) CConnectionMonitorUiContainer( 
                                                            iConnectionArray,
                                                            iMonitor,
                                                            iActiveWrapper );
        iContainer->SetMopParent( this );
        
        
	    CConnectionMonitorUiDetailsView* view = 
	        STATIC_CAST( CConnectionMonitorUiDetailsView*,
	        AppUi()->View( TUid::Uid( EConnectionMonitorUiDetailsViewTab ) ) );

	    TInt index  = view->GetSelectedConnection();
    	CMUILOGGER_WRITE_F( "index: %d", index );
                
        iContainer->ConstructL( index, ClientRect() );
        iContainer->ListBox()->SetListBoxObserver( this );

        CConnectionMonitorUiAppUi::Static()->StartConnEventNotification( 
                                                                  iContainer );
        iContainer->PushAndRefreshNaviPaneL();
        AppUi()->AddToViewStackL( *this, iContainer );
        ConnectionKeyObserver();
        }
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiView::DoActivateL" );
    
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::DoDeactivate
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::DoDeactivate()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiView::DoDeactivate" );
    
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        CConnectionMonitorUiAppUi::Static()->StopConnEventNotification( 
                                                                  iContainer );
        iContainer->PopNaviPane();
        delete iContainer;
        iContainer = NULL;
        }
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiView::DoDeactivate" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::SetTitlePaneTextL
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::SetTitlePaneTextL( TInt aResourceId )
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* titlePane =
        ( CAknTitlePane* )statusPane->ControlL(
                            TUid::Uid( EEikStatusPaneUidTitle ) );
    HBufC* text = iEikonEnv->AllocReadResourceLC( aResourceId );
    titlePane->SetTextL( text->Des() );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::BringupDetailsViewL
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::BringupDetailsViewL()
    {
    CConnectionMonitorUiDetailsView* view = 
        STATIC_CAST( CConnectionMonitorUiDetailsView*,
                     AppUi()->View( TUid::Uid( 
                     EConnectionMonitorUiDetailsViewTab ) ) );

    TInt selIndex = iContainer->SelectedConnection();
    view->SetSelectedConnection( selIndex );
    AppUi()->ActivateLocalViewL( 
                             TUid::Uid( EConnectionMonitorUiDetailsViewTab ) );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiView::IsSelectedConnectionAlive
// ---------------------------------------------------------
//
TBool CConnectionMonitorUiView::IsSelectedConnectionAlive() const
    {
    TInt selIndex = iContainer->SelectedConnection();
    TBool result( EFalse );
    if ( selIndex > -1 )
        {
        result = ( *iConnectionArray )[selIndex]->IsAlive();
        }

    return result;
    }
    
// ---------------------------------------------------------
// CConnectionMonitorUiView::IsSelectedConnectionSuspended
// ---------------------------------------------------------
//
TBool CConnectionMonitorUiView::IsSelectedConnectionSuspended() const
    {
    TInt selIndex = iContainer->SelectedConnection();
    TBool result( EFalse );
    if ( selIndex > -1 )
        {
        result = ( *iConnectionArray )[selIndex]->IsSuspended();
        }

    return result;
    }    
    
// ---------------------------------------------------------
// CConnectionMonitorUiView::ConnectionKeyObserver
// ---------------------------------------------------------
//
void CConnectionMonitorUiView::ConnectionKeyObserver() const
    {
    if ( !( iConnectionArray->MdcaCount() ) )
        {
        Cba()->MakeCommandVisible( EAknSoftkeyDetails,EFalse );
        }
    else
        {
        if ( iConnectionArray->NumberOfActiveConnections() )
            {
            Cba()->MakeCommandVisible( EAknSoftkeyDetails,ETrue );
            }
        else
            {
            Cba()->MakeCommandVisible( EAknSoftkeyDetails,EFalse );
            }
        }
	}


    

// End of File
