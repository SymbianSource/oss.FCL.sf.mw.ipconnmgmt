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
* Launches the Connection Method Manager Server.
*
*/


#include "cmmclistatic.h"

/**
 * LaunchServer
 *
 * Launches the CmManager server.
 *
 * Return KErrNone if CmManager server is launched successfully,
 * otherwise one of the system wide error codes.
 */
TInt TCmManagerLauncher::LaunchServer(
        const TDesC& aServerName,
        const TDesC& aServerFileName,
        const TUid& aServerUid3,
        const TUint aWinsMinHeapSize,
        const TUint aWinsMaxHeapSize,
        const TUint aWinsStackSize )
    {
    RMutex mutex;
    TInt err( KErrNone );
    const TUidType serverUid( KNullUid, KNullUid, aServerUid3 );

    //
    // EPOC and EKA2 is easy, we just create a new server process. Simultaneous
    // launching of two such processes should be detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    //

    // Try obtaining a lock. If that fails, server has already been started.
    // If something fails with obtaining the lock, propagate the error to user.
    if ( !ServerStartupLock( mutex, err ) )
        {
        ServerStartupUnlock( mutex );
        return err;
        }

    RProcess server;
    err = server.Create( aServerFileName, KNullDesC, serverUid );
    (void)aServerName;
    (void)aWinsMinHeapSize;
    (void)aWinsMaxHeapSize;
    (void)aWinsStackSize;

    if ( err != KErrNone )
        {
        ServerStartupUnlock( mutex );
        return err;
        }

    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 ); // Abort startup.
        }
    else
        {
        server.Resume(); // Logon OK - start the server.
        }
    User::WaitForRequest( stat ); // Wait for start or death.
    // We can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone.
    err = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();

    ServerStartupUnlock( mutex );
    return err;
    }

/**
 * ServerStartupLock
 *
 * Returns true if obtains the lock without waiting. That is, the
 * mutex is not created before this call.
 *
 * Return false if mutex is created.
 */
TBool TCmManagerLauncher::ServerStartupLock( RMutex& mutex, TInt& err )
    {
    TInt retval( ETrue );

    // Create handle to mutex.
    err = mutex.CreateGlobal( KCmManagerStartupMutex );
    if ( err != KErrNone )
        {
        // Mutex already created, wait until done.
        retval = EFalse;
        err = mutex.OpenGlobal( KCmManagerStartupMutex );
        if ( err == KErrNone )
            {
            // Wait for completion
            mutex.Wait();
            // Server already running, return.
            return EFalse;
            }
        // Else opening failed, err will be propagated to user.
        }
    // Else Obtained lock without waiting (retval is ETrue).
    // The next call completes instantly.
    mutex.Wait();
    return retval;
    }

/**
 * ServerStartupUnlock
 *
 * Frees and closes the mutex.
 */
void TCmManagerLauncher::ServerStartupUnlock( RMutex& mutex )
    {
    // Let others run
    mutex.Signal();
    // Destroy handle
    mutex.Close();
    }

// End of file
