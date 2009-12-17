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
* Description:  Implementation of WLAN Plugin interface implementation 
*                for WLAN Bearer
*
*/


#include <commdb.h>
#include <ecom/ecom.h>		// For REComSession
#include <f32file.h>    // for TParse
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <ConnectionUiUtilities.h>

#include <cmpluginbaseeng.h>
#include <cmpluginlanbase.h>
#include <mcmdexec.h>
#include <cmpbasesettingsdlg.h>
#include <cmpluginwlan.mbg>
#include <cmwlanui.rsg>
#include <cmmanager.rsg>
#include <cmcommonui.h>
#include <wlancontainer.h>

#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>
#include <featmgr.h>
#include <ConeResLoader.h>
#include <AknWaitDialog.h>
#include <uikon/eiksrvui.h>
#include <AknGlobalNote.h>

#include "cmwlancommonconstants.h"
#include "cmpluginwlan.h"
#include "cmwlancoveragecheck.h"
#include "cmlogger.h"
#include "cmpwlansettingsdlg.h"
#include <cmpluginwlandef.h>
#include <cmcommonconstants.h>
#include "cmmanagerimpl.h"
#include "cmpwlanactivewaiter.h"

#include <wifiprotuiinprocess.h>

using namespace CMManager;

#include "wapisecuritysettingsui.h"

// CONSTANTS
const TUint32 KDefaultPriorityWLan = 0;

const TInt KWlanSingleNetwork = 1; // Number of WLAN networks is 1
const TInt KWlanMaxKeyLength = 64;
const TInt KWlanLastSocketActivityTimeout = -1;
const TInt KWlanLastSessionClosedTimeout = 1;
const TInt KWlanLastSocketClosedTimeout = -1;
const TUint32 KEndOfArray = KMaxTUint;

/// Modem bearer names for WLAN connection methods
_LIT( KModemBearerWLAN, "WLANBearer" );

_LIT( KWlanFileIcons, "z:cmpluginwlan.mbm" );
_LIT( KWlanBearerName, "WLANBearer" );
_LIT( KWlanBearerAgent, "wlanagt.agt" );
_LIT( KWlanBearerNif, "wlannif" );

_LIT( KWlanLDDName, "not used" );
_LIT( KWlanPDDName, "not used" );

static const TCmAttribConvTable SWlanConvTbl[] = 
    {
    // iAttribId, iCommsDatId, iValidFuncL, 
    // iTitleId, 
    // iMaxLength, iAttribFlags, iDefValueResId, iEditorResId, iNotUsed1, iNotUsed2
    { EWlanServiceId,           EWlanRangeMax,          NULL },        
    { EWlanServiceId,           KCDTIdWlanServiceId,    NULL },
    { EWlanConnectionMode,      KCDTIdWlanConnMode,     NULL,
      R_QTN_WLAN_SETT_NETWORK_MODE,
       },
    { EWlanSSID,                KCDTIdWlanSSID,         NULL,
      R_QTN_WLAN_SETT_NETWORK_NAME,
      KCmWlanNWNameMaxLength, EConvCompulsory | EConvNoZeroLength, R_QTN_SELEC_SETTING_COMPULSORY, R_TEXT_SETTING_PAGE_NETW_NAME, NULL, NULL },
    { EWlanUsedSSID,            KCDTIdWlanUsedSSID,     NULL },
    { EWlanSecurityMode,        KCDTIdWlanSecMode,      NULL,
      R_QTN_WLAN_SETT_SECURITY_MODE,
      0, 0, 0, 0, NULL, NULL },
    { EWlanAuthenticationMode,  KCDTIdWlanAuthMode,     NULL },
    { EWlanScanSSID,            KCDTIdWlanScanSSID,     NULL,
      R_QTN_WLAN_SETT_HIDDEN_NETWORK,
      0, 0, 0, 0, NULL, NULL },
    { EWlanChannelID,           KCDTIdWlanChannelID,    NULL,
      R_QTN_WLAN_SETT_ADHOC_CHANNEL,
      KCmAdhocChannelMaxLength, EConvNumber, R_QTN_WLAN_SETT_ADHOC_CHANNEL_AUTOMATIC, 0, NULL, NULL },   
    { EWlanAllowSSIDRoaming,  KCDTIdWlanAllowSSIDRoaming,     NULL, 
      R_QTN_WLAN_SETT_SSID_ROAMING,
      0, 0, 0, 0, NULL, NULL },
    { 0,0 }
    };


static const TCmCommonAttrConvArrayItem SCommonConvTbl[] =
{
    { EWlanIfNetworks, ECmIFNetworks },
    { EWlanIpNetMask, ECmIPNetmask },
    { EWlanIpGateway, ECmIPGateway },
    { EWlanIpAddrFromServer, ECmIPAddFromServer },
    { EWlanIpAddr, ECmIPAddress },
    { EWlanIpDNSAddrFromServer, ECmIPDNSAddrFromServer },
    { EWlanIpNameServer1, ECmIPNameServer1 },
    { EWlanIpNameServer2, ECmIPNameServer2 },
    { EWlanIp6DNSAddrFromServer, ECmIP6DNSAddrFromServer },
    { EWlanIp6NameServer1, ECmIP6NameServer1 },
    { EWlanIp6NameServer2, ECmIP6NameServer2 },
    { EWlanIpAddrLeaseValidFrom, ECmIPAddrLeaseValidFrom },
    { EWlanIpAddrLeaseValidTo, ECmIPAddrLeaseValidTo },
    { EWlanConfigDaemonManagerName, ECmConfigDaemonManagerName },
    { EWlanConfigDaemonName, ECmConfigDaemonName },
    { 0, 0 }
    };


/**
 * struct for security mode conversion table
 */
typedef struct
    {
    TUint32 iSecurityMode;
    TUint32 iNWModeSelectionDlgIndex;
    TUint32 iRBSettingPageIndex;
    TUint32 iResId;
    } TCmSecurityModeConvTable;

/**
 * Conversion table for the correct positioning of WLAN security types 
 * in WLAN UI components
 */  
const TCmSecurityModeConvTable KWlanSecurityModeConvTable[] =
    {                              
        { EWlanSecModeOpen,   0, 0, R_QTN_NETW_CONSET_WLAN_SECURITY_MODE_OPEN   },
        { EWlanSecModeWep,    1, 1, R_QTN_NETW_CONSET_WLAN_SECURITY_MODE_WEP    },
        { EWlanSecMode802_1x, 2, 2, R_QTN_NETW_CONSET_WLAN_SECURITY_MODE_802_1X },
        { EWlanSecModeWpa,    3, 3, R_QTN_NETW_CONSET_WLAN_SECURITY_MODE_WPA    },
     // { EWlanSecModeWpa2,   3, 3, R_QTN_NETW_CONSET_WLAN_SECURITY_MODE_WPA2   },
        { EWlanSecModeWAPI,   4, 4, R_QTN_WLAN_SETT_SECURITY_MODE_WAPI   },
        { KEndOfArray, KEndOfArray, KEndOfArray, KEndOfArray }
    };

/**
 * struct for the network mode conversion table
 */
typedef struct
    {
    TUint32 iNwMode;
    TBool   iHidden;
    TUint32 iResId;
    } TCmNetworkModeConvTable;

/**
 * Conversion table for the correct positioning of WLAN network modes
 */
const TCmNetworkModeConvTable KWlanNetworkModeConvTable[] =
    {
        { EInfra, EFalse, R_QTN_WLAN_NETWORK_MODE_INFRA_PUBLIC }, // Infrastructure (public)
        { EInfra, ETrue,  R_QTN_WLAN_NETWORK_MODE_INFRA_HIDDEN }, // Infrastructure (hidden)
        { EAdhoc, EFalse, R_QTN_WLAN_NETWORK_MODE_ADHOC_PUBLIC }, // Ad-hoc (public)
//        { EAdhoc, ETrue,  R_QTN_WLAN_NETWORK_MODE_ADHOC_HIDDEN }, // Ad-hoc (hidden)
        { KEndOfArray, EFalse, KEndOfArray }
    };

// ======================= MEMBER FUNCTIONS =================================

