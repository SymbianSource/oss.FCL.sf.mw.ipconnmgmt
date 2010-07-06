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
* Description:  Implements the CAOConnectionMonitor class
*
*/

// INCLUDE FILES
#include "caoconnectionmonitor.h"
#include "logger.h"
#include "caoconnectionmonitorimpl.h"

// METHODS

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAOConnectionMonitor* CAOConnectionMonitor::NewL(
    MAOConnectionMonitorObserver& aObserver,
    MAOSettings& aSettings )
    {
    LOG_1( _L("CAOConnectionMonitor::NewL") );
    
    CAOConnectionMonitor* self = new( ELeave ) CAOConnectionMonitor;
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aSettings );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnectionMonitor::~CAOConnectionMonitor()
    {
    LOG_1( _L("CAOConnectionMonitor::~CAOConnectionMonitor") );

    delete iConnectionMonitorImpl;
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitor::FetchNetworkRegistrationStatus
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitor::FetchNetworkRegistrationStatus()
    {
    LOG_1( _L("CAOConnectionMonitor::FetchNetworkRegistrationStatus") );
    
    iConnectionMonitorImpl->FetchNetworkRegistrationStatus();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitor::NetworkRegistration
// ---------------------------------------------------------------------------
//
CTelephony::TRegistrationStatus 
    CAOConnectionMonitor::NetworkRegistration() const
    {
    LOG_1( _L("CAOConnectionMonitor::NetworkRegistration") );
    
    return iConnectionMonitorImpl->NetworkRegistration();
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnectionMonitor::CAOConnectionMonitor():
    iConnectionMonitorImpl( NULL )
    {
    LOG_1( _L("CAOConnectionMonitor::CAOConnectionMonitor") );

    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitor::ConstructL(
    MAOConnectionMonitorObserver& aObserver,
    MAOSettings& aSettings )
    {
    LOG_1( _L("CAOConnectionMonitor::ConstructL") );
    
    iConnectionMonitorImpl =
        CAOConnectionMonitorImpl::NewL( aObserver, aSettings );
    }
