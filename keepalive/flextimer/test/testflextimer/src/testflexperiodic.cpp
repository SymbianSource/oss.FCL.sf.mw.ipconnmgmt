/*
* ============================================================================
*  Name        : testflexperiodic.cpp
*  Part of     : src / testflextimer
*  Description : STIF test cases for CFlexPeriodic timer.
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
#include <flexperiodic.h>           // for CFlexPeriodic
#include <stiftesteventinterface.h> // for TEventIf 
#include "testflextimer.h"          // for global constants
#include "testflexperiodic.h"

/**
 *  Timer can be expired 1 sec late
 *  
 *  Note! Definition
 *  
 *      const TTimeIntervalMicroSeconds32 KTimerResolution( 1000000 );
 * 
 * will cause writable static data due non-trivial constructor of 
 * TTimeIntervalMicroSeconds32.
 *  
 */
const TInt KTimerResolution( 1000000 );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CTestFlexPeriodic::CTestFlexPeriodic()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CTestFlexPeriodic::~CTestFlexPeriodic()
    {
    }

// ---------------------------------------------------------------------------
// _ _ _ ____ _ ___    _    ____ ____ ___  
// | | | |__| |  |     |    |  | |  | |__] 
// |_|_| |  | |  |     |___ |__| |__| |    
//                                         
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Start scheduler for given time.
// ---------------------------------------------------------------------------
//
void CTestFlexPeriodic::WaitL( TTimeIntervalMicroSeconds32 aPeriod )
    {
    CPeriodic* watchdog = CPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( watchdog );
    watchdog->Start( aPeriod, aPeriod, TCallBack( StopScheduler, NULL ) );
    
    // Start scheduler. Wait here until the timer expires and stops the
    // scheduler.
    CActiveScheduler::Start();
    
    // Clean-up
    watchdog->Cancel();
    CleanupStack::PopAndDestroy( watchdog );
    }

