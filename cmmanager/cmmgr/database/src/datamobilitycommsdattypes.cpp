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
* Description:
*
*/


#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include "datamobilitycommsdattypes.h"

using namespace CommsDat;

// Following defines implementations for member functions 
// created by DATA_VTABLE macro in the container classes.
// This also creates virtual data table (iVDataTable member) 
// for the classes.
START_ATTRIBUTE_TABLE( CCDVirtualIAPNextLayerRecord, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDVirtualIAPNextLayerRecord, iIAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDVirtualIAPNextLayerRecord, iNextLayerSNAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDVirtualIAPNextLayerRecord, iNextLayerIAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDGlobalBearerTypePriorizationRecord, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDGlobalBearerTypePriorizationRecord, iServiceType, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDGlobalBearerTypePriorizationRecord, iPriority, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDGlobalBearerTypePriorizationRecord, iUIPriority, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()


START_ATTRIBUTE_TABLE( CCDDataMobilitySelectionPolicyRecord,    0,      0 )
    X_REGISTER_ATTRIBUTE( CCDDataMobilitySelectionPolicyRecord, iSNAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDDataMobilitySelectionPolicyRecord, iIAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDDataMobilitySelectionPolicyRecord, iEmbeddedSNAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDDataMobilitySelectionPolicyRecord, iPriority, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDMIPServiceExtRecord, 0, 0)
    X_REGISTER_ATTRIBUTE( CCDMIPServiceExtRecord, iSNAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIPServiceExtRecord, iIAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIPServiceExtRecord, iMIP4, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIPServiceExtRecord, iMIP6, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName,TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDSNAPMetadataRecord, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDSNAPMetadataRecord, iSNAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDSNAPMetadataRecord, iMetadata, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDSNAPMetadataRecord, iIcon, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDSNAPMetadataRecord, iIconFileName, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName,TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDIAPMetadataRecord, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDIAPMetadataRecord, iIAP, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDIAPMetadataRecord, iMetadata, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDIAPMetadataRecord, iSeamlessness, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDIAPMetadataRecord, iIconFileName, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDMIP4Record, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iHomeAddress, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMaxRegRetries, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iFirstRetryTimeout, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iInitialLifetime, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iNAI, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iHomeAgentAddress, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iSecondaryHomeAgentAddress, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iEnableReverseTunnel, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iEnableNATTraversal, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iEnableMinimalEncapsulation, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNHAAuthAlgorithm, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNHASPIIndicator, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNHASPI, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNHASSData, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNAAAAuthAlgorithm, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNAAASPIIndicator, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNAAASPI, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNAAASSData, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iUseForeignAgent, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNFAAuthAlgorithm, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNFASPIIndicator, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNFASPI, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP4Record, iMNFASSData, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CCDMIP6Record, 0, 0 )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iHomeAddress, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iHomeAgentAddress, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iPrefixLength, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iEnableRouteOptimization, TMDBNum ) 
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iEnableIPSec, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iMNHAAuthAlgorithm, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iMNHAEncryptAlgorithm, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iMNHASPIIndicator, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iMNHASPI, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iMNHASSData, TMDBLongText )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iHookState, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iIFName, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDMIP6Record, iTunnelIFName, TMDBText )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordTag, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDRecordBase, iRecordName, TMDBText )
END_ATTRIBUTE_TABLE()

// Following initializes the static member iRecordInfo of the container classes
const SRecordTypeInfo CCDVirtualIAPNextLayerRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
	      ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
		{ KCDTIdVirtualIAP,    	            EInt,  	
		  ENotNull,                         KCDTypeNameVirtualIAP }, 
		{ KCDTIdVirtualNextLayerSNAP,       EInt,		
		  ENoAttrs,                         KCDTypeNameVirtualIAPNextLayerSNAP },
		{ KCDTIdVirtualNextLayerIAP,        EInt,		
		  ENoAttrs,                         KCDTypeNameVirtualIAPNextLayerIAP },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};

const SRecordTypeInfo CCDGlobalBearerTypePriorizationRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
	      ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
		{ KCDTIdGlobalServiceType,    	    EText,  	
		  ENotNull,                         KCDTypeNameGlobalServiceType }, 
		{ KCDTIdGlobalServicePriority,      EInt,		
		  ENoAttrs,                         KCDTypeNameGlobalServicePriority },
		{ KCDTIdGlobalServiceUIPriority,    EInt,		
		  ENoAttrs,                         KCDTypeNameGlobalServiceUIPriority },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};

