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
 * Description:  Implementation of CFlexTimerServer class.
 *
 */
/*
 * %version: 2 %
 */

// System include files
#include <e32cmn.h>
#include <e32std.h>
// User include files go here:
#include "flextimercommon.h"
#include "flextimerserver.h"
#include "flextimersession.h"
#include "flextimerengine.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimerserverTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Two phased construction, 1st phase
// --------------------------------------------------------------------------
//
EXPORT_C CFlexTimerServer* CFlexTimerServer::NewL(
        CActive::TPriority aPriority )
    {
    CFlexTimerServer* self = new ( ELeave ) CFlexTimerServer( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CFlexTimerServer::~CFlexTimerServer()
    {
    // Free allocated resources, CServer2 shutdowns sessions
    delete iTimerEngine;

    OstTrace0( TRACE_INTERNAL,
            CFLEXTIMERSERVER_DEL,
            "CFlexTimerServer::~CFlexTimerServer: exiting" );
    }

// --------------------------------------------------------------------------
// Creation of new sessions, called by server framework
// --------------------------------------------------------------------------
//
CSession2* CFlexTimerServer::NewSessionL( const TVersion& aVersion,
                                          const RMessage2& /*aMessage*/ ) const
    {
    // Version checking, according to two main principles
    // 1. Newer server will always support older clients (change this if
    //    compatibility is changed at some phase)
    // 2. Guaranteed that newer client will be compatible with older server 
    //    within the same major version
    if ( aVersion.iMajor > KFlexTimerServMajorVersionNumber )
        {
        OstTrace1(
                TRACE_INTERNAL,
                CFLEXTIMERSERVER_NEWSESSIONLVER,
                "CFlexTimerServer::NewSessionL: Invalid major version (%d)",
                aVersion.iMajor );
        User::Leave( KErrNotSupported );
        }

    // Create and return session
    CSession2* session = new ( ELeave ) CFlexTimerSession( iTimerEngine );

    OstTrace1( TRACE_INTERNAL,
            CFLEXTIMERSERVER_NEWSESSIONL,
            "CFlexTimerServer::NewSessionL: Created; session=%x",
            ( TUint )session );

    return session;
    }

// --------------------------------------------------------------------------
// Thread start function, loops in active scheduler
// --------------------------------------------------------------------------
//
void CFlexTimerServer::StartServerL()
    {
    // Called when thread is created, create active scheduler
    CActiveScheduler* sched = new CActiveScheduler;
    if ( sched == NULL )
        {
        // Leave error code is used as a panic code
        User::Leave( EFlexTimerServerActiveScheduler );
        }
    // Add active scheduler object to cleanup stack before installing it.
    // It will be deleted, if any of the next operations fails.
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );

    // Create server object and start it
    CFlexTimerServer* flexServer = NULL;
    flexServer = CFlexTimerServer::NewL( EPriorityStandard );

    // Push the server object to cleanup stack before starting it
    CleanupStack::PushL( flexServer );
    
    TInt err = KErrNone;
    
    err = flexServer->Start( KFlexTimerServerName );
    if ( err != KErrNone )
        {
        // Leave error code is used as a panic code
        User::Leave( EFlexTimerServerStartServer );
        }

    // Complete open rendezvous in this process (syncs with creator)
    RProcess::Rendezvous( KErrNone );

    // Active scheduler start will not leave, thus pop added objects
    // from the cleanup stack. They will be deleted once the execution
    // returns from the active scheduler (application is exiting)
    CleanupStack::Pop( flexServer );
    CleanupStack::Pop( sched );

    OstTrace0( TRACE_INTERNAL,
            CFLEXTIMERSERVER_START,
            "CFlexTimerServer::StartServerL: Call ActiveScheduler" );

    // Start active scheduler, thread stays in active scheduler loop
    CActiveScheduler::Start();

    OstTrace0( TRACE_INTERNAL,
            CFLEXTIMERSERVER_EXIT,
            "CFlexTimerServer::StartServerL: ActiveScheduler exit" );

    // Delete allocated resources
    delete flexServer;
    delete sched;
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CFlexTimerServer::CFlexTimerServer( CActive::TPriority aPriority ) :
    CServer2( aPriority ), iTimerEngine( NULL )
    {

    }

// --------------------------------------------------------------------------
// Two phased construction, 2nd phase
// --------------------------------------------------------------------------
//
void CFlexTimerServer::ConstructL()
    {
    // Create engine, leaving is handled in server creation
    iTimerEngine = CFlexTimerEngine::NewL();
    OstTrace0( TRACE_INTERNAL,
            CFLEXTIMERSERVER_CONSTRUCTL,
            "CFlexTimerServer::ConstructL: Created engine" );
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
    __ASSERT_ALWAYS( cleanup, User::Panic( KFlexTimerServerPanicCat,
                    EFlexTimerServerCleanupStack ) );
    // Start server
    TRAPD( panicCode, CFlexTimerServer::StartServerL() );
    if ( panicCode != KErrNone )
        {
        OstTrace1( TRACE_INTERNAL,
                CFLEXTIMERSERVER_MAINERROR,
                "CFlexTimerServer - Main: Start server failed (%d)",
                panicCode );
        delete cleanup;
        User::Panic( KFlexTimerServerPanicCat, panicCode );
        }
    else
        {
        OstTrace0( TRACE_INTERNAL,
                CFLEXTIMERSERVER_MAINEXIT,
                "CFlexTimerServer - Main: Application exiting" );

        // Delete cleanup stack
        delete cleanup;
        }

    // Memory allocation check, panic in UDEB case of memory leak
    __UHEAP_MARKEND;

    return KErrNone;
    }
