/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CExpiryTimer 
*
*/


#ifndef CEXPIRYTIMER_H
#define CEXPIRYTIMER_H

// INCLUDES
#include <e32base.h>  
#include <e32std.h>		

#include "ExpiryTimerCallback.h"

// CLASS DECLARATION
/**
 * Class that implements simple n second timer and callback
 */
NONSHARABLE_CLASS( CExpiryTimer ): public CTimer
    {
public:

    /**
    * Two-phased constructor.
    * @param aCallback Pointer to the class where callback goes    
    */
    static CExpiryTimer* NewL( MExpiryTimerCallback& aCallback );  
    
    /**
    * Starts up the timer       
    */
    void Start();

private:
    /**
    * C++ constructor.
    * @param aCallback Pointer to the class where callback goes    
    */
    CExpiryTimer( MExpiryTimerCallback& aCallback );

    /**
    * ConstructL.       
    */
    void ConstructL();

    /**
    * RunL. From CTimer.       
    */
    void RunL();

private:
    /**
   * Callback reference
   */
    MExpiryTimerCallback& iCallback;
    };

#endif // CEXPIRYTIMER_H
