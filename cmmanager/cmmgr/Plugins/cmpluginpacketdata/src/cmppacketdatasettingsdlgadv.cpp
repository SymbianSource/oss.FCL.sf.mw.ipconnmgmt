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
* Description:  Dialog for setting packet data plugin advanced settings
*
*/

// INCLUDE FILES

#include <cmpacketdataui.rsg>
#include <cmmanager.rsg>
#include <StringLoader.h>
#include <akntextsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknmfnesettingpage.h>
#include <csxhelp/cp.hlp.hrh>

#include <cmpluginpacketdatadef.h>
#include "cmppacketdatasettingsdlgadv.h"
#include "cmpacketdatacommonconstants.h"
#include "cmpluginmultilinedialog.h"
#include <cmcommonui.h>
#include "cmpluginmenucommands.hrh"
#include "cmpluginpacketdata.h"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

const TUint32 KDNSSelectionItems[] = 
    { 
    R_QTN_SET_IP_ADDRESS_DYNAMIC,
    R_QTN_SET_IP_WELL_KNOWN,
    R_QTN_SET_IP_USER_DEFINED,
    0
    };            

const TUint32 KPDPSelectionItems[] =
    {
    R_GPRS_PLUGIN_VIEW_PDP_TYPE_IPV4,
    R_GPRS_PLUGIN_VIEW_PDP_TYPE_IPV6,
    0
    };
    
