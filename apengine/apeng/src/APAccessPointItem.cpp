/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of the class CApAccessPointItem
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifvar_internal.h>
#endif
#include    <cdblen.h>
#include    <f32file.h>
#include    <bautils.h>
#include    <barsc.h>
#include    <apengine.rsg>
#include    "ApAccessPointItem.h"
#include    "ApEngineConsts.h"
#include    "ApEngineCommons.h"
#include    "ApEngineLogger.h"
#include    <txtetext.h>
#include    <in_sock.h>

#include    "APItemExtra.h"
#include    "APItemCdmaData.h"
#include    "APItemWlanData.h"

#include    <featmgr.h>
#include    <etelpckt.h>
#include    <etelqos.h>

#include    <data_caging_path_literals.hrh>


// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KDefIspIfName, "ppp" );                           // required !

_LIT( KDefGprsIfName, "pppgprs" );                      // required !

_LIT( KDefIspIfNetworksIPv4, "ip" );                        // required !
//_LIT( KDefGprsIfParams, "gprsnif" );                    // required !
_LIT( KDefGprsIfParams, "" );                    // required !

_LIT( KDefWapGatewayIpAddress, "0.0.0.0" );
_LIT( KDefIspGateway, "0.0.0.0" );
_LIT( KDefPhoneIpAddress, "0.0.0.0" );
_LIT( KDefPrimaryNameServer, "0.0.0.0" );
_LIT( KDefSecondaryNameServer, "0.0.0.0" );

_LIT( KWellKnownIp6PrimaryNameServer, "fec0:000:0000:ffff::1" );
_LIT( KWellKnownIp6SecondaryNameServer, "fec0:000:0000:ffff::2" );
_LIT( KDefIp6PrimaryNameServer, "0:0:0:0:0:0:0:0" );
_LIT( KDefIp6SecondaryNameServer, "0:0:0:0:0:0:0:0" );

_LIT( KDefIspIfNetworksIPv6, "ip6" );                        // required !
_LIT( KDefIspIfNetworksIPv4IPv6, "ip,ip6" );                 // required !
_LIT( KDefIspIfNetworksIPv4IPv6LAN, "ip,ip6" );              // required !



/// ROM drive.
_LIT( KApEngineResDriveZ, "z:" );
/// ApEngine resource file name.
_LIT( KApEngineResFileName, "ApEngine.rsc" );


/**
* General Settings UID
*/
#ifdef __TEST_USE_SHARED_DATA
    LOCAL_D const TUid KGeneralSettingsUid = { 0X100058EC };
#endif // __TEST_USE_SHARED_DATA


// MACROS
#define BOOL_COMPARE(a,b) (((a) && (b)) || (!(a) && !(b)))

// LOCAL CONSTANTS AND MACROS
const TInt KApItemInitialTxtBufSize = 0;

const TInt KApMembers = EApWlanIPNameServer2+1;

_LIT( KDefProxyProtocolName, "http" );                      // required !

#if defined(_DEBUG)
    _LIT( kApSet, "APEngine" ) ;
#endif // (_DEBUG)

const TInt KEndOfArray = -1;

const TInt KCsdAnalogue[]       = { KSpeedAutobaud, KSpeed9600, KSpeed14400,
                                    KEndOfArray };

const TInt KCsdAnalogueWcdma[]  = { KSpeedAutobaud, KSpeed9600, KSpeed14400,
                                    KSpeed28800, KEndOfArray};

const TInt KCsdIsdn110[]        = { KSpeed9600, KSpeed14400, KEndOfArray};

const TInt KCsdIsdn110Wcdma[]   = { KSpeed9600, KSpeed14400, KSpeed28800,
                                    KSpeed38400, KEndOfArray};

const TInt KCsdIsdn120[]        = { KSpeed9600, KSpeed14400, KEndOfArray};

const TInt KCsdIsdn120Wcdma[]   = { KSpeed9600, KSpeed14400, KSpeed28800,
                                    KSpeed56000, KEndOfArray};

const TInt KHcsdAnal[]          = { KSpeedAutobaud, KSpeed9600, KSpeed14400,
                                    KSpeed19200, KSpeed28800, KEndOfArray};

const TInt KHcsdIsdn110[]       = { KSpeed9600, KSpeed14400, KSpeed19200, 
                                    KSpeed28800, KSpeed38400, KEndOfArray};

const TInt KHcsdIsdn120[]       = { KSpeed9600, KSpeed14400, KSpeed19200,
                                    KSpeed28800, KSpeed43200, KEndOfArray};



// ---------------------------------------------------------
// ---------------------------------------------------------
// 
//  LOCAL, CApItemExtra class implementation
// 
// ---------------------------------------------------------
// ---------------------------------------------------------
//
// ---------------------------------------------------------
// CApItemExtra::NewL
// ---------------------------------------------------------
//
CApItemExtra* CApItemExtra::NewL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemExtra::NewL" ) ) );

    CApItemExtra* self = new ( ELeave ) CApItemExtra;
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );

    CLOG( ( EApItem, 1, _L( "<- CApItemExtra::NewL" ) ) );
    
    return self;
    }


// Destructor
// ---------------------------------------------------------
// CApItemExtra::~CApItemExtra
// ---------------------------------------------------------
//
CApItemExtra::~CApItemExtra()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemExtra::~CApItemExtra" ) ) );

    delete iCdmaData;
    delete iWlanData;
    delete iIpv6PrimaryDNS;
    delete iIpv6SecondaryDNS;
    delete iLanBearerModemName;
    delete iIapBearerType;
    }


// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
// CApItemExtra::CApItemExtra
// ---------------------------------------------------------
//
CApItemExtra::CApItemExtra()
:iIsFeatureManagerInitialised( EFalse ),
iIpv6GetDNSIPFromServer( ETrue ),
iIsWcdma( EFalse ), 
iIsIpv6Supported( EFalse ),
iIsVpnAp( EFalse ),
iIsAppCsdSupport( EFalse )
    {
    }

// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApItemExtra::ConstructL
// ---------------------------------------------------------
//
void CApItemExtra::ConstructL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemExtra::ConstructL" ) ) );

    iIpv6PrimaryDNS = HBufC::NewL( KApItemInitialTxtBufSize );
    iIpv6SecondaryDNS = HBufC::NewL( KApItemInitialTxtBufSize );
    iLanBearerModemName = HBufC::NewL( KApItemInitialTxtBufSize );
    iIapBearerType = HBufC::NewL( KApItemInitialTxtBufSize );
     
#ifdef __TEST_USE_SHARED_DATA
    iIsAppHscsdSupport = 
        ApCommons::IsGivenSharedDataSupportL( KGeneralSettingsUid, 
                                              KGSHSCSDAccessPoints );
#else
    iIsAppHscsdSupport = ETrue;
#endif // __TEST_USE_SHARED_DATA


#ifdef __TEST_HSCSD_SUPPORT
    iIsAppHscsdSupport = ETrue;
#endif // __TEST_HSCSD_SUPPORT

    CLOG( ( EApItem, 1, _L( "<- CApItemExtra::ConstructL" ) ) );
    }


// ---------------------------------------------------------
// ---------------------------------------------------------
// 
//  LOCAL, CApItemCdmaData class implementation
// 
// ---------------------------------------------------------
// ---------------------------------------------------------
//
// ---------------------------------------------------------
// CApItemCdmaData::NewL
// ---------------------------------------------------------
//
CApItemCdmaData* CApItemCdmaData::NewL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemCdmaData::NewL" ) ) );

    CApItemCdmaData* self = new ( ELeave ) CApItemCdmaData;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CLOG( ( EApItem, 1, _L( "<- CApItemCdmaData::NewL" ) ) );
    
    return self;
    }


// Destructor
// ---------------------------------------------------------
// CApItemCdmaData::~CApItemCdmaData
// ---------------------------------------------------------
//
CApItemCdmaData::~CApItemCdmaData()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemCdmaData::~CApItemCdmaData" ) ) );

    delete iIwfName;
    delete iPdpAddress;
    delete iHomeAgentAddress;
    delete iMipHomeAddress;
    delete iMipPrimaryHomeAgent;
    delete iMipSecondaryHomeAgent;
    }


// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
// CApItemCdmaData::CApItemCdmaData
// ---------------------------------------------------------
//
CApItemCdmaData::CApItemCdmaData()
/* Assuming 0 default values everywhere. */
:iServiceOption( RPacketContext::KLowSpeedData ),
iPdpType( EIPv4 ),
iReqFwdPriority( 0 ), /*RPacketQoS::TQoSLinkPriority*/
iReqRevPriority( 0 ), /*RPacketQoS::TQoSLinkPriority*/
iReqFwdBitrate( 0 ),  /*RPacketQoS::TQoSDataRate*/
iReqRevBitrate( 0 ),  /*RPacketQoS::TQoSDataRate*/
iReqFwdLoss( 0 ),  /*RPacketQoS::TQoSDataLoss*/
iReqRevLoss( 0 ),  /*RPacketQoS::TQoSDataLoss*/
iReqFwdMaxDelay( 0 ),  /*RPacketQoS::TQoSDelay*/
iReqRevMaxDelay( 0 ),  /*RPacketQoS::TQoSDelay*/
iMinFwdBitrate( 0 ),  /*RPacketQoS::TQoSDataRate*/
iMinRevBitrate( 0 ),  /*RPacketQoS::TQoSDataRate*/
iAccptFwdLoss( 0 ),  /*RPacketQoS::TQoSDataLoss*/
iAccptRevLoss( 0 ),  /*RPacketQoS::TQoSDataLoss*/
iAccptFwdMaxDelay( 0 ),  /*RPacketQoS::TQoSDelay*/
iAccptRevMaxDelay( 0 ),  /*RPacketQoS::TQoSDelay*/
iQosWarningTimeout( 0 ), /*0xffffffff disables*/
iRlpMode( RPacketQoS::KRLPUnknown ),
// CDMA2000 deprecated Mobile IP fields
iMip( EFalse ),
iMipTimeout( 0 ),
// CDMA2000 specific CDMA parameters provisioned through OTA
iNaiType( 0 ),
iSimpleIpAuthAlgorithm( 0 ),
iSimpleIpPapSsHandle( 0 ),
iSimpleIpChapSsHandle( 0 ),
iMipTBit( 0 ),
iMipMnAaaAuthAlgorithm( 0 ),
iMipMnAaaSpi( 0 ),
iMipMnAaaSsHandle( 0 ),
iMipMnHaAuthAlgorithm( 0 ),
iMipMnHaSpi( 0 ),
iMipMnHaSsHandle( 0 )
    {
    }

// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApItemCdmaData::ConstructL
// ---------------------------------------------------------
//
void CApItemCdmaData::ConstructL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemCdmaData::ConstructL" ) ) );

    iIwfName = HBufC::NewL( KApItemInitialTxtBufSize );
    iPdpAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iHomeAgentAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iMipHomeAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iMipPrimaryHomeAgent = HBufC::NewL( KApItemInitialTxtBufSize );
    iMipSecondaryHomeAgent = HBufC::NewL( KApItemInitialTxtBufSize );

    CLOG( ( EApItem, 1, _L( "<- CApItemCdmaData::ConstructL" ) ) );
    }







// ---------------------------------------------------------
// ---------------------------------------------------------
// 
//  LOCAL, CApItemWlanData class implementation
// 
// ---------------------------------------------------------
// ---------------------------------------------------------
//
// ---------------------------------------------------------
// CApItemWlanData::NewL
// ---------------------------------------------------------
//
CApItemWlanData* CApItemWlanData::NewL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemWlanData::NewL" ) ) );

    CApItemWlanData* self = new ( ELeave ) CApItemWlanData;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CLOG( ( EApItem, 1, _L( "<- CApItemWlanData::NewL" ) ) );
    
    return self;
    }


// Destructor
// ---------------------------------------------------------
// CApItemWlanData::~CApItemWlanData
// ---------------------------------------------------------
//
CApItemWlanData::~CApItemWlanData()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemWlanData::~CApItemWlanData" ) ) );
    delete iWlanNetworkName;
    }


// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
// CApItemWlanData::CApItemWlanData
// ---------------------------------------------------------
//
CApItemWlanData::CApItemWlanData()
:iWlanNetworkMode( EInfra ),
iWlanSecurityMode( EOpen )
/* Assuming 0 default values elsewhere. */
    {
    }

// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApItemWlanData::ConstructL
// ---------------------------------------------------------
//
void CApItemWlanData::ConstructL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApItemWlanData::ConstructL" ) ) );

    iWlanNetworkName = HBufC::NewL( KApItemInitialTxtBufSize );

    CLOG( ( EApItem, 1, _L( "<- CApItemWlanData::ConstructL" ) ) );
    }




// MODULE DATA STRUCTURES

// =========================================================
// =========================================================
// =========================================================
// 
// CApAccessPointItem class implementation
// 
// =========================================================
// =========================================================
// =========================================================
//

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApAccessPointItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApAccessPointItem* CApAccessPointItem::NewLC()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::NewLC" ) ) );

    CApAccessPointItem* self = new ( ELeave ) CApAccessPointItem;
    CleanupStack::PushL( self );
    self->ConstructL();

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::NewLC" ) ) );
    return self;
    }


