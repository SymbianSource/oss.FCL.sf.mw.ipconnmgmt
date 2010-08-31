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
*        Implementation of WLAN Coverage Check class
*
*/


#include <wlanmgmtclient.h>

#include "cmwlancoveragecheck.h"
#include "cmpluginwlan.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmwlancoveragecheckTraces.h"
#endif


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


// ======================= MEMBER FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::CCmWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmWlanCoverageCheck::CCmWlanCoverageCheck() : CActive( EPriorityStandard )
    {
    OstTraceFunctionEntry1( CCMWLANCOVERAGECHECK_CCMWLANCOVERAGECHECK_ENTRY, this );
    CActiveScheduler::Add( this );
    OstTraceFunctionExit1( CCMWLANCOVERAGECHECK_CCMWLANCOVERAGECHECK_EXIT, this );
    }

// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::~CCmWlanCoverageCheck()
// ----------------------------------------------------------------------------
//
CCmWlanCoverageCheck::~CCmWlanCoverageCheck()
    {
    OstTraceFunctionEntry1( DUP1_CCMWLANCOVERAGECHECK_CCMWLANCOVERAGECHECK_ENTRY, this );
    Cancel();
    OstTraceFunctionExit1( DUP1_CCMWLANCOVERAGECHECK_CCMWLANCOVERAGECHECK_EXIT, this );
    }

// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::DoCancel
// ----------------------------------------------------------------------------
//
void CCmWlanCoverageCheck::DoCancel()
    {
    OstTraceFunctionEntry1( CCMWLANCOVERAGECHECK_DOCANCEL_ENTRY, this );
    iWait.AsyncStop();
    OstTraceFunctionExit1( CCMWLANCOVERAGECHECK_DOCANCEL_EXIT, this );
    }

// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::RunL
// ----------------------------------------------------------------------------
//
void CCmWlanCoverageCheck::RunL()
    {
    OstTraceFunctionEntry1( CCMWLANCOVERAGECHECK_RUNL_ENTRY, this );

    switch ( iProgState )
        {
        case EScanning:
            {
            if ( iStatus.Int() == KErrNone )
                {
                // The list is ready.
                iProgState = EDone;
                iWait.AsyncStop();
                }
            else
                {
                // Something went wrong -> no coverage.
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
            break;
        }

    OstTraceFunctionExit1( CCMWLANCOVERAGECHECK_RUNL_EXIT, this );
    }

// ----------------------------------------------------------------------------
// CCmWlanCoverageCheck::GetCoverageL
// ----------------------------------------------------------------------------
//
TBool CCmWlanCoverageCheck::GetCoverageL()
    {
    OstTraceFunctionEntry0( CCMWLANCOVERAGECHECK_GETCOVERAGEL_ENTRY );

    iProgState = EServiceStatus;
    iCoverage = EFalse;

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

        for ( scanInfo->First(); !scanInfo->IsDone(); scanInfo->Next() )
            {
            TBool isHidden( EFalse );

            TUint8 ieLen( 0 );
            const TUint8* ieData;

            TInt ret = scanInfo->InformationElement( E802Dot11SsidIE, ieLen,
                                                      &ieData );
            if ( ret == KErrNone )
                {
                isHidden = IsHiddenSsid( ieLen, ieData );
                if ( !isHidden )
                    {
                    // WLAN awailable...stop searching.
                    iCoverage = ETrue;
                    break;
                    }
                }
            else
                {
                User::Leave( ret );
                }
            }

        CleanupStack::PopAndDestroy( 2, wlanMgmt );
        }
    else
        {
        // Clean the cleanupstack in case status is KErrNone
        CleanupStack::PopAndDestroy( scanInfo );
        CleanupStack::PopAndDestroy( wlanMgmt );

        User::LeaveIfError( iStatus.Int() );
        }

    OstTraceFunctionExit0( CCMWLANCOVERAGECHECK_GETCOVERAGEL_EXIT );
    return iCoverage;
    }

// ---------------------------------------------------------
// CCmWlanCoverageCheck::IsHiddenSsid
// ---------------------------------------------------------
//
TBool CCmWlanCoverageCheck::IsHiddenSsid(
        TUint aSsidLength,
        const TUint8* aSsid ) const
    {
    OstTraceFunctionEntryExt( CCMWLANCOVERAGECHECK_ISHIDDENSSID_ENTRY, this );

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

    if ( !count || result2 )
        {
        result = ETrue;
        }

    OstTraceFunctionExitExt( CCMWLANCOVERAGECHECK_ISHIDDENSSID_EXIT, this, result );
    return result;
    }

