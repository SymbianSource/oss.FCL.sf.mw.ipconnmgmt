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
* Description:  Defines MAOConnectionObserver interface
*
*/


#ifndef M_MAOCONNECTIONOBSERVER_H
#define M_MAOCONNECTIONOBSERVER_H

// INCLUDE FILES
#include <es_sock.h>
#include "maoconnectionmanager.h"

// FORWARD DECLARATIONS

/**
*  Connection listener observer interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class MAOConnectionObserver
    {
public:

    /**
     * Succesful PDP context activation is notified
     * through this method.
     *
     * @since S60 v3.1
     */
    virtual void HandlePDPContextActivatedL() = 0;

    /**
     * PDP context activation failure is notified through
     * this method.
     *
     * @since S60 v3.1
     * @param aReason: Activation failure reason
     */
    virtual void HandlePDPContextActivationFailedL(
        MAOConnectionManager::TFailureReason aReason ) = 0;
    
    /**
     * PDP context disconnection is notified through
     * this method.
     *
     * @since S60 v3.1
     * @param aReason: Disconnection reason
     */
    virtual void HandlePDPContextDisconnectedL(
        MAOConnectionManager::TFailureReason aReason ) = 0;
    
    /**
     * PDP context temporary data transfer blocking notification
     * is recieved through this method.
     *
     * @since S60 v3.1
     */
    virtual void HandlePDPContextTemporarilyBlockedL() = 0;
    
protected:

    /**
     * Prohibit destruction
     *
     * @since S60 v3.1
     */
    virtual ~MAOConnectionObserver(){};
    };

#endif // M_MAOCONNECTIONOBSERVER_H
