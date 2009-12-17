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
 * Description:  Implementation of CConnectionErrorDiscreetPopup.
 *
 */

// INCLUDE FILES
#include "connectionerrordiscreetpopup.h"
#include <akndiscreetpopup.h>
#include <aknsconstants.hrh>
#include <StringLoader.h>
#include <cmmanagerext.h>
#include <cmconnectionmethod.h>
#include <cmconnectionmethodext.h>
#include <cmpluginwlandef.h>
#include <avkon.hrh>
#include <connectionuiutilities.mbg>
#include <ConnUiUtilsNotif.rsg>

#ifdef _DEBUG
#include <e32debug.h>
#endif

// Error code definitions are from these headers
#include <wlanerrorcodes.h> // WLAN-specific error code definitions
#include <in_iface.h>
#include <etelpckt.h>       // GPRS-specific causes for Session Management
#include <nd_err.h>         // NetDial errors
#include <inet6err.h>       // IPv6 error constants
#include <rmmcustomapi.h>

// Errors in UI spec not found elsewhere
const TInt KErrPDPMaxContextsReached = -6000;
const TInt KErrDndNameNotFound = -5120;
const TInt KErrGeneralConnection = -50000;
const TInt KErrGenConnNoGPRSNetwork = -3609;

// No discreet popup => Uid 0
const TUid KDiscreetPopupUidNone =
    {
    0x0
    };
