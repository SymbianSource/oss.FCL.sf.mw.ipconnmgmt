/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Creates and starts the server.
*
*/


#include "cmmstarter.h"


//
// Perform all server initialisation, in particular creation of the scheduler
// and server and then run the scheduler.
//
static void RunServerL()
    {
    // Create and install the active scheduler we need.
    CActiveScheduler* s = new( ELeave ) CActiveScheduler;
    CleanupStack::PushL( s );
    CActiveScheduler::Install( s );

    // Create the server.
    CServer2* server = CCmmStarter::CreateAndStartServerL();
    CleanupStack::PushL( server );

    // Naming the server thread after server startup helps to debug panics.
    User::LeaveIfError( RThread::RenameMe( CCmmStarter::ServerName() ) );

    // Initialisation complete, now signal the client.
    RProcess::Rendezvous( KErrNone );

    // Ready to run. This will return only when CCmmDelayedShutdown calls
    // CActiveScheduler::Stop().
    CActiveScheduler::Start();

    // Cleanup the server and scheduler.
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( s );
    }

//
// Server process entry-point.
//
TInt E32Main()
    {
//    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if ( cleanup )
        {
        TRAP( r, RunServerL() );
        delete cleanup;
        }

//    __UHEAP_MARKEND;
    return r;
    }

// End of file
