/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Client interface to MPM Default Connection server
*
*/

#include "rmpmdefaultconnection.h"
#include "mpmcommon.h"
#include "mpmlogger.h"

_LIT( KPanicCategory, "MPM Default Connection Client" );

// ---------------------------------------------------------------------------
// RMPMDefaultConnection::Connect
// Creates connection to server
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMPMDefaultConnection::Connect()
    {
    MPMLOGSTRING("RMPMDefaultConnection::Connect" )
    TInt r( KErrNone );
    if( !iClientConnected ) 
        {
        MPMLOGSTRING("RMPMDefaultConnection::Connect: Connecting to server")
        r = CreateSession( KMPMDefaultConnectionServerName, Version(), KNumDefConnMessageSlots );
        if(r == KErrNone)
            {
            iClientConnected = ETrue;
            }
        else
            {
            MPMLOGSTRING2("RMPMDefaultConnection::Connect: \
Error occurred %d", r )
            Close();
            }
        }
    return r;
    }

// ---------------------------------------------------------------------------
// RMPMDefaultConnection::Close
// Closes connection to server
// ---------------------------------------------------------------------------
//
EXPORT_C void RMPMDefaultConnection::Close()
    {
    MPMLOGSTRING("RMPMDefaultConnection::Close" )
    RSessionBase::Close();
    iClientConnected = EFalse;
    }

// ---------------------------------------------------------------------------
// RMPMDefaultConnection::SetDefaultIAP
// Sets IAP to be used as default connection
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMPMDefaultConnection::SetDefaultIAP( TInt aIapId )
    {
    __ASSERT_ALWAYS( iClientConnected, 
                     User::Panic( KPanicCategory, 
                                  ERMPMPanicClientNotConnected ) );
    MPMLOGSTRING("RMPMDefaultConnection::SetDefaultIAP" )
    return SendReceive( EMPMDefaultConnectionSetDefaultIap, TIpcArgs( aIapId ) );
    }

// ---------------------------------------------------------------------------
//RMPMDefaultConnection::ClearDefaultIAP
// Clears IAP from being used as default connection
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RMPMDefaultConnection::ClearDefaultIAP()
    {
    __ASSERT_ALWAYS( iClientConnected, 
                     User::Panic( KPanicCategory, 
                                  ERMPMPanicClientNotConnected ) );
    MPMLOGSTRING("RMPMDefaultConnection::ClearDefaultIAP" )
    return SendReceive( EMPMDefaultConnectionClearDefaultIap );
    }

// -----------------------------------------------------------------------------
// RMPMDefaultConnection::Version
// Returns version number
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RMPMDefaultConnection::Version() const
    {
    MPMLOGSTRING("RMPMDefaultConnection::Version" )
    return TVersion(KMPMServerMajorVersionNumber,
                    KMPMServerMinorVersionNumber,
                    KMPMServerBuildVersionNumber);
    }