// --------------------------------------------------------------------------
// CCmPluginWlan::NewL
// --------------------------------------------------------------------------
//
CCmPluginWlan* CCmPluginWlan::NewL( TCmPluginInitParam* aInitParam )
    {
    if ( !FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        User::Leave( KErrNotSupported );
        }
        
    CCmPluginWlan* self = new( ELeave ) CCmPluginWlan( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::~CCmPluginWlan
// --------------------------------------------------------------------------
//
CCmPluginWlan::~CCmPluginWlan()
    {
    AdditionalReset();
    DestroyWaitDialog();
    RemoveResourceFile( KPluginWlanResDirAndFileName );    
    delete iConnUiUtils; iConnUiUtils = NULL;
    
    iAdditionalCmsCreated.Close();

    CLOG_CLOSE;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CreateInstanceL
// --------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginWlan::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginWlan* self = new( ELeave ) CCmPluginWlan( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// --------------------------------------------------------------------------
// CCmPluginWlan::CCmPluginWlan
// --------------------------------------------------------------------------
//
CCmPluginWlan::CCmPluginWlan( TCmPluginInitParam* aInitParam )
    : CCmPluginLanBase( aInitParam )
    {
    CLOG_CREATE;
    iWlanTableId = 0;
    iBearerType = KUidWlanBearerType;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::ConstructL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::ConstructL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ConstructL" );
    
    CCmPluginLanBase::ConstructL();

    iBearerRecName = KModemBearerWLAN;
    
    // get WLAN table id
    TRAP_IGNORE( iWlanTableId = CCDWlanServiceRecord::TableIdL( Session() ) );

    if( !iWlanTableId )
        {
        iWlanTableId = CCDWlanServiceRecord::CreateTableL( Session() );

        TMDBElementId tableId = 0;

        TRAP_IGNORE( tableId = CCDWlanDeviceSettingsRecord::TableIdL( Session() ) );
        if( !tableId )
            {
            CCDWlanDeviceSettingsRecord::CreateTableL( Session() );
            }
        
        TRAP_IGNORE( tableId = CCDWLANSecSSIDTable::TableIdL( Session() ) );
        if( !tableId )
            {
            CCDWLANSecSSIDTable::CreateTableL( Session() );
            }

        TRAP_IGNORE( tableId = CCDDestNWTable::TableIdL( Session() ) );
        if( !tableId )
            {
            CCDDestNWTable::CreateTableL( Session() );
            }
        }

    AddResourceFileL( KPluginWlanResDirAndFileName );
    
    AddConverstionTableL( ( CCDRecordBase** )&iWlanServiceRecord, 
                            NULL, 
                            SWlanConvTbl );
    AddCommonConversionTableL( SCommonConvTbl );
    
    iIsWAPISupported = FeatureManager::FeatureSupported( KFeatureIdFfWlanWapi );
    }
    
	
// --------------------------------------------------------------------------
// CCmPluginWlan::GetIntAttributeL
// --------------------------------------------------------------------------
//
TUint32 CCmPluginWlan::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::GetIntAttributeL" );
    
    TInt retval;
    switch ( aAttribute )
        {
        case ECmExtensionLevel:
            {
    	    retval = CCmPluginLanBase::GetIntAttributeL( aAttribute );
    	    retval++;
            break;
            }        
        case ECmBearerIcon:
            {
            TAknsItemID id;
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KWlanFileIcons, 
                                &KDC_BITMAP_DIR,
                                NULL ) );

            // default - WLAN icon
            TInt icon = EMbmCmpluginwlanQgn_prop_wlan_bearer;
            TInt mask = EMbmCmpluginwlanQgn_prop_wlan_bearer_mask;
            
            // Easy WLAN icon
            TUint32 easyWlanId = iCmMgr.EasyWlanIdL();
            if ( easyWlanId && GetIntAttributeL( ECmId ) == easyWlanId )
                {
                icon = EMbmCmpluginwlanQgn_prop_set_conn_wlan_easy;
                mask = EMbmCmpluginwlanQgn_prop_set_conn_wlan_easy_mask;
                }
            
            retval = (TUint32)AknsUtils::CreateGulIconL( 
                                skinInstance, 
                                id,
                                mbmFile.FullName(), 
                                icon, 
                                mask );
            break;
            }  
        case ECmBearerAvailableIcon:
            {
            TAknsItemID id;
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KWlanFileIcons, 
                                &KDC_BITMAP_DIR,
                                NULL ) );

            retval = (TUint32)AknsUtils::CreateGulIconL( 
                                skinInstance, 
                                id,
                                mbmFile.FullName(), 
                                EMbmCmpluginwlanQgn_prop_set_conn_bearer_avail_wlan, 
                                EMbmCmpluginwlanQgn_prop_set_conn_bearer_avail_wlan_mask );
            break;
            }
        case ECmCommsDBBearerType:
            {
            retval = KCommDbBearerWLAN;
            break;
            }
            
        case ECmDefaultUiPriority:
            {
            retval = GlobalUiBearerPriority( TPtrC(KCDTypeNameWLANServiceExt) );
            
            if( retval == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                retval = KDefaultPriorityWLan;
                }
            }
            break;
        case ECmDefaultPriority:
            {
            retval = GlobalBearerPriority( TPtrC(KCDTypeNameWLANServiceExt) );
            
            if( retval == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                retval = KDefaultPriorityWLan;
                }
            }
            break;
        case EWlanServiceId:
            {
            if ( iWlanServiceRecord )
                {
                retval = iWlanServiceRecord->RecordId();
                break;
                }
            }
        default:
            {
    	    retval = CCmPluginBaseEng::GetIntAttributeL( aAttribute );
            break;
            }
        }
    return retval;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::GetBoolAttributeL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::GetBoolAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::GetBoolAttributeL" );
    
    TBool retval( EFalse );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
#ifndef __WINS__
            TInt& networks = CONST_CAST( TInt&, iWlanNetworks );
            networks = 0;

            networks = CheckNetworkCoverageL();
            
            // If there's at least one WLAN network around, then there IS
            // coverage.
            retval = ( networks ? ETrue : EFalse );
#else

            TInt& networks = CONST_CAST( TInt&, iWlanNetworks );
            ////networks = 0;
            ////retval = EFalse;

            networks = 1;
            retval = ETrue;



#endif
            }
            break;

        case ECmAddToAvailableList:
            {
            retval = ETrue;
            }
            break;
            
        case ECmBearerHasUi:
            {
            retval = ETrue;
            }
            break;
            
        default:
            {
            retval = CCmPluginBaseEng::GetBoolAttributeL( aAttribute );
            break;
            }
        }
    return retval;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::GetStringAttributeL
// --------------------------------------------------------------------------
//
HBufC* CCmPluginWlan::GetStringAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::GetStringAttributeL" );

	HBufC* retVal = NULL;
	
	switch( aAttribute )
	    {
	    case ECmBearerAvailableName:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_BEARER_AVAILABLE_WLAN );
	        }
	        break;
	        
        case ECmBearerAvailableText:
            {
            if ( CEikonEnv::Static() )
                {
                if ( iWlanNetworks == KWlanSingleNetwork )
                    {
                    retVal = AllocReadL( 
                                    R_QTN_NETW_CONSET_ONE_WLAN_NW_AVAILABLE );
                    }
                else
                    {
                    retVal = ReadResourceL( 
                                    R_QTN_NETW_CONSET_MANY_WLAN_NW_AVAILABLE,
                                    iWlanNetworks );
                    }
                }
            else
                {   // ReadResourceL does not work without a valid EikonEnv, so
                    // to have a consistent behaviour when there is one or more
                    // wlan networks for ECmBearerAvailableText, we always 
                    // leave when called without valid EikonEnv
                User::Leave( KErrNotSupported );
                }
            }
            break;

	    case ECmBearerSupportedName:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_BEARER_SUPPORTED_WLAN );
	        }
	        break;
	        
	    case ECmBearerNamePopupNote:
	        {
	        retVal = AllocReadL( R_QTN_NETW_CONSET_POPUP_BEARER_WLAN );
	        }
	        break;
	        
	    case ECmBearerSettingName:
	        {
	        retVal = AllocReadL( R_QTN_WLAN_SETT_BEARER_WLAN );
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


// --------------------------------------------------------------------------
// CCmPluginWlan::SetIntAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetIntAttributeL( const TUint32 aAttribute, 
                                            TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetIntAttributeL" );

    CCmPluginBaseEng::SetIntAttributeL( aAttribute, aValue );
    if ( aAttribute == EWlanSecurityMode )
        {
        UpdateSecurityModeL();
        }
    }



// --------------------------------------------------------------------------
// CCmPluginWlan::SetBoolAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetBoolAttributeL( const TUint32 aAttribute, 
                                             TBool aValue )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetBoolAttributeL" );
    
    CCmPluginBaseEng::SetBoolAttributeL( aAttribute, aValue );
    }



// --------------------------------------------------------------------------
// CCmPluginWlan::SetStringAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetStringAttributeL( const TUint32 aAttribute, 
                                               const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetStringAttributeL" );
        
    switch( aAttribute )
        {
        case ECmName:
            {
            CCmPluginLanBase::SetStringAttributeL( aAttribute, aValue );
            break;
            }
        case EWlanIpNameServer1:
        case EWlanIpNameServer2:
            {
            // when setting nameservers, must set EWlanIpDNSAddrFromServer
            // according to current nameserver settings
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            break;
            }
        case EWlanIp6NameServer1:
        case EWlanIp6NameServer2:
            {
            // when setting nameservers, must set EWlanIpDNSAddrFromServer
            // according to current nameserver settings
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            break;
            }
        default:
            {
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            break;
            }
        }
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::UpdateAdditionalRecordsL
// --------------------------------------------------------------------------
//     
void CCmPluginWlan::UpdateAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::UpdateAdditionalRecordsL" );
    
    CheckIfNameModifiedL( iWlanServiceRecord );

    iWlanServiceRecord->iWlanServiceId.SetL( iIapRecord->iService );
    
    if ( !iWlanServiceRecord->RecordId() )
        {
        iWlanServiceRecord->SetRecordId( KCDNewRecordRequest );
        iWlanServiceRecord->StoreL( Session() );
        }
    else
        {
        iWlanServiceRecord->ModifyL( Session() );
        }
    UpdateSecuritySettingsL();        
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::CanHandleIapIdL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::CanHandleIapIdL( TUint32 aIapId ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::CanHandleIapIdL1" );
    CLOG_WRITE_1( "IapId: [%d]", aIapId );
    
    TBool retVal( EFalse );
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                        ( CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord) );
        
    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );
    
    TRAPD( err, iapRecord->LoadL( Session() ) );

    if( !err )
        {
        CanHandleIapIdL( iapRecord );
        }
    
    CleanupStack::PopAndDestroy( iapRecord );
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::CanHandleIapIdL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::CanHandleIapIdL2" );

    CLOG_WRITE_1( "IapId: [%d]", aIapRecord->RecordId() );
   
    
    TBool retVal( EFalse );
    
    CLOG_WRITE_2( "IAP record: [%S][%S]",
                    &FIELD_TO_TDESC( aIapRecord->iServiceType ),
                    &FIELD_TO_TDESC( aIapRecord->iBearerType ) 
                );
    
    if( (TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameLANService) ) &&
        TPtrC(aIapRecord->iBearerType) == TPtrC(KCDTypeNameLANBearer) )
        {
        // check if there is a WLAN record with 
        // an iServiceId == iIapRecord->iService;
        CCDWlanServiceRecord* tmprec = new ( ELeave ) 
                                       CCDWlanServiceRecord ( iWlanTableId );
        CleanupStack::PushL( tmprec );

        TInt service = aIapRecord->iService;
        
        tmprec->iWlanServiceId.SetL( (TUint32)service );
        if ( tmprec->FindL( Session() ) )
            {
            // we found at least one WLAN using this IAP, 
            retVal = ETrue;
            tmprec->LoadL( Session() );
            CLOG_WRITE( "I can." );
            }
        else
            {
            // not found -> not our
            CLOG_WRITE_1( "Wlan with Service ID: [%d] NOT found ", 
                          service );
            }
        CleanupStack::PopAndDestroy(tmprec);        
        }
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::DeleteAdditionalRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::DeleteAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::DeleteAdditionalRecordsL" );    
    
    // as base class deletes service record, in this case LAN, 
    // only WLAN related stuff needs to be deleted
    // also security settings, too!
    DeleteSecuritySettingsL();
    iWlanServiceRecord->DeleteL( Session() );
    }      
    
        
