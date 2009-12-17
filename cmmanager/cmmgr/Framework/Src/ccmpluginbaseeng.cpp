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
* Description:  Implementation of plugin base class
*
*/
#include <cmmanager.rsg>
#include <es_sock.h>    // RSocketServ, RConnection
#include <in_sock.h>    // KAfInet
#include <es_enum.h>    // TConnectionInfo
#include <StringLoader.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <sysutil.h> 

#include <cmmanager.h>
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpsettingsconsts.h>
#include "cmlogger.h"
#include <cmcommonui.h>

using namespace CMManager;
using namespace CommsDat;

// EXTERNAL LINKS

// MACROS

#undef _IPV6_DISABLED

// CONSTANTS
const TInt KApMaxConnNameLength = 30;
_LIT(KFormatPostfix, "%02d");
_LIT(KFormatLargePostfix, "%d");
_LIT(KFormatNameWithPostfix, "%S(%S)" );
_LIT(KFormatNoPostfix, "%S");
const TInt KMaxPostfixLength = 5;
const TInt KEstimatedOverhead = 8192;

_LIT( KDefWapGatewayIpAddress, "0.0.0.0" );
    
_LIT( KLocationName, "Mobile" );

static const TCmAttribConvTable SIapConvTbl[] = 
    {
    // iAttribId, iCommsDatId, iValidFuncL, 
    // iTitleId, 
    // iMaxLength, iAttribFlags, iDefValueResId, iEditorResId, iNotUsed1, iNotUsed2
    // first element describes the min-max values, does not connects to DB!
    { ECmName, ECmIapId, NULL },
    { ECmName, KCDTIdRecordName, NULL,
        R_CMMANAGERUI_CONNECTION_NAME,
        KMaxConnNameLength, EConvNoZeroLength | EConvCompulsory, 
        0, R_TEXT_SETTING_PAGE_CONN_NAME, NULL, NULL },
    { ECmIapId, KCDTIdRecordTag, NULL },
    { 0, 0, NULL },
    };

static const TCmAttribConvTable SProxyConvTbl[] = 
    {
    // iAttribId, iCommsDatId, iValidFuncL, 
    // iTitleId, 
    // iMaxLength, iAttribFlags, iDefValueResId, iEditorResId, iNotUsed1, iNotUsed2
    { ECmProxyUsageEnabled, ECmProxyRangeMax, NULL },
    { ECmProxyUsageEnabled, KCDTIdUseProxyServer, NULL },
    { ECmProxyServerName,   KCDTIdServerName, &SetProxyServerNameL, 
        R_QTN_SET_PROXY_SERVER_ADDRESS, 
        KMaxProxyServerLength, 0, R_QTN_SET_PROXY_SERVER_ADDRESS_NONE, R_TEXT_SETTING_PAGE_PROXY_SERVER_ADDR, NULL, NULL },
    { ECmProxyProtocolName, KCDTIdProtocolName, NULL },
    { ECmProxyPortNumber,   KCDTIdPortNumber, &CheckPortNumberValidityL,
        R_QTN_SET_PROXY_PORT_NUMBER,
        KMaxProxyPortNumberLength, EConvNumber, 0, 0, NULL, NULL },
    { ECmProxyExceptions,   KCDTIdExceptions, NULL },
    { 0, 0, NULL },
    };

static const TCmAttribConvTable SChargeCardConvTbl[] =
    {
    { ECmChargeCardUsageEnabled, ECmChargeCardRangeMax, NULL },
    { ECmChargeCardAccountNumber, KCDTIdAccountNo, NULL },
    { ECmChargeCardPIN, KCDTIdPin, NULL },
    { ECmChargeCardLocalRule, KCDTIdLocalRule, NULL },
    { ECmChargeCardNatRule, KCDTIdNatRule, NULL },
    { ECmChargeCardIntlRule, KCDTIdIntlRule, NULL },
    { 0, 0, NULL },
    };

static const TCmAttribConvTable SLocationConvTbl[] =
    {
    { ECmLocationUsageEnabled, ECmLocationRangeMax, NULL },
    { ECmLocationIntlPrefixCode, KCDTIdIntlPrefixCode, NULL },
    { ECmLocationNatPrefixCode, KCDTIdNatPrefixCode, NULL },
    { ECmLocationNatCode, KCDTIdNatCode, NULL },
    { ECmLocationAreaCode, KCDTIdAreaCode, NULL },
    { ECmLocationDialOutCode, KCDTIdDialOutCode, NULL },
    { ECmLocationDisableCallWaitingCode, KCDTIdDisableCallWaitingCode, NULL },
    { ECmLocationMobile, KCDTIdMobile, NULL },
    { ECmLocationUsePulseDial, KCDTIdUsePulseDial, NULL },
    { ECmLocationWaitForDialTone, KCDTIdWaitForDialTone, NULL },
    { ECmLocationPauseAfterDialOut, KCDTIdPauseAfterDialOut, NULL },
    { 0, 0, NULL },
    };

static const TCmAttribConvTable SWapConvTbl[] = 
    {
    { ECmWapIPGatewayAddress, ECmWapRangeMax, NULL },
    { ECmWapIPGatewayAddress, KCDTIdWAPGatewayAddress, &CheckIPv4ValidityL },
    { ECmWapIPWSPOption, KCDTIdWAPWSPOption, &CheckWAPWspOptionValidityL },
    { ECmWapIPSecurity, KCDTIdWAPSecurity, NULL },
    { ECmWapIPProxyPort, KCDTIdWAPProxyPort, &CheckPortNumberValidityL },
    { ECmWapIPProxyLoginName, KCDTIdWAPProxyLoginName, NULL },
    { ECmWapIPProxyLoginPass, KCDTIdWAPProxyLoginPass, NULL },
    { 0, 0, NULL },
    };

static const TCmAttribConvTable SUiTable[] = 
    {
    { ECmName, 0, NULL,
        R_CMMANAGERUI_CONNECTION_NAME,
        KMaxConnNameLength, EConvNoZeroLength | EConvCompulsory, 0, R_TEXT_SETTING_PAGE_CONN_NAME, NULL, NULL },
    { ECmBearerSettingName, 0, NULL,
        R_PLUGIN_BASE_SET_AP_BEARER,
        0, 0, 0, 0, NULL, NULL },
    { ECmStartPage, 0, NULL,
        R_QTN_SET_STARTING_PAGE,
        KMaxStartPageLength, 0, R_QTN_SET_HOME_PAGE_NONE, R_TEXT_SETTING_PAGE_STARTING_PAGE, 
        R_START_PAGE_DEF_VALUE, NULL, NULL },
    { 0, 0, NULL,
        0, 
        0, 0, 0, NULL, NULL },
    };

/**
 * Private data of plugin base
 */
