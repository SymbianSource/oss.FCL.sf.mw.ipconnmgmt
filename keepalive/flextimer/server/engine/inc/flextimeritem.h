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
 * This class contains definition of CFlexTimerItem.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERITEM_H
#define FLEXTIMERITEM_H

// System includes
#include <e32base.h>

// User includes
// None

// Forward declarations
class MFlexTimerServiceCB;

// Class declaration
/**
 *  Timer item that contains expiry time, window size and other timer specific
 *  parameters. Class implements container for one timer information in
 *  FlexTimerServer.
 */
class CFlexTimerItem : public CBase
    {
public:

    /** Constructor
     *
     * Constructs new timer item that contains all information needed to
     * decide when to launch timer.
     *
     * @param aAbsoluteMinTime Left side of the timer launch window.
     * @param aAbsoluteMaxTime Right side of the timer launch window.
     * @param aCancelAtSystemTimeChange Whether system time change should
     * trigger abort.
     * @param aFlexTimerServiceCB Call back interface. Used to inform about
     * timeout, abort etc.
     * @return New CFlexTimer object.
     */
    static CFlexTimerItem* NewL( const TTime& aAbsoluteMinTime,
        const TTime& aAbsoluteMaxTime,
        TBool aCancelAtSystemTimeChange,
        const MFlexTimerServiceCB* aFlexTimerServiceCB );

    /** Destructor
     * Destructs timer item.
     */
    virtual ~CFlexTimerItem();

public:
    /**
     * Gets the earliest possible moment when this timer can expire. 
     * Places value to reference parameter. 
     *
     * @param aMinAbsoluteTime After function returns this parameter contains
     * absolute earliest moment when this timer can expire         
     */
    inline void GetMinAbsoluteTime( TTime& aMinAbsoluteTime );

    /**
     * Gets the latest possible moment when this timer must expire. 
     * Places value to reference parameter. 
     *
     * @param aMaxAbsoluteTime After function returns this parameter contains
     * absolute latest moment when this timer must expire       
     */
    inline void GetMaxAbsoluteTime( TTime& aMaxAbsoluteTime );

    /**
     * Checks whether this timer should be aborted if system time changes
     *
     * @return 
     * ETrue - If timer should be aborted when system time changes
     * EFalse - If timer should not be cancelled when system time changes        
     */
    inline TBool IsAbortedAtSystemTimeChange();

    /**
     * Returns timers corresponding CB session pointer.
     *
     * @return pointer to session that initiated this timer       
     */
    const MFlexTimerServiceCB* GetCB();

private:
    /**
     * Private contructor. Constructs new timer item that contains all
     * information needed to decide when to launch timer.
     *
     * @param aAbsoluteMinTime Left side of the timer launch window.
     * @param aAbsoluteMaxTime Right side of the timer launch window.
     * @param aCancelAtSystemTimeChange Whether system time change should
     * trigger abort.
     * @param aFlexTimerServiceCB Call back interface. Used to inform about
     * timeout, abort etc.
     *
     * @return New CFlexTimer object.
     * 
     */
    CFlexTimerItem( const TTime& aAbsoluteMinTime,
        const TTime& aAbsoluteMaxTime,
        TBool aCancelAtSystemTimeChange,
        const MFlexTimerServiceCB* aFlexTimerServiceCB );

public:
    //Data
    /**
     * Single linked list link object for timers lists
     */
    TSglQueLink iLink;

private:
    // Data
    /**
     * Whether timer should be aborted when system time changes.
     */
    TBool iAbortAtSystemTimeChange;

    /**
     * Left side of the time window when timer can be fired
     */
    TTime iAbsoluteMinTime;

    /**
     * Right side of the time window when timer can be fired
     */
    TTime iAbsoluteMaxTime;

    /**
     * pointer to the session interface that is used to inform timeout
     */
    const MFlexTimerServiceCB* iMFlexTimerServiceCB;
    };

#include "flextimeritem.inl"

#endif  //FLEXTIMERITEM_H
