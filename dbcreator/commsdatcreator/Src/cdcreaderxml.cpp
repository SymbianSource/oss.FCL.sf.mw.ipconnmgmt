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
* Description:   Implementation of the class CReaderXML
*
*/



 
// INCLUDE FILES

#include "cdcreaderxml.h"
#include "cdclogger.h"

#include <utf.h>
#include <cmpluginpacketdatadef.h>
#include <cmplugindialcommondefs.h>
#include <cmpluginlandef.h>
#include <cmpluginwlandef.h>
#include <cmpluginvpndef.h>
#include <cmconnectionmethoddef.h>

using namespace CMManager;

//TYPE DECLARATION

//CONSTANTS
//
// Literals in XML header
_LIT16( KXmlVersion,       "xml version" );
_LIT16( KEncoding,         "encoding" );
_LIT16( KEncodingUtf8,     "UTF-8" );

// File tail marker
_LIT16( KFileTail,         "</Variant>");
_LIT16( KFileTail2,         "</Data>");

// Feature head markers
_LIT16( KFeatureHeadPacketData, "<APs>" );
_LIT16( KFeatureHeadWLAN,       "<WLAN_APs>" );
_LIT16( KFeatureHeadLAN,        "<LAN_APs>" );
_LIT16( KFeatureHeadVPN,        "<VPN_APs>" );
_LIT16( KFeatureHeadGlobal,     "<Global>" );
_LIT16( KFeatureHeadDN,         "<DNs>" );

// Feature tail markers
_LIT16( KFeatureTailPacketData, "</APs>" );
_LIT16( KFeatureTailWLAN,       "</WLAN_APs>" );
_LIT16( KFeatureTailLAN,        "</LAN_APs>" );
_LIT16( KFeatureTailVPN,        "</VPN_APs>" );
_LIT16( KFeatureTailGlobal,     "</Global>" );
_LIT16( KFeatureTailDN,         "</DNs>" );

// Feature data head markers
_LIT16( KDataHeadPacketData, "<AP>" );
_LIT16( KDataHeadWLAN,       "<WLAN_AP>" );
_LIT16( KDataHeadLAN,        "<LAN_AP>" );
_LIT16( KDataHeadVPN,        "<VPN_AP>" );
_LIT16( KDataHeadDN,         "<DN>" );

// Feature data tail markers
_LIT16( KDataTailPacketData, "</AP>" );
_LIT16( KDataTailWLAN,       "</WLAN_AP>" );
_LIT16( KDataTailLAN,        "</LAN_AP>" );
_LIT16( KDataTailVPN,        "</VPN_AP>" );
_LIT16( KDataTailDN,         "</DN>" );

// Parameter Markers
// <ParamName>ParamValue</ParamName>
_LIT16( KParamValueHead,        ">" );
_LIT16( KParamValueTail,        "</" );
_LIT16( KParamNameHead,         "<" );
_LIT16( KParamNameTail,         ">" );


//maximum line size
const TInt16 KMaxLineSize   = 1024;
//maximum item (tag) size
const TInt16 KMaxItemLength = 100;

// ratio between ascii and unicode character sizes
const TUint KAsciiUnicodeRatio = 2;

//  Following table contains the input parameter names:
//
//    Name                Type    Description
//----------------------------------------------------------------------------
//General & Gprs specific:
//========================
//KConnectionName         string    The name of the access point.
//KProtected              string    Protection
//KHidden                 string    CM/DN is hidden
//KHiddenAgent            string    CM/DN is not shown in CConnDlg
//KHighlighted            string    CM/DN is highlighted in CConnDlg
//KSeamlessness           string    Seamlessness value
//KGprs_AP_Name           string    Access point name.
//KUserName               string    Login user name
//KPromptPassword         string    Prompts the user for username and 
//                                  password
//KPassword               string    Login password.
//KPasswordAuthType       string    Disables plain text authentication.
//KWAPGatewayIP           string    WAP gateway address (IP address).
//KStartingPage           string    WAP start page URL.
//KWTLS_Security          string    Attempts a secure WTLS connection 
//                                  to the gateway.
//KWAP_ConnectionType     string    Indicates whether a connection-
//                                  oriented or connectionless API 
//                                  should be used.
//KPhoneIP                string    IP address of the interface.
//KPrimaryNameServer      string    IP address of the primary name 
//                                  server.
//KSecondaryNameServer    string    IP address of the secondary name
//                                  server.
//KProxyServerAddress     string    Proxy server address.
//KProxyPortNumber        string    Proxy port number.
//KProxyProtocolName      string    Proxy protocol name.
//KPrimaryIP6NameServer   string    IP address of the primary IPv6 
//                                  name server.
//KSecondaryIP6NameServer string    IP address of the secondary IPv6 
//                                  name server.
//KNetworkType            string    Network type: IPv4 or IPv6
//KGprsUseEdge            string    Sets the GprsUseEdge parameter
//=============
//LAN specific:
//=============
//KLanIfNetworks          string    Network protcols
//KLanIpNetMask           string    IP net mask of interface
//KLanIpGateway           string    IP address of gateway
//==============
//WLAN specific:
//==============
//KNetworkName            string    The name of the WLAN network 
//KNetworkMode            string    The WLAN network mode 
//KSecurityMode           string    The security mode of the WLAN network
//KWlanIpGatewayAddress   string    IP address of gateway 
//KWlanIpNetMask          string    IP net mask of interface 
//KWlanChannelId          integer   802.11 Channel ID (1-14) 

//---------------
//WEP security 
//---------------
//WEPKeyInUse             integer   Index of the default WEP key
//WEPAuthType             string    WEP authentication mode. 
//WEPKey1Length           string    Key length of WEP key1. 64, 128, 256 bit.
//WEPKey1Format           string    Format of WEP key 1. ASCII or Hexadecimal
//WEPKey1Data             string    Data  of WEP key1
//WEPKey2Length           string    Key length of WEP key2. 64, 128, 256 bit.
//WEPKey2Format           string    Format of WEP key 2. ASCII or Hexadecimal
//WEPKey2Data             string    Data  of WEP key2
//WEPKey3Length           string    Key length of WEP key3. 64, 128, 256 bit.
//WEPKey3Format           string    Format of WEP key 3. ASCII or Hexadecimal
//WEPKey3Data             string    Data  of WEP key3
//WEPKey4Length           string    Key length of WEP key4. 64, 128, 256 bit.
//WEPKey4Format           string    Format of WEP key 4. ASCII or Hexadecimal
//WEPKey4Data             string    Data  of WEP key4
//---------------
//WPA security
//---------------
//WPAPresharedKey         string    WPA/WPA2 pre-shared key in plain text. 
//WPAKeyLength            integer   The length of the WPA/WPA2 pre-shared key.
//WPAListOfEAPs           string    A list of EAPs in use
//WPAUseOfPresharedKey    string    WPA/WPA2 pre-shared key usage.
//=============
//VPN specific:
//=============
//KVpnIapName             string    IAP name used for the connection
//KVpnServicePolicy       string    Service policy ID
//====================
//Destination Networks
//====================
//KDN_Name                string    The name of the Destination Network
//KDN_Icon                integer   Index of icon assigned to the DN
//KDN_Metadata            string    Destination type
//KDN_Protection          integer   Protection level of the DN
//KDN_Hidden              integer   Shows whether DN is hidden or not
//KDN_IAPName             string    Name of a Connection Method that is
//                                  bound to the DN
//KDN_IAPName2..10        string    Same as above
//KDN_EmbeddedDN          string    Name of an Embedded DN that is bound
//                                  to the DN
//===============
//Global settings
//===============
//KAttachMode             string    Gprs attach mode
//KDefaultAP              string    Default Gprs access point
//KTableProtection        string    Table level protection
//KDefaultDnIcon          integer   Default icon for DNs
//KPriorityLan            integer   Default priority for Lan bearer type
//KPriorityWlan           integer   Default priority for WLan bearer type
//KPriorityPan            integer   Default priority for Pan bearer type
//KPriorityGprsOut        integer   Default priority for outgoing Gprs
//                                  bearer type
//KPriorityGprsIn         integer   Default priority for incoming Gprs
//                                  bearer type
//KPriorityCdma2k         integer   Default priority for CDMA2000 bearer type
//KPriorityDialOut        integer   Default priority for incoming dial
//                                  bearer type
//KPriorityDialIn         integer   Default priority for outgoing dial
//                                  bearer type
//KPriorityVpn            integer   Default priority for Vpn bearer type
//KPriorityMip            integer   Default priority for Mip bearer type
//KUIPriorityLan          integer   Default UI priority for Lan bearer type
//KUIPriorityWlan         integer   Default UI priority for WLan bearer type
//KUIPriorityPan          integer   Default UI priority for Pan bearer type
//KUIPriorityGprsOut      integer   Default UI priority for outgoing Gprs
//                                  bearer type
//KUIPriorityGprsIn       integer   Default UI priority for incoming Gprs
//                                  bearer type
//KUIPriorityCdma2k       integer   Default UI priority for CDMA2000
//                                  bearer type
//KUIPriorityDialOut      integer   Default UI priority for incoming dial
//                                  bearer type
//KUIPriorityDialIn       integer   Default UI priority for outgoing dial
//                                  bearer type
//KUIPriorityVpn          integer   Default UI priority for Vpn bearer type
//KUIPriorityMip          integer   Default UI priority for Mip bearer type

//KDefaultConnectionType  integer   Default connection method type
//KDefaultConnectionName  string    Default connection method name

//KUsageOfWlan            integer   Usage of known WLAN networks, automatical or manual
//KSeamlessnessHome       integer   Cellular data usage in home network
//KSeamlessnessVisitor    integer   Cellular data usage in foreign networks


//KGprsLastSocketActivityTimeout    integer
//KGprsLastSessionClosedTimeout     integer The time how long Gprs PDP context stays active 
//                                          after the last application using the connection was closed.
//KGprsLastSocketClosedTimeout      integer
//KCsdLastSocketActivityTimeout     integer
//KCsdLastSessionClosedTimeout      integer
//KCsdLastSocketClosedTimeout       integer
//KWlanLastSocketActivityTimeout    integer
//KWlanLastSessionClosedTimeout     integer
//KWlanLastSocketClosedTimeout      integer
//KWlanBGScanInterval               integer
//KWlanUseDefSettings               integer
//KWlanLongRetry                    integer
//KWlanShortRetry                   integer
//KWlanRTS                          integer
//KWlanTxPowerLevel                 integer
//KWlanRadioMeasurements            integer
//KWlanPowerMode                    integer Whether WLAN power save is on or off


//String literals defined for input parameters

