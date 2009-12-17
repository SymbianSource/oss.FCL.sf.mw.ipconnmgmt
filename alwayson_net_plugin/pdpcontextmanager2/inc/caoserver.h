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
* Description:  Implements the CAlwaysOnlineEComInterface interface
*
*/


#ifndef C_CAOSERVER_H
#define C_CAOSERVER_H

// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>

#include "alwaysonconfig.hrh"

#include <rmmcustomapi.h>

#include "maostatecontext.h"
#include "maostatepool.h"
#include "maoconnectionmanagerobserver.h"
#include "taostate.h"
#include "maotimerobserver.h"
#include "maosettingsobserver.h"
#include "maoraumanagerobserver.h"
#include "maogpdsobserver.h"
#include "caoasyncwrapper.h"

// FORWARD DECLARATIONS
class CAOConnectionManager;
class CAOTimer;
class CAORAUManager;
class CAOSettings;
class CAOGpds;

/**
*  Always-On Server class.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAOServer ):
    public CBase,
    public MAOStateContext,
    public MAOStatePool,
    public MAOConnectionManagerObserver,
    public MAOTimerObserver,
    public MAOSettingsObserver,
    public MAORAUManagerObserver
    {
public:    // Constructors & destructors

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     */
    static CAOServer* NewL();

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOServer();
    
private: // New methods

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     */
    CAOServer();

    /**
     * Default Symbian second-phase constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

private: // New methods

    /**
     * Setup Always-On server
     *
     * @since S60 v3.1
     */
    void Setup();
    
    /**
     * Reactivation call back
     *
     * @since S60 v3.1
     * @param aSelf the caller
     */
    static TInt ReactivationCallBack( TAny* aSelf );
    
    /**
     * Handles reactivation
     *
     * @since S60 v3.1
     */
    void HandleReactivationL();
    
    /**
     * Does re-activation
     *
     * @since S60 v3.1
     * @param aReason reason of the failure
     */
    void DoReactivation( MAOConnectionManager::TFailureReason aReason );
    
    /**
     * PDP property observer
     *
     * @since S60 v3.1
     */
    void IssuePDPPropertySubscription();

    /**
     * PDP property observer
     *
     * @since S60 v3.1
     * @param aStatus TRequestStatus for asynchronus call 
     */
    void PDPPropertySubscriptionIssueRequest( TRequestStatus& aStatus );

    /**
     * PDP property observer
     *
     * @since S60 v3.1
     * @param aStatus result status of the returned asynchronus call
     */
    void PDPPropertySubscriptionRunL( TInt aStatus );

    /**
     * PDP property observer
     *
     * @since S60 v3.1
     */
    void PDPPropertySubscriptionDoCancel();
    
    /**
     * Releases PDP P&S property
     *
     * @since S60 v3.1
     */
    void ReleasePDPProperty();
    
    /**
     * Creates PDP preoperty and subscribes it
     *
     * @since S60 v3.1
     */
    void CreatePDPPropertyAndSubscribeL();
    
    /**
     * Setup call back
     *
     * @since S60 v3.1
     * @param aSelf the caller
     */
    static TInt SetupCallBack( TAny* aSelf );
    
    /**
     * Handles setup call back
     *
     * @since S60 v3.1
     */
    void HandleSetup();


    /**
     * Creates the StatePool and fills it with the States.
     *
     * @since S60 v3.2
     */
    void InitStatePoolL();


    /**
     * Configures Custom API
     *
     * @since S60 v3.1
     */
    void ConfigureCustomAPIL();


private: // From base class MAOStateContext

    /**
     * From MAOStateContext
     * Returns connection manager interface.
     *
     * @since S60 v3.1
     */
    MAOConnectionManager& ConnectionManager() const;

    /**
     * From MAOStateContext
     * Returns retry timer interface.
     *
     * @since S60 v3.1
     */
    MAOTimer& Timer() const;
    
    /**
     * From MAOStateContext
     * Returns RAU manager interface.
     *
     * @since S60 v3.1
     */
    MAORAUManager& RAUManager() const;
    
    /**
     * From MAOStateContext
     * Returns settings interface.
     *
     * @since S60 v3.1
     */
    MAOSettings& Settings() const;
    
    /**
     * From MAOStateContext
     * Returns GPDS interface.
     *
     * @since S60 v3.1
     */
    MAOGpds& Gpds() const;
    
private: // From base class MAOStatePool

    /**
     * From MAOStatePool
     * Changes to defined state.
     * New state is returned as a parameter.
     *
     * @since S60 v3.1
     * @param aState: New state
     */
    TAOState* ChangeState( TAOState::TAOStateName aState );
    
    /**
     * From MAOStatePool
     * Returns current state.
     *
     * @since S60 v3.1
     */
    TAOState* CurrentState() const;
    