const SRecordTypeInfo CCDDataMobilitySelectionPolicyRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                                EInt,		
		  ENoAttrs,     KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	                        EText,		
  		  ENotNull,     KCDTypeNameRecordName },
		{ KCDTIdDataMobilitySelectionPolicyNetwork,         EInt,  		
		  ENotNull,     KCDTypeNameDataMobilitySelectionPolicySNAP },
		{ KCDTIdDataMobilitySelectionPolicyIAP,    	        EInt,  		
		  ENoAttrs,     KCDTypeNameDataMobilitySelectionPolicyIAP },
		{ KCDTIdDataMobilitySelectionPolicyEmbeddedSNAP,    EInt,  		
		  ENoAttrs,     KCDTypeNameDataMobilitySelectionPolicyEmbeddedSNAP },
		{ KCDTIdDataMobilitySelectionPolicyPriority,    	EInt,  		
		  ENotNull,     KCDTypeNameDataMobilitySelectionPolicyPriority },
  		{ 0,                                                0,          
  		  ENoAttrs,     KCDNull }
  	};

const SRecordTypeInfo CCDMIPServiceExtRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
		  ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
		{ KCDTIdMIPMIP4,    		        EInt,  		
		  ENoAttrs,                         KCDTypeNameMIPMIP4 },
		{ KCDTIdMIPMIP6,    		        EInt,  		
		  ENoAttrs,                         KCDTypeNameMIPMIP6 },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};

const SRecordTypeInfo CCDSNAPMetadataRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
		  ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
		{ KCDTIdSNAPMetadataSNAP,           EInt,  		
		  ENoAttrs,                         KCDTypeNameSNAPMetadataSNAP },
		{ KCDTIdSNAPMetadataMetadata,       EInt,  		
		  ENoAttrs,                         KCDTypeNameSNAPMetadataMetadata },
		{ KCDTIdSNAPMetadataIcon,           EInt,  	
		  ENoAttrs,                         KCDTypeNameSNAPMetadataIcon },
        { KCDTIdSNAPMetadataIconFileName,   ELongText,
          ENoAttrs,                         KCDTypeNameSNAPMetadataIconFileName },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};
  	
const SRecordTypeInfo CCDIAPMetadataRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
		  ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
		{ KCDTIdIAPMetadataIAP,             EInt,  		
		  ENotNull,                         KCDTypeNameIAPMetadataIAP },
		{ KCDTIdIAPMetadataMetadata,        EInt,  		
		  ENoAttrs,                         KCDTypeNameIAPMetadataMetadata },
		{ KCDTIdIAPMetadataSeamlessness,    EInt,  	    
		  ENoAttrs,                         KCDTypeNameIAPMetadataSeamlessness },
        { KCDTIdIAPMetadataIconFileName,    ELongText,
          ENoAttrs,                         KCDTypeNameIAPMetadataIconFileName },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};

