/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for bearer WLAN.
*
*/

#include "ConnMonBearerWLAN.h"
#include "connmoncommsdatcache.h"
#include "ConnMonIAP.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
TConnMonBearerWLAN::TConnMonBearerWLAN(
        CConnMonIAP* aConnMonIAP,
        CConnMonCommsDatCache* aCommsDatCache )
        :
        TConnMonBearer( aConnMonIAP, aCommsDatCache )
    {
    }

// ---------------------------------------------------------------------------
// Is bearer available
// ---------------------------------------------------------------------------
//
TInt TConnMonBearerWLAN::GetAvailability( TBool& /*aAvailable*/ ) const
    {
    // Not defined or not needed
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Bearer ID getter
// ---------------------------------------------------------------------------
//
TUint TConnMonBearerWLAN::BearerId() const
    {
    return EBearerIdWLAN;
    }

// ---------------------------------------------------------------------------
// ETrue if bearer is valid (internal state is correct)
// ---------------------------------------------------------------------------
//
TBool TConnMonBearerWLAN::Valid() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Sets available IAPs of this type as available in CommsDat cache
// ---------------------------------------------------------------------------
//
void TConnMonBearerWLAN::FlagAvailableIaps() const
    {
    LOGENTRFN("TConnMonBearerWLAN::FlagAvailableIaps()")

    RArray<TUint> availableIdArray;
    RArray<TUint> activeIdArray;

    iConnMonIAP->GetActiveConnectionsIds( EBearerIdWLAN, activeIdArray );

    if ( activeIdArray.Count() == 0 )
        {
        iConnMonIAP->AppendAvailableWLANIaps( availableIdArray );
        }
    else
        {
        LOGIT1("Found %d active WLAN IAPs", activeIdArray.Count())

        // Policy based roaming. If current WLAN connection RSS (received
        // signal strength) is not strong enough, don't give any WLAN IAPs in
        // available state.
        if ( iConnMonIAP->WlanRssGoodEnough() )
            {
            // KErrDisconnected will be returned if request for used SSID from WLAN engine fails.
            TInt err = iConnMonIAP->AppendAvailableWLANIapsBySsid( availableIdArray );
            if ( err != KErrNone )
                {
                availableIdArray.Reset();
                if ( err == KErrDisconnected )
                    {
                    activeIdArray.Reset(); // No longer connected.
                    iConnMonIAP->AppendAvailableWLANIaps( availableIdArray );
                    }
                }
            }
        else
            {
            // Current WLAN RSS is too weak, don't report any WLAN IAPs available.
            availableIdArray.Reset();
            activeIdArray.Reset();
            }
        }

    for ( TInt i = 0; i < availableIdArray.Count(); i++ )
        {
        iCommsDatCache->SetAvailableIapWithId( availableIdArray[i] );
        }
    for ( TInt i = 0; i < activeIdArray.Count(); i++ )
        {
        // This will only add EasyWLAN IAP ID as available in those cases when
        // the connection was started with it. This is done so there can't be a
        // situation when a connection is up but ConnMon reports that
        // connections IAP as unavailable. (EasyWLAN IAP has empty SSID field in
        // CommsDat, so AppendAvailableWLANIapsBySsid-method will not find it).
        iCommsDatCache->SetAvailableIapWithId( activeIdArray[i] );
        }

    availableIdArray.Close();
    activeIdArray.Close();
    LOGEXITFN("TConnMonBearerWLAN::FlagAvailableIaps()")
    }

// End-of-file
