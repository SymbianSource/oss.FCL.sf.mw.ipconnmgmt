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
 * Description: Implementation of class CMPMConnPermQueryTimer.
 * This timer is used for preventing excessive connection permission queries,
 * when the used has cancelled the previous connection permission query.
 *
 */

#include "mpmlogger.h"
#include "mpmserver.h"
#include "mpmexpirytimer.h"
#include "mpmconnpermquerytimer.h"

// ---------------------------------------------------------------------------
// Creates a new object by calling the two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPMConnPermQueryTimer* CMPMConnPermQueryTimer::NewL( CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::NewL" )
            
    CMPMConnPermQueryTimer* self = new( ELeave ) CMPMConnPermQueryTimer( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMPMConnPermQueryTimer::CMPMConnPermQueryTimer( CMPMServer* aServer ) :
    iServer( aServer )
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::CMPMConnPermQueryTimer" )
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CMPMConnPermQueryTimer::ConstructL()
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::ConstructL" )
    iExpiryTimer = CMPMExpiryTimer::NewL( *this, KTimeout );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMPMConnPermQueryTimer::~CMPMConnPermQueryTimer()
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::~CMPMConnPermQueryTimer" )
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
void CMPMConnPermQueryTimer::StartTimer()
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::StartTimer" )
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
void CMPMConnPermQueryTimer::HandleTimedOut()
    {
    MPMLOGSTRING( "CMPMConnPermQueryTimer::HandleTimedOut" )
    iServer->ResetConnPermQueryTimer();
    }
