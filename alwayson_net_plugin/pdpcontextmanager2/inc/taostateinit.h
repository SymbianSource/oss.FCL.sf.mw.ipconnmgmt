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
* Description:  TAOStateInit class.
*
*/


#ifndef T_TAOSTATEINIT_H
#define T_TAOSTATEINIT_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
 *  Init state class.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( TAOStateInit ): public TAOState
    {
protected: // Data types
    
    /** Internal state flags */
    enum TInitStateFlag
        {
        ENone                   = 0x00,
        ESettingsReceived       = 0x01,
        ENWRegistrationReceived = 0x02
        };
    
public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateInit( MAOStateContext& aStateContext, 
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
     * Notification from network change is receive through
     * this method.
     *
     * @since S60 v3.1
     * @param aNetworkType: New network type
     */    
    TAOState* HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType aNetworkType );
        
    /**
     * From TAOState.
     * Notifications from new settings is received through
     * this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    TAOState* HandleSettingsReceivedL();
    
    // These all must be ignored in init state.
    // Though in real device these are impossible to receive.

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextActivatedL();
        
    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextDisconnectedL( TInt /*aReason*/ );

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextTemporarilyBlockedL();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleEnableAlwaysOnL();

    /**
     * From TAOState.
     * This is ignored in init state. Always returns NULL.
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

#endif // T_TAOSTATEINIT_H
