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
* Description:  Declaration of the CDestListItemList class.
*
*/

#ifndef CDestLISTITEMLIST_H
#define CDestLISTITEMLIST_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class CDestListItem;

/**
 *  A list of CDestListItem-s pointers; items are owned.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CDestListItemList ) : public CArrayPtrFlat<CDestListItem>
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CDestListItemList();

        /**
        * Destructor. Items in the list are destroyed.
        */
        virtual ~CDestListItemList();

    public:  // New methods

        /**
        * Get pointer to the item having aUid,
        * or NULL if no such item is in the
        * list.
        * @param aUid Uid of item to get.
        * @return Pointer to the item having uid aUid, or NULL. Ownership
        * not passed (the list still owns the item).
        */
        CDestListItem* ItemForUid( TUint32 aUid ) const;
    };

#endif
