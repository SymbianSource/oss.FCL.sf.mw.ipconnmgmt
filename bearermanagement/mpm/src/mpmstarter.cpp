/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MPM Server Entry Point
*
*/

/**
@file mpmstarter.cpp
Mobility Policy Manager server entry point.
*/

// INCLUDE FILES
#include <e32svr.h>

#include "mpmstarter.h"
#include "mpmscheduler.h"
#include "mpmserver.h"
#include "mpmlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
    MPMLOGSTRING( "MPMStarter::RunServerL" )
    // Create and install the active scheduler
    CMpmScheduler* scheduler = CMpmScheduler::NewLC();
    CActiveScheduler::Install( scheduler );

    // Create the server
    CServer2* server = MPMStarter::CreateAndStartServerL();
    CleanupStack::PushL( server );
    scheduler->SetMpmServer( server );

    User::LeaveIfError( RThread::RenameMe( MPMStarter::ServerName() ) );

    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    CActiveScheduler::Start();

    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( scheduler );
    }


// -----------------------------------------------------------------------------
// Server process entry-point
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if ( cleanup )
        {
        TRAP( r, RunServerL() );
        delete cleanup;
        }
#ifdef _DEBUG
    else
        {
        MPMLOGSTRING( "E32Main - null cleanup pointer!" )
        }
#endif // _DEBUG
    __UHEAP_MARKEND;
#ifdef _DEBUG
    if ( ( r != KErrNone ) && ( r != KErrAlreadyExists ) )
        {
        MPMLOGSTRING2( "E32Main - RunServerL made a leave: %i", r )
        }
#endif // _DEBUG
    return r;
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MPMStarter::CreateAndStartServerL
// create server object
// -----------------------------------------------------------------------------
//
CServer2* MPMStarter::CreateAndStartServerL()
    {
    MPMLOGSTRING( "MPMStarter::CreateAndStartServerL" )
    return CMPMServer::NewL();
    }

// -----------------------------------------------------------------------------
// MPMStarter::ServerName
// return server name
// -----------------------------------------------------------------------------
//
TPtrC MPMStarter::ServerName()
    {
    return KMPMServerName().Mid( 0 );
    }

//  End of File
