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
* Implementation of transaction handler.
*
*/


#ifndef CMMTRANSACTIONHANDLER_H_
#define CMMTRANSACTIONHANDLER_H_

#include <e32base.h>
#include <metadatabase.h>


/**
 * Transaction handler.
 */
NONSHARABLE_CLASS( CCmmTransactionHandler ) : public CBase
    {
public:
    /**
     * NewL.
     * Takes ownership of aDb.
     */
    static CCmmTransactionHandler* NewL( CommsDat::CMDBSession& aDb );

    /**
     * Destructor.
     * Intentionally not virtual.
     */
    ~CCmmTransactionHandler();

private:
    /**
     * Constructor.
     */
    CCmmTransactionHandler( CommsDat::CMDBSession& aDb );

    /**
     * Second phase constructor. Leaves on failure.
     */
    void ConstructL();

public:
    /**
     * Return the CommsDat session.
     */
    CommsDat::CMDBSession& Session() const;

    /**
     * Opens a CommsDat transaction if it is not already open. Reference
     * counter is inreased by one.
     */
    void OpenTransactionLC();

    /**
     * Decreases the reference counter by one. If it reaches zero, commits the
     * open CommsDat transaction. If an error code is given as argument, the
     * CommsDat transaction is rolled back instead.
     */
    void CommitTransactionL( TInt aError = KErrNone );

    /**
     * Performs RollbackTransactionL().
     */
    void Close();

    /**
     * Return the current reference count. Transaction is currently open if the
     * count above 0.
     */
    TUint32 GetReferenceCount();

private:
    // CommsDat session pointer.
    CommsDat::CMDBSession& iDb; // Owned.

    // Reference counter.
    TUint32 iRefCount;
    };

#endif // CMMTRANSACTIONHANDLER_H_

// End of file