//General & Gprs specific
_LIT16( KConnectionName,            "ConnectionName" );
_LIT16( KConnectionId,              "ConnectionId" );
_LIT16( KProtected,                 "Protected" );
_LIT16( KHidden,                    "Hidden" );
_LIT16( KHiddenAgent,               "HiddenAgent" );
_LIT16( KHighlighted,               "Highlighted" );
_LIT16( KSeamlessness,              "Seamlessness" );
_LIT16( KGprs_AP_Name,              "Gprs_AP_Name" );
_LIT16( KUserName,                  "UserName" );
_LIT16( KPromptPassword,            "PromptPassword" );
_LIT16( KPassword,                  "Password" );
_LIT16( KPasswordAuthType,          "PasswordAuthenticationType" );
_LIT16( KWAPGatewayIP,              "WAPGatewayIP" );
_LIT16( KStartingPage,              "StartingPage" );
_LIT16( KWTLS_Security,             "WTLS_Security" );
_LIT16( KWAP_ConnectionType,        "WAP_ConnectionType" );
_LIT16( KPhoneIP,                   "PhoneIP" );
_LIT16( KPrimaryNameServer,         "PrimaryNameServer" );
_LIT16( KSecondaryNameServer,       "SecondaryNameServer" );
_LIT16( KProxyServerAddress,        "ProxyServerAddress" );
_LIT16( KProxyPortNumber,           "ProxyPortNumber" );
_LIT16( KProxyProtocolName,         "ProxyProtocolName");
_LIT16( KPrimaryIP6NameServer,      "PrimaryIP6NameServer" );
_LIT16( KSecondaryIP6NameServer,    "SecondaryIP6NameServer" );
_LIT16( KNetworkType,               "NetworkType");
_LIT16( KIspType,                   "IspType" );
_LIT16( KGprsUseEdge,               "GprsUseEdge");
_LIT16( KLinger,                    "Linger");

//Lan specific
_LIT16( KLanIfNetworks,             "LanIfNetworks" );
_LIT16( KLanIpNetMask,              "LanIpNetMask" );
_LIT16( KLanIpGateway,              "LanIpGateway" );
_LIT16( KLanIpAddrFromServer,       "IpAddrFromServer" );
_LIT16( KLanIpDNSAddrFromServer,    "IpDNSAddrFromServer" ); 
_LIT16( KLanConfigDaemonManagerName,"ConfigDaemonManagerName" );
_LIT16( KLanConfigDaemonName,       "ConfigDaemonName" );

//Wlan specific
_LIT16( KNetworkName,               "NetworkName" );
_LIT16( KNetworkMode,               "NetworkMode" );
_LIT16( KSecurityMode,              "SecurityMode" );
_LIT16( KWlanIpGatewayAddress,      "WlanIpGatewayAddress" );
_LIT16( KWlanIpNetMask,             "WlanIpNetMask" );
_LIT16( KWlanScanSSId,              "WlanScanSSId" );
_LIT16( KWlanChannelId,             "WlanChannelId" );

//WEP security
_LIT16( KWEPKeyInUse,               "WEPKeyInUse" );
_LIT16( KWEPAuthType,               "WEPAuthType" );
_LIT16( KWEPKey1Length,             "WEPKey1Length" );
_LIT16( KWEPKey1Format,             "WEPKey1Format" );
_LIT16( KWEPKey1Data,               "WEPKey1Data" );
_LIT16( KWEPKey2Length,             "WEPKey2Length" );
_LIT16( KWEPKey2Format,             "WEPKey2Format" );
_LIT16( KWEPKey2Data,               "WEPKey2Data" );
_LIT16( KWEPKey3Length,             "WEPKey3Length" );
_LIT16( KWEPKey3Format,             "WEPKey3Format" );
_LIT16( KWEPKey3Data,               "WEPKey3Data" );
_LIT16( KWEPKey4Length,             "WEPKey4Length" );
_LIT16( KWEPKey4Format,             "WEPKey4Format" );
_LIT16( KWEPKey4Data,               "WEPKey4Data" );

//WPA security
_LIT16( KWPAPresharedKey,           "WPAPresharedKey" );
_LIT16( KWPAKeyLength,              "WPAKeyLength" );
_LIT16( KWPAListOfEAPs,             "WPAListOfEAPs" );
_LIT16( KWPAUseOfPresharedKey,      "WPAUseOfPresharedKey" );

//EAP security
_LIT16( KEapGtcUsername,            "EapGtcUsername" );
_LIT16( KEapGtcSessionValidityTime, "EapGtcSessionValidityTime" );
_LIT16( KEapGtcEncapsulation,       "EapGtcEncapsulation" );


_LIT16( KEapTlsUsername,            "EapTlsUsername" );
_LIT16( KEapTlsRealm,               "EapTlsRealm" );
_LIT16( KEapTlsVerifyServerRealm,   "EapTlsVerifyServerRealm" );
_LIT16( KEapTlsRequireClientAuth,   "EapTlsRequireClientAuth" );
_LIT16( KEapTlsSessionValidityTime, "EapTlsSessionValidityTime" );
_LIT16( KEapTlsCipherSuites,        "EapTlsCipherSuites" );
_LIT16( KEapTlsUserCertSubjectKeyId,"EapTlsUserCertSubjectKeyId" );
_LIT16( KEapTlsUserCertIssuerName,  "EapTlsUserCertIssuerName" );
_LIT16( KEapTlsUserCertSerialNumber,"EapTlsUserCertSerialNumber" );
_LIT16( KEapTlsCaCertSubjectKeyId,  "EapTlsCaCertSubjectKeyId" );
_LIT16( KEapTlsCaCertIssuerName,    "EapTlsCaCertIssuerName" );
_LIT16( KEapTlsCaCertSerialNumber,  "EapTlsCaCertSerialNumber" );
_LIT16( KEapTlsEncapsulation,       "EapTlsEncapsulation" );


_LIT16( KEapLeapUsername,           "EapLeapUsername" );
_LIT16( KEapLeapPassword,           "EapLeapPassword" );
_LIT16( KEapLeapSessionValidityTime,"EapLeapSessionValidityTime" );


_LIT16( KEapSimUsername,            "EapSimUsername" );
_LIT16( KEapSimRealm,               "EapSimRealm" );
_LIT16( KEapSimUsePseudonyms,       "EapSimUsePseudonyms" );
_LIT16( KEapSimSessionValidityTime, "EapSimSessionValidityTime" );
_LIT16( KEapSimEncapsulation,       "EapSimEncapsulation" );

_LIT16( KEapTtlsUsername,           "EapTtlsUsername" );
_LIT16( KEapTtlsRealm,              "EapTtlsRealm" );
_LIT16( KEapTtlsVerifyServerRealm,  "EapTtlsVerifyServerRealm" );
_LIT16( KEapTtlsRequireClientAuth,  "EapTtlsRequireClientAuth" );
_LIT16( KEapTtlsSessionValidityTime,"EapTtlsSessionValidityTime" );
_LIT16( KEapTtlsCipherSuites,       "EapTtlsCipherSuites" );
_LIT16( KEapTtlsEncapsulatedTypes,  "EapTtlsEncapsulatedTypes" );
_LIT16( KEapTtlsUserCertSubjectKeyId,"EapTtlsUserCertSubjectKeyId" );
_LIT16( KEapTtlsUserCertIssuerName, "EapTtlsUserCertIssuerName" );
_LIT16( KEapTtlsUserCertSerialNumber,"EapTtlsUserCertSerialNumber" );
_LIT16( KEapTtlsCaCertSubjectKeyId, "EapTtlsCaCertSubjectKeyId" );
_LIT16( KEapTtlsCaCertIssuerName,   "EapTtlsCaCertIssuerName" );
_LIT16( KEapTtlsCaCertSerialNumber, "EapTtlsCaCertSerialNumber" );

_LIT16( KEapAkaUsername,            "EapAkaUsername" );
_LIT16( KEapAkaRealm,               "EapAkaRealm" );
_LIT16( KEapAkaUsePseudonyms,       "EapAkaUsePseudonyms" );
_LIT16( KEapAkaSessionValidityTime, "EapAkaSessionValidityTime" );
_LIT16( KEapAkaEncapsulation,       "EapAkaEncapsulation" );

_LIT16( KEapPeapUsername,           "EapPeapUsername" );
_LIT16( KEapPeapRealm,              "EapPeapRealm" );
_LIT16( KEapPeapVerifyServerRealm,  "EapPeapVerifyServerRealm" );
_LIT16( KEapPeapRequireClientAuth,  "EapPeapRequireClientAuth" );
_LIT16( KEapPeapSessionValidityTime,"EapPeapSessionValidityTime" );
_LIT16( KEapPeapCipherSuites,       "EapPeapCipherSuites" );
_LIT16( KEapPeapV0Allowed,          "EapPeapV0Allowed" );
_LIT16( KEapPeapV1Allowed,          "EapPeapV1Allowed" );
_LIT16( KEapPeapV2Allowed,          "EapPeapV2Allowed" );
_LIT16( KEapPeapEncapsulatedTypes,  "EapPeapEncapsulatedTypes" );
_LIT16( KEapPeapUserCertSubjectKeyId,"EapPeapUserCertSubjectKeyId" );
_LIT16( KEapPeapUserCertIssuerName, "EapPeapUserCertIssuerName" );
_LIT16( KEapPeapUserCertSerialNumber,"EapPeapUserCertSerialNumber" );
_LIT16( KEapPeapCaCertSubjectKeyId, "EapPeapCaCertSubjectKeyId" );
_LIT16( KEapPeapCaCertIssuerName,   "EapPeapCaCertIssuerName" );
_LIT16( KEapPeapCaCertSerialNumber, "EapPeapCaCertSerialNumber" );

_LIT16( KEapMschapv2Username,       "EapMschapv2Username" );
_LIT16( KEapMschapv2Password,       "EapMschapv2Password" );
_LIT16( KEapMschapv2SessionValidityTime,"EapMschapv2SessionValidityTime" );
_LIT16( KEapMschapv2Encapsulation,  "EapMschapv2Encapsulation" );

_LIT16( KEapFastUsername,           "EapFastUsername" );
_LIT16( KEapFastRealm,              "EapFastRealm" );
_LIT16( KEapFastVerifyServerRealm,  "EapFastVerifyServerRealm" );
_LIT16( KEapFastRequireClientAuth,  "EapFastRequireClientAuth" );
_LIT16( KEapFastSessionValidityTime,"EapFastSessionValidityTime" );
_LIT16( KEapFastCipherSuites,       "EapFastCipherSuites" );
_LIT16( KEapFastEncapsulatedTypes,  "EapFastEncapsulatedTypes" );
_LIT16( KEapFastAuthProvModeAllowed,"EapFastAuthProvModeAllowed" );
_LIT16( KEapFastUnauthProvModeAllowed,"EapFastUnauthProvModeAllowed" );
_LIT16( KEapFastWarnADHPNoPAC,      "EapFastWarnADHPNoPAC" );
_LIT16( KEapFastWarnADHPNoMatchingPAC,"EapFastWarnADHPNoMatchingPAC" );
_LIT16( KEapFastWarnNotDefaultServer,"EapFastWarnNotDefaultServer" );
_LIT16( KEapFastUserCertSubjectKeyId,"EapFastUserCertSubjectKeyId" );
_LIT16( KEapFastUserCertIssuerName, "EapFastUserCertIssuerName" );
_LIT16( KEapFastUserCertSerialNumber,"EapFastUserCertSerialNumber" );
_LIT16( KEapFastCaCertSubjectKeyId, "EapFastCaCertSubjectKeyId" );
_LIT16( KEapFastCaCertIssuerName,   "EapFastCaCertIssuerName" );
_LIT16( KEapFastCaCertSerialNumber, "EapFastCaCertSerialNumber" );