const SRecordTypeInfo CCDMIP4Record::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
	      ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
        { KCDTIdMIP4HomeAddress,    	    EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4HomeAddress },
  		{ KCDTIdMIP4MaxRegRetries,	        EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4MaxRegRetries },
  		{ KCDTIdMIP4FirstRetryTimeout,	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4FirstRetryTimeout },
  		{ KCDTIdMIP4InitialLifetime, 	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4InitialLifetime },
  		{ KCDTIdMIP4NAI,    	            EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4NAI },
  		{ KCDTIdMIP4HomeAgentAddress,    	EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4HomeAgentAddress },
  		{ KCDTIdMIP4SecondaryHomeAgentAddress, EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4SecondaryHomeAgentAddress },
  		{ KCDTIdMIP4EnableReverseTunnel,	EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4EnableReverseTunnel },
  		{ KCDTIdMIP4EnableNATTraversal,	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4EnableNATTraversal },
  		{ KCDTIdMIP4EnableMinimalEncapsulation,	EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4EnableMinimalEncapsulation },
  		{ KCDTIdMIP4MNHAAuthAlgorithm,	    EInt,		
	      ENotNull,                         KCDTypeNameMIP4MNHAAuthAlgorithm },
  		{ KCDTIdMIP4MNHASPIIndicator,	    EBool,      
  		  ENotNull,                         KCDTypeNameMIP4MNHASPIIndicator },
  		{ KCDTIdMIP4MNHASPI,	            EUint32,		
	      ENoAttrs,                         KCDTypeNameMIP4MNHASPI },
  		{ KCDTIdMIP4MNHASSData,    	        EText,  	
		  ENotNull,                         KCDTypeNameMIP4MNHASSData },
  		{ KCDTIdMIP4MNAAAAuthAlgorithm,	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4MNAAAAuthAlgorithm },
  		{ KCDTIdMIP4MNAAASPIIndicator,	    EBool,      
  		  ENoAttrs,                         KCDTypeNameMIP4MNAAASPIIndicator },
  		{ KCDTIdMIP4MNAAASPI,	            EUint32,		
	      ENoAttrs,                         KCDTypeNameMIP4MNAAASPI },
  		{ KCDTIdMIP4MNAAASSData,    	    EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4MNAAASSData },
  		{ KCDTIdMIP4UseForeignAgent,	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4UseForeignAgent },
  		{ KCDTIdMIP4MNFAAuthAlgorithm,	    EInt,		
	      ENoAttrs,                         KCDTypeNameMIP4MNFAAuthAlgorithm },
  		{ KCDTIdMIP4MNFASPIIndicator,	    EBool,      
  		  ENoAttrs,                         KCDTypeNameMIP4MNFASPIIndicator }, 
  		{ KCDTIdMIP4MNFASPI,	            EUint32,		
	      ENoAttrs,                         KCDTypeNameMIP4MNFASPI },
  		{ KCDTIdMIP4MNFASSData,    	        EText,  	
		  ENoAttrs,                         KCDTypeNameMIP4MNFASSData },
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};
 
const SRecordTypeInfo CCDMIP6Record::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	                EInt,		
	      ENoAttrs,                         KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	        EText,		
  		  ENotNull,                         KCDTypeNameRecordName },
        { KCDTIdMIP6HomeAddress,    	    EText,  	
		  ENotNull,                         KCDTypeNameMIP6HomeAddress },
  		{ KCDTIdMIP6HomeAgentAddress,    	EText,  	
		  ENoAttrs,                         KCDTypeNameMIP6HomeAgentAddress },
  		{ KCDTIdMIP6PrefixLength,	        EInt,		
	      ENotNull,                         KCDTypeNameMIP6PrefixLength },
  		{ KCDTIdMIP6EnableRouteOptimization, EBool,      
  		  ENotNull,                         KCDTypeNameMIP6EnableRouteOptimization },
  		{ KCDTIdMIP6EnableIPSec,            EBool,      
  		  ENotNull,                         KCDTypeNameMIP6EnableIPSec },
  		{ KCDTIdMIP6MNHAAuthAlgorithm,	    EInt,		
	      ENotNull,                         KCDTypeNameMIP6MNHAAuthAlgorithm },
  		{ KCDTIdMIP6MNHAEncryptAlgorithm,	EInt,		
	      ENotNull,                         KCDTypeNameMIP6MNHAEncryptAlgorithm },
  		{ KCDTIdMIP6MNHASPIIndicator,	    EBool,      
  		  ENotNull,                         KCDTypeNameMIP6MNHASPIIndicator },
  		{ KCDTIdMIP6MNHASPI,	            EUint32,		
	      ENoAttrs,                         KCDTypeNameMIP6MNHASPI },
  		{ KCDTIdMIP6MNHASSData,    	        EText,  	
		  ENotNull,                         KCDTypeNameMIP6MNHASSData },
  		{ KCDTIdMIP6HookState,	            EUint32,		
	      ENoAttrs,                         KCDTypeNameMIP6HookState },
   		{ KCDTIdMIP6IFName,    	            EText,  	
		  ENotNull,                         KCDTypeNameMIP6IFName },
  		{ KCDTIdMIP6TunnelIFName,    	    EText,  	
		  ENotNull,                         KCDTypeNameMIP6TunnelIFName },		  
  		{ 0,                                0,          
  		  ENoAttrs,                         KCDNull }
  	};

// ======== LOCAL FUNCTIONS ========
TMDBElementId CreateTableL( CMDBSession& aSession,
                            const TDesC& aTableName, 
                            const SGenericRecordTypeInfo* aNewTableInfo )
    {
    CMDBGenericRecord* table = static_cast<CMDBGenericRecord*>
            ( CCDRecordBase::RecordFactoryL( KCDNewTableRequest ) );
    CleanupStack::PushL( table );    
    table->InitializeL( aTableName, aNewTableInfo );
   
    table->StoreL( aSession );
    
    TMDBElementId tableId = table->TableId();
    
    CleanupStack::PopAndDestroy( table ); 
    
    return tableId;
    }

