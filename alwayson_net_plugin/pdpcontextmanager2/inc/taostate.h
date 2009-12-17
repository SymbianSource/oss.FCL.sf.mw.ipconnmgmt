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
* Description:  State base class.
*
*/



#ifndef T_TAOSTATE_H
#define T_TAOSTATE_H

// INCLUDE FILES
#include <e32std.h>

#include "maoconnectionmanager.h"

// FORWARD DECLARATIONS
class MAOStateContext;
class MAOStatePool;

// CLASS DESCRIPTION

/**
 *  State base class.
 *  New states should derive directly from this.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( TAOState )
    {
public: // Data types

    /**  State name enums */
    enum TAOStateName
        {
        EStateInit = 0,
        EStateStarting,
        EStateDisabled,
        EStateConnecting,
        EStateConnected,
        EStateDisconnected,
        EStateWaiting,
        EStateUnconnected,
        EStateResetFactorySettings,
        EStateExternalConnection,
        EStateLastItem  // this item used for indicating 
                        // the number of members of this enum.
                        // It should be always the last even if the 
                        // enum extracted.
        };

public: // Constructors & destructors

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aStateContext Reference to MAOStateContext
     * @param aStatePool Reference to MAOStatePool
     * @return Pointer to created object.
     */
    TAOState( MAOStateContext& aStateContext, MAOStatePool& aStatePool );
    
public: // New methods

    /**
     * Returns state name (ID).
     *
     * @since S60 v3.1
     * @return TAOStateName
     */
    virtual TAOStateName StateName() const = 0;

    /**
     * Notification from PDP context activation is received
     * through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandlePDPContextActivatedL();
    
    /**
     * Notification from PDP context activation failure
     * is received through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandlePDPContextActivationFailedL();
    
    /**
     * Notification from PDP context disconnect is received
     * through this method.
     *
     * @since S60 v3.1
     * @param aReason: Disconnect reason
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandlePDPContextDisconnectedL( TInt aReason );
    
    /**
     * Notification from network change is receive through
     * this method.
     *
     * @since S60 v3.1
     * @param aNetworkType: New network type
     * @return New state or NULL if state is not
     *         needed to ba changed.
     */
    virtual TAOState* HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType aNetworkType );
    
    /**
     * Notification from succesful RAU is received
     * through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to ba changed.
     */
    virtual TAOState* HandleSuccesfulRAUEventL();

    /**
     * Notification from retry timer triggering
     * is received through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to ba changed.
     */
    virtual TAOState* HandleRetryTimerTriggeredL();
        
    /**
     * Notification from connection timer triggering
     * is received through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to ba changed.
     */
    virtual TAOState* HandleConnectionTimerTriggeredL();
    
    /**
     * Notifications from new settings is received through
     * this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleSettingsReceivedL();
    
    /**
     * Notifications from PDP context data transfer block notification
     * received through this method.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandlePDPContextTemporarilyBlockedL();
    
    /**
     * Handles error. Always leads to disconnected state.
     *
     * @since S60 v3.1
     * @return TAOStateDisconnected*
     */
    virtual TAOState* HandleGeneralError();
    
    /**
     * Handles enable always on request.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleEnableAlwaysOnL();
    
    /**
     * Changes state from disconnected state.
     *
     * @since S60 v3.1
     * @param aReason: Disconnect reason
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason aReason );
        
    /**
     * Handles connection deleted notification.
     *
     * @since S60 v3.1
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleConnectionDeletedL();
    
    /**
     * Handles reset factory settings notification.
     * If this is overridden it must be remembered that
     * connection MUST be closed after this method has been
     * called. Otherwise factory resetting will fail.
     *
     * @since S60 v3.1
     * @param aReset: ETrue will indicate reset, EFalse completion of reset
     * @return New state or NULL if state is not
     *         needed to be changed.
     */
    virtual TAOState* HandleResetFactorySettingsL( TBool aReset );
        
    /**
     * Handles external connection creation notification.
     *
     * @since 3.0
     */
    virtual TAOState* HandleExternalConnectionCreatedL();
        
    /**
     * Handles unconnect timer trigger notification.
     *
     * @since 3.0
     */
    virtual TAOState* HandleUnconnectTimerExpiredL();
        
protected: // New methods

    /**
     * Cancels all operations        
     *
     * @since S60 v3.1
     */
    void CancelAll();
    
    /**
     * Setting a flag
     *
     * @since S60 v3.1
     * @param aFlag flag to set 
     */
    inline void SetFlag( TUint aFlag );
    
    /**
     * Clears a flag
     *
     * @since S60 v3.1
     * @param aFlag flag to clear 
     */
    inline void ClearFlag( TUint aFlag );
    
    /**
     * Checks if flag is set
     *
     * @since S60 v3.1
     * @param aFlag flag to check
     * @return ETrue if the checked flag is on else EFalse
     */
    inline TBool CheckFlag( TUint aFlag );
    
    /**
     * Clears all flags
     *
     * @since S60 v3.1
     */
    inline void ClearFlags();
    
    /**
     * Checks if Always On should be enabled
     *
     * @since S60 v3.1
     * @return ETrue if Always On should be enabled else EFalse
     */
    TBool EnableAlwaysOn();
    
    /**
     * Cancels all timers
     *
     * @since S60 v3.1
     */
    void CancelTimers();
    
    /**
     * Sets always on mode to TSY
     *
     * @since S60 v3.1
     */
    void SetAlwaysOnModeL();
    
    /**
     * Basic handling for connection deletion
     *
     * @since S60 v3.1
     * @return TAOState*
     */
    TAOState* DoHandleConnectionDeletedL();
    
    /**
     * Activates PDP context
     *
     * @since S60 v3.1
     * @return TAOState*
     */
    TAOState* DoActivatePDPContextL();
    

protected: // data

    /**
     * Ref: State context
     */
    MAOStateContext& iStateContext;
    
    /**
     * Ref: State pool
     */
    MAOStatePool& iStatePool;
    
    /**
     * Internal state flag
     */
    TUint iFlags;
    };
    
// inline methods    
#include "../inl/taostate.inl"

#endif // T_TAOSTATE_H
