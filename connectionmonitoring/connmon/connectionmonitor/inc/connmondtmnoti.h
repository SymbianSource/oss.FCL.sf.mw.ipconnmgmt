/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConnMon Dual Transfer Mode watcher.
*
*/

#ifndef CONNMONDTMNOTI_H
#define CONNMONDTMNOTI_H

#include <etelpckt.h>

/**
* Connection Monitor Server Dual Transfer Mode watcher observer.
* @lib CONNMON.LIB
*/
class MConnMonDtmNotifierObserver
    {
public:
    /**
     * Informs the observer of a change in DTM
     */
    virtual void DtmStateChanged() = 0;
    };

/**
* Connection Monitor Server Dual Transfer Mode watcher.
* @lib CONNMON.LIB
*/
NONSHARABLE_CLASS( CConnMonDtmNotifier ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aPacketService A pointer to a RPacketService instance, which has
     * been initialized fully.
     * @param aObserver A refernce to the observer which wants to listen to the
     * notifications.
     *
     * @return An initialized instance of this class.
     */
    static CConnMonDtmNotifier* NewL(
            RPacketService& aPacketService,
            MConnMonDtmNotifierObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CConnMonDtmNotifier();

public:
    /**
     * Checks if there is support for Dual Transfer Mode.
     * @return ETrue support for DTM exists, otherwise EFalse.
     */
    TBool IsInDualMode() const;

    /**
     * Starts to notify client of the events
     */
    void Receive();

    /**
     * Cancels notifying the the events
     * @since
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
     * @param aPacketService A pointer to a RPacketService instance, which has
     * been initialized fully.
     * @param aObserver A refernce to the observer which wants to listen to the
     * notifications.
     */
    CConnMonDtmNotifier(
            RPacketService& aPacketService,
            MConnMonDtmNotifierObserver& aObserver );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:
    RPacketService& iPacketService;
    RPacketService::TMSClass iMsClass;
    RPacketService::TMSClass iMaxMsClass;
    MConnMonDtmNotifierObserver& iObserver;
    TBool iSendEvents;
    };

#endif // CONNMONDTMNOTI_H

// End-of-file
