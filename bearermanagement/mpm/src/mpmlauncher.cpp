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
* Description: MPM Client DLL Entry Point
*
*/

/**
@file mpmlauncher.cpp
Mobility Policy Manager client DLL entry point.
*/

#include <e32std.h>
#include "mpmlauncher.h"
#include "mpmlogger.h"


// -----------------------------------------------------------------------------
// MPMLauncher::LaunchServer
// -----------------------------------------------------------------------------
//
TInt MPMLauncher::LaunchServer(
        const TDesC& aServerFileName,
        const TUid& aServerUid2,
        const TUid& aServerUid3 )
    {
    MPMLOGSTRING("MPMLauncher::LaunchServer")
    const TUidType serverUid( KNullUid, aServerUid2, aServerUid3 );
    RSemaphore semaphore;
    TInt err( KErrNone );

    err = semaphore.CreateGlobal( KMPMLauncherSemaphore, 0 );
    if ( err != KErrNone )
        {
        // Creating semaphore failed, which means some other thread is
        // creating the server right now. Propagate error KErrServerBusy
        // to inform the client it should try connecting again.
        return KErrServerBusy;
        }

    MPMLOGSTRING("Create a new server process")
    RProcess server;
    TInt r = server.Create( aServerFileName, KNullDesC, serverUid );

    if ( r != KErrNone )
        {
        MPMLOGSTRING2("Server process creation returned error: %d", r)
        semaphore.Close();
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 ); // Abort startup.
        }
    else
        {
        server.Resume(); // Start the server.
        }
    User::WaitForRequest( stat ); // Wait for start or death.
    // We can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone.
    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    semaphore.Close();
    return r;
    }

// End of File
