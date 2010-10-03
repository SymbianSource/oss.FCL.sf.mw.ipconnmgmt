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
* Description:   Implementation of the class CProcessorGlobal
*
*/




// INCLUDE FILES

#include "cdclogger.h"
#include "cdcerrors.h"
#include "cdctagcontainer.h"
#include "cdccommon.h"
#include "cmgenconnsettings.h"
#include "cdcprocessorglobal.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1.h>
#else
#include <commsdattypesv1_1.h>
#include <commsdattypesv1_1_partner.h>
#endif
#include <datamobilitycommsdattypes.h>
#include <metadatabase.h>
#include <cmmanagerext.h>
#include <cmmanager.h>
#include <cmdestinationext.h>
#include <WlanCdbCols.h>
#include <wlancontainer.h>
#include <featmgr.h>

// CONSTANTS

// Global settings record name
_LIT16( KGlobalSettingsRecordName, "DefaultRecordName-1" );

// Default GPRS record name
_LIT( KDefaultGPRSRecordName, "Dummy Default GPRS Settings" );

// Modem bearer names for GPRS/WCDMA Access Points
_LIT( KModemBearerPacketData, "GPRS Modem" );

// LAN bearer names for Wlan Access Points
_LIT( KLANBearerWlan, "WLANBearer" );


// Structure that converts field ids to the corresponding table names
// and parameter types
typedef struct
    {
    TDbCreatorGlobalSettings iField;
    const TText* iBearerTableName;
    TBool iIsUIPriority;
    } TPriorityConversion;

const TPriorityConversion KPriorityConversionTable[] =
    {
        { EPriorityLan, KCDTypeNameLANService, EFalse },
        { EPriorityWlan, KCDTypeNameWLANServiceExt, EFalse },
        { EPriorityPan, KCDTypeNamePANServiceExt, EFalse },
        { EPriorityGprsIn, KCDTypeNameOutgoingWCDMA, EFalse },
        { EPriorityGprsOut, KCDTypeNameIncomingWCDMA, EFalse },
        { EPriorityDialOut, KCDTypeNameDialOutISP, EFalse },
        { EPriorityDialIn, KCDTypeNameDialInISP, EFalse },
        { EPriorityVpn, KCDTypeNameVPNService, EFalse },
        { EPriorityMip, KCDTypeNameMIPServiceExt, EFalse },
        { EUIPriorityLan, KCDTypeNameLANService, ETrue },
        { EUIPriorityWlan, KCDTypeNameWLANServiceExt, ETrue },
        { EUIPriorityPan, KCDTypeNamePANServiceExt, ETrue },
        { EUIPriorityGprsIn, KCDTypeNameOutgoingWCDMA, ETrue },
        { EUIPriorityGprsOut, KCDTypeNameIncomingWCDMA, ETrue },
        { EUIPriorityDialOut, KCDTypeNameDialOutISP, ETrue },
        { EUIPriorityDialIn, KCDTypeNameDialInISP, ETrue },
        { EUIPriorityVpn, KCDTypeNameVPNService, ETrue },
        { EUIPriorityMip, KCDTypeNameMIPServiceExt, ETrue },
        { EUnused, NULL, EFalse }
    };


