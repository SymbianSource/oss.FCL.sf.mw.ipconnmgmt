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
* Description:  Contains classes for accessing data mobility related tables 
*                in the CommsDat as well as data structures for creating the 
*                tables.
*
*/

#ifndef DATAMOBILITYCOMMSDATTYPES_H
#define DATAMOBILITYCOMMSDATTYPES_H

#include <commsdattypesv1_1.h>

//Definitions of table and field names. These follow
//the format of the existing CommsDat tables in CommsDatTypeInfoV1_1.h

//Virtual IAP Next Layer Table
#define KCDTypeNameVirtualIAPNextLayer  _S( "VirtualIAPNextLayer" )
#define KCDTypeNameVirtualIAP _S( "IAP" )
#define KCDTypeNameVirtualIAPNextLayerSNAP _S( "NextLayerSNAP" )
#define KCDTypeNameVirtualIAPNextLayerIAP _S( "NextLayerIAP" )

//Data Mobility Selection Policy table and fields
#define KCDTypeNameDataMobilitySelectionPolicy _S( "DataMobilitySelectionPolicy" )
#define KCDTypeNameDataMobilitySelectionPolicySNAP _S( "SNAP" )
#define KCDTypeNameDataMobilitySelectionPolicyIAP _S( "IAP" )
#define KCDTypeNameDataMobilitySelectionPolicyEmbeddedSNAP _S( "EmbeddedSNAP" )
#define KCDTypeNameDataMobilitySelectionPolicyPriority _S( "Priority" )

//SNAP Metadata table and fields
#define KCDTypeNameSNAPMetadata _S( "SNAPMetadata" )
#define KCDTypeNameSNAPMetadataSNAP _S( "SNAP" )
#define KCDTypeNameSNAPMetadataMetadata _S( "Metadata" )
#define KCDTypeNameSNAPMetadataIcon _S( "Icon" )
#define KCDTypeNameSNAPMetadataIconFileName _S( "IconFileName" )

//Global bearer priorization table and fields
#define KCDTypeNameGlobalService _S( "GlobalBearerTypePriorization" )
#define KCDTypeNameGlobalServiceType _S( "ServiceType" )
#define KCDTypeNameGlobalServicePriority _S( "Priority" )
#define KCDTypeNameGlobalServiceUIPriority _S( "UIPriority" )

//MIP Service table and fields
#define KCDTypeNameMIPServiceExt _S( "MIPServiceExtension" )
#define KCDTypeNameMIPMIP4 _S( "MIP4" )
#define KCDTypeNameMIPMIP6 _S( "MIP6" )

//IAP Metadata table and fields
#define KCDTypeNameIAPMetadata _S( "IAPMetadata" )
#define KCDTypeNameIAPMetadataIAP _S( "IAP" )
#define KCDTypeNameIAPMetadataMetadata _S( "Metadata" )
#define KCDTypeNameIAPMetadataSeamlessness _S( "Seamlessness" )
#define KCDTypeNameIAPMetadataIconFileName _S( "IconFileName" )

//MIP4 table and fields
#define KCDTypeNameMIP4 _S( "MIP4" )
#define KCDTypeNameMIP4HomeAddress _S( "HomeAddress" )
#define KCDTypeNameMIP4MaxRegRetries _S( "MaxRegRetries" ) 
#define KCDTypeNameMIP4FirstRetryTimeout _S( "FirstRetryTimeout" )
#define KCDTypeNameMIP4InitialLifetime _S( "InitialLifetime" )
#define KCDTypeNameMIP4NAI _S( "NAI" )
#define KCDTypeNameMIP4HomeAgentAddress _S( "HomeAgentAddress" )
#define KCDTypeNameMIP4SecondaryHomeAgentAddress _S( "SecondaryHomeAgentAddress" )
#define KCDTypeNameMIP4EnableReverseTunnel _S( "EnableReverseTunnel" )
#define KCDTypeNameMIP4EnableNATTraversal _S( "EnableNATTraversal" )
#define KCDTypeNameMIP4EnableMinimalEncapsulation _S( "EnableMinimalEncapsulation" )
#define KCDTypeNameMIP4MNHAAuthAlgorithm _S( "MNHAAuthAlgorithm" )
#define KCDTypeNameMIP4MNHASPIIndicator _S( "MNHASPIIndicator" )
#define KCDTypeNameMIP4MNHASPI _S( "MNHASPI" )
#define KCDTypeNameMIP4MNHASSData _S( "MNHASSData" )
#define KCDTypeNameMIP4MNAAAAuthAlgorithm _S( "MNAAAAuthAlgorithm" )
#define KCDTypeNameMIP4MNAAASPIIndicator _S( "MNAAASPIIndicator" )
#define KCDTypeNameMIP4MNAAASPI _S( "MNAAASPI" )
#define KCDTypeNameMIP4MNAAASSData _S( "MNAAASSData" )
#define KCDTypeNameMIP4UseForeignAgent _S( "USEForeignAgent" )
#define KCDTypeNameMIP4MNFAAuthAlgorithm _S( "MNFAAuthAlgorithm" )
#define KCDTypeNameMIP4MNFASPIIndicator _S( "MNFASPIIndicator" )
#define KCDTypeNameMIP4MNFASPI _S( "MNFASPI" )
#define KCDTypeNameMIP4MNFASSData _S( "MNFASSData" )