TMDBElementId TableIdL( CMDBSession& aSession, 
                        const TDesC& aTableName )
    {
    CMDBGenericRecord* ptrTable = 
            static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL( 0 ));
    CleanupStack::PushL( ptrTable );
    
    TPtrC ptr( aTableName );
    ptrTable->InitializeL( ptr, NULL );
    
    ptrTable->LoadL( aSession );
    
    TMDBElementId tableId = ptrTable->TableId();
    
    CleanupStack::PopAndDestroy( ptrTable );
    
    return tableId;
    }
 
// ======== MEMBER FUNCTIONS ========
//Constructors for the container classes

EXPORT_C CCDVirtualIAPNextLayerRecord::
    CCDVirtualIAPNextLayerRecord( TMDBElementId aElementId )
    : CCDRecordBase( aElementId ),
      iIAP( (aElementId & KCDMaskShowRecordType ) | KCDTIdVirtualIAP),
      iNextLayerSNAP( ( aElementId & KCDMaskShowRecordType ) | KCDTIdVirtualNextLayerSNAP),
      iNextLayerIAP( (aElementId & KCDMaskShowRecordType ) | KCDTIdVirtualNextLayerIAP)
      {}

EXPORT_C TMDBElementId
    CCDVirtualIAPNextLayerRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KVirtualIAPNextLayerTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,            EUint32,
                                ENoAttrs,                   KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,           EText,
                                ENotNull,                   KCDTypeNameRecordName ),    
        SGenericRecordTypeInfo( KCDTIdVirtualIAP,           EInt,
                                ENotNull,                   KCDTypeNameVirtualIAP ),
        SGenericRecordTypeInfo( KCDTIdVirtualNextLayerSNAP, EInt,
                                ENoAttrs,                   KCDTypeNameVirtualIAPNextLayerSNAP ),
        SGenericRecordTypeInfo( KCDTIdVirtualNextLayerIAP,  EInt,
                                ENoAttrs,                   KCDTypeNameVirtualIAPNextLayerIAP ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
        };

    return ::CreateTableL( aSession,
                           TPtrC( KCDTypeNameVirtualIAPNextLayer ),
                           KVirtualIAPNextLayerTableFormat );
    }

EXPORT_C TMDBElementId 
    CCDVirtualIAPNextLayerRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameVirtualIAPNextLayer ) );
    }

EXPORT_C CCDGlobalBearerTypePriorizationRecord::
    CCDGlobalBearerTypePriorizationRecord( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ),  
          iServiceType( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdGlobalServiceType ),
          iPriority( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdGlobalServicePriority ),
          iUIPriority( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdGlobalServiceUIPriority )
        {}

EXPORT_C TMDBElementId
    CCDGlobalBearerTypePriorizationRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KGlobalBearerTypeTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,             EUint32,
                                ENoAttrs,                    KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,            EText,
                                ENotNull,                    KCDTypeNameRecordName ),
        SGenericRecordTypeInfo( KCDTIdGlobalServiceType,     EText,
                                ENotNull,                    KCDTypeNameGlobalServiceType ),
        SGenericRecordTypeInfo( KCDTIdGlobalServicePriority, EInt,
                                ENoAttrs,                    KCDTypeNameGlobalServicePriority ),
        SGenericRecordTypeInfo( KCDTIdGlobalServiceUIPriority, EInt,
                                ENoAttrs,                    KCDTypeNameGlobalServiceUIPriority ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
    };
    return ::CreateTableL( aSession,
                           TPtrC( KCDTypeNameGlobalService ),
                           KGlobalBearerTypeTableFormat );
    }

EXPORT_C TMDBElementId 
    CCDGlobalBearerTypePriorizationRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameGlobalService ) );
    }

EXPORT_C CCDDataMobilitySelectionPolicyRecord::
CCDDataMobilitySelectionPolicyRecord( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ), 
          iSNAP( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdDataMobilitySelectionPolicyNetwork ),
          iIAP( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdDataMobilitySelectionPolicyIAP ),
          iEmbeddedSNAP( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdDataMobilitySelectionPolicyEmbeddedSNAP ),
          iPriority( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdDataMobilitySelectionPolicyPriority )
        {}

