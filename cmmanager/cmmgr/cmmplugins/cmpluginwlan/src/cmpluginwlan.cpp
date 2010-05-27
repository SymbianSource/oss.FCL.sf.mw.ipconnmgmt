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
*        Implementation of WLAN Plugin interface implementation
*        for WLAN Bearer
*
*/


#include <ecom/ecom.h>      // For REComSession
#include <wlancontainer.h>
#include <wlanmgmtcommon.h>
#include <cmpluginwlandef.h>
#include <cmcommonconstants.h>
#include <featmgr.h>

#include "datamobilitycommsdattypes.h"
#include "cmpluginwlan.h"
#include "cmwlancoveragecheck.h"
#include "cmmanagerdef.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmpluginwlanTraces.h"
#endif

using namespace CMManager;

enum TWlanWepKeyFormat
    {
    EWlanWepKeyFormatAscii = 0,
    EWlanWepKeyFormatHex
    };

// default values for fields...
// CONSTANTS
_LIT( KEmpty, "");

const TBool KDefIpAddrFromServer = ETrue;
const TBool KDefIpDnsAddrFromServer = ETrue;
const TBool KDefIp6DnsAddrFromServer = ETrue;

const TInt KWlanServiceRecordIndex = 0;

const TInt KWlanLastSocketActivityTimeout = -1;
const TInt KWlanLastSessionClosedTimeout = 1;
const TInt KWlanLastSocketClosedTimeout = -1;

/// Modem bearer names for WLAN connection methods.
_LIT( KModemBearerWLAN, "WLANBearer" );

_LIT( KWlanBearerName, "WLANBearer" );
_LIT( KWlanBearerAgent, "wlanagt.agt" );
_LIT( KWlanBearerNif, "wlannif" );

_LIT( KWlanLDDName, "not used" );
_LIT( KWlanPDDName, "not used" );


// --------------------------------------------------------------------------
// CCmPluginWlan::NewL
// --------------------------------------------------------------------------
//
CCmPluginWlan* CCmPluginWlan::NewL( TCmPluginInitParam* aInitParam )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_NEWL_ENTRY );

    CCmPluginWlan* self = new( ELeave ) CCmPluginWlan( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINWLAN_NEWL_EXIT );
    return self;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::~CCmPluginWlan
