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
 * Description:  Implementation of CFlexTimer class
 *
 */

/*
 * %version: 1 %
 */
//#include <e32panic.h>

#include "flextimer.h"
#include "flextimerpanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerTraces.h"
#endif

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexTimer::~CFlexTimer()
    {
    OstTrace1( TRACE_NORMAL, CFLEXTIMER_CFLEXTIMER,
               "CFlexTimer::~CFlexTimer;this=%x", this );
    
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// Fire timer at latest on the given interval. 32-bit interval.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::After( TTimeIntervalMicroSeconds32 aInterval )
    {
    OstTraceExt2( TRACE_NORMAL, CFLEXTIMER_AFTER32,
                  "CFlexTimer::After32;this=%x;aInterval=%d",
                  ( TUint )this, aInterval.Int() );
    
    __ASSERT_ALWAYS( IsAdded(),
                     User::Panic( KCFlexTimerPanicCat, 
                                  EFlexTimerNotAddedToActiveScheduler ) );
    iTimer.After( iStatus, aInterval );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Fire timer at latest on the given interval. 64-bit interval.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::After( TTimeIntervalMicroSeconds aInterval )
    {
    OstTraceExt2( TRACE_NORMAL, CFLEXTIMER_AFTER64,
                  "CFlexTimer::After64;this=%x;aInterval=%llu",
                  ( TUint )this, aInterval.Int64() );
    
    __ASSERT_ALWAYS( IsAdded(),
                     User::Panic(KCFlexTimerPanicCat,
                                 EFlexTimerNotAddedToActiveScheduler ) );
    iTimer.After( iStatus, aInterval );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Fire timer between at latest by the given time value.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::At( const TTime& aTime )
    {
    OstTraceExt2( TRACE_NORMAL, CFLEXTIMER_AT,
                  "CFlexTimer::At;this=%x;aTime=%lld", ( TUint )this,
                  aTime.Int64() );
    
    __ASSERT_ALWAYS( IsAdded(),
                     User::Panic( KCFlexTimerPanicCat,
                                  EFlexTimerNotAddedToActiveScheduler ) );
    iTimer.At( iStatus, aTime );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Fire timer between at latest by the given time value.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::AtUTC( const TTime& aTime )
    {
    OstTraceExt2( TRACE_NORMAL, CFLEXTIMER_ATUTC,
                  "CFlexTimer::AtUTC;this=%x;aTime=%lld",
                  ( TUint )this, aTime.Int64() );
    
    __ASSERT_ALWAYS( IsAdded(),
                     User::Panic(KCFlexTimerPanicCat,
                                 EFlexTimerNotAddedToActiveScheduler ) );
    iTimer.AtUTC( iStatus, aTime );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Sets the window size in which alignment is possible for the timer.
// This is a synchronous command. 32-bit function argument.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexTimer::Configure( TTimeIntervalMicroSeconds32 aWindowSize )
    {
    OstTraceExt2( TRACE_NORMAL, CFLEXTIMER_CONFIGURE,
                  "CFlexTimer::Configure32;this=%x;aWindowSize=%d",
                  ( TUint )this, aWindowSize.Int() );
    
    return iTimer.Configure( aWindowSize );
    }

// ---------------------------------------------------------------------------
// Sets the window size in which alignment is possible for the timer.
// This is a synchronous command. 64-bit function argument.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexTimer::Configure( TTimeIntervalMicroSeconds aWindowSize )
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_CFLEXTIMER_CONFIGURE,
                  "CFlexTimer::Configure64;this=%x;aWindowSize=%lld",
                  ( TUint )this, aWindowSize.Int64() );
    
    return iTimer.Configure( aWindowSize );
    }

// ---------------------------------------------------------------------------
// Constructs the object. Connects to the server and create a session.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::ConstructL()
    {
    OstTrace1( TRACE_NORMAL, CFLEXTIMER_CONSTRUCTL,
               "CFlexTimer::ConstructL;this=%x", ( TUint )this );
    
    User::LeaveIfError(iTimer.Connect());
    }

// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexTimer::CFlexTimer( TInt aPriority ) :
    CActive( aPriority )
    {
    }

// ---------------------------------------------------------------------------
// Inherited from CActive. Handles the object canceling operations.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexTimer::DoCancel()
    {
    OstTrace1( TRACE_NORMAL, CFLEXTIMER_DOCANCEL,
               "CFlexTimer::DoCancel;this=%x", ( TUint )this );
    
    iTimer.Cancel();
    }

