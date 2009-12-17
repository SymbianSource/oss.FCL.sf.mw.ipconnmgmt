/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles defualt connection logic
*
*/

#include <metadatabase.h>
#include <datamobilitycommsdattypes.h>
#include <cmdefconnvalues.h>
#include "mpmdefaultconnection.h"
#include "mpmserversession.h" //KShift8
#include "mpmserver.h" 
#include "mpmlogger.h"
#include "mpmcommsdataccess.h"

using namespace CommsDat;
// ---------------------------------------------------------------------------
// CMPMDefaultConnection::NewL
// ---------------------------------------------------------------------------
//
CMPMDefaultConnection* CMPMDefaultConnection::NewL( const CMPMServer* aServer)
    {
    CMPMDefaultConnection * self = new (ELeave) 
    CMPMDefaultConnection( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMPMDefaultConnection::~CMPMDefaultConnection()
    {
    }
// ---------------------------------------------------------------------------
// CMPMDefaultConnection::SetDefaultIAP
// ---------------------------------------------------------------------------
//
TInt CMPMDefaultConnection::SetDefaultIAP( TInt aIapId )
    {
    TInt err( KErrNone );
    //check validity
    if( ( aIapId ) > 0 && ( aIapId <= KCDMaxRecords ) )
        {
        iDefaultIapId = aIapId;
        MPMLOGSTRING2(
            "CMPMDefaultConnection::SetDefaultIAP Set default connection iap: %d", 
            aIapId)
        }
    else
        {
        err = KErrArgument;
        MPMLOGSTRING2(
            "CMPMDefaultConnection::SetDefaultIAP Invalid iap: %d", 
            aIapId)
        }    
    return err;
    }
// ---------------------------------------------------------------------------
// CMPMDefaultConnection::ClearDefaultIAP
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnection::ClearDefaultIAP()
    {
    iDefaultIapId = 0;
    MPMLOGSTRING( "CMPMDefaultConnection::ClearDefaultIAP:\
 cleared default iap" )
    }
// ---------------------------------------------------------------------------
// CMPMDefaultConnection::GetDefaultConnectionL
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnection::GetDefaultConnectionL( TCmDefConnType& aMode, 
                                                   TUint32& aConnection )
    {
    // first option is registered default iap
    if( iDefaultIapId )
        {
        MPMLOGSTRING2( "CMPMDefaultConnection::GetDefaultConnectionL:\
 default iap set as id %d", iDefaultIapId )
        aMode = ECmDefConnConnectionMethod;
        aConnection = iDefaultIapId;
        }
    else
        {
        MPMLOGSTRING( "CMPMDefaultConnection::GetDefaultConnectionL:\
 getting value from commsdat" )

        // read global commsdat setting     
        //
        TRAPD( err, ReadDefConnSettingL( aMode, aConnection ) )

        if( err != KErrNone )
            {
            //use default values
            //
            aMode = KDefaultMode;
            aConnection = KDefaultConnectionId;
            }

        MPMLOGSTRING3( "CMPMDefaultConnection::GetDefaultConnectionL: \
found type %d, id %d", aMode, aConnection )                
          
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPMDefaultConnection::CMPMDefaultConnection( const CMPMServer* aServer ) : 
    iServer( aServer )
    {
    
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnection::ConstructL
// ---------------------------------------------------------------------------
//    
void CMPMDefaultConnection::ConstructL()
    {
    
    }
    
// ---------------------------------------------------------------------------
// CMPMDefaultConnection::IntToCmDefConnType
// ---------------------------------------------------------------------------
//
TCmDefConnType CMPMDefaultConnection::IntToCmDefConnTypeL( const TUint& aInt )
    {
    TCmDefConnType type( TCmDefConnType ( 0 ) );

    if( aInt == ECmDefConnDestination )
        {
        type = ECmDefConnDestination;
        }
    else if( aInt == ECmDefConnConnectionMethod )
        {
        type = ECmDefConnConnectionMethod;
        }
    else
        {
        MPMLOGSTRING2( "CMPMDefaultConnection::IntToCmDefConnType:\
 incorrect value %d", aInt )
        User::Leave( KErrArgument );
        }
    
    return type;    
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnection::ReadDefConnSetting
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnection::ReadDefConnSettingL( TCmDefConnType& aType,
                                                 TUint32&        aId )
    {
    MPMLOGSTRING( "CMPMDefaultConnection::ReadDefConnSettingL " )
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() ); 
    TMDBElementId tableId = CCDDefConnRecord::TableIdL( *db );     

    // fetch value from commsdat using record set to be able 
    // to load any and all records in the table.
    // we don't want to make assumptions on id
    //
    CMDBRecordSet<CMDBGenericRecord> * recordSet = new ( ELeave )
    CMDBRecordSet<CMDBGenericRecord>( tableId );
    CleanupStack::PushL( recordSet );
    recordSet->LoadL( *db );
    
    CCDDefConnRecord* dconn = new (ELeave) CCDDefConnRecord( tableId );
    CleanupStack::PushL( dconn );
    if( recordSet->iRecords.Count() )
        {
        // load the first entry found
        dconn->SetElementId( CMPMCommsDatAccess::GetRealElementId( recordSet->iRecords[0] ) );
        dconn->LoadL( *db );
        aType = IntToCmDefConnTypeL( dconn->iDefConnType );
        aId = dconn->iDefConnUid;
        }
    else
        {
        // nothing found, use default value
        // 
        MPMLOGSTRING( "CMPMDefaultConnection::ReadDefConnSettingL \
no entry found" )
        aType = KDefaultMode;
        aId = KDefaultConnectionId;
        }
    db->Close();
    CleanupStack::PopAndDestroy( dconn );
    CleanupStack::PopAndDestroy( recordSet );
    CleanupStack::PopAndDestroy( db );
    }