// Destructor
// ---------------------------------------------------------
// CApAccessPointItem::~CApAccessPointItem
// ---------------------------------------------------------
//
EXPORT_C CApAccessPointItem::~CApAccessPointItem()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::~CApAccessPointItem" ) ) );

    if ( iExt )
        {
        if ( iExt->iIsFeatureManagerInitialised )
            {
            FeatureManager::UnInitializeLib();
            }
        }

    delete iExt;
    delete iSpecified;
    delete iWapAccessPointName;
    delete iWapBearer;
    delete iStartingPage;
    delete iWapGatewayAddress;
    delete iIapName;
    delete iIspName;
    delete iIspDescription;
    delete iIspDefaultTelNumber;
    delete iLoginScript;
    delete iUserName;
    delete iPassword;
    delete iIspIfName;
    delete iIspIfParams;
    delete iIspIfNetworks;
    delete iIspIfAuthName;
    delete iIspIfAuthPass;
    delete iIspIfCallbackInfo;
    delete iIspIPAddr;
    delete iIspIPNetMask;
    delete iIspGateway;
    delete iPrimaryDNS;
    delete iSecondaryDNS;
    delete iIspInitString;
    delete iIapServiceType;
    delete iGprsAccessPointName;
    delete iGprsPdpAddress;
    delete iApProxyProtocolName;
    delete iApProxyServerAddress;
    delete iApProxyExceptions;
    delete iNetworkName;
    delete iWapProxyLoginName; 
    delete iWapProxyLoginPass;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::~CApAccessPointItem" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::CopyFromL( const CApAccessPointItem&
                                            aCopyFrom  )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CopyFromL" ) ) );

    WriteTextL( EApWapAccessPointName, *aCopyFrom.iWapAccessPointName );
    WriteTextL( EApWapCurrentBearer, *aCopyFrom.iWapBearer );
    WriteLongTextL( EApWapStartPage, *aCopyFrom.iStartingPage );
    // The WAP gateway address
    WriteTextL( EApWapGatewayAddress, *aCopyFrom.iWapGatewayAddress );

    iIsWTLSSecurityOn = aCopyFrom.iIsWTLSSecurityOn;
    iIsConnectionTypeContinuous = aCopyFrom.iIsConnectionTypeContinuous;
    iChargeCard = aCopyFrom.iChargeCard;
    iWapIap = aCopyFrom.iWapIap;

    WriteTextL( EApIapName, *aCopyFrom.iIapName );
    iIapServiceId = aCopyFrom.iIapServiceId;
    WriteTextL( EApIapServiceType, *aCopyFrom.iIapServiceType );


    iIapChargeCardId = aCopyFrom.iIapChargeCardId;

    WriteTextL( EApIspName, *aCopyFrom.iIspName );
    WriteTextL( EApIspDescription, *aCopyFrom.iIspDescription );

    iIspType = aCopyFrom.iIspType;

    WriteTextL( EApIspDefaultTelNumber, *aCopyFrom.iIspDefaultTelNumber );

    iIspDialResolution = aCopyFrom.iIspDialResolution;
    iUseLoginScript = aCopyFrom.iUseLoginScript;

    WriteLongTextL( EApIspLoginScript, *aCopyFrom.iLoginScript );

    iPromptPassword = aCopyFrom.iPromptPassword;

    WriteTextL( EApIspLoginName, *aCopyFrom.iUserName );
    WriteTextL( EApIspLoginPass, *aCopyFrom.iPassword );

    iDisplayTerminalWindow = aCopyFrom.iDisplayTerminalWindow;

    WriteTextL( EApIspIfName, *aCopyFrom.iIspIfName );
    WriteTextL( EApIspIfParams, *aCopyFrom.iIspIfParams );
    WriteTextL( EApIspIfNetworks, *aCopyFrom.iIspIfNetworks );

    iIspIfPromptForAuth = aCopyFrom.iIspIfPromptForAuth;

    WriteTextL( EApIspIfAuthName, *aCopyFrom.iIspIfAuthName );
    WriteTextL( EApIspIfAuthPass, *aCopyFrom.iIspIfAuthPass );

    iIspIfAuthRetries = aCopyFrom.iIspIfAuthRetries;
    iUseCallBack = aCopyFrom.iUseCallBack;
    iCallBackTypeIsServerNum = aCopyFrom.iCallBackTypeIsServerNum;

    WriteTextL( EApIspIfCallbackInfo, *aCopyFrom.iIspIfCallbackInfo );

    iIspCallbackTimeOut = aCopyFrom.iIspCallbackTimeOut;
    iIspIPAddrFromServer = aCopyFrom.iIspIPAddrFromServer;

    WriteTextL( EApIspIPAddr, *aCopyFrom.iIspIPAddr );
    WriteTextL( EApIspIPNetMask, *aCopyFrom.iIspIPNetMask );
    WriteTextL( EApIspIPGateway, *aCopyFrom.iIspGateway );

    iGetDNSIPFromServer = aCopyFrom.iGetDNSIPFromServer;

    WriteTextL( EApIspIPNameServer1, *aCopyFrom.iPrimaryDNS );
    WriteTextL( EApIspIPNameServer2, *aCopyFrom.iSecondaryDNS );

    iEnablePPPCompression = aCopyFrom.iEnablePPPCompression;
    iIspEnableLCPExtensions = aCopyFrom.iIspEnableLCPExtensions;
    iIsPasswordAuthenticationSecure =
                aCopyFrom.iIsPasswordAuthenticationSecure;
    iIspEnableSwCompression = aCopyFrom.iIspEnableSwCompression;
    iIspBearerName = aCopyFrom.iIspBearerName;
    iMaxConnSpeed = aCopyFrom.iMaxConnSpeed;
    iIspBearerCE = aCopyFrom.iIspBearerCE;
    iApIapBearerService = aCopyFrom.iApIapBearerService;
    iBearerCallTypeIsdn = aCopyFrom.iBearerCallTypeIsdn;

    WriteTextL( EApIspInitString, *aCopyFrom.iIspInitString );

    iIspBearerType = aCopyFrom.iIspBearerType;
    iIspChannelCoding = aCopyFrom.iIspChannelCoding;
    iIspAIUR = aCopyFrom.iIspAIUR;
    iIspRequestedTimeSlots = aCopyFrom.iIspRequestedTimeSlots;
    iIspMaximumTimeSlots = aCopyFrom.iIspMaximumTimeSlots;

    WriteLongTextL( EApGprsAccessPointName, *aCopyFrom.iGprsAccessPointName );

    WriteTextL( EApGprsPdpAddress, *aCopyFrom.iGprsPdpAddress );

    iGprsPdpType = aCopyFrom.iGprsPdpType;
    iGprsReqPrecedence = aCopyFrom.iGprsReqPrecedence;
    iGprsReqDelay = aCopyFrom.iGprsReqDelay;
    iGprsReqReliability = aCopyFrom.iGprsReqReliability;
    iGprsReqPeakThroughput = aCopyFrom.iGprsReqPeakThroughput;
    iGprsReqMeanPeakThroughput = aCopyFrom.iGprsReqMeanPeakThroughput;
    iGprsMinPrecedence = aCopyFrom.iGprsMinPrecedence;

    iGprsMinDelay = aCopyFrom.iGprsMinDelay;
    iGprsMinReliability = aCopyFrom.iGprsMinReliability;
    iGprsMinPeakThroughput = aCopyFrom.iGprsMinPeakThroughput;
    iGprsMinMeanThroughput = aCopyFrom.iGprsMinMeanThroughput;

    iGprsUseAnonymAccess = aCopyFrom.iGprsUseAnonymAccess;
    WriteTextL( EApProxyProtocolName, *aCopyFrom.iApProxyProtocolName );
    WriteLongTextL( EApProxyServerAddress, *aCopyFrom.iApProxyServerAddress );
    WriteLongTextL( EApProxyExceptions, *aCopyFrom.iApProxyExceptions);        
    iApProxyPortNumber = aCopyFrom.iApProxyPortNumber;
    iApProxyUseProxy = aCopyFrom.iApProxyUseProxy;
    iApHasProxySettings = aCopyFrom.iApHasProxySettings;
    WriteTextL( EApNetworkName, *aCopyFrom.iNetworkName );
    WriteTextL( EApProxyLoginName, *aCopyFrom.iWapProxyLoginName );
    WriteTextL( EApProxyLoginPass, *aCopyFrom.iWapProxyLoginPass );

    iNetworkId = aCopyFrom.iNetworkId;
    WriteTextL( EApLanBearerName, *aCopyFrom.iExt->iLanBearerModemName );
    WriteTextL( EApIapBearerType, *aCopyFrom.iExt->iIapBearerType );


    // Copying of WLAN data must be before Ipv6 support handling as that 
    // requires the knowledge of the bearer type!!!!!
    if ( iExt->iWlanData )
        {
        iExt->iWlanData->iWlanNetworkMode = 
                    aCopyFrom.iExt->iWlanData->iWlanNetworkMode;
        iExt->iWlanData->iWlanSecurityMode = 
                    aCopyFrom.iExt->iWlanData->iWlanSecurityMode;
        ReAllocL( iExt->iWlanData->iWlanNetworkName, 
                    *aCopyFrom.iExt->iWlanData->iWlanNetworkName );
        iExt->iWlanData->iWlanId = aCopyFrom.iExt->iWlanData->iWlanId;
        iExt->iWlanData->iIsWlan = aCopyFrom.iExt->iWlanData->iIsWlan;
        iExt->iWlanData->iScanSSID = aCopyFrom.iExt->iWlanData->iScanSSID;
        iExt->iWlanData->iChannelId = aCopyFrom.iExt->iWlanData->iChannelId;
        }

    if ( iExt->iIsIpv6Supported )
        {
        iExt->iIpv6GetDNSIPFromServer = 
            aCopyFrom.iExt->iIpv6GetDNSIPFromServer;
        WriteTextL( EApIP6NameServer1, *aCopyFrom.iExt->iIpv6PrimaryDNS );
        WriteTextL( EApIP6NameServer2, *aCopyFrom.iExt->iIpv6SecondaryDNS );
        }

    if( iExt->iCdmaData )
        {
        // CDMA2000
        ReAllocL( iExt->iCdmaData->iIwfName, 
                    *aCopyFrom.iExt->iCdmaData->iIwfName );
        iExt->iCdmaData->iServiceOption = 
                    aCopyFrom.iExt->iCdmaData->iServiceOption;
        iExt->iCdmaData->iPdpType = aCopyFrom.iExt->iCdmaData->iPdpType;
        ReAllocL( iExt->iCdmaData->iPdpAddress, 
                    *aCopyFrom.iExt->iCdmaData->iPdpAddress );
        iExt->iCdmaData->iReqFwdPriority = 
                    aCopyFrom.iExt->iCdmaData->iReqFwdPriority;
        iExt->iCdmaData->iReqRevPriority = 
                    aCopyFrom.iExt->iCdmaData->iReqRevPriority;
        iExt->iCdmaData->iReqFwdBitrate = 
                    aCopyFrom.iExt->iCdmaData->iReqFwdBitrate;
        iExt->iCdmaData->iReqRevBitrate = 
                    aCopyFrom.iExt->iCdmaData->iReqRevBitrate;
        iExt->iCdmaData->iReqFwdLoss = aCopyFrom.iExt->iCdmaData->iReqFwdLoss;
        iExt->iCdmaData->iReqRevLoss = aCopyFrom.iExt->iCdmaData->iReqRevLoss;
        iExt->iCdmaData->iReqFwdMaxDelay = 
                    aCopyFrom.iExt->iCdmaData->iReqFwdMaxDelay;
        iExt->iCdmaData->iReqRevMaxDelay = 
                    aCopyFrom.iExt->iCdmaData->iReqRevMaxDelay;
        iExt->iCdmaData->iMinFwdBitrate = 
                    aCopyFrom.iExt->iCdmaData->iMinFwdBitrate;
        iExt->iCdmaData->iMinRevBitrate = 
                    aCopyFrom.iExt->iCdmaData->iMinRevBitrate;
        iExt->iCdmaData->iAccptFwdLoss = 
                    aCopyFrom.iExt->iCdmaData->iAccptFwdLoss;
        iExt->iCdmaData->iAccptRevLoss = 
                    aCopyFrom.iExt->iCdmaData->iAccptRevLoss;
        iExt->iCdmaData->iAccptFwdMaxDelay = 
                    aCopyFrom.iExt->iCdmaData->iAccptFwdMaxDelay;
        iExt->iCdmaData->iAccptRevMaxDelay = 
                    aCopyFrom.iExt->iCdmaData->iAccptRevMaxDelay;
        iExt->iCdmaData->iQosWarningTimeout = 
                    aCopyFrom.iExt->iCdmaData->iQosWarningTimeout;
        iExt->iCdmaData->iRlpMode = aCopyFrom.iExt->iCdmaData->iRlpMode;
        // CDMA2000 deprecated Mobile IP fields
        iExt->iCdmaData->iMip = aCopyFrom.iExt->iCdmaData->iMip;
        ReAllocL( iExt->iCdmaData->iHomeAgentAddress, 
                    *aCopyFrom.iExt->iCdmaData->iHomeAgentAddress );
        iExt->iCdmaData->iMipTimeout = aCopyFrom.iExt->iCdmaData->iMipTimeout;
        // CDMA2000 specific CDMA parameters provisioned through OTA
        iExt->iCdmaData->iNaiType = aCopyFrom.iExt->iCdmaData->iNaiType;
        iExt->iCdmaData->iSimpleIpAuthAlgorithm = 
                    aCopyFrom.iExt->iCdmaData->iSimpleIpAuthAlgorithm;
        iExt->iCdmaData->iSimpleIpPapSsHandle = 
                    aCopyFrom.iExt->iCdmaData->iSimpleIpPapSsHandle;
        iExt->iCdmaData->iSimpleIpChapSsHandle = 
                    aCopyFrom.iExt->iCdmaData->iSimpleIpChapSsHandle;
        iExt->iCdmaData->iMipTBit = aCopyFrom.iExt->iCdmaData->iMipTBit;
        ReAllocL( iExt->iCdmaData->iMipHomeAddress, 
                    *aCopyFrom.iExt->iCdmaData->iMipHomeAddress );
        ReAllocL( iExt->iCdmaData->iMipPrimaryHomeAgent, 
                    *aCopyFrom.iExt->iCdmaData->iMipPrimaryHomeAgent );
        ReAllocL( iExt->iCdmaData->iMipSecondaryHomeAgent, 
                    *aCopyFrom.iExt->iCdmaData->iMipSecondaryHomeAgent );
        iExt->iCdmaData->iMipMnAaaAuthAlgorithm = 
                    aCopyFrom.iExt->iCdmaData->iMipMnAaaAuthAlgorithm;
        iExt->iCdmaData->iMipMnAaaSpi = 
                    aCopyFrom.iExt->iCdmaData->iMipMnAaaSpi;
        iExt->iCdmaData->iMipMnAaaSsHandle = 
                    aCopyFrom.iExt->iCdmaData->iMipMnAaaSsHandle;
        iExt->iCdmaData->iMipMnHaAuthAlgorithm = 
                    aCopyFrom.iExt->iCdmaData->iMipMnHaAuthAlgorithm;
        iExt->iCdmaData->iMipMnHaSpi = aCopyFrom.iExt->iCdmaData->iMipMnHaSpi;
        iExt->iCdmaData->iMipMnHaSsHandle = 
                    aCopyFrom.iExt->iCdmaData->iMipMnHaSsHandle;
        }


    SetIfNetworksL();

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CopyFromL" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::operator==
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::operator==(
                                             const CApAccessPointItem& aItem
                                             ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::operator==" ) ) );

    TBool retval( EFalse );
    TRAPD( err, retval = DoCompareApL( aItem ) );
    if ( err )
        {
        retval = EFalse;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::operator==" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::operator!=
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::operator!=(
                                             const CApAccessPointItem& aItem
                                             ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::operator!=" ) ) );

    TBool ret = ( *this == aItem );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::operator!=" ) ) );
    return ( !ret );
    }



// C++ default constructor can NOT contain any code that
// might leave.
// ---------------------------------------------------------
// CApAccessPointItem::CApAccessPointItem
// ---------------------------------------------------------
//
EXPORT_C CApAccessPointItem::CApAccessPointItem()
:iIsWTLSSecurityOn( EFalse ),
iIsConnectionTypeContinuous( EWapWspOptionConnectionOriented ),
iIspType( EIspTypeInternetOnly ),
iUseLoginScript( EFalse ),
iPromptPassword( EFalse ),
iDisplayTerminalWindow( EFalse ),
iUseCallBack( EFalse ),
iCallBackTypeIsServerNum( ECallbackActionMSCBCPAcceptServerSpecifiedNumber ),
iIspCallbackTimeOut( KCallBackTimeOut ),
iEnablePPPCompression( EFalse ),
iIsPasswordAuthenticationSecure( ETrue ),
iBearerCallTypeIsdn( ECallTypeAnalogue ),
iIspBearerCE( RMobileCall::EQoSNonTransparent ),
iGprsPdpType( EIPv4 ),
iIsReadOnly( EFalse ),
iApHasProxySettings( EFalse ),
iNetworkId( 0 ),
iApIapBearerService( RMobileCall::KCapsDataCircuitAsynchronous )

    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApAccessPointItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::ConstructL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ConstructL" ) ) );
   
    iExt = CApItemExtra::NewL();

    FeatureManager::InitializeLibL();
    iExt->iIsFeatureManagerInitialised = ETrue;

    iExt->iIsWcdma = 
        FeatureManager::FeatureSupported( KFeatureIdProtocolWcdma );

    iExt->iIsAppCsdSupport = 
        FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport );
#ifdef __TEST_CSD_SUPPORT
    iExt->iIsAppCsdSupport = ETrue;
#endif // __TEST_CSD_SUPPORT


    iExt->iIsIpv6Supported = 
        FeatureManager::FeatureSupported( KFeatureIdIPv6 );
#ifdef __TEST_IPV6_SUPPORT    
    iExt->iIsIpv6Supported = ETrue;
#endif //  __TEST_IPV6_SUPPORT    
    if( FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
        {
        iExt->iCdmaData = CApItemCdmaData::NewL();
        }
#ifdef __TEST_CDMA_SUPPORT
    if( !iExt->iCdmaData )
        {
        iExt->iCdmaData = CApItemCdmaData::NewL();
        }
#endif // __TEST_CDMA_SUPPORT

    if( FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        iExt->iWlanData = CApItemWlanData::NewL();
        }
#ifdef __TEST_WLAN_SUPPORT
    iExt->iWlanData = CApItemWlanData::NewL();
#endif // __TEST_WLAN_SUPPORT

    iSpecified = new (ELeave) CArrayFixFlat<TBool>( KApMembers );
    for (TInt i=0; i< KApMembers; i++)
        {
        iSpecified->AppendL( EFalse );
        }
    // as we are in ConstructL called from NewLC,
    // 'this' is already on the CleanupStack,
    // so do not push members onto it!
    iWapAccessPointName = HBufC::NewL( KApItemInitialTxtBufSize );
    iWapBearer = HBufC::NewL( KApItemInitialTxtBufSize );
    iStartingPage = HBufC::NewL( KApItemInitialTxtBufSize );
    iWapGatewayAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iIapName = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspName = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspDescription = HBufC::NewL( KApItemInitialTxtBufSize );
    iLoginScript = HBufC::NewL( KApItemInitialTxtBufSize );
    iUserName = HBufC::NewL( KApItemInitialTxtBufSize );

    iPassword = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfName = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfParams = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfNetworks = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfAuthName = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfAuthPass = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIfCallbackInfo = HBufC8::NewL( KApItemInitialTxtBufSize );
    iIspIPAddr = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspIPNetMask = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspGateway = HBufC::NewL( KApItemInitialTxtBufSize );

    iPrimaryDNS = HBufC::NewL( KApItemInitialTxtBufSize );
    iSecondaryDNS = HBufC::NewL( KApItemInitialTxtBufSize );
    iIspInitString = HBufC8::NewL( KApItemInitialTxtBufSize );
    iIspDefaultTelNumber = HBufC::NewL( KApItemInitialTxtBufSize );


    iIapServiceType = HBufC::NewL( KApItemInitialTxtBufSize );
    iGprsAccessPointName = HBufC::NewL( KApItemInitialTxtBufSize );
    iGprsPdpAddress = HBufC::NewL( KApItemInitialTxtBufSize );


    iApProxyProtocolName = HBufC::NewL( KApItemInitialTxtBufSize );
    iApProxyServerAddress = HBufC::NewL( KApItemInitialTxtBufSize );
    iApProxyExceptions = HBufC::NewL( KApItemInitialTxtBufSize );
    iNetworkName = HBufC::NewL( KApItemInitialTxtBufSize );

    iWapProxyLoginName = HBufC::NewL( KApItemInitialTxtBufSize );
    iWapProxyLoginPass = HBufC::NewL( KApItemInitialTxtBufSize );

    RFs fs;
    CleanupClosePushL<RFs>( fs );
    User::LeaveIfError( fs.Connect() );

    // can't use resource here because it is not added yet....
    TFileName resourceFile;
    resourceFile.Append( KApEngineResDriveZ );
    resourceFile.Append( KDC_RESOURCE_FILES_DIR );
    resourceFile.Append( KApEngineResFileName );
    BaflUtils::NearestLanguageFile( fs, resourceFile );

    RResourceFile rf;
    CleanupClosePushL<RResourceFile>( rf );
    rf.OpenL( fs, resourceFile );
    rf.ConfirmSignatureL( 0 );
    HBufC8* readBuffer = rf.AllocReadLC( R_APNG_DEFAULT_AP_NAME );
    // as we are expecting HBufC16...
    __ASSERT_DEBUG( ( readBuffer->Length()%2 ) == 0,
                    ApCommons::Panic( EWrongResourceFormat ) );
    const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                 ( readBuffer->Length() + 1 ) >> 1 );
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer ); // readBuffer
    CleanupStack::PushL( textBuffer );
    
    SetNamesL( *textBuffer );

    CleanupStack::PopAndDestroy( 3 ); // textbuffer, fs, rf

    WriteTextL( EApWapCurrentBearer, TPtrC(WAP_IP_BEARER) );    // required !

    WriteTextL( EApIapServiceType, TPtrC(OUTGOING_WCDMA) );      // required !


    WriteTextL( EApIspIfName, KDefGprsIfName );                 // required !
    WriteTextL( EApIspIfParams, KDefGprsIfParams );             // required !
    WriteTextL( EApIspIPGateway, KDefIspGateway );
    
    WriteTextL( EApWapGatewayAddress, KDefWapGatewayIpAddress );
    WriteTextL( EApIspIPAddr, KDefPhoneIpAddress );
    WriteTextL( EApIspIPNameServer1, KDefPrimaryNameServer );
    WriteTextL( EApIspIPNameServer2, KDefSecondaryNameServer );

    if ( iExt->iIsIpv6Supported )
        {
        WriteTextL( EApIP6NameServer1, KDefIp6PrimaryNameServer );
        WriteTextL( EApIP6NameServer2, KDefIp6SecondaryNameServer );
        }

    SetIfNetworksL();

    WriteTextL( EApProxyProtocolName, KDefProxyProtocolName );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ConstructL" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::ReadTextL( const TApMember aColumn,
                                           TDes8& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadTextL8" ) ) );

    LeaveIfUnsupportedL( aColumn );

    switch ( aColumn )
        {
        case EApIspIfCallbackInfo:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfCallbackInfo->Length() );
            aValue.Format( KFormat8, iIspIfCallbackInfo );
            break;
            }
        case EApIspInitString:
            {
            // make it large enough to hold
            aValue.SetLength( iIspInitString->Length() );
            aValue.Format( KFormat8, iIspInitString );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadTextL8" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::ReadTextL( const TApMember aColumn,
                                           TDes16& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadTextL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    switch ( aColumn )
        {
        case EApWapAccessPointName:
            {
            aValue.SetLength( iWapAccessPointName->Length() );
            aValue.Format( KFormat, iWapAccessPointName );
            break;
            }
        case EApWapCurrentBearer:
            {
            // make it large enough to hold
            aValue.SetLength( iWapBearer->Length() );
            aValue.Format( KFormat, iWapBearer );
            break;
            }
        case EApWapGatewayAddress:
            {
            // make it large enough to hold
            aValue.SetLength( iWapGatewayAddress->Length() );
            aValue.Format( KFormat, iWapGatewayAddress );
            break;
            }
        case EApIapName:
            {
            // make it large enough to hold
            aValue.SetLength( iIapName->Length() );
            aValue.Format( KFormat, iIapName );
            break;
            }
        case EApIapServiceType:
            {
            // make it large enough to hold
            aValue.SetLength( iIapServiceType->Length() );
            aValue.Format( KFormat, iIapServiceType );
            break;
            }
        case EApIspName:
            {
            // make it large enough to hold
            aValue.SetLength( iIspName->Length() );
            aValue.Format( KFormat, iIspName );
            break;
            }
        case EApIspDescription:
            {
            // make it large enough to hold
            aValue.SetLength( iIspDescription->Length() );
            aValue.Format( KFormat, iIspDescription );
            break;
            }
        case EApIspDefaultTelNumber:
            {
            // make it large enough to hold
            aValue.SetLength( iIspDefaultTelNumber->Length() );
            aValue.Format( KFormat, iIspDefaultTelNumber );
            break;
            }
        case EApIspLoginName:
            {
            // make it large enough to hold
            aValue.SetLength( iUserName->Length() );
            aValue.Format( KFormat, iUserName );
            break;
            }
        case EApIspLoginPass:
            {
            // make it large enough to hold
            aValue.SetLength( iPassword->Length() );
            aValue.Format( KFormat, iPassword );
            break;
            }
        case EApIspIfName:
        case EApGprsIfName:
        case EApCdmaIfName:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfName->Length() );
            aValue.Format( KFormat, iIspIfName );
            break;
            }
        case EApIspIfParams:
        case EApGprsIfParams:
        case EApCdmaIfParams:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfParams->Length() );
            aValue.Format( KFormat, iIspIfParams );
            break;
            }
        case EApIspIfNetworks:
        case EApGprsIfNetworks:
        case EApCdmaIfNetworks:
        case EApWlanIfNetworks:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfNetworks->Length() );
            aValue.Format( KFormat, iIspIfNetworks );
            break;
            }
        case EApIspIfAuthName:
        case EApGprsIfAuthName:
        case EApCdmaIfAuthName:
        case EApWlanIfAuthName:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfAuthName->Length() );
            aValue.Format( KFormat, iIspIfAuthName );
            break;
            }
        case EApIspIfAuthPass:
        case EApGprsIfAuthPassword:
        case EApCdmaIfAuthPassword:
        case EApWlanIfAuthPassword:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIfAuthPass->Length() );
            aValue.Format( KFormat, iIspIfAuthPass );
            break;
            }
        case EApIspIPAddr:
        case EApGprsIpAddr:
        case EApCdmaIpAddr:
        case EApWlanIpAddr:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIPAddr->Length() );
            aValue.Format( KFormat, iIspIPAddr );
            break;
            }
        case EApIspIPNetMask:
        case EApGprsIpNetMask:
        case EApCdmaIpNetMask:
        case EApWlanIpNetMask:
            {
            // make it large enough to hold
            aValue.SetLength( iIspIPNetMask->Length() );
            aValue.Format( KFormat, iIspIPNetMask );
            break;
            }
        case EApIspIPGateway:
        case EApGprsIpGateway:
        case EApCdmaIpGateway:
        case EApWlanIpGateway:
            {
            // make it large enough to hold
            aValue.SetLength( iIspGateway->Length() );
            aValue.Format( KFormat, iIspGateway );
            break;
            }
        case EApIspIPNameServer1:
        case EApGprsIPNameServer1:
        case EApCdmaIPNameServer1:
        case EApWlanIPNameServer1:
            {
            // make it large enough to hold
            aValue.SetLength( iPrimaryDNS->Length() );
            aValue.Format( KFormat, iPrimaryDNS );
            break;
            }
        case EApIspIPNameServer2:
        case EApGprsIPNameServer2:
        case EApCdmaIPNameServer2:
        case EApWlanIPNameServer2:
            {
            // make it large enough to hold
            aValue.SetLength( iSecondaryDNS->Length() );
            aValue.Format( KFormat, iSecondaryDNS );
            break;
            }
        case EApGprsPdpAddress:
            {
            // make it large enough to hold
            aValue.SetLength( iGprsPdpAddress->Length() );
            aValue.Format( KFormat, iGprsPdpAddress );
            break;
            }
        case EApProxyProtocolName:
            {
            aValue.SetLength( iApProxyProtocolName->Length() );
            aValue.Format( KFormat, iApProxyProtocolName );
            break;
            }
        case EApProxyLoginName:
            {
            aValue.SetLength( iWapProxyLoginName->Length() );
            aValue.Format( KFormat, iWapProxyLoginName );
            break;
            }
        case EApProxyLoginPass:
            {
            aValue.SetLength( iWapProxyLoginPass->Length() );
            aValue.Format( KFormat, iWapProxyLoginPass );
            break;
            }
        case EApNetworkName:
            {
            aValue.SetLength( iNetworkName->Length() );
            aValue.Format( KFormat, iNetworkName );
            break;
            }
        case EApIP6NameServer1:
            {
            // make it large enough to hold
            if ( iExt->iIsIpv6Supported )
                {
                aValue.SetLength( iExt->iIpv6PrimaryDNS->Length() );
                aValue.Format( KFormat, iExt->iIpv6PrimaryDNS );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApIP6NameServer2:
            {
            // make it large enough to hold
            if ( iExt->iIsIpv6Supported )
                {
                aValue.SetLength( iExt->iIpv6SecondaryDNS->Length() );
                aValue.Format( KFormat, iExt->iIpv6SecondaryDNS );
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        // CDMA2000
        case EApCdmaIwfName:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength( iExt->iCdmaData->iIwfName->Length() );
            aValue.Format( KFormat, iExt->iCdmaData->iIwfName );
            break;
            }
        case EApCdmaPdpAddress:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength( iExt->iCdmaData->iPdpAddress->Length() );
            aValue.Format( KFormat, iExt->iCdmaData->iPdpAddress );
            break;
            }
        case EApCdmaHomeAgentAddress:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength( iExt->iCdmaData->iHomeAgentAddress->Length() );
            aValue.Format( KFormat, iExt->iCdmaData->iHomeAgentAddress );
            break;
            }
        case EApCdmaMipPrimaryHomeAgent:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength
                ( iExt->iCdmaData->iMipPrimaryHomeAgent->Length() );
            aValue.Format
                ( KFormat, iExt->iCdmaData->iMipPrimaryHomeAgent );
            break;
            }
        case EApCdmaMipSecondaryHomeAgent:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength
                ( iExt->iCdmaData->iMipSecondaryHomeAgent->Length() );
            aValue.Format
                ( KFormat, iExt->iCdmaData->iMipSecondaryHomeAgent );
            break;
            }
        // END CDMA2000
        // WLAN
        case EApWlanNetworkName:
            {
            // LeaveIfUnsupported prevents getting here with no WLAN support.
            __ASSERT_ALWAYS \
                ( iExt->iWlanData, ApCommons::Panic( ESanityCheckFailed ) );
            aValue.SetLength
                ( iExt->iWlanData->iWlanNetworkName->Length() );
            aValue.Format
                ( KFormat, iExt->iWlanData->iWlanNetworkName );
            break;
            }
        // END WLAN
        // LANModem
        case EApLanBearerName:
            {
            aValue.SetLength( iExt->iLanBearerModemName->Length() );
            aValue.Format( KFormat, iExt->iLanBearerModemName );            
            break;
            }
        // END LANModem
        case EApIapBearerType:
            {
            aValue.SetLength( iExt->iIapBearerType->Length() );
            aValue.Format( KFormat, iExt->iIapBearerType );            
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadTextL" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadLongTextL
// ---------------------------------------------------------
//
EXPORT_C HBufC* CApAccessPointItem::ReadLongTextL( const TApMember aColumn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadLongTextL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    HBufC* retval = NULL;
    switch ( aColumn )
        {
        case EApWapStartPage:
            {
            retval = iStartingPage;
            break;
            }
        case EApIspLoginScript:
            {
            retval = iLoginScript;
            break;
            }
        case EApGprsAccessPointName:
            {
            retval = iGprsAccessPointName;
            break;
            }
        case EApProxyServerAddress:
            {
            retval = iApProxyServerAddress;
            break;
            }
        case EApProxyExceptions:
            {
            retval = iApProxyExceptions;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadLongTextL" ) ) );
    return retval;
    }




// ---------------------------------------------------------
// CApAccessPointItem::ReadConstLongTextL
// ---------------------------------------------------------
//
EXPORT_C const HBufC* CApAccessPointItem::ReadConstLongTextL
                                            ( const TApMember aColumn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadConstLongTextL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    HBufC* retval = NULL;
    switch ( aColumn )
        {
        case EApWapStartPage:
            {
            retval = iStartingPage;
            break;
            }
        case EApIspLoginScript:
            {
            retval = iLoginScript;
            break;
            }
        case EApGprsAccessPointName:
            {
            retval = iGprsAccessPointName;
            break;
            }
        case EApProxyServerAddress:
            {
            retval = iApProxyServerAddress;
            break;
            }
        case EApProxyExceptions:
            {
            retval = iApProxyExceptions;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            break;
            }
        }
    
    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadConstLongTextL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadTextLengthL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApAccessPointItem::ReadTextLengthL( const TApMember aColumn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadTextLengthL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    TUint32 l( 0 );
    switch ( aColumn )
        { // 16 first
        case EApWapAccessPointName:
            {
            l = iWapAccessPointName->Length();
            break;
            }
        case EApWapStartPage:
            {
            l = iStartingPage->Length();
            break;
            }
        case EApWapCurrentBearer:
            {
            l = iWapBearer->Length();
            break;
            }
        case EApWapGatewayAddress:
            {
            l = iWapGatewayAddress->Length();
            break;
            }
        case EApIapName:
            {
            l = iIapName->Length();
            break;
            }
        case EApIapServiceType:
            {
            l = iIapServiceType->Length();
            break;
            }
        case EApIspName:
            {
            l = iIspName->Length();
            break;
            }
        case EApIspDescription:
            {
            l = iIspDescription->Length();
            break;
            }
        case EApIspDefaultTelNumber:
            {
            l = iIspDefaultTelNumber->Length();
            break;
            }
        case EApIspLoginName:
            {
            l = iUserName->Length();
            break;
            }
        case EApIspLoginPass:
            {
            l = iPassword->Length();
            break;
            }
        case EApIspIfName:
        case EApGprsIfName:
        case EApCdmaIfName:
            {
            l = iIspIfName->Length();
            break;
            }
        case EApIspIfParams:
        case EApGprsIfParams:
        case EApCdmaIfParams:
            {
            l = iIspIfParams->Length();
            break;
            }
        case EApIspIfNetworks:
        case EApGprsIfNetworks:
        case EApCdmaIfNetworks:
        case EApWlanIfNetworks:
            {
            l = iIspIfNetworks->Length();
            break;
            }
        case EApIspIfAuthName:
        case EApGprsIfAuthName:
        case EApCdmaIfAuthName:
        case EApWlanIfAuthName:
            {
            l = iIspIfAuthName->Length();
            break;
            }
        case EApIspIfAuthPass:
        case EApGprsIfAuthPassword:
        case EApCdmaIfAuthPassword:
        case EApWlanIfAuthPassword:
            {
            l = iIspIfAuthPass->Length();
            break;
            }
        case EApIspIPAddr:
        case EApGprsIpAddr:
        case EApCdmaIpAddr:
        case EApWlanIpAddr:
            {
            l = iIspIPAddr->Length();
            break;
            }
        case EApIspIPNetMask:
        case EApGprsIpNetMask:
        case EApCdmaIpNetMask:
        case EApWlanIpNetMask:
            {
            l = iIspIPNetMask->Length();
            break;
            }
        case EApIspIPGateway:
        case EApGprsIpGateway:
        case EApCdmaIpGateway:
        case EApWlanIpGateway:
            {
            l = iIspGateway->Length();
            break;
            }
        case EApIspIPNameServer1:
        case EApGprsIPNameServer1:
        case EApCdmaIPNameServer1:
        case EApWlanIPNameServer1:
            {
            l = iPrimaryDNS->Length();
            break;
            }
        case EApIspIPNameServer2:
        case EApGprsIPNameServer2:
        case EApCdmaIPNameServer2:
        case EApWlanIPNameServer2:
            {
            l = iSecondaryDNS->Length();
            break;
            }
        case EApGprsAccessPointName:
            {
            l = iGprsAccessPointName->Length();
            break;
            }
        case EApGprsPdpAddress:
            {
            l = iGprsPdpAddress->Length();
            break;
            }
        // 8 bit ones
        case EApIspIfCallbackInfo:
            {
            l = iIspIfCallbackInfo->Length();
            break;
            }
        case EApIspInitString:
            {
            l = iIspInitString->Length();
            break;
            }
        // Login script
        case EApIspLoginScript:
            {
            l = iLoginScript->Length();
            break;
            }
        case EApProxyProtocolName:
            {
            l = iApProxyProtocolName->Length();
            break;
            }
        case EApProxyServerAddress:
            {
            l = iApProxyServerAddress->Length();
            break;
            }
        case EApProxyExceptions:
            {
            l = iApProxyExceptions->Length();
            break;
            }
        case EApProxyLoginName:
            {
            l = iWapProxyLoginName->Length();
            break;
            }
        case EApProxyLoginPass:
            {
            l = iWapProxyLoginPass->Length();
            break;
            }
        case EApNetworkName:
            {
            l = iNetworkName->Length();
            break;
            }
        case EApIP6NameServer1:
            {
            if ( iExt->iIsIpv6Supported )
                {
                l = iExt->iIpv6PrimaryDNS->Length();
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApIP6NameServer2:
            {
            if ( iExt->iIsIpv6Supported )
                {
                l = iExt->iIpv6SecondaryDNS->Length();
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        // CDMA2000
        case EApCdmaIwfName:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iCdmaData->iIwfName->Length();
            break;
            }
        case EApCdmaPdpAddress:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iCdmaData->iPdpAddress->Length();
            break;
            }
        case EApCdmaHomeAgentAddress:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iCdmaData->iHomeAgentAddress->Length();
            break;
            }
        case EApCdmaMipPrimaryHomeAgent:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iCdmaData->iMipPrimaryHomeAgent->Length();
            break;
            }
        case EApCdmaMipSecondaryHomeAgent:
            {
            // LeaveIfUnsupported prevents getting here with no CDMA support.
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iCdmaData->iMipSecondaryHomeAgent->Length();
            break;
            }
        // END CDMA2000
        // WLAN
        case EApWlanNetworkName:
            {
            // LeaveIfUnsupported prevents getting here with no WLAN support.
            __ASSERT_ALWAYS \
                ( iExt->iWlanData, ApCommons::Panic( ESanityCheckFailed ) );
            l = iExt->iWlanData->iWlanNetworkName->Length();
            break;
            }
        // END WLAN
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            User::Leave( KErrInvalidColumn );
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadTextLengthL" ) ) );
    return l;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadUint
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::ReadUint( const TApMember aColumn,
                                           TUint32& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadUint" ) ) );

    TInt retval ( KErrNone );

    if( !IsSupported( aColumn ) )
        {
        retval = KErrNotSupported;
        }
    else
        {
        switch ( aColumn )
            {
            case EApWapAccessPointID:
                {
                aValue = iWapUid;
                break;
                }
            case EApWapIap:
                {
                aValue = iWapIap;
                break;
                }
            case EApIapServiceId:
                {
                aValue = iIapServiceId;
                break;
                }
            case EApIapChargecard:
                {
                aValue = iIapChargeCardId;
                break;
                }
            case EApIspIspType:
            case EApCdmaApType:
                {
                aValue = iIspType;
                break;
                }
            case EApIspAuthRetries:
            case EApGprsIfAuthRetries:
            case EApCdmaIfAuthRetries:
            case EApWlanIfAuthRetries:
                {
                aValue = iIspIfAuthRetries;
                break;
                }
            case EApIspIfCallbackType:
                {
                aValue = iCallBackTypeIsServerNum;
                break;
                }
            case EApIspCallBackTimeOut:
                {
                aValue = iIspCallbackTimeOut;
                break;
                }
            case EApIspBearerName:
                {
                aValue = iIspBearerName;
                break;
                }
            case EApIspBearerSpeed:
                {
                aValue = iMaxConnSpeed;
                break;
                }
            case EApIspBearerCE:
                {
                aValue = iIspBearerCE;
                break;
                }
            case EApIspBearerCallTypeIsdn:
                {
                aValue = iBearerCallTypeIsdn;
                break;
                }
            case EApIspBearerType:
                {
                aValue = iIspBearerType;
                break;
                }
            case EApIspChannelCoding:
                {
                aValue = iIspChannelCoding;
                break;
                }
            case EApIspAIUR:
                {
                aValue = iIspAIUR;
                break;
                }
            case EApIspRequestedTimeSlots:
                {
                aValue = iIspRequestedTimeSlots;
                break;
                }
            case EApIspMaximumTimeSlots:
                {
                aValue = iIspMaximumTimeSlots;
                break;
                }
            case EApGprsPdpType:
                {
                aValue = iGprsPdpType;
                break;
                }
            case EApGprsReqPrecedence:
                {
                aValue = iGprsReqPrecedence;
                break;
                }
            case EApGprsReqDelay:
                {
                aValue = iGprsReqDelay;
                break;
                }
            case EApGprsReqReliability:
                {
                aValue = iGprsReqReliability;
                break;
                }
            case EApGprsReqPeakThroughput:
                {
                aValue = iGprsReqPeakThroughput;
                break;
                }
            case EApGprsReqMeanPeakThroughput:
                {
                aValue = iGprsReqMeanPeakThroughput;
                break;
                }
            case EApGprsMinPrecedence:
                {
                aValue = iGprsMinPrecedence;
                break;
                }
            case EApGprsMinDelay:
                {
                aValue = iGprsMinDelay;
                break;
                }
            case EApGprsMinReliability:
                {
                aValue = iGprsMinReliability;
                break;
                }
            case EApGprsMinPeakThroughput:
                {
                aValue = iGprsMinPeakThroughput;
                break;
                }
            case EApGprsMinMeanThroughput:
                {
                aValue = iGprsMinMeanThroughput;
                break;
                }
            case EApWapWspOption:
                {
                aValue = iIsConnectionTypeContinuous;
                break;
                }
            case EApProxyPortNumber:
                {
                aValue = iApProxyPortNumber;
                break;
                }
            case EApWapProxyPort:
                {
                aValue = iWapProxyPort;
                break;
                }
            case EApNetworkID:
                {
                aValue = iNetworkId;
                break;
                }
            case EApIapBearerService:
                {
                aValue = iApIapBearerService;
                break;
                }

    //* DEPRECATED !!!
            case EApWapIsp:
            case EApWapChargecard:
            case EApWapIspType:
                {
                __ASSERT_DEBUG( EFalse, \
                                User::Panic( kApSet, KErrNotSupported ) );
                retval = KErrNotSupported;
                break;
                }
    //* Deprecated ends
            // CDMA2000
            case EApCdmaServiceOption:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iServiceOption;
                break;
                }
            case EApCdmaPdpType:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iPdpType;
                break;
                }
            case EApCdmaReqFwdPriority:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqFwdPriority;
                break;
                }
            case EApCdmaReqRevPriority:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqRevPriority;
                break;
                }
            case EApCdmaReqFwdBitrate:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqFwdBitrate;
                break;
                }
            case EApCdmaReqRevBitrate:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqRevBitrate;
                break;
                }
            case EApCdmaReqFwdLoss:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqFwdLoss;
                break;
                }
            case EApCdmaReqRevLoss:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqRevLoss;
                break;
                }
            case EApCdmaReqFwdMaxDelay:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqFwdMaxDelay;
                break;
                }
            case EApCdmaReqRevMaxDelay:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iReqRevMaxDelay;
                break;
                }
            case EApCdmaMinFwdBitrate:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMinFwdBitrate;
                break;
                }
            case EApCdmaMinRevBitrate:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMinRevBitrate;
                break;
                }
            case EApCdmaAccptFwdLoss:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iAccptFwdLoss;
                break;
                }
            case EApCdmaAccptRevLoss:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iAccptRevLoss;
                break;
                }
            case EApCdmaAccptFwdMaxDelay:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iAccptFwdMaxDelay;
                break;
                }
            case EApCdmaAccptRevMaxDelay:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iAccptRevMaxDelay;
                break;
                }
            case EApCdmaQosWarningTimeout:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iQosWarningTimeout;
                break;
                }
            case EApCdmaRlpMode:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iRlpMode;
                break;
                }
            case EApCdmaMipTimeout:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipTimeout;
                break;
                }
            case EApCdmaNaiType:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iNaiType;
                break;
                }
            case EApCdmaSimpleIpAuthAlgorithm:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iSimpleIpAuthAlgorithm;
                break;
                }
            case EApCdmaSimpleIpPapSsHandle:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iSimpleIpPapSsHandle;
                break;
                }
            case EApCdmaSimpleIpChapSsHandle:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iSimpleIpChapSsHandle;
                break;
                }
            case EApCdmaMipTBit:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipTBit;
                break;
                }
            case EApCdmaMipMnAaaAuthAlgorithm:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnAaaAuthAlgorithm;
                break;
                }
            case EApCdmaMipMnAaaSpi:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnAaaSpi;
                break;
                }
            case EApCdmaMipMnAaaSsHandle:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnAaaSsHandle;
                break;
                }
            case EApCdmaMipMnHaAuthAlgorithm:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnHaAuthAlgorithm;
                break;
                }
            case EApCdmaMipMnHaSpi:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnHaSpi;
                break;
                }
            case EApCdmaMipMnHaSsHandle:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMipMnHaSsHandle;
                break;
                }
            // END CDMA2000
            // WLAN
            case EApWlanNetworkMode:
                {
                // IsSupported() check prevents getting here with no WLAN supp.
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iWlanData->iWlanNetworkMode;
                break;
                }
            case EApWlanSecurityMode:
                {
                // IsSupported() check prevents getting here with no WLAN supp.
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iWlanData->iWlanSecurityMode;
                break;
                }
            case EApWlanSettingsId:
                {
                // IsSupported() check prevents getting here with no WLAN supp.
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iWlanData->iWlanId;
                break;
                }
            case EApWlanChannelId:
                {
                // IsSupported() check prevents getting here with no WLAN supp.
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iWlanData->iChannelId;
                break;                
                }
            // END WLAN
            // LANModem
            case EApIapBearerID:
                {
                aValue = iExt->iIapBearerID;
                break;
                }
            // END LANModem            
            default :
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                retval = KErrInvalidColumn;
                break;
                }
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadUint" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::ReadBool
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::ReadBool( const TApMember aColumn,
                                           TBool& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReadBool" ) ) );
    
    TInt retval ( KErrNone );

    if( !IsSupported( aColumn ) )
        {
        retval = KErrNotSupported;
        }
    else
        {
        switch ( aColumn )
            {
            case EApWapSecurity:
                {
                aValue = iIsWTLSSecurityOn;
                break;
                }
            case EApIspDialResolution:
                {
                aValue = iIspDialResolution;
                break;
                }
            case EApIspUseLoginScript:
                {
                aValue = iUseLoginScript;
                break;
                }
            case EApIspPromptForLogin:
                {
                aValue = iPromptPassword;
                break;
                }
            case EApIspDisplayPCT:
                {
                aValue = iDisplayTerminalWindow;
                break;
                }
            case EApIspIfPromptForAuth:
            case EApGprsIfPromptForAuth:
            case EApCdmaIfPromptForAuth:
            case EApWlanIfPromptForAuth:
                {
                aValue = iIspIfPromptForAuth;
                break;
                }
            case EApIspIfCallbackEnabled:
                {
                aValue = iUseCallBack;
                break;
                }
            case EApIspIPAddrFromServer:
            case EApGprsIpAddrFromServer:
            case EApCdmaIpAddrFromServer:
            case EApWlanIpAddrFromServer:
                {
                aValue = iIspIPAddrFromServer;
                break;
                }
            case EApIspIPDnsAddrFromServer:
            case EApGprsIpDnsAddrFromServer:
            case EApCdmaIpDnsAddrFromServer:
            case EApWlanIpDnsAddrFromServer:
                {
                aValue = iGetDNSIPFromServer;
                break;
                }
            case EApIspEnableIpHeaderComp:
            case EApGprsHeaderCompression:
            case EApCdmaHeaderCompression:
                {
                aValue = iEnablePPPCompression;
                break;
                }
            case EApIspEnableLCPExtensions:
            case EApGprsEnableLCPExtensions:
            case EApCdmaEnableLCPExtensions:
                {
                aValue = iIspEnableLCPExtensions;
                break;
                }
            case EApIspDisablePlainTextAuth:
            case EApGprsDisablePlainTextAuth:
            case EApCdmaDisablePlainTextAuth:
                {
                aValue = iIsPasswordAuthenticationSecure;
                break;
                }
            case EApIspEnableSWCompression:
            case EApGprsDataCompression:
            case EApCdmaDataCompression:
                {
                aValue = iIspEnableSwCompression;
                break;
                }
// NOTE! This has been deprecated.
            case EApIspBearerCallTypeIsdn:
                {
                aValue = ( iBearerCallTypeIsdn != ECallTypeAnalogue );
                break;
                }
            case EApGprsUseAnonymAccess:
            case EApCdmaAnonymousAccess:
                {
                aValue = iGprsUseAnonymAccess;
                break;
                }
            case EApIsReadOnly:
                {
                aValue = iIsReadOnly;
                break;
                }
            case EApProxyUseProxy:
                {
                aValue = iApProxyUseProxy;
                break;
                }
            case EApHasProxySettings:
                {
                aValue = iApHasProxySettings;
                break;
                }
            case EApIP6DNSAddrFromServer:
                {
                if ( iExt->iIsIpv6Supported )
                    {
                    aValue = iExt->iIpv6GetDNSIPFromServer;
                    }
                else
                    {
                    retval = KErrNotSupported;
                    }
                break;
                }
            // CDMA2000
            case EApCdmaMip:
                {
                // IsSupported() check prevents getting here with no CDMA supp.
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                aValue = iExt->iCdmaData->iMip;
                break;
                }
            // END CDMA2000
            // WLAN
            case EApWlanScanSSID:
                {
                // IsSupported() check prevents getting here with no WLAN supp.
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );                
                aValue = iExt->iWlanData->iScanSSID;
                break;
                }
            // END WLAN
            default :
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                return KErrInvalidColumn;
                }
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReadBool" ) ) );
    return retval;
    }


