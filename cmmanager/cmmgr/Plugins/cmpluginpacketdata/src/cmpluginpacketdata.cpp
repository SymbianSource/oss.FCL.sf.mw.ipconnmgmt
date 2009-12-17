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
* Description:  Dialog for editing settings for a packet data connection 
*
*/


// INCLUDE FILES

// System
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>
#include <cmpluginpacketdata.mbg>           // icons
#include <cmpacketdataui.rsg>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <cmmanager.rsg>
// User
#include "cmpluginpacketdata.h"
#include <cmpluginpacketdatadef.h>
#include "cmpacketdatacommonconstants.h"
#include "cmlogger.h"
#include "cmppacketdatasettingsdlg.h"
#include <cmpsettingsconsts.h>
#include "datamobilitycommsdattypes.h"
#include <cmcommonconstants.h>
#include <cmmanagerdef.h>

using namespace CMManager;
using namespace CommsDat;

// ================= CONSTANTS =======================

const TUint32 KDefaultPriorityPacketData = 1;

/// Modem bearer names for GPRS/WCDMA Access Points
_LIT( KModemBearerPacketData, "GPRS Modem" );
_LIT( KPacketDataFileIcons, "z:cmpluginpacketdata.mbm" );
_LIT( KDefaultQosDataRecordName, "PRIMARY1" );
_LIT( KDefaultQosDataRecordNamewithSpace, "PRIMARY1 " );

// ================= LOCAL PROTOTYPES =======================

/**
* Function to check and set PDP type and IfNetwork.
* @param aThis 'this' pointer of the plugin instance
* @param aAttribute attribute client wants to set
* @param aValue TInt* IPv4 address buffer to be checked
*/
static TBool SetPDPTypeL( CCmPluginBaseEng* aThis, 
                   TUint32 aAttribute, 
                   const TAny* aValue );

