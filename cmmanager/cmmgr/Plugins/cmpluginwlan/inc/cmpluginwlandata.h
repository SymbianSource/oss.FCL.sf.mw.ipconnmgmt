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
* Description:  Declaration of WLAN data holder for WLAN plugin
*
*/

#ifndef WLAN_DATA_H
#define WLAN_DATA_H

// INCLUDES
#include    <e32base.h>


/**
* Possible values of SignalStrength of a network
*/
enum TSignalStrength
    {
    ESignalStrengthMax = 60,
    ESignalStrengthGood = 74,
    ESignalStrengthLow = 87,
    ESignalStrengthMin = 100
    };

// FORWARD DECLARATION
/*
class CCDWlanServiceRecord;
class CWEPSecuritySettings;
class CWEPSecuritySettingsUi;
class CWPASecuritySettings;
class CWPASecuritySettingsUi;
class RConeResourceLoader;
class CConnectionUiUtilities;
*/


/**
 *  WLAN Plugin data holder class
 *
 *  ?more_complete_description
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CCmPluginWlanData ) : public CBase
    {

    public: // Constructors and destructor
    
        /**
        * Two-phased constructor. Leaves on failure, places instance
        * on cleanup stack.
        *
        * @since S60 3.2
        * @return The constructed CCmPluginWlanData.
        */
        static CCmPluginWlanData* NewLC();
    
        /**
        * Destructor
        *
        * @since S60 3.2
        */
        virtual ~CCmPluginWlanData();

    protected:
        /**
        * Constructor
        *
        * @since S60 3.2
        */
        CCmPluginWlanData();


        /**
        * Second-phase constructor.
        *
        * @since S60 3.2
        */
        void ConstructL();
        


    public: // New functions
    
        /**
        * Gives back the name of network (SSID)
        *
        * @since S60 3.2
        * @return the name of network 
        */
//        HBufC* GetNameLC() const;

        /**
        * Gives back current SignalStrength
        *
        * @since S60 3.2
        * @return iSignalStrength
        */        
//        TSignalStrength GetSignalStrength() const;
        
        
	public: // Data

        /**
        * The name of network (from SSID)
        */
        HBufC*                              iNetworkName;
        
        /**
        * Signale strength of network
        */
        TSignalStrength                     iSignalStrength;

	};

#endif // WLAN_DATA_H
