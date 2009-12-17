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
* Description:  Implementation of CDestListItemList
*
*/

#include "destlistitemlist.h"
#include "destlistitem.h"

LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ---------------------------------------------------------
// CDestListItemList::CDestListItemList
// ---------------------------------------------------------
//
CDestListItemList::CDestListItemList()
    : CArrayPtrFlat<CDestListItem>( KGranularity )
    {
    }

// ---------------------------------------------------------
// CDestListItemList::~CDestListItemList
// ---------------------------------------------------------
//
CDestListItemList::~CDestListItemList()
    {
    ResetAndDestroy();
    }

// ---------------------------------------------------------
// CDestListItemList::ItemByUid
// ---------------------------------------------------------
//
CDestListItem* CDestListItemList::ItemForUid( TUint32 aUid ) const
    {
    // This method cannot return "const CDestListItem*", because all methods
    // of CDestListItem are non-const -> if the returned item was const, it
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

    return NULL;
    }
