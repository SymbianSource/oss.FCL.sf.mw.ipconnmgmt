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
* Manages all CommsDat table specific CenRep listeners.
*
*/


#include "cmmcache.h"
#include "cmmlistenermanager.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmlistenermanagerTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmListenerManager* CCmmListenerManager::NewL( CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_NEWL_ENTRY );

    CCmmListenerManager* self = CCmmListenerManager::NewLC( aCache );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmListenerManager* CCmmListenerManager::NewLC( CCmmCache* aCache )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_NEWLC_ENTRY );

    CCmmListenerManager* self = new( ELeave ) CCmmListenerManager( aCache );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmListenerManager::~CCmmListenerManager()
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_CCMMLISTENERMANAGER_ENTRY );

    // The destructor of each listener will cancel their own active request.
    iListeners.ResetAndDestroy();

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_CCMMLISTENERMANAGER_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmListenerManager::CCmmListenerManager( CCmmCache* aCache ) : iCache( aCache )
    {
    OstTraceFunctionEntry0( DUP1_CCMMLISTENERMANAGER_CCMMLISTENERMANAGER_ENTRY );
    OstTraceFunctionExit0( DUP1_CCMMLISTENERMANAGER_CCMMLISTENERMANAGER_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmListenerManager::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_CONSTRUCTL_ENTRY );

    // Create the basic set of listeners, start them if necessary and store them in iListeners-array.

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Adds a new listener to monitor for changes in the given database table.
// ---------------------------------------------------------------------------
//
void CCmmListenerManager::AddListenerL( TUint32 aIdentifier )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_ADDLISTENERL_ENTRY );

    TBool duplicateFound( EFalse );
    for ( TInt i = 0; i < iListeners.Count(); i++ )
        {
        if ( iListeners[i]->GetTableId() == aIdentifier )
            {
            duplicateFound = ETrue;
            break;
            }
        }

    if ( !duplicateFound )
        {
        CCmmDbChangeListener* dbChangeListener = CCmmDbChangeListener::NewLC( this, aIdentifier );
        iListeners.AppendL( dbChangeListener );
        CleanupStack::Pop( dbChangeListener );
        TInt err = dbChangeListener->Start();
        if ( err )
            {
            iCache->DbChangeError( aIdentifier );
            }
        }

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_ADDLISTENERL_EXIT );
    }

// ---------------------------------------------------------------------------
// Stops and deletes the listener monitoring the given database table.
// ---------------------------------------------------------------------------
//
TInt CCmmListenerManager::RemoveListener( TUint32 aIdentifier )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_REMOVELISTENER_ENTRY );

    TInt res( KErrNotFound );
    for ( TInt i = 0; i < iListeners.Count(); i++ )
        {
        if ( iListeners[i]->GetTableId() == aIdentifier )
            {
            delete iListeners[i];
            iListeners.Remove( i );
            res = KErrNone;
            break;
            }
        }

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_REMOVELISTENER_EXIT );
    return res;
    }

// ---------------------------------------------------------------------------
// Tells the listener manager that a change has been detected in the given
// database table.
// ---------------------------------------------------------------------------
//
void CCmmListenerManager::DbChangeDetected( TUint32 aIdentifier )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_DBCHANGEDETECTED_ENTRY );

    iCache->DbChangeDetected( aIdentifier );

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_DBCHANGEDETECTED_EXIT );
    }

// ---------------------------------------------------------------------------
// Tells the listener manager that an error has been encountered while
// listening the given database table. Change notifications will not be working
// for this database table.
// ---------------------------------------------------------------------------
//
void CCmmListenerManager::DbChangeError( TUint32 aIdentifier )
    {
    OstTraceFunctionEntry0( CCMMLISTENERMANAGER_DBCHANGEERROR_ENTRY );

    iCache->DbChangeError( aIdentifier );

    OstTraceFunctionExit0( CCMMLISTENERMANAGER_DBCHANGEERROR_EXIT );
    }

// End of file