static const TCmAttribConvTable SPacketDataConvTbl[] = 
    {
    // iAttribId, iCommsDatId, iValidFuncL, 
    // iTitleId, 
    // iMaxLength, iAttribFlags, iDefValueResId, iEditorResId, iNotUsed1, iNotUsed2
    { EPacketDataAPName, EGPRSReqTrafficClass-1, NULL,
      0,
      0, 0, 0, NULL, NULL },
    { EPacketDataAPName, KCDTIdAPN, NULL,
      R_GPRS_PLUGIN_VIEW_AP_NAME,
      KMaxGprsApNameLength, 0, R_GPRS_PLUGIN_SETT_VAL_FIELD_NONE, R_TEXT_SETTING_PAGE_GPRSAP_NAME, NULL, NULL },
    { EPacketDataPDPType, KCDTIdWCDMPDPType, &SetPDPTypeL,
      R_GPRS_PLUGIN_VIEW_PDP_TYPE,
      0, 0, 0, NULL, NULL },
    { EPacketDataPDPAddress, KCDTIdWCDMAPDPAddress, NULL },
    { EPacketDataReqPrecedence, KCDTIdReqPrecedence, NULL },
    { EPacketDataReqDelay, KCDTIdReqDelay, NULL },
    { EPacketDataReliability, KCDTIdReqReliability, NULL },
    { EPacketDataPeakThroughput, KCDTIdReqPeakThroughput, NULL },
    { EPacketDataMeanThroughput, KCDTIdReqMeanThroughput, NULL },
    { EPacketDataMinPrecedence, KCDTIdMinPrecedence, NULL },
    { EPacketDataMinDelay, KCDTIdMinDelay, NULL },
    { EPacketDataMinReliability, KCDTIdMinReliability, NULL },
    { EPacketDataMinPeakThroughput, KCDTIdMinPeakThroughput, NULL },
    { EPacketDataMinMeanThroughput, KCDTIdMinMeanThroughput, NULL },
    { EPacketDataDataCompression, KCDTIdWCDMADataCompression, NULL },
    { EPacketDataHeaderCompression, KCDTIdWCDMAHeaderCompression, NULL },
    { EPacketDataUseEdge, KCDTIdWCDMAUseEdge, NULL },
    { EPacketDataAnonymousAccess, KCDTIdWCDMAAnonymousAccess, NULL },
    { EPacketDataIFParams, KCDTIdWCDMAIfParams, NULL },
    { EPacketDataIFNetworks, KCDTIdWCDMAIfNetworks, NULL },
    { EPacketDataIFPromptForAuth, KCDTIdWCDMAIfPromptForAuth, NULL,
      R_QTN_SET_PROMPT_PASSWD,
      0, 0, 0, NULL, NULL },
    { EPacketDataIFAuthName, KCDTIdWCDMAIfAuthName, NULL,
      R_QTN_SET_AP_USERNAME,
      KMaxLoginNameLength, 0, R_QTN_SET_AP_USERNAME_NONE, R_TEXT_SETTING_PAGE_LOGIN_NAME, NULL, NULL },
    { EPacketDataIFAuthPass, KCDTIdWCDMAIfAuthPass, NULL,
      R_QTN_SET_AP_PASSWD,
      KCmMaxPasswordLength, EConvPassword, R_AVKON_NUMERIC_PASSWORD_BLANKING_TEXT, NULL, NULL },
    { EPacketDataIFAuthRetries, KCDTIdWCDMAIfAuthRetries, NULL },
    { EPacketDataIPNetmask, KCDTIdWCDMAIPNetMask, &CheckIPv4ValidityL },
    { EPacketDataIPGateway, KCDTIdWCDMAIPGateway, &CheckIPv4ValidityL },
    { EPacketDataIPAddrFromServer, KCDTIdWCDMAIPAddrFromServer, NULL },
    { EPacketDataIPAddr, KCDTIdWCDMAIPAddr, &CheckIPv4ValidityL,
      R_QTN_SET_IP_ADDRESS,
      KIpAddressWidth, EConvIPv4, R_QTN_SET_IP_ADDRESS_DYNAMIC, 0, NULL, NULL },
    { EPacketDataIPDNSAddrFromServer, KCDTIdWCDMAIPDNSAddrFromServer, NULL,
      R_QTN_SET_DNS_SERVERS_IP,
      0, 0, 0, 0, NULL, NULL },
    { EPacketDataIPNameServer1, KCDTIdWCDMAIPNameServer1, &CheckIPv4ValidityL,
      R_QTN_SET_PRIMARY_DNS,
      KMaxIPv4NameServerLength, EConvIPv4, R_QTN_SET_DNS_SERVERS_AUTOMATIC, 0, NULL, NULL },
    { EPacketDataIPNameServer2, KCDTIdWCDMAIPNameServer2, &CheckIPv4ValidityL,
      R_QTN_SET_SECONDARY_DNS,
      KMaxIPv4NameServerLength, EConvIPv4, R_QTN_SET_DNS_SERVERS_AUTOMATIC, 0, NULL, NULL },
    { EPacketDataIPIP6DNSAddrFromServer, KCDTIdWCDMAIP6DNSAddrFromServer, NULL,
      R_QTN_SET_DNS_SERVERS_IP,
      0, 0, 0, NULL, NULL },
    { EPacketDataIPIP6NameServer1, KCDTIdWCDMAIP6NameServer1, &CheckIPv6ValidityL },
    { EPacketDataIPIP6NameServer2, KCDTIdWCDMAIP6NameServer2, &CheckIPv6ValidityL },
    { EPacketDataIPAddrLeaseValidFrom, KCDTIdWCDMAIPAddrLeaseValidFrom, NULL },
    { EPacketDataIPAddrLeaseValidTo, KCDTIdWCDMAIPAddrLeaseValidTo, NULL },
    { EPacketDataConfigDaemonManagerName, KCDTIdWCDMAConfigDaemonManagerName, NULL },
    { EPacketDataConfigDaemonName, KCDTIdWCDMAConfigDaemonName, NULL },
    { EPacketDataEnableLcpExtension, KCDTIdWCDMAEnableLCPExtensions, NULL },
    { EPacketDataDisablePlainTextAuth, KCDTIdWCDMADisablePlainTextAuth, NULL,
      R_QTN_SET_PASSWD_AUT,
      0, 0, 0, NULL, NULL },
    { EPacketDataApType, KCDTIdAPType, NULL },
    { EPacketDataQoSWarningTimeOut, KCDTIdQOSWarningTimeOut, NULL },
    { EPacketDataServiceEnableLLMNR, KCDTIdServiceEnableLlmnr, NULL },
    { 0, 0, NULL }
};

