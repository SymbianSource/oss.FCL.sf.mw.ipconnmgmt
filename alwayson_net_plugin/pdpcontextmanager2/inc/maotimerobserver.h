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
* Description:  Defines MAOTimerObserver interface
*
*/


#ifndef M_MAOTIMEROBSERVER_H
#define M_MAOTIMEROBSERVER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
*  Timer observer interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOTimerObserver
    {
public:

    /**
     * Handles retry timer expiration.
     *
     * @since 3.1
     */
    virtual void HandleRetryTimerExpiredL() = 0;

    /**
     * Handles connection timer expiration.
     *
     * @since 3.1
     */
    virtual void HandleConnectionTimerExpiredL() = 0;
    
    /**
     * Handles unconnect timer expiration.
     *
     * @since 3.0
     */
    virtual void HandleUnconnectTimerExpiredL() = 0;
        
protected:

    /**
     * Prohibit destruction
     *
     * @since 3.1
     */
    virtual ~MAOTimerObserver(){};
    };

#endif // M_MAOTIMEROBSERVER_H
