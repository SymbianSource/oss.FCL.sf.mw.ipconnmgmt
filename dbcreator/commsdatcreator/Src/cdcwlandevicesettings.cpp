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
* Description:   Implementation of the class CWlanDeviceSettings
*
*/



// INCLUDE FILES
#include "cdcwlandevicesettings.h"
#include "cdclogger.h"

#include <cdbcols.h>
#include <WlanCdbCols.h>

#include <metadatabase.h>
#include <commsdattypeinfov1_1.h>
#include <commsdattypesv1_1.h>
#include <wlancontainer.h>


// CONSTANTS
// WLAN Device Settings table - default settings
const TUint32 KDefaultBGScanInterval    = -1;   // Automatic scan
const TBool   KDefaultUseDefSettings    = EFalse;
const TUint32 KDefaultLongRetry         = 4;
const TUint32 KDefaultShortRetry        = 7;
const TUint32 KDefaultRTS               = 2347;
const TUint32 KDefaultTxPowerLevel      = 100;  //mW
const TBool   KDefaultRadioMeasurements = ETrue;  
const TBool   KDefaultWlanPowerMode     = ETrue;        

// Increase version every time the content of the table changes
const TUint32 KWlanDeviceSettingsTableVersion = 9;


// ================= MEMBER FUNCTIONS ======================

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CWlanDeviceSettings::CWlanDeviceSettings()
    {
    }
    

// ---------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CWlanDeviceSettings::ConstructL()
    {
    DEBUG( "CWlanDeviceSettings::ConstructL()" )
    
    iSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    
    // Open the WLAN device settings table.
    if( !iSession->IsInTransaction() )
        {
        iSession->OpenTransactionL();
        }
    OpenTableL();
    iSession->CommitTransactionL();
    DEBUG( "CWlanDeviceSettings::ConstructL() Tables opened ok." )
    }


