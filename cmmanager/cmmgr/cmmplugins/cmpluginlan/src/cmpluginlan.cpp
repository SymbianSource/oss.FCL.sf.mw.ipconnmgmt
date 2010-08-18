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
*      Implementation of Lan Plugin interface implementation
*      for Lan Bearer
*
*
*/


#include "cmpluginlan.h"

#include "datamobilitycommsdattypes.h"
#include <cmmanagerdef.h>
#include <ecom/ecom.h>  // For REComSession
#include <featmgr.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmpluginlanTraces.h"
#endif


using namespace CMManager;
using namespace CommsDat;

_LIT( KEmpty, "" );

_LIT( KLanBearerName, "LANBearer" );
_LIT( KLanBearerAgent, "nullagt.agt" );
_LIT( KLanBearerNif, "ethint" );

_LIT( KLanLDDFileName, "enet" );
_LIT( KLanPDDFileName, "ethernet" );

_LIT( KLanLDDName, "Ethernet" );
_LIT( KLanPDDName, "Ethernet.Wins" );
_LIT( KLanPacketDriverName, "EtherPkt.drv" );


const TBool KDefIpAddrFromServer = ETrue;
const TBool KDefIpDnsAddrFromServer = ETrue;
const TBool KDefIp6DnsAddrFromServer = ETrue;

const TInt KLanLastSocketActivityTimeout = -1;
const TInt KLanLastSocketClosedTimeout = -1;
const TInt KLanLastSessionClosedTimeout = -1;


// ---------------------------------------------------------
// CCmPluginLan::NewOutgoingL()
// ---------------------------------------------------------
//
CCmPluginLan* CCmPluginLan::NewL( TCmPluginInitParam* aInitParam )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_NEWL_ENTRY );

    CCmPluginLan* self = new( ELeave ) CCmPluginLan( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINLAN_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCmPluginLan::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginLan::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_CREATEINSTANCEL_ENTRY );

    CCmPluginLan* self = new( ELeave ) CCmPluginLan( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINLAN_CREATEINSTANCEL_EXIT );
    return self;
    }

// ---------------------------------------------------------
// CCmPluginLan::~CCmPluginLan()
// ---------------------------------------------------------
//
CCmPluginLan::~CCmPluginLan()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_CCMPLUGINLAN_ENTRY );

    ResetBearerRecords();

    OstTraceFunctionExit0( CCMPLUGINLAN_CCMPLUGINLAN_EXIT );
    }

// ---------------------------------------------------------
// CCmPluginLan::CCmPluginLan()
// ---------------------------------------------------------
//
CCmPluginLan::CCmPluginLan( TCmPluginInitParam* aInitParam )
        :
        CCmPluginBaseEng( aInitParam )
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINLAN_CCMPLUGINLAN_ENTRY );

    iBearerType = KUidLanBearerType;
    iBearerRecord = NULL;
    iBearerPriorityTableId = 0;

    OstTraceFunctionExit0( DUP1_CCMPLUGINLAN_CCMPLUGINLAN_EXIT );
    }

// ---------------------------------------------------------
// CCmPluginLan::ConstructL()
// ---------------------------------------------------------
//
void CCmPluginLan::ConstructL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_CONSTRUCTL_ENTRY );

    // Feature Manager is initialized by the CmmServer, so no need to do it here.

    CCmPluginBaseEng::ConstructL();
    
    // Get bearer priority table ID.
    TRAP_IGNORE( iBearerPriorityTableId = 
            CCDGlobalBearerTypePriorizationRecord::TableIdL( iSession ) );

    if ( !iBearerPriorityTableId )
        {
        iBearerPriorityTableId = 
                CCDGlobalBearerTypePriorizationRecord::CreateTableL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------
// CCmPluginLan::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginLan::CanHandleIapIdL( TUint32 aIapId ) const
    {
    OstTraceFunctionEntryExt( CCMPLUGINLAN_CANHANDLEIAPIDL_ENTRY, this );

    TBool retVal( EFalse );

    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                            ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );

    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );

    TRAPD( err, iapRecord->LoadL( iSession ) );

    if ( !err )
        {
        retVal = CanHandleIapIdL( iapRecord );
        }

    CleanupStack::PopAndDestroy( iapRecord );

    OstTraceFunctionExitExt( CCMPLUGINLAN_CANHANDLEIAPIDL_EXIT, this, retVal );

    return retVal;
    }

