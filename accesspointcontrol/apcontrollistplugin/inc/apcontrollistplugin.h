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
* Description: 
*        Header file for CAPControlListPlugin class.
*
*/

#ifndef APCONTROLLIST_PLUGIN_H__
#define APCONTROLLIST_PLUGIN_H__

// System includes
#include <eikclb.h>
#include <gsplugininterface.h>
#include <ConeResLoader.h>

// This UID is used for both the view UID and the ECOM plugin implementation UID.
const TUid KAPControlListPluginUid = { 0x10281BB5 };


// Forward declarations
class CAPControlListPluginContainer;
class CAPControlListAPIWrapper;
class MUiRunnerObserver;
class MEikListBoxObserver;
class CAPControlListCapsChange;

/**
* Interface class for AP Settings Handler UI. 
*/
class CAPControlListPlugin : public CGSPluginInterface,
                           public MEikListBoxObserver
    {
    public://Constructors & destructors

        /**
        * First phase constructor
        * Creates new Plugin and Launches CAPControlListPlugin
        * with default parameters
        * @param aAppUi pointer to the application Ui
        */ 
        static CAPControlListPlugin* NewL( TAny* aInitParams );
                        
        /**
        * Destructor
        */
        inline ~CAPControlListPlugin();
        
    public: // Functions from base class
        
        /**
        * From CAknView
        */
        TUid Id() const;
        
        /**
        * From CAknView
        */
        void HandleCommandL( TInt aCommand );
          
        /**
        * From CAknView
        */    
        void DoActivateL( const TVwsViewId& aPrevViewId, 
                          TUid aCustomMessageId, 
                          const TDesC8& aCustomMessage );
        
        /**
        * From CAknView
        */    
        void DoDeactivate();
        
        /**
        * Exit plugin
        */    
        void ExitOnUSIMRemovalL();
        

    public:
    
        /**
        * from CGSPluginInterface
        */
        
        /**
        * Method for checking plugin's Uid. Uid identifies this GS plugin. Use
        * same Uid as the ECOM plugin implementation Uid.
        *
        * @return PluginUid
        */
        TUid PluginUid() const;

        /**
        * Method for getting caption of this plugin. This should be the 
        * localized name of the settings view to be shown in parent view.
        *
        * @param aCaption pointer to Caption variable
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * Method for checking, if item has bitmap icon to be shown in list
        *
        * @return ETrue if plugin has icon bitmap
        * @return EFalse if plugin does not have a bitmap
        */
        TBool HasBitmap() const;

        /**
        * Method for reading bitmap icon.
        *
        * @param aBitmap plugin bitmap
        * @param aMask plugin icon bitmap mask
        */
        void GetBitmapL( CFbsBitmap* aBitmap, CFbsBitmap* aMask ) const;

        /**
        * Method for reading the ID of the plugin provider category. See 
        *
        * @return Plugin provider category ID defined by 
        *         
        */
        TInt PluginProviderCategory() const;
        
        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
        * Method for checking, if plugin should be visible and used in GS FW.
        * (for example shown in listbox of the parent view).
        *
        * On default plugin is visible. Overwrite this function to enable or
        * disable your plugin dynamically.
        *
        * @return ETrue if plugin should be visible in GS.
        * @return EFalse if plugin should not be visible in GS.
        */
        virtual TBool Visible() const;
                
        /**
        * From MUiRunnerObserver
        */                    
        void UiComplete( TInt aUirEvent );
        
        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );
                                          
        /**
        * Shows a information note.
        * @param aResourceId Resource id of the showed text .
        */
        void ShowInfoNoteL( TInt aResourceId );

        /**
        * Shows a confirmation note.
        * @param aResourceId Resource id of the showed text .
        */
        void ShowConfirmNoteL( TInt aResourceId );

        /**
        * Shows a confirmation note.
        * @param aResourceId Resource id of the showed text .
        */
        void ShowConfirmNoteAPNL( TInt aResourceId, TPtrC& aAPN  );
   
      /**
        * Init menu pane
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );
		
        /**
        * Method for getting the container 
        *
        * @return the container 
        *         
        */
        CAPControlListPluginContainer* Container();
                
        /**
        * Method for getting the APIWrapper 
        *
        * @return the APIWrapper 
        *         
        */
        CAPControlListAPIWrapper* APIWrapper();

    protected:
    
        /**
        * Constructor
        */
        CAPControlListPlugin();

    private:
    
        /**
        * Second phase constructor
        */
        TAny ConstructL();
                                                     
                    
    private: //data
        
        //@var pointer to the container - owned
        CAPControlListPluginContainer* 	iContainer; 
        
        //@var Previous view.
        TVwsViewId 						iPrevViewId; 
        
        //@var resource loader
        RConeResourceLoader 			iResources;

        //@var should the plugin should be visible and used in 
        // the Connection settings.        
        TBool 							iVisible;
        
        //@var class for wrapping asyncron API calls - owned
        CAPControlListAPIWrapper* 		iAPIWrapper;
        
		//@var Wrapper class for asyncron syncron API call 
		//NotifyIccAccessCapsChange - owned        
        CAPControlListCapsChange* 		iChangeNotifier;
        
        // for the FeatureManager
        TBool                           iHelp;
	};

#endif //APCONTROLLIST_PLUGIN_H__

//End of file
