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
* Description:  TAOStateResetFactorySettings class.
*
*/


#ifndef T_TAOSTATERESETFACTORYSETTINGS_H
#define T_TAOSTATERESETFACTORYSETTINGS_H

// INCLUDE FILES
#include "taostate.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
*  Reset factory settings state class.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( TAOStateResetFactorySettings ): public TAOState
    {
public: // Constructors & destructors
    
    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     */
    TAOStateResetFactorySettings( MAOStateContext& aStateContext,
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
     * Handles reset factory settings notification.
     * If this is overridden it must be remembered that
     * connection MUST be closed after this method has been
     * called. Otherwise factory resetting will fail.
     *
     * @since S60 v3.1
     * @param aReset: ETrue will indicate reset, EFalse completion of reset
     */
    virtual TAOState* HandleResetFactorySettingsL( TBool aReset );
    
    // These all must be ignored in reset factory settings state.
    // Though in real device these are impossible to receive.

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivatedL();
            
    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextActivationFailedL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextDisconnectedL( TInt aReason );

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType /*aNetworkType*/ );

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSuccesfulRAUEventL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleRetryTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionTimerTriggeredL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSettingsReceivedL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandlePDPContextTemporarilyBlockedL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleGeneralError();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleEnableAlwaysOnL();

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason aReason );

    /**
     * From TAOState.
     * This is ignored in reset factory setting state. Always returns NULL.
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleConnectionDeletedL();
    
    /**
     * From TAOState.
     * ?
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleExternalConnectionCreatedL();
    
    /**
     * From TAOState.
     * ?
     *
     * @since S60 v3.1
     */
    virtual TAOState* HandleUnconnectTimerExpiredL();

    };

#endif // T_TAOSTATERESETFACTORYSETTINGS_H
