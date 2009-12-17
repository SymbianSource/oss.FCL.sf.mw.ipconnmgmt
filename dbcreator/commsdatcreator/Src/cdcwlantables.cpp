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
* Description:   Implementation of the class CWlanTables
*
*/




// INCLUDE FILES
#include "cdclogger.h"
#include "cdcwlantables.h"

#include <wlancontainer.h>

// CONSTANTS
/// Modem bearer names for WLAN connection methods
_LIT( KWlanBearerName, "WLANBearer" );
_LIT( KWlanBearerAgent, "wlanagt.agt" );
_LIT( KWlanBearerNif, "wlannif" );

_LIT( KWlanLDDName, "not used" );
_LIT( KWlanPDDName, "not used" );
const TInt KWlanLastSocketActivityTimeout = -1;
const TInt KWlanLastSessionClosedTimeout = 1;
const TInt KWlanLastSocketClosedTimeout = -1;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWlanTables::NewL
// ---------------------------------------------------------
//
CWlanTables* CWlanTables::NewL()
    {

    CWlanTables* self = new ( ELeave ) CWlanTables;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CWlanTables::CWlanTables
// ---------------------------------------------------------
//
CWlanTables::CWlanTables()
    {
    //no implementation needed
    }


// ---------------------------------------------------------
// CWlanTables::ConstructL
// ---------------------------------------------------------
//
void CWlanTables::ConstructL()
    {
    iSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    }


// ---------------------------------------------------------
// CWlanTables::~CWlanTables
// ---------------------------------------------------------
//
CWlanTables::~CWlanTables()
    {
    if ( iSession )
        {
        iSession->Close();
        delete iSession; 
        }
    }
    
    
// ---------------------------------------------------------
// CreateWlanServiceTableL
// ---------------------------------------------------------
//
void CWlanTables::CheckAndCreateTablesL()
    {
    // Check WLAN Device Settings Table (creates if does not exist).
    CreateWlanDeviceSettingsTableL();
    
    // Check WLAN Service Table (creates if does not exist).
    CreateWlanServiceTableL();

    // Check WLAN Secondary SSID Table (creates if does not exist).
    CreateWlanSecSSIDTableL();
    
    // Check WLAN bearer record (creates if does not exist).
    CreateWlanBearerRecordL();    
    }
    
// ---------------------------------------------------------
// CreateWlanServiceTableL
// ---------------------------------------------------------
//
void CWlanTables::CreateWlanServiceTableL()
    {
    //checks whether table exists
    TInt error( KErrNone );
    
    TRAP( error, CCDWlanServiceRecord::TableIdL( *iSession ) );
    if( error == KErrNotFound )
        {    
        //table not found -- add new table
        CCDWlanServiceRecord::CreateTableL( *iSession );
        
        //check result
        TRAP( error, CCDWlanServiceRecord::TableIdL( *iSession ) );
        
        if ( error == KErrNone )
            {
            CLOG_WRITE( "Added WLAN service table" )
            }
        }
    else if ( error == KErrNone)
        {
            CLOG_WRITE( "WLAN service table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    

// ---------------------------------------------------------
// CreateWlanDeviceTableL
// ---------------------------------------------------------
//
void CWlanTables::CreateWlanDeviceSettingsTableL()
    {
    //checks whether table exists
    TInt error( KErrNone );
    
    TRAP( error, CCDWlanDeviceSettingsRecord::TableIdL( *iSession ) );
    if( error == KErrNotFound )
        {    
        //table not found -- add new table
        CCDWlanDeviceSettingsRecord::CreateTableL( *iSession );
        
        //check result
        TRAP( error, CCDWlanDeviceSettingsRecord::TableIdL( *iSession ) );
        if ( error == KErrNone )
            {
            CLOG_WRITE( "Added Device Settings table" )
            }
        }
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "Device Settings table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CreateWlanSecSSIDTableL
// ---------------------------------------------------------
//
void CWlanTables::CreateWlanSecSSIDTableL()
    {
    //checks whether table exists
    TInt error( KErrNone );
    
    TRAP( error, CCDWLANSecSSIDTable::TableIdL( *iSession ) );
    if( error == KErrNotFound )
        {    
        //table not found -- add new table
        CCDWLANSecSSIDTable::CreateTableL( *iSession );
        
        //check result
        TRAP( error, CCDWLANSecSSIDTable::TableIdL( *iSession ) );
        
        if ( error == KErrNone )
            {
            CLOG_WRITE( "Added Secondary SSID table" )
            }
        }
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "Secondary SSID table already exists" )
        }
    else
        {
        //do nothing             
        }
    }

// ---------------------------------------------------------
// CreateWlanBearerRecordL
// ---------------------------------------------------------
//
void CWlanTables::CreateWlanBearerRecordL()
    {
	CCDLANBearerRecord* record = static_cast<CCDLANBearerRecord*>
                ( CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ) );
    CleanupStack::PushL( record );    
    
   	record->iRecordName.SetL( TPtrC( KWlanBearerName ) );
   	
    if ( !record->FindL( *iSession ) )
        { // bearer not found -> create dummy values. Copied from CmManager initialization.               
        record->SetRecordId( KCDNewRecordRequest );
        
        record->iBearerAgent.SetL( KWlanBearerAgent );
        record->iRecordName.SetL( KWlanBearerName );
        record->iLanBearerNifName.SetL( KWlanBearerNif );
        record->iLanBearerLddName.SetL( KWlanLDDName );
        record->iLanBearerPddName.SetL( KWlanPDDName );
        record->iLastSocketActivityTimeout.SetL(
                                ( TUint32 )KWlanLastSocketActivityTimeout );
        record->iLastSessionClosedTimeout.SetL(
                                (TUint32 )KWlanLastSessionClosedTimeout );
        record->iLastSocketClosedTimeout.SetL(
                                ( TUint32 )KWlanLastSocketClosedTimeout );
        record->StoreL( *iSession );
        CLOG_WRITE( "Added WLAN bearer record" );
        }
    else
		{
		CLOG_WRITE( "WLAN bearer record already exists" );
		}

    CleanupStack::PopAndDestroy( record );
    }



//  End of File.