//MIP6 table and fields
#define KCDTypeNameMIP6 _S( "MIP6" )
#define KCDTypeNameMIP6HomeAddress _S( "HomeAddress" )
#define KCDTypeNameMIP6HomeAgentAddress  _S( "HomeAgentAddress" )
#define KCDTypeNameMIP6PrefixLength  _S( "PrefixLength" )
#define KCDTypeNameMIP6EnableRouteOptimization  _S( "EnableRouteOptimization" )
#define KCDTypeNameMIP6EnableIPSec  _S( "EnableIPSec" )
#define KCDTypeNameMIP6MNHAAuthAlgorithm   _S( "MNHAAuthAlgorithm" )
#define KCDTypeNameMIP6MNHAEncryptAlgorithm _S( "MNHAEncryptAlgorithm" )
#define KCDTypeNameMIP6MNHASPIIndicator  _S( "MNHASPIIndicator" )
#define KCDTypeNameMIP6MNHASPI _S( "MNHASPI" )
#define KCDTypeNameMIP6MNHASSData _S( "MNHASSData" )
#define KCDTypeNameMIP6HookState _S( "HookState" )
#define KCDTypeNameMIP6IFName _S( "IFName" )
#define KCDTypeNameMIP6TunnelIFName  _S( "TunnelIFName" )


// IDs for columns  These follow the format of 
// the existing CommsDat tables in CommsDatTypeInfoV1_1.h
    
  
    // virtual IAP next layer table
const CommsDat::TMDBElementId KCDTIdVirtualIAP = 0x00120000;
const CommsDat::TMDBElementId KCDTIdVirtualNextLayerSNAP = 0x00130000;
const CommsDat::TMDBElementId KCDTIdVirtualNextLayerIAP = 0x00140000;

   //DataMobilitySelectionPolicy table
const CommsDat::TMDBElementId KCDTIdDataMobilitySelectionPolicyNetwork  = 0x00120000;
const CommsDat::TMDBElementId KCDTIdDataMobilitySelectionPolicyIAP = 0x00130000;
const CommsDat::TMDBElementId KCDTIdDataMobilitySelectionPolicyEmbeddedSNAP = 0x00140000;
//valid priorities are 1, 2, ... 255
const CommsDat::TMDBElementId KCDTIdDataMobilitySelectionPolicyPriority  = 0x00150000;

    //SNAP Metadata 
const CommsDat::TMDBElementId KCDTIdSNAPMetadataSNAP  = 0x00120000;
const CommsDat::TMDBElementId KCDTIdSNAPMetadataMetadata  = 0x00130000;
const CommsDat::TMDBElementId KCDTIdSNAPMetadataIcon  = 0x00140000;
const CommsDat::TMDBElementId KCDTIdSNAPMetadataIconFileName  = 0x00150000;

    //mip service extension table
const CommsDat::TMDBElementId KCDTIdMIPMIP4  = 0x00120000;
const CommsDat::TMDBElementId KCDTIdMIPMIP6  = 0x00130000;

    //Global bearer type priorization
const CommsDat::TMDBElementId KCDTIdGlobalServiceType  = 0x00120000;
const CommsDat::TMDBElementId KCDTIdGlobalServicePriority  = 0x00130000;
const CommsDat::TMDBElementId KCDTIdGlobalServiceUIPriority  = 0x00140000;

    //IAP Metadata
