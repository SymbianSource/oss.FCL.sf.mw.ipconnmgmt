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
* Description:  Contains the WLAN Service table format
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <metadatabase.h>
#include <wlancontainer.h>
#include <cmmanagertablefields.h>
#include <datamobilitycommsdattypes.h>


TMDBElementId CreateTableL( CMDBSession& aSession,
                            const TDesC& aTableName, 
                            const SGenericRecordTypeInfo* aNewTableInfo );
                            
TMDBElementId TableIdL( CMDBSession& aSession, 
                        const TDesC& aTableName );
                                                    
START_ATTRIBUTE_TABLE( CCDWlanServiceRecord, 0, 0 )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanServiceId,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanConnMode,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanSSID,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanUsedSSID,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanWepKey1,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanWepKey2,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanWepKey3,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanWepKey4,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanWepIndex,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanSecMode,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanAuthMode,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanEnableWpaPsk,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanWpaPreSharedKey,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanWpaKeyLength,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanEaps,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanScanSSID,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanChannelID,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanFormatKey1,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanFormatKey2,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanFormatKey3,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanFormatKey4,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWlanAllowSSIDRoaming,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanEnabledEaps,  TMDBBin )
	X_REGISTER_ATTRIBUTE( CCDWlanServiceRecord,   iWLanDisabledEaps,  TMDBBin )
    X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordTag,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDWlanDeviceSettingsRecord, 0, 0 )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iTableVersion,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iWlanDeviceSettingsType,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iBgScanInterval,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iUseDefaultSettings,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iWlanLongRetry,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iWlanShortRetry,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iWlanRTSThreshold,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iTxPowerLevel,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iAllowRadioMeasurements,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iWlanPowerMode,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWlanDeviceSettingsRecord,   iSavedBgScanInterval,  TMDBNum )	
    X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordTag,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDWLANSecSSIDTable, 0, 0 )
	X_REGISTER_ATTRIBUTE( CCDWLANSecSSIDTable,   iWlanSecSSIDServiceId,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWLANSecSSIDTable,   iWlanSecSSIDName,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWLANSecSSIDTable,   iWlanSecSSIDScannedSSID,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWLANSecSSIDTable,   iWlanSecSSIDUsedSSID,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWLANSecSSIDTable,   iWlanSecSSIDId,  TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordTag,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDDestNWTable, 0, 0 )
	X_REGISTER_ATTRIBUTE( CCDDestNWTable,   iDestNwShowRoaming,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDestNWTable,   iDestNwAccessNwId,  TMDBText )
    X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordTag,  TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDRecordBase,          iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDDefConnRecord, 0, 0 )
	X_REGISTER_ATTRIBUTE( CCDDefConnRecord,   iDefConnType,         TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDefConnRecord,   iDefConnUid,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDefConnRecord,   iUsageOfWlan,      TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDDefConnRecord,   iCellularDataUsageHome,  TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDDefConnRecord,   iCellularDataUsageVisitor,  TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase,      iRecordTag,           TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDRecordBase,      iRecordName,          TMDBText )
END_ATTRIBUTE_TABLE()

const SRecordTypeInfo CCDDefConnRecord::iRecordInfo[]=
  	{
	    { KCDTIdRecordTag,	         EUint32,    ENoAttrs, KCDTypeNameRecordTag },
    	{ KCDTIdRecordName,	         EText,      ENotNull, KCDTypeNameRecordName},
    	{ KCDTIdDefConnType,         EUint32,    ENoAttrs, DEFCONN_TYPE },
    	{ KCDTIdDefConnUid,          EUint32,    ENoAttrs, DEFCONN_UID },
    	{ KCDTIdUsageOfWlan,         EUint32,    ENoAttrs, USAGEOFWLAN },
    	{ KCDTIdCellularDataUsageHome,     EUint32,    ENoAttrs,   CELLULAR_DATA_USAGE_HOME },
        { KCDTIdCellularDataUsageVisitor,  EUint32,    ENoAttrs,   CELLULAR_DATA_USAGE_VISITOR },
	    { 0, 0, ENoAttrs, KCDNull }
  	};


EXPORT_C const SRecordTypeInfo* CCDDefConnRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }  
  	
