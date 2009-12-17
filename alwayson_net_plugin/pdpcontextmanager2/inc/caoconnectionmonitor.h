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


#ifndef CAOCONNECTIONMONITOR_H
#define CAOCONNECTIONMONITOR_H

// INCLUDE FILES
#include <e32base.h>
#include <etel3rdparty.h>

#include "caosettings.h"

// FORWARD DECLARATIONS
class CAOConnectionMonitorImpl;
class MAOConnectionMonitorObserver;

/**
 *  Connection monitor is responsible for listening
 *  all connection related events like creation of new connections,
 *  closing existing connection, nw changes etc.
 *  These events are notified through MAOConnectionMonitorObserver
 *  interface.
 *
 *  @lib PDPContextManager.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CAOConnectionMonitor ): public CBase
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver
     * @param aSettings
     * @return ?
     */
    static CAOConnectionMonitor* NewL(
        MAOConnectionMonitorObserver& aObserver,
        MAOSettings& aSettings );

    /**
     * Default destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOConnectionMonitor();
    
    /**
     * Fetches current network registration status.
     * Current status will be informed through
     * MAOConnectionMonitorObserver interface.
     * Asynchronous method.
     *
     * @since 3.0
     */
    void FetchNetworkRegistrationStatus();
    
    /**
     * Fetches current network registration status.
     * Synchronous method.
     *
     * @since S60 v3.1
     * @return Telephony::TRegistrationStatus
     */
    CTelephony::TRegistrationStatus NetworkRegistration() const;
    
private:

    /**
     * Default constructor.
     *
     * @since S60 v3.1
     */
    CAOConnectionMonitor();

    /**
     * Default Symbian constructor
     *
     * @since S60 v3.1
     * @param aObserver
     * @param aSettings
     */
    void ConstructL( MAOConnectionMonitorObserver& aObserver,
                     MAOSettings& aSettings );

private: // Data

    /**
     * Own: Connection monitor implementation
     */
    CAOConnectionMonitorImpl* iConnectionMonitorImpl;
    };

#endif /* CAOCONNECTIONMONITOR_H */
