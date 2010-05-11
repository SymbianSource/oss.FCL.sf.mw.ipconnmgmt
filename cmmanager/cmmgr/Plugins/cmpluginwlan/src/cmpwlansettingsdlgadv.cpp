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
* Description:  Dialog for setting WLAN plugin advanced settings
*
*/

#include <StringLoader.h>
#include <akntextsettingpage.h>
#include <aknmfnesettingpage.h> // for IP dialog
#include <cmcommonui.h>
#include <cmwlanui.rsg>
#include <AknQueryDialog.h>
#include <cmpluginbaseeng.h>
#include <cmmanager.rsg>
#include <csxhelp/cp.hlp.hrh>

#include "cmpluginwlan.h"
#include <cmpluginwlandef.h>
#include "cmpwlansettingsdlgadv.h"
#include "cmpwlansettingsdlgipv4.h"
#include "cmpwlansettingsdlgipv6.h"
#include "cmwlancommonconstants.h"
#include <cmpbasesettingsdlg.h>
#include "cmpluginmenucommands.hrh"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

const TUint32 KAdhocChannelSelectionItems[] = 
    {
    R_QTN_WLAN_SETT_ADHOC_CHANNEL_AUTOMATIC,
    R_QTN_WLAN_SETT_ADHOC_CHANNEL_USER_DEFINED,
    0
    };