EXPORT_C CCDDefConnRecord::CCDDefConnRecord( TMDBElementId aElementId )
	: CCDRecordBase( aElementId ),  
      iDefConnType( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdDefConnType ),
      iDefConnUid( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdDefConnUid ),
      iUsageOfWlan( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdUsageOfWlan ),
      iCellularDataUsageHome( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdCellularDataUsageHome ),
      iCellularDataUsageVisitor( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdCellularDataUsageVisitor )
    {
    iCellularDataUsageHome.SetAttributes(ECDProtectedWrite);
    iCellularDataUsageVisitor.SetAttributes(ECDProtectedWrite);
    };  	

EXPORT_C TMDBElementId 
    CCDDefConnRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( DEFCONN_TABLE ) );
    }

EXPORT_C TMDBElementId 
    CCDDefConnRecord::CreateTableL( CMDBSession& aSession )
    {
    // table descriptor array for WLAN DEVICE SETTINGS TABLE
    const SGenericRecordTypeInfo KDefConnTable[] = 
        {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32, 
                                ENoAttrs,   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,    
                                ENotNull,   KCDTypeNameRecordName),
        SGenericRecordTypeInfo( KCDTIdDefConnType,  EUint32,    
                                ENoAttrs,   DEFCONN_TYPE ),
        SGenericRecordTypeInfo( KCDTIdDefConnUid,  EUint32,    
                                ENoAttrs,   DEFCONN_UID ),
        SGenericRecordTypeInfo( KCDTIdUsageOfWlan,  EUint32,    
                                ENoAttrs,   USAGEOFWLAN ),
        SGenericRecordTypeInfo( KCDTIdCellularDataUsageHome,  EUint32,    
                                ENoAttrs,   CELLULAR_DATA_USAGE_HOME ),
        SGenericRecordTypeInfo( KCDTIdCellularDataUsageVisitor,  EUint32,    
                                ENoAttrs,   CELLULAR_DATA_USAGE_VISITOR ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };

    return ::CreateTableL( aSession, 
                           TPtrC( DEFCONN_TABLE ), 
                           KDefConnTable );
    }
  	

const SRecordTypeInfo CCDWlanServiceRecord::iRecordInfo[]=
  	{
	    { KCDTIdRecordTag,	            EUint32,    ENoAttrs,   KCDTypeNameRecordTag }, 
    	{ KCDTIdRecordName,	            EText,      ENotNull,   KCDTypeNameRecordName},
    	{ KCDTIdWlanServiceId,          EUint32,    ENoAttrs,   WLAN_SERVICE_ID },
        // Default Settings
    	{ KCDTIdWlanConnMode,           EUint32,    ENoAttrs,   WLAN_CONNECTION_MODE },
    	{ KCDTIdWlanSSID,	            EText,      ENoAttrs,   NU_WLAN_SSID },	
	    { KCDTIdWlanUsedSSID,	        EText,      ENoAttrs,   WLAN_USED_SSID },	
    	{ KCDTIdWlanWepKey1,	        EDesC8,     ENoAttrs,   NU_WLAN_WEP_KEY1 },
	    { KCDTIdWlanWepKey2,	        EDesC8,     ENoAttrs,   NU_WLAN_WEP_KEY2 },
    	{ KCDTIdWlanWepKey3,	        EDesC8,     ENoAttrs,   NU_WLAN_WEP_KEY3 },
	    { KCDTIdWlanWepKey4,	        EDesC8,     ENoAttrs,   NU_WLAN_WEP_KEY4 },
        { KCDTIdWlanWepIndex,	        EUint32,    ENoAttrs,   WLAN_WEP_INDEX },
        // WLAN Security
        { KCDTIdWlanSecMode,	        EUint32,    ENoAttrs,   WLAN_SECURITY_MODE },
        { KCDTIdWlanAuthMode,           EUint32,    ENoAttrs,   NU_WLAN_AUTHENTICATION_MODE },
        { KCDTIdWlanEnableWpaPsk,       EUint32,    ENoAttrs,   WLAN_ENABLE_WPA_PSK },
        { KCDTIdWlanWpaPreSharedKey,    EDesC8,     ENoAttrs,   WLAN_WPA_PRE_SHARED_KEY },
        { KCDTIdWlanWpaKeyLength,	    EUint32,    ENoAttrs,   WLAN_WPA_KEY_LENGTH },
        { KCDTIdWlanEaps,	            ELongText,  ENoAttrs,   WLAN_EAPS },   	
        { KCDTIdWlanScanSSID,           EBool,      ENoAttrs,   WLAN_SCAN_SSID },
        { KCDTIdWlanChannelID,          EUint32,    ENoAttrs,   NU_WLAN_CHANNEL_ID },
        { KCDTIdWlanFormatKey1,	        EUint32,    ENoAttrs,   WLAN_WEP_KEY1_FORMAT },
	    { KCDTIdWlanFormatKey2,	        EUint32,    ENoAttrs,   WLAN_WEP_KEY2_FORMAT },
	    { KCDTIdWlanFormatKey3,	        EUint32,    ENoAttrs,   WLAN_WEP_KEY3_FORMAT },
	    { KCDTIdWlanFormatKey4,	        EUint32,    ENoAttrs,   WLAN_WEP_KEY4_FORMAT },
	    { KCDTIdWlanAllowSSIDRoaming,   EBool,      ENoAttrs,   WLAN_ALLOW_SSID_ROAMING },
        { KCDTIdWlanEnabledEaps,	    EDesC8,     ENoAttrs,   WLAN_ENABLED_EAPS },   	
        { KCDTIdWlanDisabledEaps,	    EDesC8,     ENoAttrs,   WLAN_DISABLED_EAPS },   	
	    { 0, 0, ENoAttrs, KCDNull }
  	};