_LIT16( KMschapv2Username,          "Mschapv2Username" );
_LIT16( KMschapv2Password,          "Mschapv2Password" );
_LIT16( KMschapv2SessionValidityTime,"Mschapv2SessionValidityTime" );
_LIT16( KMschapv2Encapsulation,     "Mschapv2Encapsulation" );


//VPN specific
_LIT16( KVpnIapName,                "IAP_Name" );
_LIT16( KVpnServicePolicy,          "Policy_ID" );

//DN specific
_LIT16( KDN_Name,                   "Name" );
_LIT16( KDN_Id,                     "DNId" );
_LIT16( KDN_Icon,                   "Icon" );
_LIT16( KDN_Metadata,               "Metadata" );
_LIT16( KDN_Protection,             "Protection" );
_LIT16( KDN_EmbeddedDNName,         "EmbeddedDN" );
_LIT16( KDN_IAPName,                "IAP" );

// Needed because of Customization Tool limitations
_LIT16( KDN_IAPName2,               "IAP2" );
_LIT16( KDN_IAPName3,               "IAP3" );
_LIT16( KDN_IAPName4,               "IAP4" );
_LIT16( KDN_IAPName5,               "IAP5" );
_LIT16( KDN_IAPName6,               "IAP6" );
_LIT16( KDN_IAPName7,               "IAP7" );
_LIT16( KDN_IAPName8,               "IAP8" );
_LIT16( KDN_IAPName9,               "IAP9" );
_LIT16( KDN_IAPName10,              "IAP10" );

//Global settings
_LIT16( KAttachMode,                "Attachmode" );
_LIT16( KDefaultAP,                 "DefaultAP" );
_LIT16( KDefaultDnIcon,             "DefaultDnIcon" );
_LIT16( KPriorityLan,               "PriorityLan" );
_LIT16( KPriorityWlan,              "PriorityWlan" );
_LIT16( KPriorityPan,               "PriorityPan" );
_LIT16( KPriorityGprsOut,           "PriorityGprsOut" );
_LIT16( KPriorityGprsIn,            "PriorityGprsIn" );
_LIT16( KPriorityCdma2k,            "PriorityCdma2k" );
_LIT16( KPriorityDialOut,           "PriorityDialOut" );
_LIT16( KPriorityDialIn,            "PriorityDialIn" );
_LIT16( KPriorityVpn,               "PriorityVpn" );
_LIT16( KPriorityMip,               "PriorityMip" );
_LIT16( KUIPriorityLan,             "UIPriorityLan" );
_LIT16( KUIPriorityWlan,            "UIPriorityWlan" );
_LIT16( KUIPriorityPan,             "UIPriorityPan" );
_LIT16( KUIPriorityGprsOut,         "UIPriorityGprsOut" );
_LIT16( KUIPriorityGprsIn,          "UIPriorityGprsIn" );
_LIT16( KUIPriorityCdma2k,          "UIPriorityCdma2k" );
_LIT16( KUIPriorityDialOut,         "UIPriorityDialOut" );
_LIT16( KUIPriorityDialIn,          "UIPriorityDialIn" );
_LIT16( KUIPriorityVpn,             "UIPriorityVpn" );
_LIT16( KUIPriorityMip,             "UIPriorityMip" );
_LIT16( KDefaultConnectionType,     "DefaultConnectionType" );
_LIT16( KDefaultConnectionName,     "DefaultConnectionName" );
_LIT16( KUsageOfWlan,               "UsageOfWlan" );
_LIT16( KSeamlessnessHome,          "SeamlessnessHome" );
_LIT16( KSeamlessnessVisitor,       "SeamlessnessVisitor" );
_LIT16( KGprsLastSocketActivityTimeout, "GprsLastSocketActivityTimeout" );
_LIT16( KGprsLastSessionClosedTimeout,  "GprsLastSessionClosedTimeout" );
_LIT16( KGprsLastSocketClosedTimeout,   "GprsLastSocketClosedTimeout" );
_LIT16( KWlanLastSocketActivityTimeout, "WlanLastSocketActivityTimeout" );
_LIT16( KWlanLastSessionClosedTimeout,  "WlanLastSessionClosedTimeout" );
_LIT16( KWlanLastSocketClosedTimeout,   "WlanLastSocketClosedTimeout" );
_LIT16( KWlanBGScanInterval,        "WlanBGScanInterval" );
_LIT16( KWlanUseDefSettings,        "WlanUseDefSettings" );
_LIT16( KWlanLongRetry,             "WlanLongRetry" );
_LIT16( KWlanShortRetry,            "WlanShortRetry" );
_LIT16( KWlanRTS,                   "WlanRTS" );
_LIT16( KWlanTxPowerLevel,          "WlanTxPowerLevel" );
_LIT16( KWlanRadioMeasurements,     "WlanRadioMeasurements" );
_LIT16( KWlanPowerMode,             "WlanPowerMode" );


//maximum number of PacketData AP parameters
const TInt KMaxPacketDataParam = 28;
const TInt KMaxLANParam = 26;
const TInt KMaxWLANParam = 125;
const TInt KMaxGlobalParam = 42;
const TInt KMaxVPNParam = 12;
const TInt KMaxDNParam = 19;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CReaderXML::NewL
// ---------------------------------------------------------
//
CReaderXML* CReaderXML::NewLC( const TDesC& aInFileName,
                               const TDbCreatorInputCharSet aCharSet )
    {
    CReaderXML* self = new ( ELeave ) CReaderXML( aCharSet );

    CleanupStack::PushL( self );

    self->CReaderBase::ConstructL( aInFileName );
    self->ConstructL();

    return self;
    }


// ---------------------------------------------------------
// CReaderXML::ConstructL
// ---------------------------------------------------------
//
void CReaderXML::ConstructL()
    {
    //if charset is not given as a parameter - detect it
    if ( iCharSet == ECharSetUnknown )
        {
        DetectEncodingL();
        }
        
    FillPacketDataParams();
    FillLanParams();
    FillWLanParams();
    FillVpnParams();
    FillGlobalParams();
    FillDNParams();                
    }


// ---------------------------------------------------------
// CReaderXML::CReaderXML
// ---------------------------------------------------------
//
CReaderXML::CReaderXML( const TDbCreatorInputCharSet aCharSet ) :
    CReaderBase( EFileXML, EFeatureNone ),
    iCharSet( aCharSet ),
    iLine ( NULL ),
    iFoundGlobal( EFalse )
    {
    //no implementation required
    }
    
    
// ---------------------------------------------------------
// CReaderXML::~CReaderXML
// ---------------------------------------------------------
//
CReaderXML::~CReaderXML()
    {
    delete iLine;
    
    iPDParams.Close();
    iLanParams.Close();
    iWLanParams.Close();
    iVpnParams.Close();
    iGlobalParams.Close();
    iDNParams.Close();
    }


// ---------------------------------------------------------
// CReaderXML::LocateFeature
// ---------------------------------------------------------
//
void CReaderXML::LocateFeatureL()
    {
    iCurrentFeature = EFeatureNone;
    TInt result( KErrNone );
    TBool eof( EFalse );
    
    //looks for the next feature
    while ( !eof && CurrentFeature() == EFeatureNone )
        {
        //reads the next line
        result = ReadLineL();
        
        if ( result == KErrEof )
            {
            //end of file
            eof = ETrue;    
            }
        else if ( result )    
            {
            //other error
            User::Leave( result );    
            }
        else
            {
            //no error
            if ( iLine->FindF( KFileTail ) == KErrNone || 
                 iLine->FindF( KFileTail2 ) == KErrNone )
                {
                //end of file is reached ("/variantvariant" found)
                eof = ETrue;            
                }
            else
                {
                //try to recognize a feature
                iCurrentFeature = DetectFeatureHead();
                }//else
            }//else        
        }//while
    }


// ---------------------------------------------------------
// CReaderXML::LocateAP
// ---------------------------------------------------------
//
TBool CReaderXML::LocateAPL()
    {
    TBool found( EFalse );    //found feature data
    TBool eofea( EFalse );    //end of feature
    
    if ( CurrentFeature() == EFeatureGlobal )
        {
        //for Global settings there is no feature data header
        found = iFoundGlobal;
        iFoundGlobal = EFalse;
        }
    else
        {
        //looks for the next feature header
        while ( !eofea && !found )
            {
            User::LeaveIfError( ReadLineL() );
            
            if ( DetectFeatureTail() )
                {
                //feature tail is detected - no more data to process
                eofea = ETrue;    
                }
            else
                {
                //looks for feature data header
                switch ( CurrentFeature() )
                    {
                    case EFeaturePacketData:
                        found = ( iLine->FindF( KDataHeadPacketData ) != KErrNotFound ); 
                        break;
                    case EFeatureWLAN:
                        found = ( iLine->FindF( KDataHeadWLAN ) != KErrNotFound );
                        break;
                    case EFeatureLAN:
                        found = ( iLine->FindF( KDataHeadLAN ) != KErrNotFound );
                        break;
                    case EFeatureVPN:
                        found = ( iLine->FindF( KDataHeadVPN ) != KErrNotFound );
                        break;
                    case EFeatureDN:
                        found = ( iLine->FindF( KDataHeadDN ) != KErrNotFound );
                        break;
                    }//switch
                }//else
            }//while
        }//else
            
    return found;
    
    }
           

// ---------------------------------------------------------
// CReaderXML::ParseAPL
// ---------------------------------------------------------
//
void CReaderXML::ParseAPL()
    {
    do
        {            
        User::LeaveIfError( ReadLineL() );
        TInt fieldId = DetectParam();
        
        if ( fieldId != KErrNotFound )
            {
            HBufC16* param = ReadParam( EParamValue );
            if ( param )
                {
                ConvertSpecialXMLChars( param );
                CleanupStack::PushL( param );
                TagContainer().AddL( param , fieldId );        
                CleanupStack::Pop( param );
                }    
            else
                {
                //simply skip line
                }        
            }
        
        }
    while ( !DetectTailAP() );
    }
    
// ---------------------------------------------------------
// CReaderXML::Reset
// ---------------------------------------------------------
//
TInt CReaderXML::Reset()
    {
    iFilePos = 0;
    return InputFile().Seek( ESeekStart, iFilePos );
    }
              
        
// ---------------------------------------------------------
// CReaderXML::ToDoLog
// ---------------------------------------------------------
//
void CReaderXML::ToDoLog()
    {
    CLOG_WRITE( "Line :" )
    FileLogger().Write( *iLine );
    CLOG_WRITE( "--------------------------------------------\n\n" )
    }
    
// ---------------------------------------------------------
// Private methods
// ---------------------------------------------------------
 
