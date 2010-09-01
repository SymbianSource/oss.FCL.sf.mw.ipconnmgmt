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
* Description:  WLAN Ipv6 settings dialog
*
*/

#include <StringLoader.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <cmcommonui.h>
#include <cmwlanui.rsg>
#include <cmmanager.rsg>
#include <csxhelp/cp.hlp.hrh>

#include <cmpluginwlandef.h>
#include "cmpluginwlan.h"
#include "cmpwlansettingsdlgipv6.h"
#include "cmwlancommonconstants.h"
#include "cmpluginmenucommands.hrh"
#include "cmpluginmultilinedialog.h"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

const TUint32 KIPv6SelectionItems[] = 
    {
    R_QTN_SET_DNS_SERVERS_AUTOMATIC,
    R_QTN_SET_IP_WELL_KNOWN,
    R_QTN_SET_IP_USER_DEFINED,
    0
    };
    
// ================= MEMBER FUNCTIONS =======================================

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::NewL
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv6* CmPluginWlanSettingsDlgIpv6::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginWlanSettingsDlgIpv6* self = 
	            new (ELeave) CmPluginWlanSettingsDlgIpv6( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::CmPluginWlanSettingsDlgIpv6
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv6::CmPluginWlanSettingsDlgIpv6( 
                                         CCmPluginBaseEng& aCmPluginBase ):
    CmPluginBaseSettingsDlgIp( aCmPluginBase ) ,
    iParent( NULL )
    {
    iHelpContext = KSET_HLP_AP_WLAN_AS_IPV6;
    }
    
// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::~CmPluginWlanSettingsDlgIpv6
// --------------------------------------------------------------------------
//
CmPluginWlanSettingsDlgIpv6::~CmPluginWlanSettingsDlgIpv6()
    {
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::ConstructAndRunLD
// --------------------------------------------------------------------------
//
TInt CmPluginWlanSettingsDlgIpv6::ConstructAndRunLD()
    {
    // Set this flag to allow edit continue
    iCanEditingContinue = ETrue;
    
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlgIp::ConstructAndRunLD();
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv6::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    TInt value( 0 );
    
    // DNS SERVERS IP ADDRESS
    switch ( GetIPv6DNSTypeL( EWlanIp6NameServer1,
                              EWlanIp6NameServer2 ) )
        {
        case EIpv6DNSDynamic:
            {
            value = R_QTN_SET_DNS_SERVERS_AUTOMATIC;
            break;
            }
        case EIpv6DNSWellKnown:
            {
            value = R_QTN_SET_IP_WELL_KNOWN;
            break;
            }
        case EIpv6DNSUserDefined:
            {
            value = R_QTN_SET_IP_USER_DEFINED;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    AppendSettingTextsL( aItemArray,
                         EWlanIp6DNSAddrFromServer,
                         value );
    }    

//---------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::ShowPopupSettingPageL
//---------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgIpv6::ShowPopupSettingPageL( TUint32 aAttribute, 
                                                          TInt aCommandId )
    {    
    TBool retval = EFalse;
    switch ( aAttribute )
        {
        case EWlanIp6DNSAddrFromServer:        
            {
            ShowPopupIPv6DNSEditorL( KIPv6SelectionItems, 
                                     EWlanIp6DNSAddrFromServer,
                                     EWlanIp6NameServer1,
                                     EWlanIp6NameServer2 );
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
// CmPluginWlanSettingsDlgIpv6::InitTextsL
// called before the dialog is shown to initialize localized textual data
//---------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv6::InitTextsL()
    {
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    if ( iEikonEnv )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
        iTitlePane = ( CAknTitlePane* )iStatusPane->ControlL(
                                TUid::Uid( EEikStatusPaneUidTitle ) );

        iOldTitleText = iTitlePane->Text()->AllocL();
        HBufC* title = StringLoader::LoadLC ( R_QTN_SET_IPV6_SETTINGS );
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
// CmPluginWlanSettingsDlgIpv6::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv6:: ProcessCommandL( TInt aCommandId )
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
        // fall-through intended here
        case EAknSoftkeyBack:
            {
            CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
            cmMgr.WatcherUnRegister();
            
            TryExitL( iExitReason );
            break;
            }           
        case EAknSoftkeyOk:
        case EPluginBaseCmdChange:
            {
            TInt attrib = iSettingIndex->At( iListbox->CurrentItemIndex() );
            ShowPopupSettingPageL( attrib, aCommandId);
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
// CmPluginWlanSettingsDlgIpv6::OkToExitL
// --------------------------------------------------------------------------
//
TBool CmPluginWlanSettingsDlgIpv6::OkToExitL( TInt aButtonId )
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
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case KDialogUserExit:
            {
            CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
            cmMgr.WatcherUnRegister();
            
            iExitReason = KDialogUserBack;
            retval = ETrue;
            }
            break;
        default:
            {
            retval = CmPluginBaseSettingsDlgIp::OkToExitL( aButtonId );
            break;
            }
        }
    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::RegisterParentView
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv6::RegisterParentView( CCmParentViewNotifier* aParent )
    {
    iParent = aParent;
    }

// --------------------------------------------------------------------------
// CmPluginWlanSettingsDlgIpv6::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginWlanSettingsDlgIpv6::CommsDatChangesL()
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
    else
        {
        // We may have to go back to parent view if database is changed by other application
        if( iParent )
            {
            iParent->NotifyParentView( KCmNotifiedIapDisappear );
            }
        iCanEditingContinue = EFalse;
        }
    }
