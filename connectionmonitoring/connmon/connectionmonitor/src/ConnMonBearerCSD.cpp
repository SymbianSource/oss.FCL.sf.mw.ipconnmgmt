/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for bearer CSD.
*
*/

#include "ConnMonBearerCSD.h"
#include "connmoncommsdatcache.h"
#include "ConnMonIAP.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
TConnMonBearerCSD::TConnMonBearerCSD(
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
TInt TConnMonBearerCSD::GetAvailability( TBool& aAvailable ) const
    {
    //LOGENTRFN("TConnMonBearerCSD::GetAvailability()")
    TInt err( KErrNone );
    aAvailable = EFalse;

    // Is bearer available
    TBool byCell( EFalse );
    TBool byPhone( EFalse );

    // Find out if we are in 2G or in 3G - it is needed for calculating bearer availability
    RMobilePhone::TMobilePhoneNetworkMode mode;
    err = iConnMonIAP->MobilePhone()->GetCurrentMode( mode );

    if ( KErrNone == err )
        {
        switch ( mode )
            {
            case RMobilePhone::ENetworkModeGsm:
            case RMobilePhone::ENetworkModeUnknown: // Emulator default
                err = iConnMonIAP->GetBearerSupportInfo( EBearerIdCSD, byCell, byPhone );
                break;
            case RMobilePhone::ENetworkModeWcdma:
                err = iConnMonIAP->GetBearerSupportInfo( EBearerIdWcdmaCSD, byCell, byPhone );
                break;
            default:
                LOGEXITFN1("TConnMonBearerCSD::GetAvailability()", err)
                return err; // Return silently (KErrNone), report as not available
            }

        if ( KErrNone == err )
            {
            if ( byCell && byPhone )
                {
                aAvailable = ETrue;
                }
            }
        }

    // Make CSD available in wins
#if defined(__WINSCW__)
    LOGIT2("GetAvailability: Emulator override, true values were %d <%d>", aAvailable, err)
    aAvailable = ETrue;
    err = KErrNone;
#endif

    //LOGEXITFN1("TConnMonBearerCSD::GetAvailability()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Bearer ID getter
// ---------------------------------------------------------------------------
//
TUint TConnMonBearerCSD::BearerId() const
    {
    return EBearerIdCSD;
    }

// ---------------------------------------------------------------------------
// ETrue if bearer is valid (internal state is correct)
// ---------------------------------------------------------------------------
//
TBool TConnMonBearerCSD::Valid() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Sets available IAPs of this type as available in CommsDat cache
// ---------------------------------------------------------------------------
//
void TConnMonBearerCSD::FlagAvailableIaps() const
    {
    LOGENTRFN("TConnMonBearerCSD::FlagAvailableIaps()")

    RArray<TUint> idArray;
    iConnMonIAP->GetActiveConnectionsIds( EBearerIdCSD, idArray );

    if ( idArray.Count() > 0 )
        {
        LOGIT1("Found %d active CSD IAPs", idArray.Count())
        for ( TInt i = 0; i < idArray.Count(); i++ )
            {
            iCommsDatCache->SetAvailableIapWithId( idArray[i] );
            }
        }
    else
        {
        TBool available( EFalse );
        TInt err = GetAvailability( available );
        LOGIT2("FlagAvailableIaps: CSD availability: %d (bool) <%d>", available, err)
        if ( KErrNone == err && available )
            {
            iCommsDatCache->SetAvailableIapsWithBearerId( EBearerIdCSD );
            }
        }
    idArray.Close();

    LOGEXITFN("TConnMonBearerCSD::FlagAvailableIaps()")
    }

// End-of-file
