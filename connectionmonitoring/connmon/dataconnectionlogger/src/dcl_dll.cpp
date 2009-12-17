/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data Connection Logger.
*
*/

#include <e32std.h>
#include <e32base.h>
#include "dcl.h"
#include "dcl_log.h"

// --------------------------------------------------------------------------
// GLDEF_C TInt E32Main
//
// Returns: Always 0
// --------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CEngine* iapConnections = NULL;

    if ( !cleanup )
        {
        LOGIT0("FAILED to create CTrapCleanup")
        User::Panic( KDCLName, ECannotCreateCleanup );
        }

    CActiveScheduler* dclScheduler = NULL;

    dclScheduler = new CActiveScheduler();

    if ( !dclScheduler )
        {
        LOGIT0("FAILED to create CActiveScheduler")
        User::Panic( KDCLName, ECannotCreateActiveScheduler );
        }

    CActiveScheduler::Install( dclScheduler );

    TRAPD( leaveCode, iapConnections = CEngine::NewL() );
    if ( leaveCode )
        {
        LOGIT1("FAILED to create CEngine <%d>", leaveCode)
        User::Panic( KDCLName, ECannotCreateCEngine );
        }

    TRAP( leaveCode, iapConnections->NotifyL() );
    if ( leaveCode )
        {
        LOGIT1("FAILED to request notifications <%d>", leaveCode)
        User::Panic( KDCLName, ECannotRequestNotification );
        }

    RProcess::Rendezvous( KErrNone );

    CActiveScheduler::Start();

    delete cleanup;
    delete iapConnections;
    delete dclScheduler;

    __UHEAP_MARKEND;
    return 0;
    }

// End of File
