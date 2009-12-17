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
* Description:  Event queue stores an event until all the clients can
*                receive it.
*
*/

#ifndef CONNMONCEVENTQUEUE_H
#define CONNMONCEVENTQUEUE_H

#include <rconnmon.h>

#include "ConnMonDef.h"

class CConnMonSession;

/**
* TEventNode
*/
class TEventNode
    {
private:
    TSglQueLink iLink;
public:
    TEvent iEvent;
    };

/**
* CEventQueue
* Singly linked list.
*/
NONSHARABLE_CLASS( CEventQueue ) : public CBase
    {
public:
    /**
     * Constructor.
     */
    void ConstructL( CConnMonServer* aServer );

    /**
     * Destructor.
     */
    ~CEventQueue();

public:
    /**
     * Adds a new event to the queue.
     * @param aEventInfo Event to be added.
     */
    void Add( const TEventInfo& aEventInfo, const TUint8* aPtr = NULL, TInt aSize = 0 );

    /**
     * Scans the event queue to find a new event for the session.
     * @param aSession The session requesting a new event.
     */
    void CheckEvents( CConnMonSession* aSession );

    /**
     * Removes an event from the event queue.
     * @param aEventNode Pointer to event node to be removed.
     */
    void FreeEvent( TEventNode* aEventNode );

    /**
     * Returns the time of the latest event.
     * @return TTime.
     */
    TTime LastEventTime() const
        {
        return iLastEventTime;
        }

private: // Data
    // Pointer to the server
    CConnMonServer* iServer;

    // Time of the latest event
    TTime iLastEventTime;

    // List of events
    TSglQue<TEventNode> iEvents;

    // List of free slots
    TSglQue<TEventNode> iSlots;
    };

#endif // CONNMONCEVENTQUEUE_H

// End-of-file
