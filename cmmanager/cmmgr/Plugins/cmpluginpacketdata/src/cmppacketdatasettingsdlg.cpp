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
* Description:  Dialog for editing packet data settings for a packet data
*                connection method
*
*/

// INCLUDE FILES

// System
#include <StringLoader.h>
#include <akntextsettingpage.h>
#include <cmpacketdataui.rsg>
#include <cmmanager.rsg>
#include <csxhelp/cp.hlp.hrh>

// User
#include "cmlogger.h"
#include "cmpacketdatacommonconstants.h"
#include <cmpluginpacketdatadef.h>
#include "cmppacketdatasettingsdlg.h"
#include "cmppacketdatasettingsdlgadv.h"
#include "cmpluginpacketdata.h"
#include "cmpluginmenucommands.hrh"
#include <cmcommonconstants.h>
#include <cmpsettingsconsts.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

const TUint32 KPlainAuthSelectionItems[] =
    {
    R_QTN_SET_PASSWD_AUT_NORMAL,
    R_QTN_SET_PASSWD_AUT_SECURE,
    0
    };

const TUint32 KPromptForAuthSelectionItems[] = 
    {
    R_QTN_SET_PROMPT_PASSWD_NO,
    R_QTN_SET_PROMPT_PASSWD_YES,
    0
    };

