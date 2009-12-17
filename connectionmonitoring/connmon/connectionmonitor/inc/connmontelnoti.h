/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConnMon Telephony watcher.
*
*/

#ifndef CONNMONTELNOTI_H
#define CONNMONTELNOTI_H

#include <etelpckt.h>
#include <etelmm.h>

/**
 * Connection Monitor Server Telephony watcher observer.
 * @lib CONNMON.LIB
 */
class MConnMonTelephonyObserver
    {
public:
    /**
     * Notifies the observer that the phone line status has changed.
     */
    virtual void PhoneLineStatusChange() = 0;
    };

/**
 * Connection Monitor Server Telephony watcher.
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CConnMonTelNotifier ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aObserver A refernce to the observer which wants to listen to the
     * notifications.
     * @param aMobilePhone A pointer to a RMobilePhone instance, which has been
     * initialized fully.
     * @return An initialized instance of this class.
     */
    static CConnMonTelNotifier* NewL(
            MConnMonTelephonyObserver& aObserver,
            RMobilePhone* aMobilePhone );

    /**
     * Destructor.
     */
    virtual ~CConnMonTelNotifier();

public:
    /**
     * Checks if phone call is active.
     * @return ETrue phone call is active, otherwice EFalse.
     */
    TBool IsCallActive() const;

    /**
     * Starts to notify client of the events.
     */
    void Receive();

    /**
     * Cancels notifying the the events.
     */
    void CancelNotifications();

protected:
    /**
     * From CActive. Cancels request.
     */
    void DoCancel();

    /**
     * From CActive. Run when request completes.
     */
    void RunL();

private:
    /**
    * Constructor.
    *
    * @param aObserver A refernce to the observer which wants to listen to the
    * notifications.
    * @param aMobilePhone A pointer to a RMobilePhone instance, which has been
    * initialized fully.
    */
    CConnMonTelNotifier(
            MConnMonTelephonyObserver& aObserver,
            RMobilePhone* aMobilePhone );

    /**
     * 2nd phase constructor.
     */
    void Construct();

private:
    RMobilePhone* iMobilePhone;
    TBool iVoiceCallActive;
    RCall::TStatus iCallStatus;
    MConnMonTelephonyObserver& iObserver;

    // ETrue if client has called receive and wants the notifications. Only
    // affects the sending of the notifications not the receving.
    TBool iSendEvents;

    RLine iLine;
    };

#endif // CONNMONTELNOTI_H

// End-of-file
