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
* Description:  RAU manager
*
*/



#ifndef CAOCONNECTIONIMPLSTUB_H
#define CAOCONNECTIONIMPLSTUB_H

// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>
#include "maoconnectionmanager.h"

// FORWARD DECLARATIONS
class MAOSettings;
class MAOConnectionObserver;
class CPeriodic;
template<class T> class CAOAsyncWrapper;

/**
 *  Connection implementation.
 *  This is the stub version of CAOConnectionImpl.
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
class CAOConnectionImpl : public CActive
    {
private: // Data types

    /**
     * Internal state bit
     */
    enum TInternalState
        {
        EDisconnected,
        EOpeningLink,
        EListeningAccessPoint,
        EAttachingExistingConnection
        };
        
public: // Constructors & destructors

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings ?
     * @param aSocketServ ?
     * @return ?
     */
    static CAOConnectionImpl* NewL( 
        MAOConnectionObserver& aObserver,
        MAOSettings&           aSettings,
        RSocketServ*           aSocketServ );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */ 
    virtual ~CAOConnectionImpl();
    
    /**
     * Starts a new connection if none exists.
     *
     * @since S60 v3.1
     */ 
    void ActivateConnection();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aConnectionId ?
     */
    void AttachToExistingConnection( TUint aConnectionId );
    
    /**
     * Cancels any outstanding connections.
     *
     * @since S60 v3.1
     */
    void CancelConnection();
    
    /**
     * Checks if there is already a connection ongoing.
     *
     * @since S60 v3.1
     * @return ETrue if there is connection
     */
    TBool IsConnectedToDefaultIAP();

    /**
     * Returns number of ongoing connections.
     *
     * @since 3.0
     * @return TInt
     */ 
    TInt NumberOfConnections();
            
private:

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     * @param aObserver
     * @param aSettings
     * @param aSocketServ
     */
    CAOConnectionImpl( 
        MAOConnectionObserver& aObserver,
        MAOSettings&           aSettings,
        RSocketServ*           aSocketServ );

    /**
     * Default Symbian constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

    /**
     * Change state
     *
     * @since S60 v3.1
     * @param aNewState ?
     */
    void ChangeState( TInternalState aNewState );
    
    /**
     * Completes self
     *
     * @since S60 v3.1
     * @param aStatus ?
     * @param aReason ?
     */
    void CompleteSelf( TRequestStatus& aStatus, TInt aReason );
    
    /**
     * Callback for connection timer
     *
     * @since S60 v3.1
     * @param aSelf ?
     */
    static TInt ConnectionCallBack( TAny* aSelf );
    
    /**
     * Handles callback timer trigger
     *
     * @since S60 v3.1
     */
    void HandleConnectionCallBackL();
    
    /**
     * Activates connection
     *
     * @since S60 v3.1
     */
    void DoActivateConnection();
    
    /**
     * Handles EOpeningLink request completion
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void HandleOpeningLinkL( TInt aStatus );

    /**
     * Converts error code to failure reason
     *
     * @since S60 v3.1
     * @param aError ?
     * @return ?
     */
    MAOConnectionManager::TFailureReason FailureReason( TInt aError );
    
    /**
     * Handles EListeningAccessPoint request completion
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void HandleListeningAccessPointL( TInt aStatus );
    
    /**
     * Reports PDP context activation error
     *
     * @since S60 v3.1
     * @param aError ?
     */ 
    void BroadcastPDPContextActivationFailureL( TInt aError );
    
    /**
     * Reports PDP context disconnection
     *
     * @since S60 v3.1
     * @param aError ?
     */
    void BroadcastPDPContextDisconnectionL( TInt aError );
    
    /**
     * Handles EAttachingExistingConnection request completion
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void HandleAttachingExistingConnectionL( TInt aStatus );
    
    /**
     * PDP context activation property
     *
     * @since S60 v3.1
     */
    void SubscribePDPContextActivation();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void PDPContextActivationIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void PDPContextActivationRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void PDPContextActivationDoCancel();
    
    /**
     * PDP context disconnection property
     *
     * @since S60 v3.1
     */
    void SubscribePDPContextDisconnection();
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void PDPContextDisconnectionIssueRequest( TRequestStatus& aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void PDPContextDisconnectionRunL( TInt aStatus );
    
    /**
     * ?
     *
     * @since S60 v3.1
     */
    void PDPContextDisconnectionDoCancel();
    
    /**
     * Broadcast PDP context activated
     *
     * @since S60 v3.1
     */
    void BroadcastPDPContextActivatedL();

// From base classes CActive

    /**
     * From CActive
     * ?
     *
     * @since S60 v3.1
     */
    void RunL();
    
    /**
     * From CActive
     * ?
     *
     * @since S60 v3.1
     */
    void DoCancel();
    
    /**
     * From CActive
     * ?
     *
     * @since S60 v3.1
     * @param aError ?
     * @return ?
     */
    TInt RunError( TInt aError );

private: // Data

    /**
     * Ref: Connection observer
     */
    MAOConnectionObserver& iObserver;
    
    /**
     * Ref: Settings
     */
    MAOSettings& iSettings;
    
    /**
     * Not Own: Socket server session
     */
    RSocketServ* iSocketServ;
    
    /**
     * Own: Internal state flag
     */
    TInternalState iState;
    
    /**
     * Own: Connection timer
     */
    CPeriodic* iConnectionTimer;
    
    /**
     * Own: PDP context activation property
     */
    RProperty iPDPContextActivationProperty;
    
    /**
     * Own: ?
     */
    CAOAsyncWrapper<CAOConnectionImpl>* iPDPContextActivationSubscriber;
    
    /**
     * Own: PDP context disconnection property
     */
    RProperty iPDPContextDisconnectionProperty;
    
    /**
     * Own: ?
     */
    CAOAsyncWrapper<CAOConnectionImpl>* iPDPContextDisconnectionSubscriber;
    
    /**
     * Own: PDP context activation error
     */
    TInt iActivationError;

    /**
     * Own: PDP context activation error
     */
    TInt iDisconnectionError;
    };

#endif /* CAOCONNECTIONIMPLSTUB_H */
