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
* Description:  Dialog for editing VPN settings for a VPN
*                connection method
*
*/

// INCLUDE FILES
#include <cmpluginbaseeng.h>
#include <cmpluginvpn.mbg>
#include <cmpluginvpnui.rsg>
#include <AknDialog.h>
#include <StringLoader.h>
#include <cmmanager.rsg>
#include <akntextsettingpage.h>
#include <akntitle.h>
#include <cmcommonui.h>
#include <csxhelp/cp.hlp.hrh>

#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include "cmpluginvpn.h"
#include "cmpvpnsettingsdlg.h"
#include "cmvpncommonconstants.h"
#include <cmpluginvpndef.h>
#include "cmpvpnpolicyselectiondlg.h"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmconnsettingsuiimpl.h"
#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"

using namespace CMManager;


// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginVpnSettingsDlg* CmPluginVpnSettingsDlg::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginVpnSettingsDlg* self = 
	                new ( ELeave ) CmPluginVpnSettingsDlg( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::CmPluginVpnSettingsDlg()
// --------------------------------------------------------------------------
//
CmPluginVpnSettingsDlg::CmPluginVpnSettingsDlg( 
                                            CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlg( aCmPluginBase )
    {
    
    }
    
//---------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::ConstructAndRunLD
// --------------------------------------------------------------------------
//
TInt CmPluginVpnSettingsDlg::ConstructAndRunLD( )
    {
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::ConstructAndRunLD" );
    CleanupStack::PushL( this );
    LoadResourceL( KPluginVPNResDirAndFileName );
    
    // check if VPN is available, as if not, we can not show the details...
    RVpnServ ps;
    CleanupClosePushL<RVpnServ>( ps );
    TInt err = ps.Connect();
    if ( err )
        {
        // show note
        HBufC* text = StringLoader::LoadLC( 
                                    R_QTN_VPN_INFO_MGMT_UI_NOT_ACCESSIBLE );
        TCmCommonUi::ShowNoteL( *text, TCmCommonUi::ECmInfoNote );
        CleanupStack::PopAndDestroy( text );        
        User::Leave( KLeaveWithoutAlert );
        }
    CleanupStack::PopAndDestroy(); // ps.close
    
    // check for compulsory fields missing & display note about it
    // information note VPN access point is incomplete. Try reconfiguring 
    // the access point. §qtn.vpn.info.vpn.iap.incomplete§ is displayed and 
    // Must be defined qtn_selec_setting_compulsory is displayed 
    // for the invalid settings.               
    if ( !CompulsoryFilledL() )
        {
        // show note
        HBufC* text = StringLoader::LoadLC( 
                                    R_QTN_VPN_INFO_VPN_IAP_INCOMPLETE );
        TCmCommonUi::ShowNoteL( *text, TCmCommonUi::ECmInfoNote );
        CleanupStack::PopAndDestroy( text );
        }
    
    CleanupStack::Pop( this );
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlg::ConstructAndRunLD( );
    }

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::~CmPluginVpnSettingsDlg
// Destructor
// --------------------------------------------------------------------------
//
CmPluginVpnSettingsDlg::~CmPluginVpnSettingsDlg()
    {
    iResourceReader.Close();
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginVpnSettingsDlg::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::UpdateListBoxContentBearerSpecificL" );
    HBufC* value = NULL;
        
    // VPN policy - Compulsory field
    AppendSettingTextsL( aItemArray, EVpnServicePolicyName );
    
    TUint32 iapId = iCmPluginBaseEng.GetIntAttributeL( EVpnIapId );
    TUint32 destId = iCmPluginBaseEng.GetIntAttributeL( ECmNextLayerSNAPId );
    
    CCmDestinationImpl* dest = NULL;
    
    // The underlying CM is a Destination
    if ( destId )
        {
        dest = iCmPluginBaseEng.CmMgr().DestinationL( destId );
        CleanupStack::PushL( dest );
//        CleanupReleasePushL( dest );
        value = dest->NameLC();
        }
    // The underlying CM is a CM
    else if ( iapId )
        {
        value = iCmPluginBaseEng.CmMgr().GetConnectionMethodInfoStringL( 
                                                           iapId, ECmName );
        CleanupStack::PushL( value );
        }
    else
        {
        // Must be defined
        value = StringLoader::LoadLC( R_QTN_VPN_SETT_VPN_IAP_REAL_NETW_REF_COMPULSORY );
        }
    
    AppendSettingTextsL( aItemArray,
                         ECmNextLayerIapId,
                         R_QTN_VPN_SETT_VPN_IAP_REAL_NETW_REFERENCE,
                         *value,
                         ETrue );
    CleanupStack::PopAndDestroy( value );

    if ( destId )
        {
        CleanupStack::PopAndDestroy( dest );
        }
                         
    // Proxy server address
    AppendSettingTextsL( aItemArray, ECmProxyServerName );
    // Proxy port number
    AppendSettingTextsL( aItemArray, ECmProxyPortNumber );
    
    }


// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::RunAdvancedSettingsL
// --------------------------------------------------------------------------
//
void CmPluginVpnSettingsDlg::RunAdvancedSettingsL()
    {
    /* empty functionality */
    }

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CmPluginVpnSettingsDlg::DynInitMenuPaneL( TInt aResourceId,
                                               CEikMenuPane* aMenuPane )
   {
   if ( aResourceId == R_PLUGIN_BASE_MENU )
        {
        aMenuPane->SetItemDimmed( EPluginBaseCmdAdvanced, ETrue  );           
        }
    }
        
// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::ShowPopupSettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginVpnSettingsDlg::ShowPopupSettingPageL( TUint32 aAttribute, 
                                                     TInt aCommandId )
    {
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::ShowPopupSettingPageL" );
    TBool retVal = EFalse;
    switch ( aAttribute )
        {
        case EVpnServicePolicyName:        
            {
            CmPluginVpnPolicySelectionDlg* dlg = 
                    CmPluginVpnPolicySelectionDlg::NewL( iCmPluginBaseEng );
            CleanupStack::PushL( dlg );
            retVal = dlg->ShowPolicySelectionDlgL();
            UpdateListBoxContentL();
            CleanupStack::PopAndDestroy();
            break;
            }
        case ECmProxyServerName:
            {
            retVal = ShowPopupTextSettingPageL( aAttribute );
            break;
            }
        case EVpnIapId:
        case ECmNextLayerIapId:
            {
            ShowIapSelectionSettingPageL();
            break;
            }
        case ECmProxyPortNumber:
            {
            HBufC* proxyName = iCmPluginBaseEng.GetStringAttributeL( ECmProxyServerName );
            if( !proxyName )
                {
                break;
                }
            CleanupStack::PushL( proxyName );
            if( proxyName->Length() )
                { // Port number is allowed to set after proxy address is set.
                retVal = ShowPopupPortNumSettingPageL( aAttribute );
                }
            CleanupStack::PopAndDestroy(); // proxyName
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlg::ShowPopupSettingPageL( aAttribute, 
                                                            aCommandId );
            break;
            }
        }
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::ShowIapSelectionSettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginVpnSettingsDlg::ShowIapSelectionSettingPageL()
    {  
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::ShowIapSelectionSettingPageL" ); 
    TBool retVal( EFalse );
    
    CCmConnSettingsUiImpl* settings = 
                                CCmConnSettingsUiImpl::NewL();
    CleanupStack::PushL( settings );
    settings->EnableEasyWlan( ETrue );
    settings->EnableEmptyDestinations( EFalse );
    settings->EnableVirtualCMs( EFalse );
    
    CCmDestinationImpl* d = iCmPluginBaseEng.ParentDestination();
    if ( d )
        {
        settings->SetDestinationToOmit( d->Id() );
//        settings->EnableUncat( ETrue );
        }
    else
        {
//        settings->EnableUncat( EFalse );
        }
    delete d;
    settings->EnableUncat( ETrue );
    
    CCmPluginVpn* plugin = static_cast<CCmPluginVpn *>
                    ( &iCmPluginBaseEng );
    
    RArray<TUint32> cmArray ( KCmArrayMediumGranularity );
    CleanupClosePushL( cmArray );
    
    plugin->BindableDestinationsL( cmArray );
    
    settings->SetDestinationArrayToUseL( cmArray );
    
    TCmSettingSelection selection;
    
    // Get the underlying CM
    selection.iId = iCmPluginBaseEng.GetIntAttributeL( ECmNextLayerIapId );
    
    if ( selection.iId )
        {
        selection.iResult = EConnectionMethod;
        }
    // The underlying CM is a destination
    else
        {        
        selection.iResult = EDestination;
        selection.iId = iCmPluginBaseEng.GetIntAttributeL( ECmNextLayerSNAPId );
        }
    
    TBearerFilterArray aFilterArray;
    if ( settings->RunApplicationSettingsL( selection,
                                            EShowDestinations |
                                            EShowConnectionMethods , 
                                            aFilterArray ) )
        {
        switch ( selection.iResult )
            {
            case EDestination:
                {
                iCmPluginBaseEng.SetIntAttributeL( ECmNextLayerSNAPId, selection.iId );
                break;
                }
            case EConnectionMethod:
                {
                
                iCmPluginBaseEng.SetIntAttributeL( ECmNextLayerIapId, selection.iId );
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }
        UpdateListBoxContentL();
        
        // update the title pane with the new name
        HBufC* newCmName = iCmPluginBaseEng.GetStringAttributeL( ECmName );
        CleanupStack::PushL( newCmName );
        iTitlePane->SetTextL( *newCmName );
        CleanupStack::PopAndDestroy( newCmName );
        
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( &cmArray );
    CleanupStack::PopAndDestroy( settings );
    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginVpnSettingsDlg::ProcessCommandL( TInt aCommandId )
    {
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EAknSoftkeyOk:
        case EPluginBaseCmdChange:
            {
            ShowPopupSettingPageL( 
                    iSettingIndex->At( iListbox->CurrentItemIndex() ), 
                                        aCommandId );
            break;            
            }
        case EVpnIapId:
        case ECmNextLayerIapId:
        case ECmProxyServerName:
        case ECmProxyPortNumber:
            {
            TInt attrib = iSettingIndex->At( iListbox->CurrentItemIndex() );
            ShowPopupSettingPageL( attrib, aCommandId );  
            break;
            }
        case EVpnServicePolicyName:
            {
            CmPluginVpnPolicySelectionDlg* dlg = 
                    CmPluginVpnPolicySelectionDlg::NewL( iCmPluginBaseEng );
            CleanupStack::PushL( dlg );
            if ( dlg->ShowPolicySelectionDlgL() )
                {
                // update the setting
                UpdateListBoxContentL();
                }
            // we might need to update even if not accepted
            
            CleanupStack::PopAndDestroy();
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
// CmPluginVpnSettingsDlg::CompulsoryFilledL
// --------------------------------------------------------------------------
//
TBool CmPluginVpnSettingsDlg::CompulsoryFilledL()
    {
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::CompulsoryFilledL" );
    
    TBool retval = ETrue;
    // compulsory settings are ECmName, EVpnIapId, EVpnServicePolicy
    
    // check name first
    HBufC* name = iCmPluginBaseEng.GetStringAttributeL( ECmName );
    if ( ( name->Compare( KNullDesC() ) ) == 0 )
        {
        retval = EFalse;
        }        
    delete name;
    
    // check VPN IAP ID
    TUint32 iapid = iCmPluginBaseEng.GetIntAttributeL( EVpnIapId );
    if ( !iapid )
        {
        TUint32 nextiapid = iCmPluginBaseEng.GetIntAttributeL( ECmNextLayerIapId  );
        TUint32 snapid = iCmPluginBaseEng.GetIntAttributeL( ECmNextLayerSNAPId );
        if ( !(nextiapid ^ snapid) )
            {
            retval = EFalse;
            }
        }
        
    // check policy
    HBufC* polId = iCmPluginBaseEng.GetStringAttributeL( EVpnServicePolicy );
    if ( ( polId->Compare( KNullDesC() ) ) == 0 )
        {
        retval = EFalse;
        }
    delete polId;

    // check policy name
    HBufC* polname = iCmPluginBaseEng.GetStringAttributeL( EVpnServicePolicyName );
    if ( ( polname->Compare( KNullDesC() ) ) == 0 )
        {
        retval = EFalse;
        }
    delete polname;
    
    return retval;
    }


// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::OkToExitL
// Good to know : EAknSoftkeyCancel is never called, because
// EEikDialogFlagNotifyEsc flag is not set in the resource.
// --------------------------------------------------------------------------
//
TBool CmPluginVpnSettingsDlg::OkToExitL( TInt aButtonId )
    {
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyBack:
            {
            if ( CompulsoryFilledL() )
                {
                if( iIsPossibleToSaveSetting && iHasSettingChanged )
                {
                    iCmPluginBaseEng.UpdateL();
                    iHasSettingChanged = EFalse;
                }
                retval = ETrue;

                iCmPluginBaseEng.CmMgr().WatcherUnRegister();
                }
            else
                {
                // If there are compulsory fields empty, a confirmation query
                // Incomplete VPN access point details. Exit without saving?
                // §qtn.vpn.quest.vpn.iap.incomplete.delete§ is displayed.
                // show note that compulsory settings are not filled
                TInt res = TCmCommonUi::ShowConfirmationQueryL( 
                                R_QTN_VPN_QUEST_VPN_IAP_INCOMPLETE_DELETE );
                if ( res )
                    { // quit without save
                    retval = ETrue;
                    TUint32 id = iCmPluginBaseEng.GetIntAttributeL( ECmId );
                    iCmPluginBaseEng.RestoreAttributeL( ECmName );
                    iCmPluginBaseEng.RestoreAttributeL( EVpnServicePolicyName );
                    // EVpnServicePolicy has been restored when EVpnServicePolicyName
                    // is restored.
                    //iCmPluginBaseEng.RestoreAttributeL( EVpnServicePolicy );
                    iCmPluginBaseEng.RestoreAttributeL( EVpnIapId );
                    iCmPluginBaseEng.RestoreAttributeL( ECmNextLayerIapId );
                    iCmPluginBaseEng.RestoreAttributeL( ECmNextLayerSNAPId );
                    iCmPluginBaseEng.RestoreAttributeL( ECmProxyServerName );
                    iCmPluginBaseEng.RestoreAttributeL( ECmProxyPortNumber );
                    
                    iCmPluginBaseEng.CmMgr().WatcherUnRegister();
                    }
                else
                    { // do not save, reconfigure
                    retval = EFalse;                    
                    }
                }
            break;
            }
        default:
            {
            if( aButtonId == EPluginBaseCmdExit )
                {
                iCmPluginBaseEng.CmMgr().WatcherUnRegister();
                }
            
            retval = CmPluginBaseSettingsDlg::OkToExitL( aButtonId );
            break;
            }
        }

    return retval;
    }
    
// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::GetHelpContext
// --------------------------------------------------------------------------
//
    
void CmPluginVpnSettingsDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOGGER_ENTERFN( "CmPluginVpnSettingsDlg::GetHelpContext" );
    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_VPN_AP_SETTINGS_VIEW;
    }

// --------------------------------------------------------------------------
// CmPluginVpnSettingsDlg::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginVpnSettingsDlg::CommsDatChangesL()
    {
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    CCmDestinationImpl* parentDest = iCmPluginBaseEng.ParentDestination();

    if ( parentDest )
        {
        if( !cmMgr.DestinationStillExistedL( parentDest ) )
            {
            cmMgr.WatcherUnRegister();
            // If parent destination diappears with some reason 
            // then the view must exit back to main view for it
            // may be danger if going back to parent view
            iExitReason = KDialogUserExit;
            TryExitL( iExitReason );
            
            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;
            }
        
        if( !cmMgr.IsIapStillInDestL( parentDest, iCmPluginBaseEng ) )
            {
            cmMgr.WatcherUnRegister();
            // In this case, the view can go back to the parent view
            TryExitL( iExitReason );
            
            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;            
            }
        
        // We may have to go back to parent view even though this Iap is still in CommsDat
        // for cmMgr ( = iCmPluginBaseEng.CmMgr() ) can not be accessed any more
        // after this call when some Iap is deleted.
        cmMgr.WatcherUnRegister();
        TryExitL( iExitReason );
        
        cmMgr.RemoveDestFromPool( parentDest );
        delete parentDest;
        }
    else
        {
        if( !cmMgr.IapStillExistedL( iCmPluginBaseEng ) )
            {
            cmMgr.WatcherUnRegister();
            // In this case, the dialog can go back to the parent view
            TryExitL( iExitReason );
            return;
            }
        
        // We may have to go back to parent view even though this Iap is still in CommsDat
        // for cmMgr ( = iCmPluginBaseEng.CmMgr() ) can not be accessed any more
        // after this call when some Iap is deleted.
        cmMgr.WatcherUnRegister();
        TryExitL( iExitReason );
        }
    }