EXPORT_C TMDBElementId
    CCDDataMobilitySelectionPolicyRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KDataMobilitySelectionPolicyTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag, EUint32,
                                ENoAttrs, KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName, EText,
                                ENotNull, KCDTypeNameRecordName ),
        SGenericRecordTypeInfo( KCDTIdDataMobilitySelectionPolicyNetwork, EInt,
                                ENotNull, KCDTypeNameDataMobilitySelectionPolicySNAP ),
        SGenericRecordTypeInfo( KCDTIdDataMobilitySelectionPolicyIAP, ELink,
                                ENoAttrs, KCDTypeNameDataMobilitySelectionPolicyIAP ),
        SGenericRecordTypeInfo( KCDTIdDataMobilitySelectionPolicyEmbeddedSNAP, EInt,
                                ENoAttrs, KCDTypeNameDataMobilitySelectionPolicyEmbeddedSNAP ),
        SGenericRecordTypeInfo( KCDTIdDataMobilitySelectionPolicyPriority, EInt,
                                ENotNull, KCDTypeNameDataMobilitySelectionPolicyPriority ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
    };

    return ::CreateTableL( aSession,
                           TPtrC( KCDTypeNameDataMobilitySelectionPolicy ),
                           KDataMobilitySelectionPolicyTableFormat );
    }

EXPORT_C TMDBElementId
    CCDDataMobilitySelectionPolicyRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameDataMobilitySelectionPolicy ) );
    }

EXPORT_C CCDMIPServiceExtRecord::CCDMIPServiceExtRecord( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ),
          iMIP4( ( aElementId & KCDMaskShowRecordType ) | KCDTIdMIPMIP4 ),
          iMIP6( ( aElementId & KCDMaskShowRecordType ) | KCDTIdMIPMIP6 )
        {}

EXPORT_C TMDBElementId
    CCDMIPServiceExtRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KMIPServiceExtTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32,    
                                ENoAttrs,               KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,      
                                ENotNull,               KCDTypeNameRecordName ),    
        SGenericRecordTypeInfo( KCDTIdMIPMIP4,            ELink,    
                                ENoAttrs,               KCDTypeNameMIPMIP4 ),
        SGenericRecordTypeInfo( KCDTIdMIPMIP6,            ELink,    
                                ENoAttrs,               KCDTypeNameMIPMIP6 ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )       
    };

    return ::CreateTableL( aSession, 
                           TPtrC( KCDTypeNameMIPServiceExt ), 
                           KMIPServiceExtTableFormat );
    }
    
EXPORT_C TMDBElementId 
    CCDMIPServiceExtRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameMIPServiceExt ) );
    }

EXPORT_C CCDSNAPMetadataRecord::CCDSNAPMetadataRecord( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ), 
          iSNAP( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdSNAPMetadataSNAP ),
          iMetadata( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdSNAPMetadataMetadata ),
          iIcon( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdSNAPMetadataIcon ),
          iIconFileName( ( aElementId & KCDMaskShowRecordType ) |
          KCDTIdSNAPMetadataIconFileName )
        {}

EXPORT_C TMDBElementId 
    CCDSNAPMetadataRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KSNAPMetadataTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,        EUint32,    
                                ENoAttrs,               KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,        EText,      
                                ENotNull,               KCDTypeNameRecordName ),    
        SGenericRecordTypeInfo( KCDTIdSNAPMetadataSNAP,            EInt,    
                                ENoAttrs,               KCDTypeNameSNAPMetadataSNAP ),       
        SGenericRecordTypeInfo( KCDTIdSNAPMetadataMetadata,        EUint32,    
                                ENoAttrs,               KCDTypeNameSNAPMetadataMetadata ),
        SGenericRecordTypeInfo( KCDTIdSNAPMetadataIcon,        EUint32,    
                                ENoAttrs,               KCDTypeNameSNAPMetadataIcon ),
        SGenericRecordTypeInfo( KCDTIdSNAPMetadataIconFileName,    ELongText,
                                ENoAttrs, KCDTypeNameSNAPMetadataIconFileName ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )       
    };

    return ::CreateTableL( aSession, 
                           TPtrC( KCDTypeNameSNAPMetadata ), 
                           KSNAPMetadataTableFormat );
    }
    
