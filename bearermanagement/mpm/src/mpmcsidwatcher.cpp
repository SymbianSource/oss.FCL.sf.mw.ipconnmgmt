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
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// Creates central repositor object
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::ConstructL()
    {
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
    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Order notification from changes
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::StartL()
    {
    // Request notification
    User::LeaveIfError( iRepository->NotifyRequest( KMpmConnectScreenId,
                        iStatus ));
    SetActive();

    // Get value from central repository
    User::LeaveIfError(iRepository->Get(KMpmConnectScreenId, iConnectScreenId));
    }

// ---------------------------------------------------------------------------
// Return connect screen id
// ---------------------------------------------------------------------------
//
TUint32 CMpmCsIdWatcher::ConnectScreenId() const
    {
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
    // Leave if error
    User::LeaveIfError( iStatus.Int() );

    // Request new notification
    User::LeaveIfError( iRepository->NotifyRequest( KMpmConnectScreenId,
        iStatus ));
    SetActive();
    
    // Get value from central repository
    iRepository->Get( KMpmConnectScreenId, iConnectScreenId );
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Nothing to do over here
// ---------------------------------------------------------------------------
//
TInt CMpmCsIdWatcher::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel outstanding request
// ---------------------------------------------------------------------------
//
void CMpmCsIdWatcher::DoCancel()
    {
    iRepository->NotifyCancel( KMpmConnectScreenId );
    }

