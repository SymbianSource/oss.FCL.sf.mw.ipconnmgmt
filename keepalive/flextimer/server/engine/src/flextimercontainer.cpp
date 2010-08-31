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
 * This class contains implementation of CFlexTimerContainer.
 *
 */

// System include files
#include <hal.h>

// User include files
#include "flextimercommon.h"
#include "flextimercontainer.h"
#include "flextimeritem.h"
#include "mflextimerservicecb.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimercontainerTraces.h"
#endif



// This literal is only used by __ASSERT_DEBUG macro. Therefore, to prevent 
// unnacessary warnings, it is not compiled into release builds.
#ifdef _DEBUG
static const TInt KCBNullPointer = 1;
_LIT( KCBNullPointerDescriptor, "FlexTimerService CB is NULL." );
#endif

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFlexTimerContainer* CFlexTimerContainer::NewL()
    {
    CFlexTimerContainer* self = new (ELeave) CFlexTimerContainer();
    
    OstTrace1( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_NEWL,
        "CFlexTimerContainer::NewL;this=%x",
        ( TUint )self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// If the list is not empty when coming here something has already gone wrong.
// Lets just delete the timers and forget Timeout() calling
// ---------------------------------------------------------------------------
//
CFlexTimerContainer::~CFlexTimerContainer()
    {
    CFlexTimerItem* item = NULL;
    
    OstTrace1(
        TRACE_INTERNAL,
        DUP1_CFLEXTIMERCONTAINER_CFLEXTIMERCONTAINER,
        "CFlexTimerContainer::~CFlexTimerContainer;this=%x",
        ( TUint )this );

    while ( !iTimerList.IsEmpty() )
        {
        item = iTimerList.First();
        iTimerList.Remove( *item );
        delete item;
        }
    }
// ---------------------------------------------------------------------------
// Make new timer and add it to list. On purpose do not make sanity checks. 
// Invalid(in the past) timers are found later.
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::AddTimerL(
    const TTimeIntervalMicroSeconds& aWinStartInterval,
    const TTimeIntervalMicroSeconds& aWinEndInterval,
    TBool aCancelAtSystemTimeChange,
    const MFlexTimerServiceCB* aFlexTimerServiceCB )
    {
    OstTraceExt5( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_ADDTIMERL,
        "CFlexTimerContainer::AddTimerL;this=%x;"
        "aWinStartInterval=%Ld;"
        "aWinEndInterval=%Ld;"
        "aCancelAtSystemTimeChange=%u;"
        "aFlexTimerServiceCB=%x",
        ( TUint )this,
        aWinStartInterval.Int64(),
        aWinEndInterval.Int64(),
        aCancelAtSystemTimeChange,
        ( TUint )aFlexTimerServiceCB );
    
    __ASSERT_DEBUG( aFlexTimerServiceCB,
            User::Panic( KCBNullPointerDescriptor, KCBNullPointer ) );

    // Before creating new flextimer timeout item, the interval times are
    // converted to absolute tick based time used by the engine.
    TTime winAbsStart( IntervalToAbsoluteTime( aWinStartInterval ) );
    TTime winAbsEnd( IntervalToAbsoluteTime( aWinEndInterval ) );
    
    CFlexTimerItem* newItem = CFlexTimerItem::NewL( winAbsStart,
        winAbsEnd,
        aCancelAtSystemTimeChange,
        aFlexTimerServiceCB );

    iTimerList.AddLast( *newItem );
    }
// ---------------------------------------------------------------------------
// Loop through list and if timer containing same CB that is given as a
// parameter is found, remove timer. If no timer found just return error code.
// ---------------------------------------------------------------------------
//
TInt CFlexTimerContainer::RemoveTimer(
    const MFlexTimerServiceCB* aFlexTimerServiceCB )
    {
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_REMOVETIMER,
        "CFlexTimerContainer::RemoveTimer;this=%x;aFlexTimerServiceCB=%x",
        ( TUint )this,
        ( TUint )aFlexTimerServiceCB );

    __ASSERT_DEBUG( aFlexTimerServiceCB,
        User::Panic( KCBNullPointerDescriptor, KCBNullPointer ) );
    
    TSglQueIter<CFlexTimerItem> listIter( iTimerList );
    CFlexTimerItem* item = NULL;

    listIter.SetToFirst();
    // Iter++ makes iterator to point to next element if one exists.
    // Otherwise NULL.
    while ( (item = listIter++) != NULL )
        {
        if ( item->GetCB() == aFlexTimerServiceCB )
            {
            iTimerList.Remove( *item );
            delete item;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }
// ---------------------------------------------------------------------------
// Loop through timers and find time/timer that must expire next
// Calculate time left to that moment and return it. 
// ---------------------------------------------------------------------------
//
TBool CFlexTimerContainer::GetNextTimeout(
    TTimeIntervalMicroSeconds& aNextTimeoutDelay )
    {
    TSglQueIter<CFlexTimerItem> listIter( iTimerList );
    TTime currentAbsoluteTime( 0 );
    CFlexTimerItem* item = NULL;
    TTime tempTime( 0 );
    TTime nextTimeout( 0 );

    GetCurrentTime( currentAbsoluteTime );
    // Take first item as a reference value.
    listIter.SetToFirst();
    item = listIter++;
    if ( item )
        {
        item->GetMaxAbsoluteTime( nextTimeout );
        }

    // Find the timer that needs to expire next, and set nextTimeout to the
    // time, when the timer needs to expire.
    while ( (item = listIter++) != NULL )
        {
        item->GetMaxAbsoluteTime( tempTime );
        if ( tempTime < nextTimeout )
            {
            nextTimeout = tempTime;
            }
        }
    // Calculate difference between now -> min timeout time. If in past,
    // return zero interval
    aNextTimeoutDelay = nextTimeout.MicroSecondsFrom( currentAbsoluteTime );
    if ( aNextTimeoutDelay < TTimeIntervalMicroSeconds( 0 ) )
        {
        aNextTimeoutDelay = 0;
        }
    
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_GETNEXTTIMEOUT,
        "CFlexTimerContainer::GetNextTimeout;this=%x;aNextTimeoutDelay=%Ld",
        ( TUint )this,
        aNextTimeoutDelay.Int64() );
    
    return !iTimerList.IsEmpty();
    }
// ---------------------------------------------------------------------------
// Excecute selected algorithms and finally fire all timers in candidate list.
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::FireTimers( TFlexTimerAlgorithm aAlgorithmToBeUsed )
    {
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_FIRETIMERS,
        "CFlexTimerContainer::FireTimers;this=%x;aAlgorithmToBeUsed=%x",
        ( TUint )this,
        ( TUint )aAlgorithmToBeUsed );

    TSglQue<CFlexTimerItem> candidateList( _FOFF( CFlexTimerItem, iLink ) );

    TTime currentAbsoluteTime( 0 );

    GetCurrentTime( currentAbsoluteTime );
    
    // Simple algorithm is always executed
    SimpleAlgorithm( candidateList, currentAbsoluteTime );

    if ( EFlexTimerAlgorithmLatestPossible == aAlgorithmToBeUsed )
        {
        LatestPossibleAlgorithm( candidateList );
        }
    ExpireTimers( candidateList );
    }

//
// ---------------------------------------------------------------------------
// Loop through timer list, call abort to timer if it is abortable.
// Then remove timer from list and delete it.
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::AbortTimersDueToTimeChange( TInt aReason )
    {
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_ABORTTIMERSDUETOTIMECHANGE,
        "CFlexTimerContainer::AbortTimersDueToTimeChange;this=%x;aReason=%d",
        ( TUint )this,
        aReason );

    TSglQueIter<CFlexTimerItem> listIter( iTimerList );
    CFlexTimerItem* item = NULL;
    listIter.SetToFirst();
    
    // Go through all timers and check if the timer has 
    // AbortAtSystemTimeChange flag set, if so, abort the timer.
    while ( (item = listIter++) != NULL )
        {
        if ( item->IsAbortedAtSystemTimeChange() )
            {
            item->GetCB()->Abort( aReason );
            iTimerList.Remove( *item );
            delete item;
            }
        }
    }