const SRecordTypeInfo CCDWlanDeviceSettingsRecord::iRecordInfo[]=
  	{
    { KCDTIdRecordTag,      EUint32,    ENoAttrs,   KCDTypeNameRecordTag },
    { KCDTIdRecordName,     EText,      ENotNull,   KCDTypeNameRecordName },
    { KCDTIdTableVersion,   EUint32,    ENotNull,   WLAN_TABLE_VERSION },
    { KCDTIdWlanDeviceSettingsType,     EUint32,  ENotNull,   WLAN_DEVICE_SETTINGS_TYPE },
    { KCDTIdBgScanInterval, EUint32,    ENotNull,   WLAN_BG_SCAN_INTERVAL },
    { KCDTIdUseDefaultSettings,    EBool, ENotNull,   WLAN_USE_DEFAULT_SETTINGS },
    { KCDTIdWlanLongRetry,    EUint32,  ENotNull,   WLAN_LONG_RETRY },
    { KCDTIdWlanShortRetry,   EUint32,    ENotNull,   WLAN_SHORT_RETRY },
    { KCDTIdWlanRTSThreshold,        EUint32,   ENotNull,   WLAN_RTS_THRESHOLD },
    { KCDTIdTxPowerLevel,    EUint32, ENotNull,   NU_WLAN_TX_POWER_LEVEL },
    { KCDTIdAllowRadioMeasurements,    EBool, ENotNull,   WLAN_ALLOW_RADIO_MEASUREMENTS },
    { KCDTIdWlanPowerMode,    EBool,      ENotNull,   WLAN_POWER_MODE },
    { KCDTIdSavedBgScanInterval, EUint32,    ENotNull,   WLAN_SAVED_BG_SCAN_INTERVAL },
    { 0, 0, ENoAttrs, KCDNull }
  	};

const SRecordTypeInfo CCDWLANSecSSIDTable::iRecordInfo[]=
  	{
    { KCDTIdRecordTag,        EUint32, 
                            ENoAttrs,   KCDTypeNameRecordTag },
    { KCDTIdRecordName,        EText,    
                            ENotNull,   KCDTypeNameRecordName },
    { KCDTIdWlanSecSSIDServiceId,  EUint32,
                            ENotNull,   WLAN_SEC_SSID_SERVICE_ID },
    { KCDTIdWlanSecSSIDName,   EText,
                            ENotNull,   WLAN_SEC_SSID_NAME },
    { KCDTIdWlanSecSSIDScannedSSID,        EText,      
                            ENotNull,   WLAN_SEC_SSID_SCANNED_SSID },
    { KCDTIdWlanSecSSIDUsedSSID,    EText,      
                            ENoAttrs,   WLAN_SEC_SSID_USED_SSID },
    { KCDTIdWlanSecSSIDId,  EUint32,
                            ENotNull,   WLAN_SEC_SSID_ID },
    { 0, 0, ENoAttrs, KCDNull }
  	};

const SRecordTypeInfo CCDDestNWTable::iRecordInfo[]=
  	{
    { KCDTIdDestNwShowRoaming,  EUint32,
                            ENoAttrs,   DEST_NW_SHOW_ROAMING },                            
    { KCDTIdDestNwAccessNwId,   EText,
                            ENotNull,   DEST_NW_ACCESS_NWID },
    { 0, 0, ENoAttrs, KCDNull }                                                        
  	};

