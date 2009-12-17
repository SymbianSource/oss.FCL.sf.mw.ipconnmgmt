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
* Description:  Declares an advanved settings dialog for a packet data 
*                connection method plugin
*
*/

#ifndef CMPLUGINPACKETDATA_SETTINGS_DLG_ADV_H
#define CMPLUGINPACKETDATA_SETTINGS_DLG_ADV_H

// INCLUDES
#include <mcmdexec.h>
#include <cmpbasesettingsdlgadv.h>
#include <cmpluginbaseeng.h>
#include <in_sock.h>
#include "cmcommsdatnotifier.h"

// FORWARD DECLARATIONS
class CCmPluginBaseEng;

// CLASS DECLARATION

/**
 *  CmPluginPacketDataSettingsDlgAdv dialog class
 *
 *  displays the packet data advanced settings dialog
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginPacketDataSettingsDlgAdv ) : 
                                            public CmPluginBaseSettingsDlgAdv ,
                                            public MCmCommsDatWatcher
    {
    public: // Constructors and destructor

        /**
        * Two-phase construction
        *
        * @since S60 3.2
        * @param aCmPluginBaseEng  The connection method to use
        * @return instance of the class
        */
        static CmPluginPacketDataSettingsDlgAdv* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /**
        * Destructor
        */
        virtual ~CmPluginPacketDataSettingsDlgAdv();

    public: // From CmPluginBaseSettingsDlg
    
        /**
        * From CmPluginBaseSettingsDlg
        * Collects the packet data specific setting information for 
        * displaying
        *
        * @since S60 3.2
        * @param aItemArray an array of setting information
        */
        void UpdateListBoxContentBearerSpecificL( CDesCArray& itemArray );
        
        /**
        * From CmPluginBaseSettingsDlg
        * displays the advanced settings dialog
        *
        * @since S60 3.2
        */
        void RunAdvancedSettingsL() {;}
        
    public: // from base class

        /**
        * From CmPluginBaseSettingsDlg
        * Create and launch dialog
        *
        * @since S60 v3.2
        * @return The code returned by the dialog's ExecuteLD
        */
        TInt ConstructAndRunLD();
        
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
        
    protected:
        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );

    private:
    
        /** 
        * C++ constructor
        */
        CmPluginPacketDataSettingsDlgAdv( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );
        
    private: // From CmPluginBaseSettingsDlg
    
        /**
        * From CmPluginBaseSettingsDlg
        * according to the setting type the correct edit dialog is displayed
        *
        * @since S60 3.2
        * @param aAttribute
        * @param aCommandId
        */
        TBool ShowPopupSettingPageL( TUint32 aAttribute, TInt aCommandId  );
        
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
        * Shows the setting page for IP DNS address
        * 
        * @since S60 3.2
        */
        void ShowPopupPacketDataIPDNSAddrFromServerL( );
        
        /**
        * Show the setting page for PDP type
        *
        * @since S60 3.2
        * @param aAttribute the setting to be changed
        */
        void ShowPDPTypeRBPageL( TUint32 aAttribute );
        
    private:  // Data Members

        /**
        * indicates if IPv6 is supported
        */
        TBool iIpv6Supported;
                
        CCmParentViewNotifier* iParent;
    };

#endif // CMPLUGINPACKETDATA_SETTINGS_DLG_ADV_H