// ---------------------------------------------------------
// CReaderXML::ReadLine
// ---------------------------------------------------------
//  
TInt CReaderXML::ReadLineL()
    {
    //go to the actual position
    TInt result = InputFile().Seek(ESeekStart, iFilePos);
    
    //temporary buffer for line
    TBuf8<KMaxLineSize> buf;

    if (result == KErrNone) 
        {
        result = InputFile().Read(buf, buf.MaxSize());
        }

    //if seeking and reading both succesful extract one line form the buffer
    if (result == KErrNone)
        {
        TPtrC8 ptrBuf8;
        ptrBuf8.Set( buf );

        if (result == KErrNone || result == KErrEof)
            {          
            switch ( iCharSet )
                {
                case ECharSetUnicode:
                    {                     
                    //simply copy to a Unicode descriptor
                    TPtrC16 ptrBuf16( ( TText16* )ptrBuf8.Ptr(), \
                                        ptrBuf8.Length() / KAsciiUnicodeRatio );
                                        
                    TPtrC16 ptrLine;
                    result = ExtractLine(ptrBuf16, ptrLine);  
                    
                    if ( result == KErrNone ) 
                        {
                        delete iLine;
                        iLine = NULL;
                        iLine = ptrLine.AllocL();
                        
                        //Increment the file position
                        iFilePos += iLine->Length() * KAsciiUnicodeRatio;
                        }
                    
                    break;
                    } 
                case ECharSetAscii:
                case ECharSetUtf8:
                default:
                    {                    
                    //conversion to Unicode is needed
                    HBufC16* hBuf16 = HBufC16::NewLC( KMaxLineSize );
                    TPtr16 ptrBuf16( hBuf16->Des() );
                    
                    if( iCharSet == ECharSetUtf8 )
                        {
                        CnvUtfConverter::ConvertToUnicodeFromUtf8
                                                    ( ptrBuf16, ptrBuf8 );
                        }
                    else
                        {
                        ptrBuf16.Copy( ptrBuf8 );
                        }
                    
                    TPtrC16 ptrLine;
                    result = ExtractLine(ptrBuf16, ptrLine); 
                    
                    if ( result == KErrNone ) 
                        {
                        delete iLine;
                        iLine = NULL;
                        iLine = ptrLine.AllocL();
                        
                        // Increment the file position
                        // Line in unicode format converted back to UTF-8
                        // for getting the right lenght and osition in file
                        CnvUtfConverter::ConvertFromUnicodeToUtf8( 
                                            buf, iLine->Des() );
                        iFilePos += buf.Length();
                        }
                    
                    CleanupStack::PopAndDestroy( hBuf16 ); 
                    
                    break;
                    }//case
                }//switch
            }//if
        }//if

    return result;
    }
 
 
// ---------------------------------------------------------
// CReaderXML::ExtractLine
// ---------------------------------------------------------
//     
TInt CReaderXML::ExtractLine(const TPtrC16& aBuffer, TPtrC16& aLine)
    {
    // Whitespace characters
    const TChar KSpace = ' ';
    const TChar KNewLine = '\n';
    const TChar KCarriageReturn = '\r';
    const TChar KTab = '\t';
    const TChar KTagStart = '<';
    
    
    // A line is considered to contain one parameter. It starts from the
    // current position and lasts until the end of the current parameter.
    // The end is detected by looking for the characters ">" and "<" with
    // any number of white spaces between them. The next line will start at
    // the "<" character.
    TBool lineEndFound( EFalse );
    TBool tagEndFound( EFalse );
    TInt pos( 0 );
    while( !lineEndFound && pos < aBuffer.Length() )
        {
        if( !tagEndFound && KParamNameTail.iBuf[ 0 ] == aBuffer[ pos ] )
            {
            tagEndFound = ETrue;
            }
        else if( tagEndFound && KParamNameHead.iBuf[ 0 ] == aBuffer[ pos ] )
            {
            lineEndFound = ETrue;
            break;
            }
        
        else if( ( KSpace != aBuffer[ pos ] ) &&
                 ( KNewLine != aBuffer[ pos ] ) &&
                 ( KCarriageReturn != aBuffer[ pos ] ) &&
                 ( KTab != aBuffer[ pos ] ) && 
                   tagEndFound )
            {
            tagEndFound = EFalse;
            }
        ++pos;
        }
    
    if( !lineEndFound )
        {
        //End of line not found--malformed xml file or no more data
        //get highest file position; and check EoF
        TInt hiPos = 0;
        
        if (InputFile().Size(hiPos) != KErrNone) 
            {
            return KErrGeneral;    
            }
        
        //check for end of file
        if ( aBuffer.Length() < KMaxLineSize )
            {
            aLine.Set( aBuffer );
            return KErrEof;
            }
        else
            {
            return KErrNotFound;    
            }
        }

    aLine.Set( aBuffer.Left( pos ) );

    return KErrNone ;
    }

// ---------------------------------------------------------
// CReaderXML::DetectFeatureHead
// ---------------------------------------------------------
//     
TDbCreatorFeatures CReaderXML::DetectFeatureHead()
    {
    TDbCreatorFeatures feature( EFeatureNone );
    
    if ( iLine->FindF( KFeatureHeadPacketData ) != KErrNotFound )
        {
        feature = EFeaturePacketData;
        }
    else if ( iLine->FindF( KFeatureHeadWLAN ) != KErrNotFound )
        {
        feature = EFeatureWLAN;
        }
    else if ( iLine->FindF( KFeatureHeadLAN ) != KErrNotFound )
        {
        feature = EFeatureLAN;
        }
    else if ( iLine->FindF( KFeatureHeadVPN ) != KErrNotFound )
        {
        feature = EFeatureVPN;
        }
    else if ( iLine->FindF( KFeatureHeadDN ) != KErrNotFound )
        {
        feature = EFeatureDN;
        }
    else if ( iLine->FindF( KFeatureHeadGlobal ) != KErrNotFound )
        {
        feature = EFeatureGlobal;
        iFoundGlobal = ETrue;        
        }
    else
        {
        //feature not found            
        }
    
    return feature;

    }
    
    
// ---------------------------------------------------------
// CReaderXML::DetectFeatureTail
// ---------------------------------------------------------
//     
TBool CReaderXML::DetectFeatureTail()
    {
    TBool found( EFalse );
        
    switch ( CurrentFeature() )        
        {
        case EFeaturePacketData:
            found = ( iLine->FindF( KFeatureTailPacketData ) != KErrNotFound ); 
            break;
        case EFeatureWLAN:
            found = ( iLine->FindF( KFeatureTailWLAN ) != KErrNotFound );
            break;
        case EFeatureLAN:
            found = ( iLine->FindF( KFeatureTailLAN ) != KErrNotFound );
            break;
        case EFeatureVPN:
            found = ( iLine->FindF( KFeatureTailVPN ) != KErrNotFound );
            break;
        case EFeatureDN:
            found = ( iLine->FindF( KFeatureTailDN ) != KErrNotFound );
            break;
        case EFeatureGlobal:
            found = ( iLine->FindF( KFeatureTailGlobal ) != KErrNotFound );
            break;
        }
    
    return found;
    }
    
    
// ---------------------------------------------------------
// CReaderXML::DetectTailAP
// ---------------------------------------------------------
//     
TBool CReaderXML::DetectTailAP()
    {
    TBool found( EFalse );
        
    switch ( CurrentFeature() )        
        {
        case EFeaturePacketData:
            found = ( iLine->FindF( KDataTailPacketData ) != KErrNotFound ); 
            break;
        case EFeatureWLAN:
            found = ( iLine->FindF( KDataTailWLAN ) != KErrNotFound );
            break;
        case EFeatureLAN:
            found = ( iLine->FindF( KDataTailLAN ) != KErrNotFound );
            break;
        case EFeatureVPN:
            found = ( iLine->FindF( KDataTailVPN ) != KErrNotFound );
            break;
        case EFeatureDN:
            found = ( iLine->FindF( KDataTailDN ) != KErrNotFound );
            break;
        case EFeatureGlobal:
            //there is no data tail for Global settings -- looks for feature tail
            found = ( iLine->FindF( KFeatureTailGlobal ) != KErrNotFound );
            break;
        }
    
    return found;
    }
    
    
// ---------------------------------------------------------
// CReaderXML::DetectParamL
// ---------------------------------------------------------
//
TInt CReaderXML::DetectParam()
    {
    TInt maxParam( 0 );           //maximum nunber of params in array
    TInt fieldId( KErrNotFound ); //field id of detected parameter 
    RArray<EInputParams> *params( NULL ); //pointer to the table used 
                                        //for or detection    

    switch ( CurrentFeature() )        
        {
        case EFeaturePacketData:
            maxParam = KMaxPacketDataParam;
            params   = &iPDParams;
            break;
        case EFeatureWLAN:
            maxParam = KMaxWLANParam;
            params   = &iWLanParams;
            break;
        case EFeatureLAN:
            maxParam = KMaxLANParam;
            params   = &iLanParams;
            break;
        case EFeatureVPN:
            maxParam = KMaxVPNParam;
            params   = &iVpnParams;
            break;
        case EFeatureDN:
            maxParam = KMaxDNParam;
            params =   &iDNParams;
            break;
        case EFeatureGlobal:
            maxParam = KMaxGlobalParam;
            params   = &iGlobalParams;
            break;
        default:
            User::Panic( KPanicInvalidCase, KErrInvalidCase );
        }
       
    //looks for parameter match
    HBufC16* paramName = ReadParam( EParamName );
    TBool found ( EFalse );
    
    if ( paramName == NULL )
            {
            return fieldId;
            }

    for ( TInt idx = 0; idx < maxParam && !found; idx++ )
        {        
        if ( paramName->CompareF( (*params)[idx].iParam ) == 0 )
            {
            fieldId = (*params)[idx].iId;
            found = ETrue;            
            }
        }
       
    delete paramName;   
    return fieldId;
    }
  
    
// ---------------------------------------------------------
// CReaderXML::ReadParam
// ---------------------------------------------------------
//     
HBufC16* CReaderXML::ReadParam( TParamSegment aSegment )
    {
    TBuf<KMaxItemLength> tag; //local buffer
    TInt posh( 0 ); //header position
    TInt post( 0 ); //tail position
    HBufC16* ret = NULL; // returned pointer
    
    //  calculation of the position 
    //
    //  EParamValue:           posh|     post|
    //                             |         |
    //  EParamName:  posh|    post||         |
    //                   ¡        ¡¡         ¡ 
    //                  <ParamName>ParamValue</ParamName>
    
    switch ( aSegment )
        {
        case EParamValue:
            posh = iLine->Find( KParamValueHead ) + 1;
            post = iLine->Find( KParamValueTail );
            break;
        case EParamName:
            posh = iLine->Find( KParamNameHead ) + 1;
            post = iLine->Find( KParamNameTail );
            break;
        default:
            break;
        }
    
    if ( posh == KErrNotFound || post == KErrNotFound || post == 0 )
        {
        //line must contain header and tail and longer than 0
        ret = NULL;
        }
    else
        {
        // Copis the value to the local buffer and allocates buffer
        tag.Copy( iLine->Mid( posh, post - posh ) );
        ret = tag.Alloc();
        // Checks the result.
        if( ret == NULL )
            {
            CLOG_WRITE_FORMAT( "! Warning: tag could not be allocated %S:",
                                     &tag );
            }
        }
    return ret;
    }


