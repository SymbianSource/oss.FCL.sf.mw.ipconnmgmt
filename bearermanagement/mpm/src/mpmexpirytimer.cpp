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
* Description:  Implementation of class CMPMExpiryTimer, a common timer class for MPM.
*
*/

#include "mpmexpirytimer.h"
#include "mpmexpirytimercallback.h"

// ---------------------------------------------------------------------------
// Constructs and returns the class object.
// ---------------------------------------------------------------------------
//
CMPMExpiryTimer* CMPMExpiryTimer::NewL( MMPMExpiryTimerCallback& aCallback, TInt aTimeout )
    {
    CMPMExpiryTimer* self = new( ELeave ) CMPMExpiryTimer( aCallback, aTimeout );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//

CMPMExpiryTimer::CMPMExpiryTimer( MMPMExpiryTimerCallback& aCallback, TInt aTimeout ):
        CTimer( CActive::EPriorityStandard ),
        iCallback( aCallback ),
        iTimeout( aTimeout )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//

void CMPMExpiryTimer::ConstructL()
    {
    CTimer::ConstructL();        
    }

// ---------------------------------------------------------------------------
// RunL, from CTimer, starts up the timer.
// ---------------------------------------------------------------------------
//

void CMPMExpiryTimer::Start()
    {
    After( iTimeout );
    }

// ---------------------------------------------------------------------------
// RunL, from CTimer, called when the timer expires.
// ---------------------------------------------------------------------------
//

void CMPMExpiryTimer::RunL()
    {
    iCallback.HandleTimedOut();
    }
