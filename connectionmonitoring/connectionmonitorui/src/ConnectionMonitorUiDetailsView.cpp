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
#include  <ConnectionMonitorUi.rsg>
#include  "ConnectionMonitorUiDetailsView.h"
#include  "ConnectionMonitorUiDetailsContainer.h" 
#include  "ConnectionMonitorUi.hrh"
#include  "ConnectionArray.h"

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// ConnectionMonitorUiDetailsView::ConnectionMonitorUiDetailsView( 
//                                 const CConnectionArray* aConnectionArray )
// ---------------------------------------------------------
//
CConnectionMonitorUiDetailsView::CConnectionMonitorUiDetailsView( 
                             CConnectionArray* const aConnectionArray )
    : CAknView(), 
      iConnectionArray( aConnectionArray ), 
      iViewRefreshState( EFalse )
    {}

// ---------------------------------------------------------
// ConnectionMonitorUiDetailsView::ConstructL()
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::ConstructL()
    {
    BaseConstructL( R_CONNECTIONMONITORUI_DETAILSVIEW );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::~CConnectionMonitorUiDetailsView()
// ---------------------------------------------------------
//
CConnectionMonitorUiDetailsView::~CConnectionMonitorUiDetailsView()
    {
    delete iContainer;
    }

// ---------------------------------------------------------
// TUid CConnectionMonitorUiDetailsView::Id()
// ?implementation_description
// ---------------------------------------------------------
//
TUid CConnectionMonitorUiDetailsView::Id() const
    {
    return KDetailsViewId;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::HandleCommandL( TInt aCommand )
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::HandleCommandL( TInt aCommand )
    {   
    switch ( aCommand )
        {
        case EAknSoftkeyOk:
            {
            CloseViewL();
            ActivateMainViewL();
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
// CConnectionMonitorUiDetailsView::CloseViewL()
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::CloseViewL()
    {   
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsView::CloseViewL" );    
    
    if ( GetViewRefreshState() )
        {
        // Now delete this view
        CConnectionMonitorUiAppUi* appUi =
            (CConnectionMonitorUiAppUi*)AppUi();
        appUi->StartTimerL();
        SetViewRefreshState( EFalse );
        }
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsView::CloseViewL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::SetSelectedConnectionId
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::SetSelectedConnection( TUint aIndex )
    {
    iConnectionId = aIndex;
    }
    
// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::GetSelectedConnection
// ---------------------------------------------------------
//
TUint CConnectionMonitorUiDetailsView::GetSelectedConnection()
	{
	TUint result( KErrNone );
	if ( iContainer )
		{
		result = iContainer->GetIndexOfSelectedItem();	
		}
	return result;		
	}


// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::DoActivateL(...)
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsView::DoActivateL" );
    
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CConnectionMonitorUiDetailsContainer( 
                                                            iConnectionArray );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect(), this );
        iContainer->SetSelectedConnectionL( iConnectionId );
        CConnectionMonitorUiAppUi::Static()->StartConnEventNotification( 
                                                                  iContainer );
        iContainer->DrawFirstAllListBoxItem();

        iContainer->PushAndRefreshNaviPaneL();
        iContainer->MakeVisible( ETrue );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
    
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsView::DoActivateL" );
    
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::DoDeactivate
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::DoDeactivate()
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsView::DoDeactivate" );
    
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        CConnectionMonitorUiAppUi::Static()->StopConnEventNotification( 
                                                             iContainer );
        iContainer->PopNaviPane();
        iContainer->MakeVisible( EFalse );

        delete iContainer;
        iContainer = NULL;
        }                
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::HandleListBoxEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::HandleListBoxEventL( 
                                                    CEikListBox* /*aListBox*/,
                                                    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        //case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            AppUi()->ActivateLocalViewL( 
                            TUid::Uid( EConnectionMonitorUiMainViewTab ) );
            }
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::GetViewRefreshState
// ---------------------------------------------------------
//
TBool CConnectionMonitorUiDetailsView::GetViewRefreshState()
    {
    return iViewRefreshState;	
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::SetViewRefreshState
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::SetViewRefreshState( 
                                                    TBool aViewRefreshState )
    {
    iViewRefreshState = aViewRefreshState;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsView::ActivateMainViewL()
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsView::ActivateMainViewL()
    {   
    AppUi()->ActivateLocalViewL( TUid::Uid( EConnectionMonitorUiMainViewTab ) );
    }


// End of File