// ================= MEMBER FUNCTIONS =======================================
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlgAdv* CmPluginPacketDataSettingsDlgAdv::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginPacketDataSettingsDlgAdv* self = 
	        new ( ELeave ) CmPluginPacketDataSettingsDlgAdv( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::CmPluginPacketDataSettingsDlgAdv()
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlgAdv::CmPluginPacketDataSettingsDlgAdv( 
                                            CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlgAdv( aCmPluginBase )
    , iIpv6Supported( aCmPluginBase.FeatureSupported( KFeatureIdIPv6 ) )
    , iParent( NULL )
    {
    iHelpContext = KSET_HLP_AP_SETTING_GPRS_AS;
    }
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::~CmPluginPacketDataSettingsDlgAdv
// Destructor
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlgAdv::~CmPluginPacketDataSettingsDlgAdv()
    {
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::ConstructAndRunLD
// --------------------------------------------------------------------------
//
TInt CmPluginPacketDataSettingsDlgAdv::ConstructAndRunLD()
    {
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );
    
    return CmPluginBaseSettingsDlgAdv::ConstructAndRunLD();
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    TInt valueResId = 0;    
    TInt pdpType = RPacketContext::EPdpTypeIPv4;
    
    // Get the PDP type - EPdpTypeIPv4 by default
    if ( iIpv6Supported )
        {
        pdpType = iCmPluginBaseEng.GetIntAttributeL( EPacketDataPDPType );
        
        // NETWORK (PDP) TYPE
        AppendSettingTextsL( aItemArray,
                             EPacketDataPDPType,
                             pdpType == RPacketContext::EPdpTypeIPv6 ? 
                                R_GPRS_PLUGIN_VIEW_PDP_TYPE_IPV6 :
                                R_GPRS_PLUGIN_VIEW_PDP_TYPE_IPV4 );
        }    
    
    if ( pdpType != RPacketContext::EPdpTypeIPv6 )
        {
        // IP ADDRESS
        AppendSettingTextsL( aItemArray, EPacketDataIPAddr );
        }
        
    if ( iIpv6Supported )
        {
        // DNS SERVERS IP ADDRESS
        if ( pdpType == RPacketContext::EPdpTypeIPv6 )
            {
            switch ( GetIPv6DNSTypeL( EPacketDataIPIP6NameServer1,
                                      EPacketDataIPIP6NameServer2 ) )
                {
                case EIPv6Unspecified:
                    {
                    valueResId = R_QTN_SET_IP_ADDRESS_DYNAMIC;
                    break;
                    }
                case EIPv6WellKnown:
                    {
                    valueResId = R_QTN_SET_IP_WELL_KNOWN;
                    break;
                    }
                case EIPv6UserDefined:
                    {
                    valueResId = R_QTN_SET_IP_USER_DEFINED;
                    break;
                    }
                default:
                    {                    
                    User::Leave( KErrNotSupported );
                    break;
                    }
                }
            }
        else
            {
            if ( !iCmPluginBaseEng.GetBoolAttributeL( 
                                           EPacketDataIPDNSAddrFromServer ) )
                {
                valueResId = R_QTN_SET_IP_USER_DEFINED;
                }
            else
                {
                valueResId = R_QTN_SET_DNS_SERVERS_AUTOMATIC;
                }
            }    
        AppendSettingTextsL( aItemArray,
                             EPacketDataIPIP6DNSAddrFromServer,
                             valueResId );
        }
    else            
        {
        // PRIMARY NAME SERVER
        AppendSettingTextsL( aItemArray, EPacketDataIPNameServer1 );
        AppendSettingTextsL( aItemArray, EPacketDataIPNameServer2 );
        }

    // PROXY SETTINGS
    // PROXY SERVER ADDRESS
    AppendSettingTextsL( aItemArray, ECmProxyServerName );
    // PROXY PORT NUMBER
    AppendSettingTextsL( aItemArray, ECmProxyPortNumber );
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::ShowPopupPacketDataIPDNSAddrFromServerL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::
                                    ShowPopupPacketDataIPDNSAddrFromServerL()
    {
    TInt PDPType = iCmPluginBaseEng.GetIntAttributeL( EPacketDataPDPType );

    if ( PDPType == RPacketContext::EPdpTypeIPv6 )
        {
        ShowPopupIPv6DNSEditorL( KDNSSelectionItems,
                                 EPacketDataIPIP6DNSAddrFromServer,
                                 EPacketDataIPIP6NameServer1,
                                 EPacketDataIPIP6NameServer2 );
        }
    else //ipv4
        {
        ShowPopupIPv4DNSEditorL( EPacketDataIPDNSAddrFromServer,
                                 EPacketDataIPNameServer1,
                                 EPacketDataIPNameServer2 );
        }        
    }
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::ShowPopupSettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginPacketDataSettingsDlgAdv::ShowPopupSettingPageL( 
                                           TUint32 aAttribute, TInt aCommandId )
    {    
    TBool retval = EFalse;
    
    switch ( aAttribute )
        {
        // IPDNS Settings
        case EPacketDataIPDNSAddrFromServer:
        case EPacketDataIPIP6DNSAddrFromServer:        
            {
            ShowPopupPacketDataIPDNSAddrFromServerL();
            break;
            }
        case EPacketDataPDPType:            
            {
            if ( aCommandId == EAknSoftkeyOk )
                {
                TInt intFromCM = 0;
                TRAPD( err, intFromCM = iCmPluginBaseEng.GetIntAttributeL( 
                                                              aAttribute ) );
                
                if ( !err )
                    {
                    intFromCM == RPacketContext::EPdpTypeIPv6 ? 
                                intFromCM = RPacketContext::EPdpTypeIPv4 : 
                                intFromCM = RPacketContext::EPdpTypeIPv6;
                    }
                else    
                    {
                    User::Leave( KErrNotSupported );
                    }
                              
                iCmPluginBaseEng.SetIntAttributeL( aAttribute, intFromCM );
                UpdateListBoxContentL();  
                }
            else
                {
                if ( aCommandId == EPluginBaseCmdChange )
                
                    {
                    ShowPDPTypeRBPageL( aAttribute );
                    }
                else
                    {
                    TInt pdpType = iCmPluginBaseEng.GetIntAttributeL( EPacketDataPDPType );
                    pdpType = pdpType == RPacketContext::EPdpTypeIPv4 ? 
                                            RPacketContext::EPdpTypeIPv6 : 
                                            RPacketContext::EPdpTypeIPv4;
                    iCmPluginBaseEng.SetIntAttributeL( aAttribute, pdpType );
                    UpdateListBoxContentL();
                    }
                }
            break;
            }
        // IP Setting Page
        case EPacketDataIPNameServer1:                
        case EPacketDataIPNameServer2:
        case EPacketDataIPAddr:
            {
            ShowPopupIpSettingPageL( aAttribute );
            break;
            }
        // Text Settings        
        case ECmProxyServerName:
            {
            retval = ShowPopupTextSettingPageL( aAttribute );
            break;
            }
        // Numeric Settings
        case ECmProxyPortNumber:
            {
            if ( ShowPopupPortNumSettingPageL( aAttribute ) )
                {
                retval = ETrue;
                }
            break;
            }
        default:
            {
            CmPluginBaseSettingsDlgAdv::ShowPopupSettingPageL( aAttribute, 
                                                               aCommandId );
            break;            
            }
        }
    return retval;
    }
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::ProcessCommandL( TInt aCommandId )
    {
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EPluginBaseCmdExit:
            {
            iExitReason = KDialogUserExit; //  flow to EAknSoftkeyBack
            }            
        case EAknSoftkeyBack:
            {
            TryExitL( iExitReason );
            break;
            }  
        case EAknSoftkeyOk:                     
        case EPluginBaseCmdChange:
            {
            TInt attrib = iSettingIndex->At( iListbox->CurrentItemIndex() );
            ShowPopupSettingPageL( attrib, aCommandId );

            if ( iHasSettingChanged )
                {
                iHasSettingChanged = EFalse; // Don't call UpdateL at this stage
                }
            break;
            }         
        default:
            {
            CmPluginBaseSettingsDlgAdv::ProcessCommandL( aCommandId );

            if ( iHasSettingChanged )
                {
                iHasSettingChanged = EFalse; // Don't call UpdateL at this stage
                }
            break;            
            }
        }
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::OkToExitL
// --------------------------------------------------------------------------
//
TBool CmPluginPacketDataSettingsDlgAdv::OkToExitL( TInt aButtonId )
    {
    TBool retval( EFalse );
    switch ( aButtonId )
        {
        case EPluginBaseCmdExit:
        case EAknSoftkeyBack:
            {
            CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
            cmMgr.WatcherUnRegister();
            }
        default:
            {
            retval = CmPluginBaseSettingsDlgAdv::OkToExitL( aButtonId );
            break;
            }
        }
    
    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::ShowPDPTypeRBPageL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::ShowPDPTypeRBPageL( TUint32 aAttribute )
    {
    TInt selected = iCmPluginBaseEng.GetIntAttributeL( aAttribute );
    
    TInt originalValue = selected;
    
    if ( ShowRadioButtonSettingPageL( KPDPSelectionItems, 
                                      selected, 
                                      R_GPRS_PLUGIN_VIEW_PDP_TYPE ) && 
         originalValue != selected )
        {
        iCmPluginBaseEng.SetIntAttributeL( aAttribute , selected );
        UpdateListBoxContentL();             
        }
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::RegisterParentView
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::RegisterParentView( CCmParentViewNotifier* aParent )
    {
    iParent = aParent;
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlgAdv::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlgAdv::CommsDatChangesL()
    {
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
            
            cmMgr.WatcherUnRegister();
            // If parent destination is deleted by somebody then the dialog must exit back to main view
            iExitReason = KDialogUserExit;
            TryExitL( iExitReason );
            
            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;
            }
        
        if( !cmMgr.IsIapStillInDestL( parentDest, iCmPluginBaseEng ) )
            {
            if( iParent )
                {
                iParent->NotifyParentView( KCmNotifiedIapIsNotInThisDestination );
                }
            
            cmMgr.WatcherUnRegister();
            // In this case, the dialog can go back to the parent view
            TryExitL( iExitReason );
            
            cmMgr.RemoveDestFromPool( parentDest );
            delete parentDest;
            return;
            }
        
        // We may have to notify parent view to go back to its parent view even though this Iap is still in CommsDat
        // for cmMgr ( = iCmPluginBaseEng.CmMgr() ) can not be accessed any more
        // after this call when some Iap is deleted.
        if( iParent )
            {
            iParent->NotifyParentView( KCmNotifiedIapDisappear );
            }
        cmMgr.WatcherUnRegister();
        TryExitL( iExitReason );
        
        cmMgr.RemoveDestFromPool( parentDest );
        delete parentDest;
        }
    else
        {
        if( !cmMgr.IapStillExistedL(iCmPluginBaseEng) )
            {
            if( iParent )
                {
                iParent->NotifyParentView( KCmNotifiedIapDisappear );
                }
            
            cmMgr.WatcherUnRegister();
            // In this case, the dialog can go back to the parent view
            TryExitL( iExitReason );
            return;
            }
        
        // We may have to notify parent view to go back to its parent view even though this Iap is still in CommsDat
        // for cmMgr ( = iCmPluginBaseEng.CmMgr() ) can not be accessed any more
        // after this call when some Iap is deleted.
        if( iParent )
            {
            iParent->NotifyParentView( KCmNotifiedIapDisappear );
            }
        cmMgr.WatcherUnRegister();
        TryExitL( iExitReason );
        }
    }