EXPORT_C CCDWlanServiceRecord::CCDWlanServiceRecord( TMDBElementId aElementId )
	: CCDRecordBase( aElementId ),  
      iWlanServiceId( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanServiceId ),
      iWlanConnMode( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanConnMode ),
      iWLanSSID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSSID ),
      iWLanUsedSSID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanUsedSSID ),
      iWLanWepKey1( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWepKey1 ),
      iWLanWepKey2( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWepKey2 ),
      iWLanWepKey3( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWepKey3 ),          
      iWLanWepKey4( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWepKey4 ),
      iWlanWepIndex( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWepIndex ),
      iWlanSecMode( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecMode ),
      iWlanAuthMode( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanAuthMode ),
      iWlanEnableWpaPsk( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanEnableWpaPsk ),
      iWLanWpaPreSharedKey( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWpaPreSharedKey ),
      iWlanWpaKeyLength( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanWpaKeyLength ),
      iWLanEaps( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanEaps ),
      iWlanScanSSID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanScanSSID ),
      iWlanChannelID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanChannelID ),
      iWlanFormatKey1( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanFormatKey1 ),
      iWlanFormatKey2( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanFormatKey2 ),
      iWlanFormatKey3( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanFormatKey3 ),
      iWlanFormatKey4( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanFormatKey4 ),
      iWlanAllowSSIDRoaming( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanAllowSSIDRoaming ),
      iWLanEnabledEaps( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanEnabledEaps ),
      iWLanDisabledEaps( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanDisabledEaps )
    {};

EXPORT_C CCDWlanDeviceSettingsRecord::CCDWlanDeviceSettingsRecord( TMDBElementId aElementId )
	: CCDRecordBase( aElementId ),  
      iTableVersion( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdTableVersion ),
      iWlanDeviceSettingsType( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanDeviceSettingsType ),
      iBgScanInterval( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdBgScanInterval ),
      iSavedBgScanInterval( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdSavedBgScanInterval ),
      iUseDefaultSettings( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdUseDefaultSettings ),
      iWlanLongRetry( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanLongRetry ),
      iWlanShortRetry( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanShortRetry ),
      iWlanRTSThreshold( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanRTSThreshold ),          
      iTxPowerLevel( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdTxPowerLevel ),          
      iAllowRadioMeasurements( ( aElementId & KCDMaskShowRecordType )
                        | KCDTIdAllowRadioMeasurements ),
      iWlanPowerMode( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanPowerMode )
    {};

EXPORT_C CCDWLANSecSSIDTable::CCDWLANSecSSIDTable( TMDBElementId aElementId )
	: CCDRecordBase( aElementId ),  
      iWlanSecSSIDServiceId( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecSSIDServiceId ),
      iWlanSecSSIDName( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecSSIDName ),
      iWlanSecSSIDScannedSSID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecSSIDScannedSSID ),
      iWlanSecSSIDUsedSSID( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecSSIDUsedSSID ),
      iWlanSecSSIDId( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdWlanSecSSIDId )
    {};

EXPORT_C CCDDestNWTable::CCDDestNWTable( TMDBElementId aElementId )
	: CCDRecordBase( aElementId ),  
      iDestNwShowRoaming( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdDestNwShowRoaming ),
      iDestNwAccessNwId( ( aElementId & KCDMaskShowRecordType ) 
                        | KCDTIdDestNwAccessNwId )
    {};


