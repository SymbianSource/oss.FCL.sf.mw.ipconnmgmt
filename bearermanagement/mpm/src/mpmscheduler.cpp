/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Active scheduler for MPM server.
*
*/

#include "mpmscheduler.h"
#include "mpmlogger.h"

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CMpmScheduler* CMpmScheduler::NewL()
    {
    CMpmScheduler* self = CMpmScheduler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CMpmScheduler* CMpmScheduler::NewLC()
    {
    CMpmScheduler* self = new( ELeave ) CMpmScheduler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMpmScheduler::~CMpmScheduler()
    {
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CMpmScheduler::CMpmScheduler() : iMpmServer( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CMpmScheduler::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Set the MPM server pointer. Used to restart the server in error situation
// if needed.
// ---------------------------------------------------------------------------
//
void CMpmScheduler::SetMpmServer( CServer2* aMpmServer )
    {
    iMpmServer = aMpmServer;
    }

// ---------------------------------------------------------------------------
// Error handling for the active scheduler.
// Handles any leave occuring in an active object's RunL() function that hasn't
// been properly handled in the active object's own RunError() function.
//
// All active objects in MPM server should handle their own error situations.
// If execution ends up here, there is a problem in the originating active
// object RunL() function and the problem should be fixed there. 
// ---------------------------------------------------------------------------
//
void CMpmScheduler::Error( TInt aError ) const
    {
    MPMLOGSTRING2( "CMpmScheduler::Error, ERROR: %d", aError )
    aError = aError;

    // Restart server if not active.
    if ( iMpmServer && !iMpmServer->IsActive() )
        {
        iMpmServer->ReStart();
        }
    }

// End of file
