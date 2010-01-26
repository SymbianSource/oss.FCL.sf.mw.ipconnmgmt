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
* Description:  Implementation for bearer GPRS.
*
*/

#include <centralrepository.h>
#include <cmmanagerkeys.h>
#include <cmgenconnsettings.h>

#include "ConnMonBearerGPRS.h"
#include "connmoncommsdatcache.h"
#include "ConnMonIAP.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
TConnMonBearerGPRS::TConnMonBearerGPRS(
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
TInt TConnMonBearerGPRS::GetAvailability( TBool& aAvailable ) const
    {
    //LOGENTRFN("TConnMonBearerGPRS::GetAvailability()")
    TInt err( KErrNone );
    aAvailable = EFalse;

    // Is bearer available
    TBool byCell( EFalse );
    TBool byPhone( EFalse );
 
    if ( CurrentCellularDataUsage() != ECmCellularDataUsageDisabled )
        {
        // Find out if we are in 2G or in 3G - it is needed for calculating bearer availability
        RMobilePhone::TMobilePhoneNetworkMode mode;
        err = iConnMonIAP->MobilePhone()->GetCurrentMode( mode );

        if ( KErrNone == err )
            {
            switch ( mode )
                {
                case RMobilePhone::ENetworkModeGsm:
                case RMobilePhone::ENetworkModeUnknown: // Emulator default
                    err = iConnMonIAP->GetBearerSupportInfo( EBearerIdGPRS, byCell, byPhone );
                    break;
                case RMobilePhone::ENetworkModeWcdma:
                    err = iConnMonIAP->GetBearerSupportInfo( EBearerIdWCDMA, byCell, byPhone );
                    break;
                default:
                    LOGEXITFN1("TConnMonBearerGPRS::GetAvailability()", err)
                    return err; // Return silently, report not available
                }

            if ( KErrNone == err )
                {
                if ( byCell && byPhone )
                    {
                    aAvailable = ETrue;
                    }
                }
            }
        
    
        // Make GPRS available in wins
#if defined(__WINSCW__)
        LOGIT2("GetAvailability: Emulator override, true values were %d <%d>", aAvailable, err)
        err = KErrNone;
        aAvailable = ETrue;
#endif
        }
    
    //LOGEXITFN1("TConnMonBearerGPRS::GetAvailability()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Bearer ID getter
// ---------------------------------------------------------------------------
//
TUint TConnMonBearerGPRS::BearerId() const
    {
    return EBearerIdGPRS;
    }

// ---------------------------------------------------------------------------
// ETrue if bearer is valid (internal state is correct)
// ---------------------------------------------------------------------------
//
TBool TConnMonBearerGPRS::Valid() const
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// Sets available IAPs of this type as available in CommsDat cache
// ---------------------------------------------------------------------------
//
void TConnMonBearerGPRS::FlagAvailableIaps() const
    {
    LOGENTRFN("TConnMonBearerGPRS::FlagAvailableIaps()")

    TBool bearerAvailable( EFalse );
    TInt err = GetAvailability( bearerAvailable );
    LOGIT2("FlagAvailableIaps: GPRS availability: %d (bool) <%d>", bearerAvailable, err)
    if ( !err && bearerAvailable )
        {
        iCommsDatCache->SetAvailableIapsWithBearerId( EBearerIdGPRS );
        }

    LOGEXITFN("TConnMonBearerGPRS::FlagAvailableIaps()")
    }

// ---------------------------------------------------------------------------
// Read current cellular data usage setting from Central Repository
// ---------------------------------------------------------------------------
//
TInt TConnMonBearerGPRS::CurrentCellularDataUsage() const
    {
    // Use default value if repository is not available
    TInt value( ECmCellularDataUsageAutomatic );
    CRepository* cmRepository = NULL;
    
    TRAPD( err, cmRepository = CRepository::NewL( KCRUidCmManager ) )
    
    if ( err == KErrNone )
        {
        err = cmRepository->Get( KCurrentCellularDataUsage, value );
        LOGIT2("KCurrentCellularDataUsage from repository: %d error: <%d>", value, err )
        }
    
    delete cmRepository;
    return value;
    }
    
// End-of-file
