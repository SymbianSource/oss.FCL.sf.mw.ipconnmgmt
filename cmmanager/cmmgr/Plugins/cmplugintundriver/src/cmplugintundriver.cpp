/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TUN Driver plug-in IF implementation class.
*
*/

#include <AknsUtils.h>
#include <cmpluginvpn.mbg>
#include <data_caging_path_literals.hrh>

#include "cmlogger.h"
#include "cmdestinationimpl.h"
#include "cmplugintundriver.h"

using namespace CMManager;
using namespace CommsDat;

const TUint32 KDefaultPriorityTUNDriver = 0;
_LIT( KPluginTUNDriverFileIcons, "z:cmplugintundriver.mbm" );
_LIT(KTUNDriverBearerName,"TUNDriverBearer");
#define KCDAgentNameTUNDriver _S("tundriveragt.agt")
_LIT(KTUNDriverTunnelAgent,"tundriveragt.agt");
#define KCDInterfaceNameTUNDriver  _S("tundriver")
_LIT(KTUNDriverInterfaceName,"tundriver");

const TInt KLanLastSocketActivityTimeout = -1;
const TInt KLanLastSocketClosedTimeout = -1;

const TBool KDefIpAddrFromServer = EFalse;
const TBool KDefIpDnsAddrFromServer = EFalse;
const TBool KDefIp6DnsAddrFromServer = EFalse;
_LIT( KEmpty, "");

static const TCmAttribConvTable STUNDriverConvTbl[] = 
    {      
        { ETUNDriverIfNetworks, ETUNDriverRangeMax, NULL },
        { ETUNDriverIfNetworks, KCDTIdLANIfNetworks, NULL },        
        { ETUNDriverIpNetMask, KCDTIdLANIpNetMask, &CheckIPv4ValidityL },
        { ETUNDriverIpGateway, KCDTIdLANIpGateway, &CheckIPv4ValidityL },
        { ETUNDriverIpAddrFromServer, KCDTIdLANIpAddrFromServer, NULL },
        { ETUNDriverIpAddr, KCDTIdLANIpAddr, &CheckIPv4ValidityL },
        { ETUNDriverIpDNSAddrFromServer, KCDTIdLANIpDNSAddrFromServer, NULL },
        { ETUNDriverIpNameServer1, KCDTIdLANIpNameServer1, &CheckIPv4ValidityL },
        { ETUNDriverIpNameServer2, KCDTIdLANIpNameServer2, &CheckIPv4ValidityL },
        { ETUNDriverIp6DNSAddrFromServer, KCDTIdLANIp6DNSAddrFromServer, NULL },
        { ETUNDriverIp6NameServer1, KCDTIdLANIp6NameServer1, &CheckIPv4ValidityL },
        { ETUNDriverIp6NameServer2, KCDTIdLANIp6NameServer2, &CheckIPv4ValidityL },
        { ETUNDriverIpAddrLeaseValidFrom, KCDTIdLANIpAddrLeaseValidFrom, NULL },
        { ETUNDriverIpAddrLeaseValidTo, KCDTIdLANIpAddrLeaseValidTo, NULL },
        { ETUNDriverConfigDaemonManagerName, KCDTIdLANConfigDaemonManagerName, NULL },
        { ETUNDriverConfigDaemonName, KCDTIdLANConfigDaemonName, NULL },
        { ETUNDriverServiceExtensionTableName, KCDTIdLANServiceExtensionTableName, NULL },
        { ETUNDriverServiceExtensionTableRecordId, KCDTIdLANServiceExtensionTableRecordId, NULL },
        { 0, 0, NULL }
    };

