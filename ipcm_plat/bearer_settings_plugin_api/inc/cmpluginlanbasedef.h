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
* Description:  Attribute definitions for LAN Plug-ins
*
*/

#ifndef CMPLUGINLAN_BASE_DEF_H
#define CMPLUGINLAN_BASE_DEF_H

#define KUidLanBearerType  0x1020737A

// INCLUDES
#include "cmconnectionmethoddef.h" //KLanBaseIfNetworks

namespace CMManager
{
/** LAN specific connection method attributes */
enum
    {
    ELanBaseSpecificAttributes = KLanBaseSpecificAttributes,
    ELanBaseIfNetworks = KLanBaseIfNetworks,
    ELanBaseIpNetMask,
    ELanBaseIpGateway,
    ELanBaseIpAddrFromServer,
    ELanBaseIpAddr,
    ELanBaseIpDNSAddrFromServer,
    ELanBaseIpNameServer1,
    ELanBaseIpNameServer2,
    ELanBaseIp6DNSAddrFromServer,
    ELanBaseIp6NameServer1,
    ELanBaseIp6NameServer2,
    ELanBaseIpAddrLeaseValidFrom,
    ELanBaseIpAddrLeaseValidTo,
    ELanBaseConfigDaemonManagerName,
    ELanBaseConfigDaemonName,
    ELanBaseServiceExtensionTableName,
    ELanBaseServiceExtensionTableRecordId,
    ELanBaseRangeMax = KLanBaseRangeMax
    };
} // namespace CMManager

#endif // CMPLUGINLAN_BASE_DEF_H