static const TCmAttribConvTable SQoSDataConvTbl[] =
    {
    // iAttribId, iCommsDatId, iValidFuncL, 
    // iTitleId, 
    // iMaxLength, iAttribFlags, iDefValueResId, iEditorResId, iNotUsed1, iNotUsed2
    { EGPRSReqTrafficClass, EPacketDataRangeMax, NULL },
    { EGPRSReqTrafficClass, KCDTIdWCDMAReqTrafficClass, NULL },
    { EGPRSMinTrafficClass, KCDTIdWCDMAMinTrafficClass, NULL },
    { EGPRSReqDeliveryOrder, KCDTIdWCDMAReqDeliveryOrder, NULL },
    { GPRSMinDeliveryOrder, KCDTIdWCDMAMinDeliveryOrder, NULL },
    { EGPRSReqDeliverErroneousSDU, KCDTIdWCDMAReqDeliverErroneousSDU, NULL },
    { EGPRSMinDeliverErroneousSDU, KCDTIdWCDMAMinDeliverErroneousSDU, NULL },
    { EGPRSReqMaxSDUSize, KCDTIdWCDMAReqMaxSDUSize, NULL },
    { EGPRSMinAcceptableMaxSDUSize, KCDTIdWCDMAMinAcceptableMaxSDUSize, NULL },
    { EGPRSReqMaxUplinkRate, KCDTIdWCDMAReqMaxUplinkRate, NULL },
    { EGPRSReqMinUplinkRate, KCDTIdWCDMAReqMinUplinkRate, NULL },
    { EGPRSReqMaxDownlinkRate, KCDTIdWCDMAReqMaxDownlinkRate, NULL },
    { EGPRSReqMinDownlinkRate, KCDTIdWCDMAReqMinDownlinkRate, NULL },
    { EGPRSReqBER, KCDTIdWCDMAReqBER, NULL },
    { EGPRSMaxBER, KCDTIdWCDMAMaxBER, NULL },
    { EGPRSReqSDUErrorRatio, KCDTIdWCDMAReqSDUErrorRatio, NULL },
    { EGPRSMaxSDUErrorRatio, KCDTIdWCDMAMaxSDUErrorRatio, NULL },
    { EGPRSReqTrafficHandlingPriority, KCDTIdWCDMAReqTrafficHandlingPriority, NULL },
    { EGPRSMinTrafficHandlingPriority, KCDTIdWCDMAMinTrafficHandlingPriority, NULL },
    { EGPRSReqTransferDelay, KCDTIdWCDMAReqTransferDelay, NULL },
    { EGPRSMaxTransferDelay, KCDTIdWCDMAMaxTransferDelay, NULL },
    { EGPRSReqGuaranteedUplinkRate, KCDTIdWCDMAReqGuaranteedUplinkRate, NULL },
    { EGPRSMinGuaranteedUplinkRate, KCDTIdWCDMAMinGuaranteedUplinkRate, NULL },
    { EGPRSReqGuaranteedDownlinkRate, KCDTIdWCDMAReqGuaranteedDownlinkRate, NULL },
    { EGPRSMinGuaranteedDownlinkRate, KCDTIdWCDMAMinGuaranteedDownlinkRate, NULL },
    { EGPRSSignallingIndication, KCDTIdWCDMASignallingIndication, NULL },
    { EGPRS_ImCnSignallingIndication, KCDTIdWCDMAImCmSignallingIndication, NULL },
    { EGPRSSourceStatisticsDescriptor, KCDTIdWCDMASourceStatisticsDescriptor, NULL },
    { 0, 0, NULL }
    };

static const TCmCommonAttrConvArrayItem SCommonConvTbl[] =
{
    { EPacketDataIFParams, ECmIFParams },
    { EPacketDataIFNetworks, ECmIFNetworks },
    { EPacketDataIFPromptForAuth, ECmIFPromptForAuth },
    { EPacketDataIFAuthName, ECmIFAuthName },
    { EPacketDataIFAuthPass, ECmIFAuthPass },
    { EPacketDataIFAuthRetries, ECmIFAuthRetries },
    { EPacketDataIPNetmask, ECmIPNetmask },
    { EPacketDataIPGateway, ECmIPGateway },
    { EPacketDataIPAddrFromServer, ECmIPAddFromServer },
    { EPacketDataIPAddr, ECmIPAddress },
    { EPacketDataIPDNSAddrFromServer, ECmIPDNSAddrFromServer },
    { EPacketDataIPNameServer1, ECmIPNameServer1 },
    { EPacketDataIPNameServer2, ECmIPNameServer2 },
    { EPacketDataIPIP6DNSAddrFromServer, ECmIP6DNSAddrFromServer },
    { EPacketDataIPIP6NameServer1, ECmIP6NameServer1 },
    { EPacketDataIPIP6NameServer2, ECmIP6NameServer2 },
    { EPacketDataIPAddrLeaseValidFrom, ECmIPAddrLeaseValidFrom },
    { EPacketDataIPAddrLeaseValidTo, ECmIPAddrLeaseValidTo },
    { EPacketDataConfigDaemonManagerName, ECmConfigDaemonManagerName },
    { EPacketDataConfigDaemonName, ECmConfigDaemonName },
    { EPacketDataEnableLcpExtension, ECmEnableLPCExtension },
    { EPacketDataDisablePlainTextAuth, ECmDisablePlainTextAuth },
    { 0, 0 }
    };
    
// ======== LOCAL FUNCTIONS ========
    
static TBool SetPDPTypeL( CCmPluginBaseEng* aThis, 
                          TUint32 aAttribute, 
                          const TAny* aValue )
    {
    (void)aAttribute;
    CCmPluginPacketData* myThis = static_cast<CCmPluginPacketData*>( aThis );
    
    myThis->SetPDPTypeL( (RPacketContext::TProtocolType)(TInt)aValue );
    
    return ETrue;
    }

// ======== MEMBER FUNCTIONS ========

