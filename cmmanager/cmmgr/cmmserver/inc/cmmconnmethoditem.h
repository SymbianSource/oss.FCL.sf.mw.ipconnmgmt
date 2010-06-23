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


#ifndef CMMCONNMETHODITEM_H_
#define CMMCONNMETHODITEM_H_

#include <e32base.h>


NONSHARABLE_CLASS( TCmmConnMethodItem )
    {
public:
    TCmmConnMethodItem();

    TCmmConnMethodItem(
            TUint32 aId,
            TUint32 aBearerType,
            TUint aBearerPriority,
            TUint aPriority,
            TBool aIsVirtual = 0,
            TUint32 aLinkedIapId = 0,
            TUint32 aLinkedSnapId = 0 );

    /**
     * A comparison function. Uses priority since items are in priority order.
     */
    static TInt Compare(
            const TCmmConnMethodItem& aFirst,
            const TCmmConnMethodItem& aSecond );

    /**
     * A comparison function to find a specific item by ID. (Items are not
     * sorted by ID in the array.)
     */
    static TBool FindCompare(
            const TUint32* aKey,
            const TCmmConnMethodItem& aEntry );

    /**
     * Return ETrue if this connection method item represents a virtual
     * destination.
     */
    TBool IsVirtual() const; 

    /**
     * Return ETrue if this connection method item represents an embedded
     * destination.
     */
    TBool IsEmbedded() const;

    /**
     * If this is a virtual IAP that points to an IAP, returns the ID of that
     * IAP, 0 otherwise.
     */
    TUint32 LinkedIapId() const;

    /**
     * If this is a virtual IAP that points to a SNAP, returns the ID of that
     * SNAP, 0 otherwise.
     */
    TUint32 LinkedSnapId() const;

public:
    // Connection method ID.
    TUint32 iId;

    // Connection method bearer type.
    TUint32 iBearerType;

    // Priority value of the bearer type.
    TUint iBearerPriority;

    // Priority value in the CCDDataMobilitySelectionPolicy-record. Only used
    // for sorting purposes when building an array of TCmmConnMethodItem-objects.
    TUint iPriority;

private:
    // ETrue if this connection method is virtual.
    TBool iIsVirtual;

    // If not 0, this is a virtual IAP that points to IAP with ID iLinkedIapId.
    TUint32 iLinkedIapId;

    // If not 0, this is a virtual IAP that points to SNAP with ID iLinkedSnapId.
    TUint32 iLinkedSnapId;
    };

#endif // CMMCONNMETHODITEM_H_

// End of file
