/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Cpp file for CGSConnSettingsSelectionDlg class.
*
*/

// Includes
#include "gsconnsettingsselectiondlg.h"
#include <AknInfoPopupNoteController.h>
#include <StringLoader.h>
#include <eiklbo.h>
#include <gulalign.h>

#include <gsconnsettingspluginrsc.rsg>

// Constants


// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::NewL
// ---------------------------------------------------------------------------
//
CGSConnSettingsSelectionDlg* CGSConnSettingsSelectionDlg::NewL(TInt aResourceID, 
                TInt& aCurrentSelectionIndex, 
                const MDesCArray* aItemArray,
                TInt aPopupResource)
    {
    CGSConnSettingsSelectionDlg* self = new( ELeave ) CGSConnSettingsSelectionDlg(
                                                        aResourceID,
                                                        aCurrentSelectionIndex,
                                                        aItemArray,
                                                        aPopupResource );
    CleanupStack::PushL( self );
    self->ConstructL( aPopupResource );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CGSConnSettingsSelectionDlg::ConstructL( TInt aPopupResource)
    {
    iPopupController = CAknInfoPopupNoteController::NewL();
    iPopupItems = iCoeEnv->ReadDesC16ArrayResourceL( aPopupResource );
//    ShowInfoPopupL();
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::CGSConnSettingsSelectionDlg
// ---------------------------------------------------------------------------
//
CGSConnSettingsSelectionDlg::CGSConnSettingsSelectionDlg(
                TInt aResourceID, 
                TInt& aCurrentSelectionIndex, 
                const MDesCArray* aItemArray,
                TInt /* aPopupResource */)
        :    CAknRadioButtonSettingPage( 
                aResourceID,
                aCurrentSelectionIndex,
                aItemArray )
    {

    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::~CGSConnSettingsSelectionDlg
// ---------------------------------------------------------------------------
//
CGSConnSettingsSelectionDlg::~CGSConnSettingsSelectionDlg()
    {
    if ( iPopupController )
        {
        delete iPopupController;
        }
    
    if ( iPopupItems )
        {
        iPopupItems->Reset();
        delete iPopupItems;        
        }
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGSConnSettingsSelectionDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType)
    {    
    TKeyResponse response ( EKeyWasNotConsumed );
    
    response =  CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEvent, aType );
    
    //EKeyNull enables here launching of popup note immediately when we enter the setting page
    if ( aKeyEvent.iCode == EKeyNull ||
         aKeyEvent.iCode == EKeyUpArrow || 
         aKeyEvent.iCode == EKeyDownArrow )
        {
        ShowInfoPopupL();
        }   
    return response;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CGSConnSettingsSelectionDlg::HandleListBoxEventL( 
        CEikListBox* aListBox, 
        TListBoxEvent aEventType )
    {
    if ( aEventType == EEventItemClicked || 
         aEventType == EEventItemSingleClicked )
        {
        ShowInfoPopupL();
        }
    CAknRadioButtonSettingPage::HandleListBoxEventL(aListBox, aEventType);
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::ShowInfoPopupL
// ---------------------------------------------------------------------------
//
void CGSConnSettingsSelectionDlg::ShowInfoPopupL()
    {
    TInt currentIndex = this->ListBoxControl()->CurrentItemIndex();

    iPopupController->HideInfoPopupNote();

    iPopupController->SetTextL( (*iPopupItems)[currentIndex] );
    iPopupController->SetTimeDelayBeforeShow( 500 );
    iPopupController->SetTimePopupInView( 0 ); //Zero means that popup stays in screen until it is destroyed
    iPopupController->ShowInfoPopupNote();
    }

// End of file

