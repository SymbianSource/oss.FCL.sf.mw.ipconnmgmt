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
 * Description: Implementation of class CMPMConnSelectionDlgTimer.
 * This timer is used for preventing showing of connection selection
 * dialog when the user has cancelled the previous selection dialog.
 *
 */

#include "mpmlogger.h"
#include "mpmserver.h"
#include "mpmexpirytimer.h"
#include "mpmconnselectiondlgtimer.h"

// ---------------------------------------------------------------------------
// Creates a new object by calling the two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPMConnSelectionDlgTimer* CMPMConnSelectionDlgTimer::NewL( CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::NewL" )
            
    CMPMConnSelectionDlgTimer* self = new( ELeave ) CMPMConnSelectionDlgTimer( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMPMConnSelectionDlgTimer::CMPMConnSelectionDlgTimer( CMPMServer* aServer ) :
    iServer( aServer )
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::CMPMConnSelectionDlgTimer" )
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CMPMConnSelectionDlgTimer::ConstructL()
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::ConstructL" )
    iExpiryTimer = CMPMExpiryTimer::NewL( *this, KTimeout );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMPMConnSelectionDlgTimer::~CMPMConnSelectionDlgTimer()
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::~CMPMConnSelectionDlgTimer" )
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
void CMPMConnSelectionDlgTimer::StartTimer()
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::StartTimer" )
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
void CMPMConnSelectionDlgTimer::HandleTimedOut()
    {
    MPMLOGSTRING( "CMPMConnSelectionDlgTimer::HandleTimedOut" )
    iServer->ResetConnSelectionDlgTimer();
    }
