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
* Description:  Implementation of base for Lan Plugin interface 
*                for Lan Bearer.
*
*/
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <cmpluginlanbase.h>
#include "cmlogger.h"
#include <ecom/ecom.h>        // For REComSession
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include "cmmanagerimpl.h"
#include <cmmanager.rsg>
#include <cmpsettingsconsts.h>

using namespace CMManager;
using namespace CommsDat;

// default values for fields...
// CONSTANTS
_LIT( KEmpty, "");

_LIT( KLanBearerName, "LANBearer" );
_LIT( KLanBearerAgent, "lanagt.agt" );
_LIT( KLanBearerNif, "lannif" );

_LIT( KLanLDDName, "not used" );
_LIT( KLanPDDName, "not used" );

const TInt KLanLastSocketActivityTimeout = -1;
const TInt KLanLastSocketClosedTimeout = -1;

const TBool KDefIpAddrFromServer = ETrue;
const TBool KDefIpDnsAddrFromServer = ETrue;
const TBool KDefIp6DnsAddrFromServer = ETrue;

static const TCmAttribConvTable SLanConvTbl[] = 
    {
    { ELanBaseIfNetworks, ELanBaseRangeMax, NULL },
    { ELanBaseIfNetworks, KCDTIdLANIfNetworks, NULL },        
    { ELanBaseIpNetMask, KCDTIdLANIpNetMask, &CheckIPv4ValidityL,
      R_QTN_NETW_SETT_SUBNET_MASK,
      KIpAddressWidth, EConvCompulsory | EConvIPv4, R_QTN_SELEC_SETTING_COMPULSORY, 0, NULL, NULL },
    { ELanBaseIpGateway, KCDTIdLANIpGateway, &CheckIPv4ValidityL,
      R_QTN_NETW_SETT_DEFAULT_GATEWAY,
      KIpAddressWidth, EConvCompulsory | EConvIPv4, R_QTN_SELEC_SETTING_COMPULSORY, 0, NULL, NULL },
    { ELanBaseIpAddrFromServer, KCDTIdLANIpAddrFromServer, NULL },
    { ELanBaseIpAddr, KCDTIdLANIpAddr, &CheckIPv4ValidityL,
      R_QTN_SET_IP_ADDRESS,
      KIpAddressWidth, EConvIPv4, R_QTN_SET_IP_ADDRESS_DYNAMIC, 0, NULL, NULL },
    { ELanBaseIpDNSAddrFromServer, KCDTIdLANIpDNSAddrFromServer, NULL,
      R_QTN_SET_DNS_SERVERS_IP,
      0, 0, 0, 0, NULL, NULL },
    { ELanBaseIpNameServer1, KCDTIdLANIpNameServer1, &CheckIPv4ValidityL,
      R_QTN_SET_PRIMARY_DNS,
      KMaxIPv4NameServerLength, EConvIPv4, R_QTN_SET_DNS_SERVERS_AUTOMATIC, 0, NULL, NULL },
    { ELanBaseIpNameServer2, KCDTIdLANIpNameServer2, &CheckIPv4ValidityL,
      R_QTN_SET_SECONDARY_DNS,
      KMaxIPv4NameServerLength, EConvIPv4, R_QTN_SET_DNS_SERVERS_AUTOMATIC, 0, NULL, NULL },    
    { ELanBaseIp6DNSAddrFromServer, KCDTIdLANIp6DNSAddrFromServer, NULL,
      R_QTN_SET_DNS_SERVERS_IP,
      0, 0, 0, 0, NULL, NULL },
    { ELanBaseIp6NameServer1, KCDTIdLANIp6NameServer1, &CheckIPv6ValidityL },
    { ELanBaseIp6NameServer2, KCDTIdLANIp6NameServer2, &CheckIPv6ValidityL },
    { ELanBaseIpAddrLeaseValidFrom, KCDTIdLANIpAddrLeaseValidFrom, NULL },
    { ELanBaseIpAddrLeaseValidTo, KCDTIdLANIpAddrLeaseValidTo, NULL },
    { ELanBaseConfigDaemonManagerName, KCDTIdLANConfigDaemonManagerName, NULL },
    { ELanBaseConfigDaemonName, KCDTIdLANConfigDaemonName, NULL },
    { ELanBaseServiceExtensionTableName, KCDTIdLANServiceExtensionTableName, NULL },
    { ELanBaseServiceExtensionTableRecordId, KCDTIdLANServiceExtensionTableRecordId, &CheckRecordIdValidityL },
    { 0,0, NULL }
    };

