/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ApSettingsPlugin class.
*
*/


// INCLUDE FILES

// User includes
#include "APSettingsPlugin.h"
#include "StringLoader.h"
#include "utilities.h"

#include <cmsettingsui.h>

// System includes
#include <eikon.hrh>
#include <f32file.h>
#include <ConeResLoader.h>
#include <ApSettingsPluginRsc.rsg>
#include <data_caging_path_literals.hrh>
#include <apsettingsplugin.mbg>
#include <settingsinternalcrkeys.h>
#include <centralrepository.h>


const TInt KDefaultAPSettingsValue 	  = 1;	// AP settings UI supported

// Resource file location
_LIT( KApSettingsPluginRscFileName, "ApSettingsPluginRsc.rsc" );

// This UID is used for both the view UID 
// and the ECOM plugin implementation ID.
const TUid KApSettingsPluginUid = { 0x1020736B };

// ROM folder
_LIT( KDriveZ, "z:" );

// Name of the MBM file containing icons
_LIT( KFileIcons, "apsettingsplugin.mbm");


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSettingsPlugin::NewL
// First phase constructor
// ---------------------------------------------------------
CApSettingsPlugin* CApSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CApSettingsPlugin* self = new ( ELeave ) CApSettingsPlugin();
    CleanupStack::PushL( self );
    
    self->ConstructL();
                      
    CleanupStack::Pop( self );
    return self;     
    }

// ---------------------------------------------------------
// CApSettingsPlugin::~CApSettingsPlugin
// Destructor
// ---------------------------------------------------------
CApSettingsPlugin::~CApSettingsPlugin()
    {
    iResources.Close();
#ifdef _DEBUG
    RDebug::Print( _L( "[CApSettingsPlugin] ~CApSettingsPlugin()" ) );
#endif
    }

// ---------------------------------------------------------
// CApSettingsPlugin::Id
// ---------------------------------------------------------
TUid CApSettingsPlugin::Id() const
    {
    return KApSettingsPluginUid;
    }
 
// ---------------------------------------------------------
// CApSettingsPlugin::DoActivateL()
// ---------------------------------------------------------      
void CApSettingsPlugin::DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
                          TUid /*aCustomMessageId*/, 
                          const TDesC8& /*aCustomMessage*/ )
    {
    //no implementation required
    }
    
// ---------------------------------------------------------
// CApSettingsPlugin::DoDeactivate()
// ---------------------------------------------------------
void CApSettingsPlugin::DoDeactivate()
    {
    //no implementation required
    }
       
// ---------------------------------------------------------
// CApSettingsPlugin::GetCaption
// ---------------------------------------------------------
void CApSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    StringLoader::Load( aCaption, R_QTN_NETW_CONSET_NETWORK_CONNECTIONS  );
    }

// ---------------------------------------------------------
// CApSettingsPlugin::HasBitmap
// ---------------------------------------------------------
TBool CApSettingsPlugin::HasBitmap() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CApSettingsPlugin::GetBitmapL
// ---------------------------------------------------------
void CApSettingsPlugin::GetBitmapL( CFbsBitmap* /*aBitmap*/, 
                                    CFbsBitmap* /*aMask*/ ) const
    {
    //no bitmap
    User::Leave( KErrNotFound );
    }
    
// ---------------------------------------------------------
// CApSettingsPlugin::PluginProviderCategory
// ---------------------------------------------------------
TInt CApSettingsPlugin::PluginProviderCategory() const
    {
    return EGSPluginProviderOEM;
    }
    
// ---------------------------------------------------------
// CApSettingsPlugin::HandleSelection
// ---------------------------------------------------------   
void CApSettingsPlugin::HandleSelection(
            const TGSSelectionTypes /*aSelectionType*/ )
    {
    TRAPD( error, DoSettingsL() );
    
    if( error && error != KLeaveWithoutAlert )
        {
        Utilities::ShowErrorNote(error);
        }    
    }

// ---------------------------------------------------------
// CApSettingsPlugin::ItemType
// ---------------------------------------------------------
TGSListboxItemTypes CApSettingsPlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }
    
// ---------------------------------------------------------
// CApSettingsPlugin::CreateIconL
// ---------------------------------------------------------    
CGulIcon* CApSettingsPlugin::CreateIconL( const TUid aIconType )
    {
    TFileName iconsFileName;

    iconsFileName.Append( KDriveZ );
    iconsFileName.Append( KDC_APP_BITMAP_DIR );
    iconsFileName.Append( KFileIcons );
    
	CGulIcon* icon;
       
    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetConnAccp, 
        iconsFileName,
        EMbmApsettingspluginQgn_prop_set_conn_accp,
        EMbmApsettingspluginQgn_prop_set_conn_accp_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }
	
	return icon;
    }    

// ---------------------------------------------------------
// CApSettingsPlugin::Visible
// -----------------------------------------------
TBool CApSettingsPlugin::Visible() const
    {
  	if ( iApSettingsSupported == KDefaultAPSettingsValue )
  		{
  		return ETrue;
		}
	else
		{
		return EFalse;
		}
    }

// ---------------------------------------------------------
// CApSettingsPlugin::CApSettingsPlugin
// ---------------------------------------------------------
CApSettingsPlugin::CApSettingsPlugin():
    iResources( *CCoeEnv::Static() )
    {
    // no implementation required
    }

// ---------------------------------------------------------
// CApSettingsPlugin::ConstructL
// ---------------------------------------------------------
TAny CApSettingsPlugin::ConstructL()
    {
    
#ifdef _DEBUG    
    RDebug::Print( _L("[CApSettingsPlugin] ConstructL()" ) );
    RDebug::Print( _L( "[CApSettingsPlugin] Loading resource from :" ) );
    RDebug::Print( KApSettingsPluginRscFileName );
#endif
    
    BaseConstructL();
        
    TFileName fileName;
    TDriveUnit drive( EDriveZ );
    
    fileName.Append( drive.Name() );
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KApSettingsPluginRscFileName );
    
    iResources.OpenL( fileName );
    
  	iApSettingsSupported = KDefaultAPSettingsValue;
    }
    
// ---------------------------------------------------------
// CApSettingsPlugin::DoSettingsL
// ---------------------------------------------------------
void CApSettingsPlugin::DoSettingsL()
    {
    CCmSettingsUi* settings = CCmSettingsUi::NewL();
    CleanupStack::PushL( settings );
    
    if( settings->RunSettingsL() == CCmSettingsUi::EExit )
        {
        //exit from General Settings
        ProcessCommandL( EAknCmdExit );
        }
    
    CleanupStack::PopAndDestroy( settings );
    }
    

//End of file
