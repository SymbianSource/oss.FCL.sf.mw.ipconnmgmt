/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the class CProcessorCSD
*
*/




// INCLUDE FILES
#include <wlancontainer.h>
#include <cmmanagertablefields.h>
#include <datamobilitycommsdattypes.h>
#include <f32file.h>
#include <featmgr.h>

#include "cdcdumptables.h"
#include "cdclogger.h"

// DEFINES

#define QUERY_HBUFC_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<HBufC*>*, record->GetFieldByIdL( fieldId ))
#define QUERY_HBUFC8_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<HBufC8*>*, record->GetFieldByIdL( fieldId ))
#define QUERY_UINT32_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TUint32>*, record->GetFieldByIdL( fieldId ))
#define QUERY_INT_FIELD( record, fieldId ) *STATIC_CAST(CommsDat::CMDBField<TInt>*, record->GetFieldByIdL( fieldId ))


// CONSTANTS

// commons for CSV file creatin
_LIT16( KComma, "," );
_LIT16( KNewLine, "\n" );
_LIT16( KCRT, "\r" );
_LIT16( KDQuote, "\"" );

// Table names for CSV file creation
_LIT16( KCDWlanServiceRecord,        "CCDWlanService" );
_LIT16( KCDWlanDeviceSettingsRecord, "CCDWlanDeviceSettings" );
_LIT16( KCDWLANSecSSIDTable,         "CCDWLANSecSSIDTable" );
_LIT16( KCDDestNWTable,              "CCDDestNWTable" );
_LIT16( KCDVirtualIAPNextLayerRecord,"CCDVirtualIAPNextLayer" );
_LIT16( KCDDataMobilitySelectionPolicyRecord, "CCDDataMobilitySelectionPolicy" );
_LIT16( KCDSNAPMetadataRecord,       "CCDSNAPMetadata" );
_LIT16( KCDGlobalBearerTypePriorizationRecord, "CCDGlobalBearerTypePriorization" );
_LIT16( KCDMIPServiceExtRecord,      "CCDMIPServiceExt" );
_LIT16( KCDIAPMetadataRecord,        "CCDIAPMetadata" );
_LIT16( KCDMIP4Record,               "CCDMIP4" );
_LIT16( KCDMIP6Record,               "CCDMIP6" );


// Little-endian Unicode file header.
const TUint16 KLittleEndianUnicodeHeader = 0xFEFF;
// Buffer length for number conversion
const TInt KFormatBufLen = 40;


// ---------------------------------------------------------
// CDumpTables::NewL
// ---------------------------------------------------------
//
CDumpTables* CDumpTables::NewL()
    {
    CDumpTables* self = NewLC();
    CleanupStack::Pop( self ); // self
    return self;
    }

// ---------------------------------------------------------
// CDumpTables::NewLC
// ---------------------------------------------------------
//
CDumpTables* CDumpTables::NewLC()
    {
    CDumpTables* self = new ( ELeave ) CDumpTables();
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    return self;
    }
     
     
// ---------------------------------------------------------
// CDumpTables::~CDumpTables
// ---------------------------------------------------------
//
CDumpTables::~CDumpTables()
    {
    iRfs.Close();
    if (iSession)
        {
        iSession->Close();
        delete iSession;        
        }      
    }    

// ---------------------------------------------------------
// CDumpTables::ConstructL
// ---------------------------------------------------------
//
void CDumpTables::ConstructL()
    {
    iSession = CommsDat::CMDBSession::NewL( 
               CommsDat::CMDBSession::LatestVersion() );
    }
    
    
// ---------------------------------------------------------
// CDumpTables::CDumpTables
// ---------------------------------------------------------
//
CDumpTables::CDumpTables()
    {
    
    }
    
