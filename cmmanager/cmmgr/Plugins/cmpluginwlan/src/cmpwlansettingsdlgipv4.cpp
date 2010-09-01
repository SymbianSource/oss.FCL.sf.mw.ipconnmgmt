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
* Description:  WLAN Ipv4 settings dialog
*
*/

#include <StringLoader.h>
#include <akntitle.h>
#include <akntextsettingpage.h>
#include <aknmfnesettingpage.h>  // for IP dialog
#include <cmcommonui.h>
#include <cmwlanui.rsg>
#include <cmmanager.rsg>
#include <csxhelp/cp.hlp.hrh>

#include <cmpluginbaseeng.h>
#include <cmpluginwlandef.h>
#include "cmpwlansettingsdlgipv4.h"
#include "cmwlancommonconstants.h"
#include "cmpluginmenucommands.hrh"
#include "cmpluginmultilinedialog.h"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

// ================= MEMBER FUNCTIONS =========================================

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::NewL
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv4* CmPluginWlanSettingsDlgIpv4::NewL(
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginWlanSettingsDlgIpv4* self = 
	            new ( ELeave ) CmPluginWlanSettingsDlgIpv4( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::CmPluginWlanSettingsDlgIpv4
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv4::CmPluginWlanSettingsDlgIpv4( 
                                        CCmPluginBaseEng& aCmPluginBase ) :
    CmPluginBaseSettingsDlgIp( aCmPluginBase ) ,
    iParent( NULL )
    {
    iHelpContext = KSET_HLP_AP_WLAN_AS_IPV4;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::ConstructAndRunLD
// --------------------------------------------------------------------------
//
TInt CmPluginWlanSettingsDlgIpv4::ConstructAndRunLD()
    {
    // Set this flag to allow edit continue
    iCanEditingContinue = ETrue;
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlgIp::ConstructAndRunLD();
    }
    
// ---------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::~CmPluginWlanSettingsDlgIpv4
// Destructor
// ---------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv4::~CmPluginWlanSettingsDlgIpv4()
    {
    }


// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv4::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
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

    // DNS SERVERS IP ADDRESS
    TBool boolValue = iCmPluginBaseEng.GetBoolAttributeL( 
                                                EWlanIpDNSAddrFromServer );
    AppendSettingTextsL( aItemArray,
                         EWlanIpDNSAddrFromServer,
                         boolValue ? 
                                R_QTN_SET_DNS_SERVERS_AUTOMATIC : 
                                R_QTN_SET_IP_USER_DEFINED );
    }
    
//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::ShowPopupSettingPageL
//---------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgIpv4::ShowPopupSettingPageL( TUint32 aAttribute, 
                                                          TInt aCommandId )
    {
    TBool retval = EFalse;
    switch ( aAttribute )
        {
        case EWlanIpDNSAddrFromServer:
            {
            ShowPopupIPv4DNSEditorL( EWlanIpDNSAddrFromServer, 
                                     EWlanIpNameServer1,
                                     EWlanIpNameServer2 );
            break;
            }
        case EWlanIpAddr:
        case EWlanIpNetMask:
        case EWlanIpGateway:
            {
            ShowPopupIpSettingPageL( aAttribute );
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlgIp::ShowPopupSettingPageL( aAttribute, 
                                                               aCommandId );
            break;            
            }
        }
    return retval;
    }


//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::ProcessCommandL
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv4::ProcessCommandL( TInt aCommandId )
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
            if ( ValidateAttribsL() )
                {
                CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
                cmMgr.WatcherUnRegister();
                
                iExitReason = KDialogUserExit;
                TryExitL( iExitReason );
                }
            break;
            }       
        case EPluginBaseCmdChange:
            {
            ShowPopupSettingPageL( 
                        iSettingIndex->At( iListbox->CurrentItemIndex() ),
                        aCommandId );
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlgIp::ProcessCommandL( aCommandId );
            break;            
            }
       }
   }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::OkToExitL
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgIpv4::OkToExitL( TInt aButtonId )
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
    
    TBool retval( EFalse );
    
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
        case EAknSoftkeyChange:
            {
            ProcessCommandL( EPluginBaseCmdChange );
            break;
            }
        case EAknSoftkeyBack:
            {
            if ( ValidateAttribsL() )
                {
                CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
                cmMgr.WatcherUnRegister();
                
                iExitReason = KDialogUserBack;
                retval = ETrue;
                }
            break;
            }
        default:
            {
            retval = CmPluginBaseSettingsDlgIp::OkToExitL( aButtonId );
            break;
            }
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::ValidateAttribsL
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgIpv4::ValidateAttribsL()
    {
    TBool retVal ( EFalse );
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
                // quit without saving, so restore attributes
                iCmPluginBaseEng.RestoreAttributeL( EWlanIpAddr );
                iCmPluginBaseEng.RestoreAttributeL( EWlanIpNetMask );
                iCmPluginBaseEng.RestoreAttributeL( EWlanIpGateway );
                iCmPluginBaseEng.RestoreAttributeL( EWlanIpDNSAddrFromServer );
                retVal = ETrue;
                }
            }
         else
            {
            retVal = ETrue;
            }
        }
    else
        {
        retVal = ETrue;
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::InitTextsL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv4::InitTextsL()
    {
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    if ( iEikonEnv )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
        iTitlePane =
            ( CAknTitlePane* )iStatusPane->ControlL(
                                TUid::Uid( EEikStatusPaneUidTitle ) );

        iOldTitleText = iTitlePane->Text()->AllocL();
        HBufC* title = StringLoader::LoadLC ( R_QTN_SET_IPV4_SETTINGS );
        iTitlePane->SetTextL( *title ) ;
        CleanupStack::PopAndDestroy(title);                         
        iNaviPane = ( CAknNavigationControlContainer* ) 
                        iStatusPane->ControlL( 
                                TUid::Uid( EEikStatusPaneUidNavi ) );
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
        iNaviPane->PushL( *iNaviDecorator );
        }
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::RegisterParentView
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv4::RegisterParentView( CCmParentViewNotifier* aParent )
    {
    iParent = aParent;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv4::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv4::CommsDatChangesL()
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
