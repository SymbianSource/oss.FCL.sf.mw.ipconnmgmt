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
 * This file contains inline functions of FlexTimerContainer class.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERCONTAINER_INL
#define FLEXTIMERCONTAINER_INL

// ---------------------------------------------------------------------------
// Converts micro seconds interval to tick based absolute time (also in micro
// seconds). All flextimer item time handling in engine is based on this tick
// based absolute time. Note that the time is not using system time base.
// ---------------------------------------------------------------------------
//
inline TInt64 CFlexTimerContainer::IntervalToAbsoluteTime(
    const TTimeIntervalMicroSeconds& aInterval )
    {
    TTime tempTime;
    GetCurrentTime( tempTime );

    return tempTime.Int64() + aInterval.Int64();
    }
// ---------------------------------------------------------------------------
// Converts system ticks to microseconds. 
// ---------------------------------------------------------------------------
//
inline TInt64 CFlexTimerContainer::TicksToAbsoluteTime( const TUint32 aTicks )
    {
    return static_cast<TInt64> ( aTicks )
        * static_cast<TInt64> ( iTickPeriod );
    }

#endif  //FLEXTIMERCONTAINER_INL
