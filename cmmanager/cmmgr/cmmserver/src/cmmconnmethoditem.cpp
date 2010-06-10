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


#include <cmmanagerdef.h>
#include <cmpluginembdestinationdef.h>

#include "cmmconnmethoditem.h"


// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
TCmmConnMethodItem::TCmmConnMethodItem()
        :
        iId( 0 ),
        iBearerType( 0 ),
        iBearerPriority( 0 ),
        iPriority( 0 ),
        iIsVirtual( 0 ),
        iLinkedIapId( 0 ),
        iLinkedSnapId( 0 )           
    {
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
TCmmConnMethodItem::TCmmConnMethodItem(
        TUint32 aId,
        TUint32 aBearerType,
        TUint aBearerPriority,
        TUint aPriority,
        TBool aIsVirtual,
        TUint32 aLinkedIapId,
        TUint32 aLinkedSnapId )
        :
        iId( aId ),
        iBearerType( aBearerType ),
        iBearerPriority( aBearerPriority ),
        iPriority( aPriority ),
        iIsVirtual( aIsVirtual ),
        iLinkedIapId( aLinkedIapId ),
        iLinkedSnapId( aLinkedSnapId )
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
    TInt result = ( TInt )aFirst.iPriority - ( TInt )aSecond.iPriority;

    if ( result == 0 &&
            aFirst.iPriority == CMManager::KDataMobilitySelectionPolicyPriorityWildCard )
        {
        // Embedded destination and virtual IAPs linking to SNAP have wildcard
        // priority.
        // If both connection methods have wildcard priority, only need to
        // ensure that an embedded destination looses in priority to everything
        // else.
        if ( aFirst.iBearerType == KUidEmbeddedDestination )
            {
            result = 1; // Positive.
            }
        else if ( aSecond.iBearerType == KUidEmbeddedDestination )
            {
            result = -1; // Negative.
            }
        }
    
    return result;
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

// ---------------------------------------------------------------------------
// Return ETrue if this connection method item represents a virtual
// destination.
// ---------------------------------------------------------------------------
//
TBool TCmmConnMethodItem::IsVirtual() const
    {
    return iIsVirtual;
    }

// ---------------------------------------------------------------------------
// Return ETrue if this connection method item represents an embedded
// destination.
// ---------------------------------------------------------------------------
//
TBool TCmmConnMethodItem::IsEmbedded() const
    {
    // No traces.
    TBool result( EFalse );
    if ( iBearerType == KUidEmbeddedDestination )
        {
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// If this is a virtual IAP that points to an IAP, returns the ID of that IAP,
// 0 otherwise.
// ---------------------------------------------------------------------------
//
TUint32 TCmmConnMethodItem::LinkedIapId() const
    {
    return iLinkedIapId;
    }

// ---------------------------------------------------------------------------
// If this is a virtual IAP that points to a SNAP, returns the ID of that SNAP,
// 0 otherwise.
// ---------------------------------------------------------------------------
//
TUint32 TCmmConnMethodItem::LinkedSnapId() const
    {
    return iLinkedSnapId;
    }

// End of file
