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
CGSConnSettingsSelectionDlg* CGSConnSettingsSelectionDlg::NewL( TInt aResourceID, 
                TInt& aCurrentSelectionIndex, 
                const MDesCArray* aItemArray )
    {
    CGSConnSettingsSelectionDlg* self = new( ELeave ) CGSConnSettingsSelectionDlg(
                                                        aResourceID,
                                                        aCurrentSelectionIndex,
                                                        aItemArray );
    return self;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::CGSConnSettingsSelectionDlg
// ---------------------------------------------------------------------------
//
CGSConnSettingsSelectionDlg::CGSConnSettingsSelectionDlg(
                TInt aResourceID, 
                TInt& aCurrentSelectionIndex, 
                const MDesCArray* aItemArray )
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
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsSelectionDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGSConnSettingsSelectionDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType)
    {    
    TKeyResponse response( EKeyWasNotConsumed );
    response =  CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEvent, aType );
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
    CAknRadioButtonSettingPage::HandleListBoxEventL( aListBox, aEventType );
    }

// End of file

