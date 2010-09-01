/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for the CConnMonWlanNetwork class.
*
*/

#include <e32cmn.h>
#include <rconnmon.h>

#include "ConnMonUtils.h"
#include "log.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
CConnMonWlanNetwork::CConnMonWlanNetwork()
    {
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetwork* CConnMonWlanNetwork::NewL()
    {
    CConnMonWlanNetwork* self = new( ELeave ) CConnMonWlanNetwork();
    return self;
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
CConnMonWlanNetwork::CConnMonWlanNetwork(
        const TBuf<KMaxNameLength>& aName,
        TUint aConnectionMode,
        TUint aSignalStrength,
        TUint aSecurityMode,
        const TBuf8<KWlanBssId>& aBssId,
        const TDesC& aVendorData )
        :
        iName( aName ),
        iConnectionMode( aConnectionMode ),
        iSignalStrength( aSignalStrength ),
        iSecurityMode( aSecurityMode ),
        iWlanBssId( aBssId ),
        iVendorData( NULL )
    {
    if ( aVendorData != KNullDesC() && aVendorData.Length() > 0 )
        {
        // Can't leave here, copy contents only if enough memory
        iVendorData = HBufC::New( aVendorData.Length() );
        if ( iVendorData )
            {
            iVendorData->Des().Copy( aVendorData );
            }
        }
    }

// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetwork* CConnMonWlanNetwork::NewL(
        const TBuf<KMaxNameLength>& aName,
        TUint aConnectionMode,
        TUint aSignalStrength,
        TUint aSecurityMode,
        const TBuf8<KWlanBssId>& aBssId,
        const TDesC& aVendorData )
    {
    CConnMonWlanNetwork* wlanNet = new( ELeave ) CConnMonWlanNetwork(
            aName,
            aConnectionMode,
            aSignalStrength,
            aSecurityMode,
            aBssId,
            aVendorData );

    return wlanNet;
    }

// ---------------------------------------------------------------------------
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CConnMonWlanNetwork::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//  Copy Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetwork* CConnMonWlanNetwork::NewL(
        CConnMonWlanNetwork& aConnMonWlanNetwork )
    {
    CConnMonWlanNetwork* wlanNet;
    if ( aConnMonWlanNetwork.iVendorData )
        {
        wlanNet = new( ELeave ) CConnMonWlanNetwork(
                aConnMonWlanNetwork.iName,
                aConnMonWlanNetwork.iConnectionMode,
                aConnMonWlanNetwork.iSignalStrength,
                aConnMonWlanNetwork.iSecurityMode,
                aConnMonWlanNetwork.iWlanBssId,
                aConnMonWlanNetwork.iVendorData->Des() );
        }
    else
        {
        wlanNet = new( ELeave ) CConnMonWlanNetwork(
                aConnMonWlanNetwork.iName,
                aConnMonWlanNetwork.iConnectionMode,
                aConnMonWlanNetwork.iSignalStrength,
                aConnMonWlanNetwork.iSecurityMode,
                aConnMonWlanNetwork.iWlanBssId,
                KNullDesC() );
        }
    return wlanNet;
    }

// ---------------------------------------------------------------------------
//  Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetwork::~CConnMonWlanNetwork()
    {
    delete iVendorData;
    }

// ---------------------------------------------------------------------------
//  = operator
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetwork& CConnMonWlanNetwork::operator=(
        CConnMonWlanNetwork& aConnMonWlanNetwork )
    {
    TInt err( KErrNone );

    iName.Copy( aConnMonWlanNetwork.Name() );
    iConnectionMode = aConnMonWlanNetwork.ConnectionMode();
    iSignalStrength = aConnMonWlanNetwork.SignalStrength();
    iSecurityMode = aConnMonWlanNetwork.SecurityMode();
    iWlanBssId = aConnMonWlanNetwork.WlanBssid();

    if ( aConnMonWlanNetwork.iVendorData )
        {
        delete iVendorData;
        iVendorData = NULL;
        TRAP( err, iVendorData = HBufC::NewL( aConnMonWlanNetwork.iVendorData->Length() ) );
        if ( err )
            {
            LOGIT1("VendorData handling error <%d>", err)
            }
        else
            {
            iVendorData = aConnMonWlanNetwork.iVendorData;
            }
        }
    return *this;
    }