EXPORT_C TMDBElementId 
    CCDSNAPMetadataRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameSNAPMetadata ) );
    }

EXPORT_C CCDIAPMetadataRecord::CCDIAPMetadataRecord( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ), 
          iIAP( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdIAPMetadataIAP ),
          iMetadata( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdIAPMetadataMetadata ),
          iSeamlessness( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdIAPMetadataSeamlessness ),
          iIconFileName( ( aElementId & KCDMaskShowRecordType ) |
          KCDTIdIAPMetadataIconFileName )
        {}

EXPORT_C TMDBElementId
    CCDIAPMetadataRecord::CreateTableL( CMDBSession& aSession )
    {
    const SGenericRecordTypeInfo KIAPMetadataTableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag, EUint32,
                                ENoAttrs, KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName, EText,
                                ENotNull, KCDTypeNameRecordName ),
        SGenericRecordTypeInfo( KCDTIdIAPMetadataIAP, ELink,
                                ENotNull, KCDTypeNameIAPMetadataIAP ),
        SGenericRecordTypeInfo( KCDTIdIAPMetadataMetadata, EUint32,
                                ENoAttrs, KCDTypeNameIAPMetadataMetadata ),
        SGenericRecordTypeInfo( KCDTIdIAPMetadataSeamlessness, EInt,
                                ENoAttrs, KCDTypeNameIAPMetadataSeamlessness ),
        SGenericRecordTypeInfo( KCDTIdIAPMetadataIconFileName, ELongText,
                                ENoAttrs, KCDTypeNameIAPMetadataIconFileName ),
        SGenericRecordTypeInfo( 0, 0, ENoAttrs, KCDNull )
    };

    return ::CreateTableL( aSession,
                           TPtrC( KCDTypeNameIAPMetadata ),
                           KIAPMetadataTableFormat );
    }

EXPORT_C TMDBElementId 
    CCDIAPMetadataRecord::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameIAPMetadata ) );
    }

EXPORT_C CCDMIP4Record::CCDMIP4Record( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ),
          iHomeAddress( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4HomeAddress ),
        iMaxRegRetries( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MaxRegRetries ),
        iFirstRetryTimeout( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4FirstRetryTimeout ),
        iInitialLifetime( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4InitialLifetime ),
        iNAI( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4NAI ),
        iHomeAgentAddress( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4HomeAgentAddress ),
        iSecondaryHomeAgentAddress( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4SecondaryHomeAgentAddress ),
        iEnableReverseTunnel( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4EnableReverseTunnel ),
        iEnableNATTraversal( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4EnableNATTraversal),
        iEnableMinimalEncapsulation( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4EnableMinimalEncapsulation ),
        iMNHAAuthAlgorithm( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNHAAuthAlgorithm ),
        iMNHASPIIndicator( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNHASPIIndicator ),
        iMNHASPI( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNHASPI ),
        iMNHASSData( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNHASSData ),
        iMNAAAAuthAlgorithm( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNAAAAuthAlgorithm ),
        iMNAAASPIIndicator( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNAAASPIIndicator ),
        iMNAAASPI( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNAAASPI ),
        iMNAAASSData( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNAAASSData ),
        iUseForeignAgent( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4UseForeignAgent ),
        iMNFAAuthAlgorithm( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNFAAuthAlgorithm ),
        iMNFASPIIndicator( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNFASPIIndicator ),
        iMNFASPI( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNFASPI ),
        iMNFASSData( ( aElementId & KCDMaskShowRecordType ) | 
          KCDTIdMIP4MNFASSData )
        {}

