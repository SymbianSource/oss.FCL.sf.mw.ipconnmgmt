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
*      Implementation of Lan Plugin interface implementation 
*      for Lan Bearer
*      
*
*/

 
// INCLUDE FILES

#include "cmpluginlan.h"
#include "cmlogger.h"
#include "datamobilitycommsdattypes.h"
#include <cmmanagerdef.h>
#include <ecom/ecom.h>		// For REComSession
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>

#include <cmpluginpacketdata.mbg>           // icons
#include <data_caging_path_literals.hrh>

using namespace CMManager;
using namespace CommsDat;

// ================= CONSTANTS =======================
const TUint32 KDefaultPriorityLAN = 0;

static const TCmAttribConvTable SLanConvTbl[] = 
    {
        { ELanIfNetworks, ELanRangeMax, NULL },
        { ELanIfNetworks, KCDTIdLANIfNetworks, NULL },        
        { ELanIpNetMask, KCDTIdLANIpNetMask, &CheckIPv4ValidityL },
        { ELanIpGateway, KCDTIdLANIpGateway, &CheckIPv4ValidityL },
        { ELanIpAddrFromServer, KCDTIdLANIpAddrFromServer, NULL },
        { ELanIpAddr, KCDTIdLANIpAddr, &CheckIPv4ValidityL },
        { ELanIpDNSAddrFromServer, KCDTIdLANIpDNSAddrFromServer, NULL },
        { ELanIpNameServer1, KCDTIdLANIpNameServer1, &CheckIPv4ValidityL },
        { ELanIpNameServer2, KCDTIdLANIpNameServer2, &CheckIPv4ValidityL },
        { ELanIp6DNSAddrFromServer, KCDTIdLANIp6DNSAddrFromServer, NULL },
        { ELanIp6NameServer1, KCDTIdLANIp6NameServer1, &CheckIPv4ValidityL },
        { ELanIp6NameServer2, KCDTIdLANIp6NameServer2, &CheckIPv4ValidityL },
        { ELanIpAddrLeaseValidFrom, KCDTIdLANIpAddrLeaseValidFrom, NULL },
        { ELanIpAddrLeaseValidTo, KCDTIdLANIpAddrLeaseValidTo, NULL },
        { ELanConfigDaemonManagerName, KCDTIdLANConfigDaemonManagerName, NULL },
        { ELanConfigDaemonName, KCDTIdLANConfigDaemonName, NULL },
        { ELanServiceExtensionTableName, KCDTIdLANServiceExtensionTableName, NULL },
        { ELanServiceExtensionTableRecordId, KCDTIdLANServiceExtensionTableRecordId, NULL },
        { 0, 0, NULL }
    };


static const TCmCommonAttrConvArrayItem SCommonConvTbl[] =
{
    { ELanIfNetworks, ECmIFNetworks },
    { ELanIpNetMask, ECmIPNetmask },
    { ELanIpGateway, ECmIPGateway },
    { ELanIpAddrFromServer, ECmIPAddFromServer },
    { ELanIpAddr, ECmIPAddress },
    { ELanIpDNSAddrFromServer, ECmIPDNSAddrFromServer },
    { ELanIpNameServer1, ECmIPNameServer1 },
    { ELanIpNameServer2, ECmIPNameServer2 },
    { ELanIp6DNSAddrFromServer, ECmIP6DNSAddrFromServer },
    { ELanIp6NameServer1, ECmIP6NameServer1 },
    { ELanIp6NameServer2, ECmIP6NameServer2 },
    { ELanIpAddrLeaseValidFrom, ECmIPAddrLeaseValidFrom },
    { ELanIpAddrLeaseValidTo, ECmIPAddrLeaseValidTo },
    { ELanConfigDaemonManagerName, ECmConfigDaemonManagerName },
    { ELanConfigDaemonName, ECmConfigDaemonName },
    { 0, 0 }
    };