// Maximum size to be allocated for table names
const TUint KMaxTableNameSize = 50;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CProcessorGlobal::NewL
// ---------------------------------------------------------
//
CProcessorGlobal* CProcessorGlobal::NewL( 
                          CReaderBase* aFileReader,
                          RCmManagerExt& aCmManager,
                          RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                          RPointerArray< HBufC >& aPluginNames, 
                          RPointerArray< RCmDestinationExt >& aDestArray,
                          RPointerArray< HBufC >& aDestNames )
    {
    CProcessorGlobal* self = new ( ELeave ) CProcessorGlobal( aFileReader,
                                                              aCmManager,
                                                              aPluginArray,
                                                              aPluginNames,
                                                              aDestArray,
                                                              aDestNames );
    CleanupStack::PushL( self );

    // From base class
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CProcessorGlobal::CProcessorGlobal
// ---------------------------------------------------------
//
CProcessorGlobal::CProcessorGlobal( CReaderBase* aFileReader,
                                    RCmManagerExt& aCmManager,
                                    RPointerArray< RCmConnectionMethodExt >& aPluginArray,
                                    RPointerArray< HBufC >& aPluginNames, 
                                    RPointerArray< RCmDestinationExt >& aDestArray,
                                    RPointerArray< HBufC >& aDestNames ) :
    CProcessorBase( aFileReader, aCmManager, aPluginArray, aPluginNames, aDestArray, aDestNames ),
    iAttachWhenNeeded ( EFalse )
    {
    }
    
    
// ---------------------------------------------------------
// CProcessorGlobal::ConstructL
// ---------------------------------------------------------
//
void CProcessorGlobal::ConstructL()
    {
    iSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
    
    iSession->SetAttributeMask( ECDNoWriteButDelete | ECDHidden | ECDProtectedWrite | ECDPrivate ); // Check this
    
    // Create General connection settings struct and set the default values
    iGenConnSettings = new (ELeave) TCmGenConnSettings;
    iGenConnSettings->iUsageOfWlan = ECmUsageOfWlanKnown;
    iGenConnSettings->iCellularDataUsageHome = ECmCellularDataUsageAutomatic;
    iGenConnSettings->iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;

    CLOG_WRITE( "Initialising FeatureManager\n" )   
    //checks if Wlan feature is supported
    FeatureManager::InitializeLibL();
    iIsWlanS = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    CLOG_WRITE( "FeatureManager closed\n" ) 
    }


// ---------------------------------------------------------
// CProcessorGlobal::~CProcessorGlobal
// ---------------------------------------------------------
//
CProcessorGlobal::~CProcessorGlobal()
    {
    if ( iSession )
        {
        iSession->Close();
        delete iSession; 
        }
    delete iGenConnSettings;    
    }
     
// ---------------------------------------------------------
// CProcessorGlobal::ProcessTagL
// ---------------------------------------------------------
//
void CProcessorGlobal::ProcessTagL( TBool /*aFieldIDPresent*/ )
    {
    for (TInt idx(0); idx < TagContainer().Count(); idx++)
        {  
        TDbCreatorGlobalSettings fieldId;
        
        fieldId = static_cast< TDbCreatorGlobalSettings >
        ( TagContainer().FieldId( idx ) );
        
        HBufC16* ptrTag = TagContainer().Tag(idx);
        
        switch( fieldId )
            {
            case EGPRSAttachMode:
                {
                _LIT( KAttachWhenNeeded, "whenneeded" );
                _LIT( KAttachWhenAvailable, "whenavailable" );
                
                if ( !ptrTag->CompareF( KAttachWhenNeeded ) )
                    {
                    iAttachWhenNeeded = ETrue;
                    }
                else if ( !ptrTag->CompareF( KAttachWhenAvailable ) )
                    {
                    iAttachWhenNeeded = EFalse;
                    }
                else 
                    {
                    iAttachWhenNeeded = EFalse;
                    CLOG_WRITE( "! Error : invalid attach mode. Default:whenavailable" );
                    }

                break;
                }
                
            case EDefaultAP:
                {
                CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
                CleanupStack::PushL( db );
                
                CCDDefaultWCDMARecord* defaultGPRS = static_cast< CCDDefaultWCDMARecord* >
                        ( CCDRecordBase::RecordFactoryL( KCDTIdDefaultWCDMARecord ) );
                
                CleanupStack::PushL( defaultGPRS );
                
                defaultGPRS->iRecordName.SetL( KDefaultGPRSRecordName );

                if ( !defaultGPRS->FindL( *db ) )
                    {
                    CLOG_WRITE( "! Error : CProcessorGlobal::ProcessTagL: Default AP not found" );
                    User::Leave( KErrNotFound );
                    }
                
                defaultGPRS->iAPN.SetL( *ptrTag );
                
                defaultGPRS->ModifyL( *db );
                
                CleanupStack::PopAndDestroy( defaultGPRS );
                
                db->Close();			    			    
                CleanupStack::PopAndDestroy( db );
                break;
                }
                
            case EDefaultDnIcon:
                {
                // Convert default icon to int
                TPtrC16 iconPtr = ptrTag->Right( ptrTag->Length() );
                TLex16 lex( iconPtr );
                TUint32 icon( 0 );
                if ( lex.Val( icon, EDecimal ) == KErrNone )
                    {
                    // If there was no error, write default entry to SNAP
                    // Metadata table.
                    // First load SNAP table to get table id
                    CMDBGenericRecord* snapTable = static_cast< CMDBGenericRecord* >
                            ( CCDRecordBase::RecordFactoryL( 0 ) );
                    CleanupStack::PushL( snapTable );      
                    snapTable->InitializeL( TPtrC( KCDTypeNameSNAPMetadata ),
                            NULL );
                    snapTable->LoadL( *iSession );
                    // Then add default record to table
                    CCDSNAPMetadataRecord* defaultRecord = new( ELeave )
                            CCDSNAPMetadataRecord( snapTable->TableId() );
                    CleanupStack::PushL( defaultRecord );
                    defaultRecord->SetRecordId( KCDNewRecordRequest );
                    defaultRecord->iMetadata.SetL( 0 );
                    defaultRecord->iIcon.SetL( icon );
                    defaultRecord->StoreL( *iSession );
                    
                    CleanupStack::PopAndDestroy( defaultRecord ); // defaultRecord
                    CleanupStack::PopAndDestroy( snapTable ); // snapTable
                    }
                break;
                }
                
            case EPriorityLan:
            case EPriorityWlan:
            case EPriorityPan:
            case EPriorityGprsIn:
            case EPriorityGprsOut:
            case EPriorityDialOut:
            case EPriorityDialIn:
            case EPriorityVpn:
            case EPriorityMip:
            case EUIPriorityLan:
            case EUIPriorityWlan:
            case EUIPriorityPan:
            case EUIPriorityGprsIn:
            case EUIPriorityGprsOut:
            case EUIPriorityDialOut:
            case EUIPriorityDialIn:
            case EUIPriorityVpn:
            case EUIPriorityMip:
                {
                // Convert priority to int
                TPtrC16 prioPtr = ptrTag->Right( ptrTag->Length() );
                TLex16 lex( prioPtr );
                TUint32 prio( 0 );
                if ( lex.Val( prio, EDecimal ) == KErrNone )
                    {
                    UpdateGlobalBearerArrayL( fieldId, prio );
                    }
                break;
                }
                
            case EGprsLastSocketActivityTimeout:
            case EGprsLastSessionClosedTimeout:
            case EGprsLastSocketClosedTimeout:		
                {            				    
                // Load modem bearer record
                CCDModemBearerRecord* bearerRecord = static_cast< CCDModemBearerRecord * >
                ( CCDRecordBase::RecordFactoryL( KCDTIdModemBearerRecord ) );
                CleanupStack::PushL( bearerRecord );
                
                if( fieldId == EGprsLastSocketActivityTimeout
                        || fieldId == EGprsLastSessionClosedTimeout
                        || fieldId == EGprsLastSocketClosedTimeout )
                    {
                    // Open the record "Gprs Modem"
                    bearerRecord->iRecordName.SetL( KModemBearerPacketData );	
                    }				
                
                if( !bearerRecord->FindL( *iSession ) )
                    {
                    CLOG_WRITE( "! Error : CProcessorGlobal::ProcessTagL: Modem bearer record not found" );
                    User::Leave( KErrNotFound );
                    }
                
                // Convert input parameter
                TLex lex( *ptrTag );
                
                TInt timeout( 0 );
                
                // If there are parameters that cannot be handled just leave. In that case the CommsDat creation
                // fails and default is used instead. So it is easier for the variant engineer to detect
                // that something went wrong.
                if ( lex.Val( timeout ) != KErrNone)
                    {
                    CLOG_WRITE( "! Error : invalid NIF parameter for GPRS (socket/session timeout)" );					
                    User::Leave( KErrArgument );
                    }
                
                // Set the parameter	
                if( fieldId == EGprsLastSocketActivityTimeout )
                    {
                    bearerRecord->iLastSocketActivityTimeout.SetL( timeout );
                    }
                else if( fieldId == EGprsLastSessionClosedTimeout )
                    {
                    bearerRecord->iLastSessionClosedTimeout.SetL( timeout );
                    }
                else
                    {
                    bearerRecord->iLastSocketClosedTimeout.SetL( timeout );
                    }
                
                bearerRecord->ModifyL( *iSession );
                
                CleanupStack::PopAndDestroy( bearerRecord );
                
                break;
                }
                
            case EWlanLastSocketActivityTimeout:
            case EWlanLastSessionClosedTimeout:
            case EWlanLastSocketClosedTimeout:            
                {
                if( iIsWlanS )
                    {
                    // Load LAN bearer record
                    CCDLANBearerRecord* bearerRecord = static_cast< CCDLANBearerRecord * >
                    ( CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ) );
                    CleanupStack::PushL( bearerRecord );
                    
                    // Open the record "WlanBearer"
                    bearerRecord->iRecordName.SetL( KLANBearerWlan );	
                    
                    if( !bearerRecord->FindL( *iSession ) )
                        {
                        CLOG_WRITE( "! Error : CProcessorGlobal::ProcessTagL: LAN bearer record not found" );
                        User::Leave( KErrNotFound );
                        }
                    
                    // Convert the input paramater to UINT
                    TLex lex( *ptrTag );
                    
                    TInt timeout( 0 );
                    
                    // If there are parameters that cannot be handled just leave. In that case the CommsDat creation
                    // fails and default is used instead. So it is easier for the variant engineer to detect
                    // that something went wrong.
                    if ( lex.Val( timeout ) != KErrNone)
                        {
                        CLOG_WRITE( "! Error : invalid NIF parameter for WLAN (socket/session timeout)" );					
                        User::Leave( KErrArgument );
                        }
                    
                    
                    // Set the paramter
                    if( fieldId == EWlanLastSocketActivityTimeout )
                        {				
                        bearerRecord->iLastSocketActivityTimeout.SetL( timeout );
                        }
                    else if( fieldId == EWlanLastSessionClosedTimeout)
                        {					
                        bearerRecord->iLastSessionClosedTimeout.SetL( timeout );
                        }
                    else
                        {					
                        bearerRecord->iLastSocketClosedTimeout.SetL( timeout );
                        }
                    
                    bearerRecord->ModifyL( *iSession );				
                    CleanupStack::PopAndDestroy( bearerRecord );
                    }
                break;
                }
                
            case EWlanBGScanInterval:
            case EWlanUseDefSettings:
            case EWlanLongRetry:
            case EWlanShortRetry:
            case EWlanRTS:
            case EWlanTxPowerLevel:
            case EWlanRadioMeasurements:
            case EWlanPowerMode:
                {
                if( iIsWlanS )
                    {
                    // Save the same value both to default and user settings records
                    TInt err;
                    TRAP( err, SaveGlobalWlanParameterL( KWlanDefaultSettings, fieldId, ptrTag ) );
                    if( err != KErrNone )
                        {
                        CLOG_WRITE( "! Error : Could not save global WLAN parameter. Incorrect input?" );
                        User::Leave( err  );
                        }
                    TRAP( err, SaveGlobalWlanParameterL( KWlanUserSettings, fieldId, ptrTag ) );
                    if( err != KErrNone )
                        {
                        CLOG_WRITE( "! Error : Could not save global WLAN parameter. Incorrect input?" );
                        User::Leave( err );
                        }
                    }
                break;
                }
                
            case EUsageOfWlan:
                {
                SetGenConnSettingWlanUsage( ptrTag );
                break;
                }
                
            case ECellularDataUsageHome:
                {
                SetGenConnSettingCellularDataUsageHome( ptrTag );
                break;
                }
                
            case ECellularDataUsageVisitor:
                {
                SetGenConnSettingCellularDataUsageVisitor( ptrTag );
                break;
                }
                
            default:
                break;
            }
        }
    
    if( iBearerArrayChanged )
        {
        
        CLOG_WRITE( "UpdateBearerPriorityArrayL\n" )
        
        iCmManager->UpdateBearerPriorityArrayL( iBearerArray );
        iCmManager->CleanupGlobalPriorityArray( iBearerArray );
        
        iBearerArray.Close();
        
        CLOG_WRITE( "UpdateBearerPriorityArrayL end\n" )
        }
    
    // General Connection settings are always set
    SetGenConnSettingsL();
    }

