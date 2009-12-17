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
* Description:  TAOStateConnected class.
*
*/


#ifndef T_TAOSTATECONNECTED_H
#define T_TAOSTATECONNECTED_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
 *  Connected state class.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( TAOStateConnected ): public TAOState
    {
public:
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateConnected( MAOStateContext& aStateContext, 
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
     * Notification from PDP context disconnect is received
     * through this method.
     *
     * @since S60 v3.1
     * @param aReason: Disconnect reason
     */
    virtual TAOState* HandlePDPContextDisconnectedL( TInt aReason );
        
    /**
     * From TAOState.
     * Handles connection deleted notification.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionDeletedL();
     
    // These are ignored in connected state

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivatedL();
    
    /**
     * From TAOState.
     * 
     *
     * @since S60 v3.2
     * @param aNetworkType: New network type
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType aNetworkType );
    

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextTemporarilyBlockedL();
    
    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleEnableAlwaysOnL();

    /**
     * From TAOState.
     * This is ignored in connected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSwitchFromDisconnectedL(
            MAOConnectionManager::TFailureReason /*aReason*/ );
    };

#endif // T_TAOSTATECONNECTED_H
