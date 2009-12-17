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
* Description:  Container class for an ID number.
*
*/

#include <e32cmn.h>
#include <rconnmon.h>

#include "log.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------
//
EXPORT_C TConnMonId::TConnMonId( TUint aId ): iId( aId )
    {
    }

// ---------------------------------------------------------------------------
//  iId getter
// ---------------------------------------------------------------------------
//
EXPORT_C TUint TConnMonId::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
//  Convert to HBufC*
// ---------------------------------------------------------------------------
//
EXPORT_C const HBufC* TConnMonId::ToBuf() const
    {
    HBufC* buf = HBufC::New( KMaxSize );

    LOG(_LIT(KPanicLocationMessage, "ConnectionMonitorServer: TConnMonId::ToBuf()");)
    __ASSERT_DEBUG( buf, User::Panic( KPanicLocationMessage, KErrNoMemory ) );

    if ( NULL != buf )
        {
        TPtr ptr( buf->Des() );
        ptr.Append( iId );
        }
    return buf;
    }

// ---------------------------------------------------------------------------
//  Convert from TPtrC
// ---------------------------------------------------------------------------
//
EXPORT_C TConnMonId TConnMonId::FromPtrC( const TPtrC& aPtrC )
    {
    TUint id( aPtrC[EId] );
    return TConnMonId( id );
    }

// End-of-file
