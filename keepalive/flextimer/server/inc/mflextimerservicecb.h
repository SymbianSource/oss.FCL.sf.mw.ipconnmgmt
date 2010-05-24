/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * This class contains definition of MFlexTimerServiceCB
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef MFLEXTIMERSERVICECB_H
#define MFLEXTIMERSERVICECB_H

// Class declaration
/**
 *  Callback interface for Flexible timer engine to inform a session
 *  about timeouts and aborts.
 */
class MFlexTimerServiceCB
    {
public:

    /** Pure virtual functions*/
    /**
     * Function used for inform that a timeout has happened and client
     * should be informed about it. After this function is called, there will
     * no longer be indications related to this timer. After this function
     * returns, the timer object will be deleted.
     * 
     */
    virtual void Timeout() const = 0;

    /**
     * Function for informing client session that something has happended and
     * timer should be aborted. After this function is called, there will
     * no longer be indications related to this timer. After this function
     * returns, the timer object will be deleted.
     * 
     * @param aReason Error code specifying why timer is aborted.
     * 
     * @see RFlexTimer
     *
     */
    virtual void Abort( TInt aReason ) const = 0;

    };

#endif  /* MFLEXTIMERSERVICECB_H */
