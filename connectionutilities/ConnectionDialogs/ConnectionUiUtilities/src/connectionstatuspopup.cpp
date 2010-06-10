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
* Description: 
*      Implementation of class CConnectionStatusPopup.   
*      
*
*/

#include "connectionstatuspopup.h"

#include <cmmanager.h>
#include <cmmanagerext.h>
#include <cmconnectionmethod.h>
#include <cmconnectionmethodext.h>
#include <cmpluginwlandef.h>
#include <cmpluginvpndef.h>

#include <hbtextresolversymbian.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "connectionstatuspopupTraces.h"
#endif

// Error code definitions are from these headers
#include <wlanerrorcodes.h> // WLAN-specific error code definitions
#include <gsmerror.h>       // KErrPacketDataTsyMaxPdpContextsReached 
#include <in_iface.h>
#include <etelpckt.h>       // GPRS-specific causes for Session Management
#include <nd_err.h>         // NetDial errors
#include <inet6err.h>       // IPv6 error constants
#include <rmmcustomapi.h>

// Errors in UI spec not found elsewhere
const TInt KErrDndNameNotFound          = -5120;
const TInt KErrGeneralConnection        = -50000;
const TInt KErrGenConnNoGPRSNetwork     = -3609;

// Icons representing bearer
_LIT( KIconCellular, "qtg_small_gprs" );
_LIT( KIconWlan,     "qtg_small_wlan" );
_LIT( KIconVpn,      "qtg_small_vpn" ); 
_LIT( KIconNone,     "" );

// Empty text
_LIT( KTextNone,     "" );

// Programs to fire at tap
_LIT( KWlanViewExeFile, "wlansniffer.exe" );
_LIT( KConnViewExeFile, "connview.exe" );
// Command line
_LIT( KExeFileCmd, "" );

// Localization files and texts
_LIT(KTrFile, "connectionstatuspopups_");
_LIT(KTrFilePath, "z:\\resource\\qt\\translations");

_LIT(Ktxt_occ_dpophead_connecting,
        "txt_occ_dpophead_connecting");
_LIT(Ktxt_occ_dpophead_connection_failed,
        "txt_occ_dpophead_connection_failed");
_LIT(Ktxt_occ_dpopinfo_internal_error,
        "txt_occ_dpopinfo_internal_error");
_LIT(Ktxt_occ_dpopinfo_wlan_network_not_found,
        "txt_occ_dpopinfo_wlan_network_not_found");
_LIT(Ktxt_occ_dpopinfo_authentication_unsuccessful,
        "txt_occ_dpopinfo_authentication_unsuccessful");
_LIT(Ktxt_occ_dpopinfo_check_security_key,
        "txt_occ_dpopinfo_check_security_key");
_LIT(Ktxt_occ_dpopinfo_check_connection_settings,
        "txt_occ_dpopinfo_check_connection_settings");
_LIT(Ktxt_occ_dpopinfo_sim_card_missing,
        "txt_occ_dpopinfo_sim_card_missing");
_LIT(Ktxt_occ_dpopinfo_service_unreachable,
        "txt_occ_dpopinfo_service_unreachable");
_LIT(Ktxt_occ_dpopinfo_permission_denied,
        "txt_occ_dpopinfo_permission_denied");
_LIT(Ktxt_occ_dpopinfo_connection_already_active,
        "txt_occ_dpopinfo_connection_already_active");
_LIT(Ktxt_occ_dpopinfo_signal_too_weak,
        "txt_occ_dpopinfo_signal_too_weak");
_LIT(Ktxt_occ_dpopinfo_please_try_again,
        "txt_occ_dpopinfo_please_try_again");
_LIT(Ktxt_occ_dpopinfo_connection_unavailable,
        "txt_occ_dpopinfo_connection_unavailable");
_LIT(Ktxt_occ_dpophead_configuration_failed,
        "txt_occ_dpophead_configuration_failed");
_LIT(Ktxt_occ_dpophead_maximum_connections_in_use,
        "txt_occ_dpophead_maximum_connections_in_use");
_LIT(Ktxt_occ_dpopinfo_select_to_manage,
        "txt_occ_dpopinfo_select_to_manage");


