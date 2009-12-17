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
* Description: Session to MPM Default Connection server
*
*/

#include <featmgr.h>

#include "mpmdefaultconnserversession.h"
#include "mpmdefaultconnserver.h"
#include "mpmdefaultconnection.h"
#include "mpmcommon.h"
#include "mpmserver.h"
#include "mpmlogger.h"

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::NewL
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServerSession* CMPMDefaultConnServerSession::NewL( 
    CMPMDefaultConnServer* aServer )
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::NewL" )
    CMPMDefaultConnServerSession* self = new ( ELeave ) CMPMDefaultConnServerSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServerSession::~CMPMDefaultConnServerSession()
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::~CMPMDefaultConnServerSession" )
    DefaultConnServer()->MPMServer()->DefaultConnection()->ClearDefaultIAP();
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::DefaultConnServer
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServer* CMPMDefaultConnServerSession::DefaultConnServer()
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::DefaultConnServer" )
    return iDefaultConnServer; 
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::ServiceL
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnServerSession::ServiceL( const RMessage2& aMessage )
    {
    MPMLOGSTRING2( "CMPMDefaultConnServerSession::ServiceL %d ", 
                   aMessage.Function() )
    switch ( aMessage.Function() )
        {
        case EMPMDefaultConnectionSetDefaultIap:
            {
            SetDefaultIAP( aMessage );
            break;
            }
        case EMPMDefaultConnectionClearDefaultIap:
            {
            ClearDefaultIAP( aMessage);
            break;
            }
        default:
            {
            aMessage.Complete( KErrNotSupported );
			MPMLOGSTRING( 
			    "CMPMDefaultConnServerSession::ServiceL: Unsupported message type" )
            }                   
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPMDefaultConnServerSession::CMPMDefaultConnServerSession( 
CMPMDefaultConnServer* aServer )
    : CSession2(), 
      iDefaultConnServer( aServer ) 
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::CMPMDefaultConnServerSession" )
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::ConstructL
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnServerSession::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::SetDefaultIAP
// Store IAP to be used as default connection in MPM Server
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnServerSession::SetDefaultIAP( const RMessage2& aMessage )
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::SetDefaultIAP" )
    TInt iap( 0 ), err( KErrNone );
    iap = aMessage.Int0();
    err = DefaultConnServer()->MPMServer()->DefaultConnection()->SetDefaultIAP( iap );
    aMessage.Complete( err );
    }

// ---------------------------------------------------------------------------
// CMPMDefaultConnServerSession::ClearDefaultIAP
// Clear IAP from being used as default connection in MPM Server
// ---------------------------------------------------------------------------
//
void CMPMDefaultConnServerSession::ClearDefaultIAP( const RMessage2& aMessage )
    {
    MPMLOGSTRING( "CMPMDefaultConnServerSession::ClearDefaultIAP" )
    DefaultConnServer()->MPMServer()->DefaultConnection()->ClearDefaultIAP();
    aMessage.Complete( KErrNone );
    }
