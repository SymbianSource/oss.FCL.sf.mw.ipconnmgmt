/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for ConnMon.exe (launches ConnMon server).
*
*/

#include "ConnMonServ.h"

TInt SetSignal( CConnMonScheduler::TSignal& aSignal );

// -----------------------------------------------------------------------------
// E32Main()
// Starting point (MARM).
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    CConnMonScheduler::TSignal signal;
    TInt err = SetSignal( signal );
    if ( !err )
        {
        err = CConnMonScheduler::ThreadStart( signal );
        }
    return err;
    }

// -----------------------------------------------------------------------------
// SetSignal()
// This simple function uses a lot of stack space,
// that isn't needed throughout the lifetime of the server.
// That's why it's separate from E32Main()
// Don't be tempted to amalgamate it back again.
// -----------------------------------------------------------------------------
//
TInt SetSignal( CConnMonScheduler::TSignal& aSignal )
    {
    TBuf< 0x100 > cmd;
    User::CommandLine( cmd );
    return aSignal.Set( cmd );
    }

// End-of-file
