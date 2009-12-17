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

#include "ConnMonServ.h"
#include "ConnMonSess.h"
#include "CEventQueue.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEventQueue::ConstructL
// -----------------------------------------------------------------------------
//
void CEventQueue::ConstructL( CConnMonServer* aServer )
    {
    iServer=aServer;

    // Allocate nodes
    for ( TInt i=0; i < KEventQueueSize; i++ )
        {
        TEventNode* node = new( ELeave ) TEventNode;
        iSlots.AddFirst( *node );
        }

    // Initialize latest event time
    iLastEventTime.UniversalTime();
    }

// Destructor
CEventQueue::~CEventQueue()
    {
    TEventNode* node;

    // Deallocate nodes
    for ( node=iSlots.First(); node; node=iSlots.First() )
        {
        iSlots.Remove( *node );
        delete node;
        }

    for ( node=iEvents.First(); node; node=iEvents.First() )
        {
        if ( node->iEvent.iExtra.iPtr )
            {
            delete node->iEvent.iExtra.iPtr;
            node->iEvent.iExtra.iPtr = NULL;
            }

        iEvents.Remove( *node );
        delete node;
        }

    iServer = NULL;
    }

// -----------------------------------------------------------------------------
// CEventQueue::Add
// -----------------------------------------------------------------------------
//
void CEventQueue::Add( const TEventInfo& aEventInfo, const TUint8* aPtr, TInt aSize  )
    {
    TTimeIntervalMicroSeconds microSecond( 1 );
    iLastEventTime = iLastEventTime + microSecond;

    // Get the first free slot - remove oldest event if queue is full
    TEventNode* node = iSlots.First();

    if ( !node )
        {
        //return;
        node = iEvents.First();
        FreeEvent( node );
        node = iSlots.First();
        }

    iSlots.Remove( *node );

    // Plug in values
    node->iEvent.iInfo        = aEventInfo;
    node->iEvent.iTime        = iLastEventTime;
    node->iEvent.iExtra.iPtr  = aPtr;
    node->iEvent.iExtra.iSize = aSize;

    // Add to the event queue
    iEvents.AddLast( *node );

    // Offer the event to all the clients that are listening
    // and have requested a new event.
    TInt numOffered( 0 );
    iServer->SendEventToSessions( node->iEvent, numOffered );

    // Remove immediately from the queue if all the listeners have got the event
    if ( numOffered == iServer->NumberOfListeners() )
        {
        FreeEvent( node );
        }
    }

// -----------------------------------------------------------------------------
// CEventQueue::CheckEvents
// See if there is a new event in the queue for aSession.
// -----------------------------------------------------------------------------
//
void CEventQueue::CheckEvents( CConnMonSession* aSession )
    {
    TSglQueIter< TEventNode > iter( iEvents );

    if ( aSession == 0 )
        {
        return;
        }

    for ( TEventNode* node = iter++; node; node = iter++ )
        {
        if ( aSession->CanReceiveEvent() )
            {
            if ( aSession->SendEventToClient( node->iEvent ) )
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CEventQueue::FreeEvent
// -----------------------------------------------------------------------------
//
void CEventQueue::FreeEvent( TEventNode* aNode )
    {
    if ( aNode != 0 )
        {
        if ( aNode->iEvent.iExtra.iPtr )
            {
            delete aNode->iEvent.iExtra.iPtr;
            aNode->iEvent.iExtra.iPtr = NULL;
            }

        iEvents.Remove( *aNode );

        aNode->iEvent.iInfo.Reset();
        aNode->iEvent.iExtra.Reset();

        iSlots.AddFirst( *aNode );
        }
    }

// End-of-file
