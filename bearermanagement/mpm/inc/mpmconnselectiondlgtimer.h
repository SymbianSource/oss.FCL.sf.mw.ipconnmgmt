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
* Description: Declaration of class CMPMConnSelectionDlgTimer.
*
*/

#ifndef MPMCONNSELECTIONDLGTIMER_H
#define MPMCONNSELECTIONDLGTIMER_H

//  INCLUDES
#include <e32base.h>
#include "mpmexpirytimercallback.h"

class CMPMServer;
class CMPMExpiryTimer;

/**
* Class that implements connection selection dialog's
* cancellation delay timer and callback.
* @since 5.2
*/
class CMPMConnSelectionDlgTimer : public CBase, public MMPMExpiryTimerCallback
    {

public:

    /**
    * New for calling the two-phased constructor.
    * @since 5.2
    * @param aServer Pointer to the MPM server object, for callback purposes    
    * @return Pointer to created object instance
    */
    static CMPMConnSelectionDlgTimer* NewL( CMPMServer* aServer );

    /**
    * Destructor.
    * @since 5.2
    */
    virtual ~CMPMConnSelectionDlgTimer();

    /**
    * Start the timer.
    * @since 5.2
    */
    void StartTimer();

    /**
    * Dialog expiration timeout callback, inherited from MMPMExpiryTimerCallback.
    * @since 5.2
    */
    void HandleTimedOut();
    
private:

    /**
    * C++ default constructor.
    * @since 5.2
    * @param aServer Pointer to the MPM server object, for callback purposes    
    */
    CMPMConnSelectionDlgTimer( CMPMServer* aServer );

    /**
    * Symbian 2nd phase constructor.
    * @since 5.2
    */
    void ConstructL();


private: // data

    /**
    * Pointer to the MPM Server object. Not own.
    */
    CMPMServer* iServer;
    
    /**
    * Pointer to the common MPM expiry timer object.
    */
    CMPMExpiryTimer* iExpiryTimer;

    /**
    * Timeout constant.
    */
    static const TInt KTimeout = 10000000; // 10 sec

    };

#endif // MPMCONNSELECTIONDLGTIMER_H
