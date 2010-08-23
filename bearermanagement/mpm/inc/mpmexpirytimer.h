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
* Description:  Declaration of class CMPMExpiryTimer.
*
*/

#ifndef MPMEXPIRYTIMER_H
#define MPMEXPIRYTIMER_H

// INCLUDES
#include <e32base.h>  
#include <e32std.h>		

#include "mpmexpirytimercallback.h"

/**
* Class that implements simple microsecond timer and callback.
* @since 5.2
*/
NONSHARABLE_CLASS( CMPMExpiryTimer ): public CTimer
    {
public:

    /**
    * New for calling the two-phased constructor.
    * @since 5.2
    * @param aCallback Pointer to the class where callback goes    
    * @param aTimeout Timeout value in microseconds
    * @return Pointer to created object instance
    */
    static CMPMExpiryTimer* NewL( MMPMExpiryTimerCallback& aCallback,
            TInt aTimeout);  
    
    /**
    * Starts up the timer.
    * @since 5.2
    */
    void Start();
    
private:
    /**
    * C++ constructor.
    * @since 5.2
    * @param aCallback Pointer to the class where callback goes
    * @param aTimer Timeout value in microseconds
    */
    CMPMExpiryTimer( MMPMExpiryTimerCallback& aCallback,
            TInt aTimeout );

    /**
    * ConstructL.       
    * @since 5.2
    */
    void ConstructL();

    /**
    * RunL. Inherited from CTimer.       
    * @since 5.2
    */
    void RunL();

private:
    /**
    * Callback reference.
    */
    MMPMExpiryTimerCallback& iCallback;
    
    /**
    * Timeout value in microseconds.
    */
    TInt iTimeout;
    };

#endif // MPMEXPIRYTIMER_H
