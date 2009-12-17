/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A package class for TConnMonWLANNetwork objects.
*
*/


#include <e32std.h>
#include "log.h"
#include "connmonwlannetwork.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg(
        const RArray<TConnMonWLANNetwork>& aRef,
        TUint aBufLen )
    {
    LOGENTRFN("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg()")

    RPointerArray<HBufC> items;
    LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: buflen <%d>", aBufLen )
    TUint len( 0 );
    LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: aRef.Count() <%d>", aRef.Count() )

    for ( TUint i = 0; i < aRef.Count(); ++i )
        {
        LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: i <%d>", i )

        const HBufC* item( aRef[i].ToBuf() );
        if ( item )
            {
            LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: Item->Length=<%d>", item->Length() )
            // Is there room in buffer
            TUint countFields( 3 ); // aRef.Count(); items->Count(); item->Length()
            if ( ( countFields + len + item->Length() ) > aBufLen )
                {
                LOGIT2("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: Not enough space to add item. Current len <%d>, item size <%d>", len, item->Length() )
                delete item;
                item = NULL;
                break;
                }
            LOGIT("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: Appending")
            items.Append( item );

            LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: len <%d>", len )
            ++(len+=item->Length()); // ++ is for item's size
            }
        else
            {
            LOGIT("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: Null Item!")
            }
        }

    // Check that given buflen (aBufLen) is not smaller than one
    // item (TConnMonWLANNetwork) + aRef count + items count
    // + item length
    if ( items.Count() == 0 && aRef.Count() > 0 )
        {
        aBufLen = 2; // aRef.Count(), items.Count()
        }

    iBuf = HBufC::New( aBufLen ); // Set size exactly to a buffer length
    LOG(_LIT( KPanicMessage, "ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg:" );)
    __ASSERT_DEBUG( iBuf, User::Panic( KPanicMessage, KErrNoMemory ) );

    if ( !iBuf )
        {
        items.ResetAndDestroy();
        items.Close();
        return;
        }

    LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: iBuf->Length() <%d>", iBuf->Length() )

    iBuf->Des().Append( aRef.Count() ); // Total amount of data
    iBuf->Des().Append( items.Count() );  // Amount of transferring data

    LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: iBuf->Length() <%d>", iBuf->Length() )

    for ( TUint i = 0; i < items.Count(); ++i )
        {
        LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: i <%d>", i )
        iBuf->Des().Append( items[i]->Length() );
        LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: iBuf->Length() <%d>", iBuf->Length() )
        iBuf->Des().Append( *items[i] );
        LOGIT1("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg: iBuf->Length() <%d>", iBuf->Length() )
        }

    items.ResetAndDestroy();
    items.Close();

    LOGEXITFN("ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg()")
    }

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
EXPORT_C ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg(
        TUint aBufSize )
    {
    iBuf = HBufC::New( aBufSize );
    LOG(_LIT( KPanicMessage, "ConnMonWLANNetworksArrayPckg::ConnMonWLANNetworksArrayPckg:" );)
    __ASSERT_DEBUG( iBuf, User::Panic( KPanicMessage, KErrNoMemory ) );
    }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
EXPORT_C ConnMonWLANNetworksArrayPckg::~ConnMonWLANNetworksArrayPckg()
    {
    delete iBuf;
    iBuf = NULL;
    }

// ---------------------------------------------------------------------------
// Unpacking
// ---------------------------------------------------------------------------
//
EXPORT_C void ConnMonWLANNetworksArrayPckg::UnpackToL(
        RArray<TConnMonWLANNetwork>& aRef ) const
    {
    if ( !iBuf )
        {
        User::Leave( KErrBadDescriptor );
        }

    TUint pointer( 0 );

    TUint total( (*iBuf)[pointer++] ); // Total amount of data
    TUint count( (*iBuf)[pointer++] ); // Amount of transferred data

    for ( TUint i = 0; i < count; ++i )
        {
        TUint len( (*iBuf)[pointer++] );
        TPtrC ptrc( iBuf->Mid( pointer, len ) );
        User::LeaveIfError( aRef.Append( TConnMonWLANNetwork::FromPtrC( ptrc ) ) );
        pointer += len;
        }
    }

// ---------------------------------------------------------------------------
// Getter
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* ConnMonWLANNetworksArrayPckg::Buf() const
    {
    return iBuf;
    }

// End-of-file
