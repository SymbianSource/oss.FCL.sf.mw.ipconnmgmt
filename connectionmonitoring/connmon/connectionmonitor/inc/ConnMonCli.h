/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CConnMonEventHandler is a hidden active object to receive
*                notifications on behalf of a client application.
*
*/

#ifndef CONNMONCLI_H
#define CONNMONCLI_H

void Panic( TInt aPanic );

/**
* CConnMonEventHandler
*/
NONSHARABLE_CLASS( CConnMonEventHandler ) : public CActive
    {
public:
    CConnMonEventHandler(
            MConnectionMonitorObserver* aObserver,
            RConnectionMonitor& aSession );
    void Construct();
    ~CConnMonEventHandler();

public:
    /**
     * Requests a new event from the server.
     */
    void ReceiveNotification();

    /**
     * Pauses receiving events.
     */
    void Pause();

    /**
     * Continues receiving events.
     * @param aObserver The event observer implemented by the client.
     */
    void Continue( MConnectionMonitorObserver* aObserver );

private:
    /**
     * From CActive Handles the event that has arrived from the server.
     */
    void RunL();
    /**
     * From CActive Cancels the request from the server.
     */
    void DoCancel();

private: // Data
    // Handle to the client session
    RConnectionMonitor& iSession;

    // Pointer to the observer interface
    MConnectionMonitorObserver* iObserver;

    // The event. It will be sent to the client
    CConnMonEventBase* iConnMonEvent;

    // Event information
    TEventInfo iEventInfo;

    // Needed in inter-thread reads/writes
    TPtr8 iBuf;

    // Events are not requested from the server when iPause=ETrue
    TBool iPause;

    // For big events
    TConnMonExtraBuf iExtraBuf;
    };

#endif // CONNMONCLI_H

// End-of-file
