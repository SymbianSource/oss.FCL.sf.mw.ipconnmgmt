/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CAlwaysOnlineEComInterface interface
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <AlwaysOnlineManagerCommon.h>

#include "alwaysonlinepdpplugin.h"
#include "caoserver.h"

#include "alwaysonconfig.hrh"
#include "logger.h"


#define KPDPContextManager2ImplUid 0x10281F42

//    METHODS

// ---------------------------------------------------------------------------
// CAlwaysOnlinePDPPlugin::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAlwaysOnlinePDPPlugin* CAlwaysOnlinePDPPlugin::NewL()
    {
    LOG_1( _L("||-- LOGGING STARTS --||") );
    LOG_1( _L("CAlwaysOnlinePDPPlugin:: NewL") );

    CAlwaysOnlinePDPPlugin* self = new( ELeave ) CAlwaysOnlinePDPPlugin;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlinePDPPlugin::~CAlwaysOnlinePDPPlugin
// Destructor
// ---------------------------------------------------------------------------
//
CAlwaysOnlinePDPPlugin::~CAlwaysOnlinePDPPlugin()
    {
    delete iAlwaysOnServer;
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlinePDPPlugin::CAlwaysOnlinePDPPlugin
// Constructor
// ---------------------------------------------------------------------------
//
CAlwaysOnlinePDPPlugin::CAlwaysOnlinePDPPlugin():
    CAlwaysOnlineEComInterface()
    {
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlinePDPPlugin::ConstructL
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAlwaysOnlinePDPPlugin::ConstructL()
    {
    LOG_1( _L("CAlwaysOnlinePDPPlugin:: ConstructL") );
    iAlwaysOnServer = CAOServer::NewL();
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlinePDPPlugin::HandleServerCommandL
// Handles the events coming from the Always Online Server
// ---------------------------------------------------------------------------
//
TAny* CAlwaysOnlinePDPPlugin::HandleServerCommandL(
    TInt /*aCommand*/,
    TDesC8* /*aParameters*/ )
    {
    return static_cast<TAny*>( KErrNone );
    }

// ---------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// Use the plugin UID and Implementation factory method
// as a pair for ECom instantiation.
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
        KPDPContextManager2ImplUid,
        ( TProxyNewLPtr )CAlwaysOnlinePDPPlugin::NewL )
    };

// ---------------------------------------------------------------------------
// Give the table to the caller
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                             TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
                  sizeof( ImplementationTable[0] );
    return ImplementationTable;
    }

// End of File
