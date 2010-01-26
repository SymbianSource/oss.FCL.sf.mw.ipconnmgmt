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


#ifndef CAOCONNECTIONIMPL_H
#define CAOCONNECTIONIMPL_H

// INCLUDE FILES
#include <e32base.h>
#include <commdbconnpref.h>
#include <es_sock.h>
#include <commdb.h>
#include <extendedconnpref.h>
#include "maoconnectionmanager.h"

// FORWARD DECLARATIONS
class MAOSettings;
class CCommsDatabase;
class MAOConnectionObserver;

/**
 *  Connection implementation.
 *  @see CAOConnection
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CAOConnectionImpl ): public CActive
    {
private: // Data types

    /**
     * Internal state bit
     */
    enum TInternalState
        {
        EDisconnected,
        EOpeningLink,
        EListeningAccessPoint
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
    static CAOConnectionImpl* NewL( MAOConnectionObserver& aObserver,
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
    
    /**
     * Closes connection (detaches) without stopping it.
     *
     * @since 3.2
     * @return void
     */ 
    void CloseConnection();
        
private: // New methods

    /**
    * Default constructor.
    * @param aObserver ?
    * @param aSettings ?
    * @param aSocketServ ?
    */
    CAOConnectionImpl( MAOConnectionObserver& aObserver,
                       MAOSettings&           aSettings,
                       RSocketServ*           aSocketServ );

    /**
    * Default Symbian constructor
    */
    void ConstructL();

    /**
     * Checks if current IAP is valid
     *
     * @since S60 v3.1
     * @param aId ?
     * @return ?
     */
    TBool IsValidIAP( TUint32 aId ) const;
    
    /**
     * Complete self
     *
     * @since S60 v3.1
     * @param aStatus ?
     * @param aReason ?
     */     
    void CompleteSelf( TRequestStatus& aStatus, TInt aReason );
    
    /**
     * Handle method for EOpeningLink state
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void HandleOpeningLinkL( TInt aStatus );

    /**
     * Handle method for EListeningAccessPoint state
     *
     * @since S60 v3.1
     * @param aStatus ?
     */
    void HandleListeningAccessPointL( TInt aStatus );
    
    /**
     * Change state
     *
     * @since S60 v3.1
     * @param aNewState ?
     */
    void ChangeState( TInternalState aNewState );
    
    /**
     * Issue progress notification
     *
     * @since S60 v3.1
     */
    void IssueProgressNotification();
    
    /**
     * Sends 'disconnected' error
     *
     * @since S60 v3.1
     * @param aError ?
     */
    void BrodcastDisconnectedError( TInt aError );
    
    /**
     * Sends 'activation failed' error
     *
     * @since S60 v3.1
     * @param aError ?
     */
    void BrodcastActivationFailedError( TInt aError );
    
    /**
     * Converts error code to failure reason
     *
     * @since S60 v3.1
     * @param aError ?
     * @return ?
     */
    MAOConnectionManager::TFailureReason FailureReason( TInt aError );
    
    /**
     * Activates self
     *
     * @since S60 v3.1
     * @param aCode ?
     */
    void ActivateSelf( TInt aCode );
    
    /**
     * Stops connection
     *
     * @since S60 v3.1
     */
    void StopConnection();

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
     * Own: Connection
     */
    RConnection iConnection;
    
    /**
     * Own: Progress notification buffer
     */
    TNifProgressBuf iProgressBuf;
    
    /**
     * Own: Internal state flag
     */
    TInternalState iState;
    
    /**
     * Own: Comms database
     */
    CCommsDatabase* iCommsDatabase;
    
    TConnPrefList*        iPrefsList;
    TExtendedConnPref*    iExtPrefs;
    };
#endif /* CAOCONNECTIONIMPL_H */
