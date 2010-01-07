/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CExpiryTimer 
*
*/


#include "ExpiryTimer.h"

static const TInt KTimeout     = 60000000;
static const TInt KShortTimeout = 1000000;

// ---------------------------------------------------------------------------
// NewL. Constructs and returns the class object.
// ---------------------------------------------------------------------------
//
CExpiryTimer* CExpiryTimer::NewL( MExpiryTimerCallback& aCallback )
    {
    CExpiryTimer* self = new (ELeave) CExpiryTimer( aCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

CExpiryTimer::CExpiryTimer( MExpiryTimerCallback& aCallback ) 
 : CTimer(CActive::EPriorityStandard),
   iCallback( aCallback )
    {
    CActiveScheduler::Add(this);
    }

void CExpiryTimer::ConstructL()
    {
    CTimer::ConstructL();        
    }

// ---------------------------------------------------------------------------
// Start. Starts up the timer
// ---------------------------------------------------------------------------
//

void CExpiryTimer::Start()
    {
    TTimeIntervalMicroSeconds32 timeout = KTimeout;
    After( timeout );
    }

void CExpiryTimer::StartShort()
    {
    TTimeIntervalMicroSeconds32 timeout = KShortTimeout;
    After( timeout );
    }

void CExpiryTimer::RunL()
    {
    iCallback.HandleTimedOut();
    }