// ---------------------------------------------------------
// CProcessorGlobal::ProcessAPL
// ---------------------------------------------------------
//
void CProcessorGlobal::ProcessAPL()
    {
    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );
                    
    CLOG_WRITE( "Global settings\n" )
    CLOG_WRITE( "---------------\n" )
        
    CCDGlobalSettingsRecord* globalSettings = static_cast< CCDGlobalSettingsRecord* >
                        ( CCDRecordBase::RecordFactoryL( KCDTIdGlobalSettingsRecord ) );
                        
	CleanupStack::PushL( globalSettings );
                        
    globalSettings->iRecordName.SetL( KGlobalSettingsRecordName );

    if( !globalSettings->FindL( *db ) )
        {
        CLOG_WRITE( "! Error : CProcessorGlobal::ProcessAPL: Global settings not found" );
        User::Leave( KErrNotFound );
        }

    //Gprs attach mode
    if ( iAttachWhenNeeded )
        {
        CLOG_WRITE( "GPRS attach mode : attach when needed\n" )
        globalSettings->iGPRSAttachMode = RPacketService::EAttachWhenNeeded;
        }
    else
        {
        CLOG_WRITE( "GPRS attach mode : attach when available\n" )
        globalSettings->iGPRSAttachMode = RPacketService::EAttachWhenPossible;
        }
        
    globalSettings->ModifyL( *db );
    
    CleanupStack::PopAndDestroy( globalSettings );    
    db->Close();
    CleanupStack::PopAndDestroy( db ); // db
    }
    