// Update
// ---------------------------------------------------------
// CApAccessPointItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteTextL( const TApMember aColumn,
                                            const TDesC8& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::WriteTextL8" ) ) );

    LeaveIfUnsupportedL( aColumn );

    TInt ret = KErrNone;

    switch ( aColumn )
        {
        case EApIspIfCallbackInfo:
            {
            ReAllocL( iIspIfCallbackInfo, aValue );
            break;
            }
        case EApIspInitString:
            {
            ReAllocL( iIspInitString, aValue );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            ret = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::WriteTextL8" ) ) );
    return ret;
    }



// ---------------------------------------------------------
// CApAccessPointItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteTextL( const TApMember aColumn,
                                            const TDesC16& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::WriteTextL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    TInt retval( KErrNone );
    switch ( aColumn )
    {
        case EApWapAccessPointName:
            {
            HBufC* sgd = aValue.AllocLC();
            sgd->Des().Trim();
            ReAllocL( iWapAccessPointName, *sgd);
            CleanupStack::PopAndDestroy( sgd ); // sgd
            break;
            }
        case EApWapCurrentBearer:
            {
            ReAllocL( iWapBearer, aValue);
            break;
            }
        case EApWapGatewayAddress:
            {
            // this is an IP Address, must remove leading 0s from
            // the numbers
            HBufC *tmp = GetCleanIpAddressLC( aValue );
            ReAllocL( iWapGatewayAddress, *tmp );
            CleanupStack::PopAndDestroy( tmp );
            break;
            }
        case EApIapName:
            {
            ReAllocL( iIapName, aValue);
            break;
            }
        case EApIspName:
            {
            ReAllocL( iIspName, aValue);
            break;
            }
        case EApIapServiceType:
            {
            ReAllocL( iIapServiceType, aValue);
            //Check if ISP/GPRS has changed , update lfname &/or lfnetworks.!
            if ( ( *iIapServiceType == TPtrC(DIAL_IN_ISP) ) ||
                 ( *iIapServiceType == TPtrC(DIAL_OUT_ISP) ) )
                {
                WriteTextL( EApIspIfName, KDefIspIfName );
                }
            else if ( ( *iIapServiceType == TPtrC(OUTGOING_GPRS) ) ||
                     ( *iIapServiceType == TPtrC(INCOMING_GPRS) ) )
                {
                WriteTextL( EApIspIfName, KDefGprsIfName );
                }
            else
                { // unknown ISP type, do not know what to write, 
                // simply ignore it....
                }
            break;
            }
        case EApIspDescription:
            {
            ReAllocL( iIspDescription, aValue);
            break;
            }
        case EApIspDefaultTelNumber:
            {
            ReAllocL( iIspDefaultTelNumber, aValue);
            break;
            }
        case EApIspLoginName:
            {
            ReAllocL( iUserName, aValue);
            ReAllocL( iIspIfAuthName, aValue);
            break;
            }
        case EApIspLoginPass:
            {
            ReAllocL( iPassword, aValue);
            ReAllocL( iIspIfAuthPass, aValue);
            break;
            }
        case EApIspIfName:
        case EApGprsIfName:
        case EApCdmaIfName:
            {
            ReAllocL( iIspIfName, aValue);
            break;
            }
        case EApIspIfParams:
        case EApGprsIfParams:
        case EApCdmaIfParams:
            {
            ReAllocL( iIspIfParams, aValue);
            break;
            }
        case EApIspIfNetworks:
        case EApGprsIfNetworks:
        case EApCdmaIfNetworks:
        case EApWlanIfNetworks:
            {
            ReAllocL( iIspIfNetworks, aValue); // CAN NOT BE CHANGED
            break;
            }
        case EApIspIfAuthName:
        case EApGprsIfAuthName:
        case EApCdmaIfAuthName:
        case EApWlanIfAuthName:
            {
            ReAllocL( iIspIfAuthName, aValue);
            ReAllocL( iUserName, aValue);
            break;
            }
        case EApIspIfAuthPass:
        case EApGprsIfAuthPassword:
        case EApCdmaIfAuthPassword:
        case EApWlanIfAuthPassword:
            {
            ReAllocL( iIspIfAuthPass, aValue);
            ReAllocL( iPassword, aValue);
            break;
            }
        case EApIspIPAddr:
        case EApGprsIpAddr:
        case EApCdmaIpAddr:
        case EApWlanIpAddr:
            {
            ReAllocL( iIspIPAddr, aValue);
            SetIpFromServer();
            break;
            }
        case EApIspIPNetMask:
        case EApGprsIpNetMask:
        case EApCdmaIpNetMask:
        case EApWlanIpNetMask:
            {
            ReAllocL( iIspIPNetMask, aValue);
            break;
            }
        case EApIspIPGateway:
        case EApGprsIpGateway:
        case EApCdmaIpGateway:
        case EApWlanIpGateway:
            {
            ReAllocL( iIspGateway, aValue);
            break;
            }
        case EApIspIPNameServer1:
        case EApGprsIPNameServer1:
        case EApCdmaIPNameServer1:
        case EApWlanIPNameServer1:
            {
            ReAllocL( iPrimaryDNS, aValue);
            SetDnsIpFromServer();
            break;
            }
        case EApIspIPNameServer2:
        case EApGprsIPNameServer2:
        case EApCdmaIPNameServer2:
        case EApWlanIPNameServer2:
            {
            ReAllocL( iSecondaryDNS, aValue);
            SetDnsIpFromServer();
            break;
            }
        case EApGprsPdpAddress:
            {
            ReAllocL( iGprsPdpAddress, aValue );
            break;
            }
        case EApProxyProtocolName:
            {
            ReAllocL( iApProxyProtocolName, aValue );
            SetProxyFlag();
            break;
            }
        case EApNetworkName:
            {
            ReAllocL( iNetworkName, aValue );
            break;
            }
        case EApProxyLoginName:
            {
            ReAllocL( iWapProxyLoginName, aValue );
            break;
            }
        case EApProxyLoginPass:
            {
            ReAllocL( iWapProxyLoginPass, aValue );
            break;
            }
        case EApIP6NameServer1:
            {
            if ( iExt->iIsIpv6Supported )
                {
                ReAllocL( iExt->iIpv6PrimaryDNS, aValue);
                SetDns6IpFromServer();
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        case EApIP6NameServer2:
            {
            if ( iExt->iIsIpv6Supported )
                {
                ReAllocL( iExt->iIpv6SecondaryDNS, aValue);
                SetDns6IpFromServer();
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        // CDMA2000
        case EApCdmaIwfName:
            {
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iCdmaData->iIwfName, aValue );
            break;
            }
        case EApCdmaPdpAddress:
            {
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iCdmaData->iPdpAddress, aValue );
            break;
            }
        case EApCdmaHomeAgentAddress:
            {
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iCdmaData->iHomeAgentAddress, aValue );
            break;
            }
        case EApCdmaMipPrimaryHomeAgent:
            {
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iCdmaData->iMipPrimaryHomeAgent, aValue );
            break;
            }
        case EApCdmaMipSecondaryHomeAgent:
            {
            __ASSERT_ALWAYS \
                ( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iCdmaData->iMipSecondaryHomeAgent, aValue );
            break;
            }
        // END CDMA2000
        // WLAN
        case EApWlanNetworkName:
            {
            __ASSERT_ALWAYS \
                ( iExt->iWlanData, ApCommons::Panic( ESanityCheckFailed ) );
            ReAllocL( iExt->iWlanData->iWlanNetworkName, aValue );
            break;
            }
        // END WLAN
        // LANModem
        case EApLanBearerName:
            {
            ReAllocL( iExt->iLanBearerModemName, aValue );
            break;
            }
        // END LANModem
        case EApIapBearerType:
            {
            ReAllocL( iExt->iIapBearerType, aValue );
            break;
            }        
        default:
            {
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::WriteTextL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::WriteLongTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteLongTextL( const TApMember aColumn,
                                                const TDesC& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::WriteLongTextL" ) ) );

    LeaveIfUnsupportedL( aColumn );

    TInt retval( KErrNone );
    switch ( aColumn )
        {
        case EApGprsAccessPointName:
            {
            ReAllocL( iGprsAccessPointName, aValue);
            break;
            }
        case EApWapStartPage:
            {
            ReAllocL( iStartingPage, aValue);
            break;
            }
        case EApIspLoginScript:
            {
            ReAllocL( iLoginScript, aValue );
            break;
            }
        case EApProxyServerAddress:
            {
            ReAllocL( iApProxyServerAddress, aValue );
            SetProxyFlag();
            break;
            }
        case EApProxyExceptions:
            {
            ReAllocL( iApProxyExceptions, aValue );
            SetProxyFlag();
            break;
            }
        default:
            {
            // unknown column...
            __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
            retval = KErrInvalidColumn;
            break;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::WriteLongTextL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::WriteUint
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteUint( const TApMember aColumn,
                                            const TUint32& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::WriteUint" ) ) );

    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/
    TInt retval( KErrNone );

    if ( !IsSupported( aColumn ) )
        {
        retval = KErrNotSupported;
        }
    else
        {
        switch ( aColumn )
            {
            case EApWapAccessPointID:
                {
                iWapUid = aValue;
                break;
                }
            case EApWapIap:
                {
                iWapIap = aValue;
                break;
                }
            case EApIapServiceId:
                {
                iIapServiceId = aValue;
                break;
                }
            case EApIapChargecard:
                {
                iIapChargeCardId = aValue;
                break;
                }
            case EApIspIspType:
            case EApCdmaApType:
                {
                iIspType = (TCommsDbIspType)aValue;
                break;
                }
            case EApIspAuthRetries:
            case EApGprsIfAuthRetries:
            case EApCdmaIfAuthRetries:
            case EApWlanIfAuthRetries:
                {
                iIspIfAuthRetries = aValue;
                break;
                }
            case EApIspIfCallbackType:
                {
                iCallBackTypeIsServerNum = TCallbackAction( aValue );
                break;
                }
            case EApIspCallBackTimeOut:
                {
                iIspCallbackTimeOut = aValue;
                break;
                }
            case EApIspBearerName:
                {
                iIspBearerName = 
                    RMobileCall::TMobileCallDataServiceCaps( aValue );
                break;
                }
            case EApIspBearerSpeed:
                {
                iMaxConnSpeed = TApCallSpeed( aValue );
                break;
                }
            case EApIspBearerCE:
                {
                //iIspBearerCE = RMobileCall::TMobileCallDataQoSCaps( aValue );
                iIspBearerCE = RMobileCall::TMobileCallDataQoS( aValue );
                break;
                }
            case EApIspBearerCallTypeIsdn:
                {
                /*
                // DO NOT CHECK VALIDITY HERE AS COMMSDB STORES IT DIFFERENTLY
                // AFTER READED FROM THE DB, IT WILL BE CHECKED!!!!!
                // ensure it is in the valid range
                TUint32 tempint = aValue;
                tempint = Min( tempint, TUint32(ECallTypeISDNv120) );
                tempint = Max( tempint, TUint32(ECallTypeAnalogue) );
                */
                iBearerCallTypeIsdn = (TApCallType)aValue;
                break;
                }
            case EApIspBearerType:
                {
                iIspBearerType = aValue;
                break;
                }
            case EApIspChannelCoding:
                {
                iIspChannelCoding = aValue;
                break;
                }
            case EApIspAIUR:
                {
                iIspAIUR = aValue;
                break;
                }
            case EApIspRequestedTimeSlots:
                {
                iIspRequestedTimeSlots = aValue;
                break;
                }
            case EApIspMaximumTimeSlots:
                {
                iIspMaximumTimeSlots = aValue;
                break;
                }

            case EApGprsPdpType:
                {
                iGprsPdpType = aValue;
                TRAP_IGNORE( SetIfNetworksL() );
                break;
                }
            case EApGprsReqPrecedence:
                {
                iGprsReqPrecedence = aValue;
                break;
                }
            case EApGprsReqDelay:
                {
                iGprsReqDelay = aValue;
                break;
                }
            case EApGprsReqReliability:
                {
                iGprsReqReliability = aValue;
                break;
                }
            case EApGprsReqPeakThroughput:
                {
                iGprsReqPeakThroughput = aValue;
                break;
                }
            case EApGprsReqMeanPeakThroughput:
                {
                iGprsReqMeanPeakThroughput = aValue;
                break;
                }
            case EApGprsMinPrecedence:
                {
                iGprsMinPrecedence = aValue;
                break;
                }
            case EApGprsMinDelay:
                {
                iGprsMinDelay = aValue;
                break;
                }
            case EApGprsMinReliability:
                {
                iGprsMinReliability = aValue;
                break;
                }
            case EApGprsMinPeakThroughput:
                {
                iGprsMinPeakThroughput = aValue;
                break;
                }
            case EApGprsMinMeanThroughput:
                {
                iGprsMinMeanThroughput = aValue;
                break;
                }
            case EApWapWspOption:
                {
                iIsConnectionTypeContinuous = aValue;
                break;
                }
            case EApProxyPortNumber:
                {
                iApProxyPortNumber = aValue;
                SetProxyFlag();
                break;
                }
            case EApWapProxyPort:
                {
                iWapProxyPort = aValue;
                break;
                }
            case EApNetworkID:
                { // now it is supported to set it from the outside...
                iNetworkId = aValue;
                break;
                }
            case EApIapBearerService:
                {
                iApIapBearerService = aValue;
                break;
                }

    // DEPRECATED !!!
            case EApWapIsp:
            case EApWapChargecard:
            case EApWapIspType:
                {
                __ASSERT_DEBUG( EFalse, \
                                User::Panic( kApSet, KErrNotSupported ) );
                retval = KErrNotSupported;
                break;
                }
            // CDMA2000
            case EApCdmaServiceOption:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iServiceOption = aValue;
                break;
                }
            case EApCdmaPdpType:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iPdpType = aValue;
                TRAP_IGNORE( SetIfNetworksL() );
                break;
                }
            case EApCdmaReqFwdPriority:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqFwdPriority = aValue;
                break;
                }
            case EApCdmaReqRevPriority:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqRevPriority = aValue;
                break;
                }
            case EApCdmaReqFwdBitrate:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqFwdBitrate = aValue;
                break;
                }
            case EApCdmaReqRevBitrate:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqRevBitrate = aValue;
                break;
                }
            case EApCdmaReqFwdLoss:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqFwdLoss = aValue;
                break;
                }
            case EApCdmaReqRevLoss:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqRevLoss = aValue;
                break;
                }
            case EApCdmaReqFwdMaxDelay:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqFwdMaxDelay = aValue;
                break;
                }
            case EApCdmaReqRevMaxDelay:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iReqRevMaxDelay = aValue;
                break;
                }
            case EApCdmaMinFwdBitrate:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMinFwdBitrate = aValue;
                break;
                }
            case EApCdmaMinRevBitrate:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMinRevBitrate = aValue;
                break;
                }
            case EApCdmaAccptFwdLoss:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iAccptFwdLoss = aValue;
                break;
                }
            case EApCdmaAccptRevLoss:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iAccptRevLoss = aValue;
                break;
                }
            case EApCdmaAccptFwdMaxDelay:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iAccptFwdMaxDelay = aValue;
                break;
                }
            case EApCdmaAccptRevMaxDelay:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iAccptRevMaxDelay = aValue;
                break;
                }
            case EApCdmaQosWarningTimeout:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iQosWarningTimeout = aValue;
                break;
                }
            case EApCdmaRlpMode:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iRlpMode = aValue;
                break;
                }
            case EApCdmaMipTimeout:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipTimeout = aValue;
                break;
                }
            case EApCdmaNaiType:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iNaiType = aValue;
                break;
                }
            case EApCdmaSimpleIpAuthAlgorithm:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iSimpleIpAuthAlgorithm = aValue;
                break;
                }
            case EApCdmaSimpleIpPapSsHandle:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iSimpleIpPapSsHandle = aValue;
                break;
                }
            case EApCdmaSimpleIpChapSsHandle:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iSimpleIpChapSsHandle = aValue;
                break;
                }
            case EApCdmaMipTBit:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipTBit = aValue;
                break;
                }
            case EApCdmaMipMnAaaAuthAlgorithm:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnAaaAuthAlgorithm = aValue;
                break;
                }
            case EApCdmaMipMnAaaSpi:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnAaaSpi = aValue;
                break;
                }
            case EApCdmaMipMnAaaSsHandle:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnAaaSsHandle = aValue;
                break;
                }
            case EApCdmaMipMnHaAuthAlgorithm:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnHaAuthAlgorithm = aValue;
                break;
                }
            case EApCdmaMipMnHaSpi:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnHaSpi = aValue;
                break;
                }
            case EApCdmaMipMnHaSsHandle:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMipMnHaSsHandle = aValue;
                break;
                }
            // END CDMA2000
            // WLAN
            case EApWlanNetworkMode:
                {
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iWlanData->iWlanNetworkMode = aValue;
                break;
                }
            case EApWlanSecurityMode:
                {
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iWlanData->iWlanSecurityMode = aValue;
                break;
                }
            case EApWlanSettingsId:
                {
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iWlanData->iWlanId = aValue;
                break;
                }
            case EApWlanChannelId:
                {
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iWlanData->iChannelId = aValue;
                break;
                }
            // END WLAN
            // LANModem
            case EApIapBearerID:
                {
                iExt->iIapBearerID = aValue;
                break;
                }
            // END LANModem
            default :
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                retval = KErrInvalidColumn;
                }
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::WriteUint" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::WriteBool
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteBool( const TApMember aColumn,
                                            const TBool& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::WriteBool" ) ) );

    TInt retval( KErrNone );

    if ( !IsSupported( aColumn ) )
        {
        retval = KErrNotSupported;
        }
    else
        {
        switch ( aColumn )
            {
            case EApWapSecurity:
                {
                iIsWTLSSecurityOn = aValue;
                break;
                }
            case EApIspDialResolution:
                {
                iIspDialResolution = aValue;
                break;
                }
            case EApIspUseLoginScript:
                {
                iUseLoginScript = aValue;
                break;
                }
            case EApIspPromptForLogin:
                {
                iPromptPassword = aValue;
                iIspIfPromptForAuth = aValue;
                break;
                }
            case EApIspDisplayPCT:
                {
                iDisplayTerminalWindow = EFalse;
                break;
                }
            case EApIspIfPromptForAuth:
            case EApGprsIfPromptForAuth:
            case EApCdmaIfPromptForAuth:
            case EApWlanIfPromptForAuth:
                {
                iIspIfPromptForAuth = aValue;
                iPromptPassword = aValue;
                break;
                }
            case EApIspIfCallbackEnabled:
                {
                iUseCallBack = aValue;
                break;
                }
            case EApIspIPAddrFromServer:
            case EApGprsIpAddrFromServer:
            case EApCdmaIpAddrFromServer:
            case EApWlanIpAddrFromServer:
                {
                iIspIPAddrFromServer = aValue;
                break;
                }
            case EApIspIPDnsAddrFromServer:
            case EApGprsIpDnsAddrFromServer:
            case EApCdmaIpDnsAddrFromServer:
            case EApWlanIpDnsAddrFromServer:
                {
                iGetDNSIPFromServer = aValue;
                break;
                }
            case EApIspEnableIpHeaderComp:
            case EApGprsHeaderCompression:
            case EApCdmaHeaderCompression:
                {
                iEnablePPPCompression = aValue;
                break;
                }
            case EApIspEnableLCPExtensions:
            case EApGprsEnableLCPExtensions:
            case EApCdmaEnableLCPExtensions:
                {
                iIspEnableLCPExtensions = aValue;
                break;
                }
            case EApIspDisablePlainTextAuth:
            case EApGprsDisablePlainTextAuth:
            case EApCdmaDisablePlainTextAuth:
                {
                iIsPasswordAuthenticationSecure = aValue;
                break;
                }
            case EApIspEnableSWCompression:
            case EApGprsDataCompression:
            case EApCdmaDataCompression:
                {
                iIspEnableSwCompression = aValue;
                break;
                }
    // NOTE! This is deprecated.
            case EApIspBearerCallTypeIsdn:
                {
                iBearerCallTypeIsdn = 
                    TApCallType( aValue != ECallTypeAnalogue );
                break;
                }
            case EApGprsUseAnonymAccess:
            case EApCdmaAnonymousAccess:
                {
                iGprsUseAnonymAccess = aValue;
                break;
                }
            case EApIsReadOnly:
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                retval = KErrInvalidColumn;
                break;
                }
            case EApProxyUseProxy:
                {
                iApProxyUseProxy = aValue;
                SetProxyFlag();
                break;
                }
            case EApHasProxySettings:
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                retval = KErrNotSupported;
                break;
                }
            case EApIP6DNSAddrFromServer:
                {
                if ( iExt->iIsIpv6Supported )
                    {
                    iExt->iIpv6GetDNSIPFromServer = aValue;
                    }
                else
                    {
                    retval = KErrNotSupported;
                    }
                break;
                }
            // CDMA2000
            case EApCdmaMip:
                {
                __ASSERT_ALWAYS ( iExt->iCdmaData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iCdmaData->iMip = aValue;
                break;
                }
            // END CDMA2000
            // WLAN
            case EApWlanScanSSID:
                {
                __ASSERT_ALWAYS ( iExt->iWlanData, \
                    ApCommons::Panic( ESanityCheckFailed ) );
                iExt->iWlanData->iScanSSID = aValue;
                break;
                }
            // END WLAN
            default :
                {
                __ASSERT_DEBUG( EFalse, ApCommons::Panic( EInvalidColumn ) );
                retval = KErrInvalidColumn;
                break;
                }
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::WriteBool" ) ) );
    return retval;
    }



// Query
// COMMON

// ---------------------------------------------------------
// CApAccessPointItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApAccessPointItem::WapUid() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::WapUid" ) ) );

    return iWapUid;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ConnectionName
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApAccessPointItem::ConnectionName() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::ConnectionName" ) ) );

    return *iWapAccessPointName;
    }


