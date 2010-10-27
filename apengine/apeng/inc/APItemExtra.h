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
* Description:  Declaration of class CApItemExtra 
*
*/


#ifndef CAPITEMEXTRA_H
#define CAPITEMEXTRA_H


//  INCLUDES

// FORWARD DECLARATION
class CApItemCdmaData;
class CApItemWlanData;

// CONSTANTS


// CLASS DECLARATION

/**
*  Representation of the access point extra data class.
*  Provides a way to store additional data whithout breaking BC.
*/
class CApItemExtra :public CBase
    {
    public:
        /**
        * Two-phased constructor. Leaves on failure
        * @return The constructed CApItemExtra.
        */
        static CApItemExtra* NewL();

        /**
        * Destructor.
        */
        virtual ~CApItemExtra();

        /**
        * C++ default constructor.
        */
        CApItemExtra();

        /**
        * Second-phase constructor.
        */
        void ConstructL();
    

    public:
        TBool               iIsFeatureManagerInitialised; // FALSE by default
        TBool               iIpv6GetDNSIPFromServer;
        TBool               iIsWcdma;
        HBufC*              iIpv6PrimaryDNS;
        HBufC*              iIpv6SecondaryDNS;
        TUint               iIsIpv6Supported;   // FALSE by default
        TBool               iIsVpnAp;           // False by default
        TApBearerType       iVPnRealIapBearerType;
        CApItemCdmaData*    iCdmaData;  // Owned. If NULL, CDMA is unsupported.
        CApItemWlanData*    iWlanData;  // Owned. If NULL, Wlan is unsupported.
        HBufC*              iLanBearerModemName;
        HBufC*              iIapBearerType;
        TUint32             iIapBearerID;
        TInt                iBearerProtocol;
        TInt                iBearerSpeed;

        /**
        * Indicates if CSD Access point is supported or not.
        */
        TBool               iIsAppCsdSupport;      
        
        /**
        * Indicates if HSCSD Access point is supported or not.
        */
        TBool               iIsAppHscsdSupport;              

        
    };

#endif      // CAPITEMEXTRA_H

// End of File
