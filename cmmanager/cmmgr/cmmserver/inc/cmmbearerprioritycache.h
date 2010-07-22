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
* Implementation of bearer priority array cache.
*
*/


#ifndef CMMBEARERPRIORITYCACHE_H_
#define CMMBEARERPRIORITYCACHE_H_

#include <e32base.h>
#include <metadatabase.h>

class CCmmBearerPriority;
class CCmmTransactionHandler;


NONSHARABLE_CLASS( CCmmBearerPriorityCache ) : public CBase
    {
public:
    /**
     * Two phased constuction.
     */
    static CCmmBearerPriorityCache* NewL(
            CCmmTransactionHandler* aTrans,
            CommsDat::TMDBElementId aBearerPriorityTableId );

    /**
     * Two phased constuction.
     */
    static CCmmBearerPriorityCache* NewLC(
            CCmmTransactionHandler* aTrans,
            CommsDat::TMDBElementId aBearerPriorityTableId );

    /**
     * Destructor.
     */
    ~CCmmBearerPriorityCache();

private:
    /**
     * Constructor.
     */
    CCmmBearerPriorityCache(
            CCmmTransactionHandler* aTrans,
            CommsDat::TMDBElementId aBearerPriorityTableId );

    /**
     * 2nd phase constructor.
     * Reads the bearer priorities from CommsDat and stores them to own array.
     */
    void ConstructL();

public:
    /**
     * Updates the bearer priority array with the contents of aArray.
     */
    void UpdateL( const RPointerArray<CCmmBearerPriority>& aArray );

    /**
     * Copies the cache's contents to aArray.
     */
    void CopyL( RPointerArray<CCmmBearerPriority>& aArray ) const;

    /**
     * Get the priority value for a service type name from the bearer priority
     * cache.
     */
    TUint GetPriority( const TDesC& aServiceType ) const;

private:
    void ReadL();

private:
    // Database transaction handler.
    CCmmTransactionHandler* iTrans; // Not owned.

    // Cached contents of the bearer priority array.
    RPointerArray<CCmmBearerPriority> iArray; // Owns contents.

    // Global bearer type priorization table ID.
    CommsDat::TMDBElementId iBearerPriorityTableId;
    };

#endif // CMMBEARERPRIORITYCACHE_H_

// End of file