class CCmPDCoverageCheck : public CActive
    {
    enum EPacketCoverageState
        {
        EServiceStatus,
        EPDPAttach,
        EPDPDetach,                        
        };
        
    public:
    
        CCmPDCoverageCheck();
        ~CCmPDCoverageCheck();
        
        TBool IsThereCoverageL();
        
    protected:  // from CActive
    
        virtual void DoCancel();
        virtual void RunL();

    private:

        TUint32                 iProgState;
        TBool                   iCoverage;
        CActiveSchedulerWait    iWait;    
        RTelServer              iServer;
        RPhone                  iPhone;
        RPacketService          iService;        
    };

// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::CCmPDCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmPDCoverageCheck::CCmPDCoverageCheck() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::~CCmPDCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmPDCoverageCheck::~CCmPDCoverageCheck()
    {
    Cancel();

    iService.Close();
    iPhone.Close();
    iServer.Close();
    }
    
// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::DoCancel
// ----------------------------------------------------------------------------
//
void CCmPDCoverageCheck::DoCancel()
    {
    iWait.AsyncStop();
    }
    
// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::RunL
// ----------------------------------------------------------------------------
//
void CCmPDCoverageCheck::RunL()
    {
    switch( iProgState )
        {
        case EPDPAttach:
            {
            if( !iStatus.Int() )
                // PDP context created -> there's a usable PD coverage.
                {
                iService.Detach( iStatus );
                SetActive();

                iCoverage = ETrue;
                iProgState = EPDPDetach;
                }
            else
                // something went wrong -> no coverage.
                {
                iWait.AsyncStop();
                }
            }
            break;
            
        case EPDPDetach:
            {
            iWait.AsyncStop();
            }
            break;
            
        default:
            {
            User::Leave( KErrCorrupt );
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CCmPDCoverageCheck::IsThereCoverageL
// ----------------------------------------------------------------------------
//
TBool CCmPDCoverageCheck::IsThereCoverageL()
    {
    iProgState = EServiceStatus;
    iCoverage = EFalse;
    
    User::LeaveIfError( iServer.Connect() );
    CLOG_WRITE( "Server open" );
    
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( iServer.GetPhoneInfo( 0, info ) );
    CLOG_WRITE( "Phone info ok" );
    
    User::LeaveIfError( iPhone.Open(iServer, info.iName ) );
    CLOG_WRITE( "Phone open" );

    User::LeaveIfError( iService.Open( iPhone ) );
    CLOG_WRITE( "service ok" );

    RPacketService::TStatus status;
    User::LeaveIfError( iService.GetStatus( status ) );
    if( status == RPacketService::EStatusAttached ||
        status == RPacketService::EStatusActive || 
        status == RPacketService::EStatusSuspended )
        // Attached/active/suspened, so there's a coverage
        {
        iCoverage = ETrue;
        }
    else if( status == RPacketService::EStatusUnattached )
        {
        iProgState = EPDPAttach;
        iService.Attach( iStatus );
        SetActive();
        iWait.Start();
        }
    else
        {
        CLOG_WRITE( "Unknown state" );
        }
        

    User::LeaveIfError( iStatus.Int() );
    
    return iCoverage;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::NewOutgoingL
// ----------------------------------------------------------------------------
//
CCmPluginPacketData* CCmPluginPacketData::NewL(
                                            TCmPluginInitParam* aInitParam )
	{
	CCmPluginPacketData* self = new ( ELeave ) CCmPluginPacketData( 
	                                                    aInitParam, ETrue );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::~CCmPluginPacketData
// ----------------------------------------------------------------------------
//
CCmPluginPacketData::~CCmPluginPacketData()
	{
	CCmPluginPacketData::AdditionalReset();	
	RemoveResourceFile( KPluginPacketDataResDirAndFileName );	
	CLOG_CLOSE;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginPacketData::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginPacketData* self = new( ELeave ) CCmPluginPacketData( &aInitParam, ETrue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------------------------------
// CCmPluginPacketData::CCmPluginPacketData
// ----------------------------------------------------------------------------
//
CCmPluginPacketData::CCmPluginPacketData( TCmPluginInitParam* aInitParam,
                                          TBool aOutgoing )
    : CCmPluginBaseEng( aInitParam )
    , iOutgoing( aOutgoing )
	{
	CLOG_CREATE;
	
	iBearerType = KUidPacketDataBearerType;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ConstructL
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::ConstructL()
	{
	CCmPluginBaseEng::ConstructL();

    AddResourceFileL( KPluginPacketDataResDirAndFileName );
	AddConverstionTableL( (CCDRecordBase**)&iServiceRecord, NULL, SPacketDataConvTbl );
    AddConverstionTableL( (CCDRecordBase**)&iPacketDataQoSRecord, NULL, SQoSDataConvTbl );
	AddCommonConversionTableL( SCommonConvTbl );
	}
	
// ----------------------------------------------------------------------------
// CCmPluginPacketData::GetIntAttributeL()
// ----------------------------------------------------------------------------
//
TUint32 CCmPluginPacketData::GetIntAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::GetIntAttributeL" );

    TUint32 retVal( 0 );
    
    switch( aAttribute )
        {
        case ECmBearerIcon:
            {
            TAknsItemID id;
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPacketDataFileIcons, 
                                                    &KDC_BITMAP_DIR, NULL ) );

                   
            retVal = (TUint32)AknsUtils::CreateGulIconL( 
                                    skinInstance, 
                                    id,
                                    mbmFile.FullName(), 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs, 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs_mask );
            }
            break;
               
        case ECmBearerAvailableIcon:
            {
            TAknsItemID id;
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPacketDataFileIcons, &KDC_BITMAP_DIR, NULL ) );
                   
            retVal = (TUint32)AknsUtils::CreateGulIconL( 
                    skinInstance, 
                    id,
                    mbmFile.FullName(), 
                    EMbmCmpluginpacketdataQgn_prop_set_conn_bearer_avail_packetdata, 
                    EMbmCmpluginpacketdataQgn_prop_set_conn_bearer_avail_packetdata_mask );
            }
            break;
            
        case ECmCommsDBBearerType:
            {
            retVal = KCommDbBearerWcdma;
            }
            break;
            
        case ECmDefaultUiPriority:
        case ECmDefaultPriority:
            {
            TPtrC buf;
            
            if( iOutgoing )
                {
                buf.Set( KCDTypeNameOutgoingWCDMA );
                }
            else
                {
                buf.Set( KCDTypeNameIncomingWCDMA );
                }

            retVal = aAttribute == ECmDefaultPriority ?
                     GlobalBearerPriority( buf ) :
                     GlobalUiBearerPriority( buf );
            
            if( retVal == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                retVal = KDefaultPriorityPacketData;
                }
            }
            break;
            
        default:
            {
            retVal = CCmPluginBaseEng::GetIntAttributeL( aAttribute );
            }
            break;
        }
        
    return retVal;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::GetBoolAttributeL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::GetBoolAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::GetBoolAttributeL" );

    TBool retVal( EFalse );

    switch( aAttribute )
        {
        case EPacketDataOutGoing:
            {
            retVal = iOutgoing;
            }
            break;
            
        case ECmCoverage:
            // In default the plugin has no network coverage
            {
            retVal = CheckNetworkCoverageL();
            }
            break;
            
        case ECmAddToAvailableList:
            {
            retVal = ETrue;
            }
            break;
            
        case ECmBearerHasUi:
            {
            retVal = ETrue;
            }
            break;
            
        default:
            {
            retVal = CCmPluginBaseEng::GetBoolAttributeL( aAttribute );
            }
        }
        
    return retVal;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::GetStringAttributeL()
// ----------------------------------------------------------------------------
//
HBufC* CCmPluginPacketData::GetStringAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::GetStringAttributeL" );

	HBufC* retVal = NULL;
	
	switch( aAttribute )
	    {
	    case ECmBearerAvailableName:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_BEARER_AVAILABLE_PACKET_DATA );
	        }
	        break;
	        
	    case ECmBearerSupportedName:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_BEARER_SUPPORTED_PACKET_DATA );
	        }
	        break;
	        
        case ECmBearerAvailableText:
            {
            retVal = AllocReadL( R_QTN_NETW_CONSET_PACKET_DATA_AVAILABLE );
            }
            break;
            
	    case ECmBearerNamePopupNote:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_POPUP_BEARER_PACKET_DATA );
	        }
	        break;

	    case ECmBearerSettingName:
	        {
	        retVal = AllocReadL( R_QTN_SET_BEARER_PACKET_DATA );
	        }
	        break;
	        
	    default:
	        {
	        retVal = CCmPluginBaseEng::GetStringAttributeL( aAttribute );
	        }
	        break;
	    }
	    
    return retVal;
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::SetIntAttributeL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::SetIntAttributeL( const TUint32 aAttribute, 
                                            TUint32 aValue )
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::SetIntAttributeL" );

    switch( aAttribute )
        {
        default:
            {
            CCmPluginBaseEng::SetIntAttributeL( aAttribute, aValue );
            }
        }
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::SetBoolAttributeL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::SetBoolAttributeL( const TUint32 aAttribute, 
                                             TBool aValue )
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::SetBoolAttributeL" );

    switch( aAttribute )
        {
        case EPacketDataOutGoing:
            {
            iOutgoing = aValue;
            CreateNewServiceRecordL();
            }
            break;
        
        default:
            {
            CCmPluginBaseEng::SetBoolAttributeL( aAttribute, aValue );
            }
        }
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::SetStringAttributeL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::SetStringAttributeL( const TUint32 aAttribute, 
                                               const TDesC16& aValue )
	{
    LOGGER_ENTERFN( "CCmPluginPacketData::SetStringAttributeL" );

    switch( aAttribute )
        {
        default:
            {
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            }
        }
	}

