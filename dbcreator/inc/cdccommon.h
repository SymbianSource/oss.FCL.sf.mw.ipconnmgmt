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
* Description:   Declaration of enums and const for DbCreator
*
*/




#ifndef CDCCOMMON_H
#define CDCCOMMON_H

// Possible input files
enum TDbCreatorInputFile
    {
    EFileUnknown    = 0x00000000,     // Unrecognized
    EFileXML        = 0x00000001      // XML file
    };
    
// Possible character sets of input file
enum TDbCreatorInputCharSet
    {
    ECharSetAscii   = 0x00000000,     // ASCII
    ECharSetUnicode = 0x00000001,     // Unicode
    ECharSetUtf8    = 0x00000002,     // UTF-8
    ECharSetUnknown = 0x00000003      // Unknown
    };

// Feaures DbCreator can recognize (from XML input)
enum TDbCreatorFeatures
    {
    EFeatureNone       = 0x00000000,     // No feature found
    EFeaturePacketData = 0x00000001,     // GPRS access point
    EFeatureWLAN       = 0x00000002,     // WLAN access point
    EFeatureLAN        = 0x00000003,     // LAN access point
    EFeatureCSD        = 0x00000004,     // CSD access point
    EFeatureHSCSD      = 0x00000005,     // HSCSD access point
    EFeatureVPN        = 0x00000006,     // VPN access point
    EFeatureDN         = 0x00000007,     // Destination Network
    EFeatureGlobal     = 0x00000008      // Global settings
    };
    
// Global settings
enum TDbCreatorGlobalSettings
    {
    EGPRSAttachMode       = 0x00000000,    
    EDefaultDnIcon        = 0x00000001,
    EPriorityLan          = 0x00000002,
    EPriorityWlan         = 0x00000003,
    EPriorityPan          = 0x00000004,
    EPriorityGprsIn       = 0x00000005,
    EPriorityGprsOut      = 0x00000006,
    EPriorityCdma2k       = 0x00000007,
    EPriorityDialOut      = 0x00000009,
    EPriorityDialIn       = 0x0000000a,
    EPriorityVpn          = 0x0000000b,
    EPriorityMip          = 0x0000000c,
    EUIPriorityLan        = 0x0000000d,
    EUIPriorityWlan       = 0x0000000e,
    EUIPriorityPan        = 0x0000000f,
    EUIPriorityGprsIn     = 0x00000010,
    EUIPriorityGprsOut    = 0x00000011,
    EUIPriorityCdma2k     = 0x00000012,
    EUIPriorityDialOut    = 0x00000013,
    EUIPriorityDialIn     = 0x00000014,
    EUIPriorityVpn        = 0x00000015,
    EUIPriorityMip        = 0x00000016,
    EDefaultConnectionType= 0x00000017,
    EDefaultConnectionName= 0x00000018,
    EGprsLastSocketActivityTimeout	= 0x00000019, 
    EGprsLastSessionClosedTimeout	= 0x0000001a,
    EGprsLastSocketClosedTimeout	= 0x0000001b,
    ECsdLastSocketActivityTimeout	= 0x0000001c,
    ECsdLastSessionClosedTimeout	= 0x0000001d,
    ECsdLastSocketClosedTimeout		= 0x0000001e,
    EWlanLastSocketActivityTimeout	= 0x0000001f,
    EWlanLastSessionClosedTimeout	= 0x00000020,
    EWlanLastSocketClosedTimeout	= 0x00000021,
    EWlanBGScanInterval		  = 0x00000022,
    EWlanUseDefSettings		  = 0x00000023,
    EWlanLongRetry			  = 0x00000024,
    EWlanShortRetry			  = 0x00000025,
    EWlanRTS				  = 0x00000026,
    EWlanTxPowerLevel		  = 0x00000027,
    EWlanRadioMeasurements	  = 0x00000028,
    EWlanPowerMode			  = 0x00000029,    
    EDefaultAP				  = 0x0000002a,
    EUsageOfWlan              = 0x0000002b,
    ECellularDataUsageHome    = 0x0000002c,
    ECellularDataUsageVisitor = 0x0000002d,
    EUnused               	  = 0x00010000
    };
    
    
// Destination Network parameters
enum TDbCreatorDNParams
    {
    EDN_Name           = 0x00000000,
    EDN_Icon           = 0x00000001,
    EDN_Metadata       = 0x00000002,
    EDN_Protection     = 0x00000003,
    EDN_Hidden         = 0x00000004,
    EDN_HiddenAgent    = 0x00000005,
    EDN_Highlighted    = 0x00000006,
    EDN_IAPName        = 0x00000007,
    EDN_EmbeddedDNName = 0x00000008,
    EDN_Id             = 0x00000009
    };
    
// VPN parameters
enum TDbCreatorVpnParams
    {
    EVPN_IapName       = 0x00000000
    
    };
    
//WLAN Security settings 
enum TDbCreatorSecurityMode
    {
    ESecurityModeOpen = 0x00000001,   // open mode
    ESecurityModeWEP  = 0x00000002,   // WEP security mode
    ESecurityMode802  = 0x00000004,   // 802.1X security mode
    ESecurityModeWPA  = 0x00000008,   // Wpa security mode
    ESecurityModeWPA2 = 0x00000010    // Wpa2 only security mode 
    };        
    
