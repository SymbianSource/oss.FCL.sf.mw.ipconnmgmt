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
* Description:  declares dialog for wlan cm plugin advanced view/edit.
*
*/

#ifndef CMPLUGINWLANADV_SETTINGS_DLG_ADV_H
#define CMPLUGINWLANADV_SETTINGS_DLG_ADV_H

// INCLUDES
#include <in_sock.h>
#include <mcmdexec.h>
#include <cmpbasesettingsdlgadv.h>
#include "cmcommsdatnotifier.h"

// FORWARD DECLARATIONS
class CCmPluginBaseEng;

/**
 *  CmPluginWlanSettingsDlgAdv dialog class
 *
 *  shows the advanced WLAN settings
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginWlanSettingsDlgAdv ) : 
                                            public CmPluginBaseSettingsDlgAdv ,
                                            public MCmCommsDatWatcher ,
                                            public CCmParentViewNotifier
    {
    public: // Constructors and destructor

        /**
        * Two-phase construction
        *
        * @since S60 v3.2
        * @param aCmPluginBaseEng
        */
        static CmPluginWlanSettingsDlgAdv* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );
        /**
        * Destructor.
        */
        ~CmPluginWlanSettingsDlgAdv();

    public: // from base class

        /**
        * From CmPluginBaseSettingsDlg
        * Create and launch dialog
        *
        * @since S60 v3.2
        * @return The code returned by the dialog's ExecuteLD
        */
        TInt ConstructAndRunLD();
        
        /**
        * From CmPluginBaseSettingsDlg
        * Updates bearer specific listbox contents
        *
        * @since S60 v3.2
        * @param aItemArray item array containing the setting texts
        */
        void UpdateListBoxContentBearerSpecificL( CDesCArray& itemArray );
        
    public:
        
        /**
         * Record the object of parent view
         */
        void RegisterParentView( CCmParentViewNotifier* aParent );
        
    public: // from class MCmCommsDatWatcher
        
        /**
         * Watch changes in CommsDat
         */
        void CommsDatChangesL();
        
    public: // from class CCmParentViewNotifier
        
        void NotifyParentView( TInt aValue );

    private: // construction
    
        /**
        * C++ constructor
        */
        CmPluginWlanSettingsDlgAdv( CCmPluginBaseEng& aCmPluginBaseEng );
        
    protected:
        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );

    private: // From MEikCommandObserver

        /**
        * From MEikCommandObserver
        * see base class for description
        *
        * @since S60 3.2
        * @param aCommandId
        */
        void ProcessCommandL( TInt aCommandId );

    private: // From CmPluginBaseSettingsDlg 
    
        /**
        * From CmPluginBaseSettingsDlg 
        * Determines which type of setting page should be shown
        *
        * @param aAttribute The setting to be changed by the setting page
        * @param aCommandId The command the attribute is changed with (ok/change key pressed)
        * @return the soft key selection
        */   
        TBool ShowPopupSettingPageL( TUint32 aAttribute, TInt aCommandId );

        TBool ShowIpv4SettingPageL();
        
        TBool ShowIpv6SettingPageL();
        
        /**
        * Shows a radio button page for selecting the ad-hoc channel setting value
        * If 'User defined' is selected a numeric query dialog is displayed for entering the 
        * channel number
        * 
        * @since S60 3.2
        * @return ETrue if the setting value has been changed
        */
        TBool ShowAdhocChannelSettingPageL();
        
        /**
        * From CmPluginBaseSettingsDlg
        * Collects the packet data specific setting information for 
        * displaying
        *
        * @since S60 3.2
        * @param aItemArray an array of setting information
        */
        TBool UpdateListBoxContentBearerSpecificL( TUint32 aAttribute );
        
        void HandleCommsDatChangeL();
                
    private:  // data

        /**
        * IPv6 supported indicator
        */
        TBool iIpv6Supported;
        
        CCmParentViewNotifier* iParent;
        TInt iNotifyFromIp;
    };

/** menu commands for IPv setting pages */
enum TWlanIpvMenuCommands
    {
    ESettingPageCmdIpv4,
    ESettingPageCmdIpv6
    };

/** Adhoc channel setting values */
enum TAdhocChannel
    {
    EAdhocAutomatic = 0, //radio button page commands
    EAdhocUserDefined
    };

#endif // CMPLUGINWLANADV_SETTINGS_DLG_ADV_H