// ---------------------------------------------------------
// CDumpTables::DoDumpTablesL
// ---------------------------------------------------------
//
void CDumpTables::DoDumpTablesL( const TFileName& aOutFileName )
    {

    User::LeaveIfError( OpenFileL( aOutFileName ) );


    //checks if Wlan feature is supported
    FeatureManager::InitializeLibL();
    TBool isWlanS( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) );
    FeatureManager::UnInitializeLib();
    
    if( isWlanS )
        {
        // Dumps CCDWlanService Table 
        DumpTableL<CCDWlanServiceRecord>( &KCDWlanServiceRecord );

        // Dumps CCDWlanDeviceSettings Table  
        DumpTableL<CCDWlanDeviceSettingsRecord>( &KCDWlanDeviceSettingsRecord );

        // Dumps CCDWLANSecSSID Table  
        DumpTableL<CCDWLANSecSSIDTable>( &KCDWLANSecSSIDTable );
    
        // Dumps CCDDestNWTable Table  
        DumpTableL<CCDDestNWTable>( &KCDDestNWTable );
        }
        
    // Dumps CCDVirtualIAPNextLayerRecord Table  
    DumpTableL<CCDVirtualIAPNextLayerRecord>( 
              &KCDVirtualIAPNextLayerRecord );

    // Dumps CCDDataMobilitySelectionPolicyRecord Table  
    DumpTableL<CCDDataMobilitySelectionPolicyRecord>(
              &KCDDataMobilitySelectionPolicyRecord );

    // Dumps CCDSNAPMetadataRecord Table  
    DumpTableL<CCDSNAPMetadataRecord>( &KCDSNAPMetadataRecord );

    // Dumps CCDGlobalBearerTypePriorizationRecord Table  
    DumpTableL<CCDGlobalBearerTypePriorizationRecord>( 
              &KCDGlobalBearerTypePriorizationRecord );

    // Dumps CCDMIPServiceExtRecord Table  
    DumpTableL<CCDMIPServiceExtRecord>( &KCDMIPServiceExtRecord );

    // Dumps CCDIAPMetadataRecord Table  
    DumpTableL<CCDIAPMetadataRecord>( &KCDIAPMetadataRecord );
    
    // Dumps CCDMIP4Record Table  
    DumpTableL<CCDMIP4Record>( &KCDMIP4Record );
    
    // Dumps CCDMIP6Record Table  
    DumpTableL<CCDMIP6Record>( &KCDMIP6Record );
    
    iFile.Close();   
    }

  
// ---------------------------------------------------------
// CDumpTables::DumpTableL
// ---------------------------------------------------------
//
template<class T> void CDumpTables::DumpTableL( const TDesC* aTableName )
    {
    
    CLOG_WRITE_FORMAT( "Dump table: %S \n", aTableName );

    iFile.WriteL( *aTableName );    
    iFile.WriteL( KNewLine );    

    // Retreives the table the recordset and the recordinfo
    TMDBElementId tableId = T::TableIdL( *iSession );
   
    T* table = new (ELeave) T( tableId );

    const SRecordTypeInfo* aRecordInfo = table->GetRecordInfo();              

    CommsDat::CMDBRecordSet<T>* aRecordSet = 
              new (ELeave) CommsDat::CMDBRecordSet<T>( tableId );
    
    // Loads the table          
    TRAP_IGNORE( aRecordSet->LoadL( *iSession ) );

    //The table has been loaded
    TInt count = aRecordSet->iRecords.Count();
    CLOG_WRITE_FORMAT("Record count: %d", count );
        
      
    TBuf16<KFormatBufLen> buf;
    
    // Goes throught the records        
    for ( TInt i = 0; i < count; i++ )
        {
        // The closing (after last) field in the record has 0 iTypeId 
        // to indicate the end of the record. 
        for ( TInt j = 0; 0 < aRecordInfo[j].iTypeId; j++ )
            {
            // Switch for different data types
            switch( aRecordInfo[j].iValType )
                {
                case EInt:
                case EBool:
                    {
                    // Dumping TInt
                    TInt res( QUERY_INT_FIELD( 
                          (*aRecordSet)[i], aRecordInfo[j].iTypeId ) );
                    buf.Format( _L( "%d" ), res );
                    iFile.WriteL ( buf );
                    }
                break;
                
                case EUint32:
                case ELink:
                    {
                    // Dumping TUint32
                    TUint32 res32( QUERY_UINT32_FIELD( 
                            (*aRecordSet)[i], aRecordInfo[j].iTypeId ) );
                    buf.Format( _L( "%d" ), res32 );
                    iFile.WriteL ( buf );
                    }
                break;

                case EText:
                case EMedText:
                case ELongText:
                    {
                    // Dumping text16
                    HBufC16* text16 = QUERY_HBUFC_FIELD( 
                            (*aRecordSet)[i], aRecordInfo[j].iTypeId );
                    WriteL ( text16 );
                    }
                break;
                
                case EDesC8:
                    {
                    // Dumping text8
                    HBufC8* text8 = QUERY_HBUFC8_FIELD( 
                            (*aRecordSet)[i], aRecordInfo[j].iTypeId );
            
                    WriteL ( text8 );
                    }
                break;

                default:
                    CLOG_WRITE_FORMAT("! warning: type not supported: %d", 
                                                aRecordInfo[j].iValType );
                break;                                                
                }
                iFile.WriteL( KComma );
            
            }
            iFile.WriteL( KNewLine );
        }
    }
    