// --------------------------------------------------------------------------
// CCmPluginWlan::RunSettingsL
// --------------------------------------------------------------------------
//        
TInt CCmPluginWlan::RunSettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::RunSettingsL" );
    
    CmPluginWlanSettingsDlg* cmDlg = CmPluginWlanSettingsDlg::NewL( *this );
    return cmDlg->ConstructAndRunLD( );       
    }

	
// --------------------------------------------------------------------------
// CCmPluginWlan::LoadServiceSettingL
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::LoadServiceSettingL" );
        
    if( TPtrC(KCDTypeNameLANService) == iIapRecord->iServiceType  )
        {
        iServiceRecord = static_cast<CCDServiceRecordBase *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));
        iServiceRecord->SetRecordId( iIapRecord->iService );
        CLOG_WRITE_1( "ID: [%d]", iServiceRecord->RecordId() );
        iServiceRecord->LoadL(Session());        
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::LoadAdditionalRecordsL
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::LoadAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::LoadAdditionalRecordsL" );
    
    if (!iWlanServiceRecord) 
        {
        iWlanServiceRecord = 
                        new ( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
        }
    iWlanServiceRecord->SetRecordId( 0 );

    iWlanServiceRecord->iWlanServiceId.SetL( ( TUint32 )
                                                ( iIapRecord->iService ) );
    
    if ( iWlanServiceRecord->FindL( Session() ) )
        {
        iWlanServiceRecord->LoadL( Session() );
        LoadSecuritySettingsL();
        }
    else
        {
        // not found -> error
        CLOG_WRITE_1( "Wlan with Service ID: [%d] NOT found ", 
                      iServiceRecord->RecordId() );
        User::Leave(KErrNotFound);
        }
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::CreateNewServiceRecordL
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::CreateNewServiceRecordL" );
        
    // create base (LAN) service record:
    CCmPluginLanBase::CreateNewServiceRecordL();
    CCDLANServiceRecord* tmp = 
                static_cast<CCDLANServiceRecord *>(iServiceRecord);
    if( FeatureSupported( KFeatureIdIPv6 ) )
        {
        tmp->iIfNetworks.SetL( KDefIspIfNetworksIPv4IPv6 );
        }
    else
        {
        tmp->iIfNetworks.SetL( KDefIspIfNetworksIPv4 );
        }

    // create WLAN service record:
    CreateWlanServiceRecordL();
    
    // In case of WLan 'show progress' is the default value
    SetIntAttributeL( ECmSeamlessnessLevel, ESeamlessnessShowprogress );
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::CreateWlanServiceRecordL
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::CreateWlanServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::CreateWlanServiceRecordL" );

    delete iWlanServiceRecord; 
    iWlanServiceRecord = NULL;
    
    iWlanServiceRecord = new ( ELeave ) CCDWlanServiceRecord ( iWlanTableId );
    iWlanServiceRecord->SetRecordId( KCDNewRecordRequest );

	iWlanServiceRecord->iRecordTag.SetL( 1 );
	iWlanServiceRecord->iRecordName.SetL( iIapRecord->iRecordName );
    iWlanServiceRecord->iWlanConnMode.SetL( EInfra );
    iWlanServiceRecord->iWlanSecMode.SetL( EWlanSecModeOpen );
    iWlanServiceRecord->iWlanScanSSID.SetL( EFalse );
    }


// ----------------------------------------------------------------------------
// CCmPluginWlan::PrepareToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginWlan::PrepareToUpdateRecordsL()
    {
    CCDLANServiceRecord* tmprec = static_cast<CCDLANServiceRecord*>(iServiceRecord);

    CheckDNSServerAddressL( ETrue, 
                            tmprec->iIp6NameServer1,
                            tmprec->iIp6NameServer2,
                            tmprec->iIp6DnsAddrFromServer );
                            
    CheckDNSServerAddressL( EFalse,
                            tmprec->iIpNameServer1,
                            tmprec->iIpNameServer2,
                            tmprec->iIpDnsAddrFromServer );
    SetDaemonNameL();
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::LoadSecuritySettingsL
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::LoadSecuritySettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::LoadSecuritySettingsL" );

    TUint32 secmode = iWlanServiceRecord->iWlanSecMode;
    TUint32 iapid = iIapRecord->iService;
                   
    switch ( secmode )
        {
        case EWlanSecModeOpen:
            {
            break;
            }
        case EWlanSecModeWep:
            {
            if ( !iWepSecSettings )
                {
                iWepSecSettings = CWEPSecuritySettings::NewL();
                }
                iWepSecSettings->LoadL( iapid, Session() );
            break;
            }
        case EWlanSecMode802_1x:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityMode8021x );
                }
            iWpaSecSettings->LoadL( iapid, Session() );
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityModeWpa );
                }
			iWpaSecSettings->LoadL( iapid, Session() );
            break;
            }
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                if ( !iWAPISecSettings )
                    {
                    iWAPISecSettings = CWAPISecuritySettings::NewL();
                    }
                iWAPISecSettings->LoadL( iIapRecord->RecordId(), Session() );
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            // do nothing in urel
            break;
            }
        }
    }
    

// ----------------------------------------------------------------------------
// CCmPluginWlan::EditSecuritySettingsL
// ----------------------------------------------------------------------------
//        
TInt CCmPluginWlan::EditSecuritySettingsL( CEikonEnv& aEikonEnv )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::EditSecuritySettingsL" );

    TInt ret = KErrNotFound;

    TUint32 secmode = iWlanServiceRecord->iWlanSecMode;

    HBufC* title = GetStringAttributeL(ECmName);
    CleanupStack::PushL( title );   
    switch ( secmode )
        {
        case EWlanSecModeOpen:
            {
            break;
            }
        case EWlanSecModeWep:
            {
            CWEPSecuritySettingsUi* ui = 
                            CWEPSecuritySettingsUi::NewL(aEikonEnv);
            CleanupStack::PushL( ui );
            if ( !iWepSecSettings )
                {
                iWepSecSettings = CWEPSecuritySettings::NewL();
                }            
            ret = iWepSecSettings->EditL( *ui, *title );
            CleanupStack::PopAndDestroy( ui );
            break;
            }
        case EWlanSecMode802_1x:
            {
            CWPASecuritySettingsUi* ui = 
                            CWPASecuritySettingsUi::NewL(aEikonEnv);
            CleanupStack::PushL(ui);
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                            CWPASecuritySettings::NewL( ESecurityMode8021x );
                }                        
            ret = iWpaSecSettings->EditL( *ui, *title );
            CleanupStack::PopAndDestroy( ui );
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            CWPASecuritySettingsUi* ui = 
                            CWPASecuritySettingsUi::NewL(aEikonEnv);
            CleanupStack::PushL( ui );
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityModeWpa );
                }                        
            ret = iWpaSecSettings->EditL( *ui, *title );
            CleanupStack::PopAndDestroy( ui );
            break;
            }            
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                CWAPISecuritySettingsUi* ui =
                            CWAPISecuritySettingsUi::NewL( aEikonEnv );
                CleanupStack::PushL( ui );
                if ( !iWAPISecSettings )
                    {
                    iWAPISecSettings = CWAPISecuritySettings::NewL();
                    }
                ret = iWAPISecSettings->EditL( *ui, *title );
                CleanupStack::PopAndDestroy( ui );
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }
        default:
            {
            // do nothing in urel
            break;
            }
        }
    CleanupStack::PopAndDestroy( title );
    return ret;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::AreSecuritySettingsValidL
// --------------------------------------------------------------------------
//        
TBool CCmPluginWlan::AreSecuritySettingsValidL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::AreSecuritySettingsValidL" );
  
    TBool ret = EFalse;

    TUint32 secmode = iWlanServiceRecord->iWlanSecMode;
                
    switch ( secmode )
        {
        case EWlanSecModeOpen:
            {
            break;
            }
        case EWlanSecModeWep:
            {
            if ( !iWepSecSettings )
                {
                iWepSecSettings = CWEPSecuritySettings::NewL();
                }            
            ret = iWepSecSettings->IsValid();
            break;
            }
        case EWlanSecMode802_1x:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityMode8021x );
                }
            ret = iWpaSecSettings->IsValid();
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityModeWpa );
                }
            ret = iWpaSecSettings->IsValid();
            break;
            }
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                if ( !iWAPISecSettings )
                    {
                    iWAPISecSettings = CWAPISecuritySettings::NewL();
                    }
                ret = iWAPISecSettings->IsValid();
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }
        default:
            {
            // do nothing in urel
            break;
            }
        }        
    return ret;
    }

    