// ----------------------------------------------------------------------------
// CCmPluginPacketData::PrepareToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::PrepareToUpdateRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::PrepareToUpdateRecordsL" );

    CheckDNSServerAddressL( ETrue, 
                            ServiceRecord().iGPRSIP6NameServer1,
                            ServiceRecord().iGPRSIP6NameServer2,
                            ServiceRecord().iGPRSIP6DNSAddrFromServer );
                            
    CheckDNSServerAddressL( EFalse,
                            ServiceRecord().iGPRSIPNameServer1,
                            ServiceRecord().iGPRSIPNameServer2,
                            ServiceRecord().iGPRSIPDNSAddrFromServer );
    SetDaemonNameL();
    }
    
// ----------------------------------------------------------------------------
// CCmPluginPacketData::CanHandleIapIdL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::CanHandleIapIdL( TUint32 aIapId ) const
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::CanHandleIapIdL1" );
    CLOG_WRITE_1( "IapId: [%d]", aIapId );
    
    TBool retVal( EFalse );
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                            (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
        
    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );
    
    TRAPD( err, iapRecord->LoadL( Session() ));

    if( !err )
        {
        CanHandleIapIdL( iapRecord );
        }
    
    CleanupStack::PopAndDestroy( iapRecord );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CanHandleIapIdL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginPacketData::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::CanHandleIapIdL2" );
    CLOG_WRITE_1( "IapId: [%d]", aIapRecord->RecordId() );
    
    TBool retVal( EFalse );
    
    CLOG_WRITE_2( "IAP record: [%S][%S]",
                    &FIELD_TO_TDESC(aIapRecord->iServiceType), 
                    &FIELD_TO_TDESC(aIapRecord->iBearerType)
                );
    
    if( (TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameOutgoingWCDMA) ||
        TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameIncomingWCDMA)) &&
        TPtrC(aIapRecord->iBearerType) == TPtrC(KCDTypeNameModemBearer) )
        {
        CLOG_WRITE( "I can." );
        retVal = ETrue;
        }

    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::RunSettingsL()
