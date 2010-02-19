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
 * Description:  Implementation of CConnectingViaDiscreetPopup.
 *
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <akndiscreetpopup.h>
#include <aknsconstants.hrh>
#include <StringLoader.h>
#include <utf.h>

#include <cmmanagerext.h>
#include <cmconnectionmethod.h>
#include <cmconnectionmethodext.h>
#include <cmpluginwlandef.h>

#include <connectionuiutilities.mbg>
#include <ConnUiUtilsNotif.rsg>

#include "connectingviadiscreetpopup.h"

// Connection Manager UIDs
const TUid KDiscreetPopupUid =
    {
    0x101F84D0
    };
const TUid KDiscreetPopupViewUid =
    {
    0x2
    };
// Path of the app's iconfile
_LIT( KIconFileName, "\\resource\\apps\\connectionuiutilities.mif" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::CConnectingViaDiscreetPopup
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopup::CConnectingViaDiscreetPopup(
        CConnectingViaDiscreetPopupNotif* aNotif) :
            CActive(EPriorityUserInput), // Standard priority
            iNotif(aNotif), iBearerType(0),
            iConnectionName(NULL), iPopupFlags(0)
    {
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::NewLC
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopup* CConnectingViaDiscreetPopup::NewLC(
        CConnectingViaDiscreetPopupNotif* aNotif)
    {
    CConnectingViaDiscreetPopup* self =
            new (ELeave) CConnectingViaDiscreetPopup(aNotif);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::NewL
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopup* CConnectingViaDiscreetPopup::NewL(
        CConnectingViaDiscreetPopupNotif* aNotif)
    {
    CConnectingViaDiscreetPopup* self = CConnectingViaDiscreetPopup::NewLC(
            aNotif);
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::ConstructL
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopup::ConstructL()
    {
    CActiveScheduler::Add(this); // Add to scheduler
    icmManager.OpenL();
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::~CConnectingViaDiscreetPopup
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopup::~CConnectingViaDiscreetPopup()
    {
    Cancel(); // Cancel any request, if outstanding
    icmManager.Close();
    // Delete instance variables if any
    if (iConnectionName)
        {
        delete iConnectionName;
        }
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::DoCancel
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopup::DoCancel()
    {
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::StartL
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopup::StartL( TUint32 aIapId )
    {
    Cancel(); // Cancel any request, just to be sure
    iBearerType = 0;
    delete iConnectionName;
    iConnectionName = NULL;
    iPopupFlags = KAknDiscreetPopupDurationLong;
    
    RCmConnectionMethod plugin;
    CleanupClosePushL(plugin);
    if (aIapId)
        {
        plugin = icmManager.ConnectionMethodL(aIapId);
        iBearerType = plugin.GetIntAttributeL(CMManager::ECmBearerType);
        TUint32 easyWLANIapId = icmManager.EasyWlanIdL();
        if (easyWLANIapId != aIapId)
            {
            iConnectionName = plugin.GetStringAttributeL(CMManager::ECmName);
            }
        else
            {
            iConnectionName = plugin.GetStringAttributeL(CMManager::EWlanUsedSSID);
            }
        }
    CleanupStack::PopAndDestroy(&plugin);

    SetActive(); // Tell scheduler a request is active
    iClientStatus = &iStatus;
    User::RequestComplete(iClientStatus, KErrNone);
    
    // Complete client request before launching dialog
    iNotif->SetCancelledFlag(ETrue);
    iNotif->CompleteL(KErrNone);
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::RunL
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopup::RunL()
    {
    if (iStatus == KErrNone)
        {
        TUint bitmapId = 0;
        TUint maskId = 0;
        if (iBearerType == KUidWlanBearerType)
            {
            bitmapId = EMbmConnectionuiutilitiesQgn_prop_wlan_bearer;
            maskId = EMbmConnectionuiutilitiesQgn_prop_wlan_bearer_mask;
            }
        else
            {
            bitmapId = EMbmConnectionuiutilitiesQgn_prop_wml_gprs;
            maskId = EMbmConnectionuiutilitiesQgn_prop_wml_gprs_mask;
            }

        // Load strings from resources
        HBufC* stringText1 = StringLoader::LoadLC(
                R_QTN_OCC_CONNECTING_DISCREET_POPUP_TEXT1);

        HBufC* stringText2 = NULL;
        if (iConnectionName)
            {
            stringText2 = StringLoader::LoadLC(
                    R_QTN_OCC_CONNECTING_DISCREET_POPUP_TEXT2,
                    *iConnectionName);
            }
        else
            {
            stringText2 = _L("''").AllocLC();
            }

        CAknDiscreetPopup::ShowGlobalPopupL(*stringText1, // first text row
                *stringText2, // second text row
                KAknsIIDDefault, // icon skin id
                KIconFileName, // bitmap file path
                bitmapId, // bitmap id
                maskId, // mask id
                iPopupFlags, // flags
                0, // command id
                NULL, // command observer
                KDiscreetPopupUid, // application to be launched
                KDiscreetPopupViewUid); // view to be activated
        CleanupStack::PopAndDestroy(2);
        }

    // Cleanup
    iNotif->Cancel();
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopup::RunError
// ---------------------------------------------------------
//
TInt CConnectingViaDiscreetPopup::RunError(TInt aError)
    {
    return aError;
    }