// --------------------------------------------------------------------------
// CCmPluginWlan::UpdateSecuritySettingsL()
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::UpdateSecuritySettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::UpdateSecuritySettingsL" );

    TUint32 oldwlaniapid(0);
    TUint32 wlaniapid = iIapRecord->iService;
    
    TUint32 secmode = iWlanServiceRecord->iWlanSecMode;    
    CLOG_WRITE_1( "Sec mode: [%d]", secmode );

    TTypeOfSaving savetype( ESavingBrandNewAP );
    
    if (iWlanServiceRecord->RecordId())
        {
        savetype = ESavingEditedAP;
        oldwlaniapid = wlaniapid;
        }

    if ( iIsCopy )
        {
        oldwlaniapid = iCopiedFromIapId;
        savetype = ESavingNewAPAsACopy;
        
        switch ( secmode )
            {
            case EWlanSecModeOpen:
                {
                break;
                }
            case EWlanSecModeWep:
                {
                // this is a brand new copy of another CM
                // we have to load the settings for the old wlan
                // and save with the new one.
                // first delete possibly existing other settings
                if ( iWepSecSettings )
                    {
                    delete iWepSecSettings;
                    iWepSecSettings = NULL;
                    }
                break;
                }
            case EWlanSecMode802_1x:
                {
                // this is a brand new copy of another CM
                // we have to load the settings for the old wlan
                // and save with the new one.
                // first delete possibly existing other settings
                if ( iWpaSecSettings )
                    {
                    delete iWpaSecSettings;
                    iWpaSecSettings = NULL;
                    }
                break;
                }
            case EWlanSecModeWpa:
            case EWlanSecModeWpa2:
                {
                // this is a brand new copy of another CM
                // we have to load the settings for the old wlan
                // and save with the new one.
                // first delete possibly existing other settings
                if ( iWpaSecSettings )
                    {
                    delete iWpaSecSettings;
                    iWpaSecSettings = NULL;
                    }
                break;
                }
            case EWlanSecModeWAPI:
                {
                if ( iIsWAPISupported )
                    {
                    if ( iWAPISecSettings )
                        {
                        delete iWAPISecSettings;
                        iWAPISecSettings = NULL;
                        }
                    }
                else
                    {
                    User::Leave( KErrCorrupt );
                    }
                break;
                }
            default:
                {
                CLOG_WRITE( "Unknown secmode" );
                // do nothing in urel
                break;
                }
            }
        }
    // now save, & load if still needed
    switch ( secmode )
        {
        case EWlanSecModeOpen:
            {
            // Extra fileds are deleted from WLANServiceTable
            // when security mode is changed from WPA to Open
            if ( iWpaSecSettings )
                {
                iWpaSecSettings->DeleteL( wlaniapid );
                }
            break;
            }
        case EWlanSecModeWep:
            {
            // Extra fileds are deleted from WLANServiceTable
            // when security mode is changed from WPA to WEP
            if ( iWpaSecSettings )
                {
                iWpaSecSettings->DeleteL( wlaniapid );
                }

            // we have to try to save if:
            // 1. it is a changed AP, it is possible that only 
            // sec. settings have been changed. 
            // In this case, iWepSecSettings might be NULL!!
            // 2. it's sec. settings had been edited
            
            if ( !iWepSecSettings )
                {
                iWepSecSettings = CWEPSecuritySettings::NewL();
                iWepSecSettings->LoadL( oldwlaniapid, Session() );
                }
            // save WEP settings, too, into same transaction...
            iWepSecSettings->SaveL( wlaniapid, Session() );
            break;
            }
        case EWlanSecMode802_1x:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                    CWPASecuritySettings::NewL( ESecurityMode8021x );
                iWpaSecSettings->LoadL( oldwlaniapid, Session() );
                }                            
            // save WPA settings, too, into same transaction...
            iWpaSecSettings->SaveL( wlaniapid, Session(), savetype, oldwlaniapid );
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            if ( !iWpaSecSettings )
                {
                iWpaSecSettings = 
                      CWPASecuritySettings::NewL( ESecurityModeWpa );
                iWpaSecSettings->LoadL( oldwlaniapid, Session() );
                }
            
            // save WPA settings, too, into same transaction...
            iWpaSecSettings->SaveL( wlaniapid, Session(), savetype, oldwlaniapid );
            break;
            }
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                if ( !iWAPISecSettings )
                    {
                    iWAPISecSettings = CWAPISecuritySettings::NewL();
                    iWAPISecSettings->LoadL( iIapRecord->RecordId(), Session() );
                    }
                // save WAPI settings
                TRAPD( err, iWAPISecSettings->SaveL( iIapRecord->RecordId(), Session() ) );
                if ( err )
                    {
                    if ( err != KErrArgument )
                        {
                        User::Leave( err );
                        }
                    }
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }
        default:
            {
            CLOG_WRITE( "Unknown secmode" );
            // do nothing in urel
            break;
            }
        }
    // clean up copy flags
    if ( iIsCopy )
        {
        iIsCopy = EFalse;
        iCopiedFromIapId = 0;
        }
    }
    
    
