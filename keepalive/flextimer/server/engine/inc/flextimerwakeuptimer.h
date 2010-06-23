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
 * This class contains definition of CFlexTimerWakeUpTimer.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERWAKEUPTIMER_H
#define FLEXTIMERWAKEUPTIMER_H

// System includes
#include <e32base.h>

// User includes
// None

// Forward declarations
class MFlexTimerWakeUpTimerCB;

// Class declaration
/**
 *  This class implements timer that is used to
 *  - Inform timer engine that timeout algorithms should be excecuted
 *  - Inform timer engine that system time has changed.
 *
 */
class CFlexTimerWakeUpTimer : public CTimer
    {
public:

    /**
     * Contructs new wake up timer
     * @param aObserver callback class
     *
     */
    static CFlexTimerWakeUpTimer* NewL( MFlexTimerWakeUpTimerCB& aObserver );

    /**
     * Destructs wake up timer.
     */
    virtual ~CFlexTimerWakeUpTimer();

public:
    /**
     * Starts timer. No need to cancel already running timer.
     * @param aInterval time until timer should expire         
     */

    void StartTimer( TTimeIntervalMicroSeconds& aInterval );
    
    /**
     * Stops timer.   
     */
    void StopTimer();

    /**
     * Inherited from CActive
     * Handles timer expiration and callbacks.
     */
    void RunL();

private:
    // Functions
    /**
     * Private contructor.
     * @param aObserver Callback class.         
     */
    CFlexTimerWakeUpTimer( MFlexTimerWakeUpTimerCB& aObserver );
    /**
     * Private 2nd phase contructor.   
     */
    void ConstructL();

private:
    // Data
    /**
     * Callback class
     */
    MFlexTimerWakeUpTimerCB& iObserver;

    };

#endif  //FLEXTIMERWAKEUPTIMER_H
