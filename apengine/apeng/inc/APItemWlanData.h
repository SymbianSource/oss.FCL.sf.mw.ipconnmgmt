/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CApItemWlanData 
*
*/


#ifndef CAPITEMWLANDATA_H
#define CAPITEMWLANDATA_H


//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION

/**
* Wlan-specific data extension for CApAccessPointItem.
* Access Point data which is common to all AP-s (ISP...), are not stored
* here.
*/
class CApItemWlanData : public CBase
    {
    public:
        /**
        * Two-phased constructor. Leaves on failure
        * @return The constructed CApItemWlanData.
        */
        static CApItemWlanData* NewL();

        /**
        * Destructor.
        */
        virtual ~CApItemWlanData();

        /**
        * C++ default constructor.
        */
        CApItemWlanData();

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    public:
        // data members are public for CApAccessPointItem...
        HBufC* iWlanNetworkName;    ///< WLAN netwotk name, SSID
        TUint32 iWlanNetworkMode;   ///< Gives network mode, TUint32
        TUint32 iWlanSecurityMode;  ///< The security mode, TUint32
        TUint32 iWlanId;            ///< Commdb_id of wlan settings, TUint32
        TBool   iIsWlan;            ///< Indicates that this WLAN-capable 
                                    ///< access point is really WLAN or not.
        TBool   iScanSSID;          ///< need to scan the SSID
        TUint32 iChannelId;         ///< The ad-hoc channel ID
    };

#endif      // CAPITEMWLANDATA_H

// End of File
