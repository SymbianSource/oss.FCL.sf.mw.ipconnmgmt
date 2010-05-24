/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *      Server implementation for FlexTimerServerMonitor.
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERSERVERMONITORSERVER_H
#define FLEXTIMERSERVERMONITORSERVER_H

// System includes
#include <e32base.h>
#include <rflextimer.h>

// Class declaration
/**
 * A server for detecting, has FlexTimerServer crashed during the
 * monitoring period.
 */
class CFlexTimerServerMonitorServer : public CServer2
    {
    
public:
    /**
     * Two phased constructor 
     * 
     * @param aPriority Active object priority for the server 
     * @return Pointer to created server object
     */
    IMPORT_C static CFlexTimerServerMonitorServer* NewL( 
        CActive::TPriority aPriority );

    /**
     * Destructor 
     */
    virtual ~CFlexTimerServerMonitorServer();

public:

    /**
     * From CServer2
     * Function for creating a new session to this server. 
     *
     * @param aVersion Version identifier from client
     * @param aMessage Reference to creation message sent by client
     * @return Pointer to created session object
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

    /**
     * Function for creating server thread resources. Creates the server
     * object and active scheduler. Loops in the active scheduler to serve
     * client requests until explicitly shutdown. Function leaves if the
     * creation of needed resources fails.
     */
    static void StartServerL();

    /**
     * Start monitoring FlexTimer server crashing.
     * 
     * @return KErrNone when succeeded, KErrAlreadyExists if monitoring was
     * already started, or any other system level error code.
     */
    TInt StartMonitoring();
    
    /**
     * Stop monitoring FlexTimer server crashing.
     * 
     * @return KErrNone when succeeded, KErrNotFound if monitoring is not
     * started.
     */
    TInt StopMonitoring();
    
    /**
     * Get status of monitoring the server. I.e. has the server crashed or 
     * not.
     *
     * @param aHasServerCrashed Return ETrue if server has crashed while
     * monitoring, otherwise EFalse.
     * @return KErrNone when succeeded, KErrNotFound if monitoring is not
     * started.
     */
    TInt GetStatus( TBool& aHasServerCrashed );
    
private:

    /**
     * Constructor
     * 
     * @param aPriority Active object priority 
     */
    CFlexTimerServerMonitorServer( CActive::TPriority aPriority );

private: // Data

    /**
     * Handle to FlexTimer server.
     * This parameter needs to be in server due it is manipulated by
     * multible sessions.
     */
    RFlexTimer iFlexTimer;
    
    /**
     * State of monitoring. ETrue if monitoring has started, EFalse if not.
     * This state is introduced for preventing starting/stopping the
     * monitoring more than once in a row.
     */
    TBool iIsMonitoringOn;
    };

#endif /* FLEXTIMERSERVERMONITORSERVER_H */
