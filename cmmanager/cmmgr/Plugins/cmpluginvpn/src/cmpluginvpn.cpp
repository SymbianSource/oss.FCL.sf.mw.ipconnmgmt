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
* Description:  VPN plug-in IF implementation class.
*
*/

#include <AknsUtils.h>
#include <cmpluginvpn.mbg>
#include <cmpluginvpnui.rsg>
#include <cmmanager.rsg>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include <cmcommonui.h>
#include <aknPopup.h>
#include <aknlistquerydialog.h>
#include <AknIconArray.h>
#include <vpnapidefs.h>
#include <cmpluginwlandef.h>
// #include <vpnapi.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <featmgr.h>
#include <ConeResLoader.h>
#include <bautils.h>

#include <commsdattypesv1_1_partner.h>
#include <commsdattypeinfov1_1_internal.h>

#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include "cmvpncommonconstants.h"
#include <cmcommonconstants.h>
#include "cmpluginvpn.h"
#include "cmpvpnsettingsdlg.h"
#include "cmpvpnpolicyselectiondlg.h"
#include "cmpvpnnextlayerselectdlg.h"

using namespace CMManager;

_LIT( KPluginVpnFileIcons, "z:cmpluginvpn.mbm" );
//_LIT(KListItemFormat, "%d\t%S\t%S");

static const TCmAttribConvTable SVpnConvTbl[] = 
    {
        { EVpnServicePolicyName, EPluginVpnAttribRangeMax, NULL },
        { EVpnServicePolicy, KCDTIdVPNPolicyName, NULL,
          R_QTN_VPN_SETT_VPN_IAP_POLICY_REFERENCE,
          0, EConvCompulsory, R_QTN_VPN_SETT_VPN_IAP_POLICY_REFERENCE_COMPULSORY, 0, NULL, NULL },
        { EVpnServicePolicyName, 0, NULL,
          R_QTN_VPN_SETT_VPN_IAP_POLICY_REFERENCE,
          0, EConvCompulsory, R_QTN_VPN_SETT_VPN_IAP_POLICY_REFERENCE_COMPULSORY, 0, NULL, NULL },
        { EVpnIapId, KCDTIdVPNIAPRecord, NULL },
        { EVpnNetworkId, KCDTIdVPNNetwork, &ReadOnlyAttributeL },
        { 0, 0, NULL }
    };

static const TCmAttribConvTable SVpnVirtualIAPNextLayerConvTbl[] = 
    {
        { ECmNextLayerIapId, ECmNextLayerSNAPId+1, NULL },
        { ECmNextLayerIapId, KCDTIdVirtualNextLayerIAP, NULL },
        { ECmNextLayerSNAPId, KCDTIdVirtualNextLayerSNAP, NULL },
        { 0, 0, NULL }
    };
    
_LIT( KVpnVirtualBearerName, "vpnbearer" );
_LIT( KVpnVirtualBearerAgent, "vpnconnagt.agt" );
_LIT( KVpnVirtualBearerNif, "tunnelnif" );

// constants needed for the virtualbearer table
const TInt KNoTimeout = -1;
const TUint32 KLastSessionTimeout = 3;


const TUint32 KContainsVpn =        0x01;
const TUint32 KLinksBackToParent =  0x02;
const TUint32 KEmptyDest =          0x04;
const TUint32 KNoNonVirtuals =      0x08;

const TUint32 KBindDenied = KLinksBackToParent | KContainsVpn;

