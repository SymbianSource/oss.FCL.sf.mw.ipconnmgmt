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
*      Implementation of packetdata Plugin interface.
*
*/


#include <StringLoader.h>
#include <featmgr.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>

#include "datamobilitycommsdattypes.h"
#include "cmpluginpacketdata.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmpluginpacketdataTraces.h"
#endif

using namespace CMManager;
using namespace CommsDat;

// ================= CONSTANTS =======================

/// Modem bearer names for GPRS/WCDMA access points.
_LIT( KModemBearerPacketData, "GPRS Modem" );
_LIT( KDefaultQosDataRecordName, "PRIMARY1" );
_LIT( KDefaultQosDataRecordNamewithSpace, "PRIMARY1 " );

const TInt KQosRecordIndex = 0;

// ======== MEMBER FUNCTIONS ========
/**
*  CCmPDCoverageCheck defines the functionality for packetdata service
*  coverage checking.
*/
class CCmPDCoverageCheck : public CActive
    {
    public:
        /**
         * Constructor.
         */
        CCmPDCoverageCheck();

        /**
         * Destructor.
         */
        ~CCmPDCoverageCheck();

        /**
         * Checks if the packet service is available(status is
         * ERegisteredOnHomeNetwork or ERegisteredRoaming or
         * ENotRegisteredButAvailable)
         * @return Return ETrue if the status is one mentioned above.
         * Otherwise returns EFalse.
         */
        TBool IsThereCoverageL();

    protected:  // from CActive
        virtual void DoCancel();
        virtual void RunL();

    private:
        /**
         * Ckecks if the phode mode is offline.
         * @return Returns ETrue if the phone is in offline mode.
         * Otherwise returns EFalse.
         */
        TBool IsPhoneOfflineL() const;

    private:
        /** Boolean which has the latest coverage information  */
        TBool                   iCoverage;

        /** Controller for single scheduling loop */
        CActiveSchedulerWait    iWait;

        /** Handle to telephony server */
        RTelServer              iServer;

        /** Handle to phone(needed for packet service) */
        RPhone                  iPhone;

        /** Handle to packet service */
        RPacketService          iService;

        /** Network registration status information */
        RPacketService::TRegistrationStatus iNwRegStatus;
    };

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::CCmPDCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmPDCoverageCheck::CCmPDCoverageCheck() : CActive( EPriorityStandard )
    {
    OstTraceFunctionEntry0( CCMPDCOVERAGECHECK_CCMPDCOVERAGECHECK_ENTRY );

    CActiveScheduler::Add( this );

    OstTraceFunctionExit0( CCMPDCOVERAGECHECK_CCMPDCOVERAGECHECK_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::~CCmPDCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmPDCoverageCheck::~CCmPDCoverageCheck()
    {
    OstTraceFunctionEntry0( DUP1_CCMPDCOVERAGECHECK_CCMPDCOVERAGECHECK_ENTRY );

    Cancel();

    iService.Close();
    iPhone.Close();
    iServer.Close();

    OstTraceFunctionExit0( DUP1_CCMPDCOVERAGECHECK_CCMPDCOVERAGECHECK_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::DoCancel
// ----------------------------------------------------------------------------
//
void CCmPDCoverageCheck::DoCancel()
    {
    OstTraceFunctionEntry0( CCMPDCOVERAGECHECK_DOCANCEL_ENTRY );

    iWait.AsyncStop();

    OstTraceFunctionExit0( CCMPDCOVERAGECHECK_DOCANCEL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::RunL
// ----------------------------------------------------------------------------
//
void CCmPDCoverageCheck::RunL()
    {
    OstTraceFunctionEntry0( CCMPDCOVERAGECHECK_RUNL_ENTRY );

    if ( iStatus.Int() == KErrNone )
        {
        if ( iNwRegStatus == RPacketService::ERegisteredOnHomeNetwork ||
             iNwRegStatus == RPacketService::ERegisteredRoaming ||
             iNwRegStatus == RPacketService::ENotRegisteredButAvailable )
            {
            iCoverage = ETrue;
            }
        iWait.AsyncStop();
        }
    else
        // something went wrong -> no coverage.
        {
        iWait.AsyncStop();
        }

    OstTraceFunctionExit0( CCMPDCOVERAGECHECK_RUNL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::IsThereCoverageL
// ----------------------------------------------------------------------------
//
TBool CCmPDCoverageCheck::IsThereCoverageL()
    {
    OstTraceFunctionEntry0( CCMPDCOVERAGECHECK_ISTHERECOVERAGEL_ENTRY );

    iCoverage = EFalse;

    if ( !IsPhoneOfflineL() )
        {
        User::LeaveIfError( iServer.Connect() );

        RTelServer::TPhoneInfo info;
        User::LeaveIfError( iServer.GetPhoneInfo( 0, info ) );

        User::LeaveIfError( iPhone.Open(iServer, info.iName ) );

        User::LeaveIfError( iService.Open( iPhone ) );

        iService.GetNtwkRegStatus( iStatus, iNwRegStatus );
        SetActive();
        iWait.Start();

        User::LeaveIfError( iStatus.Int() );
        }

    OstTraceFunctionExit0( CCMPDCOVERAGECHECK_ISTHERECOVERAGEL_EXIT );
    return iCoverage;
    }

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::IsPhoneOfflineL
// ----------------------------------------------------------------------------
//
TBool CCmPDCoverageCheck::IsPhoneOfflineL() const
    {
    OstTraceFunctionEntry0( CCMPDCOVERAGECHECK_ISPHONEOFFLINEL_ENTRY );

    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        CRepository* repository = CRepository::NewLC( KCRUidCoreApplicationUIs );
        TInt connAllowed( ECoreAppUIsNetworkConnectionAllowed );

        repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
        CleanupStack::PopAndDestroy( repository );

        if ( !connAllowed )
            {
            OstTraceFunctionExit0( CCMPDCOVERAGECHECK_ISPHONEOFFLINEL_EXIT );
            return ETrue;
            }
        }

    OstTraceFunctionExit0( DUP1_CCMPDCOVERAGECHECK_ISPHONEOFFLINEL_EXIT );
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::NewL
// ----------------------------------------------------------------------------
//
CCmPluginPacketData* CCmPluginPacketData::NewL(
        TCmPluginInitParam* aInitParam )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_NEWL_ENTRY );

    CCmPluginPacketData* self = new( ELeave ) CCmPluginPacketData( aInitParam, ETrue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::~CCmPluginPacketData
// ----------------------------------------------------------------------------
//
CCmPluginPacketData::~CCmPluginPacketData()
    {
    OstTraceFunctionEntry1( CCMPLUGINPACKETDATA_CCMPLUGINPACKETDATA_ENTRY, this );

    ResetBearerRecords();

    OstTraceFunctionExit1( CCMPLUGINPACKETDATA_CCMPLUGINPACKETDATA_EXIT, this );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginPacketData::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CREATEINSTANCEL_ENTRY );

    CCmPluginPacketData* self = new( ELeave ) CCmPluginPacketData( &aInitParam, ETrue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CREATEINSTANCEL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CCmPluginPacketData
// ----------------------------------------------------------------------------
//
CCmPluginPacketData::CCmPluginPacketData(
        TCmPluginInitParam* aInitParam,
        TBool aOutgoing )
        :
        CCmPluginBaseEng( aInitParam ),
        iOutgoing( aOutgoing )
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINPACKETDATA_CCMPLUGINPACKETDATA_ENTRY );

    iBearerType = KUidPacketDataBearerType;
    iBearerRecordId = 0;
    iBearerRecordName = NULL;
    iPacketDataQoSRecord = NULL;
    iBearerPriorityTableId = 0;

    OstTraceFunctionExit0( DUP1_CCMPLUGINPACKETDATA_CCMPLUGINPACKETDATA_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ConstructL
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::ConstructL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CONSTRUCTL_ENTRY );

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

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CanHandleIapIdL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::CanHandleIapIdL( TUint32 aIapId ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CANHANDLEIAPIDL_ENTRY );

    TBool retVal( EFalse );

    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );

    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );

    TRAPD( err, iapRecord->LoadL( iSession ));

    if ( !err )
        {
        retVal = CanHandleIapIdL( iapRecord );
        }

    CleanupStack::PopAndDestroy( iapRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CanHandleIapIdL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINPACKETDATA_CANHANDLEIAPIDL_ENTRY );

    TBool retVal( EFalse );

    if ( ( TPtrC( aIapRecord->iServiceType ) == TPtrC( KCDTypeNameOutgoingWCDMA ) ||
            TPtrC( aIapRecord->iServiceType ) == TPtrC( KCDTypeNameIncomingWCDMA )) &&
            TPtrC( aIapRecord->iBearerType ) == TPtrC( KCDTypeNameModemBearer ) )
        {
        retVal = ETrue;
        }


    OstTraceFunctionExit0( DUP1_CCMPLUGINPACKETDATA_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::PreparePluginToLoadRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::PreparePluginToLoadRecordsL()
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::PreparePluginToUpdateRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::PreparePluginToUpdateRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_PREPAREPLUGINTOUPDATERECORDSL_ENTRY );

    //TODO, Add null checks for mandatory specific record pointers.

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(
            aGenRecordArray[KIapRecordIndex] );

    CCDWCDMAPacketServiceRecord* serviceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
            aGenRecordArray[KServiceRecordIndex] );

    CheckIfNameModifiedL( iapRecord, serviceRecord );

    CheckDNSServerAddressL( ETrue,
            serviceRecord->iGPRSIP6NameServer1,
            serviceRecord->iGPRSIP6NameServer2,
            serviceRecord->iGPRSIP6DNSAddrFromServer );

    CheckDNSServerAddressL( EFalse,
            serviceRecord->iGPRSIPNameServer1,
            serviceRecord->iGPRSIPNameServer2,
            serviceRecord->iGPRSIPDNSAddrFromServer );

    SetDaemonNameL( aGenRecordArray, aBearerSpecRecordArray );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_PREPAREPLUGINTOUPDATERECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetDaemonNameL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetDaemonNameL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETDAEMONNAMEL_ENTRY );

    // Use DHCP if we can.
    TBool ipfromSrv = GetBearerBoolAttributeL( EPacketDataIPAddrFromServer,
                                               aGenRecordArray,
                                               aBearerSpecRecordArray );
    if ( ipfromSrv )
        {
        SetBearerStringAttributeL( ECmConfigDaemonManagerName,
                                   KDaemonManagerName,
                                   aGenRecordArray,
                                   aBearerSpecRecordArray );
        SetBearerStringAttributeL( ECmConfigDaemonName,
                                   KConfigDaemonName,
                                   aGenRecordArray,
                                   aBearerSpecRecordArray );
        }
    else
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
            {
            SetBearerStringAttributeL( ECmConfigDaemonManagerName,
                                       KDaemonManagerName,
                                       aGenRecordArray,
                                       aBearerSpecRecordArray );
            SetBearerStringAttributeL( ECmConfigDaemonName,
                                       KConfigDaemonName,
                                       aGenRecordArray,
                                       aBearerSpecRecordArray );
            }
        else
            {
            SetBearerStringAttributeL( ECmConfigDaemonManagerName,
                                       KNullDesC(),
                                       aGenRecordArray,
                                       aBearerSpecRecordArray );
            SetBearerStringAttributeL( ECmConfigDaemonName,
                                       KNullDesC(),
                                       aGenRecordArray,
                                       aBearerSpecRecordArray );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETDAEMONNAMEL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::LoadBearerRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::LoadBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_LOADBEARERRECORDSL_ENTRY );

    if ( ServiceRecord().iUmtsR99QoSAndOnTable )
        {
        iPacketDataQoSRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );

        iPacketDataQoSRecord->SetRecordId( ServiceRecord().iUmtsR99QoSAndOnTable );
        iPacketDataQoSRecord->LoadL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_LOADBEARERRECORDSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::LoadServiceRecordL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::LoadServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_LOADSERVICERECORDL_ENTRY );

    if ( TPtrC( KCDTypeNameOutgoingWCDMA ) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        iOutgoing = ETrue;
        }
    else if ( TPtrC( KCDTypeNameIncomingWCDMA ) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIncomingGprsRecord ) );
        iOutgoing = EFalse;
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }

    iServiceRecord->SetRecordId( iIapRecord->iService );
    TRAPD( err, iServiceRecord->LoadL( iSession ) );
    if ( err == KErrNotFound )
        // record not found -> create a default one
        {
        CreateServiceRecordL();
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_LOADSERVICERECORDL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ServiceRecord()
// ----------------------------------------------------------------------------
//
CCDWCDMAPacketServiceRecord& CCmPluginPacketData::ServiceRecord() const
    {
    return *static_cast<CCDWCDMAPacketServiceRecord*>( iServiceRecord );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CreateServiceRecordL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::CreateServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CREATESERVICERECORDL_ENTRY );

    delete iServiceRecord;
    iServiceRecord = NULL;

    if ( iOutgoing )
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        }
    else
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIncomingGprsRecord ) );
        }

    ServiceRecord().iGPRSAPN.SetL( KNullDesC );
    ServiceRecord().iGPRSPDPType.SetL( RPacketContext::EPdpTypeIPv4 );
    ServiceRecord().iGPRSReqPrecedence = 0;
    ServiceRecord().iGPRSReqDelay = 0;
    ServiceRecord().iGPRSReqReliability = 0;
    ServiceRecord().iGPRSReqPeakThroughput = 0;
    ServiceRecord().iGPRSReqMeanThroughput = 0;
    ServiceRecord().iGPRSMinPrecedence = 0;
    ServiceRecord().iGPRSMinDelay = 0;
    ServiceRecord().iGPRSMinReliability = 0;
    ServiceRecord().iGPRSMinPeakThroughput = 0;
    ServiceRecord().iGPRSMinMeanThroughput = 0;
    ServiceRecord().iGPRSDataCompression = 0;
    ServiceRecord().iGPRSHeaderCompression = 0;
    ServiceRecord().iGPRSAnonymousAccess = 0;
    ServiceRecord().iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv4 );
    ServiceRecord().iGPRSIfPromptForAuth = EFalse;
    ServiceRecord().iGPRSIfAuthRetries = 0;
    ServiceRecord().iGPRSIPGateway.SetL( KUnspecifiedIPv4 );
    ServiceRecord().iGPRSIPAddrFromServer = ETrue;
    ServiceRecord().iGPRSIPAddr.SetL( KUnspecifiedIPv4 );
    ServiceRecord().iGPRSIPDNSAddrFromServer = ETrue;
    ServiceRecord().iGPRSIPNameServer1.SetL( KUnspecifiedIPv4 );
    ServiceRecord().iGPRSIPNameServer2.SetL( KUnspecifiedIPv4 );
    ServiceRecord().iGPRSIP6DNSAddrFromServer = ETrue;
    ServiceRecord().iGPRSIP6NameServer1.SetL( KDynamicIpv6Address );
    ServiceRecord().iGPRSIP6NameServer2.SetL( KDynamicIpv6Address );
    ServiceRecord().iGPRSEnableLCPExtension = EFalse;
    ServiceRecord().iGPRSDisablePlainTextAuth = ETrue;
    ServiceRecord().iGPRSAPType = EPacketDataBoth;
    ServiceRecord().iGPRSQOSWarningTimeOut = TUint32(-1);

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CREATESERVICERECORDL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ServiceRecordId
// ----------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::ServiceRecordId() const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SERVICERECORDID_ENTRY );

    return ServiceRecord().RecordId();
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ServiceRecordIdLC
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::ServiceRecordNameLC( HBufC* &aName )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SERVICERECORDNAMELC_ENTRY );

    if ( iOutgoing )
        {
        aName = TPtrC( KCDTypeNameOutgoingWCDMA ).AllocLC();
        }
    else
        {
        aName = TPtrC( KCDTypeNameIncomingWCDMA ).AllocLC();
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SERVICERECORDNAMELC_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::BearerRecordIdL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::BearerRecordIdL( TUint32& aRecordId )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_BEARERRECORDIDL_ENTRY );

    CMDBRecordSet<CCDModemBearerRecord>* bearerRS =
            new( ELeave ) CMDBRecordSet<CCDModemBearerRecord>( KCDTIdModemBearerRecord );
    CleanupStack::PushL( bearerRS );

    CCDModemBearerRecord* bearerRecord = static_cast<CCDModemBearerRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdModemBearerRecord ) );

    CleanupStack::PushL( bearerRecord );

    bearerRecord->iRecordName.SetL( KModemBearerPacketData );
    bearerRS->iRecords.AppendL( bearerRecord );
    CleanupStack::Pop( bearerRecord );

    if ( bearerRS->FindL( iSession ) )
        {
        bearerRecord = static_cast<CCDModemBearerRecord*>( bearerRS->iRecords[0] );
        aRecordId = bearerRecord->RecordId();
        }
    else
        // bearer not found -> create dummy values
        {
        bearerRecord->SetRecordId( KCDNewRecordRequest );
        bearerRecord->StoreL( iSession );

        aRecordId = bearerRecord->RecordId();
        }

    CleanupStack::PopAndDestroy( bearerRS );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_BEARERRECORDIDL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::BearerRecordNameLC()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::BearerRecordNameLC( HBufC*& aBearerName )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_BEARERRECORDNAMELC_ENTRY );

    aBearerName = TPtrC( KCDTypeNameModemBearer ).AllocLC();

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_BEARERRECORDNAMELC_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CheckNetworkCoverageL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::CheckNetworkCoverageL() const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CHECKNETWORKCOVERAGEL_ENTRY );

    TBool retVal( EFalse );

