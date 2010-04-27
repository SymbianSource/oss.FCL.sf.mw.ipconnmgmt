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
* Description:  Declares dialog for a wlan cm plugin view/edit
*
*/

#ifndef CMPLUGINWLAN_SETTINGS_DLG_H
#define CMPLUGINWLAN_SETTINGS_DLG_H

// INCLUDES
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include "cmcommsdatnotifier.h"

// FORWARD DECLARATIONS
class CCmPluginBaseEng;

// CLASS DECLARATION
/**
* CmPluginWlanSettingsDlg dialog class
*
*/
NONSHARABLE_CLASS( CmPluginWlanSettingsDlg ) : public CmPluginBaseSettingsDlg
                                             , public MCmCommsDatWatcher
                                             , public CCmParentViewNotifier
    {
    public: // Constructors and destructor

        /**
        * Two-phase construction
        *
        * @since S60 3.2
        * @param aCmPluginBaseEng  The connection method object to use
        */
        static CmPluginWlanSettingsDlg* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /**
        * Destructor.
        */
        ~CmPluginWlanSettingsDlg();
    
    public: // from base class
    
        /**
        * From CmPluginBaseSettingsDlg
        * Create and launch dialog
        *
        * @since S60 3.2
        * @return The code returned by the dialog's ExecuteLD
        */
        TInt ConstructAndRunLD( );
    
        /**
        * From CmPluginBaseSettingsDlg
        * Updates bearer specific listbox contents
        *
        * @since S60 3.2
        * @param aItemArray item array containing the setting texts
        */
        void UpdateListBoxContentBearerSpecificL( CDesCArray& itemArray );
                
    public: // from class MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */
        void CommsDatChangesL();
        
    public: // from class CCmParentViewNotifier
        
        void NotifyParentView( TInt aValue );
        
    private:
    
        /**
        * C++ constructor
        *
        * @since S60 3.2
        */        
        CmPluginWlanSettingsDlg( CCmPluginBaseEng& aCmPluginBaseEng );


    protected:
        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );
        
    private: // from base class
    
        /**
        * From MEikCommandObserver
        * see base class for description
        *
        * @since S60 3.2
        * @param aCommandId
        */
        void ProcessCommandL( TInt aCommandId ) ;    

        // From CmPluginBaseSettingsDlg 
        
        /**
        * From CmPluginBaseSettingsDlg 
        * Determines which type of setting page should be shown
        *
        * @param aAttribute The setting to be changed by the setting page
        * @param aCommandId The command the attribute is changed with (ok/change key pressed)
        * @return the soft key selection
        */  
        TBool ShowPopupSettingPageL ( TUint32 aAttribute, TInt aCommandId  );

        /**
        * From CmPluginBaseSettingsDlg 
        * launches the advances settings dialog
        *
        * @since S60 3.2
        */
        virtual void RunAdvancedSettingsL();

        /**
        * From CmPluginBaseSettingsDlg
        * Get help contest of the plugin
        *
        * @since S60 5.0
        */
		virtual void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    private:  // New functions
        
        /**
        * Shows the bool setting page for a certain setting or
        * simply changes the setting if ok key was pressed
        *
        * @since S60 3.2
        * @param aAttribute The setting to be changed by the setting page
        * @param aCommandId Command id, to make difference between ok and change commands
        */  
        void ChangeBoolSettingL( TUint32 aAttribute, TInt aCommandId );
                
        /**
        * displays security setting page
        *
        * @since S60 3.2        
        */  
        void ShowSecurityModeRBPageL();    
        
        /**
        * Shows network mode setting page
        *
        * @since S60 3.2
        */  
        void ShowNetworkModeRBPageL();
        
        /**
        * Show the setting page for the WLAN Network Name settings
        *
        * @since S60 3.2
        */
        void ShowWlanNWNamePageL();
        
        /**
        * Exits plugin settings if compulsory values are filled
        *
        * @since S60 3.2
        * @param aConfirm ask user confirmation or not
        */  
        void ExitSettingsL( TBool aConfirm );
        
        void HandleCommsDatChangeL();
    private:
        TBool iSecDialExit;
        
        TInt iNotifyFromAdv;
        
    };


/** WLAN Network name setting items - zero indexed for the ui */    
enum TWlanNwName
    {
    EWlanNwNameUserDefined = 0, // radio button page commands
    EWlanNwNameScan
    };

#endif // CMPLUGINWLAN_SETTINGS_DLG_H

