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
* Description:  TAOStateDisconnected class.
*
*/


#ifndef T_TAOSTATEDISCONNECTED_H
#define T_TAOSTATEDISCONNECTED_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
 *  Disconnected state class.
 *  Disconnected state is only a temporary state where Always On
 *  changes right away to a different state.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( TAOStateDisconnected ): public TAOState
    {
    public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateDisconnected( MAOStateContext& aStateContext, 
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
     * Changes state from disconnected state.
     *
     * @since S60 v3.1
     * @param aReason: Disconnect reason
     */
    virtual TAOState* HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason aReason );
        
    /**
     * From TAOState.
     * Handles connection deleted notification.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionDeletedL();
    
    
    // These all must be ignored in disconnected state.

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivatedL();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextDisconnectedL( TInt /*aReason*/ );

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextTemporarilyBlockedL();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in disconnected state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleEnableAlwaysOnL();
                
    protected: // Data
    };

#endif // T_TAOSTATEDISCONNECTED_H
