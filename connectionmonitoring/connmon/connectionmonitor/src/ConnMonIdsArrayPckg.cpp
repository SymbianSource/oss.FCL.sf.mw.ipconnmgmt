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
* Description:  A package class for TConnMonId objects.
*
*/

#include <e32std.h>
#include <rconnmon.h>

#include "log.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Construction
// data is packed to the following format
// |all_data_amount|amount_of_transferring_data_(n)|item0|....|item(n-1)
// each item is packed in following format
// |size_of_item|item's_data
// item's data can be unpacked by MDesSerializer::FromPtr(), implemented by an item's class
// ---------------------------------------------------------------------------
//
ConnMonIdsArrayPckg::ConnMonIdsArrayPckg( const RArray<TConnMonId>& aRef, TUint aBufLen )
    {
    LOGENTRFN("ConnMonIdsArrayPckg::ConnMonIdsArrayPckg()")

    const TUint KItemLengthFieldSize = 1;

    // First 2 elements in buffer will contain the total number of objects and the number of transfered objects
    TUint currentPosition( 2 );
    TInt totalItemCount = aRef.Count();

    LOGIT2("ConnMonIdsArrayPckg: buffer size %d, item count %d", aBufLen, totalItemCount)

    // Create a temporary pointer array table for buffer objects
    RPointerArray<HBufC> items;
    // Add items until all added or buffer full
    for ( TUint i = 0; i < totalItemCount; ++i )
        {
        const HBufC* item( aRef[i].ToBuf() );
        if ( item )
            {
            // Check if there is room in buffer for the next item
            if ( ( currentPosition + KItemLengthFieldSize + item->Length() ) > aBufLen )
                {
                LOGIT4("Buffer full, current position %d, item length %d, buffer size %d, i %d",
                        currentPosition, item->Length(), aBufLen, i)
                delete item;
                item = NULL;
                break;
                }
            currentPosition += KItemLengthFieldSize + item->Length();
            items.Append( item );
            }
        }

    // Check that given buffer length (aBufLen) is not smaller than one
    // item (CConnMonWlanNetwork) + aRef count + items count + item length
    if ( items.Count() == 0 && totalItemCount > 0 )
        {
        aBufLen = 2; // aRef.Count(), items.Count()
        }

    // Allocate memory for buffer
    iBuf = HBufC::New( aBufLen );
    if ( !iBuf )
        {
        LOGIT("ConnMonIdsArrayPckg: out of memory error")
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

    LOGEXITFN("ConnMonIdsArrayPckg::ConnMonIdsArrayPckg()")
    }

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
EXPORT_C ConnMonIdsArrayPckg::ConnMonIdsArrayPckg( TUint aBufSize )
    {
    iBuf = HBufC::New( aBufSize );
    }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
EXPORT_C ConnMonIdsArrayPckg::~ConnMonIdsArrayPckg()
    {
    delete iBuf;
    }

// ---------------------------------------------------------------------------
// Unpacking
// ---------------------------------------------------------------------------
//
EXPORT_C void ConnMonIdsArrayPckg::UnpackToL( RArray<TConnMonId>& aRef ) const
    {
    if ( !iBuf || iBuf->Length() < 2 )
        {
        User::Leave( KErrBadDescriptor );
        }

    TUint index( 0 );
    TUint total( (*iBuf)[index++] ); // Amount of data totally to be transferred
    TUint count( (*iBuf)[index++] ); // Amount of packed data

    for ( TUint i = 0; i < count; ++i )
        {
        TUint len( (*iBuf)[index++] );
        TPtrC ptr( iBuf->Mid( index, len ) );
        User::LeaveIfError( aRef.Append( TConnMonId::FromPtrC( ptr ) ) );
        index += len;
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* ConnMonIdsArrayPckg::Buf() const
    {
    return iBuf;
    }

// End-of-file