// ----------------------------------------------------------------------------
//        
TInt CCmPluginPacketData::RunSettingsL()
	{
    CmPluginPacketDataSettingsDlg* settingsDlg = 
                                CmPluginPacketDataSettingsDlg::NewL( *this );
    return settingsDlg->ConstructAndRunLD( );       
    }        

// ----------------------------------------------------------------------------
// CCmPluginPacketData::LoadServiceSettingL()
// ----------------------------------------------------------------------------
//        
void CCmPluginPacketData::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::LoadServiceSettingL" );
    
    if( TPtrC(KCDTypeNameOutgoingWCDMA) == iIapRecord->iServiceType  )
        {
        iServiceRecord = static_cast<CCDServiceRecordBase *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdOutgoingGprsRecord));
        iOutgoing = ETrue;
        }
    else if( TPtrC(KCDTypeNameIncomingWCDMA) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDServiceRecordBase *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdIncomingGprsRecord));
        iOutgoing = EFalse;
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }

    CCmPluginBaseEng::LoadServiceSettingL();

    CLOG_WRITE_1( "APName: [%S]", &FIELD_TO_TDESC( ServiceRecord().iGPRSAPN ) );
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::InitializeWithUiL()
// ----------------------------------------------------------------------------
//        
TBool CCmPluginPacketData::InitializeWithUiL( TBool /*aManuallyConfigure*/ )
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::InitializeWithUiL" );    
    TBool retval = ETrue;   // meaning that everything was fine.

    // Add resource file for the duration of this method only.
    TParse parser;
    User::LeaveIfError( parser.Set( KPluginPacketDataResDirAndFileName, 
                                    &KDC_RESOURCE_FILES_DIR, 
                                    NULL ) );

    TFileName resourceFileNameBuf = parser.FullName();

    RConeResourceLoader resLoader( *CEikonEnv::Static() ) ;
    resLoader.OpenL( resourceFileNameBuf );
    CleanupClosePushL( resLoader );

    // Show dialog
    TBuf<KMaxGprsApNameLength> buf;
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog( buf );

    // Must use PrepareLC instead of ExecuteLD in order for
    // MakeLeftSoftkeyVisible() call to work.
    dlg->PrepareLC( R_APN_NAME_QUERY );

    dlg->SetMaxLength( KMaxGprsApNameLength );
    dlg->MakeLeftSoftkeyVisible( ETrue );   // Empty input accepted.

    HBufC* prompt = StringLoader::LoadLC( R_GPRS_PLUGIN_APN_PROMPT );
    dlg->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    // Strange, but true: RunLD returns 0 (instead of the actual command id)
    // if Cancel button was pressed, thus we have to check against zero here.
    if ( dlg->RunLD() )
        {
        // Change APN attribute in connection method
        SetStringAttributeL( EPacketDataAPName, buf );
        
        if( buf.Length() )
            {
            SetStringAttributeL( ECmName, buf );
            }
        }
    else
        {
        retval = EFalse;    // indicating cancellation.
        }

    // Clean-up
    CleanupStack::PopAndDestroy( &resLoader );

    return retval;
    }


