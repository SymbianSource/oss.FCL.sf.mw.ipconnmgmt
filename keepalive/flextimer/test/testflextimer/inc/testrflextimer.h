/*
* ============================================================================
*  Name        : testrflextimer.h
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
* Template version: 4.2
*/

#ifndef TESTRFLEXTIMER_H
#define TESTRFLEXTIMER_H


#include <e32base.h>

/**
 *  STIF test cases for RFlexTimer class.
 *  This class is only a container for STIF test cases - it should NOT be
 *  instantiated, just call the static functions.
 *
 *  @lib testflextimer.lib
 *  @since TB10.1
 */
class CTestRFlexTimer : public CBase
    {

public:

    /**
    * Destructor.
    */
    virtual ~CTestRFlexTimer();

    /**
    * @brief Start a timer using After (32 bit) and wait it to expire
    * 
    * Open RFlexTimer handle and connect to the server. Set timer to expire
    * after 3 secs. Close the handle and check that the expiration is done
    * at correct time.
    * 
    * Tests:
    * - RFlexTimer::RFlexTimer()
    * - RFlexTimer::Connect()
    * - RFlexTimer::After() 32 bit
    * - RFlexTimer::Close()
    * - RFlexTimer::~RFlexTimer()
    * - Expiration of timer
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ExpireAfter32L( TTestResult& aResult,
                                CTestFlexTimer* aCallback );


    /**
    * @brief Start a timer using After (64 bit) and wait it to expire
    * 
    * Open RFlexTimer handle and connect to the server. Set timer to expire
    * after 3 secs. Close the handle and check that the expiration is done
    * at correct time.
    * 
    * Tests:
    * - RFlexTimer::RFlexTimer()
    * - RFlexTimer::Connect()
    * - RFlexTimer::After() 64 bit
    * - RFlexTimer::Close()
    * - RFlexTimer::~RFlexTimer()
    * - Expiration of timer
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ExpireAfter64L( TTestResult& aResult,
                                CTestFlexTimer* aCallback );


    /**
    * @brief Start a timer using AfterTicks and wait it to expire
    *
    * Open RFlexTimer handle and connect to the server. Set timer to expire
    * after 192 ticks (3 secs). Close the handle and check that the 
    * expiration is done at correct time.
    * 
    * Tests:
    * - RFlexTimer::RFlexTimer()
    * - RFlexTimer::Connect()
    * - RFlexTimer::AfterTicks()
    * - RFlexTimer::Close()
    * - RFlexTimer::~RFlexTimer()
    * - Expiration of timer
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ExpireAfterTicksL( TTestResult& aResult,
                                   CTestFlexTimer* aCallback );

    /**
    * @brief Start a timer using At and wait it to expire
    *
    * Open RFlexTimer handle and connect to the server. Set timer to expire
    * after 3 secs from now. Close the handle and check that the expiration is
    * done at correct time.
    * 
    * Tests:
    * - RFlexTimer::RFlexTimer()
    * - RFlexTimer::Connect()
    * - RFlexTimer::At()
    * - RFlexTimer::Close()
    * - RFlexTimer::~RFlexTimer()
    * - Expiration of timer
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ExpireAtL( TTestResult& aResult,
                           CTestFlexTimer* aCallback );

    /**
    * @brief Start a timer using AtUTC and wait it to expire
    *
    * Open RFlexTimer handle and connect to the server. Set timer to expire
    * after 3 secs from now. Close the handle and check that the expiration is
    * done at correct time.
    * 
    * Tests:
    * - RFlexTimer::RFlexTimer()
    * - RFlexTimer::Connect()
    * - RFlexTimer::AtUTC()
    * - RFlexTimer::Close()
    * - RFlexTimer::~RFlexTimer()
    * - Expiration of timer
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ExpireAtUtcL( TTestResult& aResult,
                              CTestFlexTimer* aCallback );

    /**
    * @brief Start timer using After (32 bit) without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt After32WithoutConnect( TTestResult& aResult,
                                       CTestFlexTimer* aCallback );

    /**
    * @brief Start timer using After (64 bit) without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt After64WithoutConnect( TTestResult& aResult,
                                       CTestFlexTimer* aCallback );

    /**
    * @brief Start timer using AfterTicks without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt AfterTicksWithoutConnect( TTestResult& aResult,
                                          CTestFlexTimer* aCallback );

    /**
    * @brief Start timer using At without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt AtWithoutConnect( TTestResult& aResult,
                                  CTestFlexTimer* aCallback );

    /**
    * @brief Start timer using AtUtc without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt AtUtcWithoutConnect( TTestResult& aResult,
                                     CTestFlexTimer* aCallback );

    /**
    * @brief Configure (32 bit) timer without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt Configure32WithoutConnect( TTestResult& aResult,
                                           CTestFlexTimer* aCallback );

    /**
    * @brief Configure (64 bit) timer without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt Configure64WithoutConnect( TTestResult& aResult,
                                           CTestFlexTimer* aCallback );

    /**
    * @brief Cancel timer using RFlexTimer without connecting to the server first.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * KERN-EXEC 0.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CancelWithoutConnect( TTestResult& aResult,
                                      CTestFlexTimer* aCallback );

    /**
    * @brief Set timer twice by After.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * RFlexTimer 15 (EFlexTimerServerErrorPendingTimer).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallAfterTwiceL( TTestResult& aResult,
                                 CTestFlexTimer* aCallback );
    

    /**
    * @brief Set timer twice by AfterTicks.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * RFlexTimer 15 (EFlexTimerServerErrorPendingTimer).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallAfterTicksTwiceL( TTestResult& aResult,
                                      CTestFlexTimer* aCallback );
    
    /**
    * @brief Set timer twice by At.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * RFlexTimer 15 (EFlexTimerServerErrorPendingTimer).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallAtTwiceL( TTestResult& aResult,
                              CTestFlexTimer* aCallback );

    /**
    * @brief Set timer twice by AtUTC.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * RFlexTimer 15 (EFlexTimerServerErrorPendingTimer).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallAtUtcTwiceL( TTestResult& aResult,
                                 CTestFlexTimer* aCallback );

    /**
    * @brief Connect to timer server twice.
    *
    * This tests illeagal usage of the interface and it'll panic with 
    * RFlexTimer 32 (EFlexTimerAlreadyConnected).
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CallConnectTwiceL( TTestResult& aResult,
                                   CTestFlexTimer* aCallback );

    
    /**
    * @brief Reconnect to timer server.
    *
    * Connect, close and connect again the handle.
    * 
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ReconnectL( TTestResult& aResult,
                            CTestFlexTimer* aCallback );

    
    /**
    * @brief Cancel timer using RFlexTimer without starting it first.
    *
    * This tests a leagal uage of the interface. Timer can be cancelled 
    * without setting it first.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt CancelWithoutStart( TTestResult& aResult,
                                    CTestFlexTimer* aCallback );
    
    /**
    * @brief Call AfterTicks() with negative value.
    *
    * This test illegal usage of the interface. There has to be zero or more 
    * ticks.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt NegativeTicksInAfterTicksL(
        TTestResult& aResult, 
        CTestFlexTimer* aCallback );

    /**
    * @brief Call AfterTicks() with zero.
    *
    * This tests leagal usage of the interface.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ZeroTicksInAfterTicksL(
        TTestResult& aResult, 
        CTestFlexTimer* aCallback );

    /**
    * @brief Change system time during AfterTicks timer is running.
    * 
    * Set timer to be expired after 640 ticks (10 sec). After 1 sec from
    * starting the timer, add system time 3 seconds. Check that the timer
    * expires after 10 seconds from starting (13 secs by system time).
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt ChangeTimeWhileAfterTicksL(
        TTestResult& aResult, 
        CTestFlexTimer* aCallback );

    /**
    * @brief Test that timer window works correctly.
    * 
    * Create three timers:
    * T1: expire after 4-8 secs
    * T2: expire after 7-8 secs
    * T3: expire after 5 secs
    *
    * T1 and T2 are AfterTicks -timers T3 is After timer.
    * 
    * Check that T1 has expired after 5 secs and T2 after 8 secs of setting.
    *
    * @param aResult The result of the test.
    * @param aCallback Pointer to the STIF test class
    * @return The status of execution of the test. KErrNone if the test
    *         was executed ok, any system wide errorcode otherwise.
    */
    static TInt TestAfterTicksWindowL(
        TTestResult& aResult, 
        CTestFlexTimer* aCallback );

private:

    /**
    * Constructor.
    * Keep it private to prevent instantiation of the class.
    */
    CTestRFlexTimer();

    /**
    * Check is the delay in the window.
    * @param aDelay Interval to be checked.
    * @param aInterval Interval of the timer.
    * @param aWindow Allowed window of the timer. 
    * @return ETrue if the delay is correc, EFalse otherwise.
    */
    static TBool IsDelayOk( 
        const TTimeIntervalMicroSeconds aDelay,
        const TTimeIntervalMicroSeconds32 aInterval,
        const TTimeIntervalMicroSeconds32 aWindow );

    /**
    * Check is the delay in the window.
    * @param aDelay Interval to be checked.
    * @param aInterval Interval of the timer.
    * @param aWindow Allowed window of the timer. 
    * @return ETrue if the delay is correc, EFalse otherwise.
    */
    static TBool IsDelayOk( 
        const TTimeIntervalMicroSeconds aDelay,
        const TTimeIntervalMicroSeconds32 aInterval,
        const TTimeIntervalMicroSeconds aWindow );

    /**
    * Check is the delay in the window.
    * @param aDelay Interval to be checked.
    * @param aInterval Interval of the timer.
    * @param aWindow Allowed window of the timer. 
    * @return ETrue if the delay is correc, EFalse otherwise.
    */
    static TBool IsDelayOk( 
        const TTimeIntervalMicroSeconds aDelay,
        const TTimeIntervalMicroSeconds aInterval,
        const TTimeIntervalMicroSeconds32 aWindow );

    /**
    * Check is the delay in the window.
    * @param aDelay Interval to be checked.
    * @param aInterval Interval of the timer.
    * @param aWindow Allowed window of the timer. 
    * @return ETrue if the delay is correc, EFalse otherwise.
    */
    static TBool IsDelayOk( 
        const TTimeIntervalMicroSeconds aDelay,
        const TTimeIntervalMicroSeconds aInterval,
        const TTimeIntervalMicroSeconds aWindow );
    
    /**
    * A helper function for AfterTicks() testing.
    * -# Creates RFlexTimer handle, 
    * -# connects it to the server, 
    * -# configures timer's window (default 0 microseconds),
    * -# set the timer to expire after given ticks,
    * -# wait timer to be expired
    * -# return the delay of the timer
    * 
    * @param aTicks Timer's interval in number of ticks
    * @param aWindowSize Timer's window (default 0 microseconds)
    * @return The delay of the timer in microseconds
    */
    static TTimeIntervalMicroSeconds ExecuteAfterTicksL(
        TInt aTicks,
        TTimeIntervalMicroSeconds aWindowSize = 0 );
    
private: // data

    };

#endif // TESTRFLEXTIMER_H
