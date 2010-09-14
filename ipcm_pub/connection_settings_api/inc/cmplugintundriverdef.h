/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Attributes for TUN Driver plug-in.
*
*/


#ifndef CMPLUGINTUNDRIVER_DEF_H
#define CMPLUGINTUNDRIVER_DEF_H

#include <cmconnectionmethoddef.h>
const TUint KPluginTUNDriverBearerTypeUid = 0x20027F3F;

namespace CMManager
    {
    /** LAN specific connection method attributes */
    enum TConnectionMethodTUNDriverSpecificAttributes
        {
		ETUNDriverSpecificAttributes = 20000,
        
        /**
        * Comma separated list of network protocols, e.g. "PPP".
        * ( String - default: None)
        */
        ETUNDriverIfNetworks = KLanBaseIfNetworks,
        
        /**
        * IP net mask of interface.
        * ( String - default: None)
        */        
        ETUNDriverIpNetMask,

        /**
        * IP address of gateway
        * ( String - default: None)
        */
        ETUNDriverIpGateway,

        /**
        * Get IP addresses (for EPOC) from server?
        * ( TBool - default: None)
        */
        ETUNDriverIpAddrFromServer,

        /**
        * IP address of EPOC.
        * ( String - default: None)
        */
        ETUNDriverIpAddr,

        /**
        * Get DNS addresses from server?
        * ( TBool - default: None)
        */
        ETUNDriverIpDNSAddrFromServer,

        /**
        * IP Address of primary name server.
        * ( String - default: None)
        */
        ETUNDriverIpNameServer1,

        /**
        * IP Address of secondary name server.
        * ( String - default: None)
        */
        ETUNDriverIpNameServer2,

        /**
        * Get IP6 DNS addresses from server?
        * ( TBool - default: None)
        */
        ETUNDriverIp6DNSAddrFromServer,

        /**
        * IP6 Address of primary name server.
        * ( String - default: None)
        */
        ETUNDriverIp6NameServer1,

        /**
        * IP6 Address of secondary name server.
        * ( String - default: None)
        */
        ETUNDriverIp6NameServer2,

        /**
        * IP address valid from this time, used to store 
        * dynamically assigned address lease info.
        * ( String - default: None)
        */
        ETUNDriverIpAddrLeaseValidFrom,

        /**
        * IP address valid for use until this time, used to store .
        * dynamically assigned address lease info.
        * ( String - default: None)
        */
        ETUNDriverIpAddrLeaseValidTo,

        /**
        * Name of the ECOM configuration daemon manager component. 
        * This component interfaces with the server identified in 
        * ISP_CONFIG_DAEMON_NAME. If specified, ISP_CONFIG_DAEMON_NAME 
        * should also be specified. 
        * ( String - default: None)
        */
        ETUNDriverConfigDaemonManagerName,

        /**
        * Name of the configuration daemon server. 
        * This server is used to provide further configuration for 
        * a connection, e.g. dynamic IP address assignment. 
        * If specified, ISP_CONFIG_DAEMON_MANAGER_NAME should also 
        * be specified.
        * ( String - default: None)
        */
        ETUNDriverConfigDaemonName,

        /**
        * Name of the TUN Driver Service Extension table.
        * ( String - default: None)
        */
        ETUNDriverServiceExtensionTableName,

        /**
        * The record ID of the linked TUN Driver Service Extension record in the
        * TUN Driver Service Extension table.
        * ( TUint32 - default: None)
        */
        ETUNDriverServiceExtensionTableRecordId,

        ETUNDriverRangeMax = KLanBaseRangeMax
        };
    } // namespace CMManager

#endif // CMPLUGINTUNDRIVER_DEF_H
