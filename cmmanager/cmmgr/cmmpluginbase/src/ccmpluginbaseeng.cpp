/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*        Implementation of plugin base class
*
*/


#include <datamobilitycommsdattypes.h>
#include <cmpluginbaseeng.h>
#include <cmpsettingsconsts.h>
#include <featmgr.h>
#include <cmpluginembdestinationdef.h>

#include "cmmserverdefs.h"
#include "ccmpluginbaseengtextresolver.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ccmpluginbaseengTraces.h"
#endif


using namespace CMManager;
using namespace CommsDat;

const TInt KApMaxConnNameLength = 50;
_LIT( KFormatPostfix, "%02d" );
_LIT( KFormatLargePostfix, "%d" );
_LIT( KFormatNameWithPostfix, "%S(%S)" );
_LIT( KFormatNoPostfix, "%S" );
const TInt KMaxPostfixLength = 5;

_LIT( KDefWapGatewayIpAddress, "0.0.0.0" );

_LIT( KLocationName, "Mobile" );

_LIT( KDefaultIapName, "Connection Method" );


EXPORT_C CCmClientPluginInstance* CCmClientPluginInstance::NewLC()
    {
    OstTraceFunctionEntry0( CCMCLIENTPLUGININSTANCE_NEWLC_ENTRY );

    CCmClientPluginInstance* self = new( ELeave ) CCmClientPluginInstance();
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMCLIENTPLUGININSTANCE_NEWLC_EXIT );
    return self;
    }

EXPORT_C CCmClientPluginInstance* CCmClientPluginInstance::NewL()
    {
    OstTraceFunctionEntry0( CCMCLIENTPLUGININSTANCE_NEWL_ENTRY );

    CCmClientPluginInstance* self = CCmClientPluginInstance::NewLC();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMCLIENTPLUGININSTANCE_NEWL_EXIT );
    return self;
    }

CCmClientPluginInstance::CCmClientPluginInstance()
        :
        iNamingMethod( ENamingUnique ),
        iLocationEnabled( EFalse ),
        iIapId( 0 )
    {
    }