// ---------------------------------------------------------
// CReaderXML::DetectEncoding
// ---------------------------------------------------------
//     
void CReaderXML::DetectEncodingL()
    {
    TInt originalPos( iFilePos );  //stores original file position
    TBool found( EFalse );
    
    for ( TInt i = ECharSetUnicode; i >= ECharSetAscii && !found; i--)
        {
        //assumes that the first line contains version and encoding info
        iFilePos = 0;
        iCharSet = static_cast<TDbCreatorInputCharSet>( i );
        
        if ( ReadLineL() == KErrNone )
            {
            if( iCharSet == ECharSetAscii )
                {
                if ( ( iLine->FindF( KXmlVersion ) != KErrNotFound ) ||
                     ( iLine->FindF( KEncoding ) != KErrNotFound ) )
                    {
                    if( iLine->FindF( KEncodingUtf8 ) != KErrNotFound )
                        {
                        iCharSet = ECharSetUtf8;
                        }
                    found = ETrue;
                    }
                }
            else if( iCharSet == ECharSetUnicode )
                {
                if ( ( iLine->FindF( KXmlVersion ) != KErrNotFound ) ||
                     ( iLine->FindF( KEncoding ) != KErrNotFound ) )
                    {
                    found = ETrue;
                    }
                }
            }
        }
    iFilePos = originalPos;
    }
    
// ---------------------------------------------------------
// CReaderXML::ConvertSpecialXMLChars
// ---------------------------------------------------------
//
void CReaderXML::ConvertSpecialXMLChars( HBufC* aPtrTag )
    {
    if ( aPtrTag == NULL )
            {
                return;
            }   
    _LIT16( escapeStart, "&" );
    
    //check if there are parameters to be replaced, if not: return
    if ( aPtrTag->Des().Find( escapeStart ) == KErrNotFound )
            {
                return;
            }
                
    _LIT16( spXML1, "&lt;" );
    _LIT16( sp1, "<" );
    ReplaceInString( aPtrTag, spXML1, sp1 );
    _LIT16( spXML2, "&amp;" );
    _LIT16( sp2, "&" );
    ReplaceInString( aPtrTag, spXML2, sp2 );
    _LIT16( spXML3, "&gt;" );
    _LIT16( sp3, ">" );
    ReplaceInString( aPtrTag, spXML3, sp3 );
    _LIT16( spXML4, "&quot;" );
    _LIT16( sp4, "\"" );
    ReplaceInString( aPtrTag, spXML4, sp4 );
    _LIT16( spXML5, "&apos;" );
    _LIT16( sp5, "\'" );
    ReplaceInString( aPtrTag, spXML5, sp5 );
    _LIT16( spXML6, "&iexcl;" );
    _LIT16( sp6, "¡" );
    ReplaceInString( aPtrTag, spXML6, sp6 );
    _LIT16( spXML7, "&cent;" );
    _LIT16( sp7, "¢" );
    ReplaceInString( aPtrTag, spXML7, sp7 );
    _LIT16( spXML8, "&pound;" );
    _LIT16( sp8, "£" );
    ReplaceInString( aPtrTag, spXML8, sp8 );
    _LIT16( spXML9, "&curren;" );
    _LIT16( sp9, "¤" );
    ReplaceInString( aPtrTag, spXML9, sp9 );
    _LIT16( spXML10, "&yen;" );
    _LIT16( sp10, "¥" );
    ReplaceInString( aPtrTag, spXML10, sp10 );
    _LIT16( spXML11, "&brvbar;" );
    _LIT16( sp11, "¦" );
    ReplaceInString( aPtrTag, spXML11, sp11 );
    _LIT16( spXML12, "&sect;" );
    _LIT16( sp12, "§" );
    ReplaceInString( aPtrTag, spXML12, sp12 );
    _LIT16( spXML13, "&uml;" );
    _LIT16( sp13, "¨" );
    ReplaceInString( aPtrTag, spXML13, sp13 );
    _LIT16( spXML14, "&copy;" );
    _LIT16( sp14, "©" );
    ReplaceInString( aPtrTag, spXML14, sp14 );
    _LIT16( spXML15, "&ordf;" );
    _LIT16( sp15, "ª" );
    ReplaceInString( aPtrTag, spXML15, sp15 );
    _LIT16( spXML16, "&laquo;" );
    _LIT16( sp16, "«" );
    ReplaceInString( aPtrTag, spXML16, sp16 );
    _LIT16( spXML17, "&not;" );
    _LIT16( sp17, "¬" );
    ReplaceInString( aPtrTag, spXML17, sp17 );
    _LIT16( spXML18, "&reg;" );
    _LIT16( sp18, "®" );
    ReplaceInString( aPtrTag, spXML18, sp18 );
    _LIT16( spXML19, "&macr;" );
    _LIT16( sp19, "¯" );
    ReplaceInString( aPtrTag, spXML19, sp19 );
    _LIT16( spXML20, "&deg;" );
    _LIT16( sp20, "°" );
    ReplaceInString( aPtrTag, spXML20, sp20 );
    _LIT16( spXML21, "&plusmn;" );
    _LIT16( sp21, "±" );
    ReplaceInString( aPtrTag, spXML21, sp21 );
    _LIT16( spXML22, "&sup2;" );
    _LIT16( sp22, "²" );
    ReplaceInString( aPtrTag, spXML22, sp22 );
    _LIT16( spXML23, "&sup3;" );
    _LIT16( sp23, "³" );
    ReplaceInString( aPtrTag, spXML23, sp23 );
    _LIT16( spXML24, "&acute;" );
    _LIT16( sp24, "´" );
    ReplaceInString( aPtrTag, spXML24, sp24 );
    _LIT16( spXML25, "&micro;" );
    _LIT16( sp25, "µ" );
    ReplaceInString( aPtrTag, spXML25, sp25 );
    _LIT16( spXML26, "&para;" );
    _LIT16( sp26, "¶" );
    ReplaceInString( aPtrTag, spXML26, sp26 );  
    _LIT16( spXML27, "&middot;" );
    _LIT16( sp27, "·" );
    ReplaceInString( aPtrTag, spXML27, sp27 );
    _LIT16( spXML28, "&cedil;" );
    _LIT16( sp28, "¸" );
    ReplaceInString( aPtrTag, spXML28, sp28 );
    _LIT16( spXML29, "&sup1;" );
    _LIT16( sp29, "¹" );
    ReplaceInString( aPtrTag, spXML29, sp29 );
    _LIT16( spXML30, "&ordm;" );
    _LIT16( sp30, "º" );
    ReplaceInString( aPtrTag, spXML30, sp30 );
    _LIT16( spXML31, "&raquo;" );
    _LIT16( sp31, "»" );
    ReplaceInString( aPtrTag, spXML31, sp31 );
    _LIT16( spXML32, "&frac14;" );
    _LIT16( sp32, "¼" );
    ReplaceInString( aPtrTag, spXML32, sp32 );
    _LIT16( spXML33, "&frac12;" );
    _LIT16( sp33, "½" );
    ReplaceInString( aPtrTag, spXML33, sp33 );
    _LIT16( spXML34, "&frac34;" );
    _LIT16( sp34, "¾" );
    ReplaceInString( aPtrTag, spXML34, sp34 );
    _LIT16( spXML35, "&iquest;" );
    _LIT16( sp35, "¿" );
    ReplaceInString( aPtrTag, spXML35, sp35 );
    _LIT16( spXML36, "&Agrave;" );
    _LIT16( sp36, "À" );
    ReplaceInString( aPtrTag, spXML36, sp36 );
    _LIT16( spXML37, "&Aacute;" );
    _LIT16( sp37, "Á" );
    ReplaceInString( aPtrTag, spXML37, sp37 );
    _LIT16( spXML38, "&Acirc;" );
    _LIT16( sp38, "Â" );
    ReplaceInString( aPtrTag, spXML38, sp38 );
    _LIT16( spXML39, "&Atilde;" );
    _LIT16( sp39, "Ã" );
    ReplaceInString( aPtrTag, spXML39, sp39 );
    _LIT16( spXML40, "&Auml;" );
    _LIT16( sp40, "Ä" );
    ReplaceInString( aPtrTag, spXML40, sp40 );
    _LIT16( spXML41, "&Aring;" );
    _LIT16( sp41, "Å" );
    ReplaceInString( aPtrTag, spXML41, sp41 );
    _LIT16( spXML42, "&AElig;" );
    _LIT16( sp42, "Æ" );
    ReplaceInString( aPtrTag, spXML42, sp42 ); 
    _LIT16( spXML43, "&Ccedil;" );
    _LIT16( sp43, "Ç" );
    ReplaceInString( aPtrTag, spXML43, sp43 );
    _LIT16( spXML44, "&Egrave;" );
    _LIT16( sp44, "È" );
    ReplaceInString( aPtrTag, spXML44, sp44 );
    _LIT16( spXML45, "&Eacute;" );
    _LIT16( sp45, "É" );
    ReplaceInString( aPtrTag, spXML45, sp45 );
    _LIT16( spXML46, "&Ecirc;" );
    _LIT16( sp46, "Ê" );
    ReplaceInString( aPtrTag, spXML46, sp46 );
    _LIT16( spXML47, "&Euml;" );
    _LIT16( sp47, "Ë" );
    ReplaceInString( aPtrTag, spXML47, sp47 );
    _LIT16( spXML48, "&Igrave;" );
    _LIT16( sp48, "Ì" );
    ReplaceInString( aPtrTag, spXML48, sp48 );
    _LIT16( spXML49, "&Iacute;" );
    _LIT16( sp49, "Í" );
    ReplaceInString( aPtrTag, spXML49, sp49 );
    _LIT16( spXML50, "&Icirc;" );
    _LIT16( sp50, "Î" );
    ReplaceInString( aPtrTag, spXML50, sp50 );
    _LIT16( spXML51, "&Iuml;" );
    _LIT16( sp51, "Ï" );
    ReplaceInString( aPtrTag, spXML51, sp51 );
    _LIT16( spXML52, "&ETH;" );
    _LIT16( sp52, "Ð" );
    ReplaceInString( aPtrTag, spXML52, sp52 );
    _LIT16( spXML53, "&Ntilde;" );
    _LIT16( sp53, "Ñ" );
    ReplaceInString( aPtrTag, spXML53, sp53 );
    _LIT16( spXML54, "&Ograve;" );
    _LIT16( sp54, "Ò" );
    ReplaceInString( aPtrTag, spXML54, sp54 );
    _LIT16( spXML55, "&Oacute;" );
    _LIT16( sp55, "Ó" );
    ReplaceInString( aPtrTag, spXML55, sp55 );
    _LIT16( spXML56, "&Ocirc;" );
    _LIT16( sp56, "Ô" );
    ReplaceInString( aPtrTag, spXML56, sp56 );
    _LIT16( spXML57, "&Otilde;" );
    _LIT16( sp57, "Õ" );
    ReplaceInString( aPtrTag, spXML57, sp57 );
    _LIT16( spXML58, "&Ouml;" );
    _LIT16( sp58, "Ö" );
    ReplaceInString( aPtrTag, spXML58, sp58 );
    _LIT16( spXML59, "&times;" );
    _LIT16( sp59, "×" );
    ReplaceInString( aPtrTag, spXML59, sp59 );
    _LIT16( spXML60, "&Oslash;" );
    _LIT16( sp60, "Ø" );
    ReplaceInString( aPtrTag, spXML60, sp60 );
    _LIT16( spXML61, "&Ugrave;" );
    _LIT16( sp61, "Ù" );
    ReplaceInString( aPtrTag, spXML61, sp61 );
    _LIT16( spXML62, "&Uacute;" );
    _LIT16( sp62, "Ú" );
    ReplaceInString( aPtrTag, spXML62, sp62 );
    _LIT16( spXML63, "&Ucirc;" );
    _LIT16( sp63, "Û" );
    ReplaceInString( aPtrTag, spXML63, sp63 );
    _LIT16( spXML64, "&Uuml;" );
    _LIT16( sp64, "Ü" );
    ReplaceInString( aPtrTag, spXML64, sp64 );
    _LIT16( spXML65, "&Yacute;" );
    _LIT16( sp65, "Ý" );
    ReplaceInString( aPtrTag, spXML65, sp65 );
    _LIT16( spXML66, "&THORN;" );
    _LIT16( sp66, "Þ" );
    ReplaceInString( aPtrTag, spXML66, sp66 );
    _LIT16( spXML67, "&szlig;" );
    _LIT16( sp67, "ß" );
    ReplaceInString( aPtrTag, spXML67, sp67 );
    _LIT16( spXML68, "&agrave;" );
    _LIT16( sp68, "à" );
    ReplaceInString( aPtrTag, spXML68, sp68 );
    _LIT16( spXML69, "&aacute;" );
    _LIT16( sp69, "á" );
    ReplaceInString( aPtrTag, spXML69, sp69 );
    _LIT16( spXML70, "&acirc;" );
    _LIT16( sp70, "â" );
    ReplaceInString( aPtrTag, spXML70, sp70 );
    _LIT16( spXML71, "&atilde;" );
    _LIT16( sp71, "ã" );
    ReplaceInString( aPtrTag, spXML71, sp71 );
    _LIT16( spXML72, "&auml;");
    _LIT16( sp72, "ä" );
    ReplaceInString( aPtrTag, spXML72, sp72 );
    _LIT16( spXML73, "&aring;" );
    _LIT16( sp73, "å" );
    ReplaceInString( aPtrTag, spXML73, sp73 );
    _LIT16( spXML74, "&aelig;" );
    _LIT16( sp74, "æ" );
    ReplaceInString( aPtrTag, spXML74, sp74 );
    _LIT16( spXML75, "&acedil;" );
    _LIT16( sp75, "ç" );
    ReplaceInString( aPtrTag, spXML75, sp75 );
    _LIT16( spXML76, "&egrave;" );
    _LIT16( sp76, "è" );
    ReplaceInString( aPtrTag, spXML76, sp76 );
    _LIT16( spXML77, "&eacute;" );
    _LIT16( sp77, "é" );
    ReplaceInString( aPtrTag, spXML77, sp77 );
    _LIT16( spXML78, "&ecirc;" );
    _LIT16( sp78, "ê" );
    ReplaceInString( aPtrTag, spXML78, sp78 );
    _LIT16( spXML79, "&euml;" );
    _LIT16( sp79, "ë" );
    ReplaceInString( aPtrTag, spXML79, sp79 );
    _LIT16( spXML80, "&igrave;" );
    _LIT16( sp80, "ì" );
    ReplaceInString( aPtrTag, spXML80, sp80 );
    _LIT16( spXML81, "&iacute;" );
    _LIT16( sp81, "í" );
    ReplaceInString( aPtrTag, spXML81, sp81 );
    _LIT16( spXML82, "&icirc;" );
    _LIT16( sp82, "î" );
    ReplaceInString( aPtrTag, spXML82, sp82 );
    _LIT16( spXML83, "&iuml;" );
    _LIT16( sp83, "ï" );
    ReplaceInString( aPtrTag, spXML83, sp83 );
    _LIT16( spXML84, "&eth;" );
    _LIT16( sp84, "ð" );
    ReplaceInString( aPtrTag, spXML84, sp84 );
    _LIT16( spXML85, "&ntilde;" );
    _LIT16( sp85, "ñ" );
    ReplaceInString( aPtrTag, spXML85, sp85 );
    _LIT16( spXML86, "&ograve;" );
    _LIT16( sp86, "ò" );
    ReplaceInString( aPtrTag, spXML86, sp86 );
    _LIT16( spXML87, "&oacute;" );
    _LIT16( sp87, "ó" );
    ReplaceInString( aPtrTag, spXML87, sp87 );
    _LIT16( spXML88, "&ocirc;" );
    _LIT16( sp88, "ô" );
    ReplaceInString( aPtrTag, spXML88, sp88 );
    _LIT16( spXML89, "&otilde;" );
    _LIT16( sp89, "õ" );
    ReplaceInString( aPtrTag, spXML89, sp89 );
    _LIT16( spXML90, "&ouml;" );
    _LIT16( sp90, "ö" );
    ReplaceInString( aPtrTag, spXML90, sp90 );
    _LIT16( spXML91, "&divide;" );
    _LIT16( sp91, "÷" );
    ReplaceInString( aPtrTag, spXML91, sp91 );
    _LIT16( spXML92, "&oslash;" );
    _LIT16( sp92, "ø" );
    ReplaceInString( aPtrTag, spXML92, sp92 );
    _LIT16( spXML93, "&ugrave;" );
    _LIT16( sp93, "ù" );
    ReplaceInString( aPtrTag, spXML93, sp93 );
    _LIT16( spXML94, "&uacute;" );
    _LIT16( sp94, "ú" );
    ReplaceInString( aPtrTag, spXML94, sp94 );
    _LIT16( spXML95, "&ucirc;" );
    _LIT16( sp95, "û" );
    ReplaceInString( aPtrTag, spXML95, sp95 );
    _LIT16( spXML96, "&uuml;" );
    _LIT16( sp96, "ü" );
    ReplaceInString( aPtrTag, spXML96, sp96 );
    _LIT16( spXML97, "&yacute;" );
    _LIT16( sp97, "ý" );
    ReplaceInString( aPtrTag, spXML97, sp97 );
    _LIT16( spXML98, "&thorn;" );
    _LIT16( sp98, "þ" );
    ReplaceInString( aPtrTag, spXML98, sp98 );
    _LIT16( spXML99, "&yuml;" );
    _LIT16( sp99, "ÿ" );
    ReplaceInString( aPtrTag, spXML99, sp99 );
    _LIT16( spXML100, "&OElig;" );
    _LIT16( sp100, "Œ" );
    ReplaceInString( aPtrTag, spXML100, sp100 );
    _LIT16( spXML101, "&oelig;" );
    _LIT16( sp101, "œ" );
    ReplaceInString( aPtrTag, spXML101, sp101 );
    _LIT16( spXML102, "&Scaron;" );
    _LIT16( sp102, "Š" );
    ReplaceInString( aPtrTag, spXML102, sp102 );
    _LIT16( spXML103, "&scaron;" );
    _LIT16( sp103, "š" );
    ReplaceInString( aPtrTag, spXML103, sp103 );
    _LIT16( spXML104, "&Yuml;" );
    _LIT16( sp104, "Ÿ" );
    ReplaceInString( aPtrTag, spXML104, sp104 );
    _LIT16( spXML105, "&fnof;" );
    _LIT16( sp105, "ƒ" );
    ReplaceInString( aPtrTag, spXML105, sp105 );
    _LIT16( spXML106, "&circ;" );
    _LIT16( sp106, "ˆ" );
    ReplaceInString( aPtrTag, spXML106, sp106 );
    _LIT16( spXML107, "&tilde;" );
    _LIT16( sp107, "˜" );
    ReplaceInString( aPtrTag, spXML107, sp107 );
    }