// ---------------------------------------------------------
// CProcessorGlobal::UpdateGlobalBearerArrayL
// ---------------------------------------------------------
//
void CProcessorGlobal::UpdateGlobalBearerArrayL
                            ( TDbCreatorGlobalSettings aField, TUint32 aPrio )
    {
    TUint itemIndex( 0 );
    
    // Look up field from conversion table
    while( EUnused != KPriorityConversionTable[ itemIndex ].iField )
        {
        if( KPriorityConversionTable[ itemIndex ].iField == aField)
            {
            break;
            }
        ++itemIndex;
        }
        
    // If found
    if( EUnused != KPriorityConversionTable[ itemIndex ].iField )
        {
        if( !iBearerArrayChanged )
            {
            iCmManager->BearerPriorityArrayL( iBearerArray );
            iBearerArrayChanged = ETrue;
            }
        
        
        HBufC* tableName = HBufC::NewLC( KMaxTableNameSize );
        *tableName = TPtrC( KPriorityConversionTable[ itemIndex ]
                                                        .iBearerTableName );
        TBool foundBearer( EFalse );
        
        // Find the appropriate bearer type
        for( TInt i( 0 ); i < iBearerArray.Count(); ++i )
            {
            if( !( ( iBearerArray )[ i ].iServiceType->Compare( *tableName ) ) )
                {
                if( KPriorityConversionTable[ itemIndex ].iIsUIPriority )
                    {
                    ( iBearerArray )[ i ].iUIPriority = aPrio;
                    }
                else
                    {
                    ( iBearerArray )[ i ].iPriority = aPrio;
                    }
                foundBearer = ETrue;
                break;
                }
            }
        
        // If bearer was not found, add it to the table
        if( !foundBearer )
            {
            TBearerPriority newBearer;
            newBearer.iServiceType = tableName;
            if( KPriorityConversionTable[ itemIndex ].iIsUIPriority )
                {
                newBearer.iUIPriority = aPrio;
                }
            else
                {
                newBearer.iPriority = aPrio;
                }
            iBearerArray.Append( newBearer );
            }

        CleanupStack::PopAndDestroy( tableName ); // tableName 
        }
    }