static const TCmCommonAttrConvArrayItem SCommonConvTbl[] =
{
    { ELanBaseIfNetworks, ECmIFNetworks },
    { ELanBaseIpNetMask, ECmIPNetmask },
    { ELanBaseIpGateway, ECmIPGateway },
    { ELanBaseIpAddrFromServer, ECmIPAddFromServer },
    { ELanBaseIpAddr, ECmIPAddress },
    { ELanBaseIpDNSAddrFromServer, ECmIPDNSAddrFromServer },
    { ELanBaseIpNameServer1, ECmIPNameServer1 },
    { ELanBaseIpNameServer2, ECmIPNameServer2 },
    { ELanBaseIp6DNSAddrFromServer, ECmIP6DNSAddrFromServer },
    { ELanBaseIp6NameServer1, ECmIP6NameServer1 },
    { ELanBaseIp6NameServer2, ECmIP6NameServer2 },
    { ELanBaseIpAddrLeaseValidFrom, ECmIPAddrLeaseValidFrom },
    { ELanBaseIpAddrLeaseValidTo, ECmIPAddrLeaseValidTo },
    { ELanBaseConfigDaemonManagerName, ECmConfigDaemonManagerName },
    { ELanBaseConfigDaemonName, ECmConfigDaemonName },
    { 0, 0 }
    };

// ---------------------------------------------------------------------------
// CCmPluginLanBase::CCmPluginLanBase()
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginLanBase::CCmPluginLanBase( TCmPluginInitParam* aInitParam )
    : CCmPluginBaseEng( aInitParam )
    {
    CLOG_CREATE;


    }