// ---------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------
//
CWlanDeviceSettings* CWlanDeviceSettings::NewL()
    {
    DEBUG( "CWlanDeviceSettings::NewL()" )

    CWlanDeviceSettings* self = new (ELeave) CWlanDeviceSettings;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CWlanDeviceSettings::~CWlanDeviceSettings()
    {
    DEBUG( "CWlanDeviceSettings::~CWlanDeviceSettings()" )
    
    if ( iSession )
        {
        iSession->Close();
        delete iSession; 
        }
    }


// ---------------------------------------------------------
// CWlanDeviceSettings::GetDefaultSettings
// ---------------------------------------------------------
//
void CWlanDeviceSettings::GetDefaultSettings( SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlanDeviceSettings::GetDefaultSettings()" )

    aSettings.backgroundScanInterval = KDefaultBGScanInterval; 
    aSettings.savedBgScanInterval    = KDefaultBGScanInterval; 
    aSettings.useDefaultSettings     = KDefaultUseDefSettings;        
    aSettings.longRetry              = KDefaultLongRetry;
    aSettings.shortRetry             = KDefaultShortRetry;
    aSettings.rts                    = KDefaultRTS;
    aSettings.txPowerLevel           = KDefaultTxPowerLevel;
    aSettings.radioMeasurements      = KDefaultRadioMeasurements;
    aSettings.powerMode              = KDefaultWlanPowerMode;
    }


// ---------------------------------------------------------
// CWlanDeviceSettings::OpenTableL
// ---------------------------------------------------------
//
void CWlanDeviceSettings::OpenTableL()
    {
    DEBUG( "CWlanDeviceSettings::OpenTableL()" )
    
    // See if there is a record whose device settings type is
    // 'WLAN default settings'
    CMDBGenericRecord* deviceSettingsTable = static_cast< CMDBGenericRecord* >
                        ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( deviceSettingsTable );
    deviceSettingsTable->InitializeL( TPtrC( WLAN_DEVICE_SETTINGS ), NULL );
    deviceSettingsTable->LoadL( *iSession );
    
    CMDBElement* tableType = deviceSettingsTable->GetFieldByIdL
                                            ( KCDTIdWlanDeviceSettingsType );
    CMDBField< TUint32 >* tableTypeField =
                            static_cast< CMDBField< TUint32 >* >( tableType );
    tableTypeField->SetL( KWlanDefaultSettings );
    
    if( !deviceSettingsTable->FindL( *iSession ) )
        {
        // If not, store empty record
        deviceSettingsTable->SetRecordId( KCDNewRecordRequest );
        deviceSettingsTable->StoreL( *iSession );
        // And fill it with data
        FillRecordL( KWlanDefaultSettings, deviceSettingsTable );
        deviceSettingsTable->ModifyL( *iSession );
        }
    CleanupStack::PopAndDestroy( deviceSettingsTable );
    
    // See if there is a record whose device settings type is
    // 'WLAN user settings'
    deviceSettingsTable = static_cast< CMDBGenericRecord* >
                                ( CCDRecordBase::RecordFactoryL( 0 ) );
    CleanupStack::PushL( deviceSettingsTable );
    deviceSettingsTable->InitializeL( TPtrC( WLAN_DEVICE_SETTINGS ), NULL );
    deviceSettingsTable->LoadL( *iSession );
    
    tableType = deviceSettingsTable->GetFieldByIdL( KCDTIdWlanDeviceSettingsType );
    tableTypeField = static_cast< CMDBField< TUint32 >* >( tableType );
    tableTypeField->SetL( KWlanUserSettings );
    
    if( !deviceSettingsTable->FindL( *iSession ) )
        {
        // If not, store empty record
        deviceSettingsTable->SetRecordId( KCDNewRecordRequest );
        deviceSettingsTable->StoreL( *iSession );
        // And fill it with data
        FillRecordL( KWlanUserSettings, deviceSettingsTable );
        deviceSettingsTable->ModifyL( *iSession );
        }
    CleanupStack::PopAndDestroy( deviceSettingsTable );
    }


// ---------------------------------------------------------
// CWlanDeviceSettings::FillRecordL
// ---------------------------------------------------------
//

void CWlanDeviceSettings::FillRecordL( TUint32 aTableType, 
                                      CMDBGenericRecord* aRecord )
    {
    DEBUG1( "CWlanDeviceSettings::NewRecordL() Type=%d", aTableType )
    
    SWlanDeviceSettings settings;
    GetDefaultSettings( settings );
    
    // Device Settings Type
    CMDBElement* tableType = aRecord->GetFieldByIdL
                                            ( KCDTIdWlanDeviceSettingsType );
    CMDBField< TUint32 >* tableTypeField =
                            static_cast< CMDBField< TUint32 >* >( tableType );
    tableTypeField->SetL( aTableType );
    
    // Table Version
    CMDBElement* tableVersion = aRecord->GetFieldByIdL
                                                    ( KCDTIdTableVersion );
    CMDBField< TUint32 >* tableVersionField =
                        static_cast< CMDBField< TUint32 >* >( tableVersion );
    
    tableVersionField->SetL( KWlanDeviceSettingsTableVersion );

    // BG Scan Interval
    CMDBElement* bgScanInterval = aRecord->GetFieldByIdL
                                                    ( KCDTIdBgScanInterval );
    CMDBField< TUint32 >* bgScanIntervalField =
                        static_cast< CMDBField< TUint32 >* >( bgScanInterval );
    bgScanIntervalField->SetL( settings.backgroundScanInterval );
    
    // Saved BG Scan Interval
    CMDBElement* savedBgScanInterval = aRecord->GetFieldByIdL
                                                    ( KCDTIdSavedBgScanInterval );
    CMDBField< TUint32 >* savedBgScanIntervalField =
                        static_cast< CMDBField< TUint32 >* >( savedBgScanInterval );
    savedBgScanIntervalField->SetL( settings.savedBgScanInterval );
    
    // Use Default Settings
    CMDBElement* useDefaultSettings = aRecord->GetFieldByIdL
                                                ( KCDTIdUseDefaultSettings );
    CMDBField< TBool >* useDefaultSettingsField =
                    static_cast< CMDBField< TBool >* >( useDefaultSettings );
    useDefaultSettingsField->SetL( settings.useDefaultSettings );
    
    // Long Retry
    CMDBElement* longRetry = aRecord->GetFieldByIdL( KCDTIdWlanLongRetry );
    CMDBField< TUint32 >* longRetryField =
                        static_cast< CMDBField< TUint32 >* >( longRetry );
    longRetryField->SetL( settings.longRetry );
    
    // Short Retry
    CMDBElement* shortRetry = aRecord->GetFieldByIdL( KCDTIdWlanShortRetry );
    CMDBField< TUint32 >* shortRetryField =
                        static_cast< CMDBField< TUint32 >* >( shortRetry );
    shortRetryField->SetL( settings.shortRetry );
    
    // RTS Threshold
    CMDBElement* rts = aRecord->GetFieldByIdL( KCDTIdWlanRTSThreshold );
    CMDBField< TUint32 >* rtsField =
                        static_cast< CMDBField< TUint32 >* >( rts );
    rtsField->SetL( settings.rts );
    
    // TX Power Level
    CMDBElement* txPowerLevel = aRecord->GetFieldByIdL( KCDTIdTxPowerLevel );
    CMDBField< TUint32 >* txPowerLevelField =
                        static_cast< CMDBField< TUint32 >* >( txPowerLevel );
    txPowerLevelField->SetL( settings.txPowerLevel );
    
    // Radio Measurements
    CMDBElement* radioMeasurements = aRecord->GetFieldByIdL
                                            ( KCDTIdAllowRadioMeasurements );
    CMDBField< TBool >* radioMeasurementsField =
                    static_cast< CMDBField< TBool >* >( radioMeasurements );
    radioMeasurementsField->SetL( settings.radioMeasurements );
    
    // Power Mode
    CMDBElement* powerMode = aRecord->GetFieldByIdL( KCDTIdWlanPowerMode );
    CMDBField< TBool >* powerModeField =
                    static_cast< CMDBField< TBool >* >( powerMode );
    powerModeField->SetL( settings.powerMode );
    }

//  End of File
