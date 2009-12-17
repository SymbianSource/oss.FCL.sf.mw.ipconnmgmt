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
* Description:  container control class
*
*
*/

// INCLUDE FILES
#include "ConnectionMonitorUiDetailsContainer.h"

#include <aknnavi.h>    //for CNaviPane
#include <aknnavide.h>  //for CAknNavigationDecorator
#include <StringLoader.h>
#include <ConnectionMonitorUi.rsg>

#include "ConnectionArray.h"
#include "ConnectionMonitorUiLogger.h"
#include "ConnectionMonitorUiDetailsView.h"
#include "ConnectionMonitorUi.hrh"

// CONSTANTS

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::ConstructL( const TRect& aRect )
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::ConstructL( const TRect& aRect,
                               CConnectionMonitorUiDetailsView* aParentView )
    {
    CMUILOGGER_WRITE( "CConnectionMonitorUiDetailsContainer::ConstructL" );
    CreateWindowL();
    
    iParentView = aParentView;
    iListBox = new( ELeave ) CAknSingleHeadingStyleListBox;
    iListBox->SetContainerWindowL( *this );
    TInt flags = 0;
    flags |= EAknListBoxViewerFlags;
    iListBox->ConstructL( this, flags );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                                                 CEikScrollBarFrame::EOff,
                                                 CEikScrollBarFrame::EAuto );

    CMUILOGGER_WRITE( "Creating connection details array" );

    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    SetRect( aRect );

    HBufC* noConn = StringLoader::LoadLC( R_QTN_CMON_NO_ACTIVE_CONNECTIONS,
                                          iEikonEnv );
    CListBoxView *lbview = iListBox->View();
    lbview->SetListEmptyTextL( *noConn );
    CleanupStack::PopAndDestroy( noConn );


    GetPanesL();

    ActivateL();
    CMUILOGGER_WRITE( "CConnectionMonitorUiDetailsContainer::ConstructL" );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::SetSelectedConnectionL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::SetSelectedConnectionL(
                                                    TUint aConnectionIndex )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsContainer::SetSelectedConnectionL" );
    CMUILOGGER_WRITE_F( "aConnectionIndex : %d", &aConnectionIndex );
    CMUILOGGER_WRITE_F( "iConnectionArray->MdcaCount() : %d",
                        iConnectionArray->MdcaCount() );
    
    CConnectionInfoBase* temp = ( *iConnectionArray ).At( aConnectionIndex );
    CDesCArrayFlat* tempArray = temp ? temp->GetDetailsArray() : NULL ;
    if ( tempArray )
        {
        iConnectionIndex = aConnectionIndex;
        iConnectionInfo = temp;
        iListBox->Model()->SetItemTextArray( tempArray );
        iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
        iListBox->Reset();
        iListBox->UpdateScrollBarsL();        
        }
    
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsContainer::SetSelectedConnectionL" );    
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::~CConnectionMonitorUiDetailsContainer
// ---------------------------------------------------------
//
CConnectionMonitorUiDetailsContainer::~CConnectionMonitorUiDetailsContainer()
    {
    delete iListBox;
    delete iNaviDecorator;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiContainer::CConnectionMonitorUiContainer
// ---------------------------------------------------------
//
CConnectionMonitorUiDetailsContainer::CConnectionMonitorUiDetailsContainer(
                             const CConnectionArray* const aConnectionArray )
    : iConnectionArray( aConnectionArray )
    {
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiDetailsContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::ComponentControl( TInt aIndex ) const
// ---------------------------------------------------------
//
CCoeControl* CConnectionMonitorUiDetailsContainer::ComponentControl(
                                                       TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }
	
// ----------------------------------------------------------------------------
// CLogsDetailView::HandleNaviDecoratorEventL
// Handler for pointer events, when the Navi Pane arrows have been tapped
// ----------------------------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::HandleNaviDecoratorEventL( TInt aEventID )
    {
    switch (aEventID)
        {
        case MAknNaviDecoratorObserver::EAknNaviDecoratorEventRightTabArrow:
            if ( iConnectionIndex < (iConnectionArray->MdcaCount() - 1) ) {
                SetSelectedConnectionL(iConnectionIndex + 1);
            }
            break;
        
        case MAknNaviDecoratorObserver::EAknNaviDecoratorEventLeftTabArrow:
            if ( iConnectionIndex > 0 ) {
                SetSelectedConnectionL(iConnectionIndex - 1);
            }
            break;
            
        default:
            return;
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CConnectionMonitorUiDetailsContainer::OfferKeyEventL(
                                                   const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsContainer::OfferKeyEventL" );    
    
    TKeyResponse result( EKeyWasNotConsumed );
    TInt index = iConnectionIndex;
    TInt index_closed = iConnectionIndex;
    CConnectionInfoBase* connection = NULL;
    if ( aType == EEventKey )
        {
        if ( ( aKeyEvent.iCode == EKeyLeftArrow ) ||
            ( aKeyEvent.iCode == EKeyRightArrow ) )
            {
            
            TInt dir = 0;
            switch (aKeyEvent.iCode)
                {
                case EKeyLeftArrow:
                    {
                    dir = -1;
                    break;
                    }
                case EKeyRightArrow:
                    {
                    dir = 1;
                    break;
                    }
                }
            // Handle mirrored layout by negating the directionality
            if (AknLayoutUtils::LayoutMirrored())
                {
                dir = -1 * dir;
                }
                
            index += dir; 
            
            TBool step( EFalse );
            
            CMUILOGGER_WRITE_F( "OfferKeyEventL index: %d", index );
            
            while ( ( index > -1 ) && ( index < iConnectionArray->MdcaCount()
                    && !step ) )
                {
                connection = ( *iConnectionArray )[index];
                TBool alive = connection->IsAlive();
                CMUILOGGER_WRITE_F( "OfferKeyEventL alive: %b", alive );

                if ( alive )
                    {
                    if ( ( index_closed > -1 ) && 
                       ( index_closed < iConnectionArray->MdcaCount() ) )
                        {
                        CConnectionInfoBase* connection_closed 
                                       = ( *iConnectionArray )[index_closed];

                        if ( iParentView->GetViewRefreshState() )
                             {
                             step = ETrue;
                             CConnectionMonitorUiAppUi* appUi =
                                            (CConnectionMonitorUiAppUi*)
                                            iEikonEnv->EikAppUi();
                            appUi->DeleteDetailsView( 
                                        GetSelectedConnectionInfo()
                                                        ->GetConnectionId(),
                                        GetSelectedConnectionInfo() );
                            appUi->StartTimerL();
                            iParentView->SetViewRefreshState( EFalse );
                            if ( index_closed > index )
                                {
                                iConnectionInfo = connection;
                                iConnectionIndex = index;
                                }
                            iListBox->Model()->SetItemTextArray(
                                            connection->GetDetailsArray() );
                            iListBox->DrawNow();
                            iListBox->UpdateScrollBarsL();
                            PushAndRefreshNaviPaneL();
                            }
                        else
                            {
                            step = ETrue;
                            iConnectionInfo = connection;
                            iConnectionIndex = index;
                            iListBox->Model()->SetItemTextArray(
                                            connection->GetDetailsArray() );
                            iListBox->DrawNow();
                            iListBox->UpdateScrollBarsL();
                            PushAndRefreshNaviPaneL();
                            }
                        }
                    }
                else
                    {                        
                    index += dir; 
                    }    
                }
            result = iListBox->OfferKeyEventL( aKeyEvent, aType );                
            }
        else if ( aKeyEvent.iCode == EKeyDevice3 )
            // selection key press -> go back to main view
            {
            CConnectionMonitorUiAppUi::Static()->ActivateLocalViewL(
                            TUid::Uid( EConnectionMonitorUiMainViewTab ) );
            result = EKeyWasConsumed;
            }
        else
            {
            result = iListBox->OfferKeyEventL( aKeyEvent, aType );
            }
        }

    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsContainer::OfferKeyEventL" );    

    return result;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::OnEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::OnEventL(
                                    const CConnMonEventBase& aConnMonEvent,
                                    const TInt aIndex )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsContainer::OnEventL()" );
    CMUILOGGER_WRITE_F( "aIndex : %d", aIndex );
    
    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonDeleteConnection:
            {
            if ( aIndex == iConnectionIndex )
                {
                TUint count = iConnectionArray->MdcaCount();
                switch ( count )
                    {
                    case 0:
                        {
                        iListBox->Model()->SetItemTextArray(
                                           new (ELeave) CDesCArrayFlat( 1 ) );
                        iListBox->Model()->SetOwnershipType(
                                                          ELbmOwnsItemArray );
                        iConnectionIndex = 0;
                        iConnectionInfo = NULL;
                        break;
                        }
                    case 1:
                        {
                        SetSelectedConnectionL( 0 );
                        break;
                        }
                    default :
                        {
                        TInt index = iConnectionIndex < count ?
                                                        iConnectionIndex :
                                                        iConnectionIndex - 1;
                        SetSelectedConnectionL( index );
                        break;
                        }
                    }
                iListBox->DrawNow();
                iListBox->UpdateScrollBarsL();
                }
            break;
            }
        case EConnMonCreateConnection:
            {
        CMUILOGGER_WRITE_F( "iConnectionInfo : %d", iConnectionInfo );
            
            if ( !iConnectionInfo )
                {
                SetSelectedConnectionL( 0 );
                }
            break;
            }
        default:
            {
            if ( !iConnectionInfo )
                {
                SetSelectedConnectionL( 0 );
                }            
            else if ( ( aIndex == iConnectionIndex ) && ( iConnectionArray->MdcaCount() ) )
                {
                iListBox->DrawNow();
                iListBox->UpdateScrollBarsL();
                }
            break;
            }
        }
    PushAndRefreshNaviPaneL();
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsContainer::OnEventL()" );    
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::OnTimerEventL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::OnTimerEventL()
    {
    if ( iConnectionArray->MdcaCount() )
       {
        iListBox->DrawNow();
        iListBox->UpdateScrollBarsL();
        CMUILOGGER_WRITE_F( "CMUIDW GetStatus() before: %d", 
                            iConnectionInfo->GetStatus() );
        if ( iConnectionInfo->GetStatus() == EConnectionClosing )
            {
            CConnectionMonitorUiAppUi* tempApUi = 
                        CConnectionMonitorUiAppUi::Static();
            tempApUi->StopTimer();
            iConnectionInfo->StatusChangedL();
            }
        CMUILOGGER_WRITE_F( "CMUIDW GetStatus() after: %d",
                            iConnectionInfo->GetStatus() );
        }
    PushAndRefreshNaviPaneL();
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::DrawFirstAllListBoxItem
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::DrawFirstAllListBoxItem()
    {
    TInt count = iListBox->Model()->NumberOfItems() - 1;
    for ( TInt i = count; i > -1; --i )
        {
        iListBox->DrawItem( i );
        }
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::ListBox
// ---------------------------------------------------------
//
CAknSingleHeadingStyleListBox*
                        CConnectionMonitorUiDetailsContainer::ListBox() const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::GetPanesL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::GetPanesL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();

    iNaviPane =  STATIC_CAST( CAknNavigationControlContainer*,
                  statusPane->ControlL( TUid::Uid(EEikStatusPaneUidNavi ) ) );

    PushAndRefreshNaviPaneL();
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::PushAndRefreshNaviPaneL
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::PushAndRefreshNaviPaneL()
    {
    TUint countOfConnection = iConnectionArray->NumberOfActiveConnections();
    CMUILOGGER_WRITE_F( "countOfConnection : %d", countOfConnection );
    
    if ( ( iConnectionIndex > countOfConnection ) && // spec. index update
         ( countOfConnection == 0 ) )
        {
        iConnectionIndex = iConnectionArray->
                                GetArrayIndex( 
                                        GetSelectedConnectionInfo()
                                                    ->GetConnectionId() );
        }
    
    CMUILOGGER_WRITE_F( "iConnectionIndex : %d", iConnectionIndex );

    if ( iNaviDecorator )
        {
        PopNaviPane();
        }
    
    if ( iParentView->GetViewRefreshState() ) // connection closed
        {
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
        }
    else // connection alive
        {
        HBufC* textOfNaviPane = NULL;

        CArrayFixFlat<TInt>* array = new( ELeave ) CArrayFixFlat<TInt>( 2 );
        CleanupStack::PushL( array );
        
        if ( ( countOfConnection == 1 ) &&
             ( iConnectionIndex + 1 >= countOfConnection ) )
            {
            array->AppendL( countOfConnection );
            }
        else            
            {            
            array->AppendL( countOfConnection ? iConnectionIndex + 1 : 0 );
            }
            
        array->AppendL( countOfConnection );
        textOfNaviPane = StringLoader::LoadLC( 
                                            R_QTN_CMON_NAVI_NOF_CONNECTION,
                                            *array );

        iNaviDecorator = iNaviPane->CreateNavigationLabelL( 
                                                        *textOfNaviPane );
        
        CleanupStack::PopAndDestroy( textOfNaviPane );
        CleanupStack::PopAndDestroy( array );
        }

    iNaviDecorator->MakeScrollButtonVisible( ETrue );
    
    CAknNavigationDecorator::TScrollButton left = CAknNavigationDecorator::ELeftButton;
    CAknNavigationDecorator::TScrollButton right = CAknNavigationDecorator::ERightButton;
    if ( AknLayoutUtils::LayoutMirrored() )
    	{
    	left = CAknNavigationDecorator::ERightButton;
    	right = CAknNavigationDecorator::ELeftButton;
    	}

    if ( iParentView->GetViewRefreshState() )
        {
        iNaviDecorator->SetScrollButtonDimmed( left, 
                                           ( ( iConnectionIndex == 0 ) ||
                                             ( countOfConnection == 0 ) ) );
        }
    else
        {
        iNaviDecorator->SetScrollButtonDimmed( 
                          left, 
                          ( ( iConnectionIndex == 0 ) ||
                            ( countOfConnection == 0 ) ||
                            ( ( countOfConnection == 1 ) &&
                              ( iConnectionIndex + 1 >= 
                                    countOfConnection ) ) ) );
        }

    if ( iParentView->GetViewRefreshState() )
        {
        iNaviDecorator->SetScrollButtonDimmed( right, 
    			                iConnectionIndex >= countOfConnection );
        }
    else
        {
        iNaviDecorator->SetScrollButtonDimmed( right, 
    			                iConnectionIndex + 1 >= countOfConnection );
        }
        
    iNaviDecorator->SetNaviDecoratorObserver( this );
        
    iNaviPane->PushL( *iNaviDecorator );
    }


// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::PopNaviPane
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::PopNaviPane()
    {
    iNaviPane->Pop( iNaviDecorator );
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }


// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );
        SetRect( mainPaneRect );
        DrawNow();
        }
    }
    
// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::FocusChanged
// ---------------------------------------------------------
//
void CConnectionMonitorUiDetailsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CMUILOGGER_ENTERFN( "CConnectionMonitorUiDetailsContainer::FocusChanged" );
    CCoeControl::FocusChanged( aDrawNow );    
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }        
    CMUILOGGER_LEAVEFN( "CConnectionMonitorUiDetailsContainer::FocusChanged" );    
    }    
    
    
// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::GetIndexOfSelectedItem
// ---------------------------------------------------------
//
TInt CConnectionMonitorUiDetailsContainer::GetIndexOfSelectedItem()
	{
    CMUILOGGER_WRITE_F(
        "CMUIDetailsContainer::GetIndexOfSelectedItem  iConnectionIndex: %d",
        iConnectionIndex );

    return iConnectionIndex;	
    }

// ---------------------------------------------------------
// CConnectionMonitorUiDetailsContainer::GetSelectedConnectionInfo()
// ---------------------------------------------------------
//
    CConnectionInfoBase* CConnectionMonitorUiDetailsContainer::
                                        GetSelectedConnectionInfo() const
	{
	return iConnectionInfo;	
	}
    

// End of File
