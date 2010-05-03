/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Class representing any kind on connection method, including embedded
* destination. Used for priority based ordering of connection methods inside
* destinations.
*
*/


#include "cmmconnmethoditem.h"


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
TCmmConnMethodItem::TCmmConnMethodItem()
    {
    iId = 0;
    iBearerType = 0;
    iBearerPriority = 0;
    iPriority = 0;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
TCmmConnMethodItem::TCmmConnMethodItem(
        TUint32 aId,
        TUint32 aBearerType,
        TUint aBearerPriority,
        TUint aPriority )
        :
        iId( aId ),
        iBearerType( aBearerType ),
        iBearerPriority( aBearerPriority ),
        iPriority( aPriority )
    {
    }

// ---------------------------------------------------------------------------
// A comparison function. Uses priority since items are in priority order.
// ---------------------------------------------------------------------------
//
TInt TCmmConnMethodItem::Compare(
        const TCmmConnMethodItem& aFirst,
        const TCmmConnMethodItem& aSecond )
    {
    // Zero if match, negative if first is smaller, positive otherwise.
    return ( TInt )aFirst.iPriority - ( TInt )aSecond.iPriority;
    }

// ---------------------------------------------------------------------------
// A comparison function to find a specific item by ID.
// (Items are not sorted by ID in the array.)
// ---------------------------------------------------------------------------
//
TBool TCmmConnMethodItem::FindCompare(
        const TUint32* aKey,
        const TCmmConnMethodItem& aEntry )
    {
    // True if match, false otherwise.
    TBool result( EFalse );
    if ( *aKey == aEntry.iId )
        {
        result = ETrue;
        }
    return result;
    }

// End of file