static const TCmCommonAttrConvArrayItem STUNDriverCommonConvTbl[] =
    {        
        { ETUNDriverIfNetworks, ECmIFNetworks },
        { ETUNDriverIpNetMask, ECmIPNetmask },
        { ETUNDriverIpGateway, ECmIPGateway },
        { ETUNDriverIpAddrFromServer, ECmIPAddFromServer },
        { ETUNDriverIpAddr, ECmIPAddress },
        { ETUNDriverIpDNSAddrFromServer, ECmIPDNSAddrFromServer },
        { ETUNDriverIpNameServer1, ECmIPNameServer1 },
        { ETUNDriverIpNameServer2, ECmIPNameServer2 },
        { ETUNDriverIp6DNSAddrFromServer, ECmIP6DNSAddrFromServer },
        { ETUNDriverIp6NameServer1, ECmIP6NameServer1 },
        { ETUNDriverIp6NameServer2, ECmIP6NameServer2 },
        { ETUNDriverIpAddrLeaseValidFrom, ECmIPAddrLeaseValidFrom },
        { ETUNDriverIpAddrLeaseValidTo, ECmIPAddrLeaseValidTo },
        { ETUNDriverConfigDaemonManagerName, ECmConfigDaemonManagerName },
        { ETUNDriverConfigDaemonName, ECmConfigDaemonName },
        { 0, 0 }
    };

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::NewL()
// --------------------------------------------------------------------------
//
CCmPluginTUNDriver* CCmPluginTUNDriver::NewL( TCmPluginInitParam* aInitParam )
    {
    CCmPluginTUNDriver* self = new( ELeave ) CCmPluginTUNDriver( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::~CCmPluginTUNDriver()
// --------------------------------------------------------------------------
//
CCmPluginTUNDriver::~CCmPluginTUNDriver()
    {
    AdditionalReset();
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::CreateInstanceL
// --------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginTUNDriver::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginTUNDriver* self = new( ELeave ) CCmPluginTUNDriver( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// --------------------------------------------------------------------------
// CCmPluginTUNDriver::CCmPluginTUNDriver()
// --------------------------------------------------------------------------
//
CCmPluginTUNDriver::CCmPluginTUNDriver( TCmPluginInitParam* aInitParam )
    : CCmPluginLanBase( aInitParam )
    {
    CLOG_CREATE;
    iBearerType = KPluginTUNDriverBearerTypeUid;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::ConstructL()
// --------------------------------------------------------------------------
//
void CCmPluginTUNDriver::ConstructL()
    {
    CCmPluginLanBase::ConstructL();
    iBearerRecName = KCDTypeNameVirtualBearer;
   
    AddConverstionTableL( (CCDRecordBase**)&iServiceRecord, NULL, STUNDriverConvTbl );
    AddCommonConversionTableL( STUNDriverCommonConvTbl );
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::GetIntAttributeL()
// --------------------------------------------------------------------------
//
TUint32 CCmPluginTUNDriver::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::GetIntAttributeL" );

    TUint32 retVal( 0 );

    switch( aAttribute )
        {
        case ECmBearerIcon:
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TAknsItemID id;      
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPluginTUNDriverFileIcons, 
                                             &KDC_BITMAP_DIR, NULL ) );

            retVal = (TUint32)AknsUtils::CreateGulIconL( 
                                    skinInstance, 
                                    id,
                                    mbmFile.FullName(), 
                                    EMbmCmpluginvpnQgn_prop_vpn_access_point, 
                                    EMbmCmpluginvpnQgn_prop_vpn_access_point_mask );
            }
            break;             
        case ECmCommsDBBearerType:
            {
            retVal = KCommDbBearerLAN;
            }
            break;
        case ECmDefaultUiPriority:
        case ECmDefaultPriority:
            {
            retVal = aAttribute == ECmDefaultPriority ?
                        GlobalBearerPriority( TPtrC(KCDTypeNameLANService) ) :
                        GlobalUiBearerPriority( TPtrC(KCDTypeNameLANService) );

            if ( retVal == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                retVal = KDefaultPriorityTUNDriver;
                }
            }
            break;             
        default:
            {
            retVal = CCmPluginBaseEng::GetIntAttributeL( aAttribute );
            }
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::CanHandleIapIdL()
// --------------------------------------------------------------------------
//
TBool CCmPluginTUNDriver::CanHandleIapIdL( TUint32 aIapId ) const
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::CanHandleIapIdL1" );
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

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::CanHandleIapIdL()
// --------------------------------------------------------------------------
//
TBool CCmPluginTUNDriver::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::CanHandleIapIdL" );

    TBool retVal( EFalse );

    if( TPtrC( aIapRecord->iServiceType ) == TPtrC(KCDTypeNameLANService) &&
        TPtrC( aIapRecord->iBearerType ) == TPtrC(KCDTypeNameVirtualBearer) )
        {
        CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
            new ( ELeave ) CMDBRecordSet<CCDVirtualBearerRecord>(KCDTIdVirtualBearerRecord);
        CleanupStack::PushL( bearersRS );
        CCDVirtualBearerRecord* bearerRecord =
            static_cast<CCDVirtualBearerRecord *>( CCDRecordBase::RecordFactoryL( KCDTIdVirtualBearerRecord ) );
        CleanupStack::PushL( bearerRecord );

        // Find entries used "tundriveragt.agt" as agent from Table VirtualBearer
        bearerRecord->iBearerAgent.SetL( KTUNDriverTunnelAgent );
        bearerRecord->iVirtualBearerNifName.SetL( KTUNDriverInterfaceName );
        bearersRS->iRecords.AppendL( bearerRecord );

        CleanupStack::Pop( bearerRecord );
        bearerRecord = NULL;
        if ( bearersRS->FindL( Session() ) )
            {
            TUint32 recordId = (*bearersRS)[0]->RecordId();
            TPtrC bearerName( (*bearersRS)[0]->iRecordName.GetL() );
            TPtrC bearerAgent( (*bearersRS)[0]->iBearerAgent.GetL() );
            TPtrC bearerInterface( (*bearersRS)[0]->iVirtualBearerNifName.GetL() );

            // Further comparing record ID referred to by this VPN IAP with entry ID in table VirtualBearer
            // And also comparing bear name with our expected one "vpnbearer"
            if ( bearerAgent == TPtrC( KCDAgentNameTUNDriver ) &&
                 bearerInterface == TPtrC( KCDInterfaceNameTUNDriver ) )
                {
                retVal = ETrue;
                }
            }

        CleanupStack::PopAndDestroy( bearersRS );
        }
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::RunSettingsL()
// --------------------------------------------------------------------------
//        
TInt CCmPluginTUNDriver::RunSettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::RunSettingsL" );
    User::Leave( KErrNotSupported );
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::LoadServiceSettingL()
// --------------------------------------------------------------------------
//        
void CCmPluginTUNDriver::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::LoadServiceSettingL" );
    
    if( TPtrC(KCDTypeNameLANService) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDLANServiceRecord *>
                    ( CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    CCmPluginBaseEng::LoadServiceSettingL();
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::InitializeWithUiL()
// --------------------------------------------------------------------------
//   
TBool CCmPluginTUNDriver::InitializeWithUiL( TBool /*aManuallyConfigure*/ )
    {
    User::Leave( KErrNotSupported );
    return ETrue;
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::ServiceRecord()
// --------------------------------------------------------------------------
//  
CCDLANServiceRecord& CCmPluginTUNDriver::ServiceRecord() const
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::ServiceRecord" );
    return *static_cast<CCDLANServiceRecord*>( iServiceRecord );
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::CreateNewServiceRecordL()
// --------------------------------------------------------------------------
//        
void CCmPluginTUNDriver::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::CreateNewServiceRecordL" );

    delete iServiceRecord; 
    iServiceRecord = NULL;

    iServiceRecord = static_cast<CCDServiceRecordBase *>
                (CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ));

    // I do not think that we have to use the default record,
    // but for compatibility with ApEngine, we write all fields 
    CCDLANServiceRecord* record = 
                            static_cast<CCDLANServiceRecord *>( iServiceRecord );

    if ( FeatureSupported( KFeatureIdIPv6 ) )
        {
        record->iIfNetworks.SetL( KDefIspIfNetworksIPv4IPv6LAN );
        }
    else
        {
        record->iIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        }
    record->iIpGateway.SetL( KUnspecifiedIPv4 );        
    record->iIpAddrFromServer.SetL( KDefIpAddrFromServer );

    record->iIpAddr.SetL( KUnspecifiedIPv4 );

    record->iIpDnsAddrFromServer.SetL( KDefIpDnsAddrFromServer );

    record->iIpNameServer1.SetL( KUnspecifiedIPv4 );
    record->iIpNameServer2.SetL( KUnspecifiedIPv4 );
    record->iIp6DnsAddrFromServer.SetL( KDefIp6DnsAddrFromServer );
    record->iIp6NameServer1.SetL( KDynamicIpv6Address );
    record->iIp6NameServer2.SetL( KDynamicIpv6Address );

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
    }

// ---------------------------------------------------------------------------
// CCmPluginTUNDriver::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmPluginTUNDriver::ServiceRecordIdLC( HBufC* &aName, TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::ServiceRecordIdLC" );

    aName = TPtrC( KCDTypeNameLANService ).AllocLC();    
    aRecordId = iServiceRecord->RecordId();
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::BearerRecordIdLC()
// --------------------------------------------------------------------------
//      
void CCmPluginTUNDriver::BearerRecordIdLC( HBufC* &aBearerName, TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::BearerRecordIdLC" );

    CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
                  new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>
                                                       (KCDTIdVirtualBearerRecord);
    CleanupStack::PushL( bearersRS );

    CCDVirtualBearerRecord* tmprec = static_cast<CCDVirtualBearerRecord*>
                        (CCDRecordBase::RecordFactoryL( KCDTIdVirtualBearerRecord ));
    CleanupStack::PushL( tmprec );
    tmprec->iBearerAgent.SetL( KTUNDriverTunnelAgent );
    bearersRS->iRecords.AppendL( tmprec );

    CleanupStack::Pop( tmprec );
    tmprec = NULL;

    if ( !bearersRS->FindL( Session() ) )
        // No bearer record found -> create a default one.
        {
        tmprec = static_cast<CCDVirtualBearerRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
        CleanupStack::PushL(tmprec);

        tmprec->iRecordName.SetL( KTUNDriverBearerName );
        tmprec->iBearerAgent.SetL( KTUNDriverTunnelAgent );
        tmprec->iVirtualBearerNifName.SetL(KTUNDriverInterfaceName);
        tmprec->iLastSocketActivityTimeout = (TUint32)KLanLastSocketActivityTimeout;
        tmprec->iLastSessionClosedTimeout = (TUint32) KLanLastSocketClosedTimeout;
        tmprec->iLastSocketClosedTimeout = (TUint32)KLanLastSocketActivityTimeout;
        
        tmprec->SetRecordId( KCDNewRecordRequest );
        tmprec->StoreL( Session() );
        
        aRecordId = tmprec->RecordId();

        CleanupStack::PopAndDestroy(tmprec);
        }
    else
        {
        aRecordId = (*bearersRS)[0]->RecordId();
        }

    CleanupStack::PopAndDestroy( bearersRS );

    aBearerName = TPtrC( KCDTypeNameVirtualBearer ).AllocLC();
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::PrepareToUpdateRecordsL()
// --------------------------------------------------------------------------
//        
void CCmPluginTUNDriver::PrepareToUpdateRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginTUNDriver::PrepareToUpdateRecordsL" );
    CheckDNSServerAddressL( ETrue, 
                            ServiceRecord().iIp6NameServer1,
                            ServiceRecord().iIp6NameServer2,
                            ServiceRecord().iIp6DnsAddrFromServer );
                            
    CheckDNSServerAddressL( EFalse,
                            ServiceRecord().iIpNameServer1,
                            ServiceRecord().iIpNameServer2,
                            ServiceRecord().iIpDnsAddrFromServer );
    }

// --------------------------------------------------------------------------
// CCmPluginTUNDriver::AdditionalReset()
// --------------------------------------------------------------------------
//        
void CCmPluginTUNDriver::AdditionalReset()
    {

    }

// eof
