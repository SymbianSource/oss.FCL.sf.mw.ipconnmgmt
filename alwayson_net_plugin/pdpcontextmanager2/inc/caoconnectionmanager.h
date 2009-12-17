/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?
*
*/


#ifndef CAOCONNECTIONMANAGER_H
#define CAOCONNECTIONMANAGER_H

// INCLUDE FILES
#include <e32base.h>

#include "maoconnectionmanager.h"
#include "maoconnectionobserver.h"
#include "maoconnectionmonitorobserver.h"
#include "linger.h"

// FORWARD DECLARATIONS
class MAOConnectionManagerObserver;
class MAOSettings;
class CAOConnection;
class CAOConnectionMonitor;

/**
 *  Connection manager class.
 *  Connection manager is responsible for creating new
 *  connections and monitoring existing ones.
 *  Network registration is also fetchable through Connection manager.
 *
 *  Events are notified through MAOConnectionManagerObserver
 *  interface.
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CAOConnectionManager ):
    public CBase,
    public MAOConnectionManager,
    public MAOConnectionObserver,
    public MAOConnectionMonitorObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     */
    static CAOConnectionManager* NewL(
        MAOConnectionManagerObserver& aObserver,
        MAOSettings& aSettings );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOConnectionManager();
    
    /**
     * Fetches network registration status.
     * Asynchronous.
     *
     * @since S60 v3.1
     */
    void FetchNetworkRegistrationStatus();
    
// From base class MAOConnectionManager
// Allow Always-On Server to access these methods

    /**
     * From MAOConnectionManager
     *
     * @since S60 v3.1
     */ 
    void ActivatePDPContextL();
    
    /**
     * From MAOConnectionManager
     *
     * @since S60 v3.1
     * @return ETrue if PDP context is active
     */
    TBool IsPDPContextActive() const;

    /**
     * From MAOConnectionManager
     *
     * @since S60 v3.1
     */
    void CloseConnection();
    
     /**
     * From MAOConnectionManager
     *
     * @since S60 v3.2
     */
    void DetachConnection();
       
    /**
     * From MAOConnectionManager
     *
     * @since S60 v3.1
     */
    TNetworkType NetworkType() const;

    /**
     * From MAOConnectionManager
     *
     * @since S60 v3.1
     */
    TInt NumberOfConnections();
    
     /**
     * From MAOConnectionManager
     *
     * @since S60 v3.2
     */
    void HandleSettingsChangedL();
        
private:

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     */
    CAOConnectionManager(
        MAOConnectionManagerObserver& aObserver,
        MAOSettings& aSettings );

    /**
     * Default Symbian constructor
     *
     * @since S60 v3.1
     * @param aSettings
     */
    void ConstructL( MAOSettings& aSettings );
    
    /**
     * Checks if network status and settings support always-on
     *
     * @since S60 v3.2
     * @return ETrue if always-on is enabled, EFalse otherwise.
     */
    TBool IsAlwaysOnEnabled();
    
// From base class MAOConnectionObserver

    /**
     * From MAOConnectionObserver
     *
     * @since S60 v3.1
     */
    void HandlePDPContextActivatedL();

    /**
     * From MAOConnectionObserver
     *
     * @since S60 v3.1
     */
    void HandlePDPContextActivationFailedL(
        MAOConnectionManager::TFailureReason aReason );
        
    /**
     * From MAOConnectionObserver
     *
     * @since S60 v3.1
     */
    void HandlePDPContextDisconnectedL(
        MAOConnectionManager::TFailureReason aReason );
    
    /**
     * From MAOConnectionObserver
     *
     * @since S60 v3.1
     */
    void HandlePDPContextTemporarilyBlockedL();
    
// From base class MAOConnectionMonitorObserver

    /**
     * From MAOConnectionMonitorObserver
     *
     * @since S60 v3.1
     * @param aNetworkRegistration ?
     */
    void HandleNWRegistrationStatusChangedL(
        CTelephony::TRegistrationStatus aNetworkRegistration );
                
    /**
     * From MAOConnectionMonitorObserver
     *
     * @since S60 v3.1
     * @param aConnectionId connection id from connection monitor
     * @param aForward Tells whether message should be forwarded next layer
     */
    void HandleConnectionDeletedL( const TUint aConnectionId,
                                   const TBool aForward );
    
    /**
     * Notifies that external connection has been created.
     *
     * @since 3.0
     */
    void HandleExternalConnectionCreatedL();
    
    /**
     * Notifies that internal not-always-on connection has been created.
     *
     * @since S60 v3.2
     * @param aConnectionId connection id from connection monitor
     * @param aIapId iap id
     * @param anetId network id
     * @return void
     */
    void HandleInternalConnectionCreatedL( const TUint aConnectionId,
                                           const TUint aIapId, 
                                           const TUint aNetId );  
    
    /**
     * From MAOConnectionMonitorObserver
     *
     * @since S60 v3.1
     */
    void HandleError( TInt aError );
  
private: // Data

    /**
     * Ref: Connection manager observer
     */
    MAOConnectionManagerObserver& iObserver;
    
    /**
     * Ref: Settings
     */
    MAOSettings& iSettings;
    
    /**
     * Own: Socket server session
     */
    RSocketServ iSocketServ;
    
    /**
     * Own: Connection handler
     */
    CAOConnection* iConnection;
    
    /**
     * Own: Connection monitor
     */
    CAOConnectionMonitor* iConnectionMonitor;
    
    /**
     * Array of pointers to linger objects
     */
    RArray< CLingerConnection* > iLingerArray;
    };

#endif /* CAOCONNECTIONMANAGER_H */
