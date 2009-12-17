/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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

// INCLUDE FILES
#include <e32std.h>
#include "mpmlauncher.h"
#include "mpmlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MPMLauncher::LaunchServer
// -----------------------------------------------------------------------------
//
TInt MPMLauncher::LaunchServer(
    const TDesC& aServerFileName,
    const TUid& aServerUid2,
    const TUid& aServerUid3)
    {
    MPMLOGSTRING("MPMLauncher::LaunchServer")
    const TUidType serverUid(KNullUid,aServerUid2,aServerUid3);

    // We just create a new server process. Simultaneous
    // launching of two such processes should be detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    //
    MPMLOGSTRING("Create a new server process")
    RProcess server;
    TInt r=server.Create(aServerFileName,KNullDesC,serverUid);

    if ( r != KErrNone )
        {
        MPMLOGSTRING2("Server process creation returned error: %d", r)
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous(stat);
    if ( stat != KRequestPending )
        {
        server.Kill(0);        // abort startup
        }
    else
        {
        server.Resume();    // logon OK - start the server
        }
    User::WaitForRequest(stat);        // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File 