// --------------------------------------------------------------------------
//
CCmPluginWlan::~CCmPluginWlan()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CCMPLUGINWLAN_ENTRY );

    ResetBearerRecords();
    FeatureManager::UnInitializeLib();

    OstTraceFunctionExit0( CCMPLUGINWLAN_CCMPLUGINWLAN_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CreateInstanceL
// --------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginWlan::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CREATEINSTANCEL_ENTRY );

    CCmPluginWlan* self = new( ELeave ) CCmPluginWlan( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINWLAN_CREATEINSTANCEL_EXIT );
    return self;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CCmPluginWlan
// --------------------------------------------------------------------------
//
CCmPluginWlan::CCmPluginWlan( TCmPluginInitParam* aInitParam )
        :
        CCmPluginBaseEng( aInitParam )
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINWLAN_CCMPLUGINWLAN_ENTRY );

    iBearerType = KUidWlanBearerType;
    iWlanServiceRecord = NULL;
    iWlanTableId = 0;
    iWAPISupported = EFalse;

    OstTraceFunctionExit0( DUP1_CCMPLUGINWLAN_CCMPLUGINWLAN_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::ConstructL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::ConstructL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CONSTRUCTL_ENTRY );

    FeatureManager::InitializeLibL();

    if ( ! FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        FeatureManager::UnInitializeLib();

        User::Leave( KErrNotSupported );
        }

    CCmPluginBaseEng::ConstructL();

    // Get bearer priority table ID.
    TRAP_IGNORE( iBearerPriorityTableId =
            CCDGlobalBearerTypePriorizationRecord::TableIdL( iSession ) );

    if ( !iBearerPriorityTableId )
        {
        iBearerPriorityTableId =
                CCDGlobalBearerTypePriorizationRecord::CreateTableL( iSession );
        }

    // get WLAN table id
    TRAP_IGNORE( iWlanTableId = CCDWlanServiceRecord::TableIdL( iSession ) );

    if ( !iWlanTableId )
        {
        iWlanTableId = CCDWlanServiceRecord::CreateTableL( iSession );

        TMDBElementId tableId = 0;

        TRAP_IGNORE( tableId = CCDWlanDeviceSettingsRecord::TableIdL( iSession ) );
        if ( !tableId )
            {
            CCDWlanDeviceSettingsRecord::CreateTableL( iSession );
            }

        TRAP_IGNORE( tableId = CCDWLANSecSSIDTable::TableIdL( iSession ) );
        if ( !tableId )
            {
            CCDWLANSecSSIDTable::CreateTableL( iSession );
            }

        TRAP_IGNORE( tableId = CCDDestNWTable::TableIdL( iSession ) );
        if ( !tableId )
            {
            CCDDestNWTable::CreateTableL( iSession );
            }
        }

    iWAPISupported = FeatureManager::FeatureSupported( KFeatureIdFfWlanWapi );

    OstTraceFunctionExit0( CCMPLUGINWLAN_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ServiceRecord()
// ---------------------------------------------------------------------------
//
CCDLANServiceRecord& CCmPluginWlan::ServiceRecord() const
    {
    return *static_cast<CCDLANServiceRecord*>( iServiceRecord );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ServiceRecordId
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginWlan::ServiceRecordId() const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SERVICERECORDID_ENTRY );

    return ServiceRecord().RecordId();
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::ServiceRecordNameLC( HBufC* &aName )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SERVICERECORDNAMELC_ENTRY );

    aName = TPtrC( KCDTypeNameLANService ).AllocLC();

    OstTraceFunctionExit0( CCMPLUGINWLAN_SERVICERECORDNAMELC_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::BearerRecordIdL()
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::BearerRecordIdL( TUint32& aRecordId )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_BEARERRECORDIDL_ENTRY );

    CCDLANBearerRecord* lanBearerRecord = static_cast<CCDLANBearerRecord*>
                        (CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ));
    CleanupStack::PushL( lanBearerRecord );

    lanBearerRecord->iRecordName.SetL( TPtrC( KModemBearerWLAN ) );
    if ( !lanBearerRecord->FindL( iSession ) )
        { // bearer not found -> create dummy values
        lanBearerRecord->SetRecordId( KCDNewRecordRequest );
        lanBearerRecord->iBearerAgent = KWlanBearerAgent;
        lanBearerRecord->iRecordName = KWlanBearerName;
        lanBearerRecord->iLanBearerNifName = KWlanBearerNif;
        lanBearerRecord->iLanBearerLddName = KWlanLDDName;
        lanBearerRecord->iLanBearerPddName = KWlanPDDName;
        lanBearerRecord->iLastSocketActivityTimeout =
                                        (TUint32)KWlanLastSocketActivityTimeout;
        lanBearerRecord->iLastSessionClosedTimeout =
                                        (TUint32)KWlanLastSessionClosedTimeout;
        lanBearerRecord->iLastSocketClosedTimeout =
                                        (TUint32)KWlanLastSocketClosedTimeout;

        lanBearerRecord->StoreL( iSession );
        }

    aRecordId = lanBearerRecord->RecordId();

    CleanupStack::PopAndDestroy( lanBearerRecord );

    OstTraceFunctionExit0( CCMPLUGINWLAN_BEARERRECORDIDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::BearerRecordNameLC()
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::BearerRecordNameLC( HBufC* &aBearerName )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_BEARERRECORDNAMELC_ENTRY );

    aBearerName = TPtrC( KCDTypeNameLANBearer ).AllocLC();

    OstTraceFunctionExit0( CCMPLUGINWLAN_BEARERRECORDNAMELC_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::GetBearerIntAttributeL
// --------------------------------------------------------------------------
//
TUint32 CCmPluginWlan::GetBearerIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmInvalidAttribute:
            {
            retVal = 0;
            }
            break;
        case ECmExtensionLevel:
            {
            // This is extension from LAN plugin
            retVal = KExtensionBaseLevel + 1;
            }
            break;
        case ECmCommsDBBearerType:
            {
            retVal = KCommDbBearerWLAN;
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
        case EWlanServiceId:
            {
            retVal = wlanServiceRecord->iWlanServiceId;
            }
            break;
        case EWlanConnectionMode:
            {
            retVal = wlanServiceRecord->iWlanConnMode;
            }
            break;
        case EWlanSecurityMode:
            {
            retVal = wlanServiceRecord->iWlanSecMode;
            }
            break;
        case EWlanAuthenticationMode:
            {
            retVal = wlanServiceRecord->iWlanAuthMode;
            }
            break;
        case EWlanChannelID:
            {
            retVal = wlanServiceRecord->iWlanChannelID;
            }
            break;
        case EWlanServiceExtensionTableRecordId:
            {
            retVal = serviceRecord->iServiceExtensionTableRecordId;
            }
            break;
        case EWlanWepKeyIndex:
            {
            retVal = wlanServiceRecord->iWlanWepIndex;
            }
            break;
        case EWlanWapiPskFormat:
            {
            retVal = wlanServiceRecord->iWlanFormatKey1;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERINTATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::GetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::GetBearerBoolAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERBOOLATTRIBUTEL_ENTRY );

    TBool retVal( EFalse );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
#ifndef __WINS__
            retVal = CheckNetworkCoverageL();
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
        case EWlanScanSSID:
            {
            retVal = wlanServiceRecord->iWlanScanSSID;
            }
            break;
        case EWlanAllowSSIDRoaming:
            {
            retVal = wlanServiceRecord->iWlanAllowSSIDRoaming;
            }
            break;
        case ECmIPAddFromServer:
        case EWlanIpAddrFromServer:
            {
            retVal = serviceRecord->iIpAddrFromServer;
            }
            break;
        case ECmIPDNSAddrFromServer:
        case EWlanIpDNSAddrFromServer:
            {
            retVal = serviceRecord->iIpDnsAddrFromServer;
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case EWlanIp6DNSAddrFromServer:
            {
            retVal = serviceRecord->iIp6DnsAddrFromServer;
            }
            break;
        case EWlanEnableWpaPsk:
            {
            retVal = static_cast<TBool>( wlanServiceRecord->iWlanEnableWpaPsk );
            }
            break;
        case EWlan802_1xAllowUnencrypted:
            {
            retVal = static_cast<TBool>( wlanServiceRecord->iWlanWpaKeyLength );//TODO, change typecast to if-else structure?
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::GetBearerStringAttributeL
// --------------------------------------------------------------------------
//
HBufC* CCmPluginWlan::GetBearerStringAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERSTRINGATTRIBUTEL_ENTRY );

    HBufC* retVal = NULL;

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmIFNetworks:
        case EWlanIfNetworks:
            {
            if ( !serviceRecord->iIfNetworks.IsNull() )
                {
                retVal = ( serviceRecord->iIfNetworks.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPNetmask:
        case EWlanIpNetMask:
            {
            if ( !serviceRecord->iIpNetmask.IsNull() )
                {
                retVal = ( serviceRecord->iIpNetmask.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPGateway:
        case EWlanIpGateway:
            {
            if ( !serviceRecord->iIpGateway.IsNull() )
                {
                retVal = ( serviceRecord->iIpGateway.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPAddress:
        case EWlanIpAddr:
            {
            if ( !serviceRecord->iIpAddr.IsNull() )
                {
                retVal = ( serviceRecord->iIpAddr.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPNameServer1:
        case EWlanIpNameServer1:
            {
            if ( !serviceRecord->iIpNameServer1.IsNull() )
                {
                retVal = ( serviceRecord->iIpNameServer1.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPNameServer2:
        case EWlanIpNameServer2:
            {
            if ( !serviceRecord->iIpNameServer2.IsNull() )
                {
                retVal = ( serviceRecord->iIpNameServer2.GetL() ).AllocL();
                }
            }
            break;
        case ECmIP6NameServer1:
        case EWlanIp6NameServer1:
            {
            if ( !serviceRecord->iIp6NameServer1.IsNull() )
                {
                retVal = ( serviceRecord->iIp6NameServer1.GetL() ).AllocL();
                }
            }
            break;
        case ECmIP6NameServer2:
        case EWlanIp6NameServer2:
            {
            if ( !serviceRecord->iIp6NameServer2.IsNull() )
                {
                retVal = ( serviceRecord->iIp6NameServer2.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPAddrLeaseValidFrom:
        case EWlanIpAddrLeaseValidFrom:
            {
            if ( !serviceRecord->iIpAddrLeaseValidFrom.IsNull() )
                {
                retVal = ( serviceRecord->iIpAddrLeaseValidFrom.GetL() ).AllocL();
                }
            }
            break;
        case ECmIPAddrLeaseValidTo:
        case EWlanIpAddrLeaseValidTo:
            {
            if ( !serviceRecord->iIpAddrLeaseValidTo.IsNull() )
                {
                retVal = ( serviceRecord->iIpAddrLeaseValidTo.GetL() ).AllocL();
                }
            }
            break;
        case ECmConfigDaemonManagerName:
        case EWlanConfigDaemonManagerName:
            {
            if ( !serviceRecord->iConfigDaemonManagerName.IsNull() )
                {
                retVal = ( serviceRecord->iConfigDaemonManagerName.GetL() ).AllocL();
                }
            }
            break;
        case ECmConfigDaemonName:
        case EWlanConfigDaemonName:
            {
            if ( !serviceRecord->iConfigDaemonName.IsNull() )
                {
                retVal = ( serviceRecord->iConfigDaemonName.GetL() ).AllocL();
                }
            }
            break;
        case EWlanServiceExtensionTableName:
            {
            if ( !serviceRecord->iServiceExtensionTableName.IsNull() )
                {
                retVal = ( serviceRecord->iServiceExtensionTableName.GetL() ).AllocL();
                }
            }
            break;
        case EWlanSSID:
            {
            if ( !wlanServiceRecord->iWLanSSID.IsNull() )
                {
                retVal = ( wlanServiceRecord->iWLanSSID.GetL() ).AllocL();
                }
            }
            break;
        case EWlanUsedSSID:
            {
            if ( !wlanServiceRecord->iWLanUsedSSID.IsNull() )
                {
                retVal = ( wlanServiceRecord->iWLanUsedSSID.GetL() ).AllocL();
                }
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERSTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::GetBearerString8AttributeL
// --------------------------------------------------------------------------
//
HBufC8* CCmPluginWlan::GetBearerString8AttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERSTRING8ATTRIBUTEL_ENTRY );

    HBufC8* retVal( NULL );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EWlanWpaPreSharedKey:
            {
            if ( !wlanServiceRecord->iWLanWpaPreSharedKey.IsNull() )
                {
                retVal = ( wlanServiceRecord->iWLanWpaPreSharedKey.GetL() ).AllocL();
                }
            }
            break;
        case EWlanWepKey1InHex:
            {
            if ( !wlanServiceRecord->iWlanFormatKey1.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey1.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey1 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ConvertAsciiToHexL( wlanServiceRecord->iWLanWepKey1.GetL() );
                    }
                else
                    {
                    // Format is hex.
                    retVal = ( wlanServiceRecord->iWLanWepKey1.GetL() ).AllocL();
                    }
                }
            }
            break;
        case EWlanWepKey2InHex:
            {
            if ( !wlanServiceRecord->iWlanFormatKey2.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey2.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey2 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ConvertAsciiToHexL( wlanServiceRecord->iWLanWepKey2.GetL() );
                    }
                else
                    {
                    // Format is hex.
                    retVal = ( wlanServiceRecord->iWLanWepKey2.GetL() ).AllocL();
                    }
                }
            }
            break;
        case EWlanWepKey3InHex:
            {
            if ( !wlanServiceRecord->iWlanFormatKey3.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey3.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey3 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ConvertAsciiToHexL( wlanServiceRecord->iWLanWepKey3.GetL() );
                    }
                else
                    {
                    // Format is hex.
                    retVal = ( wlanServiceRecord->iWLanWepKey3.GetL() ).AllocL();
                    }
                }
            }
            break;
        case EWlanWepKey4InHex:
            {
            if ( !wlanServiceRecord->iWlanFormatKey4.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey4.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey4 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ConvertAsciiToHexL( wlanServiceRecord->iWLanWepKey4.GetL() );
                    }
                else
                    {
                    // Format is hex.
                    retVal = ( wlanServiceRecord->iWLanWepKey4.GetL() ).AllocL();
                    }
                }
            }
            break;
        case EWlanWepKey1InAscii:
            {
            if ( !wlanServiceRecord->iWlanFormatKey1.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey1.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey1 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ( wlanServiceRecord->iWLanWepKey1.GetL() ).AllocL();
                    }
                else
                    {
                    // Format is hex.
                    retVal = ConvertHexToAsciiL( wlanServiceRecord->iWLanWepKey1.GetL() );
                    }
                }
            }
            break;
        case EWlanWepKey2InAscii:
            {
            if ( !wlanServiceRecord->iWlanFormatKey2.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey2.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey2 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ( wlanServiceRecord->iWLanWepKey2.GetL() ).AllocL();
                    }
                else
                    {
                    // Format is hex.
                    retVal = ConvertHexToAsciiL( wlanServiceRecord->iWLanWepKey2.GetL() );
                    }
                }
            }
            break;
        case EWlanWepKey3InAscii:
            {
            if ( !wlanServiceRecord->iWlanFormatKey3.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey3.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey3 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ( wlanServiceRecord->iWLanWepKey3.GetL() ).AllocL();
                    }
                else
                    {
                    // Format is hex.
                    retVal = ConvertHexToAsciiL( wlanServiceRecord->iWLanWepKey3.GetL() );
                    }
                }
            }
            break;
        case EWlanWepKey4InAscii:
            {
            if ( !wlanServiceRecord->iWlanFormatKey4.IsNull() &&
                    !wlanServiceRecord->iWLanWepKey4.IsNull() )
                {
                if ( wlanServiceRecord->iWlanFormatKey4 == EWlanWepKeyFormatAscii )
                    {
                    // Format is ascii.
                    retVal = ( wlanServiceRecord->iWLanWepKey4.GetL() ).AllocL();
                    }
                else
                    {
                    // Format is hex.
                    retVal = ConvertHexToAsciiL( wlanServiceRecord->iWLanWepKey4.GetL() );
                    }
                }
            }
            break;
        case EWlanWapiPsk:
            {
            if ( !wlanServiceRecord->iWLanWpaPreSharedKey.IsNull() )
                {
                retVal = ( wlanServiceRecord->iWLanWpaPreSharedKey.GetL() ).AllocL();
                }
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERSTRING8ATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::SetBearerIntAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetBearerIntAttributeL(
        TUint32 aAttribute,
        TUint32 aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SETBEARERINTATTRIBUTEL_ENTRY );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

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
        case EWlanServiceId:
            {
            wlanServiceRecord->iWlanServiceId.SetL( aValue );
            }
            break;
        case EWlanConnectionMode:
            {
            wlanServiceRecord->iWlanConnMode.SetL( aValue );
            }
            break;
        case EWlanSecurityMode:
            {
            CheckSecurityAttributeValidityL( aAttribute, aValue );
            wlanServiceRecord->iWlanSecMode.SetL( aValue );
            // When setting security mode, also reset authentication mode to open.
            wlanServiceRecord->iWlanAuthMode.SetL( CMManager::EWlanAuthModeOpen );
            if ( aValue == EWlanSecMode802_1x )
                {
                // By default, do not allow unencrypted mode for 802.1x (Korea).
                wlanServiceRecord->iWlanWpaKeyLength.SetL( 0 );
                }
            }
            break;
        case EWlanAuthenticationMode:
            {
            CheckSecurityAttributeValidityL( aAttribute, aValue );
            wlanServiceRecord->iWlanAuthMode.SetL( aValue );
            }
            break;
        case EWlanChannelID:
            {
            wlanServiceRecord->iWlanChannelID.SetL( aValue );
            }
            break;
        case EWlanServiceExtensionTableRecordId:
            {
            serviceRecord->iServiceExtensionTableRecordId.SetL( aValue );
            }
            break;
        case EWlanWepKeyIndex:
            {
            CheckSecurityAttributeValidityL( aAttribute, aValue );
            wlanServiceRecord->iWlanWepIndex.SetL( aValue );
            }
            break;
        case EWlanWapiPskFormat:
            {
            CheckSecurityAttributeValidityL( aAttribute, aValue );
            wlanServiceRecord->iWlanFormatKey1.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_SETBEARERINTATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::SetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetBearerBoolAttributeL(
        TUint32 aAttribute,
        TBool aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SETBEARERBOOLATTRIBUTEL_ENTRY );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EWlanScanSSID:
            {
            wlanServiceRecord->iWlanScanSSID.SetL( aValue );
            }
            break;
        case EWlanAllowSSIDRoaming:
            {
            wlanServiceRecord->iWlanAllowSSIDRoaming.SetL( aValue );
            }
            break;
        case ECmIPAddFromServer:
        case EWlanIpAddrFromServer:
            {
            serviceRecord->iIpAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIPDNSAddrFromServer:
        case EWlanIpDNSAddrFromServer:
            {
            serviceRecord->iIpDnsAddrFromServer.SetL( aValue );
            }
            break;
        case ECmIP6DNSAddrFromServer:
        case EWlanIp6DNSAddrFromServer:
            {
            serviceRecord->iIp6DnsAddrFromServer.SetL( aValue );
            }
            break;
        case EWlanEnableWpaPsk:
            {
            wlanServiceRecord->iWlanEnableWpaPsk.SetL( static_cast<TUint>( aValue ) );
            }
            break;
        case EWlan802_1xAllowUnencrypted:
            {
            wlanServiceRecord->iWlanWpaKeyLength.SetL( static_cast<TUint>( aValue ) );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_SETBEARERBOOLATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::SetBearerStringAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetBearerStringAttributeL(
        TUint32 aAttribute,
        const TDesC16& aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SETBEARERSTRINGATTRIBUTEL_ENTRY );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

    switch ( aAttribute )
        {
        case ECmIFNetworks:
        case EWlanIfNetworks:
            {
            serviceRecord->iIfNetworks.SetL( aValue );
            }
            break;
        case ECmIPNetmask:
        case EWlanIpNetMask:
            {
            serviceRecord->iIpNetmask.SetL( aValue );
            }
            break;
        case ECmIPGateway:
        case EWlanIpGateway:
            {
            serviceRecord->iIpGateway.SetL( aValue );
            }
            break;
        case ECmIPAddress:
        case EWlanIpAddr:
            {
            if ( ( aValue.Compare( KUnspecifiedIPv4 ) == 0 ) ||
                    ( aValue.Compare( KNullDesC ) == 0 ) )
                {
                // If unspecified or NULL address is set dynamic
                // IP must be enabled.
                SetBearerBoolAttributeL( EWlanIpAddrFromServer,
                                         ETrue,
                                         aGenRecordArray,
                                         aBearerSpecRecordArray );
                }
            else
                {
                // If valid address is set dynamic IP is disabled.
                SetBearerBoolAttributeL( EWlanIpAddrFromServer,
                                         EFalse,
                                         aGenRecordArray,
                                         aBearerSpecRecordArray );
                }
            serviceRecord->iIpAddr.SetL( aValue );
            }
            break;
        case ECmIPNameServer1:
        case EWlanIpNameServer1:
            {
            serviceRecord->iIpNameServer1.SetL( aValue );
            }
            break;
        case ECmIPNameServer2:
        case EWlanIpNameServer2:
            {
            serviceRecord->iIpNameServer2.SetL( aValue );
            }
            break;
        case ECmIP6NameServer1:
        case EWlanIp6NameServer1:
            {
            serviceRecord->iIp6NameServer1.SetL( aValue );
            }
            break;
        case ECmIP6NameServer2:
        case EWlanIp6NameServer2:
            {
            serviceRecord->iIp6NameServer2.SetL( aValue );
            }
            break;
        case ECmIPAddrLeaseValidFrom:
        case EWlanIpAddrLeaseValidFrom:
            {
            serviceRecord->iIpAddrLeaseValidFrom.SetL( aValue );
            }
            break;
        case ECmIPAddrLeaseValidTo:
        case EWlanIpAddrLeaseValidTo:
            {
            serviceRecord->iIpAddrLeaseValidTo.SetL( aValue );
            }
            break;
        case ECmConfigDaemonManagerName:
        case EWlanConfigDaemonManagerName:
            {
            serviceRecord->iConfigDaemonManagerName.SetL( aValue );
            }
            break;
        case ECmConfigDaemonName:
        case EWlanConfigDaemonName:
            {
            serviceRecord->iConfigDaemonName.SetL( aValue );
            }
            break;
        case EWlanServiceExtensionTableName:
            {
            serviceRecord->iServiceExtensionTableName.SetL( aValue );
            }
            break;
        case EWlanSSID:
            {
            wlanServiceRecord->iWLanSSID.SetL( aValue );
            }
            break;
        case EWlanUsedSSID:
            {
            wlanServiceRecord->iWLanUsedSSID.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_SETBEARERSTRINGATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::SetBearerString8AttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetBearerString8AttributeL(
        TUint32 aAttribute,
        const TDesC8& aValue,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SETBEARERSTRING8ATTRIBUTEL_ENTRY );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    switch ( aAttribute )
        {
        case EWlanWpaPreSharedKey:
            {
            wlanServiceRecord->iWLanWpaPreSharedKey.SetL( aValue );
            wlanServiceRecord->iWlanWpaKeyLength.SetL( static_cast<TUint>( aValue.Length() ) );
            }
            break;
        case EWlanWepKey1InHex:
            {
            wlanServiceRecord->iWLanWepKey1.SetL( aValue );
            wlanServiceRecord->iWlanFormatKey1.SetL( EWlanWepKeyFormatHex );
            }
            break;
        case EWlanWepKey2InHex:
            {
            wlanServiceRecord->iWLanWepKey2.SetL( aValue );
            wlanServiceRecord->iWlanFormatKey2.SetL( EWlanWepKeyFormatHex );
            }
            break;
        case EWlanWepKey3InHex:
            {
            wlanServiceRecord->iWLanWepKey3.SetL( aValue );
            wlanServiceRecord->iWlanFormatKey3.SetL( EWlanWepKeyFormatHex );
            }
            break;
        case EWlanWepKey4InHex:
            {
            wlanServiceRecord->iWLanWepKey4.SetL( aValue );
            wlanServiceRecord->iWlanFormatKey4.SetL( EWlanWepKeyFormatHex );
            }
            break;
        case EWlanWepKey1InAscii:
            {
            HBufC8* valueInHex = ConvertAsciiToHexL( aValue );
            CleanupStack::PushL( valueInHex );
            wlanServiceRecord->iWLanWepKey1.SetL( *valueInHex );
            wlanServiceRecord->iWlanFormatKey1.SetL( EWlanWepKeyFormatHex );
            CleanupStack::PopAndDestroy( valueInHex );
            }
            break;
        case EWlanWepKey2InAscii:
            {
            HBufC8* valueInHex = ConvertAsciiToHexL( aValue );
            CleanupStack::PushL( valueInHex );
            wlanServiceRecord->iWLanWepKey2.SetL( *valueInHex );
            wlanServiceRecord->iWlanFormatKey2.SetL( EWlanWepKeyFormatHex );
            CleanupStack::PopAndDestroy( valueInHex );
            }
            break;
        case EWlanWepKey3InAscii:
            {
            HBufC8* valueInHex = ConvertAsciiToHexL( aValue );
            CleanupStack::PushL( valueInHex );
            wlanServiceRecord->iWLanWepKey3.SetL( *valueInHex );
            wlanServiceRecord->iWlanFormatKey3.SetL( EWlanWepKeyFormatHex );
            CleanupStack::PopAndDestroy( valueInHex );
            }
            break;
        case EWlanWepKey4InAscii:
            {
            HBufC8* valueInHex = ConvertAsciiToHexL( aValue );
            CleanupStack::PushL( valueInHex );
            wlanServiceRecord->iWLanWepKey4.SetL( *valueInHex );
            wlanServiceRecord->iWlanFormatKey4.SetL( EWlanWepKeyFormatHex );
            CleanupStack::PopAndDestroy( valueInHex );
            }
            break;
        case EWlanWapiPsk:
            {
            wlanServiceRecord->iWLanWpaPreSharedKey.SetL( aValue );
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_SETBEARERSTRING8ATTRIBUTEL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CanHandleIapIdL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::CanHandleIapIdL( TUint32 aIapId ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CANHANDLEIAPIDL_ENTRY );

    TBool retVal( EFalse );

    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>(
            CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord) );

    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );

    TRAPD( err, iapRecord->LoadL( iSession ) );

    if ( !err )
        {
        retVal = CanHandleIapIdL( iapRecord );
        }

    CleanupStack::PopAndDestroy( iapRecord );

    OstTraceFunctionExit0( CCMPLUGINWLAN_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CanHandleIapIdL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINWLAN_CANHANDLEIAPIDL_ENTRY );

    TBool retVal( EFalse );

    if ( ( TPtrC( aIapRecord->iServiceType ) == TPtrC( KCDTypeNameLANService ) ) &&
            TPtrC( aIapRecord->iBearerType ) == TPtrC( KCDTypeNameLANBearer ) )
        {
        // Check if there is a WLAN record with an iServiceId == iIapRecord->iService.
        CCDWlanServiceRecord* tmprec = new( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
        CleanupStack::PushL( tmprec );

        tmprec->iWlanServiceId.SetL( ( TUint32 )( aIapRecord->iService ) );
        if ( tmprec->FindL( iSession ) )
            {
            // we found at least one WLAN using this IAP,
            retVal = ETrue;
            }

        CleanupStack::PopAndDestroy(tmprec);
        }

    OstTraceFunctionExit0( DUP1_CCMPLUGINWLAN_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::DeleteBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::DeleteBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_DELETEBEARERRECORDSL_ENTRY );

    // As base class deletes service record, in this case LAN, only WLAN
    // related stuff needs to be deleted.
    iWlanServiceRecord->DeleteL( iSession );

    OstTraceFunctionExit0( CCMPLUGINWLAN_DELETEBEARERRECORDSL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::LoadServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::LoadServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_LOADSERVICERECORDL_ENTRY );

    if ( TPtrC( KCDTypeNameLANService ) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDLANServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

        ServiceRecord().SetRecordId( iIapRecord->iService );
        ServiceRecord().LoadL( iSession );
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_LOADSERVICERECORDL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::LoadBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::LoadBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_LOADBEARERRECORDSL_ENTRY );

    if ( !iWlanServiceRecord )
        {
        iWlanServiceRecord = new( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
        }
    iWlanServiceRecord->SetRecordId( 0 );

    iWlanServiceRecord->iWlanServiceId.SetL( ( TUint32 )( iIapRecord->iService ) );

    if ( iWlanServiceRecord->FindL( iSession ) )
        {
        iWlanServiceRecord->LoadL( iSession );
        }
    else
        {
        // not found -> error
        User::Leave( KErrNotFound );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_LOADBEARERRECORDSL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CreateServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::CreateServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CREATESERVICERECORDL_ENTRY );

    delete iServiceRecord;
    iServiceRecord = NULL;

    iServiceRecord = static_cast<CCDLANServiceRecord*>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

    CCDLANServiceRecord* lanServiceRecord = static_cast<CCDLANServiceRecord*>( iServiceRecord );

    if ( FeatureManager::FeatureSupported( KFeatureIdIPv6 ) )
        {
        lanServiceRecord->iIfNetworks.SetL( KDefIspIfNetworksIPv4IPv6 );
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

    // Create WLAN service record.
    CreateWlanServiceRecordL();

    OstTraceFunctionExit0( CCMPLUGINWLAN_CREATESERVICERECORDL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CreateWlanServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::CreateWlanServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CREATEWLANSERVICERECORDL_ENTRY );

    delete iWlanServiceRecord;

    iWlanServiceRecord = NULL;

    iWlanServiceRecord = new( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
    iWlanServiceRecord->SetRecordId( KCDNewRecordRequest );

    // Some attributes need to have default values set.
    iWlanServiceRecord->iRecordName.SetL( iIapRecord->iRecordName );
    iWlanServiceRecord->iWlanConnMode.SetL( CMManager::EInfra );
    iWlanServiceRecord->iWlanSecMode.SetL( CMManager::EWlanSecModeOpen );
    iWlanServiceRecord->iWlanAuthMode.SetL( CMManager::EWlanAuthModeOpen );
    iWlanServiceRecord->iWlanScanSSID.SetL( EFalse );

    OstTraceFunctionExit0( CCMPLUGINWLAN_CREATEWLANSERVICERECORDL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginWlan::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginWlan::PreparePluginToLoadRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_PREPAREPLUGINTOLOADRECORDSL_ENTRY );

    OstTraceFunctionExit0( CCMPLUGINWLAN_PREPAREPLUGINTOLOADRECORDSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginWlan::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginWlan::PreparePluginToUpdateRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_PREPAREPLUGINTOUPDATERECORDSL_ENTRY );

    CCDWlanServiceRecord* wlanServiceRecord =
            static_cast<CCDWlanServiceRecord*>(
                    aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    CCDIAPRecord* iapRecord =
            static_cast<CCDIAPRecord*>(
                    aGenRecordArray[KIapRecordIndex] );

    CheckIfNameModifiedL( iapRecord, wlanServiceRecord );
    wlanServiceRecord->iWlanServiceId.SetL( ( TUint32 )( iapRecord->iService ) );

    CCDLANServiceRecord* serviceRecord =
            static_cast<CCDLANServiceRecord*>(
                    aGenRecordArray[KServiceRecordIndex] );

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

    OstTraceFunctionExit0( CCMPLUGINWLAN_PREPAREPLUGINTOUPDATERECORDSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginWlan::UpdateServiceRecordL()
// ----------------------------------------------------------------------------
//
void CCmPluginWlan::UpdateServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_UPDATESERVICERECORDL_ENTRY );

    // Delete the original record and create a copy from the client's copy.
    delete iServiceRecord;
    iServiceRecord = NULL;

    iServiceRecord = static_cast<CCDLANServiceRecord *>(
            CCDRecordBase::RecordFactoryL( KCDTIdLANServiceRecord ) );

    // LAN Service copy does not work so we have to copy it manually.

    // Plugin's commsdat copy.
    CCDLANServiceRecord* origServiceRecord = static_cast<CCDLANServiceRecord*>( iServiceRecord );

    // Client's copy of lan service record.
    CCDLANServiceRecord* clientServiceRecordCopy =
            static_cast<CCDLANServiceRecord*>( aGenRecordArray[KServiceRecordIndex] );

    if ( !clientServiceRecordCopy->iRecordTag.IsNull() )
        {
        origServiceRecord->iRecordTag.SetL( clientServiceRecordCopy->iRecordTag );
        }
    if ( !clientServiceRecordCopy->iRecordName.IsNull() )
        {
        origServiceRecord->iRecordName.SetL( clientServiceRecordCopy->iRecordName );
        }
    if ( !clientServiceRecordCopy->iServiceEnableLlmnr.IsNull() )
        {
        origServiceRecord->iServiceEnableLlmnr.SetL( clientServiceRecordCopy->iServiceEnableLlmnr );
        }
    if ( !clientServiceRecordCopy->iIfNetworks.IsNull() )
        {
        origServiceRecord->iIfNetworks.SetL( clientServiceRecordCopy->iIfNetworks );
        }
    if ( !clientServiceRecordCopy->iIpNetmask.IsNull() )
        {
        origServiceRecord->iIpNetmask.SetL( clientServiceRecordCopy->iIpNetmask );
        }
    if ( !clientServiceRecordCopy->iIpGateway.IsNull() )
        {
        origServiceRecord->iIpGateway.SetL( clientServiceRecordCopy->iIpGateway );
        }
    if ( !clientServiceRecordCopy->iIpAddrFromServer.IsNull() )
        {
        origServiceRecord->iIpAddrFromServer.SetL( clientServiceRecordCopy->iIpAddrFromServer );
        }
    if ( !clientServiceRecordCopy->iIpAddr.IsNull() )
        {
        origServiceRecord->iIpAddr.SetL( clientServiceRecordCopy->iIpAddr );
        }
    if ( !clientServiceRecordCopy->iIpDnsAddrFromServer.IsNull() )
        {
        origServiceRecord->iIpDnsAddrFromServer.SetL( clientServiceRecordCopy->iIpDnsAddrFromServer );
        }
    if ( !clientServiceRecordCopy->iIpNameServer1.IsNull() )
        {
        origServiceRecord->iIpNameServer1.SetL( clientServiceRecordCopy->iIpNameServer1 );
        }
    if ( !clientServiceRecordCopy->iIpNameServer2.IsNull() )
        {
        origServiceRecord->iIpNameServer2.SetL( clientServiceRecordCopy->iIpNameServer2 );
        }
    if ( !clientServiceRecordCopy->iIp6DnsAddrFromServer.IsNull() )
        {
        origServiceRecord->iIp6DnsAddrFromServer.SetL( clientServiceRecordCopy->iIp6DnsAddrFromServer );
        }
    if ( !clientServiceRecordCopy->iIp6NameServer1.IsNull() )
        {
        origServiceRecord->iIp6NameServer1.SetL( clientServiceRecordCopy->iIp6NameServer1 );
        }
    if ( !clientServiceRecordCopy->iIp6NameServer2.IsNull() )
        {
        origServiceRecord->iIp6NameServer2.SetL( clientServiceRecordCopy->iIp6NameServer2 );
        }
    if ( !clientServiceRecordCopy->iIpAddrLeaseValidFrom.IsNull() )
        {
        origServiceRecord->iIpAddrLeaseValidFrom.SetL( clientServiceRecordCopy->iIpAddrLeaseValidFrom );
        }
    if ( !clientServiceRecordCopy->iIpAddrLeaseValidTo.IsNull() )
        {
        origServiceRecord->iIpAddrLeaseValidTo.SetL( clientServiceRecordCopy->iIpAddrLeaseValidTo );
        }
    if ( !clientServiceRecordCopy->iConfigDaemonManagerName.IsNull() )
        {
        origServiceRecord->iConfigDaemonManagerName.SetL( clientServiceRecordCopy->iConfigDaemonManagerName );
        }
    if ( !clientServiceRecordCopy->iConfigDaemonName.IsNull() )
        {
        origServiceRecord->iConfigDaemonName.SetL( clientServiceRecordCopy->iConfigDaemonName );
        }
    if ( !clientServiceRecordCopy->iServiceExtensionTableName.IsNull() )
        {
        origServiceRecord->iServiceExtensionTableName.SetL( clientServiceRecordCopy->iServiceExtensionTableName );
        }
    if ( !clientServiceRecordCopy->iServiceExtensionTableRecordId.IsNull() )
        {
        origServiceRecord->iServiceExtensionTableRecordId.SetL( clientServiceRecordCopy->iServiceExtensionTableRecordId ); //TODO, check this works ok.
        }

    origServiceRecord->SetElementId( clientServiceRecordCopy->ElementId() );

    if ( !origServiceRecord->RecordId() )
        {
        origServiceRecord->SetRecordId( KCDNewRecordRequest );
        origServiceRecord->StoreL( iSession );

        // Update received element ID to client's copy too.
        clientServiceRecordCopy->SetElementId( origServiceRecord->ElementId() );
        // Update needed values to other table records too (wlanservice).
        CCDWlanServiceRecord* wlanServiceRecord = static_cast<CCDWlanServiceRecord *>(
                aBearerSpecRecordArray[KWlanServiceRecordIndex] );
        wlanServiceRecord->iWlanServiceId.SetL( origServiceRecord->RecordId() );
        }
    else
        {
        origServiceRecord->ModifyL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_UPDATESERVICERECORDL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCmPluginWlan::UpdateBearerRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginWlan::UpdateBearerRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_UPDATEBEARERRECORDSL_ENTRY );

    // Delete the original record and create a copy from the client's copy.
    delete iWlanServiceRecord;
    iWlanServiceRecord = NULL;

    iWlanServiceRecord = new( ELeave ) CCDWlanServiceRecord( iWlanTableId );

    // Client's copy of wlan service record.
    CCDWlanServiceRecord* clientWlanServiceRecord = static_cast<CCDWlanServiceRecord*>(
            aBearerSpecRecordArray[KWlanServiceRecordIndex] );

    if ( !clientWlanServiceRecord->iRecordTag.IsNull() )
        {
        iWlanServiceRecord->iRecordTag.SetL(
                clientWlanServiceRecord->iRecordTag );
        }
    if ( !clientWlanServiceRecord->iRecordName.IsNull() )
        {
        iWlanServiceRecord->iRecordName.SetL(
                clientWlanServiceRecord->iRecordName );
        }
    if ( !clientWlanServiceRecord->iWlanServiceId.IsNull() )
        {
        iWlanServiceRecord->iWlanServiceId.SetL(
                clientWlanServiceRecord->iWlanServiceId );
        }
    if ( !clientWlanServiceRecord->iWlanConnMode.IsNull() )
        {
        iWlanServiceRecord->iWlanConnMode.SetL(
                clientWlanServiceRecord->iWlanConnMode );
        }
    if ( !clientWlanServiceRecord->iWLanSSID.IsNull() )
        {
        iWlanServiceRecord->iWLanSSID.SetL(
                clientWlanServiceRecord->iWLanSSID );
        }
    if ( !clientWlanServiceRecord->iWLanUsedSSID.IsNull() )
        {
        iWlanServiceRecord->iWLanUsedSSID.SetL(
                clientWlanServiceRecord->iWLanUsedSSID );
        }
    if ( !clientWlanServiceRecord->iWLanWepKey1.IsNull() )
        {
        iWlanServiceRecord->iWLanWepKey1.SetL(
                clientWlanServiceRecord->iWLanWepKey1 );
        }
    if ( !clientWlanServiceRecord->iWLanWepKey2.IsNull() )
        {
        iWlanServiceRecord->iWLanWepKey2.SetL(
                clientWlanServiceRecord->iWLanWepKey2 );
        }
    if ( !clientWlanServiceRecord->iWLanWepKey3.IsNull() )
        {
        iWlanServiceRecord->iWLanWepKey3.SetL(
                clientWlanServiceRecord->iWLanWepKey3 );
        }
    if ( !clientWlanServiceRecord->iWLanWepKey4.IsNull() )
        {
        iWlanServiceRecord->iWLanWepKey4.SetL(
                clientWlanServiceRecord->iWLanWepKey4 );
        }
    if ( !clientWlanServiceRecord->iWlanWepIndex.IsNull() )
        {
        iWlanServiceRecord->iWlanWepIndex.SetL(
                clientWlanServiceRecord->iWlanWepIndex );
        }
    if ( !clientWlanServiceRecord->iWlanSecMode.IsNull() )
        {
        iWlanServiceRecord->iWlanSecMode.SetL(
                clientWlanServiceRecord->iWlanSecMode );
        }
    if ( !clientWlanServiceRecord->iWlanAuthMode.IsNull() )
        {
        iWlanServiceRecord->iWlanAuthMode.SetL(
                clientWlanServiceRecord->iWlanAuthMode );
        }
    if ( !clientWlanServiceRecord->iWlanEnableWpaPsk.IsNull() )
        {
        iWlanServiceRecord->iWlanEnableWpaPsk.SetL(
                clientWlanServiceRecord->iWlanEnableWpaPsk );
        }
    if ( !clientWlanServiceRecord->iWLanWpaPreSharedKey.IsNull() )
        {
        iWlanServiceRecord->iWLanWpaPreSharedKey.SetL(
                clientWlanServiceRecord->iWLanWpaPreSharedKey );
        }
    if ( !clientWlanServiceRecord->iWlanWpaKeyLength.IsNull() )
        {
        iWlanServiceRecord->iWlanWpaKeyLength.SetL(
                clientWlanServiceRecord->iWlanWpaKeyLength );
        }
    if ( !clientWlanServiceRecord->iWLanEaps.IsNull() )
        {
        iWlanServiceRecord->iWLanEaps.SetL(
                clientWlanServiceRecord->iWLanEaps );
        }
    if ( !clientWlanServiceRecord->iWlanScanSSID.IsNull() )
        {
        iWlanServiceRecord->iWlanScanSSID.SetL(
                clientWlanServiceRecord->iWlanScanSSID );
        }
    if ( !clientWlanServiceRecord->iWlanChannelID.IsNull() )
        {
        iWlanServiceRecord->iWlanChannelID.SetL(
                clientWlanServiceRecord->iWlanChannelID );
        }
    if ( !clientWlanServiceRecord->iWlanFormatKey1.IsNull() )
        {
        iWlanServiceRecord->iWlanFormatKey1.SetL(
                clientWlanServiceRecord->iWlanFormatKey1 );
        }
    if ( !clientWlanServiceRecord->iWlanFormatKey2.IsNull() )
        {
        iWlanServiceRecord->iWlanFormatKey2.SetL(
                clientWlanServiceRecord->iWlanFormatKey2 );
        }
    if ( !clientWlanServiceRecord->iWlanFormatKey3.IsNull() )
        {
        iWlanServiceRecord->iWlanFormatKey3.SetL(
                clientWlanServiceRecord->iWlanFormatKey3 );
        }
    if ( !clientWlanServiceRecord->iWlanFormatKey4.IsNull() )
        {
        iWlanServiceRecord->iWlanFormatKey4.SetL(
                clientWlanServiceRecord->iWlanFormatKey4 );
        }
    if ( !clientWlanServiceRecord->iWlanAllowSSIDRoaming.IsNull() )
        {
        iWlanServiceRecord->iWlanAllowSSIDRoaming.SetL(
                clientWlanServiceRecord->iWlanAllowSSIDRoaming );
        }
    if ( !clientWlanServiceRecord->iWLanEnabledEaps.IsNull() )
        {
        iWlanServiceRecord->iWLanEnabledEaps.SetL(
                clientWlanServiceRecord->iWLanEnabledEaps );
        }
    if ( !clientWlanServiceRecord->iWLanDisabledEaps.IsNull() )
        {
        iWlanServiceRecord->iWLanDisabledEaps.SetL(
                clientWlanServiceRecord->iWLanDisabledEaps );
        }

    iWlanServiceRecord->SetElementId( clientWlanServiceRecord->ElementId() );

    if ( !iWlanServiceRecord->RecordId() )
        {
        iWlanServiceRecord->SetRecordId( KCDNewRecordRequest );
        iWlanServiceRecord->StoreL( iSession );

        // Update received element ID to client's copy too.
        clientWlanServiceRecord->SetElementId( iWlanServiceRecord->ElementId() );
        }
    else
        {
        iWlanServiceRecord->ModifyL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_UPDATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerSpecificRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::GetBearerSpecificRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERSPECIFICRECORDSL_ENTRY );

    if ( !iWlanServiceRecord )
        {
        // IAP not yet in CommsDat.
        GetDefaultWlanServiceRecordL( aRecordArray );
        }
    else
        {
        CCDWlanServiceRecord* wlanServiceRecord = new( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
        CleanupStack::PushL( wlanServiceRecord );
        CopyWlanServiceRecordL( iWlanServiceRecord, wlanServiceRecord );
        wlanServiceRecord->SetElementId( iWlanServiceRecord->ElementId() );
        aRecordArray.AppendL( wlanServiceRecord );
        CleanupStack::Pop( wlanServiceRecord );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERSPECIFICRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::CopyServiceRecordL
// ---------------------------------------------------------------------------
//
CommsDat::CCDRecordBase* CCmPluginWlan::CopyServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_COPYSERVICERECORDL_ENTRY );

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

    OstTraceFunctionExit0( CCMPLUGINWLAN_COPYSERVICERECORDL_EXIT );
    return serviceRecord;
    }

// ----------------------------------------------------------------------------
// CCmPluginWlan::CheckNetworkCoverageL()
// ----------------------------------------------------------------------------
//
TBool CCmPluginWlan::CheckNetworkCoverageL() const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CHECKNETWORKCOVERAGEL_ENTRY );

#ifdef __WINS__
    OstTraceFunctionExit0( CCMPLUGINWLAN_CHECKNETWORKCOVERAGEL_EXIT );

    return ETrue;
#else
    CCmWlanCoverageCheck* coverage = new (ELeave) CCmWlanCoverageCheck;
    CleanupStack::PushL( coverage );

    TBool retVal( EFalse );
    retVal = coverage->GetCoverageL();

    CleanupStack::PopAndDestroy( coverage );

    OstTraceFunctionExit0( DUP1_CCMPLUGINWLAN_CHECKNETWORKCOVERAGEL_EXIT );

    return retVal;
#endif  // __WINS
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::SetDaemonNameL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::SetDaemonNameL(
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_SETDAEMONNAMEL_ENTRY );

    // we have to check first that these values has not been
    // set(or changed) by any 3rd party sw to ensure that
    // we don't break their settings
    HBufC* daemonName = GetBearerStringAttributeL( ECmConfigDaemonManagerName,
                                                   aGenRecordArray,
                                                   aBearerSpecRecordArray );
    if ( daemonName )
        {
        if ( daemonName->Compare( KDaemonManagerName ) != 0 )
            {
            delete daemonName;

            OstTraceFunctionExit0( CCMPLUGINWLAN_SETDAEMONNAMEL_EXIT );

            return;
            }
        delete daemonName;
        }

    // use DHCP if we can
    TBool ipfromSrv = GetBearerBoolAttributeL( ECmIPAddFromServer,
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

    OstTraceFunctionExit0( DUP1_CCMPLUGINWLAN_SETDAEMONNAMEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ResetBearerRecords
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::ResetBearerRecords()
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_RESETBEARERRECORDS_ENTRY );

    delete iWlanServiceRecord;
    iWlanServiceRecord = NULL;

    OstTraceFunctionExit0( CCMPLUGINWLAN_RESETBEARERRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::CopyWlanServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::CopyWlanServiceRecordL(
        CCDWlanServiceRecord* aSourceRecord,
        CCDWlanServiceRecord* aDestRecord )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_COPYWLANSERVICERECORDL_ENTRY );

    if ( !( aSourceRecord && aDestRecord ) )
        {
        User::Leave( KErrArgument );
        }

    if ( !aSourceRecord->iRecordTag.IsNull() )
        {
        aDestRecord->iRecordTag.SetL( aSourceRecord->iRecordTag );
        }
    if ( !aSourceRecord->iRecordName.IsNull() )
        {
        aDestRecord->iRecordName.SetL( aSourceRecord->iRecordName );
        }

    if ( !aSourceRecord->iWlanServiceId.IsNull() )
        {
        aDestRecord->iWlanServiceId.SetL( aSourceRecord->iWlanServiceId );
        }
    if ( !aSourceRecord->iWlanConnMode.IsNull() )
        {
        aDestRecord->iWlanConnMode.SetL( aSourceRecord->iWlanConnMode );
        }
    if ( !aSourceRecord->iWLanSSID.IsNull() )
        {
        aDestRecord->iWLanSSID.SetL( aSourceRecord->iWLanSSID );
        }
    if ( !aSourceRecord->iWLanUsedSSID.IsNull() )
        {
        aDestRecord->iWLanUsedSSID.SetL( aSourceRecord->iWLanUsedSSID );
        }
    if ( !aSourceRecord->iWLanWepKey1.IsNull() )
        {
        aDestRecord->iWLanWepKey1.SetL( aSourceRecord->iWLanWepKey1 );
        }
    if ( !aSourceRecord->iWLanWepKey2.IsNull() )
        {
        aDestRecord->iWLanWepKey2.SetL( aSourceRecord->iWLanWepKey2 );
        }
    if ( !aSourceRecord->iWLanWepKey3.IsNull() )
        {
        aDestRecord->iWLanWepKey3.SetL( aSourceRecord->iWLanWepKey3 );
        }
    if ( !aSourceRecord->iWLanWepKey4.IsNull() )
        {
        aDestRecord->iWLanWepKey4.SetL( aSourceRecord->iWLanWepKey4 );
        }
    if ( !aSourceRecord->iWlanWepIndex.IsNull() )
        {
        aDestRecord->iWlanWepIndex.SetL( aSourceRecord->iWlanWepIndex );
        }
    if ( !aSourceRecord->iWlanSecMode.IsNull() )
        {
        aDestRecord->iWlanSecMode.SetL( aSourceRecord->iWlanSecMode );
        }
    if ( !aSourceRecord->iWlanAuthMode.IsNull() )
        {
        aDestRecord->iWlanAuthMode.SetL( aSourceRecord->iWlanAuthMode );
        }
    if ( !aSourceRecord->iWlanEnableWpaPsk.IsNull() )
        {
        aDestRecord->iWlanEnableWpaPsk.SetL( aSourceRecord->iWlanEnableWpaPsk );
        }
    if ( !aSourceRecord->iWLanWpaPreSharedKey.IsNull() )
        {
        aDestRecord->iWLanWpaPreSharedKey.SetL( aSourceRecord->iWLanWpaPreSharedKey );
        }
    if ( !aSourceRecord->iWlanWpaKeyLength.IsNull() )
        {
        aDestRecord->iWlanWpaKeyLength.SetL( aSourceRecord->iWlanWpaKeyLength );
        }
    if ( !aSourceRecord->iWLanEaps.IsNull() )
        {
        aDestRecord->iWLanEaps.SetL( aSourceRecord->iWLanEaps );
        }
    if ( !aSourceRecord->iWlanScanSSID.IsNull() )
        {
        aDestRecord->iWlanScanSSID.SetL( aSourceRecord->iWlanScanSSID );
        }
    if ( !aSourceRecord->iWlanChannelID.IsNull() )
        {
        aDestRecord->iWlanChannelID.SetL( aSourceRecord->iWlanChannelID );
        }
    if ( !aSourceRecord->iWlanFormatKey1.IsNull() )
        {
        aDestRecord->iWlanFormatKey1.SetL( aSourceRecord->iWlanFormatKey1 );
        }
    if ( !aSourceRecord->iWlanFormatKey2.IsNull() )
        {
        aDestRecord->iWlanFormatKey2.SetL( aSourceRecord->iWlanFormatKey2 );
        }
    if ( !aSourceRecord->iWlanFormatKey3.IsNull() )
        {
        aDestRecord->iWlanFormatKey3.SetL( aSourceRecord->iWlanFormatKey3 );
        }
    if ( !aSourceRecord->iWlanFormatKey4.IsNull() )
        {
        aDestRecord->iWlanFormatKey4.SetL( aSourceRecord->iWlanFormatKey4 );
        }
    if ( !aSourceRecord->iWlanAllowSSIDRoaming.IsNull() )
        {
        aDestRecord->iWlanAllowSSIDRoaming.SetL( aSourceRecord->iWlanAllowSSIDRoaming );
        }
    if ( !aSourceRecord->iWLanEnabledEaps.IsNull() )
        {
        aDestRecord->iWLanEnabledEaps.SetL( aSourceRecord->iWLanEnabledEaps );
        }
    if ( !aSourceRecord->iWLanDisabledEaps.IsNull() )
        {
        aDestRecord->iWLanDisabledEaps.SetL( aSourceRecord->iWLanDisabledEaps );
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_COPYWLANSERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetDefaultWlanServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::GetDefaultWlanServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETDEFAULTWLANSERVICERECORDL_ENTRY );

    CCDWlanServiceRecord* wlanServiceRecord = new( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
    CleanupStack::PushL( wlanServiceRecord );

    wlanServiceRecord->SetRecordId( KCDNewRecordRequest );

    wlanServiceRecord->iRecordTag.SetL( 1 );
    wlanServiceRecord->iRecordName.SetL( iIapRecord->iRecordName );
    wlanServiceRecord->iWlanConnMode.SetL( CMManager::EInfra );
    wlanServiceRecord->iWlanSecMode.SetL( CMManager::EWlanSecModeOpen );
    wlanServiceRecord->iWlanScanSSID.SetL( EFalse );

    aRecordArray.AppendL( wlanServiceRecord );

    CleanupStack::Pop( wlanServiceRecord );

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETDEFAULTWLANSERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginWlan::GetBearerInfoIntL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERINFOINTL_ENTRY );

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
            retVal = KCommDbBearerWLAN;
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
            // This is extension from LAN plugin.
            retVal = KExtensionBaseLevel + 1;
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

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmPluginWlan::GetBearerInfoBoolL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERINFOBOOLL_ENTRY );

    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
#ifndef __WINS__
            retVal = CheckNetworkCoverageL();
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

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginWlan::GetBearerInfoStringL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERINFOSTRINGL_ENTRY );

    HBufC* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
        break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERINFOSTRINGL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerInfoString8L
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginWlan::GetBearerInfoString8L( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERINFOSTRING8L_ENTRY );

    HBufC8* retVal( NULL );
    switch ( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
        break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERINFOSTRING8L_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::CheckSecurityAttributeValidityL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::CheckSecurityAttributeValidityL(
        const TUint32 aAttribute,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_CHECKSECURITYATTRIBUTEVALIDITYL_ENTRY );

    switch ( aAttribute )
        {
        case CMManager::EWlanConnectionMode:
            {
            switch ( aValue )
                {
                case CMManager::EAdhoc:
                case CMManager::EInfra:
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            }
            break;

        case CMManager::EWlanSecurityMode:
            {
            switch ( aValue )
                {
                case CMManager::EWlanSecModeOpen:
                case CMManager::EWlanSecModeWep:
                case CMManager::EWlanSecMode802_1x:
                case CMManager::EWlanSecModeWpa:
                case CMManager::EWlanSecModeWpa2:
                case CMManager::EWlanSecModeWAPI:
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            }
            break;

        case CMManager::EWlanAuthenticationMode:
            {
            switch ( aValue )
                {
                case CMManager::EWlanAuthModeOpen:
                case CMManager::EWlanAuthModeShared:
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            }
            break;

        case CMManager::EWlanEnableWpaPsk:     // Boolean
        case CMManager::EWlanWpaPreSharedKey:  // Valid values: 0, 1, 8-64
            break;

        case CMManager::EWlanWepKey1InHex:     // Length should be 10 or 26.
        case CMManager::EWlanWepKey2InHex:
        case CMManager::EWlanWepKey3InHex:
        case CMManager::EWlanWepKey4InHex:
        case CMManager::EWlanWepKey1InAscii:   // Length should be 5 or 13.
        case CMManager::EWlanWepKey2InAscii:
        case CMManager::EWlanWepKey3InAscii:
        case CMManager::EWlanWepKey4InAscii:
            break;

        case CMManager::EWlanWepKeyIndex:
            {
            switch ( aValue )
                {
                case CMManager::EWepKeyIndex1:
                case CMManager::EWepKeyIndex2:
                case CMManager::EWepKeyIndex3:
                case CMManager::EWepKeyIndex4:
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            }
            break;

        case CMManager::EWlanWapiPsk:
            break;

        case CMManager::EWlanWapiPskFormat:
            {
            switch ( aValue )
                {
                case CMManager::EWlanWapiPskFormatAscii:
                case CMManager::EWlanWapiPskFormatHex:
                    break;
                default:
                    User::Leave( KErrArgument );
                    break;
                }
            }
            break;

        case CMManager::EWlan802_1xAllowUnencrypted:   // Boolean
            break;

        default:
            User::Leave( KErrArgument );
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINWLAN_CHECKSECURITYATTRIBUTEVALIDITYL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::CopyBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_COPYBEARERRECORDSL_ENTRY );

    CCmPluginWlan* plugin = static_cast<CCmPluginWlan*>( aCopyInstance );
    plugin->iWlanServiceRecord = new( ELeave ) CCDWlanServiceRecord( iWlanTableId );

    CopyWlanServiceRecordL( iWlanServiceRecord, plugin->iWlanServiceRecord );
    plugin->iWlanServiceRecord->iWlanServiceId.SetL( 0 );
    plugin->iWAPISupported = iWAPISupported;

    OstTraceFunctionExit0( CCMPLUGINWLAN_COPYBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::PrepareToCopyDataL( CCmPluginBaseEng* /*aCopyInstance*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_PREPARETOCOPYDATAL_ENTRY );

    OstTraceFunctionExit0( CCMPLUGINWLAN_PREPARETOCOPYDATAL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetBearerTableIdsToBeObserved
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::GetBearerTableIdsToBeObservedL(
        RArray<TUint32>& aTableIdArray ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINWLAN_GETBEARERTABLEIDSTOBEOBSERVEDL_ENTRY );

    aTableIdArray.AppendL( KCDTIdLANServiceRecord );
    aTableIdArray.AppendL( KCDTIdLANBearerRecord );
    aTableIdArray.AppendL( iWlanTableId );

    OstTraceFunctionExit0( CCMPLUGINWLAN_GETBEARERTABLEIDSTOBEOBSERVEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ConvertAsciiToHexL
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginWlan::ConvertAsciiToHexL( const TDesC8& aSource )
    {
    // Characters used in hex-format.
    _LIT( hex, "0123456789ABCDEF" );

    // Length of ascii-format source string.
    TInt length = aSource.Length();
    //TInt length = aSource.Size();

    if ( length <= 0 )
        {
        HBufC8* result = KNullDesC8().AllocL();
        return result;
        }

    HBufC8* result = HBufC8::NewLC( length * 2 );
    TPtr8 ptr = result->Des();

    // Iterate for each character in ascii-format source string.
    for ( TInt i = 0; i < length; i++ )
        {
        // Next character in ascii-format source string to be converted.
        TText8 ch = aSource[i];

        // Convert and append the higher 4 bits of the character.
        ptr.Append( hex()[ ( ch / 16 ) & 0x0f ] );

        // Convert and append the lower 4 bits of the character.
        ptr.Append( hex()[ ch & 0x0f ] );
        }

    CleanupStack::Pop( result );
    return result; // Ownership is transfered.
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ConvertHexToAsciiL
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginWlan::ConvertHexToAsciiL( const TDesC8& aSource )
    {
    // Length of hex-format source string.
    TInt length = aSource.Length();

    if ( length <= 0 )
        {
        HBufC8* result = KNullDesC8().AllocL();
        return result;
        }

    HBufC8* result = HBufC8::NewLC( ( length + 1 ) / 2 );
    TPtr8 ptr = result->Des();

    TInt i( 0 );
    TUint8 ch( 0 );
    if ( length % 2 )
        {
        ch = ConvertHexCharToNumberL( aSource[i] );
        ptr.Append( ch );
        i++;
        }
    for ( ; i < length; )
        {
        ch = ConvertHexCharToNumberL( aSource[i] ) << 4;
        i++;
        ch += ConvertHexCharToNumberL( aSource[i] );
        i++;
        ptr.Append( ch );
        }

    CleanupStack::Pop( result );
    return result; // Ownership is transfered.
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::ConvertHexCharToNumberL
// ---------------------------------------------------------------------------
//
TUint8 CCmPluginWlan::ConvertHexCharToNumberL( const TUint8 aHexChar )
    {
    TUint8 result( 0 );
    if ( '0' <= aHexChar && aHexChar <= '9' )
        {
        result = aHexChar - '0';
        }
    else if ( 'a' <= aHexChar && aHexChar <= 'f' )
        {
        result = aHexChar - 'a' + 10u;
        }
    else if ( 'A' <= aHexChar && aHexChar <= 'F' )
        {
        result = aHexChar - 'A' + 10u;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::GetDefPriorityL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginWlan::GetDefPriorityL( const TUint32 aAttribute ) const//TODO, OST
    {

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

    return retVal;
    }

// End of File
