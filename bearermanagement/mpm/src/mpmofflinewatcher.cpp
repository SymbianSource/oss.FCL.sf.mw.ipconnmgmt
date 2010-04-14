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
 * Description: Listens central repository for offline mode changes.
 *
 */

#include <e32base.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <featmgr.h>

#include "mpmlogger.h"
#include "mpmserver.h"
#include "mpmofflinewatcher.h"

// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMpmOfflineWatcher::CMpmOfflineWatcher( CMPMServer* aServer ) :
    CActive( EPriorityStandard ), iServer( aServer )
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::CMpmOfflineWatcher" )

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor. Creates a central repository object.
// ---------------------------------------------------------------------------
//
void CMpmOfflineWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::ConstructL" )

    FeatureManager::InitializeLibL();
    // If feature isn't supported, then no watching, but return in StartL().
    iOfflineFeatureSupported = FeatureManager::FeatureSupported( 
                               KFeatureIdOfflineMode );
    FeatureManager::UnInitializeLib();

        
    iRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
    }

// ---------------------------------------------------------------------------
// Creates a new object by calling the two-phased constructor.
// ---------------------------------------------------------------------------
//
CMpmOfflineWatcher* CMpmOfflineWatcher::NewL( CMPMServer* aServer )
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::NewL" )

    CMpmOfflineWatcher* self = new( ELeave ) CMpmOfflineWatcher( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMpmOfflineWatcher::~CMpmOfflineWatcher()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::~CMpmOfflineWatcher" )

    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Order notification from changes.
// ---------------------------------------------------------------------------
//
void CMpmOfflineWatcher::StartL()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::StartL" )
            
    if ( !iOfflineFeatureSupported )
        {
        return;
        }

    // Get the initial data usage value from repository.
    User::LeaveIfError( GetCurrentOfflineValue() );
    iServer->UpdateOfflineMode( iOfflineMode );
    
    // Request notifications.
    User::LeaveIfError( RequestNotifications() );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Event is received when there is a change in central repository key.
// ---------------------------------------------------------------------------
//
void CMpmOfflineWatcher::RunL()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::RunL" )

    if ( iStatus.Int() < KErrNone )
        {
        MPMLOGSTRING2("Status: 0x%08X", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KMpmOfflineWatcherCenRepErrorThreshold )
            {
            MPMLOGSTRING2("Over %d consecutive errors, stopping notifications permanently.",
                    KMpmOfflineWatcherCenRepErrorThreshold)
            return;
            }
        // Else: Error occured but counter not expired. Proceed.
        }
    else
        {
        // Notification is received ok => Reset the counter.
        iErrorCounter = 0;

        // Check if mode has changed (it should).
        TInt oldMode = iOfflineMode;
        
        TInt err = GetCurrentOfflineValue();
        if ( err == KErrNone && oldMode != iOfflineMode )
            {
            iServer->UpdateOfflineMode( iOfflineMode );
            }
        }
    
    RequestNotifications();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel the outstanding request.
// ---------------------------------------------------------------------------
//
void CMpmOfflineWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::DoCancel" )

    iRepository->NotifyCancel( KCoreAppUIsNetworkConnectionAllowed );
    }

// ---------------------------------------------------------------------------
// Request for notifications.
// ---------------------------------------------------------------------------
//
TInt CMpmOfflineWatcher::RequestNotifications()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::RequestNotifications" )

    TInt err = iRepository->NotifyRequest( KCoreAppUIsNetworkConnectionAllowed, iStatus );
        
    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        // MPM's offline mode watching wouldn't recover... 
        MPMLOGSTRING2( "CMpmOfflineWatcher::RequestNotifications, ERROR: %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Get the current repository key value.
// ---------------------------------------------------------------------------
//
TInt CMpmOfflineWatcher::GetCurrentOfflineValue()
    {
    MPMLOGSTRING( "CMpmOfflineWatcher::GetCurrentOfflineValue" )

    TInt err = iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, iOfflineMode );
        
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmOfflineWatcher::GetCurrentOfflineValue, ERROR: %d", err )
        }
    return err;
    }