// ---------------------------------------------------------
// CProcessorGlobal::SetGenConnSettingWlanUsage
// ---------------------------------------------------------
//
void CProcessorGlobal::SetGenConnSettingWlanUsage( HBufC16* aPtrTag )
    {
    // This parameter should't be set at all if WLAN isn't supported.
    // However, CMM should take care of this in WriteGenConnSettingsL().
    if ( aPtrTag->CompareF( KStrManual ) == 0 ) 
        {
        iGenConnSettings->iUsageOfWlan = ECmUsageOfWlanManual;
        }
    // else: Default value for the string is: "Known", set in ConstructL()
    }

// ---------------------------------------------------------
// CProcessorGlobal::SetGenConnSettingCellularDataUsageHome
// ---------------------------------------------------------
//
void CProcessorGlobal::SetGenConnSettingCellularDataUsageHome( HBufC16* aPtrTag )
    {
    if ( aPtrTag->CompareF( KStrAutomatic ) == 0 ) 
        {
        iGenConnSettings->iCellularDataUsageHome = ECmCellularDataUsageAutomatic;
        }
    else if ( iIsWlanS && aPtrTag->CompareF( KStrWlanOnly ) == 0 ) 
        {
        // This value can't be set if WLAN isn't supported
        iGenConnSettings->iCellularDataUsageHome = ECmCellularDataUsageDisabled;
        }
    // else: Default value for the string is: "Confirm", set in ConstructL()
    }

