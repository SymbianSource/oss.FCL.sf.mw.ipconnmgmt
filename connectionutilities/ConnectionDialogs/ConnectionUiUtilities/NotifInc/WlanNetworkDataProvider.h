/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     
*
*/


#ifndef WLAN_NETWORK_DATA_PROVIDER_NOTIF_H_INCLUDED
#define WLAN_NETWORK_DATA_PROVIDER_NOTIF_H_INCLUDED

// INCLUDES
#include "NetworkInfoArray.h"

#ifndef __WINS__
#include <wlanmgmtclient.h>
#endif


// CONSTANTS

/**
* Bit masks for Capability Information field.
*/
enum T802Dot11CapabilityBitMask
    {
    E802Dot11CapabilityEssMask          = 0x0001,
    E802Dot11CapabilityIbssMask         = 0x0002,
    E802Dot11CapabilityCfPollableMask   = 0x0004,
    E802Dot11CapabilityCfPollRequestMask= 0x0008,
    E802Dot11CapabilityPrivacyMask      = 0x0010,
    E802Dot11ShortPreambleMask          = 0x0020,
    E802Dot11PbccMask                   = 0x0040,      
    E802Dot11ChannelAgilityMask         = 0x0080
    };


// FORWARD DECLARATIONS
class CNetworkInfoArray;


// TYPES

// CLASS DECLARATION
    
/**
* 
*/
NONSHARABLE_CLASS( CWlanNetworkDataProvider ) : public CBase
    {
    public:    
        /**
        * Static constructor
        * @return A pointer to a CWlanNetworkDataProvider instance.
        */
        static CWlanNetworkDataProvider* NewL();        
                
        /**
        * Called by Tick. Refresh all networks info.
        */
        void OnTimerExpiredL();    
        
        /**
        * Destructor
        */
        virtual ~CWlanNetworkDataProvider();
                        
        /**
        * Gives back a non modifiable pointer to CNetworkInfoArray*
        */
        inline CNetworkInfoArray** GetNetworkInfoArray(); 
        
#ifndef __WINS__
        /**
        * Starts asynchron MWlanMgmtInterface.GetScanResults
        * @param aStatus request status of active object
        */        
        inline void RefreshScanInfo( TRequestStatus& aStatus );
#endif // ! __WINS__           
                        
    protected:        
        /**
        * Constructor
        */
        CWlanNetworkDataProvider();
    
        /**
        * Second phase construction
        */
        void ConstructL();
        
#ifndef __WINS__
        /**
        * Decides if SSID of wlan network hidden or not.
        * @param aSsidLength length of ssid
        * @param aSsid ssid itself 
        * @return ETrue if hidden, EFalse otherwise
        */
        TBool IsHiddenSsid( TUint aSsidLength, const TUint8* aSsid );
#endif // !__WINS__
        
    private:
    
        /**
        * Sorts actual network info array
        */
        void SortNetworkInfoArrayL();

        void AddNetworkL( TDes8& aNetworkName, TSignalStrength aStrength,
                          TNetworkMode aNetworkMode,
                          TWlanConnectionSecurityMode aSecurityMode,
                          TWlanConnectionExtentedSecurityMode aExtSecurityMode,
                          TBool aProtectedSetupSupported );

        void AddOthersEntryL();
        
        TBool IsPhoneOfflineL() const;

    private:
        
#ifndef __WINS__
        /**
        * used for wlan networks info, owned
        */
        CWlanMgmtClient*            iWlanMgmt;  

        /**
        * used for scanning networks,  owned
        */    
        CWlanScanInfo*              iScanInfo; 
#endif // !__WINS__
                
        /**
        * Owned. This poiter points to refreshing array.
        */
        CNetworkInfoArray*          iNetworkInfoArray;
    };
    
#include "WlanNetworkDataProvider.inl"  

#endif // WLAN_NETWORK_DATA_PROVIDER_NOTIF_H_INCLUDED

// End of File