// --------------------------------------------------------------------------
// CCmPluginWlan::AdditionalReset()
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::AdditionalReset()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::AdditionalReset" );
    
    delete iWlanServiceRecord;
    iWlanServiceRecord = NULL;
    
    delete iWepSecSettings;
    iWepSecSettings = NULL;
    
    delete iWpaSecSettings;
    iWpaSecSettings = NULL;

    if ( iIsWAPISupported )
        {
        delete iWAPISecSettings;
        iWAPISecSettings = NULL;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::BearerRecordIdLC()
// --------------------------------------------------------------------------
//        
void CCmPluginWlan::BearerRecordIdLC( HBufC* &aBearerName, 
                                                  TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::BearerRecordIdLC" );

	CCDLANBearerRecord* tmprec = static_cast<CCDLANBearerRecord*>
                ( CCDRecordBase::RecordFactoryL( KCDTIdLANBearerRecord ) );
    CleanupStack::PushL( tmprec );

    tmprec->iRecordName.SetL( TPtrC( iBearerRecName ) );
    if ( !tmprec->FindL( Session() ) )
        { // bearer not found -> create dummy values
        CLOG_WRITE( "No bearer found" );
        
        tmprec->SetRecordId( KCDNewRecordRequest );
        
        tmprec->iBearerAgent.SetL( KWlanBearerAgent );
        tmprec->iRecordName.SetL( KWlanBearerName );
        tmprec->iLanBearerNifName.SetL( KWlanBearerNif );
        tmprec->iLanBearerLddName.SetL( KWlanLDDName );
        tmprec->iLanBearerPddName.SetL( KWlanPDDName );
        tmprec->iLastSocketActivityTimeout.SetL(
                                ( TUint32 )KWlanLastSocketActivityTimeout );
        tmprec->iLastSessionClosedTimeout.SetL(
                                (TUint32 )KWlanLastSessionClosedTimeout );
        tmprec->iLastSocketClosedTimeout.SetL(
                                ( TUint32 )KWlanLastSocketClosedTimeout );
        tmprec->StoreL( Session() );
        }

    aRecordId = tmprec->RecordId();
    
    CLOG_WRITE_2( "Name: [%S], id: [%d]", 
                  &tmprec->iRecordName.GetL(),
                  aRecordId );

    CleanupStack::PopAndDestroy( tmprec );

    aBearerName = TPtrC(KCDTypeNameLANBearer).AllocLC();
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::ReadResourceL()
// --------------------------------------------------------------------------
//        
HBufC* CCmPluginWlan::ReadResourceL( TInt aResId, TInt aInt ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ReadResourceL" );
    
    HBufC* retval = NULL;

    RConeResourceLoader resourceLoader( *CEikonEnv::Static() );
    LoadResourceLC( resourceLoader );
    retval = StringLoader::LoadL( aResId, aInt );

    // Clean-up
    CleanupStack::PopAndDestroy();

    return retval;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::LoadResourceL()
// --------------------------------------------------------------------------
//  
void CCmPluginWlan::LoadResourceLC( 
                                RConeResourceLoader& aResourceLoader ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::LoadResourceLC" );
    
    // Add resource file.
    TParse fp;
    User::LeaveIfError( fp.Set( KPluginWlanResDirAndFileName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ) );
    TFileName resourceFileNameBuf = fp.FullName();

    CleanupClosePushL( aResourceLoader );
    aResourceLoader.OpenL( resourceFileNameBuf );
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::InitializeWithUiL
// --------------------------------------------------------------------------
//        
TBool CCmPluginWlan::InitializeWithUiL( TBool aManuallyConfigure )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::InitializeWithUiL" );
    
    TBool retVal ( EFalse );
    RConeResourceLoader resourceLoader( *CEikonEnv::Static() );
    LoadResourceLC( resourceLoader );
    if ( aManuallyConfigure )
        {
        retVal = ManuallyConfigureL();
        }
    else
        {
        retVal = AutomaticallyConfigureL();
        }
    // Clean-up
    CleanupStack::PopAndDestroy();
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::ProtectedSetupL
// --------------------------------------------------------------------------
//
WiFiProt::TWiFiReturn CCmPluginWlan::ProtectedSetupL( TWlanSsid& aSsid )
    {
    RCmManagerExt packedCmManager;
    iCmMgr.WrapCmManager( packedCmManager );
    CWifiProtUiInProcess* wifi = CWifiProtUiInProcess::NewL(
                             &packedCmManager );
    CleanupStack::PushL( wifi );
    WiFiProt::TWiFiReturn wifiret = wifi->StartFromUiL( aSsid, EFalse, iAdditionalCmsCreated);                         
    CleanupStack::PopAndDestroy( wifi );                                         
    switch ( wifiret )
        {
        case WiFiProt::EWiFiCancel:
            {
            break;
            }
        case WiFiProt::EWiFiOK:
            {
            if (iAdditionalCmsCreated.Count())
                {
                // Loading the first item in the array
                // to the actual plugin
                Reset();
                AdditionalReset();
                LoadL( iAdditionalCmsCreated[0] );
                iAdditionalCmsCreated.Remove(0);
                }
             else 
                {//no settings configured, do the same as cancel case
                return WiFiProt::EWiFiCancel;
                }
                
            break;
            }
        case WiFiProt::EWifiNoAuto:
            {
            break;
            }  
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }                                              
        }    
    return wifiret;
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::AutomaticallyConfigureL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::AutomaticallyConfigureL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::AutomaticallyConfigureL" );
    
    TBool retval ( ETrue );
    
    if ( !iConnUiUtils )
        {
        iConnUiUtils = CConnectionUiUtilities::NewL();
        }

    // Let's search for a WLAN network
    TWlanSsid ssid;
    TWlanConnectionMode connectionMode;
    TWlanConnectionExtentedSecurityMode securityMode;

    TUint32 secmode( EWlanSecModeOpen );
    
    // Fortunately user will not see WLAN networks with empty ssid in the list
    // shown by next call.
    CCmPluginWlanActiveWaiter* waiter = CCmPluginWlanActiveWaiter::NewL();
    CleanupStack::PushL( waiter );

    TBool isProtectedSetupSupported;
    iConnUiUtils->SearchWLANNetworkAsync( waiter->iStatus, ssid, connectionMode, securityMode, isProtectedSetupSupported );

    TInt searchWlanReturn = waiter->WaitForRequest();
    CleanupStack::PopAndDestroy( waiter );

    if ( searchWlanReturn == KErrNone)
        {
        if ( isProtectedSetupSupported )
            {
            TBool wifiret = ProtectedSetupL( ssid );
            switch ( wifiret )
                {
                case WiFiProt::EWiFiCancel: //cancel pressed, cancel process
                    {
                    return EFalse;
                    //break;
                    }
                case WiFiProt::EWiFiOK: //settings configured already, we can return
                    {
                    return ETrue;
                    //break;
                    }
                case WiFiProt::EWifiNoAuto://proceed with normal setup
                    {
                    break;
                    }  
                default:
                    {
                    User::Leave( KErrNotSupported );
                    break;
                    }                                              
                }    
            }
        switch( securityMode )
            {
            case EWlanConnectionExtentedSecurityModeOpen:
                {
                secmode = EWlanSecModeOpen;
                }
                break;
                
            case EWlanConnectionExtentedSecurityModeWepOpen:
            case EWlanConnectionExtentedSecurityModeWepShared:
                {
                secmode = EWlanSecModeWep;
                }
                break;
                
            case EWlanConnectionExtentedSecurityMode802d1x:
                {
                secmode = EWlanSecMode802_1x;
                }
                break;
                
            case EWlanConnectionExtentedSecurityModeWpa:
            case EWlanConnectionExtentedSecurityModeWpaPsk:
                {
                secmode = EWlanSecModeWpa;
                }
                break;
                
            case EWlanConnectionExtentedSecurityModeWpa2:
            case EWlanConnectionExtentedSecurityModeWpa2Psk:
                {
                secmode = EWlanSecModeWpa2;
                }
                break;
            case EWlanConnectionExtentedSecurityModeWapi:
            case EWlanConnectionExtentedSecurityModeWapiPsk:
                {
                if ( iIsWAPISupported )
                    {
                    secmode = EWlanSecModeWAPI;
                    }
                else
                    {
                    User::Leave( KErrCorrupt );
                    }
                }
                break;
            default:
                {
                User::Leave( KErrNotSupported );
                }
            }

        // Let's check the security mode and if it's other than "none", then
        // ask for the key.
        HBufC* key = HBufC::NewLC( KWlanMaxKeyLength );    // security key
        TPtr keyPtr( key->Des() );
        TBool wepKeyInAsciiFormat;
        switch( securityMode )
            {
            case EWlanConnectionExtentedSecurityModeWepOpen:
            case EWlanConnectionExtentedSecurityModeWepShared:
                {
    	          retval = iConnUiUtils->EasyWepDlg( &keyPtr, wepKeyInAsciiFormat );
                break;
                }
            case EWlanConnectionExtentedSecurityModeWpaPsk:
            case EWlanConnectionExtentedSecurityModeWpa2Psk:
                {
            	  retval = iConnUiUtils->EasyWpaDlg( &keyPtr );
                break;
                }
            case EWlanConnectionExtentedSecurityModeWapiPsk:
                {
                retval = iConnUiUtils->EasyWapiDlg( &keyPtr );
                break;
                }
            case EWlanConnectionExtentedSecurityModeOpen:
            case EWlanConnectionExtentedSecurityMode802d1x:
            case EWlanConnectionExtentedSecurityModeWpa:
            case EWlanConnectionExtentedSecurityModeWpa2:
            case EWlanConnectionExtentedSecurityModeWapi:
            default:
                {
                break;
                }
            }

        // If retval is not ETrue here, then most probably the user has
        // cancelled the password dialog ==> she has cancelled the whole
        // process.
        if ( retval )
            {
            // We have to convert the 8-bit SSID to 16-bit for CommsDat.
            HBufC* ssid16 = HBufC::NewLC( ssid.Length() );
            TPtr ssid16Ptr( ssid16->Des() );
            ssid16Ptr.Copy( ssid );

            SetStringAttributeL( ECmName, *ssid16 );
            SetStringAttributeL( EWlanSSID, *ssid16 );
            // ConvertWLANConnectionStatesL( )converts WLAN connection state 
            // (TWlanConnectionMode) into EInfra or EAdhoc (TWlanNetMode)
            SetIntAttributeL( EWlanConnectionMode, 
                    (TUint32) ConvertWLANConnectionStatesL( connectionMode ) );
            SetIntAttributeL( EWlanSecurityMode, secmode );

            // Check if the network is hidden
            TBool hidden( EFalse );

            // These values we already have so we are not interested of
            // them
            TWlanNetMode tempNetworkMode;
            TWlanConnectionExtentedSecurityMode tempSecurityMode;

            TBool tempProtectedSetupSupported;

            TInt retVal( KErrNone );

            CCmWlanCoverageCheck* coverage = new( ELeave )CCmWlanCoverageCheck;
            CleanupStack::PushL( coverage );

	          // We need to catch leave value of Wlan scanning
	          TRAPD( err, retVal = coverage->ScanForPromptedSsidL( ssid, ETrue, 
                   tempNetworkMode, 
                   tempSecurityMode,
                   tempProtectedSetupSupported ) );
            if ( !retVal || err != KErrNone)
                {
                TRAPD( err, retVal = coverage->ScanForPromptedSsidL( ssid,
								     EFalse,
								     tempNetworkMode, 
								     tempSecurityMode,
								     tempProtectedSetupSupported ) );
                if ( err == KErrNone && retVal )
                    {
                    hidden = ETrue;
                    }
                else // direct scan is ok, but network could not found with given ssid
		            {
                CleanupStack::PopAndDestroy( coverage ); // coverage
                CleanupStack::PopAndDestroy( ssid16 );
                CleanupStack::PopAndDestroy( key );
			
                TCmCommonUi::ShowNoteL( R_QTN_NETW_QUEST_IAP_INCOMPLETE_DELETE, TCmCommonUi::ECmOkNote );
                return EFalse;
		            }
            }

            CleanupStack::PopAndDestroy( coverage ); // coverage
            
            SetBoolAttributeL( EWlanScanSSID, hidden );
            CLOG_WRITE_1( "Is hidden[%d]", (TInt) hidden );

            // We have to store the security key as well.
            if ( securityMode == EWlanConnectionExtentedSecurityModeWepOpen )
                {
                if ( !iWepSecSettings )
                    {
                    iWepSecSettings = CWEPSecuritySettings::NewL();
                    }
                iWepSecSettings->SetKeyDataL( 0, *key,
                                             wepKeyInAsciiFormat );
                // Note that the key will NOT be persisted at this very
                // point, but only after SaveL, which will be called
                // later on.
                }
            else if ( securityMode == EWlanConnectionExtentedSecurityModeWpaPsk
                      || securityMode == EWlanConnectionExtentedSecurityModeWpa2Psk )
                {
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = CWPASecuritySettings::NewL( ESecurityModeWpa );
                    }
                iWpaSecSettings->SetWPAPreSharedKey( *key );

                // Note that the key will NOT be persisted at this very
                // point, but only after SaveL, which will be called
                // later on.
                }
            else if ( securityMode == EWlanConnectionExtentedSecurityModeWapiPsk )
                {
                if ( !iWAPISecSettings )
                    {
                    iWAPISecSettings = CWAPISecuritySettings::NewL();
                    }
                
                iWAPISecSettings->SetPreSharedKeyL( CWAPISecuritySettings::EWapiKeyAscii, *key );
                // Note that the key will NOT be persisted at this very
                // point, but only after SaveL, which will be called
                // later on.
                }

            CLOG_WRITE( "InitializeWithUiL stored everything successfully" );

            CleanupStack::PopAndDestroy( ssid16 );
            }
        CleanupStack::PopAndDestroy( key );
        }
    else
        {
        if ( searchWlanReturn == KErrNotFound )
            {
            // no Wlan network found, continuing process...
            HBufC* ssid16 = HBufC::NewLC( ssid.Length() );
            TPtr ssid16Ptr( ssid16->Des() );
            ssid16Ptr.Copy( ssid );
            // write ssid and go on...
            SetStringAttributeL( ECmName, *ssid16 );
            SetStringAttributeL( EWlanSSID, *ssid16 );
            CleanupStack::PopAndDestroy( ssid16 );
            retval = ProceedWithManualL( ETrue ); // revert to fully manual...
            }
        else
            {
            retval = EFalse;
            }
        }
    return retval;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::ManuallyConfigureL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::ManuallyConfigureL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ManuallyConfigureL" );
    
    TBool proceed ( EFalse );
    TBool askNWModeAndSec ( EFalse );
    
    // Step 1 - WLAN Network Name query (SSID)
    // sets: ECmName, EWlanSSID
    RBuf nwName;
    nwName.CreateL( KCmWlanNWNameMaxLength );
    CleanupClosePushL( nwName );

    proceed = ShowNWNameQueryL( nwName );
    
    TWlanConnectionExtentedSecurityMode securityMode =
                                        EWlanConnectionExtentedSecurityModeOpen;
            
    TWpaMode wpaMode = EWpaModeUndefined;

    if ( proceed )
        {
        TWlanNetMode nwMode( EInfra );

        // Step 2 - SSID scan ( first broadcast, then direct )
        // sets: EWlanConnectionMode
        //       EWlanScanSSID
        //       EWlanSecurityMode

        TWlanSsid ssid;
        ssid.Copy( nwName );       
        
        TBool protectedSetupSupported = EFalse;
        if ( ScanForWlanNwL( ssid, nwMode, securityMode, protectedSetupSupported ) )
            {
            if ( protectedSetupSupported )
                {
                TBool wifiret = ProtectedSetupL( ssid );
                switch ( wifiret )
                    {
                    case WiFiProt::EWiFiCancel: //cancel pressed, cancel process
                        {
                        CleanupStack::PopAndDestroy( &nwName ); //cleanup and return 
                        return EFalse;
                        //break;
                        }
                    case WiFiProt::EWiFiOK:
                        {
                        CleanupStack::PopAndDestroy( &nwName );//cleanup and return 
                        //UpdateSecuritySettingsL();
                        return ETrue;
                        //break;
                        }
                    case WiFiProt::EWifiNoAuto://proceed with normal setup
                        {
                        break;
                        }  
                    default:
                        {
                        User::Leave( KErrNotSupported );
                        break;
                        }                                              
                    }    
                }

            if ( securityMode == EWlanConnectionExtentedSecurityModeWpaPsk
                 || securityMode == EWlanConnectionExtentedSecurityModeWpa2Psk )
                {
                wpaMode = EWpaModePreSharedKey;
                }
            else if ( securityMode == EWlanConnectionExtentedSecurityModeWpa
                      || securityMode == EWlanConnectionExtentedSecurityMode802d1x
                      || securityMode == EWlanConnectionExtentedSecurityModeWpa2 )
                {
                wpaMode = EWpaModeEap;
                }
            }
        else
            {
            askNWModeAndSec = ETrue;
            }
        proceed = ProceedWithManualL ( askNWModeAndSec, nwMode, securityMode, wpaMode  );
        }
    CleanupStack::PopAndDestroy( &nwName );

    return proceed;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::ProceedWithManualL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::ProceedWithManualL( TBool aAskNWModeAndSec,
                                         TWlanNetMode aNwMode, 
                                         TWlanConnectionExtentedSecurityMode aSecurityMode,
                                         TWpaMode aWpaMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ProceedWithManualL" );

    TInt proceed = ETrue;
    if ( aAskNWModeAndSec )
        {
        // Step 3 - Network Mode
        // sets: EWlanConnectionMode
        //       EWlanScanSSID
        proceed = SelectNWModeL( aNwMode );

        // Step 4 - Network security
        // sets: EWlanSecurityMode
        // Open       - finished
        // WEP        - go to step 5
        // WPA/802.1x - go to step 6
        // WAPI       - go to step 9
        if ( proceed )
            {
            proceed = ShowNWSecurityDlgL( aNwMode, aSecurityMode );
            }
        }
        
    // Security settings - Steps 5-8
    if ( proceed && aSecurityMode != EWlanConnectionExtentedSecurityModeOpen )
        {
        switch ( aSecurityMode )
            {
            // Step 5 - Enter WEP key
            // sets: wep key
            // finished
            case EWlanConnectionExtentedSecurityModeWepOpen:
            case EWlanConnectionExtentedSecurityModeWepShared:
                {
                proceed = EnterWEPKeyL();
                break;
                }
            // Step 6 - WPA/WPA2 mode
            // sets: wpa/802.1x keys
            // Pre-shared - go to step 7
            // EAP        - go to step 8
            case EWlanConnectionExtentedSecurityMode802d1x:
            case EWlanConnectionExtentedSecurityModeWpa:
            case EWlanConnectionExtentedSecurityModeWpaPsk:
            case EWlanConnectionExtentedSecurityModeWpa2:
            case EWlanConnectionExtentedSecurityModeWpa2Psk:
                {
                if ( aWpaMode == EWpaModeUndefined )
                    {
                    proceed = SelectWpaModeDialogL( aWpaMode );
                    }

                if ( proceed )
                    {
                    // Step 7 - Pre-shared key
                    // sets: pre-shared key
                    // finished
                    if ( aWpaMode == EWpaModePreSharedKey )
                        {
                        proceed = EnterWpaPreSharedKeyL();
                        }
                    // Step 8 - EAP selection note
                    else
                        {
                        TCmCommonUi::ShowNoteL( *( StringLoader::LoadLC( 
                            R_QTN_NETW_CONSET_INFO_EAP_SETTINGS_DEFAULT ) ),
                            TCmCommonUi::ECmInfoNote );
                        CleanupStack::PopAndDestroy();
                        }
                    }
                break;
                }
            // Step 9
            case EWlanConnectionExtentedSecurityModeWapi:
            case EWlanConnectionExtentedSecurityModeWapiPsk:
                {
                if ( iIsWAPISupported )
                    {
                    if ( aSecurityMode == EWlanConnectionExtentedSecurityModeWapiPsk )
                        {
                        proceed = EnterWAPIPreSharedKeyL();
                        }
                    }
                else
                    {
                    User::Leave( KErrCorrupt );
                    }
                break;
                }
            default:
                {
                User::Leave( KErrNotSupported );
                break;
                }
            }
        }
        
    // Store all the unsaved settings
  /* if ( proceed )
        {
        UpdateSecuritySettingsL();
        }*/
    return proceed;
    }
    
// --------------------------------------------------------------------------
// CCmPluginWlan::ShowNWNameQueryL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::ShowNWNameQueryL( TDes& aNwName )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ShowNWNameQueryL" );
    
    TBool retVal ( EFalse );
    
    retVal = TCmCommonUi::ShowConfirmationQueryWithInputL( 
                                            R_QTN_WLAN_PRMPT_NETWORK_NAME,
                                            aNwName );
    if ( retVal )
        {
        SetStringAttributeL( ECmName, aNwName );
        SetStringAttributeL( EWlanSSID, aNwName );
        
        retVal = ETrue;
        }
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::ScanForWlanNwL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::ScanForWlanNwL( 
#ifdef __WINS__
                         TWlanSsid& /*aSsid*/, TWlanNetMode& /*aNetworkMode*/,
                         TWlanConnectionExtentedSecurityMode& /*aSecurityMode*/,
                         TBool& /*aProtectedSetupSupported*/ )
#else
                         TWlanSsid& aSsid, TWlanNetMode& aNetworkMode,
                         TWlanConnectionExtentedSecurityMode& aSecurityMode,
                         TBool& aProtectedSetupSupported )
#endif  // __WINS__
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ScanForWlanNwL" );
    
    TBool retVal ( EFalse );

    DestroyWaitDialog();

    iWaitDialog = new( ELeave )CAknWaitDialog
                    ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ), ETrue );
    iWaitDialog->ExecuteLD( R_SEARCHING_WLAN_WAIT_NOTE ); 

