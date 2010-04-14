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
* Description:  Implementation for the TConnMonWLANNetwork class.
*
*/

#include <e32cmn.h>
#include "log.h"

const TInt KMaxWlanNetBufSize = 48;
#include "connmonwlannetwork.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------
//
EXPORT_C TConnMonWLANNetwork::TConnMonWLANNetwork(
        const TBuf<KMaxNameLength>& aName,
        TUint aConnectionMode,
        TUint aSignalStrength,
        TUint aSecurityMode,
        const RArray<TInt>& aBuf )
        :
        iName( aName ),
        iConnectionMode( aConnectionMode ),
        iSignalStrength( aSignalStrength ),
        iSecurityMode( aSecurityMode )
    {
    _LIT( KPanicMessage, "TConnMonWLANNetwork::TConnMonWLANNetwork:" );
    __ASSERT_ALWAYS( aBuf.Count() == KNumberOfExtraFields,
            User::Panic( KPanicMessage, KErrArgument ) );
    // This doesn't do anything as long as KNumberOfExtraFields equals 0
    // since it will panic on the assert if aBuf.Count is not 0
    // Left here for future reference
    /*
    for(TUint i(0); i < aBuf.Count(); ++i)
        {
        iBuf.Append(aBuf[i]);
        }
    */
    }

// ---------------------------------------------------------------------------
//  Destruction
// ---------------------------------------------------------------------------
//
EXPORT_C TConnMonWLANNetwork::~TConnMonWLANNetwork()
    {
    iBuf.Close();
    }

// ---------------------------------------------------------------------------
// compares two networks by iSignalStrength;
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TConnMonWLANNetwork::LessBySignal(
        const TConnMonWLANNetwork& aFirst,
        const TConnMonWLANNetwork& aSecond )
    {
    if ( aFirst.iSignalStrength > aSecond.iSignalStrength ) return 1;
    if ( aFirst.iSignalStrength < aSecond.iSignalStrength ) return -1;
    return 0;
    }

// ---------------------------------------------------------------------------
// Packages object to HBufC descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C const HBufC* TConnMonWLANNetwork::ToBuf() const
    {
    HBufC* buf = HBufC::New( KMaxWlanNetBufSize );
    if( !buf )
        {
        return buf;
        }

    TPtr ptr( buf->Des() );

    ptr.Append( iName.Length() );
    ptr.Append( iName );
    ptr.Append( iConnectionMode );
    ptr.Append( iSignalStrength );
    ptr.Append( iSecurityMode );
    ptr.Append( KNumberOfExtraFields );

    for( TUint i = 0; i < iBuf.Count(); ++i )
        {
        ptr.Append( iBuf[i] );
        }

    return buf;
    }

// ---------------------------------------------------------------------------
// Unpackages TConnMonWLANNetwork from TPtrC descriptor
// ---------------------------------------------------------------------------
//
EXPORT_C TConnMonWLANNetwork TConnMonWLANNetwork::FromPtrC( const TPtrC& aPtrC )
    {

    TUint len( (TUint)aPtrC[ENameLength] );
    TBuf<KMaxNameLength> name( aPtrC.Mid(EName, len) );
    TUint connectionMode( aPtrC[len+EConnectionMode] );
    TUint signalStrength( aPtrC[len+ESignalStrength] );
    TUint securityMode( aPtrC[len+ESecurityMode] );

    RArray<TInt> buf;
    TUint count( aPtrC[len+EBufCounter] );
    for ( TUint i = 1; i < count+1; ++i )
        {
        buf.Append( aPtrC[len+EBufBase+i] );
        }
    TConnMonWLANNetwork connMonWLANNetwork = TConnMonWLANNetwork( name, 
            connectionMode, signalStrength, securityMode, buf );
    buf.Close();
    return connMonWLANNetwork;
    }

// End-of-file
