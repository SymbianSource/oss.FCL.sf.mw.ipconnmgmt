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

#ifndef CMPLUGINBASEIP_SETTINGS_DLG_H
#define CMPLUGINBASEIP_SETTINGS_DLG_H

// INCLUDES
#include <cmpbasesettingsdlg.h>


// FORWARD DECLARATIONS

/**
 *  CmPluginBaseSettingsDlgIP dialog class
 *  base class for plugin IP settings dialogs
 *
 *  @lib cmmanager.lib
 *  @since S60 3.2
 */
class CmPluginBaseSettingsDlgIp : public CmPluginBaseSettingsDlg 
                                
    {

    public: // Constructors and destructor

        /** Constructor */
        IMPORT_C CmPluginBaseSettingsDlgIp( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /** Destructor. */
        IMPORT_C ~CmPluginBaseSettingsDlgIp();
    
    public: // From CmPluginBaseSettingsDlg
        
        virtual void UpdateListBoxContentBearerSpecificL( 
                                               CDesCArray& aItemArray );
                                               
        IMPORT_C virtual void RunAdvancedSettingsL();

    public: // From MEikMenuObserver
        
        IMPORT_C virtual void DynInitMenuPaneL( TInt aResourceId, 
                                                CEikMenuPane* aMenuPane ) ;

    protected: // from CmPluginBaseSettingsDlg
    
        IMPORT_C virtual void UpdateListBoxContentL();

    };
#endif // CMPLUGINBASEIP_SETTINGS_DLG_H
