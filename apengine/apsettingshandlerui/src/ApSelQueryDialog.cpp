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
* Description: 
*     Defines dialog CApSelQueryDialog for access point selection.
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <ApListItem.h>
#include <AknIconArray.h>
#include <AknsUtils.h>

#include <apsetui.rsg>
#include <apsettings.mbg>

#include <featmgr.h>

#include "ApSelectorListBoxModel.h"
#include "ApSettingsModel.h"
#include "ApSettingsHandlerUI.hrh"
#include "ApSelQueryDialog.h"


#include "ApSettingsHandlerLogger.h"
#include <data_caging_path_literals.hrh>

// CONSTANTS
// Drive and file name of the MBM file containing icons for Protection
_LIT( KFileIcons, "z:ApSettings.mbm" );


// ================= MEMBER FUNCTIONS =======================

// Destructor
CApSelQueryDialog::~CApSelQueryDialog()
    {
    }



// Constructor
CApSelQueryDialog::CApSelQueryDialog( CApSettingsModel& aModel,
                                      TUint32* aIndex )
:CAknListQueryDialog( (TInt*)aIndex ),
iNeedUnlock( EFalse )
    {
    }



// ---------------------------------------------------------
// CApSelQueryDialog::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelQueryDialog::HandleApDbEventL( TEvent anEvent )
    {
    }




// ---------------------------------------------------------
// CApSelQueryDialog::PostLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelQueryDialog::PreLayoutDynInitL()
    {
    }


// ---------------------------------------------------------
// CApSelQueryDialog::PostLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelQueryDialog::PostLayoutDynInitL()
    {
    }


// ---------------------------------------------------------
// CApSelQueryDialog::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelQueryDialog::OkToExitL( TInt aButtonId )
    {
    return EFalse;
    }




// From CCoeControl
// ---------------------------------------------------------
// CApSelQueryDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    User::Leave( KErrNotSupported );
    }



// From MEikCommandObserver
// ---------------------------------------------------------
// CApSelQueryDialog::ProcessCommandL
// ---------------------------------------------------------
//
void CApSelQueryDialog::ProcessCommandL( TInt aCommandId )
    {
    }



// ---------------------------------------------------------
// CApSelQueryDialog::FillListBoxWithDataL()
// called when listbopx needs to be filled with data
// ---------------------------------------------------------
//
void CApSelQueryDialog::FillListBoxWithDataL()
    {
    }




// ---------------------------------------------------------
// CApSelQueryDialog::LoadIconsL()
// called when listbox is constructed 
// ---------------------------------------------------------
//
TInt CApSelQueryDialog::LoadIconsL()
    {
    return 0;
    }

// End of File
