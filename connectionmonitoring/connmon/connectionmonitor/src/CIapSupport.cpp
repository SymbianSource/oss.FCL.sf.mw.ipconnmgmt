/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides IAP availability information.
*
*/

#include <commdb.h>
#include <rconnmon.h>

#include "CIapSupport.h"
#include "ConnMonIAP.h"
#include "ConnMonDef.h"
#include "log.h"

// -----------------------------------------------------------------------------
// CIapSupport::CIapSupport
// -----------------------------------------------------------------------------
//
CIapSupport::CIapSupport()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIapSupport::~CIapSupport()
    {
    }

// -----------------------------------------------------------------------------
// CIapSupport::NewL
// -----------------------------------------------------------------------------
//
CIapSupport* CIapSupport::NewL()
    {
    CIapSupport* self = new( ELeave ) CIapSupport;
    return self;
    }

// -----------------------------------------------------------------------------
// CIapSupport::GetIapsFromCommDBL using TFullIapInfo
// -----------------------------------------------------------------------------
//
void CIapSupport::GetIapsFromCommDbL(
        const TUint& aBearerId,
        TFullIapInfo& aIapBuf ) const
    {
    // Reset iap buffer
    aIapBuf.Reset();

    AppendIapsFromCommDbL( aBearerId, aIapBuf );
    }

// -----------------------------------------------------------------------------
// CIapSupport::AppendIapsFromCommDBL
// -----------------------------------------------------------------------------
//
void CIapSupport::AppendIapsFromCommDbL(
        const TUint& aBearerId,
        TFullIapInfo& aIapBuf ) const
    {
    LOGENTRFN("CIapSupport::AppendIapsFromCommDbL()")

    CCommsDatabase* commDB = NULL;
    CCommsDbTableView* table = NULL;

    // Is already full...
    if ( aIapBuf.iCount >= KConnMonMaxIAPCount )
        {
        LOGIT("AppendIapsFromCommDbL: table is already full")
        LOGEXITFN("CIapSupport::AppendIapsFromCommDbL()")
        return;
        }

    // Open commDB
    commDB = CCommsDatabase::NewL();
    CleanupStack::PushL( commDB );

    // Iterate the IAP table from CommDB and store iaps with matching bearerID
    table = commDB->OpenTableLC( TPtrC( IAP ) );

    TInt err = table->GotoFirstRecord();

    while ( !err && ( aIapBuf.iCount < KConnMonMaxIAPCount ) )
        {
        TUint iapBearerId( 0 );
        TUint32 iapID( 0 );
        TUint32 netID( 0 );
        TBuf<KCommsDbSvrMaxFieldLength> bearerType;
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;

        table->ReadUintL( TPtrC( COMMDB_ID ), iapID );
        table->ReadUintL( TPtrC( IAP_NETWORK ), netID );

        table->ReadTextL( TPtrC( IAP_BEARER_TYPE ), bearerType );
        table->ReadTextL( TPtrC( IAP_SERVICE_TYPE ), serviceType );

        // Find out the bearer ID of the IAP
        if ( ( bearerType == TPtrC( VIRTUAL_BEARER ) ) &&
                ( serviceType == TPtrC( VPN_SERVICE ) ) )
            {
            // VPN
            iapBearerId = EBearerIdVirtualVPN;
            }
        else if ( serviceType == TPtrC( DIAL_OUT_ISP ) ||
                serviceType == TPtrC( DIAL_IN_ISP ) )
            {
            // CSD
            if ( aBearerId == EBearerIdWcdmaCSD )
                {
                iapBearerId = EBearerIdWcdmaCSD;
                }
            else
                {
                iapBearerId = EBearerIdCSD;
                }
            }
        else if ( serviceType == TPtrC( OUTGOING_WCDMA ) ||
                serviceType == TPtrC( INCOMING_WCDMA ) )
            {
            // GPRS or WCDMA
            if ( aBearerId == EBearerIdWCDMA )
                {
                iapBearerId = EBearerIdWCDMA;
                }
            else
                {
                iapBearerId = EBearerIdGPRS;
                }
            }
        else if ( serviceType == TPtrC( LAN_SERVICE ) )
            {
            // LAN or WLAN

            // More checking is needed
            TUint32 rowId;

            table->ReadUintL( TPtrC( IAP_BEARER ), rowId );

            // Open service table
            CCommsDbTableView* table2 = commDB->OpenViewMatchingUintLC(
                    bearerType,
                    TPtrC( COMMDB_ID ),
                    rowId );

            // Get commDB name
            TBuf<KCommsDbSvrMaxFieldLength> commDbName;

            User::LeaveIfError( table2->GotoFirstRecord() );
            table2->ReadTextL( TPtrC( COMMDB_NAME ), commDbName );
            CleanupStack::PopAndDestroy( table2 );

            if ( commDbName == TPtrC( KWlanBearerRecordName ) )
                {
                // WLAN
                iapBearerId = EBearerIdWLAN;
                }
            else
                {
                // LAN = IP Passthrough
                iapBearerId = EBearerIdLAN;
                }
            }
        else
            {
            // Unknown bearer
            iapBearerId = KIapSupportUnknown;
            }

        // Drop IAPs that don't match with the aBearerId
        if ( aBearerId == iapBearerId ||
                ( aBearerId == EBearerIdAll && iapBearerId != KIapSupportUnknown ) )
            {
            aIapBuf.iIap[aIapBuf.iCount].iIapId = iapID;
            aIapBuf.iIap[aIapBuf.iCount].iNetId = netID;
            aIapBuf.iIap[aIapBuf.iCount].iBearerId = iapBearerId;
            aIapBuf.iCount++;
            }

        err = table->GotoNextRecord();
        }

    // err should be now KErrNotFound because there are no more records
    if ( err != KErrNotFound )
        {
        User::Leave( err );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( table );
    CleanupStack::PopAndDestroy( commDB );

    LOGEXITFN("CIapSupport::AppendIapsFromCommDbL()")
    }

// End-of-file
