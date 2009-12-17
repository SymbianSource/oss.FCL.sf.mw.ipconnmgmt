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
* Description:  TAOStateConnecting class.
*
*/



#ifndef T_TAOSTATECONNECTING_H
#define T_TAOSTATECONNECTING_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
 *  Connecting state class.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( TAOStateConnecting ): public TAOState
    {
    public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateConnecting( MAOStateContext& aStateContext, 
                        MAOStatePool& aStatePool );
        
    protected: 
    
// From base class TAOState
    
    /**
     * From TAOState.
     * Returns state name (ID).
     *
     * @since S60 v3.1
     */
    virtual TAOStateName StateName() const;
    
    /**
     * From TAOState.
     * Notification from PDP context activation is received
     * through this method.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivatedL();
    
    /**
     * From TAOState.
     * Notification from PDP context activation failure
     * is received through this method.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivationFailedL();
    
    /**
     * From TAOState.
     * Notification from retry timer triggering
     * is received through this method.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionTimerTriggeredL();
    
    /**
     * From TAOState.
     * Handles connection deleted notification.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionDeletedL();
    
    // These are ignored in connecting state
    
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextDisconnectedL( TInt /*aReason*/ );
        
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleRetryTimerTriggeredL();
    
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextTemporarilyBlockedL();
    
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleGeneralError();
    
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleEnableAlwaysOnL();
    
    /**
     * From TAOState.
     * This is ignored in connecting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ );
        
        
    private:
    
    /**
     * Own: Connection counter
     */
    TInt iConnectionCounter;    
    };

#endif // T_TAOSTATECONNECTING_H
