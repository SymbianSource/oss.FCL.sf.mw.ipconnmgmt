/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Declaration of the CApSelect class.
*
*/


// INCLUDE FILES

#include "ApListItemList.h"
#include "ApListItem.h"
#include "ApEngineLogger.h"


// CONSTANTS

LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApListItemList::CApListItemList
// ---------------------------------------------------------
//
EXPORT_C CApListItemList::CApListItemList()
: CArrayPtrFlat<CApListItem>( KGranularity )
    {
    }

// Destructor
// ---------------------------------------------------------
// CApListItemList::~CApListItemList
// ---------------------------------------------------------
//
EXPORT_C CApListItemList::~CApListItemList()
    {
    CLOG( ( EApList, 0, _L( "-> CApListItemList::~CApListItemList" ) ) );

    ResetAndDestroy();

    CLOG( ( EApList, 1, _L( "<- CApListItemList::~CApListItemList" ) ) );
    }

// ---------------------------------------------------------
// CApListItemList::ItemByUid
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItemList::ItemForUid( TUint32 aUid ) const
    {
    CLOG( ( EApList, 0, _L( "-> CApListItemList::ItemForUid" ) ) );

    // This method cannot return "const CApListItem*", because all methods
    // of CApListItem are non-const -> if the returned item was const, it
    // would be unusable.
    TInt i;
    TInt count = Count();
    for ( i = 0; i < count; i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            return At( i );
            }
        }

    CLOG( ( EApList, 0, _L( "-> CApListItemList::ItemForUid" ) ) );
    return NULL;
    }

//  End of File
