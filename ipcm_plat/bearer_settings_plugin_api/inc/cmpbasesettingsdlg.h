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
* Description:  Declares dialog for access point view/edit.
*
*/

#ifndef CMPLUGINBASE_SETTINGS_DLG_H
#define CMPLUGINBASE_SETTINGS_DLG_H

// INCLUDES
#include <eiklbo.h>
#include <aknlists.h>
#include <AknDialog.h>
#include <ConeResLoader.h>
#include <cmcommonconstants.h>
#include <cmconnectionmethoddef.h>

#include <coehelp.h>

// FORWARD DECLARATIONS
class CCmPluginBaseEng;
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknSettingPage;
class CCmdExec;
class MCmdExec;

/**
 *  CmPluginBaseSettingsDlg dialog class.
 *  Base class for plugin settings dialogs.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
class CmPluginBaseSettingsDlg : public CAknDialog, 
                                public MEikListBoxObserver,
                                public MCmdExec
    {

    public: // Constructors and destructor

        /* C++ Constructor */
        IMPORT_C CmPluginBaseSettingsDlg( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /**
        * Create and launch dialog.        
        * @since S60 3.2
        * @return The code returned by the dialog's ExecuteLD
        */
        IMPORT_C TInt ConstructAndRunLD( );

        /* Destructor. */
        IMPORT_C  ~CmPluginBaseSettingsDlg();

    public: // from MEikCommandObserver

        IMPORT_C  virtual void ProcessCommandL( TInt aCommandId );

    public: // From CCoeControl
    
        /**
        * Returns the help context
        * From CCoeControl
        * 
        * @since S60 3.2
        * @param aContext the returned help context
        */
        IMPORT_C void GetHelpContext( TCoeHelpContext& aContext ) const;

         // New functions - API

        IMPORT_C  virtual void InitTextsL();

        IMPORT_C  void HandleListboxDataChangeL( );

        /**
        * Updates bearer specific listbox contents.         
        * @since S60 3.2
        * @param aItemArray The listbox's item array containing the setting 
        * texts
        */
        virtual void UpdateListBoxContentBearerSpecificL( 
                                               CDesCArray& aItemArray ) = 0;

        /**
        * Runs the Advanced settings dialog 
        * @since S60 3.2
        */
        virtual void RunAdvancedSettingsL() = 0;
        
    protected:

        /** From CEikDialog */
        IMPORT_C virtual void PreLayoutDynInitL();

        /** From CAknDialog */
        IMPORT_C TBool OkToExitL( TInt aButtonId );

        /** From MEikMenuObserver */
        IMPORT_C virtual void DynInitMenuPaneL( TInt aResourceId, 
                                             CEikMenuPane* aMenuPane ) ;

        /** From MEikListBoxObserver */
        IMPORT_C void HandleListBoxEventL( CEikListBox* aListBox, 
                                        TListBoxEvent aEventType );

        /** From CCoeControl */
        IMPORT_C TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType ); 
                                              
        /**
        * Updates Setting items in the listbox (setting names and values are
        * refreshed).
        * @since S60 3.2
        */                                 
        IMPORT_C virtual void UpdateListBoxContentL();
        
        /**
        * Shows the setting page for a certain setting.        
        * @since S60 3.2
        * @param aAttribute The setting to be changed by the setting page
        * @param aCommandId The command the attribute is changed with 
        * (ok/change key pressed)
        */                                 
        IMPORT_C virtual TBool ShowPopupSettingPageL( TUint32 aAttribute, 
                                                       TInt aCommandId );
            
            
        /**
        * Adds the setting item text to the setting list array
        * @since S60 3.2
        * @param aItemArray an array of setting texts
        * @param aSettingItemId the Id used to recognise a setting
        * @param aValueResId the resource if for the setting value        
        */
        IMPORT_C void AppendSettingTextsL( CDesCArray& aItemArray,
                                           const TInt aSettingItemId,
                                           const TInt aValueResId );

        /**
        * Adds the setting item text to the setting list array
        * @since S60 3.2
        * @param aItemArray an array of setting texts
        * @param aSettingItemId the Id used to recognise a setting
        * @param aValueString the value of a setting
        */
        IMPORT_C void AppendSettingTextsL( CDesCArray& aItemArray,
                                           const TInt aSettingItemId,
                                           const HBufC* aValueString = NULL );

        /**
        * Adds the setting item text to the setting list array
        * @since S60 3.2
        * @param aItemArray an array of setting texts
        * @param aSettingItemId the Id used to recognise a setting
        * @param aTitleResId the resource if for the setting title 
        * @param aValueResId the resource if for the setting value        
        * @param aCompulsory indicates compulsory settings
        */  
        IMPORT_C void AppendSettingTextsL(   
                                    CDesCArray& aItemArray,
                                    const TInt aSettingItemId,
                                    const TInt aTitleResId,
                                    const TInt aValueResId,
                                    const TBool aCompulsory = EFalse ) const;
                                                                               
        /**
        * Adds the setting item text to the setting list array
        * @since S60 3.2
        * @param aItemArray an array of setting texts
        * @param aSettingItemId the Id used to recognise a setting
        * @param aTitleResId the resource if for the setting title 
        * @param aValueString the value of a setting
        * @param aCompulsory indicates compulsory settings
        */  
        IMPORT_C void AppendSettingTextsL(  
                                    CDesCArray& aItemArray,
                                    const TInt aSettingItemId,
                                    const TInt aTitleResId,
                                    const TDesC& aValueString,
                                    const TBool aCompulsory = EFalse ) const;

        /**
        * Selects one item of the listbox.        
        * @since S60 3.2
        * @param aItemIndex The index of the item to be select.
        */
        IMPORT_C void SelectItem( TInt aItemIndex );        

        /**
        * Sets a string attribute in the settings listbox.
        * @since S60 3.2
        * @param aAttribute The attribute id to set.
        * @param aDefRes The default string's resource to use.
        * HBuf ownership is passed to the caller
        */
        IMPORT_C HBufC* StringAttributeWithDefaultL( TUint32 aAttribute,
                                                  TUint32 aDefRes );  

        /**
        * Loads a resource file.
        * @since S60 3.2
        * @param aResFileName the resource file name.
        */
        IMPORT_C TInt LoadResourceL ( const TDesC& aResFileName );


    public: // 'Show' functions
    
        /**
        * Displays a Radio button setting page
        * @since S60 3.2
        * @param aResIds an array of item texts terminated with 0.
        * @param aCurrentItem The currently selected item
        * @param aTitleId The title of the setting page
        * @return The code returned by the dialogs' ExecuteLD
        */
        IMPORT_C TInt ShowRadioButtonSettingPageL( const TUint32* aResIds,
                                                   TInt& aCurrentItem,
                                                   TUint32 aTitleId );

        /**
        * Displays a Radio button setting page
        * @since S60 3.2
        * @param aItems array of selection items
        * @param aCurrentItem The currently selected item
        * @param aTitleId The title of the setting page
        * @return The code returned by the dialogs' ExecuteLD
        */
        IMPORT_C TInt ShowRadioButtonSettingPageL( 
                                                CDesCArrayFlat& aItems, 
                                                TInt& aCurrentItem,
                                                TUint32 aTitleId );
        /**
        * Displays an IP setting page
        * @param aAttribute the setting to be modified
        * @param aTitleId The title of the setting page
        * @return ETrue if changed
        */
        IMPORT_C TBool ShowPopupIpSettingPageL( TUint32 aAttribute,
                                                TUint32 aTitleId = 0 );
        
        /**
        * Shows a popup page for numeric settings
        * @since S60 3.2
        * @param aAttribute the setting to be modified
        * @param aTitleId The title of the setting page
        */
        IMPORT_C TBool ShowPopupPortNumSettingPageL( 
                                                TUint32 aAttribute,
                                                TUint32 aTitleId = 0 );
           
        /*
        * Shows a text based setting page for a specific setting
        * @since S60 3.2
        * @param aAttribute the setting to be modified
        * @param aAttribute the setting
        */
        IMPORT_C TBool ShowPopupTextSettingPageL( TUint32 aAttribute,
                                                  TUint32 aTitleId = 0 );
        
        /**
        * Shows a popup page for password settings
        * @since S60 3.2
        * @param aAttribute the setting to be modified
        * @param aTitleId The title of the setting page
        * if NULL, default is used
        */
        IMPORT_C TBool ShowPopupPasswordSettingPageL( TUint32 aAttribute, 
                                                      TUint32 aTitleId = 0 );
          
        /**
        */                                            
        IMPORT_C TBool ShowPopupIPv4DNSEditorL( TUint32 aDNSUsageAttribute,
                                                TUint32 aPrimServAttribute,
                                                TUint32 aSecServAttribute,
                                                TUint32 aTitleId = 0 );

        /**
        */
        IMPORT_C TInt ShowPopupIPv6DNSEditorL(
                                            const TUint32* aSelectionItems,
                                            TUint32 aDNSUsageAttribute,
                                            TUint32 aDNS1ServAttribute,
                                            TUint32 aDNS2ServAttribute );

        /**
        */
        IMPORT_C TInt GetIPv6DNSTypeL( TUint32 aDNS1ServAttribute,
                                             TUint32 aDNS2ServAttribute );
        
    public:
        /**
        * Handles the resource change
        * From CCoeControl
        * 
        * @since S60 5.0
        * @param aType Type of the resource change
        */
        IMPORT_C void HandleResourceChange( TInt aType );

    private:

        /**
        * 
        */
        TUint32 TitleId( TUint32 aConvTitleId,
                         TUint32 aTitleId ) const;
        /**
        */
        void SetTitleL( CAknSettingPage* aDlg,
                        TUint32 aConvTitleId,
                        TUint32 aTitleId ) const;
        
        /**
        *
        */
        TUint32 EditorId( TUint32 aConvEditorId,
                          TUint32 aEditorId ) const;
             
        /**
        * Tries to save the connection name. If it is not unique opens a dialog
        * to notify the user and asks the name again.
        */
        void SetUniqueNameL( TPtr ptrTextToChange );
                          
    public:
        /**
         * From base class MCmdExec
         */
        IMPORT_C virtual void Execute();

        /**
         * From base class MCmdExec
         */
        IMPORT_C virtual void HandleLeaveError( TInt aError );

    protected:

		// None

    protected: //data

        TUint32*                        iUid;
        CEikStatusPane*                 iStatusPane;    // NOT owned
        CAknTitlePane*                  iTitlePane;     // NOT owned
        CAknNavigationControlContainer* iNaviPane;      // NOT owned
        CAknNavigationDecorator*        iNaviDecorator; // owned

        HBufC*                          iOldTitleText;

        CAknSettingStyleListBox*    iListbox;      // owned through resources,
                                                   // destroyed automatically 
                                                   // by the dialog

        CDesCArrayFlat*         iItemArray;        // not owned

        CCmPluginBaseEng&       iCmPluginBaseEng;  // not owned
        CArrayFixFlat<TUint>*   iSettingIndex;     // OWNED    
        TInt                    iExitReason;       // Exit reason, can be 
                                                   // KUserExit or KUserBack
                                                   // used to pass user exit 
                                                   // event          
        RConeResourceLoader     iResourceReader;

        /**
         * Contains the help context of the current settings view
         */   
        TCoeContextName         iHelpContext;

    private:
        // for the FeatureManager
        TBool                   iHelp;

    private:
        // A flag to check if there is ongoing processing
        TBool                   iProcessing;
        
        // An object to keep plugin object alive if e.g Exit started from UI
        CCmdExec*               iCmdExec;

    public:
        // A flag to indicate if there is memory problem
        TBool                   iIsPossibleToSaveSetting;

        // A flag to indicate if setting has been changed by user
        TBool                   iHasSettingChanged;
        
    };

#endif // CMPLUGINBASE_SETTINGS_DLG_H