// ----------------------------------------------------------------------------
// CCmPluginPacketData::ServiceRecord()
// ----------------------------------------------------------------------------
//        
CCDWCDMAPacketServiceRecord& CCmPluginPacketData::ServiceRecord() const
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::ServiceRecord" );

    return *static_cast<CCDWCDMAPacketServiceRecord*>(iServiceRecord);
    }
    
// ----------------------------------------------------------------------------
// CCmPluginPacketData::CreateNewServiceRecordL()
// ----------------------------------------------------------------------------
//        
void CCmPluginPacketData::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::CreateNewServiceRecordL" );

    delete iServiceRecord; iServiceRecord = NULL;
    
    if( iOutgoing )
        {
        iServiceRecord = static_cast<CCDServiceRecordBase *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdOutgoingGprsRecord));
        }
    else
        {
        iServiceRecord = static_cast<CCDServiceRecordBase *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdIncomingGprsRecord));
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
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::ServiceRecordIdLC
// ----------------------------------------------------------------------------
//
void CCmPluginPacketData::ServiceRecordIdLC( HBufC* &aName, 
                                             TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::ServiceRecordIdLC" );

    if( iOutgoing )
        {
        aName = TPtrC( KCDTypeNameOutgoingWCDMA ).AllocLC();
        }
    else
        {
        aName = TPtrC( KCDTypeNameIncomingWCDMA ).AllocLC();
        }
        
    aRecordId = iServiceRecord->RecordId();
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::BearerRecordIdLC()
// ----------------------------------------------------------------------------
//        
void CCmPluginPacketData::BearerRecordIdLC( HBufC* &aBearerName, 
                                            TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::BearerRecordIdLC" );

	CMDBRecordSet<CCDModemBearerRecord>* bearerRS = 
	        new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
	CleanupStack::PushL( bearerRS );
    
    CCDModemBearerRecord* bearerRecord = static_cast<CCDModemBearerRecord *>
                            (CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
                                    
    CleanupStack::PushL( bearerRecord );
    
    bearerRecord->iRecordName.SetL( KModemBearerPacketData );
    bearerRS->iRecords.AppendL( bearerRecord );
    CleanupStack::Pop( bearerRecord );
    
    if( bearerRS->FindL( Session() ) )
        {
        CLOG_WRITE_1( "Bearers: [%d]", bearerRS->iRecords.Count() );

        bearerRecord = static_cast<CCDModemBearerRecord*>(bearerRS->iRecords[0]);
        aRecordId = bearerRecord->RecordId();
        }
    else
        // bearer not found -> create dummy values
        {
        CLOG_WRITE( "No bearer record found" );

        bearerRecord->SetRecordId( KCDNewRecordRequest );
        bearerRecord->StoreL( Session() );
        
        aRecordId = bearerRecord->RecordId();
        }

    CleanupStack::PopAndDestroy( bearerRS );

    aBearerName = TPtrC(KCDTypeNameModemBearer).AllocLC();
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::CheckNetworkCoverageL()
// ----------------------------------------------------------------------------
//        
TBool CCmPluginPacketData::CheckNetworkCoverageL() const
    {
    LOGGER_ENTERFN("CCmPluginPacketData::CheckNetworkCoverageL");
    
    TBool retVal( EFalse );
    
#ifdef __WINS__
    retVal = ETrue;
#else
    CCmPDCoverageCheck* coverage = new (ELeave) CCmPDCoverageCheck;
    CleanupStack::PushL( coverage );
    
    retVal = coverage->IsThereCoverageL();
    
    CleanupStack::PopAndDestroy( coverage );
    
#endif  // __WINS
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCmPluginPacketData::SetPDPTypeL()
// ----------------------------------------------------------------------------
//        
void CCmPluginPacketData::SetPDPTypeL( RPacketContext::TProtocolType aPdpType )
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::SetPDPTypeL" );

    if( aPdpType != RPacketContext::EPdpTypeIPv4 &&
        aPdpType != RPacketContext::EPdpTypeIPv6 )
        {
        User::Leave( KErrArgument );
        }
        
    if( FeatureSupported( KFeatureIdIPv6 ) && aPdpType == 
        RPacketContext::EPdpTypeIPv6 )
        {
        ServiceRecord().iGPRSPDPType = RPacketContext::EPdpTypeIPv6;
        ServiceRecord().iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv6 );
        }
    else
        {
        if( aPdpType == RPacketContext::EPdpTypeIPv6 )
            {
            User::Leave( KErrNotSupported );
            }
            
        ServiceRecord().iGPRSIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        ServiceRecord().iGPRSPDPType = RPacketContext::EPdpTypeIPv4;
        }
    }
    
// ----------------------------------------------------------------------------
// CCmPluginPacketData::AdditionalReset()
// ----------------------------------------------------------------------------
//        
void CCmPluginPacketData::AdditionalReset()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::AdditionalReset" );

    delete iPacketDataQoSRecord;
    iPacketDataQoSRecord = NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::PrepareToCopyDataL( CCmPluginBaseEng* aDestInst ) const
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::PrepareToCopyDataL" );

    aDestInst->SetBoolAttributeL( EPacketDataOutGoing, iOutgoing );
    }


// ---------------------------------------------------------------------------
// CCmPluginPacketData::SetDaemonNameL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::SetDaemonNameL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::SetDaemonNameL" );

    // use DHCP if we can
    TBool ipfromSrv = GetBoolAttributeL( ECmIPAddFromServer );
    if ( ipfromSrv )
        {
        SetStringAttributeL( ECmConfigDaemonManagerName, 
                           KDaemonManagerName );
        SetStringAttributeL( ECmConfigDaemonName, 
                           KConfigDaemonName );
        }
    else
        {
        if ( FeatureSupported( KFeatureIdIPv6 ) )
            {
            SetStringAttributeL( ECmConfigDaemonManagerName, 
                               KDaemonManagerName );
            SetStringAttributeL( ECmConfigDaemonName, 
                               KConfigDaemonName );
            }
        else
            {
            SetStringAttributeL( ECmConfigDaemonManagerName, 
                               KNullDesC() );
            SetStringAttributeL( ECmConfigDaemonName, 
                               KNullDesC() );
            }
        }
    }


