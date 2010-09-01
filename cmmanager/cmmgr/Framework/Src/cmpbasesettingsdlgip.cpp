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
* Description:  Dialog for IPv4 settings for plugin base.
*
*/

#include <cmmanager.rsg>

#include <mcmdexec.h>
#include <cmpbasesettingsdlgip.h>
#include <cmmanagerdef.h>
#include "cmmanager.hrh"

using namespace CMManager;

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgIp::CmPluginBaseSettingsDlgIp()
// --------------------------------------------------------------------------
//
EXPORT_C CmPluginBaseSettingsDlgIp::CmPluginBaseSettingsDlgIp( 
                                           CCmPluginBaseEng& aCmPluginBase )
    : CmPluginBaseSettingsDlg( aCmPluginBase )
    {
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgIp::~CmPluginBaseSettingsDlgIp
// Destructor
// --------------------------------------------------------------------------
//
EXPORT_C CmPluginBaseSettingsDlgIp::~CmPluginBaseSettingsDlgIp()
    {
    }

// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgIp::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgIp::DynInitMenuPaneL( 
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
// CmPluginBaseSettingsDlgIp::UpdateListBoxContentBearerSpecificL
// --------------------------------------------------------------------------
//
void CmPluginBaseSettingsDlgIp::UpdateListBoxContentBearerSpecificL( 
                                                 CDesCArray& /*aItemArray*/ )
    {
    // Empty implementation - forced by inheritance
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgIp::RunAdvancedSettingsL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgIp::RunAdvancedSettingsL()
    {
    // Empty implementation. This function has no meaning here
    }
    
// --------------------------------------------------------------------------
// CmPluginBaseSettingsDlgIp::UpdateListBoxContentL
// --------------------------------------------------------------------------
//
EXPORT_C void CmPluginBaseSettingsDlgIp::UpdateListBoxContentL()
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
    // Listbox might not have index
    if ( lastListBoxPos != -1 )
        {
        iListbox->SetCurrentItemIndex( lastListBoxPos );
        iListbox->SetTopItemIndex( lastListBoxTopPos );
        }
    // Draw it
    iListbox->DrawNow();
    }
