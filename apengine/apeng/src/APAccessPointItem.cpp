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
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApItemExtra::~CApItemExtra
// ---------------------------------------------------------
//
CApItemExtra::~CApItemExtra()
    {
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
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApItemCdmaData::~CApItemCdmaData
// ---------------------------------------------------------
//
CApItemCdmaData::~CApItemCdmaData()
    {
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
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApItemWlanData::~CApItemWlanData
// ---------------------------------------------------------
//
CApItemWlanData::~CApItemWlanData()
    {
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
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApAccessPointItem::~CApAccessPointItem
// ---------------------------------------------------------
//
EXPORT_C CApAccessPointItem::~CApAccessPointItem()
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::CopyFromL( const CApAccessPointItem&
                                            aCopyFrom  )
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::operator==
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::operator==(
                                             const CApAccessPointItem& aItem
                                             ) const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::operator!=
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::operator!=(
                                             const CApAccessPointItem& aItem
                                             ) const
    {
    return EFalse;
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
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::ReadTextL( const TApMember aColumn,
                                           TDes8& aValue )
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::ReadTextL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::ReadTextL( const TApMember aColumn,
                                           TDes16& aValue )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadLongTextL
// ---------------------------------------------------------
//
EXPORT_C HBufC* CApAccessPointItem::ReadLongTextL( const TApMember aColumn )
    {
    return NULL;
    }




// ---------------------------------------------------------
// CApAccessPointItem::ReadConstLongTextL
// ---------------------------------------------------------
//
EXPORT_C const HBufC* CApAccessPointItem::ReadConstLongTextL
                                            ( const TApMember aColumn )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadTextLengthL
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApAccessPointItem::ReadTextLengthL( const TApMember aColumn )
    {
    return 0;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReadUint
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::ReadUint( const TApMember aColumn,
                                           TUint32& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApAccessPointItem::ReadBool
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::ReadBool( const TApMember aColumn,
                                           TBool& aValue )
    {
    return KErrNotSupported;
    }


// Update
// ---------------------------------------------------------
// CApAccessPointItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteTextL( const TApMember aColumn,
                                            const TDesC8& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApAccessPointItem::WriteTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteTextL( const TApMember aColumn,
                                            const TDesC16& aValue )
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApAccessPointItem::WriteLongTextL
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteLongTextL( const TApMember aColumn,
                                                const TDesC& aValue )
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApAccessPointItem::WriteUint
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteUint( const TApMember aColumn,
                                            const TUint32& aValue )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApAccessPointItem::WriteBool
// ---------------------------------------------------------
//
EXPORT_C TInt CApAccessPointItem::WriteBool( const TApMember aColumn,
                                            const TBool& aValue )
    {
    return KErrNotSupported;
    }



// Query
// COMMON

// ---------------------------------------------------------
// CApAccessPointItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApAccessPointItem::WapUid() const
    {
    return 0;
    }


// ---------------------------------------------------------
// CApAccessPointItem::ConnectionName
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApAccessPointItem::ConnectionName() const
    {
    User::Leave( KErrNotSupported );
    }


// WAP-spec.
// ---------------------------------------------------------
// CApAccessPointItem::WapBearer
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApAccessPointItem::WapBearer() const
    {
    User::Leave( KErrNotSupported );
    }



// WAP-spec.
// ---------------------------------------------------------
// CApAccessPointItem::BearerTypeL
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApAccessPointItem::BearerTypeL()
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetBearerTypeL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::SetBearerTypeL( TApBearerType aBearer )
    {    
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetNamesL
// ---------------------------------------------------------
//
EXPORT_C void CApAccessPointItem::SetNamesL( const TDesC16& aValue )
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::SanityCheckOk
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::SanityCheckOk()
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::HasSameIspAndBearer
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::HasSameIspAndBearerL(
                                 CApAccessPointItem& aItem )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApAccessPointItem::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApAccessPointItem::IsReadOnly() const
    {
    return EFalse;
    }




// ================= OTHER EXPORTED FUNCTIONS ==============


// =============== OTHER NON - EXPORTED FUNCTIONS ============
// ---------------------------------------------------------
// CApAccessPointItem::IsVpnAp
// ---------------------------------------------------------
//
TBool CApAccessPointItem::IsVpnAp() const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApAccessPointItem::SetVpnAp
// ---------------------------------------------------------
//
void CApAccessPointItem::SetVpnAp( TBool aIsVpn )
    {
    }

// ---------------------------------------------------------
// CApAccessPointItem::Specified
// ---------------------------------------------------------
//
TBool CApAccessPointItem::Specified( TApMember aMember )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApAccessPointItem::Specify
// ---------------------------------------------------------
//
void CApAccessPointItem::Specify( TApMember aMember, TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyAll
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyAll( TBool aOn )
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::SpecifyWap
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyWap( TBool aOn )
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::SpecifyIpBearer
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyIpBearer( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyIAP
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyIAP( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyISP
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyISP( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyGPRS
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyGPRS( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyCDMA
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyCDMA( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SpecifyWLAN
// ---------------------------------------------------------
//
void CApAccessPointItem::SpecifyWLAN( TBool aOn )
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::SetDnsIpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetDnsIpFromServer()
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetIpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetIpFromServer()
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetDns6IpFromServer
// ---------------------------------------------------------
//
void CApAccessPointItem::SetDns6IpFromServer()
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReAllocL
// ---------------------------------------------------------
//
void CApAccessPointItem::ReAllocL(HBufC8*& aDes, const TDesC8& aValue)
    {
    }


// ---------------------------------------------------------
// CApAccessPointItem::ReAllocL
// ---------------------------------------------------------
//
void CApAccessPointItem::ReAllocL(HBufC*& aDes, const TDesC16& aValue)
    {
    }



// ---------------------------------------------------------
// CApAccessPointItem::DoCompareApL
// ---------------------------------------------------------
//
TBool CApAccessPointItem::DoCompareApL( const CApAccessPointItem& aItem )const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareWap
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareWap( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareIap
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareIap( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApAccessPointItem::CompareIsp
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareIsp( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }




// ---------------------------------------------------------
// CApAccessPointItem::CompareGprs
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareGprs( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareCdma
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareCdma( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::CompareWlan
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareWlan( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApAccessPointItem::GetCleanIpAddressLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::GetCleanIpAddressLC( const TDesC& aValue )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApAccessPointItem::GetPlainPhoneNumberLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::GetPlainPhoneNumberLC( const TDesC& aNumber )
    {
    return NULL;
    }





// ---------------------------------------------------------
// CApAccessPointItem::IsSamePhoneNumber
// ---------------------------------------------------------
//
TBool CApAccessPointItem::IsSamePhoneNumberL( CApAccessPointItem& aItem )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidPhoneCharsLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::RemoveInvalidPhoneCharsLC( const TDesC16& aInText )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC* CApAccessPointItem::RemoveInvalidCharsLC( const TDesC16& aInText )
    {
    return NULL;
    }



// ---------------------------------------------------------
// CApAccessPointItem::RemoveInvalidCharsLC
// ---------------------------------------------------------
//
HBufC8* CApAccessPointItem::RemoveInvalidCharsLC( const TDesC8& aInText )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApAccessPointItem::CompareProxy
// ---------------------------------------------------------
//
TBool CApAccessPointItem::CompareProxy( const CApAccessPointItem& aItem ) const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApAccessPointItem::HasProxy
// ---------------------------------------------------------
//
TBool CApAccessPointItem::HasProxy() const
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApAccessPointItem::SetProxyFlag
// ---------------------------------------------------------
//
void CApAccessPointItem::SetProxyFlag()
    {
    }




//----------------------------------------------------------
// CApAccessPointItem::LimitSpeed
//----------------------------------------------------------
//
void CApAccessPointItem::LimitSpeedL()
    {
    }



//----------------------------------------------------------
// CApAccessPointItem::SetIfNetworksL
//----------------------------------------------------------
//
void CApAccessPointItem::SetIfNetworksL()
    {
    }


//----------------------------------------------------------
// CApAccessPointItem::IsSupported
//----------------------------------------------------------
//
TBool CApAccessPointItem::IsSupported( TApMember aColumn )
    {
    return EFalse;
    }

//----------------------------------------------------------
// CApAccessPointItem::LeaveIfUnsupportedL
//----------------------------------------------------------
//
void CApAccessPointItem::LeaveIfUnsupportedL( TApMember aColumn )
    {
    }




//----------------------------------------------------------
// CApAccessPointItem::IsWlan
//----------------------------------------------------------
//
TBool CApAccessPointItem::IsWlan()
    {
    return EFalse;
    }


//----------------------------------------------------------
// CApAccessPointItem::LimitIp6DNSL
//----------------------------------------------------------
//
void CApAccessPointItem::LimitIp6DNSL()
    {
    }
    
//  End of File
