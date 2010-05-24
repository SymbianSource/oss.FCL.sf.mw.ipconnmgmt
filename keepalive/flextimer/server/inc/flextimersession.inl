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
 * This file contains inline functions of FlexTimerSession class.
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERSESSION_INL
#define FLEXTIMERSESSION_INL

// --------------------------------------------------------------------------
// Function for calculating interval from current time to timestamp
// --------------------------------------------------------------------------
//
inline void CFlexTimerSession::GetIntervalToMsgTime(
        const RMessage2& aMessage,
        TTime& aRefTime,
        TTimeIntervalMicroSeconds& aInterval )
    {
    // Construct requested timestamp from the message
    TInt timeLo( aMessage.Int0() );
    TInt timeHi( aMessage.Int1() );
    TInt64 timeStamp = MAKE_TINT64( timeHi, timeLo );
    TTime reqTime( timeStamp );

    // Calculate the difference between current time and requested time
    // Reference time used by pure At()-timer is HomeTime
    aInterval = reqTime.MicroSecondsFrom( aRefTime );
    }

#endif  /* FLEXTIMERSESSION_INL */
