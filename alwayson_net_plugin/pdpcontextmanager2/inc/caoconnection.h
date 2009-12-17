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
* Description:  Class CAOConnection declaratation.
*
*/


#ifndef CAOCONNECTION_H
#define CAOCONNECTION_H

// INCLUDE FILES
#include <e32base.h>
#include <es_sock.h>

// FORWARD DECLARATIONS
class CAOConnectionImpl;
class MAOConnectionObserver;
class MAOSettings;

/**
 *  CAOConnection is responsible for creating a new connection
 *  if requested. Also it is possible to attach to an existing
 *  connection via connection ID.
 *  Events like succesfull connection, disconnection, connection failures
 *  are notified through MAOConnectionObserver interface.
 *  
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CAOConnection ): public CBase
    {
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
    static CAOConnection* NewL( MAOConnectionObserver& aObserver,
                                MAOSettings&           aSettings,
                                RSocketServ*           aSocketServ );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOConnection();
    
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
     * Closes connection without stopping it.
     *
     * @since S60 v3.2
     */
    void CloseConnection();
    
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
        
private: // New methods

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     */
    CAOConnection();

    /**
     * Default Symbian constructor
     *
     * @since S60 v3.1
     * @param aObserver ?
     * @param aSettings ?
     * @param aSocketServ ?
     */
    void ConstructL( MAOConnectionObserver& aObserver,
                     MAOSettings&           aSettings,
                     RSocketServ*           aSocketServ );

private: // Data

    /**
     * Own: Connection implementation
     */
    CAOConnectionImpl* iConnectionImpl;
    
    };
    
#endif /* CAOCONNECTION_H */
