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
* Description:  WLAN table container class.
*
*/



#ifndef WLAN_CONTAINER_H
#define WLAN_CONTAINER_H


// INCLUDES
#include <e32base.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <cmmanagertablefields.h>

// FORWARD DECLARATIONS

// CONSTANTS

using namespace CommsDat;


// CLASS DECLARATION


/**
 *  Class representing records in the WLAN Service table
 *  @since Series60_3.2
 */
NONSHARABLE_CLASS( CCDWlanServiceRecord ) : public CCDRecordBase
    {
	public:
	IMPORT_C CCDWlanServiceRecord( TMDBElementId aElementId );

    IMPORT_C const SRecordTypeInfo* GetRecordInfo();
	
    IMPORT_C static TMDBElementId CreateTableL( CMDBSession& aSession );
    IMPORT_C static TMDBElementId TableIdL( CMDBSession& aSession );

	DATA_VTABLE
		
	public:
	CMDBField<TUint>    iWlanServiceId;
	CMDBField<TUint>    iWlanConnMode;
	CMDBField<TDesC>    iWLanSSID;
	CMDBField<TDesC>    iWLanUsedSSID;
	CMDBField<TDesC8>   iWLanWepKey1;
	CMDBField<TDesC8>   iWLanWepKey2;
	CMDBField<TDesC8>   iWLanWepKey3;
	CMDBField<TDesC8>   iWLanWepKey4;
	CMDBField<TUint>    iWlanWepIndex;
    CMDBField<TUint>    iWlanSecMode;
    CMDBField<TUint>    iWlanAuthMode;
    CMDBField<TUint>    iWlanEnableWpaPsk;
    CMDBField<TDesC8>   iWLanWpaPreSharedKey;
    CMDBField<TUint>    iWlanWpaKeyLength;
    CMDBField<TDesC>    iWLanEaps;
    CMDBField<TBool>    iWlanScanSSID;
    CMDBField<TUint>    iWlanChannelID;
    CMDBField<TUint>    iWlanFormatKey1;
    CMDBField<TUint>    iWlanFormatKey2;
    CMDBField<TUint>    iWlanFormatKey3;
    CMDBField<TUint>    iWlanFormatKey4;
    CMDBField<TBool>    iWlanAllowSSIDRoaming;
    CMDBField<TDesC8>   iWLanEnabledEaps;   // binary
    CMDBField<TDesC8>   iWLanDisabledEaps;  // binary

	private:
	
		static const SRecordTypeInfo iRecordInfo[];
    };

/**
 * Wlan Device settings record.
 * @since Series60_3.2
 */
NONSHARABLE_CLASS( CCDWlanDeviceSettingsRecord ) : public CCDRecordBase
    {
	public:
	IMPORT_C CCDWlanDeviceSettingsRecord( TMDBElementId aElementId );

	IMPORT_C const SRecordTypeInfo* GetRecordInfo();
	
    IMPORT_C static TMDBElementId CreateTableL( CMDBSession& aSession );
    IMPORT_C static TMDBElementId TableIdL( CMDBSession& aSession );

	DATA_VTABLE
		
	public:
	CMDBField<TUint>    iTableVersion;
	CMDBField<TUint>    iWlanDeviceSettingsType;
	CMDBField<TUint>    iBgScanInterval;
	CMDBField<TUint>    iSavedBgScanInterval;
	CMDBField<TBool>    iUseDefaultSettings;
	CMDBField<TUint>    iWlanLongRetry;
	CMDBField<TUint>    iWlanShortRetry;
	CMDBField<TUint>    iWlanRTSThreshold;
	CMDBField<TUint>    iTxPowerLevel;
	CMDBField<TBool>    iAllowRadioMeasurements;
	CMDBField<TBool>    iWlanPowerMode;

	private:
	
		static const SRecordTypeInfo iRecordInfo[];
    };

/**
 * Wlan Secure SSID record
 * @since Series60_3.2
 */
NONSHARABLE_CLASS( CCDWLANSecSSIDTable ) : public CCDRecordBase
    {
	public:
	IMPORT_C CCDWLANSecSSIDTable( TMDBElementId aElementId );

	IMPORT_C const SRecordTypeInfo* GetRecordInfo();
	
    IMPORT_C static TMDBElementId CreateTableL( CMDBSession& aSession );
    IMPORT_C static TMDBElementId TableIdL( CMDBSession& aSession );

	DATA_VTABLE
		
	public:
	CMDBField<TUint>    iWlanSecSSIDServiceId;
	CMDBField<TDesC>    iWlanSecSSIDName;
	CMDBField<TDesC>    iWlanSecSSIDScannedSSID;
	CMDBField<TDesC>    iWlanSecSSIDUsedSSID;
	CMDBField<TUint>    iWlanSecSSIDId;

	private:
	
		static const SRecordTypeInfo iRecordInfo[];
    };

/**
 * Destination Network record
 * @since Series60_3.2
 */
NONSHARABLE_CLASS( CCDDestNWTable ) : public CCDRecordBase
    {
	public:
	IMPORT_C CCDDestNWTable( TMDBElementId aElementId );

	IMPORT_C const SRecordTypeInfo* GetRecordInfo();
	
    IMPORT_C static TMDBElementId CreateTableL( CMDBSession& aSession );
    IMPORT_C static TMDBElementId TableIdL( CMDBSession& aSession );

	DATA_VTABLE
		
	public:
	CMDBField<TUint>    iDestNwShowRoaming;
	CMDBField<TDesC>    iDestNwAccessNwId;

	private:
	
		static const SRecordTypeInfo iRecordInfo[];
    };
#endif      // WLAN_CONTAINER_H

// End of File.
