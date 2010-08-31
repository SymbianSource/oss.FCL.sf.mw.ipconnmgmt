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

#include <e32debug.h>           // for RDebug
#include <e32math.h>             // for Math::Rand()
#include <StifTestInterference.h>
#include "testflextimer.h"      // for global constants & CB
#include "testcflextimer.h"
#include "flextimerpanic.h"     // for panic constants
#include "inheritedcflextimer.h"// CFlexTimer implementation (CB is always TExtendedTestResult*)
#include "inheritedcflextimer2.h"// another CFlexTimer implementation (CB is always RArray<TTimerStruct> *list)
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

const TInt64 KTickResolution( 1000000 / 64 );
const TInt KExtraSlack = 500 * 1000;
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CTestCFlexTimer::CTestCFlexTimer()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CTestCFlexTimer::~CTestCFlexTimer()
    {
    }
// --------------------------------------------------------------------------
// Start timer and delete it.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndDestroyCFlexTimerL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TExtendedTestResult *eResult = new TExtendedTestResult(&aResult);
    CInheritedCFlexTimer
            * timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard,
                    TCallBack(
                            InheritedFlexTimerCallbackStopSchedulerKErrNone,
                            eResult));

    TTimeIntervalMicroSeconds32 expireTime(5 * 1000 * 1000); //5s
    timer->After(expireTime);
    CActiveScheduler::Start();

    delete timer;
    timer = NULL;
    _LIT( KDescription , "Test case passed");
    aResult.SetResult(KErrNone, KDescription);
    delete eResult;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start timer and see that it expires at max time
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndExpireCFlexTimerL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TExtendedTestResult *eResult = new TExtendedTestResult(&aResult);
    CInheritedCFlexTimer
            * timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard,
                    TCallBack(
                            InheritedFlexTimerCallbackStopSchedulerKErrNone,
                            eResult));

    // Expire in 5 seconds
    TTimeIntervalMicroSeconds32 expireTime(5 * 1000 * 1000);

    // Save current time.
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer->After(expireTime);

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        delete timer;
        delete eResult;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timer expiry happened within maxtime to maxtime+accuracy
    if ((utcNow + expireTime) <= utcNow2 && (utcNow + expireTime
            + TTimeIntervalMicroSeconds32(KTimerAccuracyMicroSeconds))
            > utcNow2)
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer;
    delete eResult;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndExpireTwoOverlappingAfterTimersL(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds32 expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)));

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(15 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds32(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime2 + TTimeIntervalMicroSeconds32(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        RDebug::Printf("utcNow = %lld\n", utcNow.Int64());
        RDebug::Printf("eResult1->iTimerStoppedTime = %lld\n",
                eResult1->iTimerStoppedTime);
        RDebug::Printf("eResult2->iTimerStoppedTime = %lld\n",
                eResult2->iTimerStoppedTime);
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time. 64bit version
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndExpireTwoOverlappingAfterTimers64L(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)));

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(15 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime2 + TTimeIntervalMicroSeconds(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at their own max time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndExpireTwoNonOverlappingAfterTimersL(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    //Expire one second before first timers window starts
    TTimeIntervalMicroSeconds32 expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)) - 1 * 1000 * 1000);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(15 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within at their own max time edge.
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds32(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime1) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime1 + TTimeIntervalMicroSeconds32(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start many timers and see that nothing goes wrong when they are expired at rapid sequence
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::HumongousAmountOfTimeridelidilidousL(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    RDebug::Printf("HugemongousAmountOfTimeridelidilidousL");
    // This list will contain all the timer structs, expiry times etc.
    // First cell will always tell which timer is currently active
    // active info is transferred from e.g. InheritedCFlexTimer2::RunL to CB function
    RArray<TTimerStruct> timers;
    // This CB contains all the logic about timer addition
    StartTimerAndAddToList( &timers, 2000 );
    // CBs stop this after all timers are expired 
    CActiveScheduler::Start();
    // Delete CPeriodic that was used to launch new FlexTimers
    delete timers[0].iStartTimer;
    _LIT( KDescriptionP , "Passed");
    aResult.SetResult(KErrNone, KDescriptionP);
    // Loop throug list and see if timers fired within limits.
    // also delete all stuff reserved earlier by StartTimerAndAddToList
    while (timers.Count())
        {
        // Have fun with reading this. It is not as bad as it looks
        // Just checks if CB already failed the case
        // and if timer was launched within its window(default) and timer accuracy
        if (!(!timers[0].iEResult->iCaseAlreadyFailed
                && (timers[0].iEResult->iTimerStartedTime
                        + timers[0].iEResult->iTimerExpiryTime
                        - (static_cast<float> (timers[0].iEResult->iTimerExpiryTime)
                                * KDefaultWindowMultiplier)
                        <= timers[0].iEResult->iTimerStoppedTime+KTickResolution)
                && (timers[0].iEResult->iTimerStartedTime
                        + timers[0].iEResult->iTimerExpiryTime
                        + KTimerAccuracyMicroSeconds)
                        > timers[0].iEResult->iTimerStoppedTime))
            {
            _LIT( KDescription , "Some timer was not fired within its window.\n");
            aResult.SetResult(KErrGeneral, KDescription);

            }
        delete timers[0].iEResult->iTestResult;
        delete timers[0].iEResult;
        delete timers[0].iTimer;
        timers.Remove(0);
        }
    // Case was executed
    timers.Close();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start three timers At, AtUTC and After and see that they are treated equally.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CreateAndExpireThreeTimersL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TTestResult *res3 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    TExtendedTestResult *eResult3 = new TExtendedTestResult(res3);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    eResult2->iTimerID = 3;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    CInheritedCFlexTimer* timer3 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult3));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(9 * 1000 * 1000);
    TTimeIntervalMicroSeconds expireTime3(3 * 1000 * 1000); // Expire far enough from others

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();
    TTime homeNow(0);
    homeNow.HomeTime();

    timer1->After(expireTime1);
    timer2->At(homeNow + expireTime2);
    timer3->AtUTC(utcNow + expireTime3);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(15 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        aResult.SetResult(
            eResult1->iTestResult->iResult,
            eResult1->iTestResult->iResultDes );
        }
    else if (eResult2->iCaseAlreadyFailed)
        {
        aResult.SetResult(
            eResult2->iTestResult->iResult,
            eResult2->iTestResult->iResultDes );
        }
    else if (eResult3->iCaseAlreadyFailed)
        {
        aResult.SetResult(
            eResult3->iTestResult->iResult,
            eResult3->iTestResult->iResultDes );
        }

    // Check if timers expiry happened within limits.
    // First and second timer at seconds timers max time and third at its own max time.
    //
    // Compare results against utcNow due timer's stopped time is UTC.
    else if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution) && 
        (utcNow + expireTime2 + TTimeIntervalMicroSeconds( KTimerAccuracyMicroSeconds)) > TTime( eResult2->iTimerStoppedTime) &&

        (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && 
        (utcNow + expireTime2 + TTimeIntervalMicroSeconds( KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime) &&

        (utcNow + expireTime3) <= TTime(eResult3->iTimerStoppedTime+KTickResolution) && 
        (utcNow + expireTime3 + TTimeIntervalMicroSeconds( KTimerAccuracyMicroSeconds)) > TTime( eResult3->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete timer3;
    delete eResult1;
    delete eResult2;
    delete eResult3;
    delete res1;
    delete res2;
    delete res3;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start three timers At, AtUTC and After and see that they can be cancelled.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::NormalCancelL( TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TTestResult *res3 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    TExtendedTestResult *eResult3 = new TExtendedTestResult(res3);
    eResult1->iTimerID = 0;
    eResult2->iTimerID = 1;
    eResult3->iTimerID = 2;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult1), 0);
    CInheritedCFlexTimer2* timer2 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult2), 1);
    CInheritedCFlexTimer2* timer3 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult3), 2);
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(10 * 1000 * 1000);
    //just something
    TTimeIntervalMicroSeconds expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)));
    TTimeIntervalMicroSeconds expireTime3(7 * 1000 * 1000); // just something
    RArray<TTimerStruct> timers;
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);
    TTimerStruct *s2 = new TTimerStruct();
    s2->iEResult = eResult2;
    s2->iTimer = timer2;
    timers.Append(*s2);
    TTimerStruct *s3 = new TTimerStruct();
    s3->iEResult = eResult3;
    s3->iTimer = timer3;
    timers.Append(*s3);
    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();
    TTime homeNow(0);
    homeNow.HomeTime();

    timer1->After(expireTime1);
    timer2->At(homeNow + expireTime2);
    timer3->AtUTC(utcNow + expireTime3);

    timers[0].iCurrentlyActive = 0; //CB uses this to decide which timer to cancel;

    //Timer to cancel timers under test and cancel active scheduler
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(15 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CPeriodic *pp = CPeriodic::NewL(EPriorityNormal);
    s1->iStartTimer = pp;
    pp->Start(3 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));
    CPeriodic *ppp = CPeriodic::NewL(EPriorityNormal);
    s2->iStartTimer = ppp;
    ppp->Start(4 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));
    CPeriodic *pppp = CPeriodic::NewL(EPriorityNormal);
    s3->iStartTimer = pppp;
    pppp->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    delete pp;
    delete ppp;
    delete pppp;
    delete s1;
    delete s2;
    delete s3;
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed
            || eResult3->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        if (eResult2->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult3->iTestResult->iResult,
                    eResult3->iTestResult->iResultDes);
            }
        }
    delete timer1;
    delete timer2;
    delete timer3;
    delete eResult1;
    delete eResult2;
    delete eResult3;
    delete res1;
    delete res2;
    delete res3;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelTimerThatWouldHaveTriggeredAnotherTimerL(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult1));
    CInheritedCFlexTimer
            * timer2 =
                    CInheritedCFlexTimer::NewL(
                            CActive::EPriorityStandard,
                            TCallBack(
                                    InheritedFlexTimerCallbackKErrNoneMarkTimestampStopScheduler,
                                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds32 expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)));

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to cancel Timer 1
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(3 * 1000 * 1000, 1000, TCallBack(CancelCFlexTimer, timer1));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds32(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureCTimerOverlap1L( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 window1(8 * 1000 * 1000);
    timer1->Configure(window1);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds32 expireTime2(9 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 window2(1 * 1000 * 1000);
    timer2->Configure(window2);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(17 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds32(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime2 + TTimeIntervalMicroSeconds32(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureCTimerOverlap2L( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds window1(8 * 1000 * 1000);
    timer1->Configure(window1);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(17 * 1000 * 1000);
    TTimeIntervalMicroSeconds window2(1 * 1000 * 1000);
    timer2->Configure(window2);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(18 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer1 maxtime to maxtime+accuracy
    if ((utcNow + expireTime1) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime1 + TTimeIntervalMicroSeconds(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime1) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime1 + TTimeIntervalMicroSeconds(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureCTimerOverlap3L( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds window1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 zeroWindow(0);
    timer1->Configure(zeroWindow);
    timer1->Configure(window1);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(8 * 1000 * 1000);
    TTimeIntervalMicroSeconds window2(0);
    timer2->Configure(window2);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(18 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime2 + TTimeIntervalMicroSeconds(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at their own max time
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureCTimerNonOverlap1L( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds window1(0);
    timer1->Configure(window1);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(15 * 1000 * 1000);
    TTimeIntervalMicroSeconds window2(1 * 1000 * 1000);
    timer2->Configure(window2);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(18 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timer expired within their own  maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime1) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime1 + TTimeIntervalMicroSeconds(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that they expire at their own max time
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureCTimerNonOverlap2L( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(16 * 1000 * 1000);
    TTimeIntervalMicroSeconds window1(16 * 1000 * 1000);
    timer1->Configure(window1);
    window1 = 8 * 1000 * 1000;
    timer1->Configure(window1);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds expireTime2(7 * 1000 * 1000);
    TTimeIntervalMicroSeconds window2(4 * 1000 * 1000);
    timer2->Configure(window2);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(18 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timer expired within their own  maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime1) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime1 + TTimeIntervalMicroSeconds(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start three timers and see that system time change aborts At & AtUtc timers
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::SystemTimeChangesL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TTestResult *res3 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    TExtendedTestResult *eResult3 = new TExtendedTestResult(res3);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    eResult3->iTimerID = 3;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(
                    InheritedFlexTimerCallbackKErrAbort, eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(
                    InheritedFlexTimerCallbackKErrAbort, eResult2));
    CInheritedCFlexTimer* timer3 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackStopSchedulerKErrNone,
                    eResult3));
    // Expire in 10, 11 and 12 seconds. One and two are to be aborted
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 expireTime2(11 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 expireTime3(12 * 1000 * 1000);

    
    // Save current time as ticks. This use case will change system time
    // and timestamps will not be exact -- ticks are.
    TUint startTicks( User::TickCount() );
    
    TTime now, nowUtc;
    now.HomeTime();
    nowUtc.UniversalTime();

    timer1->At(now + expireTime1);
    timer2->AtUTC(nowUtc + expireTime2);
    timer3->After(expireTime3);

    //Timer to change system time after timers have been running for a while
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(5 * 1000 * 1000, 1000, TCallBack(ChangeSystemTime1sAndCancel, p));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed
            || eResult3->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else if (eResult2->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult3->iTestResult->iResult,
                    eResult3->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete timer3;
        delete eResult1;
        delete eResult2;
        delete eResult3;
        delete res1;
        delete res2;
        delete res3;
        return KErrNone;
        }
    // Get new current time
    TUint endTicks( User::TickCount() );
    TInt delay( ( endTicks - startTicks ) * KTickInMicroSeconds );
    
    //Check if timers expiry happened within timer3 maxtime to maxtime+accuracy
    if ( delay+KTickResolution >= expireTime3.Int() && 
         delay < expireTime3.Int() + KTimerAccuracyMicroSeconds )
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete timer3;
    delete eResult1;
    delete eResult2;
    delete eResult3;
    delete res1;
    delete res2;
    delete res3;

    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start three timers and see that time zone change aborts At & AtUtc timers
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::TimeZoneChangesL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TTestResult *res3 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    TExtendedTestResult *eResult3 = new TExtendedTestResult(res3);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    eResult3->iTimerID = 3;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(
                    InheritedFlexTimerCallbackKErrAbort, eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(
                    InheritedFlexTimerCallbackKErrAbort, eResult2));
    CInheritedCFlexTimer* timer3 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackStopSchedulerKErrNone,
                    eResult3));
    // Expire in 10, 11 and 12 seconds. One and two are to be aborted
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 expireTime2(11 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 expireTime3(12 * 1000 * 1000);

    // Save current time as ticks. This use case will change system time
    // and timestamps will not be exact -- ticks are.
    TUint startTicks( User::TickCount() );
    
    TTime now, nowUtc;
    now.HomeTime();
    nowUtc.UniversalTime();
    
    timer1->At(now + expireTime1);
    timer2->AtUTC(nowUtc + expireTime2);
    timer3->After(expireTime3);

    //Timer to change system time after timers have been running for a while
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(5 * 1000 * 1000, 1000, TCallBack(ChangeTimeZoneAndCancel, p));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed
            || eResult3->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else if (eResult2->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult3->iTestResult->iResult,
                    eResult3->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete timer3;
        delete eResult1;
        delete eResult2;
        delete eResult3;
        delete res1;
        delete res2;
        delete res3;
        return KErrNone;
        }

    // Get new current time
    TUint endTicks( User::TickCount() );
    TInt delay( ( endTicks - startTicks ) * KTickInMicroSeconds );
    
    //Check if timers expiry happened within timer3 maxtime to maxtime+accuracy
    if ( delay+KTickResolution >= expireTime3.Int() && 
         delay < expireTime3.Int() + KTimerAccuracyMicroSeconds )
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete timer3;
    delete eResult1;
    delete eResult2;
    delete eResult3;
    delete res1;
    delete res2;
    delete res3;

    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimerL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;
    CInheritedCFlexTimer
            * timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard,
                    TCallBack(
                            InheritedFlexTimerCallbackStopSchedulerKErrNone,
                            eResult));

    /*32 bit IF called with 1 parameter*/
    /***********************************/
    // Expire in 3 seconds. 
    TTimeIntervalMicroSeconds32 expireTime(1);
    // Save current time
    TTime now(0);
    now.HomeTime();
    timer->After(expireTime);
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult->iTestResult->iResult,
                eResult->iTestResult->iResultDes);
        delete timer;
        delete eResult;
        delete res;
        return KErrNone;
        }
    // Get new current time
    TTime now2(0);
    now2.HomeTime();
    //Check if timers expiry happened immediately.
    if (!((now + expireTime) <= now2 && (now + expireTime
            + TTimeIntervalMicroSeconds32(KTimerAccuracyMicroSeconds)) > now2))
        {
        _LIT( KDescription , "32 bit After(1) failed.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        delete timer;
        delete eResult;
        delete res;
        return KErrNone;
        }

    /*32 bit IF called with max parameter*/
    /***********************************/
    expireTime = 0x7FFFFFFF;
    now.HomeTime();
    delete timer;
    timer = NULL;
    timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard, TCallBack(
            NotCalledCB, eResult));
    timer->After(expireTime);
    //Timer to change system time after timers have been running for a while
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(5 * 1000 * 1000, 1000, TCallBack(
            CancelCFlexTimerAndStopActiveScheduler, timer));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    p->Cancel();
    delete p;
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult->iTestResult->iResult,
                eResult->iTestResult->iResultDes);
        delete timer;
        delete eResult;
        delete res;
        return KErrNone;
        }

    /*64 bit IF called with 1 parameter*/
    /***********************************/
    // Expire in 3 seconds. 
    TTimeIntervalMicroSeconds expireTime64(1);
    // Save current time
    now.HomeTime();
    delete timer;
    timer = NULL;
    timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard, TCallBack(
            InheritedFlexTimerCallbackStopSchedulerKErrNone, eResult));
    timer->After(expireTime64);
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult->iTestResult->iResult,
                eResult->iTestResult->iResultDes);
        delete timer;
        delete eResult;
        delete res;
        return KErrNone;
        }
    // Get new current time
    now2.HomeTime();
    //Check if timers expiry happened immediately
    if (!((now + expireTime64) <= now2 && (now + expireTime64
            + TTimeIntervalMicroSeconds(KTimerAccuracyMicroSeconds)) > now2))
        {
        _LIT( KDescription , "64 bit After(1) failed.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        delete timer;
        delete eResult;
        delete res;
        return KErrNone;
        }
    // Finally if we are here set whole case to passed.
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimerZero32bitL( TTestResult& aResult,
        CTestFlexTimer* /* cb */ )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    /*32 bit IF called with 0 parameter*/
    /***********************************/
    const TTimeIntervalMicroSeconds32 kExpireTime32(0);

    // Save current time
    TTime startTime;
    startTime.HomeTime();

    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
        CActive::EPriorityStandard, 
        TCallBack( InheritedFlexTimerCallbackStopSchedulerKErrNone, eResult ) );

    timer->After( kExpireTime32 );

    CActiveScheduler::Start();  // WAIT timer to expire

    TTime endTime;
    endTime.HomeTime();
    
    TTimeIntervalMicroSeconds delay = endTime.MicroSecondsFrom( startTime );
    
    if ( delay.Int64() < kExpireTime32.Int() || 
         delay.Int64() > kExpireTime32.Int() + KTimerAccuracyMicroSeconds )
        {
        aResult.SetResult( KErrGeneral, _L("Wrong expiration") );
        }
    else
        {
        aResult.SetResult( KErrNone, _L("Test case passed") );
        }
    
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimerZero64bitL( TTestResult& aResult,
        CTestFlexTimer* /* cb */ )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    /*64 bit IF called with 0 parameter*/
    /***********************************/
    const TTimeIntervalMicroSeconds kExpireTime64(0);

    // Save current time
    TTime startTime;
    startTime.HomeTime();

    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(InheritedFlexTimerCallbackStopSchedulerKErrNone, eResult));

    timer->After(kExpireTime64);
    CActiveScheduler::Start();  // WAIT timer to expire

    TTime endTime;
    endTime.HomeTime();
    
    TTimeIntervalMicroSeconds delay = endTime.MicroSecondsFrom( startTime );
    
    if ( delay.Int64()+KTickResolution < kExpireTime64.Int64() || 
         delay.Int64() > kExpireTime64.Int64() + KTimerAccuracyMicroSeconds )
        {
        aResult.SetResult( KErrGeneral, _L("Wrong expiration") );
        }
    else
        {
        aResult.SetResult( KErrNone, _L("Test case passed") );
        }
    
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimer64MaxL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    /*64 bit IF called with 0x7FFFFFFFFFFFFFFF parameter*/
    /***********************************/
    // This will cause panic 
    TTimeIntervalMicroSeconds expireTime64(0x7FFFFFFFFFFFFFFF);
    // Save current time
    TTime now(0);
    now.HomeTime();
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerServerIllegalTimerValue);
    timer->After(expireTime64);
    // Start timer that stops active scheduler just in case.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(3 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CActiveScheduler::Start();
    delete p;
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "64 bit After(0x7FFFFFFFFFFFFFFF) failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimer64ThreeYearsL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    /*64 bit IF called with 93312000000000 (about three years) parameter*/
    /***********************************/
    // This will cause panic since only two years ahead timers are supported 
    TTimeIntervalMicroSeconds expireTime64(93312000000000);
    // Save current time
    TTime now(0);
    now.HomeTime();
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerServerIllegalTimerValue);
    timer->After(expireTime64);
    // Start timer that stops active scheduler just in case.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(3 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CActiveScheduler::Start();
    delete p;
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "64 bit After(93312000000000) failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start one year timer and cancel it.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::DumbAssParametersForCTimer64OneYearL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    RDebug::Printf("DumbAssParametersForCTimer64OneYearL");
    TTestResult *res1 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    eResult1->iTimerID = 0;
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1( 33312000000000 ); // About on year
    //just something
    RArray<TTimerStruct> timers;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB,
                    &timers), 0);
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);

    timer1->After(expireTime1);

    timers[0].iCurrentlyActive = 0;

    //Timers to cancel timer and stop scheduler.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(7 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CPeriodic *pp = CPeriodic::NewL(EPriorityNormal);
    s1->iStartTimer = pp;
    pp->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p; delete pp;
    delete s1;
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult1->iTestResult->iResult,
                eResult1->iTestResult->iResultDes);
        }
    
    delete timer1;
    delete eResult1;
    delete res1;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with negative value
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AfterWithNegativeValueL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    // This will cause panic
    TTimeIntervalMicroSeconds32 expireTime(-1);
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerAfterIntervalLessThanZero);
    timer->After(expireTime);
    CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "32 bit After(-1) failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with negative value
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AfterWithNegativeValue64L( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    // This will cause panic
    TTimeIntervalMicroSeconds expireTime(-1);
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerAfterIntervalLessThanZero);
    timer->After(expireTime);
    CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "64 bit After(-1) failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }


// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AtWithCurrentTimeL( TTestResult& aResult,
        CTestFlexTimer* /* cb */ )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    TTimeIntervalMicroSeconds expireTime64(0);
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
        CActive::EPriorityStandard, 
        TCallBack(InheritedFlexTimerCallbackStopSchedulerKErrNone, eResult));

    // Save current time
    TTime now(0);
    now.HomeTime();
    timer->At(now + expireTime64);
    CActiveScheduler::Start(); // WAIT timer to expire

    // Analyze results
    TTime endTime;
    endTime.HomeTime();
    
    TTimeIntervalMicroSeconds delay = endTime.MicroSecondsFrom( now );

    if ( delay.Int64()+KTickResolution < expireTime64.Int64() || 
         delay.Int64() > expireTime64.Int64() + KTimerAccuracyMicroSeconds )
        {
        aResult.SetResult(KErrGeneral, _L("Wrong expiration"));
        }
    else
        {
        aResult.SetResult( KErrNone, _L("Test case passed") );
        }

    // Clean up
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AtUTCWithCurrentTimeL( TTestResult& aResult,
        CTestFlexTimer* /* cb */ )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    TTimeIntervalMicroSeconds expireTime64(0);
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
        CActive::EPriorityStandard, 
        TCallBack(InheritedFlexTimerCallbackStopSchedulerKErrNone, eResult));

    // Save current time
    TTime now(0);
    now.UniversalTime();
    timer->AtUTC(now + expireTime64);
    CActiveScheduler::Start(); // WAIT timer to expire

    // Analyze results
    TTime endTime;
    endTime.UniversalTime();
    
    TTimeIntervalMicroSeconds delay = endTime.MicroSecondsFrom( now );

    if ( delay.Int64()+KTickResolution < expireTime64.Int64() || 
         delay.Int64() > expireTime64.Int64() + KTimerAccuracyMicroSeconds )
        {
        aResult.SetResult(KErrGeneral, _L("Wrong expiration"));
        }
    else
        {
        aResult.SetResult( KErrNone, _L("Test case passed") );
        }

    // Clean up
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timers with dumbass parameters
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AtWithThreeYearsL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = 1;

    // This will cause panic 
    TTimeIntervalMicroSeconds expireTime64(93312000000000);
    CInheritedCFlexTimer *timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerServerIllegalTimerValue);
    // Save current time
    TTime now(0);
    now.HomeTime();
    timer->At(now + expireTime64);
    // Start timer that stops active scheduler just in case.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(3 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CActiveScheduler::Start();
    delete p;
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "At(93312000000000) failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start one year timer and cancel it.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AtWithOneYearL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    RDebug::Printf("AtWithOneYearL");
    TTestResult *res1 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    eResult1->iTimerID = 0;
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1( 33312000000000 ); // About on year
    //just something
    RArray<TTimerStruct> timers;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB,
                    &timers), 0);
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);
    TTime now(0);
    now.HomeTime();
    timer1->At(now + expireTime1);

    timers[0].iCurrentlyActive = 0;

    //Timers to cancel timer and stop scheduler.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(7 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CPeriodic *pp = CPeriodic::NewL(EPriorityNormal);
    s1->iStartTimer = pp;
    pp->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p; delete pp;
    delete s1;
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult1->iTestResult->iResult,
                eResult1->iTestResult->iResultDes);
        }
    delete timer1;
    delete eResult1;
    delete res1;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Call At and AtUTC with time in the past
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::AtWithTimeInThePastL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    RDebug::Printf("AtWithTimeInThePastL");
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);

    // Expire in the past
    TTimeIntervalMicroSeconds expireTime(-1);
    CInheritedCFlexTimer* timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    // Save current time
    TTime homeNow(0);
    homeNow.HomeTime();
    TTime homeExpireTime(homeNow + expireTime);
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerAtIntervalLessThanZero);
    timer->At(homeExpireTime);
    CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "At in the past failed to panic.\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timer, expire, cancel, start again and expire
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelExpiredTimerL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TExtendedTestResult *eResult = new TExtendedTestResult(&aResult);
    CInheritedCFlexTimer
            * timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard,
                    TCallBack(
                            InheritedFlexTimerCallbackStopSchedulerKErrNone,
                            eResult));

    // Expire in 5 seconds
    TTimeIntervalMicroSeconds32 expireTime(5 * 1000 * 1000);

    // Save current time.
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer->After(expireTime);

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    // Ignore time and cb stuff
    // Cancel timer that just expired
    timer->Cancel();

    // Start timer again
    // Save current time.
    utcNow.UniversalTime();

    timer->After(expireTime);

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        delete timer;
        delete eResult;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timer expiry happened within maxtime to maxtime+accuracy
    if ((utcNow + expireTime) <= utcNow2+TTimeIntervalMicroSeconds32(KTickResolution) && (utcNow + expireTime
            + TTimeIntervalMicroSeconds32(KTimerAccuracyMicroSeconds))
            > utcNow2)
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer;
    delete eResult;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start timer that has left side of the window in the past.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureLeftSideInThePastL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    TExtendedTestResult *eResult = new TExtendedTestResult(&aResult);
    CInheritedCFlexTimer
            * timer = CInheritedCFlexTimer::NewL(CActive::EPriorityStandard,
                    TCallBack(
                            InheritedFlexTimerCallbackStopSchedulerKErrNone,
                            eResult));

    // Expire in 5 seconds
    TTimeIntervalMicroSeconds32 winTime(10 * 1000 * 1000);
    TTimeIntervalMicroSeconds32 expireTime(5 * 1000 * 1000);

    // Save current time.
    TTime utcNow(0);
    utcNow.UniversalTime();
    timer->Configure(winTime);
    timer->After(expireTime);

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    //See if CB has failed the case
    if (eResult->iCaseAlreadyFailed)
        {
        delete timer;
        delete eResult;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timer expiry happened within maxtime to maxtime+accuracy
    if ((utcNow + expireTime) <= utcNow2+TTimeIntervalMicroSeconds32(KTickResolution) && (utcNow + expireTime
            + TTimeIntervalMicroSeconds32(KTimerAccuracyMicroSeconds))
            > utcNow2)
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer;
    delete eResult;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Start two timers and see that window works if another is started with left 
// side of the window in the past.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureLeftSideInThePastWindowCheckL(
        TTestResult& aResult, CTestFlexTimer */*cb*/)
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds32 expireTime2(9 * 1000 * 1000);

    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();

    timer1->Configure(TTimeIntervalMicroSeconds(15 * 1000 * 1000));
    timer1->After(expireTime1);
    timer2->After(expireTime2);

    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(17 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed || eResult2->iCaseAlreadyFailed)
        {
        if (eResult1->iCaseAlreadyFailed)
            {
            aResult.SetResult(eResult1->iTestResult->iResult,
                    eResult1->iTestResult->iResultDes);
            }
        else
            {
            aResult.SetResult(eResult2->iTestResult->iResult,
                    eResult2->iTestResult->iResultDes);
            }
        delete timer1;
        delete timer2;
        delete eResult1;
        delete eResult2;
        delete res1;
        delete res2;
        return KErrNone;
        }
    // Get new current time
    TTime utcNow2(0);
    utcNow2.UniversalTime();

    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if ((utcNow + expireTime2) <= TTime(eResult2->iTimerStoppedTime+KTickResolution)
            && (utcNow + expireTime2 + TTimeIntervalMicroSeconds32(
                    KTimerAccuracyMicroSeconds)) > TTime(
                    eResult2->iTimerStoppedTime) &&

    (utcNow + expireTime2) <= TTime(eResult1->iTimerStoppedTime+KTickResolution) && (utcNow
            + expireTime2 + TTimeIntervalMicroSeconds32(
            KTimerAccuracyMicroSeconds)) > TTime(eResult1->iTimerStoppedTime))
        {
        _LIT( KDescription , "Passed");
        aResult.SetResult(KErrNone, KDescription);
        }
    else
        {
        _LIT( KDescription , "Some timer was fired either too early or too late.\n");
        aResult.SetResult(KErrGeneral, KDescription);
        }
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    return KErrNone;

    }

