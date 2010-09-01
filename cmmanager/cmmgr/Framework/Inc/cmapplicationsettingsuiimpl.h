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
* Description:  Declaration of application selection dialog
*
*/

#ifndef C_CMAPLICATIONSETTINGSUIIMPL_H
#define C_CMAPLICATIONSETTINGSUIIMPL_H

#include <e32def.h>
#include <e32base.h>
#include <ConeResLoader.h>
#include <badesca.h>    // CDesCArrayFlat
#include "cmmanager.hrh"
#include "cmapplicationsettingsui.h"

class TCmSettingSelection;
class CCmManagerImpl;
class CCmDestinationImpl;


//IMPLEMENTATION REMOVED!!!!
//JUST HERE TO MANTAIN BC COMPATIBILITY!!!
/**
 *  Implementation of application setting UI.
 *
 *  @lib cmmanage.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CCmApplicationSettingsUiImpl) : public CBase
    {

    public:

        IMPORT_C static CCmApplicationSettingsUiImpl* NewL();


    //=========================================================================    
    // UI API
    //
    public:

        /**
        * Launches the 'easy to use' Settings UI provided for applications
        * for handling Destinations and Connection Methods
        * @since 3.1U
        * @params aSelection User selection.
        * @returns ETrue if a selection was made
        */
        TBool RunApplicationSettingsL( TCmSettingSelection& aSelection );


        /**
        * Enables/Disabled AlwaysAsk
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableAlwaysAsk( TBool aEnable ) 
                                        { iShowAlwaysAsk = aEnable; }
        
        /**
        * Enables/Disabled EasyWlan
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableEasyWlan( TBool aEnable )
                                        { iShowEasyWlan = aEnable; }
        
        /**
        * Enables/Disabled VirtualCMs
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableVirtualCMs( TBool aEnable )
                                        { iShowVirtualCMs = aEnable; }
        
        /**
        * Enables/Disabled EmptyDestinations
        * 
        * @param ETrue enables
        * @since S60 3.2
        */
        inline void EnableEmptyDestinations( TBool aEnable ) 
                                        { iShowEmptyDestinations = aEnable; }
                                        
        /**
        * Sets a destination id which should be omitted from the list
        * typically this will be the parent destination of a virtual CM
        * 
        * @param aDestinationId
        * @since S60 3.2
        */
        inline void SetDestinationToOmit( TUint32 aDestinationId )
                                    { iDestinationToOmit = aDestinationId; }

                         
    private: // data

        /**
         * pointer to the CmManager - OWNED
         */
        CCmManagerImpl* iCmManagerImpl;
        
        /**
         * resource loader
         */
        RConeResourceLoader iResourceReader;
        
        /**
         * List of destination IDs to show
         */ 
        RArray<TUint32> iDestinations;
                
        /**
         * Indicates whether Always Ask should be shown in the page
         */
        TBool iShowAlwaysAsk;
        
        /**
         * Indicates whether EasyWlan should be shown in the page
         */
        TBool iShowEasyWlan;
        
        /**
         * Indicates whether virtual iaps should be shown in the page
         */
        TBool iShowVirtualCMs;
        
        /**
         * Indicates whether Empty destinations should be shown in the page
         */
        TBool iShowEmptyDestinations;
        
        /**
         * Indicates the id of destination to specifically omit (usually a parent destination)
         * if zero, it can be ignored
         */
        TUint iDestinationToOmit;
    };

#endif // C_CMAPLICATIONSETTINGSUIIMPL_H
