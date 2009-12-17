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
* Description:  Implementation of WLAN Coverage Check class
*
*/


#include <wlanmgmtclient.h>

#include "wlancoveragecheck.h"
#include "NetworkInfoBase.h"
#include "ConnectionDialogsLogger.h"


/**
* Management frame information element IDs.
* needed to determine coverage
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


/**
* Bit masks for Capability Information field.
*/
enum T802Dot11CapabilityBitMask
    {
    E802Dot11CapabilityEssMask          = 0x0001,
    E802Dot11CapabilityIbssMask         = 0x0002,
    E802Dot11CapabilityCfPollableMask   = 0x0004,
    E802Dot11CapabilityCfPollRequestMask= 0x0008,
    E802Dot11CapabilityPrivacyMask      = 0x0010,
    E802Dot11ShortPreambleMask          = 0x0020,
    E802Dot11PbccMask                   = 0x0040,      
    E802Dot11ChannelAgilityMask         = 0x0080
    };


// ======================= MEMBER FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CWlanCoverageCheck::CWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CWlanCoverageCheck::CWlanCoverageCheck() 
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CWlanCoverageCheck::~CWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CWlanCoverageCheck::~CWlanCoverageCheck()
    {
    Cancel();
    }
    
// ----------------------------------------------------------------------------
// CWlanCoverageCheck::DoCancel
// ----------------------------------------------------------------------------
//
void CWlanCoverageCheck::DoCancel()
    {
    iWait.AsyncStop();
    }
    
// ----------------------------------------------------------------------------
// CWlanCoverageCheck::RunL
// ----------------------------------------------------------------------------
//
void CWlanCoverageCheck::RunL()
    {
    CLOG_WRITE( "RunL entered" );
    switch( iProgState )
        {
        case EScanning:
            {
            CLOG_WRITE( "Scanning state" );
            if( !iStatus.Int() )
                // The list is ready 
                {
                CLOG_WRITE( "No error" );
                iProgState = EDone;
                iWait.AsyncStop();
                CLOG_WRITE( "wait stopped" );
                }
            else
                // something went wrong -> no coverage.
                {
                CLOG_WRITE( "error" );
                iWait.AsyncStop();
                }
            }
            break;            
        case EDone:
            {
            CLOG_WRITE( "State Done entered" );
            iWait.AsyncStop();
            }
            break;
            
        default:
            {
            }
        }
    }



// ----------------------------------------------------------------------------
// CWlanCoverageCheck::ScanForPromptedSsidL
// ----------------------------------------------------------------------------
//
TBool CWlanCoverageCheck::ScanForPromptedSsidL( TWlanSsid aEnteredSsid,
                                TBool aBroadcast,
                                TWlanConnectionMode& aConnectionMode, 
                                TWlanConnectionSecurityMode& aSecurityMode,
                                TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                                TBool& aProtectedSetupSupported )
    {
    TBool found = EFalse;

    CLOG_WRITE( "ScanForPromptedSsidL started" );
        
    CWlanMgmtClient* wlanMgmt = CWlanMgmtClient::NewL();
    CleanupStack::PushL( wlanMgmt );
    CLOG_WRITE( "Client created" );

    CWlanScanInfo* scanInfo = CWlanScanInfo::NewL();
    CleanupStack::PushL( scanInfo );
    CLOG_WRITE( "Scaninfo created" );

    iProgState = EScanning;
    CLOG_WRITE( "State set to scanning" );
    
    if ( aBroadcast )
        {
        wlanMgmt->GetScanResults( iStatus, *scanInfo );
        }
    else
        {
        wlanMgmt->GetScanResults( aEnteredSsid, iStatus, *scanInfo );
        }

    CLOG_WRITE( "GetScanResult called" );
    
    SetActive();
    CLOG_WRITE( "SetActive called" );
    CLOG_WRITE( "wait started" );
    iWait.Start();
    
    CLOG_WRITE( "Wait returned" );
    
    // now we have the results and might start to work on them...
    if ( iProgState == EDone )
        {       //finished without error, work on the result...
        TSignalStrength signalStrength = ESignalStrengthMin;

        for ( scanInfo->First(); !scanInfo->IsDone(); scanInfo->Next() )
            {
            TUint8 ieLen( 0 );
            const TUint8* ieData;
            TBuf8<KWlanMaxSsidLength> ssid8;
            
            TInt ret = scanInfo->InformationElement( E802Dot11SsidIE, ieLen, 
                                                     &ieData );
            if ( ret == KErrNone ) 
                {               
                CLOG_WRITE( "Starting copying ssid" );  
                // get the ssid
                ssid8.Copy( ieData, ieLen );
                CLOG_WRITE( "SSID copied" );  
                
                if ( !aEnteredSsid.Compare( ssid8 ) )
                    {
                    // get the signal strength                        
                    TUint8 rxLevel8 = scanInfo->RXLevel();
                    TInt rxLevel = static_cast< TInt >( rxLevel8 );
            
                    TSignalStrength strength = ( TSignalStrength )rxLevel;
                    if ( !found || ( found && strength < signalStrength ) )
                        {
                        found = ETrue;
                        signalStrength = strength;

                        if ( scanInfo->Capability() & 
                             E802Dot11CapabilityEssMask )
                            {
                            aConnectionMode = 
                             ( TWlanConnectionMode )ENetworkModeInfrastructure;
                            }
                        else
                            {
                            aConnectionMode = 
                                    ( TWlanConnectionMode )ENetworkModeAdHoc;
                            }

                        aSecurityMode = scanInfo->SecurityMode();
                        aExtSecurityMode = scanInfo->ExtendedSecurityMode();
                        aProtectedSetupSupported = 
                                        scanInfo->IsProtectedSetupSupported();
                        CLOG_WRITEF( _L( "****Protected Setup in CWlanCoverageCheck::ScanForPromptedSsidL: %d***" ), aProtectedSetupSupported );
                        }
                    }
                }
            else
                {
                User::Leave( ret );
                }
        
            }
        
        CleanupStack::PopAndDestroy( scanInfo );
        CleanupStack::PopAndDestroy( wlanMgmt );
        }
    else
        {
        User::LeaveIfError( iStatus.Int() );        
        }
    
    return found;
    }


