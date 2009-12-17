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
* Description:  table field definitions
*
*/



#ifndef TABLE_FIELDS_H
#define TABLE_FIELDS_H


// INCLUDES
#include <e32base.h>
#include <Metadatabase.h>
#include <WlanCdbCols.h>
#include <CommsDatTypesV1_1.h>

using namespace CommsDat;

// FORWARD DECLARATIONS

// CONSTANTS

// WLAN Service Table
const TMDBElementId KCDTIdWlanServiceId        = 0x00030000;
const TMDBElementId KCDTIdWlanConnMode         = 0x00040000;
   
const TMDBElementId KCDTIdWlanSSID             = 0x00050000;
const TMDBElementId KCDTIdWlanWepKey1          = 0x00060000;
const TMDBElementId KCDTIdWlanWepKey2          = 0x00070000;
const TMDBElementId KCDTIdWlanWepKey3          = 0x00080000;
const TMDBElementId KCDTIdWlanWepKey4          = 0x00090000;
const TMDBElementId KCDTIdWlanWepIndex         = 0x000A0000;
const TMDBElementId KCDTIdWlanSecMode          = 0x000B0000;
const TMDBElementId KCDTIdWlanAuthMode         = 0x000C0000;
const TMDBElementId KCDTIdWlanEnableWpaPsk     = 0x000D0000;
const TMDBElementId KCDTIdWlanWpaPreSharedKey  = 0x000E0000;

const TMDBElementId KCDTIdWlanWpaKeyLength     = 0x000F0000;
const TMDBElementId KCDTIdWlanEaps             = 0x00100000;
const TMDBElementId KCDTIdWlanScanSSID         = 0x00110000;
const TMDBElementId KCDTIdWlanChannelID        = 0x00120000;
const TMDBElementId KCDTIdWlanUsedSSID         = 0x00130000;
const TMDBElementId KCDTIdWlanFormatKey1       = 0x00140000;
const TMDBElementId KCDTIdWlanFormatKey2       = 0x00150000;
const TMDBElementId KCDTIdWlanFormatKey3       = 0x00160000;
const TMDBElementId KCDTIdWlanFormatKey4       = 0x00170000;

const TMDBElementId KCDTIdWlanAllowSSIDRoaming = 0x00180000;

const TMDBElementId KCDTIdWlanEnabledEaps      = 0x00190000;
const TMDBElementId KCDTIdWlanDisabledEaps     = 0x001A0000;


// WLAN Device Settings Table
const TMDBElementId KCDTIdTableVersion              = 0x00030000;

const TMDBElementId KCDTIdWlanDeviceSettingsType    = 0x00040000;
const TMDBElementId KCDTIdBgScanInterval            = 0x00050000;

const TMDBElementId KCDTIdUseDefaultSettings        = 0x00060000;
const TMDBElementId KCDTIdWlanLongRetry             = 0x00070000;
const TMDBElementId KCDTIdWlanShortRetry            = 0x00080000;
const TMDBElementId KCDTIdWlanRTSThreshold          = 0x00090000;
const TMDBElementId KCDTIdTxPowerLevel              = 0x000A0000;
const TMDBElementId KCDTIdAllowRadioMeasurements    = 0x000B0000;
const TMDBElementId KCDTIdWlanPowerMode             = 0x000C0000;

const TMDBElementId KCDTIdSavedBgScanInterval       = 0x000D0000;

// WLAN Secondary SSID Table
const TMDBElementId KCDTIdWlanSecSSIDServiceId      = 0x00030000;
const TMDBElementId KCDTIdWlanSecSSIDName           = 0x00040000;

const TMDBElementId KCDTIdWlanSecSSIDScannedSSID    = 0x00050000;
const TMDBElementId KCDTIdWlanSecSSIDUsedSSID       = 0x00060000;
const TMDBElementId KCDTIdWlanSecSSIDId             = 0x00070000;

// WLAN Destination Network Table
const TMDBElementId KCDTIdDestNwShowRoaming         = 0x00030000;
const TMDBElementId KCDTIdDestNwAccessNwId          = 0x00040000;

//Default Connection
const CommsDat::TMDBElementId KCDTIdDefConnType         = 0x00120000;
const CommsDat::TMDBElementId KCDTIdDefConnUid          = 0x00130000;
const CommsDat::TMDBElementId KCDTIdUsageOfWlan         = 0x00150000;
const CommsDat::TMDBElementId KCDTIdSeamlessnessHome    = 0x00170000;
const CommsDat::TMDBElementId KCDTIdSeamlessnessVisitor = 0x00180000;

#endif      // TABLE_FIELDS_H

// End of File.