#ifdef __WINS__
    retVal = ETrue;
#else
    CCmPDCoverageCheck* coverage = new( ELeave ) CCmPDCoverageCheck;
    CleanupStack::PushL( coverage );

    retVal = coverage->IsThereCoverageL();

    CleanupStack::PopAndDestroy( coverage );

#endif  // __WINS

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CHECKNETWORKCOVERAGEL_EXIT );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::SetPDPTypeL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::SetPDPTypeL(
        RPacketContext::TProtocolType aPdpType,
        CCDWCDMAPacketServiceRecord* aServiceRecord )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETPDPTYPEL_ENTRY );

    if ( aPdpType == RPacketContext::EPdpTypeIPv6 )
        {
        aServiceRecord->iGPRSPDPType = RPacketContext::EPdpTypeIPv6;
        aServiceRecord->iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv6 );
        }
    else if ( aPdpType == RPacketContext::EPdpTypeIPv4 )
        {
        aServiceRecord->iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        aServiceRecord->iGPRSPDPType = RPacketContext::EPdpTypeIPv4;
        }
    else
        {
        User::Leave( KErrArgument );
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETPDPTYPEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::CreateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::CreateBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_CREATEBEARERRECORDSL_ENTRY );

    delete iPacketDataQoSRecord;
    iPacketDataQoSRecord = NULL;

    iPacketDataQoSRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );

    iPacketDataQoSRecord->iGPRSReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    iPacketDataQoSRecord->iGPRSMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    iPacketDataQoSRecord->iGPRSReqDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    iPacketDataQoSRecord->iGPRSMinDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    iPacketDataQoSRecord->iGPRSReqDeliverErroneousSDU =
            RPacketQoS::EErroneousSDUDeliveryUnspecified;
    iPacketDataQoSRecord->iGPRSMinDeliverErroneousSDU =
            RPacketQoS::EErroneousSDUDeliveryUnspecified;
    iPacketDataQoSRecord->iGPRSReqMaxSDUSize = 0;
    iPacketDataQoSRecord->iGPRSMinAcceptableMaxSDUSize = 0;
    iPacketDataQoSRecord->iGPRSReqMaxUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqMinUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqMaxDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqMinDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqBER = RPacketQoS::EBERUnspecified;
    iPacketDataQoSRecord->iGPRSMaxBER = RPacketQoS::EBERUnspecified;
    iPacketDataQoSRecord->iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    iPacketDataQoSRecord->iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    iPacketDataQoSRecord->iGPRSReqTrafficHandlingPriority =
            RPacketQoS::ETrafficPriorityUnspecified;
    iPacketDataQoSRecord->iGPRSMinTrafficHandlingPriority =
            RPacketQoS::ETrafficPriorityUnspecified;
    iPacketDataQoSRecord->iGPRSReqTransferDelay = 0;
    iPacketDataQoSRecord->iGPRSMaxTransferDelay = 0;
    iPacketDataQoSRecord->iGPRSReqGuaranteedUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSMinGuaranteedUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqGuaranteedDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSMinGuaranteedDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSSignallingIndication = EFalse;
    iPacketDataQoSRecord->iGPRS_ImCnSignallingIndication = EFalse;
    iPacketDataQoSRecord->iGPRSSourceStatisticsDescriptor =
            RPacketQoS::ESourceStatisticsDescriptorUnknown;

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_CREATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::UpdateServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_UPDATESERVICERECORDL_ENTRY );

    // Delete the original record and create a copy from the parameter.
    delete iServiceRecord;
    iServiceRecord = NULL;

    if ( iOutgoing )
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        }
    else
        {
        iServiceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIncomingGprsRecord ) );
        }

    CCDWCDMAPacketServiceRecord* origServiceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>( iServiceRecord );

    // Client's copy of packet service record.
    CCDWCDMAPacketServiceRecord* clientServiceRecordCopy =
            static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    CopyRecordFieldsL( *clientServiceRecordCopy, *origServiceRecord );

    if ( iPacketDataQoSRecord )
        {
        origServiceRecord->iUmtsR99QoSAndOnTable = iPacketDataQoSRecord->RecordId();
        }

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

    CopyRecordFieldsL( *origServiceRecord, *clientServiceRecordCopy );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_UPDATESERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::UpdateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::UpdateBearerRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_UPDATEBEARERRECORDSL_ENTRY );

    CCDUmtsR99QoSAndOnTableRecord* packetDataQoSRecord =
            static_cast<CCDUmtsR99QoSAndOnTableRecord*>( aBearerSpecRecordArray[KQosRecordIndex] );

    delete iPacketDataQoSRecord;
    iPacketDataQoSRecord = NULL;
    iPacketDataQoSRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );
    CopyRecordFieldsL( *packetDataQoSRecord, *iPacketDataQoSRecord );
    iPacketDataQoSRecord->SetElementId( aBearerSpecRecordArray[KQosRecordIndex]->ElementId() );

    if ( !iPacketDataQoSRecord->RecordId() )
        {
        iPacketDataQoSRecord->SetRecordId( KCDNewRecordRequest );
        iPacketDataQoSRecord->StoreL( iSession );
        iPacketDataQoSRecord->LoadL( iSession );
        packetDataQoSRecord->SetElementId( iPacketDataQoSRecord->ElementId() );

        // Set service record to point to QoS record.
        ServiceRecord().iUmtsR99QoSAndOnTable = iPacketDataQoSRecord->RecordId();
        ServiceRecord().ModifyL( iSession );

        CCDWCDMAPacketServiceRecord* serviceRecord =
                static_cast<CCDWCDMAPacketServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );
        serviceRecord->iUmtsR99QoSAndOnTable = iPacketDataQoSRecord->RecordId();
        }
    else
        {
        iPacketDataQoSRecord->ModifyL( iSession );
        }

    CopyRecordFieldsL( *iPacketDataQoSRecord, *packetDataQoSRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_UPDATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerSpecificRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::GetBearerSpecificRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERSPECIFICRECORDSL_ENTRY );

    if ( !iPacketDataQoSRecord )
        {
        // IAP not yet in CommDat.
        GetDefaultQosRecordL( aRecordArray );
        }
    else
        {
        CCDUmtsR99QoSAndOnTableRecord* qosRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );
        CleanupStack::PushL( qosRecord );
        CopyRecordFieldsL( *iPacketDataQoSRecord, *qosRecord );
        qosRecord->SetElementId( iPacketDataQoSRecord->ElementId() );
        aRecordArray.AppendL( static_cast<CommsDat::CCDRecordBase*>( qosRecord ) );
        CleanupStack::Pop( qosRecord );
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERSPECIFICRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetDefaultServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::GetDefaultServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETDEFAULTSERVICERECORDL_ENTRY );

    CCDWCDMAPacketServiceRecord* serviceRecord( NULL );
    if ( iOutgoing )
        {
        serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        }
    else
        {
        serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        }

    CleanupStack::PushL( serviceRecord );
    serviceRecord->iGPRSAPN.SetL( KNullDesC );
    serviceRecord->iGPRSPDPType.SetL( RPacketContext::EPdpTypeIPv4 );
    serviceRecord->iGPRSReqPrecedence = 0;
    serviceRecord->iGPRSReqDelay = 0;
    serviceRecord->iGPRSReqReliability = 0;
    serviceRecord->iGPRSReqPeakThroughput = 0;
    serviceRecord->iGPRSReqMeanThroughput = 0;
    serviceRecord->iGPRSMinPrecedence = 0;
    serviceRecord->iGPRSMinDelay = 0;
    serviceRecord->iGPRSMinReliability = 0;
    serviceRecord->iGPRSMinPeakThroughput = 0;
    serviceRecord->iGPRSMinMeanThroughput = 0;
    serviceRecord->iGPRSDataCompression = EFalse;
    serviceRecord->iGPRSHeaderCompression = EFalse;
    serviceRecord->iGPRSAnonymousAccess = EFalse;
    serviceRecord->iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv4 );
    serviceRecord->iGPRSIfPromptForAuth = EFalse;
    serviceRecord->iGPRSIfAuthRetries = 0;
    serviceRecord->iGPRSIPGateway.SetL( KUnspecifiedIPv4 );
    serviceRecord->iGPRSIPAddrFromServer = ETrue;
    serviceRecord->iGPRSIPAddr.SetL( KUnspecifiedIPv4 );
    serviceRecord->iGPRSIPDNSAddrFromServer = ETrue;
    serviceRecord->iGPRSIPNameServer1.SetL( KUnspecifiedIPv4 );
    serviceRecord->iGPRSIPNameServer2.SetL( KUnspecifiedIPv4 );
    serviceRecord->iGPRSIP6DNSAddrFromServer = ETrue;
    serviceRecord->iGPRSIP6NameServer1.SetL( KDynamicIpv6Address );
    serviceRecord->iGPRSIP6NameServer2.SetL( KDynamicIpv6Address );
    serviceRecord->iGPRSEnableLCPExtension = EFalse;
    serviceRecord->iGPRSDisablePlainTextAuth = ETrue;
    serviceRecord->iGPRSAPType = EPacketDataBoth;
    serviceRecord->iGPRSQOSWarningTimeOut = TUint32(-1);

    aRecordArray.AppendL( static_cast<CommsDat::CCDRecordBase*>( serviceRecord ) );
    CleanupStack::Pop( serviceRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETDEFAULTSERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetDefaultQosRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::GetDefaultQosRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETDEFAULTQOSRECORDL_ENTRY );

    CCDUmtsR99QoSAndOnTableRecord* packetDataQoSRecord =
            static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );
    CleanupStack::PushL( packetDataQoSRecord );

    packetDataQoSRecord->iGPRSReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    packetDataQoSRecord->iGPRSMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    packetDataQoSRecord->iGPRSReqDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    packetDataQoSRecord->iGPRSMinDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    packetDataQoSRecord->iGPRSReqDeliverErroneousSDU =
            RPacketQoS::EErroneousSDUDeliveryUnspecified;
    packetDataQoSRecord->iGPRSMinDeliverErroneousSDU =
            RPacketQoS::EErroneousSDUDeliveryUnspecified;
    packetDataQoSRecord->iGPRSReqMaxSDUSize = 0;
    packetDataQoSRecord->iGPRSMinAcceptableMaxSDUSize = 0;
    packetDataQoSRecord->iGPRSReqMaxUplinkRate = 0;
    packetDataQoSRecord->iGPRSReqMinUplinkRate = 0;
    packetDataQoSRecord->iGPRSReqMaxDownlinkRate = 0;
    packetDataQoSRecord->iGPRSReqMinDownlinkRate = 0;
    packetDataQoSRecord->iGPRSReqBER = RPacketQoS::EBERUnspecified;
    packetDataQoSRecord->iGPRSMaxBER = RPacketQoS::EBERUnspecified;
    packetDataQoSRecord->iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    packetDataQoSRecord->iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    packetDataQoSRecord->iGPRSReqTrafficHandlingPriority =
            RPacketQoS::ETrafficPriorityUnspecified;
    packetDataQoSRecord->iGPRSMinTrafficHandlingPriority =
            RPacketQoS::ETrafficPriorityUnspecified;
    packetDataQoSRecord->iGPRSReqTransferDelay = 0;
    packetDataQoSRecord->iGPRSMaxTransferDelay = 0;
    packetDataQoSRecord->iGPRSReqGuaranteedUplinkRate = 0;
    packetDataQoSRecord->iGPRSMinGuaranteedUplinkRate = 0;
    packetDataQoSRecord->iGPRSReqGuaranteedDownlinkRate = 0;
    packetDataQoSRecord->iGPRSMinGuaranteedDownlinkRate = 0;
    packetDataQoSRecord->iGPRSSignallingIndication = EFalse;
    packetDataQoSRecord->iGPRS_ImCnSignallingIndication = EFalse;
    packetDataQoSRecord->iGPRSSourceStatisticsDescriptor =
            RPacketQoS::ESourceStatisticsDescriptorUnknown;

    aRecordArray.AppendL( static_cast<CommsDat::CCDRecordBase*>( packetDataQoSRecord ) );
    CleanupStack::Pop( packetDataQoSRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETDEFAULTQOSRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetBearerIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    CCDWCDMAPacketServiceRecord* serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    if ( aAttribute < ECmCommonAttributesEnd ||
            ( aAttribute > ECmBearerCreationCommonAttributes &&
              aAttribute < ECmBearerCreationCommonAttributesEnd ) )
        {
        switch ( aAttribute )
            {
            case ECmBearerType:
                {
                retVal = iBearerType;
                }
                break;
            case ECmCommsDBBearerType:
                {
                retVal = KCommDbBearerWcdma;
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
            case ECmInvalidAttribute:
                {
                // This attribute has been deprecated since Symbian^4.
                User::Leave( KErrNotSupported );
                }
                break;
            case ECmIFAuthRetries:
                {
                retVal = serviceRecord->iGPRSIfAuthRetries;
                }
                break;
            default:
                {
                User::Leave( KErrNotFound ); //TODO, check leave code, should it be KErrNotSupported ?
                }
                break;
            }

        OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERINTATTRIBUTEL_EXIT );
        return retVal;
        }

    if ( aAttribute >= EPacketDataSpecificAttributes &&
            aAttribute < EPacketDataServiceEnableLLMNR )
        {
        retVal = GetServiceIntAttributeL( aAttribute, aGenRecordArray );
        }
    else if ( aAttribute >= EGPRSReqTrafficClass &&
            aAttribute <= EGPRSSourceStatisticsDescriptor )
        {
        retVal = GetQosIntAttributeL( aAttribute, aBearerSpecRecordArray );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    OstTraceFunctionExit0( DUP1_CCMPLUGINPACKETDATA_GETBEARERINTATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetServiceIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetServiceIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETSERVICEINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    CCDWCDMAPacketServiceRecord* serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EPacketDataPDPType:
            {
            retVal = serviceRecord->iGPRSPDPType;
            }
            break;

        case EPacketDataReqPrecedence:
            {
            retVal = serviceRecord->iGPRSReqPrecedence;
            }
            break;

        case EPacketDataReqDelay:
            {
            retVal = serviceRecord->iGPRSReqDelay;
            }
            break;

        case EPacketDataReliability:
            {
            retVal = serviceRecord->iGPRSReqReliability;
            }
            break;

        case EPacketDataPeakThroughput:
            {
            retVal = serviceRecord->iGPRSReqPeakThroughput;
            }
            break;

        case EPacketDataMeanThroughput:
            {
            retVal = serviceRecord->iGPRSMinMeanThroughput;
            }
            break;

        case EPacketDataMinPrecedence:
            {
            retVal = serviceRecord->iGPRSMinPrecedence;
            }
            break;

        case EPacketDataMinDelay:
            {
            retVal = serviceRecord->iGPRSMinDelay;
            }
            break;

        case EPacketDataMinReliability:
            {
            retVal = serviceRecord->iGPRSMinReliability;
            }
            break;

        case EPacketDataMinPeakThroughput:
            {
            retVal = serviceRecord->iGPRSMinPeakThroughput;
            }
            break;

        case EPacketDataMinMeanThroughput:
            {
            retVal = serviceRecord->iGPRSMinMeanThroughput;
            }
            break;

        case EPacketDataIFAuthRetries:
            {
            retVal = serviceRecord->iGPRSIfAuthRetries;
            }
            break;

        case EPacketDataApType:
            {
            retVal = serviceRecord->iGPRSAPType;
            }
            break;

        case EPacketDataQoSWarningTimeOut:
            {
            retVal = serviceRecord->iGPRSQOSWarningTimeOut;
            }
            break;
        default:
            {
            User::Leave( KErrNotFound );
            }
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETSERVICEINTATTRIBUTEL_EXIT );

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetQosIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetQosIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETQOSINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    CCDUmtsR99QoSAndOnTableRecord* qosRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
            aBearerSpecRecordArray[KQosRecordIndex] );

    switch ( aAttribute )
        {
        case EGPRSReqTrafficClass:
            {
            retVal = qosRecord->iGPRSReqTrafficClass;
            }
            break;

        case EGPRSMinTrafficClass:
            {
            retVal = qosRecord->iGPRSMinTrafficClass;
            }
            break;

        case EGPRSReqDeliveryOrder:
            {
            retVal = qosRecord->iGPRSReqDeliveryOrder;
            }
            break;

        case GPRSMinDeliveryOrder:
            {
            retVal = qosRecord->iGPRSMinDeliveryOrder;
            }
            break;
        case EGPRSReqDeliverErroneousSDU:
            {
            retVal = qosRecord->iGPRSReqDeliverErroneousSDU;
            }
            break;
        case EGPRSMinDeliverErroneousSDU:
            {
            retVal = qosRecord->iGPRSMinDeliverErroneousSDU;
            }
            break;
        case EGPRSReqMaxSDUSize:
            {
            retVal = qosRecord->iGPRSReqMaxSDUSize;
            }
            break;

        case EGPRSMinAcceptableMaxSDUSize:
            {
            retVal = qosRecord->iGPRSMinAcceptableMaxSDUSize;
            }
            break;

        case EGPRSReqMaxUplinkRate:
            {
            retVal = qosRecord->iGPRSReqMaxUplinkRate;
            }
            break;

        case EGPRSReqMinUplinkRate:
            {
            retVal = qosRecord->iGPRSReqMinUplinkRate;
            }
            break;

        case EGPRSReqMaxDownlinkRate:
            {
            retVal = qosRecord->iGPRSReqMaxDownlinkRate;
            }
            break;

        case EGPRSReqMinDownlinkRate:
            {
            retVal = qosRecord->iGPRSReqMinDownlinkRate;
            }
            break;

        case EGPRSReqBER:
            {
            retVal = qosRecord->iGPRSReqBER;
            }
            break;

        case EGPRSMaxBER:
            {
            retVal = qosRecord->iGPRSMaxBER;
            }
            break;

        case EGPRSReqSDUErrorRatio:
            {
            retVal = qosRecord->iGPRSReqSDUErrorRatio;
            }
            break;

        case EGPRSMaxSDUErrorRatio:
            {
            retVal = qosRecord->iGPRSMaxSDUErrorRatio;
            }
            break;

        case EGPRSReqTrafficHandlingPriority:
            {
            retVal = qosRecord->iGPRSReqTrafficHandlingPriority;
            }
            break;

        case EGPRSMinTrafficHandlingPriority:
            {
            retVal = qosRecord->iGPRSMinTrafficHandlingPriority;
            }
            break;

        case EGPRSReqTransferDelay:
            {
            retVal = qosRecord->iGPRSReqTransferDelay;
            }
            break;

        case EGPRSMaxTransferDelay:
            {
            retVal = qosRecord->iGPRSMaxTransferDelay;
            }
            break;

        case EGPRSReqGuaranteedUplinkRate:
            {
            retVal = qosRecord->iGPRSReqGuaranteedUplinkRate;
            }
            break;

        case EGPRSMinGuaranteedUplinkRate:
            {
            retVal = qosRecord->iGPRSMinGuaranteedUplinkRate;
            }
            break;

        case EGPRSReqGuaranteedDownlinkRate:
            {
            retVal = qosRecord->iGPRSReqGuaranteedDownlinkRate;
            }
            break;

        case EGPRSMinGuaranteedDownlinkRate:
            {
            retVal = qosRecord->iGPRSMinGuaranteedDownlinkRate;
            }
            break;

        case EGPRSSourceStatisticsDescriptor:
            {
            retVal = qosRecord->iGPRSSourceStatisticsDescriptor;
            }
            break;

        default:
            {
            User::Leave( KErrNotFound );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETQOSINTATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerBoolAttributeL
// ---------------------------------------------------------------------------
//
TBool CCmPluginPacketData::GetBearerBoolAttributeL( TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERBOOLATTRIBUTEL_ENTRY );

    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case EPacketDataOutGoing:
            {
            retVal = iOutgoing;
            }
            break;
        case ECmCoverage:
            {
#ifndef __WINS__
            CCmPDCoverageCheck* coverage = new( ELeave ) CCmPDCoverageCheck;
            CleanupStack::PushL( coverage );

            retVal = coverage->IsThereCoverageL();

            CleanupStack::PopAndDestroy( coverage );
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
            retVal = ETrue;
            }
            break;
        case ECmIPv6Supported:
            {
            retVal = FeatureManager::FeatureSupported( KFeatureIdIPv6 );
            }
            break;
        case EPacketDataDataCompression:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSDataCompression;
            }
            break;
        case EPacketDataHeaderCompression:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSHeaderCompression;
            }
            break;
        case EPacketDataUseEdge:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSUseEdge;
            }
            break;
        case EPacketDataAnonymousAccess:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSAnonymousAccess;
            }
            break;
        case ECmIFPromptForAuth:
        case EPacketDataIFPromptForAuth:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSIfPromptForAuth;
            }
            break;
        case ECmIPAddFromServer:
        case EPacketDataIPAddrFromServer:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSIPAddrFromServer;
            }
            break;
        case ECmIPDNSAddrFromServer:
        case EPacketDataIPDNSAddrFromServer:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSIPDNSAddrFromServer;
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case EPacketDataIPIP6DNSAddrFromServer:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSIP6DNSAddrFromServer;
            }
            break;
        case ECmEnableLPCExtension:
        case EPacketDataEnableLcpExtension:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSEnableLCPExtension;
            }
            break;
        case EPacketDataServiceEnableLLMNR:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iServiceEnableLlmnr;
            }
            break;
        case ECmDisablePlainTextAuth:
        case EPacketDataDisablePlainTextAuth:
            {
            retVal = static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] )->iGPRSDisablePlainTextAuth;
            }
            break;
        case EGPRSSignallingIndication:
            {
            retVal = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                    aBearerSpecRecordArray[KQosRecordIndex] )->iGPRSSignallingIndication;
            }
            break;
        case EGPRS_ImCnSignallingIndication:
            {
            retVal = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                    aBearerSpecRecordArray[KQosRecordIndex] )->iGPRS_ImCnSignallingIndication;
            }
            break;
        default:
            {
            User::Leave( KErrNotFound );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerStringAttributeL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginPacketData::GetBearerStringAttributeL( TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERSTRINGATTRIBUTEL_ENTRY );

    HBufC* retVal( NULL );
    if ( ( aAttribute > EPacketDataSpecificAttributes && aAttribute < EGPRSReqTrafficClass ) ||
            ( aAttribute > ECmCommonAttributesStart && aAttribute < ECmCommonAttributesEnd ) )
        {
        retVal = GetServiceStringAttributeL( aAttribute, aGenRecordArray );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERSTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetServiceStringAttributeL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginPacketData::GetServiceStringAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETSERVICESTRINGATTRIBUTEL_ENTRY );

    HBufC* retVal = NULL;

    CCDWCDMAPacketServiceRecord* serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );
    switch ( aAttribute )
        {
        case ECmIFName:
        case EPacketDataAPName:
            {
            retVal = TPtrC( serviceRecord->iGPRSAPN ).AllocL();
            }
            break;
        case EPacketDataPDPAddress:
            {
            retVal = TPtrC( serviceRecord->iGPRSPDPAddress ).AllocL();
            }
            break;
        case ECmIFParams:
        case EPacketDataIFParams:
            {
            retVal = TPtrC( serviceRecord->iGPRSIfParams ).AllocL();
            }
            break;
        case ECmIFNetworks:
        case EPacketDataIFNetworks:
            {
            retVal = TPtrC( serviceRecord->iGPRSIfNetworks ).AllocL();
            }
            break;
        case ECmIFAuthName:
        case EPacketDataIFAuthName:
            {
            retVal = TPtrC( serviceRecord->iGPRSIfAuthName ).AllocL();
            }
            break;
        case ECmIFAuthPass:
        case EPacketDataIFAuthPass:
            {
            retVal = TPtrC( serviceRecord->iGPRSIfAuthPass ).AllocL();
            }
            break;
        case ECmIPNetmask:
        case EPacketDataIPNetmask:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPNetMask ).AllocL();
            }
            break;
        case ECmIPGateway:
        case EPacketDataIPGateway:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPGateway ).AllocL();
            }
            break;
        case ECmIPAddress:
        case EPacketDataIPAddr:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPAddr ).AllocL();
            }
            break;
        case ECmIPNameServer1:
        case EPacketDataIPNameServer1:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPNameServer1 ).AllocL();
            }
            break;
        case ECmIPNameServer2:
        case EPacketDataIPNameServer2:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPNameServer2 ).AllocL();
            }
            break;
        case ECmIP6NameServer1:
        case EPacketDataIPIP6NameServer1:
            {
            retVal = TPtrC( serviceRecord->iGPRSIP6NameServer1 ).AllocL();
            }
            break;
        case ECmIP6NameServer2:
        case EPacketDataIPIP6NameServer2:
            {
            retVal = TPtrC( serviceRecord->iGPRSIP6NameServer2 ).AllocL();
            }
            break;
        case ECmIPAddrLeaseValidFrom:
        case EPacketDataIPAddrLeaseValidFrom:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPAddrLeaseValidFrom ).AllocL();
            }
            break;
        case ECmIPAddrLeaseValidTo:
        case EPacketDataIPAddrLeaseValidTo:
            {
            retVal = TPtrC( serviceRecord->iGPRSIPAddrLeaseValidTo ).AllocL();
            }
            break;
        case ECmConfigDaemonManagerName:
        case EPacketDataConfigDaemonManagerName:
            {
            retVal = TPtrC( serviceRecord->iGPRSConfigDaemonManagerName ).AllocL();
            }
            break;
        case ECmConfigDaemonName:
        case EPacketDataConfigDaemonName:
            {
            retVal = TPtrC( serviceRecord->iGPRSConfigDaemonName ).AllocL();
            }
            break;

        default:
            {
            User::Leave( KErrNotFound );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETSERVICESTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerString8AttributeL
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginPacketData::GetBearerString8AttributeL( TUint32 /*aAttribute*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERSTRING8ATTRIBUTEL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERSTRING8ATTRIBUTEL_EXIT );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetBearerIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetBearerIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETBEARERINTATTRIBUTEL_ENTRY );

    if ( aAttribute >= EPacketDataSpecificAttributes && aAttribute < EPacketDataServiceEnableLLMNR )
        {
        SetServiceIntAttributeL( aAttribute, aValue, aGenRecordArray );
        }
    else if ( aAttribute >= EGPRSReqTrafficClass && aAttribute <= EGPRSSourceStatisticsDescriptor )
        {
        SetQosIntAttributeL( aAttribute, aValue, aBearerSpecRecordArray );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETBEARERINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetServiceIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetServiceIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETSERVICEINTATTRIBUTEL_ENTRY );

    CCDWCDMAPacketServiceRecord* serviceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
            aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EPacketDataPDPType:
            {
            SetPDPTypeL( static_cast<RPacketContext::TProtocolType>( aValue ),
                         serviceRecord );
            }
            break;

        case EPacketDataReqPrecedence:
            {
            serviceRecord->iGPRSReqPrecedence.SetL( aValue );
            }
            break;

        case EPacketDataReqDelay:
            {
            serviceRecord->iGPRSReqDelay.SetL( aValue );
            }
            break;

        case EPacketDataReliability:
            {
            serviceRecord->iGPRSReqReliability.SetL( aValue );
            }
            break;

        case EPacketDataPeakThroughput:
            {
            serviceRecord->iGPRSReqPeakThroughput.SetL( aValue );
            }
            break;

        case EPacketDataMeanThroughput:
            {
            serviceRecord->iGPRSMinMeanThroughput.SetL( aValue );
            }
            break;

        case EPacketDataMinPrecedence:
            {
            serviceRecord->iGPRSMinPrecedence.SetL( aValue );
            }
            break;

        case EPacketDataMinDelay:
            {
            serviceRecord->iGPRSMinDelay.SetL( aValue );
            }
            break;

        case EPacketDataMinReliability:
            {
            serviceRecord->iGPRSMinReliability.SetL( aValue );
            }
            break;

        case EPacketDataMinPeakThroughput:
            {
            serviceRecord->iGPRSMinPeakThroughput.SetL( aValue );
            }
            break;

        case EPacketDataMinMeanThroughput:
            {
            serviceRecord->iGPRSMinMeanThroughput.SetL( aValue );
            }
            break;

        case EPacketDataIFAuthRetries:
            {
            serviceRecord->iGPRSIfAuthRetries.SetL( aValue );
            }
            break;

        case EPacketDataApType:
            {
            serviceRecord->iGPRSAPType.SetL( aValue );
            }
            break;

        case EPacketDataQoSWarningTimeOut:
            {
            serviceRecord->iGPRSQOSWarningTimeOut.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotFound );
            }
        }
    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETSERVICEINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetQosIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetQosIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETQOSINTATTRIBUTEL_ENTRY );

    CCDUmtsR99QoSAndOnTableRecord* qosRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
            aBearerSpecRecordArray[KQosRecordIndex] );

    switch ( aAttribute )
        {
        case EGPRSReqTrafficClass:
            {
            qosRecord->iGPRSReqTrafficClass.SetL(
                    static_cast<RPacketQoS::TTrafficClass>( aValue ) );
            }
            break;

        case EGPRSMinTrafficClass:
            {
            qosRecord->iGPRSMinTrafficClass.SetL(
                    static_cast<RPacketQoS::TTrafficClass>( aValue ) );
            }
            break;

        case EGPRSReqDeliveryOrder:
            {
            qosRecord->iGPRSReqDeliveryOrder.SetL(
                    static_cast<RPacketQoS::TDeliveryOrder>( aValue ) );
            }
            break;

        case GPRSMinDeliveryOrder:
            {
            qosRecord->iGPRSMinDeliveryOrder.SetL(
                    static_cast<RPacketQoS::TDeliveryOrder>( aValue ) );
            }
            break;
        case EGPRSReqDeliverErroneousSDU:
            {
            qosRecord->iGPRSReqDeliverErroneousSDU.SetL(
                    static_cast<RPacketQoS::TErroneousSDUDelivery>( aValue ) );
            }
            break;
        case EGPRSMinDeliverErroneousSDU:
            {
            qosRecord->iGPRSMinDeliverErroneousSDU.SetL(
                    static_cast<RPacketQoS::TErroneousSDUDelivery>( aValue ) );
            }
            break;
        case EGPRSReqMaxSDUSize:
            {
            qosRecord->iGPRSReqMaxSDUSize.SetL( aValue );
            }
            break;

        case EGPRSMinAcceptableMaxSDUSize:
            {
            qosRecord->iGPRSMinAcceptableMaxSDUSize.SetL( aValue );
            }
            break;

        case EGPRSReqMaxUplinkRate:
            {
            qosRecord->iGPRSReqMaxUplinkRate.SetL( aValue );
            }
            break;

        case EGPRSReqMinUplinkRate:
            {
            qosRecord->iGPRSReqMinUplinkRate.SetL( aValue );
            }
            break;

        case EGPRSReqMaxDownlinkRate:
            {
            qosRecord->iGPRSReqMaxDownlinkRate.SetL( aValue );
            }
            break;

        case EGPRSReqMinDownlinkRate:
            {
            qosRecord->iGPRSReqMinDownlinkRate.SetL( aValue );
            }
            break;

        case EGPRSReqBER:
            {
            qosRecord->iGPRSReqBER.SetL(
                    static_cast<RPacketQoS::TBitErrorRatio>( aValue ) );
            }
            break;

        case EGPRSMaxBER:
            {
            qosRecord->iGPRSMaxBER.SetL(
                    static_cast<RPacketQoS::TBitErrorRatio>( aValue ) );
            }
            break;

        case EGPRSReqSDUErrorRatio:
            {
            qosRecord->iGPRSReqSDUErrorRatio.SetL(
                    static_cast<RPacketQoS::TSDUErrorRatio>( aValue ) );
            }
            break;

        case EGPRSMaxSDUErrorRatio:
            {
            qosRecord->iGPRSMaxSDUErrorRatio.SetL(
                    static_cast<RPacketQoS::TSDUErrorRatio>( aValue ) );
            }
            break;

        case EGPRSReqTrafficHandlingPriority:
            {
            qosRecord->iGPRSReqTrafficHandlingPriority.SetL(
                    static_cast<RPacketQoS::TTrafficHandlingPriority>( aValue ) );
            }
            break;

        case EGPRSMinTrafficHandlingPriority:
            {
            qosRecord->iGPRSMinTrafficHandlingPriority.SetL(
                    static_cast<RPacketQoS::TTrafficHandlingPriority>( aValue ) );
            }
            break;

        case EGPRSReqTransferDelay:
            {
            qosRecord->iGPRSReqTransferDelay.SetL( aValue );
            }
            break;

        case EGPRSMaxTransferDelay:
            {
            qosRecord->iGPRSMaxTransferDelay.SetL( aValue );
            }
            break;

        case EGPRSReqGuaranteedUplinkRate:
            {
            qosRecord->iGPRSReqGuaranteedUplinkRate.SetL( aValue );
            }
            break;

        case EGPRSMinGuaranteedUplinkRate:
            {
            qosRecord->iGPRSMinGuaranteedUplinkRate.SetL( aValue );
            }
            break;

        case EGPRSReqGuaranteedDownlinkRate:
            {
            qosRecord->iGPRSReqGuaranteedDownlinkRate.SetL( aValue );
            }
            break;

        case EGPRSMinGuaranteedDownlinkRate:
            {
            qosRecord->iGPRSMinGuaranteedDownlinkRate.SetL( aValue );
            }
            break;

        case EGPRSSourceStatisticsDescriptor:
            {
            qosRecord->iGPRSSourceStatisticsDescriptor.SetL(
                    static_cast<RPacketQoS::TSourceStatisticsDescriptor>( aValue ) );
            }
            break;

        default:
            {
            User::Leave( KErrNotFound );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETQOSINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetBearerBoolAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetBearerBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETBEARERBOOLATTRIBUTEL_ENTRY );

    CCDWCDMAPacketServiceRecord* serviceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
            aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EPacketDataOutGoing:
            {
            iOutgoing = aValue;
            }
            break;
        case EPacketDataDataCompression:
            {
            serviceRecord->iGPRSDataCompression.SetL( aValue );
            }
            break;
        case EPacketDataHeaderCompression:
            {
            serviceRecord->iGPRSHeaderCompression.SetL( aValue );
            }
            break;
        case EPacketDataUseEdge:
            {
            serviceRecord->iGPRSUseEdge.SetL( aValue );
            }
            break;
        case EPacketDataAnonymousAccess:
            {
            serviceRecord->iGPRSAnonymousAccess.SetL( aValue );
            }
            break;
        case ECmIFPromptForAuth:
        case EPacketDataIFPromptForAuth:
            {
            serviceRecord->iGPRSIfPromptForAuth.SetL( aValue );
            }
            break;
        case ECmIPAddFromServer:
        case EPacketDataIPAddrFromServer:
            {
            serviceRecord->iGPRSIPAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIPDNSAddrFromServer:
        case EPacketDataIPDNSAddrFromServer:
            {
            serviceRecord->iGPRSIPDNSAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case EPacketDataIPIP6DNSAddrFromServer:
            {
            serviceRecord->iGPRSIP6DNSAddrFromServer.SetL( aValue );
            }
            break;
        case ECmEnableLPCExtension:
        case EPacketDataEnableLcpExtension:
            {
            serviceRecord->iGPRSEnableLCPExtension.SetL( aValue );
            }
            break;
        case ECmDisablePlainTextAuth:
        case EPacketDataDisablePlainTextAuth:
            {
            serviceRecord->iGPRSDisablePlainTextAuth.SetL( aValue );
            }
            break;
        case EGPRSSignallingIndication:
            {
            static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
                    aBearerSpecRecordArray[KQosRecordIndex] )
                    ->iGPRSSignallingIndication.SetL( aValue );
            }
            break;
        case EGPRS_ImCnSignallingIndication:
            {
            static_cast<CCDUmtsR99QoSAndOnTableRecord*>
                 ( aBearerSpecRecordArray[KQosRecordIndex] )
                 ->iGPRS_ImCnSignallingIndication.SetL( aValue );
            }
            break;
        case EPacketDataServiceEnableLLMNR:
            {
            serviceRecord->iServiceEnableLlmnr.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotFound );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETBEARERBOOLATTRIBUTEL_EXIT );
    }


// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetBearerStringAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetBearerStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETBEARERSTRINGATTRIBUTEL_ENTRY );

    CCDWCDMAPacketServiceRecord* serviceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EPacketDataAPName:
            {
            serviceRecord->iGPRSAPN.SetL( aValue );
            }
            break;

        case EPacketDataPDPAddress:
            {
            serviceRecord->iGPRSPDPAddress.SetL( aValue );
            }
            break;

        case ECmIFParams:
        case EPacketDataIFParams:
            {
            serviceRecord->iGPRSIfParams.SetL( aValue );
            }
            break;

        case ECmIFNetworks:
        case EPacketDataIFNetworks:
            {
            serviceRecord->iGPRSIfNetworks.SetL( aValue );
            }
            break;

        case ECmIFAuthName:
        case EPacketDataIFAuthName:
            {
            serviceRecord->iGPRSIfAuthName.SetL( aValue );
            }
            break;

        case ECmIFAuthPass:
        case EPacketDataIFAuthPass:
            {
            serviceRecord->iGPRSIfAuthPass.SetL( aValue );
            }
            break;

        case ECmIPNetmask:
        case EPacketDataIPNetmask:
            {
            serviceRecord->iGPRSIPNetMask.SetL( aValue );
            }
            break;

        case ECmIPGateway:
        case EPacketDataIPGateway:
            {
            serviceRecord->iGPRSIPGateway.SetL( aValue );
            }
            break;

        case ECmIPAddress:
        case EPacketDataIPAddr:
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
            serviceRecord->iGPRSIPAddr.SetL( aValue );
            }
            break;

        case ECmIPNameServer1:
        case EPacketDataIPNameServer1:
            {
            serviceRecord->iGPRSIPNameServer1.SetL( aValue );
            }
            break;

        case ECmIPNameServer2:
        case EPacketDataIPNameServer2:
            {
            serviceRecord->iGPRSIPNameServer2.SetL( aValue );
            }
            break;

        case ECmIP6NameServer1:
        case EPacketDataIPIP6NameServer1:
            {
            serviceRecord->iGPRSIP6NameServer1.SetL( aValue );
            }
            break;

        case ECmIP6NameServer2:
        case EPacketDataIPIP6NameServer2:
            {
            serviceRecord->iGPRSIP6NameServer2.SetL( aValue );
            }
            break;

        case ECmIPAddrLeaseValidFrom:
        case EPacketDataIPAddrLeaseValidFrom:
            {
            serviceRecord->iGPRSIPAddrLeaseValidFrom.SetL( aValue );
            }
            break;

        case ECmIPAddrLeaseValidTo:
        case EPacketDataIPAddrLeaseValidTo:
            {
            serviceRecord->iGPRSIPAddrLeaseValidTo.SetL( aValue );
            }
            break;

        case ECmConfigDaemonManagerName:
        case EPacketDataConfigDaemonManagerName:
            {
            serviceRecord->iGPRSConfigDaemonManagerName.SetL( aValue );
            }
            break;

        case ECmConfigDaemonName:
        case EPacketDataConfigDaemonName:
            {
            serviceRecord->iGPRSConfigDaemonName.SetL( aValue );
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETBEARERSTRINGATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetBearerString8AttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetBearerString8AttributeL(
        TUint32 /*aAttribute*/,
        const TDesC8& /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_SETBEARERSTRING8ATTRIBUTEL_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_SETBEARERSTRING8ATTRIBUTEL_EXIT );
    return;
    }


// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetBearerInfoIntL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERINFOINTL_ENTRY );

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
            retVal = KCommDbBearerWcdma;
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

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmPluginPacketData::GetBearerInfoBoolL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERINFOBOOLL_ENTRY );

    TBool retVal( EFalse );
    switch ( aAttribute )
        {
        case ECmCoverage:
            {
#ifndef __WINS__
            CCmPDCoverageCheck* coverage = new (ELeave) CCmPDCoverageCheck;
            CleanupStack::PushL( coverage );

            retVal = coverage->IsThereCoverageL();

            CleanupStack::PopAndDestroy( coverage );
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

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }


// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginPacketData::GetBearerInfoStringL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERINFOSTRINGL_ENTRY );

    HBufC* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
        break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERINFOSTRINGL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerInfoString8L
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginPacketData::GetBearerInfoString8L( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERINFOSTRING8L_ENTRY );

    HBufC8* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERINFOSTRING8L_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::PrepareToCopyDataL( CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_PREPARETOCOPYDATAL_ENTRY );

    CCmPluginPacketData* plugin = static_cast<CCmPluginPacketData*>( aCopyInstance );
    plugin->iOutgoing = iOutgoing;

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_PREPARETOCOPYDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::CopyServiceRecordL
// ---------------------------------------------------------------------------
//
CommsDat::CCDRecordBase* CCmPluginPacketData::CopyServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_COPYSERVICERECORDL_ENTRY );

    // New service record to be returned.
    CCDRecordBase* serviceRecord = NULL;
    if ( iOutgoing )
        {
        serviceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdOutgoingGprsRecord ) );
        }
    else
        {
        serviceRecord = static_cast<CCDWCDMAPacketServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdIncomingGprsRecord ) );
        }
    CleanupStack::PushL( serviceRecord );

    CCDWCDMAPacketServiceRecord* tempServiceRecordPtrToNew =
            static_cast<CCDWCDMAPacketServiceRecord*>( serviceRecord );

    // CommsDat version of service record.
    CCDWCDMAPacketServiceRecord* origServiceRecord =
            static_cast<CCDWCDMAPacketServiceRecord*>( iServiceRecord );

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

    if ( !origServiceRecord->iGPRSAPN.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSAPN.SetL(
                origServiceRecord->iGPRSAPN );
        }
    if ( !origServiceRecord->iGPRSPDPType.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSPDPType.SetL(
                origServiceRecord->iGPRSPDPType );
        }
    if ( !origServiceRecord->iGPRSPDPAddress.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSPDPAddress.SetL(
                origServiceRecord->iGPRSPDPAddress );
        }
    if ( !origServiceRecord->iGPRSReqPrecedence.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSReqPrecedence.SetL(
                origServiceRecord->iGPRSReqPrecedence );
        }
    if ( !origServiceRecord->iGPRSReqDelay.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSReqDelay.SetL(
                origServiceRecord->iGPRSReqDelay );
        }
    if ( !origServiceRecord->iGPRSReqReliability.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSReqReliability.SetL(
                origServiceRecord->iGPRSReqReliability );
        }
    if ( !origServiceRecord->iGPRSReqPeakThroughput.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSReqPeakThroughput.SetL(
                origServiceRecord->iGPRSReqPeakThroughput );
        }
    if ( !origServiceRecord->iGPRSReqMeanThroughput.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSReqMeanThroughput.SetL(
                origServiceRecord->iGPRSReqMeanThroughput );
        }
    if ( !origServiceRecord->iGPRSMinPrecedence.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSMinPrecedence.SetL(
                origServiceRecord->iGPRSMinPrecedence );
        }
    if ( !origServiceRecord->iGPRSMinDelay.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSMinDelay.SetL(
                origServiceRecord->iGPRSMinDelay );
        }
    if ( !origServiceRecord->iGPRSMinReliability.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSMinReliability.SetL(
                origServiceRecord->iGPRSMinReliability );
        }
    if ( !origServiceRecord->iGPRSMinPeakThroughput.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSMinPeakThroughput.SetL(
                origServiceRecord->iGPRSMinPeakThroughput );
        }
    if ( !origServiceRecord->iGPRSMinMeanThroughput.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSMinMeanThroughput.SetL(
                origServiceRecord->iGPRSMinMeanThroughput );
        }
    if ( !origServiceRecord->iGPRSDataCompression.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSDataCompression.SetL(
                origServiceRecord->iGPRSDataCompression );
        }
    if ( !origServiceRecord->iGPRSHeaderCompression.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSHeaderCompression.SetL(
                origServiceRecord->iGPRSHeaderCompression );
        }
    if ( !origServiceRecord->iGPRSUseEdge.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSUseEdge.SetL(
                origServiceRecord->iGPRSUseEdge );
        }
    if ( !origServiceRecord->iGPRSAnonymousAccess.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSAnonymousAccess.SetL(
                origServiceRecord->iGPRSAnonymousAccess );
        }
    if ( !origServiceRecord->iGPRSIfParams.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfParams.SetL(
                origServiceRecord->iGPRSIfParams );
        }
    if ( !origServiceRecord->iGPRSIfNetworks.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfNetworks.SetL(
                origServiceRecord->iGPRSIfNetworks );
        }
    if ( !origServiceRecord->iGPRSIfPromptForAuth.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfPromptForAuth.SetL(
                origServiceRecord->iGPRSIfPromptForAuth );
        }
    if ( !origServiceRecord->iGPRSIfAuthName.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfAuthName.SetL(
                origServiceRecord->iGPRSIfAuthName );
        }
    if ( !origServiceRecord->iGPRSIfAuthPass.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfAuthPass.SetL(
                origServiceRecord->iGPRSIfAuthPass );
        }
    if ( !origServiceRecord->iGPRSIfAuthRetries.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIfAuthRetries.SetL(
                origServiceRecord->iGPRSIfAuthRetries );
        }
    if ( !origServiceRecord->iGPRSIPNetMask.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPNetMask.SetL(
                origServiceRecord->iGPRSIPNetMask );
        }
    if ( !origServiceRecord->iGPRSIPGateway.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPGateway.SetL(
                origServiceRecord->iGPRSIPGateway );
        }
    if ( !origServiceRecord->iGPRSIPAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPAddrFromServer.SetL(
                origServiceRecord->iGPRSIPAddrFromServer );
        }
    if ( !origServiceRecord->iGPRSIPAddr.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPAddr.SetL(
                origServiceRecord->iGPRSIPAddr );
        }
    if ( !origServiceRecord->iGPRSIPDNSAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPDNSAddrFromServer.SetL(
                origServiceRecord->iGPRSIPDNSAddrFromServer );
        }
    if ( !origServiceRecord->iGPRSIPNameServer1.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPNameServer1.SetL(
                origServiceRecord->iGPRSIPNameServer1 );
        }
    if ( !origServiceRecord->iGPRSIPNameServer2.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPNameServer2.SetL(
                origServiceRecord->iGPRSIPNameServer2 );
        }
    if ( !origServiceRecord->iGPRSIP6DNSAddrFromServer.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIP6DNSAddrFromServer.SetL(
                origServiceRecord->iGPRSIP6DNSAddrFromServer );
        }
    if ( !origServiceRecord->iGPRSIP6NameServer1.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIP6NameServer1.SetL(
                origServiceRecord->iGPRSIP6NameServer1 );
        }
    if ( !origServiceRecord->iGPRSIP6NameServer2.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIP6NameServer2.SetL(
                origServiceRecord->iGPRSIP6NameServer2 );
        }
    if ( !origServiceRecord->iGPRSIPAddrLeaseValidFrom.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPAddrLeaseValidFrom.SetL(
                origServiceRecord->iGPRSIPAddrLeaseValidFrom );
        }
    if ( !origServiceRecord->iGPRSIPAddrLeaseValidTo.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSIPAddrLeaseValidTo.SetL(
                origServiceRecord->iGPRSIPAddrLeaseValidTo );
        }
    if ( !origServiceRecord->iGPRSConfigDaemonManagerName.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSConfigDaemonManagerName.SetL(
                origServiceRecord->iGPRSConfigDaemonManagerName );
        }
    if ( !origServiceRecord->iGPRSConfigDaemonName.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSConfigDaemonName.SetL(
                origServiceRecord->iGPRSConfigDaemonName );
        }
    if ( !origServiceRecord->iGPRSEnableLCPExtension.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSEnableLCPExtension.SetL(
                origServiceRecord->iGPRSEnableLCPExtension );
        }
    if ( !origServiceRecord->iGPRSDisablePlainTextAuth.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSDisablePlainTextAuth.SetL(
                origServiceRecord->iGPRSDisablePlainTextAuth );
        }
    if ( !origServiceRecord->iGPRSAPType.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSAPType.SetL(
                origServiceRecord->iGPRSAPType );
        }
    if ( !origServiceRecord->iGPRSQOSWarningTimeOut.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSQOSWarningTimeOut.SetL(
                origServiceRecord->iGPRSQOSWarningTimeOut );
        }
    if ( !origServiceRecord->iUmtsR99QoSAndOnTable.IsNull() )
        {
        tempServiceRecordPtrToNew->iUmtsR99QoSAndOnTable.SetL(
                origServiceRecord->iUmtsR99QoSAndOnTable );
        }
    if ( !origServiceRecord->iGPRSR5DataCompression.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSR5DataCompression.SetL(
                origServiceRecord->iGPRSR5DataCompression );
        }
    if ( !origServiceRecord->iGPRSR5HeaderCompression.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSR5HeaderCompression.SetL(
                origServiceRecord->iGPRSR5HeaderCompression );
        }
    if ( !origServiceRecord->iGPRSPacketFlowIdentifier.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSPacketFlowIdentifier.SetL(
                origServiceRecord->iGPRSPacketFlowIdentifier );
        }
    if ( !origServiceRecord->iGPRSUmtsGprsRelease.IsNull() )
        {
        tempServiceRecordPtrToNew->iGPRSUmtsGprsRelease.SetL(
                origServiceRecord->iGPRSUmtsGprsRelease );
        }

    CleanupStack::Pop( serviceRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_COPYSERVICERECORDL_EXIT );
    return serviceRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::ResetBearerRecords
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::ResetBearerRecords()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_RESETBEARERRECORDS_ENTRY );

    delete iPacketDataQoSRecord;
    iPacketDataQoSRecord = NULL;

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_RESETBEARERRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::DeleteBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::DeleteBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_DELETEBEARERRECORDSL_ENTRY );

    if ( !TPtrC(iPacketDataQoSRecord->iRecordName).CompareF( KDefaultQosDataRecordName ) ||
            !TPtrC(iPacketDataQoSRecord->iRecordName).CompareF( KDefaultQosDataRecordNamewithSpace ) )
        {
        OstTraceFunctionExit0( CCMPLUGINPACKETDATA_DELETEBEARERRECORDSL_EXIT );
        return;
        }

    iPacketDataQoSRecord->DeleteL( iSession );

    OstTraceFunctionExit0( DUP1_CCMPLUGINPACKETDATA_DELETEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::CopyBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_COPYBEARERRECORDSL_ENTRY );

    CCmPluginPacketData* plugin = static_cast<CCmPluginPacketData*>( aCopyInstance );

    CCDUmtsR99QoSAndOnTableRecord* qosRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdUmtsR99QoSAndOnTableRecord ) );
    CleanupStack::PushL( qosRecord );
    CopyRecordFieldsL( *iPacketDataQoSRecord, *qosRecord );
    CleanupStack::Pop( qosRecord );
    plugin->iPacketDataQoSRecord = qosRecord;

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_COPYBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetBearerTableIdsToBeObserved
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::GetBearerTableIdsToBeObservedL(
        RArray<TUint32>& aTableIdArray ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETBEARERTABLEIDSTOBEOBSERVEDL_ENTRY );

    aTableIdArray.AppendL( KCDTIdOutgoingGprsRecord );
    aTableIdArray.AppendL( KCDTIdUmtsR99QoSAndOnTableRecord );

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETBEARERTABLEIDSTOBEOBSERVEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::GetDefPriorityL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetDefPriorityL( const TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINPACKETDATA_GETDEFPRIORITYL_ENTRY );

    TUint32 retVal( KDataMobilitySelectionPolicyPriorityWildCard );

    CCDGlobalBearerTypePriorizationRecord* priorityRecord =
            new( ELeave ) CCDGlobalBearerTypePriorizationRecord( iBearerPriorityTableId );
    CleanupStack::PushL( priorityRecord );

    priorityRecord->iServiceType.SetL( TPtrC( KCDTypeNameOutgoingWCDMA ) );

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

    OstTraceFunctionExit0( CCMPLUGINPACKETDATA_GETDEFPRIORITYL_EXIT );
    return retVal;
    }

// End of file