// ================= MEMBER FUNCTIONS =======================================
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::NewL()
// Two-phase dconstructor, second phase is ConstructAndRunLD
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlg* CmPluginPacketDataSettingsDlg::NewL( 
                                            CCmPluginBaseEng& aCmPluginBase )
	{
	CmPluginPacketDataSettingsDlg* self = 
	           new ( ELeave ) CmPluginPacketDataSettingsDlg( aCmPluginBase );
	return self;
	}
	
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::CmPluginPacketDataSettingsDlg()
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlg::CmPluginPacketDataSettingsDlg( 
                                            CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlg( aCmPluginBase )
    , iNotifyFromSon( KCmNotifiedNone )
    {
    
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::UpdateListBoxContentL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::UpdateListBoxContentL()
    {
    CmPluginBaseSettingsDlg::UpdateListBoxContentL();
    }
    
//---------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::ConstructAndRunLD
// --------------------------------------------------------------------------
//

TInt CmPluginPacketDataSettingsDlg::ConstructAndRunLD( )
    {
    CleanupStack::PushL( this );
    LoadResourceL( KPluginPacketDataResDirAndFileName );
    CleanupStack::Pop( this );
   
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherRegisterL( this );

    return CmPluginBaseSettingsDlg::ConstructAndRunLD( );
    }

    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::~CmPluginPacketDataSettingsDlg
// Destructor
// --------------------------------------------------------------------------
//
CmPluginPacketDataSettingsDlg::~CmPluginPacketDataSettingsDlg()
    {
    iResourceReader.Close();
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlg::RunAdvancedSettingsL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::RunAdvancedSettingsL()
    {
    CmPluginPacketDataSettingsDlgAdv* advDlg = 
            CmPluginPacketDataSettingsDlgAdv::NewL( iCmPluginBaseEng );
    
    // This view may have to un-register as watcher here
    CCmManagerImpl& cmMgr = iCmPluginBaseEng.CmMgr();
    cmMgr.WatcherUnRegister();
    
    advDlg->RegisterParentView( this );
    TInt ret = advDlg->ConstructAndRunLD();

    if( iNotifyFromSon == KCmNotifiedNone )
        {
        if( ret == KDialogUserExit )
            {
            iExitReason = KDialogUserExit;
            TryExitL( iExitReason );
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
void CmPluginPacketDataSettingsDlg::UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray )
    {
    LOGGER_ENTERFN( "CmPluginPacketDataSettingsDlg::UpdateListBoxContentBearerSpecificL" );

    TBool boolSettingVal( EFalse ); // just for helping make code readable
        
    // DATA BEARER - only for real bearer types
    AppendSettingTextsL( aItemArray, ECmBearerSettingName );
    // AP NAME
    AppendSettingTextsL( aItemArray, EPacketDataAPName );
    // USER NAME
    AppendSettingTextsL( aItemArray, EPacketDataIFAuthName );
    // PROMPT PASSWORD
    boolSettingVal = iCmPluginBaseEng.GetBoolAttributeL( 
                                                EPacketDataIFPromptForAuth );
    AppendSettingTextsL( aItemArray, EPacketDataIFPromptForAuth,
                        boolSettingVal ? 
                                      R_QTN_SET_PROMPT_PASSWD_YES :
                                      R_QTN_SET_PROMPT_PASSWD_NO );
    // PASSWORD
    AppendSettingTextsL( aItemArray, EPacketDataIFAuthPass );
    // PASSWORD AUTHENTICATION
    boolSettingVal = iCmPluginBaseEng.GetBoolAttributeL( 
                                           EPacketDataDisablePlainTextAuth );
    AppendSettingTextsL( aItemArray,
                                EPacketDataDisablePlainTextAuth,
                                boolSettingVal ?
                                   R_QTN_SET_PASSWD_AUT_SECURE :
                                   R_QTN_SET_PASSWD_AUT_NORMAL );
    // HOMEPAGE
    AppendSettingTextsL( aItemArray, ECmStartPage );
    }
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::ChangeBoolSettingL
// --------------------------------------------------------------------------
//
TBool CmPluginPacketDataSettingsDlg::ChangeBoolSettingL( TUint32 aAttribute, 
                                                         TInt aCommandId )
    {
    LOGGER_ENTERFN( "CmPluginPacketDataSettingsDlg::ChangeBoolSettingL" );

    if ( aCommandId == EPluginBaseCmdChange )
        {
        const TUint32* items = NULL;
        TInt titleResId(0);
        
        // Add the list items
        switch ( aAttribute )
            {
            case EPacketDataDisablePlainTextAuth:
                {
                titleResId = R_QTN_SET_PASSWD_AUT;
                items = KPlainAuthSelectionItems;
                break;
                }
            case EPacketDataIFPromptForAuth:
                {
                titleResId = R_QTN_SET_PROMPT_PASSWD;
                items = KPromptForAuthSelectionItems;
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }
        
        TInt selected = iCmPluginBaseEng.GetBoolAttributeL( aAttribute );
        TInt originalValue = selected;        
        if ( ShowRadioButtonSettingPageL( items, 
                                          selected,
                                          titleResId ) && 
                                          originalValue != selected )
             {
             iCmPluginBaseEng.SetBoolAttributeL( aAttribute, selected );
             UpdateListBoxContentL();             
             }

        return selected;
        }
    else        
        {
        TBool boolFromCM = boolFromCM = 
                            iCmPluginBaseEng.GetBoolAttributeL( aAttribute );
        iCmPluginBaseEng.SetBoolAttributeL( aAttribute, !boolFromCM );
        UpdateListBoxContentL();
        return !boolFromCM;
        }
    }
    
// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::ShowPopupSettingPageL
// --------------------------------------------------------------------------
//
TBool CmPluginPacketDataSettingsDlg::ShowPopupSettingPageL( 
                                           TUint32 aAttribute, TInt aCommandId )
    {
    LOGGER_ENTERFN( "CmPluginPacketDataSettingsDlg::ShowPopupSettingPageL" );

    TBool retval = EFalse;
    switch ( aAttribute )
        {
        case EPacketDataIFAuthPass:
            {
            retval = ShowPopupPasswordSettingPageL( aAttribute );
            break;
            }
        case EPacketDataDisablePlainTextAuth:            
        case EPacketDataIFPromptForAuth:
            {
            ChangeBoolSettingL( aAttribute, aCommandId );
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
// CmPluginPacketDataSettingsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::ProcessCommandL( TInt aCommandId )
    {
    LOGGER_ENTERFN( "CmPluginPacketDataSettingsDlg::ProcessCommandL" );

    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EPluginBaseCmdAdvanced:
            {
            RunAdvancedSettingsL();
            break;
            }
        case EPluginBaseCmdChange: // flow to EAknSoftkeyOk
        case EAknSoftkeyOk:
            {
            TInt attrib = iSettingIndex->At( iListbox->CurrentItemIndex() );
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
// CmPluginPacketDataSettingsDlg::OkToExitL
// --------------------------------------------------------------------------
//
TBool CmPluginPacketDataSettingsDlg::OkToExitL( TInt aButtonId )
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
            retval = CmPluginBaseSettingsDlg::OkToExitL( aButtonId );
            break;
            }
        }
    
    return retval;
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::GetHelpContext
// --------------------------------------------------------------------------
//
    
void CmPluginPacketDataSettingsDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    LOGGER_ENTERFN( "CmPluginPacketDataSettingsDlg::GetHelpContext" );

    aContext.iMajor = KHelpUidPlugin;
    aContext.iContext = KSET_HLP_AP_SETTING_GPRS;
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::CommsDatChangesL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::CommsDatChangesL()
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

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::NotifyParentView
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::NotifyParentView( TInt aValue )
    {
    iNotifyFromSon = aValue;
    }

// --------------------------------------------------------------------------
// CmPluginPacketDataSettingsDlg::HandleCommsDatChangeL
// --------------------------------------------------------------------------
//
void CmPluginPacketDataSettingsDlg::HandleCommsDatChangeL()
    {    
    if( iNotifyFromSon == KCmNotifiedIapIsNotInThisDestination || 
            iNotifyFromSon == KCmNotifiedIapDisappear )
        {
        TryExitL( iExitReason );
        }
    else if( iNotifyFromSon == KCmNotifiedDestinationDisappear )
        {
        iExitReason = KDialogUserExit;
        TryExitL( iExitReason );
        }
    }