// WAP-spec.
// ---------------------------------------------------------
// CApAccessPointItem::WapBearer
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApAccessPointItem::WapBearer() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::WapBearer" ) ) );

    return *iWapBearer;
    }



// WAP-spec.
// ---------------------------------------------------------
// CApAccessPointItem::BearerTypeL
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApAccessPointItem::BearerTypeL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::BearerTypeL" ) ) );

    TApBearerType retval( EApBearerTypeAllBearers );
    // decide which bearer, read bearer spec. data.
    if ( *iWapBearer == TPtrC(WAP_IP_BEARER) )
        {
        if ( ( *iIapServiceType == TPtrC(DIAL_OUT_ISP) ) ||
             ( *iIapServiceType == TPtrC(DIAL_IN_ISP) ) )
            {
            // now check speed
            if ( iIspBearerType == EBearerTypeCSD )
                {
                retval = EApBearerTypeCSD;
                }
            else
                {
                if ( iIspBearerType == EBearerTypeHSCSD )
                    {
                    retval = EApBearerTypeHSCSD;
                    }
                else
                    {
                    User::Leave( KErrInvalidBearer );
                    }
                }
            }
        else
            {
            if ( ( *iIapServiceType == TPtrC(OUTGOING_WCDMA) ) ||
                 ( *iIapServiceType == TPtrC(INCOMING_WCDMA) ) )
                {
                retval = EApBearerTypeGPRS;
                }
            else
                { // some other bearer, currently only VPN...
                if ( iExt->iIsVpnAp )
                    {
                    // in this case get REAL IAP's bearer type...
                    retval = iExt->iVPnRealIapBearerType;
                    }
                else
                    {
                    if ( iExt->iWlanData )
                        {                        
                        // check if it is WLAN...
                        if ( *iIapServiceType == TPtrC(LAN_SERVICE) )
                            {
                            if ( IsWlan() )
                                {
                                retval = EApBearerTypeWLAN;
                                }
                            else
                                {
                                if ( *iExt->iLanBearerModemName == 
                                        KModemBearerLANModem )
                                    {// IPPass Through
                                    retval = EApBearerTypeLANModem;
                                    }
                                else
                                    {                                
                        #ifdef __TEST_LAN_BEARER
                                    retval = EApBearerTypeLAN;
                        #else
                                    User::Leave( KErrInvalidBearer );
                        #endif // __TEST_LAN_BEARER
                                    }
                                }
                            }
                        else
                            { // otherwise, it is not known, invalid...
                            User::Leave( KErrInvalidBearer );
                            }
                        }
                    else
                        {
                        if ( *iExt->iLanBearerModemName == 
                                KModemBearerLANModem )
                            {// IPPass Through
                            retval = EApBearerTypeLANModem;
                            }
                        else
                            {
                    #ifdef __TEST_LAN_BEARER
                        retval = EApBearerTypeLAN;
                    #else // __TEST_LAN_BEARER
                        User::Leave( KErrInvalidBearer );
                    #endif // __TEST_LAN_BEARER
                            }
                        }
                    }
                }
            }
        }
    else
        {
        User::Leave( KErrInvalidBearer );
        }
    if ( retval == EApBearerTypeAllBearers )
        {
        User::Leave( KErrInvalidBearer );
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::BearerTypeL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetBearerTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::SetBearerTypeL( TApBearerType aBearer )
    {    
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetBearerTypeL" ) ) );

    HBufC* tmpbuf;
    switch ( aBearer )
        {
        case EApBearerTypeGPRS: // GPRS
            {
            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(OUTGOING_WCDMA) );
            WriteTextL( EApIspIfParams, KDefGprsIfParams );      // required !
            if( iExt->iWlanData )
                {
                iExt->iWlanData->iIsWlan = EFalse;
                }
            WriteTextL( EApLanBearerName, KModemBearerGPRS );
            break;
            }
        case EApBearerTypeCSD: // CSD
            {
            if ( !iExt->iIsAppCsdSupport )
                {
                User::Leave( KErrNotSupported );
                }

            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(DIAL_OUT_ISP) );            
            WriteTextL( EApIspIfParams, KEmpty );             // required !
            iIspBearerType = EBearerTypeCSD;            
            if( iExt->iWlanData )
                {
                iExt->iWlanData->iIsWlan = EFalse;
                }            
            WriteTextL( EApLanBearerName, KModemBearerCSD );
            LimitIp6DNSL();            
            break;
            }
        case EApBearerTypeHSCSD: // HSCSD
            {
            if ( !iExt->iIsAppHscsdSupport || !iExt->iIsAppCsdSupport )
                {
                User::Leave( KErrNotSupported );
                }
            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(DIAL_OUT_ISP) );
            WriteTextL( EApIspIfParams, KEmpty );             // required !
            iIspBearerType = EBearerTypeHSCSD;
            if( iExt->iWlanData )
                {
                iExt->iWlanData->iIsWlan = EFalse;
                }            
            WriteTextL( EApLanBearerName, KModemBearerCSD );
            LimitIp6DNSL();
            break;
            }
        case EApBearerTypeWLAN: // WLAN
            { 
            if( !iExt->iWlanData )
                {
                User::Leave( KErrNotSupported );
                }
            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(LAN_SERVICE) );
            WriteTextL( EApIspIfParams, KEmpty );             // required !
            WriteTextL( EApLanBearerName, KModemBearerWLAN );
            iExt->iWlanData->iIsWlan = ETrue;
            break;
            }