const CommsDat::TMDBElementId KCDTIdIAPMetadataIAP  = 0x00120000;
const CommsDat::TMDBElementId KCDTIdIAPMetadataMetadata  = 0x00130000;
const CommsDat::TMDBElementId KCDTIdIAPMetadataSeamlessness  = 0x00140000;
const CommsDat::TMDBElementId KCDTIdIAPMetadataIconFileName  = 0x00150000;

//MIP4 Metadata
const CommsDat::TMDBElementId KCDTIdMIP4HomeAddress = 0x00120000;
const CommsDat::TMDBElementId KCDTIdMIP4MaxRegRetries = 0x00130000;
const CommsDat::TMDBElementId KCDTIdMIP4FirstRetryTimeout = 0x00140000;
const CommsDat::TMDBElementId KCDTIdMIP4InitialLifetime = 0x00150000;
const CommsDat::TMDBElementId KCDTIdMIP4NAI = 0x00160000;
const CommsDat::TMDBElementId KCDTIdMIP4HomeAgentAddress = 0x00170000;
const CommsDat::TMDBElementId KCDTIdMIP4SecondaryHomeAgentAddress = 0x00180000;
const CommsDat::TMDBElementId KCDTIdMIP4EnableReverseTunnel = 0x00190000;
const CommsDat::TMDBElementId KCDTIdMIP4EnableNATTraversal = 0x001A0000;
const CommsDat::TMDBElementId KCDTIdMIP4EnableMinimalEncapsulation = 0x001B0000;
const CommsDat::TMDBElementId KCDTIdMIP4MNHAAuthAlgorithm = 0x001C0000;
const CommsDat::TMDBElementId KCDTIdMIP4MNHASPIIndicator = 0x001D0000;
const CommsDat::TMDBElementId KCDTIdMIP4MNHASPI = 0x001E0000;
const CommsDat::TMDBElementId KCDTIdMIP4MNHASSData = 0x001F0000;
const CommsDat::TMDBElementId KCDTIdMIP4MNAAAAuthAlgorithm = 0x00200000;
const CommsDat::TMDBElementId KCDTIdMIP4MNAAASPIIndicator = 0x00210000;
const CommsDat::TMDBElementId KCDTIdMIP4MNAAASPI = 0x00220000;
const CommsDat::TMDBElementId KCDTIdMIP4MNAAASSData = 0x00230000;
const CommsDat::TMDBElementId KCDTIdMIP4UseForeignAgent = 0x00240000;
const CommsDat::TMDBElementId KCDTIdMIP4MNFAAuthAlgorithm = 0x00250000;
const CommsDat::TMDBElementId KCDTIdMIP4MNFASPIIndicator = 0x00260000;
const CommsDat::TMDBElementId KCDTIdMIP4MNFASPI = 0x00270000;
const CommsDat::TMDBElementId KCDTIdMIP4MNFASSData = 0x00280000;

//MIP6 Metadata
const CommsDat::TMDBElementId KCDTIdMIP6HomeAddress = 0x00120000;
const CommsDat::TMDBElementId KCDTIdMIP6HomeAgentAddress = 0x00130000;
const CommsDat::TMDBElementId KCDTIdMIP6PrefixLength = 0x00140000;
const CommsDat::TMDBElementId KCDTIdMIP6EnableRouteOptimization = 0x00150000;
const CommsDat::TMDBElementId KCDTIdMIP6EnableIPSec = 0x00160000;
const CommsDat::TMDBElementId KCDTIdMIP6MNHAAuthAlgorithm = 0x00170000;
const CommsDat::TMDBElementId KCDTIdMIP6MNHAEncryptAlgorithm = 0x00180000;
const CommsDat::TMDBElementId KCDTIdMIP6MNHASPIIndicator = 0x00190000;
const CommsDat::TMDBElementId KCDTIdMIP6MNHASPI = 0x001A0000;
const CommsDat::TMDBElementId KCDTIdMIP6MNHASSData = 0x001B0000;
const CommsDat::TMDBElementId KCDTIdMIP6HookState = 0x001C0000;
const CommsDat::TMDBElementId KCDTIdMIP6IFName = 0x001D0000;
const CommsDat::TMDBElementId KCDTIdMIP6TunnelIFName = 0x001E0000;