//WEP related field ids
enum TDbCreatorWEPFields
    {
    EWEPKeyInUse    = 0x00001000,     // Key in use
    EWEPAuthType    = 0x00001001,     // Authentication type
    EWEPKey1Length  = 0x00001002,     // Key 1 length
    EWEPKey1Format  = 0x00001003,     // Key 1 format
    EWEPKey1Data    = 0x00001004,     // Key 1 data
    EWEPKey2Length  = 0x00001005,     // Key 2 length
    EWEPKey2Format  = 0x00001006,     // Key 2 format
    EWEPKey2Data    = 0x00001007,     // Key 2 data
    EWEPKey3Length  = 0x00001008,     // Key 3 length
    EWEPKey3Format  = 0x00001009,     // Key 3 format 
    EWEPKey3Data    = 0x0000100a,     // Key 3 data
    EWEPKey4Length  = 0x0000100b,     // Key 4 length
    EWEPKey4Format  = 0x0000100c,     // Key 4 format 
    EWEPKey4Data    = 0x0000100d,     // Key 4 data
    };

//number of WEP fields
const TInt KWEPKeyNumOfFields = 14;
    
//WPA related field ids
enum TDbCreatorWPAFields
    {
    EWPAPresharedKey = 0x00002000,    // Preshared key 
    EWPAKeyLength    = 0x00002001,    // Key length
    EWPAEapMethod    = 0x00002002,    // EAP Method
    EWPAUseOfPresharedKey =0x00002003,// Use of preshared key
    };
    
//number of WPA fields
const TInt KWPAKeyNumOfFields = 4;

// Enumeration of the possible keys in use
enum TWEPKeyInUse
    {
    EKeyNumber1,                      // Key number 1
    EKeyNumber2,                      // Key number 2
    EKeyNumber3,                      // Key number 3
    EKeyNumber4                       // Key number 4
    };

// Enumeration of the possible authentication types
enum TWEPAuthentication
    {
    EAuthOpen,                        // Open authentication
    EAuthShared                       // Shared authentication
    };

// Possible lengths of the keys
enum TWEPKeyLength
    {
    E40Bits,                          // 40 bits
    E104Bits,                         // 104 bits
    E232Bits                          // 232 bits
    };

// Possible formats of the keys
enum TWEPKeyFormat
    {
    EAscii,                           // Ascii format
    EHexadecimal                      // Hex format
    };

// Empty key
LOCAL_D const TUint KKeyDataLengthEmpty = 0;

// Number of characters for a 40 bits key
LOCAL_D const TUint KKeyDataLength40Bits  = 10;

// Number of characters for a 104 bits key
LOCAL_D const TUint KKeyDataLength104Bits = 26;

// Number of characters for a 232 bits key
LOCAL_D const TUint KKeyDataLength232Bits = 58;

// The maximum length of key data
LOCAL_D const TUint KMaxLengthOfKeyData = KKeyDataLength232Bits;

// Number of keys
LOCAL_D const TUint KMaxNumberofKeys = 4;

