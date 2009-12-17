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
* Description:  Implementation of transaction handler
*
*/

#ifndef CMTRANSACTION_HANDLER_H
#define CMTRANSACTION_HANDLER_H

#include <commsdattypesv1_1.h>

/**
 *  Transaction handler.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmTransactionHandler ) : public CBase
    {
    public:

        /** Epoc constructor */
        static CCmTransactionHandler* NewL( CommsDat::CMDBSession& aDb );
        
        // destructor is intentionally not virtual.
        ~CCmTransactionHandler();

    public:
    
        /**
        * @return commsdat session
        */
        CommsDat::CMDBSession& Session()const{ return iDb; };
        
        /**
        * - Checks if transaction is already opened
        * - Set iOwnTransaction is not and opens it and
        *   put 'this' on cleanup&close stack
        */
        void OpenTransactionLC( TBool aSetAttribs = ETrue );
        
        /**
        * - Checks if the transaction is ours.
        * - If yes, commits the transaction.
        * - anyway does nothing
        * @praram aError - error id
        */
        void CommitTransactionL( TInt aError = KErrNone );
        
        /**
        * Performs RollbackTransactionL().
        */
        void Close();     

    private:
    
        /** Constructor. */
        CCmTransactionHandler( CommsDat::CMDBSession& aDb );

        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    private: // data
    
        CommsDat::CMDBSession&    iDb;   ///< Not owned CommsDat session pointer
        TUint32         iRefCount;
        TUint32         iProtectionFlag;
    };

#endif // CMTRANSACTION_HANDLER