NONSHARABLE_CLASS( CCmPluginBasePrivate ) : public CBase
    {
    public:
    
        CCmPluginBasePrivate();
        ~CCmPluginBasePrivate();
    
    public:
    
        CCDWAPAccessPointRecord*    iWapAPRecord;
        CCDWAPIPBearerRecord*       iWapIPBearerRecord;
        CCDIAPMetadataRecord*       iMetaDataRecord;
        
        TNamingMethod   iNamingMethod;
        
        CArrayPtrFlat<TCmAttrConvArrayItem>*        iConvTblArray;
        RArray<const TCmCommonAttrConvArrayItem*>   iCommonConvTblArray;
        
        TInt    iLoadResult;
    };    

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ReadOnlyAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool ReadOnlyAttributeL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute,
                                  const TAny* aValue )
    {
    User::Leave( KErrNotSupported );

    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CheckIPv4ValidityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CheckIPv4ValidityL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute,
                                  const TAny* aValue )
    {
    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CheckIPv6ValidityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CheckIPv6ValidityL( CCmPluginBaseEng* aThis, 
                                  TUint32 aAttribute,
                                  const TAny* aValue )
    {
    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CheckPortNumberValidityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CheckPortNumberValidityL( CCmPluginBaseEng* aThis, 
                                        TUint32 aAttribute,
                                        const TAny* aValue )
    {
    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CheckRecordIdValidityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CheckRecordIdValidityL( CCmPluginBaseEng* aThis, 
                                      TUint32 aAttribute,
                                      const TAny* aValue )
    {
    TUint32 recId = *(TUint32*)aValue;
    
    if( recId < 1 || recId > 254 )
        {
        CLOG_WRITE_1_PTR( aThis, "Not valid Rec id: [%d]", recId );
        User::Leave( KErrArgument );
        }

    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CheckWAPWspOptionValidityL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CheckWAPWspOptionValidityL( CCmPluginBaseEng* aThis, 
                                          TUint32 aAttribute,
                                          const TAny* aValue )
    {
    TUint32 wspOpt = (TUint32)aValue;
    
    
    if( wspOpt != ECmWapWspOptionConnectionless &&
        wspOpt != ECmWapWspOptionConnectionOriented )
        {
        User::Leave( KErrArgument );
        }

    (void)aThis;
    (void)aAttribute;
    (void)aValue;
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// IsUnspecifiedIPv4Address
// ---------------------------------------------------------------------------
//
EXPORT_C TBool IsUnspecifiedIPv4Address( const TDesC& aIpv4Address )
    {
    if( aIpv4Address.Length() == 0 ||
        !aIpv4Address.CompareF( KUnspecifiedIPv4 ) )
        {
        return ETrue;
        }
        
    return EFalse;
    }

// ---------------------------------------------------------------------------
// ClassifyIPv6Address
// ---------------------------------------------------------------------------
//
EXPORT_C TIPv6Types ClassifyIPv6Address( const TDesC& aIpv6Address )
    {
    if( aIpv6Address.Length() == 0 ||
        !aIpv6Address.CompareF( KDynamicIpv6Address ) )
        {
        return EIPv6Unspecified;
        }
    else if( !aIpv6Address.CompareF( KKnownIp6NameServer1 ) ||
             !aIpv6Address.CompareF( KKnownIp6NameServer2 ) )
        {
        return EIPv6WellKnown;
        }
        
    return EIPv6UserDefined;
    }

// ---------------------------------------------------------------------------
// SetProxyServerNameL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool SetProxyServerNameL( CCmPluginBaseEng* aThis, 
                                   TUint32 /*aAttribute*/, 
                                   const TAny* aValue )
    {
    const TDesC* proxyServer = (const TDesC*)aValue;
    
    return aThis->SetProxyServerNameL( *proxyServer );
    }
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TCmPluginInitParam::TCmPluginInitParam
// ---------------------------------------------------------------------------
//
TCmPluginInitParam::TCmPluginInitParam( CCmManagerImpl& aCmMgr )
    : iCmMgr( aCmMgr )
    {
    iParentDest = 0;
    iNotused1 = NULL;
    iNotused2 = NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginBasePrivate::CCmPluginBasePrivate
// ---------------------------------------------------------------------------
//
CCmPluginBasePrivate::CCmPluginBasePrivate()
    : iNamingMethod( ENamingUnique )
    , iCommonConvTblArray( KDefAttrsArrayGranSize )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginBasePrivate::~CCmPluginBasePrivate
// ---------------------------------------------------------------------------
//
CCmPluginBasePrivate::~CCmPluginBasePrivate()
    {
    delete iWapAPRecord;
    delete iWapIPBearerRecord;
    delete iMetaDataRecord;
    
    if ( iConvTblArray )
        {
        iConvTblArray->ResetAndDestroy();
        delete iConvTblArray;
        }
        
    iCommonConvTblArray.Reset();
    iCommonConvTblArray.Close();
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CCmPluginBaseEng
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng::CCmPluginBaseEng( TCmPluginInitParam *aInitParam )
    : iCmMgr( aInitParam->iCmMgr )
    , iParentDest( aInitParam->iParentDest )
    {
    CLOG_CREATE;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::~CCmPluginBaseEng
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng::~CCmPluginBaseEng()
    {
    Reset();
    
    delete iPriv;
    
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::ConstructL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::ConstructL" );

    iPriv = new (ELeave) CCmPluginBasePrivate;
    
    // 4 for our tables + 4 for bearer specific ones.
    iPriv->iConvTblArray = new (ELeave)CArrayPtrFlat<TCmAttrConvArrayItem>( 8 );

    AddConverstionTableL( (CCDRecordBase**)&iIapRecord, 
                          NULL, SIapConvTbl );
    AddConverstionTableL( (CCDRecordBase**)&iProxyRecord, 
                          NULL, SProxyConvTbl );
    AddConverstionTableL( (CCDRecordBase**)&iChargeCardRecord, 
                          &iChargeCardEnabled, SChargeCardConvTbl );
    AddConverstionTableL( (CCDRecordBase**)&iLocationRecord, 
                          &iLocationEnabled, SLocationConvTbl );
    AddConverstionTableL( (CCDRecordBase**)&iPriv->iWapIPBearerRecord, 
                          NULL, SWapConvTbl );
    }


// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetIntAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 
            CCmPluginBaseEng::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetIntAttributeL" );

    TUint32 retVal( 0 );
    
    switch ( aAttribute )
        {
        case ECmBearerIcon:
            {
            retVal = 0; // bearer doesn't provide any icon
            }
            break;
            
        case ECmBearerAvailableIcon:            
            {
            retVal = 0; // bearer doesn't provide any icon
            }
            break;
            
        case ECmId: // if the CM has an IapId than the ECmId is the ECmId
        case ECmIapId:
            {
            retVal = iIapId;
            }
            break;
        case ECmWapId:
            {
            retVal = iPriv->iWapIPBearerRecord->iWAPAccessPointId;
            }
            break;
        case ECmIapServiceId:
            {
            retVal = TUint32(iIapRecord->iService);
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
            retVal = iPriv->iNamingMethod;
            }
            break;
            
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
            
        case ECmInvalidAttribute:
            {
            retVal = iInvalidAttribute;
            }
            break;
            
        case ECmCommsDBBearerType:
            {
            User::Leave( KErrUnknown );
            }
            break;
            
        case ECmSeamlessnessLevel:
            {
            retVal = iPriv->iMetaDataRecord->iSeamlessness;
            }
            break;
            
        case ECmElementID:
            {
            if ( !iIapRecord )
                {
                User::Leave( KErrNotReady );
                }
                
            retVal = (KCDMaskShowField & iIapRecord->ElementId());
            }
            break;
            
        case ECmLoadResult:
            {
            retVal = iPriv->iLoadResult;
            }
            break;
            
        case ECmNetworkId:
            {
            retVal = iIapRecord->iNetwork;
            }
            break;
            
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );            
            CMDBField<TUint32>* field;

            FindFieldL( attribute, ECmInt, (CMDBElement*&)field );

            retVal = *field;        
            }
            break;
        }
        
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetBoolAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool 
        CCmPluginBaseEng::GetBoolAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetBoolAttributeL" );

    TBool retVal;

    switch ( aAttribute )
        {
        case ECmProtected:
            {
            retVal = IsProtected();
            }
            break;
            
        case ECmHidden:
            {
            retVal = iIapRecord->Attributes() & ECDHidden;
            }
            break;

        case ECmDestination:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmChargeCardUsageEnabled:
            {
            retVal = iChargeCardEnabled;
            }
            break;
            
        case ECmCoverage:
            // In default the plugin has no network coverage
            {
            retVal = EFalse;
            }
            break;
            
        case ECmAddToAvailableList:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmBearerHasUi:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmMetaHighlight:
            {
            retVal = iPriv->iMetaDataRecord->iMetadata & EMetaHighlight;
            }
            break;
            
        case ECmMetaHiddenAgent:
            {
            retVal = iPriv->iMetaDataRecord->iMetadata & EMetaHiddenAgent;
            }
            break;
            
        case ECmIPv6Supported:
            {
            retVal = FeatureSupported( KFeatureIdIPv6 );
            }
            break;
            
        case ECmConnected:
            {
            retVal = CheckIfAlreadyConnected();
            }
            break;

        case ECmVirtual:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmIsLinked:
            {
            retVal = iCmMgr.IsIapLinkedL( *this );
            }
            break;
            
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );            
            CMDBField<TBool>* field;

            FindFieldL( attribute, ECmBool, (CMDBElement*&)field );

            retVal = *field;        
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetStringAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* 
    CCmPluginBaseEng::GetStringAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetStringAttributeL" );

    HBufC* retVal = NULL;
    
    switch ( aAttribute )
        {
        case ECmStartPage:
            {
            retVal = TPtrC(iPriv->iWapAPRecord->iWAPStartPage).AllocL();
            }
            break;
            
        case ECmName:
            {
            retVal = TPtrC(iIapRecord->iRecordName).AllocL();
            }
            break;

        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );
            CMDBField<TDesC>* field;
            
            FindFieldL( attribute, ECmText, (CMDBElement*&)field );

            if ( !field->IsNull() )
                {
                retVal = field->GetL().AllocL();
                }
            else
                {
                retVal = KNullDesC().AllocL();
                }
            }
        }
        
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetString8AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* 
    CCmPluginBaseEng::GetString8AttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetString8AttributeL" );

    HBufC8* retVal = NULL;
    
    switch ( aAttribute )
        {
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );
            CMDBField<TDesC8>* field;
                        
            FindFieldL( attribute, ECmText8, (CMDBElement*&)field );

            if ( !field->IsNull() )
                {
                retVal = field->GetL().AllocL();
                }
            else
                {
                retVal = KNullDesC8().AllocL();
                }
            }
        }
        
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetIntAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::SetIntAttributeL( const TUint32 aAttribute, 
                                                  TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetIntAttributeL" );

    switch ( aAttribute )
        {
        case ECmNamingMethod:
            {
            iPriv->iNamingMethod = (TNamingMethod)aValue;
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
            if ( aValue == ESeamlessnessConfirmFirst ||
                 aValue == ESeamlessnessShowprogress )
                {
                iPriv->iMetaDataRecord->iSeamlessness = aValue;
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
           
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );
            CMDBField<TUint32>* field;
            
            TValidationFunctionL func = 
                        FindFieldL( attribute, ECmInt, (CMDBElement*&)field );
            
            if( func )
                {
                func( this, attribute, (TAny*)aValue );
                }
                
            *field = aValue;
            }
            break;
        }
        
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetBoolAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::SetBoolAttributeL( const TUint32 aAttribute, 
                                                   TBool aValue )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetBoolAttributeL" );

    switch ( aAttribute )
        {
        case ECmProtected:
            {
            SetAttribute( iIapRecord, ECDProtectedWrite, aValue );
            }
            break;
            
        case ECmHidden:
            {
            // Check that this is not Default Connection
            TCmDefConnValue defConn;
            iCmMgr.ReadDefConnL( defConn );
            if ( defConn.iType == ECmDefConnConnectionMethod &&
                 defConn.iId == iIapRecord->RecordId() )
                {
                User::Leave( KErrArgument );
                }
            
            SetAttribute( iIapRecord, ECDHidden, aValue );
            }
            break;
            
        case ECmProxyUsageEnabled:
            {
            EnableProxyL( aValue );
            }
            break;
            
        case ECmChargeCardUsageEnabled:
            {
            EnableChargeCardL( aValue );
            }
            break;
            
        case ECmLocationUsageEnabled:
            {
            EnableLocationL( aValue );
            }
            break;
            
        case ECmMetaHighlight:
            {
            if( aValue )
                {
                iPriv->iMetaDataRecord->iMetadata =
                          EMetaHighlight | iPriv->iMetaDataRecord->iMetadata;
                }
            else
                {
                iPriv->iMetaDataRecord->iMetadata =
                        ~EMetaHighlight & iPriv->iMetaDataRecord->iMetadata;
                }
            }
            break;
            
        case ECmMetaHiddenAgent:
            {
            if( aValue )
                {
                iPriv->iMetaDataRecord->iMetadata =
                        EMetaHiddenAgent | iPriv->iMetaDataRecord->iMetadata;
                }
            else
                {
                iPriv->iMetaDataRecord->iMetadata = 
                      ~EMetaHiddenAgent & iPriv->iMetaDataRecord->iMetadata;
                }
            }
            break;
            
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );
            CMDBField<TBool>* field;
            
            TValidationFunctionL func = 
                        FindFieldL( attribute, ECmBool, (CMDBElement*&)field );
            
            if( func )
                {
                func( this, attribute, (const TAny*)aValue );
                }
                

            *field = aValue;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetStringAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::SetStringAttributeL( const TUint32 aAttribute, 
                                                     const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetStringAttributeL" );

    TUint32 commonattribute = MappedCommonAttribute( aAttribute );
    switch ( commonattribute )
        {
        case ECmStartPage:
            {
            iPriv->iWapAPRecord->iWAPStartPage.SetL( aValue );
            }
            break;
            
        case ECmName:
            {
            SetNameL( aValue );
            }
            break;
        case ECmIPAddress:
            {
            GenericStringWriterL( commonattribute, aValue );
            if (( aValue.Compare(KUnspecifiedIPv4) == 0) || ( aValue.Compare(KNullDesC)==0 ) )
                {
                SetBoolAttributeL( ECmIPAddFromServer, ETrue );
                }
            else
                {
                SetBoolAttributeL( ECmIPAddFromServer, EFalse );
                }
            }
            break;
        default:
            {
            GenericStringWriterL( commonattribute, aValue );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetString8AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::SetString8AttributeL( const TUint32 aAttribute, 
                                                      const TDesC8& aValue )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetString8AttributeL" );

    switch ( aAttribute )
        {
        default:
            {
            TUint32 attribute = CheckForCommonAttribute( aAttribute );
            CMDBField<TDesC8>* field;
            
            TValidationFunctionL func = 
                        FindFieldL( attribute, ECmText8, (CMDBElement*&)field );
            
            if( func )
                {
                func( this, attribute, (const TAny*)&aValue );
                }

            field->SetL( aValue );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::RestoreAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::RestoreAttributeL( const TUint32 aAttribute )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::RestoreAttributeL" );

    TCmAttrConvArrayItem* item = ConversionTable( aAttribute );

    TInt index( 0 );
    CMDBElement& field = FieldByAttributeL( item, aAttribute, index );
    
    TRAPD( err, field.LoadL( Session() ) );
    if( err == KErrNotFound )
        {
        switch ( field.Type() )
            {
            case EText:
            case EMedText:
            case ELongText:
                {
                CMDBField<TDesC>* textField = static_cast<CMDBField<TDesC>*>( &field );
                
                textField->SetL( KNullDesC );
                }
                break;

            case EDesC8:
                {
                CMDBField<TDesC8>* textField = static_cast<CMDBField<TDesC8>*>( &field );
                
                textField->SetL( KNullDesC8 );
                }
                break;
                
            case EUint32:
            case EInt:
            case EBool:
                {
                CMDBField<TInt>* intField = static_cast<CMDBField<TInt>*>( &field );
                
                intField->SetL( 0 );
                }
                break;
                
            default:
                {
                User::Leave( KErrArgument );
                }
                break;
            }
        }
    else
        {
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::Destination
// ---------------------------------------------------------------------------
//
EXPORT_C CCmDestinationImpl* CCmPluginBaseEng::Destination() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::Destination" );

    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::UpdateL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateL" );

    CheckSpaceBelowCriticalLevelL();

    OpenTransactionLC();
    
    PrepareToUpdateRecordsL();
    
    UpdateChargeCardRecordL();
    UpdateServiceRecordL();
    
    UpdateLocationRecordL();
    UpdateNetworkRecordL();
    
    UpdateIAPRecordL();

    UpdateWapRecordL();
    UpdateProxyRecordL();
    UpdateSeamlessnessRecordL();
    UpdateConnPrefSettingL();
    
    UpdateAdditionalRecordsL();
    
    CommitTransactionL( 0 );
    iCmMgr.ConnMethodUpdatedL( this );
    iIdIsValid = ETrue;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::NumOfConnMethodReferencesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmPluginBaseEng::NumOfConnMethodReferencesL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::NumOfConnMethodReferencesL" );

    return iCmMgr.NumOfConnMethodReferencesL( iIapId );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::Session
// ---------------------------------------------------------------------------
//
EXPORT_C CMDBSession& CCmPluginBaseEng::Session()const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::Session" );

    return iCmMgr.Session();
    }

// ---------------------------------------------------------------------------
// Delete this connection methond.
// Record ids are checked to be sure that we delete only records
// that were loaded before.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmPluginBaseEng::DeleteL( TBool aForced,
                                          TBool aOneRefAllowed )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::DeleteL" );
    
    if ( CheckIfAlreadyConnected() )
        {
        User::Leave( KErrInUse );
        }

    if( iPriv->iLoadResult )
        // previous load failed -> do NOT delete this partially loaded cm.
        {
        User::Leave( KErrNotReady );
        }
        
    if( GetBoolAttributeL( ECmIsLinked ) )
        {
        User::Leave( KErrLocked );
        }
        
    // If there is no parent destination, we can delete
    TBool retVal(ETrue);
    
    if( !aForced )
        {
        TInt refs = NumOfConnMethodReferencesL();
        
        switch ( refs )
            {
            case 0:
                // no referencies to this cm
                {
                CLOG_WRITE( "No ref -> do delete it" );
                }
                break;
                
            case 1: 
                // one reference
                {
                if ( !aOneRefAllowed )
                    // this is not a legacy cm -> cannot be deleted.
                    {
                    retVal = EFalse;
                    }
                }
                break;
                
            default:
                // connection method is multiple referenced from
                // destinations -> no delete
                {
                retVal = EFalse;
                }
                break;
            }
        }
        
    if ( retVal )
        {
        TCmDefConnValue deletedItem;
        deletedItem.iType = ECmDefConnConnectionMethod;
        deletedItem.iId = iIapId;
        iCmMgr.HandleDefConnDeletedL( deletedItem );       

        if( aForced )
            {
            iCmMgr.RemoveAllReferencesWoTransL( *this );
            }

        OpenTransactionLC();
        // And now we have everything from CommsDat, 
        // so we delete only those records.
        PrepareToDeleteRecordsL();

        if ( iIapRecord && iIapRecord->RecordId() )
            {
            iIapRecord->DeleteL( Session() );
            }
        
        if ( iProxyRecord && iProxyRecord->RecordId() )
            {
            iProxyRecord->DeleteL( Session() );
            }
            
        if ( iServiceRecord && iServiceRecord->RecordId() )
            {
            iServiceRecord->DeleteL( Session() );
            }
        
        if ( iChargeCardRecord && iChargeCardRecord->RecordId() )
            {
            iChargeCardRecord->DeleteL( Session() );
            }
        
        if ( iNetworkRecord && iNetworkRecord->RecordId() )
            {
            iNetworkRecord->DeleteL( Session() );
            }
        
        if ( iLocationRecord && iLocationRecord->RecordId() )
            {
            iLocationRecord->DeleteL( Session() );
            }
        
        if ( iPriv->iWapAPRecord && 
            iPriv->iWapAPRecord->RecordId() )
            {
            iPriv->iWapAPRecord->DeleteL( Session() );
            }
        
        if ( iPriv->iWapIPBearerRecord && 
            iPriv->iWapIPBearerRecord->RecordId() )
            {
            iPriv->iWapIPBearerRecord->DeleteL( Session() );
            }

        if ( iPriv->iMetaDataRecord &&
             iPriv->iMetaDataRecord->RecordId() )
            {
            iPriv->iMetaDataRecord->DeleteL( Session() );
            }
        
        DeleteAdditionalRecordsL();

        CommitTransactionL( KErrNone );
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::LoadL( TUint32 aIapId )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadL" );
    CLOG_NAME_2( _L("plugin_0x%x_%d"), this, aIapId );

    OpenTransactionLC();

    // trap it to set iPriv->iLoadResult
    TRAP( iPriv->iLoadResult, DoLoadL( aIapId ) );

    User::LeaveIfError( iPriv->iLoadResult );
    
    RollbackTransaction();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CreateNewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::CreateNewL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CreateNewL" );

    CheckSpaceBelowCriticalLevelL();
    
    if ( iIapId )
        // we already have IAP id ->
        {
        User::Leave( KErrAlreadyExists );
        }

    iPreDefIapId = 0;

    // optional record are disable in default
    iChargeCardEnabled = EFalse;
    iLocationEnabled = EFalse;
    
    // create mandatory records
    iIapRecord = static_cast<CCDIAPRecord *>
                          (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    iNetworkRecord = static_cast<CCDNetworkRecord *>
                          (CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord));
    iProxyRecord = static_cast<CCDProxiesRecord *>
                  (CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
    NewWapRecordL();
    iPriv->iWapIPBearerRecord->iWAPGatewayAddress.SetL( 
                                                    KDefWapGatewayIpAddress );
    
    iPriv->iMetaDataRecord = NewSeamlessnessRecordL( ETrue );

    // call plugin to create its own records
    CreateNewServiceRecordL();
    CreateAdditionalRecordsL();

    EnableProxyL( EFalse );

    // Load default CM name from resource
    HBufC* defApName = AllocReadL( R_CMMANAGERENG_DEFAULT_AP_NAME );
    CleanupStack::PushL( defApName );

    SetNameL( *defApName );

    CleanupStack::PopAndDestroy( defApName );

    iIapRecord->iNetworkWeighting = 0; // it's always 0.
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CreateCopyL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng* CCmPluginBaseEng::CreateCopyL() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CreateCopyL" );
    CheckSpaceBelowCriticalLevelL();
    
    TCmPluginInitParam params( iCmMgr );
    
    CCmPluginBaseEng* copyInst = CreateInstanceL( params );
    CleanupStack::PushL( copyInst );
    
    copyInst->CreateNewL();

    TInt err( KErrNone );
    TRAP( err, copyInst->SetBoolAttributeL( ECmChargeCardUsageEnabled, iChargeCardEnabled ) );
    if( err != KErrNotSupported )
        {
        User::LeaveIfError( err );
        }
    TRAP( err, copyInst->SetBoolAttributeL( ECmLocationUsageEnabled, iLocationEnabled ) );
    if( err != KErrNotSupported )
        {
        User::LeaveIfError( err );
        }
    copyInst->SetIntAttributeL( ECmNamingMethod, iPriv->iNamingMethod );
    copyInst->SetStringAttributeL( ECmName, TPtrC( iIapRecord->iRecordName ) );
    copyInst->SetStringAttributeL( ECmStartPage, TPtrC( iPriv->iWapAPRecord->iWAPStartPage ) );


    PrepareToCopyDataL( *copyInst );

    for ( TInt tblInd = 0; tblInd < iPriv->iConvTblArray->Count(); ++tblInd )
        {
        const TCmAttrConvArrayItem* item = (*iPriv->iConvTblArray)[tblInd];
        
        if ( item->iEnabled && !(*item->iEnabled) )
            // Enabled flag is set to 'disabled' state
            {
            continue;
            }
            
        if( !item->iRecord || 
            (item->iRecord && !(*item->iRecord)) )
            // no record
            {
            continue;
            }
                
        for ( TInt attrInd = 1; item->iConvTable[attrInd].iAttribId; ++attrInd )
            {
            if( item->iConvTable[attrInd].iAttribFlags & EConvReadOnly )
                // Read-only flags are not copied.
                {
                continue;
                }
                
            TInt commsdatid( item->iConvTable[attrInd].iCommsDatId );

            if ( !commsdatid )
                { // not connected to the DB, calculated field, no copy
                continue;
                }
                
            CMDBElement* field = (*item->iRecord)->GetFieldByIdL( commsdatid );
    
            if( field->IsNull() )
                {
                continue;
                }
                
            switch ( field->Type() )
                {
                case EText:
                case EMedText:
                case ELongText:
                    {
                    CMDBField<TDesC>* textField = (CMDBField<TDesC>*)field;
                
                    TRAP( err, copyInst->SetStringAttributeL( 
                                            item->iConvTable[attrInd].iAttribId,
                                            *textField ) );
                    if( err != KErrNotSupported )
                        {
                        User::LeaveIfError( err );
                        }
                    }
                    break;
                    
                case EDesC8:
                    {
                    CMDBField<TDesC8>* textField = (CMDBField<TDesC8>*)field;
                
                    TRAP( err, copyInst->SetString8AttributeL( 
                                            item->iConvTable[attrInd].iAttribId,
                                            *textField ) );
                    if( err != KErrNotSupported )
                        {
                        User::LeaveIfError( err );
                        }
                    }
                    break;
                    
                case EUint32:
                case EInt:
                    {
                    CMDBField<TUint32>* intField = (CMDBField<TUint32>*)field;
                    
                    TRAP( err, copyInst->SetIntAttributeL( 
                                            item->iConvTable[attrInd].iAttribId,
                                            *intField ) );
                    if ( !(( err == KErrNotSupported ) || ( err == KErrArgument )) )
                        {
                        User::LeaveIfError( err );
                        }
                    }
                    break;
                    
                case EBool:
                    {
                    CMDBField<TBool>* intField = (CMDBField<TBool>*)field;
                    
                    TRAP( err, copyInst->SetBoolAttributeL( 
                                            item->iConvTable[attrInd].iAttribId,
                                            *intField ) );
                    if( err != KErrNotSupported )
                        {
                        User::LeaveIfError( err );
                        }
                    }
                    break;
                    
                case ELink: // links are not copied!
                default:
                    {
                    }
                    break;
                }
            }                    
        }

    CopyAdditionalDataL( *copyInst );
    
    CleanupStack::Pop( copyInst );
    
    return copyInst;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::AddResourceFileL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::AddResourceFileL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::AddResourceFileL" );
    
    CLOG_WRITE_1( "[%S]", &aName );
    
    TParse fp;
    TInt err = fp.Set( aName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    if ( err != KErrNone)
        {
        User::Leave( err );
        }
        
    TFileName fName;
    
    fName.Copy( fp.FullName() );

    iCmMgr.AddResourceFileL( fName );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::RemoveResourceFile
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::RemoveResourceFile( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::RemoveResourceFileL" );

    TParse fp;
    TInt err = fp.Set( aName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    CLOG_WRITE_1( "Set: [%d]", err );
        
    TFileName fName;
    
    fName.Copy( fp.FullName() );

    iCmMgr.RemoveResourceFile( fName );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::AllocReadL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CCmPluginBaseEng::AllocReadL( TInt aResourceId ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::AllocReadL" );

    return iCmMgr.AllocReadL( aResourceId );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::OpenTransactionLC
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::OpenTransactionLC()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::OpenTransactionL" );

    iCmMgr.OpenTransactionLC( ETrue );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CommitTransactionL( TInt aError )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CommitTransactionL" );
    
    iCmMgr.CommitTransactionL( aError );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::RollbackTransaction
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::RollbackTransaction()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::RollbackTransaction" );
    
    iCmMgr.RollbackTransaction();
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::PrepareToLoadRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::DoLoadL( TUint32 aIapId )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::DoLoadL" );

    PrepareToLoadRecordsL();
    
    LoadIAPRecordL( aIapId );
    LoadWapRecordL();
    LoadSeamlessnessRecordL();
    LoadNetworkSettingL();
    LoadLocationSettingL();

    // This is a connectionmethodinfo instance, that has no
    // service and proxy setting.
    if( KDummyBearerType != iBearerType )
        {
        LoadServiceSettingL();
        LoadProxySettingL();
        }

    LoadAdditionalRecordsL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::PrepareToLoadRecordsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::PrepareToLoadRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::PrepareToLoadRecordsL" );

    iChargeCardEnabled = EFalse;
    iLocationEnabled = EFalse;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadIAPRecordL( TUint32 aIapId )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadIAPRecordL" );

    iIapId = aIapId;
    
    // Load IAP record
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                            (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
                            
    CleanupStack::PushL( iapRecord );
    
    iapRecord->SetRecordId( iIapId );
    
    iapRecord->LoadL( Session() );
    
    CleanupStack::Pop( iapRecord );
    
    iIapRecord = iapRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadProxySettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadProxySettingL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadProxySettingL" );
    
    // Load Proxy record
    CMDBRecordSet<CCDProxiesRecord>* proxieRS = 
              new(ELeave) CMDBRecordSet<CCDProxiesRecord>(KCDTIdProxiesRecord);
    CleanupStack::PushL(proxieRS);

    // Now try to find the linked proxy record
    // create new record
    CCDProxiesRecord* proxyRecord = static_cast<CCDProxiesRecord *>
                          (CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));

    CleanupStack::PushL( proxyRecord );
    
    // Prime record
    TPtrC serviceType( iIapRecord->iServiceType );
    
    proxyRecord->iServiceType.SetL( serviceType );
    
    proxieRS->iRecords.AppendL( proxyRecord );

    CleanupStack::Pop( proxyRecord );
    
    proxyRecord = NULL;
    
    CLOG_WRITE_1( "Looking for proxy: [%d]", TUint32(iIapRecord->iService) );
    
    if ( proxieRS->FindL(Session()) )
        {
        TInt i(0);
        while( i<proxieRS->iRecords.Count() )
            // Now that we have a list of services with the proper service type 
            // search for our proxy record and remove it from the array, 
            // then destroy the array.
            {
            CCDProxiesRecord* proxyRecord = (*proxieRS)[i];
            
            // Compare the record id of these 2 services
            if ( TUint32(proxyRecord->iService) == 
                TUint32(iIapRecord->iService) )
                {
                iProxyRecord = proxyRecord;
                // take ownership of this record
                proxieRS->iRecords.Remove( i );
                break;
                }
            ++i;
            }
        }
    
    proxieRS->iRecords.ResetAndDestroy();
    
    if( !iProxyRecord )
        {
        // This creates a proxy record, where usage is disabled.
        EnableProxyL( EFalse );
        }

    CleanupStack::PopAndDestroy( proxieRS );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadServiceSettingL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadServiceSettingL" );
    
    if ( iServiceRecord )
        {
        iServiceRecord->SetRecordId( iIapRecord->iService );
        TRAPD( err, iServiceRecord->LoadL( Session() ) );
        if( err == KErrNotFound )
            // record not found -> create a default one
            {
            CreateNewServiceRecordL();
            }
        else
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
        CLOG_WRITE( "CCmPluginBaseEng::LoadServiceSettingL: No service rec" );
        }
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadChargeCardSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadChargeCardSettingL( TUint32 aRecordId )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadChargeCardSettingL" );

    if ( !aRecordId )
        // No charge card is link to this IAP
        {
        iChargeCardEnabled = EFalse;
        
        CLOG_WRITE( "CCmPluginBaseEng::LoadChargeCardSettingL: No charge card" );
        return;
        }
        
    iChargeCardRecord = static_cast<CCDChargecardRecord *>
                       (CCDRecordBase::RecordFactoryL(KCDTIdChargecardRecord));
    iChargeCardRecord->SetRecordId( aRecordId );
    TRAPD( err, iChargeCardRecord->LoadL( Session() ) );
    if( err == KErrNotFound )
        // referenced but doesn't exist. Try to fix it.
        {
        iChargeCardRecord->SetRecordId( KCDNewRecordRequest );
        
        // if it fails we can't do anything to solve this problem
        iChargeCardRecord->StoreL( Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    iChargeCardEnabled = ETrue;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadNetworkSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadNetworkSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadNetworkSettingL" );
    
    CCDNetworkRecord* networkRecord = static_cast<CCDNetworkRecord*>
                          (CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord));
    CleanupStack::PushL( networkRecord );
                       
    networkRecord->SetRecordId( iIapRecord->iNetwork );
    TRAPD( err, networkRecord->LoadL( Session() ) );
    if( err == KErrNotFound )
        {
        CleanupStack::PopAndDestroy( networkRecord );

        networkRecord = static_cast<CCDNetworkRecord*>
                          (CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord));
        }
    else
        // KErrNotFound -> doesn't matter. We will do it UpdateL()
        {
        User::LeaveIfError( err );

        CleanupStack::Pop( networkRecord );
        }

    iNetworkRecord = networkRecord;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadLocationSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadLocationSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadLocationSettingL" );

    CCDLocationRecord* locationRecord = static_cast<CCDLocationRecord*>
                         (CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
         
    CleanupStack::PushL( locationRecord );

    locationRecord->SetRecordId( iIapRecord->iLocation );
    if ( locationRecord->FindL(Session()) )
        {
        iLocationEnabled = ETrue;
        
        iLocationRecord = locationRecord;
        
        CleanupStack::Pop( locationRecord );
        }
    else
        {
        iLocationEnabled = EFalse;

        CleanupStack::PopAndDestroy( locationRecord );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::PrepareToUpdateRecordsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::PrepareToUpdateRecordsL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::PrepareToUpdateRecordsL" );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateIAPRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateIAPRecordL" );

    if ( !iIapId )
        {
        if ( iPreDefIapId )
            {
            iIapRecord->SetRecordId( iPreDefIapId );
            iPreDefIapId = 0;
            }
        else
            {
            iIapRecord->SetRecordId( KCDNewRecordRequest );
            }

        TUint32 id;
        HBufC* name;
        
        ServiceRecordIdLC( name, id );
        iIapRecord->iService = id;
        iIapRecord->iServiceType.SetL( *name );
        CleanupStack::PopAndDestroy( name );
            
        BearerRecordIdLC( name, id );
        iIapRecord->iBearer = id;
        iIapRecord->iBearerType.SetL( *name );
        CleanupStack::PopAndDestroy( name );

        iIapRecord->iNetwork = iNetworkRecord->RecordId();
        
        if ( iLocationRecord )
            {
            iIapRecord->iLocation = iLocationRecord->RecordId();
            }
            
        iIapRecord->iLocation = GetLocationIdL();
        
        iIapRecord->StoreL( Session() );
        iIapId = iIapRecord->RecordId();
        
        CLOG_NAME_2( _L("plugin_0x%x_%d"), this, iIapId );
        CLOG_WRITE_1( "New IapId: [%d]", iIapId );
        }
    else
        {
        iIapRecord->ModifyL( Session() );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateProxyRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateProxyRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateProxyRecordL" );

    CopyAttributes( iIapRecord, iProxyRecord );
    CheckIfNameModifiedL( iProxyRecord );
    
    if ( iProxyRecord->iUseProxyServer )
        {
        if ( !iProxyRecord->RecordId() )
            // new proxy setting -> create new record
            {
            iProxyRecord->iService = iServiceRecord->RecordId();
            iProxyRecord->iServiceType.SetL( iIapRecord->iServiceType );

            // By default protocol is set to "http"
            if ( TPtrC(iProxyRecord->iProtocolName).Length() == 0 )
                {
                iProxyRecord->iProtocolName.SetL(KDefProxyProtocolName);
                }

            iProxyRecord->SetRecordId( KCDNewRecordRequest );

            iProxyRecord->StoreL( Session() );
            }
        else
            // already existing record -> update only
            {
            iProxyRecord->ModifyL( Session() );
            }        
        }
    else
        {
        if ( iProxyRecord->RecordId() )
            {
            iProxyRecord->DeleteL( Session() );
            }
        }
    }


// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateSeamlessnessRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateSeamlessnessRecordL" );

    CopyAttributes( iIapRecord, iPriv->iMetaDataRecord );
    CheckIfNameModifiedL( iPriv->iMetaDataRecord );
    
    if ( !iPriv->iMetaDataRecord->RecordId() )
        {
        iPriv->iMetaDataRecord->iIAP = IAPRecordElementId();
        iPriv->iMetaDataRecord->SetRecordId( KCDNewRecordRequest );
        iPriv->iMetaDataRecord->StoreL( Session() );
        }
    else
        {
        iPriv->iMetaDataRecord->ModifyL( Session() );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateChargeCardRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateChargeCardRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateChargeCardRecordL" );
    if ( iChargeCardEnabled )
        {
        CopyAttributes( iIapRecord, iChargeCardRecord );
        CheckIfNameModifiedL( iChargeCardRecord );
        
        if ( !iChargeCardRecord->RecordId() )
            {
            iChargeCardRecord->SetRecordId( KCDNewRecordRequest );
            iChargeCardRecord->StoreL( Session() );
            }
        else
            {
            iChargeCardRecord->ModifyL( Session() );
            }
        }
    else
        {
        if ( iChargeCardRecord )
            {
            iChargeCardRecord->DeleteL( Session() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::UpdateServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateServiceRecordL" );
    
    if ( iServiceRecord )
        {
        CopyAttributes( iIapRecord, iServiceRecord );
        CheckIfNameModifiedL( iServiceRecord );
        
        if ( !iServiceRecord->RecordId() )
            {
            iServiceRecord->SetRecordId( KCDNewRecordRequest );
            iServiceRecord->StoreL( Session() );
            }
        else
            {
            iServiceRecord->ModifyL( Session() );
            }
        }
    else
        {
        CLOG_WRITE( "No service record" );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateNetworkRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateNetworkRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateNetworkRecordL" );
    
    CopyAttributes( iIapRecord, iNetworkRecord );
    if ( !iNetworkRecord->RecordId() )
        {
        CheckIfNameModifiedL( iNetworkRecord );
        
        iNetworkRecord->SetRecordId( KCDNewRecordRequest );
        iNetworkRecord->StoreL( Session() );
        }
    else
        {
        iNetworkRecord->ModifyL( Session() );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateLocationRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateLocationRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateLocationRecordL" );
    if ( iLocationEnabled )
        {
        CopyAttributes( iIapRecord, iLocationRecord );
        CheckIfNameModifiedL( iLocationRecord );
                        
        if ( !iLocationRecord->RecordId() )
            {
            iLocationRecord->SetRecordId( KCDNewRecordRequest );
            iLocationRecord->StoreL( Session() );
            }
        else
            {
            iLocationRecord->ModifyL( Session() );
            }
        }
    else
        {
        if ( iLocationRecord )
            {
            iLocationRecord->DeleteL( Session() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateConnPrefSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateConnPrefSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateConnPrefSettingL" );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnableProxyL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::EnableProxyL( TBool aEnable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::EnableProxyL" );

    if ( !iServiceRecord )
        {
        CLOG_WRITE( "No service record -> no proxy" );
        User::Leave( KErrNotSupported );
        }

    if( !iProxyRecord )
        {
        iProxyRecord = static_cast<CCDProxiesRecord *>
                      (CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
        }
        
    iProxyRecord->iUseProxyServer = aEnable;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnableChargeCardL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::EnableChargeCardL( TBool aEnable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::EnableChargeCardL" );

    if ( aEnable )
        {
        if ( !iChargeCardRecord )
            {
            iChargeCardRecord = static_cast<CCDChargecardRecord *>
                       (CCDRecordBase::RecordFactoryL(KCDTIdChargecardRecord));
            }
        }
    else
        // charge card is disabled. To be deleted in UpdateL
        // Nothing to do here
        {
        }
        
    iChargeCardEnabled = aEnable;        
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnableLocationL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::EnableLocationL( TBool aEnable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::EnableLocationL" );

    if ( aEnable )
        {
        if ( !iLocationRecord )
            {
            iLocationRecord = static_cast<CCDLocationRecord *>
                         (CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
            }
        }
    else
        // location is disabled. To be deleted in UpdateL.
        // Nothing to do here
        {
        }
        
    iLocationEnabled = aEnable;        
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadWapRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadWapRecordL" );

    iPriv->iWapIPBearerRecord = FindWAPRecordL();
    if ( iPriv->iWapIPBearerRecord )
        {
        CCDWAPAccessPointRecord *wapApRecord = static_cast<CCDWAPAccessPointRecord *>
                   (CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
                   
        CleanupStack::PushL( wapApRecord );
                                        
        wapApRecord->SetRecordId( 
                                iPriv->iWapIPBearerRecord->iWAPAccessPointId );
        TRAPD( err, wapApRecord->LoadL( Session() ) );
        if( err == KErrNotFound )
            {
            CleanupStack::PopAndDestroy( wapApRecord );
            
            wapApRecord = static_cast<CCDWAPAccessPointRecord *>
                   (CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
            }
        else
            {
            CLOG_WRITE( "WAP bearer record found, WAP AP Not??" );

            User::LeaveIfError( err );

            CleanupStack::Pop( wapApRecord );
            }
        
        iPriv->iWapAPRecord = wapApRecord;
        }
    else
        // No WAP record found -> create a new one
        {
        NewWapRecordL();
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::LoadSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::LoadSeamlessnessRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::LoadSeamlessnessRecordL" );

    iPriv->iMetaDataRecord = FindSeamlessnessRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::UpdateWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::UpdateWapRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::UpdateWapRecordL" );
    
    if ( !iPriv->iWapIPBearerRecord )
        // nothing to update
        {
        return;
        }
    
    // we have the bearer but to make sure that we have WAP_AP, too.    
    NewWapRecordL();
    
    CopyAttributes( iIapRecord, iPriv->iWapAPRecord );
    CopyAttributes( iIapRecord, iPriv->iWapIPBearerRecord );
    
    CheckIfNameModifiedL( iPriv->iWapAPRecord );
    CheckIfNameModifiedL( iPriv->iWapIPBearerRecord );

    if ( !iPriv->iWapAPRecord->RecordId() )
        {
        iPriv->iWapAPRecord->SetRecordId( KCDNewRecordRequest );
        iPriv->iWapAPRecord->StoreL( Session() );
        }
    else
        {
        iPriv->iWapAPRecord->ModifyL( Session() );
        }
    
    if ( !iPriv->iWapIPBearerRecord->RecordId() )
        {
        iPriv->iWapIPBearerRecord->iWAPAccessPointId = iPriv->iWapAPRecord->RecordId();
        iPriv->iWapIPBearerRecord->iWAPIAP = iIapId;
        
        iPriv->iWapIPBearerRecord->SetRecordId( KCDNewRecordRequest );
        iPriv->iWapIPBearerRecord->StoreL( Session() );
        }
    else
        {
        iPriv->iWapIPBearerRecord->ModifyL( Session() );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::NewWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::NewWapRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::NewWapBearerRecordL" );

    if ( !iPriv->iWapIPBearerRecord )
        {
        iPriv->iWapIPBearerRecord = static_cast<CCDWAPIPBearerRecord *>
                      (CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord));
        iPriv->iWapIPBearerRecord->iWAPProxyPort = 0;
        iPriv->iWapIPBearerRecord->iWAPWSPOption = ECmWapWspOptionConnectionOriented;
        iPriv->iWapIPBearerRecord->iWAPSecurity = EFalse;
        }

    if ( !iPriv->iWapAPRecord )
        {
        iPriv->iWapAPRecord = static_cast<CCDWAPAccessPointRecord *>
                   (CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
        // SMS bearer is not supported by this version
        iPriv->iWapAPRecord->iWAPCurrentBearer.SetL( 
                                               TPtrC(KCDTypeNameWAPIPBearer) );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::NewSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBaseEng::NewSeamlessnessRecordL( TBool aSetDef )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::NewSeamlessnessRecordL" );

    CCDIAPMetadataRecord* record = new (ELeave) CCDIAPMetadataRecord( 
                                          iCmMgr.SeamlessnessTableId() );
                                          
    if( aSetDef )
        {
        record->iSeamlessness = ESeamlessnessConfirmFirst;
        }
        
    return record;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetAttribute
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::SetAttribute( CCDRecordBase* aRecord, 
                                     TUint32 aAttribute, 
                                     TBool aSet )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetAttribute" );
   
    if ( aSet )
        {
        aRecord->SetAttributes( aAttribute );
        }
    else
        {
        aRecord->ClearAttributes( aAttribute );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CopyAttributes
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::CopyAttributes( CCDRecordBase* aSrcRecord, 
                                       CCDRecordBase* aDstRecord )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CopyAttributes" );

    TMDBAttributeFlags attr( aSrcRecord->Attributes() );
    
    aDstRecord->SetAttributes( attr );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FindWAPRecordL
// ---------------------------------------------------------------------------
//
CCDWAPIPBearerRecord* CCmPluginBaseEng::FindWAPRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::FindWAPRecordL" );

    CMDBRecordSet<CCDWAPIPBearerRecord>* wapRS = 
            new(ELeave) 
                   CMDBRecordSet<CCDWAPIPBearerRecord>(KCDTIdWAPIPBearerRecord);
    CleanupStack::PushL( wapRS );
    
    CCDWAPIPBearerRecord* wapBearerRecord = static_cast<CCDWAPIPBearerRecord *>
                      (CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord));
                      
    CleanupStack::PushL( wapBearerRecord );
    
    wapBearerRecord->iWAPIAP = iIapId;
    wapRS->iRecords.AppendL( wapBearerRecord );
    
    CleanupStack::Pop( wapBearerRecord );
    wapBearerRecord = NULL;
    
    if ( wapRS->FindL(Session()) )
        {
        CLOG_WRITE( "WAP bearer record found" );
        
        wapBearerRecord = 
                        static_cast<CCDWAPIPBearerRecord *>(wapRS->iRecords[0]);

        // we take over the ownership of this record
        wapRS->iRecords.Remove( 0 );
        }

    CleanupStack::PopAndDestroy( wapRS );
        
    return wapBearerRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FindSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBaseEng::FindSeamlessnessRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::FindSeamlessnessRecordL" );

    CMDBRecordSet<CCDIAPMetadataRecord>* slnRS = 
      new(ELeave) CMDBRecordSet<CCDIAPMetadataRecord>(iCmMgr.SeamlessnessTableId());
    CleanupStack::PushL( slnRS );
    
    CCDIAPMetadataRecord* slnRecord = 
            new (ELeave) CCDIAPMetadataRecord( iCmMgr.SeamlessnessTableId() );
                                                                  
    slnRecord->iIAP = IAPRecordElementId();
    slnRS->iRecords.AppendL( slnRecord );
    
    slnRecord = NULL;
    
    if ( slnRS->FindL(Session()) )
        {
        CMDBRecordBase* record = slnRS->iRecords[0];

        slnRecord = NewSeamlessnessRecordL( EFalse );
        slnRecord->SetRecordId( record->RecordId() );
        
        // This can leave only in case of OOM.
        slnRecord->LoadL( Session() );
        }
    else
        {
        slnRecord = NewSeamlessnessRecordL( ETrue );
        }

    CleanupStack::PopAndDestroy( slnRS );
        
    return slnRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::DoMakeValidNameL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::DoMakeValidNameL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::DoMakeValidNameL" );

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
        valid = IsValidNameL( *temp );
        if ( !valid )
            {
            changed = ETrue;
            postfix++;
            // check the length of postfix, check text length accordingly
            pf = postfix;
            for (i=1; i<10; i++)
                {
                pf /= 10;
                if ( !pf )
                    break;
                }
            TPtr sgdptr( temp->Des() );
            TPtr sgdptr2( temp2->Des() );
            if ( postfix )
                {
                if ( postfix < 10 )
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
                TBuf< KMaxPostfixLength > postfixString;
                if ( postfix > 9 )
                    {
                    postfixString.Format( KFormatLargePostfix, postfix );
                    AknTextUtils::LanguageSpecificNumberConversion( postfixString );
                    }
                else
                    {
                    postfixString.Format( KFormatPostfix, postfix );
                    AknTextUtils::LanguageSpecificNumberConversion( postfixString );
                    }
                sgdptr.Format( KFormatNameWithPostfix, &sgdptr2,
                                   &postfixString );
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
        CLOG_WRITE_1( "New name: [%S]", temp );
        CleanupStack::Pop( temp );
        }
    else
        {
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }

    return temp;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EnsureMaxLengthLC
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::EnsureMaxLengthLC( const TDesC& aName, 
                                            TBool& aChanged )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::EnsureMaxLengthLC" );

    TInt length = aName.Length();
    
    aChanged = EFalse;
    
    if ( !length )
        {
        // Name is required.        
        User::Leave( KErrArgument );
        }

    HBufC* corrname;
    if ( KApMaxConnNameLength < length )
        { // name too long, truncate.
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
        // comes here only if name is valid
        if ( corrname->Length() != aName.Length() )
            {
            aChanged = ETrue;
            }
        }

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
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetPrefix" );

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
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetPostfix" );
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
                    AknTextUtils::ConvertDigitsTo( pf, EDigitTypeWestern );
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
    return postfix;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IsValidNameL
// ---------------------------------------------------------------------------
//
TBool CCmPluginBaseEng::IsValidNameL( const TDesC& aNameText )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::IsValidNameL" );

    TBool retVal( ETrue );
    
    OpenTransactionLC();
    
    CMDBRecordSet<CCDIAPRecord>* iapRS = 
                      new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
    CleanupStack::PushL(iapRS);

    CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord *>
                            (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));

    CleanupStack::PushL( iapRecord );
    
    // Prime record
    iapRecord->iRecordName.SetL( aNameText );
    
    iapRS->iRecords.AppendL( iapRecord );

    CleanupStack::Pop( iapRecord );
    
    iapRecord = NULL;
    
    if ( iapRS->FindL(Session()) )
        {
        if ( iIapId )
            // this is not a new CM
            {
            for ( TInt i = 0; i < iapRS->iRecords.Count(); ++i )
                {
                if ( iapRS->iRecords[i]->RecordId() != iIapId )
                    // duplication because it's not our name
                    {
                    retVal = EFalse;
                    break;
                    }
                }
            }
        else
            // new CM -> any occurence is a duplication
            {
            retVal = EFalse;
            }
        }
        
    CleanupStack::PopAndDestroy( iapRS );
    
    RollbackTransaction();
        
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::EscapeTextLC
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginBaseEng::EscapeTextLC( const TDesC& aLiteral )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::EscapeTextLC" );

    TInt l( aLiteral.Length() );
    //> 2*, to ensure correct esc. even if ALL chars are quotes...
    HBufC* retbuf = HBufC::NewLC( 2*l );
    TPtr ret = retbuf->Des();
    TUint quote( '\'' );  // TChar gives warnings in THUMB & ARMI
    TInt i( 0 );

    for ( i=0; i<l; i++ )
        {
        ret.Append( aLiteral[i] );
        if ( aLiteral[i] == quote )
            {
            // Duplicate quote.
            ret.Append( quote );
            }
        }
    return retbuf;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetNameL
// ---------------------------------------------------------------------------
//
void CCmPluginBaseEng::SetNameL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetNameL" );
    
    if ( iPriv->iNamingMethod == ENamingNothing )
        {
        iIapRecord->iRecordName.SetL( aName );
        }
    else if ( iPriv->iNamingMethod == ENamingUnique )
        {
        HBufC* newName = DoMakeValidNameL( aName );
        
        if ( newName )
            // name converted to unique
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
    else if ( iPriv->iNamingMethod == ENamingNotAccept )
        {
        if ( IsValidNameL( aName ) )
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
        
    CLOG_WRITE_1( "Finale name: [%S]", &FIELD_TO_TDESC(iIapRecord->iRecordName) );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::AddConverstionTableL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::AddConverstionTableL( CCDRecordBase* *aRecord,
                           TBool* aEnabled,
                           const TCmAttribConvTable* aConvTable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::AddConverstionTableL" );
    TCmAttrConvArrayItem* item = new (ELeave) TCmAttrConvArrayItem;
    
    item->iRecord = aRecord;
    item->iEnabled = aEnabled;
    item->iConvTable = aConvTable;
    
    iPriv->iConvTblArray->AppendL( item );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::AddConverstionTableL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::AddCommonConversionTableL( 
                                const TCmCommonAttrConvArrayItem* aConvTable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::AddCommonConversionTableL" );

    User::LeaveIfError( iPriv->iCommonConvTblArray.Append( aConvTable ) );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::RemoveConversionTable
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::RemoveConversionTable( 
                                    const TCmAttribConvTable* aConvTable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::RemoveConversionTable" );

    for ( TInt i = 0; i < iPriv->iConvTblArray->Count(); ++i )
        {
        if ( (*iPriv->iConvTblArray)[i]->iConvTable == aConvTable )
            {
            delete (*iPriv->iConvTblArray)[i];
            iPriv->iConvTblArray->Delete( i );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::RemoveCommonConversionTable
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::RemoveCommonConversionTable( 
                                 const TCmCommonAttrConvArrayItem* aConvTable )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::RemoveCommonConversionTable" );

    for ( TInt i = 0; i < iPriv->iCommonConvTblArray.Count(); ++i )
        {
        if ( iPriv->iCommonConvTblArray[i] == aConvTable )
            {
            iPriv->iCommonConvTblArray.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FindFieldL
// ---------------------------------------------------------------------------
//
TValidationFunctionL CCmPluginBaseEng::FindFieldL( TUint32 aAttribute,
                                           TCMFieldTypes aType,
                                           CMDBElement* &aElement ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::FindFieldL" );

    TValidationFunctionL func = NULL;
    const TCmAttrConvArrayItem* foundItem = ConversionTable( aAttribute );
    
    aElement = NULL;
    
    if( !foundItem )
        {
        User::Leave( KErrNotSupported );
        }
        
    if ( foundItem->iEnabled )
        {
        if ( !(*foundItem->iEnabled) )
            {
            User::Leave( KErrNotSupported );
            }
        }

    if ( !(*foundItem->iRecord) )
        {
        User::Leave( KErrNotSupported );
        }
            
    TInt index( 0 );
    CMDBElement& field = FieldByAttributeL( foundItem, aAttribute, index );
    switch ( field.Type() )
        {
        case EText:
        case EMedText:
        case ELongText:
            {
            if ( aType == ECmText )
                {
                aElement = &field;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
            break;

        case EDesC8:
            {
            if ( aType == ECmText8 )
                {
                aElement = &field;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
            break;
            
        case EUint32:
        case EInt:
        case EBool:
            {
            if ( aType == ECmInt ||
                 aType == ECmBool )
                {
                aElement = &field;
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
            break;
            
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }
        
    if( aElement )
        {
        func = foundItem->iConvTable[index].iValidFuncL;
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    return func;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::Reset
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::Reset()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::Reset" );

    iIapId = 0;
    
    delete iIapRecord; iIapRecord = NULL;
    delete iProxyRecord; iProxyRecord = NULL;
    delete iServiceRecord; iServiceRecord = NULL;
    delete iChargeCardRecord; iChargeCardRecord = NULL;
    delete iNetworkRecord; iNetworkRecord = NULL;
    delete iLocationRecord; iLocationRecord = NULL;
    if ( iPriv )
        {
        iPriv->iLoadResult = KErrNone;
        delete iPriv->iWapAPRecord; iPriv->iWapAPRecord = NULL;
        delete iPriv->iWapIPBearerRecord; iPriv->iWapIPBearerRecord = NULL;
        delete iPriv->iMetaDataRecord; iPriv->iMetaDataRecord = NULL;            
        }
    
    iChargeCardEnabled = EFalse;
    iLocationEnabled = EFalse;
    }
    

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckIfAlreadyConnected
// ---------------------------------------------------------------------------
EXPORT_C TBool CCmPluginBaseEng::CheckIfAlreadyConnected() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CheckIfAlreadyConnected" );

    TBool retVal( EFalse );
    RSocketServ serv;
    RConnection connection;
    TUint   count;

    if ( serv.Connect() == KErrNone )
        {
        if ( connection.Open( serv, KAfInet ) == KErrNone )
            {
            if ( connection.EnumerateConnections( count ) == KErrNone )
                {
                TPckgBuf<TConnectionInfo> connInfo;

                for ( TUint32 i = 1; i <= count; ++i )
                    {
                    connection.GetConnectionInfo( i, connInfo );

                    if ( connInfo().iIapId == iIapId )
                        {
                        CLOG_WRITE( "Found connection" );
                        retVal = ETrue;
                        break;
                        }
                    }
                }

            connection.Close();
            }

        serv.Close();
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckIfNameModifiedL
// ---------------------------------------------------------------------------
EXPORT_C void 
        CCmPluginBaseEng::CheckIfNameModifiedL( CCDRecordBase* aRecord ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CheckIfNameModifiedL" );

    if ( !TPtrC(iIapRecord->iRecordName).CompareF( TPtrC(aRecord->iRecordName)) )
        // names matche
        {
        return;
        }
    
    aRecord->iRecordName.SetL( TPtrC(iIapRecord->iRecordName) );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IAPRecordElementId
// ---------------------------------------------------------------------------
TMDBElementId CCmPluginBaseEng::IAPRecordElementId() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::IAPRecordElementId" );
    
    return ( KCDMaskShowField & iIapRecord->ElementId() );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::IsProtected
// ---------------------------------------------------------------------------
TBool CCmPluginBaseEng::IsProtected() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::IsProtected" );

    return ( iIapRecord->Attributes() & ECDProtectedWrite );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GetLocationIdL
// ---------------------------------------------------------------------------
TUint32 CCmPluginBaseEng::GetLocationIdL() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetLocationIdL" );

    TUint32 locId( 0 );
    CMDBRecordSet<CCDLocationRecord>* locRS = 
            new(ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);
    CleanupStack::PushL( locRS );
    
    CCDLocationRecord* locRecord = static_cast<CCDLocationRecord *>
                         (CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));

    CleanupStack::PushL( locRecord );
    locRecord->iRecordName.SetL( KLocationName );                                            
    locRS->iRecords.AppendL( locRecord );
    CleanupStack::Pop( locRecord );
   
    if ( locRS->FindL(Session()) )
        {
        locRecord = static_cast<CCDLocationRecord *>(locRS->iRecords[0]);
        
        locId = locRecord->RecordId();
        }
    else
        {
        CLOG_WRITE( "Loc id not found" );
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( locRS );
        
    return locId;
    }            

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckForCommonAttribute
// ---------------------------------------------------------------------------
TUint32 
    CCmPluginBaseEng::CheckForCommonAttribute( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CheckForCommonAttribute" );

    TUint32 convAttr( aAttribute );
    
    if ( iPriv->iCommonConvTblArray.Count() &&
        aAttribute > ECmCommonAttributesStart &&
        aAttribute < ECmCommonAttributesEnd )
        {
        for ( TInt i = 0; i < iPriv->iCommonConvTblArray.Count(); ++i )
            {
            const TCmCommonAttrConvArrayItem* convArray = 
                                                iPriv->iCommonConvTblArray[i];
            for ( TInt item = 0; convArray[item].iCommonAttribId; ++item )
                {
                if ( convArray[item].iCommonAttribId == aAttribute )
                    {
                    return convArray[item].iAttribId;
                    }
                }
            }
        }
        
    return convAttr;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CommonAttribute
// ---------------------------------------------------------------------------
/*TUint32 CCmPluginBaseEng::CommonAttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CommonAttribute" );

    if ( iPriv->iCommonConvTblArray.Count() )
        {
        for ( TInt i = 0; i < iPriv->iCommonConvTblArray.Count(); ++i )
            {
            const TCmCommonAttrConvArrayItem* convArray = 
                                                iPriv->iCommonConvTblArray[i];
            for ( TInt item = 0; convArray[item].iCommonAttribId; ++item )
                {
                if ( convArray[item].iAttribId == aAttribute )
                    {
                    return convArray[item].iCommonAttribId;
                    }
                }
            }
        }
        
    User::Leave( KErrArgument );
    
    return 0;
    }*/

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FeatureSupported
// ---------------------------------------------------------------------------
EXPORT_C TBool CCmPluginBaseEng::FeatureSupported( TInt aFeature )
    {
#ifdef _IPV6_DISABLED
    if( aFeature == KFeatureIdIPv6 )
        {
        return EFalse;
        }
#endif
    return CCmManagerImpl::FeatureSupported( aFeature );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GlobalBearerPriority
// ---------------------------------------------------------------------------
EXPORT_C TInt 
    CCmPluginBaseEng::GlobalBearerPriority( const TDesC& aServiceType ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GlobalBearerPriority" );

    return iCmMgr.GlobalBearerPriority( EFalse, aServiceType );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::GlobalUiBearerPriority
// ---------------------------------------------------------------------------
EXPORT_C TInt 
    CCmPluginBaseEng::GlobalUiBearerPriority( const TDesC& aServiceType ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GlobalUiBearerPriority" );

    return iCmMgr.GlobalBearerPriority( ETrue, aServiceType );
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetProxyServerNameL
// ---------------------------------------------------------------------------
TBool CCmPluginBaseEng::SetProxyServerNameL( const TDesC& aProxyServer )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetProxyServerNameL" );

    iProxyRecord->iServerName.SetL( aProxyServer );
    if( !aProxyServer.Length() )
        {
        iProxyRecord->iPortNumber = 0;
        iProxyRecord->iUseProxyServer = EFalse;
        }
    else
        {
        iProxyRecord->iUseProxyServer = ETrue;
        }
        
    return ETrue;        
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ConvTableItemL
// ---------------------------------------------------------------------------
EXPORT_C const TCmAttribConvTable* 
                        CCmPluginBaseEng::ConvTableItem( TUint32 aAttribute )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::ConvTableItem" );

    TInt i;
    TCmAttrConvArrayItem* foundItem = ConversionTable( aAttribute );
         
    if( foundItem )
        {
        // From 2nd item, because the first one is for the range.            
        for ( i = 1; foundItem->iConvTable[i].iAttribId; ++i )
            {
            if ( foundItem->iConvTable[i].iAttribId == aAttribute )
                {
                return &foundItem->iConvTable[i];
                }
            }
        }
    else
        {
        // I know that this is a not nice solution, but 
        // I couldn't find any more inteligent way of adding
        // ECmBearerSettingName to the array.
        const TCmAttribConvTable* item = SUiTable;
        
        // This is a where use 
        for ( i = 0; item[i].iAttribId; ++i )
            {
            if( item[i].iAttribId == aAttribute )
                {
                return &item[i];
                }
            }
        }
        
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::ConversionTable
// ---------------------------------------------------------------------------
TCmAttrConvArrayItem* 
                CCmPluginBaseEng::ConversionTable( TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::ConversionTable" );

    TCmAttrConvArrayItem* foundItem = NULL;
    TInt i;
    
    for ( i = 0; i < iPriv->iConvTblArray->Count(); ++i )
        {
        TCmAttrConvArrayItem* item = (*iPriv->iConvTblArray)[i];
        
        if ( item->iConvTable[0].iAttribId <= aAttribute &&
             item->iConvTable[0].iCommsDatId >= aAttribute )
            {
            foundItem = item;
            break;
            }
        }
        
    return foundItem;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::FieldByAttributeL
// ---------------------------------------------------------------------------
CMDBElement& CCmPluginBaseEng::FieldByAttributeL( 
                                            const TCmAttrConvArrayItem* aItem,
                                            const TUint32 aAttribute,
                                            TInt& aIndex ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::FieldByAttributeL" );

    // From 2nd item, because the first one is for the range.            
    for ( aIndex = 1; aItem->iConvTable[aIndex].iAttribId; ++aIndex )
        {
        if ( aItem->iConvTable[aIndex].iAttribId == aAttribute )
            {
            return *(*aItem->iRecord)->GetFieldByIdL( 
                                      aItem->iConvTable[aIndex].iCommsDatId );
            }
        }
        
    User::Leave( KErrNotFound );
    // Dead code
    return *(CMDBElement*)1;
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::SetDNSServerAddressL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::SetDNSServerAddressL( TUint32 aSrv1Attr,
                                                      const TDesC& aSrv1,
                                                      TUint32 aSrv2Attr,
                                                      const TDesC& aSrv2,
                                                      TUint32 aAddrFromSrvAttr,
                                                      TBool aIPv6 )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetDNSServerAddressL" );

    TBool undef1;
    TBool undef2;
    TPtrC dynamic;
    
    if( aIPv6 )
        // IPv6
        {
        undef1 = ClassifyIPv6Address( aSrv1 ) == EIPv6Unspecified;
        undef2 = ClassifyIPv6Address( aSrv2 ) == EIPv6Unspecified;
        
        dynamic.Set( KDynamicIpv6Address );
        }
    else
        {
        undef1 = IsUnspecifiedIPv4Address( aSrv1 );
        undef2 = IsUnspecifiedIPv4Address( aSrv2 );
        
        dynamic.Set( KUnspecifiedIPv4 );
        }
        
    if( undef1 )
        {
        SetStringAttributeL( aSrv1Attr, aSrv2 );
        SetStringAttributeL( aSrv2Attr, dynamic );
        
        SetBoolAttributeL( aAddrFromSrvAttr, undef2 );
        }
    else
        {
        SetStringAttributeL( aSrv1Attr, aSrv1 );
        SetStringAttributeL( aSrv2Attr, aSrv2 );

        SetBoolAttributeL( aAddrFromSrvAttr, EFalse );
        }        
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckDNSServerAddressL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBaseEng::CheckDNSServerAddressL( TBool aIPv6,
                                                        CMDBField<TDesC>& aDNS1,
                                                        CMDBField<TDesC>& aDNS2,
                                                        CMDBField<TBool>& /*aDNSFromServer*/ )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CheckDNSServerAddressL" );

    // aDNSFromServer is commented out because the API must not set this
    // field any more. It is only set from the UI
    if( !(aDNS1.ElementId() & KCDChangedFlag) &&
        !(aDNS2.ElementId() & KCDChangedFlag) )
        // No change
        {
        return;
        }

    if( aIPv6 )
        {
        if( ClassifyIPv6Address( aDNS1 ) == EIPv6Unspecified )
            {
            if( ClassifyIPv6Address( aDNS2 ) != EIPv6Unspecified )
                {
                aDNS1.SetL( aDNS2 );
                aDNS2.SetL( KDynamicIpv6Address );
                }
            }
        }
    else // IPv4
        {
        if( IsUnspecifiedIPv4Address( aDNS1 ) )
            {
            if( !IsUnspecifiedIPv4Address( aDNS2 ) )
                {
                aDNS1.SetL( aDNS2 );
                aDNS2.SetL( KUnspecifiedIPv4 );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CCmPluginBaseEng::CheckSpaceBelowCriticalLevelL
// ---------------------------------------------------------------------------
//
TBool CCmPluginBaseEng::CheckSpaceBelowCriticalLevelL() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::CheckSpaceBelowCriticalLevelL" );

    // OOD handling. If disk space is low user is notified.
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    
    // Checks the FFS space "after" addition
    TBool belowCL = SysUtil::FFSSpaceBelowCriticalLevelL
            ( &fs, KEstimatedOverhead );
    
    CleanupStack::PopAndDestroy(); // fs
    
    if( belowCL )
        {
        // Raise a dialog to notify the user.
        TCmCommonUi::ShowMemoryFullConfirmationQueryL();
        // It must leave because it is the only way to notify the caller about
        // the operation failed. (Its caller does not return indicator.)
        // KLeaveWithoutAlert means no more dialog will raise
        User::Leave( KLeaveWithoutAlert );
        }
        
    // Return value keept to have a suitable API    
    return belowCL;    
    
    }
    
// -----------------------------------------------------------------------------
// CCmPluginBaseEng::IncrementRefCounter
// -----------------------------------------------------------------------------
void CCmPluginBaseEng::IncrementRefCounter()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::IncrementRefCounter" );

    iRefCounter++;
    }
// -----------------------------------------------------------------------------
// CCmPluginBaseEng::DecrementRefCounter
// -----------------------------------------------------------------------------
void CCmPluginBaseEng::DecrementRefCounter()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::DecrementRefCounter" );

    iRefCounter--;
    }
// -----------------------------------------------------------------------------
// CCmPluginBaseEng::GetRefCounter
// -----------------------------------------------------------------------------
TInt CCmPluginBaseEng::GetRefCounter()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetRefCounter" );

    return iRefCounter;
    }

// -----------------------------------------------------------------------------
// CCmPluginBaseEng::IdIsValid
// -----------------------------------------------------------------------------
TBool CCmPluginBaseEng::IdIsValid()
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::IdIsValid" );

    return iIdIsValid;
    }


// -----------------------------------------------------------------------------
// CCmPluginBaseEng::IdIsValid
// -----------------------------------------------------------------------------
void CCmPluginBaseEng::SetIdValidity(TBool validity)
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetIdValidity" );

    iIdIsValid = validity;
    }


// -----------------------------------------------------------------------------
// CCmPluginBaseEng::GetRefCounter
// -----------------------------------------------------------------------------
void CCmPluginBaseEng::GenericStringWriterL( const TUint32 aAttribute, 
                                             const TDesC16& aValue )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GenericStringWriterL" );

    TUint32 attribute = CheckForCommonAttribute( aAttribute );
    CMDBField<TDesC>* field;
    
    TValidationFunctionL func = 
                FindFieldL( attribute, ECmText, (CMDBElement*&)field );
    
    if( func )
        {
        func( this, attribute, (const TAny*)&aValue );
        }
        
    field->SetL( aValue );    
    }

// ---------------------------------------------------------------------------
// CCmPluginBaseEng::MappedCommonAttribute
// ---------------------------------------------------------------------------
TUint32 
    CCmPluginBaseEng::MappedCommonAttribute( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::MappedCommonAttribute" );

    TUint32 convAttr( aAttribute );
    
    if ( iPriv->iCommonConvTblArray.Count() )
        {
        for ( TInt i = 0; i < iPriv->iCommonConvTblArray.Count(); ++i )
            {
            const TCmCommonAttrConvArrayItem* convArray = 
                                                iPriv->iCommonConvTblArray[i];
            for ( TInt item = 0; convArray[item].iCommonAttribId; ++item )
                {
                if ( convArray[item].iAttribId == aAttribute )
                    {
                    return convArray[item].iCommonAttribId;
                    }
                }
            }
        }
        
    return convAttr;
    }
//-----------------------------------------------------------------------------
// CCmPluginBaseEng::ParentDestination
// -----------------------------------------------------------------------------
EXPORT_C CCmDestinationImpl* CCmPluginBaseEng::ParentDestination() const
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::ParentDestination" );

    if (iParentDest)
        {
        CCmDestinationImpl* parentDest = NULL;
        TRAPD( err, parentDest = iCmMgr.DestinationL( iParentDest ) );
        if ( !err )
            {
            return parentDest;
            }
        }
    
    return NULL;
    }
   
//-----------------------------------------------------------------------------
// CCmPluginBaseEng::GetAdditionalUids
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::GetAdditionalUids( RArray<TUint32>& /*aIapIds*/ )
    {
    LOGGER_ENTERFN( "CCmPluginBaseEng::GetAdditionalUids" );
    }

//-----------------------------------------------------------------------------
// CCmPluginBaseEng::SetPreDefinedId
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBaseEng::SetPreDefinedId( const TUint32 aPreDefId )
	{
    LOGGER_ENTERFN( "CCmPluginBaseEng::SetPreDefinedId" );

	iPreDefIapId = aPreDefId;
	}

