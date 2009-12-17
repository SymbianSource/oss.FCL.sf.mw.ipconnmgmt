/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines MAOConnectionManager interface
*
*/


#ifndef MAOCONNECTIONMANAGER_H
#define MAOCONNECTIONMANAGER_H

// INCLUDE FILES

// FORWARD DECLARATIONS

/**
 *  Connection manager interface.
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
class MAOConnectionManager
    {
public: // Data types

    /**
     * Network type
     */
    enum TNetworkType
        {
        ENotRegistered,
        EHPLMN,
        EVPLMN
        };

    /**
     * Failure reason
     */
    enum TFailureReason
        {
        ETemporary,
        EPermanent,
        EDisconnected
        };
        
public:

    /**
     * Activate PDP context.
     * Asynchronous.
     *
     * @since S60 v3.1
     */
    virtual void ActivatePDPContextL() = 0;
    
    /**
     * Checks if we have a PDP context active.
     * Synchronous.
     *
     * @since S60 v3.1
     * @return ETrue if PDP context is active
     */
    virtual TBool IsPDPContextActive() const = 0;       

    /**
     * Closes current connection.
     * Synchronous.
     *
     * @since S60 v3.1
     */
    virtual void CloseConnection() = 0;
    
    /**
     * Closes current connection without stopping it.
     * Synchronous.
     *
     * @since S60 v3.2
     */
    virtual void DetachConnection() = 0;
    
    /**
     * Gets current network type.
     * Synchronous.
     *
     * @since S60 v3.1
     * @return TNetworkType
     */
    virtual TNetworkType NetworkType() const = 0;       
    
    /**
     * Gets numer of currently active connections.
     * Synchronous.
     *
     * @since 3.0
     * @return TInt
     */     
    virtual TInt NumberOfConnections() = 0;
    
    /**
     * Handles settings changes.
     *
     * @since S60 v3.2
     */
    virtual void HandleSettingsChangedL() = 0;
    
protected:

    /**
     * Prohibit destruction
     *
     * @since S60 v3.1
     */
    virtual ~MAOConnectionManager(){};
    };

#endif /* MAOCONNECTIONMANAGER_H */
