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
* Description:  Implements the TAOStateDisabled class.
*
*/


#ifndef T_TAOSTATEDISABLED_H
#define T_TAOSTATEDISABLED_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
*  Disabled state class.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( TAOStateDisabled ): public TAOState
    {
public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateDisabled( MAOStateContext& aStateContext, 
                      MAOStatePool& aStatePool );
     
protected: // From base class TAOState
    
    /**
     * From TAOState.
     * Returns state name (ID).
     *
     * @since S60 v3.1
     */
    virtual TAOStateName StateName() const;
        
    /**
     * From TAOState.    
     * Notification from network change is receive through
     * this method.
     *
     * @since S60 v3.1
     * @param aNetworkType: New network type
     */
    virtual TAOState* HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType aNetworkType );


    // These all must be ignored in disabled state.
 
    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextActivatedL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextDisconnectedL( TInt /*aReason*/ );

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandlePDPContextTemporarilyBlockedL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    TAOState* HandleEnableAlwaysOnL();

    /**
     * From TAOState.
     * This is ignored in disabled state. Always returns NULL.
     *
     * @since S60 v3.1
     * @param aReason ?
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

#endif // T_TAOSTATEDISABLED_H
