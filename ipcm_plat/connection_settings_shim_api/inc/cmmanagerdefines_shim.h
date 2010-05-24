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
* Description:  
* Defines for data types and attributes used in CmManagerShim interface.
*/

#ifndef CMMANAGERDEFINES_SHIM_H
#define CMMANAGERDEFINES_SHIM_H

// System includes

#include <cmmanagerdef.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginwlandef.h>
#include <cmpluginpacketdatadef.h>

// User includes

// Macro for exporting Shim classes
#ifdef BUILD_CONNECTIONSETTINGSSHIMDLL_DLL
#define CONNECTIONSETTINGSSHIMDLL_EXPORT Q_DECL_EXPORT
#else
#define CONNECTIONSETTINGSSHIMDLL_EXPORT Q_DECL_IMPORT
#endif

/*!
 *  Constants and data types used in CmManagerShim, CmDestinationShim,
 *  and CmConnectionMethodShim interfaces.
 */
namespace CMManagerShim
{
    /*!
     *  Constants for bearer types.
     */
    //! Wlan bearer type
    const uint BearerTypeWlan           = KUidWlanBearerType;
    //! Packet data bearer type
    const uint BearerTypePacketData     = KUidPacketDataBearerType;

    /*!
     *  Constants for IPv4 and IPv6 addresses.
     */
    //! Unspecified IPv4 address
    const QString UnspecifiedIpv4Address("0.0.0.0");
    //! Known IPv6 name server 1
    const QString KnownIpv6NameServer1("fec0:000:0000:ffff::1");
    //! Known IPv6 name server 2
    const QString KnownIpv6NameServer2("fec0:000:0000:ffff::2");
    //! Dynamic IPv6 address
    const QString DynamicIpv6Address("0:0:0:0:0:0:0:0");

    /*!
     *  Constants for maximum string lengths.
     */
    //! Connection name length
    const uint CmNameLength                 = 50;
    //! Homepage address length
    const uint CmStartPageLength            = 1024;
    //! IPv4 IP address length
    const uint CmIP4AddressLength           = 15;
    //! IPv6 DNS address length
    const uint CmIP6NameServerLength        = 50;
    //! Proxy server address length
    const uint CmProxyServerNameLength      = 1024;
    //! Proxy server port number length
    const uint CmProxyPortNumberLength      = 5;
    //! Packet data AP name length
    const uint PacketDataAPNameLength       = 100;
    //! Packet data AP user name length
    const uint PacketDataIFAuthNameLength   = 50;
    //! Packet data AP password length
    const uint PacketDataIFAuthPassLength   = 50;
    //! WLAN SSID length
    const uint WlanSSIDLength               = 32;
    
    /*!
     *  Enumerations for connection method attributes.
     */
    enum ConnectionMethodAttribute {
        // Common attributes        
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmBearerType                    = CMManager::ECmBearerType,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmName                          = CMManager::ECmName,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIapId                         = CMManager::ECmIapId,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmHidden                        = CMManager::ECmHidden,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmProtected                     = CMManager::ECmProtected,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmNetworkId                     = CMManager::ECmNetworkId,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmConnected                     = CMManager::ECmConnected,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmId                            = CMManager::ECmId,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmStartPage                     = CMManager::ECmStartPage,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPv6Supported                 = CMManager::ECmIPv6Supported,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPDNSAddrFromServer           = CMManager::ECmIPDNSAddrFromServer,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPAddress                     = CMManager::ECmIPAddress,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPAddrFromServer              = CMManager::ECmIPAddFromServer,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPNetmask                     = CMManager::ECmIPNetmask,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPGateway                     = CMManager::ECmIPGateway,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPNameServer1                 = CMManager::ECmIPNameServer1,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIPNameServer2                 = CMManager::ECmIPNameServer2,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIP6DNSAddrFromServer          = CMManager::ECmIP6DNSAddrFromServer,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIP6NameServer1                = CMManager::ECmIP6NameServer1,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmIP6NameServer2                = CMManager::ECmIP6NameServer2,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmProxyUsageEnabled             = CMManager::ECmProxyUsageEnabled,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmProxyProtocolName             = CMManager::ECmProxyProtocolName,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmProxyServerName               = CMManager::ECmProxyServerName,
        //! From TConnectionMethodCommonAttributes in cmconnectionmethoddef.h
        CmProxyPortNumber               = CMManager::ECmProxyPortNumber,

        // Packet data specific attributes
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataAPName                = CMManager::EPacketDataAPName,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataPDPType               = CMManager::EPacketDataPDPType,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIFPromptForAuth       = CMManager::EPacketDataIFPromptForAuth,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIFAuthName            = CMManager::EPacketDataIFAuthName,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPAddrFromServer      = CMManager::EPacketDataIPAddrFromServer,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPAddr                = CMManager::EPacketDataIPAddr,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPDNSAddrFromServer   = CMManager::EPacketDataIPDNSAddrFromServer,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPNameServer1         = CMManager::EPacketDataIPNameServer1,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPNameServer2         = CMManager::EPacketDataIPNameServer2,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPIP6DNSAddrFromServer = CMManager::EPacketDataIPIP6DNSAddrFromServer,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPIP6NameServer1      = CMManager::EPacketDataIPIP6NameServer1,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIPIP6NameServer2      = CMManager::EPacketDataIPIP6NameServer2,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataIFAuthPass            = CMManager::EPacketDataIFAuthPass,
        /*!
         * From TConnectionMethodPacketDataSpecificAttributes in
         * cmpluginpacketdatadef.h
         */
        PacketDataDisablePlainTextAuth  = CMManager::EPacketDataDisablePlainTextAuth,
        
