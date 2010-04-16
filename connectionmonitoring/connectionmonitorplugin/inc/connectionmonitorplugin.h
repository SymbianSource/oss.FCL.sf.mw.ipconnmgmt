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
* Description:  ConnectionMonitorPlugin implementation.
*
*/

#ifndef CONNECTIONMONITORPLUGIN_H
#define CONNECTIONMONITORPLUGIN_H

// User includes

// System includes
#include <gsplugininterface.h>
#include <aknview.h>
#include <ConeResLoader.h>
#include <eikclb.h>
#include <AknServerApp.h>

// Classes referenced
class CAknNavigationDecorator;
class CAknViewAppUi;
class CAknNullService;

// Constants
const TUid KConnectionMonitorPluginUID = { 0x10281BC2 };

const TUid KConnectionMonitorAppUid    = { 0x101F84D0 };

_LIT( KConnectionMonitorPluginResourceFileName,
                                    "z:connectionmonitorpluginrsc.rsc" );
_LIT( KConnectionMonitorPluginIconDirAndName,
                                    "z:connectionmonitorplugin.mif ");
                                                    // Use KDC_BITMAP_DIR

// CLASS DECLARATION

/**
* CConnectionMonitorPlugin.
*
*/
class CConnectionMonitorPlugin : 
                            public CGSPluginInterface,
                            public MAknServerAppExitObserver // Embedding
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @param aInitParams initialization parameters.
        * @return new instance of CConnectionMonitorPlugin.
        */
        static CConnectionMonitorPlugin* NewL( TAny* aInitParams );

        /**
        * Destructor.
        */
        ~CConnectionMonitorPlugin();

    public: // From CAknView

        /**
        * From CAknView.
        * This view's Id.
        * @return This plugin's UID.
        */
        TUid Id() const;

    public: // From CGSPluginInterface

        /**
        * From CGSPluginInterface.
        * @param aCaption this plugin's caption.
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * From CGSPluginInterface.
        * @return this plugin's provider category.
        */
        TInt PluginProviderCategory() const;

        /**
        * From CGSPluginInterface.
        * @return this plugin's item type.
        */
        TGSListboxItemTypes ItemType();

        /**
        * From CGSPluginInterface.
        * @param aKey key name.
        * @param aValue key value.
        */
        void GetValue( const TGSPluginValueKeys aKey,
                       TDes& aValue );

        /**
        * From CGSPluginInterface.
        * @param aSelectionType selection type.
        */
        void HandleSelection( const TGSSelectionTypes aSelectionType );
        
        /**
        * From CEikonEnv.
        * @return this plugin's pointer to the created icon.
        */
        CGulIcon* CreateIconL( const TUid aIconType );

    protected: // New

        /**
        * C++ default constructor.
        */
        CConnectionMonitorPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    protected: // From CAknView

        /**
        * From CAknView.
        * This implementation is empty because this class, being just
        * a dialog, does not implement the CAknView finctionality.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From CAknView.
        * This implementation is empty because this class, being just 
        * a dialog, does not implement the CAknView finctionality.
        */
        void DoDeactivate();

    private: // New

        /**
        * Opens localized resource file.
        */
        void OpenLocalizedResourceFileL(
                            const TDesC& aResourceFileName,
                            RConeResourceLoader& aResourceLoader );

        /**
        * Launches ConnectionMonitor application as embedded.
        */
        void LaunchConnectionMonitorAppL();
        
        /**
        * Shows an error note in case of error. 
        * @param aErrorCode The error code.
        */
        void ShowErrorNote(TInt aErrorCode);

        /**
        * Shows the error note and resolves the error.
        * @param aErrorCode The error code.
        */
        TBool ShowErrorNoteL(TInt aErrorCode);
    
        /**
        * Shows a general error note
        */
        void ShowGeneralErrorNoteL();
        
    protected: // Data

        /**
        * Resource loader for this plugin's resources.
        */
        RConeResourceLoader iResources;

        /**
        * AVKON NULL Service.
        * Own.
        */
        CAknNullService* iNullService;
    };

#endif // CONNECTIONMONITORPLUGIN_H
// End of File