// ---------------------------------------------------------
// CCmPluginLan::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginLan::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    OstTraceFunctionEntryExt( DUP1_CCMPLUGINLAN_CANHANDLEIAPIDL_ENTRY, this );

    TBool retVal( EFalse );

    if ( (TPtrC( aIapRecord->iServiceType ) == TPtrC( KCDTypeNameLANService ) ) &&
        TPtrC( aIapRecord->iBearerType ) == TPtrC( KCDTypeNameLANBearer ) )
        {
        retVal = ETrue;
        }

    OstTraceFunctionExitExt( DUP1_CCMPLUGINLAN_CANHANDLEIAPIDL_EXIT, this, retVal );

    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginLan::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginLan::PreparePluginToLoadRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_PREPAREPLUGINTOLOADRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_PREPAREPLUGINTOLOADRECORDSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginLan::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginLan::PreparePluginToUpdateRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_PREPAREPLUGINTOUPDATERECORDSL_ENTRY );

    CCDIAPRecord* iapRecord =
                static_cast<CCDIAPRecord*>( aGenRecordArray[KIapRecordIndex] );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    CheckIfNameModifiedL( iapRecord, serviceRecord );

    CheckDNSServerAddressL( ETrue,
                            serviceRecord->iIp6NameServer1,
                            serviceRecord->iIp6NameServer2,
                            serviceRecord->iIp6DnsAddrFromServer );

    CheckDNSServerAddressL( EFalse,
                            serviceRecord->iIpNameServer1,
                            serviceRecord->iIpNameServer2,
                            serviceRecord->iIpDnsAddrFromServer );

    SetDaemonNameL( aGenRecordArray, aBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINLAN_PREPAREPLUGINTOUPDATERECORDSL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::CreateServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginLan::CreateServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_CREATESERVICERECORDL_ENTRY );

    delete iServiceRecord;
    iServiceRecord = NULL;

    iServiceRecord = static_cast<CCDLANServiceRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

    CCDLANServiceRecord* lanServiceRecord = static_cast<CCDLANServiceRecord*>( iServiceRecord );

    if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
        {
        lanServiceRecord->iIfNetworks.SetL( KDefIspIfNetworksIPv4IPv6LAN );
        }
    else
        {
        lanServiceRecord->iIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        }

    lanServiceRecord->iIpGateway.SetL( KUnspecifiedIPv4 );
    lanServiceRecord->iIpAddrFromServer.SetL( KDefIpAddrFromServer );

    lanServiceRecord->iIpAddr.SetL( KUnspecifiedIPv4 );

    lanServiceRecord->iIpDnsAddrFromServer.SetL( KDefIpDnsAddrFromServer );

    lanServiceRecord->iIpNameServer1.SetL( KUnspecifiedIPv4 );
    lanServiceRecord->iIpNameServer2.SetL( KUnspecifiedIPv4 );
    lanServiceRecord->iIp6DnsAddrFromServer.SetL( KDefIp6DnsAddrFromServer );
    lanServiceRecord->iIp6NameServer1.SetL( KDynamicIpv6Address );
    lanServiceRecord->iIp6NameServer2.SetL( KDynamicIpv6Address );

    if ( KDefIpAddrFromServer )
        {
        lanServiceRecord->iConfigDaemonManagerName.SetL( KDaemonManagerName );
        lanServiceRecord->iConfigDaemonName.SetL( KConfigDaemonName );
        }
    else
        {
        lanServiceRecord->iConfigDaemonManagerName.SetL( KEmpty );
        lanServiceRecord->iConfigDaemonName.SetL( KEmpty );
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_CREATESERVICERECORDL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::LoadServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginLan::LoadServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_LOADSERVICERECORDL_ENTRY );

    if ( TPtrC( KCDTypeNameLANService ) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDLANServiceRecord *>(
                CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

        ServiceRecord().SetRecordId( iIapRecord->iService );
        ServiceRecord().LoadL( iSession );
        }
    else
        {
        // This IAP service is not supported by this plugin.
        User::Leave( KErrNotSupported );
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_LOADSERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginLan::PrepareToCopyDataL( CCmPluginBaseEng* /*aCopyInstance*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_PREPARETOCOPYDATAL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_PREPARETOCOPYDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::CopyServiceRecordL
// ---------------------------------------------------------------------------
//
CommsDat::CCDRecordBase* CCmPluginLan::CopyServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_COPYSERVICERECORDL_ENTRY );

    // New service record to be returned.
    CCDRecordBase* serviceRecord = static_cast<CCDLANServiceRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );
    CleanupStack::PushL( serviceRecord );

    CCDLANServiceRecord* tempServiceRecordPtrToNew =
            static_cast<CCDLANServiceRecord*>( serviceRecord );

    // CommsDat version of service record.
    CCDLANServiceRecord* origServiceRecord =
            static_cast<CCDLANServiceRecord*>( iServiceRecord );

    if ( !origServiceRecord->iRecordTag.IsNull() )
        {
        tempServiceRecordPtrToNew->iRecordTag.SetL(
                origServiceRecord->iRecordTag );
        }
    if ( !origServiceRecord->iRecordName.IsNull() )
        {
        tempServiceRecordPtrToNew->iRecordName.SetL(
                origServiceRecord->iRecordName );
        }
    if ( !origServiceRecord->iServiceEnableLlmnr.IsNull() )
        {
        tempServiceRecordPtrToNew->iServiceEnableLlmnr.SetL(
                origServiceRecord->iServiceEnableLlmnr );
        }
    if ( !origServiceRecord->iIfNetworks.IsNull() )
        {
        tempServiceRecordPtrToNew->iIfNetworks.SetL(
                origServiceRecord->iIfNetworks );
        }
    if ( !origServiceRecord->iIpNetmask.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpNetmask.SetL(
                origServiceRecord->iIpNetmask );
        }
    if ( !origServiceRecord->iIpGateway.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpGateway.SetL(
                origServiceRecord->iIpGateway );
        }
    if ( !origServiceRecord->iIpAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpAddrFromServer.SetL(
                origServiceRecord->iIpAddrFromServer );
        }
    if ( !origServiceRecord->iIpAddr.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpAddr.SetL(
                origServiceRecord->iIpAddr );
        }
    if ( !origServiceRecord->iIpDnsAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpDnsAddrFromServer.SetL(
                origServiceRecord->iIpDnsAddrFromServer );
        }
    if ( !origServiceRecord->iIpNameServer1.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpNameServer1.SetL(
                origServiceRecord->iIpNameServer1 );
        }
    if ( !origServiceRecord->iIpNameServer2.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpNameServer2.SetL(
                origServiceRecord->iIpNameServer2 );
        }
    if ( !origServiceRecord->iIp6DnsAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iIp6DnsAddrFromServer.SetL(
                origServiceRecord->iIp6DnsAddrFromServer );
        }
    if ( !origServiceRecord->iIp6NameServer1.IsNull() )
        {
        tempServiceRecordPtrToNew->iIp6NameServer1.SetL(
                origServiceRecord->iIp6NameServer1 );
        }
    if ( !origServiceRecord->iIp6NameServer2.IsNull() )
        {
        tempServiceRecordPtrToNew->iIp6NameServer2.SetL(
                origServiceRecord->iIp6NameServer2 );
        }
    if ( !origServiceRecord->iIpAddrLeaseValidFrom.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpAddrLeaseValidFrom.SetL(
                origServiceRecord->iIpAddrLeaseValidFrom );
        }
    if ( !origServiceRecord->iIpAddrLeaseValidTo.IsNull() )
        {
        tempServiceRecordPtrToNew->iIpAddrLeaseValidTo.SetL(
                origServiceRecord->iIpAddrLeaseValidTo );
        }
    if ( !origServiceRecord->iConfigDaemonManagerName.IsNull() )
        {
        tempServiceRecordPtrToNew->iConfigDaemonManagerName.SetL(
                origServiceRecord->iConfigDaemonManagerName );
        }
    if ( !origServiceRecord->iConfigDaemonName.IsNull() )
        {
        tempServiceRecordPtrToNew->iConfigDaemonName.SetL(
                origServiceRecord->iConfigDaemonName );
        }
    if ( !origServiceRecord->iServiceExtensionTableName.IsNull() )
        {
        tempServiceRecordPtrToNew->iServiceExtensionTableName.SetL(
                origServiceRecord->iServiceExtensionTableName );
        }
    if ( !origServiceRecord->iServiceExtensionTableRecordId.IsNull() )
        {
        tempServiceRecordPtrToNew->iServiceExtensionTableRecordId.SetL(
                origServiceRecord->iServiceExtensionTableRecordId );
        }

    CleanupStack::Pop( serviceRecord );

    OstTraceFunctionExit0( CCMPLUGINLAN_COPYSERVICERECORDL_EXIT );
    return serviceRecord;
    }

// ----------------------------------------------------------------------------
// CCmPluginLan::UpdateServiceRecordL()
// ----------------------------------------------------------------------------
//
void CCmPluginLan::UpdateServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_UPDATESERVICERECORDL_ENTRY );

    // Delete the original record and create a copy from the client's copy.
    delete iServiceRecord;
    iServiceRecord = NULL;

    iServiceRecord = static_cast<CCDLANServiceRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

    CCDLANServiceRecord* origServiceRecord = static_cast<CCDLANServiceRecord*>( iServiceRecord );

    // Client's copy of lan service record.
    CCDLANServiceRecord* clientServiceRecordCopy =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    CopyRecordFieldsL( *clientServiceRecordCopy, *origServiceRecord );

    origServiceRecord->SetElementId( clientServiceRecordCopy->ElementId() );

    if ( !origServiceRecord->RecordId() )
        {
        origServiceRecord->SetRecordId( KCDNewRecordRequest );
        origServiceRecord->StoreL( iSession );

        // Have to be "reloaded" to get possible default values from template records.
        origServiceRecord->LoadL( iSession );

        // Update received element ID to client's copy too.
        clientServiceRecordCopy->SetElementId( origServiceRecord->ElementId() );
        }
    else
        {
        origServiceRecord->ModifyL( iSession );
        }

    CCmPluginBaseEng::CopyRecordFieldsL( *origServiceRecord,
            *clientServiceRecordCopy );

    OstTraceFunctionExit0( CCMPLUGINLAN_UPDATESERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::CreateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginLan::CreateBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_CREATEBEARERRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_CREATEBEARERRECORDSL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::LoadBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginLan::LoadBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_LOADBEARERRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_LOADBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::ServiceRecord()
// ---------------------------------------------------------------------------
//
CCDLANServiceRecord& CCmPluginLan::ServiceRecord() const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SERVICERECORD_ENTRY );

    return *static_cast<CCDLANServiceRecord*>( iServiceRecord );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginLan::ServiceRecordId() const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SERVICERECORDID_ENTRY );

    return ServiceRecord().RecordId();
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::ServiceRecordNameLC
// ---------------------------------------------------------------------------
//
void CCmPluginLan::ServiceRecordNameLC( HBufC* &aName )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SERVICERECORDNAMELC_ENTRY );

    aName = TPtrC( KCDTypeNameLANService ).AllocLC();

    OstTraceFunctionExit0( CCMPLUGINLAN_SERVICERECORDNAMELC_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::BearerRecordIdL()
// ---------------------------------------------------------------------------
//
void CCmPluginLan::BearerRecordIdL( TUint32& aRecordId )
    {
    CCDLANBearerRecord* lanBearerRecord = static_cast<CCDLANBearerRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ) );
    CleanupStack::PushL( lanBearerRecord );

    lanBearerRecord->iRecordName.SetL( TPtrC( KLanBearerName ) );
    if ( !lanBearerRecord->FindL( iSession ) )
        { // Bearer not found -> create dummy values.
        lanBearerRecord->SetRecordId( KCDNewRecordRequest );
        lanBearerRecord->iBearerAgent = KLanBearerAgent;
        lanBearerRecord->iRecordName = KLanBearerName;
        lanBearerRecord->iLanBearerNifName = KLanBearerNif;

        lanBearerRecord->iLanBearerLddFilename = KLanLDDFileName;
        lanBearerRecord->iLanBearerLddName = KLanLDDName;
        lanBearerRecord->iLanBearerPddFilename = KLanPDDFileName;
        lanBearerRecord->iLanBearerPddName = KLanPDDName;
        lanBearerRecord->iLanBearerPacketDriverName = KLanPacketDriverName;
        lanBearerRecord->iLastSocketActivityTimeout = (TUint32)KLanLastSocketActivityTimeout;
        lanBearerRecord->iLastSessionClosedTimeout = (TUint32)KLanLastSessionClosedTimeout;
        lanBearerRecord->iLastSocketClosedTimeout = (TUint32)KLanLastSocketClosedTimeout;

        lanBearerRecord->StoreL( iSession );
        }

    aRecordId = lanBearerRecord->RecordId();

    CleanupStack::PopAndDestroy( lanBearerRecord );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::BearerRecordNameLC()
// ---------------------------------------------------------------------------
//
void CCmPluginLan::BearerRecordNameLC( HBufC*& aBearerName )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_BEARERRECORDNAMELC_ENTRY );

    aBearerName = TPtrC( KCDTypeNameLANBearer ).AllocLC();

    OstTraceFunctionExit0( CCMPLUGINLAN_BEARERRECORDNAMELC_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerSpecificRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginLan::GetBearerSpecificRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERSPECIFICRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERSPECIFICRECORDSL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::DeleteBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginLan::DeleteBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_DELETEBEARERRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_DELETEBEARERRECORDSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginLan::UpdateBearerRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginLan::UpdateBearerRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_UPDATEBEARERRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_UPDATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------
// CCmPluginLan::ResetBearerRecords()
// ---------------------------------------------------------
//
void CCmPluginLan::ResetBearerRecords()
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_RESETBEARERRECORDS_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_RESETBEARERRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::SetDaemonNameL
// ---------------------------------------------------------------------------
//
void CCmPluginLan::SetDaemonNameL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SETDAEMONNAMEL_ENTRY );

    // We have to check first that these values have not been set (or changed)
    // by any 3rd party SW to ensure that we don't break their settings.
    HBufC* daemonName = GetBearerStringAttributeL(
            ECmConfigDaemonManagerName,
            aGenRecordArray,
            aBearerSpecRecordArray );
    if ( daemonName )
        {
        if ( daemonName->Compare( KDaemonManagerName ) != 0 )
            {
            delete daemonName;
            OstTraceFunctionExit1( CCMPLUGINWLAN_SETDAEMONNAMEL_EXIT, this );
            OstTraceFunctionExit0( CCMPLUGINLAN_SETDAEMONNAMEL_EXIT );
            return;
            }
        delete daemonName;
        }

    // Use DHCP if we can.
    TBool ipfromSrv = GetBearerBoolAttributeL(
            ECmIPAddFromServer,
            aGenRecordArray,
            aBearerSpecRecordArray );
    if ( ipfromSrv )
        {
        SetBearerStringAttributeL(
                ECmConfigDaemonManagerName,
                KDaemonManagerName,
                aGenRecordArray,
                aBearerSpecRecordArray );
        SetBearerStringAttributeL(ECmConfigDaemonName,
                KConfigDaemonName,
                aGenRecordArray,
                aBearerSpecRecordArray );
        }
    else
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
            {
            SetBearerStringAttributeL(
                    ECmConfigDaemonManagerName,
                    KDaemonManagerName,
                    aGenRecordArray,
                    aBearerSpecRecordArray );
            SetBearerStringAttributeL(
                    ECmConfigDaemonName,
                    KConfigDaemonName,
                    aGenRecordArray,
                    aBearerSpecRecordArray );
            }
        else
            {
            SetBearerStringAttributeL(
                    ECmConfigDaemonManagerName,
                    KNullDesC(),
                    aGenRecordArray,
                    aBearerSpecRecordArray );
            SetBearerStringAttributeL(
                    ECmConfigDaemonName,
                    KNullDesC(),
                    aGenRecordArray,
                    aBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( DUP1_CCMPLUGINLAN_SETDAEMONNAMEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::GetBearerIntAttributeL
// --------------------------------------------------------------------------
//
TUint32 CCmPluginLan::GetBearerIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmInvalidAttribute:
            {
            // This attribute has been deprecated since Symbian^4.
            User::Leave( KErrNotSupported ); //TODO, update attribute definition comments.
            }
            break;
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        case ECmCommsDBBearerType:
            {
            retVal = KCommDbBearerLAN;
            }
            break;
        case ECmDefaultUiPriority:
            {
            retVal = GetDefPriorityL( aAttribute );
            }
            break;
        case ECmDefaultPriority:
            {
            retVal = GetDefPriorityL( aAttribute );
            }
            break;
        case ELanServiceExtensionTableRecordId:
            {
            if ( !serviceRecord->iServiceExtensionTableRecordId.IsNull() )
                {
                retVal = serviceRecord->iServiceExtensionTableRecordId.RecordId();
                }
            else
                {
                retVal = 0;
                }
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERINTATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginLan::GetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
TBool CCmPluginLan::GetBearerBoolAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERBOOLATTRIBUTEL_ENTRY );

    TBool retVal( 0 );

    CCDLANServiceRecord* serviceRecord =
                static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
            retVal = ETrue;
            }
            break;
        case ECmDestination:
            {
            retVal = EFalse;
            }
            break;
        case ECmBearerHasUi:
            {
            retVal = EFalse;
            }
            break;
        case ECmIPv6Supported:
            {
            retVal = FeatureManager::FeatureSupported( KFeatureIdIPv6 );
            }
            break;
        case ECmIPAddFromServer:
        case ELanIpAddrFromServer:
            {
            retVal = serviceRecord->iIpAddrFromServer;
            }
            break;
        case ECmIPDNSAddrFromServer:
        case ELanIpDNSAddrFromServer:
            {
            retVal = serviceRecord->iIpDnsAddrFromServer;
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case ELanIp6DNSAddrFromServer:
            {
            retVal = serviceRecord->iIp6DnsAddrFromServer;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginLan::GetBearerStringAttributeL
// --------------------------------------------------------------------------
//
HBufC* CCmPluginLan::GetBearerStringAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERSTRINGATTRIBUTEL_ENTRY );

    HBufC* retVal = NULL;

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmIFNetworks:
        case ELanIfNetworks:
            {
            retVal = TPtrC( serviceRecord->iIfNetworks ).AllocL();
            }
            break;
        case ECmIPNetmask:
        case ELanIpNetMask:
            {
            retVal = TPtrC( serviceRecord->iIpNetmask ).AllocL();
            }
            break;
        case ECmIPGateway:
        case ELanIpGateway:
            {
            retVal = TPtrC( serviceRecord->iIpGateway ).AllocL();
            }
            break;
        case ECmIPAddress:
        case ELanIpAddr:
            {
            retVal = TPtrC( serviceRecord->iIpAddr ).AllocL();
            }
            break;
        case ECmIPNameServer1:
        case ELanIpNameServer1:
            {
            retVal = TPtrC( serviceRecord->iIpNameServer1 ).AllocL();
            }
            break;
        case ECmIPNameServer2:
        case ELanIpNameServer2:
            {
            retVal = TPtrC( serviceRecord->iIpNameServer2 ).AllocL();
            }
            break;
        case ECmIP6NameServer1:
        case ELanIp6NameServer1:
            {
            retVal = TPtrC( serviceRecord->iIp6NameServer1 ).AllocL();
            }
            break;
        case ECmIP6NameServer2:
        case ELanIp6NameServer2:
            {
            retVal = TPtrC( serviceRecord->iIp6NameServer2 ).AllocL();
            }
            break;
        case ECmIPAddrLeaseValidFrom:
        case ELanIpAddrLeaseValidFrom:
            {
            retVal = TPtrC( serviceRecord->iIpAddrLeaseValidFrom ).AllocL();
            }
            break;
        case ECmIPAddrLeaseValidTo:
        case ELanIpAddrLeaseValidTo:
            {
            retVal = TPtrC( serviceRecord->iIpAddrLeaseValidTo ).AllocL();
            }
            break;
        case ECmConfigDaemonManagerName:
        case ELanConfigDaemonManagerName:
            {
            retVal = TPtrC( serviceRecord->iConfigDaemonManagerName ).AllocL();
            }
            break;
        case ECmConfigDaemonName:
        case ELanConfigDaemonName:
            {
            retVal = TPtrC( serviceRecord->iConfigDaemonName ).AllocL();
            }
            break;
        case ELanServiceExtensionTableName:
            {
            retVal = TPtrC( serviceRecord->iServiceExtensionTableName ).AllocL();
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERSTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginLan::GetBearerString8AttributeL
// --------------------------------------------------------------------------
//
HBufC8* CCmPluginLan::GetBearerString8AttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERSTRING8ATTRIBUTEL_ENTRY );

    HBufC8* retVal( NULL );

    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERSTRING8ATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginLan::SetBearerIntAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginLan::SetBearerIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SETBEARERINTATTRIBUTEL_ENTRY );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmExtensionLevel:
        case ECmCommsDBBearerType:
        case ECmDefaultUiPriority:
        case ECmDefaultPriority:
            {
            User::Leave( KErrArgument );
            }
            break;
        case ELanServiceExtensionTableRecordId:
            {
            serviceRecord->iServiceExtensionTableRecordId = aValue;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_SETBEARERINTATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::SetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginLan::SetBearerBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SETBEARERBOOLATTRIBUTEL_ENTRY );

    CCDLANServiceRecord* serviceRecord =
                static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmIPAddFromServer:
        case ELanIpAddrFromServer:
            {
            serviceRecord->iIpAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIPDNSAddrFromServer:
        case ELanIpDNSAddrFromServer:
            {
            serviceRecord->iIpDnsAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case ELanIp6DNSAddrFromServer:
            {
            serviceRecord->iIp6DnsAddrFromServer.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_SETBEARERBOOLATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::SetBearerStringAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginLan::SetBearerStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SETBEARERSTRINGATTRIBUTEL_ENTRY );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmIFNetworks:
        case ELanIfNetworks:
            {
            serviceRecord->iIfNetworks.SetL( aValue );
            }
            break;
        case ECmIPNetmask:
        case ELanIpNetMask:
            {
            serviceRecord->iIpNetmask.SetL( aValue );
            }
            break;
        case ECmIPGateway:
        case ELanIpGateway:
            {
            serviceRecord->iIpGateway.SetL( aValue );
            }
            break;
        case ECmIPAddress:
        case ELanIpAddr:
            {
            if ( ( aValue.Compare( KUnspecifiedIPv4 ) == 0 ) ||
                    ( aValue.Compare( KNullDesC ) == 0 ) )
                {
                // If unspecified or NULL address is set dynamic
                // IP must be enabled.
                SetBearerBoolAttributeL( ECmIPAddFromServer,
                                         ETrue,
                                         aGenRecordArray,
                                         aBearerSpecRecordArray );
                }
            else
                {
                // If valid address is set dynamic IP is disabled.
                SetBearerBoolAttributeL( ECmIPAddFromServer,
                                         EFalse,
                                         aGenRecordArray,
                                         aBearerSpecRecordArray );
                }
            serviceRecord->iIpAddr.SetL( aValue );
            }
            break;
        case ECmIPNameServer1:
        case ELanIpNameServer1:
            {
            serviceRecord->iIpNameServer1.SetL( aValue );
            }
            break;
        case ECmIPNameServer2:
        case ELanIpNameServer2:
            {
            serviceRecord->iIpNameServer2.SetL( aValue );
            }
            break;
        case ECmIP6NameServer1:
        case ELanIp6NameServer1:
            {
            serviceRecord->iIp6NameServer1.SetL( aValue );
            }
            break;
        case ECmIP6NameServer2:
        case ELanIp6NameServer2:
            {
            serviceRecord->iIp6NameServer2.SetL( aValue );
            }
            break;
        case ECmIPAddrLeaseValidFrom:
        case ELanIpAddrLeaseValidFrom:
            {
            serviceRecord->iIpAddrLeaseValidFrom.SetL( aValue );
            }
            break;
        case ECmIPAddrLeaseValidTo:
        case ELanIpAddrLeaseValidTo:
            {
            serviceRecord->iIpAddrLeaseValidTo.SetL( aValue );
            }
            break;
        case ECmConfigDaemonManagerName:
        case ELanConfigDaemonManagerName:
            {
            serviceRecord->iConfigDaemonManagerName.SetL( aValue );
            }
            break;
        case ECmConfigDaemonName:
        case ELanConfigDaemonName:
            {
            serviceRecord->iConfigDaemonName.SetL( aValue );
            }
            break;
        case ELanServiceExtensionTableName:
            {
            serviceRecord->iServiceExtensionTableName.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_SETBEARERSTRINGATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginLan::SetBearerString8AttributeL
// --------------------------------------------------------------------------
//
void CCmPluginLan::SetBearerString8AttributeL(
        TUint32 aAttribute,
        const TDesC8& /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_SETBEARERSTRING8ATTRIBUTEL_ENTRY );

    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_SETBEARERSTRING8ATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginLan::GetBearerInfoIntL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERINFOINTL_ENTRY );

    TUint32 retVal( 0 );
    switch ( aAttribute )
        {
        case ECmBearerType:
            {
            retVal = iBearerType;
            }
            break;
        case ECmCommsDBBearerType:
            {
            retVal = KCommDbBearerLAN;
            }
            break;
        case ECmDefaultUiPriority:
            {
            retVal = GetDefPriorityL( aAttribute );
            }
            break;
        case ECmDefaultPriority:
            {
            retVal = GetDefPriorityL( aAttribute );
            }
            break;
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        case ECmSeamlessnessLevel:
            {
            retVal = ESeamlessnessShowprogress;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmPluginLan::GetBearerInfoBoolL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERINFOBOOLL_ENTRY );

    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
#ifndef __WINS__
            retVal = EFalse;
#else
            retVal = ETrue;
#endif
            }
            break;
        case ECmDestination:
            {
            retVal = EFalse;
            }
            break;
        case ECmBearerHasUi:
            {
            retVal = EFalse;
            }
            break;
        case ECmIPv6Supported:
            {
            retVal = FeatureManager::FeatureSupported( KFeatureIdIPv6 );
            }
            break;
        case ECmVirtual:
            {
            retVal = EFalse;
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }


// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginLan::GetBearerInfoStringL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERINFOSTRINGL_ENTRY );

    HBufC* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERINFOSTRINGL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerInfoString8L
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginLan::GetBearerInfoString8L( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERINFOSTRING8L_ENTRY );

    HBufC8* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERINFOSTRING8L_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::CopyBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginLan::CopyBearerRecordsL( CCmPluginBaseEng* /*aCopyInstance*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_COPYBEARERRECORDSL_ENTRY );
    OstTraceFunctionExit0( CCMPLUGINLAN_COPYBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetBearerTableIdsToBeObserved
// ---------------------------------------------------------------------------
//
void CCmPluginLan::GetBearerTableIdsToBeObservedL(
        RArray<TUint32>& aTableIdArray ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETBEARERTABLEIDSTOBEOBSERVED_ENTRY );

    aTableIdArray.AppendL( KCDTIdLANServiceRecord );

    OstTraceFunctionExit0( CCMPLUGINLAN_GETBEARERTABLEIDSTOBEOBSERVED_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginLan::GetDefPriorityL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginLan::GetDefPriorityL( const TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINLAN_GETDEFPRIORITYL_ENTRY );

    TUint32 retVal( KDataMobilitySelectionPolicyPriorityWildCard );

    CCDGlobalBearerTypePriorizationRecord* priorityRecord =
            new( ELeave ) CCDGlobalBearerTypePriorizationRecord( iBearerPriorityTableId );
    CleanupStack::PushL( priorityRecord );

    priorityRecord->iServiceType.SetL( TPtrC( KCDTypeNameLANService ) );

    if ( priorityRecord->FindL( iSession ) )
        {
        priorityRecord->LoadL( iSession );
        switch ( aAttribute )
            {
            case ECmDefaultPriority:
                {
                retVal = priorityRecord->iPriority;
                }
                break;
            case ECmDefaultUiPriority:
                {
                retVal = priorityRecord->iUIPriority;
                }
                break;
            default:
                break;
            }
        }
    CleanupStack::PopAndDestroy( priorityRecord );

    OstTraceFunctionExit0( CCMPLUGINLAN_GETDEFPRIORITYL_EXIT );
    return retVal;
    }

//  End of File