// ---------------------------------------------------------
// CDumpTables::OpenFileL
// ---------------------------------------------------------
//
TInt CDumpTables::OpenFileL( const TFileName& aFileName )
    {
    ////CLOG_WRITE_FORMAT( "Output file name %S", aFileName ); 
    
    TInt err = iRfs.Connect();

    if( err == KErrNone )
        {
        err = iFile.Replace( iRfs, aFileName, EFileStreamText | EFileWrite );
        }
    
    if( err == KErrNotFound ) 
        // file does not exist - create it
        {
        err=iFile.Create( iRfs, aFileName, EFileStreamText | EFileWrite );
        }
    
    if( err == KErrNone )
        {
        // Write little-endian Unicode file header.
        iFile.WriteUint16L( KLittleEndianUnicodeHeader );
        }
        
    if( err != KErrNone )
        {
        iFile.Close();
        ////CLOG_WRITE_FORMAT( "!Error Output file cannot be opened: %S", aFileName );
        }
        
    return err;
    }


// ---------------------------------------------------------
// CDumpTables::WriteL
// ---------------------------------------------------------
//
void CDumpTables::WriteL( HBufC8* aDes )
    {

    if( aDes )
        {
    
        CleanupStack::PushL( aDes );

        HBufC16* text16 = HBufC16::NewL( aDes->Length() );
        text16->Des().Copy( aDes->Des() );
    
        WriteL ( text16 );

        CleanupStack::PopAndDestroy( aDes );
        }

    }    

    
// ---------------------------------------------------------
// CDumpTables::WriteL
// ---------------------------------------------------------
//
void CDumpTables::WriteL( HBufC16* aDes )
    {
    
    if( aDes )
        {
        CleanupStack::PushL( aDes );
        
        TBool res = CheckCSVTransformNeedL( aDes );

        // Need conversion
        if( res )
            {
            HBufC16* cvsFormat = TransformToCSVFormatLC( aDes );
            iFile.WriteL ( *cvsFormat );
            CleanupStack::PopAndDestroy( cvsFormat );
            }
        else
            {
            iFile.WriteL ( *aDes );
            }
        CleanupStack::PopAndDestroy( aDes );
        }
    }    


// ---------------------------------------------------------
// CDumpTables::TransformToCSVFormatLC
// ---------------------------------------------------------
//
HBufC16* CDumpTables::TransformToCSVFormatLC( const HBufC16* aField )
    {
    CLOG_WRITE_FORMAT( "Transform needed:  %S", aField );

    // Length of the csv format string is calculated.
    // "2" are the leading and trailing d.qquotes
    TInt csvLen = aField->Length() + iNumOfDQuotes + 2; 
    HBufC16* csv = HBufC16::NewLC( csvLen );
    
    TPtr16 ptr = csv->Des();

    // Heading quote    
    ptr.Append( KDQuote );
    ptr.Append( *aField );
    
    // Quotes might be duplicated
    if( 0 < iNumOfDQuotes )
        {
        // Heading quote step over
        TInt pos = 1;
        while( pos < ptr.Length() && pos != KErrNotFound )
            {
            ptr = ptr.Mid( pos );
            pos = ptr.Find( KDQuote );
            if( pos != KErrNotFound )
                {
                ptr.Insert( pos, KDQuote );
                pos+=2;                
                }
            }
        }

    // Tailing quote
    ptr.Append( KDQuote );

    CLOG_WRITE_FORMAT( "Transformed:      %S", csv );
 
    return csv;
    }


// ---------------------------------------------------------
// CDumpTables::CheckCSVTransformNeedL
// ---------------------------------------------------------
//
TBool CDumpTables::CheckCSVTransformNeedL( HBufC16* aField )
    {
    // Looks for comma, new line or carrige return characters.
    // Only existance is necessary. If any of them exist the
    // quoting is necessary  
    TInt cnrPos = aField->Find( KComma );
    if( cnrPos == KErrNotFound )
        {
        cnrPos = aField->Find( KNewLine );
        if( cnrPos == KErrNotFound )
            {
            cnrPos = aField->Find( KCRT );
            }
        }
    
    // Serches for all dquote. Number is usefull for csv text creation
    iNumOfDQuotes = 0;        
    TPtr16 ptr = aField->Des();
    
    TInt quotePos = ptr.Find( KDQuote );
    while( quotePos != KErrNotFound )
        {
        iNumOfDQuotes++;
        ptr = ptr.Mid( quotePos+1 );
        quotePos = ptr.Find( KDQuote );
        }
      
    // csv file conversion necessary if comma... or quote is in the text.        
    return ( 0 < iNumOfDQuotes || cnrPos != KErrNotFound );
    }