// ---------------------------------------------------------
// CProcessorGlobal::SetGenConnSettingCellularDataUsageVisitor
// ---------------------------------------------------------
//
void CProcessorGlobal::SetGenConnSettingCellularDataUsageVisitor( HBufC16* aPtrTag )
    {
    if ( aPtrTag->CompareF( KStrAutomatic ) == 0 ) 
        {
        iGenConnSettings->iCellularDataUsageVisitor = ECmCellularDataUsageAutomatic;
        }
    else if ( iIsWlanS && aPtrTag->CompareF( KStrWlanOnly ) == 0 ) 
        {
        // This value can't be set if WLAN isn't supported
        iGenConnSettings->iCellularDataUsageVisitor = ECmCellularDataUsageDisabled;
        }
    // else: Default value for the string is: "Confirm", set in ConstructL()
    }

// ---------------------------------------------------------
// CProcessorGlobal::SetGenConnSettingsL
// ---------------------------------------------------------
//
void CProcessorGlobal::SetGenConnSettingsL()
    {
    RCmManager cmManager;
    cmManager.CreateTablesAndOpenL();
    CleanupClosePushL( cmManager );
    cmManager.WriteGenConnSettingsL( *iGenConnSettings );
    CleanupStack::PopAndDestroy( &cmManager );
    }

//-----------------------------------------------------------------------------
//  CProcessorGlobal::SaveGlobalWlanParameterL()
//-----------------------------------------------------------------------------
//
void CProcessorGlobal::SaveGlobalWlanParameterL( const TUint32 aTableType, const TDbCreatorGlobalSettings aField, const HBufC16* const aValue )
	{	
		// Initialize the record
		CMDBGenericRecord* deviceSettingsTable = static_cast< CMDBGenericRecord* >
                ( CCDRecordBase::RecordFactoryL( 0 ) );
                
	    CleanupStack::PushL( deviceSettingsTable );
	    
	    deviceSettingsTable->InitializeL( TPtrC( WLAN_DEVICE_SETTINGS ), NULL );
	    			   			    
	    deviceSettingsTable->LoadL( *iSession );
	    
	    // Open the record that matches aTableType (either default or user settings table)
	    
	    CMDBField<TUint32>* settingsTypeField = (CMDBField<TUint32>*)deviceSettingsTable->GetFieldByIdL(KCDTIdWlanDeviceSettingsType);
	    
	   	settingsTypeField->SetL( aTableType );
	    			   			    
	    if( !deviceSettingsTable->FindL( *iSession ) )
            {
            CLOG_WRITE( "! Error : CProcessorGlobal::SaveGlobalWlanParameterL: Table not found" );
            User::Leave( KErrNotFound );
            }
	    
	    // Convert the input parameter to integer
    	TLex lex( *aValue );
		
    	TInt value( 0 );
        User::LeaveIfError( lex.Val( value ) );
        
		// Set the paramter value
	    
	    if ( aField == EWlanBGScanInterval )
	    	{
	    	CMDBField< TUint32 >* field = ( CMDBField< TUint32 >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdBgScanInterval );
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanUseDefSettings )
	    	{
	    	CMDBField< TBool >* field = ( CMDBField< TBool >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdUseDefaultSettings );	
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanLongRetry )
	    	{
			CMDBField< TUint32 >* field = ( CMDBField< TUint32 >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdWlanLongRetry );				    	
			field->SetL( value );
	    	}
	    else if ( aField == EWlanShortRetry )
	    	{
	    	CMDBField< TUint32 >* field = ( CMDBField< TUint32 >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdWlanShortRetry );	
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanRTS )
	    	{
	    	CMDBField< TUint32 >* field = ( CMDBField< TUint32 >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdWlanRTSThreshold );	
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanTxPowerLevel )
	    	{
	    	CMDBField< TUint32 >* field = ( CMDBField< TUint32 >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdTxPowerLevel );	
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanRadioMeasurements )
	    	{
	    	CMDBField< TBool >* field = ( CMDBField< TBool >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdAllowRadioMeasurements );	
	    	field->SetL( value );
	    	}
	    else if ( aField == EWlanPowerMode )
	    	{
	    	CMDBField< TBool >* field = ( CMDBField< TBool >* ) deviceSettingsTable->GetFieldByIdL( KCDTIdWlanPowerMode );	
	    	field->SetL( value );
	    	}			    			    			      			

		deviceSettingsTable->ModifyL( *iSession );
				
		CleanupStack::PopAndDestroy( deviceSettingsTable );				
	}
// End of File.
