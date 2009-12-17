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

#include "cmwlancoveragecheck.h"
#include "cmpluginwlan.h"
#include "cmpluginwlandata.h"
#include "cmpluginwlandataarray.h"
#include "cmlogger.h"


/**
* Management frame information element IDs.
* needed to determine coverage
*/
enum T802Dot11InformationElementID
    {
    E802Dot11SsidIE                 = 0,
    E802Dot11SupportedRatesIE       = 1,
    E802Dot11FhParameterSetIE       = 2,
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
// CCmWlanCoverageCheck::CCmWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmWlanCoverageCheck::CCmWlanCoverageCheck() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }
    
// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::~CCmWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmWlanCoverageCheck::~CCmWlanCoverageCheck()
    {
    Cancel();
    }
    
// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::DoCancel
// ----------------------------------------------------------------------------
//
void CCmWlanCoverageCheck::DoCancel()
    {
    iWait.AsyncStop();
    }
    
// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::RunL
// ----------------------------------------------------------------------------
//
void CCmWlanCoverageCheck::RunL()
    {
    CLOG_WRITE( "RunL entered" );
    switch( iProgState )
        {
        case EScanning:
            {
             if( !iStatus.Int() )
                // The list is ready 
                {
                iProgState = EDone;
                iWait.AsyncStop();
                }
            else
                // something went wrong -> no coverage.
                {
                iWait.AsyncStop();
                }
            }
            break;            
        case EDone:
            {
            iWait.AsyncStop();
            }
            break;
            
        default:
            {
            User::Leave( KErrCorrupt );
            }
        }
    }



// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::GetCoverageL
// ----------------------------------------------------------------------------
//
TInt CCmWlanCoverageCheck::GetCoverageL()
    {
    LOGGER_ENTERFN( "CCmWlanCoverageCheck::GetCoverageL" );

    iProgState = EServiceStatus;
    iCoverage = 0;
    
    CWlanMgmtClient* wlanMgmt = CWlanMgmtClient::NewL();
    CleanupStack::PushL( wlanMgmt );

    CWlanScanInfo* scanInfo = CWlanScanInfo::NewL();
    CleanupStack::PushL( scanInfo );

    iProgState = EScanning;
    
    wlanMgmt->GetScanResults( iStatus, *scanInfo );
    
    SetActive();
    iWait.Start();
    
    // now we have the results and might start to work on them...
    if ( iProgState == EDone )
        { //finished without error, work on the result...
        CCmPluginWlanDataArray* wlandataarray = new ( ELeave ) 
            CCmPluginWlanDataArray();
        CleanupStack::PushL( wlandataarray );

        for ( scanInfo->First(); !scanInfo->IsDone(); scanInfo->Next() )
            {
            // iWlanNetworks cannot be modified 'directly' in
            // a const method (like GetBoolAttributeL), so we have to
            // cast away the constness.
            
            // not simply count as before, but check if it is hidden,
            // and do not allow duplicates...
            TBool isHidden( EFalse );
            
            TUint8 ieLen( 0 );
            const TUint8* ieData;
            TBuf8<KWlanMaxSsidLength> ssid8;
            
            TInt ret = scanInfo->InformationElement( E802Dot11SsidIE, ieLen, 
                                                      &ieData );
            if ( ret == KErrNone ) 
                {               
                isHidden = IsHiddenSsid( ieLen, ieData );
                if ( !isHidden )
                    {
                    // get the ssid
                    ssid8.Copy( ieData, ieLen );
                    HBufC* tmp = HBufC::NewLC(KWlanMaxSsidLength);
                    TPtr tmpptr( tmp->Des() );
                    
                    tmpptr.Copy(ssid8);
                    // get the signal strength                        
                    TUint8 rxLevel8 = scanInfo->RXLevel();
                    TInt rxLevel = static_cast< TInt >( rxLevel8 );
                    
                    // now add it to an array of CCmPluginWlanData,
                    // as it is not hidden
                    // the adding function should handle uniqeness and 
                    // signal strength checking, it should contain only
                    // the strongest signal!                        
                    AddToArrayIfNeededL( *wlandataarray, rxLevel, tmpptr );
                    
                    CleanupStack::PopAndDestroy( tmp );
                    }                
                }
            else
                {
                User::Leave( ret );
                }
            }
        iCoverage = wlandataarray->Count();
        
        CleanupStack::PopAndDestroy( wlandataarray );

        CleanupStack::PopAndDestroy( 2, wlanMgmt );
        }
    else
        {
        User::LeaveIfError( iStatus.Int() );        
        }        
        
    return iCoverage;
    }

// ---------------------------------------------------------
// CCmWlanCoverageCheck::IsHiddenSsid
// ---------------------------------------------------------
//    
TBool CCmWlanCoverageCheck::IsHiddenSsid( TUint aSsidLength, 
                                   const TUint8* aSsid ) const
    {
    LOGGER_ENTERFN( "CCmWlanCoverageCheck::IsHiddenSsid" );
    
    const TUint8 CSPACE = 0x20; 
    TBool result( EFalse );
    TBool result2( ETrue );

    if ( !aSsidLength )
        {
        result = ETrue;
        }

    TInt count( 0 );
    TUint8 temp( 0x00 );
    
    for ( TUint i( 0 ); i < aSsidLength; ++i )
        {
        temp = aSsid[i];
        count += temp; // in hidden networks characters are: 0x00
        result2 &= temp == CSPACE ;
        }

    if( !count || result2 )
        {
        result = ETrue;
        }
        
    return result;    
    }


// ---------------------------------------------------------
// CCmWlanCoverageCheck::AddToArrayIfNeededL
// ---------------------------------------------------------
//    
void CCmWlanCoverageCheck::AddToArrayIfNeededL( 
                            CCmPluginWlanDataArray& aArray, 
                            TInt aRXLevel, 
                            TDesC& aNetworkName ) const
    {
    LOGGER_ENTERFN( "CCmWlanCoverageCheck::AddToArrayIfNeededL" );

    // first check if it is already in the array
    
    TBool found( EFalse );
    TInt index( 0 );
    TInt count = aArray.Count();
    
    while( ( index <  count ) && !found  )
        {
        if( ( aArray )[index]->iNetworkName->Compare( aNetworkName ) )
            {
            ++index;   
            }
        else
            {
            found = ETrue;
            }
        }

    TSignalStrength strength = ( TSignalStrength )aRXLevel;
    if ( found )
        {
        // if already exists, check signal strengths
        TSignalStrength signal = ( aArray )[index]->iSignalStrength;
        
        // set to higher value
        ( aArray )[index]->iSignalStrength = 
                              signal < strength ? signal : strength ;
        }
    else
        {
        // new element, add it to array with signal strength!!
        CCmPluginWlanData* data = CCmPluginWlanData::NewLC();
        data->iNetworkName = aNetworkName.AllocL();
        data->iSignalStrength = strength;
        aArray.AppendL( data );
        CleanupStack::Pop( data ); // item owned by list!
        }    
    }


// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::ScanForPromptedSsidL
// ----------------------------------------------------------------------------
//
TBool CCmWlanCoverageCheck::ScanForPromptedSsidL( TWlanSsid aEnteredSsid,
                                TBool aBroadcast,
                                TWlanNetMode& aNetworkMode, 
                                TWlanConnectionExtentedSecurityMode& aSecurityMode,
                                TBool& aProtectedSetupSupported )
    {
    LOGGER_ENTERFN( "CCmWlanCoverageCheck::ScanForPromptedSsidL" );
    
    TBool found = EFalse;

    iProgState = EServiceStatus;
       
    CWlanMgmtClient* wlanMgmt = CWlanMgmtClient::NewL();
    CleanupStack::PushL( wlanMgmt );

    CWlanScanInfo* scanInfo = CWlanScanInfo::NewL();
    CleanupStack::PushL( scanInfo );

    iProgState = EScanning;
    
    if ( aBroadcast )
        {
        wlanMgmt->GetScanResults( iStatus, *scanInfo );
        }
    else
        {
        wlanMgmt->GetScanResults( aEnteredSsid, iStatus, *scanInfo );
        }

    SetActive();
    iWait.Start();
    
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
                // get the ssid
                ssid8.Copy( ieData, ieLen );
                
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

                        aNetworkMode = ( scanInfo->Capability() & 
                                         E802Dot11CapabilityEssMask ) ?
                                       EInfra : EAdhoc;
                        aSecurityMode = scanInfo->ExtendedSecurityMode();

                        aProtectedSetupSupported = 
                                        scanInfo->IsProtectedSetupSupported();
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


