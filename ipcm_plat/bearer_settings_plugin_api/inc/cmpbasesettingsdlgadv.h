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

#ifndef CMPLUGINBASEADV_SETTINGS_DLG_H
#define CMPLUGINBASEADV_SETTINGS_DLG_H

// INCLUDES
#include <coecntrl.h>
#include <coeccntx.h>
#include <eiklbo.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <aknlists.h>
#include <akndialog.h>
#include <cmpbasesettingsdlg.h>

// FORWARD DECLARATIONS
class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CCmPluginBaseEng;

/**
 *  CmPluginBaseSettingsDlgAdv dialog class
 *  base class for plugin settings dialogs
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
class CmPluginBaseSettingsDlgAdv : public CmPluginBaseSettingsDlg 
                                
    {

    public: // Constructors and destructor

        /** Constructor */
        IMPORT_C CmPluginBaseSettingsDlgAdv( 
                                        CCmPluginBaseEng& aCmPluginBaseEng );

        /** Destructor. */
        IMPORT_C ~CmPluginBaseSettingsDlgAdv();
    
    public: // From CmPluginBaseSettingsDlg
        
        virtual void UpdateListBoxContentBearerSpecificL( 
                                               CDesCArray& aItemArray );
                                               
        IMPORT_C virtual void RunAdvancedSettingsL();

    public: // From MEikMenuObserver
        
        IMPORT_C virtual void DynInitMenuPaneL( TInt aResourceId, 
                                                CEikMenuPane* aMenuPane ) ;

    protected: // from CmPluginBaseSettingsDlg
    
        IMPORT_C virtual void UpdateListBoxContentL();


        IMPORT_C virtual TBool ShowPopupSettingPageL( TInt aAttribute, 
                                                       TInt aCommandId );
    };
#endif