// --------------------------------------------------------------------------
// CCmPluginVpn::NewL()
// --------------------------------------------------------------------------
//
CCmPluginVpn* CCmPluginVpn::NewL( TCmPluginInitParam* aInitParam )
    {

    CRepository* repository = CRepository::NewLC(KCRUidCommunicationSettings);

    TInt vpnSupported;
    repository->Get(KSettingsVPNSupported, vpnSupported);

    CleanupStack::PopAndDestroy(repository);

    if( !vpnSupported )
        {
        User::Leave( KErrNotSupported );
        }

    CCmPluginVpn* self = new( ELeave ) CCmPluginVpn( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::~CCmPluginVpn()
// --------------------------------------------------------------------------
//
CCmPluginVpn::~CCmPluginVpn()
    {
    CCmPluginVpn::AdditionalReset();
    
    RemoveResourceFile( KPluginVPNResDirAndFileName );
    
    iBindableMethods.Reset();
    iBindableMethods.Close();
    delete iPolicyName;
    CLOG_CLOSE;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::CreateInstanceL
// --------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginVpn::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginVpn* self = new( ELeave ) CCmPluginVpn( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// --------------------------------------------------------------------------
// CCmPluginVpn::CCmPluginVpn()
// --------------------------------------------------------------------------
//
CCmPluginVpn::CCmPluginVpn( TCmPluginInitParam* aInitParam )
    : CCmPluginBaseEng( aInitParam )
    , iBindableMethods( KCmArraySmallGranularity )
    {
    CLOG_CREATE;
    iBearerType = KPluginVPNBearerTypeUid;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::ConstructL()
// --------------------------------------------------------------------------
//
void CCmPluginVpn::ConstructL()
    {
    CCmPluginBaseEng::ConstructL();

    TRAP_IGNORE( iVirtualTableId = 
                        CCDVirtualIAPNextLayerRecord::TableIdL( Session() ) );
    if( !iVirtualTableId )
        {
        iVirtualTableId = CCDVirtualIAPNextLayerRecord::CreateTableL( Session() );
        }

    AddResourceFileL( KPluginVPNResDirAndFileName );
    
    AddConverstionTableL( (CCDRecordBase**)&iServiceRecord, NULL, SVpnConvTbl );
    
    iPolicyName = KNullDesC().AllocL();
    }
    
// --------------------------------------------------------------------------
// CCmPluginVpn::GetIntAttributeL()
// --------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GetIntAttributeL" );

    TUint32 retVal( 0 );
    
    switch( aAttribute )
        {
        case ECmBearerIcon:
            {
            TAknsItemID id;
            
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
                   
            TParse mbmFile;
            User::LeaveIfError( mbmFile.Set( KPluginVpnFileIcons, 
                                             &KDC_BITMAP_DIR, 
                                             NULL ) );
                   
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
            retVal = KCommDbBearerVirtual;
            }
            break;
        
        case ECmDefaultUiPriority:
        case ECmDefaultPriority:            
            {
            retVal = aAttribute == ECmDefaultPriority ?
                        GlobalBearerPriority( TPtrC(KCDTypeNameVPNService) ) :
                        GlobalUiBearerPriority( TPtrC(KCDTypeNameVPNService) );
            }
            break;  
            
        case EVpnIapId:
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }

            retVal = ServiceRecord().iServiceIAP;
            }
            break;
        case EVpnNetworkId:
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }

            TUint32 snapId = iVirtualRecord->iNextLayerSNAP;
            if ( snapId > KCmDefaultDestinationAPTagId &&
                    snapId < ( KCmDefaultDestinationAPTagId+255 ) )
                {
                retVal = snapId - KCmDefaultDestinationAPTagId;
                }
            else
                {
                retVal = 0;
                }
            }
            break;
        case ECmNextLayerIapId:    
            {
            retVal = iVirtualRecord->iNextLayerIAP;
            }  
            break;
        case ECmNextLayerSNAPId:           
            {
            retVal = iVirtualRecord->iNextLayerSNAP;
            }  
            break;
        case ECmIapId:
        case ECmId:
        case ECmWapId:
        case ECmIapServiceId:
        case ECmInvalidAttribute:
        case ECmLoadResult:
        case ECmBearerType:
        case ECmSeamlessnessLevel:
        case ECmElementID:
        case ECmNetworkId:
        case ECmProxyPortNumber:
        case ECmExtensionLevel:
        case ECmWapIPWSPOption:
        case ECmWapIPProxyPort:
        case ECmNamingMethod:
            {
            retVal = CCmPluginBaseEng::GetIntAttributeL( aAttribute );
            }
            break;
        
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
        
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::GetBoolAttributeL()
// --------------------------------------------------------------------------
//
TBool CCmPluginVpn::GetBoolAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GetBoolAttributeL" );

    TBool retVal( EFalse );

    switch( aAttribute )
        {
        case ECmAddToAvailableList:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmChargeCardUsageEnabled:
            {
            retVal = EFalse;
            }
            break;
        case ECmBearerHasUi:
            {
            retVal = ETrue;
            break;
            }
        case ECmCoverage:
        case ECmDestination:
        case ECmIPv6Supported:
        case ECmHidden:
        case ECmProtected:
        case ECmMetaHighlight:
        case ECmMetaHiddenAgent:
        case ECmProxyUsageEnabled:
        case ECmConnected:
        case ECmIsLinked:
        case ECmWapIPSecurity:
            {
            retVal = CCmPluginBaseEng::GetBoolAttributeL( aAttribute );
            }
            break;
        case ECmVirtual:
            {
            return ETrue;
            }           
        default:
            {
            User::Leave( KErrNotSupported );
            
            }
        }
        
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::GetStringAttributeL()
// --------------------------------------------------------------------------
//
HBufC* CCmPluginVpn::GetStringAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GetStringAttributeL" );

    HBufC* retVal = NULL;
    
    switch( aAttribute )
        {           
        case ECmBearerSupportedName:
            {
            retVal = AllocReadL( R_QTN_NETW_CONSET_BEARER_SUPPORTED_VPN );
            }
            break;
        
        case EVpnServicePolicyName:
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }
            else 
                {
                CCmPluginVpn* tmp = const_cast< CCmPluginVpn * >( this );
                tmp->UpdatePolicyNameL();
                }
            retVal = iPolicyName->AllocL();
            break;
            }
        case EVpnServicePolicy:
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }

            if( !ServiceRecord().iServicePolicy.IsNull() )
                {
                retVal = ServiceRecord().iServicePolicy.GetL().AllocL();
                } 
            else
                {
                retVal = KNullDesC().AllocL();
                }
            }
            break;
            
        case ECmName:   
        case ECmProxyServerName:
        case ECmProxyProtocolName:
        case ECmProxyExceptions:
        case ECmWapIPGatewayAddress:
        case ECmWapIPProxyLoginName:
        case ECmWapIPProxyLoginPass:
        case ECmStartPage:
            {
            retVal = CCmPluginBaseEng::GetStringAttributeL( aAttribute );
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
        
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::GetString8AttributeL()
// --------------------------------------------------------------------------
//
HBufC8* CCmPluginVpn::GetString8AttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GetString8AttributeL" );

    HBufC8* retVal = NULL;
    
    switch( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
        
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::SetIntAttributeL()
// --------------------------------------------------------------------------
//
void CCmPluginVpn::SetIntAttributeL( const TUint32 aAttribute, 
                                            TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::SetIntAttributeL" );

    switch( aAttribute )
        {
        case ECmNextLayerIapId: 
        case EVpnIapId:
            {
            CheckRecordIdValidityL( this, EVpnIapId, &aValue );
            
            // ServiceRecord.iServiceIAP should be 0 according to 
            // DM CommsDat Contents v1.4, but for now it is made
            // backward compatible
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }
            
            ServiceRecord().iServiceIAP = aValue;
            ServiceRecord().iServiceNetwork = 0;
            ServiceRecord().iServiceSNAP = 0;
            iVirtualRecord->iNextLayerIAP = aValue;
            iVirtualRecord->iNextLayerSNAP = 0;            
            }
            break;

        case EVpnNetworkId:
        case ECmNextLayerSNAPId:
            {
            // To keep backwards compability, keep these 2 attributes
            // (EVpnNetworkId & ECmNextLayerSNAPId) together even though
            // network id and destination id are no longer exactly the same.
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }

            TInt destinationId( 0 );
            destinationId = CheckValidityAndConvertDestinationIdL( aValue );
            TMDBElementId defaultAP = DefaultAPRecordL();
            
            ServiceRecord().iServiceIAP = 0;
            ServiceRecord().iServiceNetwork = 0;
            ServiceRecord().iServiceSNAP = defaultAP;
            iVirtualRecord->iNextLayerIAP = 0;
            iVirtualRecord->iNextLayerSNAP = destinationId;
            }
            break;

        case ECmSeamlessnessLevel:
        case ECmProxyPortNumber:
        case ECmNamingMethod:
        case ECmWapIPWSPOption:
        case ECmWapIPProxyPort:
            {
            CCmPluginBaseEng::SetIntAttributeL( aAttribute, aValue );
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::SetBoolAttributeL()
// --------------------------------------------------------------------------
//
void CCmPluginVpn::SetBoolAttributeL( const TUint32 aAttribute, 
                                      TBool aValue )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::SetBoolAttributeL" );

    switch( aAttribute )
        {
        case ECmChargeCardUsageEnabled:
            {
            User::Leave( KErrNotSupported );
            }
            break;

        case ECmHidden:
        case ECmProtected:
        case ECmMetaHighlight:
        case ECmMetaHiddenAgent:
        case ECmProxyUsageEnabled:
        case ECmWapIPSecurity:
            {
            CCmPluginBaseEng::SetBoolAttributeL( aAttribute, aValue );
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::SetStringAttributeL()
// --------------------------------------------------------------------------
//
void CCmPluginVpn::SetStringAttributeL( const TUint32 aAttribute, 
                                        const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::SetStringAttributeL" );

    switch( aAttribute )
        {
        case EVpnServicePolicyName:
            {
            HBufC* tmp = aValue.AllocLC();
            delete iPolicyName;
            iPolicyName = tmp;
            CleanupStack::Pop( tmp );
            break;
            }        
        case EVpnServicePolicy:
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }
            
            CLOG_WRITE_1( "New policy set:[%S]", &aValue );
            ServiceRecord().iServicePolicy.SetL( aValue );
            }
            break;
        case ECmProxyServerName:
            {
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            if( !aValue.Length() || !iProxyRecord->iPortNumber )
                {
                iProxyRecord->iPortNumber = KVpnProxyPortNumberDefault;
                }
            }
            break;

        case ECmName:
        case ECmProxyProtocolName:
        case ECmProxyExceptions:
        case ECmWapIPGatewayAddress:
        case ECmWapIPProxyLoginName:
        case ECmWapIPProxyLoginPass:
        case ECmStartPage:
            {
            CCmPluginBaseEng::SetStringAttributeL( aAttribute, aValue );
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::SetString8AttributeL()
// --------------------------------------------------------------------------
//
void CCmPluginVpn::SetString8AttributeL( const TUint32 aAttribute, 
                                         const TDesC8& /*aValue*/ )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::SetString8AttributeL" );

    switch( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::CanHandleIapIdL()
// --------------------------------------------------------------------------
//
TBool CCmPluginVpn::CanHandleIapIdL( TUint32 aIapId ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::CanHandleIapIdL1" );
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
// CCmPluginVpn::CanHandleIapIdL()
// --------------------------------------------------------------------------
//
TBool CCmPluginVpn::CanHandleIapIdL( CCDIAPRecord *aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::CanHandleIapIdL" );
    
    TBool retVal( EFalse );
    
    if( TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameVPNService) ||
        TPtrC(aIapRecord->iBearerType) == TPtrC(KCDTypeNameVirtualBearer) )
        {
        // Further comparision is to find exact info that the IAP can handle by this plugin
        CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
                new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>
                                                   (KCDTIdVirtualBearerRecord);
        CleanupStack::PushL( bearersRS );

        CCDVirtualBearerRecord* bearerRecord = 
                static_cast<CCDVirtualBearerRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));

        CleanupStack::PushL( bearerRecord );
    
        // Find entries used "vpnconnagt.agt" as agent from Table VirtualBearer
        bearerRecord->iBearerAgent.SetL( KVpnVirtualBearerAgent );
    
        bearersRS->iRecords.AppendL( bearerRecord );

        CleanupStack::Pop( bearerRecord );
        bearerRecord = NULL;
        
        if ( bearersRS->FindL( Session() ) )
            {
            TUint32 recordId = (*bearersRS)[0]->RecordId();
            TPtrC bearerName( (*bearersRS)[0]->iRecordName.GetL() );
        
            // Further comparing record ID referred to by this VPN IAP with entry ID in table VirtualBearer
            // And also comparing bear name with our expected one "vpnbearer"
            if( recordId == aIapRecord->iBearer && bearerName 
                == TPtrC( KVpnVirtualBearerName ) )
                {
                retVal = ETrue;
                }
            }
        
        CleanupStack::PopAndDestroy( bearersRS );
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::RunSettingsL()
// --------------------------------------------------------------------------
//        
TInt CCmPluginVpn::RunSettingsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::RunSettingsL" );
    CmPluginVpnSettingsDlg* dlg = CmPluginVpnSettingsDlg::NewL( *this );
    
    return dlg->ConstructAndRunLD();
    }        

// --------------------------------------------------------------------------
// CCmPluginVpn::LoadServiceSettingL()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::LoadServiceSettingL" );
    
    if( TPtrC(KCDTypeNameVPNService) == iIapRecord->iServiceType  )
        {
        iServiceRecord = static_cast<CCDVPNServiceRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVPNServiceRecord));
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }

        
    CCmPluginBaseEng::LoadServiceSettingL();
    }
    

// --------------------------------------------------------------------------
// CCmPluginVpn::LoadAdditionalRecordsL()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::LoadAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::LoadAdditionalRecordsL" );

    iVirtualRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    
    CMDBRecordSet<CCDVirtualIAPNextLayerRecord>* virtualRS = 
              new(ELeave) CMDBRecordSet<CCDVirtualIAPNextLayerRecord>( iVirtualTableId );
    CleanupStack::PushL( virtualRS );
    
    CCDVirtualIAPNextLayerRecord* record = 
                            new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    
    record->iIAP = iIapId;
    CleanupStack::PushL( record );
    virtualRS->iRecords.AppendL( record );
    CleanupStack::Pop( record );
    record = NULL;
    
    if( virtualRS->FindL( Session() ) )
        {
        iVirtualRecord->SetRecordId( (*virtualRS)[0]->RecordId() );
        
        iVirtualRecord->LoadL( Session() );
        
        CLOG_WRITE_3( "Next layer: IAP[%d], NextSNAP[%d], NextIAP[%d]",
                      TUint( iVirtualRecord->iIAP ),
                      TUint( iVirtualRecord->iNextLayerSNAP ),
                      TUint( iVirtualRecord->iNextLayerIAP ) );
        }
    else
        {
        CLOG_WRITE( "Virtual records not found" );
        }
    
    CleanupStack::PopAndDestroy( virtualRS );
    AddConverstionTableL( (CCDRecordBase**)&iVirtualRecord, NULL, SVpnVirtualIAPNextLayerConvTbl );
    
    if (iIapRecord->iRecordName.GetL() == KNullDesC) 
        {
        //now we have next layer info, so generate default cm name if name is empty (new cm)
        GenerateDefaultCmNameL();
        }
        
    if ( !iProxyRecord->iPortNumber )
        {
        iProxyRecord->iPortNumber = KVpnProxyPortNumberDefault;
        }
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::InitializeWithUiL()
// --------------------------------------------------------------------------
//        
TBool CCmPluginVpn::InitializeWithUiL( TBool /*aManuallyConfigure*/ )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::InitializeWithUiL" );
    TInt retVal( EFalse );
    
    // Compile a list of bindable methods, issue a warning if none found
    if ( BindableRealConnectionsExistL() )
        {
        // check if the intended destination is not available for VPN,
        // or, if the destination has not been set yet, whether there is 
        // at least one suitable destination to place it in
        TBool validParent = EFalse;
        
        if ( iParentDest )
            {
            // parent destination is known
            // check if it is not linked by any other VPN
            validParent = !HasUpstreamVpnL( iParentDest, ETrue );
            }
        else
            {
            // parent destination is not known yet
            // check if there is any destination into which this VPN could be put

            // trigger the recalculation
            RArray<TUint32> parents ( CmMgr().DestinationCountL() );
            CleanupClosePushL( parents );
            
            CmMgr().AllDestinationsL( parents );
            
            FilterPossibleParentsL( parents );

            // if there is only one such destination, we should filter out 
            // that one from the underlying candidates
            if ( parents.Count() == 1 )
                {
                TInt destIndex = iBindableMethods.Find( parents[0] );
                if ( destIndex != KErrNotFound )
                    {
                    // got it, now remove
                    iBindableMethods.Remove( destIndex );
                    }

                if ( iBindableMethods.Count() )
                    {
                    // if there are still some methods to bind to,
                    // mark the destination as valid
                    validParent = ETrue;
                    }
                    
                }
            else 
                {
                validParent = ( parents.Count() > 0 );
                }
                
            CleanupStack::PopAndDestroy( &parents );
            }

        if ( !validParent )
            {
            TCmCommonUi::ShowNoteL( R_CMWIZARD_NO_VPN_STACKING,
                                    TCmCommonUi::ECmErrorNote );
            
            return retVal;
            }
              
        
        // Check VPN policies installed on the device
        if ( ShowPolicySelectionDlgL() )
            {
            retVal = ShowRealConnectionSelectionDlgL();

            if ( retVal )
                {
                if ( iIapRecord->iRecordName.GetL() == KNullDesC ) 
                    {
                    // now we have next layer info, so generate default cm name 
                    GenerateDefaultCmNameL();
                    }
                
                }
            }
        }
    else
        {
        HBufC* buf = AllocReadL( R_QTN_NETW_CONSET_INFO_NO_CM_TO_BIND_TO );
        TPtr ptr ( buf->Des() );
        CleanupStack::PushL( buf );
        TCmCommonUi::ShowNoteL( ptr, TCmCommonUi::ECmErrorNote );
        CleanupStack::PopAndDestroy( buf );
        }              
    
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecord()
// --------------------------------------------------------------------------
//        
CCDVPNServiceRecord& CCmPluginVpn::ServiceRecord() const
    {
    LOGGER_ENTERFN( "CCmPluginVpn::ServiceRecord" );
    return *static_cast<CCDVPNServiceRecord*>( iServiceRecord );
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecordExists()
// --------------------------------------------------------------------------
// 
TBool CCmPluginVpn::ServiceRecordExists() const
    {
    if ( !iServiceRecord )
        {
        return EFalse;
        }
    return ETrue;
    }
    
// --------------------------------------------------------------------------
// CCmPluginVpn::CreateNewServiceRecordL()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::CreateNewServiceRecordL" );

    delete iServiceRecord; iServiceRecord = NULL;
    
    iServiceRecord = static_cast<CCDVPNServiceRecord *>
                    (CCDRecordBase::RecordFactoryL( KCDTIdVPNServiceRecord ));

    CLOG_WRITE("New service record, policy emptied");
    ServiceRecord().iServicePolicy.SetL( KNullDesC );
    ServiceRecord().iServiceIAP = 0;
    ServiceRecord().iServiceNetwork = 0;
    ServiceRecord().iServiceSNAP = 0;

    
    iProxyRecord->iPortNumber = KVpnProxyPortNumberDefault;

    SetIntAttributeL( ECmSeamlessnessLevel, ESeamlessnessShowprogress );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CreateAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::CreateAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::CreateAdditionalRecordsL" );

    delete iVirtualRecord;
    iVirtualRecord = NULL;
    
    iVirtualRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::UpdateServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::UpdateServiceRecordL" );

    CCmPluginBaseEng::UpdateServiceRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::UpdateAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::UpdateAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::UpdateAdditionalRecordsL" );

    CheckIfNameModifiedL( iVirtualRecord );
    
    if( !iVirtualRecord->RecordId() )
        {
        iVirtualRecord->SetRecordId( KCDNewRecordRequest );
        
        iVirtualRecord->iIAP = iIapId;
        iVirtualRecord->StoreL( Session() );
        }
    else
        {
        iVirtualRecord->ModifyL( Session() );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::ServiceRecordIdLC( HBufC* &aName, 
                                      TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::ServiceRecordIdLC" );

    aName = TPtrC( KCDTypeNameVPNService ).AllocLC();
    aRecordId = iServiceRecord->RecordId();
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::BearerRecordIdLC()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::BearerRecordIdLC( HBufC* &aBearerName, 
                                     TUint32& aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::BearerRecordIdLC" );

    CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
              new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>
                                                   (KCDTIdVirtualBearerRecord);
    CleanupStack::PushL( bearersRS );

    // Now try to find the linked proxy record
    // create new record
    CCDVirtualBearerRecord* bearerRecord = 
                static_cast<CCDVirtualBearerRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));

    CleanupStack::PushL( bearerRecord );
    
    bearerRecord->iBearerAgent.SetL( KVpnVirtualBearerAgent );
    
    bearersRS->iRecords.AppendL( bearerRecord );

    CleanupStack::Pop( bearerRecord );
    bearerRecord = NULL;
    
    CLOG_WRITE_1( "Looking for bearer: [%d]", TUint32(iIapRecord->iService) );
    
    if ( !bearersRS->FindL( Session() ) )
        // No bearer record found -> create a default one.
        {
        bearerRecord = static_cast<CCDVirtualBearerRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
        CleanupStack::PushL(bearerRecord);

        bearerRecord->iRecordName.SetL( KVpnVirtualBearerName );
        bearerRecord->iBearerAgent.SetL( KVpnVirtualBearerAgent );
        bearerRecord->iVirtualBearerNifName.SetL(KVpnVirtualBearerNif);
        bearerRecord->iLastSocketActivityTimeout = (TUint32)KNoTimeout;
        bearerRecord->iLastSessionClosedTimeout = KLastSessionTimeout;
        bearerRecord->iLastSocketClosedTimeout = (TUint32)KNoTimeout;
        
        bearerRecord->SetRecordId( KCDNewRecordRequest );
        bearerRecord->StoreL( Session() );
        
        aRecordId = bearerRecord->RecordId();

        CleanupStack::PopAndDestroy(bearerRecord);
        }
    else
        {
        aRecordId = (*bearersRS)[0]->RecordId();
        }

    CleanupStack::PopAndDestroy( bearersRS );

    aBearerName = TPtrC( KCDTypeNameVirtualBearer ).AllocLC();
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::AdditionalReset()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::AdditionalReset()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::AdditionalReset" );

    delete iNetworkRecord;
    iNetworkRecord = NULL;

    delete iVirtualRecord;
    iVirtualRecord = NULL;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::PrepareToUpdateRecordsL()
// --------------------------------------------------------------------------
//        
void CCmPluginVpn::PrepareToUpdateRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::PrepareToUpdateRecordsL" );
    // Check here if every data of the connection method is valid
    // before records were saved to CommsDat.
    // Set incorrect attribute id in iInvalidAttribute and leave
    // with KErrArgument.
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::ShowPolicySelectionDlgL
// --------------------------------------------------------------------------
//  
TBool CCmPluginVpn::ShowPolicySelectionDlgL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::ShowPolicySelectionDlgL" );

    TBool retVal ( EFalse );
    
    CmPluginVpnPolicySelectionDlg* dlg = 
                                CmPluginVpnPolicySelectionDlg::NewL( *this );
    CleanupStack::PushL( dlg );
    retVal = dlg->ShowPolicySelectionListL();
    CleanupStack::PopAndDestroy();
    
    return retVal;
    }


// --------------------------------------------------------------------------
// CCmPluginVpn::BindableRealConnectionsExistL
// --------------------------------------------------------------------------
//  
TBool CCmPluginVpn::BindableRealConnectionsExistL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::BindableRealConnectionsExistL" );

    TBool retVal ( EFalse );
    
    iBindableMethods.Close();
    
    // Compile and array of destination IDs which the VPN CM can be bound to
    // includes Uncategorised destination and Easy WLAN if valid
    CmMgr().AllDestinationsL( iBindableMethods );

    // Filter out invalid methods
    for ( TInt i = 0; i < iBindableMethods.Count(); i++ )
        {
        TUint32 bindresult = 
                CanDestUsedAsUnderlyingConnectionL( iBindableMethods[i] );
                
        if ( bindresult & (KBindDenied | KNoNonVirtuals | KEmptyDest ) )
            {
            iBindableMethods.Remove( i );
            i--;
            }
        }
    
    // If there are destination IDs which the VPN CM can be bound to.
    if( iBindableMethods.Count() )
        {
        retVal = ETrue;
        }

    // 2. Are there uncategorised connection methods which aren't virtual?
    RArray<TUint32> cmArray ( KCmArrayMediumGranularity );
    CleanupClosePushL( cmArray );
    CmMgr().ConnectionMethodL( cmArray, EFalse );
    
    for ( TInt k = 0; k < cmArray.Count(); k++ )
        {
        TInt bearerType (0);
        TRAPD( err, bearerType = CmMgr().GetConnectionMethodInfoIntL( 
            cmArray[k], ECmBearerType ););

        //if no plugin is found to handle the bearertype, GetConnectionMethodInfoIntL leaves with KErrNotSupported
        if ( err == KErrNotSupported )				
            {
            //continue with the next element in the array
            continue;
            }
                                                
        // uncategorised contains a non-virtual connection method
        if ( !CmMgr().GetBearerInfoBoolL( bearerType, ECmVirtual ) )
            {
            retVal = ETrue;
            iBindableMethods.Append( KDestItemUncategorized );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &cmArray );
    
    // 3. Is Easy WLAN available?
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {        
        // easy wlan is available
        if ( CmMgr().EasyWlanIdL() )
            {
            iBindableMethods.Append( KDestItemEasyWlan );
            retVal = ETrue;
            }
        }
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::ShowRealConnectionSelectionDlgL
// --------------------------------------------------------------------------
//  
TBool CCmPluginVpn::ShowRealConnectionSelectionDlgL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::ShowRealConnectionSelectionDlgL" );
    TBool retVal ( EFalse );

    TBool snapSelected ( EFalse );
    TUint32 nextLayerId ( 0 );
    
    CmPluginVpnNextLayerSelectDlg* dlg = 
                    CmPluginVpnNextLayerSelectDlg::NewL( *this,
                                                         iBindableMethods,
                                                         snapSelected,
                                                         nextLayerId );
    if ( dlg->ExecuteLD( R_VPN_REAL_CM_SELECTION_LIST ) )
        {
        // Handle results
        retVal = ETrue;
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::IsLinkedToIap
// --------------------------------------------------------------------------
//  
TBool CCmPluginVpn::IsLinkedToIap( TUint32 aIapId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::IsLinkedToIap" );

    return ( ServiceRecord().iServiceIAP == aIapId );
    }


// --------------------------------------------------------------------------
// CCmPluginVpn::IsLinkedToSnap
// --------------------------------------------------------------------------
//  
TBool CCmPluginVpn::IsLinkedToSnap( TUint32 aSnapId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::IsLinkedToSnap" );

    return ( iVirtualRecord->iNextLayerSNAP == aSnapId );
    }


// ---------------------------------------------------------------------------
// CCmPluginVpn::RestoreAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::RestoreAttributeL( const TUint32 aAttribute )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::RestoreAttributeL" );
        
    if ( aAttribute == EVpnServicePolicyName )
        {
        // also restore policy name
        RVpnServ ps;
        CleanupClosePushL<RVpnServ>( ps );
        
        TInt err = ps.Connect();
        
        if ( !err )
            {
            if( !ServiceRecordExists() )
                {
                User::Leave( KErrNotSupported );
                }

            CCDVPNServiceRecord *r = &(ServiceRecord());
            TVpnPolicyName pId( r->iServicePolicy );
            
            TInt policyCount(0);
            err = ps.EnumeratePolicies(policyCount);
            if ( !err && policyCount)
                {                
                CArrayFixFlat<TVpnPolicyInfo>* policyList;
                policyList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(policyCount);
                CleanupStack::PushL(policyList);

                err = ps.GetPolicyInfoList(policyList);
                
                TBool found(EFalse);
                
                if ( err == KErrNone )
                    {
                    for (TInt i(0); i<policyList->Count(); ++i)
                        {
                        if (pId.Compare((*policyList)[i].iId) == 0 )
                            {
                            SetStringAttributeL( EVpnServicePolicyName,
                                (*policyList)[i].iName );
                            found = ETrue;
                            break;    
                            }
                        }                    
                    }
                    
                if ( err || (!found) )
                    {
                    CLOG_WRITE_1("Error when restoring policy attribute, [%d]", err );
                    // the set policies are not installed,
                    // or we can not access vpn client:
                    // Must be defined qtn_selec_setting_compulsory is displayed 
                    // for the invalid settings.                           
                    // reset policy to compulsory
                    SetStringAttributeL( EVpnServicePolicyName,
                                                          KNullDesC() );
                    SetStringAttributeL( EVpnServicePolicy, 
                                                          KNullDesC() );                        
                    }
                CleanupStack::PopAndDestroy(policyList);              
                }
            }
        CleanupStack::PopAndDestroy(); // ps.Close
        }
    // Type of EVpnIapId is ELink. This is plugin specified type.
    // So, it should be handled here.
    else if( aAttribute == EVpnIapId )
        {
        if( !ServiceRecordExists() )
            {
            User::Leave( KErrNotSupported );
            }

        ServiceRecord().iServiceIAP = 0;
        iVirtualRecord->iNextLayerIAP = 0;
        iVirtualRecord->iNextLayerSNAP = 0;
        }
    else
        {
        CCmPluginBaseEng::RestoreAttributeL( aAttribute );
        }
    }



// --------------------------------------------------------------------------
// CCmPluginVpn::GenerateDefaultCmNameL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::GenerateDefaultCmNameL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GenerateDefaultCmNameL" );

    //create default string
    RFs fs;
    CleanupClosePushL<RFs>( fs );
    User::LeaveIfError( fs.Connect() );

    // There may not be a UI context yet....
    TFileName resourceFile;
    _LIT( KDriveZ, "z:" );
    resourceFile.Append( KDriveZ );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    _LIT( KCmmResFileName, "cmpluginvpnui.rsc" );
    resourceFile.Append( KCmmResFileName );
    BaflUtils::NearestLanguageFile( fs, resourceFile );

    RResourceFile rf;
    CleanupClosePushL<RResourceFile>( rf );
    rf.OpenL( fs, resourceFile );
    rf.ConfirmSignatureL( 0 );
    HBufC8* readBuffer = rf.AllocReadLC( R_QTN_NETW_CONSET_METHOD_NAME_VPN );
    const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                 ( readBuffer->Length() + 1 ) >> 1 );
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
    CleanupStack::PushL( textBuffer );
    *textBuffer=ptrReadBuffer;

    // Find %U and replace it with %S
    TPtr sourceString = textBuffer->Des();
    _LIT(KPercentS,"%S");
    _LIT(KPercentU,"%U");
    TInt puPos = sourceString.Find(KPercentU);
    if ( puPos >= 0 )
        {
        sourceString.Replace( puPos, 
            static_cast<TDesC>(KPercentS).Length(), KPercentS );                
        }

    // Check if there is a length defined in copy of the recourse string e.g. "VPN(%U[08])"
    // If it's found convert it to integer and remove it from the string.
    TInt maxDataLength( 0 );
    TInt perceLength = static_cast<TDesC>(KPercentS).Length();
    if ( sourceString[puPos+perceLength] == '[' )
        {
        // Key includes max datalength 
        maxDataLength = 10*( sourceString[puPos+perceLength+1]-'0' ) 
                           + ( sourceString[puPos+perceLength+2]-'0' );
        sourceString.Delete(puPos+perceLength,4);
        }

    // Get the IAP name which the VPM Connection Method points to
    HBufC* refName = NULL;

    if( !ServiceRecordExists() )
        {
        User::Leave( KErrNotSupported );
        }
    
    if ( ServiceRecord().iServiceIAP )
        {
        // ask for the bearer type
        TUint32 bearerType = CmMgr().GetConnectionMethodInfoIntL( 
                ServiceRecord().iServiceIAP, ECmBearerType );
        
        if ( iCmMgr.EasyWlanIdL() && bearerType == KUidWlanBearerType )
            {           
            refName = GenerateVPNPointToWLANNetworkLC(rf, refName);
            }
        else
            {
            refName = iCmMgr.GetConnectionMethodInfoStringL(
                                                ServiceRecord().iServiceIAP,
                                                ECmName );
            CleanupStack::PushL( refName );
            }
        }
    else
        {
        CCmDestinationImpl* dest = iCmMgr.DestinationL( 
                                            iVirtualRecord->iNextLayerSNAP );
        CleanupStack::PushL( dest );
        refName = dest->NameLC();
        CleanupStack::Pop( refName ); 
        CleanupStack::PopAndDestroy( dest );
        CleanupStack::PushL( refName );
        }
    
    TPtr16 refNamePtr = refName->Des();
    // If there was a length definition e.g. [08]--> length is 8 the CM or
    // Destination name must be cut to length - 1 and copy the ellipsis(shown as
    // "...") to the end.
    if( maxDataLength > 0 && refName->Length() > maxDataLength )
        {
        refNamePtr.Delete( maxDataLength-1, refName->Length()-maxDataLength+1);
        TText ellipsis(KEllipsis);
        refNamePtr.Insert( maxDataLength-1, TPtrC(&ellipsis,1) );
        }
        
    if ( maxDataLength == 0 )
        {
        // Length not defined in resource string so set it to refName length
        maxDataLength = refNamePtr.Length();
        }
    
    HBufC* cmName = HBufC::NewL( sourceString.Length() + maxDataLength );
    CleanupStack::PushL( cmName );
    cmName->Des().Format(sourceString,  &refNamePtr);
    SetStringAttributeL( ECmName,  *cmName );
    CleanupStack::PopAndDestroy( cmName );
    CleanupStack::PopAndDestroy( refName ); 
    CleanupStack::PopAndDestroy( textBuffer ); 
    CleanupStack::PopAndDestroy( readBuffer ); 
    CleanupStack::PopAndDestroy( &rf );
    CleanupStack::PopAndDestroy( &fs );
    }
    
// --------------------------------------------------------------------------
// CCmPluginVpn::GenerateDefaultCmNameL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::CreateNewL()
    {
    CCmPluginBaseEng::CreateNewL();

    iIapRecord->iRecordName.SetL( KNullDesC );
    }



// --------------------------------------------------------------------------
// CCmPluginVpn::LoadL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::LoadL( TUint32 aIapId )
    {
    CCmPluginBaseEng::LoadL( aIapId );
    if ( iServiceRecord )
        {
//      UpdatePolicyNameL();
        }
    }
        
    
// --------------------------------------------------------------------------
// CCmPluginVpn::UpdatePolicyNameL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::UpdatePolicyNameL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::UpdatePolicyNameL" );
    // now get policy name from ID
    
    RVpnServ ps;
    CleanupClosePushL<RVpnServ>( ps );
    TInt err = ps.Connect();

    CLOG_WRITE_1( "Connected to VPN server, err [%d]", err ); 
    
    if (! err )        
        {
        if( !ServiceRecordExists() )
            {
            User::Leave( KErrNotSupported );
            }

        CCDVPNServiceRecord *r = &(ServiceRecord());
        TVpnPolicyName pId( r->iServicePolicy );
        TInt policyCount(0);
        err = ps.EnumeratePolicies(policyCount);
        if ( !err && policyCount)
            {                
            CArrayFixFlat<TVpnPolicyInfo>* policyList;
            policyList = new (ELeave) CArrayFixFlat<TVpnPolicyInfo>(policyCount);
            CleanupStack::PushL(policyList);

            err = ps.GetPolicyInfoList(policyList);
            if ( err )
                {
                // the set policies are not installed, 
                // Must be defined qtn_selec_setting_compulsory is displayed 
                // for the invalid settings.                           
                // reset policy to compulsory
                SetStringAttributeL( EVpnServicePolicyName,
                                                      KNullDesC() );
                SetStringAttributeL( EVpnServicePolicy, 
                                                      KNullDesC() );                        
                }
            else
                {
                for (TInt i(0); i<policyList->Count(); ++i)
                    {
                    if (pId.Compare((*policyList)[i].iId) == 0 )
                        {
                        SetStringAttributeL( EVpnServicePolicyName,
                            (*policyList)[i].iName );
                        break;    
                        }
                    }
                }
            CleanupStack::PopAndDestroy(policyList);              
            }
        }
    CleanupStack::PopAndDestroy(); // ps.close 
    }


// --------------------------------------------------------------------------
// CCmPluginVpn::CanDestUsedAsUnderlyingConnectionL
// --------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::CanDestUsedAsUnderlyingConnectionL( TUint32 aDestinationId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::CanDestUsedAsUnderlyingConnectionL" );

    TUint32 retval(KEmptyDest);
    
    TBool hasNonVirtualCm(EFalse);
    
    CCmDestinationImpl* dest = CmMgr().DestinationL( aDestinationId );
    CleanupStack::PushL( dest );
    
    // is this our parent destination (if there is one)
    CCmDestinationImpl* parentDest = ParentDestination();
    if ( parentDest )
        {
        if ( dest->Id() == parentDest->Id() )
            {
            retval |= KLinksBackToParent;
            }
        }
    delete parentDest;    
    if ( !( retval & KBindDenied ) )
        {
        // filter out:
        //      empty destinations
        //      destinations containing VPN in or linked in
        //      destinations containing emb. dest. if the linked dest is empty
        
        // so if we found a VPN inside, or a VPN linked through an amb. dest, 
        // than it should be filtered out. 
        // it also should be filtered out if it links back 
        // to our parent dest. in any way...
        
        TInt count = dest->ConnectionMethodCount();
        
        if ( count )
            {
            // has at least one CM
            retval &= ~KEmptyDest;
            for ( TInt j = 0; j < dest->ConnectionMethodCount(); j++ )
                {
                CCmPluginBase* cm = dest->GetConnectionMethodL( j );
                CleanupStack::PushL( cm );
                TUint32 cmId = cm->GetIntAttributeL( ECmId );
                
                TInt bearerType = CmMgr().GetConnectionMethodInfoIntL( 
                                        cmId, ECmBearerType );
                if ( !CmMgr().GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                    {
                    // found a non-virtual connection method
                    hasNonVirtualCm = ETrue;
                    }
                else
                    {
                    // now we have to check if it is an emb.dest., if it is, 
                    // check if the referenced destination is not empty,
                    // it has at least one, non-virtual CM, 
                    // OR it has another emb.dest., then recurse it...
                    if ( CmMgr().GetBearerInfoBoolL( bearerType, ECmDestination ) )
                        { // it is an emb. destination
                        // get the emb. destination
                        CCmDestinationImpl* embdest = cm->Destination();
                        TUint32 emdestId = embdest->Id();
                        // check if the embeded destination can be used
                        TUint32 res = CanDestUsedAsUnderlyingConnectionL( emdestId );
                        if ( res & KBindDenied )
                            {
                            // emb. dest. can not be used
                            retval |= (res & KBindDenied );
                            }
                        else
                            {
                            // if binding is not denied, still what to do with 
                            // KEmptyDest & KNoNonVirtuals?
                            // if it does not return KNoNonVirtuals, then it 
                            // has at least one, non-virt. CM
                            if ( res & KNoNonVirtuals )
                                {
                                // if we already have a non-virt, then it is no
                                // problem, no need to change hasNonVirtualCm
                                // as it starts from EFalse.
                                }
                            else
                                {
                                // if it is not empty, than 
                                // it has at least one, non-virt. CM, so:
                                if ( !( res & KEmptyDest ) )
                                    {
                                    hasNonVirtualCm = ETrue;
                                    }
                                }                                
                            // if it is an empty dest., then:
                            // if we still have the base as empty, then 
                            // there is nothing to do.
                            // if the base is non-empty, again, nothing to do 
                            if ( !(res & KEmptyDest ) )
                                { 
                                // not empty dest, so set base 
                                // value to reflect it
                                hasNonVirtualCm = ETrue;
                                }
                            }
                        }
                    else
                        {
                        // virtual, but not emb. dest
                        // check if it is VPN
                        // if it is VPN, 
                        if ( bearerType == KPluginVPNBearerTypeUid )
                            {
                            retval |= KContainsVpn;
                            }
                        }
                    }
                CleanupStack::Pop( cm );
                cm = NULL; // not owned                    
                if ( retval & KBindDenied )
                    { // binding is already denied, we can break...
                    break;
                    }
                }
            if ( !hasNonVirtualCm )
                {
                retval |= KNoNonVirtuals;
                }
            }
        else
            {
            retval |= KEmptyDest;
            }
        }
    CleanupStack::PopAndDestroy( dest );
    
    return retval;
    }



// --------------------------------------------------------------------------
// CCmPluginVpn::BindableDestinationsL
// --------------------------------------------------------------------------
//  
void CCmPluginVpn::BindableDestinationsL( RArray<TUint32>& aDestinationArray )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::BindableDestinationsL" );
    TUint32 upLimit = KCmDefaultDestinationAPTagId + 0xff;
    
    aDestinationArray.Close();
    if ( BindableRealConnectionsExistL() )
        {
        for (TInt i=0; i<iBindableMethods.Count(); i++)
            {
            if ( iBindableMethods[i] <= KDestMaxId ||
                    (iBindableMethods[i] > KCmDefaultDestinationAPTagId && iBindableMethods[i] <= upLimit) )
                {
                aDestinationArray.AppendL( iBindableMethods[i] );
                }
            }
        }
    }


// --------------------------------------------------------------------------
// CCmPluginVpn::HasUpstreamVpnL
// --------------------------------------------------------------------------
//
TBool CCmPluginVpn::HasUpstreamVpnL( TUint32 aId, TBool aIsDestination )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::HasUpstreamVpnL" );
        
    // the function returns true if:
    //  - the destination given is referenced by a CM which is either VPN or 
    //    has an upstream VPN
    //  - the CM given is linked by a VPN or the CM or its destination have 
    //    an upstream VPN
    TBool hasUpstreamVpn( EFalse );

    // array of destinations
    RArray<TUint32> destArray( CmMgr().DestinationCountL() );
    CleanupClosePushL( destArray );
    
    // get all destinations
    CmMgr().AllDestinationsL( destArray );
    
    CLOG_WRITE( "CCmPluginVpn::HasUpstreamVpnL checking destinations..." );
    
    for ( TInt i = 0; !hasUpstreamVpn && i < destArray.Count(); ++i )
        {
        if ( aIsDestination && destArray[i] == aId )
            {
            // connection methods cannot link to their parent destination
            // so we can skip this round
            continue;
            }

        CCmDestinationImpl* dest = CmMgr().DestinationL( destArray[i] );
        CleanupStack::PushL( dest );

        // check out all CM's from destinations
        TInt cmCount = dest->ConnectionMethodCount();
        
        for ( TInt j = 0; j < cmCount; ++j )
            {
            // instantiate a connmethod
            CCmPluginBase* cm = dest->GetConnectionMethodL( j );
            CleanupStack::PushL( cm );

            TUint32 cmId = cm->GetIntAttributeL( ECmId );

            // ask for the bearer type
            TUint32 bearerType = CmMgr().GetConnectionMethodInfoIntL( 
                                                        cmId, ECmBearerType );
            
            // is it virtual? if not, there's nothing to do
            if ( CmMgr().GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                {
                // it is virtual

                if ( CmMgr().GetBearerInfoBoolL( bearerType, ECmDestination ) )
                    {
                    // what's more, it is an embedded destination
                    if ( aIsDestination && cm->IsLinkedToSnap( aId ) )
                        {
                        hasUpstreamVpn = 
                            HasUpstreamVpnL( cmId, EFalse ) ||
                            HasUpstreamVpnL( dest->Id(), ETrue );
                        }
                    }
                else
                    {
                    // it is not an emb.dest.
                    if ( aIsDestination )
                        {
                        // does it link to this destination?
                        if ( cm->IsLinkedToSnap( aId ) )
                            {
                            hasUpstreamVpn = 
                                ( bearerType == KPluginVPNBearerTypeUid ) ||
                                HasUpstreamVpnL( cmId, EFalse ) ||
                                HasUpstreamVpnL( dest->Id(), ETrue );
                            }
                        }
                    else
                        {
                        CCmPluginBase* targetCm = 
                                            CmMgr().GetConnectionMethodL( aId );
                        CleanupStack::PushL( targetCm );
                        
                        // does it link to this CM or to its destination?
                        if ( cm->IsLinkedToIap( aId ) )
                            {
                            hasUpstreamVpn = 
                                ( bearerType == KPluginVPNBearerTypeUid ) ||
                                HasUpstreamVpnL( cmId, EFalse );
                            }
/************** after cmmanager fix **************
    CCmDestinationImpl* parentDest = targetCm->ParentDestination();
                        else if ( parentDest &&
                                  cm->IsLinkedToSnap( 
                                        parentDest->Id() ) )
                            {
                            hasUpstreamVpn = 
                                ( bearerType == KPluginVPNBearerTypeUid ) ||
                                HasUpstreamVpnL( cmId, EFalse ) ||
                                HasUpstreamVpnL( dest->Id(), ETrue );
                            }
     delete parentDest;
***************************************************/
                        else
                            {
                            TUint32 parent( 0 );
                            parent = FindParentDestinationL( aId );
                            
                            if ( parent && cm->IsLinkedToSnap( parent ) )
                                {
                                hasUpstreamVpn = 
                                    ( bearerType == KPluginVPNBearerTypeUid ) ||
                                    HasUpstreamVpnL( cmId, EFalse ) ||
                                    HasUpstreamVpnL( dest->Id(), ETrue );
                                }
                            
                            }

                        CleanupStack::PopAndDestroy( targetCm );
                        }
                    
                    } // if !emb.dest.

                } // if virtual

            CleanupStack::Pop( cm );
            } // for j

        CleanupStack::PopAndDestroy( dest );
        } // for i
    
    CleanupStack::PopAndDestroy( &destArray );
    
    
    // if we already know the answer, jump out here
    if (hasUpstreamVpn) 
        {
        return hasUpstreamVpn;
        } 
        

    CLOG_WRITE( "CCmPluginVpn::HasUpstreamVpnL checking uncategorized group..." );    
    
    // we have to check the uncategorized, too
    
    RArray<TUint32> cmArray ( KCmArrayMediumGranularity );
    CleanupClosePushL( cmArray );
    
    CmMgr().ConnectionMethodL( cmArray );

    // search for virtual CM's
    for ( TInt k = 0; !hasUpstreamVpn && k < cmArray.Count(); ++k )
        {
        TInt bearerType = CmMgr().GetConnectionMethodInfoIntL( cmArray[k], 
                                                               ECmBearerType );
                                                
        if ( CmMgr().GetBearerInfoBoolL( bearerType, ECmVirtual ) )
            {
            // if virtual, does it link to this destination?

            CCmPluginBase* cm = CmMgr().GetConnectionMethodL( cmArray[k] );
            CleanupStack::PushL( cm );
            
            TUint32 cmId = cm->GetIntAttributeL( ECmId );

            if ( aIsDestination )
                {
                // does it link to this destination?
                if ( cm->IsLinkedToSnap( aId ) )
                    {
                    hasUpstreamVpn = 
                        ( bearerType == KPluginVPNBearerTypeUid ) ||
                        HasUpstreamVpnL( cmId, EFalse );
                    }
                }
            else
                {
                // does it link to this CM?           
                if ( cm->IsLinkedToIap( aId ) )
                    {
                    hasUpstreamVpn = 
                        ( bearerType == KPluginVPNBearerTypeUid ) ||
                        HasUpstreamVpnL( cmId, EFalse );
                    }
                     
                }

            CleanupStack::PopAndDestroy( cm );
            } // if virtual
            
        } // for k
    

    CleanupStack::PopAndDestroy( &cmArray );

    return hasUpstreamVpn;
    }
    


// --------------------------------------------------------------------------
// CCmPluginVpn::FilterPossibleParentsL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::FilterPossibleParentsL( RArray<TUint32>& aDests )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::FilterPossibleParentsL" );

    // check if there is any destination into which this VPN could be put
    
    // Filter out invalid destinations
    for ( TInt i = 0; i < aDests.Count(); ++i )
        {
        if ( HasUpstreamVpnL( aDests[i], ETrue ) )
            {
            aDests.Remove( i );
            --i;
            }
        }

    // also filter out the underlying connection's destination
    if ( aDests.Count() )
        {
        // still have work to do
        TUint32 destId = 0;
        
        if ( iVirtualRecord->iNextLayerSNAP )
            {
            // the VPN links to a destination
            destId = iVirtualRecord->iNextLayerSNAP;
            }
        else if ( iVirtualRecord->iNextLayerIAP )
            {
            // the VPN links to a connmethod, find out parent destination id

/********** after CmManager fix ***********************
            CCmPluginBase* cm = CmMgr().GetConnectionMethodL( 
                                               iVirtualRecord->iNextLayerIAP );
            
            CCmDestinationImpl* parentDest = cm->ParentDestination();
            if ( parentDest )
                {
                // take the id only if cm has a parent
                destId = parentDest->Id();
                }
            
            delete cm;
            delete parentDest;
*******************************************************/
            destId = FindParentDestinationL( iVirtualRecord->iNextLayerIAP );
            }

            
        // now search for the id
        if ( destId )
            {
            TInt index = aDests.Find( destId );
            if ( index != KErrNotFound )
                {
                // got it, remove
                aDests.Remove( index );
                }
            }
        
        } // if aDests.Count
        
        
    }


