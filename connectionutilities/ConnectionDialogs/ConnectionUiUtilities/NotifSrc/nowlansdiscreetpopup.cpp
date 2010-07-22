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
 * Description:  Implementation of CNoWlansDiscreetPopup.
 *
 */

// INCLUDE FILES

#include <akndiscreetpopup.h>
#include <aknsconstants.hrh>
#include <StringLoader.h>
#include <avkon.hrh>
#include <connectionuiutilities.mbg>
#include <connuiutilsnotif.rsg>

#include "nowlansdiscreetpopup.h"


// No discreet popup => Uid 0
const TUid KDiscreetPopupUidNone =
    {
    0x0
    };

// Path of the app's iconfile
_LIT( KIconFileName, "\\resource\\apps\\connectionuiutilities.mif" );
// Empty string
_LIT( KEmpty, "");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::CNoWlansDiscreetPopup
// ---------------------------------------------------------
//
CNoWlansDiscreetPopup::CNoWlansDiscreetPopup(
    CConnectionDialogsNotifBase* aNotif ) :
    CActive( EPriorityUserInput ), // Standard priority
    iNotif( aNotif )
    {
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::NewLC
// ---------------------------------------------------------
//
CNoWlansDiscreetPopup* CNoWlansDiscreetPopup::NewLC(
    CConnectionDialogsNotifBase* aNotif )
    {
    CNoWlansDiscreetPopup* self =
            new (ELeave) CNoWlansDiscreetPopup( aNotif );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::NewL
// ---------------------------------------------------------
//
CNoWlansDiscreetPopup* CNoWlansDiscreetPopup::NewL(
    CConnectionDialogsNotifBase* aNotif )
    {
    CNoWlansDiscreetPopup* self =
            CNoWlansDiscreetPopup::NewLC( aNotif );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::ConstructL
// ---------------------------------------------------------
//
void CNoWlansDiscreetPopup::ConstructL()
    {
    CActiveScheduler::Add( this ); // Add to scheduler
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::~CNoWlansDiscreetPopup
// ---------------------------------------------------------
//
CNoWlansDiscreetPopup::~CNoWlansDiscreetPopup()
    {
    Cancel(); // Cancel any request, if outstanding
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::DoCancel
// ---------------------------------------------------------
//
void CNoWlansDiscreetPopup::DoCancel()
    {
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::StartL
// ---------------------------------------------------------
//
void CNoWlansDiscreetPopup::StartL()
    {
    Cancel(); // Cancel any request, just to be sure

    SetActive(); // Tell scheduler a request is active
    iClientStatus = &iStatus;
    User::RequestComplete( iClientStatus, KErrNone );

    // Complete the note so client can continue its tasks
    // immediately
    iNotif->SetCancelledFlag( ETrue );
    iNotif->CompleteL( KErrNone );
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::RunL
// ---------------------------------------------------------
//
void CNoWlansDiscreetPopup::RunL()
    {
    if ( iStatus == KErrNone )
        {
            
        // Load strings from resources
        HBufC* stringText1 = StringLoader::LoadLC( R_OCC_INFO_NO_WLAN_AVAILABLE );
        HBufC* stringText2 = KEmpty().AllocLC();
        	
        CAknDiscreetPopup::ShowGlobalPopupL(
        	    *stringText1,    // 1st text row
                *stringText2,    // second text row
                KAknsIIDDefault, // icon skin id
                KIconFileName,   // bitmap file path
                EMbmConnectionuiutilitiesQgn_prop_wlan_bearer,      // bitmap id
                EMbmConnectionuiutilitiesQgn_prop_wlan_bearer_mask, // mask id
                KAknDiscreetPopupDurationLong, // flags
                0,    // command id
                NULL, // command observer
                KDiscreetPopupUidNone,  // application to be launched
                KDiscreetPopupUidNone); // view to be activated

        CleanupStack::PopAndDestroy(2, stringText1);
        }

    // Cleanup
    iNotif->Cancel();
    }

// ---------------------------------------------------------
// CNoWlansDiscreetPopup::RunError
// ---------------------------------------------------------
//
TInt CNoWlansDiscreetPopup::RunError( TInt aError )
    {
    return aError;
    }

