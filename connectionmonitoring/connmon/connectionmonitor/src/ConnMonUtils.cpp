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
* Description:  ConnMon utilities.
*
*/

#include "ConnMonUtils.h"
#include "log.h"

// -----------------------------------------------------------------------------
//Converts from TMobilePhoneNetworkMode to TConnMonMobilePhoneNetworkMode
// -----------------------------------------------------------------------------
//
TInt ConnMonUtils::MobilePhoneNetworkModeToConnMonMobilePhoneNetworkMode(
        RMobilePhone::TMobilePhoneNetworkMode aMode,
        TConnMonMobilePhoneNetworkMode& aValue )
    {
    switch ( aMode )
        {
        case RMobilePhone::ENetworkModeUnknown:
            aValue = EConnMonNetworkModeUnknown;
            break;
        case RMobilePhone::ENetworkModeUnregistered:
            aValue = EConnMonNetworkModeUnregistered;
            break;
        case RMobilePhone::ENetworkModeGsm:
            aValue = EConnMonNetworkModeGsm;
            break;
        case RMobilePhone::ENetworkModeAmps:
            aValue = EConnMonNetworkModeAmps;
            break;
        case RMobilePhone::ENetworkModeCdma95:
            aValue = EConnMonNetworkModeCdma95;
            break;
        case RMobilePhone::ENetworkModeCdma2000:
            aValue = EConnMonNetworkModeCdma2000;
            break;
        case RMobilePhone::ENetworkModeWcdma:
            aValue = EConnMonNetworkModeWcdma;
            break;
        case RMobilePhone::ENetworkModeTdcdma:
            aValue = EConnMonNetworkModeTdcdma;
            break;
        default:
            return KErrNotFound;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Converts from TDesC8 to build independent TDesC
// -----------------------------------------------------------------------------
//
void ConnMonUtils::TDes8ToTDes( const TDes8& aBuf8, TDes& aInd )
    {
    #if defined(_UNICODE)

    for ( TUint i = 0; i < aBuf8.Length(); ++i )
        {
        aInd.Append( aBuf8[i] );
        }
    #else
    aInd = aBuf8;
    #endif
    }

// -----------------------------------------------------------------------------
// Converts from TDesC to TDesC8
// -----------------------------------------------------------------------------
//
void ConnMonUtils::TDesToTDes8( const TDes& aSrc, TDes8& aDest )
    {
    for ( TUint i = 0; i < aSrc.Length(); ++i )
        {
        aDest.Append( aSrc[i] & 0x00FF );
        }
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

TConnMonInap::TConnMonInap()
    {
    }


TConnMonVirtualIap::TConnMonVirtualIap()
    {
    }

TBool TConnMonVirtualIap::Real() const
    {
    return EFalse;
    }

TBool TConnMonVirtualIap::Valid() const
    {
    return EFalse;
    }


TConnMonRealIap::TConnMonRealIap()
    {
    }

TBool TConnMonRealIap::Real() const
    {
    return ETrue;
    }

TBool TConnMonRealIap::Valid() const
    {
    return EFalse;
    }


TConnMonSNAP::~TConnMonSNAP()
    {
    }

EXPORT_C TBool TConnMonSNAP::Valid() const
    {
    return ETrue;
    }

TConnMonSNAP::TConnMonSNAP()
    {
    }

TConnMonSNAP::TConnMonSNAP( TUint aId ) : iId( aId )
    {
    }

TConnMonSNAP::TConnMonSNAP( const TConnMonSNAP& aSnap )
    {
    iId = aSnap.iId;
    }


CConnMonSNAPsAvailabilityManager* CConnMonSNAPsAvailabilityManager::NewL()
    {
    CConnMonSNAPsAvailabilityManager* self = new( ELeave ) CConnMonSNAPsAvailabilityManager();
    return self;
    }

CConnMonSNAPsAvailabilityManager::~CConnMonSNAPsAvailabilityManager()
    {
    }

CConnMonSNAPsAvailabilityManager::CConnMonSNAPsAvailabilityManager()
    {
    }


CConnMonCommsDataNotifier* CConnMonCommsDataNotifier::NewL()
    {
    CConnMonCommsDataNotifier* self = new( ELeave ) CConnMonCommsDataNotifier();
    return self;
    }

CConnMonCommsDataNotifier::~CConnMonCommsDataNotifier()
    {
    }

CConnMonCommsDataNotifier::CConnMonCommsDataNotifier()
    {
    }


CConnMonIapsAvailabilityManager* CConnMonIapsAvailabilityManager::NewL()
    {
    CConnMonIapsAvailabilityManager* self = new( ELeave ) CConnMonIapsAvailabilityManager();
    return self;
    }

CConnMonIapsAvailabilityManager::CConnMonIapsAvailabilityManager()
    {
    }

CConnMonIapsAvailabilityManager::~CConnMonIapsAvailabilityManager()
    {
    }

// End-of-file
