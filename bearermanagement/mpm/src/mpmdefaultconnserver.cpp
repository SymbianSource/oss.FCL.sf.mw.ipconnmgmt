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
* Description: Default connection server implementation
*
*/

#include "mpmdefaultconnserver.h"
#include "mpmdefaultconnserversession.h"
#include "mpmlogger.h"
#include "mpmserver.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServer::CMPMDefaultConnServer( CMPMServer* aMPMServer )
    : CPolicyServer( CPolicyServer::EPriorityStandard, KMPMPolicy ),
    iMPMServer( aMPMServer )
    {        
    }


// ---------------------------------------------------------------------------
// CMPMDefaultConnServer::ConstructL
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnServer::ConstructL()
    {
    MPMLOGSTRING( "MPMDefConnServer starting" )
    StartL(KMPMDefaultConnectionServerName);
    }


// ---------------------------------------------------------------------------
// CMPMDefaultConnServer::NewL
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServer* CMPMDefaultConnServer::NewL( CMPMServer* aMPMServer )
    {
    CMPMDefaultConnServer* self = new ( ELeave ) CMPMDefaultConnServer( aMPMServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServer::~CMPMDefaultConnServer()
    {

    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServer::MPMServer
// ---------------------------------------------------------------------------
//
CMPMServer* CMPMDefaultConnServer::MPMServer()
    {
    return iMPMServer;
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServer::NewSessionL
// From class CServer2.
// ---------------------------------------------------------------------------
//
CSession2* CMPMDefaultConnServer::NewSessionL( const TVersion& /*aVersion*/,
                                               const RMessage2& /*aMessage*/) const
    {
    //create session
    return CMPMDefaultConnServerSession::NewL( 
        const_cast<CMPMDefaultConnServer*> ( this ) );
    }