// Path of the app's iconfile
_LIT( KIconFileName, "\\resource\\apps\\connectionuiutilities.mif" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::CConnectionErrorDiscreetPopup
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopup::CConnectionErrorDiscreetPopup(
        CConnectionErrorDiscreetPopupNotif* aNotif) :
    CActive(EPriorityUserInput), // Standard priority
            iNotif(aNotif), iErrorCode(KErrNone)
    {
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::NewLC
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopup* CConnectionErrorDiscreetPopup::NewLC(
        CConnectionErrorDiscreetPopupNotif* aNotif)
    {
    CConnectionErrorDiscreetPopup* self =
            new (ELeave) CConnectionErrorDiscreetPopup(aNotif);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::NewL
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopup* CConnectionErrorDiscreetPopup::NewL(
        CConnectionErrorDiscreetPopupNotif* aNotif)
    {
    CConnectionErrorDiscreetPopup* self =
            CConnectionErrorDiscreetPopup::NewLC(aNotif);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::ConstructL
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopup::ConstructL()
    {
    CActiveScheduler::Add(this); // Add to scheduler
    icmManager.OpenL();
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::~CConnectionErrorDiscreetPopup
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopup::~CConnectionErrorDiscreetPopup()
    {
    Cancel(); // Cancel any request, if outstanding
    icmManager.Close();
    // Delete instance variables if any
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::DoCancel
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopup::DoCancel()
    {
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::StartL
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopup::StartL(TUint32 aErrCode)
    {
    Cancel(); // Cancel any request, just to be sure

    iErrorCode = aErrCode;

    SetActive(); // Tell scheduler a request is active
    iClientStatus = &iStatus;
    User::RequestComplete(iClientStatus, KErrNone);

    // Complete the note so client can continue its tasks
    // immediately
    iNotif->SetCancelledFlag(ETrue);
    iNotif->CompleteL(KErrNone);
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::RunL
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopup::RunL()
    {
    if (iStatus == KErrNone)
        {
        TInt text1 = 0;
        TInt text2 = 0;
        TInt bitmapId = 0;
        TInt maskId = 0;
        // Show only if error code can be resolved
        if (ResolveErrorCode(text1, text2, bitmapId, maskId))
            {
            // Load strings from resources
            HBufC* stringText1 = StringLoader::LoadLC(text1);
            HBufC* stringText2 = StringLoader::LoadLC(text2);
            CAknDiscreetPopup::ShowGlobalPopupL(*stringText1, // 1st text row
                    *stringText2, // second text row
                    KAknsIIDDefault, // icon skin id
                    KIconFileName, // bitmap file path
                    bitmapId, // bitmap id
                    maskId, // mask id
                    KAknDiscreetPopupDurationLong, // flags
                    0, // command id
                    NULL, // command observer
                    KDiscreetPopupUidNone, // application to be launched
                    KDiscreetPopupUidNone); // view to be activated

            CleanupStack::PopAndDestroy(2, stringText1);
            }
        }

    // Cleanup
    iNotif->Cancel();
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::RunError
// ---------------------------------------------------------
//
TInt CConnectionErrorDiscreetPopup::RunError(TInt aError)
    {
    return aError;
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopup::ResolveErrorCode
// ---------------------------------------------------------
//
TInt CConnectionErrorDiscreetPopup::ResolveErrorCode(TInt& aText1,
        TInt& aText2, TInt& aBitmap, TInt& aMask)
    {
    
#ifdef _DEBUG
    RDebug::Print( _L("CConnectionErrorDiscreetPopup::ResolveErrorCode - code %d"),
            iErrorCode );
#endif
    
    // Icons, which are resolved at the end
    enum TIconType
        {
        EIconWlan, EIconCellular, EIconNone
        };

    // For most cases, icon is WLAN
    TIconType icon = EIconWlan;

    // For most cases, aText1 = "Connection failed"
    aText1 = R_QTN_OCC_CONNECTION_FAILED_DISCREET_POPUP_TEXT1;

    // Resolve aText1, aText2 and icon of error code.
    // Groups are from OCC GUI spec 1.0
    switch (iErrorCode)
        {
        
        // Group 1
        case KErrIfChangingPassword:
            icon = EIconCellular;
            // These were GPRS; fall through
        case KErrWlanInternalError:
            aText2 = R_QTN_OCC_INTERNAL_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 2
        case KErrWlanNetworkNotFound:
            aText2 = R_QTN_OCC_WLAN_NOT_FOUND_DISCREET_POPUP_TEXT2;
            break;

        // Group 3
        case KErrGprsUserAuthenticationFailure:
            icon = EIconCellular;
        // These were GPRS; fall through
        case KErrWlanOpenAuthFailed:
        case KErrWlanSharedKeyAuthRequired:
        case KErrWlanWpaAuthRequired:
        case KErrWlanWpaAuthFailed:
        case KErrWlan802dot1xAuthFailed:
        case KErrWlanWpaCounterMeasuresActive:
        case KErrWlanPskModeRequired:
        case KErrWlanEapModeRequired:
        case KErrWlanEapSimFailed:
        case KErrWlanEapTlsFailed:
        case KErrWlanEapPeapFailed:
        case KErrWlanEapMsChapv2: // Is the same as KErrWlanEapMsChaPFailed
        case KErrWlanEapAkaFailed:
        case KErrWlanEapTtlsFailed:
        case KErrWlanLeapFailed:
        case KErrWlanEapGtcFailed:
        case KErrWlanWpa2OnlyModeNotSupported:
        case KErrWlanEapFastTunnelCompromiseError:
        case KErrWlanEapFastUnexpextedTlvExhanged:
        case KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled:
        case KErrWlanEapFastNoMatchingPacForAid:
        case KErrWlanEapFastAuthFailed:
        case KErrWlanEapFastPacStoreCorrupted:
            aText2 = R_QTN_OCC_AUTH_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 4
        case KErrWlanSharedKeyAuthFailed:
        case KErrWlanIllegalWpaPskKey:
        case KErrWlanIllegalEncryptionKeys:
            aText2 = R_QTN_OCC_CHECK_KEY_DISCREET_POPUP_TEXT2;
            break;

        // Group 5
        case KErrExitModemError:
        case KErrExitLoginFail:
        case KErrExitScriptTimeOut:
        case KErrExitScriptError:
        case KErrIfAuthenticationFailure:
        case KErrIfAuthNotSecure:
        case KErrIfAccountDisabled:
        case KErrIfRestrictedLogonHours:
        case KErrIfPasswdExpired:
        case KErrIfNoDialInPermission:
        case KErrGprsMissingorUnknownAPN:
            icon = EIconCellular;
            aText2 = R_QTN_OCC_CHECK_SETTINGS_DISCREET_POPUP_TEXT2;
            break;

        // Group 6
        case KErrWlanSimNotInstalled:
            aText2 = R_QTN_OCC_SIM_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 7
        case KErrWlanNotSubscribed:
        case KErrWlanAccessBarred:
            aText2 = R_QTN_OCC_SERVICE_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 8
        case KErrPermissionDenied:
        case KErrWlanPasswordExpired:
        case KErrWlanNoDialinPermissions:
        case KErrWlanAccountDisabled:
        case KErrWlanRestrictedLogonHours:
        case KErrWlanServerCertificateExpired:
        case KErrWlanCerficateVerifyFailed:
        case KErrWlanNoUserCertificate:
        case KErrWlanNoCipherSuite:
        case KErrWlanUserRejected:
        case KErrWlanUserCertificateExpired:
            aText2 = R_QTN_OCC_ACCESS_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 9
        case KErrPDPMaxContextsReached:
            icon = EIconCellular;
            // These were GPRS; fall through
        case KErrWlanConnAlreadyActive:
            aText2 = R_QTN_OCC_ALREADY_ACT_DISCREET_POPUP_TEXT2;
            break;

        // Group 10
        case KErrWlanSignalTooWeak:
            aText2 = R_QTN_OCC_WLAN_WEAK_DISCREET_POPUP_TEXT2;
            break;

        // Group 11
        case KErrIfCallbackNotAcceptable:
        case KErrIfDNSNotFound:
        case KErrIfLRDBadLine:
        case KErrNetUnreach:
        case KErrHostUnreach:
        case KErrNoProtocolOpt:
        case KErrUrgentData:
        case KErrInet6NoRoute:
        case KErrDndNameNotFound:
        case KErrGeneralConnection:
        case KErrGprsMSCTemporarilyNotReachable:
        case KErrGprsLlcOrSndcpFailure:
        case KErrGprsInsufficientResources:
        case KErrGprsActivationRejectedByGGSN:
        case KErrPacketDataTsyInvalidAPN:
            icon = EIconCellular;
            // These were GPRS; fall through
        case KErrWlanRoamingFailed:
            aText2 = R_QTN_OCC_TRY_AGAIN_DISCREET_POPUP_TEXT2;
            break;

        // Group 12
        case KErrGenConnNoGPRSNetwork:
        case KErrGprsServicesNotAllowed:
        case KErrGprsAndNonGprsServicesNotAllowed:
        case KErrGprsMSIdentityCannotBeDerivedByTheNetwork:
        case KErrGprsMSImplicitlyDetached:
        case KErrGsmMMNetworkFailure:
        case KErrGprsUnknownPDPAddress:
        case KErrGprsActivationRejected:
        case KErrGsmMMServiceOptionNotSubscribed:
        case KErrGsmMMServiceOptionTemporaryOutOfOrder:
        case KErrGprsNSAPIAlreadyUsed:
        case KErrGprsQoSNotAccepted:
        case KErrGprsReactivationRequested:
            icon = EIconCellular;
            aText2 = R_QTN_OCC_CONN_UNAVAILABLE_DISCREET_POPUP_TEXT2;
            break;

        // Group 13
        case KErrWlanProtectedSetupDevicePasswordAuthFailure:
        case KErrWlanProtectedSetupPINMethodNotSupported:
        case KErrWlanProtectedSetupPBMethodNotSupported:
            aText1 = R_QTN_OCC_CONFIG_FAILED_DISCREET_POPUP_TEXT1;
            aText2 = R_QTN_OCC_AUTH_ERROR_DISCREET_POPUP_TEXT2;
            break;

        // Group 14
        case KErrWlanProtectedSetupOOBInterfaceReadError:
        case KErrWlanProtectedSetupDecryptionCRCFailure:
        case KErrWlanProtectedSetup2_4ChannelNotSupported:
        case KErrWlanProtectedSetup5_0ChannelNotSupported:
        case KErrWlanProtectedSetupNetworkAuthFailure:
        case KErrWlanProtectedSetupNoDHCPResponse:
        case KErrWlanProtectedSetupFailedDHCPConfig:
        case KErrWlanProtectedSetupIPAddressConflict:
        case KErrWlanProtectedSetupCouldNotConnectToRegistrar:
        case KErrWlanProtectedSetupMultiplePBCSessionsDetected:
        case KErrWlanProtectedSetupRogueActivitySuspected:
        case KErrWlanProtectedSetupDeviceBusy:
        case KErrWlanProtectedSetupSetupLocked:
        case KErrWlanProtectedSetupMessageTimeout:
        case KErrWlanProtectedSetupRegistrationSessionTimeout:
            aText1 = R_QTN_OCC_CONFIG_FAILED_DISCREET_POPUP_TEXT1;
            aText2 = R_QTN_OCC_TRY_AGAIN_DISCREET_POPUP_TEXT2;
            break;

        // Skipped errors, for which popup is not shown.
        case KErrNone:
        case KErrConnectionTerminated:
        case KErrDisconnected:
        case KErrCancel:
            return EFalse;

        // For error values not specifically mapped to any error message
        // the discreet pop-up #12 is used.
        default:
            aText2 = R_QTN_OCC_CONN_UNAVAILABLE_DISCREET_POPUP_TEXT2;
            icon = EIconNone;
            break;
        }

    // Resolve icon to aBitmap & aMask
    switch (icon)
        {
        case EIconWlan:
            aBitmap = EMbmConnectionuiutilitiesQgn_prop_wlan_bearer;
            aMask = EMbmConnectionuiutilitiesQgn_prop_wlan_bearer_mask;
            break;
        case EIconCellular:
            aBitmap = EMbmConnectionuiutilitiesQgn_prop_wml_gprs;
            aMask = EMbmConnectionuiutilitiesQgn_prop_wml_gprs_mask;
            break;
        default:
            ASSERT(icon == EIconNone);
            aBitmap = -1;
            aMask = -1;
            break;
        }

    return ETrue;
    }
