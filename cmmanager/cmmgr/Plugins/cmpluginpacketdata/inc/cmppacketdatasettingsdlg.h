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
* Description:  Declares a settings dialog for a packet data connection 
*                method plugin
*
*/

#ifndef CMPLUGINPACKETDATA_SETTINGS_DLG_H
#define CMPLUGINPACKETDATA_SETTINGS_DLG_H

// INCLUDES
#include <cmpluginbaseeng.h>
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include "cmcommsdatnotifier.h"

// CLASS DECLARATION

/**
 *  CmPluginPacketDataSettingsDlg dialog class
 *
 *  Displays the Packet data settings
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginPacketDataSettingsDlg ) : 
                                               public CmPluginBaseSettingsDlg
                                               , public MCmCommsDatWatcher
                                               , public CCmParentViewNotifier
    {
    public: // Constructors and destructor
    
        /**
        * Two-phase constructor
        *
        * @since S60 3.2
        * @param aCmPluginBaseEng  The connection method to use
        * @return instance of the class
        */
        static CmPluginPacketDataSettingsDlg* NewL( CCmPluginBaseEng& 
                                                        aCmPluginBaseEng );

        /**
        * Destructor.
        */
        virtual ~CmPluginPacketDataSettingsDlg();
    
    public: // From CmPluginBaseSettingsDlg
    
        /**
        * From CmPluginBaseSettingsDlg
        * constructor for the dialog
        *
        * @since S60 3.2
        */
        virtual TInt ConstructAndRunLD( );
        
        /**
        * From CmPluginBaseSettingsDlg
        * Collects the packet data specific setting information for 
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
        
    public: // from class CCmParentViewNotifier
        
        void NotifyParentView( TInt aValue );
        
    protected:
        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );
        
    private: // Constructors
    
        CmPluginPacketDataSettingsDlg( CCmPluginBaseEng& aCmPluginBaseEng );    
        
    private: // From CmPluginBaseSettingsDlg
        
        /**
        * From CmPluginBaseSettingsDlg
        * Redraws the list box
        *
        * @since S60 3.2
        */
        virtual void UpdateListBoxContentL();
        
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
        
    private: // From MEikCommandObserver
                
        /**
        * From MEikCommandObserver
        * see base class for description
        *
        * @since S60 3.2
        * @param aCommandId
        */
        virtual void ProcessCommandL( TInt aCommandId );
        
    private: // New functions
            
        /**
        * Shows the bool setting page for a certain setting or
        * simply changes the setting if ok key was pressed
        * @since S60 3.2
        * @param aAttribute The setting to be changed by the setting page
        * @param aCommandId Command id, to make difference between ok and change commands
        * @return the new setting value
        */  
        TBool ChangeBoolSettingL( TUint32 aAttribute, TInt aCommandId );
        
        void HandleCommsDatChangeL();
        
    private:
        
        TInt iNotifyFromSon;
    };

#endif // CMPLUGINPACKETDATA_SETTINGS_DLG_H
