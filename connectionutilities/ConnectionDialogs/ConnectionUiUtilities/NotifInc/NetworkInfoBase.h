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


#ifndef NETWORK_BASE_NOTIF_H_INCLUDED
#define NETWORK_BASE_NOTIF_H_INCLUDED

// INCLUDES

#include    <stdlib.h>
#include    <cdblen.h>
#include    <badesca.h>
#include    <e32std.h>
#include    <stdlib.h>
#include    <wlanmgmtcommon.h>

// CONSTANTS
/**
* Max length of ListBox item text
*/
LOCAL_D const TUint KNetworkListItemText = 64;

/**
* This masks are used for set ListBox item text at network details
*/
_LIT( KNetworkListItemTextFormat, "%d\t%S\t%S\t%d\t%d" );


// FORWARD DECLARATIONS
class CWlanNetworkDataProvider;
class CNetworkInfoArray;
class CNetworkInfoKey;

// TYPES


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


/**
* Possible modes of a network
*/
enum TNetworkMode
    {
    ENetworkModeInfrastructure = 1,
    ENetworkModeAdHoc    
    };
        

// CLASS DECLARATION
/**
* Represents a network.
*/
NONSHARABLE_CLASS( CNetworkInfoBase ): public CBase
    {
    public:
        /**
        * Constructor
        */
        CNetworkInfoBase();

        /**
        * Destructor
        */
        virtual ~CNetworkInfoBase();

    public: // New functions

        /**
        * Refresh iListBoxItemText text, network ListBox has a reference to
        * this text.
        */
        /*virtual*/ void RefreshNetworksListBoxItemTextL() /*= 0*/;

        /**
        * Gives back the name of network (SSID)
        * @return the name of network 
        */
        inline TPtrC NetworkName() const;

        /**
        * Gives back current SignalStrength
        * @return iSignalStrength
        */        
        inline TSignalStrength SignalStrength() const;

        /**
        * Gives back current NetworkMode
        * @return iNetworkMode
        */
        inline TNetworkMode NetworkMode() const;

        /**
        * Gives back current SecurityMode
        * @return iSecurityMode
        */        
        inline TWlanConnectionSecurityMode SecurityMode() const;

        /**
        * Gives back current Protected Setup Support
        * @return iProtectedSetupSupported
        */        
        inline TBool ProtectedSetupSupported() const;

/**
        * Gives back current ExtendedSecurityMode
        * @return iExtendedSecurityMode
        */        
        inline TWlanConnectionExtentedSecurityMode ExtendedSecurityMode() const;
        
        /**
        * Gives back a pointer the iListBoxItemText of network 
        * @return the iListBoxItemText of network 
        */
        inline TPtrC GetNetworkListBoxItemText() const;

        inline TBool OthersEntry() const;
            
        inline void SetNetworkName( TDes8& aNetworkName );
        inline void SetSignalStrength( TSignalStrength aStrength );
        inline void SetNetworkMode( TNetworkMode aNetworkMode );
        inline void SetSecurityMode( 
                                TWlanConnectionSecurityMode aSecurityMode );
        inline void SetOthersEntry( TBool aOthersEntry );
        inline void SetProtectedSetupSupported( 
                                            TBool aProtectedSetupSupported );
        inline void SetExtendedSecurityMode( 
                 TWlanConnectionExtentedSecurityMode aExtendedSecurityMode );

    private:
        /**
        * The name of network (from SSID)
        */
        TName iNetworkName;
        
        /**
        * Signale strength of network
        */
        TSignalStrength iSignalStrength;
        
        /**
        * Network mode of network
        */ 
        TNetworkMode iNetworkMode;        
               
        /**
        * Security mode of network
        */ 
        TWlanConnectionSecurityMode iSecurityMode;

        /**
        * The text of network list item.
        */
        TBuf<KNetworkListItemText> iListBoxItemText; 

        TBool iOthersEntry;

        TBool iProtectedSetupSupported;

        TWlanConnectionExtentedSecurityMode iExtendedSecurityMode;
 
    };

#include "NetworkInfoBase.inl"  

#endif //NETWORK_BASE_NOTIF_H_INCLUDED

// End of File