        // WLAN specific attributes
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanConnectionMode              = CMManager::EWlanConnectionMode,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanSSID                        = CMManager::EWlanSSID,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanSecurityMode                = CMManager::EWlanSecurityMode,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanScanSSID                    = CMManager::EWlanScanSSID,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanChannelID                   = CMManager::EWlanChannelID,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey1InHex                = CMManager::EWlanWepKey1InHex,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey2InHex                = CMManager::EWlanWepKey2InHex,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey3InHex                = CMManager::EWlanWepKey3InHex,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey4InHex                = CMManager::EWlanWepKey4InHex,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey1InAscii              = CMManager::EWlanWepKey1InAscii,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey2InAscii              = CMManager::EWlanWepKey2InAscii,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey3InAscii              = CMManager::EWlanWepKey3InAscii,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKey4InAscii              = CMManager::EWlanWepKey4InAscii,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWepKeyIndex                 = CMManager::EWlanWepKeyIndex,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        Wlan802_1xAllowUnencrypted      = CMManager::EWlan802_1xAllowUnencrypted,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanEnableWpaPsk                = CMManager::EWlanEnableWpaPsk,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWpaPreSharedKey             = CMManager::EWlanWpaPreSharedKey,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWapiPsk                     = CMManager::EWlanWapiPsk,
        //!  From TConnectionMethodWlanSpecificAttributes in cmpluginwlandef.h
        WlanWapiPskFormat               = CMManager::EWlanWapiPskFormat
    };
    
    /*!
     * Enumerations for Wlan connection modes. Values come from
     * TWlanNetMode in cmpluginwlandef.h
     */
    enum WlanConnMode {
        Adhoc                           = CMManager::EAdhoc,
        Infra                           = CMManager::EInfra
    };
    
    /*!
     * Enumerations for Wlan security modes. Values come from
     * TWlanSecMode in cmpluginwlandef.h
     */
    enum WlanSecMode {
        WlanSecModeOpen                 = CMManager::EWlanSecModeOpen,
        WlanSecModeWep                  = CMManager::EWlanSecModeWep,
        WlanSecMode802_1x               = CMManager::EWlanSecMode802_1x,
        WlanSecModeWpa                  = CMManager::EWlanSecModeWpa,
        WlanSecModeWpa2                 = CMManager::EWlanSecModeWpa2,
        WlanSecModeWapi                 = CMManager::EWlanSecModeWAPI
    };
    
    /*!
     * Enumerations for Wlan WEP key index. Values come from
     * TWepKeyIndex in cmpluginwlandef.h
     */
    enum WlanWepKeyIndex {
        WepKeyIndex1                    = CMManager::EWepKeyIndex1,
        WepKeyIndex2                    = CMManager::EWepKeyIndex2,
        WepKeyIndex3                    = CMManager::EWepKeyIndex3,
        WepKeyIndex4                    = CMManager::EWepKeyIndex4
    };

    /*!
     * Enumerations for SNAP metadata items. Values come from
     * TSnapMetadataField in cmmanagerdef.h
     */
    enum SnapMetadataField {
        SnapMetadataInternet            = CMManager::ESnapMetadataInternet,
        SnapMetadataHighlight           = CMManager::ESnapMetadataHighlight,
        SnapMetadataHiddenAgent         = CMManager::ESnapMetadataHiddenAgent,
        SnapMetadataDestinationIsLocalised = CMManager::ESnapMetadataDestinationIsLocalised,
        SnapMetadataPurpose             = CMManager::ESnapMetadataPurpose
    };
    
    /*!
     * Enumerations for SnapMetadataPurpose. Values come from
     * TSnapPurpose in cmmanagerdef.h
     */
    enum SnapPurpose
        {
        SnapPurposeUnknown              = CMManager::ESnapPurposeUnknown,
        SnapPurposeInternet             = CMManager::ESnapPurposeInternet,
        SnapPurposeOperator             = CMManager::ESnapPurposeOperator,
        SnapPurposeMMS                  = CMManager::ESnapPurposeMMS,
        SnapPurposeIntranet             = CMManager::ESnapPurposeIntranet
        };
    
    /*!
     * Enumerations for CM Manager protection levels. Values come from
     * TProtectionLevel in cmmanagerdef.h
     */
    enum CmmProtectionLevel {
        ProtLevel0                      = CMManager::EProtLevel0,
        ProtLevel1                      = CMManager::EProtLevel1,
        ProtLevel2                      = CMManager::EProtLevel2,
        ProtLevel3                      = CMManager::EProtLevel3
    };
}

#endif // CMMANAGERDEFINES_SHIM_H
