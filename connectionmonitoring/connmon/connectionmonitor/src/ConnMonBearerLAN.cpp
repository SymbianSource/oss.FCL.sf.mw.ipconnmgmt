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
* Description:  Implementation for bearer LAN.
*
*/

#include "ConnMonBearerLAN.h"
#include "connmoncommsdatcache.h"
#include "ConnMonIAP.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
TConnMonBearerLAN::TConnMonBearerLAN(
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
TInt TConnMonBearerLAN::GetAvailability( TBool& /*aAvailable*/ ) const
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// Bearer ID getter
// ---------------------------------------------------------------------------
//
TUint TConnMonBearerLAN::BearerId() const
    {
    return EBearerIdLAN;
    }

// ---------------------------------------------------------------------------
// ETrue if bearer is valid (internal state is correct)
// ---------------------------------------------------------------------------
//
TBool TConnMonBearerLAN::Valid() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Sets available IAPs of this type as available in CommsDat cache
// ---------------------------------------------------------------------------
//
void TConnMonBearerLAN::FlagAvailableIaps() const
    {
    LOGENTRFN("TConnMonBearerLAN::FlagAvailableIaps()")

    RArray<TUint> idArray;
    iConnMonIAP->GetActiveConnectionsIds( EBearerIdLAN, idArray );

    if ( idArray.Count() > 0 )
        {
        LOGIT1("Found %d active LAN IAPs", idArray.Count())
        for ( TInt i = 0; i < idArray.Count(); i++ )
            {
            iCommsDatCache->SetAvailableIapWithId( idArray[i] );
            }
        }
    else
        {
        // Currently LAN bearer only supported in emulator, unless LAN IAP already active.
        #ifdef __WINS__
        LOGIT("Emulator environment, enabling all LAN IAPs")
        iCommsDatCache->SetAvailableIapsWithBearerId( EBearerIdLAN );
        #endif
        }
    idArray.Close();

    LOGEXITFN("TConnMonBearerLAN::FlagAvailableIaps()")
    }

// End-of-file