// --------------------------------------------------------------------------
// Call Configure 32 bit with Negative window size
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWithNegativeWindowSize32L(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    RDebug::Printf("ConfigureWithNegativeWindowSize32L");
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);

    // negative time
    TTimeIntervalMicroSeconds32 confTime(-1);
    CInheritedCFlexTimer* timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerWindowLessThanZero);
    timer->Configure(confTime);
    //CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "Configure(32) with negative interval failed to panic\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Call Configure 64 bit with Negative window size
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWithNegativeWindowSize64L(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    RDebug::Printf("ConfigureWithNegativeWindowSize64L");
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);

    // negative time
    TTimeIntervalMicroSeconds confTime(-1);
    CInheritedCFlexTimer* timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerWindowLessThanZero);
    timer->Configure(confTime);
    //CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "Configure(64) with negative interval failed to panic\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// -
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWithMaxWindow32L(
        TTestResult& aResult, CTestFlexTimer* /* cb */ )
    {
    RDebug::Printf("ConfigureWithMaxWindow32L");
    TTestResult *res1 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    eResult1->iTimerID = 0;
    TTimeIntervalMicroSeconds32 expireTime1( 0x7FFFFFFF );
    //just something
    RArray<TTimerStruct> timers;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB,
                    &timers), 0);
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);
    timer1->Configure(expireTime1);
    timer1->After(expireTime1);

    timers[0].iCurrentlyActive = 0;

    //Timers to cancel timer and stop scheduler.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(7 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StopActiveScheduler));
    CPeriodic *pp = CPeriodic::NewL(EPriorityNormal);
    s1->iStartTimer = pp;
    pp->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            CancelCFlexTimerArray, &timers));

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p; delete pp;
    delete s1;
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult1->iTestResult->iResult,
                eResult1->iTestResult->iResultDes);
        }
    delete timer1;
    delete eResult1;
    delete res1;
    return KErrNone;
    }
