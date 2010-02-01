/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Cpp file for Connectivity Settings plugin.
*
*/


// Includes
#include <featmgr.h>
#include "gsconnsettingsplugin.h"
#include "gsconnsettingsplugin.hrh"
#include "gsconnsettingspluginmodel.h"
#include "gsconnsettingsplugincontainer.h"
#include "gsconnsettingsselectiondlg.h"

#include <gsconnsettingspluginrsc.rsg>
#include <gsconnsettingsplugin.mbg>

// Includes from GS framework:
#include <GSCommon.hrh>
#include <GSFWViewUIDs.h>
#include <GSPrivatePluginProviderIds.h>
#include <GSTabHelper.h>
#include <GSMainView.h>

#include <hlplch.h> // For HlpLauncher
#include <featmgr.h>
#include <csxhelp/cp.hlp.hrh>

#include <gulicon.h>
#include <AknsUtils.h>
#include <aknViewAppUi.h>
#include <aknradiobuttonsettingpage.h>
#include <AknQueryDialog.h>
#include <ConeResLoader.h>
#include <barsread.h> // For TResourceReader
#include <StringLoader.h>

// Constants


// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::CConnectivitySettingsPlugin()
// Constructor
// ---------------------------------------------------------------------------
//
CGSConnSettingsPlugin::CGSConnSettingsPlugin()
    : iResourceLoader( *iCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::~CConnectivitySettingsPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CGSConnSettingsPlugin::~CGSConnSettingsPlugin()
    {
    iResourceLoader.Close();
    
    
    // Delete plugin loader
    if ( iPluginLoader )
        {
        delete iPluginLoader;
        }

    // Delete possible plugins
    if ( iPluginArray )
        {
        // Since the plugins are actually avkon views, avkon is responsible
        // for owning the plugins. This means we do not reset and destroy
        // the contents of the array in which the plugins reside. We have to
        // leave it up to avkon to tidy up.
        delete iPluginArray;
        }      
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::NewL()
// First phase constructor
// ---------------------------------------------------------------------------
//
CGSConnSettingsPlugin* CGSConnSettingsPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CGSConnSettingsPlugin* self = new(ELeave) CGSConnSettingsPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::ConstructL()
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::ConstructL()
    {
    // Initialize FeatureManager
    FeatureManager::InitializeLibL();
    iIsWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    // Unload FeatureManager
    FeatureManager::UnInitializeLib();  
    
    OpenLocalizedResourceFileL( KGSConnSettingsPluginResourceFileName,
                                iResourceLoader ); 
    BaseConstructL( R_GS_CONN_SETTINGS_PLUGIN_VIEW );
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::Id()
// ---------------------------------------------------------------------------
//
TUid CGSConnSettingsPlugin::Id() const
    {
    return KGSConnSettingsPluginUid;
    }



// ----------------------------------------------------------------------------
// CGSConnSettingsPlugin::NewContainerL()
// Creates new iContainer.
//
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::NewContainerL()
    {
    iContainer = new( ELeave ) CGSConnSettingsPluginContainer( *this );
    
    //Check if ECOM plugins are already loaded
    if ( iPluginArray == NULL )
        {
        //Load plugins here after container is constructed. This saves memory when Connectivity Settings view
        //is not visible(object from CGSConnSettingsPlugin is constructed already in Connectivity view by GS and
        //container class only when the plugin is actually accessed) 

        iPluginArray = new  ( ELeave ) CArrayPtrFlat<CGSPluginInterface>( KGConnSettingsPluginCount );
        iPluginLoader = CGSPluginLoader::NewL( iAppUi );
        iPluginLoader->SetObserver( this );
        iPluginLoader->LoadAsyncL( KGSPluginInterfaceUid, Id(), iPluginArray ); 
        }
    
    Container()->SetPluginArray( iPluginArray );
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::DoActivateL()
//
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                    TUid aCustomMessageId,
                                    const TDesC8& aCustomMessage )
    {
    CGSBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );

    // Add support for external view activation    
    if ( iPrevViewId.iAppUid != KUidGS || iPrevViewId.iViewUid == TUid::Uid(0x0DEADBED) )
      {
      CEikButtonGroupContainer* cbaGroup = Cba();
      if(cbaGroup)
        {
        HBufC* rightSKText = StringLoader::LoadLC (R_GS_CONN_SETTINGS_RSK_LABEL_EXIT);
        TPtr rskPtr = rightSKText->Des();
        cbaGroup->SetCommandL(2,EAknSoftkeyExit,*rightSKText);
        CleanupStack::PopAndDestroy(rightSKText);
        }
      }    
    iModel = Container()->Model();
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::DoDeactivate()
//
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::DoDeactivate()
    {
    CGSBaseView::DoDeactivate();
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin:: HandleForegroundEventL
//
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::HandleForegroundEventL( TBool aForeground )
    {
    //Load saved settings from Connection Settings API
    //If function leaves it is trapped and ignored as there is nothing that we can do about it
    if ( iModel && aForeground )
        {
        TRAP_IGNORE( iModel->LoadSettingsL() );
        UpdateListBoxL( EGSSettIdDataUsageHomeNw );
        }
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPlugin::HandleCommandL
//
// Handles menu commands directed to this class.
// ----------------------------------------------------------------------------
void CGSConnSettingsPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EGSConnSettMenuOpen:
        case EGSConnSettMenuChange:
        case EGSConnSettMskGeneral:
            HandleListBoxSelectionL();
            break;
            
        case EAknSoftkeyBack:
            iAppUi->ActivateLocalViewL( KGSConPluginUid );
            break;
            
        case EAknCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
            break;
            }
        default:
            iAppUi->HandleCommandL( aCommand );
            break;
        }
    }


// -----------------------------------------------------------------------------
// CGSSensorPlugin::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Check resource
    if ( R_GS_CONN_SETTINGS_MENU_ITEM_OPEN == aResourceId )
        {
        // First, resolve current item in listbox
        const TInt currentItem = Container()->CurrentFeatureId();

        switch (currentItem)
            {
            case EGSSettIdUsageOfWlan:
            case EGSSettIdDataUsageAbroad:
            case EGSSettIdDataUsageHomeNw:
                // Hide menu item 'Open'
                aMenuPane->SetItemDimmed( EGSConnSettMenuOpen, ETrue );
                aMenuPane->SetItemDimmed( EGSConnSettMenuChange, EFalse );
            break;
            
            default:
                {
                // Hide menu item 'Change' if current item is ECOM plugin
                aMenuPane->SetItemDimmed( EGSConnSettMenuOpen, EFalse );
                aMenuPane->SetItemDimmed( EGSConnSettMenuChange, ETrue );
                break;
                }
            }
        }
    // Forward to base class
    CGSBaseView::DynInitMenuPaneL( aResourceId, aMenuPane );
    }


// ----------------------------------------------------------------------------
// CGSConnSettingsPlugin::HandleListBoxSelectionL()
//
// Handles events raised through a rocker key
// ----------------------------------------------------------------------------
void CGSConnSettingsPlugin::HandleListBoxSelectionL()
    {
    const TInt currentFeatureId = Container()->CurrentFeatureId();
        
    switch ( currentFeatureId )
        {
        case EGSSettIdUsageOfWlan:
            ShowUsageOfWlanSettingPageL();
            break;
            
        case EGSSettIdDataUsageAbroad:
            ShowDataUsageAbroadSettingPageL();
            break;
            
        case EGSSettIdDataUsageHomeNw:
            ShowDataUsageInHomeNwSettingPageL();
            break;
            
        default:
            {
            //Feature ID didn't match, try plugins
            CGSPluginInterface* selectedPlugin( Container()->SelectedPluginL() );

            switch( selectedPlugin->ItemType() )
                {
                // In these cases the plugin is a view:
                case EGSItemTypeSingleLarge:
                case EGSItemTypeSetting:
                case EGSItemTypeSettingIcon:
                    {
                    CleanupStack::PushL( selectedPlugin );
                    iAppUi->ActivateLocalViewL( selectedPlugin->Id() );
                    CleanupStack::Pop( selectedPlugin );
                    break;
                    }
                    
                // In these cases the plugin is a dialog:
                case EGSItemTypeSettingDialog:
                case EGSItemTypeSingleLargeDialog:
                     selectedPlugin->HandleSelection( EGSSelectionByMenu );
                     break;
                default:
                    break;
                }
            break;
            }        
        }
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPlugin::UpdateListBoxL
//
//
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::UpdateListBoxL( TInt aItemId )
    {
    if( Container() )
        {
        Container()->UpdateListBoxL( aItemId );
        }
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPlugin::Container
//
// Return handle to container class.
// ----------------------------------------------------------------------------
//
CGSConnSettingsPluginContainer* CGSConnSettingsPlugin::Container()
    {
    return static_cast<CGSConnSettingsPluginContainer*>( iContainer );
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::UpperLevelViewUid()
//
//
// ---------------------------------------------------------------------------
//
TUid CGSConnSettingsPlugin::UpperLevelViewUid()
    {
    return KGSConPluginUid;
    }

// -----------------------------------------------------------------------------
// CGSConnSettingsPlugin::GetHelpContext()
//
//
// -----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::GetHelpContext( TCoeHelpContext& aContext )
    {
    aContext.iMajor = KUidGS;
    aContext.iContext = KHLP_OCC_CONN_SETTINGS;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::GetCaptionL()
//
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // The resource file is already opened by iResourceLoader.
    HBufC* result = StringLoader::LoadL( R_GS_CONN_SETTINGS_VIEW_CAPTION );
    aCaption.Copy( *result );
    delete result;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::CreateIconL
//
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CGSConnSettingsPlugin::CreateIconL( const TUid aIconType )
    {

    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    fp->Set( KGSConnSettingsPluginIconDirAndName, &KDC_BITMAP_DIR, NULL );

    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropCpConnAdmin,
        fp->FullName(),
// Let's use Connectivity plugin icon until we get graphics
// for Connectivity Settings plugin icon
//      EMbmGsconnsettingspluginQgn_prop_cp_conn_settings,
//      EMbmGsconnsettingspluginQgn_prop_cp_conn_settings_mask );
        EMbmGsconnsettingspluginQgn_prop_cp_conn, //from generated \epoc32\include\gsconnsettingsplugin.mbg
        EMbmGsconnsettingspluginQgn_prop_cp_conn_mask ); //from generated \epoc32\include\gsconnsettingsplugin.mbg
        }
       else
           {
           icon = CGSPluginInterface::CreateIconL( aIconType );
           }
    CleanupStack::PopAndDestroy( fp );
    return icon;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::PluginProviderCategory()
//
//
// ---------------------------------------------------------------------------
//
TInt CGSConnSettingsPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::ShowUsageOfNewWlanSettingPageL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::ShowUsageOfWlanSettingPageL()
    {

    CDesCArrayFlat* items = iCoeEnv->ReadDesC16ArrayResourceL(
            R_USAGE_OF_WLAN_SETTING_PAGE_LBX );
    CleanupStack::PushL( items );

    TInt currentItem = iModel->UsageOfWlan();
    TInt oldItem = currentItem;
    
    CGSConnSettingsSelectionDlg* dlg = CGSConnSettingsSelectionDlg::NewL(
                                                R_USAGE_OF_WLAN_SETTING_PAGE,
                                                currentItem,
                                                items,
                                                R_USAGE_OF_WLAN_INFO_POPUP_TEXTS );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {

        if( currentItem != oldItem )
            {
            iModel->SetUsageOfWlan( currentItem );
            UpdateListBoxL( EGSSettIdUsageOfWlan );
            //Save current settings when the setting is changed
            //If function leaves it is trapped and ignored as there is nothing that we can do about it
            TRAP_IGNORE(iModel->SaveSettingsL());
            }
        }

    CleanupStack::PopAndDestroy( items );

    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::ShowDataUsageAbroadSettingPageL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::ShowDataUsageAbroadSettingPageL()
    {
    CDesCArrayFlat* items;
    
    if ( iIsWlanSupported )
        {
        items = iCoeEnv->ReadDesC16ArrayResourceL(
                R_DATA_USAGE_ABROAD_SETTING_PAGE_LBX );
        }
    else
        {
        items = iCoeEnv->ReadDesC16ArrayResourceL(
                R_DATA_USAGE_ABROAD_SETTING_PAGE_LBX_NOWLAN );        
        }
    
    CleanupStack::PushL( items );

    TInt currentItem = iModel->DataUsageAbroad();
    TInt oldItem = currentItem;

    CAknSettingPage* dlg = new( ELeave ) CAknRadioButtonSettingPage(
            R_DATA_USAGE_ABROAD_SETTING_PAGE, currentItem, items );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {

        if( currentItem != oldItem )
            {
            iModel->SetDataUsageAbroad( currentItem );
            UpdateListBoxL( EGSSettIdDataUsageAbroad );
            //Save current settings when the setting is changed
            //If function leaves it is trapped and ignored as there is nothing that we can do about it
            TRAP_IGNORE(iModel->SaveSettingsL());
            }
        }
    CleanupStack::PopAndDestroy( items );
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::ShowDataUsageInHomeNwSettingPageL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::ShowDataUsageInHomeNwSettingPageL()
    {
    CDesCArrayFlat* items;
    
    if ( iIsWlanSupported )
        {
        items = iCoeEnv->ReadDesC16ArrayResourceL(
            R_DATA_USAGE_HOME_NW_SETTING_PAGE_LBX );
        }
    else
        {
        items = iCoeEnv->ReadDesC16ArrayResourceL(
            R_DATA_USAGE_HOME_NW_SETTING_PAGE_LBX_NOWLAN );        
        }
    
    CleanupStack::PushL( items );

    TInt currentItem = iModel->DataUsageInHomeNw();
    TInt oldItem = currentItem;

    CAknSettingPage* dlg = new( ELeave ) CAknRadioButtonSettingPage(
            R_DATA_USAGE_HOME_NW_SETTING_PAGE, currentItem, items );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {

        if( currentItem != oldItem )
            {
            iModel->SetDataUsageInHomeNw( currentItem );
            UpdateListBoxL(  EGSSettIdDataUsageHomeNw );
            //Save current settings when the setting is changed
            //If function leaves it is trapped and ignored as there is nothing that we can do about it
            TRAP_IGNORE(iModel->SaveSettingsL());
            }
        }
    CleanupStack::PopAndDestroy( items );
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::CheckMiddleSoftkeyLabelL
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::CheckMiddleSoftkeyLabelL() 
    {
    //Resolve current item
    const TInt currentItem = Container()->CurrentFeatureId();

    //remove current middle soft key label
    RemoveCommandFromMSK();

    switch (currentItem)
        {
        case EGSSettIdUsageOfWlan:
        case EGSSettIdDataUsageAbroad:
        case EGSSettIdDataUsageHomeNw:
            //Set middle soft key label to "Change"
            SetMiddleSoftKeyLabelL( R_GS_CONN_SETTINGS_MSK_LABEL_CHANGE, EGSConnSettMskGeneral );
        break;
        
        default:
            {
            //If current item is ECOM plugin set middle soft key label to "Open"
            SetMiddleSoftKeyLabelL( R_GS_CONN_SETTINGS_MSK_LABEL_OPEN, EGSConnSettMskGeneral );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CGSConnSettingsPlugin::HandleListBoxEventL
//
// -----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
    }

// -----------------------------------------------------------------------------
// CGSConnSettingsPlugin::RemoveCommandFromMSK
//
// -----------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::RemoveCommandFromMSK()
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        // Remove command from stack
        cbaGroup->RemoveCommandFromStack( KGSConnSettingsMSKControlID, EGSConnSettMskGeneral );
        }
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::SetMiddleSoftKeyLabelL 
// Sets middle softkey label.
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::SetMiddleSoftKeyLabelL( 
    const TInt aResourceId, const TInt aCommandId )
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
        TPtr mskPtr = middleSKText->Des();
        cbaGroup->AddCommandToStackL( 
                KGSConnSettingsMSKControlID, 
            aCommandId, 
            mskPtr );
        CleanupStack::Pop( middleSKText );
        delete middleSKText;
        }
    }

// ---------------------------------------------------------------------------
// CGSConnSettingsPlugin::HandlePluginLoaded
// Called by GSPluginLoader when plugins are loaded or error occured during that
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPlugin::HandlePluginLoaded( KGSPluginLoaderStatus aStatus )
    {
    if ( iContainer && MGSPluginLoadObserver::EGSFinished == aStatus )
        {
        #ifdef __DEBUG
        TRAPD( ignore, Container()->UpdateListBoxL( EGSExtPluginsListItemId ); );
        TRACE_1( "[GSSensorPlugin] CGSSensorPlugin::HandlePluginLoaded() - Error: %i", ignore );
        #else
        TRAP_IGNORE( Container()->UpdateListBoxL( EGSExtPluginsListItemId ); );
        #endif // __DEBUG
        }
    }

// End of File
