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
* Description: Listen connect screen id key changes in central repository.
*
*/

#include <e32base.h>
#include <centralrepository.h>
#include <mpmconnectscreenid.h>
#include <featmgr.h>                     // FeatureManager
#include "mpmcsidwatcher.h"
#include "mpmlogger.h"

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMpmCsIdWatcher::CMpmCsIdWatcher()
    : CActive( EPriorityStandard )
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::CMpmCsIdWatcher" )
            
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// Creates central repository object
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::ConstructL()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::ConstructL" )

    iRepository = CRepository::NewL( KMpmOccCenRepUid );

    // Check whether user connection is supported
    FeatureManager::InitializeLibL();
    iUserConnectionSupported = FeatureManager::FeatureSupported( 
                                   KFeatureIdFfConnectionOverride );
 
     FeatureManager::UnInitializeLib();
    }


// ---------------------------------------------------------------------------
// Creates new object
// ---------------------------------------------------------------------------
//
CMpmCsIdWatcher* CMpmCsIdWatcher::NewL()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::NewL" )

    CMpmCsIdWatcher* self = new( ELeave ) CMpmCsIdWatcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMpmCsIdWatcher::~CMpmCsIdWatcher()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::~CMpmCsIdWatcher" )

    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Order notification from changes
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::StartL()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::StartL" )
            
    // Get the initial Connect screen ID from repository.
    User::LeaveIfError( GetConnectScreenId() );

    // Request for notifications.
    User::LeaveIfError( RequestNotifications() );
    }

// ---------------------------------------------------------------------------
// Return connect screen id
// ---------------------------------------------------------------------------
//
TUint32 CMpmCsIdWatcher::ConnectScreenId() const
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::ConnectScreenId" )

    if ( iUserConnectionSupported )
        {
        // Return real value if user connection is supported
        return iConnectScreenId;
        }
    else
        {
        // Return 0xFFFFFFFF if user connection is not supported
        return 0xFFFFFFFF;
        }
    }

// ---------------------------------------------------------------------------
// From class CActive.
// When there is a change in central repository key, event is received in here
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::RunL()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::RunL" )

    if ( iStatus.Int() < KErrNone )
        {
        MPMLOGSTRING2("Status: 0x%08X", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KMpmCsIdWatcherCenRepErrorThreshold )
            {
            MPMLOGSTRING2("Over %d consecutive errors, stopping notifications permanently.",
                    KMpmCsIdWatcherCenRepErrorThreshold)
            return;
            }
        // Else: Error occured but counter not expired. Proceed.
        }
    else
        {
        // Notification is received ok => Reset the counter.
        iErrorCounter = 0;

        // Get value from central repository
        GetConnectScreenId();
        }

    RequestNotifications();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel outstanding request
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::DoCancel()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::DoCancel" )
    iRepository->NotifyCancel( KMpmConnectScreenId );
    }

// ---------------------------------------------------------------------------
// Request notifications.
// ---------------------------------------------------------------------------
//
TInt CMpmCsIdWatcher::RequestNotifications()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::RequestNotifications" )

    TInt err = iRepository->NotifyRequest( KMpmConnectScreenId, iStatus );
        
    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        MPMLOGSTRING2( "CMpmCsIdWatcher::RequestNotifications, ERROR: %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Get current repository key value.
// ---------------------------------------------------------------------------
//
TInt CMpmCsIdWatcher::GetConnectScreenId()
    {
    MPMLOGSTRING( "CMpmCsIdWatcher::GetConnectScreenId" )

    TInt err = iRepository->Get( KMpmConnectScreenId, iConnectScreenId );
        
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmCsIdWatcher::GetConnectScreenId, ERROR: %d", err )
        }
    return err;
    }
