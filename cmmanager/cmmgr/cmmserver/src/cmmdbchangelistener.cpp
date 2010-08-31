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
* Listens for changes in one CommsDat table through CenRep.
*
*/


#include <commsdat.h>
#include <centralrepository.h>

#include "cmmdbchangelistener.h"
#include "cmmlistenermanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdbchangelistenerTraces.h"
#endif



// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmDbChangeListener* CCmmDbChangeListener::NewL(
        CCmmListenerManager* aListenerManager,
        TUint32 aTableId )
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_NEWL_ENTRY );

    CCmmDbChangeListener* self = CCmmDbChangeListener::NewLC( aListenerManager, aTableId );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_NEWL_EXIT );

    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmDbChangeListener* CCmmDbChangeListener::NewLC(
        CCmmListenerManager* aListenerManager,
        TUint32 aTableId )
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_NEWLC_ENTRY );

    CCmmDbChangeListener* self = new( ELeave ) CCmmDbChangeListener( aListenerManager, aTableId );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_NEWLC_EXIT );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// Cancels any active requests.
// ---------------------------------------------------------------------------
//
CCmmDbChangeListener::~CCmmDbChangeListener()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_CCMMDBCHANGELISTENER_ENTRY );

    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;

    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_CCMMDBCHANGELISTENER_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmDbChangeListener::CCmmDbChangeListener(
        CCmmListenerManager* aListenerManager,
        TUint32 aTableId )
        :
        CActive( 0 ),
        iListenerManager( aListenerManager ),
        iTableId( aTableId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMDBCHANGELISTENER_CCMMDBCHANGELISTENER_ENTRY );

    iRepository = NULL;
    iErrorCounter = 0;

    OstTraceFunctionExit0( DUP1_CCMMDBCHANGELISTENER_CCMMDBCHANGELISTENER_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmDbChangeListener::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_CONSTRUCTL_ENTRY );

    iRepository = CRepository::NewL( KCDCommsRepositoryId );
    CActiveScheduler::Add( this );
    // Not started yet.
    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the database table ID that this listener is monitoring.
// ---------------------------------------------------------------------------
//
TUint32 CCmmDbChangeListener::GetTableId()
    {
    // No traces.
    return iTableId;
    }

// ---------------------------------------------------------------------------
// Start the listener if not yet started.
// ---------------------------------------------------------------------------
//
TInt CCmmDbChangeListener::Start()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_START_ENTRY );

    TInt err( KErrNone );
    if ( !IsActive() )
        {
        err = RequestNotification();
        }
    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_START_EXIT );

    return err;
    }

// ---------------------------------------------------------------------------
// Cancels an outstanding request.
// ---------------------------------------------------------------------------
//
void CCmmDbChangeListener::DoCancel()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_DOCANCEL_ENTRY );

    iRepository->NotifyCancel( iTableId, KCDMaskShowRecordType );

    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_DOCANCEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Handles the asynchronous request completion event.
// ---------------------------------------------------------------------------
//
void CCmmDbChangeListener::RunL()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_RUNL_ENTRY );

    // Don't leave, or implement RunError()
    if ( iStatus.Int() < KErrNone )
        {
        // Error: iStatus.Int()
        iErrorCounter++;
        if ( iErrorCounter > KCmmCenRepErrorThreshold )
            {
            // Too many errors
            iListenerManager->DbChangeError( iTableId );
            OstTraceFunctionExit0( CCMMDBCHANGELISTENER_RUNL_EXIT );
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        }

    TInt err = RequestNotification(); // Start to listen for next change.

    // React to current change notification
    iListenerManager->DbChangeDetectedL( iTableId );

    if ( err )
        {
        iListenerManager->DbChangeError( iTableId );
        }
    OstTraceFunctionExit0( DUP1_CCMMDBCHANGELISTENER_RUNL_EXIT );
    }

// ---------------------------------------------------------------------------
// Initiates an asynchronous request to be notified of any change in the
// monitored database table.
// ---------------------------------------------------------------------------
//
TInt CCmmDbChangeListener::RequestNotification()
    {
    OstTraceFunctionEntry0( CCMMDBCHANGELISTENER_REQUESTNOTIFICATION_ENTRY );

    TInt err( KErrNone );
    err = iRepository->NotifyRequest( iTableId, KCDMaskShowRecordType, iStatus );
    if ( !err )
        {
        SetActive();
        }
    OstTraceFunctionExit0( CCMMDBCHANGELISTENER_REQUESTNOTIFICATION_EXIT );

    return err;
    }

// -----------------------------------------------------------------------------
// Handles the leave from the RunL()
// -----------------------------------------------------------------------------
//
TInt CCmmDbChangeListener::RunError( TInt /*aLeaveCode*/ )
    {
    OstTrace0( TRACE_ERROR, CCMMDBCHANGELISTENER_RUNERROR, "CCmmDbChangeListener::RunError" );

    return KErrNone;
    }

// End of file