#ifndef __WINS__
    TBool hidden ( EFalse );
    
    CCmWlanCoverageCheck* coverage = new( ELeave )CCmWlanCoverageCheck;
    CleanupStack::PushL( coverage );

    CLOG_WRITE( "Coverage created, calling ScanForPromptedSsidL, broadcast" );
    retVal = coverage->ScanForPromptedSsidL( aSsid, ETrue, aNetworkMode, 
                                             aSecurityMode,
                                             aProtectedSetupSupported );
    CLOG_WRITE( "ScanForPromptedSsidL returned" );
    
    if ( !retVal )
        {
        CLOG_WRITE( "Calling ScanForPromptedSsidL, direct" );
        retVal = coverage->ScanForPromptedSsidL( aSsid, EFalse, aNetworkMode, 
                                                 aSecurityMode,
                                                 aProtectedSetupSupported );
        CLOG_WRITE( "ScanForPromptedSsidL returned" );

        if ( retVal )
            {
            hidden = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( coverage );

    if ( retVal )
        {
        CLOG_WRITE( "Ssid has been found" );

        // Network mode
        SetIntAttributeL( EWlanConnectionMode, aNetworkMode );
        // Security mode
        SetIntAttributeL( EWlanSecurityMode, 
                          ConvertConnectionSecurityModeToSecModeL( 
                          aSecurityMode ) );
        // Hidden
        SetBoolAttributeL( EWlanScanSSID, hidden );    
        }
    else    
#else   // __WINS
  if ( !retVal )  
#endif  // __WINS
        {
        HBufC* stringLabel = StringLoader::LoadLC( 
                                        R_INFO_NO_NETWORKS_FOUND );

        CAknGlobalNote * note = CAknGlobalNote::NewL();
        CleanupStack::PushL(note);
        note->ShowNoteL(EAknGlobalInformationNote, *stringLabel);
        CleanupStack::PopAndDestroy(note);
        CleanupStack::PopAndDestroy( stringLabel );        
        }


    DestroyWaitDialog();

    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::SelectNWModeL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::SelectNWModeL( TWlanNetMode& aNwMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SelectNWModeL" );
    
    TBool retVal ( EFalse );
    
    // Title
    HBufC* title = StringLoader::LoadLC( 
                        R_QTN_NETW_CONSET_PRMPT_WLAN_NW_MODE_AND_STATUS );
    TPtr ptrTitle ( title->Des() );
    
    // Items
    CDesCArrayFlat* items = 
	            new ( ELeave ) CDesCArrayFlat( KCmArrayMediumGranularity );
    CleanupStack::PushL( items );
    
    TInt i = 0;
    while ( KWlanNetworkModeConvTable[i].iResId != KEndOfArray )
        {
        items->AppendL( 
            *StringLoader::LoadLC( KWlanNetworkModeConvTable[i].iResId ) );
        CleanupStack::PopAndDestroy(); 
        i++;
        }
        
    // Show the dialog
    TInt selection(0);
    if ( TCmCommonUi::ShowPopupListL( ptrTitle , 
                                      items,
                                      NULL,
                                      R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                      selection ) )
        {
        // result:        
        i = 0;
        
        aNwMode = 
            ( TWlanNetMode ) KWlanNetworkModeConvTable[selection].iNwMode;
        
        // Network mode
        SetIntAttributeL( EWlanConnectionMode, aNwMode );
        
        // Network status
        SetBoolAttributeL( EWlanScanSSID, 
                           KWlanNetworkModeConvTable[selection].iHidden );
                           
        retVal = ETrue;
        }
                                      
    CleanupStack::Pop( items );
    CleanupStack::PopAndDestroy( title );
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::ShowNWSecurityDlgL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::ShowNWSecurityDlgL( TWlanNetMode aNwMode, 
                                TWlanConnectionExtentedSecurityMode& aSecurityMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ShowNWSecurityDlgL" );

    TBool retVal ( EFalse );    

    // Collect NW Security Mode items
	CDesCArrayFlat* items = 
	            new ( ELeave ) CDesCArrayFlat( KCmArrayMediumGranularity );
    
    TInt i = 0;
    while ( KWlanSecurityModeConvTable[i].iResId != KEndOfArray )
        {
        TWlanSecMode secMode = 
                ( TWlanSecMode ) KWlanSecurityModeConvTable[i].iSecurityMode;
        
        if ( ( secMode == EWlanSecModeOpen || secMode == EWlanSecModeWep )
                || aNwMode == EInfra )
             {
            items->AppendL( *StringLoader::LoadLC( 
                                KWlanSecurityModeConvTable[i].iResId ) );
            CleanupStack::PopAndDestroy(); 
            }
        i++;
        // don't show WAPI if it's not supported
        if ( !iIsWAPISupported )
            {
            if ( KWlanSecurityModeConvTable[i].iSecurityMode == EWlanSecModeWAPI )
                {
                break;
                }
            }
        }
   
    // Title
    HBufC* title = StringLoader::LoadLC( 
                            R_QTN_NETW_CONSET_PRMPT_WLAN_NW_SECURITY_MODE );
    TPtr ptrTitle ( title->Des() );
    
    TInt selection = 0;
    
    // Show the dialog
    if ( TCmCommonUi::ShowPopupListL( ptrTitle , 
                                      items,
                                      NULL,
                                      R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                      selection ) )
        {
        // result:
        aSecurityMode = 
                ConvertSecModeToConnectionSecurityModeL( ( TWlanSecMode )
                        KWlanSecurityModeConvTable[selection].iSecurityMode );
        SetIntAttributeL( EWlanSecurityMode, 
                        KWlanSecurityModeConvTable[selection].iSecurityMode );
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( title );
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::EnterWEPKeyL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::EnterWEPKeyL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::EnterWEPKeyL" );
    
    TBool retVal ( EFalse );
    
    // Security key
    HBufC* key = HBufC::NewLC( KWlanMaxKeyLength );
    TPtr keyPtr( key->Des() );
    TBool wepKeyInAsciiFormat ( EFalse );
    
    if ( !iConnUiUtils )
        {
        iConnUiUtils = CConnectionUiUtilities::NewL();
        }
    
    if ( iConnUiUtils->EasyWepDlg( &keyPtr, wepKeyInAsciiFormat ) )
        {
        if ( !iWepSecSettings )
            {
            iWepSecSettings = CWEPSecuritySettings::NewL();
            }       
        iWepSecSettings->SetKeyDataL( 0, 
                                      *key,
                                      wepKeyInAsciiFormat );
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( key );    
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::EnterWpaPreSharedKeyL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::EnterWpaPreSharedKeyL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::EnterWpaPreSharedKeyL" );
    
    TBool retVal ( EFalse );
    
    HBufC* key = HBufC::NewLC( KWlanMaxKeyLength );
    TPtr keyPtr( key->Des() );

    if ( !iConnUiUtils )
        {
        iConnUiUtils = CConnectionUiUtilities::NewL();
        }
    
    if ( iConnUiUtils->EasyWpaDlg( &keyPtr ) )
        {
        if ( !iWpaSecSettings )
            {
            iWpaSecSettings = CWPASecuritySettings::NewL( ESecurityModeWpa );
            }
        iWpaSecSettings->SetWPAPreSharedKey( *key );
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( key );  
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::SelectWpaModeDialogL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::SelectWpaModeDialogL( TWpaMode& aWpaMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SelectWpaModeDialogL" );
    
    TBool retVal ( EFalse );
    
    // Collect NW Security Mode items
    CDesCArrayFlat* items = 
	            new ( ELeave ) CDesCArrayFlat( KCmArrayMediumGranularity );
   
    // Title
    HBufC* title = 
            StringLoader::LoadLC( R_QTN_NETW_CONSET_PRMPT_WLAN_WPA_MODE );
    TPtr ptrTitle ( title->Des() );
    
    TInt selection = 0;
    
    // EWpaModeEap
    items->AppendL( *StringLoader::LoadLC(
                                    R_QTN_NETW_CONSET_PRMPT_WPA_MODE_EAP ) );
    CleanupStack::PopAndDestroy(); 
    
    // EWpaModePreSharedKey
    items->AppendL( *StringLoader::LoadLC( 
                                    R_QTN_NETW_CONSET_PRMPT_WPA_MODE_PSK ) );
    CleanupStack::PopAndDestroy(); 
        
    // Show the dialog    
    if( TCmCommonUi::ShowPopupListL( ptrTitle, 
                                     items,
                                     NULL,
                                     R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                     selection ) )
        {
        aWpaMode = ( TWpaMode ) selection;
        retVal = ETrue;
        }
    
    CleanupStack::PopAndDestroy( title );
    
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginWlan::UpdateSecurityModeL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::UpdateSecurityModeL( )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::UpdateSecurityModeL" );
    
    TUint32 secmode = GetIntAttributeL( EWlanSecurityMode );

    TUint32 wlaniapid = iIapRecord->iService;

    switch ( secmode )
        {
        case EWlanSecModeOpen:
            {
            break;
            }
        case EWlanSecModeWep:
            {
            if ( iWepSecSettings )
                {
                delete iWepSecSettings;
                iWepSecSettings = NULL;
                }
            iWepSecSettings = CWEPSecuritySettings::NewL();
            iWepSecSettings->LoadL( wlaniapid, Session() );
            break;
            }
        case EWlanSecMode802_1x:
            {
            if ( iWpaSecSettings )
                {
                delete iWpaSecSettings;
                iWpaSecSettings = NULL;
                }
            iWpaSecSettings = 
                CWPASecuritySettings::NewL( ESecurityMode8021x );
            iWpaSecSettings->LoadL( wlaniapid, Session() );
            break;
            }
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            if ( iWpaSecSettings )
                {
                delete iWpaSecSettings;
                iWpaSecSettings = NULL;
                }
            iWpaSecSettings = 
                  CWPASecuritySettings::NewL( ESecurityModeWpa );
            iWpaSecSettings->LoadL( wlaniapid, Session() );
            break;
            }
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                if ( iWAPISecSettings )
                    {
                    delete iWAPISecSettings;
                    iWAPISecSettings = NULL;
                    }
                iWAPISecSettings = CWAPISecuritySettings::NewL();
                if ( iIapRecord->RecordId() )
                    {
                    iWAPISecSettings->LoadL( iIapRecord->RecordId(), Session() );
                    }
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CCmPluginWlan::CopyAdditionalDataL
// --------------------------------------------------------------------------
//
void CCmPluginWlan::CopyAdditionalDataL( CCmPluginBaseEng& aDestInst ) const
    {
    LOGGER_ENTERFN( "CCmPluginWlan::CopyAdditionalDataL" );
    
    STATIC_CAST( CCmPluginWlan*, &aDestInst )->
                    SetAsCopyOf( iIapRecord->iService );
    }
    

// --------------------------------------------------------------------------
// CCmPluginWlan::SetAsCopyOf
// --------------------------------------------------------------------------
//
void CCmPluginWlan::SetAsCopyOf( TUint32 aBaseIapID )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetAsCopyOf" );
    
    iIsCopy = ETrue;
    
    iCopiedFromIapId = aBaseIapID;    
    }

// ---------------------------------------------------------
// CCmPluginWlan::DeleteSecuritySettingsL
// ---------------------------------------------------------
//    
void CCmPluginWlan::DeleteSecuritySettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::DeleteSecuritySettingsL" );

    TUint32 wlaniapid = iIapRecord->iService;
    
    // check security mode and existence of sec.settings
    if (iWlanServiceRecord->RecordId() )
        {// ha sat least WLAN service record, may start to wonder...
        // wlan service record will be deleted separatelly!
        // get current security mode
        TUint32 secmode = iWlanServiceRecord->iWlanSecMode;
     
        switch ( secmode )
            {     
            case EWlanSecModeOpen:
            case EWlanSecModeWep:
                {
                // nothing to do here
                break;
                }
            case EWlanSecMode802_1x:
                {                
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityMode8021x );
                    }
                iWpaSecSettings->DeleteL( wlaniapid );
                break;
                }
            case EWlanSecModeWpa:
            case EWlanSecModeWpa2:
                {
                if ( !iWpaSecSettings )
                    {
                    iWpaSecSettings = 
                        CWPASecuritySettings::NewL( ESecurityModeWpa );
                    }
                iWpaSecSettings->DeleteL( wlaniapid );
                break;
                }
            case EWlanSecModeWAPI:
                if ( iIsWAPISupported )
                    {
                    if ( !iWAPISecSettings )
                        {
                        iWAPISecSettings = CWAPISecuritySettings::NewL();
                        }
                    TUint32 serviceId = iWlanServiceRecord->RecordId();
                    iWAPISecSettings->DeleteAPSpecificDataL( serviceId );
                    }
                else
                    {
                    User::Leave( KErrCorrupt );
                    }
                break;
            default:
                {
                User::Leave( KErrNotSupported );
                // do nothing in urel
                break;
                }
            }
        }
    // if there is no wlan service record, 
    // then there can be no security settings, skipping...
    }


// ---------------------------------------------------------
// CCmPluginWlan::SetDnsIpFromServer
// ---------------------------------------------------------
//    
void CCmPluginWlan::SetDnsIpFromServerL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetDnsIpFromServer" );
    
    HBufC* ip1 = GetStringAttributeL( EWlanIpNameServer1 );
    CleanupStack::PushL( ip1 );

    HBufC* ip2 = GetStringAttributeL( EWlanIpNameServer2 );
    CleanupStack::PushL( ip2 );
    

    if ( ( ip1->Compare( KDynIpAddress ) == 0 ) &&
         ( ip2->Compare( KDynIpAddress ) == 0 ) )
        {
        SetBoolAttributeL( EWlanIpDNSAddrFromServer, ETrue );
        }
    else
        {
        SetBoolAttributeL( EWlanIpDNSAddrFromServer, EFalse );
        }
    CleanupStack::PopAndDestroy( 2,ip1 );
    }


// ---------------------------------------------------------
// CCmPluginWlan::SetDns6IpFromServer
// ---------------------------------------------------------
//
void CCmPluginWlan::SetDns6IpFromServerL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetDns6IpFromServer" );

    HBufC* ip1 = GetStringAttributeL( EWlanIp6NameServer1 );
    CleanupStack::PushL( ip1 );

    HBufC* ip2 = GetStringAttributeL( EWlanIp6NameServer2 );
    CleanupStack::PushL( ip2 );

    if ( ( ip1->Compare( KDynIpv6Address ) == 0 ) &&
         ( ip2->Compare( KDynIpv6Address ) == 0 ) )
        {
        SetBoolAttributeL( EWlanIp6DNSAddrFromServer, ETrue );
        }
    else
        {
        SetBoolAttributeL( EWlanIp6DNSAddrFromServer, EFalse );
        }
    CleanupStack::PopAndDestroy( 2,ip1 );
    }


// ----------------------------------------------------------------------------
// CCmPluginWlan::CheckNetworkCoverageL()
// ----------------------------------------------------------------------------
//        
TInt CCmPluginWlan::CheckNetworkCoverageL() const
    {
    LOGGER_ENTERFN("CCmPluginWlan::CheckNetworkCoverageL");
    
    TInt retVal( 0 ); 
    
#ifdef __WINS__
    retVal = 1;
#else
    CCmWlanCoverageCheck* coverage = new (ELeave) CCmWlanCoverageCheck;
    CleanupStack::PushL( coverage );

    CLOG_WRITE( "Coverage created, calling GetCoverageL" );
    retVal = coverage->GetCoverageL();
    CLOG_WRITE( "GetCoverageL returned" );
    
    CleanupStack::PopAndDestroy( coverage );
#endif  // __WINS
    return retVal;
    }


// ----------------------------------------------------------------------------
// CCmPluginWlan::ConvertConnectionSecurityModeToSecModeL()
// ----------------------------------------------------------------------------
//        
TWlanSecMode CCmPluginWlan::ConvertConnectionSecurityModeToSecModeL(
                                    TWlanConnectionExtentedSecurityMode aSecurityMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ConvertConnectionSecurityModeToSecModeL " );

    switch( aSecurityMode )
        {
        case EWlanConnectionExtentedSecurityModeOpen:
            {
            return EWlanSecModeOpen;
            }
        
        case EWlanConnectionExtentedSecurityModeWepOpen:
        case EWlanConnectionExtentedSecurityModeWepShared:
            {
            return EWlanSecModeWep;
            }
        
        case EWlanConnectionExtentedSecurityMode802d1x:
            {
            return EWlanSecMode802_1x;
            }
        
        case EWlanConnectionExtentedSecurityModeWpa:
        case EWlanConnectionExtentedSecurityModeWpaPsk:
        case EWlanConnectionExtentedSecurityModeWpa2:
        case EWlanConnectionExtentedSecurityModeWpa2Psk:
            {
            return EWlanSecModeWpa;
            }
        case EWlanConnectionExtentedSecurityModeWapi:
        case EWlanConnectionExtentedSecurityModeWapiPsk:
            {
            if ( iIsWAPISupported )
                {
                return EWlanSecModeWAPI;
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    return EWlanSecModeOpen;
    }


// ----------------------------------------------------------------------------
// CCmPluginWlan::ConvertSecModeToConnectionSecurityModeL()
// ----------------------------------------------------------------------------
//        
TWlanConnectionExtentedSecurityMode 
        CCmPluginWlan::ConvertSecModeToConnectionSecurityModeL(
                                                        TWlanSecMode aSecMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ConvertSecModeToConnectionSecurityModeL " );

    switch( aSecMode )
        {
        case EWlanSecModeOpen:
            {
            return EWlanConnectionExtentedSecurityModeOpen;
            }
        
        case EWlanSecModeWep:
            {
            return EWlanConnectionExtentedSecurityModeWepOpen;
            }
        
        case EWlanSecMode802_1x:
            {
            return EWlanConnectionExtentedSecurityMode802d1x;
            }
        
        case EWlanSecModeWpa:
        case EWlanSecModeWpa2:
            {
            return EWlanConnectionExtentedSecurityModeWpa;
            }
        case EWlanSecModeWAPI:
            {
            if ( iIsWAPISupported )
                {
                return EWlanConnectionExtentedSecurityModeWapi;
                }
            else
                {
                User::Leave( KErrCorrupt );
                }
            }
        
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }

    return EWlanConnectionExtentedSecurityModeOpen;
    }


// ---------------------------------------------------------
// CCmPluginWlan::DestroyWaitDialog()
// ---------------------------------------------------------
//
void CCmPluginWlan::DestroyWaitDialog()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::DestroyWaitDialog " );

    delete iWaitDialog;
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginWlan::SetDaemonNameL
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::SetDaemonNameL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::SetDaemonNameL" );

    // we have to check first that these values has not been 
    // set(or changed) by any 3rd party sw to ensure that
    // we don't break their settings
    HBufC* daemonName = GetStringAttributeL( ECmConfigDaemonManagerName );
    if ( daemonName )
        {
        if ( daemonName->Compare( KDaemonManagerName ) != 0 )
    	    {
    	    delete daemonName;
            return;
    	    }
        delete daemonName;
        }
        
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
    
    
// --------------------------------------------------------------------------
// CCmPluginWlan::ConvertWLANConnectionStatesL
// --------------------------------------------------------------------------
//
TWlanNetMode CCmPluginWlan::ConvertWLANConnectionStatesL(
                                    TWlanConnectionMode aConnectionMode )
    {
    LOGGER_ENTERFN( "CCmPluginWlan::ConvertWLANConnectionStatesL " );
    
    switch ( aConnectionMode )
        {
        /** Values for possible WLAN connection states. */
        /** Connection to an infrastructure network is active. */
        case EWlanConnectionModeInfrastructure:    
            {
            return EInfra;
            }
        /** Connection to an ad-hoc network is active. */
        case EWlanConnectionModeAdhoc:
            {
            return EAdhoc;
            }
        default:
        	/**
        	 * The connection mode. This can be either Ad-hoc or infrastructure.
        	 * TWlanNetMode enum is to be used.
        	 * ( TUint32 - default: EInfra )
        	 */
            {
            return EInfra;
            }
        }
    }
  
// ---------------------------------------------------------------------------
// CCmPluginWlan::GetAdditionalUids
// ---------------------------------------------------------------------------
//
void CCmPluginWlan::GetAdditionalUids( RArray<TUint32>& aIapIds )
    {
    for (TInt i = 0; i<iAdditionalCmsCreated.Count(); i++ )
        {
        aIapIds.Append(iAdditionalCmsCreated[i]);
        }
    }
    
// --------------------------------------------------------------------------
// CCmPluginWlan::EnterWAPIPreSharedKeyL
// --------------------------------------------------------------------------
//
TBool CCmPluginWlan::EnterWAPIPreSharedKeyL()
    {
    LOGGER_ENTERFN( "CCmPluginWlan::EnterWAPIPreSharedKeyL" );
    
    TBool retVal ( EFalse );
    
    HBufC* key = HBufC::NewLC( KWlanMaxKeyLength );
    TPtr keyPtr( key->Des() );

    if ( !iConnUiUtils )
        {
        iConnUiUtils = CConnectionUiUtilities::NewL();
        }
    
    if ( iConnUiUtils->EasyWapiDlg( &keyPtr ) )
        {
        if ( !iWAPISecSettings )
            {
            iWAPISecSettings = CWAPISecuritySettings::NewL();
            }
        iWAPISecSettings->SetPreSharedKeyL( CWAPISecuritySettings::EWapiKeyAscii, *key );
        retVal = ETrue;
        }
    CleanupStack::PopAndDestroy( key );
    
    return retVal;
    }

