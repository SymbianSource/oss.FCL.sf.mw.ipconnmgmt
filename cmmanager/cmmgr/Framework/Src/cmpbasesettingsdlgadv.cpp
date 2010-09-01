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
* Description:  Dialog for advanced settings for plugin base.
*
*/

#include <avkon.hrh>
#include <eikdef.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <cmmanager.rsg>
#include <data_caging_path_literals.hrh>
#include <aknnavide.h>
#include <akntitle.h>
#include <eikmenup.h>
#include <StringLoader.h>
#include <akntextsettingpage.h>
#include <cmconnectionmethoddef.h>

#include <mcmdexec.h>
#include <cmpbasesettingsdlgadv.h>
#include "cmpluginmenucommands.hrh"
#include "cmmanager.hrh"

using namespace CMManager;

enum TSMTPSettingIds
    {
    EPluginBaseCmCmdSmtp
    };
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::CmPluginBaseSettingsDlgAdv()
// --------------------------------------------------------------------------
//
EXPORT_C CmPluginBaseSettingsDlgAdv::CmPluginBaseSettingsDlgAdv( 
                                           CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlg( aCmPluginBase )
    {
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::~CmPluginBaseSettingsDlgAdv
// Destructor
// --------------------------------------------------------------------------
//
EXPORT_C  CmPluginBaseSettingsDlgAdv::~CmPluginBaseSettingsDlgAdv()
    {
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgAdv::DynInitMenuPaneL( 
                                                    TInt aResourceId,
                                                    CEikMenuPane* aMenuPane )
    {
    CmPluginBaseSettingsDlg::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_PLUGIN_BASE_MENU )
        {
        aMenuPane->SetItemDimmed( EPluginBaseCmdAdvanced, ETrue  );           
        }
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginBaseSettingsDlgAdv::UpdateListBoxContentBearerSpecificL( 
                                                 CDesCArray& /*aItemArray*/ )
    {
    // Empty implementation - forced by inheritance
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::RunAdvancedSettingsL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgAdv::RunAdvancedSettingsL()
    {
    // Empty implementation. This function has no meaning here
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgAdv::UpdateListBoxContentL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgAdv::UpdateListBoxContentL()
    {
    // Must save listbox indexes because content will be deleted
    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    // deleting listbox content
    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = (CDesCArray*)itemList;
    while ( itemArray->Count() )
        {
        itemArray->Delete( 0 );
        }
    while ( iSettingIndex->Count() )
        {
        iSettingIndex->Delete( 0 );
        }

    UpdateListBoxContentBearerSpecificL( *itemArray );
                     
    // Handle content changes
    iListbox->Reset();
    
    iListbox->HandleItemAdditionL();
    
    // Listbox might not have index
    if ( lastListBoxPos != -1 )
        {
        iListbox->SetCurrentItemIndex( lastListBoxPos );
        iListbox->SetTopItemIndex( lastListBoxTopPos );
        }
    // Draw it
    iListbox->DrawNow();
    }

//---------------------------------------------------------------------------         
// CmPluginBaseSettingsDlgAdv::ShowPopupSettingPageL
//---------------------------------------------------------------------------
//
EXPORT_C TBool CmPluginBaseSettingsDlgAdv::ShowPopupSettingPageL( 
                                           TInt aAttribute, TInt aCommandId )
    {
    TBool retval( EFalse );
    
    switch ( aAttribute )
        {
        case EPluginBaseCmCmdSmtp:
            {
            // Show the SMTP settings page
            User::Leave( KErrNotSupported );
            break;
            }      
        default:
            {
            CmPluginBaseSettingsDlg::ProcessCommandL( aCommandId );
            break;
            }
        }
    return retval;
    }