// ---------------------------------------------------------
// CReaderXML::Replace
// ---------------------------------------------------------
//
void CReaderXML::ReplaceInString(   HBufC* aPtrTag, 
                                const TDesC& aSpXML, 
                                const TDesC& aSp )
    {
        if( aPtrTag == NULL )
        {
            return;
        }
    TInt strPos = aPtrTag->Des().Find(aSpXML);
    while ( strPos != KErrNotFound )
        {
        aPtrTag->Des().Replace(strPos, aSpXML.Length() , aSp);
        strPos = aPtrTag->Des().Find(aSpXML);     
        }
    }   

//maximum number of VPN AP parameters


void CReaderXML::FillPacketDataParams()    
    {
    iPDParams.Append( EInputParams( ECmName,                          KConnectionName ) );
    iPDParams.Append( EInputParams( ECmId,                            KConnectionId ) );
    iPDParams.Append( EInputParams( ECmProtected,                     KProtected ) );
    iPDParams.Append( EInputParams( ECmHidden,                        KHidden ) );
    iPDParams.Append( EInputParams( ECmMetaHiddenAgent,               KHiddenAgent ) );
    iPDParams.Append( EInputParams( ECmMetaHighlight,                 KHighlighted ) );
    iPDParams.Append( EInputParams( ECmSeamlessnessLevel,             KSeamlessness ) );
    iPDParams.Append( EInputParams( EPacketDataAPName,                KGprs_AP_Name ) );
    iPDParams.Append( EInputParams( EPacketDataIFAuthName,            KUserName ) );
    iPDParams.Append( EInputParams( EPacketDataIFPromptForAuth,       KPromptPassword ) );
    iPDParams.Append( EInputParams( EPacketDataIFAuthPass,            KPassword ) );
    iPDParams.Append( EInputParams( EPacketDataDisablePlainTextAuth,  KPasswordAuthType ) );
    iPDParams.Append( EInputParams( ECmWapIPGatewayAddress,           KWAPGatewayIP ) );
    iPDParams.Append( EInputParams( ECmStartPage,                     KStartingPage ) );
    iPDParams.Append( EInputParams( ECmWapIPSecurity,                 KWTLS_Security ) );
    iPDParams.Append( EInputParams( ECmWapIPWSPOption,                KWAP_ConnectionType ) );
    iPDParams.Append( EInputParams( EPacketDataIPAddr,                KPhoneIP ) );
    iPDParams.Append( EInputParams( EPacketDataIPNameServer1,         KPrimaryNameServer ) );
    iPDParams.Append( EInputParams( EPacketDataIPNameServer2,         KSecondaryNameServer ) );
    iPDParams.Append( EInputParams( ECmProxyServerName,               KProxyServerAddress ) );
    iPDParams.Append( EInputParams( ECmProxyPortNumber,               KProxyPortNumber ) );
    iPDParams.Append( EInputParams( ECmProxyProtocolName,             KProxyProtocolName ) );
    iPDParams.Append( EInputParams( EPacketDataIPIP6NameServer1,      KPrimaryIP6NameServer ) );
    iPDParams.Append( EInputParams( EPacketDataIPIP6NameServer2,      KSecondaryIP6NameServer ) );
    iPDParams.Append( EInputParams( EPacketDataApType,                KIspType ) );
    iPDParams.Append( EInputParams( EPacketDataPDPType,               KNetworkType ) );
    iPDParams.Append( EInputParams( EPacketDataUseEdge,               KGprsUseEdge ) );
    iPDParams.Append( EInputParams( EPacketDataLinger,                KLinger ) );
    };

