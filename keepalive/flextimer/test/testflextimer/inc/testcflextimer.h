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

#ifndef TESTCFLEXTIMER_H
#define TESTCFLEXTIMER_H

#include <e32base.h>
#include "testflextimer.h"
#include "inheritedcflextimer2.h"
// FORWARD DECLARATIONS
class CTestFlexTimer;

class TTimerStruct
    {
public:
    CInheritedCFlexTimer2 *iTimer;
    TExtendedTestResult *iEResult;
    TInt iTimersRunning; //For quick check that how many timers are currently running
    TInt iCurrentlyActive; // Stupid as hell, but won't do another container
    // This is only valid in first item and tells which item
    // e.g CB belongs to.
    CPeriodic *iStartTimer; // Same but this is start timer pointer.
    TTimerStruct() :
        iTimer(0), iEResult(0), iTimersRunning(0), iCurrentlyActive(0),
                iStartTimer(0)
        {
        }
    ;
    };

/**
 *  STIF test cases for CFlexTimer class.
 *  This class implements test cases for testing CFlexTimer class.
 *
 *  @since TB10.1
 */
NONSHARABLE_CLASS( CTestCFlexTimer ) : public CBase
    {

public:

    /**
     * Destructor.
     */
    virtual ~CTestCFlexTimer();

    // Creating and firing timers - normal cases

    /**
     * @brief Create and destroy a timer - After
     * 
     * Test that timer can be instantiated and deleted.<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Create, start and delete timer.
     * 
     * @param aResult Result of the test for the FW.
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndDestroyCFlexTimerL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Create and expire a timer - After
     * 
     * Test that timer expires at it's max time <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - See that timeout is happening at latest possible moment.
     * 
     * @param aResult Result of the test for the FW.
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndExpireCFlexTimerL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Create two timers and expire those - overlapping - After 32bit
     * 
     * Test that two overlapping timers are fired at latest possible moment<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use overlapping time windows. (default window).<BR>
     * - See that both timers expire when first timer expires.<BR>
     * - Use 32bit After()
     * 
     * @param aResult Result of the test for the FW.
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndExpireTwoOverlappingAfterTimersL(
            TTestResult& aResult, CTestFlexTimer *cb );

    /**
     * @brief Create two timers and expire those - overlapping - After 64bit
     * 
     * Test that two overlapping timers are fired at latest possible moment<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use overlapping time windows. (default window).<BR>
     * - See that both timers expire when first timer expires.<BR>
     * - use 64bit After().
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndExpireTwoOverlappingAfterTimers64L(
            TTestResult& aResult, CTestFlexTimer *cb );

    /**
     * @brief Create two timers and expire those - not overlapping - After
     * 
     * Test that two timer that are not overlapping are fired at their own max time.<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use non-overlapping time windows. (default window).<BR>
     * - See that both timers expire at latest possible moment.<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndExpireTwoNonOverlappingAfterTimersL(
            TTestResult& aResult, CTestFlexTimer *cb );

    /**
     * @brief Create 2000 timers and expire those - After
     * 
     * Test that many timers are started and expired at rapid sequence<BR><BR>
     * - Use class that is inherited from CFlexTimer.
     * - Start and expire at 2000 timers. Some of having overlapping
     * windows (default window)with each other and others not having.<BR> 
     * - See that all timers expire within their window. Start timers so that at least 1000 timers.
     * - are running simultaneously. <BR>
     * - No need to check that they expire at latest possible moment
     * - that is checked in algorithm tests.
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt HumongousAmountOfTimeridelidilidousL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Create and expire three timers - After/At/AtUTC
     * 
     * Test that At, After and AtUTC are handled equally<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start timers and wait for them to expire.<BR>
     * - Start timers so that timers started with After and At
     * overlap and timer started with AtUTC does not overlap.<BR>
     * - See that After and At expire at the same time and that
     * - AT UTC expires at it's max time
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CreateAndExpireThreeTimersL( TTestResult& aResult,
            CTestFlexTimer *cb );

    // Cancelling timers - normal cases

    /**
     * @brief Cancel a timer
     * 
     * See that normal cancelling works<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start timers With At, AtUTC and After.<BR>
     * - Cancel a running timers and see that those do not expire anymore.<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt NormalCancelL( TTestResult& aResult, CTestFlexTimer *cb );

    /**
     * @brief Cancel a timer that would have triggered another timer - After
     * 
     * Test that canceling timer actually cancels it<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start two timers with overlapping windows.<BR>
     * - Cancel first timer before it expires and check that
     * second timer is fired at it's max time.
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt CancelTimerThatWouldHaveTriggeredAnotherTimerL(
            TTestResult& aResult, CTestFlexTimer *cb );

    //Configuring align window

    /**
     * @brief Configure time window - overlap 1
     * 
     * Test that Timers are triggered correctly after Configure<BR><BR> 
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use two timers. Set first timer having 50% window
     * and second having 10% window.<BR>
     * - Set timers so that first timers max window overlaps 1s with
     * second timers window -> first timer causes second timer to be fired 
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureCTimerOverlap1L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure time window - overlap 2
     * 
     * Test that Timers are triggered correctly after Configure<BR><BR> 
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use 64bit IF.
     * - Use two timers. Set first timer having 50% window
     * and second having 10% window.<BR>
     * - Set timers so that first timers min window overlaps 1s with
     * second timers window -> second timer causes first timer to be fired 
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureCTimerOverlap2L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure time window - overlap 3
     * 
     * Test that if many Configures are used, latest is the one that remains. Also test 0 and 100% values<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use two timers. Set first timer having 0% window
     * then overwrite that by giving 100% window to first timer.<BR>
     * - Give second timer 0% window.<BR>
     * - Start second timer so that it overlaps with first timer<BR>
     * - See that first timer is fired when second is fired. 
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureCTimerOverlap3L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure time window - non-overlap
     * 
     * Test that Timers are triggered correctly after Configure<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use two timers. Set first timer having 0% window
     * and second having 10% window.<BR>
     * - Set timers so that second timers max window is 1 seconds earlier than first timers
     * min window.<BR>
     * - See that both timers fire at max time
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureCTimerNonOverlap1L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure time window - non-overlap 2
     * 
     * Test that Timers are triggered correctly after Configure<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Use two timers. Set first timer having 100% window
     * - then overwrite that by giving first timer 50% window.<BR>
     * - Give second timer 50% window.<BR>
     * - Set timers so that second timers max window is 1 seconds earlier than first timers
     * min window.<BR>
     * - See that both timers fire at max time
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureCTimerNonOverlap2L( TTestResult& aResult,
            CTestFlexTimer *cb );

    //Time changes
    /**
     * @brief System time changes
     * 
     * Test that changing system time cancels AT and AtUTC timers and leaves After timers running<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start three timers. At/AtUTC/After<BR>
     * - Change system time e.g. User::SetHomeTime()<BR>
     * - See that At/AtUTC timers are cancelled with error code KErrAbort<BR>
     * - See that remaining timer is fired at max time<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt SystemTimeChangesL( TTestResult& aResult, CTestFlexTimer *cb );

    /**
     * @brief Timezone changes
     * 
     * Test that changing time zone cancels AT and AtUTC timers and leaves After timers running<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start three timers. At/AtUTC/After<BR>
     * - Change timezone e.g. User::SetUTCOffset()<BR>
     * - See that At/AtUTC timers are cancelled with error code KErrAbort<BR>
     * - See that remaining timer is fired at max time
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt TimeZoneChangesL( TTestResult& aResult, CTestFlexTimer *cb );

    // Misuse of API cases
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that stupid parameters do not cause any harm<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(32bit) with interval 1. See that timer expires almost right away<BR>
     * - Call After(32bit) with interval 0x7FFFFFFF. Cancel timer<BR>
     * - Call After(64bit) with interval 1. See that timer expires almost right away<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimerL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that calling After(32bit) with zero interval panics client<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(32bit) with interval 0.
     * - See that client is paniced
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimerZero32bitL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that calling After(64bit) with zero interval panics client<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(64bit) with interval 0.
     * - See that client is paniced
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimerZero64bitL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that calling After(64bit) with 0xFFFFFFFFFFFFFFFF interval panics client<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(64bit) with interval 0xFFFFFFFFFFFFFFFF.
     * - See that client is paniced
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimer64MaxL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that calling After(64bit) with three years interval panics client<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(64bit) with interval 93312000*1000*1000.
     * - See that client is paniced
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimer64ThreeYearsL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After with dumbass parameters 
     * 
     * Test that calling After(64bit) with one year interval panics client<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(64bit) with interval 30*1000*1000*1000*1000.
     * - Cancel timer
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt DumbAssParametersForCTimer64OneYearL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief call After with invalid parameters 
     * 
     * Test that negative intervals cause panic in After()<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(32bit) with negative interval See that it causes a panic.<BR>
     * TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AfterWithNegativeValueL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call After(64bit) with invalid parameters 
     * 
     * Test that negative intervals cause panic in After()<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call After(64bit) with negative interval See that it causes a panic.<BR>
     * TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AfterWithNegativeValue64L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief call At with current time 
     * 
     * Test that At() with current time panics<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call At with current time. See that client panics<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AtWithCurrentTimeL( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief call AtUTC with current time 
     * 
     * Test that AtUTC() with current time panics<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call At with current time. See that client panics<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AtUTCWithCurrentTimeL( TTestResult& aResult,
            CTestFlexTimer *cb );
    /**
     * @brief call At with time three years from now 
     * 
     * Test that At() with three years from now panics<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call At with three years from now. See that client panics<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AtWithThreeYearsL( TTestResult& aResult,
            CTestFlexTimer *cb );
    /**
     * @brief call At with time one year from now 
     * 
     * Test that At() with one year from now panics<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call At with one year from now. 
     * - Cancel timer<BR>
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AtWithOneYearL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief call At/AtUTC with invalid parameters 
     * 
     * Test that calling At() with time that is in the past causes panic<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call At with time in the past<BR>
     * See that it causes panic TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt AtWithTimeInThePastL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Cancel an expired timer - After
     * 
     * Test that calling already expired timer does not cause any harm<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start timer and wait for it to expire. After it expires
     * cancel it<BR>
     * - Start timer again and see that it expires normally.
     * 
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt
            CancelExpiredTimerL( TTestResult& aResult, CTestFlexTimer *cb );

    // Hilarious Configure calls
    /**
     * @brief Left edge of the time window lays in the past - max check
     * 
     * Test that no harm is done if left side of the window lays in the past<BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure with window_size x.<BR>
     * - Call After with interval < x.<BR>
     * - See that timer expires when max time is reached
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureLeftSideInThePastL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Left edge of the time window lays in the past - window check
     * 
     * Test that window works when left side of the window lays in the past <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure with window_size x.<BR>
     * - Call After with interval < x.<BR>
     * - Start another timer that expires before first timer.<BR>
     * - See that both timers expire when max time of the second timer is reached.
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureLeftSideInThePastWindowCheckL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure(32bit) with negative window_size
     * 
     * Test that negative window panics client <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure(32bit) with negative window_size value.<BR>
     * - See that it causes panic TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWithNegativeWindowSize32L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure(64bit) with negative window_size
     * 
     * Test that negative window panics client <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure(64bit) with negative window_size value.<BR>
     * - See that it causes panic TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWithNegativeWindowSize64L( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Configure(32bit) with 0xFFFFFFFF window_size
     * 
     * Test that 0xFFFFFFFF is valid window size <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure(32bit) with 0xFFFFFFFF window_size value.<BR>
     * - See that it causes no harm
     *  
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWithMaxWindow32L( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief Configure(64bit) with three year window_size
     * 
     * Test that negative window panics client <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure(64bit) with three year window_size value.<BR>
     * - See that it causes panic TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWithThreeYearWindow64L( TTestResult& aResult,
            CTestFlexTimer *cb );
    
    /**
     * @brief Configure(64bit) with max window_size
     * 
     * Test that negative window panics client <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Call Configure(64bit) with max window_size value.<BR>
     * - See that it causes panic TODO what panic? Shouldn't this read in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWithMaxWindow64L( TTestResult& aResult,
            CTestFlexTimer *cb );
        
    /**
     * @brief Call configure after starting timer
     * 
     * Test that configure is not used and returns error while timer is already running <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start timer.<BR>
     * - Call Configure.<BR>
     * - See that configure returns KErrInUse.<BR>
     * - See that timer expires normally.
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt ConfigureWhileTimerIsInUseL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /**
     * @brief Start timer twice
     * 
     * Test that client is paniced if timer is started twise <BR><BR>
     * - Use class that is inherited from CFlexTimer.<BR>
     * - Start timer with After()<BR>
     * - Start timer again with At()<BR>
     * - See that client panics with code EFlexTimerServerErrorPendingTimer //TODO referense in documentation
     * 
     * @param TBD
     * @return The status of execution of the test. KErrNone if the test
     *         was executed ok, any system wide errorcode otherwise.
     */
    static TInt TimerIsStartedTwiceL( TTestResult& aResult,
            CTestFlexTimer *cb );

    /*****************************************
     * CB functions used by CFlexTimer tests *
     /*****************************************/

    /**
     * @brief Check timer stop code and stop active scheduler
     * 
     * Simple function that stops the active scheduler. Checks that Error code is KErrNone
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackStopSchedulerKErrNone(
            TAny* aArgument );

    /**
     * @brief Check timer stop code and stop active scheduler
     * 
     * Simple function that stops the active scheduler. Checks that Error code is KErrCancel
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackStopSchedulerKErrCancel(
            TAny* aArgument );

    /**
     * @brief Check timer stop code and stop active scheduler
     * 
     * Simple function that stops the active scheduler. Checks that Error code is KErrAbort
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackStopSchedulerKErrAbort(
            TAny* aArgument );

    /**
     * @brief Check timer stop code
     * 
     * Simple function that checks that Error code is KErrNone
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackKErrNone( TAny* aArgument );

    /**
     * @brief Check timer stop code
     * 
     * Simple function that checks that Error code is KErrAbort
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackKErrAbort( TAny* aArgument );

    /**
     * @brief Check error code and mark timestamps
     * 
     * Function that checks that Error code is KErrNone. ans saves timestamps
     * @see CreateAndExpireTwoOverlappingAfterTimersL
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackKErrNoneMarkTimestamp(
            TAny* aArgument );

    /**
     * @brief Check error code and mark timestamps
     * 
     * Function that checks that Error code is KErrNone. ans saves timestamps
     * @see CreateAndExpireTwoOverlappingAfterTimersL
     * 
     * @param N/A
     * @return N/A
     */
    static TInt InheritedFlexTimerCallbackKErrNoneMarkTimestampStopScheduler(
            TAny* aArgument );

    /**
     * @brief Stop active scheduler
     * 
     * Function that stops the active scheduler
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StopActiveScheduler( TAny* aArgument );

    /**
     * @brief Changes system time and cancels CPeriodic given as parameter
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt ChangeSystemTime1sAndCancel( TAny* aArgument );

    /**
     * @brief Changes system time and cancels CPeriodic given as parameter
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt ChangeTimeZoneAndCancel( TAny* aArgument );

    /**
     * @brief Changes system time and cancels CPeriodic given as parameter
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt ChangeSecureTimeAndCancel( TAny* aArgument );

    /**
     * @brief cancel CFlexTimer given as parameter
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CancelCFlexTimer( TAny* aArgument );

    /**
     * @brief See that expiry reason is correct. Array versio
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StopSchedulerKErrNoneArray( TAny* aArgument );

    /**
     * @brief cancel CFlexTimer given as parameter also cancel CPeriodic
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CancelCFlexTimerCancelPeriodic( TAny* aArgument );
    /**
     * @brief cancel CFlexTimer given as parameter
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CancelCFlexTimerArray( TAny* aArgument );
    /**
     * @brief cancel CFlexTimer given as parameter and stops active scheduler
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CancelCFlexTimerAndStopActiveScheduler( TAny* aArgument );

    /**
     * @brief stops activescheduler and cancels it self
     * 
     * N/A
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StopSchedulerAndCancel( TAny* aArgument );

    /**
     * @brief Not called CB
     * 
     * Fails if gets called
     * 
     * @param N/A
     * @return N/A
     */
    static TInt NotCalledCB( TAny* aArgument );
    
    /**
     * @brief Callback function, that calls StartTimerAndAddToList
     * 
     * -
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CTestCFlexTimer::StartTimerAndAddToListCB( TAny* aArgument );
    /**
     * @brief Start random timer and add it to list given as parameter
     * 
     * -
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StartTimerAndAddToList(
        RArray<TTimerStruct>* aArgument, TInt aTimers );

    /**
     * @brief Stop timer and mark time to list given as parameter
     * 
     * -
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StopTimer( TAny* aArgument );
    /**
     * @brief Try to configure timer and see that KErrInUse is returned
     * 
     * -
     * 
     * @param N/A
     * @return N/A
     */
    static TInt ConfigureKErrInUseArray( TAny* aArgument );

    /**
     * @brief start timer again
     * 
     * -
     * 
     * @param N/A
     * @return N/A
     */
    static TInt StartTimerAgainArray( TAny* aArgument );
    
    /**
     * @brief Create two overlapping timers and start, run and expire them
     * during high CPU load.
     * 
     * Test steps:<BR>
     * 1. Create CFlexTimer_1, (inteval = 10s, window=0.2*10s (default))<BR>
     * 2. Create CFlexTimer_2, (inteval = 8s, window=0.2*8s (default))<BR>
     * 3. Start 99% CPU Load<BR>
     * 4. Start timers<BR>
     * 5. Wait timers to expire<BR>
     * 6. Stop CPU load<BR>
     * 5. Check that timers expired within ~8sec<BR>
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CTestCFlexTimer::CpuLoadCreateAndExpireTwoOverlappingAfterTimersL(
            TTestResult& aResult, CTestFlexTimer *cb );
    
    /**
     * @brief Create 100  timers and start, run and expire them during high
     *  CPU load.
     * 
     * Test steps:<BR>
     * 1. Create 100 random timers with random interval and window.<BR>
     * 3. Start 98% CPU Load<BR>
     * 4. Start timers<BR>
     * 5. Wait timers to expire<BR>
     * 6. Stop CPU load<BR>
     * 5. Check that timers expired within expected limits.<BR>
     * 
     * @param N/A
     * @return N/A
     */
    static TInt CTestCFlexTimer::CpuLoadManyTimersL(
            TTestResult& aResult, CTestFlexTimer *cb );
private:

    /**
     * Constructor is private to prevent instantiation of the class. 
     */
    CTestCFlexTimer();

    };

#endif // TESTCFLEXTIMER_H
