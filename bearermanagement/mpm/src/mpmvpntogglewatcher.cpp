/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Listen VPN toggle key changes in central repository.
*
*/

#include <centralrepository.h>
#include <mpmvpntoggleapi.h>
#include "mpmlogger.h"
#include "mpmvpntogglewatcher.h"

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMpmVpnToggleWatcher::CMpmVpnToggleWatcher( MMpmVpnToggleWatcherNotify& aNotify )
    : CActive( EPriorityStandard ),
      iNotify( aNotify )
    {
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// Creates central repositor object
// ---------------------------------------------------------------------------
//
void CMpmVpnToggleWatcher::ConstructL()
    {
    TRAPD(err, iRepository = CRepository::NewL( KMpmVpnToggleCenRepUid ));
    if ( err == KErrNone )
        {
        StartL();
        }
    }

// ---------------------------------------------------------------------------
// Creates new object
// ---------------------------------------------------------------------------
//
CMpmVpnToggleWatcher* CMpmVpnToggleWatcher::NewL( MMpmVpnToggleWatcherNotify& aNotify )
    {
    CMpmVpnToggleWatcher* self = new( ELeave ) CMpmVpnToggleWatcher(aNotify);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMpmVpnToggleWatcher::~CMpmVpnToggleWatcher()
    {    
    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Order notification from changes
// ---------------------------------------------------------------------------
//
void CMpmVpnToggleWatcher::StartL()
    {
    MPMLOGSTRING( "CMpmVpnToggleWatcher::StartL" )
            
    // Get the initial Connect screen ID from repository.
    User::LeaveIfError( GetVpnToggleValues() );

    // Request for notifications.
    User::LeaveIfError( RequestNotifications() );
    }

// ---------------------------------------------------------------------------
// Request notifications.
// ---------------------------------------------------------------------------
//
TInt CMpmVpnToggleWatcher::RequestNotifications()
    {
    MPMLOGSTRING( "CMpmVpnToggleWatcher::RequestNotifications" )

    TInt err = iRepository->NotifyRequest( KMpmVpnTogglePreferVpn, iStatus );
        
    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        MPMLOGSTRING2( "CMpmVpnToggleWatcher::RequestNotifications, ERROR: %d", err )
        }
    return err;
    }

// ---------------------------------------------------------------------------
// Get VPN toggle values.
// ---------------------------------------------------------------------------
//
TInt CMpmVpnToggleWatcher::GetVpnToggleValues()
    {
    MPMLOGSTRING( "CMpmVpnToggleWatcher::GetVpnToggleValues" )
            
    // Get values from central repository    
    TInt err = iRepository->Get( KMpmVpnTogglePreferVpn, iVpnConnectionPreferred );
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmVpnToggleWatcher::GetVpnToggleValues, preferred value, ERROR: %d", err )    
        return err;
        }    
    TInt value(0);    
    err = iRepository->Get( KMpmVpnToggleIapId, value );
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmVpnToggleWatcher::GetVpnToggleValues, IAP Id value, ERROR: %d", err )
        return err;
        }    
    iVpnIapId = value;   
    err = iRepository->Get( KMpmVpnToggleSnapId, value );
    if ( err != KErrNone )
        {
        MPMLOGSTRING2( "CMpmVpnToggleWatcher::GetVpnToggleValues, SNAP Id value, ERROR: %d", err )
        return err;
        }
    iSnapId = value;        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Return VPN toggle value.
// ---------------------------------------------------------------------------
//
TBool CMpmVpnToggleWatcher::IsVpnConnectionPreferred() const
    {
    return iVpnConnectionPreferred;
    }

// ---------------------------------------------------------------------------
// Return VPN IAP Id.
// ---------------------------------------------------------------------------
//
TUint32 CMpmVpnToggleWatcher::VpnIapId() const
    {
    return iVpnIapId;
    }

// ---------------------------------------------------------------------------
// Return SNAP Id.
// ---------------------------------------------------------------------------
//
TUint32 CMpmVpnToggleWatcher::SnapId() const
    {
    return iSnapId;
    }

// ---------------------------------------------------------------------------
// Resets VPN toggle values.
// ---------------------------------------------------------------------------
//
void CMpmVpnToggleWatcher::ResetVpnToggleValues()
    {
    // Cancel listening VPN toggle value change
    Cancel();

    // Reset values.
    iVpnConnectionPreferred = EFalse;
    iVpnIapId = 0;
    iSnapId = 0;
    
    TInt err = iRepository->Set( KMpmVpnTogglePreferVpn, 0 );
    if ( err == KErrNone ) 
        {
        err = iRepository->Set( KMpmVpnToggleIapId, 0 );
        }
    if ( err == KErrNone )
        {
        err = iRepository->Set( KMpmVpnToggleSnapId, 0 );
        }
    
    MPMLOGSTRING2( "CMpmVpnToggleWatcher::ResetVpnToggleValues, ERROR: %d", err )
    
    // Restart listening VPN toggle value change
    RequestNotifications();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// When there is a change in VPN toggle key value, event is received in here
// ---------------------------------------------------------------------------
//
void CMpmVpnToggleWatcher::RunL()
    {            
    if ( iStatus.Int() < KErrNone )
        {
        MPMLOGSTRING2("CMpmVpnToggleWatcher::RunL, status: 0x%08X", iStatus.Int())
        iErrorCounter++;
        if ( iErrorCounter > KMpmVpnToggleWatcherCenRepErrorThreshold )
            {
            MPMLOGSTRING2("Over %d consecutive errors, stopping notifications permanently.",
                    KMpmVpnToggleWatcherCenRepErrorThreshold)
            return;
            }
        // Else: Error occured but counter not expired. Proceed.
        RequestNotifications();
        }
    else
        {
        // Notification is received ok => Reset the counter.
        iErrorCounter = 0;
    
        RequestNotifications();
        
        // Get values from central repository
        GetVpnToggleValues();
        
        // Notify values.
        TRAP_IGNORE( iNotify.SetVpnToggleValuesL( iVpnConnectionPreferred,
                                                  iVpnIapId,
                                                  iSnapId ) );        
        }    
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Cancel outstanding request
// ---------------------------------------------------------------------------
//
void CMpmVpnToggleWatcher::DoCancel()
    {
    iRepository->NotifyCancel( KMpmVpnTogglePreferVpn );
    }
