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
* Description:  Implementation of CCmListItemList
*
*/

#include "cmlistitemlist.h"
#include "cmlistitem.h"


LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ---------------------------------------------------------
// CCmListItemList::CCmListItemList
// ---------------------------------------------------------
//
CCmListItemList::CCmListItemList()
    : CArrayPtrFlat<CCmListItem>( KGranularity )
    {
    }

// ---------------------------------------------------------
// CCmListItemList::~CCmListItemList
// ---------------------------------------------------------
//
CCmListItemList::~CCmListItemList()
    {
    ResetAndDestroy();
    }

// ---------------------------------------------------------
// CCmListItemList::ItemForUid
// ---------------------------------------------------------
//
CCmListItem* CCmListItemList::ItemForUid( TUint32 aUid ) const
    {
    // This method cannot return "const CCmListItem*", because all methods
    // of CCmListItem are non-const -> if the returned item was const, it
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
