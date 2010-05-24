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
 * Class definition for Flexible timer server side sessions. Session
 * instances handle the commmunication with clients and implement a 
 * callback interface for the CFlexTimerEngine to fire timers
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERSESSION_H
#define FLEXTIMERSESSION_H

// System includes
#include <e32base.h>

// User includes
#include "mflextimerservicecb.h"

// Forward declarations
class MFlexTimerService;

// Class declaration
/**
 *  Flexible timer server side session
 *  CFlexTimerSession implements sessions in CFlexTimerServer. A sessions
 *  parses client messages and adds/cancels timers to CFlexTimerEngine. The
 *  expiration of the timer completes client side message.
 */
class CFlexTimerSession : public CSession2, public MFlexTimerServiceCB
    {

public:

    /**
     * Constructor
     * 
     * @param aFlexTimerService A pointer to engine interface providing
     *                          timeout service
     */
    CFlexTimerSession( MFlexTimerService* aFlexTimerService );

    /**
     * Destructor
     */
    virtual ~CFlexTimerSession();

    /**
     * From: CSession2
     * Message handler servicing client requests. Interprets message type and
     * passes it to correct function for processing.
     * 
     * @param aMessage Message from client
     */
    void ServiceL( const RMessage2& aMessage );

    /**
     * From CSession2
     * Called by a server when it receives a disconnect message for the 
     * session.
     * 
     * @param aMessage The disconnect message
     */
    void Disconnect( const RMessage2 &aMessage );
    
    /**
     * From: MFlexTimerServiceCB
     * Timeout callback function called by timeout engine when a timer
     * associated to this session expires.
     * 
     * @see MFlexTimerServiceCB
     */
    virtual void Timeout() const;

    /**
     * From: MFlexTimerServiceCB
     * Callback function called by CFlexTimerEngine, when a timer associated
     * to this session is aborted. Most probable cause for this is At()-timer
     * aborting due to the system time change
     * 
     * @param aReason Error code to be passed for client (KErrAbort in case 
     *                of system time change)
     *                
     * @see MFlexTimerServiceCB
     */
    virtual void Abort( TInt aReason ) const;

private:

    /**
     * Handles client messages that are issued using At()-function in
     * RFlexTimer
     * 
     * @param aMessage Message from client
     *
     * @see RFlexTimer
     */
    void NewAtTimer( const RMessage2& aMessage );

    /**
     * Handles client messages that are issued using AtUTC()-function in
     * RFlexTimer
     * 
     * @param aMessage Message from client
     *
     * @see RFlexTimer
     */
    void NewAtUtcTimer( const RMessage2& aMessage );

    /**
     * Handles client messages that a issued using After()-function in
     * RFlexTimer
     * 
     * @param aMessage Message from client
     * 
     * @see RFlexTimer
     */
    void NewAfterTimer( const RMessage2& aMessage );

    /**
     * Handles client messages that a issued using AfterTicks()-function in
     * RFlexTimer
     * 
     * @param aMessage Message from client
     * 
     * @see RFlexTimer
     */
    void NewAfterTicksTimer( const RMessage2& aMessage );

    /**
     * Handles timer cancellation initiated by client via RFlexTimer::Cancel
     * 
     * @param aMessage Message from client
     * 
     * @see RFlexTimer
     */
    void CancelTimer( const RMessage2& aMessage );

    /**
     * Configuration message handler, stores configuration parameters for
     * this session
     * 
     * @param aMessage Message from client
     * 
     * @see RFlexTimer
     */
    void ConfigureParameters( const RMessage2& aMessage );

    /**
     * Inline function for calculating the period from current time to
     * the timestamp given in message parameters. This is used for
     * converting the timestamp given in At()- and AtUtc() -functions
     * to an interval given to DoAddTimer function.
     * 
     * @param aRefTime  Reference time used as a basis for interval
     *                  calculation (interval = timestamp - aRefTime)
     * @param aMessage  Message containing timestamp in Int0 and Int1
     *                  parameters (Int0 contains LSW)
     * @param aInterval On return, contains the interval in microseconds
     */
    inline void GetIntervalToMsgTime( const RMessage2& aMessage,
                                      TTime& aRefTime,
                                      TTimeIntervalMicroSeconds& aInterval );
    
    /**
     * Function for adding timer with desired window to engine, called in
     * AtXxx() and AfterXxx() -functions
     * 
     * @param aMessage      Message from client. Stored if adding a timer is
     *                      successful, completed if adding fails
     * @param aInterval     Microseconds until to requested end time
     * @param aRequestAbort ETrue if timer should be cancelled on system
     *                      time change, EFalse otherwise
     */
    void DoAddTimer( const RMessage2& aMessage,
                     TTimeIntervalMicroSeconds& aInterval,
                     TBool aRequestAbort );

private:

    /**
     * Pointer to engine interface through which new timers can be added
     * and cancelled.
     */
    MFlexTimerService* iService;

    /**
     * Timeout window size in microseconds (64-bit). Timeout window is
     * calculated backwards from the absolute timestamp given in AtXxx()
     * and AfterXxx() -functions
     */
    TTimeIntervalMicroSeconds iTimerWinSize;

    /**
     * ETrue if client has not set timeout window, EFalse otherwise
     */
    TBool iUseDefaultWin;

    /**
     * Handle to pending timeout message
     */
    RMessagePtr2 iPendingTimeoutMsg;
    };

#include "flextimersession.inl"

#endif /* FLEXTIMERSESSION_H */