// ---------------------------------------------------------------------------
// Constructor for CFlexTimerContainer
// ---------------------------------------------------------------------------
//
CFlexTimerContainer::CFlexTimerContainer() :
    iTimerList( _FOFF( CFlexTimerItem, iLink ) ),
    iLastTicks( 0 ),
    iCurrentAbsoluteTime( 0 )    
    {
    OstTrace1( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_CFLEXTIMERCONTAINER,
        "CFlexTimerContainer::CFlexTimerContainer;this=%x",
        ( TUint )this );
    TInt err;
    
    // Get system tick length for converting tics to microseconds.
    err = HAL::Get( HAL::ESystemTickPeriod, iTickPeriod );
    
    __ASSERT_ALWAYS(
        err == KErrNone,
        User::Panic(
            KFlexTimerContainerPanicCat,
            static_cast <TInt> ( EFlexTimerContainerNoTickPeriod ) ) );
    }
// ---------------------------------------------------------------------------
// Loops through timer list and moves all timers that can be fired to
// candidate list. Can be fired means that minimum time is reached. Note that
// if we are late for some reason (very likely if someone has win size zero),
// candidate list can now contain timers that are late (max time passed).
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::SimpleAlgorithm(
    TSglQue<CFlexTimerItem>& aCandidateList,
    TTime& aCurrentTime )
    {
    OstTraceExt3( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_SIMPLEALGORITHM,
        "CFlexTimerContainer::SimpleAlgorithm;"
        "this=%x;aCandidateList=%x;aCurrentTime=%Ld;",
        ( TUint )this,
        ( TUint )&aCandidateList,
        aCurrentTime.Int64() );

    TSglQueIter<CFlexTimerItem> listIter( iTimerList );
    CFlexTimerItem* item = NULL;
    TTime minTime( 0 );

    listIter.SetToFirst();
    // Iter++ makes iterator to point to next element if one exists.
    // Otherwise NULL.
    while ( (item = listIter++) != NULL )
        {
        item->GetMinAbsoluteTime( minTime );
        if ( minTime <= aCurrentTime )
            {
            OstTraceExt2( TRACE_INTERNAL,
                DUP1_CFLEXTIMERCONTAINER_SIMPLEALGORITHM,
                "CFlexTimerContainer::SimpleAlgorithm - Adding item;"
                "this=%x;item=%x",
                ( TUint )this,
                ( TUint )item );
            // Remove from candidate list needs to be done before the
            // item is added to back to iTimerList, because the lists
            // use the same iLink member.
            iTimerList.Remove( *item );
            
            // This timer can be timeouted, add it to timeout candidate list.
            // The list is called candidate list, because the content of the
            // list may be pruned by passing it to other algorithms for furher
            // prosessing.
            aCandidateList.AddLast( *item );
            }
        }
    }

