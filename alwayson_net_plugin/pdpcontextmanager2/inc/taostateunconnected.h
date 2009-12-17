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
* Description:  TAOStateUnconnected class.
*
*/


#ifndef T_TAOSTATEUNCONNECTED_H
#define T_TAOSTATEUNCONNECTED_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
*  Unconnected state class.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( TAOStateUnconnected ): public TAOState
    {
public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     * @return Pointer to created object.
     */
    TAOStateUnconnected( MAOStateContext& aStateContext, 
                         MAOStatePool& aStatePool );
        
protected: 

// From base class TAOState
    
    /**
     * From TAOState.
     * Returns state name (ID).
     *
     * @since S60 v3.1
     */
    TAOStateName StateName() const;

    /**
     * From TAOState.
     * Handles connection deleted notification.
     *
     * @since S60 v3.1
     */
    TAOState* HandleConnectionDeletedL();
    
    /**
     * Handles unconnect timer trigger notification.
     *
     * @since 3.0
     */
    TAOState* HandleUnconnectTimerExpiredL();
    
    // These must be ignored in unconnected state        
    // @see TAOState
    
    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextActivatedL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextActivationFailedL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextDisconnectedL( TInt /*aReason*/ );

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextTemporarilyBlockedL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleEnableAlwaysOnL();

    /**
     * From TAOState.
     * This is ignored in unconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ );
            
    /**
     * From TAOState.
     * ?
     *
     * @since S60 v3.1
     */        
    TAOState* HandleExternalConnectionCreatedL();

    };

#endif // T_TAOSTATEUNCONNECTED_H
