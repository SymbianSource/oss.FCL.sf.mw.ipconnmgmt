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
 * This class contains definition of CFlexTimerContainer.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERCONTAINER_H
#define FLEXTIMERCONTAINER_H

// System includes
#include <e32base.h>

// Forward declarations
class MFlexTimerServiceCB;
class CFlexTimerItem;

// Class declaration
/**
 *  Timer container that contains list of CFlexTimerItems, means to add and
 *  delete them and implementation of algorithms that select which timers
 *   should be expired.
 */
class CFlexTimerContainer : public CBase
    {
public:
    /** Supported timer algorithms.*/
    enum TFlexTimerAlgorithm
        {
        /** Simple algorithm timeouts all timers that have open timeout
         * window when one timer expires.
         */
        EFlexTimerAlgorithmSimple = 1,
        /** Latest possible algorithm tries to delay expiration of timer
         * as long as possible.
         */
        EFlexTimerAlgorithmLatestPossible
        };

    /**
     * Constructs new CFlexTimerContainer.
     */
    static CFlexTimerContainer* NewL();

    /**
     * Destructs timercontainer and all timers in the internal lists.
     */
    virtual ~CFlexTimerContainer();

public:
    /**
     * Makes new timer item and adds it to timerlist.
     *
     * @param aWinStartInterval Time interval from present to left side of the
     *  timer launch window
     * @param aWinEndInterval Time interval from present to right side of the
     *  timer launch window
     * @param aCancelAtSystemTimeChange Whether system time change should
     *  trigger abort
     * @param aFlexTimerServiceCB Call back interface. Used to inform about
     *  timeout, abort etc.
     */
    void AddTimerL(
        const TTimeIntervalMicroSeconds& aWinStartInterval,
        const TTimeIntervalMicroSeconds& aWinEndInterval,
        TBool aCancelAtSystemTimeChange,
        const MFlexTimerServiceCB* aFlexTimerServiceCB );

    /**
     * Finds correct timer according to aFlexTimerServiceCB and removes it
     * from list and finally 
     * deletes the corresponding CFlexTimerItem.
     * @param aFlexTimerServiceCB Call back interface aka. session that
     * handles timer.
     * @return KErrNone or some system wide error code.
     */
    TInt RemoveTimer( const MFlexTimerServiceCB* aFlexTimerServiceCB );

    /**
     * Finds earliest moment that at least one timer must expire and after
     * this function returns aNextTimeoutDelay contains number of microseconds
     * from now to that moment.
     *
     * @param after return aNextTimeoutDelay contains time in microseconds
     * from now till next timer expiry. If aNextTimeoutDelay is zero, it means
     * that expiry time has already passed or is due right now
     * @return Active timers indication
     * - ETrue if there is at least one timer in queue
     * - EFalse if there are no timers
     */
    TBool GetNextTimeout( TTimeIntervalMicroSeconds& aNextTimeoutDelay );

    /**
     * Fires all timers that can be fired according to algorithm that is
     * supplied as parameter.
     *
     * @param aAlgorithmToBeUsed Algorithm that is used to fire timers.
     * - EFlexTimerAlgorithmSimple
     * - EFlexTimerAlgorithmLatestPossible
     */
    void FireTimers( TFlexTimerAlgorithm aAlgorithmToBeUsed );

    /**
     * Aborts and removes all timers that are marked with
     * aCancelAtSystemTimeChange = ETrue during adding timer.
     *
     * @param aReason Timers will be aborted with this reason code. Can be
     * e.g. KErrAbort
     */
    void AbortTimersDueToTimeChange( TInt aReason );

private:
    /**
     * Private constructor.
     */
    CFlexTimerContainer();

    /**
     * Excecutes simple algorithm.
     * 
     * Moves timers (that can be fired right now) from iTimerList to
     * candidateList.
     * 
     * @param aCandidateList After this function returns, this list contains
     * all timers that can be fire right now.
     * @param aCurrentTime Current safe UTC time.
     */
    void SimpleAlgorithm( TSglQue<CFlexTimerItem>& aCandidateList,
        TTime& aCurrentTime );

    /**
     * Excecutes latest possible algorithm.
     * 
     * Moves timers (that don't necessarily be fired right now) from
     * candidateList to iTimerList.
     * 
     * @param aCandidateList After this function returns, this list contains
     * all timers that can be fire right now.
     */
    void LatestPossibleAlgorithm( TSglQue<CFlexTimerItem>& aCandidateList );

    /**
     * Fires all timers in aCandidateList. Calls Timeout() to all timers and
     * deletes timer items.
     * 
     * @param aCandidateList Timers that are fired.
     */
    void ExpireTimers( TSglQue<CFlexTimerItem>& aCandidateList );

    /**
     * Gets current absolute time. The time in FlexTimer engine time base, not
     * system time base. This time base is begins from the first call to
     *  GetCurrentTime and it is base on system ticks.
     * 
     * @param aCurrentAbsoluteTime After this function returns this contains
     * current flextimer timebase time.
     */
    void GetCurrentTime( TTime& aCurrentAbsoluteTime );

    /**
     * Converts microseconds interval to tick based reference time used by
     * FlexTimer engine (also in microseconds). All FlexTimer timeout element
     * time handling is based on this tick based absolute time.
     *
     * @param aInterval Interval to be converted.
     */
    inline TInt64 IntervalToAbsoluteTime(
        const TTimeIntervalMicroSeconds& aInterval );

    /**
     * Converts ticks to microseconds.
     *
     * @param aTicks. Ticks to be converted to microseconds
     */
    inline TInt64 TicksToAbsoluteTime( TUint32 aTicks );

private:
    // Data
    /**
     * Linked list and iterator for all timer objects.
     */
    TSglQue<CFlexTimerItem> iTimerList;
    
    /**
     * Length of system tick in milliseconds. The value is read once
     * to member variable during construction of FlexTimerContainer so that
     * it does not need be read every time the absolute time is calculated.
     */
    TInt iTickPeriod;
     
    /**
     * Number of tics during the last time they were read. This is used to
     * detect possible tick counter overflow and to calculate time passed
     * since the last read.
     */
    TUint32 iLastTicks;
    
    /**
     * Current FlexTimer engine time. This is system tick based time and
     * it is updated every time someone request for it.
     */
    TInt64 iCurrentAbsoluteTime;
    };

#include "flextimercontainer.inl"

#endif  //FLEXTIMERCONTAINER_H
