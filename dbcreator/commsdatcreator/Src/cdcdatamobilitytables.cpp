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
* Description:   Implementation of the class CDataMobilityTables
*
*/




// INCLUDE FILES
#include "cdcdatamobilitytables.h"
#include "cdclogger.h"

#include <commsdattypesv1_1.h>
#include <datamobilitycommsdattypes.h>
#include <cmmanagerext.h>
#include <wlancontainer.h>
#include <cmmanagerdef.h>

// CONSTANTS

// Maximum bearer type name length
const TUint KMaxNameLength = 50;

// Default bearer priorities
const TUint KDefaultPriorityLan = 0;
const TUint KDefaultPriorityWlan = 1;
const TUint KDefaultPriorityPan = 2;
const TUint KDefaultPriorityGprsOut = 3;
const TUint KDefaultPriorityGprsIn = 4;
const TUint KDefaultPriorityDialOut = 6;
const TUint KDefaultPriorityDialIn = 7;
const TUint KDefaultUIPriorityVpn = 253;
const TUint KDefaultUIPriorityMip = 254;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDataMobilityTables::NewL
// ---------------------------------------------------------
//
CDataMobilityTables* CDataMobilityTables::NewL()
    {

    CDataMobilityTables* self = new ( ELeave ) CDataMobilityTables;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CDataMobilityTables::CDataMobilityTables
// ---------------------------------------------------------
//
CDataMobilityTables::CDataMobilityTables()
    {
    //no implementation needed
    }


// ---------------------------------------------------------
// CDataMobilityTables::ConstructL
// ---------------------------------------------------------
//
void CDataMobilityTables::ConstructL()
    {
    //no implementation needed
    iSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    }


// ---------------------------------------------------------
// CDataMobilityTables::~CDataMobilityTables
// ---------------------------------------------------------
//
CDataMobilityTables::~CDataMobilityTables()
    {
    if ( iSession )
        {
        iSession->Close();
        delete iSession; 
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CheckAndCreateAlrTablesL
// ---------------------------------------------------------
//
void CDataMobilityTables::CheckAndCreateAlrTablesL()
    {
    CreateVirtualIAPNextLayerTableL();
    CreateGlobalBearerTypeTableL();
    PopulateGlobalBearerTypeTableL();
    CreateDataMobilitySelectionPolicyTableL();
    CreateSNAPMetadataTableL();
    CreateIAPMetadataTableL();
    CreateDestNetworkTableL();
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CheckAndCreateMipTablesL
// ---------------------------------------------------------
//
void CDataMobilityTables::CheckAndCreateMipTablesL()
    {
    CreateMIPServiceTableL();
    CreateMIP4TableL();
    CreateMIP6TableL();
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateGlobalBearerTypeTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateGlobalBearerTypeTableL()
    {
    //Check whether Global Bearer Type Priorization table exists
    TInt error( KErrNone );
    
    TRAP( error, CCDGlobalBearerTypePriorizationRecord::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDGlobalBearerTypePriorizationRecord::CreateTableL( *iSession );

        //check result
        TInt id( KErrNone );
        TRAP( id, CCDGlobalBearerTypePriorizationRecord::TableIdL( *iSession ) );
        if( id >= KErrNone )
            {
            CLOG_WRITE( "Added Global Bearer Type Priorization table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add Global Bearer Type Priorization\
                             table: %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE ( "Global Bearer Type Priorization table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ------------------------------------------------------------
// CDataMobilityTables::CreateDataMobilitySelectionPolicyTableL
// ------------------------------------------------------------
//
void CDataMobilityTables::CreateDataMobilitySelectionPolicyTableL()
    {
    //Check whether Data Mobility Selection Policy table exists
    TInt error( KErrNone );
    TRAP( error, CCDDataMobilitySelectionPolicyRecord::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDDataMobilitySelectionPolicyRecord::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDDataMobilitySelectionPolicyRecord::TableIdL( *iSession ) );
        if( id >= KErrNone )
            {
            CLOG_WRITE( "Added Data Mobility Selection Policy table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( 
            "Failed to add Data Mobility Selection Policy table. %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "Data Mobility Selection Policy table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateMIPServiceTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateMIPServiceTableL()
    {
    //Check whether MIP Service table exists
    TInt error( KErrNone );
    TRAP( error, CCDMIPServiceExtRecord::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDMIPServiceExtRecord::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDMIPServiceExtRecord::TableIdL( *iSession ) );
        if( KErrNone <= id )
            {
            CLOG_WRITE( "Added MIP Service table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add MIP Service table: %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "MIP Service table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateMIP4TableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateMIP4TableL()
    {
    //Check whether MIP4 table exists
    TInt error( KErrNone );
    TRAP( error, CCDMIP4Record::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDMIP4Record::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDMIP4Record::TableIdL( *iSession ) );
        if( KErrNone <= id )
            {
            CLOG_WRITE( "Added MIP4 table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add MIP4 table: %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "MIP4 table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateMIP6TableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateMIP6TableL()
    {
    //Check whether MIP6 table exists
    TInt error( KErrNone );
    TRAP( error, CCDMIP6Record::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDMIP6Record::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDMIP6Record::TableIdL( *iSession ) );
        if( KErrNone <= id )
            {
            CLOG_WRITE( "Added MIP6 table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add MIP6 table %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
            CLOG_WRITE( "MIP6 table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateSNAPMetadataTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateSNAPMetadataTableL()
    {
    //Check whether SNAP Metadata table exists
    TInt error( KErrNone );
    TRAP( error, CCDSNAPMetadataRecord::TableIdL( *iSession ) );
    
    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDSNAPMetadataRecord::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDSNAPMetadataRecord::TableIdL( *iSession ) );
        if( KErrNone <= id )
            {
            CLOG_WRITE( "Added SNAP Metadata table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add SNAP Metadata table? %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "SNAP Metadata table already exists" )
        }
    else
        {
        //do nothing             
        }
    }
    
    
// ---------------------------------------------------------
// CDataMobilityTables::CreateIAPMetadataTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateIAPMetadataTableL()
    {
    //Check whether IAP Metadata table exists
    TInt error( KErrNone );
    TRAP( error, CCDIAPMetadataRecord::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDIAPMetadataRecord::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDIAPMetadataRecord::TableIdL( *iSession ) );
        if( KErrNone <= id )
            {
            CLOG_WRITE( "Added IAP Metadata table\n" )
            }
        else
            {
            CLOG_WRITE_FORMAT( "Failed to add IAP Metadata table: %d\n", id )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "IAP Metadata table already exists" )
        }
    else
        {
        //do nothing             
        }
    }


// ---------------------------------------------------------
// CDataMobilityTables::CreateVirtualIAPNextLayerTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateVirtualIAPNextLayerTableL()
    {
    //Check whether IAP Metadata table exists
    TInt error( KErrNone );
    TRAP( error, CCDVirtualIAPNextLayerRecord::TableIdL( *iSession ) );

    //Table not found -> create it
    if( error == KErrNotFound )
        {
        //Add table to CommsDat
        CCDVirtualIAPNextLayerRecord::CreateTableL( *iSession );
        
        //check result
        TInt id( KErrNone );
        TRAP( id, CCDVirtualIAPNextLayerRecord::TableIdL( *iSession ) );
        if( KErrNone > id )
            {
            CLOG_WRITE_FORMAT( "Failed to add Virtual IAP Next Layer table %d\n", id )
            }
        else
            {
            CLOG_WRITE( "Added Virtual IAP Next Layer table\n" )
            }
        }
    //Table already exists
    else if ( error == KErrNone)
        {
        CLOG_WRITE( "Virtual IAP Next Layer table already exists" )
        }
    else
        {
        //do nothing             
        }
    }

// ---------------------------------------------------------
// CreateDestNetworkTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::CreateDestNetworkTableL()
    {    
    //checks whether table exists
    TInt error( KErrNone );

    TRAP( error, CCDDestNWTable::TableIdL( *iSession ) );
    if( error == KErrNotFound )
        {    
        //table not found -- add new table
        CCDDestNWTable::CreateTableL( *iSession );
        
        //check result
        TRAP( error, CCDDestNWTable::TableIdL( *iSession ) );
        
        if ( error == KErrNone )
            {
            CLOG_WRITE( "Added Destination Network table" )
            }
        }
    else if ( error == KErrNone)
        {
            CLOG_WRITE( "Destination Network table already exists" )
        }
    else
        {
        //do nothing             
        }  
    }

// ---------------------------------------------------------
// CDataMobilityTables::PopulateGlobalBearerTypeTableL
// ---------------------------------------------------------
//
void CDataMobilityTables::PopulateGlobalBearerTypeTableL()
    {
    RCmManagerExt cmManager;
    cmManager.CreateTablesAndOpenL();
    
    CleanupClosePushL( cmManager );
    
    RArray< TBearerPriority >* bearerArray = new( ELeave )
                                            RArray< TBearerPriority >();
    
    CleanupStack::PushL( bearerArray );
                           
    // Check if the bearer table has already been filled
    // (and skip populating in this case)
    
    cmManager.BearerPriorityArrayL( *bearerArray );
    
    if( !bearerArray->Count() )
        {
        // Prepare bearer priority array
        // Strings are needed to be available until the update of the bearer array
        // (TBearerPriority has a HBufC* member)
        HBufC* lanServiceName = HBufC::NewLC( KMaxNameLength );
        *lanServiceName = TPtrC( KCDTypeNameLANService );
        HBufC* wLanServiceName = HBufC::NewLC( KMaxNameLength );
        *wLanServiceName = TPtrC( KCDTypeNameWLANServiceExt );
        HBufC* panServiceName = HBufC::NewLC( KMaxNameLength );
        *panServiceName = TPtrC( KCDTypeNamePANServiceExt );
        HBufC* gprsOutServiceName = HBufC::NewLC( KMaxNameLength );
        *gprsOutServiceName = TPtrC( KCDTypeNameOutgoingWCDMA );
        HBufC* gprsInServiceName = HBufC::NewLC( KMaxNameLength );
        *gprsInServiceName = TPtrC( KCDTypeNameIncomingWCDMA );
        HBufC* dialOutServiceName = HBufC::NewLC( KMaxNameLength );
        *dialOutServiceName = TPtrC( KCDTypeNameDialOutISP );
        HBufC* dialInServiceName = HBufC::NewLC( KMaxNameLength );
        *dialInServiceName = TPtrC( KCDTypeNameDialInISP );
        HBufC* vpnServiceName = HBufC::NewLC( KMaxNameLength );
        *vpnServiceName = TPtrC( KCDTypeNameVPNService );
        HBufC* mipServiceName = HBufC::NewLC( KMaxNameLength );
        *mipServiceName = TPtrC( KCDTypeNameMIPServiceExt );
        
        bearerArray->Reset();
        
        TBearerPriority lanRecord;
        lanRecord.iServiceType = lanServiceName;
        lanRecord.iPriority = KDefaultPriorityLan;
        lanRecord.iUIPriority = KDefaultPriorityLan;
        bearerArray->Append( lanRecord );
        
        TBearerPriority wLanRecord;
        wLanRecord.iServiceType = wLanServiceName;
        wLanRecord.iPriority = KDefaultPriorityWlan;
        wLanRecord.iUIPriority = KDefaultPriorityWlan;
        bearerArray->Append( wLanRecord );
        
        TBearerPriority panRecord;
        panRecord.iServiceType = panServiceName;
        panRecord.iPriority = KDefaultPriorityPan;
        panRecord.iUIPriority = KDefaultPriorityPan;
        bearerArray->Append( panRecord );
        
        TBearerPriority gprsOutRecord;
        gprsOutRecord.iServiceType = gprsOutServiceName;
        gprsOutRecord.iPriority = KDefaultPriorityGprsOut;
        gprsOutRecord.iUIPriority = KDefaultPriorityGprsOut;
        bearerArray->Append( gprsOutRecord );
        
        TBearerPriority gprsInRecord;
        gprsInRecord.iServiceType = gprsInServiceName;
        gprsInRecord.iPriority = KDefaultPriorityGprsIn;
        gprsInRecord.iUIPriority = KDefaultPriorityGprsIn;
        bearerArray->Append( gprsInRecord );
                        
        TBearerPriority dialOutRecord;
        dialOutRecord.iServiceType = dialOutServiceName;
        dialOutRecord.iPriority = KDefaultPriorityDialOut;
        dialOutRecord.iUIPriority = KDefaultPriorityDialOut;
        bearerArray->Append( dialOutRecord );
        
        TBearerPriority dialInRecord;
        dialInRecord.iServiceType = dialInServiceName;
        dialInRecord.iPriority = KDefaultPriorityDialIn;
        dialInRecord.iUIPriority = KDefaultPriorityDialIn;
        bearerArray->Append( dialInRecord );
        
        TBearerPriority vpnRecord;
        vpnRecord.iServiceType = vpnServiceName;
        vpnRecord.iPriority = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
        vpnRecord.iUIPriority = KDefaultUIPriorityVpn;
        bearerArray->Append( vpnRecord );
        
        TBearerPriority mipRecord;
        mipRecord.iServiceType = mipServiceName;
        mipRecord.iPriority = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
        mipRecord.iUIPriority = KDefaultUIPriorityMip;
        bearerArray->Append( mipRecord );
        
        cmManager.UpdateBearerPriorityArrayL( *bearerArray );
        
        CLOG_WRITE( "Global Bearer Priorization table updated with default values\n" )
        
        // Only pop, because ownership is passed to the array
        CleanupStack::Pop( mipServiceName ); 
        CleanupStack::Pop( vpnServiceName );
        CleanupStack::Pop( dialInServiceName ); 
        CleanupStack::Pop( dialOutServiceName );
        CleanupStack::Pop( gprsInServiceName );
        CleanupStack::Pop( gprsOutServiceName ); 
        CleanupStack::Pop( panServiceName );
        CleanupStack::Pop( wLanServiceName ); 
        CleanupStack::Pop( lanServiceName );
        }
        
    else
        {
        CLOG_WRITE( "Not updating Global Bearer Priorization table\n" )
        }
    
    cmManager.CleanupGlobalPriorityArray( *bearerArray );

    CleanupStack::PopAndDestroy( bearerArray ); 
    CleanupStack::PopAndDestroy( &cmManager );
    }


//  End of File.
