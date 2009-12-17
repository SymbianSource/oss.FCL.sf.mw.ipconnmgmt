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
* Description:  Container for the Connectivity Settings plugin
*
*/

// INCLUDE FILES
#include <featmgr.h>
#include "gsconnsettingsplugincontainer.h"
#include "gsconnsettingspluginmodel.h"
#include "gsconnsettingsplugin.h"
#include "gsconnsettingsplugin.hrh"
#include "gsconnsettingsmskobserver.h"
#include <gsconnsettingspluginrsc.rsg>

#include <aknlists.h>
#include <csxhelp/cp.hlp.hrh>
#include <GSFWViewUIDs.h>     // for KUidGS
#include <GSListbox.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const   TInt    KGSBufSize128 = 128;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================== MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSDisplayPluginContainer::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new ( ELeave ) CAknSettingStyleListBox;
    iModel = CGSConnSettingsPluginModel::NewL();
    iModel->SetOwner( this );
    
    //Load saved settings from Connection Settings API
    //If function leaves it is trapped and ignored as there is nothing that we can do about it
    TRAP_IGNORE(iModel->LoadSettingsL());

    if ( iIsWlanSupported )
        {
        BaseConstructL( aRect, R_GS_CONN_SETTINGS_PLUGIN_VIEW_TITLE, R_CONN_SETTINGS_LBX );
        }
    else
        {
        BaseConstructL( aRect, R_GS_CONN_SETTINGS_PLUGIN_VIEW_TITLE, R_CONN_SETTINGS_LBX_NOWLAN );
        }
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::~GSConnSettingsPluginContainer()
//
// Destructor
// ---------------------------------------------------------------------------
//
CGSConnSettingsPluginContainer::~CGSConnSettingsPluginContainer()
    {    
    //Note: iListbox is not deleted here as base class destructor handels it
    
    if ( iModel )
        {        
        delete iModel;
        iModel = NULL;
        }

    if ( iListboxItemArray )
        {
        delete iListboxItemArray;
        }
    
    if ( iListBoxTextArray )
       {
       iListBoxTextArray->Reset();
       delete iListBoxTextArray;
       }
    
    if ( iIsWlanSupported )
        {
        if ( iUsageOfWlanItems )
            {
            iUsageOfWlanItems->Reset();
            delete iUsageOfWlanItems;
            }
        }

    if ( iDataUsageAbroadItems )
        {
        iDataUsageAbroadItems->Reset();
        delete iDataUsageAbroadItems;
        }

    if ( iDataUsageHomeNwItems )
        {
        iDataUsageHomeNwItems->Reset();
        delete iDataUsageHomeNwItems;
        }    

    if ( iListItems )
        {
        iListItems->Reset();
        delete iListItems;
        }
    }


// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::GSConnSettingsPluginContainer()
//
// Constructor
// ---------------------------------------------------------------------------
//
CGSConnSettingsPluginContainer::CGSConnSettingsPluginContainer( 
        MGSConnSettingsMskObserver& aMskObserver )
        : iMskObserver( aMskObserver )
    {
    iIsWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    }

void CGSConnSettingsPluginContainer::SetPluginArray ( 
                                CArrayPtrFlat<CGSPluginInterface>* aPluginArray )
{
    iPluginArray = aPluginArray;
}


// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::ConstructListBoxL()
// 
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::ConstructListBoxL( TInt aResLbxId )
    {
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListboxItemArray = CGSListBoxItemTextArray::NewL( aResLbxId, *iListBox, *iCoeEnv );
    iListBoxTextArray = static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    if ( iIsWlanSupported )
        {
        iUsageOfWlanItems = iCoeEnv->ReadDesC16ArrayResourceL(
                R_USAGE_OF_WLAN_SETTING_PAGE_LBX );

        iDataUsageHomeNwItems = iCoeEnv->ReadDesC16ArrayResourceL( 
                R_DATA_USAGE_HOME_NW_SETTING_PAGE_LBX );
    
        iDataUsageAbroadItems = iCoeEnv->ReadDesC16ArrayResourceL( 
                R_DATA_USAGE_ABROAD_SETTING_PAGE_LBX );
        
        iListItems = iCoeEnv->ReadDesC16ArrayResourceL( R_CONN_SETTINGS_LBX_TXT );
        }
    else
        {
        iDataUsageHomeNwItems = iCoeEnv->ReadDesC16ArrayResourceL( 
                R_DATA_USAGE_HOME_NW_SETTING_PAGE_LBX_NOWLAN );
    
        iDataUsageAbroadItems = iCoeEnv->ReadDesC16ArrayResourceL( 
                R_DATA_USAGE_ABROAD_SETTING_PAGE_LBX_NOWLAN );
        
        iListItems = iCoeEnv->ReadDesC16ArrayResourceL( R_CONN_SETTINGS_LBX_TXT_NOWLAN );
        }
        
    CreateListBoxItemsL();
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::CreateListBoxItemsL()
//
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::CreateListBoxItemsL()
    {   
    if ( iIsWlanSupported )
        {
        MakeUsageOfWlanNetworksItemL();
        iGeneralItemCount++;
        }

    MakeDataUsageHomeNwItemL();
    iGeneralItemCount++;

    MakeDataUsageAbroadItemL();
    iGeneralItemCount++;

    if ( iPluginArray )
        {
        MakeECOMPluginItemL();
        }
    iListBox->HandleItemAdditionL();
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::UpdateListBoxL(TInt aFeatureId)
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::UpdateListBoxL( TInt aFeatureId )
    {
    switch ( aFeatureId )
        {
        case  EGSSettIdUsageOfWlan:
            iListBoxTextArray->Delete( EGSSettIdUsageOfWlan );
            MakeUsageOfWlanNetworksItemL();
            break;

        case EGSSettIdDataUsageHomeNw:
            iListBoxTextArray->Delete( Index( EGSSettIdDataUsageHomeNw ) );
            MakeDataUsageHomeNwItemL();
            break;
            
        case EGSSettIdDataUsageAbroad:
            iListBoxTextArray->Delete( Index ( EGSSettIdDataUsageAbroad ) );
            MakeDataUsageAbroadItemL();
            break;

        case EGSExtPluginsListItemId:
            MakeECOMPluginItemL();
            break;     
                        
        default:
            break;
        }
    iListBox->HandleItemAdditionL();
    }


// ---------------------------------------------------------------------------
// CGSConnSettingsPluginContainer::AppendListItem
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::AppendListItemL( 
    const TInt aPos,
    const TDesC16& aFirstLabel, 
    const TDesC16& aSecondLabel )
    {
    TBuf<KGSBufSize128> itemText;
    _LIT( KSpaceAndTab, " \t" );
    _LIT( KTab, "\t" );

    // The format is: " \tFirstLabel\tSecondLabel\t0\t0"
    // First, append space and tab
    itemText.Append( KSpaceAndTab );
    itemText.Append( aFirstLabel );
    itemText.Append( KTab );
    itemText.Append( KTab );
    // Currently this is used only to show sensors active status (on/off)
    itemText.Append( aSecondLabel ); 

    // No need to add last tabs
    if ( ( KErrNotFound != aPos ) && ( aPos <= iListBoxTextArray->Count() ) )
        {
        iListBoxTextArray->InsertL( aPos, itemText );
        }
    else
        {
        iListBoxTextArray->AppendL( itemText );
        }
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::MakeUsageOfNewWlanNetworksItemL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::MakeUsageOfWlanNetworksItemL()
    {
    TInt currValue = iModel->UsageOfWlan();
    AppendListItemL( EGSSettIdUsageOfWlan, 
        iListItems->operator[]( EGSSettIdUsageOfWlan ), 
        ( *iUsageOfWlanItems )[currValue] );
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::MakeDataUsageAbroadItemL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::MakeDataUsageAbroadItemL()
{
    TInt currValue = iModel->DataUsageAbroad();
    
    AppendListItemL( Index( EGSSettIdDataUsageAbroad ),
        iListItems->operator[]( Index( EGSSettIdDataUsageAbroad ) ),
        ( *iDataUsageAbroadItems )[currValue] );
}
            
// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::MakeDataUsageAbroadItemL()
//
// 
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::MakeDataUsageHomeNwItemL()
{
    TInt currValue = iModel->DataUsageInHomeNw();
    
    AppendListItemL( Index( EGSSettIdDataUsageHomeNw ),
        iListItems->operator[]( Index( EGSSettIdDataUsageHomeNw ) ),
        ( *iDataUsageHomeNwItems )[currValue] );
}


// ---------------------------------------------------------------------------
// CGSConnSettingsPluginContainer::MakeExtendedPluginsItemL
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::MakeECOMPluginItemL()
    {

    // Create list item for all child plug-ins
    const TInt pluginCount( iPluginArray->Count() );
    CGSPluginInterface* plugin = NULL;

    // 1) Read item descriptor from plugin:
    TBuf<KGSBufSize128> caption;

    for ( TInt i = 0; i < pluginCount; i++ )
        {
        plugin = iPluginArray->operator[]( i );
        //Append to list only if plugin is meant to be visible
        if( ( plugin ) && ( plugin->Visible() ) )
            {
            plugin->GetCaptionL( caption );

            // Append text to list
            AppendListItemL( KErrNotFound, caption, KNullDesC16 );
            }
        }        
    }
    
 
// -----------------------------------------------------------------------------
// CGSConnSettingsPluginContainer::SelectedPlugin
// Returns pointer to selected plugin
// -----------------------------------------------------------------------------
//
CGSPluginInterface* CGSConnSettingsPluginContainer::SelectedPluginL()
    {
    CGSPluginInterface* plugin = NULL;

    // 1) Decrease index with the count
    TInt index = iListBox->CurrentItemIndex() - iGeneralItemCount;
    
    // 2) Check that new index is in range of plugins
    if ( index >= 0 && index < iPluginArray->Count() )
        {
        // 3) Get correct plugin
        plugin = iPluginArray->operator[]( index );
        }

    // Leave if not found
    if ( !plugin )
        {
        User::Leave( KErrNotFound );
        }

    return plugin;
    }



// ---------------------------------------------------------------------------
// CGSConnSettingsPluginContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGSConnSettingsPluginContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {

    TKeyResponse returnValue( EKeyWasNotConsumed );

    switch ( aKeyEvent.iCode )
        {
        case EKeyUpArrow:
        case EKeyDownArrow:
            {
            // Send key event to Listbox
            returnValue = iListBox->OfferKeyEventL( aKeyEvent, aType );
            // Change MSK label if needs to changed
            iMskObserver.CheckMiddleSoftkeyLabelL();
            break;
            }
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            // Listbox takes all events even if it doesn't use them.
            // Stick with the default return value
            break;
            }
        default:
            {
            // Forward key event to listbox
            returnValue = iListBox->OfferKeyEventL( aKeyEvent, aType );
            break;
            }
        }

    return returnValue;
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::CurrentFeatureId()
//
// ---------------------------------------------------------------------------
//
TInt CGSConnSettingsPluginContainer::CurrentFeatureId( ) const
    {
    return iListboxItemArray->CurrentFeature( );
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::GetHelpContext() const
// Gets Help
//
// ---------------------------------------------------------------------------
//
void CGSConnSettingsPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidGS;
//    aContext.iContext = KCP_HLP_DISPLAY;
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::Model()
//
// Return its member variable iModel.
// ---------------------------------------------------------------------------
//
CGSConnSettingsPluginModel* CGSConnSettingsPluginContainer::Model()
    {
    return iModel;
    }

// ---------------------------------------------------------------------------
// GSConnSettingsPluginContainer::Index()
//
// Return index for the list.
// ---------------------------------------------------------------------------
//
TInt CGSConnSettingsPluginContainer::Index( TInt aIndex )
    {
    return ( iIsWlanSupported ? ( aIndex ) : ( aIndex - 1) );
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
