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
* Description:  Implementation of CApSelectorListbox.
*
*/


// INCLUDE FILES

#include <eikclbd.h>
#include <AknIconArray.h>
//#include <EIKON.mbg>
#include <avkon.mbg>
#include <aknkeys.h>
#include <apsetui.rsg>
#include <featmgr.h>

#include "ApSelectorListbox.h"
#include "ApSelectorListBoxModel.h"
#include "ApListItem.h"

#include <AknsUtils.h>
#include <apsettings.mbg>
#include <data_caging_path_literals.hrh>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS
// Drive and file name of the MBM file containing icons for Protection
_LIT( KFileIcons, "z:ApSettings.mbm" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSelectorListbox::NewL
// ---------------------------------------------------------
//
CApSelectorListbox* CApSelectorListbox::NewL
( const CCoeControl* aParent )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApSelectorListbox::CApSelectorListbox
// ---------------------------------------------------------
//
CApSelectorListbox::CApSelectorListbox()
    {
    }


// ---------------------------------------------------------
// CApSelectorListbox::~CApSelectorListbox
// ---------------------------------------------------------
//
CApSelectorListbox::~CApSelectorListbox()
    {

    }


// ---------------------------------------------------------
// CApSelectorListbox::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelectorListbox::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApSelectorListbox::FocusChanged
// ---------------------------------------------------------
//
void CApSelectorListbox::FocusChanged( TDrawNow aDrawNow )
    {
    }



// ---------------------------------------------------------
// CApSelectorListbox::LoadIconsL
// ---------------------------------------------------------
//
void CApSelectorListbox::LoadIconsL()
    {
    }


// ---------------------------------------------------------
// CApSelectorListbox::Uid4Item
// ---------------------------------------------------------
//
TUint32 CApSelectorListbox::Uid4Item( TInt aItem ) const
    {
    return 0;
    }


// ---------------------------------------------------------
// CApSelectorListbox::CurrentItemUid
// ---------------------------------------------------------
//
TUint32 CApSelectorListbox::CurrentItemUid() const
    {
    return 0;
    }



// ---------------------------------------------------------
// CApSelectorListbox::CurrentItemNameL
// ---------------------------------------------------------
//
const TDesC& CApSelectorListbox::CurrentItemNameL()
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSelectorListbox::HandleResourceChange
// ---------------------------------------------------------
//
void CApSelectorListbox::HandleResourceChange(TInt aType)
    {
    }

// End of File
