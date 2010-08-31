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
 * This class contains implementation of CFlexTimerItem.
 *
 */

// System include files
// None

// User include files go here:
#include "flextimeritem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimeritemTraces.h"
#endif

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CFlexTimerItem::CFlexTimerItem( const TTime& aAbsoluteMinTime,
    const TTime& aAbsoluteMaxTime,
    TBool aCancelAtSystemTimeChange,
    const MFlexTimerServiceCB* aFlexTimerServiceCB ) :
        iAbortAtSystemTimeChange( aCancelAtSystemTimeChange ),
        iAbsoluteMinTime( aAbsoluteMinTime ),
        iAbsoluteMaxTime( aAbsoluteMaxTime ),
        iMFlexTimerServiceCB( aFlexTimerServiceCB )
    {
    OstTraceExt5( TRACE_INTERNAL,
        CFLEXTIMERITEM_CFLEXTIMERITEM,
        "CFlexTimerItem::CFlexTimerItem;this=%x;aAbsoluteMinTime=%Ld;"
        "aAbsoluteMaxTime=%Ld;aCancelAtSystemTimeChange=%d;"
        "aFlexTimerServiceCB=%x",
        ( TUint )this,
        aAbsoluteMinTime.Int64(),
        aAbsoluteMaxTime.Int64(),
        aCancelAtSystemTimeChange,
        ( TUint )aFlexTimerServiceCB );

    // Nothing to do - Coverity warning of uninitialize iLink.Next suppressed,
    // because this should be initialized by TSglQueLink constructor.
    // coverity[uninit_member]
    }
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFlexTimerItem* CFlexTimerItem::NewL( const TTime& aAbsoluteMinTime,
    const TTime& aAbsoluteMaxTime,
    TBool aCancelAtSystemTimeChange,
    const MFlexTimerServiceCB* aFlexTimerServiceCB )
    {
    CFlexTimerItem* self = new (ELeave) CFlexTimerItem( aAbsoluteMinTime,
        aAbsoluteMaxTime,
        aCancelAtSystemTimeChange,
        aFlexTimerServiceCB );

    return self;
    }

// ---------------------------------------------------------------------------
// destructor, nothing to destruct
// ---------------------------------------------------------------------------
//
CFlexTimerItem::~CFlexTimerItem()
    {
        OstTraceExt2( TRACE_INTERNAL,
            DUP1_CFLEXTIMERITEM_CFLEXTIMERITEM,
            "CFlexTimerItem::~CFlexTimerItem;this=%x;iMFlexTimerServiceCB=%x",
            ( TUint )this,
            ( TUint )iMFlexTimerServiceCB );
    //Nothing to do here
    }
// ---------------------------------------------------------------------------
// GetCB
// ---------------------------------------------------------------------------
//
const MFlexTimerServiceCB* CFlexTimerItem::GetCB()
    {
    return iMFlexTimerServiceCB;
    }