void CReaderXML::FillLanParams()    
    {
    iLanParams.Append( EInputParams( ECmName,                           KConnectionName ) );
    iLanParams.Append( EInputParams( ECmId,                             KConnectionId ) );
    iLanParams.Append( EInputParams( ECmProtected,                      KProtected ) );
    iLanParams.Append( EInputParams( ECmHidden,                         KHidden ) );
    iLanParams.Append( EInputParams( ECmMetaHiddenAgent,                KHiddenAgent ) );
    iLanParams.Append( EInputParams( ECmMetaHighlight,                  KHighlighted ) );
    iLanParams.Append( EInputParams( ECmSeamlessnessLevel,              KSeamlessness ) );
    iLanParams.Append( EInputParams( ECmWapIPGatewayAddress,            KWAPGatewayIP ) );
    iLanParams.Append( EInputParams( ECmStartPage,                      KStartingPage ) );
    iLanParams.Append( EInputParams( ECmWapIPSecurity,                  KWTLS_Security ) );
    iLanParams.Append( EInputParams( ECmWapIPWSPOption,                 KWAP_ConnectionType ) );
    iLanParams.Append( EInputParams( ECmProxyServerName,                KProxyServerAddress ) );
    iLanParams.Append( EInputParams( ECmProxyPortNumber,                KProxyPortNumber ) );
    iLanParams.Append( EInputParams( ECmProxyProtocolName,              KProxyProtocolName ) );
    iLanParams.Append( EInputParams( ELanIfNetworks,                    KLanIfNetworks ) );
    iLanParams.Append( EInputParams( ELanIpNetMask,                     KLanIpNetMask ) );
    iLanParams.Append( EInputParams( ELanIpGateway,                     KLanIpGateway ) );
    iLanParams.Append( EInputParams( ELanIpAddr,                        KPhoneIP ) );
    iLanParams.Append( EInputParams( ELanIpNameServer1,                 KPrimaryNameServer ) );
    iLanParams.Append( EInputParams( ELanIpNameServer2,                 KSecondaryNameServer ) );
    iLanParams.Append( EInputParams( ELanIp6NameServer1,                KPrimaryIP6NameServer ) );
    iLanParams.Append( EInputParams( ELanIp6NameServer2,                KSecondaryIP6NameServer ) );
    iLanParams.Append( EInputParams( ELanIpAddrFromServer,              KLanIpAddrFromServer ) );
    iLanParams.Append( EInputParams( ELanIpDNSAddrFromServer,           KLanIpDNSAddrFromServer ) );
    iLanParams.Append( EInputParams( ELanConfigDaemonManagerName,       KLanConfigDaemonManagerName ) );
    iLanParams.Append( EInputParams( ELanConfigDaemonName,              KLanConfigDaemonName ) );
    };
    
//Binds WLAN access point parameters to field IDs
void CReaderXML::FillWLanParams()    
    {        
    iWLanParams.Append( EInputParams( ECmName,                      KConnectionName ) );
    iWLanParams.Append( EInputParams( ECmId,                        KConnectionId ) );
    iWLanParams.Append( EInputParams( ECmProtected,                 KProtected ) );
    iWLanParams.Append( EInputParams( ECmHidden,                    KHidden ) );
    iWLanParams.Append( EInputParams( ECmMetaHiddenAgent,           KHiddenAgent ) );
    iWLanParams.Append( EInputParams( ECmMetaHighlight,             KHighlighted ) );
    iWLanParams.Append( EInputParams( ECmSeamlessnessLevel,         KSeamlessness ) );
    iWLanParams.Append( EInputParams( ECmStartPage,                 KStartingPage ) );
    iWLanParams.Append( EInputParams( EWlanSSID,                    KNetworkName ) );
    iWLanParams.Append( EInputParams( EWlanConnectionMode,          KNetworkMode ) );
    iWLanParams.Append( EInputParams( EWlanSecurityMode,            KSecurityMode ) );
    iWLanParams.Append( EInputParams( EWlanIpNameServer1,           KPrimaryNameServer ) );
    iWLanParams.Append( EInputParams( EWlanIpNameServer2,           KSecondaryNameServer ) );
    iWLanParams.Append( EInputParams( EWlanIp6NameServer1,          KPrimaryIP6NameServer ) );
    iWLanParams.Append( EInputParams( EWlanIp6NameServer2,          KSecondaryIP6NameServer ) );
    iWLanParams.Append( EInputParams( EWlanIpGateway,               KWlanIpGatewayAddress ) );
    iWLanParams.Append( EInputParams( ECmProxyServerName,           KProxyServerAddress ) );
    iWLanParams.Append( EInputParams( ECmProxyProtocolName,         KProxyProtocolName ) );
    iWLanParams.Append( EInputParams( EWlanIpNetMask,               KWlanIpNetMask ) );
    iWLanParams.Append( EInputParams( ECmProxyPortNumber,           KProxyPortNumber ) );
    iWLanParams.Append( EInputParams( EWlanIpAddr,                  KPhoneIP ) ); 
    iWLanParams.Append( EInputParams( EWlanScanSSID,                KWlanScanSSId ) );
    iWLanParams.Append( EInputParams( EWlanChannelID,               KWlanChannelId ) );
    iWLanParams.Append( EInputParams( EWEPKeyInUse,                 KWEPKeyInUse ) );
    iWLanParams.Append( EInputParams( EWEPAuthType,                 KWEPAuthType ) );
    iWLanParams.Append( EInputParams( EWEPKey1Length,               KWEPKey1Length ) );
    iWLanParams.Append( EInputParams( EWEPKey1Format,               KWEPKey1Format ) );
    iWLanParams.Append( EInputParams( EWEPKey1Data,                 KWEPKey1Data ) );
    iWLanParams.Append( EInputParams( EWEPKey2Length,               KWEPKey2Length ) );
    iWLanParams.Append( EInputParams( EWEPKey2Format,               KWEPKey2Format ) );
    iWLanParams.Append( EInputParams( EWEPKey2Data,                 KWEPKey2Data ) );
    iWLanParams.Append( EInputParams( EWEPKey3Length,               KWEPKey3Length ) );
    iWLanParams.Append( EInputParams( EWEPKey3Format,               KWEPKey3Format ) );
    iWLanParams.Append( EInputParams( EWEPKey3Data,                 KWEPKey3Data ) );
    iWLanParams.Append( EInputParams( EWEPKey4Length,               KWEPKey4Length ) );
    iWLanParams.Append( EInputParams( EWEPKey4Format,               KWEPKey4Format ) );
    iWLanParams.Append( EInputParams( EWEPKey4Data,                 KWEPKey4Data ) );
    iWLanParams.Append( EInputParams( EWPAPresharedKey,             KWPAPresharedKey ) );
    iWLanParams.Append( EInputParams( EWPAKeyLength,                KWPAKeyLength ) );
    iWLanParams.Append( EInputParams( EWPAListOfEAPs,               KWPAListOfEAPs ) );
    iWLanParams.Append( EInputParams( EWPAUseOfPresharedKey,        KWPAUseOfPresharedKey ) );
    iWLanParams.Append( EInputParams( EEapGtcUsername,              KEapGtcUsername ) );
    iWLanParams.Append( EInputParams( EEapGtcSessionValidityTime,   KEapGtcSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapGtcEncapsulation,         KEapGtcEncapsulation ) );
    iWLanParams.Append( EInputParams( EEapTlsUsername,              KEapTlsUsername ) );
    iWLanParams.Append( EInputParams( EEapTlsRealm,                 KEapTlsRealm ) );
    iWLanParams.Append( EInputParams( EEapTlsVerifyServerRealm,     KEapTlsVerifyServerRealm ) );
    iWLanParams.Append( EInputParams( EEapTlsRequireClientAuth,     KEapTlsRequireClientAuth ) );
    iWLanParams.Append( EInputParams( EEapTlsSessionValidityTime,   KEapTlsSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapTlsCipherSuites,          KEapTlsCipherSuites ) );
    iWLanParams.Append( EInputParams( EEapTlsUserCertSubjectKeyId,  KEapTlsUserCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapTlsUserCertIssuerName,    KEapTlsUserCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapTlsUserCertSerialNumber,  KEapTlsUserCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapTlsCaCertSubjectKeyId,    KEapTlsCaCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapTlsCaCertIssuerName,      KEapTlsCaCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapTlsCaCertSerialNumber,    KEapTlsCaCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapTlsEncapsulation,         KEapTlsEncapsulation ) );
    iWLanParams.Append( EInputParams( EEapLeapUsername,             KEapLeapUsername ) );
    iWLanParams.Append( EInputParams( EEapLeapPassword,             KEapLeapPassword ) );
    iWLanParams.Append( EInputParams( EEapLeapSessionValidityTime,  KEapLeapSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapSimUsername,              KEapSimUsername ) );
    iWLanParams.Append( EInputParams( EEapSimRealm,                 KEapSimRealm ) );
    iWLanParams.Append( EInputParams( EEapSimUsePseudonyms,         KEapSimUsePseudonyms ) );
    iWLanParams.Append( EInputParams( EEapSimSessionValidityTime,   KEapSimSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapSimEncapsulation,         KEapSimEncapsulation ) );
    iWLanParams.Append( EInputParams( EEapTtlsUsername,             KEapTtlsUsername ) );
    iWLanParams.Append( EInputParams( EEapTtlsRealm,                KEapTtlsRealm ) );
    iWLanParams.Append( EInputParams( EEapTtlsVerifyServerRealm,    KEapTtlsVerifyServerRealm ) );
    iWLanParams.Append( EInputParams( EEapTtlsRequireClientAuth,    KEapTtlsRequireClientAuth ) );
    iWLanParams.Append( EInputParams( EEapTtlsSessionValidityTime,  KEapTtlsSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapTtlsCipherSuites,         KEapTtlsCipherSuites ) );
    iWLanParams.Append( EInputParams( EEapTtlsEncapsulatedTypes,    KEapTtlsEncapsulatedTypes ) );
    iWLanParams.Append( EInputParams( EEapTtlsUserCertSubjectKeyId, KEapTtlsUserCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapTtlsUserCertIssuerName,   KEapTtlsUserCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapTtlsUserCertSerialNumber, KEapTtlsUserCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapTtlsCaCertSubjectKeyId,   KEapTtlsCaCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapTtlsCaCertIssuerName,     KEapTtlsCaCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapTtlsCaCertSerialNumber,   KEapTtlsCaCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapAkaUsername,              KEapAkaUsername ) );
    iWLanParams.Append( EInputParams( EEapAkaRealm,                 KEapAkaRealm ) );
    iWLanParams.Append( EInputParams( EEapAkaUsePseudonyms,         KEapAkaUsePseudonyms ) );
    iWLanParams.Append( EInputParams( EEapAkaSessionValidityTime,   KEapAkaSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapAkaEncapsulation,         KEapAkaEncapsulation ) );
    iWLanParams.Append( EInputParams( EEapPeapUsername,             KEapPeapUsername ) );
    iWLanParams.Append( EInputParams( EEapPeapRealm,                KEapPeapRealm ) );
    iWLanParams.Append( EInputParams( EEapPeapVerifyServerRealm,    KEapPeapVerifyServerRealm ) );
    iWLanParams.Append( EInputParams( EEapPeapRequireClientAuth,    KEapPeapRequireClientAuth ) );
    iWLanParams.Append( EInputParams( EEapPeapSessionValidityTime,  KEapPeapSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapPeapCipherSuites,         KEapPeapCipherSuites ) );
    iWLanParams.Append( EInputParams( EEapPeapV0Allowed,            KEapPeapV0Allowed ) );
    iWLanParams.Append( EInputParams( EEapPeapV1Allowed,            KEapPeapV1Allowed ) );
    iWLanParams.Append( EInputParams( EEapPeapV2Allowed,            KEapPeapV2Allowed ) );
    iWLanParams.Append( EInputParams( EEapPeapEncapsulatedTypes,    KEapPeapEncapsulatedTypes ) );
    iWLanParams.Append( EInputParams( EEapPeapUserCertSubjectKeyId, KEapPeapUserCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapPeapUserCertIssuerName,   KEapPeapUserCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapPeapUserCertSerialNumber, KEapPeapUserCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapPeapCaCertSubjectKeyId,   KEapPeapCaCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapPeapCaCertIssuerName,     KEapPeapCaCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapPeapCaCertSerialNumber,   KEapPeapCaCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapMschapv2Username,         KEapMschapv2Username ) );
    iWLanParams.Append( EInputParams( EEapMschapv2Password,         KEapMschapv2Password ) );
    iWLanParams.Append( EInputParams( EEapMschapv2SessionValidityTime, KEapMschapv2SessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapMschapv2Encapsulation,    KEapMschapv2Encapsulation ) );
    iWLanParams.Append( EInputParams( EEapFastUsername,             KEapFastUsername ) );
    iWLanParams.Append( EInputParams( EEapFastRealm,                KEapFastRealm ) );
    iWLanParams.Append( EInputParams( EEapFastVerifyServerRealm,    KEapFastVerifyServerRealm ) );
    iWLanParams.Append( EInputParams( EEapFastRequireClientAuth,    KEapFastRequireClientAuth ) );
    iWLanParams.Append( EInputParams( EEapFastSessionValidityTime,  KEapFastSessionValidityTime ) );
    iWLanParams.Append( EInputParams( EEapFastCipherSuites,         KEapFastCipherSuites ) );
    iWLanParams.Append( EInputParams( EEapFastEncapsulatedTypes,    KEapFastEncapsulatedTypes ) );
    iWLanParams.Append( EInputParams( EEapFastAuthProvModeAllowed,  KEapFastAuthProvModeAllowed ) );
    iWLanParams.Append( EInputParams( EEapFastUnauthProvModeAllowed, KEapFastUnauthProvModeAllowed ) );
    iWLanParams.Append( EInputParams( EEapFastWarnADHPNoPAC,        KEapFastWarnADHPNoPAC ) );
    iWLanParams.Append( EInputParams( EEapFastWarnADHPNoMatchingPAC, KEapFastWarnADHPNoMatchingPAC ) );
    iWLanParams.Append( EInputParams( EEapFastWarnNotDefaultServer, KEapFastWarnNotDefaultServer ) );
    iWLanParams.Append( EInputParams( EEapFastUserCertSubjectKeyId, KEapFastUserCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapFastUserCertIssuerName,   KEapFastUserCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapFastUserCertSerialNumber, KEapFastUserCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EEapFastCaCertSubjectKeyId,   KEapFastCaCertSubjectKeyId ) );
    iWLanParams.Append( EInputParams( EEapFastCaCertIssuerName,     KEapFastCaCertIssuerName ) );
    iWLanParams.Append( EInputParams( EEapFastCaCertSerialNumber,   KEapFastCaCertSerialNumber ) );
    iWLanParams.Append( EInputParams( EMschapv2Username,            KMschapv2Username ) );
    iWLanParams.Append( EInputParams( EMschapv2Password,            KMschapv2Password ) );
    iWLanParams.Append( EInputParams( EMschapv2SessionValidityTime, KMschapv2SessionValidityTime ) );
    iWLanParams.Append( EInputParams( EMschapv2Encapsulation, 		KMschapv2Encapsulation ) );        
    };
    
    

