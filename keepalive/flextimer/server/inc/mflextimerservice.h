/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 * This class contains definition of MFlexTimerService
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef MFLEXTIMERSERVICE_H
#define MFLEXTIMERSERVICE_H

//Forward declarations
class TTime;
class MFlexTimerServiceCB;

// Class declaration
/**
 *  Interface to flexible timer engine. This interface is used by client
 *  sessions to start and cancel timers.
 */
class MFlexTimerService
    {
public:

    /** Pure virtual functions*/
    /**
     * Function for adding a new timer with specified parameters. 
     * After timer expires aFlexTimerServiceCB is used to inform a client
     * session about that. Both parameter times should be in the future
     * and min should be earlier than max
     * 
     * @param aWinStartInterval Time interval from present to left side of the
     *  window
     * @param aWinEndInterval Time interval from present to right side of the
     *  window
     * @param aCancelAtSystemTimeChange Whether timer should be aborted if
     *  system time changes.
     * @param aFlexTimerServiceCB Callback interface for timer
     *  expiration (also used as an ID)
     * 
     * @return KErrNone or some other system wide error code.
     *
     */
    virtual TInt AddTimer(
            const TTimeIntervalMicroSeconds& aWinStartInterval,
            const TTimeIntervalMicroSeconds& aWinEndInterval,
            TBool aCancelAtSystemTimeChange,
            const MFlexTimerServiceCB* aFlexTimerServiceCB ) = 0;

    /**
     * Function for cancellin a running timer. After this function returns,
     * all references to corresponding timer are deleted and they can not be
     * referred again.
     * 
     * @param aFlexTimerServiceCB ID used when timer was added
     * @return KErrNone or some other system wide error code.
     */
    virtual TInt CancelTimer(
            const MFlexTimerServiceCB* aFlexTimerServiceCB ) = 0;

    };

#endif  /* MFLEXTIMERSERVICE_H */