EXPORT_C TMDBElementId 
    CCDWlanServiceRecord::CreateTableL( CMDBSession& aSession )
    {
    // table descriptor array for WLAN SERVICE TABLE
    const SGenericRecordTypeInfo KWLANServiceTable[] = 
        {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32, 
                                ENoAttrs,   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,    
                                ENotNull,   KCDTypeNameRecordName),
        SGenericRecordTypeInfo( KCDTIdWlanServiceId,  EUint32,    
                                ENoAttrs,   WLAN_SERVICE_ID ),
        SGenericRecordTypeInfo( KCDTIdWlanConnMode,   EUint32,    
                                ENoAttrs,   WLAN_CONNECTION_MODE ),
        SGenericRecordTypeInfo( KCDTIdWlanSSID,        EText,      
                                ENoAttrs,   NU_WLAN_SSID ),
        SGenericRecordTypeInfo( KCDTIdWlanUsedSSID,        EText,      
                                ENoAttrs,   WLAN_USED_SSID ),
        SGenericRecordTypeInfo( KCDTIdWlanWepKey1,    EDesC8,      
                                ENoAttrs,   NU_WLAN_WEP_KEY1 ),
        SGenericRecordTypeInfo( KCDTIdWlanWepKey2,    EDesC8,      
                                ENoAttrs,   NU_WLAN_WEP_KEY2 ),
        SGenericRecordTypeInfo( KCDTIdWlanWepKey3,    EDesC8,      
                                ENoAttrs,   NU_WLAN_WEP_KEY3 ),
        SGenericRecordTypeInfo( KCDTIdWlanWepKey4,    EDesC8,      
                                ENoAttrs,   NU_WLAN_WEP_KEY4 ),
        SGenericRecordTypeInfo( KCDTIdWlanWepIndex,    EUint32,    
                                ENoAttrs,   WLAN_WEP_INDEX ),
        SGenericRecordTypeInfo( KCDTIdWlanSecMode,    EUint32,    
                                ENoAttrs,   WLAN_SECURITY_MODE ),
        SGenericRecordTypeInfo( KCDTIdWlanAuthMode,        EUint32,    
                                ENoAttrs,   NU_WLAN_AUTHENTICATION_MODE ),
        SGenericRecordTypeInfo( KCDTIdWlanEnableWpaPsk,    EUint32,    
                                ENoAttrs,   WLAN_ENABLE_WPA_PSK ),
        SGenericRecordTypeInfo( KCDTIdWlanWpaPreSharedKey,EDesC8,
                                ENoAttrs,   WLAN_WPA_PRE_SHARED_KEY ),
        SGenericRecordTypeInfo( KCDTIdWlanWpaKeyLength,    EUint32,    
                                ENoAttrs,   WLAN_WPA_KEY_LENGTH ),
        SGenericRecordTypeInfo( KCDTIdWlanEaps,        ELongText,
                                ENoAttrs,   WLAN_EAPS ),
        SGenericRecordTypeInfo( KCDTIdWlanScanSSID, EBool,
                                ENoAttrs,   WLAN_SCAN_SSID ),
        SGenericRecordTypeInfo( KCDTIdWlanChannelID, EUint32,
                                ENoAttrs,   NU_WLAN_CHANNEL_ID ),
        SGenericRecordTypeInfo( KCDTIdWlanFormatKey1,    EUint32,      
                                ENoAttrs,   WLAN_WEP_KEY1_FORMAT ),
        SGenericRecordTypeInfo( KCDTIdWlanFormatKey2,    EUint32,      
                                ENoAttrs,   WLAN_WEP_KEY2_FORMAT ),
        SGenericRecordTypeInfo( KCDTIdWlanFormatKey3,    EUint32,      
                                ENoAttrs,   WLAN_WEP_KEY3_FORMAT ),
        SGenericRecordTypeInfo( KCDTIdWlanFormatKey4,    EUint32,      
                                ENoAttrs,   WLAN_WEP_KEY4_FORMAT ),
        SGenericRecordTypeInfo( KCDTIdWlanAllowSSIDRoaming,    EBool,      
                                ENoAttrs,   WLAN_ALLOW_SSID_ROAMING ),
        SGenericRecordTypeInfo( KCDTIdWlanEnabledEaps,      EDesC8,
                                ENoAttrs,   WLAN_ENABLED_EAPS ),
        SGenericRecordTypeInfo( KCDTIdWlanDisabledEaps,      EDesC8,
                                ENoAttrs,   WLAN_DISABLED_EAPS ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };
    return ::CreateTableL( aSession, 
                           TPtrC( WLAN_SERVICE ), 
                           KWLANServiceTable );
    }
    
EXPORT_C TMDBElementId 
    CCDWlanServiceRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( WLAN_SERVICE ) );
    }