TUint32 CCmPluginVpn::FindParentDestinationL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::FindParentDestinationL" );

    TBool found = EFalse;
    TUint32 targetDestId( 0 ); 
    
    RArray<TUint32> destinations( CmMgr().DestinationCountL() );
   
    CmMgr().AllDestinationsL( destinations );
    CleanupClosePushL( destinations );
    
    for ( TInt i = 0; i < destinations.Count() && !found; i++ )
        {            
        CCmDestinationImpl* dest = 
                          CmMgr().DestinationL( destinations[i] );
        CleanupStack::PushL( dest );

        TInt cmCount = dest->ConnectionMethodCount();

        for ( TInt j = 0; j < cmCount && !found; j++ )
            {
            // instantiate a connmethod
            CCmPluginBase* cm = dest->GetConnectionMethodL( j );
            CleanupStack::PushL( cm );
            TUint32 cmId = cm->GetIntAttributeL( ECmId );

            if ( cmId == aCmId)
                {
                targetDestId = destinations[i];
                found = ETrue;
                }
                
            CleanupStack::Pop( cm );
            }

        CleanupStack::PopAndDestroy( dest );
        }

    CleanupStack::PopAndDestroy( &destinations );
    
    return targetDestId;    
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::DeleteAdditionalRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginVpn::DeleteAdditionalRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginVpn::DeleteAdditionalRecordsL" );    

    // Virtual record must be deleted
    iVirtualRecord->DeleteL( Session() );
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::GenerateVPNPointToEasyWLANLC
// --------------------------------------------------------------------------
//
HBufC* CCmPluginVpn::GenerateVPNPointToWLANNetworkLC(RResourceFile& rf, HBufC*& aName)
    {
    LOGGER_ENTERFN( "CCmPluginVpn::GenerateVPNPointToWLANNetworkLC" );
    aName = NULL;

    HBufC8* readBuffer2 = rf.AllocReadLC( R_QTN_NETW_CONSET_VPN_EASY_WLAN );
    const TPtrC16 ptrReadBuffer2( (TText16*) readBuffer2->Ptr(),
                                 ( readBuffer2->Length() + 1 ) >> 1 );
    aName = HBufC16::NewL( ptrReadBuffer2.Length() );
    *aName = ptrReadBuffer2;
    
    CleanupStack::PopAndDestroy( readBuffer2 );
    CleanupStack::PushL( aName );
    
    return aName;
    }
    
