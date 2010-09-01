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
* Description:  Declares dialog for wlan cm plugin ipv6 settings view/edit
*
*/

#ifndef CMPLUGINWLANIPV6_SETTINGS_DLG_H
#define CMPLUGINWLANIPV6_SETTINGS_DLG_H

// INCLUDES
#include <mcmdexec.h>
#include <cmpbasesettingsdlgip.h>
#include "cmcommsdatnotifier.h"

// FORWARD DECLARATIONS
class CCmPluginBaseEng;

/**
 *  CmPluginWlanSettingsDlgIpv6 dialog class
 *
 *  shows settings for WLAN IPv6
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CmPluginWlanSettingsDlgIpv6 ) : 
                                            public CmPluginBaseSettingsDlgIp ,
                                            public MCmCommsDatWatcher
    {
    public: // Constructors and destructor
        
        /**
        * Two-phase construction.
        * @param aCmPluginBaseEng  wlan plugin to edit
        */
        static CmPluginWlanSettingsDlgIpv6* NewL( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /**
        * Destructor.
        */
        ~CmPluginWlanSettingsDlgIpv6();
        
    public: // from base class

        /**
        * From CmPluginBaseSettingsDlg
        * Create and launch dialog
        *
        * @since S60 v3.2
        * @return The code returned by the dialog's ExecuteLD
        */
        TInt ConstructAndRunLD();

    public: // From CmPluginBaseSettingsDlg

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

    protected:
    
        /** From CAknDialog */
        TBool OkToExitL( TInt aButtonId );
                
    private: // construction
    
        CmPluginWlanSettingsDlgIpv6( CCmPluginBaseEng& aCmPluginBaseEng );

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
        TBool ShowPopupSettingPageL ( TUint32 aAttribute, TInt aCommandId );

        /**
        * From CmPluginBaseSettingsDlg
        * Collects the packet data specific setting information for 
        * displaying
        *
        * @since S60 3.2
        * @param aItemArray an array of setting information
        */
        TBool UpdateListBoxContentBearerSpecificL( TUint32 aAttribute );
        
    private: // new functions
    
        /**
        * Text intialisation
        *
        * @since S60 v3.2
        */
        void InitTextsL();
        
    private:
        
        CCmParentViewNotifier* iParent;
        
        TBool iIspossibleExit;
        TBool iExitImediate;
    };

/** IPv6 types */
enum TIpv6DNSTypes
    {
    EIpv6DNSDynamic = 0, //radio button page commands
    EIpv6DNSWellKnown,
    EIpv6DNSUserDefined
    };


#endif // CMPLUGINWLANIPV6_SETTINGS_DLG_H
