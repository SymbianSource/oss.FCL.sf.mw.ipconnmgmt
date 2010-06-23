/*
* ============================================================================
*  Name        : testflexperiodic.h
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
* Template version: 4.2
*/

#ifndef TESTFLEXPERIODIC_H
#define TESTFLEXPERIODIC_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CFlexPeriodic;

/**
 *  STIF test cases for CFlexPeriodic class.
 *  This class is only a container for STIF test cases - it should NOT be
 *  instantiated, just call the static functions.
 *
 *  @lib testflextimer.lib
 *  @since TB10.1
 */
NONSHARABLE_CLASS( CTestFlexPeriodic ) : public CBase
    {

/**
 * Data for restarting timer in callback function.
 */
struct TRestartInfo
    {
    CFlexPeriodic* iTimer;                      ///< Pointer to the timer
    TInt iFirstTicksLeft;                       ///< Number of times the 1st timer settings are run
    RArray<TTime>* iFirstTimestamps;            ///< Timestamps for the 1st timer settings
    RArray<TTime>* iSecondTimestamps;           ///< Timestamps for the 2nd timer settings
    TTimeIntervalMicroSeconds iSecondInterval;  ///< Interval for the 2nd timer settings
    };

/**
 * Data for CFlexPeriodic::Configure() tests
 */
struct TConfigureInfo
    {
    CFlexPeriodic* iTimer;   ///< Pointer to the timer under testing
    TInt iDelayWindow;       ///< value for delay window to be set
    TInt iIntervalWindow;    ///< value for interval window to be set
    TInt iConfigResult32;    ///< Result of configure(32 bit)
    TInt iConfigResult64;    ///< Result of configure(64 bit)
    };

/**
 * Data for running a test case in own thread
 */
struct TTestCaseArguments
    {
    TInt (*iTestFunction)( TTestResult&, CTestFlexTimer* ); ///< Function pointer to the test case to be run
    TTestResult& iResult;                                   ///< The result of the test (argument for the test case)
    CTestFlexTimer* iCallback;                              ///< Pointer to the STIF test class (argument for the test case)
    };

public:

    /**
    * Destructor.
    */
    virtual ~CTestFlexPeriodic();

    /**
    * @brief Create and expire CFlexPeriodic timer.
    * 
    * Create and start one CFlexPeriodic timer with 1 sec delay and interval 
    * wait for 5 secs. Check that the timer has been expired at correct times.
    * 
    * Timer expiration times (sec):
    * 1, 2, 3, 4, ...
    * 
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Start() 32-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartOneTimerL( TTestResult& aResult, 
                                CTestFlexTimer* aCallback );
    
    /**
    * @brief Create and expire CFlexPeriodic timer with Error CB.
    * 
    * Create and start one CFlexPeriodic timer with 1 sec delay and interval 
    * wait for 5 secs. Give timer also Error CB argument.
    * Check that the timer has been expired at correct times.
    * If callback gets called case panics.
    * 
    * Timer expiration times (sec):
    * 1, 2, 3, 4, ...
    * 
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Start() 32-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartOneTimerWithErrorCbL( TTestResult& aResult, 
                                CTestFlexTimer* aCallback );
    
    /**
    * @brief Restart CFlexPeriodic in callback function. 
    *
    * Start a periodic timer with 1 sec delay and interval.
    *
    * After 2 expiration cancel and start the timer in its callback function
    * with 2 secs delay and interval.
    * 
    * Timer expiration times (sec):
    * 1, 2, 4, 6, 8, ...
    * 
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Start() 64-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    * - Cancelling and starting timer in callback function
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallbackRestartL( TTestResult& aResult, 
                                  CTestFlexTimer* aCallback ); 

    /**
    * @brief Configure window sizes, 32 bit
    * 
    * Start 2 timers:
    * T1:
    * - delay       3.0 sec, window 0.0 sec
    * - interval    2.0 sec, window 1.5 sec
    * T2:
    * - delay       3.5 sec, window 1.5 sec
    * - interval    1.0 sec, window 0.0 sec
    *
    * Both timers shoud expire at (sec)
    * 3.0, 4.0, 5.0, 6.0, ...
    * 
    * Functionality of delay window is tested in sec 3.0 expiration and
    * functionality of interval window is tested in other expirations.
    *
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Configure() 32-bit
    * - CFlexPeriodic::Start() 32-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    * - Delay
    * - Interval
    * - Delay window
    * - Interval window  
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWindow32L( TTestResult& aResult,
                                    CTestFlexTimer* aCallback );

    /**
    * @brief Configure window sizes, 64 bit
    * 
    * @see ConfigureWindow32L for test setup.
    * 
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Configure() 64-bit
    * - CFlexPeriodic::Start() 64-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    * - Delay
    * - Interval
    * - Delay window
    * - Interval window  
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWindow64L( TTestResult& aResult,
                                    CTestFlexTimer* aCallback );

    /**
    * @brief Configure window sizes, 32 bit and 64 bit
    * 
    * @see ConfigureWindow32L for test setup.
    * 
    * Tests:
    * - CFlexPeriodic::NewL()
    * - CFlexPeriodic::Configure() 32-bit
    * - CFlexPeriodic::Configure() 64-bit
    * - CFlexPeriodic::Start() 32-bit
    * - CFlexPeriodic::Start() 64-bit
    * - CFlexPeriodic::Cancel()
    * - CFlexPeriodic::~CFlexPeriodic()
    * - Expiration of CFlexPeriodic timers
    * - Delay
    * - Interval
    * - Delay window
    * - Interval window  
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWindowMixL( TTestResult& aResult,
                                     CTestFlexTimer* aCallback );

    /**
    * @brief Configure timer after it has been started.
    * 
    * Start two timers:
    * T1:
    * - delay 1 microsecond, window 0
    * - interval 1 sec, window 0
    * 
    * T2:
    * - delay 2 sec, default window
    * - interval 2 sec, default window
    * 
    * Try to configure T2 with 1.5 sec delay and interval windows by both
    * 32 bit and 64 bit Configure(). Check that configure returns KErrInUse
    * and it has not changed the window sizes. The timer expirations should be
    * T1: 1, 2, 3, 4, 5, 6, 7, 8, ...
    * T2:    2,    4,    6,    8, ...  
    * 
    * Cancel the timer T2 and set its callback function to call Configure() 
    * (32 bit and 64 bit versions) while the timer is running. Check that both
    * configurations return KErrInUse
    *
    * Cancel timer T2 and configure its delay and interval windows to 1.5 sec.
    * Let the timer(s) run a while and check that the timer expirations are:
    * T1: ..., 10, 11, 12, 13, ...
    * T2: ..., 10, 11, 12, 13, ...
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureAfterStartL( TTestResult& aResult,
                                      CTestFlexTimer* aCallback );

    /**
    * @brief Start a running timer.
    * 
    * Test should panic with E32USER-CBase 42 (attempt to active CActive when 
    * a request is still outstanding).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartAfterStartL( TTestResult& aResult,
                                  CTestFlexTimer* aCallback );

    /**
    * @brief Start a running timer in its callback function.
    * 
    * Test should panic with E32USER-CBase 42 (attempt to active CActive when 
    * a request is still outstanding).
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartInCallbackL( TTestResult& aResult,
                                  CTestFlexTimer* aCallback );

    /**
    * Start timer with negative delay (32 bit).
    * 
    * Start should panic with CFlexPeriodic 6 (EFlexPeriodicDelayLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNegativeDelay32L( TTestResult& aResult, 
                                           CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with zero interval (32 bit).
    * 
    * Start should panic with CFlexPeriodic 7 (EFlexPeriodicIntervalTooSmall)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithZeroInterval32L( TTestResult& aResult, 
                                          CTestFlexTimer*  aCallback  );
    
    /**
    * @brief Start timer with negative interval (32 bit).
    * 
    * Start should panic with CFlexPeriodic 7 (EFlexPeriodicIntervalTooSmall)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNegativeInterval32L( TTestResult& aResult, 
                                              CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with negative delay (64 bit).
    * 
    * Start should panic with CFlexPeriodic 6 (EFlexPeriodicDelayLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNegativeDelay64L( TTestResult& aResult, 
                                          CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with zero interval (64 bit).
    * 
    * Start should panic with CFlexPeriodic 7 (EFlexPeriodicIntervalTooSmall)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithZeroInterval64L( TTestResult& aResult, 
                                          CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with negative interval (64 bit).
    * 
    * Start should panic with CFlexPeriodic 7 (EFlexPeriodicIntervalTooSmall)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNegativeInterval64L( TTestResult& aResult, 
                                              CTestFlexTimer*  aCallback  );

    /**
    * @brief Configure timer with negative delay window (32 bit).
    * 
    * Configure should panic with CFlexPeriodic 9 
    * (EFlexPeriodicIntervalWindowLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWithNegativeDelayWindow32L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    /**
    * @brief Configure timer with negative interval window (32 bit).
    * 
    * Configure should panic with CFlexPeriodic 8 
    * (EFlexPeriodicDelayWindowLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWithNegativeIntervalWindow32L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    /**
    * @brief Configure timer with negative delay window (64 bit).
    * 
    * Configure should panic with CFlexPeriodic 9 
    * (EFlexPeriodicIntervalWindowLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWithNegativeDelayWindow64L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    /**
    * @brief Configure timer with negative interval window (64 bit).
    * 
    * Configure should panic with CFlexPeriodic 8 
    * (EFlexPeriodicDelayWindowLessThanZero)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ConfigureWithNegativeIntervalWindow64L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    
    /**
    * @brief Test timer start allowed value range.
    * 
    * Start timers with following parameters:
    * 
    * T1:
    * - Configure (32 bit): delay window 1 sec, interval window 1 sec
    * - Start (32 bit): delay 0 microseconds, interval 1 sec
    * 
    * T2:
    * - Configure (32 bit): delay window 1 sec, interval window 1 sec
    * - Start (32 bit): delay 0x7FFFFF microseconds, interval 1 sec
    * 
    * T3:
    * - Configure (32 bit): delay window 1 sec, interval window 1 sec
    * - Start (32 bit): delay 1 sec, interval 0 microseconds
    * 
    * T4:
    * - Configure (32 bit): delay window 1 sec, interval window 1 sec
    * - Start (32 bit): delay 1 sec, interval 0x7FFFFF microseconds
    *
    * T5:
    * - Configure (64 bit): delay window 1 sec, interval window 1 sec
    * - Start (64 bit): delay 0 microseconds, interval 1 sec
    * 
    * T6:
    * - Configure (64 bit): delay window 1 sec, interval window 1 sec
    * - Start (64 bit): delay 1 year, interval 1 sec
    * 
    * T7:
    * - Configure (64 bit): delay window 1 sec, interval window 1 sec
    * - Start (64 bit): delay 1 sec, interval 0 microseconds
    * 
    * T8:
    * - Configure (64 bit): delay window 1 sec, interval window 1 sec
    * - Start (64 bit): delay 1 sec, interval 1 year
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithMinAndMaxL( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    
    /**
    * @brief Start timer with maximum delay (64 bit), 0x7FFFFFFFFFFFFFFF.
    * 
    * Start should panic with  CFlexPeriodic 24 
    * (EFlexTimerServerIllegalTimerValue)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithMaximumDelay64L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with maximum interval (64 bit), 0x7FFFFFFFFFFFFFFF.
    * 
    * Start should panic with  CFlexPeriodic 24 
    * (EFlexTimerServerIllegalTimerValue)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithMaximumInterval64L( 
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );
    
    /**
    * @brief Start timer with NULL callback function (32 bit)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNullCallBack32L(
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    /**
    * @brief Start timer with NULL callback function (64 bit)
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt StartWithNullCallBack64L(
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

    
    /**
    * @brief Crash client with active periodic flexible timer.
    * 
    * Start two timers in separate threads:
    * T1: delay 1 sec, interval 1 sec with default windows
    * T2: delay 1 sec, interval 1 sec with default windows
    * 
    * Panic timer T1's thread after 3 sec.
    * 
    * Cancel T2 after 5 sec and check that it had worked correctly.
    * Implementation of T2 uses test case StartOneTimerL().
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ClientCrashL(
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );

private:

    /**
    * Constructor is private to prevent instantiation of the class. 
    */
    CTestFlexPeriodic();

    /**
     * Run active scheduler for given time.
     * @param aPeriod Time to run active scheduler.
     */
    static void WaitL( TTimeIntervalMicroSeconds32 aPeriod );

    /**
     * Stop the active scheduler. A function for TCallBack.
     * @param aArgument Not used.
     * @return KErrNone. Always.
     */
    static TInt StopScheduler( TAny* aArgument );


    /**
     * Do nothing. A function for TCallBack.
     * @param aArgument Not used.
     * @return KErrNone.
     */
    static TInt DoNothing( TAny* aArgument );
    
    /**
     * Panics testcase if get called. A function for TCallBack.
     * @param aArgument Not used.
     * @return KErrNone.
     */
    static TInt PanicClient( TAny* aArgument );
    
    /**
     * Add current time to given array. A function for TCallBack.
     * @param aArgument Timestamp storage, type: RArray<TTime>*
     * @return KErrNone.
     */
    static TInt AddTimestamp( TAny* aArgument );

    /**
     * Restart the given timer. A function for TCallBack.
     * @param aArgument The restart data, type: TRestartInfo*
     * @return KErrNone.
     */
    static TInt RestartTimer( TAny* aArgument );

    /**
     * Configure timer without cancellation in callback. A function for 
     * TCallBack.
     * @param aArgument The conficure data, type: TConfigureInfo*
     * @return KErrNone.
     */
    static TInt ConfigureTimer( TAny* aArgument );

    /**
     * Start timer without cancellation in callback. A function for TCallBack.
     * @param aArgument The timer, type: CFlexPeriodic*
     * @return KErrNone.
     */
    static TInt StartTimer( TAny* aArgument );

    /**
     * Verify that the timestamps are expired at maximum window
     * @param aTimestamps An array of times
     * @param aDelay Requested delay of the 1st time
     * @param aInterval Requested delay from the 2nd time
     * @param aDelayWindow Time how much 1st time can be early
     * @param aIntervalWindow Time how much times can be early from the 2nd time 
     * @return ETrue if all timestamps are in given delay
     */
    static TBool AreTimestampsAtWindow( 
        const RArray<TTime>& aTimestamps,
        const TTimeIntervalMicroSeconds32& aDelay,
        const TTimeIntervalMicroSeconds32& aInterval,
        const TTimeIntervalMicroSeconds32& aDelayWindow,
        const TTimeIntervalMicroSeconds32& aIntervalWindow );

    /**
     * Verify that the timestamps are expired at maximum window
     * @param aTimestamps An array of times
     * @param aDelay Requested delay of the 1st time
     * @param aInterval Requested delay from the 2nd time
     * @param aDelayWindow Time how much 1st time can be early
     * @param aIntervalWindow Time how much times can be early from the 2nd time 
     * @return ETrue if all timestamps are in given delay
     */
    static TBool AreTimestampsAtWindow( 
        const RArray<TTime>& aTimestamps,
        const TTimeIntervalMicroSeconds& aDelay,
        const TTimeIntervalMicroSeconds& aInterval,
        const TTimeIntervalMicroSeconds& aDelayWindow,
        const TTimeIntervalMicroSeconds& aIntervalWindow );

    /**
     * Compare two arrays of timestamps.
     * @param aLeft Array of timestamps to be compared
     * @param aRight Array of timestamps to be compared
     * @return ETrue if the arrays has the same timestamps, EFalse otherwise
     */
    static TBool AreTimestampsSame( 
        const RArray<TTime>& aLeft, 
        const RArray<TTime>& aRight );

    /**
     * Parameter types of two timers are templated, so, 32 bit and 64 bit
     * interfaces can be tested without duplicated source code.
     * 
     * Possible values for template are:
     * - TTimeIntervalMicroSeconds
     * - TTimeIntervalMicroSeconds32
     */
    template <class firstType, class secondType> 
    static TInt ConfigureWindowL( TTestResult& aResult );

    /**
     * Template function for NOK API test cases.
     * 
     * Types for CFlexPeriodic::Start() and CFlexPeriodic::Configure() are
     * templated, so, 32 bit and 64 bin interfaces can be tested without
     * duplicated code.
     * 
     * Function creates a flexible periodic timer, configures is, starts it.
     * After one second the timer will be cancelled and destroyed.
     * 
     * Possible values for configureType and startType are:
     * - TTimeIntervalMicroSeconds
     * - TTimeIntervalMicroSeconds32
     *
     * @param aDelay Value for timer's delay in start
     * @param aInterval Value for timer's interval in start
     * @param aDelayWindow Value for timer's delay window in configure
     * @param aIntervalWindow Value for timer's interval window in configure
     */
    template <class configureType, class startType>
    static void ConfigureAndStartTimerL( 
        RArray<TTime>& aTimestamps,
        TInt64 aDelay,
        TInt64 aInterval,
        TInt64 aDelayWindow,
        TInt64 aIntervalWindow );

    /**
    * Start timer with NULL callback function
    * 
    * Possible values for startType are:
    * - TTimeIntervalMicroSeconds
    * - TTimeIntervalMicroSeconds32
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    template <class startType>
    static TInt StartWithNullCallBackL(
        TTestResult& aResult, 
        CTestFlexTimer*  aCallback  );
    
    /**
    * Run test case in own thread
    * 
    * @param aThread The thread in which the test case is run
    * @param aArguments Arguments for running a test case
    * @return The status of execution of the test
    */
    static TInt RunInThread( 
        RThread& aThread,
        TTestCaseArguments& aArguments );

    /**
    * Start running a thread. A function of format TThreadFunction.
    * Create trap harness for the thread and call RunTestCaseL()
    * 
    * @param aArgument Test case data, type: TTestCaseArguments
    * @return The status of running the thread
    */
    static TInt RunTestCase( TAny* aArgument );

    /**
    * The second phase of start running a thread.
    * Initialize scheduler and start the test case.
    * 
    * @param aArgument Test case data, type: TTestCaseArguments
    */
    static void RunTestCaseL( TAny* aArgument );

    /**
    * Panic thread after a while. 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt DoPanicL( TTestResult& aResult, CTestFlexTimer*  aCallback  );

    };

#endif // TESTFLEXPERIODIC_H