EXPORT_C TMDBElementId 
    CCDWlanDeviceSettingsRecord::CreateTableL( CMDBSession& aSession )
    {
    // table descriptor array for WLAN DEVICE SETTINGS TABLE
    const SGenericRecordTypeInfo KWlanDeviceSettingsTable[] = 
        {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32, 
                                ENoAttrs,   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,    
                                ENotNull,   KCDTypeNameRecordName),
        SGenericRecordTypeInfo( KCDTIdTableVersion,  EUint32,    
                                ENotNull,   WLAN_TABLE_VERSION ),
        SGenericRecordTypeInfo( KCDTIdWlanDeviceSettingsType,   EUint32,
                                ENotNull,   WLAN_DEVICE_SETTINGS_TYPE ),
        SGenericRecordTypeInfo( KCDTIdBgScanInterval,        EUint32,
                                ENotNull,   WLAN_BG_SCAN_INTERVAL ),
        SGenericRecordTypeInfo( KCDTIdUseDefaultSettings,    EBool,
                                ENotNull,   WLAN_USE_DEFAULT_SETTINGS ),
        SGenericRecordTypeInfo( KCDTIdWlanLongRetry,    EUint32,
                                ENotNull,   WLAN_LONG_RETRY ),
        SGenericRecordTypeInfo( KCDTIdWlanShortRetry,   EUint32,    
                                ENotNull,   WLAN_SHORT_RETRY ),
        SGenericRecordTypeInfo( KCDTIdWlanRTSThreshold,        EUint32,
                                ENotNull,   WLAN_RTS_THRESHOLD ),
        SGenericRecordTypeInfo( KCDTIdTxPowerLevel,    EUint32,
                                ENotNull,   NU_WLAN_TX_POWER_LEVEL ),
        SGenericRecordTypeInfo( KCDTIdAllowRadioMeasurements,    EBool,      
                                ENotNull,   WLAN_ALLOW_RADIO_MEASUREMENTS ),
        SGenericRecordTypeInfo( KCDTIdWlanPowerMode,    EBool,      
                                ENotNull,   WLAN_POWER_MODE ),    
        SGenericRecordTypeInfo( KCDTIdSavedBgScanInterval,     EUint32,
                                ENotNull,   WLAN_SAVED_BG_SCAN_INTERVAL ),
                                
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };

    return ::CreateTableL( aSession, 
                           TPtrC( WLAN_DEVICE_SETTINGS ), 
                           KWlanDeviceSettingsTable );
    }
    
EXPORT_C TMDBElementId 
    CCDWlanDeviceSettingsRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( WLAN_DEVICE_SETTINGS ) );
    }

EXPORT_C TMDBElementId 
    CCDWLANSecSSIDTable::CreateTableL( CMDBSession& aSession )
    {
    // table descriptor array for WLAN SECONDARY SSID TABLE
    const SGenericRecordTypeInfo KWLANSecSSIDTable[] = 
        {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32, 
                                ENoAttrs,   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,    
                                ENotNull,   KCDTypeNameRecordName),
        SGenericRecordTypeInfo( KCDTIdWlanSecSSIDServiceId,  EUint32,
                                ENotNull,   WLAN_SEC_SSID_SERVICE_ID ),
        SGenericRecordTypeInfo( KCDTIdWlanSecSSIDName,   EText,
                                ENotNull,   WLAN_SEC_SSID_NAME ),
        SGenericRecordTypeInfo( KCDTIdWlanSecSSIDScannedSSID,        EText,      
                                ENotNull,   WLAN_SEC_SSID_SCANNED_SSID ),
        SGenericRecordTypeInfo( KCDTIdWlanSecSSIDUsedSSID,    EText,      
                                ENoAttrs,   WLAN_SEC_SSID_USED_SSID ),
        SGenericRecordTypeInfo( KCDTIdWlanSecSSIDId,  EUint32,
                                ENotNull,   WLAN_SEC_SSID_ID ),

        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };

    return ::CreateTableL( aSession, 
                           TPtrC( WLAN_SECONDARY_SSID ), 
                           KWLANSecSSIDTable );
    }
    
EXPORT_C TMDBElementId 
    CCDWLANSecSSIDTable::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( WLAN_SECONDARY_SSID ) );
    }

EXPORT_C TMDBElementId 
    CCDDestNWTable::CreateTableL( CMDBSession& aSession )
    {
    // table descriptor array for Destination Network Table
    const SGenericRecordTypeInfo KDestNWTable[] = 
        {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32, 
                                ENoAttrs,   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,    
                                ENotNull,   KCDTypeNameRecordName),
        SGenericRecordTypeInfo( KCDTIdDestNwShowRoaming,  EUint32,
                                ENoAttrs,   DEST_NW_SHOW_ROAMING ),                            
        SGenericRecordTypeInfo( KCDTIdDestNwAccessNwId,   EText,
                                ENotNull,   DEST_NW_ACCESS_NWID ),                            
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };

    return ::CreateTableL( aSession, 
                           TPtrC( DEST_NETWORK ), 
                           KDestNWTable );
    }
    
EXPORT_C TMDBElementId 
    CCDDestNWTable::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( DEST_NETWORK ) );
    }

EXPORT_C const SRecordTypeInfo* CCDWlanServiceRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDWlanDeviceSettingsRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDWLANSecSSIDTable::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDDestNWTable::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