EXPORT_C TMDBElementId 
    CCDMIP4Record::CreateTableL( CMDBSession& aSession )
    {
   const SGenericRecordTypeInfo KMIP4TableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,                    EInt,        
          ENoAttrs,                         KCDTypeNameRecordTag ),
          SGenericRecordTypeInfo( KCDTIdRecordName,                 EText,        
            ENotNull,                         KCDTypeNameRecordName ),
        SGenericRecordTypeInfo( KCDTIdMIP4HomeAddress,            EText,      
          ENoAttrs,                         KCDTypeNameMIP4HomeAddress ),
          SGenericRecordTypeInfo( KCDTIdMIP4MaxRegRetries,            EInt,        
          ENoAttrs,                         KCDTypeNameMIP4MaxRegRetries ),
          SGenericRecordTypeInfo( KCDTIdMIP4FirstRetryTimeout,        EInt,        
          ENoAttrs,                         KCDTypeNameMIP4FirstRetryTimeout ),
          SGenericRecordTypeInfo( KCDTIdMIP4InitialLifetime,         EInt,        
          ENoAttrs,                         KCDTypeNameMIP4InitialLifetime ),
          SGenericRecordTypeInfo( KCDTIdMIP4NAI,                    EText,      
          ENoAttrs,                         KCDTypeNameMIP4NAI ),
          SGenericRecordTypeInfo( KCDTIdMIP4HomeAgentAddress,        EText,      
          ENoAttrs,                         KCDTypeNameMIP4HomeAgentAddress ),
          SGenericRecordTypeInfo( KCDTIdMIP4SecondaryHomeAgentAddress, EText,      
          ENoAttrs,                         KCDTypeNameMIP4SecondaryHomeAgentAddress ),
          SGenericRecordTypeInfo( KCDTIdMIP4EnableReverseTunnel,    EInt,        
          ENoAttrs,                         KCDTypeNameMIP4EnableReverseTunnel ),
          SGenericRecordTypeInfo( KCDTIdMIP4EnableNATTraversal,        EInt,        
          ENoAttrs,                         KCDTypeNameMIP4EnableNATTraversal ),
          SGenericRecordTypeInfo( KCDTIdMIP4EnableMinimalEncapsulation,    EInt,        
          ENoAttrs,                         KCDTypeNameMIP4EnableMinimalEncapsulation ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNHAAuthAlgorithm,        EInt,        
          ENotNull,                         KCDTypeNameMIP4MNHAAuthAlgorithm ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNHASPIIndicator,        EBool,      
            ENotNull,                         KCDTypeNameMIP4MNHASPIIndicator ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNHASPI,                EUint32,        
          ENoAttrs,                         KCDTypeNameMIP4MNHASPI ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNHASSData,                EText,      
          ENotNull,                         KCDTypeNameMIP4MNHASSData ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNAAAAuthAlgorithm,        EInt,        
          ENoAttrs,                         KCDTypeNameMIP4MNAAAAuthAlgorithm ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNAAASPIIndicator,        EBool,      
            ENoAttrs,                         KCDTypeNameMIP4MNAAASPIIndicator ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNAAASPI,                EUint32,        
          ENoAttrs,                         KCDTypeNameMIP4MNAAASPI ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNAAASSData,            EText,      
          ENoAttrs,                         KCDTypeNameMIP4MNAAASSData ),
          SGenericRecordTypeInfo( KCDTIdMIP4UseForeignAgent,        EInt,        
          ENoAttrs,                         KCDTypeNameMIP4UseForeignAgent ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNFAAuthAlgorithm,        EInt,        
          ENoAttrs,                         KCDTypeNameMIP4MNFAAuthAlgorithm ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNFASPIIndicator,        EBool,      
            ENoAttrs,                         KCDTypeNameMIP4MNFASPIIndicator ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNFASPI,                EUint32,        
          ENoAttrs,                         KCDTypeNameMIP4MNFASPI ),
          SGenericRecordTypeInfo( KCDTIdMIP4MNFASSData,                EText,      
          ENoAttrs,                         KCDTypeNameMIP4MNFASSData ),
          SGenericRecordTypeInfo( 0,                                0,          
            ENoAttrs,                         KCDNull )
      };
 
    return ::CreateTableL( aSession, 
                           TPtrC( KCDTypeNameMIP4 ), 
                           KMIP4TableFormat );
    }
    
EXPORT_C TMDBElementId 
    CCDMIP4Record::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameMIP4 ) );
    }

EXPORT_C CCDMIP6Record::CCDMIP6Record( TMDBElementId aElementId )
        : CCDRecordBase( aElementId ),
        iHomeAddress( ( aElementId & KCDMaskShowRecordType ) |
           KCDTIdMIP6HomeAddress ),
        iHomeAgentAddress( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6HomeAgentAddress ),
        iPrefixLength( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6PrefixLength ),
        iEnableRouteOptimization( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6EnableRouteOptimization ),
        iEnableIPSec( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6EnableIPSec ),
        iMNHAAuthAlgorithm( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6MNHAAuthAlgorithm ),
        iMNHAEncryptAlgorithm( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6MNHAEncryptAlgorithm ),
        iMNHASPIIndicator( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6MNHASPIIndicator ),
        iMNHASPI( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6MNHASPI ),
        iMNHASSData( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6MNHASSData ),
        iHookState( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6HookState ),
        iIFName( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6IFName ),
        iTunnelIFName( ( aElementId & KCDMaskShowRecordType ) |
        KCDTIdMIP6TunnelIFName )
        {}

