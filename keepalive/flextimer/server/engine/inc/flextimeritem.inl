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
 * This file contains inline functions of FlexTimerItem class.
 *
 */

// Protection against nested includes
#ifndef FLEXTIMERITEM_INL
#define FLEXTIMERITEM_INL

// ---------------------------------------------------------------------------
// GetMinAbsoluteTime
// ---------------------------------------------------------------------------
//
inline void CFlexTimerItem::GetMinAbsoluteTime( TTime& aMinAbsoluteTime )
    {
    aMinAbsoluteTime = iAbsoluteMinTime;
    }
// ---------------------------------------------------------------------------
// GetMaxAbsoluteTime
// ---------------------------------------------------------------------------
//
inline void CFlexTimerItem::GetMaxAbsoluteTime( TTime& aMaxAbsoluteTime )
    {
    aMaxAbsoluteTime = iAbsoluteMaxTime;
    }
// ---------------------------------------------------------------------------
// IsAbortedAtSystemTimeChange
// ---------------------------------------------------------------------------
//
inline TBool CFlexTimerItem::IsAbortedAtSystemTimeChange()
    {
    return iAbortAtSystemTimeChange;
    }

#endif  //FLEXTIMERITEM_INL
