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
* Description:  Declaration of class CApItemCdmaData 
*
*/


#ifndef CAPITEMCDMADATA_H
#define CAPITEMCDMADATA_H


//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION

/**
* CDMA2000-specific data extension for CApAccessPointItem.
* Access Point data which is common to all AP-s (ISP...), are not stored
* here.
*/
class CApItemCdmaData :public CBase
    {
    public:
        /**
        * Two-phased constructor. Leaves on failure
        * @return The constructed CApItemCdmaData.
        */
        static CApItemCdmaData* NewL();

        /**
        * Destructor.
        */
        virtual ~CApItemCdmaData();

        /**
        * C++ default constructor.
        */
        CApItemCdmaData();

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    public:
        HBufC*  iIwfName;           ///< IWF name.
        TUint32 iServiceOption;     ///< Service option.
        TUint32 iPdpType;           ///< PDP type.
        HBufC*  iPdpAddress;        ///< PDP address.
        TUint32 iReqFwdPriority;    ///< Requested forward priority.
        TUint32 iReqRevPriority;    ///< Requested reverse priority.
        TUint32 iReqFwdBitrate;     ///< Requested forward bitrate.
        TUint32 iReqRevBitrate;     ///< Requested reverse bitrate.
        TUint32 iReqFwdLoss;        ///< Req. forward frame loss rate.
        TUint32 iReqRevLoss;        ///< Req. reverse frame loss rate.
        TUint32 iReqFwdMaxDelay;    ///< Requested forward delay.
        TUint32 iReqRevMaxDelay;    ///< Requested reverse delay.
        TUint32 iMinFwdBitrate;     ///< Min. acceptable fwd bitrate.
        TUint32 iMinRevBitrate;     ///< Min. acceptable rev bitrate.
        TUint32 iAccptFwdLoss;      ///< Max. acc. fwd frame loss rate.
        TUint32 iAccptRevLoss;      ///< Max. acc. rev frame loss rate.
        TUint32 iAccptFwdMaxDelay;  ///< Max. acceptable forward delay.
        TUint32 iAccptRevMaxDelay;  ///< Max. acceptable reverse delay.
        TUint32 iQosWarningTimeout; ///< Notify user if the requested QOS
                                    ///< cannot be satisfied for this time in
                                    ///< microseconds
        TUint32 iRlpMode;           ///< Transparent or non-transparent
                                    ///< radio link protocol.
        // CDMA2000 deprecated Mobile IP fields
        TBool   iMip;               ///< Attempt to use mobile IP.
        HBufC*  iHomeAgentAddress;  ///< Home Agent address.
        TUint32 iMipTimeout;        ///< A normal reconnect (informing the
                                    ///< user) takes place if both the PPP link
                                    ///< drops and a change in SID or NID is
                                    ///< received within this time in
                                    ///< milliseconds.
        // CDMA2000 specific CDMA parameters provisioned through OTA and
        // defined in TIA-683B section 3.5.8
        // These fields replace the old Mobile IP fields (above).
        TUint32 iNaiType;               ///< Type of NAI.
        TUint32 iSimpleIpAuthAlgorithm; ///< SimpleIP auth.algorithm.
        TUint32 iSimpleIpPapSsHandle;   ///< SimpleIP PAP shared secret handle
        TUint32 iSimpleIpChapSsHandle;  ///< SimpleIP CHAP shared secret handle
        TUint32 iMipTBit;               ///< Reverse-tunneling reqd flag.
        HBufC*  iMipHomeAddress;        ///< Home address.
        HBufC*  iMipPrimaryHomeAgent;   ///< IP address of mobile node's
                                        ///< primary home agent.
        HBufC*  iMipSecondaryHomeAgent; ///< IP address of mobile node's
                                        ///< secondary home agent.
        TUint32 iMipMnAaaAuthAlgorithm; ///< MN-AAA auth algorithm.
        TUint32 iMipMnAaaSpi;           ///< MN-AAA security param index.
        TUint32 iMipMnAaaSsHandle;      ///< MN-AAA shared secret handle.
        TUint32 iMipMnHaAuthAlgorithm;  ///< MN-HA auth algorithm.
        TUint32 iMipMnHaSpi;            ///< MN-HA security param index.
        TUint32 iMipMnHaSsHandle;       ///< MN-HA shared secret handle.

    };

#endif      // CAPITEMCDMADATA_H

// End of File
