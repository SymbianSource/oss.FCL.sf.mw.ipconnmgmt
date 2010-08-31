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
 * This class contains implementation of CFlexTimerWakeUpTimer.
 *
 */

// System include files
// None

// User include files go here:
#include "flextimerwakeuptimer.h"
#include "mflextimerwakeuptimercb.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerwakeuptimerTraces.h"
#endif

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFlexTimerWakeUpTimer* CFlexTimerWakeUpTimer::NewL(
    MFlexTimerWakeUpTimerCB& aObserver )
    {
    CFlexTimerWakeUpTimer* self = new (ELeave) CFlexTimerWakeUpTimer(
        aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// ---------------------------------------------------------------------------
// destructor, Cancel timer just in case it is still running.
// ---------------------------------------------------------------------------
//
CFlexTimerWakeUpTimer::~CFlexTimerWakeUpTimer()
    {
    // Super class CTimer destructor Cancels any pending requests.
    OstTrace0( TRACE_INTERNAL,
        DUP1_CFLEXTIMERWAKEUPTIMER_CFLEXTIMERWAKEUPTIMER,
        "CFlexTimerWakeUpTimer::~CFlexTimerWakeUpTimer" );
    }
// ---------------------------------------------------------------------------
// Start timer so that it will expire aInterval after this moment.
// UTC is used to get indications about system time change.
// ---------------------------------------------------------------------------
//
void CFlexTimerWakeUpTimer::StartTimer( TTimeIntervalMicroSeconds& aInterval )
    {
    CTimer::Cancel();
    TTime utcNow( 0 );

    utcNow.UniversalTime();
    utcNow = utcNow + aInterval;
    
    OstTraceExt1( TRACE_INTERNAL,
        CFLEXTIMERWAKEUPTIMER_STARTTIMER,
        "CFlexTimerWakeUpTimer::StartTimer;aInterval=%Ld",
        aInterval.Int64() );

    AtUTC( utcNow );
    }
// ---------------------------------------------------------------------------
// just call Cancel. If timer is alreay running it will be stopped.
// ---------------------------------------------------------------------------
//
void CFlexTimerWakeUpTimer::StopTimer()
    {
    OstTrace0( TRACE_INTERNAL,
        CFLEXTIMERWAKEUPTIMER_STOPTIMER,
        "CFlexTimerWakeUpTimer::StopTimer" );

    Cancel();
    }
// ---------------------------------------------------------------------------
// KErrAbort comes if system time has changed. It is up to client to restart
// timer
// ---------------------------------------------------------------------------
//
void CFlexTimerWakeUpTimer::RunL()
    {
    if ( KErrAbort == iStatus.Int() )
        {
        iObserver.SystemTimeChanged();
        }
    else
        {
        iObserver.WakeUp();
        }
    }
// ---------------------------------------------------------------------------
// Private constructor
// ---------------------------------------------------------------------------
//
CFlexTimerWakeUpTimer::CFlexTimerWakeUpTimer(
    MFlexTimerWakeUpTimerCB& aObserver ) :
        CTimer( EPriorityStandard ), iObserver( aObserver )
    {
    OstTrace0( TRACE_INTERNAL,
        CFLEXTIMERWAKEUPTIMER_CFLEXTIMERWAKEUPTIMER,
        "CFlexTimerWakeUpTimer::CFlexTimerWakeUpTimer" );

    //Nothing to do here
    }
// ---------------------------------------------------------------------------
// Private 2nd phase construction
// ---------------------------------------------------------------------------
//
void CFlexTimerWakeUpTimer::ConstructL()
    {
    CActiveScheduler::Add( this );
    CTimer::ConstructL();
    }
