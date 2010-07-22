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
 *
 */

/*
 * %version: 1 %
 */
#include "rflextimerservermonitor.h"
#include "flextimerservermonitorcommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rflextimerservermonitorTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C RFlexTimerServerMonitor::RFlexTimerServerMonitor()
    {
    }

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C RFlexTimerServerMonitor::~RFlexTimerServerMonitor()
    {
    Close();
    }

// ---------------------------------------------------------------------------
// Connects to the server and create a session.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimerServerMonitor::Connect()
    {
    // Starts the server, if it does not already exist in the system.
    TInt ret = StartServer();

    if ( ret == KErrNone )
        { // No session, create it
        ret = CreateSession( 
            KFlexTimerServerMonitorServerName,
            Version(),
            KFlexTimerServerMonitorServerMessageSlots );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Start monitoring FlexTimer server crashes
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimerServerMonitor::StartMonitoring()
    {
    return SendReceive( EFlexTimerServerMonitorStartMonitoring );
    }

// ---------------------------------------------------------------------------
// Get the status of the monitoring; has FlexTimer server crashed while
// monitoring or not.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimerServerMonitor::GetStatus( TBool& aHasServerCrashed )
    {
    TPckgBuf<TBool> pckg;
    TIpcArgs args( &pckg );

    TInt ret = SendReceive( EFlexTimerServerMonitorHasServerCrashed, args );

    // Extract the value returned from the server.
    aHasServerCrashed = pckg();
    return ret;
    }

// ---------------------------------------------------------------------------
// Stop monitoring FlexTimer server crashes
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimerServerMonitor::StopMonitoring()
    {
    return SendReceive( EFlexTimerServerMonitorStopMonitoring );
    }

// ---------------------------------------------------------------------------
// Gets the version number.
// ---------------------------------------------------------------------------
//
TVersion RFlexTimerServerMonitor::Version() const
    {
    return ( TVersion( KFlexTimerServerMonitorMajorVersionNumber,
                       KFlexTimerServerMonitorMinorVersionNumber,
                       KFlexTimerServerMonitorBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
// Connects to the server. If server does not exist, it is created.
// ---------------------------------------------------------------------------
//
TInt RFlexTimerServerMonitor::StartServer()
    {
    TFindServer findServer( KFlexTimerServerMonitorServerName );
    TFullName serverName;

    // See if the server is already started. 
    TInt ret = findServer.Next( serverName );
    
    if ( ret != KErrNone )
        {
        //Server was not found so create one.
        RProcess serverProcess;
        TBuf<1> serverParameters;

        // Load the executable for the server.
        ret = serverProcess.Create( KFlexTimerServerMonitorServerExe,
                                    serverParameters,
                                    EOwnerThread );

        if ( ret == KErrNone )
            {
            // Server has been created successfully. It is initially 
            // in suspended state. Now resume the server process.

            // Wait until the server process has been started.
            TRequestStatus status;
            
            serverProcess.Rendezvous( status );
            serverProcess.Resume();
            User::WaitForRequest( status );

            // Check if server has panicked during initialization.
            ret = serverProcess.ExitType();
            if ( ret == EExitPanic )
                {
                ret = KErrServerTerminated;
                }
            else
                {
                ret = status.Int();
                }

            // The server process stands on its own. This handle can
            // be closed. 
            serverProcess.Close();
            }
        }
    return ret;
    }