EXPORT_C CCmClientPluginInstance::~CCmClientPluginInstance()
    {
    OstTraceFunctionEntry0( CCMCLIENTPLUGININSTANCE_CCMCLIENTPLUGININSTANCE_ENTRY );

    iGenRecordArray.ResetAndDestroy();
    iBearerSpecRecordArray.ResetAndDestroy();

    OstTraceFunctionExit0( CCMCLIENTPLUGININSTANCE_CCMCLIENTPLUGININSTANCE_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmClientPluginInstance::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// TCmPluginInitParam::TCmPluginInitParam
// ---------------------------------------------------------------------------
//
EXPORT_C TCmPluginInitParam::TCmPluginInitParam( CommsDat::CMDBSession& aSession )
        :
        iSessionRef( aSession )
    {
    OstTraceFunctionEntry0( TCMPLUGININITPARAM_TCMPLUGININITPARAM_ENTRY );

    iNotused1 = NULL;
    iNotused2 = NULL;

    OstTraceFunctionExit0( TCMPLUGININITPARAM_TCMPLUGININITPARAM_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CCmPluginBaseEng
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng::CCmPluginBaseEng( TCmPluginInitParam *aInitParam )
        :
        iSession( aInitParam->iSessionRef ),
        iNamingMethod( ENamingUnique )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CCMPLUGINBASEENG_ENTRY );

    iDtor_ID_Key.iUid = 0;
    iBearerType = 0;

    iIapRecord = NULL;
    iProxyRecord = NULL;
    iServiceRecord = NULL;
    iNetworkRecord = NULL;
    iLocationRecord = NULL;
    iWapAPRecord = NULL;
    iWapIPBearerRecord = NULL;
    iMetaDataRecord = NULL;

    iCmId = 0;
    iMetadataTableId = 0;
    iLocationEnabled = EFalse;

    OstTraceFunctionExit0( CCMPLUGINBASEENG_CCMPLUGINBASEENG_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::~CCmPluginBaseEng
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng::~CCmPluginBaseEng()
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINBASEENG_CCMPLUGINBASEENG_ENTRY );

    iCmId = 0;
    delete iIapRecord; iIapRecord = NULL;
    delete iProxyRecord; iProxyRecord = NULL;
    delete iServiceRecord; iServiceRecord = NULL;
    delete iNetworkRecord; iNetworkRecord = NULL;
    delete iLocationRecord; iLocationRecord = NULL;
    delete iWapAPRecord; iWapAPRecord = NULL;
    delete iWapIPBearerRecord; iWapIPBearerRecord = NULL;
    delete iMetaDataRecord; iMetaDataRecord = NULL;

    iLocationEnabled = EFalse;

    REComSession::DestroyedImplementation( iDtor_ID_Key );

    OstTraceFunctionExit0( DUP1_CCMPLUGINBASEENG_CCMPLUGINBASEENG_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::ConstructL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CONSTRUCTL_ENTRY );

    TRAPD( err, iMetadataTableId = CCDIAPMetadataRecord::TableIdL( iSession ) );
    if ( err )
        {
        if ( err == KErrNotFound )
            {
            iMetadataTableId = CCDIAPMetadataRecord::CreateTableL( iSession );
            }
        else
            {
            User::LeaveIfError( err );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::UpdateL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATEL_ENTRY );

    PrepareToUpdateRecordsL( aClientPluginInstance );

    UpdateServiceRecordL( aClientPluginInstance );
    UpdateLocationRecordL( aClientPluginInstance );
    UpdateNetworkRecordL( aClientPluginInstance );
    UpdateIAPRecordL( aClientPluginInstance );
    UpdateWapRecordL( aClientPluginInstance );
    UpdateProxyRecordL( aClientPluginInstance );
    UpdateMetadataRecordL( aClientPluginInstance );
    UpdateConnPrefSettingL();

    UpdateBearerRecordsL(
            aClientPluginInstance->iGenRecordArray,
            aClientPluginInstance->iBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Delete this connection methond.
// Record ids are checked to be sure that we delete only records
// that were loaded before.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::DeleteL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_DELETEL_ENTRY );

    PrepareToDeleteRecordsL();

    if ( iIapRecord && iIapRecord->RecordId() )
        {
        iIapRecord->DeleteL( iSession );
        }

    if ( iProxyRecord && iProxyRecord->RecordId() )
        {
        iProxyRecord->DeleteL( iSession );
        }

    if ( iServiceRecord && iServiceRecord->RecordId() )
        {
        iServiceRecord->DeleteL( iSession );
        }

    if ( iNetworkRecord && iNetworkRecord->RecordId() )
        {
        iNetworkRecord->DeleteL( iSession );
        }

    if ( iLocationRecord && iLocationRecord->RecordId() )
        {
        iLocationRecord->DeleteL( iSession );
        }

    if ( iWapAPRecord && iWapAPRecord->RecordId() )
        {
        iWapAPRecord->DeleteL( iSession );
        }

    if ( iWapIPBearerRecord && iWapIPBearerRecord->RecordId() )
        {
        iWapIPBearerRecord->DeleteL( iSession );
        }

    if ( iMetaDataRecord && iMetaDataRecord->RecordId() )
        {
        iMetaDataRecord->DeleteL( iSession );
        }

    DeleteBearerRecordsL();

    OstTraceFunctionExit0( CCMPLUGINBASEENG_DELETEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ReLoad
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::ReLoadL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_RELOAD_ENTRY );

    delete iIapRecord; iIapRecord = NULL;
    delete iProxyRecord; iProxyRecord = NULL;
    delete iServiceRecord; iServiceRecord = NULL;
    delete iNetworkRecord; iNetworkRecord = NULL;
    delete iLocationRecord; iLocationRecord = NULL;
    delete iWapAPRecord; iWapAPRecord = NULL;
    delete iWapIPBearerRecord; iWapIPBearerRecord = NULL;
    delete iMetaDataRecord; iMetaDataRecord = NULL;

    iLocationEnabled = EFalse;

    ResetBearerRecords();

    LoadL( iCmId );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_RELOAD_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::LoadL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADL_ENTRY );

    // Embedded destination must be handled separately first.
    if ( aIapId > KCmDefaultDestinationAPTagId )
        {
        iCmId = aIapId;
        return;
        }

    DoLoadL( aIapId );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CreateNewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::CreateNewL( TUint32 aCmId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CREATENEWL_ENTRY );

    if ( iCmId )
        // we already have IAP id ->
        {
        User::Leave( KErrAlreadyExists );
        }

    // Optional record, disabled by default.
    iLocationEnabled = EFalse;

    // Create mandatory records.
    iIapRecord = static_cast<CCDIAPRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    iNetworkRecord = static_cast<CCDNetworkRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
    iProxyRecord = static_cast<CCDProxiesRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
    NewWapRecordL();
    iWapIPBearerRecord->iWAPGatewayAddress.SetL( KDefWapGatewayIpAddress );

    iMetaDataRecord = NewMetadataRecordL( ETrue );

    // Call plugin to create its own records.
    CreateServiceRecordL();

    // Update iaprecord servicetype name.
    HBufC* servicetypeName( NULL );
    ServiceRecordNameLC( servicetypeName );
    iIapRecord->iServiceType.SetL( *servicetypeName );
    CleanupStack::PopAndDestroy( servicetypeName );

    // Update iaprecord bearertype name.
    HBufC* bearerTypeName( NULL );
    BearerRecordNameLC( bearerTypeName );
    iIapRecord->iBearerType.SetL( *bearerTypeName );
    CleanupStack::PopAndDestroy( bearerTypeName );

    CreateBearerRecordsL();

    EnableProxyL( EFalse );

    _LIT( KDefaultConnNameTextId, "txt_occ_setlabel_connection_name_val_connection" );
    HBufC* resolvedText( NULL );
    resolvedText = CCmPluginBaseEngTextResolver::ResolveTextL( KDefaultConnNameTextId );
    if ( resolvedText )
        {
        SetDefaultNameL( *resolvedText );
        }
    else
        {
        SetDefaultNameL( KDefaultIapName );
        }

    delete resolvedText;
    resolvedText = NULL;

    iIapRecord->iNetworkWeighting = 0; // it's always 0.

    if ( aCmId != 0 )
        {
        iIapRecord->SetRecordId( aCmId );
        iCmId = 0;
        }
    else
        {
        iIapRecord->SetRecordId( KCDNewRecordRequest );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_CREATENEWL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CreateCopyL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng* CCmPluginBaseEng::CreateCopyL(
        CCmClientPluginInstance* /*aClientPluginInstance*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CREATECOPYL_ENTRY );

    TCmPluginInitParam params( iSession );
    CCmPluginBaseEng* copyInst = CreateInstanceL( params );
    CleanupStack::PushL( copyInst );

    PrepareToCopyDataL( copyInst );
    CopyDataL( copyInst );
    CleanupStack::Pop( copyInst );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_CREATECOPYL_EXIT );
    return copyInst;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetGenericTableIdsToBeObserved
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::GetGenericTableIdsToBeObservedL(
        RArray<TUint32>& aTableIdArray ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETGENERICTABLEIDSTOBEOBSERVED_ENTRY );

    // Service and bearer records should be added by plugins.
    aTableIdArray.AppendL( KCDTIdIAPRecord );
    aTableIdArray.AppendL( KCDTIdWAPAccessPointRecord );
    aTableIdArray.AppendL( KCDTIdWAPIPBearerRecord );
    aTableIdArray.AppendL( KCDTIdProxiesRecord );
    aTableIdArray.AppendL( iMetadataTableId );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETGENERICTABLEIDSTOBEOBSERVED_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CopyDataL( CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_COPYDATAL_ENTRY );

    aCopyInstance->iLocationEnabled = iLocationEnabled;
    aCopyInstance->iNamingMethod = iNamingMethod;
    CopyRecordsL( aCopyInstance );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_COPYDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CopyRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CopyRecordsL( CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_COPYRECORDSL_ENTRY );

    CopyRecordDataL( KIapRecordIndex, aCopyInstance );

    // Ensure that IAP's name is set by the rules (Naming Method).
    SetNameL(
            iIapRecord->iRecordName.GetL(),
            aCopyInstance->iIapRecord,
            aCopyInstance->iNamingMethod );

    CopyRecordDataL( KServiceRecordIndex, aCopyInstance );
    CopyRecordDataL( KNetworkRecordIndex, aCopyInstance );
    CopyRecordDataL( KWAPAPRecordIndex, aCopyInstance );
    CopyRecordDataL( KWAPBearerRecordIndex, aCopyInstance );
    CopyRecordDataL( KMetaDataRecordIndex, aCopyInstance );
    CopyRecordDataL( KProxyRecordIndex, aCopyInstance );

    CopyBearerRecordsL( aCopyInstance );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_COPYRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CopyRecordDataL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CopyRecordDataL(
        TUint32 aRecordIdentifier,
        CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_COPYRECORDDATAL_ENTRY );

    switch ( aRecordIdentifier )
        {
        case KIapRecordIndex:
            {
            aCopyInstance->iIapRecord = static_cast<CCDIAPRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
            CopyRecordFieldsL( *iIapRecord, *aCopyInstance->iIapRecord );
            }
            break;
        case KServiceRecordIndex:
            {
            aCopyInstance->iServiceRecord = CopyServiceRecordL();
            // The name of the service record copy is changed during update
            // process, in PrepareToUpdateRecordsL()-method.
            }
            break;
        case KNetworkRecordIndex:
            {
            aCopyInstance->iNetworkRecord = static_cast<CCDNetworkRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
            CopyRecordFieldsL( *iNetworkRecord, *aCopyInstance->iNetworkRecord );
            }
            break;
        case KWAPAPRecordIndex:
            {
            aCopyInstance->iWapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );
            CopyRecordFieldsL( *iWapAPRecord, *aCopyInstance->iWapAPRecord );
            }
            break;
        case KWAPBearerRecordIndex:
            {
            aCopyInstance->iWapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
            CopyRecordFieldsL( *iWapIPBearerRecord, *aCopyInstance->iWapIPBearerRecord );
            }
            break;
        case KMetaDataRecordIndex:
            {
            aCopyInstance->iMetaDataRecord = new( ELeave ) CCDIAPMetadataRecord( 
                    iMetadataTableId );
            CopyRecordFieldsL( *iMetaDataRecord, *aCopyInstance->iMetaDataRecord );
            }
            break;
        case KProxyRecordIndex:
            {
            if ( iProxyRecord )
                {
                aCopyInstance->iProxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
                CopyRecordFieldsL( *iProxyRecord, *aCopyInstance->iProxyRecord );
                }
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_COPYRECORDDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// Copies the values and attributes of all fields from aSource-record into
// aDestination record. Does not copy the record element ID. Also, does not
// touch any field in aDestination-record that is NULL in aSource-record.
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::CopyRecordFieldsL(
        CommsDat::CMDBRecordBase& aSource,
        CommsDat::CMDBRecordBase& aDestination )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_COPYRECORDFIELDSL_ENTRY );

    const SRecordTypeInfo* recordInfo = aSource.GetRecordInfo();
    if ( !recordInfo )
        {
        User::Leave( KErrCorrupt );
        }

    CMDBElement* ptrSource( NULL );
    CMDBElement* ptrDest( NULL );

    while ( recordInfo && recordInfo->iTypeId != 0 )
        {
        ptrSource = aSource.GetFieldByIdL( recordInfo->iTypeId );
        ptrDest = aDestination.GetFieldByIdL( recordInfo->iTypeId );

        if ( !( ptrSource->IsNull() ) )
            {
            // Make sure we see only basic type info. Masks out any additional info
            // on links (CommsDat internal stuff).
            switch ( recordInfo->iValType & 0x000000ff )
                {
                case CommsDat::EInt:
                case CommsDat::EBool:
                    {
                    static_cast<CMDBField<TInt>&>( *ptrDest ).SetL(
                            static_cast<CMDBField<TInt>&>( *ptrSource ) );
                    }
                    break;
                case CommsDat::EUint32:
                case CommsDat::ELink:
                    {
                    static_cast<CMDBField<TUint32>&>( *ptrDest ).SetL(
                            static_cast<CMDBField<TUint32>&>( *ptrSource ) );
                    }
                    break;
                case CommsDat::EDesC8:
                    {
                    static_cast<CMDBField<TDesC8>&>( *ptrDest ).SetL(
                            static_cast<CMDBField<TDesC8>&>( *ptrSource ) );
                    }
                    break;
                case CommsDat::EText:
                case CommsDat::EMedText:
                case CommsDat::ELongText:
                    {
                    static_cast<CMDBField<TDesC>&>( *ptrDest ).SetL(
                            static_cast<CMDBField<TDesC>&>( *ptrSource ) );
                    }
                    break;
                default:
                    {
                    User::Leave( KErrCorrupt );
                    }
                    break;
                }
            ptrDest->ClearAttributes( ptrDest->Attributes() );
            ptrDest->SetAttributes( ptrSource->Attributes() );
            }
        recordInfo++;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_COPYRECORDFIELDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::DoLoadL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::DoLoadL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_DOLOADL_ENTRY );

    PrepareToLoadRecordsL();

    LoadIAPRecordL( aIapId );
    LoadWapRecordL();
    LoadMetadataRecordL();
    LoadNetworkRecordL();

    LoadServiceRecordL();
    LoadProxyRecordL();

    LoadBearerRecordsL();

    OstTraceFunctionExit0( CCMPLUGINBASEENG_DOLOADL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::PrepareToLoadRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::PrepareToLoadRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_PREPARETOLOADRECORDSL_ENTRY );

    iLocationEnabled = EFalse;
    PreparePluginToLoadRecordsL();

    OstTraceFunctionExit0( CCMPLUGINBASEENG_PREPARETOLOADRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadIAPRecordL( TUint32 aIapId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADIAPRECORDL_ENTRY );

    iCmId = aIapId;

    // Load IAP record.
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );

    CleanupStack::PushL( iapRecord );

    iapRecord->SetRecordId( iCmId );
    iapRecord->LoadL( iSession );

    CleanupStack::Pop( iapRecord );
    iIapRecord = iapRecord;

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADIAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadProxySettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadProxyRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADPROXYSETTINGL_ENTRY );

    // Load Proxy record.
    CMDBRecordSet<CCDProxiesRecord>* proxieRS =
            new( ELeave ) CMDBRecordSet<CCDProxiesRecord>( KCDTIdProxiesRecord );
    CleanupStack::PushL(proxieRS);

    // Now try to find the linked proxy record.
    // Create new record.
    CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
    CleanupStack::PushL( proxyRecord );

    // Prime record.
    TPtrC serviceType( iIapRecord->iServiceType );
    proxyRecord->iServiceType.SetL( serviceType );
    proxieRS->iRecords.AppendL( proxyRecord ); // Ownership transfered.

    CleanupStack::Pop( proxyRecord );
    proxyRecord = NULL;

    if ( proxieRS->FindL(iSession) )
        {
        TInt i( 0 );
        while ( i < proxieRS->iRecords.Count() )
            // Now that we have a list of services with the proper service type
            // search for our proxy record and remove it from the array,
            // then destroy the array.
            {
            CCDProxiesRecord* proxyRecord = (*proxieRS)[i];

            // Compare the record id of these 2 services.
            if ( TUint32( proxyRecord->iService ) == TUint32( iIapRecord->iService ) )
                {
                iProxyRecord = proxyRecord;
                // Take ownership of this record.
                proxieRS->iRecords.Remove( i );
                break;
                }
            ++i;
            }
        }

    proxieRS->iRecords.ResetAndDestroy();

    if ( !iProxyRecord )
        {
        // This creates a proxy record, where usage is disabled.
        EnableProxyL( EFalse );
        }

    CleanupStack::PopAndDestroy( proxieRS );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADPROXYSETTINGL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadNetworkSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadNetworkRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADNETWORKSETTINGL_ENTRY );

    CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
    CleanupStack::PushL( networkRecord );

    networkRecord->SetRecordId( iIapRecord->iNetwork );
    TRAPD( err, networkRecord->LoadL( iSession ) );
    if ( err == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( networkRecord );

        networkRecord = static_cast<CCDNetworkRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
        }
    else
        // KErrNotFound -> doesn't matter. We will do it UpdateL()
        {
        User::LeaveIfError( err );

        CleanupStack::Pop( networkRecord );
        }

    iNetworkRecord = networkRecord;

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADNETWORKSETTINGL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadLocationSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadLocationRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADLOCATIONSETTINGL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADLOCATIONSETTINGL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::PrepareToUpdateRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::PrepareToUpdateRecordsL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_PREPARETOUPDATERECORDSL_ENTRY );

    //TODO, Add null checks for mandatory generic record pointers.
    //

    // Set the record attributes to bearer specific records.
    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    for ( TInt i = 0; i < aClientPluginInstance->iBearerSpecRecordArray.Count(); i++ )
        {
        CCDRecordBase* record = static_cast<CCDRecordBase*>(
                aClientPluginInstance->iBearerSpecRecordArray[i] );

        CopyAttributes( iapRecord, record );
        }

    PreparePluginToUpdateRecordsL(
            aClientPluginInstance->iGenRecordArray,
            aClientPluginInstance->iBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_PREPARETOUPDATERECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateIAPRecordL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATEIAPRECORDL_ENTRY );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    if ( iCmId == 0 )
        {
        // Otherwise predefined iapid.
        if ( !iIapRecord->RecordId() )
            {
            iIapRecord->SetRecordId( KCDNewRecordRequest );
            }

        TUint32 id( 0 );
        id = ServiceRecordId();
        iIapRecord->iService = id;

        HBufC* name( NULL );
        ServiceRecordNameLC( name );
        iIapRecord->iServiceType.SetL( *name );
        CleanupStack::PopAndDestroy( name );

        BearerRecordIdL( id );
        iIapRecord->iBearer = id;
        BearerRecordNameLC( name );
        iIapRecord->iBearerType.SetL( *name );
        CleanupStack::PopAndDestroy( name );

        iIapRecord->iNetwork = iNetworkRecord->RecordId();

        iIapRecord->iLocation = GetLocationIdL();

        CopyAttributes( iapRecord, iIapRecord );
        CheckIfNameModifiedL( iapRecord, iIapRecord );

        iIapRecord->StoreL( iSession );
        // Have to be "reloaded" to get possible default values 
        // from template records.
        iIapRecord->LoadL( iSession );

        iCmId = iIapRecord->RecordId();
        aClientPluginInstance->iIapId = iCmId;

        CCDIAPRecord* tempIapRecord = static_cast<CCDIAPRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        CleanupStack::PushL( tempIapRecord );
        CopyRecordFieldsL( *iIapRecord, *tempIapRecord );
        tempIapRecord->SetElementId( iIapRecord->ElementId() );

        aClientPluginInstance->iGenRecordArray.Remove( KIapRecordIndex );
        delete iapRecord;
        iapRecord = NULL;

        aClientPluginInstance->iGenRecordArray.InsertL(
                static_cast<CommsDat::CCDRecordBase*>( tempIapRecord ),
                KIapRecordIndex );

        CleanupStack::Pop( tempIapRecord );
        }
    else
        {
        delete iIapRecord;
        iIapRecord = NULL;
        iIapRecord = static_cast<CCDIAPRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        CopyRecordFieldsL( *iapRecord, *iIapRecord );

        iIapRecord->SetElementId( iapRecord->ElementId() );
        iIapRecord->ModifyL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATEIAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateProxyRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateProxyRecordL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATEPROXYRECORDL_ENTRY );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
            aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );

    if ( !iapRecord || !proxyRecord )
        {
        User::Leave( KErrCorrupt );
        }

    CopyAttributes( iapRecord, proxyRecord );
    CheckIfNameModifiedL( iapRecord, proxyRecord );

    delete iProxyRecord;
    iProxyRecord = NULL;
    iProxyRecord = static_cast<CCDProxiesRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
    CopyRecordFieldsL( *proxyRecord, *iProxyRecord );
    iProxyRecord->SetElementId( proxyRecord->ElementId() );

    if ( !iProxyRecord->RecordId() )
        {
        // New proxy setting -> create new record.
        iProxyRecord->iService = iServiceRecord->RecordId();
        iProxyRecord->iServiceType.SetL( iIapRecord->iServiceType );

        // By default protocol is set to "http".
        if ( TPtrC( proxyRecord->iProtocolName ).Length() == 0 )
            {
            iProxyRecord->iProtocolName.SetL( KDefProxyProtocolName );
            }

        iProxyRecord->SetRecordId( KCDNewRecordRequest );
        iProxyRecord->StoreL( iSession );
        // Have to be "reloaded" to get possible default values from template records.
        iProxyRecord->LoadL( iSession );

        proxyRecord->SetElementId( iProxyRecord->ElementId() );
        }
    else
        // Already existing record -> update only.
        {
        iProxyRecord->ModifyL( iSession );
        }
    CopyRecordFieldsL( *iProxyRecord, *proxyRecord );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATEPROXYRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateMetadataRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateMetadataRecordL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATEMETADATARECORDL_ENTRY );

    CCDIAPRecord* clientIapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    CCDIAPMetadataRecord* clientMetadataRecord = static_cast<CCDIAPMetadataRecord*>(
            aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );

    // Adjust client side metadata record attributes and name if needed.
    CopyAttributes( clientIapRecord, clientMetadataRecord );
    CheckIfNameModifiedL( clientIapRecord, clientMetadataRecord );

    // Make a copy of client's metadata record and save to database.
    delete iMetaDataRecord;
    iMetaDataRecord = NULL;
    iMetaDataRecord = new( ELeave ) CCDIAPMetadataRecord( iMetadataTableId );

    CopyRecordFieldsL( *clientMetadataRecord, *iMetaDataRecord );

    iMetaDataRecord->SetElementId( clientMetadataRecord->ElementId() );

    if ( !iMetaDataRecord->RecordId() )
        {
        iMetaDataRecord->iIAP = IAPRecordElementId();
        iMetaDataRecord->SetRecordId( KCDNewRecordRequest );
        iMetaDataRecord->StoreL( iSession );
        // Have to be "reloaded" to get possible default values from template records.
        iMetaDataRecord->LoadL( iSession );

        clientMetadataRecord->SetElementId( iMetaDataRecord->ElementId() );
        }
    else
        {
        iMetaDataRecord->ModifyL( iSession );
        }
    CopyRecordFieldsL( *iMetaDataRecord, *clientMetadataRecord );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATEMETADATARECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateServiceRecordL(
    CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATESERVICERECORDL_ENTRY );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    CCDRecordBase* serviceRecord = static_cast<CCDRecordBase*>(
            aClientPluginInstance->iGenRecordArray[KServiceRecordIndex] );

    CopyAttributes( iapRecord, serviceRecord );

    UpdateServiceRecordL(
            aClientPluginInstance->iGenRecordArray,
            aClientPluginInstance->iBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATESERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateNetworkRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateNetworkRecordL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATENETWORKRECORDL_ENTRY );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>(
            aClientPluginInstance->iGenRecordArray[KNetworkRecordIndex] );

    CopyAttributes( iapRecord, networkRecord );

    delete iNetworkRecord;
    iNetworkRecord = NULL;

    CheckIfNameModifiedL( iapRecord, networkRecord );

    iNetworkRecord = static_cast<CCDNetworkRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
    CopyRecordFieldsL( *networkRecord, *iNetworkRecord );

    iNetworkRecord->SetElementId( networkRecord->ElementId() );

    if ( !iNetworkRecord->RecordId() )
        {
        iNetworkRecord->SetRecordId( KCDNewRecordRequest );
        iNetworkRecord->StoreL( iSession );
        // Have to be "reloaded" to get possible default values from template records.
        iNetworkRecord->LoadL( iSession );

        networkRecord->SetElementId( iNetworkRecord->ElementId() );
        }
    else
        {
        iNetworkRecord->ModifyL( iSession );
        }
    CopyRecordFieldsL( *iNetworkRecord, *networkRecord );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATENETWORKRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateLocationRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateLocationRecordL(
        CCmClientPluginInstance* /*aClientPluginInstance*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATELOCATIONRECORDL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATELOCATIONRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateConnPrefSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateConnPrefSettingL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATECONNPREFSETTINGL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINBASEENG_UPDATECONNPREFSETTINGL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnableProxyL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::EnableProxyL( TBool aEnable )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ENABLEPROXYL_ENTRY );

    if ( !iServiceRecord )
        {
        User::Leave( KErrNotSupported );
        }

    if ( !iProxyRecord )
        {
        iProxyRecord = static_cast<CCDProxiesRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
        }

    iProxyRecord->iUseProxyServer = aEnable;

    OstTraceFunctionExit0( CCMPLUGINBASEENG_ENABLEPROXYL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnableLocationL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::EnableLocationL( TBool aEnable )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ENABLELOCATIONL_ENTRY );

    // Not supported anymore
    iLocationEnabled = aEnable;

    OstTraceFunctionExit0( CCMPLUGINBASEENG_ENABLELOCATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadWapRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADWAPRECORDL_ENTRY );

    iWapIPBearerRecord = FindWAPRecordL();
    if ( iWapIPBearerRecord )
        {
        CCDWAPAccessPointRecord *wapApRecord = static_cast<CCDWAPAccessPointRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );

        CleanupStack::PushL( wapApRecord );

        wapApRecord->SetRecordId( iWapIPBearerRecord->iWAPAccessPointId );
        TRAPD( err, wapApRecord->LoadL( iSession ) );
        if ( err == KErrNotFound )
            {
            CleanupStack::PopAndDestroy( wapApRecord );

            wapApRecord = static_cast<CCDWAPAccessPointRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );
            }
        else
            {
            User::LeaveIfError( err );

            CleanupStack::Pop( wapApRecord );
            }

        iWapAPRecord = wapApRecord;
        }
    else
        // No WAP record found -> create a new one
        {
        NewWapRecordL();
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADWAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadMetadataRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadMetadataRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_LOADSEAMLESSNESSRECORDL_ENTRY );

    iMetaDataRecord = FindMetadataRecordL();

    OstTraceFunctionExit0( CCMPLUGINBASEENG_LOADSEAMLESSNESSRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateWapRecordL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_UPDATEWAPRECORDL_ENTRY );

    delete iWapAPRecord;
    iWapAPRecord = NULL;

    delete iWapIPBearerRecord;
    iWapIPBearerRecord = NULL;

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    CCDWAPAccessPointRecord* wapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
            aClientPluginInstance->iGenRecordArray[KWAPAPRecordIndex] );

    CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
            aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

    CopyAttributes( iapRecord, wapAPRecord );
    CopyAttributes( iapRecord, wapIPBearerRecord );

    CheckIfNameModifiedL( iapRecord, wapAPRecord );
    CheckIfNameModifiedL( iapRecord, wapIPBearerRecord );

    // WAP AP record
    iWapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );
    CopyRecordFieldsL( *wapAPRecord, *iWapAPRecord );
    iWapAPRecord->SetElementId( wapAPRecord->ElementId() );

    // WAP IP Bearer record
    iWapIPBearerRecord =static_cast<CCDWAPIPBearerRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
    CopyRecordFieldsL( *wapIPBearerRecord, *iWapIPBearerRecord );
    iWapIPBearerRecord->SetElementId( wapIPBearerRecord->ElementId() );

    if ( !iWapAPRecord->RecordId() )
        {
        iWapAPRecord->SetRecordId( KCDNewRecordRequest );
        iWapAPRecord->StoreL(iSession );
        // Have to be "reloaded" to get possible default values from template records.
        iWapAPRecord->LoadL(iSession );

        wapAPRecord->SetElementId( iWapAPRecord->ElementId() );
        }
    else
        {
        iWapAPRecord->ModifyL( iSession );
        }
    CopyRecordFieldsL( *iWapAPRecord, *wapAPRecord );

    if ( !iWapIPBearerRecord->RecordId() )
        {
        iWapIPBearerRecord->iWAPAccessPointId = iWapAPRecord->RecordId();
        iWapIPBearerRecord->iWAPIAP = iapRecord->RecordId();

        iWapIPBearerRecord->SetRecordId( KCDNewRecordRequest );
        iWapIPBearerRecord->StoreL( iSession );
        // Have to be "reloaded" to get possible default values from template records.
        iWapIPBearerRecord->LoadL( iSession );

        wapIPBearerRecord->SetElementId( iWapIPBearerRecord->ElementId() );
        }
    else
        {
        iWapIPBearerRecord->ModifyL( iSession );
        }
    CopyRecordFieldsL( *iWapIPBearerRecord, *wapIPBearerRecord );

    OstTraceFunctionExit0( DUP1_CCMPLUGINBASEENG_UPDATEWAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::NewWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::NewWapRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_NEWWAPRECORDL_ENTRY );

    if ( !iWapIPBearerRecord )
        {
        iWapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        iWapIPBearerRecord->iWAPProxyPort = 0;
        iWapIPBearerRecord->iWAPWSPOption = ECmWapWspOptionConnectionOriented;
        iWapIPBearerRecord->iWAPSecurity = EFalse;
        }

    if ( !iWapAPRecord )
        {
        iWapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );
        // SMS bearer is not supported by this version.
        iWapAPRecord->iWAPCurrentBearer.SetL( TPtrC( KCDTypeNameWAPIPBearer ) );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_NEWWAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::NewMetadataRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBaseEng::NewMetadataRecordL( TBool aSetDef )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_NEWSEAMLESSNESSRECORDL_ENTRY );

    CCDIAPMetadataRecord* record = new( ELeave ) CCDIAPMetadataRecord( iMetadataTableId );

    if ( aSetDef )
        {
        TRAPD( err, record->iSeamlessness = GetBearerInfoIntL( ECmSeamlessnessLevel ) );
        if ( err )
            {
            record->iSeamlessness = ESeamlessnessConfirmFirst;
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_NEWSEAMLESSNESSRECORDL_EXIT );
    return record;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetAttribute
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::SetAttribute(
        CCDRecordBase* aRecord,
        TUint32 aAttribute,
        TBool aSet )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETATTRIBUTE_ENTRY );

    if ( aSet )
        {
        aRecord->SetAttributes( aAttribute );
        }
    else
        {
        aRecord->ClearAttributes( aAttribute );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETATTRIBUTE_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CopyAttributes
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CopyAttributes(
        CCDRecordBase* aSrcRecord,
        CCDRecordBase* aDstRecord )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_COPYATTRIBUTES_ENTRY );

    if ( !aSrcRecord || !aDstRecord )
        {
        OstTraceFunctionExit0( CCMPLUGINBASEENG_COPYATTRIBUTES_EXIT );
        return;
        }

    // Clear the target record attributes first.
    aDstRecord->ClearAttributes( aDstRecord->Attributes() );
    aDstRecord->SetAttributes( aSrcRecord->Attributes() );

    OstTraceFunctionExit0( DUP1_CCMPLUGINBASEENG_COPYATTRIBUTES_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FindWAPRecordL
// ---------------------------------------------------------------------------
//
CCDWAPIPBearerRecord* CCmPluginBaseEng::FindWAPRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_FINDWAPRECORDL_ENTRY );

    CMDBRecordSet<CCDWAPIPBearerRecord>* wapRS =
            new( ELeave ) CMDBRecordSet<CCDWAPIPBearerRecord>( KCDTIdWAPIPBearerRecord );
    CleanupStack::PushL( wapRS );

    CCDWAPIPBearerRecord* wapBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );

    CleanupStack::PushL( wapBearerRecord );

    wapBearerRecord->iWAPIAP = iCmId;
    wapRS->iRecords.AppendL( wapBearerRecord );

    CleanupStack::Pop( wapBearerRecord );
    wapBearerRecord = NULL;

    if ( wapRS->FindL( iSession ) )
        {
        wapBearerRecord = static_cast<CCDWAPIPBearerRecord*>( wapRS->iRecords[0] );

        // Take over the ownership of this record.
        wapRS->iRecords.Remove( 0 );
        }

    CleanupStack::PopAndDestroy( wapRS );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_FINDWAPRECORDL_EXIT );
    return wapBearerRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FindSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBaseEng::FindMetadataRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_FINDSEAMLESSNESSRECORDL_ENTRY );

    CMDBRecordSet<CCDIAPMetadataRecord>* metadataRecordSet =
            new( ELeave ) CMDBRecordSet<CCDIAPMetadataRecord>( iMetadataTableId );
    CleanupStack::PushL( metadataRecordSet );

    CCDIAPMetadataRecord* metadataRecord =
            new( ELeave ) CCDIAPMetadataRecord( iMetadataTableId );
    CleanupStack::PushL( metadataRecord );

    metadataRecord->iIAP = IAPRecordElementId();
    metadataRecordSet->iRecords.AppendL( metadataRecord );

    CleanupStack::Pop( metadataRecord );
    metadataRecord = NULL;

    if ( metadataRecordSet->FindL( iSession ) )
        {
        CMDBRecordBase* record = metadataRecordSet->iRecords[0];

        metadataRecord = NewMetadataRecordL( EFalse );
        metadataRecord->SetRecordId( record->RecordId() );

        // This can leave only in case of OOM.
        metadataRecord->LoadL( iSession );
        }
    else
        {
        metadataRecord = NewMetadataRecordL( ETrue );
        }

    CleanupStack::PopAndDestroy( metadataRecordSet );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_FINDSEAMLESSNESSRECORDL_EXIT );
    return metadataRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::DoMakeValidNameL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::DoMakeValidNameL(
        const TDesC& aName,
        const TUint32& aIapId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_DOMAKEVALIDNAMEL_ENTRY );

    const TInt decimalBase = 10;
    TBool changed( EFalse );
    HBufC* temp = HBufC::NewLC( KApMaxConnNameLength );
    HBufC* temp2 = HBufC::NewLC( KApMaxConnNameLength );

    HBufC* corrname = EnsureMaxLengthLC( aName, changed );
    *temp = *corrname;
    TInt postfix( 0 );
    TInt pf( 0 );
    TInt i( 0 );
    TBool valid ( EFalse );

    TPtrC prefix = GetPrefix( *corrname );

    postfix = GetPostfix( *temp, prefix );
    postfix = -1;
    do
        {
        valid = IsValidNameL( *temp, aIapId );
        if ( !valid )
            {
            changed = ETrue;
            postfix++;
            // Check the length of postfix, check text length accordingly.
            pf = postfix;
            for ( i = 1; i < decimalBase; i++ )
                {
                pf /= decimalBase;
                if ( !pf )
                    break;
                }
            TPtr sgdptr( temp->Des() );
            TPtr sgdptr2( temp2->Des() );
            if ( postfix )
                {
                if ( postfix < decimalBase )
                    {
                    sgdptr2 = prefix.Left( KApMaxConnNameLength - i - 3 );
                    }
                else
                    {
                    sgdptr2 = prefix.Left( KApMaxConnNameLength - i - 2 );
                    }
                }
            else
                {
                sgdptr2 = prefix.Left( KApMaxConnNameLength );
                }
            if ( postfix )
                {
                TBuf<KMaxPostfixLength> postfixString;
                if ( postfix >= decimalBase )
                    {
                    postfixString.Format( KFormatLargePostfix, postfix );
//     TODO:               AknTextUtils::LanguageSpecificNumberConversion( postfixString );
                    }
                else
                    {
                    postfixString.Format( KFormatPostfix, postfix );
//     TODO:               AknTextUtils::LanguageSpecificNumberConversion( postfixString );
                    }
                sgdptr.Format( KFormatNameWithPostfix, &sgdptr2, &postfixString );
                }
            else
                {
                sgdptr.Format( KFormatNoPostfix, &sgdptr2 );
                }
            }
        } while ( !valid );

    CleanupStack::PopAndDestroy( corrname );
    CleanupStack::PopAndDestroy( temp2 );

    if ( changed )
        {
        CleanupStack::Pop( temp );
        }
    else
        {
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_DOMAKEVALIDNAMEL_EXIT );
    return temp;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnsureMaxLengthLC
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::EnsureMaxLengthLC(
        const TDesC& aName,
        TBool& aChanged )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ENSUREMAXLENGTHLC_ENTRY );

    TInt length = aName.Length();

    aChanged = EFalse;

    if ( !length )
        {
        // Name is required.
        User::Leave( KErrArgument );
        }

    HBufC* corrname;
    if ( KApMaxConnNameLength < length )
        {
        // Name too long, truncate.
        corrname = aName.Left( KApMaxConnNameLength ).AllocLC();
        aChanged = ETrue;
        }
    else
        {
        corrname = aName.AllocLC();
        corrname->Des().Trim();
        if ( corrname->Length() == 0 )
            {
            User::Leave( KErrArgument );
            }
        // Comes here only if name is valid.
        if ( corrname->Length() != aName.Length() )
            {
            aChanged = ETrue;
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_ENSUREMAXLENGTHLC_EXIT );
    return corrname;
    }

// ---------------------------------------------------------------------------
// Given aName in the format <prefix> or <prefix><brace><integer><brace>,
// return a pointer to the leading part.
// That is, if there is trailing <space><integer>,
// then that is excluded; if there is no trailing part, then the original
// decriptor is returned.
// Examples:
//   - "Foo" returns "Foo";
//   - "Foo 12" returns "Foo 12";
//   - "Foo(12)" returns "Foo";
//   - "Foo 12 (34)" returns "Foo 12 ";
//   - "Foo bar" returns "Foo bar";
//   - "Foo " returns "Foo ".
// ---------------------------------------------------------------------------
TPtrC CCmPluginBaseEng::GetPrefix( const TDesC& aName )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETPREFIX_ENTRY );

    TPtrC prefix = aName;
    TInt lastBrace = aName.LocateReverse('(');
    if ( lastBrace != KErrNotFound )
        {
        // aName looks like "<prefix><brace><something>".
        // See if <something> is an integer number.
        TPtrC num = aName.Right( aName.Length() - lastBrace - 1 );
        TInt val;
        TLex lex( num );
        if ( lex.Val( val ) == KErrNone )
            {
            // Yes, the trailer is an integer.
            prefix.Set( aName.Left( lastBrace ) );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETPREFIX_EXIT );
    return prefix;
    }


// ---------------------------------------------------------------------------
// If aName is constructed from aPrefix with a postfix, get the numeric
// value of the postfix, e.g:
//   - GetPostfix( "Foo (3)", "Foo" ) == 3
//   - GetPostfix( "Foo 23 (45)", "Foo 23" ) == 45
// If aName is the same as aPrefix, return 0, e.g.:
//   - GetPostfix( "Foo", "Foo" ) == 0
// If aName is not constructed from aPrefix, return -1, e.g.:
//   - GetPostfix( "Foobar", "Foo" ) == -1
//   - GetPostfix( "Fo 23 45", "Foo" ) == -1
// ---------------------------------------------------------------------------
TInt CCmPluginBaseEng::GetPostfix( const TDesC& aName, const TDesC& aPrefix )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETPOSTFIX_ENTRY );

    TInt postfix( KErrNotFound );
    TInt nameLength = aName.Length();
    TInt prefixLength = aPrefix.Length();
    if ( nameLength >= prefixLength && aName.FindF( aPrefix ) == 0 )
        {
        // aName is longer or equal length, and
        // aPrefix can be found in the beginning of aName.
        if ( nameLength == prefixLength )
            {
            // They have the same length; they equal.
            postfix = 0;
            }
        else
            {
            if ( prefixLength > 0 )
                {
                if ( aName[ prefixLength ] == '(' )
                    {
                    // (Now we know that aName is longer than aPrefix.)
                    // aName looks like "aPrefix<brace><something>".
                    // See if <something> is an integer number.
                    TPtrC num = aName.Right( nameLength - prefixLength - 1 );
                        TBuf< KApMaxConnNameLength > pf;
                    pf = num;
// TODO                    AknTextUtils::ConvertDigitsTo( pf, EDigitTypeWestern );
                    TInt val;
                    TLex lex( pf );
                    if ( lex.Val( val ) == KErrNone )
                        {
                        // Yes, the trailer is an integer.
                        if ( val > 0 )
                            {
                            postfix = val;
                            }
                        else
                            {
                            // signal that it is invalid...
                            postfix = -1;
                            }
                        }
                    }
                }
            else
                {
                postfix = -1;
                }
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETPOSTFIX_EXIT );
    return postfix;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IsValidNameL
// ---------------------------------------------------------------------------
//
TBool CCmPluginBaseEng::IsValidNameL(
        const TDesC& aNameText,
        const TUint32& aIapId )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ISVALIDNAMEL_ENTRY );

    TBool retVal( ETrue );

    CMDBRecordSet<CCDIAPRecord>* iapRS =
            new( ELeave ) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
    CleanupStack::PushL(iapRS);

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );

    // Prime record.
    iapRecord->iRecordName.SetL( aNameText );
    iapRS->iRecords.AppendL( iapRecord );
    CleanupStack::Pop( iapRecord );
    iapRecord = NULL;

    if ( iapRS->FindL( iSession ) )
        {
        if ( iCmId )
            {
            // This is not a new CM.
            for ( TInt i = 0; i < iapRS->iRecords.Count(); ++i )
                {
                if ( iapRS->iRecords[i]->RecordId() != aIapId )
                    {
                    // Duplication because it's not our name.
                    retVal = EFalse;
                    break;
                    }
                }
            }
        else
            {
            // New CM -> any occurence is a duplication.
            retVal = EFalse;
            }
        }

    CleanupStack::PopAndDestroy( iapRS );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_ISVALIDNAMEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EscapeTextLC
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::EscapeTextLC( const TDesC& aLiteral )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ESCAPETEXTLC_ENTRY );

    TInt l( aLiteral.Length() );
    //> 2*, to ensure correct esc. even if ALL chars are quotes...
    HBufC* retbuf = HBufC::NewLC( 2*l );
    TPtr ret = retbuf->Des();
    TUint quote( '\'' );  // TChar gives warnings in THUMB & ARMI
    TInt i( 0 );

    for ( i = 0; i < l; i++ )
        {
        ret.Append( aLiteral[i] );
        if ( aLiteral[i] == quote )
            {
            // Duplicate quote.
            ret.Append( quote );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_ESCAPETEXTLC_EXIT );
    return retbuf;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetDefaultNameL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::SetDefaultNameL( const TDesC& aName )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETDEFAULTNAMEL_ENTRY );

    if ( iNamingMethod == ENamingNothing )
        {
        iIapRecord->iRecordName.SetL( aName );
        }
    else if ( iNamingMethod == ENamingUnique )
        {
        HBufC* newName = DoMakeValidNameL( aName, iCmId );

        if ( newName )
            // Name converted to unique.
            {
            CleanupStack::PushL( newName );
            iIapRecord->iRecordName.SetL( *newName );
            CleanupStack::PopAndDestroy( newName );
            }
        else
            {
            iIapRecord->iRecordName.SetL( aName );
            }
        }
    else if ( iNamingMethod == ENamingNotAccept )
        {
        if ( IsValidNameL( aName, iCmId ) )
            {
            iIapRecord->iRecordName.SetL( aName );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETDEFAULTNAMEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetNameL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::SetNameL(
        const TDesC& aName,
        CommsDat::CCDRecordBase* aIapRecord,
        const CMManager::TNamingMethod aNamingMethod )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETNAMEL_ENTRY );

    if ( aNamingMethod == ENamingNothing )
        {
        aIapRecord->iRecordName.SetL( aName );
        }
    else if ( aNamingMethod == ENamingUnique )
        {
        TUint32 iapid = aIapRecord->RecordId();
        HBufC* newName = DoMakeValidNameL( aName, iapid );

        if ( newName )
            // Name converted to unique.
            {
            CleanupStack::PushL( newName );
            aIapRecord->iRecordName.SetL( *newName );
            CleanupStack::PopAndDestroy( newName );
            }
        else
            {
            aIapRecord->iRecordName.SetL( aName );
            }
        }
    else if ( aNamingMethod == ENamingNotAccept )
        {
        if ( IsValidNameL( aName, aIapRecord->RecordId() ) )
            {
            aIapRecord->iRecordName.SetL( aName );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETNAMEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckIfNameModifiedL
// ---------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::CheckIfNameModifiedL(
        CCDRecordBase* aSrcRecord,
        CCDRecordBase* aDestRecord ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CHECKIFNAMEMODIFIEDL_ENTRY );

    if ( !aSrcRecord || !aDestRecord )
        {
        User::Leave( KErrCorrupt );
        }

    if ( !TPtrC( aSrcRecord->iRecordName ).CompareF( TPtrC( aDestRecord->iRecordName ) ) )
        {
        // Names match.
        OstTraceFunctionExit0( CCMPLUGINBASEENG_CHECKIFNAMEMODIFIEDL_EXIT );
        return;
        }

    aDestRecord->iRecordName.SetL( TPtrC( aSrcRecord->iRecordName ) );

    OstTraceFunctionExit0( DUP1_CCMPLUGINBASEENG_CHECKIFNAMEMODIFIEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IAPRecordElementId
// ---------------------------------------------------------------------------
TMDBElementId CCmPluginBaseEng::IAPRecordElementId() const
    {
    return ( KCDMaskShowField & iIapRecord->ElementId() );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IsProtected
// ---------------------------------------------------------------------------
TBool CCmPluginBaseEng::IsProtected() const
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_ISPROTECTED_ENTRY );

    return ( iIapRecord->Attributes() & ECDProtectedWrite );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetLocationIdL
// ---------------------------------------------------------------------------
TUint32 CCmPluginBaseEng::GetLocationIdL() const
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETLOCATIONIDL_ENTRY );

    TUint32 locId( 0 );
    CMDBRecordSet<CCDLocationRecord>* locRS =
            new( ELeave ) CMDBRecordSet<CCDLocationRecord>( KCDTIdLocationRecord );
    CleanupStack::PushL( locRS );

    CCDLocationRecord* locRecord = static_cast<CCDLocationRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLocationRecord ) );
    CleanupStack::PushL( locRecord );

    locRecord->iRecordName.SetL( KLocationName );
    locRS->iRecords.AppendL( locRecord );
    CleanupStack::Pop( locRecord );

    if ( locRS->FindL(iSession) )
        {
        locRecord = static_cast<CCDLocationRecord*>( locRS->iRecords[0] );
        locId = locRecord->RecordId();
        }
    else
        {
        User::Leave( KErrNotFound ); //TODO, what to do if not found. Can we create it? need study.
        }

    CleanupStack::PopAndDestroy( locRS );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETLOCATIONIDL_EXIT );
    return locId;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetProxyServerNameL
// ---------------------------------------------------------------------------
 void CCmPluginBaseEng::SetProxyServerNameL(
         const TDesC& aProxyServer,
         CCDRecordBase* aProxyRecord )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETPROXYSERVERNAMEL_ENTRY );

    CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>( aProxyRecord );

    proxyRecord->iServerName.SetL( aProxyServer );
    if ( !aProxyServer.Length() )
        {
        proxyRecord->iUseProxyServer = EFalse;
        }
    else
        {
        proxyRecord->iUseProxyServer = ETrue;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETPROXYSERVERNAMEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckDNSServerAddressL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::CheckDNSServerAddressL(
        TBool aIPv6,
        CMDBField<TDesC>& aDNS1,
        CMDBField<TDesC>& aDNS2,
        CMDBField<TBool>& aDNSFromServer )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_CHECKDNSSERVERADDRESSL_ENTRY );

    if ( !aDNSFromServer )
        {
        if ( aIPv6 )
            {
            if ( ClassifyIPv6Address( aDNS1 ) == EIPv6Unspecified )
                {
                if ( ClassifyIPv6Address( aDNS2 ) != EIPv6Unspecified )
                    {
                    aDNS1.SetL( aDNS2 );
                    aDNS2.SetL( KDynamicIpv6Address );
                    }
                else
                    {
                    // Both are unspecified.
                    aDNSFromServer = ETrue;
                    }
                }
            }
        else // IPv4
            {
            if ( IsUnspecifiedIPv4Address( aDNS1 ) )
                {
                if ( !IsUnspecifiedIPv4Address( aDNS2 ) )
                    {
                    aDNS1.SetL( aDNS2 );
                    aDNS2.SetL( KUnspecifiedIPv4 );
                    }
                else
                    {
                    // Both are unspecified.
                    aDNSFromServer = ETrue;
                    }
                }
            }
        }

    OstTraceFunctionExit0( DUP1_CCMPLUGINBASEENG_CHECKDNSSERVERADDRESSL_EXIT );
    }

//-----------------------------------------------------------------------------
// CCmPluginBaseEng::GetGenericCmRecordsL
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::GetPluginDataL(
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETPLUGINDATAL_ENTRY );

    if ( !aClientPluginInstance )
        {
        User::Leave( KErrCorrupt );
        }

    aClientPluginInstance->iGenRecordArray.ResetAndDestroy();
    aClientPluginInstance->iBearerSpecRecordArray.ResetAndDestroy();

    aClientPluginInstance->iNamingMethod = iNamingMethod;
    aClientPluginInstance->iLocationEnabled = iLocationEnabled;

    if ( iIapRecord )
        {
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        CleanupStack::PushL( iapRecord );
        CopyRecordFieldsL( *iIapRecord, *iapRecord );
        iapRecord->SetElementId( iIapRecord->ElementId() );
        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( iapRecord ) );
        CleanupStack::Pop( iapRecord );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( iServiceRecord )
        {
        CCDRecordBase* serviceRecord = CopyServiceRecordL();
        serviceRecord->SetElementId( iServiceRecord->ElementId() );
        CleanupStack::PushL( serviceRecord );
        aClientPluginInstance->iGenRecordArray.AppendL( serviceRecord );
        CleanupStack::Pop( serviceRecord );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( iNetworkRecord )
        {
        CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
        CleanupStack::PushL( networkRecord );
        CopyRecordFieldsL( *iNetworkRecord, *networkRecord );
        networkRecord->SetElementId( iNetworkRecord->ElementId() );
        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( networkRecord ) );
        CleanupStack::Pop( networkRecord );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( iWapAPRecord )
        {
        CCDWAPAccessPointRecord* wapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdWAPAccessPointRecord ) );
        CleanupStack::PushL( wapAPRecord );
        CopyRecordFieldsL( *iWapAPRecord, *wapAPRecord );
        wapAPRecord->SetElementId( iWapAPRecord->ElementId() );
        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( wapAPRecord ) );
        CleanupStack::Pop( wapAPRecord );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( iWapIPBearerRecord )
        {
        CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdWAPIPBearerRecord ) );
        CleanupStack::PushL( wapIPBearerRecord );
        CopyRecordFieldsL( *iWapIPBearerRecord, *wapIPBearerRecord );
        wapIPBearerRecord->SetElementId( iWapIPBearerRecord->ElementId() );
        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( wapIPBearerRecord ) );
        CleanupStack::Pop( wapIPBearerRecord );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    if ( iMetaDataRecord )
        {
        CCDIAPMetadataRecord* metadataRecord =
                new( ELeave ) CCDIAPMetadataRecord( iMetadataTableId );
        CleanupStack::PushL( metadataRecord );
        CopyRecordFieldsL( *iMetaDataRecord, *metadataRecord );
        metadataRecord->SetElementId( iMetaDataRecord->ElementId() );
        // Record name is set/changed during update.

        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( metadataRecord ) );
        CleanupStack::Pop( metadataRecord );
        }
    else
        {
        aClientPluginInstance->iGenRecordArray.AppendL( NULL );
        }

    // Location record does not need to be loaded
    aClientPluginInstance->iGenRecordArray.AppendL( NULL );

    if ( iProxyRecord )
        {
        CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
        CleanupStack::PushL( proxyRecord );
        CopyRecordFieldsL( *iProxyRecord, *proxyRecord );
        proxyRecord->SetElementId( iProxyRecord->ElementId() );
        aClientPluginInstance->iGenRecordArray.AppendL(
                static_cast<CommsDat::CCDRecordBase*>( proxyRecord ) );
        CleanupStack::Pop( proxyRecord );
        }
    else
        {
        aClientPluginInstance->iGenRecordArray.AppendL( NULL );
        }

    GetBearerSpecificRecordsL( aClientPluginInstance->iBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETPLUGINDATAL_EXIT );
    }

//-----------------------------------------------------------------------------
// CCmPluginBaseEng::GetIntAttributeL
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCmPluginBaseEng::GetIntAttributeL(
        TUint32 aAttribute,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    switch ( aAttribute )
        {
        case ECmIapId: // If the CM has an ECmIapId then the ECmIapId is the ECmId.
        case ECmId:
            {
            if ( iBearerType == KUidEmbeddedDestination )
                {
                retVal = GetBearerIntAttributeL(
                        aAttribute,
                        aClientPluginInstance->iGenRecordArray,
                        aClientPluginInstance->iBearerSpecRecordArray );
                }
            else
                {
                retVal = static_cast<CCDIAPRecord*>(
                        aClientPluginInstance->iGenRecordArray[KIapRecordIndex] )->RecordId();
                }
            }
            break;
        case ECmWapId:
            {
            retVal = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )
                    ->iWAPAccessPointId;
            }
            break;
        case ECmIapServiceId:
            {
            retVal = static_cast<CCDIAPRecord*>(
                    aClientPluginInstance->iGenRecordArray[KIapRecordIndex] )->iService;
            }
            break;
        case ECmBearerType:
            {
            retVal = iBearerType;
            }
            break;
        case ECmDefaultPriority:
            {
            retVal = KDataMobilitySelectionPolicyPriorityWildCard;
            }
            break;
        case ECmDefaultUiPriority:
            {
            retVal = KDataMobilitySelectionPolicyPriorityWildCard;
            }
            break;
        case ECmNamingMethod:
            {
            retVal = iNamingMethod;
            }
            break;
        case ECmSeamlessnessLevel:
            {
            if ( iMetaDataRecord )
                {
                retVal = static_cast<CCDIAPMetadataRecord*>(
                        aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] )
                        ->iSeamlessness;
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            }
            break;
        case ECmElementID:
            {
            if ( !iIapRecord )
                {
                User::Leave( KErrNotReady );
                }

            retVal = ( KCDMaskShowField & ( static_cast<CCDIAPRecord*>(
                    aClientPluginInstance->iGenRecordArray[KIapRecordIndex] )->ElementId() ) );
            }
            break;

        case ECmLoadResult:
            {
            // This attribute has been deprecated since Symbian^4.
            User::Leave( KErrNotSupported ); //TODO, update attribute definition comments.
            }
            break;

        case ECmNetworkId:
            {
            retVal = static_cast<CCDIAPRecord*>(
                    aClientPluginInstance->iGenRecordArray[KIapRecordIndex] )->iNetwork;
            }
            break;
        case ECmProxyPortNumber:
            {
            if ( !aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )
                {
                User::Leave( KErrNotFound );
                }

            retVal = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->iPortNumber;
            }
            break;
        case ECmWapIPWSPOption:
            {
            retVal = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )->iWAPWSPOption;
            }
            break;
        case ECmWapIPProxyPort:
            {
            retVal = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )->iWAPProxyPort;
            }
            break;
        default:
            {
            retVal = GetBearerIntAttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETINTATTRIBUTEL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmPluginBaseEng::GetBoolAttributeL
// -----------------------------------------------------------------------------
EXPORT_C TBool CCmPluginBaseEng::GetBoolAttributeL(
        TUint32 aAttribute,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETBOOLATTRIBUTEL_ENTRY );

    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case ECmProtected:
            {
            if ( iBearerType == KUidEmbeddedDestination )
                {
                retVal = GetBearerBoolAttributeL(
                        aAttribute,
                        aClientPluginInstance->iGenRecordArray,
                        aClientPluginInstance->iBearerSpecRecordArray );
                }
            else
                {
                CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                        aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );
                retVal = iapRecord->Attributes() & ECDProtectedWrite;
                }
            }
            break;
        case ECmHidden:
            {
            if ( iBearerType == KUidEmbeddedDestination )
                {
                retVal = GetBearerBoolAttributeL(
                        aAttribute,
                        aClientPluginInstance->iGenRecordArray,
                        aClientPluginInstance->iBearerSpecRecordArray );
                }
            else
                {
                CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                        aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );
                retVal = iapRecord->Attributes() & ECDHidden;
                }
            }
            break;
        case ECmProxyUsageEnabled:
            {
            if ( !aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )
                {
                retVal = EFalse;
                }
            else
                {
                retVal = static_cast<CCDProxiesRecord*>(
                        aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )
                        ->iUseProxyServer;
                }
            }
            break;
        case ECmDestination:
            {
            TRAPD( err, retVal = GetBearerBoolAttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray ) );
            if ( err )
                {
                retVal = EFalse;
                }
            }
            break;
        case ECmChargeCardUsageEnabled:
            {
            retVal = EFalse;
            }
            break;
        case ECmLocationUsageEnabled:
            {
            retVal = iLocationEnabled;
            }
            break;
        case ECmCoverage:
            // By default the plugin has no network coverage.
            {
            TRAPD( err, retVal = GetBearerBoolAttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray ) );
            if ( err )
                {
                if ( err == KErrNotFound )
                    {
                    retVal = EFalse;
                    }
                else
                    {
                    User::Leave( err );
                    }
                }
            }
            break;
        case ECmMetaHighlight:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            retVal = metadataRecord->iMetadata & EMetaHighlight;
            }
            break;
        case ECmMetaHiddenAgent:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            retVal = metadataRecord->iMetadata & EMetaHiddenAgent;
            }
            break;
        case ECmMetaHotSpot:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            retVal = metadataRecord->iMetadata & EMetaHotSpot;
            }
            break;
        case ECmVirtual:
            {
            // This is bearer specific attribute.
            TRAPD( err, retVal = GetBearerInfoBoolL( aAttribute ) );
            if ( err )
                {
                retVal = EFalse;
                }
            }
            break;
        case ECmWapIPSecurity:
            {
            retVal = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )->iWAPSecurity;
            }
            break;
        default:
            {
            retVal = GetBearerBoolAttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }
    
    if( retVal )
        {
        retVal = ETrue;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::GetStringAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCmPluginBaseEng::GetStringAttributeL(
        TUint32 aAttribute,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETSTRINGATTRIBUTEL_ENTRY );

    if ( !aClientPluginInstance )
        {
        User::Leave( KErrCorrupt );
        }

    HBufC* retVal = NULL;

    switch ( aAttribute )
        {
        case ECmBearerIcon:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );

            if ( !metadataRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( metadataRecord->iIconFileName ).AllocL();
            }
            break;
        case ECmStartPage:
            {
            CCDWAPAccessPointRecord* wapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPAPRecordIndex] );

            if ( !wapAPRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( wapAPRecord->iWAPStartPage ).AllocL();
            }
            break;
        case ECmName:
            {
            CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                    aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

            if ( !iapRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( iapRecord->iRecordName ).AllocL();
            }
            break;
        case ECmProxyServerName:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );

            if ( !proxyRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( proxyRecord->iServerName ).AllocL();
            }
            break;
        case ECmProxyProtocolName:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );

            if ( !proxyRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( proxyRecord->iExceptions ).AllocL();
            }
            break;
        case ECmProxyExceptions:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );

            if ( !proxyRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( proxyRecord->iProtocolName ).AllocL();
            }
            break;
        case ECmWapIPGatewayAddress:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( wapIPBearerRecord->iWAPGatewayAddress ).AllocL();
            }
            break;
        case ECmWapIPProxyLoginName:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( wapIPBearerRecord->iWAPProxyLoginName ).AllocL();
            }
            break;
        case ECmWapIPProxyLoginPass:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }

            retVal = TPtrC( wapIPBearerRecord->iWAPProxyLoginPass ).AllocL();
            }
            break;
        default:
            {
            retVal = GetBearerStringAttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETSTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::GetString8AttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CCmPluginBaseEng::GetString8AttributeL(
        TUint32 aAttribute,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_GETSTRING8ATTRIBUTEL_ENTRY );

    HBufC8* retVal = NULL;

    switch ( aAttribute )
        {
        default:
            {
            retVal = GetBearerString8AttributeL(
                    aAttribute,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_GETSTRING8ATTRIBUTEL_EXIT );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::GetIntAttributeL
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::SetIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETINTATTRIBUTEL_ENTRY );

    switch ( aAttribute )
        {
        case ECmNamingMethod:
            {
            aClientPluginInstance->iNamingMethod = ( TNamingMethod )aValue;
            }
            break;

        case ECmCommsDBBearerType:
        case ECmElementID:
        case ECmIapId:
        case ECmId:
            {
            User::Leave( KErrNotSupported );
            }
            break;

        case ECmSeamlessnessLevel:
            {
            if ( aValue == ESeamlessnessConfirmFirst || aValue == ESeamlessnessShowprogress )
                {
                static_cast<CCDIAPMetadataRecord*>(
                        aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] )->
                        iSeamlessness.SetL( aValue );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
            break;

        case ECmNetworkId:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        case ECmProxyPortNumber:
            {
            if ( !aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )
                {
                CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
                CleanupStack::PushL( proxyRecord );
                CopyRecordFieldsL( *iProxyRecord, *proxyRecord );
                proxyRecord->SetElementId( iProxyRecord->ElementId() );
                aClientPluginInstance->iGenRecordArray.InsertL(
                        static_cast<CommsDat::CCDRecordBase*>( proxyRecord ),
                        KProxyRecordIndex );
                CleanupStack::Pop( proxyRecord );
                }

            static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->
                    iPortNumber = aValue;
            }
            break;
        case ECmWapIPWSPOption:
            {
            if ( aValue > KMaxTUint8 )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                static_cast<CCDWAPIPBearerRecord*>(
                        aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )->
                        iWAPWSPOption.SetL( aValue );
                }
            }
            break;
        case ECmWapIPProxyPort:
            {
            static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )->
                    iWAPProxyPort.SetL( aValue );
            }
            break;
        default:
            {
            SetBearerIntAttributeL(
                    aAttribute,
                    aValue,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETINTATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::SetBoolAttributeL
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::SetBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETBOOLATTRIBUTEL_ENTRY );

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

    switch ( aAttribute )
        {
        case ECmProtected:
            {
            /**
             * ECDProtectedWrite attribute is set in
             * - iap record
             * - network record
             * - iapmetadata record
             * - wap records
             * - proxy record
             * And depending the bearer
             * - service record
             * - bearer record
             * - other bearer specific records
             */
            SetAttribute( iapRecord, ECDProtectedWrite, aValue );
            }
            break;
        case ECmHidden:
            {
            /**
             * ECDHidden attribute is set in
             * - iap record
             * - network record
             * - iapmetadata record
             * - wap records
             * - proxy record
             * And depending the bearer
             * - service record
             * - bearer record
             * - other bearer specific records
             */
            SetAttribute( iapRecord, ECDHidden, aValue );
            }
            break;
        case ECmProxyUsageEnabled:
            {
            if ( !aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )
                {
                CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );
                CleanupStack::PushL( proxyRecord );
                CopyRecordFieldsL( *iProxyRecord, *proxyRecord );
                proxyRecord->SetElementId( iProxyRecord->ElementId() );
                aClientPluginInstance->iGenRecordArray.InsertL(
                        static_cast<CommsDat::CCDRecordBase*>( proxyRecord ),
                        KProxyRecordIndex );
                CleanupStack::Pop( proxyRecord );
                }

            static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->iUseProxyServer =
                            aValue;
            }
            break;
        case ECmChargeCardUsageEnabled:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        case ECmLocationUsageEnabled:
            {
            EnableLocationL( aValue );
            }
            break;
        case ECmMetaHighlight:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            if ( aValue )
                {
                metadataRecord->iMetadata = EMetaHighlight | metadataRecord->iMetadata;
                }
            else
                {
                metadataRecord->iMetadata = ~EMetaHighlight & metadataRecord->iMetadata;
                }
            }
            break;
        case ECmMetaHiddenAgent:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            if ( aValue )
                {
                metadataRecord->iMetadata = EMetaHiddenAgent | metadataRecord->iMetadata;
                }
            else
                {
                metadataRecord->iMetadata = ~EMetaHiddenAgent & metadataRecord->iMetadata;
                }
            }
            break;
        case ECmMetaHotSpot:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );
            if ( aValue )
                {
                metadataRecord->iMetadata = EMetaHotSpot | metadataRecord->iMetadata;
                }
            else
                {
                metadataRecord->iMetadata = ~EMetaHotSpot & metadataRecord->iMetadata;
                }
            }
            break;
        case ECmWapIPSecurity:
            {
            static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] )
                    ->iWAPSecurity.SetL( aValue );
            }
            break;
        default:
            {
            SetBearerBoolAttributeL(
                    aAttribute,
                    aValue,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETBOOLATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::SetStringAttributeL
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::SetStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETSTRINGATTRIBUTEL_ENTRY );

    switch ( aAttribute )
        {
        case ECmBearerIcon:
            {
            CCDIAPMetadataRecord* metadataRecord = static_cast<CCDIAPMetadataRecord*>(
                    aClientPluginInstance->iGenRecordArray[KMetaDataRecordIndex] );

            if ( !metadataRecord )
                {
                User::Leave( KErrCorrupt );
                }
            metadataRecord->iIconFileName.SetL( aValue );
            }
            break;
        case ECmStartPage:
            {
            CCDWAPAccessPointRecord* wapAPRecord = static_cast<CCDWAPAccessPointRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPAPRecordIndex] );

            if ( !wapAPRecord )
                {
                User::Leave( KErrCorrupt );
                }
            wapAPRecord->iWAPStartPage.SetL( aValue );
            }
            break;
        case ECmName:
            {
            CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
                    aClientPluginInstance->iGenRecordArray[KIapRecordIndex] );

            if ( !iapRecord )
                {
                User::Leave( KErrCorrupt );
                }
            SetNameL( aValue, iapRecord, aClientPluginInstance->iNamingMethod );
            }
            break;
        case ECmProxyServerName:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );
            if ( !proxyRecord )
                {
                proxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );

                // Map proxy record to this iap.
                proxyRecord->iService = static_cast<CCDProxiesRecord*>(
                        aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->RecordId();

                aClientPluginInstance->iGenRecordArray.Insert( proxyRecord, KProxyRecordIndex );
                }

            SetProxyServerNameL( aValue, proxyRecord );
            }
            break;
        case ECmProxyProtocolName:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );
            if ( !proxyRecord )
                {
                proxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );

                // Map proxy record to this iap.
                proxyRecord->iService = static_cast<CCDProxiesRecord*>(
                        aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->RecordId();

                aClientPluginInstance->iGenRecordArray.Insert( proxyRecord, KProxyRecordIndex );
                }

            proxyRecord->iProtocolName.SetL( aValue );
            }
            break;
        case ECmProxyExceptions:
            {
            CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord*>(
                    aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] );
            if ( !proxyRecord )
                {
                proxyRecord = static_cast<CCDProxiesRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdProxiesRecord ) );

                // Map proxy record to this iap.
                proxyRecord->iService = static_cast<CCDProxiesRecord*>(
                        aClientPluginInstance->iGenRecordArray[KProxyRecordIndex] )->RecordId();

                aClientPluginInstance->iGenRecordArray.Insert( proxyRecord, KProxyRecordIndex );
                }

            proxyRecord->iExceptions.SetL( aValue );
            }
            break;
        case ECmWapIPGatewayAddress:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }
            wapIPBearerRecord->iWAPGatewayAddress.SetL( aValue );
            }
            break;
        case ECmWapIPProxyLoginName:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }
            wapIPBearerRecord->iWAPProxyLoginName.SetL( aValue );
            }
            break;
        case ECmWapIPProxyLoginPass:
            {
            CCDWAPIPBearerRecord* wapIPBearerRecord = static_cast<CCDWAPIPBearerRecord*>(
                    aClientPluginInstance->iGenRecordArray[KWAPBearerRecordIndex] );

            if ( !wapIPBearerRecord )
                {
                User::Leave( KErrCorrupt );
                }
            wapIPBearerRecord->iWAPProxyLoginPass.SetL( aValue );
            }
            break;
        default:
            {
            SetBearerStringAttributeL(
                    aAttribute,
                    aValue,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETSTRINGATTRIBUTEL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::SetString8AttributeL
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::SetString8AttributeL(
        TUint32 aAttribute,
        const TDesC8& aValue,
        CCmClientPluginInstance* aClientPluginInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINBASEENG_SETSTRING8ATTRIBUTEL_ENTRY );

    switch ( aAttribute )
        {
        default:
            {
            SetBearerString8AttributeL(
                    aAttribute,
                    aValue,
                    aClientPluginInstance->iGenRecordArray,
                    aClientPluginInstance->iBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINBASEENG_SETSTRING8ATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IsUnspecifiedIPv4Address
// ---------------------------------------------------------------------------
//
TBool CCmPluginBaseEng::IsUnspecifiedIPv4Address( const TDesC& aIpv4Address )
    {
    OstTraceFunctionEntry0( _ISUNSPECIFIEDIPV4ADDRESS_ENTRY );

    if ( aIpv4Address.Length() == 0 || !aIpv4Address.CompareF( KUnspecifiedIPv4 ) )
        {
        OstTraceFunctionExit0( _ISUNSPECIFIEDIPV4ADDRESS_EXIT );
        return ETrue;
        }

    OstTraceFunctionExit0( DUP1__ISUNSPECIFIEDIPV4ADDRESS_EXIT );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ClassifyIPv6Address
// ---------------------------------------------------------------------------
//
TIPv6Types CCmPluginBaseEng::ClassifyIPv6Address( const TDesC& aIpv6Address )
    {
    OstTraceFunctionEntry0( _CLASSIFYIPV6ADDRESS_ENTRY );

    if ( aIpv6Address.Length() == 0 || !aIpv6Address.CompareF( KDynamicIpv6Address ) )
        {
        OstTraceFunctionExit0( _CLASSIFYIPV6ADDRESS_EXIT );

        return EIPv6Unspecified;
        }
    else if ( !aIpv6Address.CompareF( KKnownIp6NameServer1 ) ||
            !aIpv6Address.CompareF( KKnownIp6NameServer2 ) )
        {
        OstTraceFunctionExit0( DUP1__CLASSIFYIPV6ADDRESS_EXIT );

        return EIPv6WellKnown;
        }

    OstTraceFunctionExit0( DUP2__CLASSIFYIPV6ADDRESS_EXIT );
    return EIPv6UserDefined;
    }

// End of file
