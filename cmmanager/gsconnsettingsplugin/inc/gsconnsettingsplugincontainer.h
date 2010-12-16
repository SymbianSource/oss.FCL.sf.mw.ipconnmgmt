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
* Description:  Container for the Connectivity Settings Plugin
*
*/


#ifndef GSCONNSETTINGSPLUGINCONTAINER_H
#define GSCONNSETTINGSPLUGINCONTAINER_H

// INCLUDES
#include "gsconnsettingspluginmodel.h"
#include <gsbasecontainer.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CGSListBoxItemTextArray;
class CGSPluginInterface;
class MGSConnSettingsMskObserver;

// CLASS DECLARATION

/**
*  GSConnSettingsPluginContainer container class
*
*  Container class for Connectivity Settings plugin view
*  @lib gsconnsettingsplugin.lib
*  @since Series 60_3.1
*/
class CGSConnSettingsPluginContainer : public CGSBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS constructor.
        * @param aRect Listbox's rect.
        * 
        */
        void ConstructL( const TRect& aRect );
        
        /**
        * Destructor.
        */
        ~CGSConnSettingsPluginContainer();
        
        /**
        * Constructor
        */
        CGSConnSettingsPluginContainer( MGSConnSettingsMskObserver& aMskObserver, TBool aHomeOperatorSettingSupported );

    public: //new

        /**
        * Updates listbox's item's value.
        * @since Series 60_3.1
        * @param aItemId An item which is updated.
        */
        void UpdateListBoxL( TInt aFeatureId );
        
        /**
        * Retrieves the currently selected listbox feature id
        * @since Series 60_3.1
        * @return feature id.
        */
        TInt CurrentFeatureId() const;
        
        /**
         * Return its member variable iMode
         * @param
         */
        CGSConnSettingsPluginModel* Model();
        
        /**
         * Finds user selected plugin from plugin array
         *
         * @return Plugin which is selected in the lbx or leaves 
         * with KErrnotFound if no plugins exist.
         */
        CGSPluginInterface* SelectedPluginL();

        /**
         * Makes ECOM plugin item
         */
        void MakeECOMPluginItemL();

        /**
        * Sets iPluginArray member.
        * @param aPluginArray Pointer to loaded ECOM plugin array
        */
        void SetPluginArray ( CArrayPtrFlat<CGSPluginInterface>* aPluginArray );
        
        
    protected: // from CGSBaseContainer
        void ConstructListBoxL( TInt aResLbxId );
        
    private: // new
        // create listbox from resource
        void CreateListBoxItemsL();

        /**
         * Adds items from iUsageOfWlanItems to main views list
         */
        void MakeUsageOfWlanNetworksItemL();
                
        /**
         * Adds items from iDataUsageAbroadItems to main views list
         */
        void MakeDataUsageAbroadItemL();
        
        /**
         * Adds items from iDataUsageHomeNwItems to main views list
         */
        void MakeDataUsageHomeNwItemL();
        
        /**
         * From CGSBaseContainer. Constructs correct textformat for one list box item 
         * and adds it in listbox
         *
         * @param aPos Position of text in listbox
         * @param aFirstLabel Text in first row of listbox item
         * @param aSecondLabel Text in second row of listbox item
         */
        void AppendListItemL( const TInt aPos, 
                              const TDesC16& aFirstLabel, 
                              const TDesC16& aSecondLabel );
               
        /**
         * Handles changes in MSK label between item changes in listbox
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
                        
        /**
         * Return list index according to wlan variation.
         */
        TInt Index( TInt aIndex );

        // from CGSBaseContainer
        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    private: // Data
        
        /**
        * This holds items from resource R_USAGE_OF_WLAN_SETTING_PAGE_LBX
        */
        CDesCArrayFlat* iUsageOfWlanItems;
                
        /**
        * This holds items from resource R_DATA_USAGE_ABROAD_SETTING_PAGE_LBX
        */
        CDesCArrayFlat* iDataUsageAbroadItems;
        
        /**
        * This holds items from resource R_DATA_USAGE_HOME_NW_SETTING_PAGE_LBX
        */
        CDesCArrayFlat* iDataUsageHomeNwItems;
        
        /**
         * Listbox item array.
         */
        CGSListBoxItemTextArray* iListboxItemArray;
        
        /**
         * Listbox item text array. Contains listbox item texts
         */
        CDesCArray* iListBoxTextArray;
               
        /** 
         * General list box items. Contains localized listbox texts
         * from R_CONN_SETTINGS_LBX_TXT resource .
         */
        CDesCArrayFlat* iListItems;
        
        // plugin model
        CGSConnSettingsPluginModel* iModel;        

        /**
         * Array of the child plugins. Owned by CGSConnSettingsPlugin.
         */
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;
        
        /**
         * General item count. Used when calculating item indexes to ECOM plugins
         */
        TInt iGeneralItemCount;
        
        /**
         * Reference to main view. Does not own
         */
        MGSConnSettingsMskObserver& iMskObserver;

        /**
         * Flag is set when wlan is supported
         */
        TBool iIsWlanSupported;

        /**
         * Flag is set when additional item 'Home network only'
         * is added to Data usage in home country -dialog.
         */
        TBool iHomeOperatorSettingSupported;
    };

#endif //GSCONNSETTINGSPLUGINCONTAINER_H

//End of File