// --------------------------------------------------------------------------
// -
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWithThreeYearWindow64L(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    RDebug::Printf("ConfigureWithThreeYearWindow64L");
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);

    // negative time
    TTimeIntervalMicroSeconds confTime(93312000000000);
    CInheritedCFlexTimer* timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerServerIllegalTimerValue);
    timer->Configure(confTime);
    //CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "Configure(93312000000000 failed to panic\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// -
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWithMaxWindow64L(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    RDebug::Printf("ConfigureWithMaxWindow64L");
    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);

    // negative time
    TTimeIntervalMicroSeconds confTime(0x7FFFFFFFFFFFFFFF);
    CInheritedCFlexTimer* timer = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult));
    //Accept panic as passed case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic,
            EFlexTimerServerIllegalTimerValue);
    timer->Configure(confTime);
    //CActiveScheduler::Start();
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    _LIT( KDescription , "Configure(0x7FFFFFFFFFFFFFFF) failed to panic\n");
    aResult.SetResult(KErrGeneral, KDescription);
    delete timer;
    delete eResult;
    delete res;
    return KErrNone;
    }
// --------------------------------------------------------------------------
// Start timer and call configure while it is running
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureWhileTimerIsInUseL( TTestResult& aResult,
        CTestFlexTimer */*cb*/)
    {
    RDebug::Printf("ConfigureWhileTimerIsInUseL");
    TTestResult *res1 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    eResult1->iTimerID = 0;
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(10 * 1000 * 1000);
    //just something
    RArray<TTimerStruct> timers;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(StopSchedulerKErrNoneArray,
                    &timers), 0);
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);

    TTime homeNow(0);
    homeNow.HomeTime();

    timer1->After(expireTime1);

    timers[0].iCurrentlyActive = 0;

    //Timer to call configure while timer is running
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            ConfigureKErrInUseArray, &timers));

    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    delete p;
    delete s1;
    _LIT( KDescription , "Passed");
    aResult.SetResult(KErrNone, KDescription);
    //See if CB has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        aResult.SetResult(eResult1->iTestResult->iResult,
                eResult1->iTestResult->iResultDes);
        delete timer1;
        delete eResult1;
        delete res1;
        return KErrNone;
        }
    TTime homeNow2(0);
    homeNow2.HomeTime();
    //Check if timers expiry happened within timer2 maxtime to maxtime+accuracy
    if (!((homeNow + expireTime1) <= homeNow2+TTimeIntervalMicroSeconds32(KTickResolution) && (homeNow + expireTime1
            + TTimeIntervalMicroSeconds32(KTimerAccuracyMicroSeconds))
            > homeNow2))
        {
        _LIT( KEDescription , "Timer was not fired within limits");
        aResult.SetResult(KErrGeneral, KEDescription);
        }
    delete timer1;
    delete eResult1;
    delete res1;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// timer is started twice
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::TimerIsStartedTwiceL( TTestResult& aResult,
        CTestFlexTimer *cb )
    {
    TTestResult *res1 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    eResult1->iTimerID = 0;
    CInheritedCFlexTimer2* timer1 = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard, TCallBack(NotCalledCB, eResult1), 0);
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds expireTime1(10 * 1000 * 1000);
    RArray<TTimerStruct> timers;
    TTimerStruct *s1 = new TTimerStruct();
    s1->iEResult = eResult1;
    s1->iTimer = timer1;
    timers.Append(*s1);

    timer1->After(expireTime1);
    timers[0].iCurrentlyActive = 0; //CB uses this to decide which timer to start again;

    //Timer to start timer again
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
    p->Start(5 * 1000 * 1000, 30 * 1000 * 1000, TCallBack(
            StartTimerAgainArray, &timers));
    // Panic category: "E32USER-CBase"
    // Panic reason:   42 (attempt to active CActive when a request is still 
    // outstanding.
    cb->TestModuleIf().SetExitReason(CTestModuleIf::EPanic, 42);
    // Start scheduler and wait for callback to panic it
    CActiveScheduler::Start();
    // We should never get here -> fail the case
    cb->TestModuleIf().SetExitReason(CTestModuleIf::ENormal, KErrNone);
    delete p;
    delete s1;
    _LIT( KDescription , "double start failed to panic.");
    aResult.SetResult(KErrGeneral, KDescription);

    delete timer1;
    delete eResult1;
    delete res1;
    return KErrNone;
    }

// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason and stop scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackStopSchedulerKErrNone(
        TAny* aArgument )
    {
    RDebug::Printf("InheritedFlexTimerCallbackStopSchedulerKErrNone");
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrNone)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason\n");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason and stop scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackStopSchedulerKErrAbort(
        TAny* aArgument )
    {
    RDebug::Printf("InheritedFlexTimerCallbackStopSchedulerKErrAbort");
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrAbort)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason and stop scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackStopSchedulerKErrCancel(
        TAny* aArgument )
    {
    RDebug::Printf("InheritedFlexTimerCallbackStopSchedulerKErrCancel");
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrCancel)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackKErrNone( TAny* aArgument )
    {
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrNone)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackKErrAbort( TAny* aArgument )
    {
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrAbort)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that both timers are stoppped with valid reason and save expiry time
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackKErrNoneMarkTimestamp(
        TAny* aArgument )
    {
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrNone)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    TTime utcNow(0);
    utcNow.UniversalTime();
    eRes->iTimerStoppedTime = utcNow.Int64();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// See that both timers are stoppped with valid reason and save expiry time
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::InheritedFlexTimerCallbackKErrNoneMarkTimestampStopScheduler(
        TAny* aArgument )
    {
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    if (eRes->iStatusCode != KErrNone)
        {
        eRes->iCaseAlreadyFailed = ETrue;
        _LIT(KError,"Error: Wrong timer expiry reason");
        eRes->iTestResult->SetResult(KErrGeneral, KError);
        }
    TTime utcNow(0);
    utcNow.UniversalTime();
    eRes->iTimerStoppedTime = utcNow.Int64();
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Stop active scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StopActiveScheduler( TAny*/*aArgument*/)
    {
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Change system time + 1s and cancel Periodic timer given as parameter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ChangeSystemTime1sAndCancel( TAny* aArgument )
    {
    RDebug::Printf("ChangeSystemTime1sAndCancel");
    TTime now(0);
    now.HomeTime();
    User::SetHomeTime(now + TTimeIntervalMicroSeconds32(1000 * 1000));
    CPeriodic *p = reinterpret_cast<CPeriodic*> (aArgument);
    p->Cancel();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Change time zone and cancel Periodic timer given as parameter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ChangeTimeZoneAndCancel( TAny* aArgument )
    {
    RDebug::Printf("ChangeTimeZoneAndCancel");
    TTimeIntervalSeconds currentUtcOffset( User::UTCOffset() );
    User::SetUTCOffset( currentUtcOffset.Int() + 3000 );
    CPeriodic *p = reinterpret_cast<CPeriodic*> (aArgument);
    p->Cancel();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Change secure time + 1s and cancel Periodic timer given as parameter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ChangeSecureTimeAndCancel( TAny* aArgument )
    {
    RDebug::Printf("ChangeSecureTimeAndCancel");
    TTime now(0);
    now.UniversalTime();
    User::SetUTCTimeSecure(now + TTimeIntervalMicroSeconds32(1000 * 1000));
    CPeriodic *p = reinterpret_cast<CPeriodic*> (aArgument);
    p->Cancel();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Fail result if this is called
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::NotCalledCB( TAny* aArgument )
    {
    TExtendedTestResult *eRes =
            reinterpret_cast<TExtendedTestResult*> (aArgument);
    eRes->iCaseAlreadyFailed = ETrue;
    _LIT(KError,"Error: invalid CB called");
    eRes->iTestResult->SetResult(KErrGeneral, KError);
    return 1; //nada
    }

// --------------------------------------------------------------------------
// Stop active schduler and cancel Periodic timer given as paramter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StopSchedulerAndCancel( TAny* aArgument )
    {
    CPeriodic *p = reinterpret_cast<CPeriodic*> (aArgument);
    p->Cancel();
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Cancels CFlexTimer given a parameter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelCFlexTimer( TAny* aArgument )
    {
    CFlexTimer *p = reinterpret_cast<CFlexTimer*> (aArgument);
    p->Cancel();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Cancels CFlexTimer
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelCFlexTimerArray( TAny* aArgument )
    {
    RDebug::Printf("CancelCFlexTimerCancelPeriodic");
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    (*list)[(*list)[0].iCurrentlyActive].iTimer->Cancel();
    (*list)[0].iCurrentlyActive += 1;
    return 1; // nada
    }
// --------------------------------------------------------------------------
// Cancels CFlexTimer and CPeriodic given a parameter
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelCFlexTimerCancelPeriodic( TAny* aArgument )
    {
    RDebug::Printf("CancelCFlexTimerCancelPeriodic");
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    (*list)[(*list)[0].iCurrentlyActive].iTimer->Cancel();
    (*list)[0].iCurrentlyActive += 1;
    (*list)[0].iStartTimer->Cancel();
    return 1; // nada
    }
// --------------------------------------------------------------------------
// See that timer is stoppped with valid reason and stop scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StopSchedulerKErrNoneArray( TAny* aArgument )
    {
    RDebug::Printf("StopSchedulerKErrNoneArray");
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    if ((*list)[(*list)[0].iCurrentlyActive].iEResult->iStatusCode
            != KErrNone)
        {
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iCaseAlreadyFailed
                = ETrue;
        _LIT(KErrorDes,"Error: Wrong timer expiry reason");
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iTestResult->SetResult(
                KErrGeneral, KErrorDes);
        }
    CActiveScheduler::Stop();
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Cancels CFlexTimer given a parameter and stops active scheduler
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CancelCFlexTimerAndStopActiveScheduler( TAny* aArgument )
    {
    CFlexTimer *p = reinterpret_cast<CFlexTimer*> (aArgument);
    p->Cancel();
    CActiveScheduler::Stop();
    return 1; // nada
    }
    
// --------------------------------------------------------------------------
// Callback for StartTimerAndAddToList method. This is hack to enable aTimers
// parameter in StartTimerAndAddToList method.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StartTimerAndAddToListCB( TAny* aArgument )
    {
    return StartTimerAndAddToList(
        reinterpret_cast<RArray<TTimerStruct>*> (aArgument), -1 );
    }

// --------------------------------------------------------------------------
// Starts timer and adds it to list given as param
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StartTimerAndAddToList(
    RArray<TTimerStruct>* aList, TInt aTimers )
    {
    const TInt KTimerStartInterval = 5 * 1000; //Start new timer every 5ms
    const TInt KTimerMaxDelay = 15 * 1000 * 1000; //Timer delay is between 0 and 15 seconds
    static TInt numberOfTimers = 0; // How many timers to start
    
    if (aTimers > 0)
        {
        numberOfTimers = aTimers;
        }

    TTestResult *res = new TTestResult();
    TExtendedTestResult *eResult = new TExtendedTestResult(res);
    eResult->iTimerID = aList->Count();
    CInheritedCFlexTimer2* timer = CInheritedCFlexTimer2::NewL(
            CActive::EPriorityStandard,
            TCallBack(StopTimer, reinterpret_cast<TAny*> (aList)),
            eResult->iTimerID);

    // Expire in 0-15s seconds
    TInt random = Math::Random() % (KTimerMaxDelay);
    TTimeIntervalMicroSeconds expireTime(random);
    TTimerStruct *s = new TTimerStruct();
    s->iEResult = eResult;
    s->iTimer = timer;
    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();
    s->iEResult->iTimerStartedTime = utcNow.Int64();
    s->iEResult->iTimerExpiryTime = expireTime.Int64();
    TInt listerr = aList->Append(*s);
    if (KErrNone != listerr)
        {
        RDebug::Printf("Error: Adding to list failed %d", listerr);
        }
    (*aList)[0].iTimersRunning += 1;
    timer->After(expireTime);
    //First time here. Lets start timer to make many timers.
    if (aList->Count() == 1)
        {
        CPeriodic *p = CPeriodic::NewL(EPriorityNormal);
        p->Start(KTimerStartInterval, KTimerStartInterval, TCallBack(
                StartTimerAndAddToListCB, reinterpret_cast<TAny*> (aList) ));
        (*aList)[0].iStartTimer = p;
        }
    RDebug::Printf("StartTimerAndAddToList: timers running %d",
            (*aList)[0].iTimersRunning);
    RDebug::Printf("StartTimerAndAddToList: list->Count() %d", aList->Count());
    if (aList->Count() == numberOfTimers)
        {
        ((*aList)[0].iStartTimer->Cancel());
        }
    return 1; // nada
    }

// --------------------------------------------------------------------------
// Starts timer and adds it to list given as param
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StopTimer( TAny* aArgument )
    {
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    RDebug::Printf("StopTimer: timers running %d", (*list)[0].iTimersRunning);
    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();
    (*list)[0].iTimersRunning -= 1;
    (*list)[(*list)[0].iCurrentlyActive].iEResult->iTimerStoppedTime
            = utcNow.Int64();
    if ((*list)[0].iTimersRunning == 0)
        {
        (*list)[0].iStartTimer->Cancel();
        CActiveScheduler::Stop();
        }
    return 1; // nada
    }
// --------------------------------------------------------------------------
// Try to configure timer ans see that correct error code is returned
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::ConfigureKErrInUseArray( TAny* aArgument )
    {
    RDebug::Printf("ConfigureKErrInUseArray");
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    TInt error = (*list)[(*list)[0].iCurrentlyActive].iTimer->Configure(
            TTimeIntervalMicroSeconds(12 * 1000 * 1000));
    if (KErrInUse != error)
        {
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iCaseAlreadyFailed
                = ETrue;
        _LIT(KError,"Error: Wrong return code 64bit if\n");
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iTestResult->SetResult(
                KErrGeneral, KError);
        return 1;
        }
    //Try same with 32 bit if
    error = (*list)[(*list)[0].iCurrentlyActive].iTimer->Configure(
            TTimeIntervalMicroSeconds32(12 * 1000 * 1000));
    if (KErrInUse != error)
        {
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iCaseAlreadyFailed
                = ETrue;
        _LIT(KError,"Error: Wrong return code 32bit if\n");
        (*list)[(*list)[0].iCurrentlyActive].iEResult->iTestResult->SetResult(
                KErrGeneral, KError);
        }
    return 1; // nada
    }

// --------------------------------------------------------------------------
// start timer again
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::StartTimerAgainArray( TAny* aArgument )
    {
    RDebug::Printf("StartTimerAgainArray");
    RArray<TTimerStruct> *list =
            reinterpret_cast<RArray<TTimerStruct>*> (aArgument);
    (*list)[(*list)[0].iCurrentlyActive].iTimer->After(
            TTimeIntervalMicroSeconds(12 * 1000 * 1000));
    return 1; // nada
    }
// --------------------------------------------------------------------------
// Start two timers and see that they expire at the same time.
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CpuLoadCreateAndExpireTwoOverlappingAfterTimersL(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    TTestResult *res1 = new TTestResult();
    TTestResult *res2 = new TTestResult();
    TExtendedTestResult *eResult1 = new TExtendedTestResult(res1);
    TExtendedTestResult *eResult2 = new TExtendedTestResult(res2);
    

    eResult1->iTimerID = 1;
    eResult2->iTimerID = 2;
    
    CInheritedCFlexTimer* timer1 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult1));
    CInheritedCFlexTimer* timer2 = CInheritedCFlexTimer::NewL(
            CActive::EPriorityStandard,
            TCallBack(InheritedFlexTimerCallbackKErrNoneMarkTimestamp,
                    eResult2));
    
    // Expire in 10 seconds
    TTimeIntervalMicroSeconds32 expireTime1(10 * 1000 * 1000);
    //Expire within first timers default window
    TTimeIntervalMicroSeconds32 expireTime2(static_cast<TInt64> ((10 * 1000
            * 1000 - static_cast<float> (10 * 1000 * 1000)
            * KDefaultWindowMultiplier)));
    
    // Save current time
    TTime utcNow(0);
    utcNow.UniversalTime();
    
    timer1->After(expireTime1);
    timer2->After(expireTime2);
    
    //Timer to stop active scheduler after timers under test are completed.
    CPeriodic *p = CPeriodic::NewL(EPriorityNormal);

    p->Start(15 * 1000 * 1000, 1000, TCallBack(StopActiveScheduler));

    MSTIFTestInterference* interference =  MSTIFTestInterference::NewL(
        cb, MSTIFTestInterference::EThread );

    // Start interference to generate 99% CPU load (100 ms load, 1% idle)
    interference->StartL( MSTIFTestInterference::ECpuLoad, 1, 100 );
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    interference->Stop();
    delete interference;
    
    delete p;

    // Check results
    TInt64 timer1RightWin =
        utcNow.Int64() + static_cast<TInt64>(expireTime1.Int());
    TInt64 timer2RightWin =
        utcNow.Int64() + static_cast<TInt64>(expireTime2.Int());
    TInt64 timer1LeftWin = timer2RightWin - static_cast<TInt64>(
        (static_cast<TReal64>(expireTime1.Int()) * KDefaultWindowMultiplier));
    TInt64 timer2LeftWin = timer2RightWin - static_cast<TInt64>(
        (static_cast<TReal64>(expireTime2.Int()) * KDefaultWindowMultiplier));
    TInt64 timer1StopedTime = eResult1->iTimerStoppedTime;
    TInt64 timer2StopedTime = eResult2->iTimerStoppedTime;
    
    //See if Timer 1 Callback has failed the case
    if (eResult1->iCaseAlreadyFailed)
        {
        _LIT( KDescription ,"Timer 1 has failed" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    //See if Timer 2 Callback has failed the case
    else if (eResult2->iCaseAlreadyFailed)
        {
        _LIT( KDescription,"Timer 2 has failed" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Check if timer 1 has timed out too early
    else if ((timer1StopedTime + KTickResolution) < timer1LeftWin)
        {
        _LIT( KDescription, "Timer 1 timed out too early" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Check if timer 1 has timed out too Late
    else if ((timer1StopedTime - KTimerAccuracyMicroSeconds) > timer1RightWin)
        {
        _LIT( KDescription, "Timer 1 timed out too late" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Check if timer 2 has timed out too early
    else if ((timer2StopedTime + KTickResolution) < timer2LeftWin)
        {
        _LIT( KDescription, "Timer 2 timed out too early" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Check if timer 2 has timed out too Late
    else if ((timer2StopedTime - KTimerAccuracyMicroSeconds - KExtraSlack) > timer2RightWin)
        {
        _LIT( KDescription, "Timer 2 timed out too late" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Check that the timers 1 and 2 timed out at the same time
    else if (((timer1StopedTime + KTimerAccuracyMicroSeconds) < timer2StopedTime) ||
        ((timer1StopedTime - KTimerAccuracyMicroSeconds) > timer2StopedTime))
        {
        _LIT( KDescription, "Timers 1 and 2 did not timeout at the same time" );
        aResult.SetResult( KErrGeneral, KDescription );
        }
    // Everything was successful
    else
        {
        _LIT( KDescription, "Passed" );
        aResult.SetResult( KErrNone, KDescription );
        }
    
    // Case was executed
    delete timer1;
    delete timer2;
    delete eResult1;
    delete eResult2;
    delete res1;
    delete res2;
    
    return KErrNone;
    }
// --------------------------------------------------------------------------
// Start many timers and see that nothing goes wrong when they are expired at rapid sequence
// --------------------------------------------------------------------------
//
TInt CTestCFlexTimer::CpuLoadManyTimersL(
        TTestResult& aResult, CTestFlexTimer *cb )
    {
    RDebug::Printf("CpuLoadManyTimersL");
    // This list will contain all the timer structs, expiry times etc.
    // First cell will always tell which timer is currently active
    // active info is transferred from e.g. InheritedCFlexTimer2::RunL to CB function
    RArray<TTimerStruct> timers;
    // This CB contains all the logic about timer addition
    StartTimerAndAddToList( &timers, 100 );
    
    MSTIFTestInterference* interference =  MSTIFTestInterference::NewL(
        cb, MSTIFTestInterference::EThread );
    // Start interference to generate 99% CPU load (100 ms load, 1% idle)
    interference->StartL( MSTIFTestInterference::ECpuLoad, 1, 50 );
    // Start scheduler and wait for callback to stop it
    CActiveScheduler::Start();
    interference->Stop();
    delete interference;
    
    // Delete CPeriodic that was used to launch new FlexTimers
    delete timers[0].iStartTimer;
    _LIT( KDescriptionP , "Passed");
    aResult.SetResult(KErrNone, KDescriptionP);
    // Loop throug list and see if timers fired within limits.
    // also delete all stuff reserved earlier by StartTimerAndAddToList
    while (timers.Count())
        {
        TInt64 rightWin = timers[0].iEResult->iTimerStartedTime +
            timers[0].iEResult->iTimerExpiryTime;
        TInt64 leftWin = rightWin -
            (static_cast<float> (timers[0].iEResult->iTimerExpiryTime) *
                KDefaultWindowMultiplier);
        TInt64 stopTime = timers[0].iEResult->iTimerStoppedTime;
        
        // Have fun with reading this. It is not as bad as it looks
        // Just checks if CB already failed the case
        // and if timer was launched within its window(default) and timer accuracy
        if (timers[0].iEResult->iCaseAlreadyFailed)
            {
            _LIT( KDescription , "Case was failed earlier for unknown reason.");
            aResult.SetResult(KErrGeneral, KDescription);
            }
        // Check if timer has expired before the window has started.
        else if((stopTime + KTickResolution)  < leftWin)
            {
            _LIT( KDescription , "Some timer has expired too soon");
            aResult.SetResult(KErrGeneral, KDescription);
            }
        // Check if timeout has happened too long time after window end.
        else if (stopTime > (rightWin + KTimerAccuracyMicroSeconds + KExtraSlack))
            {
            _LIT( KDescription , "Some timer has expired too late");
            aResult.SetResult(KErrGeneral, KDescription);
            }
        delete timers[0].iEResult->iTestResult;
        delete timers[0].iEResult;
        delete timers[0].iTimer;
        timers.Remove(0);
        }
    // Case was executed
    timers.Close();
    return KErrNone;
    }

// ======== GLOBAL FUNCTIONS ========
