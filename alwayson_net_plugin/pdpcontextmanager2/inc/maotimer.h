/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines MAOTimer interface
*
*/


#ifndef M_MAOTIMER_H
#define M_MAOTIMER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
*  Timer interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOTimer
    {
public:

    /**
     * Starts retry timer.
     *
     * @since S60 v3.1
     */
    virtual void StartRetryTimer() = 0;

    /**
     * Starts connection timer.
     *
     * @since S60 v3.1
     */
    virtual void StartConnectionTimer() = 0;
    
    /**
     * Stops retry timer.
     *
     * @since S60 v3.1
     */
    virtual void StopRetryTimer() = 0;
    
    /**
     * Stops connection timer.
     *
     * @since S60 v3.1
     */
    virtual void StopConnectionTimer() = 0;
    
    /**
     * Starts unconnect timer.
     *
     * @since 3.0
     */
    virtual void StartUnconnectTimer() = 0;
    
    /**
     * Stops unconnect timer.
     *
     * @since 3.0
     */
    virtual void StopUnconnectTimer() = 0;
    
    /**
     * Checks if retry timer is set to infinite (disabled).
     *
     * @since S60 v3.1
     * @return ETrue if set to infinite
     */
    virtual TBool RetryTimerDisabled() const = 0;
    
    /**
     * Checks if unconnect timer is set to zero (disabled).
     *
     * @since 3.0
     * @return ETrue if set to infinite
     */
    virtual TBool UnconnectTimerDisabled() const = 0;
        
protected:

    /**
     * Prohibit destruction
     *
     * @since S60 v3.1
     */
    virtual ~MAOTimer(){};
    };

#endif // M_MAOTIMER_H