//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::CConnectionStatusPopup()
//-----------------------------------------------------------------------------
//
CConnectionStatusPopup::CConnectionStatusPopup()
:iPopup( NULL ), iPopupState( EPopupClosed ), iTouchAction( EDoNothing )
    {
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::NewL()
//-----------------------------------------------------------------------------
//
CConnectionStatusPopup* CConnectionStatusPopup::NewL()
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_NEWL_ENTRY );
    
    CConnectionStatusPopup* popup = new ( ELeave ) CConnectionStatusPopup();
    CleanupStack::PushL( popup );
    popup->ConstructL();
    CleanupStack::Pop( popup ); 
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_NEWL_EXIT );
    return popup;
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::ConstructL()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::ConstructL()
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_CONSTRUCTL_ENTRY );
    
    iPopup = CHbDeviceNotificationDialogSymbian::NewL( this );

    TBool result = HbTextResolverSymbian::Init( KTrFile, KTrFilePath );
    if (!result)
        {
        OstTrace0( TRACE_FLOW, CCONNECTIONSTATUSPOPUP_CONSTRUCTL,"CConnectionStatusPopup::ConstructL; Loading of localization failed!" );
        }
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_CONSTRUCTL_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::~CConnectionStatusPopup()
//-----------------------------------------------------------------------------
//
CConnectionStatusPopup::~CConnectionStatusPopup()
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_CCONNECTIONSTATUSPOPUP_ENTRY );
    
    if (iPopup)
        {
        iPopup->Close();
        delete iPopup;
        }
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_CCONNECTIONSTATUSPOPUP_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::ConnectingDiscreetPopup()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::ConnectingViaDiscreetPopup( )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_CONNECTINGVIADISCREETPOPUP_ENTRY );

    TRAP_IGNORE( 
        HBufC* title = HbTextResolverSymbian::LoadLC(
                Ktxt_occ_dpophead_connecting );
        iPopup->SetTitleL( title->Des() );
        CleanupStack::PopAndDestroy( title );
        iPopup->SetTextL( KTextNone );
        iPopup->SetIconNameL( KIconNone );
        );
    iTouchAction = EDoNothing;
    iPopup->SetTimeout( KForeverTimeout );

    SetState( EPopupConnecting );
 
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_CONNECTINGVIADISCREETPOPUP_EXIT );
    }