// ================= MEMBER FUNCTIONS =========================================

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgAdv* CmPluginWlanSettingsDlgAdv::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginWlanSettingsDlgAdv* self = 
	                new (ELeave) CmPluginWlanSettingsDlgAdv( aCmPluginBase );
	return self;
	}
	
	
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ConstructAndRunLD
// Constructs the dialog and runs it.
// --------------------------------------------------------------------------
//
TInt CmPluginWlanSettingsDlgAdv::ConstructAndRunLD( )
    {
    // Set this flag to allow edit continue
    iCanEditingContinue = ETrue;
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlgAdv::ConstructAndRunLD();
    }
    	
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::CmPluginWlanSettingsDlgAdv()
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgAdv::CmPluginWlanSettingsDlgAdv( 
                                        CCmPluginBaseEng& aCmPluginBase ) :
    CmPluginBaseSettingsDlgAdv( aCmPluginBase ),
    iIpv6Supported( aCmPluginBase.FeatureSupported( KFeatureIdIPv6 ) ),
    iParent( NULL )
    {
    iHelpContext = KSET_HLP_AP_SETTING_WLAN_AS;
    }
    
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::~CmPluginWlanSettingsDlgAdv
// Destructor
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgAdv::~CmPluginWlanSettingsDlgAdv()
    {
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    if ( iIpv6Supported )
        {
        // IPV4 SETTINGS
        AppendSettingTextsL( aItemArray,
                             ESettingPageCmdIpv4,
                             R_QTN_SET_IPV4_SETTINGS,
                             0 );
        // IPV6 SETTINGS
        AppendSettingTextsL( aItemArray,
                             ESettingPageCmdIpv6,
                             R_QTN_SET_IPV6_SETTINGS,
                             0 );
        }
    else
        {
        // PHONE IP ADDRESS
        AppendSettingTextsL( aItemArray, EWlanIpAddr );
        HBufC* ipAddr = iCmPluginBaseEng.GetStringAttributeL( EWlanIpAddr );
        TBool isUnspec = IsUnspecifiedIPv4Address( *ipAddr );
        delete ipAddr; ipAddr = NULL;

        if ( !isUnspec )
            {
            // SUBNET MASK
            AppendSettingTextsL( aItemArray, EWlanIpNetMask );
            // DEFAULT GATEWAY
            AppendSettingTextsL( aItemArray, EWlanIpGateway );
            }
                
        // PRIMARY NAME SERVER
        AppendSettingTextsL( aItemArray, EWlanIpNameServer1 );
        // SECONDARY NAME SERVER
        AppendSettingTextsL( aItemArray, EWlanIpNameServer2 );
        }
    
    // AD-HOC CHANNEL
    if ( iCmPluginBaseEng.GetIntAttributeL( EWlanConnectionMode ) == EAdhoc )
        {
        AppendSettingTextsL( aItemArray, EWlanChannelID );
        }
    
    // PROXY SERVER ADDRESS
    AppendSettingTextsL( aItemArray, ECmProxyServerName );            
    // PROXY PORT NUMBER
    AppendSettingTextsL( aItemArray, ECmProxyPortNumber );
/*    
    TInt valResId(0);
    // Allow SSID Roaming?

    TBool res = iCmPluginBaseEng.GetBoolAttributeL( EWlanAllowSSIDRoaming );
    if ( res )
        {
        valResId = R_QTN_WLAN_SETT_SSID_ROAMING_ENABLED;
        }
    else
        {
        valResId = R_QTN_WLAN_SETT_SSID_ROAMING_DISABLED;
        }    
        
    AppendSettingTextsL( aItemArray, EWlanAllowSSIDRoaming, valResId  );
*/    
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ShowPopupSettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgAdv::ShowPopupSettingPageL( TUint32 aAttribute,
                                                         TInt aCommandId )
    {    
    TBool retval = EFalse;
    switch ( aAttribute )
        {
        case EWlanIpAddr:
        case EWlanIpNetMask:
        case EWlanIpGateway:
        case EWlanIpNameServer1:
        case EWlanIpNameServer2:        
            {
            ShowPopupIpSettingPageL( aAttribute );
            break;
            }        
        case ESettingPageCmdIpv4:                
            {
            ShowIpv4SettingPageL();
            break;
            }
        case ESettingPageCmdIpv6:                
            {
            ShowIpv6SettingPageL();
            break;
            }
        case ECmProxyServerName:                
            {
            retval = ShowPopupTextSettingPageL( aAttribute );
            break;
            }
        case ECmProxyPortNumber:                
            {
            retval = ShowPopupPortNumSettingPageL( aAttribute );
            break;
            }
        case EWlanChannelID: // may not be correct
            {
            retval = ShowAdhocChannelSettingPageL();
            break;
            }
        case EWlanAllowSSIDRoaming:
            {
//            retval = ShowAdhocChannelSettingPageL();
            break;            
            }
        default:
            {
            CmPluginBaseSettingsDlgAdv::ShowPopupSettingPageL( aAttribute, aCommandId );
            break;            
            }
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ShowIpv4SettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgAdv::ShowIpv4SettingPageL()
    {
    TInt retVal( EFalse );
    CmPluginWlanSettingsDlgIpv4* ipv4Dlg = 
                    CmPluginWlanSettingsDlgIpv4::NewL( iCmPluginBaseEng );

    // This view may have to un-register as watcher here
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherUnRegister();
    
    ipv4Dlg->RegisterParentView( this );
    TInt ret = ipv4Dlg->ConstructAndRunLD();
    
    if( iNotifyFromIp == KCmNotifiedNone )
        {
        if ( ret == KDialogUserExit )
            {
            retVal = ETrue;
            TryExitL( KDialogUserExit );
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
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ShowIpv6SettingPageL
// --------------------------------------------------------------------------
//    
TBool CmPluginWlanSettingsDlgAdv::ShowIpv6SettingPageL()
    {
    TInt retVal( EFalse );
    CmPluginWlanSettingsDlgIpv6* ipv6Dlg = 
                    CmPluginWlanSettingsDlgIpv6::NewL( iCmPluginBaseEng );

    // This view may have to un-register as watcher here
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherUnRegister();
 
    ipv6Dlg->RegisterParentView( this );
    TInt ret = ipv6Dlg->ConstructAndRunLD();
 
    if( iNotifyFromIp == KCmNotifiedNone )
        {
        if ( ret == KDialogUserExit )
            {
            retVal = ETrue;
            TryExitL( KDialogUserExit );
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
    
    return retVal;
    }


// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ShowAdhocChannelSettingPageL
// --------------------------------------------------------------------------
//    
TBool CmPluginWlanSettingsDlgAdv::ShowAdhocChannelSettingPageL()
    {
    TInt retVal( EFalse );
    const TCmAttribConvTable* convItem = 
                        iCmPluginBaseEng.ConvTableItem( EWlanChannelID );
    const TUint32* items = KAdhocChannelSelectionItems;
    TInt value = iCmPluginBaseEng.GetIntAttributeL( EWlanChannelID );
    
    TInt selected = value ? EAdhocUserDefined : EAdhocAutomatic;
        
    // Show the dialog  
    if ( ShowRadioButtonSettingPageL( items, 
                                      selected,
                                      R_QTN_WLAN_SETT_ADHOC_CHANNEL ) )
        {
        // 'User defined' selected
        if ( selected == EAdhocUserDefined )
            {
            // if EWlanChannelID doesn't have a valid number, use the defaul
            if ( value < KCmAdhocChannelMinValue || value > KCmAdhocChannelMaxValue )
                {
                value = KCmAdhocChannelDefaultValue;
                }
            
            TBool loop ( ETrue );
            
            do
                {
                HBufC* buf = HBufC::NewLC( convItem->iMaxLength );
                TPtr ptr ( buf->Des() );
                ptr.AppendNum( value );
                
                // Display a numeric query dialog
                CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL( ptr );
                
                dlg->PrepareLC( R_NUMERIC_QUERY );
                dlg->SetMaxLength( convItem->iMaxLength );
                dlg->SetDefaultInputMode( EAknEditorNumericInputMode );
                dlg->MakeLeftSoftkeyVisible( EFalse );
                
                HBufC* prompt = StringLoader::LoadLC( 
                                                R_QTN_WLAN_PRMPT_ADHOC_CHANNEL );
                dlg->SetPromptL( *prompt );
                CleanupStack::PopAndDestroy( prompt );
                
                // Show the popup
                if ( dlg->RunLD() )
                    {
                    TLex lex ( ptr );
                    lex.Val( value );
                    
                    // Check valid limits
                    if ( value >= KCmAdhocChannelMinValue && 
                         value <= KCmAdhocChannelMaxValue )
                        {
                        // set result                
                        iCmPluginBaseEng.SetIntAttributeL( EWlanChannelID, value );
                        loop = EFalse;
                        UpdateListBoxContentL();
                        retVal = ETrue;
                        }
                    else
                        {
                        value = value <  KCmAdhocChannelMinValue ? 
                                         KCmAdhocChannelMinValue : 
                                         KCmAdhocChannelMaxValue;
                        }
                    }
                else
                    {
                    loop = EFalse;
                    }
                CleanupStack::PopAndDestroy( buf );
                } while ( loop );
            }
        else
            {
            iCmPluginBaseEng.SetIntAttributeL( EWlanChannelID, EAdhocAutomatic );
            UpdateListBoxContentL();
            retVal = ETrue;
            }        
        }
        
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv:: ProcessCommandL( TInt aCommandId )
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
            
            HBufC* ipAddr = iCmPluginBaseEng.GetStringAttributeL( EWlanIpAddr );
            TBool isUnspec = IsUnspecifiedIPv4Address( *ipAddr );
            delete ipAddr; ipAddr = NULL;
            
            if ( !isUnspec )
                {
                ipAddr = iCmPluginBaseEng.GetStringAttributeL( EWlanIpNetMask );
                TBool isUnspec1 = IsUnspecifiedIPv4Address( *ipAddr );
                delete ipAddr;
                ipAddr = iCmPluginBaseEng.GetStringAttributeL( EWlanIpGateway );
                TBool isUnspec2 = IsUnspecifiedIPv4Address( *ipAddr );
                delete ipAddr;
                
                if ( isUnspec1 || isUnspec2 )
                    {
                    if ( TCmCommonUi::ShowConfirmationQueryL(
                            R_QTN_NETW_QUEST_IAP_INCOMPLETE_DELETE ) )
                        {
                        iCmPluginBaseEng.RestoreAttributeL( EWlanIpAddr );
                        iCmPluginBaseEng.RestoreAttributeL( EWlanIpNetMask );
                        iCmPluginBaseEng.RestoreAttributeL( EWlanIpGateway );
                        iCmPluginBaseEng.RestoreAttributeL( EWlanIpDNSAddrFromServer );
                        TryExitL( iExitReason );
                        }
                    }
                 else
                    {
                    TryExitL( iExitReason );
                    }
                }
            else
                {
                TryExitL( iExitReason );
                }
            break;
            }  
        case EAknSoftkeyOk:                    
        case EPluginBaseCmdChange:
            {
            ShowPopupSettingPageL( 
                        iSettingIndex->At( iListbox->CurrentItemIndex() ),
                        aCommandId );
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlgAdv::ProcessCommandL( aCommandId );
            break;            
            }
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgAdv::OkToExitL( TInt aButtonId )
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
        default:
            {
            retval = ETrue;
            break;
            }
        }

    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::RegisterParentView
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv::RegisterParentView( CCmParentViewNotifier* aParent )
    {
    iParent = aParent;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv::CommsDatChangesL()
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
            if( iParent )
                {
                iParent->NotifyParentView( KCmNotifiedDestinationDisappear );
                }
            
            // If parent destination is deleted by somebody then the dialog must exit back to main view
            iExitReason = KDialogUserExit;
            iCanEditingContinue = EFalse;

            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;
            }
        
        // We may have to go back to parent view if database is changed by other application
        if( iParent )
            {
            iParent->NotifyParentView( KCmNotifiedIapDisappear );
            }
        iCanEditingContinue = EFalse;

        cmMgr.RemoveDestFromPool( parentDest );
        delete parentDest;
        }
    else // Legacy
        {
        // We may have to go back to parent view if database is changed by other application
        if( iParent )
            {
            iParent->NotifyParentView( KCmNotifiedIapDisappear );
            }
        iCanEditingContinue = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::NotifyParentView
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv::NotifyParentView( TInt aValue )
    {
    iNotifyFromIp = aValue;
    
    if( iParent )
        {
        iParent->NotifyParentView( aValue );
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgAdv::HandleCommsDatChangeL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgAdv::HandleCommsDatChangeL()
    {
    if ( iCanEditingContinue )
        {
        // Set iCanEditingContinue to False so that exit fromn this view without update editings
        iCanEditingContinue = EFalse;
        }
    
    if( iNotifyFromIp == KCmNotifiedIapIsNotInThisDestination || 
            iNotifyFromIp == KCmNotifiedIapDisappear )
        {
        TryExitL( iExitReason );
        }
    else if( iNotifyFromIp == KCmNotifiedDestinationDisappear )
        {
        iExitReason = KDialogUserExit;
        TryExitL( iExitReason );
        }
    }