// ---------------------------------------------------------------------------
// Compares two networks by iSignalStrength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonWlanNetwork::CompareBySignal(
        const CConnMonWlanNetwork& aFirst,
        const CConnMonWlanNetwork& aSecond )
    {
    //LOGENTRFN("CConnMonWlanNetwork::CompareBySignal()")

    TInt rc( 0 );
    if ( aFirst.iSignalStrength > aSecond.iSignalStrength )
        {
        rc = 1;
        }
    else if ( aFirst.iSignalStrength < aSecond.iSignalStrength )
        {
        rc = -1;
        }

    //LOGEXITFN1("CConnMonWlanNetwork::CompareBySignal()", rc)
    return rc;
    }

// ---------------------------------------------------------------------------
// Packages object to HBufC descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C const HBufC* CConnMonWlanNetwork::ToBuf() const
    {
    //LOGENTRFN("CConnMonWlanNetwork::ToBuf()")

    // Check the vendor data length
    TInt vendorDataSize( 0 );
    if ( iVendorData )
        {
        vendorDataSize = iVendorData->Length();
        }

    // Allocate buffer to "all" data
    HBufC* buf = HBufC::New( sizeof( CConnMonWlanNetwork ) + vendorDataSize );
    if ( !buf )
        {
        LOGIT("ToBuf: out of memory error")
        }
    else
        {
        TPtr ptr( buf->Des() );

        // Add CConnMonWlanNetwork object
        ptr.Copy( (TUint16*)this, (TInt)sizeof( CConnMonWlanNetwork ) / sizeof( TUint16 ) );

        // If vendor data exists, add it also
        if ( vendorDataSize > 0 )
            {
            ptr.Append( iVendorData->Des() );
            }
        }

    //LOGEXITFN("CConnMonWlanNetwork::ToBuf()")
    return buf;
    }

// ---------------------------------------------------------------------------
// Unpackages CConnMonWlanNetwork from TPtrC descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CConnMonWlanNetwork::FromPtrC(
        const TPtrC& aPtrC,
        CConnMonWlanNetwork* aConnMonWlanNetwork )
    {
    //LOGENTRFN("CConnMonWlanNetwork::FromPtrC()")
    TInt err( KErrNone );

    // Copy CConnMonWlanNetwork object data from the beginning of aPtrC
    memcpy( aConnMonWlanNetwork, aPtrC.Ptr(), sizeof( CConnMonWlanNetwork ) );

    // Reset vendor data pointer
    aConnMonWlanNetwork->iVendorData = NULL;

    // If vendor data exists, allocate memory and read it (it exists just after
    // CConnMonWlanNetwork object data) from aPtrC
    if ( aPtrC.Length() > sizeof( CConnMonWlanNetwork ) )
        {
        TInt vendorDataLen = aPtrC.Length() - sizeof( CConnMonWlanNetwork );

        aConnMonWlanNetwork->iVendorData = HBufC::New( vendorDataLen );
        if ( !aConnMonWlanNetwork->iVendorData )
            {
            err = KErrNoMemory;
            LOGIT("FromPtrC: out of memory error")
            }
        else
            {
            aConnMonWlanNetwork->iVendorData->Des().Copy(
                    aPtrC.Mid( sizeof( CConnMonWlanNetwork ), vendorDataLen ) );
            }
        }

    //LOGEXITFN1("CConnMonWlanNetwork::FromPtrC()", err)
    return err;
    }

// End-of-file