EXPORT_C TMDBElementId 
    CCDMIP6Record::CreateTableL( CMDBSession& aSession )
    {
   const SGenericRecordTypeInfo KMIP6TableFormat[] = {
        SGenericRecordTypeInfo( KCDTIdRecordTag,                    EInt,        
        ENoAttrs,                         KCDTypeNameRecordTag ),
        SGenericRecordTypeInfo( KCDTIdRecordName,                 EText,        
        ENotNull,                         KCDTypeNameRecordName ),
        SGenericRecordTypeInfo( KCDTIdMIP6HomeAddress,            EText,      
        ENotNull,                         KCDTypeNameMIP6HomeAddress ),
        SGenericRecordTypeInfo( KCDTIdMIP6HomeAgentAddress,        EText,      
        ENoAttrs,                         KCDTypeNameMIP6HomeAgentAddress ),
        SGenericRecordTypeInfo( KCDTIdMIP6PrefixLength,            EInt,        
        ENotNull,                         KCDTypeNameMIP6PrefixLength ),
        SGenericRecordTypeInfo( KCDTIdMIP6EnableRouteOptimization, EBool,      
        ENotNull,                         KCDTypeNameMIP6EnableRouteOptimization ),
        SGenericRecordTypeInfo( KCDTIdMIP6EnableIPSec,            EBool,      
        ENotNull,                         KCDTypeNameMIP6EnableIPSec ),
        SGenericRecordTypeInfo( KCDTIdMIP6MNHAAuthAlgorithm,        EInt,        
        ENotNull,                         KCDTypeNameMIP6MNHAAuthAlgorithm ),
        SGenericRecordTypeInfo( KCDTIdMIP6MNHAEncryptAlgorithm,    EInt,        
        ENotNull,                         KCDTypeNameMIP6MNHAEncryptAlgorithm ),
        SGenericRecordTypeInfo( KCDTIdMIP6MNHASPIIndicator,        EBool,      
        ENotNull,                         KCDTypeNameMIP6MNHASPIIndicator ),
        SGenericRecordTypeInfo( KCDTIdMIP6MNHASPI,                EUint32,        
        ENoAttrs,                         KCDTypeNameMIP6MNHASPI ),
        SGenericRecordTypeInfo( KCDTIdMIP6MNHASSData,                EText,      
        ENotNull,                         KCDTypeNameMIP6MNHASSData ),
        SGenericRecordTypeInfo( KCDTIdMIP6HookState,                EUint32,        
        ENoAttrs,                         KCDTypeNameMIP6HookState ),
        SGenericRecordTypeInfo( KCDTIdMIP6IFName,                    EText,      
        ENotNull,                         KCDTypeNameMIP6IFName ),
        SGenericRecordTypeInfo( KCDTIdMIP6TunnelIFName,            EText,      
        ENotNull,                         KCDTypeNameMIP6TunnelIFName ),          
        SGenericRecordTypeInfo( 0,                                0,          
        ENoAttrs,                         KCDNull )
        };

    return ::CreateTableL( aSession, 
                           TPtrC( KCDTypeNameMIP6 ), 
                           KMIP6TableFormat );
    }
    
EXPORT_C TMDBElementId 
    CCDMIP6Record::TableIdL( CMDBSession& aSession )
    {
    return ::TableIdL( aSession, TPtrC( KCDTypeNameMIP6 ) );
    }

EXPORT_C const SRecordTypeInfo* CCDVirtualIAPNextLayerRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDGlobalBearerTypePriorizationRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDDataMobilitySelectionPolicyRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
       
EXPORT_C const SRecordTypeInfo* CCDMIPServiceExtRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    

EXPORT_C const SRecordTypeInfo* CCDSNAPMetadataRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
    
EXPORT_C const SRecordTypeInfo* CCDIAPMetadataRecord::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    

EXPORT_C const SRecordTypeInfo* CCDMIP4Record::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    
EXPORT_C const SRecordTypeInfo* CCDMIP6Record::GetRecordInfo()
    { 
    return iRecordInfo; 
    }    