#ifdef __TEST_LAN_BEARER
        case EApBearerTypeLAN: // LAN
            { 
            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(LAN_SERVICE) );
            WriteTextL( EApIspIfParams, KEmpty );             // required !
            if( iExt->iWlanData )
                {
                iExt->iWlanData->iIsWlan = EFalse;
                }            
            WriteTextL( EApLanBearerName, KModemBearerLAN );
            break;
            }
#endif // __TEST_LAN_BEARER
        case EApBearerTypeLANModem: // LAN modem
            { 
            tmpbuf = TPtrC(WAP_IP_BEARER).AllocL();
            delete iWapBearer;
            iWapBearer = tmpbuf;
            WriteTextL( EApIapServiceType, TPtrC(LAN_SERVICE) );
            WriteTextL( EApIspIfParams, KEmpty );             // required !
            if( iExt->iWlanData )
                {
                iExt->iWlanData->iIsWlan = EFalse;
                }            
            WriteTextL( EApLanBearerName, KModemBearerLANModem );
            break;
            }            
        default:
            {
            User::Leave( KErrInvalidBearer );
            break;
            }
        }
    LimitSpeedL();

    SetIfNetworksL();

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetBearerTypeL" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetNamesL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::SetNamesL( const TDesC16& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetNamesL" ) ) );

    WriteTextL( EApWapAccessPointName, aValue );
    WriteTextL( EApIapName, aValue );
    WriteTextL( EApIspName, aValue );
    WriteTextL( EApNetworkName, aValue );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetNamesL" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SanityCheckOk
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::SanityCheckOk()
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::SanityCheckOk" ) ) );

    TBool retval( ETrue );
    // as iWApIspType had been deprecated, this method also...
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::HasSameIspAndBearer
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::HasSameIspAndBearerL(
                                 CApAccessPointItem& aItem )
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApAccessPointItem::HasSameIspAndBearerL" ) ) );

    TBool retval( EFalse );
    TApBearerType bearer = BearerTypeL();
    if ( bearer == aItem.BearerTypeL() )
        {
        switch ( bearer )
            {
            case EApBearerTypeCSD:
            case EApBearerTypeHSCSD:
                {
                // compare phone number, username, pwd
                // first compare phone numbers
                if ( IsSamePhoneNumberL( aItem ) )
                    {
                    TBool pwd1;
                    ReadBool( EApIspPromptForLogin, pwd1 );
                    TBool pwd2;
                    aItem.ReadBool( EApIspPromptForLogin, pwd2 );
                    if ( ( !pwd1 ) && ( !pwd2 ) )
                        {
                        HBufC* tmpuser = HBufC::NewLC( 
                                aItem.ReadTextLengthL( EApIspLoginName ) );
                        HBufC* tmppwd = HBufC::NewLC( 
                                aItem.ReadTextLengthL( EApIspLoginPass ) );
                        TPtr16 tmpptruser( tmpuser->Des() );
                        TPtr16 tmpptrpwd( tmppwd->Des() );
                        aItem.ReadTextL( EApIspLoginName, tmpptruser );
                        aItem.ReadTextL( EApIspLoginPass, tmpptrpwd );
                        if ( ( iUserName->Compare( *tmpuser ) == 0 )
                            && (iPassword->Compare( *tmppwd ) == 0 ) )
                            {
                            retval = ETrue;
                            }
                        // tmpuser, tmppw
                        CleanupStack::PopAndDestroy( 2, tmpuser ); 
                        }
                    }
                break;
                }
            case EApBearerTypeGPRS:
                {
                // compare APN, (username/pwd?)
                if ( aItem.ReadConstLongTextL( EApGprsAccessPointName )
                    ->Compare( *iGprsAccessPointName ) == 0 )
                    {
                    retval = ETrue;
                    }
                break;
                }
            case EApBearerTypeCDMA:
                {
                retval = ETrue;
                break;
                }
            case EApBearerTypeWLAN:
                {
                if( iExt->iWlanData )
                    {                    
                    retval = ETrue;
                    }
                else
                    {
                    User::Leave( KErrNotSupported );
                    }
                break;
                }
#ifdef __TEST_LAN_BEARER
            case EApBearerTypeLAN:
                {
                retval = ETrue;
                break;
                }
#endif // __TEST_LAN_BEARER
            case EApBearerTypeLANModem:
                {
                retval = ETrue;
                break;
                }
            default:
                {
                User::Leave( KErrInvalidBearer );
                break;
                }
            }
        }

    CLOG( ( EApItem, 1, 
            _L( "<- CApAccessPointItem::HasSameIspAndBearerL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::IsReadOnly() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::IsReadOnly" ) ) );

    return iIsReadOnly;
    }




