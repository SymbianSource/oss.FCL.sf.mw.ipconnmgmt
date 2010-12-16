/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connectivity Settings Plugin model implementation.
*
*/


// INCLUDE FILES

#include <featmgr.h>
#include "gsconnsettingsplugincontainer.h"
#include "gsconnsettingspluginmodel.h"
#include "cmmanager.h"
#include "gsconnsettingsplugin.hrh"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsconnsettingspluginmodelTraces.h"
#endif


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const   TInt    KSecondRowIndex = 1;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ==============================

// ========================= MEMBER FUNCTIONS =================================


// ----------------------------------------------------------------------------
// CGSDisplayPluginModel::NewL
//
// Symbian OS two-phased constructor
// ----------------------------------------------------------------------------
//
CGSConnSettingsPluginModel* CGSConnSettingsPluginModel::NewL()
    {
    CGSConnSettingsPluginModel* self = new( ELeave ) CGSConnSettingsPluginModel;
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::CGSConnSettingsPluginModel
//
//
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CGSConnSettingsPluginModel::CGSConnSettingsPluginModel()
    {
    }


// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::ConstructL
//
// EPOC default constructor can leave.
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iIsWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    }


// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::~CGSConnSettingsPluginModel
//
// Destructor
// ----------------------------------------------------------------------------
//
CGSConnSettingsPluginModel::~CGSConnSettingsPluginModel()
    {
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::LoadSettings
//
// Loads settings by using CMManager's general connection settings API
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::LoadSettingsL()
    {
    RCmManager CmManager;
    CmManager.CreateTablesAndOpenL();
    CleanupClosePushL(CmManager); 
    CmManager.ReadGenConnSettingsL( iSettings );
    CleanupStack::PopAndDestroy( 1 );     //CmManager

    //Print out loaded settings
    OstTrace1( TRACE_NORMAL, CSP_LOADSETTINGSL_USAGE_OF_NEW_WLAN_PARAM_TRACE, "iSettings.iUsageOfWlan=%d", iSettings.iUsageOfWlan );
    OstTrace1( TRACE_NORMAL, CSP_LOADSETTINGSL_CELLULAR_DATA_USAGE_VISITOR_PARAM_TRACE, "iSettings.iCellularDataUsageVisitor=%d", iSettings.iCellularDataUsageVisitor );
    OstTrace1( TRACE_NORMAL, CSP_LOADSETTINGSL_CELLULAR_DATA_USAGE_HOME_PARAM_TRACE, "iSettings.iCellularDataUsageHome=%d", iSettings.iCellularDataUsageHome );
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::SaveSettingsL
//
// Saves settings by using CMManager's general connection settings API
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::SaveSettingsL()
    {
    RCmManager CmManager;
    CmManager.CreateTablesAndOpenL();
    CleanupClosePushL(CmManager); 
    CmManager.WriteGenConnSettingsL( iSettings );
    CleanupStack::PopAndDestroy( 1 );     //CmManager

    //Print out saved settings
    OstTrace1( TRACE_NORMAL, CSP_SAVESETTINGSL_USAGE_OF_NEW_WLAN_PARAM_TRACE, "iSettings.iUsageOfWlan=%d", iSettings.iUsageOfWlan );
    OstTrace1( TRACE_NORMAL, CSP_SAVESETTINGSL_CELLULAR_DATA_USAGE_VISITOR_PARAM_TRACE, "iSettings.iCellularDataUsageVisitor=%d", iSettings.iCellularDataUsageVisitor );
    OstTrace1( TRACE_NORMAL, CSP_SAVESETTINGSL_CELLULAR_DATA_USAGE_HOME_PARAM_TRACE, "iSettings.iCellularDataUsageHome=%d", iSettings.iCellularDataUsageHome );
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::UsageOfWlan
//
// Maps value from general connection settings API to match index in UI
// ----------------------------------------------------------------------------
//
TInt CGSConnSettingsPluginModel::UsageOfWlan()
    {
    TInt mappedValue = 0;
    
    //Map to correct index    
    switch ( iSettings.iUsageOfWlan )
        {
        case ECmUsageOfWlanKnown: 
            mappedValue = EUsageOfWlanKnown; //Known
            break;
            
        case ECmUsageOfWlanManual:
            mappedValue = EUsageOfNewWlanManual; //Manual
            break;
                        
        default:
            break;
        }
    return mappedValue;
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::DataUsageAbroad
//
// Maps value from general connection settings API to match index in UI
// ----------------------------------------------------------------------------
//
TInt CGSConnSettingsPluginModel::DataUsageAbroad()
    {   
    TInt mappedValue = 0;

    //Map to correct index    
    switch ( iSettings.iCellularDataUsageVisitor )
        {
        case ECmCellularDataUsageAutomatic:
            mappedValue = EDataUsageAbroadAutomatic; // Automatic
            break;

        case ECmCellularDataUsageConfirm:
            mappedValue = EDataUsageAbroadConfirm; // Confirm
            break;

        case ECmCellularDataUsageDisabled:
            if( iIsWlanSupported )
                {
                mappedValue = EDataUsageAbroadWlanOnly; // Wlan only
                }
            else
                {
                mappedValue = EDataUsageAbroadDisabled; // Disabled when nowlan
                }
            break;

        default:
            break;
        }
    return mappedValue;
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::DataUsageInHomeNw
//
// Maps value from general connection settings API to match index in UI
// ----------------------------------------------------------------------------
//
TInt CGSConnSettingsPluginModel::DataUsageInHomeNw( const TBool aHomeOperatorSettingSupported )
    {     
    TInt mappedValue = 0;

    //Map to correct index    
    switch ( iSettings.iCellularDataUsageHome )
        {
        case ECmCellularDataUsageAutomatic:
            mappedValue = EDataUsageHomeNwAutomatic; // Automatic
            break;

        case ECmCellularDataUsageConfirm:
            mappedValue = EDataUsageHomeNwConfirm; // Confirm
            break;

        case ECmCellularDataUsageDisabled:
            if( iIsWlanSupported )
                {
                mappedValue = EDataUsageHomeNwWlanOnly; // Wlan only
                }
            else
                {
                mappedValue = EDataUsageHomeNwDisabled; // Disabled when nowlan
                }
            break;

       case ECmCellularDataUsageAutomaticInHomeNetwork: //Automatic in home operator network only
            mappedValue = EDataUsageHomeNwOperator;
            break;

        default:
            break;
        }
        
    // Home network only -entry will be displayed on the second row.
    // Adjust listbox indexes accordingly.
    if ( aHomeOperatorSettingSupported )
        {
        if ( mappedValue == EDataUsageHomeNwOperator )
            {
            mappedValue = KSecondRowIndex;
            }
        else if ( mappedValue > EDataUsageHomeNwAutomatic )
            {
            mappedValue++;
            }
        }
   
    return mappedValue;
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::SetUsageOfNewWlanL
//
// Maps UI index to according general connection setting API value
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::SetUsageOfWlan( TInt aValue )
    {    
    //Map to correct index
    switch ( aValue )
        {        
        case EUsageOfWlanKnown: //Join and inform
            iSettings.iUsageOfWlan = ECmUsageOfWlanKnown;
            break;
            
        case EUsageOfNewWlanManual: //inform
            iSettings.iUsageOfWlan = ECmUsageOfWlanManual;
            break;
                        
        default:
            break;
        }   
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::SetDataUsageAbroadL
//
// Maps UI index to according general connection setting API value
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::SetDataUsageAbroad( TInt aValue )
    {
    switch ( aValue )
        {
        case EDataUsageAbroadAutomatic:  //Automatic
            iSettings.iCellularDataUsageVisitor = ECmCellularDataUsageAutomatic; 
            break;
        case EDataUsageAbroadConfirm: //Confirm
            iSettings.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;
            break;

        case EDataUsageAbroadWlanOnly: //Wlan only
        case EDataUsageAbroadDisabled:
            iSettings.iCellularDataUsageVisitor = ECmCellularDataUsageDisabled;
            break;
                        
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::SetDataUsageInHomeNwL
//
// Maps UI index to according general connection setting API value
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::SetDataUsageInHomeNw( TInt aValue, const TBool aHomeOperatorSettingSupported )
    {
    // Home network only -entry is displayed on the second row.
    // Adjust aValue accordingly.
    if ( aHomeOperatorSettingSupported )
        {
        if ( aValue == KSecondRowIndex )
            {
            aValue = EDataUsageHomeNwOperator;
            }
        else if ( aValue > EDataUsageHomeNwAutomatic )
            {
    	      aValue--;
            }
        }
    	        
    switch ( aValue )
        {
        case EDataUsageHomeNwAutomatic: //Automatic
            iSettings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic; 
            break;

        case EDataUsageHomeNwConfirm: //Confirm
            iSettings.iCellularDataUsageHome = ECmCellularDataUsageConfirm;
            break;

        case EDataUsageHomeNwWlanOnly: //Wlan only
        case EDataUsageHomeNwDisabled:
            iSettings.iCellularDataUsageHome = ECmCellularDataUsageDisabled;
            break;

        case EDataUsageHomeNwOperator: //Automatic in home operator network only
            iSettings.iCellularDataUsageHome = ECmCellularDataUsageAutomaticInHomeNetwork;
            break;
           
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CGSConnSettingsPluginModel::SetOwner
//
// Sets its owner.
// ----------------------------------------------------------------------------
//
void CGSConnSettingsPluginModel::SetOwner( CGSConnSettingsPluginContainer* aContainer)
    {
    iContainer = aContainer;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