//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::ConnectingViaDiscreetPopup()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::ConnectingViaDiscreetPopup(
        TUint32 aIapId,
        TBool aConnectionAlreadyActive )
    {
    OstTraceFunctionEntry0( DUP1_CCONNECTIONSTATUSPOPUP_CONNECTINGVIADISCREETPOPUP_ENTRY );
    OstTraceExt2( TRACE_FLOW, CCONNECTIONSTATUSPOPUP_CONNECTINGVIADISCREETPOPUP, "CConnectionStatusPopup::ConnectingViaDiscreetPopup;aIapId=%u;aConnectionAlreadyActive=%u", aIapId, aConnectionAlreadyActive );

    // Grab iap info even if we do not need it yet (when sharing),
    // because icon may be used later if general error happens.
    TRAP_IGNORE( FormatIapInfoL( aIapId ); );

    if (aConnectionAlreadyActive)
        {
        // If sharing a connection, do not show the popup.
        SetState( EPopupClosed );
        }
    else
        {
        iPopup->SetTimeout( KForeverTimeout );
        SetState( EPopupConnectingIap );
        }
    
    OstTraceFunctionExit0( DUP1_CCONNECTIONSTATUSPOPUP_CONNECTINGVIADISCREETPOPUP_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::CancelConnectingViaDiscreetPopup()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::CancelConnectingViaDiscreetPopup()
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_CANCELCONNECTINGVIADISCREETPOPUP_ENTRY );
    
    SetState( EPopupClosed );

    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_CANCELCONNECTINGVIADISCREETPOPUP_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::ConnectionErrorDiscreetPopup()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::ConnectionErrorDiscreetPopup( TInt aErrorCode )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_CONNECTIONERRORDISCREETPOPUP_ENTRY );
    
    TBool showPopup( EFalse );
    TRAP_IGNORE( showPopup = ResolveErrorCodeL( aErrorCode ); );
    
    OstTraceExt2( TRACE_FLOW, CCONNECTIONSTATUSPOPUP_CONNECTIONERRORDISCREETPOPUP, "CConnectionStatusPopup::ConnectionErrorDiscreetPopup;aErrorCode=%d;showPopup=%u", aErrorCode, showPopup );
    
    if (showPopup)
        {
        iPopup->SetTimeout( KHbLongNotificationDialogTimeout ); 
        SetState( EPopupError );
        }

    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_CONNECTIONERRORDISCREETPOPUP_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::SetState()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::SetState( TPopupState aNewState )
    {
    OstTraceExt2( TRACE_FLOW, CCONNECTIONSTATUSPOPUP_SETSTATE, "CConnectionStatusPopup::SetState;iPopupState=%u;aNewState=%u", iPopupState, aNewState );

    if (aNewState != EPopupClosed)
        {
        // Enable touch activation based on defined action
        iPopup->EnableTouchActivation( iTouchAction != EDoNothing );

        // Show or update popup
        if (iPopupState == EPopupClosed)
            {
            TRAP_IGNORE( iPopup->ShowL() );
            }
        else
            {
            TRAP_IGNORE( iPopup->UpdateL() );
            }
        }
    else
        {
        // Close popup if it's not already closed or closing 
        if (iPopupState != EPopupClosed && iPopupState != EPopupError)
            {
            iPopup->Close();
            }
        }
    
    iPopupState = aNewState;
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::FormatIapInfoL()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::FormatIapInfoL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_FORMATIAPINFOL_ENTRY );
    
    // Title
    HBufC* title = HbTextResolverSymbian::LoadLC(
            Ktxt_occ_dpophead_connecting );
    iPopup->SetTitleL( title->Des() );
    CleanupStack::PopAndDestroy( title );

    // Load iap info from cmmanager
    HBufC* connectionName( NULL );
    TUint bearerType( 0 );

    RCmManager cmMgr;
    CleanupClosePushL( cmMgr );
    RCmConnectionMethod plugin;
    CleanupClosePushL( plugin );
    cmMgr.OpenL();
    plugin = cmMgr.ConnectionMethodL( aIapId );
    bearerType = plugin.GetIntAttributeL( CMManager::ECmBearerType );
    connectionName = plugin.GetStringAttributeL( CMManager::ECmName );
    CleanupStack::PopAndDestroy( &plugin );
    CleanupStack::PopAndDestroy( &cmMgr );
    
    // Text
    CleanupStack::PushL( connectionName );
    if (connectionName)
        {
        iPopup->SetTextL( connectionName->Des() );
        }
    else
        {
        iPopup->SetTextL( KTextNone );
        }
    CleanupStack::PopAndDestroy( connectionName ); 

    // Icon and action
    switch (bearerType)
        {
        case KUidWlanBearerType:
            iTouchAction = EOpenWlanView;
            iPopup->SetIconNameL( KIconWlan );
            break;
        case KPluginVPNBearerTypeUid:
            iTouchAction = EDoNothing;
            iPopup->SetIconNameL( KIconVpn );
            break;
        default:
            iTouchAction = EOpenCellularView;
            iPopup->SetIconNameL( KIconCellular );
        }
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_FORMATIAPINFOL_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::ResolveErrorCodeL()
//-----------------------------------------------------------------------------
//
TBool CConnectionStatusPopup::ResolveErrorCodeL( TInt aErrorCode )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_RESOLVEERRORCODEL_ENTRY );
    
    // Icons, which are resolved at the end
    enum TIconType
        {
        // WLAN icon
        EIconWlan, 
        // Cellular icon
        EIconCellular, 
        // No icon to be used
        EIconNoneSet, 
        // Icon from connecting popup to be used
        EIconUsePreviouslyDefined
        };

    // Most errors have following values
    TIconType icon = EIconWlan;
    iTouchAction = EDoNothing;
    TPtrC titlePtr;
    titlePtr.Set( Ktxt_occ_dpophead_connection_failed );
    
    TPtrC textPtr;

    // Resolve title, text and icon of error code.
    // Groups are from OCC UI spec 1.0
    switch (aErrorCode)
        {

        // Skipped errors, for which the popup is not shown.
        case KErrNone:
        case KErrConnectionTerminated:
        case KErrDisconnected:
        case KErrCancel:
            return EFalse;

        // Group 1
        case KErrIfChangingPassword:
            icon = EIconCellular;
        // These were GPRS; fall through
        case KErrWlanInternalError:
            textPtr.Set( Ktxt_occ_dpopinfo_internal_error );
            break;

        // Group 2
        case KErrWlanNetworkNotFound:
            textPtr.Set( Ktxt_occ_dpopinfo_wlan_network_not_found );
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
            textPtr.Set( Ktxt_occ_dpopinfo_authentication_unsuccessful );
            break;

        // Group 4
        case KErrWlanSharedKeyAuthFailed:
        case KErrWlanIllegalWpaPskKey:
        case KErrWlanIllegalEncryptionKeys:
            textPtr.Set( Ktxt_occ_dpopinfo_check_security_key );
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
            textPtr.Set( Ktxt_occ_dpopinfo_check_connection_settings );
            break;

        // Group 6
        case KErrWlanSimNotInstalled:
            textPtr.Set( Ktxt_occ_dpopinfo_sim_card_missing );
            break;

        // Group 7
        case KErrWlanNotSubscribed:
        case KErrWlanAccessBarred:
            textPtr.Set( Ktxt_occ_dpopinfo_service_unreachable );
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
            textPtr.Set( Ktxt_occ_dpopinfo_permission_denied );
            break;

        // Group 9
        case KErrWlanConnAlreadyActive:
            textPtr.Set( Ktxt_occ_dpopinfo_connection_already_active );
            break;

        // Group 10
        case KErrWlanSignalTooWeak:
            textPtr.Set( Ktxt_occ_dpopinfo_signal_too_weak );
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
        case KErrGprsActivationRejectedByGGSN:
        case KErrPacketDataTsyInvalidAPN:
            icon = EIconCellular;
            // These were GPRS; fall through
        case KErrWlanRoamingFailed:
            textPtr.Set( Ktxt_occ_dpopinfo_please_try_again );
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
        case KErrGprsOfflineMode:
            icon = EIconCellular;
        case KErrWlanOff:
        case KErrWlanForceOff:
            textPtr.Set( Ktxt_occ_dpopinfo_connection_unavailable );
            break;

        // Group 13
        case KErrWlanProtectedSetupDevicePasswordAuthFailure:
        case KErrWlanProtectedSetupPINMethodNotSupported:
        case KErrWlanProtectedSetupPBMethodNotSupported:
            titlePtr.Set( Ktxt_occ_dpophead_configuration_failed );
            textPtr.Set( Ktxt_occ_dpopinfo_authentication_unsuccessful );
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
            titlePtr.Set( Ktxt_occ_dpophead_configuration_failed );
            textPtr.Set( Ktxt_occ_dpopinfo_please_try_again );
            break;
            
        // Group 15, errors related to maximum number of PDP contexts
        case KErrGprsInsufficientResources:
        case KErrUmtsMaxNumOfContextExceededByNetwork: 
        case KErrUmtsMaxNumOfContextExceededByPhone:
        case KErrPacketDataTsyMaxPdpContextsReached:
            // Open cellular view for these errors
            iTouchAction = EOpenCellularView;
            // The icon is left out from these popups in order to make room 
            // for the text which needs to be as descriptive as possible
            icon = EIconNoneSet;
            titlePtr.Set( Ktxt_occ_dpophead_maximum_connections_in_use );
            textPtr.Set( Ktxt_occ_dpopinfo_select_to_manage );
            break;
            
        // For error values not specifically mapped to any error message
        // the discreet pop-up #12 is used.
        default:
            icon = EIconUsePreviouslyDefined;
            textPtr.Set( Ktxt_occ_dpopinfo_connection_unavailable );
            break;
        }

    // Load and set title, text and icon
    HBufC* titleBuf = HbTextResolverSymbian::LoadLC( titlePtr );
    iPopup->SetTitleL( titleBuf->Des() );
    HBufC* textBuf = HbTextResolverSymbian::LoadLC( textPtr );
    iPopup->SetTextL( textBuf->Des() );
    CleanupStack::PopAndDestroy( 2, titleBuf );

    switch (icon)
        {
        case EIconWlan:
            iPopup->SetIconNameL( KIconWlan );
            break;
        case EIconCellular:
            iPopup->SetIconNameL( KIconCellular );
            break;
        case EIconNoneSet:
            iPopup->SetIconNameL( KIconNone );
            break;
        default:
            __ASSERT_DEBUG( icon == EIconUsePreviouslyDefined, User::Invariant());
            // Leave icon as-is, either set by ConnectingViaDiscreetPopup
            // or undefined.
            break;
        }
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_RESOLVEERRORCODEL_EXIT );
    return ETrue;
    }


