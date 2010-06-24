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
* Description:  Implements the CAOConnection class
*
*/


// INCLUDE FILES
#include "caoconnection.h"
#include "caoconnectionimpl.h"
#include "logger.h"

// METHODS

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAOConnection* CAOConnection::NewL(
    MAOConnectionObserver& aObserver,
    MAOSettings&           aSettings,
    RSocketServ*           aSocketServ
     )
    {
    LOG_1( _L("CAOConnection::NewL") );
    
    CAOConnection* self = new( ELeave ) CAOConnection();
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aSettings, aSocketServ );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnection::~CAOConnection()
    {
    LOG_1( _L("CAOConnection::~CAOConnection") );

    delete iConnectionImpl; 
    }

// ---------------------------------------------------------------------------
// CAOConnection::ActivateConnection
// ---------------------------------------------------------------------------
//
void CAOConnection::ActivateConnection()
    {
    LOG_1( _L("CAOConnection::ActivateConnection") );

    iConnectionImpl->ActivateConnection();      
    }

// ---------------------------------------------------------------------------
// CAOConnection::CancelConnection
// ---------------------------------------------------------------------------
//
void CAOConnection::CancelConnection()
    {
    LOG_1( _L("CAOConnection::CancelConnection") );
    
    iConnectionImpl->CancelConnection();
    }
    
// ---------------------------------------------------------------------------
// CAOConnection::CloseConnection
// ---------------------------------------------------------------------------
//
void CAOConnection::CloseConnection()
    {
    LOG_1( _L("CAOConnection::CloseConnection") );
    
    iConnectionImpl->CloseConnection();
    }    

// ---------------------------------------------------------------------------
// CAOConnection::IsConnectedToDefaultIAP
// ---------------------------------------------------------------------------
//
TBool CAOConnection::IsConnectedToDefaultIAP()
    {
    LOG_1( _L("CAOConnection::IsConnectedToDefaultIAP" ) );
    
    return iConnectionImpl->IsConnectedToDefaultIAP();
    }

// ---------------------------------------------------------------------------
// CAOConnection::NumberOfConnections
// ---------------------------------------------------------------------------
//
TInt CAOConnection::NumberOfConnections()
    {
    LOG_1( _L("CAOConnection::NumberOfConnections" ) );
    
    return iConnectionImpl->NumberOfConnections();
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnection::CAOConnection():
    iConnectionImpl( NULL )
    {
    LOG_1( _L("CAOConnection::CAOConnection") );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnection::ConstructL(
    MAOConnectionObserver& aObserver,
    MAOSettings&           aSettings,
    RSocketServ*           aSocketServ )
    {
    LOG_1( _L("CAOConnection::ConstructL") );
    
    iConnectionImpl = CAOConnectionImpl::NewL( aObserver, aSettings, aSocketServ );
    }
