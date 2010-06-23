/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - Initial contribution
 *
 * Description:
 * This class contains implementation of CFlexTimerEngine.
 *
 */

// System include files
// None

// User include files
#include "flextimerengine.h"
#include "flextimercontainer.h"
#include "flextimerwakeuptimer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerengineTraces.h"
#endif


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexTimerEngine* CFlexTimerEngine::NewL()
    {
    CFlexTimerEngine* self = new (ELeave) CFlexTimerEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFlexTimerEngine::~CFlexTimerEngine()
    {
    OstTrace0( TRACE_INTERNAL,
        CFLEXTIMERENGINE_CFLEXTIMERENGINE,
        "CFlexTimerEngine::~CFlexTimerEngine" );

    delete iFlexTimerContainer;
    delete iWakeUpTimer;
    }
// ---------------------------------------------------------------------------
// Add new timer and trap leave. Return Leave code to session.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexTimerEngine::AddTimer(
    const TTimeIntervalMicroSeconds& aWinStartInterval,
    const TTimeIntervalMicroSeconds& aWinEndInterval,
    TBool aCancelAtSystemTimeChange,
    const MFlexTimerServiceCB* aFlexTimerServiceCB )
    {
    TRAPD( err,
        iFlexTimerContainer->AddTimerL( aWinStartInterval,
            aWinEndInterval,
            aCancelAtSystemTimeChange,
            aFlexTimerServiceCB ) );

    if ( KErrNone == err )
        {
        StartTimer();
        }
    return err;
    }
// ---------------------------------------------------------------------------
// Remove timer from container queue and start new wakeup watch timer
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexTimerEngine::CancelTimer(
    const MFlexTimerServiceCB* aFlexTimerServiceCB )
    {
    TInt error = iFlexTimerContainer->RemoveTimer( aFlexTimerServiceCB );
    StartTimer();
    
    return error;
    }

// ---------------------------------------------------------------------------
// Wakeup timer expired, Fire timers according to some algorithm set and
// restart wakeup timer.
// ---------------------------------------------------------------------------
//
void CFlexTimerEngine::WakeUp()
    {
    OstTrace0( TRACE_INTERNAL,
        CFLEXTIMERENGINE_WAKEUP,
        "CFlexTimerEngine::WakeUp" );
    
    iFlexTimerContainer->FireTimers(
        CFlexTimerContainer::EFlexTimerAlgorithmLatestPossible );

    StartTimer();
    }
// ---------------------------------------------------------------------------
// System time is changed. Abort all timers that need to be aborted and
// restart wakeup timer.
// ---------------------------------------------------------------------------
//
void CFlexTimerEngine::SystemTimeChanged()
    {
    OstTrace0( TRACE_INTERNAL,
        CFLEXTIMERENGINE_SYSTEMTIMECHANGED,
        "CFlexTimerEngine::SystemTimeChanged" );
    
    iFlexTimerContainer->AbortTimersDueToTimeChange( KErrAbort );
    StartTimer();
    }
// ---------------------------------------------------------------------------
// Private constructor
// ---------------------------------------------------------------------------
//
CFlexTimerEngine::CFlexTimerEngine() : iFlexTimerContainer( NULL ),
    iWakeUpTimer( NULL )
    {
    OstTrace0( TRACE_INTERNAL,
        DUP_CFLEXTIMERENGINE_CFLEXTIMERENGINE,
        "CFlexTimerEngine::CFlexTimerEngine" );
    //Nothing to do here
    }
// ---------------------------------------------------------------------------
// Private 2nd phase construction
// ---------------------------------------------------------------------------
//
void CFlexTimerEngine::ConstructL()
    {
    iFlexTimerContainer = CFlexTimerContainer::NewL();
    iWakeUpTimer = CFlexTimerWakeUpTimer::NewL( *this );
    }
// ---------------------------------------------------------------------------
// Stop timer just in case it is already running (e.g. after AddTimer())
// and Start new wakeup timer according to current situation in container
// queues If timeout window has passed fire timers right now.
// ---------------------------------------------------------------------------
//
void CFlexTimerEngine::StartTimer()
    {
    TTimeIntervalMicroSeconds nextTimeout;

    // Stop the timer, because, if there are no more pending timeouts, the
    // wake-up timer needs to be stopped. If timeouts are found, the timer is
    // is restarted below.
    iWakeUpTimer->StopTimer();

    if ( iFlexTimerContainer->GetNextTimeout( nextTimeout ) )
        {
        // If timeout is in the future, timer is started to wake up at that
        // moment.
        if ( nextTimeout > TTimeIntervalMicroSeconds( 0 ) )
            {
            iWakeUpTimer->StartTimer( nextTimeout );
            }
        // If timer is due now or in the past, Lets fire those right now.
        else
            {
            WakeUp();
            }
        }
    // Else no timers, so no need for timeouts. Server is propably about to be
    // deleted.
    }