//EAP related field ids
enum TDbCreatorEAPFields
	{
	EEapParameterFirst = 0x0002fff,
	EEapGtcUsername = 0x00003000,
	EEapGtcSessionValidityTime = 0x00003001,
	EEapGtcEncapsulation = 0x00003002,
	EEapTlsUsername = 0x00003053,
	EEapTlsRealm = 0x00003003,
	EEapTlsVerifyServerRealm = 0x00003004,
	EEapTlsRequireClientAuth = 0x00003005,
	EEapTlsSessionValidityTime = 0x00003006,
	EEapTlsCipherSuites = 0x00003007,
	EEapTlsUserCertSubjectKeyId = 0x00003008,
	EEapTlsUserCertIssuerName = 0x00003009,
	EEapTlsUserCertSerialNumber = 0x0000300a,
	EEapTlsCaCertSubjectKeyId = 0x0000300b,
	EEapTlsCaCertIssuerName = 0x0000300c,
	EEapTlsCaCertSerialNumber = 0x0000300d,
	EEapTlsEncapsulation = 0x0000300e,
	EEapLeapUsername = 0x0000300f,
	EEapLeapPassword = 0x00003010,
	EEapLeapSessionValidityTime = 0x00003011,
	EEapSimUsername = 0x00003012,
	EEapSimRealm = 0x00003013,
	EEapSimUsePseudonyms = 0x00003014,
	EEapSimSessionValidityTime = 0x00003015,
	EEapSimEncapsulation = 0x00003016,
	EEapTtlsUsername = 0x00003017,
	EEapTtlsRealm = 0x00003018,
	EEapTtlsVerifyServerRealm = 0x00003019,
	EEapTtlsRequireClientAuth = 0x0000301a,
	EEapTtlsSessionValidityTime = 0x0000301b,
	EEapTtlsCipherSuites = 0x0000301c,
	EEapTtlsEncapsulatedTypes = 0x0000301d,
	EEapTtlsUserCertSubjectKeyId = 0x0000301e,
	EEapTtlsUserCertIssuerName = 0x0000301f,
	EEapTtlsUserCertSerialNumber = 0x00003020,
	EEapTtlsCaCertSubjectKeyId = 0x00003021,
	EEapTtlsCaCertIssuerName = 0x00003022,
	EEapTtlsCaCertSerialNumber = 0x00003023,
	EEapAkaUsername = 0x00003024,
	EEapAkaRealm = 0x00003025,
	EEapAkaUsePseudonyms = 0x00003026,
	EEapAkaSessionValidityTime = 0x00003027,
	EEapAkaEncapsulation = 0x00003028,
	EEapPeapUsername = 0x00003029,
	EEapPeapRealm = 0x0000302a,
	EEapPeapVerifyServerRealm = 0x0000302b,
	EEapPeapRequireClientAuth = 0x0000302c,
	EEapPeapSessionValidityTime = 0x0000302d,
	EEapPeapCipherSuites = 0x0000302e,
	EEapPeapV0Allowed = 0x0000302f,
	EEapPeapV1Allowed = 0x00003030,
	EEapPeapV2Allowed = 0x00003031,
	EEapPeapEncapsulatedTypes = 0x00003032,
	EEapPeapUserCertSubjectKeyId = 0x00003033,
	EEapPeapUserCertIssuerName = 0x00003034,
	EEapPeapUserCertSerialNumber = 0x00003035,
	EEapPeapCaCertSubjectKeyId = 0x00003036,
	EEapPeapCaCertIssuerName = 0x00003037,
	EEapPeapCaCertSerialNumber = 0x00003038,
	EEapMschapv2Username = 0x00003039,
	EEapMschapv2Password = 0x0000303a,
	EEapMschapv2SessionValidityTime = 0x0000303b,
	EEapMschapv2Encapsulation = 0x0000303c,
	EEapFastUsername = 0x0000303d,
	EEapFastRealm = 0x0000303e,
	EEapFastVerifyServerRealm = 0x0000303f,
	EEapFastRequireClientAuth = 0x00003040,
	EEapFastSessionValidityTime = 0x00003041,
	EEapFastCipherSuites = 0x00003042,
	EEapFastEncapsulatedTypes = 0x00003043,
	EEapFastAuthProvModeAllowed = 0x00003044,
	EEapFastUnauthProvModeAllowed = 0x00003045,
	EEapFastWarnADHPNoPAC = 0x00003046,
	EEapFastWarnADHPNoMatchingPAC = 0x00003047,
	EEapFastWarnNotDefaultServer = 0x00003048,
	EEapFastUserCertSubjectKeyId = 0x00003049,
	EEapFastUserCertIssuerName = 0x0000304a,
	EEapFastUserCertSerialNumber = 0x0000304b,
	EEapFastCaCertSubjectKeyId = 0x0000304c,
	EEapFastCaCertIssuerName = 0x0000304d,
	EEapFastCaCertSerialNumber = 0x0000304e,
	EMschapv2Username = 0x0000304f,
	EMschapv2Password = 0x00003050,
	EMschapv2SessionValidityTime = 0x00003051,
	EMschapv2Encapsulation = 0x00003052,
	EEapParameterLast = 0x0003100
	};    
   
    
// exceptional field ids that are missing from access point item definition
const TInt KFieldIgnored         = -1; // Ignored field
const TInt KFieldApplicationName = -2; // Application name.

// maximum length of default GPRS name
const TInt KMaxLengthOfApName    = 50;

// String constants for general connection method parameters
_LIT16( KStrYes,             "Yes" );
_LIT16( KStrNo,              "No" );
_LIT16( KStrOn,              "On" );
_LIT16( KStrOff,             "Off" );
_LIT16( KStrEmpty,           "" );

// Metadata
_LIT16( KStrMMS,             "MMS" );
_LIT16( KStrNotMMS,          "NotMMS" );

// Seamlessness
_LIT16( KStrNotRoamedTo,     "NotRoamedTo" );
_LIT16( KStrConfirmFirst,    "ConfirmFirst" );
_LIT16( KStrShowProgress,    "ShowProgress" );
_LIT16( KStrFullySeamless,   "FullySeamless" );
    
// Default connection type
_LIT16( KStrAlwaysAsk,       "Always ask" );
_LIT16( KStrAskOnce,         "Ask once" );
_LIT16( KStrDestination,     "Destination" );
_LIT16( KStrConnectionMethod,"Connection method" );

// General connection settings: Wlan usage: Known / Manual
_LIT16( KStrKnown,           "Known" );
_LIT16( KStrManual,          "Manual" );

// General connection settings: Seamlessness: Automatic / Confirm / Wlan only
_LIT16( KStrAutomatic,       "Automatic" );
_LIT16( KStrConfirm,         "Confirm" );
_LIT16( KStrWlanOnly,        "Wlan only" );

// Packet Data Settings
enum TDbCreatorPacketDataSettings
    {
    EPacketDataLinger = 0x29041982
    };

    
#endif //CDCCOMMON_H
