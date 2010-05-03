/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handles displaying wlan dialogs
*
*/


#include <wlanmgmtcommon.h>
#include <e32std.h>
#include <utf.h>
#include <cmpluginwlandef.h>
#include <ctsydomainpskeys.h>

#include "mpmwlanquerydialog.h"
#include "mpmiapselection.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::CMPMWlanQueryDialog
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog::CMPMWlanQueryDialog( CMPMIapSelection&  aIapSelection,
                                          TUint32            aWlanIapId )
  : CActive( CActive::EPriorityStandard ),
    iIapSelection( aIapSelection ),
    iWlanQueryState( EOffline ),
    iWlanIapId( aWlanIapId ),
    iOverrideStatus( KErrNone )    
    {
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::ConstructL
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::ConstructL()
    {
    User::LeaveIfError(iNotifier.Connect());
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::NewL
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog* CMPMWlanQueryDialog::NewL( CMPMIapSelection&  aSession,
                                                TUint32            aWlanIapElementId )
    {
    CMPMWlanQueryDialog* self = new( ELeave ) CMPMWlanQueryDialog( aSession,
                                                                   aWlanIapElementId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::~CMPMWlanQueryDialog
// ---------------------------------------------------------------------------
//
CMPMWlanQueryDialog::~CMPMWlanQueryDialog()
    {
    MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog enters" )

    // Check if this dialog instance was not started but only added to the queue
    if ( iIapSelection.Session()->MyServer().FirstInWlanQueryQueue() != this )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog, not a active delete" )

        // Close notifier
        iNotifier.Close();
        
        // We're not first in the queue, thus we can just delete.
        // But remember the pointer in the array.
        iIapSelection.Session()->MyServer().RemoveFromWlanQueryQueue( this );
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog exits (break)" )
        return;
        }

    // Cancel previous dialogs if any.
    Cancel();

    // Close notifier
    iNotifier.Close();

    // Remove self from the queue
    iIapSelection.Session()->MyServer().RemoveFromWlanQueryQueue( this );

    // Start the next query
    CMPMWlanQueryDialog* dlg = iIapSelection.Session()->MyServer().FirstInWlanQueryQueue();
    if ( dlg )
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog starts new dialog" )
        dlg->OfferInformation( iStatus.Int() );
        // In destructor we cannot let the query leave
        TRAPD( err, dlg->StartWlanQueryL(); )
        if ( err != KErrNone )
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog caught Leave %d, executing RunError()", err )
            dlg->RunError( err );
            }
        }

    MPMLOGSTRING( "CMPMWlanQueryDialog::~CMPMWlanQueryDialog exits" )
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::DoCancel
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::DoCancel()
    {
    MPMLOGSTRING2( "CMPMWlanQueryDialog::DoCancel state %d", iWlanQueryState )
    if ( iWlanQueryState == EOffline )
        {
        iNotifier.CancelNotifier( KUidCOfflineWlanNoteDlg );
        }
    }
    
// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::RunL
// ---------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::RunL()
    {
    MPMLOGSTRING3( "CMPMWlanQueryDialog::RunL status %d state %d", 
                   iStatus.Int(), 
                   iWlanQueryState )

    if( iWlanQueryState == EOffline )
        {
        if( iStatus.Int() == KErrNone )
            {
            iIapSelection.Session()->MyServer().SetOfflineWlanQueryResponse(
                    EOfflineResponseYes );
            }
        else if ( iStatus.Int() == KErrCancel )
            {
            iIapSelection.Session()->MyServer().SetOfflineWlanQueryResponse(
                    EOfflineResponseNo );
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL offline query returned %d", 
                           iStatus.Int() )
            }
        else
            {
            MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL offline query returned %d", 
                           iStatus.Int() )
            }
        }
    // if an error was given through OfferInformation() -call we abort the execution.
    //
    else if ( iOverrideStatus != KErrNone )
        {
        MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery inherited error %d", iOverrideStatus )
        iIapSelection.UserWlanSelectionDoneL( iOverrideStatus, iWlanIapId );
        }
    else
        {
		MPMLOGSTRING2( "CMPMWlanQueryDialog::RunL, unknown state: %d", iWlanQueryState )
        User::Leave( KErrCancel );
        }
        
    iIapSelection.UserWlanSelectionDoneL( iStatus.Int(), iWlanIapId );
    
    iWlanIapId = 0;
    }

// ---------------------------------------------------------------------------
// CMPMWlanQueryDialog::RunError
// ---------------------------------------------------------------------------
//
TInt CMPMWlanQueryDialog::RunError( TInt aError )
    {
    MPMLOGSTRING2( "CMPMWlanQueryDialog::RunError failed with %d", aError )
    iIapSelection.ChooseIapComplete( aError, NULL );    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::StartWlanQueryL
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::StartWlanQueryL()
    {
    MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery" )
    
    TUint32 activeWlanIap = iIapSelection.Session()->MyServer().IsWlanConnectionStartedL( 
        iIapSelection.Session()->MyServer().CommsDatAccess() );

    // Get EmergencyCallInfo via Publish & Subscribe
    // 
    TInt emergencyCallEstablished( 0 );
    RProperty::Get( KPSUidCtsyEmergencyCallInfo, 
                    KCTSYEmergencyCallInfo, 
                    emergencyCallEstablished );  
    MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery KCTSYEmergencyCallInfo = %d", 
                   emergencyCallEstablished )

    // Get note behaviour setting
    TUint32 noteBehaviour( 0 );
    noteBehaviour = iIapSelection.MpmConnPref().NoteBehaviour();
    MPMLOGSTRING2( "CMPMWlanQueryDialog::StartWlanQuery noteBehaviour = %d", noteBehaviour )
    
    if( !emergencyCallEstablished && 
        iIapSelection.Session()->MyServer().IsPhoneOffline() && 
        !activeWlanIap && 
        iIapSelection.Session()->MyServer().OfflineWlanQueryResponse() != EOfflineResponseYes &&
        iOverrideStatus == KErrNone )
        {
        if ( noteBehaviour & TExtendedConnPref::ENoteBehaviourConnDisableQueries )
            {
            MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery offline note query not shown due to disabled queries" )
            iIapSelection.UserWlanSelectionDoneL( KErrPermissionDenied, iWlanIapId );
            }
        else
            {
            MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery, starting offline note" )            
            iWlanQueryState = EOffline;
            iNotifier.StartNotifierAndGetResponse( iStatus, 
                                                   KUidCOfflineWlanNoteDlg, 
                                                   KNullDesC8(), 
                                                   iOfflineReply );
            SetActive();
            }
        }
    else
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::StartWlanQuery no wlan dialog to show" )
        iIapSelection.UserWlanSelectionDoneL( KErrNone, iWlanIapId );
        }
    }

// -----------------------------------------------------------------------------
// CMPMWlanQueryDialog::OfferInformation
// -----------------------------------------------------------------------------
//
void CMPMWlanQueryDialog::OfferInformation( TInt aDialogStatus )
    {
    TOfflineWlanQueryResponse offlineResponse =
            iIapSelection.Session()->MyServer().OfflineWlanQueryResponse();
    if ( offlineResponse != EOfflineResponseUndefined )
        {
        MPMLOGSTRING3( "CMPMWlanQueryDialog<0x%x>::OfferInformation: offline response %d",
                       iIapSelection.Session()->ConnectionId(),
                       offlineResponse )
        iOverrideStatus = aDialogStatus;
        }

#ifdef _LOG
    else
        {
        MPMLOGSTRING( "CMPMWlanQueryDialog::OfferInformation, information not taken." ) 
        }
#endif 
    }