// ---------------------------------------------------------------------------
// CCmPluginPacketData::CreateAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::CreateAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::CreateAdditionalRecordsL" );

    delete iPacketDataQoSRecord;
    iPacketDataQoSRecord = NULL;

    iPacketDataQoSRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord *>
                           (CCDRecordBase::RecordFactoryL(KCDTIdUmtsR99QoSAndOnTableRecord));

    iPacketDataQoSRecord->iGPRSReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    iPacketDataQoSRecord->iGPRSMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
    iPacketDataQoSRecord->iGPRSReqDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    iPacketDataQoSRecord->iGPRSMinDeliveryOrder = RPacketQoS::EDeliveryOrderUnspecified;
    iPacketDataQoSRecord->iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
    iPacketDataQoSRecord->iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
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
    iPacketDataQoSRecord->iGPRSReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
    iPacketDataQoSRecord->iGPRSMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
    iPacketDataQoSRecord->iGPRSReqTransferDelay = 0;
    iPacketDataQoSRecord->iGPRSMaxTransferDelay = 0;
    iPacketDataQoSRecord->iGPRSReqGuaranteedUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSMinGuaranteedUplinkRate = 0;
    iPacketDataQoSRecord->iGPRSReqGuaranteedDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSMinGuaranteedDownlinkRate = 0;
    iPacketDataQoSRecord->iGPRSSignallingIndication = EFalse;
    iPacketDataQoSRecord->iGPRS_ImCnSignallingIndication = EFalse;
    iPacketDataQoSRecord->iGPRSSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
    }

// --------------------------------------------------------------------------
// CCmPluginPacketData::DeleteAdditionalRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginPacketData::DeleteAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::DeleteAdditionalRecordsL" );    

    // If packet record is common with other packet iaps, do not delete it!
    if ( !TPtrC(iPacketDataQoSRecord->iRecordName).CompareF( KDefaultQosDataRecordName ) 
         || !TPtrC(iPacketDataQoSRecord->iRecordName).CompareF( KDefaultQosDataRecordNamewithSpace ) )
        {
        return;
        }
    
    iPacketDataQoSRecord->DeleteL( Session() );
    }

// --------------------------------------------------------------------------
// CCmPluginPacketData::LoadAdditionalRecordsL()
// --------------------------------------------------------------------------
//        
void CCmPluginPacketData::LoadAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::LoadAdditionalRecordsL" );

    if ( ServiceRecord().iUmtsR99QoSAndOnTable )
        {
        iPacketDataQoSRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord *>
                           (CCDRecordBase::RecordFactoryL(KCDTIdUmtsR99QoSAndOnTableRecord));
        
        iPacketDataQoSRecord->SetRecordId( ServiceRecord().iUmtsR99QoSAndOnTable );

        iPacketDataQoSRecord->LoadL( Session() );
                
//        AddConverstionTableL( (CCDRecordBase**)&iPacketDataQoSRecord, NULL, SQoSDataConvTbl );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginPacketData::UpdateAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginPacketData::UpdateAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginPacketData::UpdateAdditionalRecordsL" );

    if( !iPacketDataQoSRecord->RecordId() )
        {
        iPacketDataQoSRecord->SetRecordId( KCDNewRecordRequest );
        iPacketDataQoSRecord->StoreL( Session() );
        }
    else
        {
        iPacketDataQoSRecord->ModifyL( Session() );
        }
    // Set service record to point to QoS record if it does not yet
    if ( !ServiceRecord().iUmtsR99QoSAndOnTable )
        {
        ServiceRecord().iUmtsR99QoSAndOnTable = iPacketDataQoSRecord->RecordId();
        ServiceRecord().ModifyL( Session() );
        }
    }
