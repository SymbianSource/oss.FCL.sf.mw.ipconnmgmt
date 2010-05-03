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
            TUint aPriority );

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

public:
    // Connection method ID.
    TUint32 iId;

    // Connection method bearer type.
    TUint32 iBearerType;

    // Priority value of the bearer type.
    TUint iBearerPriority;

    // Priority value in the CCDDataMobilitySelectionPolicy-record. Only used
    // for sorting purposes when building an array ofTCmmConnMethodItem-objects.
    TUint iPriority;
    };

#endif // CMMCONNMETHODITEM_H_

// End of file
