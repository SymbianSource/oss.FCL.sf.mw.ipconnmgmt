/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  APControl list Plugin container
*
*/


// INCLUDE FILES
#include <hlplch.h>          //for help
#include <csxhelp/cp.hlp.hrh>


#include <akntitle.h>        //for CAknTitlePane
#include <barsread.h>        //for TResourceReader
#include <aknlists.h>        //for CListBox
#include <aknnavi.h>         //for CNaviPane
#include <aknnavide.h>       //for CAknNavigationDecorator
#include <apcontrollistpluginrsc.rsg>
#include <etelmm.h>

#include "apcontrollistplugincontainer.h"
#include "apcontrollistplugin.h"
#include "apcontrollistplugin.hrh"
#include "apcontrollistbox.h"
#include "apcontrollistboxmodel.h"
#include "apcontrollistpluginlogger.h"

// CONSTANTS
_LIT( KEmpty, "");

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------

// ---------------------------------------------------------
// CAPControlListPluginContainer::ConstructL
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::ConstructL( const TRect& aRect, 
                                              MEikListBoxObserver* aListObserver )
    {       
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::ConstructL" ) ) );
    if ( !iListBox )
        {

        TResourceReader rr;

        //creates resource reader. reader is pushed to cleaup stack
        ControlEnv()->CreateResourceReaderLC( rr, R_APCONTROL_LISTBOX );

        CreateWindowL();

        iListBox = new( ELeave ) CApControlListbox;
        iListBox->SetContainerWindowL( *this );

        iListBox->CreateScrollBarFrameL( ETrue );
        iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                        CEikScrollBarFrame::EOff, 
                                        CEikScrollBarFrame::EAuto );

        iListBox->ConstructFromResourceL( rr );
	    
	    //Put in empty text thing
	    iListBox->SetListEmptyTextL();
        iListBox->SetListBoxObserver( aListObserver );

    	iModel = new( ELeave )CApControlListboxModel;
    	iModel->ConstructL();
    	iListBox->Model()->SetItemTextArray( iModel );//give ownership of
    	// iModel to iListBox
        
        CleanupStack::PopAndDestroy(); //resource reader
        
        SetRect( aRect );
                                                           
        HandleResourceChange( KEikDynamicLayoutVariantSwitch );
        }      
        

    TitlePaneTextsL( R_APCLPLUGIN_TITLE );
    ActivateL();
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::ConstructL" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPluginContainer::AddToListBoxL
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::AddToListBoxL(TPtrC& newItem)
    {
    CLOG( ( ESelector, 0, _L( 
        "-> CAPControlListPluginContainer::AddToListBoxL" ) ) );
	MDesCArray* textArray = iModel->ItemTextArray();
	CDesCArray* listBoxItems = static_cast<CDesCArray*>(textArray);

	TBuf<RMobilePhone::KMaxApnName> item;
	// RMobilePhone::KMaxApnName - label length + TAB = 
	// RMobilePhone::KMaxApnName 
	item.Format(_L("\t%S"), &newItem); 
	listBoxItems->AppendL(item);

	iListBox->HandleItemAdditionL(); // Update listbox
	iListBox->SetCurrentItemIndexAndDraw(
	    listBoxItems->Count() - 1); // select new item
    CLOG( ( ESelector, 0, _L( 
        "<- CAPControlListPluginContainer::AddToListBoxL" ) ) );
    }
// ---------------------------------------------------------
// CAPControlListPluginContainer::CurrentItemIndex
// ---------------------------------------------------------
//
TInt CAPControlListPluginContainer::CurrentItemIndex()
    {
    CLOG( ( ESelector, 0, _L( 
        "-> CAPControlListPluginContainer::CurrentItemIndex" ) ) );
    CLOG( ( ESelector, 0, _L( 
        "<- CAPControlListPluginContainer::CurrentItemIndex" ) ) );
	return iListBox->CurrentItemIndex(); 
    }
    
// ---------------------------------------------------------
// CAPControlListPluginContainer::SetCurrentItemIndex
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::SetCurrentItemIndex(TInt index)
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::SetCurrentItemIndex" ) ) );
	iListBox->SetCurrentItemIndexAndDraw(index); 
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::SetCurrentItemIndex" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPluginContainer::RemoveFromListBoxL
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::RemoveFromListBoxL()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::RemoveFromListBoxL" ) ) );
	TInt currentItem = iListBox->CurrentItemIndex();

	MDesCArray* textArray = iModel->ItemTextArray();
	CDesCArray* listBoxItems = static_cast<CDesCArray*>(textArray);
	listBoxItems->Delete(currentItem, 1); // 1 = how many items to delete
	AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(iListBox, currentItem, ETrue);
	iListBox->DrawNow(); // Update listbox    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::RemoveFromListBoxL" ) ) );
	}

