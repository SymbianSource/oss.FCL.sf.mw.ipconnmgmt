/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class provides available wlan network array.
*     
*
*/


// INCLUDE FILES
#include "WlanNetworkDataProvider.h"
#include "NetworkInfoArray.h"
#include "NetworkInfoBase.h"
#include "NetworkInfoKey.h"
#include "ConnectionDialogsLogger.h"

#include <featmgr.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h> // KCRUidCoreApplicationUIs


// CONSTANTS 

/**
* Management frame information element IDs.
*/
enum T802Dot11InformationElementID
    {
    E802Dot11SsidIE                 = 0,
    E802Dot11SupportedRatesIE       = 1,
    E802Doi11FhParameterSetIE       = 2,
    E802Dot11DsParameterSetIE       = 3,
    E802Dot11CfParameterSetIE       = 4,
    E802Dot11TimIE                  = 5,
    E802Dot11IbssParameterSetIE     = 6,
    E802Dot11CountryIE              = 7,
    E802Dot11HoppingPatternParamIE  = 8,
    E802Dot11HoppingPatternTableIE  = 9,
    E802Dot11RequestIE              = 10,

    E802Dot11ChallengeTextIE        = 16,
    // Reserved for challenge text extension 17 - 31
    E802Dot11ErpInformationIE       = 42,
    E802Dot11ExtendedRatesIE        = 50,
    E802Dot11AironetIE              = 133,
    E802Dot11ApIpAddressIE          = 149,
    E802Dot11RsnIE                  = 221
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWlanNetworkDataProvider::CWlanNetworkDataProvider
// ---------------------------------------------------------
//
CWlanNetworkDataProvider::CWlanNetworkDataProvider() 
: CBase(),
#ifndef __WINS__
  iWlanMgmt( NULL ),
  iScanInfo( NULL ),
#endif // !__WINS__
  iNetworkInfoArray( NULL )
    {
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::~CWlanNetworkDataProvider
// ---------------------------------------------------------
//
CWlanNetworkDataProvider::~CWlanNetworkDataProvider()
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::~CWlanNetworkDataProvider" );
    delete iNetworkInfoArray;

#ifndef __WINS__
    if  ( iWlanMgmt )
        {
        iWlanMgmt->CancelGetScanResults();
        }
    delete iWlanMgmt;
    delete iScanInfo;
#endif // !__WINS__

    CLOG_LEAVEFN( "CWlanNetworkDataProvider::~CWlanNetworkDataProvider" );
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::NewL
// ---------------------------------------------------------
//
CWlanNetworkDataProvider* CWlanNetworkDataProvider::NewL()
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::NewL" );  

    CWlanNetworkDataProvider* self = new ( ELeave ) CWlanNetworkDataProvider();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CLOG_LEAVEFN( "CWlanNetworkDataProvider::NewL" );

    return self;
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::CWlanNetworkDataProvider
// ---------------------------------------------------------
//
void CWlanNetworkDataProvider::ConstructL()
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::ConstructL" );

#ifndef __WINS__
    iWlanMgmt = CWlanMgmtClient::NewL();
    iScanInfo = CWlanScanInfo::NewL();
#endif // !__WINS__

    iNetworkInfoArray = CNetworkInfoArray::NewL();

    CLOG_LEAVEFN( "CWlanNetworkDataProvider::ConstructL" );
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::OnTimerExpiredL
// ---------------------------------------------------------
//
void CWlanNetworkDataProvider::OnTimerExpiredL()
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::OnTimerExpiredL" );

    TBool hideOtherEntry( EFalse );

    iNetworkInfoArray->Reset();

#ifdef __WINS__

    TBuf8<KWlanMaxSsidLength> ssid8_a( _L8( "Test SSID" ) );
    TBuf8<KWlanMaxSsidLength> ssid8_c( _L8( "Another SSID" ) );
    TInt rxLevel_a = ESignalStrengthMax;
    TInt rxLevel_b = ESignalStrengthMin;
    TInt rxLevel_c = ESignalStrengthMax;

    AddNetworkL( ssid8_a, ( TSignalStrength )rxLevel_a, 
                 ENetworkModeInfrastructure, EWlanConnectionSecurityWep,
                 EWlanConnectionExtentedSecurityModeWepOpen,
                 ETrue );
    AddNetworkL( ssid8_a, ( TSignalStrength )rxLevel_b, 
                 ENetworkModeInfrastructure, EWlanConnectionSecurityWep,
                 EWlanConnectionExtentedSecurityModeWepOpen,
                 ETrue );
    AddNetworkL( ssid8_c, ( TSignalStrength )rxLevel_c, 
                 ENetworkModeInfrastructure, EWlanConnectionSecurityWpa,
                 EWlanConnectionExtentedSecurityModeWpa,
                 ETrue );

#else

    TUint8 ieLen( 0 );
    const TUint8* ieData;
    TBuf8<KWlanMaxSsidLength> ssid8;
    TUint8 rxLevel8( 0 );
    TInt rxLevel( 0 );

    for( iScanInfo->First(); !iScanInfo->IsDone(); iScanInfo->Next() )
        {
        User::LeaveIfError( iScanInfo->InformationElement( E802Dot11SsidIE, 
                                                           ieLen, &ieData ) );
        if( !IsHiddenSsid( ieLen, ieData ) )
            {
            rxLevel8 = iScanInfo->RXLevel();
            rxLevel = static_cast< TInt >( rxLevel8 );

            ssid8.Copy( ieData, ieLen );
            AddNetworkL( ssid8, 
                         ( TSignalStrength )rxLevel, 
                         ( iScanInfo->Capability() & 
                           E802Dot11CapabilityEssMask ) ?
                            ENetworkModeInfrastructure : ENetworkModeAdHoc,
                         iScanInfo->SecurityMode(),
                         iScanInfo->ExtendedSecurityMode(),
                         iScanInfo->IsProtectedSetupSupported()
                          );
            }
        }

    hideOtherEntry = IsPhoneOfflineL();

#endif

    SortNetworkInfoArrayL();

    if ( !hideOtherEntry )
        {
        AddOthersEntryL();
        }

    CLOG_LEAVEFN( "CWlanNetworkDataProvider::OnTimerExpiredL" );      
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::SortNetworkInfoArray
// ---------------------------------------------------------
//    
void CWlanNetworkDataProvider::SortNetworkInfoArrayL()
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::SortNetworkInfoArrayL" );      

    CNetworkInfoKey* networkInfoKey = new( ELeave ) CNetworkInfoKey();
    networkInfoKey->SetPtr( iNetworkInfoArray );
    CleanupStack::PushL( networkInfoKey );

    // Sort returns KErrGeneral if stack overflow, otherwise, returns
    // KErrNone. So we will Leave only if stack overflow,
    // but than that really does not matter...
    User::LeaveIfError( iNetworkInfoArray->Sort( *networkInfoKey ) );

    CleanupStack::PopAndDestroy( networkInfoKey );

    CLOG_LEAVEFN( "CWlanNetworkDataProvider::SortNetworkInfoArrayL" );
    }


#ifndef __WINS__
// ---------------------------------------------------------
// CWlanNetworkDataProvider::IsHiddenSsid
// ---------------------------------------------------------
//    
TBool CWlanNetworkDataProvider::IsHiddenSsid( TUint aSsidLength, 
                                              const TUint8* aSsid )
    {
    CLOG_ENTERFN( "CWlanNetworkDataProvider::IsHiddenSsid" );
    
    if ( !aSsidLength )
        {
        return ETrue;
        }

    const TUint8 CSPACE = 0x20; 
    TBool result( ETrue );

    TInt count( 0 );
    TUint8 temp( 0x00 );
    for ( TUint i( 0 ); i < aSsidLength; ++i )
        {
        temp = aSsid[i];
        count += temp; // in hidden networks characters are: 0x00
        result &= temp == CSPACE;

        CLOG_WRITEF( _L( "temp %x" ), temp );
        }

    return ( !count || result );
    }

#endif // !__WINS__


// ---------------------------------------------------------
// CWlanNetworkDataProvider::AddNetworkL
// ---------------------------------------------------------
//
void CWlanNetworkDataProvider::AddNetworkL( TDes8& aNetworkName, 
                                   TSignalStrength aStrength, 
                                   TNetworkMode aNetworkMode,
                                   TWlanConnectionSecurityMode aSecurityMode,
                                   TWlanConnectionExtentedSecurityMode aExtSecurityMode,
                                   TBool aProtectedSetupSupported )
    {
    TName networkName16;
    networkName16.Copy( aNetworkName );
    TInt index = iNetworkInfoArray->GetArrayIndex( networkName16 );

    // if not found
    if ( index == KErrNotFound )
        {
        CNetworkInfoBase* networkInfoBase = new( ELeave )CNetworkInfoBase();

        networkInfoBase->SetNetworkName( aNetworkName );
        networkInfoBase->SetSignalStrength( aStrength );

        networkInfoBase->SetNetworkMode( aNetworkMode );
        networkInfoBase->SetSecurityMode( aSecurityMode );
        networkInfoBase->SetExtendedSecurityMode( aExtSecurityMode );

        networkInfoBase->SetOthersEntry( EFalse );
        CLOG_WRITEF( _L( "Protected Setup in CWlanNetworkDataProvider::AddNetworkL: %d" ), aProtectedSetupSupported );
        networkInfoBase->SetProtectedSetupSupported( 
                                                    aProtectedSetupSupported );

        CleanupStack::PushL( networkInfoBase );
        networkInfoBase->RefreshNetworksListBoxItemTextL();

        iNetworkInfoArray->AppendL( networkInfoBase );
        CleanupStack::Pop( networkInfoBase );
        }
    else    // if found show highest strength
        {
        TSignalStrength signal = ( *iNetworkInfoArray )[index]->
                                                            SignalStrength();
        if ( signal )
            {
            ( *iNetworkInfoArray )[index]->SetSignalStrength( 
                                                        signal < aStrength ?
                                                        signal : aStrength );
            }
        }
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::AddOthersEntryL
// ---------------------------------------------------------
//    
void CWlanNetworkDataProvider::AddOthersEntryL()
    {
    CNetworkInfoBase* networkInfoBase = new( ELeave )CNetworkInfoBase();
    networkInfoBase->SetOthersEntry( ETrue );

    CleanupStack::PushL( networkInfoBase );
    networkInfoBase->RefreshNetworksListBoxItemTextL();

    iNetworkInfoArray->AppendL( networkInfoBase );
    CleanupStack::Pop( networkInfoBase );
    }


// ---------------------------------------------------------
// CWlanNetworkDataProvider::IsPhoneOfflineL
// 
// Checks if phone is in offline mode or not.
// Return ETrue if phone is in offline mode.
// Return EFalse if phone is not in offline mode.
// ---------------------------------------------------------
//
TBool CWlanNetworkDataProvider::IsPhoneOfflineL() const
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        CRepository* repository = CRepository::NewLC( 
                                                    KCRUidCoreApplicationUIs );
        TInt connAllowed = 1;
        repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
        CleanupStack::PopAndDestroy();  // repository
        if ( !connAllowed )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// End of File
