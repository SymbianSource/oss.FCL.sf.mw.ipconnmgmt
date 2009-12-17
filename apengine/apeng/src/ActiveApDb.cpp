/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CActiveApDb.
*
*/


// INCLUDE FILES

#include <ActiveApDb.h>
#include "ActiveApDbNotifier.h"
#include <ApEngineConsts.h>
#include "ApEngineCommons.h"
#include "ApEngineLogger.h"

const TInt KObserverArrayGranularity = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveApDb::NewL
// ---------------------------------------------------------
//
EXPORT_C CActiveApDb* CActiveApDb::NewL( TCommDbDatabaseType /*aType*/ )
    {
    CLOG( ( EActiveDb, 0, _L( "-> CActiveApDb::NewL" ) ) );

    CActiveApDb* db = new ( ELeave ) CActiveApDb;
    CleanupStack::PushL( db );
    db->ConstructL();
    CleanupStack::Pop( db ); // db

    CLOG( ( EActiveDb, 1, _L( "<- CActiveApDb::NewL" ) ) );
    return db;
    }

// ---------------------------------------------------------
// CActiveApDb::~CActiveApDb
// ---------------------------------------------------------
//
EXPORT_C CActiveApDb::~CActiveApDb()
    {
    CLOG( ( EActiveDb, 0, _L( "-> CActiveApDb::~CActiveApDb" ) ) );

    delete iDbNotifier;
    delete iObservers;
    delete iDb;

    CLOG( ( EActiveDb, 1, _L( "<- CActiveApDb::~CActiveApDb" ) ) );
    }


// ---------------------------------------------------------
// CActiveApDb::CActiveApDb
// ---------------------------------------------------------
//
CActiveApDb::CActiveApDb()
    {
    }


// ---------------------------------------------------------
// CActiveApDb::ConstructL
// ---------------------------------------------------------
//
void CActiveApDb::ConstructL()
    {
    iDb = CCommsDatabase::NewL( );
    iObservers = new ( ELeave ) CArrayPtrFlat<MActiveApDbObserver>
                        ( KObserverArrayGranularity );
    iDbNotifier = new ( ELeave ) CActiveApDbNotifier( *this );

    iState = EReady;
    }


// ---------------------------------------------------------
// CActiveApDb::AddObserverL
// ---------------------------------------------------------
//
EXPORT_C void CActiveApDb::AddObserverL( MActiveApDbObserver* anObserver )
    {
    CLOG( ( EActiveDb, 0, _L( "-> CActiveApDb::AddObserverL" ) ) );

    if ( !anObserver )
        {
        User::Leave( KErrNullPointerPassed );
        }
    iObservers->AppendL( anObserver );
    iDbNotifier->Start();

    CLOG( ( EActiveDb, 1, _L( "<- CActiveApDb::AddObserverL" ) ) );
    }


// ---------------------------------------------------------
// CActiveApDb::RemoveObserver
// ---------------------------------------------------------
//
EXPORT_C void CActiveApDb::RemoveObserver( MActiveApDbObserver* anObserver )
    {
    CLOG( ( EActiveDb, 0, _L( "-> CActiveApDb::RemoveObserver" ) ) );

    __ASSERT_DEBUG( anObserver, ApCommons::Panic( ENullPointer ) );
    TInt count = iObservers->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iObservers->At( i ) == anObserver )
            {
            iObservers->Delete( i );
            // leave cycle for faster processing, can not return because
            // we may have to stop() if it was the last one...
            break;
            }
        }
    if ( !iObservers->Count() )
        { // no more observers waiting so stop notification...
        iDbNotifier->Stop();
        }
    // for some reason, observer was not found->
    // Someone has already removed it
    // simply ignore it
    CLOG( ( EActiveDb, 1, _L( "<- CActiveApDb::RemoveObserver" ) ) );

    }


// ---------------------------------------------------------
// CActiveApDb::Database
// ---------------------------------------------------------
//
EXPORT_C CCommsDatabase*  CActiveApDb::Database()
    {
    CLOG( ( EActiveDb, 0, _L( "<-> CActiveApDb::Database" ) ) );

    return iDb;
    }

// ---------------------------------------------------------
// CActiveApDb::HandleDbEventL
// ---------------------------------------------------------
//
void CActiveApDb::HandleDbEventL( TInt anEvent )
    {
    CLOG( ( EActiveDb, 0, _L( "-> CActiveApDb::HandleDbEventL" ) ) );

    // possible events: EClose,EUnlock,ECommit,ERollback,ERecover
    switch ( anEvent )
        {
        case RDbNotifier::EUnlock:
            {
            CLOG( ( EActiveDb, 2, _L( "Notifier Unlock" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbAvailable );
            break;
            }
        case RDbNotifier::ECommit:
            {
            CLOG( ( EActiveDb, 2, _L( "Notifier Commit" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbChanged );
            break;
            }
        case RDbNotifier::EClose:
            {
            CLOG( ( EActiveDb, 2, _L( "Notifier Close/Rollbac/Recover" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbClosing );
            // Use base class' method to keep the notifier alive.
            // We need to know when all clients has released the db.
            break;
            }
        case RDbNotifier::ERollback:            
            {
            CLOG( ( EActiveDb, 2, _L( "Notifier Close/Rollbac/Recover" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbAvailable );
            break;
            }
        case RDbNotifier::ERecover:
            {
            CLOG( ( EActiveDb, 2, _L( "Notifier Close/Rollbac/Recover" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbClosing );
            // Use base class' method to keep the notifier alive.
            // We need to know when all clients has released the db.
            break;
            }
        default:
            {
            // Don't know what's happened, but instead of
            // Panicking, it's better to 'simulate' a change.
            // 'Defensive' programming.
            CLOG( ( EActiveDb, 2, _L( "Notifier UNKNOWN" ) ) );
            NotifyObserversL( MActiveApDbObserver::EDbChanged );
            break;
            }
        }
    CLOG( ( EActiveDb, 1, _L( "<- CActiveApDb::HandleDbEventL" ) ) );

    }




// ---------------------------------------------------------
// CActiveApDb::NotifyObserversL
// ---------------------------------------------------------
//
void CActiveApDb::NotifyObserversL( MActiveApDbObserver::TEvent anEvent )
    {

    TInt i;
    TInt count = iObservers->Count();
    // one hazard is that if one client leaves,
    // the rest of the clients will not be notified this time...
    for ( i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iObservers->At( i )->HandleApDbEventL( anEvent ) );
        }
    }

// End of File
