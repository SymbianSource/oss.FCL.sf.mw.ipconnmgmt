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
 *      Server implementation for FlexTimerServerMonitor
 */
/*
 * %version: 2 %
 */

#include "flextimerservermonitorserver.h"
#include "flextimerservermonitorcommon.h"
#include "flextimerservermonitorsession.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerservermonitorserverTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Construction
// --------------------------------------------------------------------------
//
EXPORT_C CFlexTimerServerMonitorServer* CFlexTimerServerMonitorServer::NewL(
    CActive::TPriority aPriority )
    {
    return new ( ELeave ) CFlexTimerServerMonitorServer( aPriority );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CFlexTimerServerMonitorServer::~CFlexTimerServerMonitorServer()
    {
    iFlexTimer.Close();
    }

// --------------------------------------------------------------------------
// Creation of new sessions, called by server framework
// --------------------------------------------------------------------------
//
CSession2* CFlexTimerServerMonitorServer::NewSessionL( 
    const TVersion& aVersion ,
    const RMessage2& /* aMessage */ ) const
    {

    if ( aVersion.iMajor > KFlexTimerServerMonitorMajorVersionNumber )
        {
        User::Leave( KErrNotSupported );
        }

    // Create and return session
    return CFlexTimerServerMonitorSession::NewL();
    }

// --------------------------------------------------------------------------
// Connect to FlexTimer service
// --------------------------------------------------------------------------
//
TInt CFlexTimerServerMonitorServer::StartMonitoring()
    {
    if ( iIsMonitoringOn )
        {
        return KErrAlreadyExists;
        }
    iIsMonitoringOn = ETrue;
    return iFlexTimer.Connect();
    }

// --------------------------------------------------------------------------
// Close connection to FlexTimer service
// --------------------------------------------------------------------------
//
TInt CFlexTimerServerMonitorServer::StopMonitoring()
    {
    if ( !iIsMonitoringOn )
        {
        return KErrNotFound; 
        }
    iIsMonitoringOn = EFalse;
    iFlexTimer.Close();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// Get status of the monitored process
// --------------------------------------------------------------------------
//
TInt CFlexTimerServerMonitorServer::GetStatus( TBool& aHasServerCrashed )
    {
    if ( !iIsMonitoringOn )
        {
        return KErrNotFound;
        }
    
    // Check has the FlexTimerServer crashed by trying to configure
    // the timer. If the server has crashed, it fails. Usually, in case of
    // server crash this will return KErrServerTerminated (-15).
    TInt ret = iFlexTimer.Configure( TTimeIntervalMicroSeconds( 1000000 ) );

    if ( ret == KErrNone )
        { // No crash.
        aHasServerCrashed = EFalse;
        }
    else
        { // Crash has happened.
        aHasServerCrashed = ETrue;
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------
// Thread start function, loops in active scheduler
// --------------------------------------------------------------------------
//
void CFlexTimerServerMonitorServer::StartServerL()
    {
    // Called when thread is created, create active scheduler
    CActiveScheduler* scheduler = new CActiveScheduler;
    User::LeaveIfNull( scheduler );

    // Add active scheduler object to cleanup stack before installing it.
    // It will be deleted, if any of the next operations fails.
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Create server object and start it
    CFlexTimerServerMonitorServer* server( NULL );
    server = CFlexTimerServerMonitorServer::NewL( EPriorityStandard );
    
    // Push the server object to cleanup stack before starting it
    CleanupStack::PushL( server );

    User::LeaveIfError( server->Start( KFlexTimerServerMonitorServerName ) );

    // Complete open rendezvous in this process (syncs with creator)
    RProcess::Rendezvous( KErrNone );

    // Active scheduler start will not leave, thus pop added objects
    // from the cleanup stack. They will be deleted once the execution
    // returns from the active scheduler (application is exiting)
    CleanupStack::Pop( server );
    CleanupStack::Pop( scheduler );

    // Start active scheduler, thread stays in active scheduler loop
    CActiveScheduler::Start();

    // Delete allocated resources
    delete server;
    delete scheduler;
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CFlexTimerServerMonitorServer::CFlexTimerServerMonitorServer( 
    CActive::TPriority aPriority ) :
    CServer2( aPriority ), iIsMonitoringOn( EFalse )
    {
    }

// ======== GLOBAL FUNCTIONS ========

// --------------------------------------------------------------------------
// Main function of the server executable.
// --------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    // Mark memory allocation check 
    __UHEAP_MARK;

    // Create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    __ASSERT_ALWAYS(
        cleanup != NULL, 
        User::Panic( KFlexTimerServerMonitorPanicCategory, 
                     ECleanupStackCreationFailed ) );
    
    // Start server
    TRAPD( panicCode, CFlexTimerServerMonitorServer::StartServerL() );

    delete cleanup;
    if ( panicCode != KErrNone )
        {
        User::Panic( KFlexTimerServerMonitorPanicCategory, panicCode );
        }

    // Memory allocation check, panic in UDEB case of memory leak
    __UHEAP_MARKEND;

    return KErrNone;
    }
