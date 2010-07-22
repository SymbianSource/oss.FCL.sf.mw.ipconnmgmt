/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: 
 * Common header for flextimer panic codes.
 *
 */
/*
 * %version: 1 %
 */
#ifndef FLEXTIMERPANIC_H
#define FLEXTIMERPANIC_H
// System includes
#include <e32base.h>


/**
 * Server and session panic category and codes
 */
// Panic category
_LIT( KRFlexTimerPanicCat, "RFlexTimer");
_LIT( KCFlexTimerPanicCat, "CFlexTimer");
_LIT( KCFlexPeriodicPanicCat, "CFlexPeriodic");
_LIT( KFlexTimerServerPanicCat, "FlexTimerServer" );
_LIT( KFlexTimerSessionPanicCat, "FlexTimerSession" );
_LIT( KFlexTimerContainerPanicCat, "FlexTimerContainer" );

enum TFlexTimerPanic
    {
    /**
     * Timer After-function has been provided with a negative interval value. 
     * @see RFlexTimer
     */
    EFlexTimerAfterIntervalLessThanZero = 1,

    /**
     * Timer AfterTicks-function has been provided with a negative interval
     * value. 
     * @see RFlexTimer
     */
    EFlexTimerAfterTicksIntervalLessThanZero = 2,

    /**
     * Timer At-function has been provided with an expiry time prior to
     * TTime::HomeTime. 
     * @see RFlexTimer
     * @see TTime
     */
    EFlexTimerAtIntervalLessThanZero = 3,

    /**
     * Timer AtUTC-function has been provided with an expiry time prior to
     * TTime::UniversalTime. 
     * @see RFlexTimer
     * @see TTime
     */
    EFlexTimerAtUTCIntervalLessThanZero = 4,

    /**
     * Timer has been configured with flexible window size less than zero. 
     * @see RFlexTimer
     */
    EFlexTimerWindowLessThanZero = 5,

    /**
     * Periodic timer Start-function has been called with delay value less
     * than zero. 
     * @see CFlexPeriodic
     */
    EFlexPeriodicDelayLessThanZero = 6,

    /**
     * Periodic timer Start-function has been called with interval value less
     * than one microsecond.
     * @see CFlexPeriodic
     */
    EFlexPeriodicIntervalTooSmall = 7,

    /**
     * Periodic has been configured with delay window size less than zero. 
     * @see CFlexPeriodic
     */
    EFlexPeriodicDelayWindowLessThanZero = 8,

    /**
     * Periodic has been configured with interval window size less than zero. 
     * @see CFlexPeriodic
     */
    EFlexPeriodicIntervalWindowLessThanZero = 9,

    /**
     * This panic is raised by the At(), After() and Lock() member functions
     * of the CFlexTimer active object.
     * 
     * It is caused by an attempt to request a timer event when the CFlexTimer
     * active object has not been added to the active scheduler.
     * @see CFlexTimer
     */
    EFlexTimerNotAddedToActiveScheduler = 10,
                
    /**
     * New timer has been issued while previous operation is still pending
     * @see RFlexTimer
     */
    EFlexTimerServerErrorPendingTimer = 15,

    /**
     * Cleanup stack memory creation failed for FlexTimer server process
     */
    EFlexTimerServerCleanupStack = 20,

    /**
     * Memory allocation for active scheduler object filed in server creation
     */
    EFlexTimerServerActiveScheduler = 21,

    /**
     * Starting CServer2 based FlexTimer server failed
     */
    EFlexTimerServerStartServer = 23,
    
    /**
     * User has issued a timer request with parameters that can not be
     * fulfilled
     */
    EFlexTimerServerIllegalTimerValue = 24,

    /**
     * FlexTimer session failed to read tick period from HAL. This means that
     * we are not able to map our tick time to system time.
     */
    EFlexTimerContainerNoTickPeriod = 30,

    /**
     * In CFlexPeriodic::Start() has been passed a NULL function pointer in
     * TCallBack aCallBack parameter.
     */
    EFlexPeriodicCallbackFunctionIsNull = 31,
    
    /**
     * In CFlexPeriodic::Start() has been passed a NULL function pointer in
     * TCallBack aCallBackError parameter and error has occurred.
     */
    EFlexPeriodicErrorCallbackFunctionIsNull = 32,
    
    /**
     * RFlexTimer::Connect() has been called twice without closing the handle
     * first.
     */
    EFlexTimerAlreadyConnected = 33
    };

#endif /* FLEXTIMERPANIC_H */