NONSHARABLE_CLASS( CCDVirtualIAPNextLayerRecord ) : public CommsDat::CCDRecordBase
    {
    public:
    
    IMPORT_C CCDVirtualIAPNextLayerRecord( CommsDat::TMDBElementId aElementId );
    
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
        
    public:
    CommsDat::CMDBField<TUint> iIAP;
    CommsDat::CMDBField<TUint> iNextLayerSNAP;
    CommsDat::CMDBField<TUint> iNextLayerIAP;    
    
    private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
    };
                                                
/**
 *  Class representing records in the Data Mobility Selection Policy table
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
// Should be inherited from CCDSelectionPolicyRecordBase 
// when SNAPs are taken to use. Constructor implementation 
// only available for CommsDat::CCDRecordBase
NONSHARABLE_CLASS( CCDDataMobilitySelectionPolicyRecord ) : public CommsDat::CCDRecordBase
    {
    public:
        
        IMPORT_C CCDDataMobilitySelectionPolicyRecord( CommsDat::TMDBElementId aElementId );
            
        IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

        IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
        IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
        
        DATA_VTABLE
    
    public:
    
        //if snap taken to use dn (and iap) could be changed to ap link
        CommsDat::CMDBField<TInt> iSNAP;
        CommsDat::CMDBRecordLink<CommsDat::CCDIAPRecord> iIAP;
        CommsDat::CMDBField<TInt> iEmbeddedSNAP;
        CommsDat::CMDBField<TUint> iPriority;
    
     private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
    };

/**
 *  Class representing records in the SNAP Metadata table
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CCDSNAPMetadataRecord ): public CommsDat::CCDRecordBase
{
    public:
        
    IMPORT_C CCDSNAPMetadataRecord( CommsDat::TMDBElementId aElementId );
        
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
    
    public:
    CommsDat::CMDBField<TInt> iSNAP;
    CommsDat::CMDBField<TUint32> iMetadata;
    CommsDat::CMDBField<TUint32> iIcon;         //< Not used.
    CommsDat::CMDBField<TDesC> iIconFileName;   //< May contain path information also.
    
     private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
};

/**
 *  Class representing records in the Global bearer type priorization table
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCDGlobalBearerTypePriorizationRecord ) : 
                                                            public CommsDat::CCDRecordBase
    {
    public:
    
    IMPORT_C CCDGlobalBearerTypePriorizationRecord( CommsDat::TMDBElementId aElementId );
        
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
        
    public:
    CommsDat::CMDBField<TDesC> iServiceType;
    CommsDat::CMDBField<TUint> iPriority;
    CommsDat::CMDBField<TUint> iUIPriority;    
    
    private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
    };

class CCDMIP4Record;
class CCDMIP6Record;

/**
 *  Class representing records in the MIP service extension table
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
//should be inherited from CCDServiceRecordBase
//constructor implementation only available for CommsDat::CCDRecordBase
NONSHARABLE_CLASS( CCDMIPServiceExtRecord ): public CommsDat::CCDRecordBase
{
    public:
        
    IMPORT_C CCDMIPServiceExtRecord( CommsDat::TMDBElementId aElementId );

    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
    
    public:
    //if snap taken to use DN link changed to AP link
    CommsDat::CMDBRecordLink<CommsDat::CCDNetworkRecord> iSNAP;
    CommsDat::CMDBRecordLink<CommsDat::CCDIAPRecord> iIAP;
    CommsDat::CMDBRecordLink<CCDMIP4Record> iMIP4;
    CommsDat::CMDBRecordLink<CCDMIP6Record> iMIP6;

     private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
};

/**
 *  Class representing records in the IAP Metadata table
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CCDIAPMetadataRecord ): public CommsDat::CCDRecordBase
{
    public:
        
    IMPORT_C CCDIAPMetadataRecord( CommsDat::TMDBElementId aElementId );
        
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
    
    public:
    CommsDat::CMDBRecordLink<CommsDat::CCDIAPRecord> iIAP;
    CommsDat::CMDBField<TUint32> iMetadata;
    CommsDat::CMDBField<TInt> iSeamlessness;
    CommsDat::CMDBField<TDesC> iIconFileName;   //< May contain path information also.

     private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
};

/**
 *  Class representing records in the MIP4 table
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CCDMIP4Record ): public CommsDat::CCDRecordBase
{
    public:
        
    IMPORT_C CCDMIP4Record( CommsDat::TMDBElementId aElementId );
        
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
    
    public:
    
        CommsDat::CMDBField<TDesC> iHomeAddress;
        CommsDat::CMDBField<TUint32> iMaxRegRetries;
        CommsDat::CMDBField<TUint32> iFirstRetryTimeout;
        CommsDat::CMDBField<TUint32> iInitialLifetime;
        CommsDat::CMDBField<TDesC> iNAI;
        CommsDat::CMDBField<TDesC> iHomeAgentAddress;
        CommsDat::CMDBField<TDesC> iSecondaryHomeAgentAddress;
        CommsDat::CMDBField<TUint32> iEnableReverseTunnel;
        CommsDat::CMDBField<TUint32> iEnableNATTraversal;
        CommsDat::CMDBField<TUint32> iEnableMinimalEncapsulation;
        CommsDat::CMDBField<TUint32> iMNHAAuthAlgorithm;
        CommsDat::CMDBField<TBool> iMNHASPIIndicator;
        CommsDat::CMDBField<TUint32> iMNHASPI;
        CommsDat::CMDBField<TDesC> iMNHASSData;
        CommsDat::CMDBField<TUint32> iMNAAAAuthAlgorithm;
        CommsDat::CMDBField<TBool> iMNAAASPIIndicator;
        CommsDat::CMDBField<TUint32> iMNAAASPI;
        CommsDat::CMDBField<TDesC> iMNAAASSData;
        CommsDat::CMDBField<TUint32> iUseForeignAgent;
        CommsDat::CMDBField<TUint32> iMNFAAuthAlgorithm;
        CommsDat::CMDBField<TBool> iMNFASPIIndicator;
        CommsDat::CMDBField<TUint32> iMNFASPI;
        CommsDat::CMDBField<TDesC> iMNFASSData;

    private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
};
/**
 *  Class representing records in the MIP4 table
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CCDMIP6Record ) : public CommsDat::CCDRecordBase
{
    public:
        
    IMPORT_C CCDMIP6Record( CommsDat::TMDBElementId aElementId );

    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
    
    public:
    
        CommsDat::CMDBField<TDesC> iHomeAddress;   
        CommsDat::CMDBField<TDesC> iHomeAgentAddress;
        CommsDat::CMDBField<TUint32> iPrefixLength;
        CommsDat::CMDBField<TBool> iEnableRouteOptimization;
        CommsDat::CMDBField<TBool> iEnableIPSec;
        CommsDat::CMDBField<TUint32> iMNHAAuthAlgorithm;
        CommsDat::CMDBField<TUint32> iMNHAEncryptAlgorithm;
        CommsDat::CMDBField<TBool> iMNHASPIIndicator;
        CommsDat::CMDBField<TUint32> iMNHASPI;
        CommsDat::CMDBField<TDesC> iMNHASSData;
        CommsDat::CMDBField<TUint32> iHookState;
        CommsDat::CMDBField<TDesC> iIFName;
        CommsDat::CMDBField<TDesC> iTunnelIFName;

    private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
};

//table name
#define DEFCONN_TABLE _S("DefConn")
//table fields
#define DEFCONN_TYPE _S("DefConnType")
#define DEFCONN_UID _S("DefConnUId")
#define USAGEOFWLAN _S("UsageOfWlan")
#define CELLULAR_DATA_USAGE_HOME _S("CellularDataUsageHome")
#define CELLULAR_DATA_USAGE_VISITOR _S("CellularDataUsageVisitor")

NONSHARABLE_CLASS( CCDDefConnRecord ) : public CommsDat::CCDRecordBase
    {
    public:
    
    IMPORT_C CCDDefConnRecord( CommsDat::TMDBElementId aElementId );
    
    IMPORT_C const CommsDat::SRecordTypeInfo* GetRecordInfo();

    IMPORT_C static CommsDat::TMDBElementId CreateTableL( CommsDat::CMDBSession& aSession );
    IMPORT_C static CommsDat::TMDBElementId TableIdL( CommsDat::CMDBSession& aSession );
    
    DATA_VTABLE
        
    public:
    CommsDat::CMDBField<TUint> iDefConnType;    //< Not used.
    CommsDat::CMDBField<TUint> iDefConnUid;     //< Not used.
    CommsDat::CMDBField<TUint> iUsageOfWlan;
    CommsDat::CMDBField<TUint> iCellularDataUsageHome;
    CommsDat::CMDBField<TUint> iCellularDataUsageVisitor;
    
    private:
    
        static const CommsDat::SRecordTypeInfo iRecordInfo[];
    };
    
#endif // DATAMOBILITYCOMMSDATTYPES_H
