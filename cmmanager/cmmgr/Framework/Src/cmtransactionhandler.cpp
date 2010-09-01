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
* Description:  Common transaction handler of framework and plugins.
*
*/

#include <e32std.h>
#include <e32base.h>

#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include "cmtransactionhandler.h"

using namespace CommsDat;

const TUint32 KMaxOpenTransAttempts = 10;
const TUint32 KRetryAfter = 100000;

// ---------------------------------------------------------------------------
// CCmTransactionHandler::NewL
// ---------------------------------------------------------------------------
//
CCmTransactionHandler* CCmTransactionHandler::NewL( CMDBSession& aDb )
    {
    CCmTransactionHandler* self = new (ELeave) CCmTransactionHandler( aDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );   // self
    return self;
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::CCmTransactionHandler
// ---------------------------------------------------------------------------
//
CCmTransactionHandler::CCmTransactionHandler( CMDBSession& aDb )
    : iDb( aDb )
    {
    CLOG_CREATE;
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CCmTransactionHandler::ConstructL()
    {
    TRAPD( err, CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl ) );
    
    if( !err )
        {
        iProtectionFlag = ECDProtectedWrite;
        }
    else
        {
        iProtectionFlag = 0;
        }
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::~CCmTransactionHandler
// ---------------------------------------------------------------------------
//
CCmTransactionHandler::~CCmTransactionHandler()
    {
    CLOG_CLOSE;
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::OpenTransactionLC
// ---------------------------------------------------------------------------
//
void CCmTransactionHandler::OpenTransactionLC( TBool aSetAttribs )
    {
    LOGGER_ENTERFN( "CCmTransactionHandler::OpenTransactionL" );

    ++iRefCount;
    CLOG_WRITE_1( "Refs: [%d]", iRefCount );

    // To call ::Close() on leave
    CleanupClosePushL( *this );

    // We don't have parent destination -> Session is opened only once        
    if( !iDb.IsInTransaction() )
        {
        TInt err( KErrNone );
        TUint32 attempts( KMaxOpenTransAttempts );
        
        do
            {
            CLOG_WRITE( "Opening" );
            TRAP( err, iDb.OpenTransactionL() );
            CLOG_WRITE_1( "Error: [%d]", err );
            
            if( err )
                {
                User::After( KRetryAfter );
                }
            }while( err && attempts-- );

        User::LeaveIfError( err );
        
        if( aSetAttribs )
            {
            iDb.SetAttributeMask( ECDHidden | iProtectionFlag );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmTransactionHandler::CommitTransactionL( TInt aError )
    {
    LOGGER_ENTERFN( "CCmTransactionHandler::CommitTransactionL" );
    
    --iRefCount;
    CLOG_WRITE_1( "ref: [%d]", iRefCount );
    CLOG_WRITE_1( "Error: [%d]", aError );

    CleanupStack::Pop( this );

    if( !iRefCount )
        {
        if( aError )
            {
            iDb.RollbackTransactionL();
            }
        else
            {
            iDb.CommitTransactionL();
            }

        iDb.ClearAttributeMask( ECDHidden | ECDProtectedWrite );
        }
    }

// ---------------------------------------------------------------------------
// CCmTransactionHandler::Close
// ---------------------------------------------------------------------------
//
void CCmTransactionHandler::Close()
    {
    LOGGER_ENTERFN( "CCmTransactionHandler::Close" );
    
    if( 0 == iRefCount )
        {
        CLOG_WRITE( "No active transaction. Do nothing" );
        return;
        }

    if( !iDb.IsInTransaction() )
        // Sometimes CommsDat closes the transaction 
        // on its own decision w/o any notification or reaseon.
        // e.g. when you try to delete a non-existing record.
        // It leaves with KErrNotFound, but rolls back the transaction.
        {
        CLOG_WRITE( "CommsDat already rolled back transaction. :(((" );
        iRefCount = 0;
        }
    else
        {
        --iRefCount;
        CLOG_WRITE_1( "ref: [%d]", iRefCount );

        if( !iRefCount )
            {
            iDb.ClearAttributeMask( ECDHidden | ECDProtectedWrite );
            
            if( iDb.IsInTransaction() )
                {
                TRAP_IGNORE( iDb.RollbackTransactionL() );
                }
            }
        }
    }