// ================= OTHER EXPORTED FUNCTIONS ==============


// =============== OTHER NON - EXPORTED FUNCTIONS ============
// ---------------------------------------------------------
// CApAccessPointItem::IsVpnAp
// ---------------------------------------------------------
//
TBool CApAccessPointItem::IsVpnAp() const
    {
    return iExt->iIsVpnAp;
    }


// ---------------------------------------------------------
// CApAccessPointItem::SetVpnAp
// ---------------------------------------------------------
//
void CApAccessPointItem::SetVpnAp( TBool aIsVpn )
    {
    iExt->iIsVpnAp = aIsVpn;
    }

// ---------------------------------------------------------
// CApAccessPointItem::Specified
// ---------------------------------------------------------
//
TBool CApAccessPointItem::Specified( TApMember aMember )
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::Specified" ) ) );

    return iSpecified->At( aMember );
    }


// ---------------------------------------------------------
// CApAccessPointItem::Specify
// ---------------------------------------------------------
//
void CApAccessPointItem::Specify( TApMember aMember, TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::Specify" ) ) );

    iSpecified->At( aMember ) = aOn;
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyAll
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyAll( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyAll" ) ) );

    for( TInt i=0; i< KApMembers; i++ )
        {
        iSpecified->At( i ) = aOn;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyAll" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SpecifyWap
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyWap( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyWap" ) ) );

    iSpecified->At( EApWapAccessPointID ) = aOn;
    iSpecified->At( EApWapAccessPointName ) = aOn;
    iSpecified->At( EApWapCurrentBearer ) = aOn;
    iSpecified->At( EApWapStartPage ) = aOn;
    iSpecified->At( EApWapGatewayAddress ) = aOn;
    iSpecified->At( EApWapWspOption ) = aOn;
    iSpecified->At( EApWapSecurity ) = aOn;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyWap" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SpecifyIpBearer
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyIpBearer( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyIpBearer" ) ) );

//* DEPRECATED
    iSpecified->At( EApWapIsp ) = EFalse;
//* DEPRECATED
    iSpecified->At( EApWapChargecard ) = EFalse;
    iSpecified->At( EApWapIap ) = aOn;
//* DEPRECATED
    iSpecified->At( EApWapIspType ) = EFalse;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyIpBearer" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyIAP
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyIAP( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyIAP" ) ) );

    iSpecified->At( EApWapIap ) = aOn;
    iSpecified->At( EApIapName ) = aOn;
    iSpecified->At( EApIapServiceType ) = aOn;
    iSpecified->At( EApIapServiceId ) = aOn;
    iSpecified->At( EApIapChargecard ) = aOn;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyIAP" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyISP
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyISP( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyISP" ) ) );

    iSpecified->At( EApIspName ) = aOn;
    iSpecified->At( EApIspDescription ) = aOn;
    iSpecified->At( EApIspIspType ) = aOn;

    iSpecified->At( EApIspDefaultTelNumber ) = aOn;
    iSpecified->At( EApIspDialResolution ) = aOn;
    iSpecified->At( EApIspUseLoginScript ) = aOn;
    iSpecified->At( EApIspLoginScript ) = aOn;
    iSpecified->At( EApIspPromptForLogin ) = aOn;
    iSpecified->At( EApIspLoginName ) = aOn;
    iSpecified->At( EApIspLoginPass ) = aOn;
    iSpecified->At( EApIspDisplayPCT ) = aOn;
    iSpecified->At( EApIspIfName ) = aOn;
    iSpecified->At( EApIspIfParams ) = aOn;
    iSpecified->At( EApIspIfNetworks ) = aOn;
    iSpecified->At( EApIspIfPromptForAuth ) = aOn;
    iSpecified->At( EApIspIfAuthName ) = aOn;
    iSpecified->At( EApIspIfAuthPass ) = aOn;
    iSpecified->At( EApIspAuthRetries ) = aOn;
    iSpecified->At( EApIspIfCallbackEnabled ) = aOn;
    iSpecified->At( EApIspIfCallbackType ) = aOn;
    iSpecified->At( EApIspIfCallbackInfo ) = aOn;
    iSpecified->At( EApIspCallBackTimeOut ) = aOn;
    iSpecified->At( EApIspIPAddrFromServer ) = aOn;
    iSpecified->At( EApIspIPAddr ) = aOn;
    iSpecified->At( EApIspIPNetMask ) = aOn;
    iSpecified->At( EApIspIPGateway ) = aOn;
    iSpecified->At( EApIspIPDnsAddrFromServer ) = aOn;
    iSpecified->At( EApIspIPNameServer1 ) = aOn;
    iSpecified->At( EApIspIPNameServer2 ) = aOn;
    iSpecified->At( EApIspEnableIpHeaderComp ) = aOn;
    iSpecified->At( EApIspEnableLCPExtensions ) = aOn;
    iSpecified->At( EApIspDisablePlainTextAuth ) = aOn;
    iSpecified->At( EApIspEnableSWCompression ) = aOn;
    iSpecified->At( EApIspBearerName ) = aOn;
    iSpecified->At( EApIspBearerSpeed ) = aOn;
    iSpecified->At( EApIspBearerCallTypeIsdn ) = aOn;
    iSpecified->At( EApIspBearerCE ) = aOn;
    iSpecified->At( EApIspInitString ) = aOn;
    iSpecified->At( EApIspBearerType ) = aOn;
    iSpecified->At( EApIspChannelCoding ) = aOn;
    iSpecified->At( EApIspAIUR ) = aOn;
    iSpecified->At( EApIspRequestedTimeSlots ) = aOn;
    iSpecified->At( EApIspMaximumTimeSlots ) = aOn;

    if ( iExt->iIsIpv6Supported )
        {
        iSpecified->At( EApIP6DNSAddrFromServer ) = aOn;
        iSpecified->At( EApIP6NameServer1 ) = aOn;
        iSpecified->At( EApIP6NameServer2 ) = aOn;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyISP" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyGPRS
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyGPRS( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyGPRS" ) ) );

    iSpecified->At( EApGprsAccessPointName ) = aOn;
    iSpecified->At( EApGprsPdpType ) = aOn;
    iSpecified->At( EApGprsPdpAddress ) = aOn;
    iSpecified->At( EApGprsReqPrecedence ) = aOn;
    iSpecified->At( EApGprsReqDelay ) = aOn;
    iSpecified->At( EApGprsReqReliability ) = aOn;
    iSpecified->At( EApGprsReqPeakThroughput ) = aOn;
    iSpecified->At( EApGprsReqMeanPeakThroughput ) = aOn;
    iSpecified->At( EApGprsMinPrecedence ) = aOn;
    iSpecified->At( EApGprsMinDelay ) = aOn;
    iSpecified->At( EApGprsMinReliability ) = aOn;
    iSpecified->At( EApGprsMinPeakThroughput ) = aOn;
    iSpecified->At( EApGprsMinMeanThroughput ) = aOn;
    iSpecified->At( EApGprsUseAnonymAccess ) = aOn;

    // followings are mapped to ISP... fields
    iSpecified->At( EApGprsDataCompression ) = aOn;
    iSpecified->At( EApGprsHeaderCompression ) = aOn;
    iSpecified->At( EApGprsIfName ) = aOn;
    iSpecified->At( EApGprsIfParams ) = aOn;
    iSpecified->At( EApGprsIfNetworks ) = aOn;
    iSpecified->At( EApGprsIfPromptForAuth ) = aOn;
    iSpecified->At( EApGprsIfAuthName ) = aOn;
    iSpecified->At( EApGprsIfAuthPassword ) = aOn;
    iSpecified->At( EApGprsIfAuthRetries ) = aOn;
    iSpecified->At( EApGprsIpNetMask ) = aOn;
    iSpecified->At( EApGprsIpGateway ) = aOn;
    iSpecified->At( EApGprsIpAddrFromServer ) = aOn;
    iSpecified->At( EApGprsIpAddr ) = aOn;
    iSpecified->At( EApGprsIpDnsAddrFromServer ) = aOn;
    iSpecified->At( EApGprsIPNameServer1 ) = aOn;
    iSpecified->At( EApGprsIPNameServer2 ) = aOn;
    iSpecified->At( EApGprsEnableLCPExtensions ) = aOn;
    iSpecified->At( EApGprsDisablePlainTextAuth ) = aOn;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyGPRS" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyCDMA
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyCDMA( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyCDMA" ) ) );

    __ASSERT_DEBUG( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );

    iSpecified->At( EApCdmaIwfName ) = aOn;
    iSpecified->At( EApCdmaServiceOption ) = aOn;
    iSpecified->At( EApCdmaPdpType ) = aOn;
    iSpecified->At( EApCdmaPdpAddress ) = aOn;
    iSpecified->At( EApCdmaReqFwdPriority ) = aOn;
    iSpecified->At( EApCdmaReqRevPriority ) = aOn;
    iSpecified->At( EApCdmaReqFwdBitrate ) = aOn;
    iSpecified->At( EApCdmaReqRevBitrate ) = aOn;
    iSpecified->At( EApCdmaReqFwdLoss ) = aOn;
    iSpecified->At( EApCdmaReqRevLoss ) = aOn;
    iSpecified->At( EApCdmaReqFwdMaxDelay ) = aOn;
    iSpecified->At( EApCdmaReqRevMaxDelay ) = aOn;
    iSpecified->At( EApCdmaMinFwdBitrate ) = aOn;
    iSpecified->At( EApCdmaMinRevBitrate ) = aOn;
    iSpecified->At( EApCdmaAccptFwdLoss ) = aOn;
    iSpecified->At( EApCdmaAccptRevLoss ) = aOn;
    iSpecified->At( EApCdmaAccptFwdMaxDelay ) = aOn;
    iSpecified->At( EApCdmaAccptRevMaxDelay ) = aOn;
    // CDMA2000, mapped to ISP... fields
    iSpecified->At( EApCdmaDataCompression ) = aOn;
    iSpecified->At( EApCdmaHeaderCompression ) = aOn;
    iSpecified->At( EApCdmaAnonymousAccess ) = aOn;
    iSpecified->At( EApCdmaIfName ) = aOn;
    iSpecified->At( EApCdmaIfParams ) = aOn;
    iSpecified->At( EApCdmaIfNetworks ) = aOn;
    iSpecified->At( EApCdmaIfPromptForAuth ) = aOn;
    iSpecified->At( EApCdmaIfAuthName ) = aOn;
    iSpecified->At( EApCdmaIfAuthPassword ) = aOn;
    iSpecified->At( EApCdmaIfAuthRetries ) = aOn;
    iSpecified->At( EApCdmaIpNetMask ) = aOn;
    iSpecified->At( EApCdmaIpGateway ) = aOn;
    iSpecified->At( EApCdmaIpAddrFromServer ) = aOn;
    iSpecified->At( EApCdmaIpAddr ) = aOn;
    iSpecified->At( EApCdmaIpDnsAddrFromServer ) = aOn;
    iSpecified->At( EApCdmaIPNameServer1 ) = aOn;
    iSpecified->At( EApCdmaIPNameServer2 ) = aOn;
    iSpecified->At( EApCdmaEnableLCPExtensions ) = aOn;
    iSpecified->At( EApCdmaDisablePlainTextAuth ) = aOn;
    // CDMA2000
    iSpecified->At( EApCdmaApType ) = aOn;
    iSpecified->At( EApCdmaQosWarningTimeout ) = aOn;
    iSpecified->At( EApCdmaRlpMode ) = aOn;
    // CDMA2000 deprecated Mobile IP fields
    iSpecified->At( EApCdmaMip ) = aOn;
    iSpecified->At( EApCdmaHomeAgentAddress ) = aOn;
    iSpecified->At( EApCdmaMipTimeout ) = aOn;
    // CDMA2000 specific CDMA parameters provisioned through OTA
    iSpecified->At( EApCdmaNaiType ) = aOn;
    iSpecified->At( EApCdmaSimpleIpAuthAlgorithm ) = aOn;
    iSpecified->At( EApCdmaSimpleIpPapSsHandle ) = aOn;
    iSpecified->At( EApCdmaSimpleIpChapSsHandle ) = aOn;
    iSpecified->At( EApCdmaMipTBit ) = aOn;
    iSpecified->At( EApCdmaMipHomeAddress ) = aOn;
    iSpecified->At( EApCdmaMipPrimaryHomeAgent ) = aOn;
    iSpecified->At( EApCdmaMipSecondaryHomeAgent ) = aOn;
    iSpecified->At( EApCdmaMipMnAaaAuthAlgorithm ) = aOn;
    iSpecified->At( EApCdmaMipMnAaaSpi ) = aOn;
    iSpecified->At( EApCdmaMipMnAaaSsHandle ) = aOn;
    iSpecified->At( EApCdmaMipMnHaAuthAlgorithm ) = aOn;
    iSpecified->At( EApCdmaMipMnHaSpi ) = aOn;
    iSpecified->At( EApCdmaMipMnHaSsHandle ) = aOn;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyCDMA" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyWLAN
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyWLAN( TBool aOn )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SpecifyWLAN" ) ) );

    __ASSERT_DEBUG( iExt->iWlanData, ApCommons::Panic( ESanityCheckFailed ) );

    // WLAN, mapped to ISP... fields
    iSpecified->At( EApWlanIfNetworks ) = aOn;
    iSpecified->At( EApWlanIfPromptForAuth ) = aOn;
    iSpecified->At( EApWlanIfAuthName ) = aOn;
    iSpecified->At( EApWlanIfAuthPassword ) = aOn;
    iSpecified->At( EApWlanIfAuthRetries ) = aOn;
    iSpecified->At( EApWlanIpNetMask ) = aOn;
    iSpecified->At( EApWlanIpGateway ) = aOn;
    iSpecified->At( EApWlanIpAddrFromServer ) = aOn;
    iSpecified->At( EApWlanIpAddr ) = aOn;
    iSpecified->At( EApWlanIpDnsAddrFromServer ) = aOn;
    iSpecified->At( EApWlanIPNameServer1 ) = aOn;
    iSpecified->At( EApWlanIPNameServer2 ) = aOn;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SpecifyWLAN" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::SetDnsIpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetDnsIpFromServer()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetDnsIpFromServer" ) ) );

    if ( ( iPrimaryDNS->Compare( KDynIpAddress ) == 0 ) &&
         ( iSecondaryDNS->Compare( KDynIpAddress ) == 0 ) )
        {
        iGetDNSIPFromServer = ETrue;
        }
    else
        {
        iGetDNSIPFromServer = EFalse;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetDnsIpFromServer" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetIpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetIpFromServer()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetIpFromServer" ) ) );

    if ( iIspIPAddr->Compare( KDynIpAddress ) == 0 )
        {
        iIspIPAddrFromServer = ETrue;
        }
    else
        {
        iIspIPAddrFromServer = EFalse;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetIpFromServer" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetDns6IpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetDns6IpFromServer()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetDns6IpFromServer" ) ) );

    if ( iExt->iIsIpv6Supported )
        {
        if ( ( iExt->iIpv6PrimaryDNS->Compare( KDynIpv6Address ) == 0 ) &&
             ( iExt->iIpv6SecondaryDNS->Compare( KDynIpv6Address ) == 0 ) )
            {
            
            TApBearerType bt(EApBearerTypeGPRS);
            TInt err(KErrNone);
            TRAP( err, bt = BearerTypeL() );
            if ( err == KErrNone )
                {
                switch (bt) 
                    {
                    case EApBearerTypeCSD:
                    case EApBearerTypeHSCSD:
                        { // consider it user defined
                        iExt->iIpv6GetDNSIPFromServer = EFalse; 
                        break;
                        }
                    default:
                        {
                        iExt->iIpv6GetDNSIPFromServer = ETrue;
                        break;
                        }
                    }
                }
            }
        else
            {
            iExt->iIpv6GetDNSIPFromServer = EFalse;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetDns6IpFromServer" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReAllocL
// ---------------------------------------------------------
//
void CApAccessPointItem::ReAllocL(HBufC8*& aDes, const TDesC8& aValue)
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReAllocL8" ) ) );

    // now remove 'invalid' chars (TABs) as they cause problems
    // in other places, like listboxes...
    HBufC8* tmp = RemoveInvalidCharsLC( aValue );
    delete aDes;
    aDes = tmp;
    CleanupStack::Pop( tmp );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReAllocL8" ) ) );
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReAllocL
// ---------------------------------------------------------
//
void CApAccessPointItem::ReAllocL(HBufC*& aDes, const TDesC16& aValue)
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::ReAllocL" ) ) );

    // now remove 'invalid' chars (TABs) as they cause problems
    // in other places, like listboxes...
    HBufC* tmp = RemoveInvalidCharsLC( aValue );
    delete aDes;
    aDes = tmp;
    CleanupStack::Pop( tmp );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::ReAllocL" ) ) );
    }



// ---------------------------------------------------------
// CApAccessPointItem::DoCompareApL
// ---------------------------------------------------------
//
TBool CApAccessPointItem::DoCompareApL( const CApAccessPointItem& aItem )const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::DoCompareApL" ) ) );

    TBool retval( EFalse );
    // sorry for those CONST_CASTS, but could not do it otherwise
    // without API breask....
    CApAccessPointItem* thisitem = CONST_CAST( CApAccessPointItem*, this );
    CApAccessPointItem* otheritem = CONST_CAST( CApAccessPointItem*, &aItem );

    TApBearerType thisbearer = thisitem->BearerTypeL();
    if ( thisbearer == otheritem->BearerTypeL() )
        { // if diff. bearers, compareing might give false results
        // compare WAP:
            // expect IAP ID
        if ( CompareWap( aItem ) )
            {
            switch ( thisbearer )
                {
                case EApBearerTypeCSD:
                case EApBearerTypeHSCSD:
                    {
                    // compare IAP
                    if ( CompareIap( aItem ) )
                        {
                        // compare ISP
                        if ( CompareIsp( aItem ) )
                            {
                            if ( CompareProxy( aItem ) )
                                {
                                retval = ETrue;
                                }
                            }
                        }
                    break;
                    }
                case EApBearerTypeGPRS:
                    {
                    // compare IAP
                    if ( CompareIap( aItem ) )
                        {
                        // comapre GPRS
                        if ( CompareGprs( aItem ) )
                            {
                            if ( CompareProxy( aItem ) )
                                {
                                retval = ETrue;
                                }
                            }
                        }
                    break;
                    }
                case EApBearerTypeCDMA:
                    {
                    // compare IAP
                    if ( CompareIap( aItem ) )
                        {
                        // comapre CDMA
                        if ( CompareCdma( aItem ) )
                            {
                            if ( CompareProxy( aItem ) )
                                {
                                retval = ETrue;
                                }
                            }
                        }
                    break;
                    }
                case EApBearerTypeWLAN:
                    { 
                    if ( iExt->iWlanData )
                        {
                        // Compare Iap
                        if ( CompareIap( aItem ) )
                            {
                            // Check compareing for WLAN and LAN!!!
                            // Compare WLAN
                            if ( CompareWlan( aItem ) )
                                {
                                if ( CompareProxy( aItem ) )
                                    {
                                    retval = ETrue;
                                    }
                                }
                            }
                        }
                    else
                        {
                        User::Leave( KErrNotSupported );
                        }
                    break;
                    }
                default:
                    {
                    // KErrInvalidBearer, but can not Leave here,
                    // so just return False
                    // retval is already False, nothing to do
                    break;
                    }
                }
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::DoCompareApL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareWap
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareWap( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareWap" ) ) );

    TBool retval( EFalse );
// WAP AP Table
/*
WAP_CURRENT_BEARER
WAP_START_PAGE
*/

// WAP IP Bearer Table
/*
    -- WAP_ACCESS_POINT_ID
WAP_GATEWAY_ADDRESS
    -- WAP_IAP
    -- WAP_ISP
    -- WAP_CHARGECARD
WAP_ISP_TYPE
WAP_WSP_OPTION
WAP_SECURITY
*/

    if ( BOOL_COMPARE( iIsWTLSSecurityOn, aItem.iIsWTLSSecurityOn )
       && ( iIsConnectionTypeContinuous == aItem.iIsConnectionTypeContinuous )
       )
        {
        if (
           ( iWapAccessPointName->Compare( *aItem.iWapAccessPointName ) == 0 )
           && ( iWapBearer->Compare( *aItem.iWapBearer ) == 0 )
           && ( iStartingPage->Compare( *aItem.iStartingPage ) == 0 )
           && ( iWapGatewayAddress->Compare( *aItem.iWapGatewayAddress ) == 0 )
           && ( iWapProxyLoginName->Compare(
                                        *aItem.iWapProxyLoginName ) == 0 )
           && ( iWapProxyLoginPass->Compare(
                                        *aItem.iWapProxyLoginPass ) == 0 )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareWap" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareIap
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareIap( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareIap" ) ) );

    TBool retval( EFalse );
// WAP IAP Table
/*
COMMDB_NAME
-- IAP_SERVICE                                      _S("IAPService")
IAP_SERVICE_TYPE                                _S("IAPServiceType")
-- IAP_CHARGECARD                                   _S("Chargecard")
*/
    if (
       ( iIapName->Compare( *aItem.iIapName ) == 0 )
        && ( iIapServiceType->Compare( *aItem.iIapServiceType ) == 0 )
        && ( iNetworkName->Compare( *aItem.iNetworkName ) == 0 )
        && ( iNetworkId == aItem.iNetworkId )
        )
        {
        retval = ETrue;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareIap" ) ) );
    return retval;
    }




// ---------------------------------------------------------
// CApAccessPointItem::CompareIsp
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareIsp( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareIsp" ) ) );

    TBool retval( EFalse );
    if (
        BOOL_COMPARE( iIspDialResolution , aItem.iIspDialResolution )
        && BOOL_COMPARE( iUseLoginScript, aItem.iUseLoginScript )
        && BOOL_COMPARE( iPromptPassword, aItem.iPromptPassword )
        && BOOL_COMPARE( iDisplayTerminalWindow, aItem.iDisplayTerminalWindow )
        && BOOL_COMPARE( iIspIfPromptForAuth, aItem.iIspIfPromptForAuth )
        && iIspIfAuthRetries == aItem.iIspIfAuthRetries
        && BOOL_COMPARE( iUseCallBack, aItem.iUseCallBack )
        && iCallBackTypeIsServerNum == aItem.iCallBackTypeIsServerNum
        && iIspCallbackTimeOut == aItem.iIspCallbackTimeOut
        && BOOL_COMPARE( iIspIPAddrFromServer, aItem.iIspIPAddrFromServer )
        && BOOL_COMPARE( iGetDNSIPFromServer, aItem.iGetDNSIPFromServer )
        && BOOL_COMPARE( iEnablePPPCompression, aItem.iEnablePPPCompression )
        && BOOL_COMPARE( iIspEnableLCPExtensions,
                                aItem.iIspEnableLCPExtensions )
        && BOOL_COMPARE( iIsPasswordAuthenticationSecure,
                                aItem.iIsPasswordAuthenticationSecure )
        && BOOL_COMPARE( iIspEnableSwCompression, 
                                aItem.iIspEnableSwCompression )
        && iIspBearerName == aItem.iIspBearerName
        && iMaxConnSpeed == aItem.iMaxConnSpeed
        && iIspBearerCE == aItem.iIspBearerCE
        && iApIapBearerService == aItem.iApIapBearerService
        && iIspBearerType == aItem.iIspBearerType
        && iBearerCallTypeIsdn == aItem.iBearerCallTypeIsdn
        && iIspChannelCoding == aItem.iIspChannelCoding
        && iIspAIUR == aItem.iIspAIUR
        && iIspRequestedTimeSlots == aItem.iIspRequestedTimeSlots
        && iIspMaximumTimeSlots == aItem.iIspMaximumTimeSlots
        && ( 
            ( iExt->iIsIpv6Supported 
            && ( BOOL_COMPARE( iExt->iIpv6GetDNSIPFromServer,
                                 aItem.iExt->iIpv6GetDNSIPFromServer ) ) )
            || !iExt->iIsIpv6Supported )
        )
        {
        if (
            ( iIspName->Compare( *aItem.iIspName ) == 0 )
            && ( iIspDescription->Compare( *aItem.iIspDescription ) == 0 )
            &&
            ( iIspDefaultTelNumber->Compare( 
                        *aItem.iIspDefaultTelNumber ) == 0 )
            && ( iLoginScript->Compare( *aItem.iLoginScript ) == 0 )
            && ( iUserName->Compare( *aItem.iUserName ) == 0 )
            && ( iPassword->Compare( *aItem.iPassword ) == 0 )
            && ( iIspIfName->Compare( *aItem.iIspIfName ) == 0 )
            && ( iIspIfParams->Compare( *aItem.iIspIfParams ) == 0 )
            && ( iIspIfNetworks->Compare( *aItem.iIspIfNetworks ) == 0 )
            && ( iIspIfAuthName->Compare( *aItem.iIspIfAuthName ) == 0 )
            && ( iIspIfAuthPass->Compare( *aItem.iIspIfAuthPass ) == 0 )
            &&
            ( iIspIfCallbackInfo->Compare( *aItem.iIspIfCallbackInfo ) == 0 )
            && ( iIspIPAddr->Compare( *aItem.iIspIPAddr ) == 0 )
            && ( iIspIPNetMask->Compare( *aItem.iIspIPNetMask ) == 0 )
            && ( iIspGateway->Compare( *aItem.iIspGateway ) == 0 )
            && ( iPrimaryDNS->Compare( *aItem.iPrimaryDNS ) == 0 )
            && ( iSecondaryDNS->Compare( *aItem.iSecondaryDNS ) == 0 )
            && ( iIspInitString->Compare( *aItem.iIspInitString ) == 0 )
            && (
               ( iExt->iIsIpv6Supported && 
                    ( 
                        ( iExt->iIpv6PrimaryDNS->Compare( 
                                *aItem.iExt->iIpv6PrimaryDNS ) == 0 )
                        && 
                        ( iExt->iIpv6SecondaryDNS->Compare( 
                                *aItem.iExt->iIpv6SecondaryDNS ) == 0 )
                    )
                || !iExt->iIsIpv6Supported )
                )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareIsp" ) ) );
    return retval;
    }




// ---------------------------------------------------------
// CApAccessPointItem::CompareGprs
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareGprs( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareGprs" ) ) );

    TBool retval( EFalse );

    if (
        iGprsPdpType == aItem.iGprsPdpType
//
        && BOOL_COMPARE( iIspIfPromptForAuth, aItem.iIspIfPromptForAuth )
        && iIspIfAuthRetries == aItem.iIspIfAuthRetries
        && BOOL_COMPARE( iIspIPAddrFromServer, aItem.iIspIPAddrFromServer )
        && BOOL_COMPARE( iGetDNSIPFromServer, aItem.iGetDNSIPFromServer )
        && BOOL_COMPARE( iEnablePPPCompression, aItem.iEnablePPPCompression )
        && BOOL_COMPARE( iIspEnableLCPExtensions, 
                                aItem.iIspEnableLCPExtensions )
        && BOOL_COMPARE( iIsPasswordAuthenticationSecure,
                                aItem.iIsPasswordAuthenticationSecure )
        && BOOL_COMPARE( iIspEnableSwCompression, 
                                aItem.iIspEnableSwCompression )
//
        && iGprsReqPrecedence == aItem.iGprsReqPrecedence
        && iGprsReqDelay == aItem.iGprsReqDelay
        && iGprsReqReliability == aItem.iGprsReqReliability
        && iGprsReqPeakThroughput == aItem.iGprsReqPeakThroughput
        && iGprsReqMeanPeakThroughput == aItem.iGprsReqMeanPeakThroughput
        && iGprsMinPrecedence == aItem.iGprsMinPrecedence
        && iGprsMinDelay == aItem.iGprsMinDelay
        && iGprsMinReliability == aItem.iGprsMinReliability
        && iGprsMinPeakThroughput == aItem.iGprsMinPeakThroughput
        && iGprsMinMeanThroughput == aItem.iGprsMinMeanThroughput
        && BOOL_COMPARE( iGprsUseAnonymAccess, aItem.iGprsUseAnonymAccess )
        && ( 
            ( iExt->iIsIpv6Supported 
            && (BOOL_COMPARE( iExt->iIpv6GetDNSIPFromServer,
                                 aItem.iExt->iIpv6GetDNSIPFromServer ) ) )
            || ( !iExt->iIsIpv6Supported ) )
        )
        {
        if (
            ( iGprsAccessPointName->Compare( 
                        *aItem.iGprsAccessPointName ) == 0 )
//
            && ( iIspIfName->Compare( *aItem.iIspIfName ) == 0 )
            && ( iIspIfParams->Compare( *aItem.iIspIfParams ) == 0 )
            && ( iIspIfNetworks->Compare( *aItem.iIspIfNetworks ) == 0 )
            && ( iIspIfAuthName->Compare( *aItem.iIspIfAuthName ) == 0 )
            && ( iIspIfAuthPass->Compare( *aItem.iIspIfAuthPass ) == 0 )
            && ( iIspIPAddr->Compare( *aItem.iIspIPAddr ) == 0 )
            && ( iIspIPNetMask->Compare( *aItem.iIspIPNetMask ) == 0 )
            && ( iIspGateway->Compare( *aItem.iIspGateway ) == 0 )
            && ( iPrimaryDNS->Compare( *aItem.iPrimaryDNS ) == 0 )
            && ( iSecondaryDNS->Compare( *aItem.iSecondaryDNS ) == 0 )
//
            && ( iGprsPdpAddress->Compare( *aItem.iGprsPdpAddress ) == 0 )  
            && (
               ( iExt->iIsIpv6Supported && 
                    ( 
                        ( iExt->iIpv6PrimaryDNS->Compare( 
                                *aItem.iExt->iIpv6PrimaryDNS ) == 0 )
                        && 
                        ( iExt->iIpv6SecondaryDNS->Compare( 
                                *aItem.iExt->iIpv6SecondaryDNS ) == 0 )
                    )
                || !iExt->iIsIpv6Supported )
                )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareGprs" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareCdma
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareCdma( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareCdma" ) ) );

    TBool retval( EFalse );

    __ASSERT_ALWAYS( iExt->iCdmaData, ApCommons::Panic( ESanityCheckFailed ) );

    if (
           iExt->iCdmaData->iServiceOption == 
                                aItem.iExt->iCdmaData->iServiceOption
        && iExt->iCdmaData->iPdpType == aItem.iExt->iCdmaData->iPdpType
        && iExt->iCdmaData->iReqFwdPriority == 
                                aItem.iExt->iCdmaData->iReqFwdPriority
        && iExt->iCdmaData->iReqRevPriority == 
                                aItem.iExt->iCdmaData->iReqRevPriority
        && iExt->iCdmaData->iReqFwdBitrate == 
                                aItem.iExt->iCdmaData->iReqFwdBitrate
        && iExt->iCdmaData->iReqRevBitrate == 
                                aItem.iExt->iCdmaData->iReqRevBitrate
        && iExt->iCdmaData->iReqFwdLoss == aItem.iExt->iCdmaData->iReqFwdLoss
        && iExt->iCdmaData->iReqRevLoss == aItem.iExt->iCdmaData->iReqRevLoss
        && iExt->iCdmaData->iReqFwdMaxDelay == 
                                aItem.iExt->iCdmaData->iReqFwdMaxDelay
        && iExt->iCdmaData->iReqRevMaxDelay == 
                                aItem.iExt->iCdmaData->iReqRevMaxDelay
        && iExt->iCdmaData->iMinFwdBitrate == 
                                aItem.iExt->iCdmaData->iMinFwdBitrate
        && iExt->iCdmaData->iMinRevBitrate == 
                                aItem.iExt->iCdmaData->iMinRevBitrate
        && iExt->iCdmaData->iAccptFwdLoss == 
                                aItem.iExt->iCdmaData->iAccptFwdLoss
        && iExt->iCdmaData->iAccptRevLoss == 
                                aItem.iExt->iCdmaData->iAccptRevLoss
        && iExt->iCdmaData->iAccptFwdMaxDelay == 
                                aItem.iExt->iCdmaData->iAccptFwdMaxDelay
        && iExt->iCdmaData->iAccptRevMaxDelay == 
                                aItem.iExt->iCdmaData->iAccptRevMaxDelay
//
        && BOOL_COMPARE( iIspIfPromptForAuth, aItem.iIspIfPromptForAuth )
        && iIspIfAuthRetries == aItem.iIspIfAuthRetries
        && BOOL_COMPARE( iIspIPAddrFromServer, aItem.iIspIPAddrFromServer )
        && BOOL_COMPARE( iGetDNSIPFromServer, aItem.iGetDNSIPFromServer )
        && BOOL_COMPARE( iEnablePPPCompression, aItem.iEnablePPPCompression )
        && BOOL_COMPARE( iIspEnableLCPExtensions, 
                                aItem.iIspEnableLCPExtensions )
        && BOOL_COMPARE( iIsPasswordAuthenticationSecure,
                                aItem.iIsPasswordAuthenticationSecure )
        && BOOL_COMPARE( iIspEnableSwCompression, 
                                aItem.iIspEnableSwCompression )
//
        && iIspType == aItem.iIspType
        && iExt->iCdmaData->iQosWarningTimeout == 
                                aItem.iExt->iCdmaData->iQosWarningTimeout
        && iExt->iCdmaData->iRlpMode == aItem.iExt->iCdmaData->iRlpMode
        && BOOL_COMPARE( iExt->iCdmaData->iMip, aItem.iExt->iCdmaData->iMip )
        && iExt->iCdmaData->iMipTimeout == aItem.iExt->iCdmaData->iMipTimeout
        && iExt->iCdmaData->iNaiType == aItem.iExt->iCdmaData->iNaiType
        && iExt->iCdmaData->iSimpleIpAuthAlgorithm == 
                                aItem.iExt->iCdmaData->iSimpleIpAuthAlgorithm
        && iExt->iCdmaData->iSimpleIpPapSsHandle == 
                                aItem.iExt->iCdmaData->iSimpleIpPapSsHandle
        && iExt->iCdmaData->iSimpleIpChapSsHandle == 
                                aItem.iExt->iCdmaData->iSimpleIpChapSsHandle
        && iExt->iCdmaData->iMipTBit == aItem.iExt->iCdmaData->iMipTBit
        && iExt->iCdmaData->iMipMnAaaAuthAlgorithm == 
                                aItem.iExt->iCdmaData->iMipMnAaaAuthAlgorithm
        && iExt->iCdmaData->iMipMnAaaSpi == 
                                aItem.iExt->iCdmaData->iMipMnAaaSpi
        && iExt->iCdmaData->iMipMnAaaSsHandle == 
                                aItem.iExt->iCdmaData->iMipMnAaaSsHandle
        && iExt->iCdmaData->iMipMnHaAuthAlgorithm == 
                                aItem.iExt->iCdmaData->iMipMnHaAuthAlgorithm
        && iExt->iCdmaData->iMipMnHaSpi == aItem.iExt->iCdmaData->iMipMnHaSpi
        && iExt->iCdmaData->iMipMnHaSsHandle == 
                                aItem.iExt->iCdmaData->iMipMnHaSsHandle
//
        && BOOL_COMPARE( iGprsUseAnonymAccess, aItem.iGprsUseAnonymAccess )
        && ( 
            ( iExt->iIsIpv6Supported 
            && ( BOOL_COMPARE( iExt->iIpv6GetDNSIPFromServer, 
                                 aItem.iExt->iIpv6GetDNSIPFromServer ) ) )
            || ( !iExt->iIsIpv6Supported ) )
        )
        {
        if (
               ( iExt->iCdmaData->iIwfName->Compare( 
                        *aItem.iExt->iCdmaData->iIwfName ) == 0 )
            && ( iExt->iCdmaData->iPdpAddress->Compare( 
                        *aItem.iExt->iCdmaData->iPdpAddress ) == 0 )
            && ( iExt->iCdmaData->iHomeAgentAddress->Compare( 
                        *aItem.iExt->iCdmaData->iHomeAgentAddress ) == 0 )
            && ( iExt->iCdmaData->iMipHomeAddress->Compare( 
                        *aItem.iExt->iCdmaData->iMipHomeAddress ) == 0 )
            && ( iExt->iCdmaData->iMipPrimaryHomeAgent->Compare( 
                        *aItem.iExt->iCdmaData->iMipPrimaryHomeAgent ) == 0 )
            && ( iExt->iCdmaData->iMipSecondaryHomeAgent->Compare( 
                        *aItem.iExt->iCdmaData->iMipSecondaryHomeAgent ) == 0 )
//
            && ( iIspIfName->Compare( *aItem.iIspIfName ) == 0 )
            && ( iIspIfParams->Compare( *aItem.iIspIfParams ) == 0 )
            && ( iIspIfNetworks->Compare( *aItem.iIspIfNetworks ) == 0 )
            && ( iIspIfAuthName->Compare( *aItem.iIspIfAuthName ) == 0 )
            && ( iIspIfAuthPass->Compare( *aItem.iIspIfAuthPass ) == 0 )
            && ( iIspIPAddr->Compare( *aItem.iIspIPAddr ) == 0 )
            && ( iIspIPNetMask->Compare( *aItem.iIspIPNetMask ) == 0 )
            && ( iIspGateway->Compare( *aItem.iIspGateway ) == 0 )
            && ( iPrimaryDNS->Compare( *aItem.iPrimaryDNS ) == 0 )
            && ( iSecondaryDNS->Compare( *aItem.iSecondaryDNS ) == 0 )
//
            && (
               ( iExt->iIsIpv6Supported && 
                    ( 
                        ( iExt->iIpv6PrimaryDNS->Compare( 
                                *aItem.iExt->iIpv6PrimaryDNS ) == 0 )
                        && 
                        ( iExt->iIpv6SecondaryDNS->Compare( 
                                *aItem.iExt->iIpv6SecondaryDNS ) == 0 )
                    )
                || !iExt->iIsIpv6Supported )
                )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareCdma" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareWlan
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareWlan( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareWlan" ) ) );

    TBool retval( EFalse );

    __ASSERT_ALWAYS( iExt->iWlanData, ApCommons::Panic( ESanityCheckFailed ) );
    __ASSERT_ALWAYS( aItem.iExt->iWlanData, 
                     ApCommons::Panic( ESanityCheckFailed ) );

    if (  
         iExt->iWlanData->iWlanNetworkMode == 
                                aItem.iExt->iWlanData->iWlanNetworkMode
         && iExt->iWlanData->iScanSSID == aItem.iExt->iWlanData->iScanSSID
         && iExt->iWlanData->iWlanSecurityMode == 
                                aItem.iExt->iWlanData->iWlanSecurityMode
         && iExt->iWlanData->iChannelId == 
                                aItem.iExt->iWlanData->iChannelId
        && BOOL_COMPARE( iGetDNSIPFromServer, aItem.iGetDNSIPFromServer )
        )        
        {
        if (   ( iExt->iWlanData->iWlanNetworkName->Compare( 
                    *aItem.iExt->iWlanData->iWlanNetworkName ) == 0 )
            && ( iIspIPAddr->Compare( *aItem.iIspIPAddr ) == 0 )
            && ( iIspIPNetMask->Compare( *aItem.iIspIPNetMask ) == 0 )
            && ( iIspGateway->Compare( *aItem.iIspGateway ) == 0 )
            && ( iPrimaryDNS->Compare( *aItem.iPrimaryDNS ) == 0 )
            && ( iSecondaryDNS->Compare( *aItem.iSecondaryDNS ) == 0 )
// Check if AuthName is needed or not
//            && ( iIspIfAuthName->Compare( *aItem.iIspIfAuthName ) == 0 )
            )
            {
            retval = ETrue;
            }
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareWlan" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::GetCleanIpAddressLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::GetCleanIpAddressLC( const TDesC& aValue )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::GetCleanIpAddressLC" ) ) );

    TBuf<256> buf;
    TInetAddr in;
    User::LeaveIfError( in.Input( aValue ) );
    in.Output( buf );

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::GetCleanIpAddressLC" ) ) );
    return buf.AllocLC();
    }



// ---------------------------------------------------------
// CApAccessPointItem::GetPlainPhoneNumberLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::GetPlainPhoneNumberLC( const TDesC& aNumber )
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApAccessPointItem::GetPlainPhoneNumberLC" ) ) );

    // Removes invalid chars e.g. space, () from the passed phone number.
    // (valid are 0,1,2,..9,p,w,+,*,#)
    // Strips pre- and postfixes, prefix separator is#,
    // postfix separators are p, w or +
    HBufC* retval = NULL;

    HBufC* numbuf = RemoveInvalidPhoneCharsLC( aNumber );
    TPtr16 num( numbuf->Des() );

    TInt prefixpos( 0 );
    TInt postfixpos( 0 );
    TInt lasthashmark = num.LocateReverse('#');
    if ( lasthashmark != KErrNotFound )
        { // has prefix
        prefixpos = ++lasthashmark;
        }
    else
        { // no prefix
        prefixpos = 0;
        }
    // now get postfixes, if any
    // be carefull not to say postfix for the international code...
    // as can be more than one postfix, get each postfix-separator type's
    // position and use the lowest one
    TInt ppos( 0 );
    TInt wpos( 0 );
    TInt pluspos( 0 );
    TInt tmppos( 0 );

    tmppos = num.Locate( '+' );
    if ( tmppos != KErrNotFound )
        {
        pluspos = tmppos;
        if ( tmppos == 0 )
            { // possibly country code comes, get next if any
            // also inc. prefixpos
            prefixpos++;
            num = num.Right( num.Length() - 1 );
            tmppos = num.Locate( '+' );
            if ( tmppos != KErrNotFound )
                {
                tmppos = pluspos;
                }
            }
        }

    tmppos = num.Locate( 'p' );
    if ( tmppos != KErrNotFound )
        {
        ppos = tmppos;
        num = num.Left( num.Length() - ppos );
        }
    tmppos = num.LocateReverse( 'w' );
    if ( tmppos != KErrNotFound )
        {
        wpos = tmppos;
        num = num.Left( num.Length() - wpos );
        }
    // now we have 3 positions,
    // decide which is the smallest but greater than 0
    // if all is 0, it is 0...
    if ( wpos )
        {
        postfixpos = wpos;
        }
    else
        {
        if ( ppos )
            {
            postfixpos = ppos;
            }
        else
            {
            if ( pluspos )
                {
                postfixpos = pluspos;
                }
            }
        }
    // now we have pre- and postfix positions, get the resulting string
    if ( postfixpos == 0 )
        {
        postfixpos = aNumber.Length();
        }
    retval = aNumber.Mid( prefixpos, postfixpos-prefixpos ).AllocLC();
    CleanupStack::Pop( retval ); // retval
    CleanupStack::PopAndDestroy( numbuf ); // numbuf
    CleanupStack::PushL( retval );

    CLOG( ( EApItem, 1, 
            _L( "<- CApAccessPointItem::GetPlainPhoneNumberLC" ) ) );
    return retval;
    }





// ---------------------------------------------------------
// CApAccessPointItem::IsSamePhoneNumber
// ---------------------------------------------------------
//
TBool CApAccessPointItem::IsSamePhoneNumberL( CApAccessPointItem& aItem )
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::IsSamePhoneNumberL" ) ) );

    TBool retval( EFalse );

    HBufC* tmpphone = 
        HBufC::NewLC( aItem.ReadTextLengthL( EApIspDefaultTelNumber ) );
    TPtr16 tmpptr( tmpphone->Des() );
    aItem.ReadTextL( EApIspDefaultTelNumber, tmpptr );

    HBufC* ph1 = GetPlainPhoneNumberLC( *iIspDefaultTelNumber );
    HBufC* ph2 = GetPlainPhoneNumberLC( tmpptr );
    TInt cl ( 7 );
    cl = Min( cl, ph1->Length() );
    cl = Min( cl, ph2->Length() );
    TPtrC16 ptr1 = ph1->Right( cl );
    TPtrC16 ptr2 = ph2->Right( cl );
    if ( ptr1.Compare( ptr2 ) == 0 )
        {
        retval = ETrue;
        }
    CleanupStack::PopAndDestroy( 3, tmpphone ); // ph1, ph2, tmpphone

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::IsSamePhoneNumberL" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidPhoneCharsLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::RemoveInvalidPhoneCharsLC( const TDesC16& aInText )
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApAccessPointItem::RemoveInvalidPhoneCharsLC" ) ) );

    // valid are 0,1,2,..9,p,w,+,*,#
    TInt size = aInText.Length();
    HBufC* aOutText = HBufC::NewLC( size );
    TPtr ptr = aOutText->Des();

    for(TInt i = 0; i< size; i++)
        {
        //TText ch = aInText[i];
        TChar ch = aInText[i];
        if ( ( ch == 'p' ) || ( ch == 'P' ) || ( ch == 'W' ) || ( ch == 'W' )
            || ( ch == '+' ) || ( ch == '0' ) || ( ch == '1' ) || ( ch == '2' )
            || ( ch == '3' ) || ( ch == '4' ) || ( ch == '5' ) || ( ch == '6' )
            || ( ch == '7' ) || ( ch == '8' ) || ( ch == '9' ) || ( ch == '*' )
            || ( ch == '#' )
            )
            {
            ptr.Append( ch );
            }
        }

    CLOG( ( EApItem, 1, 
            _L( "<- CApAccessPointItem::RemoveInvalidPhoneCharsLC" ) ) );
    return aOutText;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::RemoveInvalidCharsLC( const TDesC16& aInText )
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApAccessPointItem::RemoveInvalidCharsLC" ) ) );

    TInt size = aInText.Length();
    HBufC* aOutText = HBufC::NewLC( size );
    TPtr ptr = aOutText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        TText ch = aInText[ii];
        if ( ch == CEditableText::ETabCharacter )
            {
            ch = ' ';
            }
        ptr.Append( ch );
        }

    CLOG( ( EApItem, 1, 
            _L( "<- CApAccessPointItem::RemoveInvalidCharsLC" ) ) );
    return aOutText;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC8* CApAccessPointItem::RemoveInvalidCharsLC( const TDesC8& aInText )
    {
    CLOG( ( EApItem, 0, 
            _L( "-> CApAccessPointItem::RemoveInvalidCharsLC8" ) ) );

    TInt size = aInText.Length();
    HBufC8* aOutText = HBufC8::NewLC( size );
    TPtr8 ptr = aOutText->Des();

    for ( TInt ii=0; ii<size; ++ii )
        {
        TText ch = aInText[ii];
        if ( ch == CEditableText::ETabCharacter )
            {
            ch = ' ';
            }
        ptr.Append( ch );
        }

    CLOG( ( EApItem, 1, 
            _L( "<- CApAccessPointItem::RemoveInvalidCharsLC8" ) ) );
    return aOutText;
    }


// ---------------------------------------------------------
// CApAccessPointItem::CompareProxy
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareProxy( const CApAccessPointItem& aItem ) const
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::CompareProxy" ) ) );

    TBool retval( EFalse );
    if ( BOOL_COMPARE( iApHasProxySettings, aItem.iApHasProxySettings )
         && iApProxyPortNumber == aItem.iApProxyPortNumber
         && BOOL_COMPARE( iApProxyUseProxy, iApProxyUseProxy )
         && ( iApProxyProtocolName->Compare( 
                        *aItem.iApProxyProtocolName ) == 0 )
         && ( iApProxyServerAddress->Compare( 
                        *aItem.iApProxyServerAddress ) == 0 )
         && ( iApProxyExceptions->Compare( *aItem.iApProxyExceptions ) == 0 ) )
        {
        retval = ETrue;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::CompareProxy" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApAccessPointItem::HasProxy
// ---------------------------------------------------------
//
TBool CApAccessPointItem::HasProxy() const
    {
    CLOG( ( EApItem, 0, _L( "<-> CApAccessPointItem::HasProxy" ) ) );

    return iApHasProxySettings;
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetProxyFlag
// ---------------------------------------------------------
//
void CApAccessPointItem::SetProxyFlag()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::SetProxyFlag" ) ) );

    if ( ( iApProxyServerAddress->Compare( KDynIpAddress ) 
         && iApProxyServerAddress->Compare( KEmpty ) )
         || ( iApProxyPortNumber != 0 ) )
        {
        iApHasProxySettings = ETrue;
        }
    else
        {
        iApHasProxySettings = EFalse;
        }
    // BY DEFAULT, as it is not accessible on the UI,
    // we set the USE PROXY flag if ANY proxy setting is defined...
    iApProxyUseProxy = iApHasProxySettings;

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::SetProxyFlag" ) ) );
    }




//----------------------------------------------------------
// CApAccessPointItem::LimitSpeed
//----------------------------------------------------------
//
void CApAccessPointItem::LimitSpeedL()
    {
    CLOG( ( EApItem, 0, _L( "-> CApAccessPointItem::LimitSpeedL" ) ) );

    TBool isWcdma = iExt->iIsWcdma;
    TApBearerType bearer = BearerTypeL();
    const TInt* actSpeeds = NULL;
    switch ( bearer )
        {
        case EApBearerTypeCSD:
            {
            switch ( iBearerCallTypeIsdn )
                {
                case ECallTypeISDNv110:
                    {
                    isWcdma ? actSpeeds = KCsdIsdn110Wcdma :
                              actSpeeds = KCsdIsdn110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    isWcdma ? actSpeeds = KCsdIsdn120Wcdma :
                              actSpeeds = KCsdIsdn120;
                    break;
                    }
                case ECallTypeAnalogue:
                default:
                    {
                    isWcdma ? actSpeeds = KCsdAnalogueWcdma :
                              actSpeeds = KCsdAnalogue;
                    break;
                    }
                }
            break;
            }
        case EApBearerTypeHSCSD:
            {
            switch ( iBearerCallTypeIsdn )
                {
                case ECallTypeISDNv110:
                    {
                    actSpeeds = KHcsdIsdn110;
                    break;
                    }
                case ECallTypeISDNv120:
                    {
                    actSpeeds = KHcsdIsdn120;
                    break;
                    }
                case ECallTypeAnalogue:
                default:
                    {
                    actSpeeds = KHcsdAnal;
                    break;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }


    if( actSpeeds )
        {
        TInt tempSpeed = *actSpeeds;
        TInt nextSpeed = *( actSpeeds + 1);
        while ( ( nextSpeed != KEndOfArray ) 
                && ( tempSpeed < ( TInt )iMaxConnSpeed ) 
                && ( nextSpeed <= ( TInt )iMaxConnSpeed ) )
            {
            tempSpeed = *( ++actSpeeds );
            nextSpeed = *( actSpeeds + 1);
            }
        iMaxConnSpeed = ( TApCallSpeed )tempSpeed ;
        }

    CLOG( ( EApItem, 1, _L( "<- CApAccessPointItem::LimitSpeedL" ) ) );
    }



//----------------------------------------------------------
// CApAccessPointItem::SetIfNetworksL
//----------------------------------------------------------
//
void CApAccessPointItem::SetIfNetworksL()
    {
    // Check if Wlan affects this one!!!
    // Check if Lan affects this one!!!
    TApBearerType bearertype = BearerTypeL();
    if ( iExt->iIsIpv6Supported )
        {
        if ( bearertype == EApBearerTypeGPRS )
            {
            if ( iGprsPdpType  == EIPv6 )
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv6 );
                }
            else
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4 );
                }
            }
        else if ( bearertype == EApBearerTypeCDMA )
            {
            __ASSERT_ALWAYS( iExt->iCdmaData, \
                ApCommons::Panic( ESanityCheckFailed ) );
            if ( iExt->iCdmaData->iPdpType  == EIPv6 )
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv6 );
                }
            else
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4 );
                }
            }
        else 
            {
#ifdef __TEST_LAN_BEARER
            if ( bearertype == EApBearerTypeLAN )
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4IPv6LAN );
                }            
#endif// __TEST_LAN_BEARER
            if ( ( bearertype == EApBearerTypeWLAN ) 
                || ( bearertype == EApBearerTypeLANModem ) )
                {
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4IPv6LAN );
                }
            else
                { // CSD, HSCSD
                WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4IPv6 );
                }
            }
        }
    else
        {
        WriteTextL( EApIspIfNetworks, KDefIspIfNetworksIPv4 );
        }
    }


//----------------------------------------------------------
// CApAccessPointItem::IsSupported
//----------------------------------------------------------
//
TBool CApAccessPointItem::IsSupported( TApMember aColumn )
    {
    TBool ret( ETrue );

    switch ( aColumn )
        {
        case EApCdmaIwfName:
        case EApCdmaServiceOption:
        case EApCdmaPdpType:
        case EApCdmaPdpAddress:
        case EApCdmaReqFwdPriority:
        case EApCdmaReqRevPriority:
        case EApCdmaReqFwdBitrate:
        case EApCdmaReqRevBitrate:
        case EApCdmaReqFwdLoss:
        case EApCdmaReqRevLoss:
        case EApCdmaReqFwdMaxDelay:
        case EApCdmaReqRevMaxDelay:
        case EApCdmaMinFwdBitrate:
        case EApCdmaMinRevBitrate:
        case EApCdmaAccptFwdLoss:
        case EApCdmaAccptRevLoss:
        case EApCdmaAccptFwdMaxDelay:
        case EApCdmaAccptRevMaxDelay:
        case EApCdmaDataCompression:
        case EApCdmaHeaderCompression:
        case EApCdmaAnonymousAccess:
        case EApCdmaIfName:
        case EApCdmaIfParams:
        case EApCdmaIfNetworks:
        case EApCdmaIfPromptForAuth:
        case EApCdmaIfAuthName:
        case EApCdmaIfAuthPassword:
        case EApCdmaIfAuthRetries:
        case EApCdmaIpNetMask:
        case EApCdmaIpGateway:
        case EApCdmaIpAddrFromServer:
        case EApCdmaIpAddr:
        case EApCdmaIpDnsAddrFromServer:
        case EApCdmaIPNameServer1:
        case EApCdmaIPNameServer2:
        case EApCdmaEnableLCPExtensions:
        case EApCdmaDisablePlainTextAuth:
        case EApCdmaApType:
        case EApCdmaQosWarningTimeout:
        case EApCdmaRlpMode:
        case EApCdmaMip:
        case EApCdmaHomeAgentAddress:
        case EApCdmaMipTimeout:
        case EApCdmaNaiType:
        case EApCdmaSimpleIpAuthAlgorithm:
        case EApCdmaSimpleIpPapSsHandle:
        case EApCdmaSimpleIpChapSsHandle:
        case EApCdmaMipTBit:
        case EApCdmaMipHomeAddress:
        case EApCdmaMipPrimaryHomeAgent:
        case EApCdmaMipSecondaryHomeAgent:
        case EApCdmaMipMnAaaAuthAlgorithm:
        case EApCdmaMipMnAaaSpi:
        case EApCdmaMipMnAaaSsHandle:
        case EApCdmaMipMnHaAuthAlgorithm:
        case EApCdmaMipMnHaSpi:
        case EApCdmaMipMnHaSsHandle:
            {
            ret = iExt->iCdmaData != NULL;
            break;
            }
        case EApWlanNetworkName:
        case EApWlanNetworkMode:
        case EApWlanSecurityMode:
        case EApWlanScanSSID:
        case EApWlanChannelId:
            {
            ret = iExt->iWlanData != NULL;
            break;
            }

        default:
            {
            break;
            }
        }

    return ret;
    }

//----------------------------------------------------------
// CApAccessPointItem::LeaveIfUnsupportedL
//----------------------------------------------------------
//
void CApAccessPointItem::LeaveIfUnsupportedL( TApMember aColumn )
    {
    if( !IsSupported( aColumn ) )
        {
        User::Leave( KErrNotSupported );
        }
    }




//----------------------------------------------------------
// CApAccessPointItem::IsWlan
//----------------------------------------------------------
//
TBool CApAccessPointItem::IsWlan()
    {
    TBool retval( EFalse );
    if ( iExt->iWlanData )
        {
        retval = iExt->iWlanData->iIsWlan;
        }
    return retval;
    }


//----------------------------------------------------------
// CApAccessPointItem::LimitIp6DNSL
//----------------------------------------------------------
//
void CApAccessPointItem::LimitIp6DNSL()
    {
    if ( iExt->iIsIpv6Supported )
        {
        if ( ( iExt->iIpv6PrimaryDNS->Compare( KDynIpv6Address ) == 0 ) &&
             ( iExt->iIpv6SecondaryDNS->Compare( KDynIpv6Address ) == 0 ) )
            {
            WriteTextL( EApIP6NameServer1, KWellKnownIp6PrimaryNameServer );
            WriteTextL( EApIP6NameServer2, KWellKnownIp6SecondaryNameServer );
            }
        }
    
    }
    
//  End of File
