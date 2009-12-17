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
* Description:  A package class for CConnMonWlanNetwork objects.
*
*/

#include <e32std.h>
#include <rconnmon.h>

#include "log.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CConnMonWlanNetworksPtrArrayPckg::CConnMonWlanNetworksPtrArrayPckg(
        const RPointerArray<CConnMonWlanNetwork>& aRef,
        TUint aBufSize )
    {
    LOGENTRFN("CConnMonWlanNetworksPtrArrayPckg::CConnMonWlanNetworksPtrArrayPckg()")

    const TUint KItemLengthFieldSize = 1;

    // First 2 elements in buffer will contain the total number of objects and the number of transfered objects
    TUint currentPosition( 2 );
    TInt totalItemCount = aRef.Count();

    LOGIT2("CConnMonWlanNetworksPtrArrayPckg: buffer size %d, item count %d", aBufSize, totalItemCount)

    // Create a temporary pointer array table for buffer objects
    RPointerArray<HBufC> items;

    for ( TUint i = 0; i < totalItemCount; ++i )
        {
        const HBufC* item( aRef[i]->ToBuf() );
        if ( item )
            {
            // Check if there is room in buffer for the next item
            if ( ( currentPosition + KItemLengthFieldSize + item->Length() ) > aBufSize )
                {
                LOGIT4("Buffer full, current position %d, item length %d, buffer size %d, i %d",
                        currentPosition, item->Length(), aBufSize, i)
                delete item;
                item = NULL;
                break;
                }
            currentPosition += KItemLengthFieldSize + item->Length();
            items.Append( item );
            }
        }

    // Check that given buffer length (aBufSize) is not smaller than one
    // item (CConnMonWlanNetwork) + aRef count + items count + item length
    if ( items.Count() == 0 && totalItemCount > 0 )
        {
        aBufSize = 2; // aRef.Count(), items.Count()
        }

    // Allocate memory for buffer
    iBuf = HBufC::New( aBufSize );
    if ( !iBuf )
        {
        LOGIT("CConnMonWlanNetworksPtrArrayPckg: out of memory error")
        }
    else
        {
        // Add total and transferred counts to buffer
        iBuf->Des().Append( totalItemCount ); // Total amount of objects
        iBuf->Des().Append( items.Count() ); // Amount of objects in buffer

        // Add item length and item data to buffer
        for ( TUint i = 0; i < items.Count(); ++i )
            {
            iBuf->Des().Append( items[i]->Length() );
            iBuf->Des().Append( *items[i] );
            }
        LOGIT1("Used buffer %d", iBuf->Length())
        }
    items.ResetAndDestroy();

    LOGEXITFN("CConnMonWlanNetworksPtrArrayPckg::CConnMonWlanNetworksPtrArrayPckg()")
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetworksPtrArrayPckg::CConnMonWlanNetworksPtrArrayPckg(
        TUint aBufSize )
    {
    iBuf = HBufC::New( aBufSize );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CConnMonWlanNetworksPtrArrayPckg::~CConnMonWlanNetworksPtrArrayPckg()
    {
    delete iBuf;
    }

// ---------------------------------------------------------------------------
// Unpacking
// ---------------------------------------------------------------------------
//
EXPORT_C void CConnMonWlanNetworksPtrArrayPckg::UnpackToL(
        RPointerArray<CConnMonWlanNetwork>& aRef ) const
    {
    LOGENTRFN("CConnMonWlanNetworksPtrArrayPckg::UnpackToL()")

    if ( !iBuf || iBuf->Length() < 2 )
        {
        User::Leave( KErrBadDescriptor );
        }

    TUint index( 0 );
    TUint total( (*iBuf)[index++] ); // Total amount of objects
    TUint count( (*iBuf)[index++] ); // Amount of objects in  buffer
    LOGIT2("UnpackToL: total %d, in buffer %d", total, count)

    for ( TUint i = 0; i < count; ++i )
        {
        TUint len( (*iBuf)[index++] );
        TPtrC ptr( iBuf->Mid( index, len ) );
        index += len;

        CConnMonWlanNetwork* wlanNet = CConnMonWlanNetwork::NewL();
        CleanupStack::PushL( wlanNet );
        TInt err = CConnMonWlanNetwork::FromPtrC( ptr, wlanNet );
        if ( err != KErrNone )
            {
            LOGIT2("UnpackToL: error <%d> reading data, stopping. i %d", err, i)
            CleanupStack::Pop( wlanNet );
            break;
            }

        User::LeaveIfError( aRef.Append( wlanNet ) );
        CleanupStack::Pop( wlanNet );
        }

    LOGEXITFN("CConnMonWlanNetworksPtrArrayPckg::UnpackToL()")
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CConnMonWlanNetworksPtrArrayPckg::Buf() const
    {
    return iBuf;
    }

// End-of-file
