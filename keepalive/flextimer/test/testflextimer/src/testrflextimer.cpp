/*
* ============================================================================
*  Name        : testrflextimer.cpp
*  Part of     : src / testflextimer
*  Description : STIF test cases for RFlexTimer
*  Version     : %version: 1 %
*
*  Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies).
*  All rights reserved.
*  This component and the accompanying materials are made available
*  under the terms of the License "Eclipse Public License v1.0"
*  which accompanies this distribution, and is available
*  at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
*  Initial Contributors:
*  Nokia Corporation - initial contribution.
*
*  Contributors:
*  Nokia Corporation
* ============================================================================
* Template version: 4.1
*/

#include <e32debug.h>               // for RDebug
#include <rflextimer.h>             // for RFlexTimer
#include <stiftesteventinterface.h> // for TEventIf 
#include "testflextimer.h"          // for global constants
#include "testrflextimer.h"


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CTestRFlexTimer::CTestRFlexTimer()
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CTestRFlexTimer::~CTestRFlexTimer()
    {
    }


// ---------------------------------------------------------------------------
// ___  ____ _    ____ _   _    ____ _  _ ____ ____ _  _ ____ 
// |  \ |___ |    |__|  \_/     |    |__| |___ |    |_/  [__  
// |__/ |___ |___ |  |   |      |___ |  | |___ |___ | \_ ___] 
// 
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
TBool CTestRFlexTimer::IsDelayOk( 
    const TTimeIntervalMicroSeconds aDelay,
    const TTimeIntervalMicroSeconds32 aInterval,
    const TTimeIntervalMicroSeconds32 aWindow )
    {
    TTimeIntervalMicroSeconds interval( aInterval.Int() );
    TTimeIntervalMicroSeconds window( aWindow.Int() );
    return IsDelayOk( aDelay, interval, window );
    }

// ---------------------------------------------------------------------------
TBool CTestRFlexTimer::IsDelayOk( 
    const TTimeIntervalMicroSeconds aDelay,
    const TTimeIntervalMicroSeconds32 aInterval,
    const TTimeIntervalMicroSeconds aWindow )
    {
    TTimeIntervalMicroSeconds interval( aInterval.Int() );
    return IsDelayOk( aDelay, interval, aWindow );
    }

// ---------------------------------------------------------------------------
TBool CTestRFlexTimer::IsDelayOk( 
    const TTimeIntervalMicroSeconds aDelay,
    const TTimeIntervalMicroSeconds aInterval,
    const TTimeIntervalMicroSeconds32 aWindow )
    {
    TTimeIntervalMicroSeconds window( aWindow.Int() );
    return IsDelayOk( aDelay, aInterval, window );
    }

// ---------------------------------------------------------------------------
TBool CTestRFlexTimer::IsDelayOk( 
    const TTimeIntervalMicroSeconds aDelay,
    const TTimeIntervalMicroSeconds aInterval,
    const TTimeIntervalMicroSeconds aWindow )
    {
    /**
     *  Timer can be expired 1 sec late or 1 system tick (15625 microseconds)
     *  early.
     */
    const TInt64 KTimerResolution( 1000000 ); 
    const TInt64 KTickResolution( 1000000 / 64 );
    
    RDebug::Print( 
       _L("=== Timer delay: %Ld, interval: %Ld, window: %Ld"), 
       aDelay.Int64(),
       aInterval.Int64(),
       aWindow.Int64() );
    
    return !( aDelay.Int64() < aInterval.Int64() - aWindow.Int64() - KTickResolution ||
              aDelay.Int64() > aInterval.Int64() + KTimerResolution );
    }

// ---------------------------------------------------------------------------
// Create, configure, start with AfterTicks and close RFlexTimer 
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CTestRFlexTimer::ExecuteAfterTicksL(
    TInt aTicks,
    TTimeIntervalMicroSeconds aWindowSize )
    {
    // Create, connect and configure the timer handle
    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    User::LeaveIfError( timer.Configure( aWindowSize ) );
    
    TTime startTime;
    startTime.UniversalTime();
    
    // Run the timer and wait its expiration
    TRequestStatus status;
    timer.AfterTicks( status, aTicks );
    User::WaitForRequest( status );
    
    TTime endTime;
    endTime.UniversalTime();

    // Clean up
    timer.Close();

    // Return running time of the timer
    return endTime.MicroSecondsFrom( startTime );
    }