//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::NotificationDialogActivated()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::NotificationDialogActivated(
        const CHbDeviceNotificationDialogSymbian* /*aDialog*/ )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGACTIVATED_ENTRY );
    
    // Launch wlan view or connection view depending on bearer
    TPtrC procName; 
    if ( iTouchAction == EOpenWlanView )
        {
        procName.Set( KWlanViewExeFile );
        }
    else if ( iTouchAction == EOpenCellularView )
        {
        procName.Set( KConnViewExeFile );
        }
    else
        {
        OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGACTIVATED_EXIT );

        return;
        }

    RProcess proc;
    TInt procError = proc.Create( procName, KExeFileCmd );
    OstTrace1( TRACE_FLOW, CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGACTIVATED, "CConnectionStatusPopup::NotificationDialogActivated;procError=%d", procError );
    if (!procError)
        {
        proc.Resume();
        proc.Close(); // Closes the handle, not the process.
        }

    OstTraceFunctionExit0( DUP1_CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGACTIVATED_EXIT );
    }

//-----------------------------------------------------------------------------
//  CConnectionStatusPopup::NotificationDialogClosed()
//-----------------------------------------------------------------------------
//
void CConnectionStatusPopup::NotificationDialogClosed(
        const CHbDeviceNotificationDialogSymbian* /*aDialog*/,
        TInt /*aCompletionCode*/ )
    {
    OstTraceFunctionEntry0( CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGCLOSED_ENTRY );
    
    SetState( EPopupClosed );
    
    OstTraceFunctionExit0( CCONNECTIONSTATUSPOPUP_NOTIFICATIONDIALOGCLOSED_EXIT );
    }