//
// ---------------------------------------------------------------------------
// Loops through dropped list and candidate list and find timers where:
// dropped timers right side of the window is earlier than candidate timers
// right side of the window i.e. Candidate can be fired later with dropped
// timer, without additional timer firings
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::LatestPossibleAlgorithm(
    TSglQue<CFlexTimerItem>& aCandidateList )
    {   
    TSglQueIter<CFlexTimerItem> listIter( iTimerList );
    TSglQueIter<CFlexTimerItem> candidateListIter( aCandidateList );
    CFlexTimerItem* droppedItem = NULL;
    CFlexTimerItem* candidateItem = NULL;
    TTime droppedMaxTime( 0 );
    TTime candidateMaxTime( 0 );
    
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_LATESTPOSSIBLEALGORITHM,
        "CFlexTimerContainer::LatestPossibleAlgorithm;"
        "this=%x;aCandidateList=%x",
        ( TUint )this,
        ( TUint )&aCandidateList );
    
    listIter.SetToFirst();
    candidateListIter.SetToFirst();
            
    droppedItem = listIter++;
    
    if ( droppedItem != NULL )
        {
        // Initiliaze next dropped timeout time to some value.
        droppedItem->GetMaxAbsoluteTime( droppedMaxTime );

        // Loop through dropped timers and find the dropped timer that will have
        // shortest time to its timeout.
        while ( (droppedItem = listIter++) != NULL )
            {
            droppedItem->GetMaxAbsoluteTime( candidateMaxTime );
            if ( droppedMaxTime > candidateMaxTime )
                {
                droppedMaxTime = candidateMaxTime;
                }
            }

        // Loop through candidate timers
        while ( (candidateItem = candidateListIter++) != NULL )
            {
            candidateItem->GetMaxAbsoluteTime( candidateMaxTime );
            // If candidate can be fired together with dropped timer ->
            // don't fire candidate yet.
            if ( droppedMaxTime <= candidateMaxTime )
                {
                OstTraceExt2( TRACE_INTERNAL,
                    DUP1_CFLEXTIMERCONTAINER_LATESTPOSSIBLEALGORITHM,
                    "CFlexTimerContainer::LatestPossibleAlgorithm -"
                    " Removing item;this=%x;candidateItem=%x",
                    ( TUint )this,
                    ( TUint )candidateItem );

                // Remove from candidate list needs to be done before the
                // item is added to back to iTimerList, because the lists
                // use the same iLink member.
                aCandidateList.Remove( *candidateItem );
                // Add to first so we don't handle this again.
                iTimerList.AddFirst( *candidateItem );
                }
            }
        }
    }