_LIT( KPacketDataFileIcons, "z:cmpluginpacketdata.mbm" );
    

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CCmPluginLan::NewOutgoingL()
// ---------------------------------------------------------
//
CCmPluginLan* CCmPluginLan::NewL( TCmPluginInitParam* aInitParam )
	{
	CCmPluginLan* self = new( ELeave ) CCmPluginLan( aInitParam );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( /*self*/ );

	return self;
	}

// ----------------------------------------------------------------------------
// CCmPluginLan::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginLan::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginLan* self = new( ELeave ) CCmPluginLan( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CCmPluginLan::~CCmPluginLan()
// ---------------------------------------------------------
//
CCmPluginLan::~CCmPluginLan()
	{
	AdditionalReset();
	
	CLOG_CLOSE;
	}


// ---------------------------------------------------------
// CCmPluginLan::CCmPluginLan()
// ---------------------------------------------------------
//
CCmPluginLan::CCmPluginLan( TCmPluginInitParam* aInitParam )
    : CCmPluginLanBase( aInitParam )
	{
	CLOG_CREATE;
	
	iBearerType = KUidLanBearerType;
	}


// ---------------------------------------------------------
// CCmPluginLan::ConstructL()
// ---------------------------------------------------------
//
void CCmPluginLan::ConstructL()
	{
	CCmPluginLanBase::ConstructL();
	
    iBearerRecName = KCDTypeNameLANBearer;
	
	AddConverstionTableL( (CCDRecordBase**)&iServiceRecord, NULL, SLanConvTbl );
	
	AddCommonConversionTableL( SCommonConvTbl );
	}
	

// ---------------------------------------------------------
// CCmPluginLan::GetIntAttributeL()
// ---------------------------------------------------------
//
TUint32 CCmPluginLan::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::GetIntAttributeL" );

    TUint32 retVal;
    
    switch( aAttribute )
        {
        // LAN bearer has no icon, but added temporary 
        // to help work on emulator.
        case ECmBearerIcon:
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPacketDataFileIcons, 
                                                    &KDC_BITMAP_DIR, NULL ) );

                   
            retVal = (TUint32)AknsUtils::CreateGulIconL( 
                                    skinInstance, 
                                    KAknsIIDQgnPropWmlGprs,
                                    mbmFile.FullName(), 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs, 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs_mask );
            }
            break;
               
        // LAN bearer has no icon, but added temporary 
        // to help work on emulator.
        case ECmBearerAvailableIcon:
            {
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPacketDataFileIcons, &KDC_BITMAP_DIR, NULL ) );

                   
            retVal = (TUint32)AknsUtils::CreateGulIconL( 
                                    skinInstance, 
                                    KAknsIIDQgnPropWmlGprs,
                                    mbmFile.FullName(), 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs, 
                                    EMbmCmpluginpacketdataQgn_prop_wml_gprs_mask );
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
            
            if( retVal == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                retVal = KDefaultPriorityLAN;
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
    
// ---------------------------------------------------------
// CCmPluginLan::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginLan::CanHandleIapIdL( TUint32 aIapId ) const
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::CanHandleIapIdL1" );
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

// ---------------------------------------------------------
// CCmPluginLan::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginLan::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginLanBase::CanHandleIapIdL2" );

    CLOG_WRITE_1( "IapId: [%d]", aIapRecord->RecordId() );
    
    TBool retVal( EFalse );
    
    CLOG_WRITE_2( "IAP record: [%S][%S]",
                    &FIELD_TO_TDESC(aIapRecord->iServiceType), 
                    &FIELD_TO_TDESC(aIapRecord->iBearerType)
                );
    
    if( (TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameLANService) ) &&
        TPtrC(aIapRecord->iBearerType) == TPtrC(KCDTypeNameLANBearer) )
        {
        CLOG_WRITE( "I can." );
        retVal = ETrue;
        }

    return retVal;
    }

// ---------------------------------------------------------
// CCmPluginLan::AdditionalReset()
// ---------------------------------------------------------
//        
void CCmPluginLan::AdditionalReset()
    {
    }

//  End of File
