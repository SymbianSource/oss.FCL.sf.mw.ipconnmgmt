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
* Description:  Display Settings model.
*
*/

#ifndef GSCONNSETTINGSPLUGINMODEL_H
#define GSCONNSETTINGSPLUGINMODEL_H

#include <e32base.h>
#include <cmgenconnsettings.h>

class CGSConnSettingsPluginContainer;

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
*   CGSConnSettingsPluginModel is the model class of Connectivity Settings plugin.
*  It provides functions to get and set setting values.
*  @lib gsconnsettingsplugin.lib
*  @since Series 60_3.1

*/
NONSHARABLE_CLASS( CGSConnSettingsPluginModel ): public CBase
    {
    public:  // Constructor and destructor
        /**
        * Two-phased constructor
        */
        static CGSConnSettingsPluginModel* NewL();

        /**
        * Destructor
        */
        ~CGSConnSettingsPluginModel();

    public:
        /**
        * Loads settings by using CMManager's general connection settings API
        */
        void LoadSettingsL();

        /**
        * Saves settings by using CMManager's general connection settings API
        */
        void SaveSettingsL();

        /**
        * Gets and maps value from general connection settings API to match index in UI
        * @return TInt
        */ 
        TInt UsageOfWlan();
        
        /**
        * Gets and maps value from general connection settings API to match index in UI
        * @return TInt
        */ 
        TInt DataUsageAbroad();

        /**
        * Gets and maps value from general connection settings API to match index in UI
         @param aHomeOperatorSettingSupported ETrue if dialog contains 'Home network only'
        *                                     setting, EFalse otherwise.
        * @return TInt
        */ 
        TInt DataUsageInHomeNw( const TBool aHomeOperatorSettingSupported );

        /**
        * Maps UI index to according general connection setting API value
        * @param aValue index to be mapped to API value
        */
        void SetUsageOfWlan( TInt aValue );
        
        /**
        * Maps UI index to according general connection setting API value
        * @param aValue index to be mapped to API value
        */
        void SetDataUsageAbroad( TInt aValue );

        /**
        * Maps UI index to according general connection setting API value
        * @param aValue index to be mapped to API value
        * @param aHomeOperatorSettingSupported ETrue if dialog contains 'Home network only'
        *                                      setting, EFalse otherwise.   
        */
        void SetDataUsageInHomeNw( TInt aValue, const TBool aHomeOperatorSettingSupported );
        
        /**
         * @param
         */
        void SetOwner( CGSConnSettingsPluginContainer* aPlugin);

    private: // Private constructors

        /**
        * Default C++ contructor
        */
        CGSConnSettingsPluginModel();

        /**
        * Symbian OS default constructor
        * @return void
        */
        void ConstructL();

    private:
        /**
        * Pointer to container class
        */
        CGSConnSettingsPluginContainer* iContainer;

        /**
        * Struct to hold current settings
        */
        TCmGenConnSettings iSettings;
        
        /**
         * Flag is set when wlan is supported
         */
        TBool iIsWlanSupported;
        
    };

#endif //GSCONNSETTINGSPLUGINMODEL_H

