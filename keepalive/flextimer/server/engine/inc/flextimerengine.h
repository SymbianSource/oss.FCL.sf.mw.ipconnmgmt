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
 * This class contains definition of CFlexTimerEngine.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERENGINE_H
#define FLEXTIMERENGINE_H

// System includes
#include <e32base.h>

// User includes
#include "mflextimerservice.h"
#include "mflextimerwakeuptimercb.h"

// Forward declarations
class MFlexTimerServiceCB;
class CFlexTimerContainer;
class CFlexTimerWakeUpTimer;

// Class declaration
/**
 *  Flexible timer engine that acts as service point towards session.
 *  Handles timer wakeups and system time changes.
 */
class CFlexTimerEngine : public CBase,
        public MFlexTimerService,
        public MFlexTimerWakeUpTimerCB
    {
public:

    /**
     * Constructs new timer engine.
     */
    IMPORT_C static CFlexTimerEngine* NewL();

    /**
     * Destructs timer item.
     */
    virtual ~CFlexTimerEngine();

public:
    /**
     * Inherited from MFlexTimerService
     */

    /**
     * Implementation of inherited pure virtual function
     *  
     * @see MFlexTimerService::AddTimer
     */
    IMPORT_C TInt AddTimer(
        const TTimeIntervalMicroSeconds& aWinStartInterval,
        const TTimeIntervalMicroSeconds& aWinEndInterval,
        TBool aCancelAtSystemTimeChange,
        const MFlexTimerServiceCB* aFlexTimerServiceCB );

    /**
     * Implementation of inherited pure virtual function
     *  
     * @see MFlexTimerService::CancelTimer
     */
    IMPORT_C TInt CancelTimer(
        const MFlexTimerServiceCB* aFlexTimerServiceCB );

    /**
     * Inherited from MFlexTimerWakeUpTimerCB
     */

    /**
     * Implementation of inherited pure virtual function
     *  
     * @see MFlexTimerWakeUpTimerCB::WakeUp
     */
    void WakeUp();

    /**
     * Implementation of inherited pure virtual function
     *  
     * @see MFlexTimerWakeUpTimerCB::SystemTimeChanged
     */
    void SystemTimeChanged();

private:
    /**
     * Private constructor.
     *  
     */
    CFlexTimerEngine();

    /**
     * Private 2nd pahse constructor.
     *  
     */
    void ConstructL();

    /**
     * Finds out what is the next time we have to wake up to fire some
     * FlexTimers and starts CTimer accordingly.
     *  
     */
    void StartTimer();

private:
    // Data
    /**
     * Container for all timers. Lives and dies with engine.
     */
    CFlexTimerContainer* iFlexTimerContainer;

    /**
     * Wake up timer to watch flextimer firings and system time change
     */
    CFlexTimerWakeUpTimer* iWakeUpTimer;

    };

#endif  //FLEXTIMERENGINE_H
