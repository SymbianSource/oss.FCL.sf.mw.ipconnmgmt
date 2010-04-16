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
* Description: 
*        Header file for CApSettingsPlugin class.
*
*/

#ifndef __AP_SETTINGS_PLUGIN_H__
#define __AP_SETTINGS_PLUGIN_H__

// System includes
#include <e32def.h>
#include <e32cmn.h>
#include <gsplugininterface.h>
#include <ConeResLoader.h>

// Forward declaration
class CApSettingsPluginContainer;

/**
* Wrapper class for AP Settings Handler UI. 
*/
class CApSettingsPlugin : public CGSPluginInterface
    {
    public://Constructors & destructors

        /**
        * First phase constructor
        * Creates new Plugin and Launches APSettingsHandlerUI
        * with default parameters
        */ 
        static CApSettingsPlugin* NewL( TAny* aInitParams );
                        
        /**
        * Destructor
        */
        ~CApSettingsPlugin();
        
    public: // Functions from base class
        
        /**
        * from CAknView
        */
        TUid Id() const;
        
        void DoActivateL( const TVwsViewId& aPrevViewId, 
                          TUid aCustomMessageId, 
                          const TDesC8& aCustomMessage );
    
        void DoDeactivate();

    public:
    
        /**
        * from CGSPluginInterface
        */

        void GetCaptionL( TDes& aCaption ) const;

        TBool HasBitmap() const;
  
        void GetBitmapL( CFbsBitmap* aBitmap, CFbsBitmap* aMask ) const;
        
        TInt PluginProviderCategory() const; 
        
        void HandleSelection( const TGSSelectionTypes aSelectionType );
          
        TGSListboxItemTypes ItemType();
        
        CGulIcon* CreateIconL( const TUid aIconType );
            
        TBool Visible() const;

    protected:
    
        /**
        * Constructor
        */
        CApSettingsPlugin();

    private:
    
        /**
        * Second phase constructor
        */
        TAny ConstructL();
        
        /**
        * Creates and starts WLAN Settings UI 
        */
        void DoSettingsL();
                     
    private: //data
        
        //@var resource loader
        RConeResourceLoader iResources;
        TInt iApSettingsSupported;

    };

#endif //__AP_SETTINGS_PLUGIN_H__
//End of file