// ---------------------------------------------------------
// CAPControlListPluginContainer::~CAPControlListPluginContainer
// ---------------------------------------------------------
//
CAPControlListPluginContainer::~CAPControlListPluginContainer()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::~CAPControlListPluginContainer" ) ) );
    delete iListBox;
    if (iNaviPane)
        {
        iNaviPane->Pop(iNaviDecorator);        
        }
    delete iNaviDecorator;    
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::~CAPControlListPluginContainer" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CAPControlListPluginContainer::CountComponentControls() const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::CountComponentControls" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::CountComponentControls" ) ) );
    return 1;
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CAPControlListPluginContainer::ComponentControl( TInt aIndex ) const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::ComponentControl" ) ) );
    switch( aIndex )
        {
        case 0 :
            {
    		CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::ComponentControl 1" ) ) );
            return iListBox;
            }
        default:
            {
    		CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::ComponentControl 2" ) ) );
            return 0;
            } 
        }
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAPControlListPluginContainer::OfferKeyEventL( 
                                            const TKeyEvent& aKeyEvent, 
                                            TEventCode aType )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::OfferKeyEventL" ) ) );
    TKeyResponse response = EKeyWasNotConsumed;
    if ( aType == EEventKey )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::OfferKeyEventL" ) ) );
    return response;
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::SizeChanged
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::SizeChanged()
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::SizeChanged" ) ) );
    iListBox->SetRect( Rect() );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::SizeChanged" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::GetHelpContext()
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::GetHelpContext( TCoeHelpContext& aContext) const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::GetHelpContext" ) ) );
    aContext.iMajor = KHelpUidAPControlList;
    aContext.iContext = KACL_HLP_MAIN;
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::GetHelpContext" ) ) );
    }

// ---------------------------------------------------------
// CAPControlListPluginContainer::TitlePaneTextsL
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::TitlePaneTextsL( TInt aResourceId )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::TitlePaneTextsL" ) ) );
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* titlePane =
        ( CAknTitlePane* )statusPane->ControlL(
                            TUid::Uid( EEikStatusPaneUidTitle ) );

    //creates and sets title. title is pushed to cleanup stack
    HBufC* title = iEikonEnv->AllocReadResourceLC( aResourceId );
    titlePane->SetTextL( *title );

    iNaviPane = STATIC_CAST( CAknNavigationControlContainer*, 
                 statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( KEmpty );
    iNaviPane->PushL( *iNaviDecorator );

    CleanupStack::PopAndDestroy(title);
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::TitlePaneTextsL" ) ) );
    }


// ---------------------------------------------------------
// CAPControlListPluginContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::HandleResourceChange( TInt aType )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::HandleResourceChange" ) ) );
    CCoeControl::HandleResourceChange( aType );
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );
        SetRect( mainPaneRect );
        DrawNow();
        }
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::HandleResourceChange" ) ) );
    }
    
// ---------------------------------------------------------
// CAPControlListPluginContainer::FocusChanged
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::FocusChanged" ) ) );
    CCoeControl::FocusChanged( aDrawNow );
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }        
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::FocusChanged" ) ) );
    }
           
// ---------------------------------------------------------
// CAPControlListPluginContainer::CurrentItemNameL
// ---------------------------------------------------------
//
TInt CAPControlListPluginContainer::NumberOfItems()  const
    {
    CLOG( ( ESelector, 0, _L( "-> CAPControlListPluginContainer::NumberOfItems" ) ) );
    CLOG( ( ESelector, 0, _L( "<- CAPControlListPluginContainer::NumberOfItems" ) ) );
    return iListBox->Model()->ItemTextArray()->MdcaCount();
    }
    
// ---------------------------------------------------------
// CAPControlListPluginContainer::WriteToNaviPaneL
// ---------------------------------------------------------
//
void CAPControlListPluginContainer::WriteToNaviPaneL( TInt aResourceId )
    {
    CLOG( ( ESelector, 0, 
        _L( "-> CAPControlListPluginContainer::WriteToNaviPaneL" ) ) );
    HBufC* text = iCoeEnv->AllocReadResourceLC( aResourceId );
    if ( iNaviDecorator )
        {
        iNaviPane->Pop(iNaviDecorator);
        delete iNaviDecorator;
        iNaviDecorator = NULL;// CodeScanner needs this line
        }
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( *text );
    iNaviPane->PushL( *iNaviDecorator );
    
    CleanupStack::PopAndDestroy( text );
    CLOG( ( ESelector, 0, 
        _L( "<- CAPControlListPluginContainer::WriteToNaviPaneL" ) ) );
    }
    
// End of File  
