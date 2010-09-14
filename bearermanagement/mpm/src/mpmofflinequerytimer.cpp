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
 * Description: Implementation of class CMPMOfflineQueryTimer.
 * This timer is used for preventing offline query when the user
 * has cancelled the previous offline query.
 *
 */

#include "mpmlogger.h"
#include "mpmserver.h"
#include "mpmexpirytimer.h"
#include "mpmofflinequerytimer.h"

// ---------------------------------------------------------------------------
// Creates a new object by calling the two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPMOfflineQueryTimer* CMPMOfflineQueryTimer::NewL( CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::NewL" )
            
    CMPMOfflineQueryTimer* self = new( ELeave ) CMPMOfflineQueryTimer( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMPMOfflineQueryTimer::CMPMOfflineQueryTimer( CMPMServer* aServer ) :
    iServer( aServer )
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::CMPMOfflineQueryTimer" )
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CMPMOfflineQueryTimer::ConstructL()
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::ConstructL" )
    iExpiryTimer = CMPMExpiryTimer::NewL( *this, KTimeout );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMPMOfflineQueryTimer::~CMPMOfflineQueryTimer()
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::~CMPMOfflineQueryTimer" )
    if ( iExpiryTimer )
        {
        iExpiryTimer->Cancel();
        delete iExpiryTimer;
        }
    }

// ---------------------------------------------------------------------------
// Start the timer.
// ---------------------------------------------------------------------------
//
void CMPMOfflineQueryTimer::StartTimer()
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::StartTimer" )
    if ( iExpiryTimer )
        {
        iExpiryTimer->Cancel();
        iExpiryTimer->Start();    
        }
    }

// ---------------------------------------------------------------------------
// Handles the timeout.
// ---------------------------------------------------------------------------
//
void CMPMOfflineQueryTimer::HandleTimedOut()
    {
    MPMLOGSTRING( "CMPMOfflineQueryTimer::HandleTimedOut" )
    iServer->ResetOfflineQueryTimer();
    }