EXPORT_C CCmPluginLanBase::~CCmPluginLanBase()
    {
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::ConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginLanBase::ConstructL()
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::ConstructL" );
    
    CCmPluginBaseEng::ConstructL();
    
    AddConverstionTableL( (CCDRecordBase**)&iServiceRecord, NULL, SLanConvTbl );
	AddCommonConversionTableL( SCommonConvTbl );
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::RunSettingsL()
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt CCmPluginLanBase::RunSettingsL()
    {
    User::Leave( KErrNotSupported );
    return 0;
    }
        
// ---------------------------------------------------------------------------
// CCmPluginLanBase::InitializeWithUiL()
// ---------------------------------------------------------------------------
//        
EXPORT_C TBool CCmPluginLanBase::InitializeWithUiL( 
                                            TBool /*aManuallyConfigure*/ )
    {
    User::Leave(KErrNotSupported);
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::LoadServiceSettingL()
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmPluginLanBase::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::LoadServiceSettingL" );
    
    if( TPtrC(KCDTypeNameLANService) == iIapRecord->iServiceType  )
        {
        iServiceRecord = static_cast<CCDLANServiceRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }

    CCmPluginBaseEng::LoadServiceSettingL();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginLanBase::ServiceRecord()
// ---------------------------------------------------------------------------
//        
EXPORT_C CCDLANServiceRecord& CCmPluginLanBase::ServiceRecord()const
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::ServiceRecord" );
    return *static_cast<CCDLANServiceRecord*>(iServiceRecord);
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginLanBase::ServiceRecordIdLC( HBufC* &aName, 
                                                   TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::ServiceRecordIdLC" );

    aName = TPtrC( KCDTypeNameLANService ).AllocLC();    
    aRecordId = iServiceRecord->RecordId();
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::BearerRecordIdLC()
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmPluginLanBase::BearerRecordIdLC( HBufC* &aBearerName, 
                                                  TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::BearerRecordIdLC" );
                
    CCDLANBearerRecord* tmprec = static_cast<CCDLANBearerRecord*>
                        (CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ));
    CleanupStack::PushL( tmprec );

    tmprec->iRecordName.SetL( TPtrC(iBearerRecName) );
    if ( !tmprec->FindL( Session() ) )
        { // bearer not found -> create dummy values
        CLOG_WRITE( "No bearer found" );

        tmprec->SetRecordId( KCDNewRecordRequest );
        tmprec->iBearerAgent = KLanBearerAgent;
        tmprec->iRecordName = KLanBearerName;
        tmprec->iLanBearerNifName = KLanBearerNif;
        tmprec->iLanBearerLddName = KLanLDDName;
        tmprec->iLanBearerPddName = KLanPDDName;
        tmprec->iLastSocketActivityTimeout = 
                                        (TUint32)KLanLastSocketActivityTimeout;
        tmprec->iLastSocketClosedTimeout = 
                                        (TUint32)KLanLastSocketClosedTimeout;

        tmprec->StoreL( Session() );
        }

    aRecordId = tmprec->RecordId();
    
    CLOG_WRITE_2( "Name: [%S], id: [%d]", 
                  &tmprec->iRecordName.GetL(), 
                  aRecordId );

    CleanupStack::PopAndDestroy( tmprec );

    aBearerName = TPtrC(KCDTypeNameLANBearer).AllocLC();    
    }

// ---------------------------------------------------------------------------
// CCmPluginLanBase::CreateNewServiceRecordL()
// ---------------------------------------------------------------------------
//        
EXPORT_C void CCmPluginLanBase::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::CreateNewServiceRecordL" );

    delete iServiceRecord; 
    iServiceRecord = NULL;
    
    iServiceRecord = static_cast<CCDServiceRecordBase *>
                (CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));

    // I do not think that we have to use the default record,
    // but for compatibility with ApEngine, we write all fields 

    CCDLANServiceRecord* record = 
                            static_cast<CCDLANServiceRecord *>(iServiceRecord);


    if( FeatureSupported( KFeatureIdIPv6 ) )
        {
        record->iIfNetworks.SetL( KDefIspIfNetworksIPv4IPv6LAN );
        }
    else
        {
        record->iIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        }

//    record->iIpNetmask.SetL();
    
    record->iIpGateway.SetL( KUnspecifiedIPv4 );        
    record->iIpAddrFromServer.SetL( KDefIpAddrFromServer );
    
    record->iIpAddr.SetL( KUnspecifiedIPv4 );
    
    record->iIpDnsAddrFromServer.SetL( KDefIpDnsAddrFromServer );
    
    record->iIpNameServer1.SetL( KUnspecifiedIPv4 );
    record->iIpNameServer2.SetL( KUnspecifiedIPv4 );
    record->iIp6DnsAddrFromServer.SetL( KDefIp6DnsAddrFromServer );
    record->iIp6NameServer1.SetL( KDynamicIpv6Address );
    record->iIp6NameServer2.SetL( KDynamicIpv6Address );
        
//        record->iIpAddrLeaseValidFrom.SetL();
//        record->iIpAddrLeaseValidTo.SetL();
        
    if ( KDefIpAddrFromServer )
        {
        record->iConfigDaemonManagerName.SetL( KDaemonManagerName );
        record->iConfigDaemonName.SetL( KConfigDaemonName );
        }
    else
        {
        record->iConfigDaemonManagerName.SetL( KEmpty );
        record->iConfigDaemonName.SetL( KEmpty );
        }

//        record->iServiceExtensionTableName.SetL();
//        record->iServiceExtensionTableRecordId.SetL();
    }

// --------------------------------------------------------------------------
// CCmPluginLanBase::PrepareToUpdateRecordsL()
// --------------------------------------------------------------------------
//        
EXPORT_C void CCmPluginLanBase::PrepareToUpdateRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::PrepareToUpdateRecordsL" );
    
    CheckDNSServerAddressL( ETrue, 
                            ServiceRecord().iIp6NameServer1,
                            ServiceRecord().iIp6NameServer2,
                            ServiceRecord().iIp6DnsAddrFromServer );
                            
    CheckDNSServerAddressL( EFalse,
                            ServiceRecord().iIpNameServer1,
                            ServiceRecord().iIpNameServer2,
                            ServiceRecord().iIpDnsAddrFromServer );
    }