void CReaderXML::FillVpnParams()    
    {
    iVpnParams.Reset();
        
    iVpnParams.Append( EInputParams( ECmName,              KConnectionName ) );
    iVpnParams.Append( EInputParams( ECmId,                KConnectionId ) );
    iVpnParams.Append( EInputParams( ECmProtected,         KProtected ) );
    iVpnParams.Append( EInputParams( ECmHidden,            KHidden ) );
    iVpnParams.Append( EInputParams( ECmMetaHiddenAgent,   KHiddenAgent ) );
    iVpnParams.Append( EInputParams( ECmMetaHighlight,     KHighlighted ) );
    iVpnParams.Append( EInputParams( ECmSeamlessnessLevel, KSeamlessness ) );
    iVpnParams.Append( EInputParams( ECmProxyServerName,   KProxyServerAddress ) );
    iVpnParams.Append( EInputParams( ECmProxyPortNumber,   KProxyPortNumber ) );
    iVpnParams.Append( EInputParams( ECmProxyProtocolName, KProxyProtocolName ) );
    iVpnParams.Append( EInputParams( EVPN_IapName,         KVpnIapName ) );
    iVpnParams.Append( EInputParams( EVpnServicePolicy,    KVpnServicePolicy ) );

    };    

//Binds Gobal settings parameters to field IDs
void CReaderXML::FillGlobalParams()    
    {         
    iGlobalParams.Append( EInputParams( EGPRSAttachMode,        KAttachMode ) );
    iGlobalParams.Append( EInputParams( EDefaultAP,             KDefaultAP) );
    iGlobalParams.Append( EInputParams( EDefaultDnIcon,         KDefaultDnIcon ) );
    iGlobalParams.Append( EInputParams( EPriorityLan,           KPriorityLan ) );
    iGlobalParams.Append( EInputParams( EPriorityWlan,          KPriorityWlan ) );
    iGlobalParams.Append( EInputParams( EPriorityPan,           KPriorityPan ) );
    iGlobalParams.Append( EInputParams( EPriorityGprsOut,       KPriorityGprsOut ) );
    iGlobalParams.Append( EInputParams( EPriorityGprsIn,        KPriorityGprsIn ) );
    iGlobalParams.Append( EInputParams( EPriorityCdma2k,        KPriorityCdma2k ) );
    iGlobalParams.Append( EInputParams( EPriorityDialOut,       KPriorityDialOut ) );
    iGlobalParams.Append( EInputParams( EPriorityDialIn,        KPriorityDialIn ) );
    iGlobalParams.Append( EInputParams( EPriorityVpn,           KPriorityVpn ) );
    iGlobalParams.Append( EInputParams( EPriorityMip,           KPriorityMip ) );
    iGlobalParams.Append( EInputParams( EUIPriorityLan,         KUIPriorityLan ) );
    iGlobalParams.Append( EInputParams( EUIPriorityWlan,        KUIPriorityWlan ) );
    iGlobalParams.Append( EInputParams( EUIPriorityPan,         KUIPriorityPan ) );
    iGlobalParams.Append( EInputParams( EUIPriorityGprsOut,     KUIPriorityGprsOut ) );
    iGlobalParams.Append( EInputParams( EUIPriorityGprsIn,      KUIPriorityGprsIn ) );
    iGlobalParams.Append( EInputParams( EUIPriorityCdma2k,      KUIPriorityCdma2k ) );
    iGlobalParams.Append( EInputParams( EUIPriorityDialOut,     KUIPriorityDialOut ) );
    iGlobalParams.Append( EInputParams( EUIPriorityDialIn,      KUIPriorityDialIn ) );
    iGlobalParams.Append( EInputParams( EUIPriorityVpn,         KUIPriorityVpn ) );
    iGlobalParams.Append( EInputParams( EUIPriorityMip,         KUIPriorityMip ) );
    iGlobalParams.Append( EInputParams( EDefaultConnectionType, KDefaultConnectionType ) );
    iGlobalParams.Append( EInputParams( EDefaultConnectionName, KDefaultConnectionName ) );
    iGlobalParams.Append( EInputParams( EUsageOfWlan,           KUsageOfWlan ) );
    iGlobalParams.Append( EInputParams( ESeamlessnessHome,      KSeamlessnessHome ) );
    iGlobalParams.Append( EInputParams( ESeamlessnessVisitor,   KSeamlessnessVisitor ) );
    iGlobalParams.Append( EInputParams( EGprsLastSocketActivityTimeout, KGprsLastSocketActivityTimeout ) );
    iGlobalParams.Append( EInputParams( EGprsLastSessionClosedTimeout,  KGprsLastSessionClosedTimeout ) );
    iGlobalParams.Append( EInputParams( EGprsLastSocketClosedTimeout,   KGprsLastSocketClosedTimeout ) );
    iGlobalParams.Append( EInputParams( EWlanLastSocketActivityTimeout, KWlanLastSocketActivityTimeout ) );
    iGlobalParams.Append( EInputParams( EWlanLastSessionClosedTimeout,  KWlanLastSessionClosedTimeout ) );
    iGlobalParams.Append( EInputParams( EWlanLastSocketClosedTimeout,   KWlanLastSocketClosedTimeout ) );
    iGlobalParams.Append( EInputParams( EWlanBGScanInterval,    KWlanBGScanInterval ) );
    iGlobalParams.Append( EInputParams( EWlanUseDefSettings,    KWlanUseDefSettings ) );
    iGlobalParams.Append( EInputParams( EWlanLongRetry,         KWlanLongRetry ) );
    iGlobalParams.Append( EInputParams( EWlanShortRetry,        KWlanShortRetry ) );
    iGlobalParams.Append( EInputParams( EWlanRTS,               KWlanRTS ) );
    iGlobalParams.Append( EInputParams( EWlanTxPowerLevel,      KWlanTxPowerLevel ) );
    iGlobalParams.Append( EInputParams( EWlanRadioMeasurements, KWlanRadioMeasurements ) );
    iGlobalParams.Append( EInputParams( EWlanPowerMode,         KWlanPowerMode ) );
    
    };

//Describes a Destination Network
void CReaderXML::FillDNParams()    
    {        
    iDNParams.Append( EInputParams( EDN_Name,           KDN_Name ) );
    iDNParams.Append( EInputParams( EDN_Id,             KDN_Id ) );
    iDNParams.Append( EInputParams( EDN_Icon,           KDN_Icon ) );
    iDNParams.Append( EInputParams( EDN_Metadata,       KDN_Metadata ) );
    iDNParams.Append( EInputParams( EDN_Protection,     KDN_Protection ) );
    iDNParams.Append( EInputParams( EDN_Hidden,         KHidden ) );
    iDNParams.Append( EInputParams( EDN_HiddenAgent,    KHiddenAgent ) );
    iDNParams.Append( EInputParams( EDN_Highlighted,    KHighlighted ) );
    iDNParams.Append( EInputParams( EDN_EmbeddedDNName, KDN_EmbeddedDNName ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName ) );
    // Needed because of Customization Tool limitations
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName2 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName3 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName4 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName5 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName6 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName7 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName8 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName9 ) );
    iDNParams.Append( EInputParams( EDN_IAPName,        KDN_IAPName10 ) );   
    };

        
// End of File.
