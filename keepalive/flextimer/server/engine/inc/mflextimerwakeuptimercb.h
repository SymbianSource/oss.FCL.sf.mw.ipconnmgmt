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
 * This class contains definition of MFlexTimerWakeUpTimerCB
 *
 */

// Protection against nested includes
#ifndef MFLEXTIMERWAKEUPTIMERCB_H
#define MFLEXTIMERWAKEUPTIMERCB_H

// Class declaration
/**
 *  Callback functions for FlexTimer engine wake up timers. If a client
 *  wants to receive indicaitions from wake-up timer, it need to inherit
 *  this interface and implement the callback methods.
 */
class MFlexTimerWakeUpTimerCB
    {
public:

    /**
     * This function is used to inform about timer expiry.
     */
    virtual void WakeUp() = 0;

    /** Pure virtual functions*/
    /**
     * This function is used to inform about system time change.
     */
    virtual void SystemTimeChanged() = 0;
    };

#endif  //MFLEXTIMERWAKEUPTIMERCB_H
