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

    // Get the initial data usage value from repository.
    User::LeaveIfError( GetCurrentDataUsageValue() );

    // Request notifications.
    User::LeaveIfError( RequestNotifications() );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Event is received when there is a change in central repository key.
// ---------------------------------------------------------------------------
//
void CMpmDataUsageWatcher::RunL()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::RunL" )

    if ( iStatus.Int() < KErrNone )
        {
        MPMLOGSTRING2("Status: 0x%08X", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KMpmDataUsageWatcherCenRepErrorThreshold )
            {
            MPMLOGSTRING2("Over %d consecutive errors, stopping notifications permanently.",
                    KMpmDataUsageWatcherCenRepErrorThreshold)
            return;
            }
        // Else: Error occured but counter not expired. Proceed.
        }
    else
        {
        // Notification is received ok => Reset the counter.
        iErrorCounter = 0;

        // Get the new Cellular data usage setting value from central repository.
        TInt oldCellularDataUsage = iCellularDataUsage;

        if ( GetCurrentDataUsageValue() == KErrNone )
            {
            // Stop cellular connections if the setting changes into Disabled.
            if ( oldCellularDataUsage != ECmCellularDataUsageDisabled &&
                    iCellularDataUsage == ECmCellularDataUsageDisabled &&
                    iServer->RoamingWatcher()->RoamingStatus() != EMPMRoamingStatusUnknown )
                {
                iServer->StopCellularConns();
                }
            }
        }
    
    RequestNotifications();
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

// ---------------------------------------------------------------------------
// Request notifications.
// ---------------------------------------------------------------------------
//
TInt CMpmDataUsageWatcher::RequestNotifications()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::RequestNotifications" )

    TInt err = iRepository->NotifyRequest( KCurrentCellularDataUsage, iStatus );
        
    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        MPMLOGSTRING2( "CMpmDataUsageWatcher::RequestNotifications, ERROR: %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Get current repository key value.
// ---------------------------------------------------------------------------
//
TInt CMpmDataUsageWatcher::GetCurrentDataUsageValue()
    {
    MPMLOGSTRING( "CMpmDataUsageWatcher::GetCurrentDataUsageValue" )

    TInt err = iRepository->Get( KCurrentCellularDataUsage, iCellularDataUsage );
        
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmDataUsageWatcher::GetCurrentDataUsageValue, ERROR: %d", err )
        }
    return err;
    }