private: // From base class MAOConnectionManagerObserver

    /**
     * From MAOConnectionManagerObserver
     * Succesful PDP context activation is notified
     * through this method.
     *
     * @since S60 v3.1
     */
    void HandlePDPContextActivatedL();

    /**
     * From MAOConnectionManagerObserver
     * PDP context activation failure is notified through
     * this method.
     *
     * @since S60 v3.1
     * @param aReason: Activation failure reason
     */
    void HandlePDPContextActivationFailedL(
        MAOConnectionManager::TFailureReason aReason );
        
    /**
     * From MAOConnectionManagerObserver
     * PDP context disconnection is notified through
     * this method.
     *
     * @since S60 v3.1
     * @param aReason: Disconnection reason
     */
    void HandlePDPContextDisconnectedL(
        MAOConnectionManager::TFailureReason aReason );
    
    /**
     * From MAOConnectionManagerObserver
     * PDP context temporary data transfer blocking notification
     * is recieved through this method.
     *
     * @since S60 v3.1
     */
    void HandlePDPContextTemporarilyBlockedL();
    
    /**
     * From MAOConnectionManagerObserver
     * Notifies network registration status changes.
     *
     * @since S60 v3.1
     * @param aNetworkRegistration: NW registration
     */
    void HandleNWRegistrationStatusChangedL(
        CTelephony::TRegistrationStatus aNetworkRegistration );
        
    /**
     * From MAOConnectionManagerObserver
     * Notifies from a deleted connection.
     *
     * @since S60 v3.1
     * @param aMyConnectionId connection id from connection monitor
     * @param aForward Tells whether message should be forwarded next layer
     */
    void HandleConnectionDeletedL( const TUint aConnectionId,
                                   const TBool aForward);
    
     /**
     * From MAOConnectionManagerObserver
     * ?
     *
     * @since S60 v3.1
     */
    void HandleExternalConnectionCreatedL();
    
    
    /**
     * From MAOConnectionManagerObserver
     *
     * @since S60 v3.2
     * @param aMyConnectionId connection id from connection monitor
     * @param aIapId iap id
     * @param anetId network id
     * @return void
     */
    void HandleInternalConnectionCreatedL( const TUint aConnectionId,
                                           const TUint aIapId, 
                                           const TUint aNetId );
    
    /**
     * From MAOConnectionManagerObserver
     * Notifies error if network registration status
     * is not received.
     *
     * @since S60 v3.1
     * @param aError: Error code
     */
    void HandleError( TInt aError );
    
private: // From base class MAOTimerObserver

    /**
     * From MAOTimerObserver
     * Handles retry timer expiration.
     *
     * @since S60 v3.1
     */
    void HandleRetryTimerExpiredL();

    /**
     * From MAOTimerObserver
     * Handles connection timer expiration.
     *
     * @since S60 v3.1
     */
    void HandleConnectionTimerExpiredL();

    /**
     * Handles unconnect timer expiration.
     *
     * @since 3.0
     */
    void HandleUnconnectTimerExpiredL();
    
private: // From base class MAOSettingsObserver

    /**
     * From MAOSettingsObserver
     * Method through setting changes are notified.
     *
     * @since S60 v3.1
     */
    void HandleSettingsChangedL();
    
private: // From base class MAORAUManagerObserver

    /**
     * From MAORAUManagerObserver
     * Succesfull RAU is notified through this method.
     *
     * @since S60 v3.1
     * @param aType: RAU type
     */
    void HandleSuccesfulRAUL( TRAUType aType );
    
    
private: // Data

    
    /**
     * Tel server
     */
    RTelServer iTelServer;

    /**
     * Mobile phone
     */
    RMobilePhone iMobilePhone;

    /**
     * Custom API
     */
    RMmCustomAPI iCustomAPI;
    
    
    /** 
     * Connection manager
     * Own
     */ 
    CAOConnectionManager* iConnectionManager;
    
    /** 
     * Retry timer
     * Own
     */ 
    CAOTimer* iTimer;
    
    /** 
     * RAU event manager
     * Own
     */ 
    CAORAUManager* iRAUManager;
    
    /** 
     * Settings
     * Own
     */ 
    CAOSettings* iSettings;
    
    /** 
     * GPDS interface
     * Own
     */ 
    CAOGpds* iGpds;
    
    /** 
     * States array
     */ 
    CArrayPtrFlat<TAOState>* iPointerStatePool;
    
    /** 
     * Async PDP context re-activation timer
     * Own
     */ 
    CPeriodic* iAsyncReactivation;
    
    /** 
     * Current disconnect / failure reason
     */ 
    MAOConnectionManager::TFailureReason iFailure;
    
    /** 
     * Current activation failure
     */ 
    TBool iActivationFailure;
    
    /** 
     * Current state
     * Not own
     */ 
    TAOState* iCurrentState;
    
    /** 
     * PDPProperty
     */ 
    RProperty iPDPProperty;
    
    /** 
     * PDP P&S property observer
     * Own
     */ 
    CAOAsyncWrapper<CAOServer>* iPDPPropertySubscriber;
    
    /** 
     * Timer setup
     * Own
     */ 
    CPeriodic* iAsyncSetup;
    };

#endif //C_CAOSERVER_H
