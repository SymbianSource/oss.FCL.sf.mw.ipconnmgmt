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
 *      Client implementation for FlexTimerServerMonitor
 */

/*
 * %version: 1 %
 */

#ifndef RFLEXTIMERSERVERMONITOR_H
#define RFLEXTIMERSERVERMONITOR_H

// INCLUDE FILES
#include <e32std.h> 

// CLASS DECLARATION
/**
 * Client implementation for FlexTimerServerMonitor.
 * Forwards all requests as synchronous messages to the client.
 * 
 * Creates the server if it is not up'n'running when connecting to the server
 * at the first time.
 * 
 * StartMonitoring() has to be called before StopMonitoring() and GetStatus()
 * works. Note! The starting can be done by different handle. I.e.
 * 
 * @code
 * 
 *      RFlexTimerServerMonitor mon1;
 *      mon1.Connect();
 *      mon1.StartMonitoring();
 *      mon1.Close();
 *      
 *      .
 *      .
 *      . 
 *      
 *      RFlexTimerServerMonitor mon2;
 *      mon2.Connect();
 *      TBool hasCrashed;
 *      mon2.GetStatus( hasCrashed );
 *      // Do your stuff
 *      ...
 *      mon2.StopMonitoring();
 *      mon2.Close();
 *      
 * @endcode
 */
class RFlexTimerServerMonitor : public RSessionBase
    {
public:
    // Constructors and destructors

    /**
     * Constructor
     */
    IMPORT_C RFlexTimerServerMonitor();

    /**
     * Destructor
     */
    IMPORT_C ~RFlexTimerServerMonitor();

    /**
     * Connect to the server.
     * 
     * @return KErrNone on success. Otherwise returns one of the system-wide
     * error codes.
     */
    IMPORT_C TInt Connect();

    /**
     * Start detecting the crashes.
     * 
     * @return KErrNone if succeeded, KErrAlreadyExists if monitoring is
     * already activated, or any other system-wide error code.
     */
    IMPORT_C TInt StartMonitoring();

    /**
     * Stop detecting the crashes.
     * 
     * @return KErrNone if succeeded, KErrNotFound if monitoring is not
     * activated, or any other system-wide error code.
     */
    IMPORT_C TInt StopMonitoring();
    
    /**
     * Get the current status of monitoring.
     * 
     * 
     * @param aHasServerCrashed ETrue if server has crashed, EFalse otherwise
     * @return KErrNone if succeeded, KErrNotFound if monitoring is not
     * activated, or any other system-wide error code.
     */
    IMPORT_C TInt GetStatus( TBool& aHasServerCrashed );
    
private:
    
    /**
     * Gets the version number.
     * @return The version.
     */
    TVersion Version() const;

    /**
     * Connects to the server. If server does not exist, it is created.
     * @return KErrNone on success. Otherwise returns one of the system-wide
     * error codes.
     */
    TInt StartServer();
    };

#endif // RFLEXTIMERSERVERMONITOR_H

// End of File
