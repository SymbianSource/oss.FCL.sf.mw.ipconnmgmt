/*
 * Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Listen cellular data usage key changes in central repository.
 *
 */

#include <e32base.h>
#include <centralrepository.h>
#include <cmmanagerkeys.h>
#include <cmgenconnsettings.h>

#include "mpmlogger.h"
#include "mpmserver.h"
#include "mpmdatausagewatcher.h"

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMpmDataUsageWatcher::CMpmDataUsageWatcher( CMPMServer* aServer ) :
    CActive( EPriorityStandard ), iServer( aServer )
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::CMpmDataUsageWatcher" )

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor. Creates a central repository object.
// ---------------------------------------------------------------------------
//
void CMpmDataUsageWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::ConstructL" )

    iRepository = CRepository::NewL( KCRUidCmManager );
    }

// ---------------------------------------------------------------------------
// Creates a new object by calling the two-phased constructor.
// ---------------------------------------------------------------------------
//
CMpmDataUsageWatcher* CMpmDataUsageWatcher::NewL( CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::NewL" )

    CMpmDataUsageWatcher* self = new( ELeave ) CMpmDataUsageWatcher( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMpmDataUsageWatcher::~CMpmDataUsageWatcher()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::~CMpmDataUsageWatcher" )

    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Order notification from changes.
// ---------------------------------------------------------------------------
//
void CMpmDataUsageWatcher::StartL()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::StartL" )

    // Request notification
    User::LeaveIfError( iRepository->NotifyRequest( KCurrentCellularDataUsage,
            iStatus ) );
    SetActive();

    // Get value from central repository
    User::LeaveIfError( iRepository->Get( KCurrentCellularDataUsage,
            iCellularDataUsage ) );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Event is received when there is a change in central repository key.
// ---------------------------------------------------------------------------
//
void CMpmDataUsageWatcher::RunL()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::RunL" )

    User::LeaveIfError( iStatus.Int() );

    // Request new notification
    User::LeaveIfError( iRepository->NotifyRequest( KCurrentCellularDataUsage,
            iStatus ) );
    SetActive();

    TInt oldCellularDataUsage = iCellularDataUsage;

    // Get the new value from central repository
    User::LeaveIfError( iRepository->Get( KCurrentCellularDataUsage,
            iCellularDataUsage ) );

    // Stop cellular connections if the setting changes into Disabled
    if (oldCellularDataUsage != ECmCellularDataUsageDisabled &&
            iCellularDataUsage == ECmCellularDataUsageDisabled)
        {
        // TODO: Uncomment this when trying to get stopping working.
        // iServer->StopCellularConns();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Nothing to do over here.
// ---------------------------------------------------------------------------
//
TInt CMpmDataUsageWatcher::RunError( TInt /*aError*/ )
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::RunError" )

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel outstanding request.
// ---------------------------------------------------------------------------
//
void CMpmDataUsageWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::DoCancel" )

    iRepository->NotifyCancel( KCurrentCellularDataUsage );
    }

