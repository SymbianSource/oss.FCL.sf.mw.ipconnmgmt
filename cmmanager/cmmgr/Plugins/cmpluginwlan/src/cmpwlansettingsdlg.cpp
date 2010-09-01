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
* Description:  Dialog for editing wlan settings for a wlan connection method
*
*/

#include <badesca.h>
#include <cmcommonui.h>
#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>
#include <wapisecuritysettingsui.h>
#include <StringLoader.h>
#include <akntextsettingpage.h>
#include <cmwlanui.rsg>
#include <ConnectionUiUtilities.h>
#include <csxhelp/cp.hlp.hrh>
#include <featmgr.h>
#include "cmlogger.h"
#include "cmwlancommonconstants.h"
#include "cmpwlansettingsdlg.h"
#include "cmpwlansettingsdlgadv.h"
#include <cmpluginwlandef.h>
#include "cmpluginwlan.h"
#include "cmpluginmenucommands.hrh"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

const TUint32 KScanSSIDSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_HIDDEN_NETWORK_NO,
    R_QTN_WLAN_SETT_HIDDEN_NETWORK_YES,
    0
    };

const TUint32 KSecModeRBSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_SECURITY_MODE_OPEN,
    R_QTN_WLAN_SETT_SECURITY_MODE_WEP,
    0
    };

const TUint32 KSecModeRBIRSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_SECURITY_MODE_OPEN,
    R_QTN_WLAN_SETT_SECURITY_MODE_WEP,
    R_QTN_WLAN_SETT_SECURITY_MODE_802_1X,
    R_QTN_WLAN_SETT_SECURITY_MODE_WPA,
    0
    };

const TUint32 KSecModeRBIRSelectionItemsWAPI[] = 
    {
    R_QTN_WLAN_SETT_SECURITY_MODE_OPEN,
    R_QTN_WLAN_SETT_SECURITY_MODE_WEP,
    R_QTN_WLAN_SETT_SECURITY_MODE_802_1X,
    R_QTN_WLAN_SETT_SECURITY_MODE_WPA,
    R_QTN_WLAN_SETT_SECURITY_MODE_WAPI,
    0
    };

const TUint32 KNetworkRBSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_NETWORK_MODE_INFRA,
    R_QTN_WLAN_SETT_NETWORK_MODE_ADHOC,
    0
    };

const TUint32 KNWNameSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_NETWORK_NAME_USER_DEFINED,
    R_QTN_WLAN_SETT_NETWORK_NAME_SCAN,
    0
    };
    
// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::NewL()
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlg* CmPluginWlanSettingsDlg::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginWlanSettingsDlg* self = 
	                new (ELeave) CmPluginWlanSettingsDlg( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::CmPluginWlanSettingsDlg()
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlg::CmPluginWlanSettingsDlg(
                                            CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlg( aCmPluginBase )
    , iNotifyFromAdv( KCmNotifiedNone )
    {
    
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ConstructAndRunLD
// --------------------------------------------------------------------------
//
TInt CmPluginWlanSettingsDlg::ConstructAndRunLD( )
    {
    // Set this flag to allow edit continue
    iCanEditingContinue = ETrue;
    
    CleanupStack::PushL( this );
    LoadResourceL( KPluginWlanResDirAndFileName );
    CleanupStack::Pop( this );
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlg::ConstructAndRunLD( );
    }

// ---------------------------------------------------------
// CmPluginWlanSettingsDlg::~CmPluginWlanSettingsDlg
// Destructor
// ---------------------------------------------------------
//
CmPluginWlanSettingsDlg::~CmPluginWlanSettingsDlg()
    {
    iResourceReader.Close();
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::RunAdvancedSettingsL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::RunAdvancedSettingsL()
    {
    CmPluginWlanSettingsDlgAdv* AdvDlg = 
                        CmPluginWlanSettingsDlgAdv::NewL( iCmPluginBaseEng );

    if (!iHasSettingChanged)
        {
        iHasSettingChanged = ETrue;
        }                      
    
    // This view may have to un-register as watcher here
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherUnRegister();
    
    AdvDlg->RegisterParentView( this );
    TInt ret = AdvDlg->ConstructAndRunLD();

    if( iNotifyFromAdv == KCmNotifiedNone )
        {
        if( ret == KDialogUserExit )
            {
            iExitReason = KDialogUserExit;
            ExitSettingsL(EFalse);
            }
        else
            {
            // Re-register as watcher if advanced view comes back or exists normally
            cmMgr.WatcherRegisterL( this );
            }
        }
    else
        {
        HandleCommsDatChangeL();
        }
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    TBool boolSettingVal( EFalse ); // just for helping make code readable
    TInt valResId(0);
    
    // DATA BEARER - only for real bearer types
    AppendSettingTextsL( aItemArray, ECmBearerSettingName );
    // NETWORK NAME
    AppendSettingTextsL( aItemArray, EWlanSSID );
    // NETWORK STATUS
    boolSettingVal = iCmPluginBaseEng.GetBoolAttributeL( EWlanScanSSID );
    AppendSettingTextsL( aItemArray,
                         EWlanScanSSID,
                         boolSettingVal ? 
                            R_QTN_WLAN_SETT_HIDDEN_NETWORK_YES :
                            R_QTN_WLAN_SETT_HIDDEN_NETWORK_NO );
    // WLAN NETWORK MODE    
    switch( iCmPluginBaseEng.GetIntAttributeL( EWlanConnectionMode ) )
        {
        case EAdhoc:
            {
            valResId = R_QTN_WLAN_SETT_NETWORK_MODE_ADHOC;
            break;
            }
        case EInfra:
            // fall-through intended here
        default:
            {
            valResId = R_QTN_WLAN_SETT_NETWORK_MODE_INFRA;
            break;
            }
        }
    
    AppendSettingTextsL( aItemArray, EWlanConnectionMode, valResId );
    // WLAN SECURITY MODE
    switch( iCmPluginBaseEng.GetIntAttributeL( EWlanSecurityMode ) )
        {
        case EWlanSecModeWep :
            {
            valResId = R_QTN_WLAN_SETT_SECURITY_MODE_WEP;
            break;
            }
        case EWlanSecMode802_1x:
            {
            valResId = R_QTN_WLAN_SETT_SECURITY_MODE_802_1X;
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            valResId = R_QTN_WLAN_SETT_SECURITY_MODE_WPA;
            break;
            }
        case EWlanSecModeWAPI:
            {
            valResId = R_QTN_WLAN_SETT_SECURITY_MODE_WAPI;
            break;
            }
        case EWlanSecModeOpen:
            // fall-through intended here
        default:
            {
            valResId = R_QTN_WLAN_SETT_SECURITY_MODE_OPEN;
            break;
            }
        }
    AppendSettingTextsL( aItemArray, EWlanSecurityMode, valResId );
    // WLAN SECURITY SETTINGS
    AppendSettingTextsL( aItemArray,
                         EWlanSecuritySettings,
                         R_QTN_WLAN_SETT_SECURITY_SETTINGS,
                         0 );
    // HOMEPAGE
    AppendSettingTextsL( aItemArray, ECmStartPage );
    }

    
//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ChangeBoolSettingL
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::ChangeBoolSettingL( TUint32 aAttribute, 
                                                  TInt aCommandId )
    {
    // Change was selected from menu
    if ( aCommandId == EPluginBaseCmdChange )
        {
        TInt selected = 
                    iCmPluginBaseEng.GetBoolAttributeL( aAttribute );
        TInt origVal = selected;
                    
        if ( ShowRadioButtonSettingPageL( KScanSSIDSelectionItems, 
                                          selected,
                                          R_QTN_WLAN_SETT_HIDDEN_NETWORK ) &&
             origVal != selected )
             {
             iCmPluginBaseEng.SetBoolAttributeL( aAttribute, 
                                                 selected );
             UpdateListBoxContentL();
             }
        }
    // Selection key was selected
    else        
        {
        switch ( aAttribute )
            {
            case EWlanScanSSID:
                {        
                TBool boolFromCM = EFalse;
                boolFromCM = iCmPluginBaseEng.GetBoolAttributeL( aAttribute );
                iCmPluginBaseEng.SetBoolAttributeL( aAttribute , !boolFromCM );
                UpdateListBoxContentL();              
                break;
                }
            case EWlanConnectionMode:
                {        
                TInt intFromCM = 0;
                intFromCM = iCmPluginBaseEng.GetIntAttributeL( aAttribute );
                iCmPluginBaseEng.SetIntAttributeL( aAttribute , !intFromCM );
                UpdateListBoxContentL();              
                break;
                }
            }
        }
    }

//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ShowSecurityModeRBPageL
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::ShowSecurityModeRBPageL()
    {   
    TInt selected = 0;
    TInt secMode = iCmPluginBaseEng.GetIntAttributeL( EWlanSecurityMode );
    const TUint32* items = NULL;
    
    //converting secMode to idex
    switch ( secMode ) 
        {
        case EWlanSecModeOpen:
            {
            selected = 0;
            break;
            }
        case EWlanSecModeWep:
            {
            selected = 1;
            break;
            }
        case EWlanSecMode802_1x:
            {
            selected = 2;
            break;
            }
        case EWlanSecModeWpa:
            {
            selected = 3;
            break;
            }
        // is this a real case?
        case EWlanSecModeWpa2:
            {
            selected = 3;
            break;
            }
        case EWlanSecModeWAPI:
            {
            selected = 4;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
        
    if ( iCmPluginBaseEng.GetIntAttributeL( EWlanConnectionMode ) == EInfra )
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdFfWlanWapi ) )
            {
            items = KSecModeRBIRSelectionItems;
            }
        else
            {
            items = KSecModeRBIRSelectionItemsWAPI;
            }
        }
    else
        {
        items = KSecModeRBSelectionItems;
        }

    TInt origVal = selected;
    if ( ShowRadioButtonSettingPageL( items, 
                                      selected, 
                                      R_QTN_WLAN_SETT_SECURITY_MODE ) && 
                                      origVal != selected )
        {
        if ( !iHasSettingChanged )
            {
            iHasSettingChanged = ETrue;
            }
		
        switch ( selected )
            {
            case 0:
                {
                secMode = EWlanSecModeOpen;
                break;
                }
            case 1:
                {
                secMode = EWlanSecModeWep;
                break;
                }
            case 2:
                {
                secMode = EWlanSecMode802_1x;
                break;
                }
            case 3:
                {
                secMode = EWlanSecModeWpa;
                break;
                }
            case 4:
                {
                secMode = EWlanSecModeWAPI;
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                break;
                }
            }
        iCmPluginBaseEng.SetIntAttributeL( EWlanSecurityMode , secMode );
        UpdateListBoxContentL();
        }
    }

//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ShowNetworkModeRBPageL
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg:: ShowNetworkModeRBPageL()
    {   
    const TUint32* items = KNetworkRBSelectionItems;
    
    TInt selected;
    
    // convert values to indices for UI
    TInt netMode = iCmPluginBaseEng.GetIntAttributeL( EWlanConnectionMode );
    switch ( netMode )
        {
        case EAdhoc:
            {
            selected = 1;
            break;
            }
        case EInfra:
            {
            selected = 0;
            break;
            }
        }
    
    TInt origVal = selected;
    if ( ShowRadioButtonSettingPageL( items, 
                                      selected,
                                      R_QTN_WLAN_SETT_NETWORK_MODE ) && 
         origVal != selected )
        {
        // convert indices back to proper values
        switch ( selected )
            {
            case 0:
                {
                netMode = EInfra;
                break;
                }
            case 1:
                {
                netMode = EAdhoc;
                TInt secMode = iCmPluginBaseEng.GetIntAttributeL( 
                                                        EWlanSecurityMode );
                if( secMode != EWlanSecModeOpen &&
                    secMode != EWlanSecModeWep )
                    {
                    iCmPluginBaseEng.SetIntAttributeL( EWlanSecurityMode,
                                                       EWlanSecModeOpen );
                    }
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                break;
                }
            }
        iCmPluginBaseEng.SetIntAttributeL( EWlanConnectionMode , netMode );
        UpdateListBoxContentL();
        }    
    }  

//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ShowWlanNWNamePageL
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::ShowWlanNWNamePageL()
    {
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( EWlanSSID );
    
    const TUint32* items = KNWNameSelectionItems;
    TInt selected ( EWlanNwNameUserDefined );
    
    // Display a radio button page with options: User defined and Scan for Networks
    if ( ShowRadioButtonSettingPageL( items, 
                                      selected,
                                      R_QTN_WLAN_SETT_NETWORK_NAME ) )
        {
        if ( selected == EWlanNwNameUserDefined )
            {
            HBufC* oldssId = iCmPluginBaseEng.GetStringAttributeL( EWlanSSID );
            CleanupStack::PushL( oldssId );
            
            TBuf<KCmWlanNWNameMaxLength> buf;
            buf.Copy( *oldssId );

            if ( TCmCommonUi::ShowConfirmationQueryWithInputL(
                                           R_QTN_WLAN_PRMPT_NETWORK_NAME,
                                           buf ) )
                {
                if( buf.Compare( *oldssId ) )
                    // SSID is changed
                    {
                    iCmPluginBaseEng.SetStringAttributeL( EWlanSSID, buf );
                    UpdateListBoxContentL();
                    }
                }
            CleanupStack::PopAndDestroy( oldssId );
            }
        else
            {
            // Let's search for a WLAN network
            TWlanSsid ssid;
            TWlanConnectionMode connectionMode;
            TWlanConnectionSecurityMode securityMode;

            CConnectionUiUtilities* uiUtils = CConnectionUiUtilities::NewL();
            CleanupStack::PushL( uiUtils );

            if ( uiUtils->SearchWLANNetwork( ssid, connectionMode, securityMode ) )
                {
                HBufC* temp = HBufC::NewLC( convItem->iMaxLength );
                temp->Des().Copy( ssid );
                iCmPluginBaseEng.SetStringAttributeL( EWlanSSID, *temp );
                CleanupStack::PopAndDestroy( temp );
                
                UpdateListBoxContentL();
                }                
            CleanupStack::PopAndDestroy( uiUtils );
            }
        }
    }

//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ShowPopupSettingPageL
//---------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlg::ShowPopupSettingPageL( TUint32 aAttribute, 
                                                      TInt aCommandId )
    {    
    TBool retval = EFalse;
    switch( aAttribute )
        {
        // Boolean settings
        case EWlanScanSSID:            
            {
            ChangeBoolSettingL( aAttribute, aCommandId );
            break;
            }
        // Textual Settings
        case ECmStartPage:                
            {
            ShowPopupTextSettingPageL( aAttribute );
            break;
            }
        case EWlanSecurityMode:            
            {
            ShowSecurityModeRBPageL();
            break;
            }
        case EWlanSecuritySettings:
            {
            TInt secmode = iCmPluginBaseEng.GetIntAttributeL( EWlanSecurityMode );
            if(  secmode == EWlanSecModeOpen )
                {
                TCmCommonUi::ShowNoteL( R_QTN_WLAN_INFO_NO_SECURITY_SETTINGS, 
                                        TCmCommonUi::ECmInfoNote);
                }
            else
                {
                TInt exitValue = 
                    STATIC_CAST( CCmPluginWlan*, &iCmPluginBaseEng )->
                           EditSecuritySettingsL( *( CEikonEnv::Static() ) );
                if ( secmode == EWlanSecModeWep )
                    { // WEP
                    if ( exitValue & CWEPSecuritySettings::EExitReq )
                        {
                        iSecDialExit = ETrue;
                        iExitReason = KDialogUserExit;
                        ExitSettingsL(EFalse);
                        }                    
                    }
                else if ( secmode == EWlanSecModeWAPI )
                    {
                    if ( exitValue & CWAPISecuritySettings::EExitReq )
                        {
                        iSecDialExit = ETrue;
                        iExitReason = KDialogUserExit;
                        ExitSettingsL(EFalse);
                        }
                    }
                else
                    {// 802.1x, WPA/WPA2
                    if ( exitValue & CWPASecuritySettings::EExitReq )
                        {
                        iSecDialExit = ETrue;
                        iExitReason = KDialogUserExit;
                        ExitSettingsL(EFalse);
                        }
                    }
                }
            break;
            }
        case EWlanConnectionMode:
            {
            if ( aCommandId == EPluginBaseCmdChange )
                {
                ShowNetworkModeRBPageL();
                }
            else
                {
                TInt netMode = 
                    iCmPluginBaseEng.GetIntAttributeL( EWlanConnectionMode );
                // OK has been used: switch the value without opening the setting page
                if ( netMode == EInfra )
                    {
                    netMode = EAdhoc;
                    TInt secMode = iCmPluginBaseEng.GetIntAttributeL( 
                                                            EWlanSecurityMode );
                    if( secMode != EWlanSecModeOpen &&
                        secMode != EWlanSecModeWep )
                        {
                        iCmPluginBaseEng.SetIntAttributeL( EWlanSecurityMode,
                                                           EWlanSecModeOpen );
                        }
                    }
                else
                    {
                    netMode = EInfra;
                    }
                iCmPluginBaseEng.SetIntAttributeL( EWlanConnectionMode, netMode );
                UpdateListBoxContentL();
                }
            break;
            }
        case EWlanSSID:
            {
            ShowWlanNWNamePageL();
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlg::ShowPopupSettingPageL( aAttribute, 
                                                            aCommandId );
            break;            
            }
        }
    return retval;
    }
    
        
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ExitSettingsL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::ExitSettingsL( TBool aConfirm )
    {
     if ( STATIC_CAST( CCmPluginWlan*, &iCmPluginBaseEng )->
                                            AreSecuritySettingsValidL()  )
        {
        if ( iIsPossibleToSaveSetting && iHasSettingChanged )
            {
            iCmPluginBaseEng.UpdateL();
            iHasSettingChanged = EFalse;
            }
        TryExitL( iExitReason );
        }
     else
        {
        HBufC* netwName = iCmPluginBaseEng.GetStringAttributeL( EWlanSSID );
        TBool noNetwName = ETrue;
        CleanupStack::PushL( netwName );
        if ( *netwName != KNullDesC )
            {
            noNetwName = EFalse;
            }
        CleanupStack::PopAndDestroy( netwName );
        
        if ( noNetwName )
            {
            if ( aConfirm )
                {                    
                if ( TCmCommonUi::ShowConfirmationQueryL( 
                                    R_QTN_WLAN_QUEST_NW_NAME_NOT_DEFINED ) )
                    {
                    iExitReason = KDialogUserDelete;
                    TryExitL( iExitReason );
                    }
                else
                    {
                    // Find the index of the EWlanSSID item
                    for ( TInt i = 0; i < iSettingIndex->Count(); i++ )
                        {
                        if ( iSettingIndex->At(i) == EWlanSSID )
                            {
                            SelectItem(i);
                            break;
                            }
                        }
                    }
                }
            else
                {
                TryExitL( iExitReason );
                }
            }
        else
            {
            if ( iCmPluginBaseEng.GetIntAttributeL( EWlanSecurityMode ) == 
                                                           EWlanSecModeOpen )
                {
                if ( iIsPossibleToSaveSetting && iHasSettingChanged)
                {
                    iCmPluginBaseEng.UpdateL();
                    iHasSettingChanged = EFalse;
                }
                TryExitL( iExitReason );
                }
            else
                {
                if ( aConfirm )
                    {                    
                    if ( TCmCommonUi::ShowConfirmationQueryL( 
                            R_QTN_WLAN_QUEST_SECURITY_SETTINGS_EMPTY ) )
                        {
                        iExitReason = KDialogUserDeleteConfirmed;
                        TryExitL( iExitReason );
                        }
                     else                    
                        {
                        // Show the securitysettings page for WEP, other security mode
                        // is not supported here(WPA has a default value)...
                        ShowPopupSettingPageL( EWlanSecuritySettings, EWlanSecModeWep );
                        }
                    }
                else
                    {
                    TryExitL( iExitReason );
                    }
                }
            }
        }
    }


// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlg::OkToExitL( TInt aButtonId )
    {
    // Database has been changed by other application so exit from this view
    // without update editings to database
    if ( !iCanEditingContinue )
        {
        if ( iExitReason == KDialogUserExit )
            {
            iCmPluginBaseEng.CmMgr().WatcherUnRegister();
            
            // Set iExitReason back to KDialogUserBack so as to exit from this view through else in the next call
            TInt exitValue = KDialogUserExit;
            iExitReason = KDialogUserBack;
            // If destination has been deleted by other application
            // then we may have to exit from Cmmgr
            TryExitL( exitValue );
            return EFalse;
            }
        else
            {
            // Exit from this view here to avoid possible update to databse
            return ETrue;
            }
        }
    
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        case EAknSoftkeyChange:
            {
            ProcessCommandL( aButtonId );
            break;
            }        
        case EAknSoftkeyBack:
            {
            ProcessCommandL( aButtonId );
            break;
            }
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }
        case KDialogUserExit:
            {
            if ( iSecDialExit )
                {
                iSecDialExit = EFalse;
                }
            retval=ETrue;
            break;
            }
        default:
            {
            retval = ETrue;
            break;
            }
        }

    return retval;
    }


// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::ProcessCommandL( TInt aCommandId )
    {
    if ( !iCanEditingContinue )
        {
        // We have to block all editing activity if database changed by
        // other application
        return;
        }

    if ( MenuShowing() )
        {
        HideMenu();
        }
    
    switch ( aCommandId )
        {
        case EPluginBaseCmdExit:
            {
            iExitReason = KDialogUserExit;
            }            
        case EAknSoftkeyBack:
            {
            CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
            cmMgr.WatcherUnRegister();
            
            ExitSettingsL(ETrue);
            break;
            }           
        case EPluginBaseCmdAdvanced:
            {
            RunAdvancedSettingsL();
            break;
            }
        case EPluginBaseCmdChange:
        case EAknSoftkeyOk:
            {
            TUint32 attrib = iSettingIndex->At( iListbox->CurrentItemIndex() );
            ShowPopupSettingPageL( attrib, aCommandId );
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlg::ProcessCommandL( aCommandId );
            break;            
            }
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::GetHelpContext
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOGGER_ENTERFN( "CmPluginWlanSettingsDlg::GetHelpContext" );	
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_AP_SETTING_WLAN;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::CommsDatChangesL()
    {
    if ( !iCanEditingContinue )
        {
        return;
        }
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    CCmDestinationImpl* parentDest = iCmPluginBaseEng.ParentDestination();

    if ( parentDest )
        {
        if( !cmMgr.DestinationStillExistedL( parentDest ) )
            {
            // If parent destination is deleted by somebody then the dialog must exit back to main view
            iExitReason = KDialogUserExit;
            iCanEditingContinue = EFalse;

            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;
            }
        
        // We may have to go back to parent view if database is changed by other application
        iCanEditingContinue = EFalse;

        cmMgr.RemoveDestFromPool( parentDest );
        delete parentDest;
        }
    else // Legacy
        {
        // We may have to go back to parent view if database is changed by other application
        iCanEditingContinue = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::NotifyParentView
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::NotifyParentView( TInt aValue )
    {
    iNotifyFromAdv = aValue;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlg::HandleCommsDatChangeL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlg::HandleCommsDatChangeL()
    {
    if ( iCanEditingContinue )
        {
        // Set iCanEditingContinue to False so that exit fromn this view without update editings
        iCanEditingContinue = EFalse;
        }
    
    if( iNotifyFromAdv == KCmNotifiedIapIsNotInThisDestination || 
            iNotifyFromAdv == KCmNotifiedIapDisappear )
        {
        TryExitL( iExitReason );
        }
    else if( iNotifyFromAdv == KCmNotifiedDestinationDisappear )
        {
        iExitReason = KDialogUserExit;
        TryExitL( iExitReason );
        }
    }
