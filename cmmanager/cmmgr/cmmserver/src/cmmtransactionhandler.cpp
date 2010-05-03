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
* Common transaction handler of framework and plugins.
*
*/


#include "cmmtransactionhandler.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmtransactionhandlerTraces.h"
#endif

const TUint32 KMaxOpenTransAttempts = 10;
const TUint32 KRetryAfter = 100000;


// ---------------------------------------------------------------------------
// NewL.
// ---------------------------------------------------------------------------
//
CCmmTransactionHandler* CCmmTransactionHandler::NewL( CommsDat::CMDBSession& aDb )
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_NEWL_ENTRY );

    CCmmTransactionHandler* self = new( ELeave ) CCmmTransactionHandler( aDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmTransactionHandler::~CCmmTransactionHandler()
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_CCMTRANSACTIONHANDLER_ENTRY );
    delete &iDb;
    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_CCMTRANSACTIONHANDLER_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmTransactionHandler::CCmmTransactionHandler( CommsDat::CMDBSession& aDb ) : iDb( aDb )
    {
    OstTraceFunctionEntry0( DUP1_CCMTRANSACTIONHANDLER_CCMTRANSACTIONHANDLER_ENTRY );
    iRefCount = 0;
    OstTraceFunctionExit0( DUP1_CCMTRANSACTIONHANDLER_CCMTRANSACTIONHANDLER_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmTransactionHandler::ConstructL()
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_CONSTRUCTL_ENTRY );
    iDb.SetAttributeMask( CommsDat::ECDHidden | CommsDat::ECDProtectedWrite );
    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Return the CommsDat session.
// ---------------------------------------------------------------------------
//
CommsDat::CMDBSession& CCmmTransactionHandler::Session() const
    {
    return iDb;
    }

// ---------------------------------------------------------------------------
// Opens a CommsDat transaction if it is not already open. Reference counter
// is inreased by one.
// ---------------------------------------------------------------------------
//
void CCmmTransactionHandler::OpenTransactionLC()
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_OPENTRANSACTIONLC_ENTRY );

    iRefCount++;
    CleanupClosePushL( *this );

    if ( !iDb.IsInTransaction() )
        {
        TInt err( KErrNone );
        TUint32 attempts( KMaxOpenTransAttempts );

        do
            {
            TRAP( err, iDb.OpenTransactionL() );
            if ( err )
                {
                User::After( KRetryAfter );
                }
            } while ( err && attempts-- );
        User::LeaveIfError( err );
        }

    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_OPENTRANSACTIONLC_EXIT );
    }

// ---------------------------------------------------------------------------
// Decreases the reference counter by one. If it reaches zero, commits the open
// CommsDat transaction. If an error code is given as argument, the CommsDat
// transaction is rolled back instead.
// ---------------------------------------------------------------------------
//
void CCmmTransactionHandler::CommitTransactionL( TInt aError )
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_COMMITTRANSACTIONL_ENTRY );

    iRefCount--;
    CleanupStack::Pop( this );

    if ( !iRefCount )
        {
        if ( aError )
            {
            iDb.RollbackTransactionL();
            }
        else
            {
            iDb.CommitTransactionL();
            }
        }

    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_COMMITTRANSACTIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Performs RollbackTransactionL().
// Pay attention to CleanupStack if calling this. The transaction handler
// needs to be popped from CleanupStack manually.
// ---------------------------------------------------------------------------
//
void CCmmTransactionHandler::Close()
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_CLOSE_ENTRY );

    if ( iRefCount == 0 )
        {
        // No active transaction, do nothing.
        OstTraceFunctionExit0( DUP1_CCMTRANSACTIONHANDLER_CLOSE_EXIT );
        return;
        }

    if ( !iDb.IsInTransaction() )
        {
        // Sometimes CommsDat closes the transaction on its own decision w/o any
        // notification or reason. E.g. when you try to delete a non-existing
        // record, it leaves with KErrNotFound, but rolls back the transaction.
        iRefCount = 0;
        }
    else
        {
        iRefCount--;

        if ( !iRefCount )
            {
            if ( iDb.IsInTransaction() )
                {
                TRAP_IGNORE( iDb.RollbackTransactionL() );
                }
            }
        }

    OstTraceFunctionExit0( DUP2_CCMTRANSACTIONHANDLER_CLOSE_EXIT );
    }

// ---------------------------------------------------------------------------
// Return the current reference count. Transaction is currently open if the
// count above 0.
// ---------------------------------------------------------------------------
//
TUint32 CCmmTransactionHandler::GetReferenceCount()
    {
    OstTraceFunctionEntry0( CCMTRANSACTIONHANDLER_GETREFERENCECOUNT_ENTRY );
    OstTraceFunctionExit0( CCMTRANSACTIONHANDLER_GETREFERENCECOUNT_EXIT );
    return iRefCount;
    }

// End of file