// ---------------------------------------------------------------------------
// ___ ____ ____ ___    ____ ____ ____ ____ ____ 
//  |  |___ [__   |     |    |__| [__  |___ [__  
//  |  |___ ___]  |     |___ |  | ___] |___ ___] 
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TEST CASE: Start a timer using After (32 bit) and wait it to expire.
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ExpireAfter32L(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;
    
    const TTimeIntervalMicroSeconds32 KInterval( 3000000 ); // Time to wait timer
    const TTimeIntervalMicroSeconds KWindow( 0 );           // Window for check results

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    timer.After( status, KInterval );

                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //

    timer.Close();

    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start a timer using After (32 bit) and wait it to expire.
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ExpireAfter64L(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;
    
    const TTimeIntervalMicroSeconds KInterval( 3000000 );   // Time to wait timer
    const TTimeIntervalMicroSeconds KWindow( 0 );           // Window for check results

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    timer.After( status, KInterval );

                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //

    timer.Close();
    
    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }
    
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start a timer using AfterTicks and wait it to expire.
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ExpireAfterTicksL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;
    
    const TTimeIntervalMicroSeconds32 KInterval( 3000000 );     // Time to wait timer
    const TTimeIntervalMicroSeconds KWindow( 0 );               // Window for check results
    const TTimeIntervalMicroSeconds32 KOneTick( 1000000 / 64 ); // System tick: 1/64 sec

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    
    timer.AfterTicks( status, KInterval.Int() / KOneTick.Int() );
    
                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //

    timer.Close();
    
    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }
    
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start a timer using At and wait it to expire.
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ExpireAtL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;
    
    const TTimeIntervalMicroSeconds KInterval( 3000000 ); // Time to wait timer
    const TTimeIntervalMicroSeconds KWindow( 0 );         // Window for check results

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    TTime now;
    now.HomeTime();
    
    timer.At( status, now + KInterval );

                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //
    
    timer.Close();

    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start a timer using AtUTC and wait it to expire.
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ExpireAtUtcL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;
    
    const TTimeIntervalMicroSeconds KInterval( 3000000 ); // Time to wait timer
    const TTimeIntervalMicroSeconds KWindow( 0 );         // Window for check results

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    TTime now;
    now.UniversalTime();
    
    timer.AtUTC( status, now + KInterval );

                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //

    timer.Close();
    
    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with After (32 bit) without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::After32WithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    TRequestStatus status;

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    timer.After( status, TTimeIntervalMicroSeconds32( 1000000 ) );
    User::WaitForRequest( status ); // PANIC
    timer.Close();
    
    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with After (64 bit) without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::After64WithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    TRequestStatus status;

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );

    // Do the actual test
    //-----------------------------------------------------
    
    RFlexTimer timer;

    timer.After( status, TTimeIntervalMicroSeconds( 1000000 ) );
    User::WaitForRequest( status ); // PANIC
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with AfterTicks without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::AfterTicksWithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    TRequestStatus status;

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    timer.AfterTicks( status, 1000 );
    User::WaitForRequest( status ); // PANIC
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with At without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::AtWithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    TRequestStatus status;

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    TTime expirationTime;
    expirationTime.HomeTime();
    expirationTime += TTimeIntervalMicroSeconds( 1000000 );
    
    timer.At( status, expirationTime );
    User::WaitForRequest( status ); // PANIC
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with At without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::AtUtcWithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    TRequestStatus status;

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    TTime expirationTime;
    expirationTime.UniversalTime();
    expirationTime += TTimeIntervalMicroSeconds( 1000000 );
    
    timer.AtUTC( status, expirationTime );
    User::WaitForRequest( status ); // PANIC
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure (32 bit) timer without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::Configure32WithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    timer.Configure( TTimeIntervalMicroSeconds32( 1000000 ) ); // PANIC
    // Configure is synchronous command. No need to wait.
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure (32 bit) timer without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::Configure64WithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    timer.Configure( TTimeIntervalMicroSeconds( 1000000 ) ); // PANIC
    // Configure is synchronous command. No need to wait.
    timer.Close();
    
    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Cancel timer without connecting first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CancelWithoutConnect(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {

    // Test should panic with
    // Category: "KERN-EXEC"
    // Reason:   0
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        0 );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    timer.Cancel(); // PANIC
    // Cancel is synchronous command. No need to wait.
    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Call After twice
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CallAfterTwiceL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   15 (EFlexTimerServerErrorPendingTimer)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        15 );

    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    timer.After( status, TTimeIntervalMicroSeconds32( 100000 ) );
    timer.After( status, TTimeIntervalMicroSeconds( 100000 ) );

    User::WaitForRequest( status ); // PANIC

    timer.Close();

    //-----------------------------------------------------


    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Call AfterTicks twice
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CallAfterTicksTwiceL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   15 (EFlexTimerServerErrorPendingTimer)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        15 );

    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    timer.AfterTicks( status, 1000 );
    timer.AfterTicks( status, 1000 );

    User::WaitForRequest( status ); // PANIC

    timer.Close();

    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Call At twice
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CallAtTwiceL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   15 (EFlexTimerServerErrorPendingTimer)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        15 );

    TRequestStatus status;

    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );

    TTime expirationTime;
    expirationTime.HomeTime();
    expirationTime += TTimeIntervalMicroSeconds( 1000000 );
    
    timer.At( status, expirationTime );
    timer.At( status, expirationTime );

    User::WaitForRequest( status ); // PANIC

    timer.Close();

    //-----------------------------------------------------


    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Call AtUTC twice
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CallAtUtcTwiceL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   15 (EFlexTimerServerErrorPendingTimer)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        15 );

    TRequestStatus status;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    
    TTime expirationTime;
    expirationTime.UniversalTime();
    expirationTime += TTimeIntervalMicroSeconds( 1000000 );
    
    timer.AtUTC( status, expirationTime );
    timer.AtUTC( status, expirationTime );

    
    User::WaitForRequest( status ); // PANIC

    timer.Close();
    
    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Call Connect twice
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CallConnectTwiceL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   33 (EFlexTimerAlreadyConnected)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        33 );

    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    User::LeaveIfError( timer.Connect() ); // PANIC

    timer.Close();
    
    //-----------------------------------------------------

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Reconnect the handle to the server
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ReconnectL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    timer.Close();
    User::LeaveIfError( timer.Connect() );
    timer.Close();
    
    //-----------------------------------------------------

    aResult.SetResult( KErrNone, _L("Test case passed") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Cancel timer without starting it first
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::CancelWithoutStart(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;

    User::LeaveIfError( timer.Connect() );
    timer.Cancel();
    timer.Close();

    //-----------------------------------------------------

    aResult.SetResult( KErrNone, _L("Test case passed") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start tick timer with negative ticks
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::NegativeTicksInAfterTicksL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    const TInt KTestValue( -1 );
    
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Test should panic with
    // Category: "RFlexTimer"
    // Reason:   2 (EFlexTimerAfterTicksIntervalLessThanZero)
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        2 );

    TTimeIntervalMicroSeconds delay = ExecuteAfterTicksL( KTestValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start tick timer with zero ticks
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ZeroTicksInAfterTicksL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    const TInt KTestValue( 0 );
    
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTimeIntervalMicroSeconds delay = ExecuteAfterTicksL( KTestValue );

    // Analyze the results
    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( delay, 
                     TTimeIntervalMicroSeconds( KTestValue ),
                     TTimeIntervalMicroSeconds( 0 )) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Wrong expiration time.") );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Change system time while tick timer is active
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::ChangeTimeWhileAfterTicksL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    const TTimeIntervalMicroSeconds32 KTimerInterval( 10000000 );   // Time to wait timer
    const TTimeIntervalMicroSeconds KWaitTillChangeTime( 1000000 ); // Time to wait before change system time
    const TTimeIntervalMicroSeconds KTimeChange( 3000000 );         // Duration to change system time

    // Interval for check results
    const TTimeIntervalMicroSeconds KInterval( KTimerInterval.Int() + KTimeChange.Int64() );
    const TTimeIntervalMicroSeconds KWindow( 0 );                   // Window for check results
    const TTimeIntervalMicroSeconds32 KOneTick( 1000000 / 64 );     // System tick: 1/64 sec

    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus status, helperStatus;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timer;
    User::LeaveIfError( timer.Connect() );
    
    timer.AfterTicks( status, KTimerInterval.Int() / KOneTick.Int() );

    //-----------------------------------------------------
    // ... meanwhile change system time

        RFlexTimer helper;
        
        User::LeaveIfError( helper.Connect() );
        helper.After( helperStatus, KWaitTillChangeTime );
    
                                               //    //  ___     _____
        User::WaitForRequest( helperStatus ); // // // //_ // //  //
                                             //_//_// //  // //  //
        
        TTime newNow;
        newNow.UniversalTime();
        newNow += KTimeChange;
        
        User::SetUTCTime( newNow );
    
        helper.Close();

    // The test continues...
    //-----------------------------------------------------
    
                                     //    //  ___     _____
    User::WaitForRequest( status ); // // // //_ // //  //
                                   //_//_// //  // //  //

    timer.Close();

    //-----------------------------------------------------

    // Verify the test
    TTime endTime;
    endTime.UniversalTime();

    aResult.SetResult( KErrNone, _L("Test case passed") );
    if ( !IsDelayOk( endTime.MicroSecondsFrom( startTime ), KInterval, KWindow ) )
        {
        //RDebug::Print( _L("Timer delay: %lld"), 
        //               endTime.MicroSecondsFrom( startTime ).Int64() );
        aResult.SetResult( KErrGeneral, _L("Test case failed. Wrong expiration.") );
        }
    
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Timer window works correctly
// ---------------------------------------------------------------------------
//
TInt CTestRFlexTimer::TestAfterTicksWindowL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {

    // T1: expiration after 4-8 sec
    const TTimeIntervalMicroSeconds32 KTimerOneInterval( 8000000 );
    const TTimeIntervalMicroSeconds32 KTimerOneWindow( 4000000 );

    // T2: expiration after 9-10 sec
    // T2's interval must not overlap with T1's -- otherwise T1 will be 
    // delayed
    const TTimeIntervalMicroSeconds32 KTimerTwoInterval( 10000000 );
    const TTimeIntervalMicroSeconds32 KTimerTwoWindow( 1000000 );

    // T3: expiration after 5 secs
    const TTimeIntervalMicroSeconds KAfterTimerInterval( 5000000 );
    const TTimeIntervalMicroSeconds KAfterTimerWindow( 0 );

    // System tick: 1/64 sec
    const TTimeIntervalMicroSeconds32 KOneTick( 1000000 / 64 );
    const TTimeIntervalMicroSeconds KNoWindow( 0 );

    
    
    // Initialize case
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    TTime startTime;
    startTime.UniversalTime();
    TRequestStatus oneStatus, twoStatus, afterStatus;
    
    // Do the actual test
    //-----------------------------------------------------

    RFlexTimer timerOne;
    User::LeaveIfError( timerOne.Connect() );
    timerOne.Configure( KTimerOneWindow );
    timerOne.AfterTicks( oneStatus, KTimerOneInterval.Int() / KOneTick.Int() );

    RFlexTimer timerTwo;
    User::LeaveIfError( timerTwo.Connect() );
    timerTwo.Configure( KTimerTwoWindow );
    timerTwo.AfterTicks( twoStatus, KTimerTwoInterval.Int() / KOneTick.Int() );
    
    RFlexTimer afterTimer;
    User::LeaveIfError( afterTimer.Connect() );
    afterTimer.Configure( KAfterTimerWindow );
    afterTimer.After( afterStatus, KAfterTimerInterval );
    
                                        //    //  ___     _____
    User::WaitForRequest( oneStatus ); // // // //_ // //  //
                                      //_//_// //  // //  //

    TTime oneEndTime;
    oneEndTime.UniversalTime();
                                        //    //  ___     _____
    User::WaitForRequest( twoStatus ); // // // //_ // //  //
                                      //_//_// //  // //  //

    TTime twoEndTime;
    twoEndTime.UniversalTime();
    
    timerOne.Close();
    timerTwo.Close();
    afterTimer.Close();

    //-----------------------------------------------------
    
    // Handle afterStatus too - to get rid of unhandled asynchronous message
    // error from STIF.
    User::WaitForRequest( afterStatus );
    
    aResult.SetResult( KErrNone, _L("Test case passed") );
    
    // T1 should be expired at the same time than T3 - check with T3's values
    if ( !IsDelayOk( oneEndTime.MicroSecondsFrom( startTime ), KAfterTimerInterval, KNoWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Timer one wrong expiration.") );
        }
    // T2 should be expired at it's max window
    else if ( !IsDelayOk( twoEndTime.MicroSecondsFrom( startTime ), KTimerTwoInterval, KNoWindow ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. Timer two wrong expiration.") );
        }
    
    __UHEAP_MARKEND;

    return KErrNone;
    }