// ---------------------------------------------------------------------------
// TCallBack function to stop the scheduler.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StopScheduler( TAny* /* aArgument */ )
    {
    CActiveScheduler::Stop();
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// ____ ____ _    _    ___  ____ ____ _  _ ____ 
// |    |__| |    |    |__] |__| |    |_/  [__  
// |___ |  | |___ |___ |__] |  | |___ | \_ ___] 
//                                              
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TCallBack function that does nothing. Not ment to be called - just to 
// fulfill the interface.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::DoNothing( TAny* /* aArgument */ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TCallBack function that panics testcase if it get called.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::PanicClient( TAny* /* aArgument */ )
    {
    User::Panic(_L("NotCalled CB got called"), 0xDEAD);
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// TCallBack function for adding time stamp to an RArray of TTimes.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::AddTimestamp( TAny* aArgument )
    {
    __ASSERT_ALWAYS(
        aArgument != NULL,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    //RDebug::Print( _L("CTestFlexPeriodic::AddTimestamp()") );
    
    RArray<TTime>* times =  reinterpret_cast<RArray<TTime>*>( aArgument );

    TTime now;
    now.UniversalTime();

    TInt err = times->Append( now ); // Data is copied by RArray
    __ASSERT_ALWAYS(
        err == KErrNone,
        User::Panic( KTestFlexTimerPanicCategory, err ) );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TCallBack function for restarting CFlexPeriodic timer.
// Action depends the value of iFirstTicksLeft;
// >0 -- Add timestamp to iFirstTimestamps 
// <0 -- Add timestamp to iSecondTimestamps
// =0 -- Add timestamp to iFirstTimestamps AND iSecondTimestamps AND restart
//       the timer with iSecondInterval
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::RestartTimer( TAny* aArgument )
    {
    __ASSERT_ALWAYS(
        aArgument != NULL,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    TRestartInfo* info =  reinterpret_cast<TRestartInfo*>( aArgument );

    __ASSERT_ALWAYS(
        info->iTimer != NULL && 
        info->iFirstTimestamps != NULL && 
        info->iSecondTimestamps != NULL,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );
    
    // Add current time to timestamps
    // Data is copied by RArray - no need to use heap
    TTime now;
    now.UniversalTime();
    
    TInt ticks( info->iFirstTicksLeft-- ); // Update the ticks
    TInt err( KErrNone );
    if ( ticks > 0 )
        {
        err = info->iFirstTimestamps->Append( now );
        }
    else if ( ticks < 0 )
        {
        err = info->iSecondTimestamps->Append( now );
        }
    else // ticks == 0
        {
        // Set the timestamps.
        // 1st timer settings expiration time is the starting time of the
        // second timer settings. Add timestamp to both arrays.
        err = info->iFirstTimestamps->Append( now );
        __ASSERT_ALWAYS(
            err == KErrNone,
            User::Panic( KTestFlexTimerPanicCategory, err ) );
        err = info->iSecondTimestamps->Append( now );

        // Restart the timer
        info->iTimer->Cancel();
        info->iTimer->Start(
            info->iSecondInterval,
            info->iSecondInterval, 
            TCallBack( RestartTimer, info ) );
        }
    __ASSERT_ALWAYS(
        err == KErrNone,
        User::Panic( KTestFlexTimerPanicCategory, err ) );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TCallBack function for trying to configure timer in callback.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureTimer( TAny* aArgument )
    {
    __ASSERT_ALWAYS(
        aArgument != NULL,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    TConfigureInfo* info =  reinterpret_cast<TConfigureInfo*>( aArgument );

    const TTimeIntervalMicroSeconds32 delayWindow32( info->iDelayWindow );
    const TTimeIntervalMicroSeconds32 intervalWindow32( info->iIntervalWindow );

    const TTimeIntervalMicroSeconds delayWindow64( info->iDelayWindow );
    const TTimeIntervalMicroSeconds intervalWindow64( info->iIntervalWindow );

    info->iConfigResult32 = info->iTimer->Configure( delayWindow32,
                                                     intervalWindow32);

    info->iConfigResult64 = info->iTimer->Configure( delayWindow64,
                                                     intervalWindow64);

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TCallBack function for starting a flexible periodic timer.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartTimer( TAny* aArgument )
    {
    __ASSERT_ALWAYS(
        aArgument != NULL,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    const TTimeIntervalMicroSeconds32 KTimerDelay( 2000000 );
    const TTimeIntervalMicroSeconds32 KTimerInterval( 2000000 );

    CFlexPeriodic* timer =  reinterpret_cast<CFlexPeriodic*>( aArgument );

    timer->Start(
        KTimerDelay,
        KTimerInterval,
        TCallBack( StartTimer, timer )
        );

    return KErrNone;
    }



// ---------------------------------------------------------------------------
// ___ _ _  _ ____ ____ ___ ____ _  _ ___     ____ _  _ _  _ ____ . ____ 
//  |  | |\/| |___ [__   |  |__| |\/| |__]    |___ |  | |\ | |    ' [__  
//  |  | |  | |___ ___]  |  |  | |  | |       |    |__| | \| |___   ___] 
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Verify that the timestamps are expired at window.
//
// Note that the timer resolution is 1 sec, so, the actual expiration time can
// be later than set.
// ---------------------------------------------------------------------------
//
TBool CTestFlexPeriodic::AreTimestampsAtWindow( 
    const RArray<TTime>& aTimestamps,
    const TTimeIntervalMicroSeconds32& aDelay,
    const TTimeIntervalMicroSeconds32& aInterval,
    const TTimeIntervalMicroSeconds32& aDelayWindow,
    const TTimeIntervalMicroSeconds32& aIntervalWindow )
    {
    __ASSERT_ALWAYS(
        aTimestamps.Count() >= 2 && 
        aDelay.Int() >= 0 &&
        aInterval.Int() >= 0 &&
        aDelayWindow.Int() >= 0 &&
        aIntervalWindow.Int() >= 0,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    // Timestamps are correct unless proved otherwise 
    TBool ret = ETrue;

    // For the 1st expiration is delay
    TTimeIntervalMicroSeconds delay;
    delay = aTimestamps[1].MicroSecondsFrom( aTimestamps[0] );
    RDebug::Print( _L("Timer delay %Ld"), delay.Int64() );

    if ( delay < aDelay.Int() - aDelayWindow.Int() || 
         delay > aDelay.Int() + KTimerResolution )
        {
        ret = EFalse;
        }

    // The rest of the expirations are intervals
    for ( TInt i( 2 ); i < aTimestamps.Count(); ++i )
        {
        TTimeIntervalMicroSeconds interval;
        interval = aTimestamps[i].MicroSecondsFrom( aTimestamps[i-1] );
        RDebug::Print( _L("Timer interval %Ld"), interval.Int64() );

        if ( interval < aInterval.Int() - aIntervalWindow.Int() || 
             interval > aInterval.Int() + KTimerResolution )
            {
            ret = EFalse;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// Convert the intervals from 64 bit to 32 bit and call 32 bit checking 
// function.
// ---------------------------------------------------------------------------
//
TBool CTestFlexPeriodic::AreTimestampsAtWindow( 
    const RArray<TTime>& aTimestamps,
    const TTimeIntervalMicroSeconds& aDelay,
    const TTimeIntervalMicroSeconds& aInterval,
    const TTimeIntervalMicroSeconds& aDelayWindow,
    const TTimeIntervalMicroSeconds& aIntervalWindow )
    {
    __ASSERT_ALWAYS(
        I64HIGH( aDelay.Int64() ) == 0 &&
        I64HIGH( aInterval.Int64() ) == 0 &&
        I64HIGH( aDelayWindow.Int64() ) == 0 &&
        I64HIGH( aIntervalWindow.Int64() ) == 0,
        User::Panic( KTestFlexTimerPanicCategory, KErrArgument ) );

    TTimeIntervalMicroSeconds32 delay( I64INT( aDelay.Int64() ) );
    TTimeIntervalMicroSeconds32 interval( I64INT( aInterval.Int64() ) );
    TTimeIntervalMicroSeconds32 delayWindow( I64INT( aDelayWindow.Int64() ) );
    TTimeIntervalMicroSeconds32 intervalWindow( I64INT( aIntervalWindow.Int64() ) );

    return AreTimestampsAtWindow( aTimestamps, 
                                  delay,
                                  interval,
                                  delayWindow,
                                  intervalWindow );
    }

// ---------------------------------------------------------------------------
// Compare are timestamp arrays same
// ---------------------------------------------------------------------------
//
TBool CTestFlexPeriodic::AreTimestampsSame(
    const RArray<TTime>& aLeft,
    const RArray<TTime>& aRight )
    {
    TBool ret( ETrue );
    if ( aLeft.Count() == aRight.Count() )
        {
        for ( TInt i( 0 ); i < aLeft.Count(); ++i )
            {
            if ( aLeft[i] != aRight[i] )
                { // Different timestamp has been found
                ret = EFalse;
                break;
                }
            }
        }
    else
        { // Arrays has different number of timestamps.
        ret = EFalse;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// ___ ____ _  _ ___  _    ____ ___ ____    ____ _  _ _  _ ____ . ____ 
//  |  |___ |\/| |__] |    |__|  |  |___    |___ |  | |\ | |    ' [__  
//  |  |___ |  | |    |___ |  |  |  |___    |    |__| | \| |___   ___] 
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Configure window sizes - template function
// 
// Timers are designed so that the 1st timer will test that the delay is
// correct and the 2nd timer will test that the interval is correct
// ---------------------------------------------------------------------------
template <class firstType, class secondType>
TInt CTestFlexPeriodic::ConfigureWindowL( TTestResult& aResult )
    {
    __UHEAP_MARK;

    // Constants
    const firstType KFirstDelay( 3000000 );             // 3 sec
    const firstType KFirstInterval( 2000000 );          // 2 sec
    const firstType KFirstDelayWindow( 0 );             // no window
    const firstType KFirstIntervalWindow( 1500000 );    // 1.5 sec

    const secondType KSecondDelay( 3500000 );           // 3.5 sec
    const secondType KSecondInterval( 1000000 );        // 1 sec
    const secondType KSecondDelayWindow( 1500000 );     // 1.5 sec
    const secondType KSecondIntervalWindow( 0 );        // no window

    const TUint KTestRunTime( 10000000 ); // 10 sec

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Create, configure and start the flexible periodic timer
    RArray<TTime> firstTimestamps;
    CFlexPeriodic* firstTimer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( firstTimer );
    firstTimer->Configure( KFirstDelayWindow, KFirstIntervalWindow );
    firstTimer->Start( 
        KFirstDelay, 
        KFirstInterval, 
        TCallBack( AddTimestamp, &firstTimestamps ) );

    RArray<TTime> secondTimestamps;
    CFlexPeriodic* secondTimer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( secondTimer );
    secondTimer->Configure( KSecondDelayWindow, KSecondIntervalWindow );
    secondTimer->Start( 
        KSecondDelay, 
        KSecondInterval,
        TCallBack( AddTimestamp, &secondTimestamps ) );

    // Initialize timer expiration time array with starting time to 
    // ease analysing of data.
    TTime startTime;
    startTime.UniversalTime();
    firstTimestamps.Append( startTime );
    secondTimestamps.Append( startTime );

    // The execution will be pending here while active scheduler is running...

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Analyze results
    aResult.SetResult( KErrNone, _L("Test case passed") );

    // Check the 1st timer expiration time
    if ( !AreTimestampsAtWindow( 
            firstTimestamps, 
            KFirstDelay, 
            KFirstInterval, 
            KFirstDelayWindow, 
            KFirstIntervalWindow ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. First timer not in window.") );
        }

    // Check the 2nd timer expiration time
    else if ( !AreTimestampsAtWindow( 
            secondTimestamps, 
            KSecondDelay, 
            KSecondInterval, 
            KSecondDelayWindow, 
            KSecondIntervalWindow ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Second timer not in window.") );
        }
    
    // Check that both timers are expired at the same time
    else if ( !AreTimestampsSame( firstTimestamps, secondTimestamps ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Timers are not expired at the same time.") );
        }

    // Clean up
    secondTimer->Cancel();
    secondTimestamps.Close();
    CleanupStack::PopAndDestroy( secondTimer );

    firstTimer->Cancel();
    firstTimestamps.Close();
    CleanupStack::PopAndDestroy( firstTimer );
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Configure window sizes and start timer with given values - template 
// function
// ---------------------------------------------------------------------------
template <class configureType, class startType>
void CTestFlexPeriodic::ConfigureAndStartTimerL( 
    RArray<TTime>& aTimestamps,
    TInt64 aDelay,
    TInt64 aInterval,
    TInt64 aDelayWindow,
    TInt64 aIntervalWindow )
    {
    const startType KDelay( aDelay );
    const startType KInterval( aInterval );
    const configureType KDelayWindow( aDelayWindow );
    const configureType KIntervalWindow( aIntervalWindow );

    const TUint KTestRunTime( 3000000 );

    TTime now;
    now.UniversalTime();
    aTimestamps.Append( now );

    // Create and start the flexible periodic timer
    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Configure( KDelayWindow, KIntervalWindow );
    timer->Start( KDelay, KInterval, TCallBack( AddTimestamp, &aTimestamps ) );
    
    // The execution will be pending here while active scheduler is running...

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Clean up
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with NULL callback 
// ---------------------------------------------------------------------------
//
template<class startType>
TInt CTestFlexPeriodic::StartWithNullCallBackL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    __UHEAP_MARK;

    // Constants
    const startType KTimerDelay( 1000000 );
    const startType KTimerInterval( 1000000 );

    TInt(* nullCallback)(TAny*) = NULL;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   31 (EFlexPeriodicCallbackFunctionIsNull)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 31 );
    
    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( nullCallback, NULL ) );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Panic thread after a few seconds
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::DoPanicL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds32 KTimerDelay( 1000000 );
    const TTimeIntervalMicroSeconds32 KTimerInterval( 1000000 );
    const TTimeIntervalMicroSeconds32 KTestRunTime( 3000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( DoNothing, NULL ) );

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    User::Panic(_L("Die die die!"), 0xDEAD);

    // We should NEVER be here...
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ____ _  _ _  _    _ _  _    ___ _  _ ____ ____ ____ ___  
// |__/ |  | |\ |    | |\ |     |  |__| |__/ |___ |__| |  \ 
// |  \ |__| | \|    | | \|     |  |  | |  \ |___ |  | |__/ 
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Run test case in own thread
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::RunInThread( 
    RThread& aThread, 
    TTestCaseArguments& aArguments )
    {
    // RThread::Create() parameters
    const TInt stackSize = 1024;
    const TInt heapMinSize = 1024;
    const TInt heapMaxSize = 1024;

    TBuf<8> processName;
    processName.Format( _L("%x"), &aArguments.iResult );
    
    // Create the thread
    TInt ret = aThread.Create(
        processName,
        RunTestCase, 
        stackSize,
        heapMinSize,
        heapMaxSize,
        &aArguments,
        EOwnerProcess );

    // Start execution of the thread
    aThread.Resume();

    return ret;
    }

// ---------------------------------------------------------------------------
// Create cleanup stack
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::RunTestCase( TAny* aArgument )
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Out of memory assert
    __ASSERT_ALWAYS(
        cleanup != NULL, 
        User::Panic( KTestFlexTimerPanicCategory, KErrNoMemory ) );
    
    TRAPD( err, CTestFlexPeriodic::RunTestCaseL( aArgument ) );

    delete cleanup;
    return err;
    }

// ---------------------------------------------------------------------------
// Create scheduler and run the test case
// ---------------------------------------------------------------------------
//
void CTestFlexPeriodic::RunTestCaseL( TAny* aArgument )
    {
    // Create and install active scheduler
    CActiveScheduler* scheduler = scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    
    // Parse the arguments
    TTestCaseArguments* args = reinterpret_cast<TTestCaseArguments*>( aArgument );
    
    // Call the function pointer with given arguments
    TInt ret = (*(args->iTestFunction))(args->iResult, args->iCallback );
    User::LeaveIfError( ret );

    // Clean up
    CleanupStack::PopAndDestroy( scheduler );
    }

// ---------------------------------------------------------------------------
// ___ ____ ____ ___    ____ ____ ____ ____ ____ 
//  |  |___ [__   |     |    |__| [__  |___ [__  
//  |  |___ ___]  |     |___ |  | ___] |___ ___] 
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// TEST CASE: Start one CFlexPeriodic and wait for few expirations.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartOneTimerL( 
        TTestResult& aResult, 
        CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds32 KTickInterval( 1000000 ); // 1 sec
    const TUint KTestRunTime( 5000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Storage for flexible periodic timer timestamps
    RArray<TTime> timestamps;

    // Create and start the flexible periodic timer
    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Start( 
        KTickInterval, 
        KTickInterval, 
        TCallBack( AddTimestamp, &timestamps ) );

    // Initialize timer expiration time array with starting time to 
    // ease analysing of data.
    TTime startTime;
    startTime.UniversalTime();
    timestamps.Append( startTime );
    
    // The execution will be pending here while active scheduler is running...

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Analyze results
    aResult.SetResult( KErrNone, _L("Test case passed") );

    // Only start time in timestamp array
    if ( timestamps.Count() == 1 )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. No timer expired.") );
        }

    // Check that the timers are expired at maximum delay due there are only
    // one timer, so, no alignment can be happened.
    if ( !AreTimestampsAtWindow( timestamps, KTickInterval, KTickInterval, 0, 0 ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Timer not in maximum window.") );
        }

    // Clean up
    timer->Cancel();
    timestamps.Close();
    CleanupStack::PopAndDestroy( timer );
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start one CFlexPeriodic and wait for few expirations.
// Give also Error callback function.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartOneTimerWithErrorCbL( 
        TTestResult& aResult, 
        CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds32 KTickInterval( 1000000 ); // 1 sec
    const TUint KTestRunTime( 5000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // Storage for flexible periodic timer timestamps
    RArray<TTime> timestamps;

    // Create and start the flexible periodic timer
    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Start( 
        KTickInterval, 
        KTickInterval, 
        TCallBack( AddTimestamp, &timestamps ),
        TCallBack( PanicClient, NULL ) );

    // Initialize timer expiration time array with starting time to 
    // ease analysing of data.
    TTime startTime;
    startTime.UniversalTime();
    timestamps.Append( startTime );
    
    // The execution will be pending here while active scheduler is running...

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Analyze results
    aResult.SetResult( KErrNone, _L("Test case passed") );

    // Only start time in timestamp array
    if ( timestamps.Count() == 1 )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. No timer expired.") );
        }

    // Check that the timers are expired at maximum delay due there are only
    // one timer, so, no alignment can be happened.
    if ( !AreTimestampsAtWindow( timestamps, KTickInterval, KTickInterval, 0, 0 ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Timer not in maximum window.") );
        }

    // Clean up
    timer->Cancel();
    timestamps.Close();
    CleanupStack::PopAndDestroy( timer );
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start one CFlexPeriodic, cancel it and restart it in callback
// function.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::CallbackRestartL( 
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds KTickInterval( 1000000 ); // 1 sec
    const TTimeIntervalMicroSeconds KTick2ndInterval( 2000000 ); // 2 sec
    const TUint KTestRunTime( 10000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    RArray<TTime> firstTimestamps;
    RArray<TTime> secondTimestamps;

    TRestartInfo info;
    info.iTimer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    info.iFirstTicksLeft = 1;
    info.iFirstTimestamps = &firstTimestamps;
    info.iSecondTimestamps = &secondTimestamps;
    info.iSecondInterval = KTick2ndInterval;

    // Create and start the flexible periodic timer
    CleanupStack::PushL( info.iTimer );
    info.iTimer->Start( 
        KTickInterval, 
        KTickInterval, 
        TCallBack( RestartTimer, &info ) );

    // Initialize timer expiration time array with starting time to 
    // ease analysing of data.
    TTime startTime;
    startTime.UniversalTime();
    firstTimestamps.Append( startTime );
    
    // The execution will be pending here while active scheduler is running...

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Analyze results
    aResult.SetResult( KErrNone, _L("Test case passed") );

    // Check the 1st timer expiration time
    if ( !AreTimestampsAtWindow( firstTimestamps, KTickInterval, KTickInterval, 0, 0 ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Pre-reset timer not in maximum window.") );
        }
    
    // Check the rest of timers
    else if ( !AreTimestampsAtWindow( secondTimestamps, KTick2ndInterval, KTick2ndInterval, 0, 0 ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Post-reset timer not in maximum window.") );
        }
    
    // Clean up
    info.iTimer->Cancel();
    firstTimestamps.Close();
    secondTimestamps.Close();
    CleanupStack::PopAndDestroy( info.iTimer );
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure window sizes - 32 bit
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWindow32L( 
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    return ConfigureWindowL<TTimeIntervalMicroSeconds32,TTimeIntervalMicroSeconds32>( aResult );
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure window sizes - 64 bit
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWindow64L(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    return ConfigureWindowL<TTimeIntervalMicroSeconds,TTimeIntervalMicroSeconds>( aResult );
    }

// ---------------------------------------------------------------------------
// OK TEST CASE: Configure window sizes - mixed 32 bit and 64 bit
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWindowMixL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    return ConfigureWindowL<TTimeIntervalMicroSeconds32,TTimeIntervalMicroSeconds>( aResult );
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure after timer has started 
// The test case is divided into following parts:
// 1) Remove slack from the heartbeat to make it sure that timer's default
//    windows does not cause the drifting of the timer.
// 2) Start timer, try to configure it, verify that configure has not been
//    changed (no alignment should be made by heartbeat)
// 3) Cancel the timer and start it with a callback that tries to configure
//    it while in callback, check the results
// 4) Verify that the configuration stil works by happened timer alignment
//    by the heartbeat
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureAfterStartL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */ )
    {
    __UHEAP_MARK;

    // Constants
    
    // Heartbeat timer
    // Use 1us to adjust the engine's timer. 0 returns immediately.
    const TTimeIntervalMicroSeconds32 KHeartbeatDelay( 1 );
    const TTimeIntervalMicroSeconds32 KHeartbeatInterval( 1000000 );
    const TTimeIntervalMicroSeconds32 KHeartbeatDelayWindow( 0 );
    const TTimeIntervalMicroSeconds32 KHeartbeatIntervalWindow( 0 );

    // Timer under test
    const TTimeIntervalMicroSeconds32 KTimerDelayNow( 0 );
    const TTimeIntervalMicroSeconds32 KTimerDelay( 2000000 );
    const TTimeIntervalMicroSeconds32 KTimerInterval( 2000000 );
    const TTimeIntervalMicroSeconds32 KTimerInitialDelayWindow( 0 );
    const TTimeIntervalMicroSeconds32 KTimerInitialIntervalWindow( 0 );

    const TTimeIntervalMicroSeconds32 KTimerDelayWindow32( 1500000 );
    const TTimeIntervalMicroSeconds32 KTimerIntervalWindow32( 1500000 );

    const TTimeIntervalMicroSeconds KTimerDelayWindow64( 1500000 );
    const TTimeIntervalMicroSeconds KTimerIntervalWindow64( 1500000 );
    
    const TUint KConfigNokTestRunTime( 7000000 );
    const TUint KTestCbConfigRunTime( KHeartbeatInterval.Int() );
    const TUint KConfigOkTestRunTime( 4000000 );
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Create, configure and initialize and start the heartbeat
    // This timer is used for checking that the timer under test is reacting
    // correctly to configurations.
    RArray<TTime> heartbeatTimes;
    CFlexPeriodic* heartbeat = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( heartbeat );
    heartbeat->Configure( KHeartbeatDelayWindow, KHeartbeatIntervalWindow );
    heartbeat->Start( 
        KHeartbeatDelay, 
        KHeartbeatInterval, 
        TCallBack( AddTimestamp, &heartbeatTimes ) );

    // Remove the slack from timer start up -- wait till next second
    const TInt64 KSecondInMicroSeconds( 1000000 );
    TTime now;
    now.UniversalTime();
    heartbeatTimes.Append( now );
    TUint slack( I64LOW( KSecondInMicroSeconds - now.Int64() % KSecondInMicroSeconds ) );

                     //    //  ___     _____
    WaitL( slack ); // // // //_ // //  //
                   //_//_// //  // //  //

    // Create and start the flexible periodic timer
    RArray<TTime> timestamps;
    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );
    timer->Configure( KTimerInitialDelayWindow, KTimerInitialIntervalWindow );
    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( AddTimestamp, &timestamps ) );

    aResult.SetResult( KErrNone, _L("Test case passed") );

    if ( timer->Configure( KTimerDelayWindow32, KTimerIntervalWindow32 ) != KErrInUse )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. 32 bit configure didn't return KErrInUse") );
        }
    else if ( timer->Configure( KTimerDelayWindow64, KTimerIntervalWindow64 ) != KErrInUse )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. 64 bit configure didn't return KErrInUse") );
        }

    // Initialize timer expiration time array with starting time to 
    // ease analysing of data.
    now.UniversalTime();
    timestamps.Append( now );
                                     //    //  ___     _____
    WaitL( KConfigNokTestRunTime ); // // // //_ // //  //
                                   //_//_// //  // //  //

    // Check the 1st timer expiration time
    RDebug::Print( _L("Timer:") );
    if ( !AreTimestampsAtWindow( 
            timestamps, 
            KTimerDelay,
            KTimerInterval,
            KTimerInitialDelayWindow,
            KTimerInitialIntervalWindow ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Configuration changed after start. ") );
        }

    // Restart the timer to try configuration while callback
    timer->Cancel();

    TConfigureInfo configInfo;
    configInfo.iTimer = timer;
    configInfo.iDelayWindow = KTimerDelayWindow32.Int();
    configInfo.iIntervalWindow = KTimerIntervalWindow32.Int();
    configInfo.iConfigResult32 = 0xDEADBEEF; // Initialize result
    configInfo.iConfigResult64 = 0xDEADBEEF; // Initialize result

    timer->Start(
        KTimerDelayNow, 
        KTimerInterval, 
        TCallBack( ConfigureTimer, &configInfo ) );

                                    //    //  ___     _____
    WaitL( KTestCbConfigRunTime ); // // // //_ // //  //
                                  //_//_// //  // //  //

    timer->Cancel();

    RDebug::Print( _L("configInfo.iConfigResult32=%d (0x%x)"), configInfo.iConfigResult32, configInfo.iConfigResult32 );
    RDebug::Print( _L("configInfo.iConfigResult64=%d (0x%x)"), configInfo.iConfigResult64, configInfo.iConfigResult64 );

    // Analyze the results
    if ( configInfo.iConfigResult32 != KErrInUse )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. 32 bit configure in callback didn't return KErrInUse") );
        }
    else if ( configInfo.iConfigResult64 != KErrInUse )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. 64 bit configure in callback didn't return KErrInUse") );
        }

    // Test that the configuration still works
    RArray<TTime> secondTimestamps;
    now.UniversalTime();
    secondTimestamps.Append( now );

    if ( timer->Configure( KTimerDelayWindow32, KTimerIntervalWindow32 ) != KErrNone )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Configure failed.") );
        }
    else
        {
        timer->Start(
            KTimerDelay,
            KTimerInterval,
            TCallBack( AddTimestamp, &secondTimestamps ));

                                        //    //  ___     _____
        WaitL( KConfigOkTestRunTime ); // // // //_ // //  //
                                      //_//_// //  // //  //

        // Check timestamps, they should be the same as heartbeat
        RDebug::Print( _L("secondTimestamps:") );
        if ( !AreTimestampsAtWindow( 
                secondTimestamps, 
                KHeartbeatInterval, // Heartbeat was running already 
                KHeartbeatInterval,
                KHeartbeatInterval, // There can be adjustement with 1st expiration 
                0 ) )
            {
            aResult.SetResult( 
                KErrGeneral, 
                _L("Test case failed. Configure does not work.") );
            }
        }

    RDebug::Print( _L("Heartbeat:") );
    if ( !AreTimestampsAtWindow( 
            heartbeatTimes, 
            KHeartbeatDelay,
            KHeartbeatInterval,
            KHeartbeatDelayWindow,
            KHeartbeatIntervalWindow ) )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Heartbeat failure.") );
        }

    // Clean up
    timestamps.Close();
    secondTimestamps.Close();
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    heartbeatTimes.Close();
    heartbeat->Cancel();
    CleanupStack::PopAndDestroy( heartbeat );

    __UHEAP_MARKEND;

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// TEST CASE: Start a running timer again.
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartAfterStartL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds32 KTimerDelay( 2000000 );
    const TTimeIntervalMicroSeconds32 KTimerInterval( 2000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );

    // This start should work...
    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( DoNothing, NULL ) );

    // ... and next start should panic with
    // Panic category: "E32USER-CBase"
    // Panic reason:   42 (attempt to active CActive when a request is still 
    //                     outstanding)
    
    // Set panic code 42 to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        42 );

    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( DoNothing, NULL ) );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::ENormal,
        KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    // Clean up
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    __UHEAP_MARKEND;    

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start a running timer again in callback function
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartInCallbackL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    // Constants
    const TTimeIntervalMicroSeconds32 KTimerDelay( 1000000 );
    const TTimeIntervalMicroSeconds32 KTimerInterval( 1000000 );
    const TTimeIntervalMicroSeconds32 KTestRunTime( 3000000 );
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    CFlexPeriodic* timer = CFlexPeriodic::NewL( CActive::EPriorityStandard );
    CleanupStack::PushL( timer );

    // The callback should panic with
    // Panic category: "E32USER-CBase"
    // Panic reason:   42 (attempt to active CActive when a request is still 
    //                     outstanding)
    
    // Set panic code 42 to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( 
        CTestModuleIf::EPanic,
        42 );

    timer->Start( 
        KTimerDelay, 
        KTimerInterval, 
        TCallBack( StartTimer, timer ) );

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );

    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );

    // Clean up
    timer->Cancel();
    CleanupStack::PopAndDestroy( timer );

    __UHEAP_MARKEND;    

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with negative delay, 32 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNegativeDelay32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );

    RArray<TTime> timestamps;

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   6 (EFlexPeriodicDelayLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 6 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();
    
    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with zero interval, 32 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithZeroInterval32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( 0 );

    RArray<TTime> timestamps;

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   7 (EFlexPeriodicIntervalTooSmall)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 7 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with negative interval, 32 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNegativeInterval32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );

    RArray<TTime> timestamps;

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   7 (EFlexPeriodicIntervalTooSmall)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 7 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with negative delay, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNegativeDelay64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );

    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   6 (EFlexPeriodicDelayLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 6 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with zero interval, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithZeroInterval64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( 0 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   7 (EFlexPeriodicIntervalTooSmall)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 7 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with negative interval, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNegativeInterval64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   7 (EFlexPeriodicIntervalTooSmall)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 7 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure timer with negative delay window, 32 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWithNegativeDelayWindow32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   8 (EFlexPeriodicDelayWindowLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 8 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KAllowedValue,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure timer with negative interval window, 32 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWithNegativeIntervalWindow32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   9 (EFlexPeriodicIntervalWindowLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 9 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KAllowedValue,
        KAllowedValue,
        KAllowedValue,
        KIllegalValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure timer with negative delay window, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWithNegativeDelayWindow64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   8 (EFlexPeriodicDelayWindowLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 8 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KAllowedValue,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Configure timer with negative interval window, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ConfigureWithNegativeIntervalWindow64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt KIllegalValue( -1 );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   9 (EFlexPeriodicIntervalWindowLessThanZero)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 9 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KAllowedValue,
        KAllowedValue,
        KAllowedValue,
        KIllegalValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start with minimum and maximum values
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithMinAndMaxL(
    TTestResult& aResult, 
    CTestFlexTimer* /* aCallback */  )
    {
    __UHEAP_MARK;

    const TInt KMinimumDelayValue( 0 );
    const TInt KMinimumIntervalValue( 1 );
    const TInt KNormalValue( 1000000 );
    const TInt KMaximum32BitValue( 0x7FFFFFFF );

//    const TInt64 KLarge64BitValue( 0x6FFFFFFFFFFFFFFF );  // Should not panic (till few years)

    // Years * Days * Hours * Minutes * Seconds * Microseconds
    const TInt64 KLarge64BitValue( ((TInt64)(1)) * 365 * 24 * 60 * 60 * 1000000 );
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    RArray<TTime> timestamps;

    TTime now;
    now.UniversalTime();

    // > DEBUG
    TTimeIntervalMicroSeconds longLongInterval( KLarge64BitValue );
    now += longLongInterval;
    TDateTime date;
    date = now.DateTime();
    // < DEBUG

    // Start with min delay, 32 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KMinimumDelayValue,
        KNormalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds32( KMinimumDelayValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 32 bit min delay failed") );
        }
    timestamps.Reset();

    // Start with max delay, 32 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KMaximum32BitValue,
        KNormalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    // Only one timestamp (the start time) is allowed
    if ( timestamps.Count() > 1 )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 32 bit max delay failed") );
        }
    timestamps.Reset();

    // Start with min interval, 32 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KNormalValue,
        KMinimumIntervalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KMinimumIntervalValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 32 bit min interval failed") );
        }
    timestamps.Reset();

    // Start with max interval, 32 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds32, TTimeIntervalMicroSeconds32>(
        timestamps,
        KNormalValue,
        KMaximum32BitValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KMaximum32BitValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ),
            TTimeIntervalMicroSeconds32( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 32 bit min interval failed") );
        }
    timestamps.Reset();

    // Start with min delay, 64 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KMinimumDelayValue,
        KNormalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds( KMinimumDelayValue ),
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 64 bit min delay failed") );
        }
    timestamps.Reset();

    // Start with large delay, 64 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KLarge64BitValue,
        KNormalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    // Only one timestamp (the start time) is allowed
    if ( timestamps.Count() > 1 )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 64 bit max delay failed") );
        }
    timestamps.Reset();

    // Start with min interval, 64 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KNormalValue,
        KMinimumIntervalValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KMinimumIntervalValue ),
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 64 bit min interval failed") );
        }
    timestamps.Reset();

    // Start with large interval, 64 bit
    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KNormalValue,
        KLarge64BitValue,
        KNormalValue,
        KNormalValue );
    // Validate results
    // AreTimestampsAtWindow() does not accept values over 0xFFFFFFFF.
    // However, this is not a problem due there should be only the timestamps
    // of start time and delay expiration; 32 bit value can be used instead of
    // the real interval.
    if ( !AreTimestampsAtWindow(
            timestamps,
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KMaximum32BitValue ),
            TTimeIntervalMicroSeconds( KNormalValue ),
            TTimeIntervalMicroSeconds( KNormalValue ) ) )
        {
        aResult.SetResult( KErrGeneral, _L("Test case failed. 64 bit max interval failed") );
        }
    timestamps.Reset();

    // If test execution is here, we'll passed
    aResult.SetResult( KErrNone, _L("Test case passed") );

    // Clean up
    timestamps.Close();
    __UHEAP_MARKEND;

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// TEST CASE: Start timer with maximum delay, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithMaximumDelay64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt64 KIllegalValue( 0x7FFFFFFFFFFFFFFF );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   24 (EFlexTimerServerIllegalTimerValue)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 24 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with maximum interval, 64 bit interface
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithMaximumInterval64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback  )
    {
    __UHEAP_MARK;

    const TInt KAllowedValue( 1 );
    const TInt64 KIllegalValue( 0x7FFFFFFFFFFFFFFF );
    
    RArray<TTime> timestamps;
    
    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );

    // ConfigureAndStartTimerL should panic:
    //  Category: "CFlexPeriodic" 
    //  Reason:   24 (EFlexTimerServerIllegalTimerValue)
    // Set the panic code to acceptable exit reason
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 24 );

    ConfigureAndStartTimerL<TTimeIntervalMicroSeconds, TTimeIntervalMicroSeconds>(
        timestamps,
        KAllowedValue,
        KIllegalValue,
        KAllowedValue,
        KAllowedValue );

    // No panic, change result back to normal
    aCallback->TestModuleIf().SetExitReason( CTestModuleIf::ENormal, KErrNone );
    aResult.SetResult( KErrGeneral, _L("Test case failed. No panic.") );
    
    // Clean up
    timestamps.Close();

    __UHEAP_MARKEND;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with NULL callback (32 bit)
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNullCallBack32L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    return StartWithNullCallBackL<TTimeIntervalMicroSeconds32>( aResult, aCallback );
    }

// ---------------------------------------------------------------------------
// TEST CASE: Start timer with NULL callback (64 bit)
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::StartWithNullCallBack64L(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    return StartWithNullCallBackL<TTimeIntervalMicroSeconds>( aResult, aCallback );
    }

// ---------------------------------------------------------------------------
// TEST CASE: Client crashes
// 1) Start two timers in separate threads
// 2) The 1st timer will panic after a while
// 3) Check that the 2nd timer is ok -> server works OK
// ---------------------------------------------------------------------------
//
TInt CTestFlexPeriodic::ClientCrashL(
    TTestResult& aResult, 
    CTestFlexTimer* aCallback )
    {
    __UHEAP_MARK;

    // Must be bigger than ClientCrashL and StartOneTimerL
    const TTimeIntervalMicroSeconds32 KTestRunTime( 6000000 );

    // Default result if anything leaves i.e. no analyze done.
    aResult.SetResult( KErrGeneral, _L("Test case leaved") );
    
    // Start test case ClientCrashL
    RThread threadA;
    TTestResult resultA;
    TTestCaseArguments caseA = { DoPanicL, resultA, aCallback }; 
    RunInThread( threadA, caseA );

    // Start test case StartOneTimerL
    RThread threadB;
    TTestResult resultB;
    TTestCaseArguments caseB = { StartOneTimerL, resultB, aCallback }; 
    RunInThread( threadB, caseB );

                            //    //  ___     _____
    WaitL( KTestRunTime ); // // // //_ // //  //
                          //_//_// //  // //  //

    // Analyze results
    aResult.SetResult(KErrNone, _L("Test case passed.") );

    // ClientCrashL should be paniced with reason 0xDEAD
    if ( threadA.ExitType() != EExitPanic || threadA.ExitReason() != 0xDEAD )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Client did not panic (correctly).") );
        }
    // StartOneTimerL should be finished ok
    else if ( resultB.iResult != KErrNone || threadB.ExitType() == EExitPanic )
        {
        aResult.SetResult( 
            KErrGeneral, 
            _L("Test case failed. Timer failed.") );
        }

    threadA.Close();
    threadB.Close();
    __UHEAP_MARKEND;

    return KErrNone;
    }
