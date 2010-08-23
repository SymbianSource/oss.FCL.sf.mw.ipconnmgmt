/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declaration of the class CWlanDeviceSettings.
*
*/



#ifndef WLANDEVICESETTINGS_H
#define WLANDEVICESETTINGS_H

// INCLUDES
#include <e32base.h>
#include <metadatabase.h>

#define DEBUG( a )
#define DEBUG1( a, b )

using namespace CommsDat;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface to read and write WLAN device specific settings.
*  The data is stored in CommsDB.
*/
NONSHARABLE_CLASS( CWlanDeviceSettings ): public CBase
    {
public:

    NONSHARABLE_STRUCT( SWlanDeviceSettings )
        {
        //Is WLAN indication icon been showed in UI.
        TUint32 backgroundScanInterval;
        //Stores the previously set background scan interval.
        TUint32 savedBgScanInterval;
        //If - ETrue default values are being used, 
        //   - EFalse use the user defined values.
        TBool useDefaultSettings;
        //How many times packets bigger than 'rts' are been resent.
        TUint32 longRetry; 
        //How many times packets smaller than 'rts' are been resent.
        TUint32 shortRetry;
        //Min. size of a packet that CTS/RTS handshake is been used.
        TUint32 rts;
        //Transmission power level in use. In mWs.
        TUint32 txPowerLevel;
        //Are the radio measurements allowed.
        TBool radioMeasurements;
         //Is WLAN in power mode enabled
        TBool powerMode;
        };
 
    public:     // Methods

    // Constructors and destructor

        /**
        * Static constructor.
        */
        static CWlanDeviceSettings* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CWlanDeviceSettings();

    // New methods

        /**
        * Returns the default WLAN device settings.
        * @param aSettings Default WLAN device settings
        */
        static void GetDefaultSettings( SWlanDeviceSettings& aSettings );

    private:    //Methods

        /**
        * C++ default constructor.
        */
        CWlanDeviceSettings();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Fills a WLAN device settings record with default data.
        * @param aTableType 0 for default settings, 1 for user settings.
        * @param aRecord pointer to commsdat table view
        */
        void FillRecordL( TUint32 aTableType, CMDBGenericRecord* aRecord );

        /**
        * Open database table.
        */
        void OpenTableL();
        
    private:    // Data
    
        //@var session - Owned
        CMDBSession* iSession;
    };

#endif      // WLANDEVICESETTINGS_H

// End of File
