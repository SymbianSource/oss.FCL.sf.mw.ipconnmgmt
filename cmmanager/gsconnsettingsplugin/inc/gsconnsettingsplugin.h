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
* Description:    Header file for Connectivity Settings plugin.
*
*/


#ifndef GSCONNSETTINGSPLUGIN_H
#define GSCONNSETTINGSPLUGIN_H

// Includes
#include <GSPluginLoader.h>
#include <GSBaseView.h>
#include "gsconnsettingsmskobserver.h"

#include <aknview.h>
#include <eikclb.h>

// Classes referenced
class CAknViewAppUi;
class RConeResourceLoader;
class CGSConnSettingsPluginModel;
class CGSConnSettingsPluginContainer;

// Constants

//Should we put these to .hrh 
// This UID is used for both the view UID 
// and the ECOM plugin implementation ID.
const TUid KGSConnSettingsPluginUid = { 0x200255B9 };
const int KGConnSettingsPluginCount = 10;

_LIT( KGSConnSettingsPluginResourceFileName, "z:gsconnsettingspluginrsc.rsc" );
_LIT( KGSConnSettingsPluginIconDirAndName, "z:gsconnsettingsplugin.mbm"); // Use KDC_BITMAP_DIR

// CLASS DECLARATION

/**
* CConnSettingsPlugin view class.
*
* @since Series60_3.1
*/
class CGSConnSettingsPlugin : public CGSBaseView, public MGSPluginLoadObserver, public MGSConnSettingsMskObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return
        */
        static CGSConnSettingsPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor.
        */
        ~CGSConnSettingsPlugin();

// From CAknView

        /**
        * See base class.
        */
        TUid Id() const;
        
        /**
        * Handles commands.
        * @param aCommand Command to be handled.
        * 
        */
        void HandleCommandL( TInt aCommand );
        

        /**
        * See base class.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
        * See base class.
        */
        void DoDeactivate();
        
        /**
        * See base class.
        */
        void HandleForegroundEventL( TBool aForeground );  

        
        /**
        * From MEikMenuObserver.
        * Changes MenuPane dynamically.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
// From CGSParentPlugin

        /**
        * See base class.
        */
        TUid UpperLevelViewUid();

        
        /**
        * See base class.
        */
        void GetHelpContext( TCoeHelpContext& aContext );

// From CGSPluginInterface

        /**
        * See base class.
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * See base class.
        */
        CGulIcon* CreateIconL( const TUid aIconType );

        /**
        * See base class.
        */
        TInt PluginProviderCategory() const;

        /**
        * @see MGSPluginLoadObserver header file.
        */
        void HandlePluginLoaded( KGSPluginLoaderStatus aStatus );

        
        /**
        * Updates listbox's item's value.
        * @param aItemId An item which is updated.
        * 
        */
        void UpdateListBoxL( TInt aItemId );

        /**
        * Get CGSConnSettingsPlugin's ccontainer.
        */
        CGSConnSettingsPluginContainer* Container();
                
        /**
         * From MGSSensorMskObserver.
         * Checks currently activated item in list and updates MSK label if needed
         */
        void CheckMiddleSoftkeyLabelL();
        
        /**
         * From MEikListBoxObserver (through CGSBaseView)
         */
        void HandleListBoxEventL( CEikListBox* aListBox,
                    TListBoxEvent aEventType );
        
        
    protected: // New
        /**
        * C++ default constructor.
        */
        CGSConnSettingsPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
        
    private:

        void NewContainerL();
        
        /**
         * From CGSBaseView. Handles list box selection
         */
        void HandleListBoxSelectionL();

        /**
        * Show "Join new WLAN networks" setting page
        */
        void ShowUsageOfWlanSettingPageL();
        
        /**
        * Show "Cellular data usage abroad" setting page
        */
        void ShowDataUsageAbroadSettingPageL();
        
        /**
        * Show "Cellular data usage in home network" setting page
        */
        void ShowDataUsageInHomeNwSettingPageL();
        
        /**
         * Removes current label from MSK
         */
        void RemoveCommandFromMSK();

        /**
         * Adds new label for MSK
         *
         * @param aResourceId Text of the new label
         * @param aCommandId Command id of the MSK
         */
        void SetMiddleSoftKeyLabelL( const TInt aResourceId, const TInt aCommandId );       
        
        
    private: //Private data

        /**
         * Resource loader
         */
        RConeResourceLoader iResourceLoader;

        /**
         * Pointer to model.
         * Owned by CGSConnSettingsPluginContainer
         */
        CGSConnSettingsPluginModel *iModel;

        /**
         * Asynchronous loader for the GS plug-ins.
         */
        CGSPluginLoader* iPluginLoader;
        
        /**
         * Array of the child plugins
         */
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;

        /**
         * Flag is set when wlan is supported
         */
        TBool iIsWlanSupported;
    };


#endif // GSCONNSETTINGSPLUGIN_H
// End of File