// ---------------------------------------------------------------------------
// Loops through candidate list and calls timeout to all timers.
// Then removes timer from list and deletes it. 
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::ExpireTimers(
    TSglQue<CFlexTimerItem>& aCandidateList )
    {
    OstTraceExt2( TRACE_INTERNAL,
        CFLEXTIMERCONTAINER_EXPIRETIMERS,
        "CFlexTimerContainer::ExpireTimers;this=%x;candidateList=%x",
        ( TUint )this,
        ( TUint )&aCandidateList );

    TSglQueIter<CFlexTimerItem> candidateListIter( aCandidateList );
    CFlexTimerItem* item = NULL;

    candidateListIter.SetToFirst();
    // Iter++ makes iterator to point to next element if one exists.
    // Otherwise NULL.
    while ( (item = candidateListIter++) != NULL )
        {
        item->GetCB()->Timeout();
        aCandidateList.Remove( *item );
        delete item;
        }
    }
// ---------------------------------------------------------------------------
// Get current time return current time in FlexTimer engine time base. This
// time base is begins from the first call to GetCurrentTime and it is base on
// system ticks.
// ---------------------------------------------------------------------------
//
void CFlexTimerContainer::GetCurrentTime( TTime& aAbsoluteTime )
    {
    TUint32 currentTicks = User::TickCount();

    // Accumulate current absolute time with the time passed since last
    // update. Both currentTicks and iLastTicks are unsigned int types, thus
    // "currentTicks - iLastTicks" will handle also the case currentTicks
    // overflows.
    iCurrentAbsoluteTime += TicksToAbsoluteTime( currentTicks
            - iLastTicks );
        
    iLastTicks = currentTicks;
    
    // N.B. Even though the time is is returned as TTime, this time has own
    // time base. (See function description)
    aAbsoluteTime = iCurrentAbsoluteTime;
    }






