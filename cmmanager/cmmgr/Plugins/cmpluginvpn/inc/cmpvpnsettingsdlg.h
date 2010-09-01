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
* Description:  Declares a settings dialog for a VPN connection 
*                method plugin
*
*/

#ifndef CMPLUGINVPN_SETTINGS_DLG_H
#define CMPLUGINVPN_SETTINGS_DLG_H

// INCLUDES
#include <cmpluginbaseeng.h>
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include "cmcommsdatnotifier.h"

// CLASS DECLARATION

/**
 *  CmPluginVpnSettingsDlg dialog class
 *
 *  Displays the VPN data settings
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginVpnSettingsDlg ) : public CmPluginBaseSettingsDlg
                                            , public MCmCommsDatWatcher
    {
    public: // Constructors and destructor
    
        /**
        * Two-phase constructor
        *
        * @since S60 3.2
        * @param aCmPluginBaseEng  The connection method to use
        * @return instance of the class
        */
        static CmPluginVpnSettingsDlg* NewL( CCmPluginBaseEng& 
                                                        aCmPluginBaseEng );
                                                        
        /**
        * Destructor.
        */
        virtual ~CmPluginVpnSettingsDlg();
    
    public: // From CmPluginBaseSettingsDlg
    
        
        /**
        * From CmPluginBaseSettingsDlg
        * constructor for the dialog
        *
        * @since S60 3.2
        */
        virtual TInt ConstructAndRunLD();
        
        /**
        * From CmPluginBaseSettingsDlg
        * Collects the VPN specific setting information for 
        * displaying
        *
        * @since S60 3.2
        * @param aItemArray an array of setting information
        */
        virtual void UpdateListBoxContentBearerSpecificL( 
                                                    CDesCArray& aItemArray );
        
        /**
        * From CmPluginBaseSettingsDlg
        * displays the advanced settings dialog
        *
        * @since S60 3.2
        */
        virtual void RunAdvancedSettingsL();
        
    public: // from class MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */
        void CommsDatChangesL();
   
    protected:  // From MEikMenuObserver 
    
        /**
        * From MEikMenuObserver
        * displays the options menu
        *
        * @param aResourceId the options menu to use
        * @param aMenuPane a pointer to the menu pane control
        * @since S60 3.2
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                             CEikMenuPane* aMenuPane ) ;

        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );
    
    private: // Constructors
    
        CmPluginVpnSettingsDlg( CCmPluginBaseEng& aCmPluginBaseEng );    
    
    private: // From CmPluginBaseSettingsDlg
        
        /**
        * From CmPluginBaseSettingsDlg
        * according to the setting type the correct edit dialog is displayed
        *
        * @since S60 3.2
        * @param aAttribute
        * @param aCommandId
        */
        virtual TBool ShowPopupSettingPageL( TUint32 aAttribute, 
                                             TInt aCommandId  );

        /**
        * From CmPluginBaseSettingsDlg
        * Get help contest of the plugin
        *
        * @since S60 5.0
        */
				virtual void GetHelpContext( TCoeHelpContext& aContext ) const;
                                             
    private: // New functions
    
        /**
        * Displays a radio button dialog for user selection of connection
        * methods (filters out VPN connection method) takes responsibility
        * for updating the setting value
        *
        * @since S60 3.2
        * @returns ETrue if update was made
        */
        TBool ShowIapSelectionSettingPageL();
        
    private: // From MEikCommandObserver
    
        /**
        * From MEikCommandObserver
        * see base class for description
        *
        * @since S60 3.2
        * @param aCommandId
        */
        virtual void ProcessCommandL( TInt aCommandId );


        /**
        * Checks whether the compulsory fields have been filled or not.
        *
        * @since S60 3.2
        * @return Whether the compulsory fields have been filled or not.
        */
        TBool CompulsoryFilledL();
        
    };

#endif // CMPLUGINVPN_SETTINGS_DLG_H