// --------------------------------------------------------------------------
// CCmPluginVpn::CheckValidityAndConvertDestinationIdL
// --------------------------------------------------------------------------
//
TInt CCmPluginVpn::CheckValidityAndConvertDestinationIdL( TUint32 aDestId )
    {
    TInt id( 0 );

    if ( aDestId > 0 && aDestId < 255 )
         {
         id = aDestId + KCmDefaultDestinationAPTagId;
         }
     else if ( aDestId > KCmDefaultDestinationAPTagId 
               && ( aDestId < ( KCmDefaultDestinationAPTagId+255 ) ) )
         {
         id = aDestId;
         }
     else
         {
         User::Leave( KErrArgument );
         }
    
    return id;
    }

// --------------------------------------------------------------------------
// CCmPluginVpn::DefaultAPRecordL
// --------------------------------------------------------------------------
//
TMDBElementId CCmPluginVpn::DefaultAPRecordL( const TInt aTierIdentifier )
    {
    LOGGER_ENTERFN( "CCmPluginVpn::DefaultAPRecordL" );
    // Resolve the Default SNAP AP elementid 
    //
    CCDTierRecord* tierRecord = static_cast<CCDTierRecord *>(
            CCDRecordBase::RecordFactoryL( KCDTIdTierRecord ) );
    
    CleanupStack::PushL( tierRecord );

    // Find the correct tier table based on given identifier.
    //
    tierRecord->iRecordTag = aTierIdentifier;
    if ( !tierRecord->FindL( Session() ) )
        {
        User::Leave( KErrCorrupt );
        }
    
    // Find out the default AP for IP connections.
    //
    TMDBElementId defaultAP = tierRecord->iDefaultAccessPoint;

    // Cleanup tier record.
    //
    CleanupStack::PopAndDestroy( tierRecord );
        
    return defaultAP;
    }

// eof
