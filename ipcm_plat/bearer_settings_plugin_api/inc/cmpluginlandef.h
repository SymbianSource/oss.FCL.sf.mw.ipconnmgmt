/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Resource headers for LAN Plug-in.
*
*/


#ifndef CMPLUGINLAN_DEF_H
#define CMPLUGINLAN_DEF_H

#include <cmconnectionmethoddef.h>

const TUint KUidLanBearerType = 0x1020737A;

namespace CMManager
    {
    /** LAN specific connection method attributes */
    enum TConnectionMethodLanSpecificAttributes
        {
        ELanSpecificAttributes = KLanBaseSpecificAttributes,
        
        /**
        * Comma separated list of network protocols, e.g. "PPP".
        * ( String - default: None)
        */
        ELanIfNetworks = KLanBaseIfNetworks,
        
        /**
        * IP net mask of interface.
        * ( String - default: None)
        */        
        ELanIpNetMask,

        /**
        * IP address of gateway
        * ( String - default: None)
        */
        ELanIpGateway,

        /**
        * Get IP addresses (for EPOC) from server?
        * ( TBool - default: None)
        */
        ELanIpAddrFromServer,

        /**
        * IP address of EPOC.
        * ( String - default: None)
        */
        ELanIpAddr,

        /**
        * Get DNS addresses from server?
        * ( TBool - default: None)
        */
        ELanIpDNSAddrFromServer,

        /**
        * IP Address of primary name server.
        * ( String - default: None)
        */
        ELanIpNameServer1,

        /**
        * IP Address of secondary name server.
        * ( String - default: None)
        */
        ELanIpNameServer2,

        /**
        * Get IP6 DNS addresses from server?
        * ( TBool - default: None)
        */
        ELanIp6DNSAddrFromServer,

        /**
        * IP6 Address of primary name server.
        * ( String - default: None)
        */
        ELanIp6NameServer1,

        /**
        * IP6 Address of secondary name server.
        * ( String - default: None)
        */
        ELanIp6NameServer2,

        /**
        * IP address valid from this time, used to store 
        * dynamically assigned address lease info.
        * ( String - default: None)
        */
        ELanIpAddrLeaseValidFrom,

        /**
        * IP address valid for use until this time, used to store .
        * dynamically assigned address lease info.
        * ( String - default: None)
        */
        ELanIpAddrLeaseValidTo,

        /**
        * Name of the ECOM configuration daemon manager component. 
        * This component interfaces with the server identified in 
        * ISP_CONFIG_DAEMON_NAME. If specified, ISP_CONFIG_DAEMON_NAME 
        * should also be specified. 
        * ( String - default: None)
        */
        ELanConfigDaemonManagerName,

        /**
        * Name of the configuration daemon server. 
        * This server is used to provide further configuration for 
        * a connection, e.g. dynamic IP address assignment. 
        * If specified, ISP_CONFIG_DAEMON_MANAGER_NAME should also 
        * be specified.
        * ( String - default: None)
        */
        ELanConfigDaemonName,

        /**
        * Name of the LAN Service Extension table.
        * ( String - default: None)
        */
        ELanServiceExtensionTableName,

        /**
        * The record ID of the linked LAN Service Extension record in the
        * LAN Service Extension table.
        * ( TUint32 - default: None)
        */
        ELanServiceExtensionTableRecordId,

        ELanRangeMax = KLanBaseRangeMax
        };
    } // namespace CMManager

#endif // CMPLUGINLAN_DEF_H
