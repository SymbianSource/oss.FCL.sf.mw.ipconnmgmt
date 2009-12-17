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
* Description:  Defines MAOConnectionMonitorObserver interface
*
*/


#ifndef M_MAOCONNECTIONMONITOROBSERVER_H
#define M_MAOCONNECTIONMONITOROBSERVER_H

// INCLUDE FILES
#include <etel3rdparty.h>

// FORWARD DECLARATIONS

/**
*  Connection monitor observer interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 3.1
*/
class MAOConnectionMonitorObserver
    {
public:

    /**
      * Notifies network registration status changes.
     *
     * @since 3.1
     * @param aNetworkRegistration: NW registration
      */
    virtual void HandleNWRegistrationStatusChangedL(
        CTelephony::TRegistrationStatus aNetworkRegistration ) = 0;
    
    /**
     * Notifies from a deleted connection.
     *
     * @since 3.1
     * @param aConnectionId connection id from connection monitor
     * @param aForward Tells whether message should be forwarded next layer
     */
    virtual void HandleConnectionDeletedL( const TUint aConnectionId, 
                                           const TBool aForward ) = 0;
    
    /**
     * Notifies that external connection has been created.
     *
     * @since 3.0
     */
    virtual void HandleExternalConnectionCreatedL() = 0;
    
    /**
     * Notifies that internal packet data connection has been created.
     *
     * @since S60 v3.2
     * @param aMyConnectionId connection id from connection monitor
     * @param aIapId iap id
     * @param anetId network id
     * @return void
     */
    virtual void HandleInternalConnectionCreatedL( const TUint aConnectionId,
                                                   const TUint aIapId, 
                                                   const TUint aNetId ) = 0;  
        
    /**
     * Notifies error if network registration status
     * is not received.
     *
     * @since 3.1
     * @param aError: Error code
     */
    virtual void HandleError( TInt aError ) = 0;
    
protected:

    /**
     * Prohibit destruction
     *
     * @since 3.1
     */
    virtual ~MAOConnectionMonitorObserver(){};
    };

#endif // M_MAOCONNECTIONMONITOROBSERVER_H
