/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM disconnect dialog handling
*
*/

/**
@file mpmdisconnectdlg.cpp
Mobility Policy Manager disconnect dialog initiation.
*/

// INCLUDE FILES
#include "mpmdisconnectdlg.h"
#include "mpmconnmonevents.h"
#include "mpmlogger.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::NewL
// -----------------------------------------------------------------------------
//
CMPMDisconnectDlg* CMPMDisconnectDlg::NewL(
    CMPMServerSession&                aSession,
    TInt                              aOrigError,
    CArrayPtrFlat<CMPMDisconnectDlg>& aDisconnectQueue )
    {
    CMPMDisconnectDlg* self = new (ELeave) CMPMDisconnectDlg( aSession,
                                                              aOrigError,
                                                              aDisconnectQueue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::CMPMDisconnectDlg
// -----------------------------------------------------------------------------
//
CMPMDisconnectDlg::CMPMDisconnectDlg(
    CMPMServerSession&                aSession,
    TInt                              aOrigError,
    CArrayPtrFlat<CMPMDisconnectDlg>& aDisconnectQueue )
    :   CMPMDialogBase( *(CArrayPtrFlat<CMPMDialogBase>*)&aDisconnectQueue ), 
        iSession( aSession ),
        iOrigError( aOrigError )
    {
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::~CMPMDisconnectDlg
// -----------------------------------------------------------------------------
//
CMPMDisconnectDlg::~CMPMDisconnectDlg()
    {
    MPMLOGSTRING( "CMPMDisconnectDlg::~CMPMDisconnectDlg" )
    Cancel();
    iDlgServ.Close();
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::ConstructL()
    {
    User::LeaveIfError( iDlgServ.Connect() );
    CMPMDialogBase::ConstructL();
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::RunL
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::RunL()
    {
    MPMLOGSTRING2( "CMPMDisconnectDlg::RunL status %d", iStatus.Int() )

    TInt error = iStatus.Int();
    if ( error == KErrNone )
        {
        UserSelectedOk( iStatus.Int() );
        }
    else
        {
        UserSelectedCancel( iStatus.Int() );
        }
    // Store selection
    iUserStatus = iStatus.Int();
    iUserIap = iIapId;
    }

    
// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::UserSelectedOk
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::UserSelectedOk( TInt aError )
    {
    MPMLOGSTRING( "CMPMDisconnectDlg::UserSelectedOk: \
Tell BM to ignore error and do reselection" )

    TInt error = aError;

    // Get the current connection IapId for this connId 
    TUint32 iapId = iSession.MyServer().GetBMIap( iSession.ConnectionId() );
    
    TBMNeededAction neededActionForPendingMsgs( EIgnoreError );
    iSession.MyServer().HandlePendingMsgs( iapId, 
                                           KErrNone,
                                           &error,
                                           &neededActionForPendingMsgs );
										   
    TBMNeededAction neededAction( EDoReselection );
    iSession.ProcessErrorComplete( KErrNone,
                                   &error,
                                   &neededAction );
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::UserSelectedCancel
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::UserSelectedCancel( TInt aError )
    {
    // User has selected Cancel
    // 
    // Read the Connection Id of the application
    // 
    TConnectionId connId = iSession.ConnectionId();

    TBMNeededAction neededAction( EDoReselection );

    TInt error = aError;
    TInt* errorPtr( NULL );
    
    if ( error == KErrNotFound )
        {
        MPMLOGSTRING2( "CMPMDisconnectDlg::UserSelectedCancelL: \
Use original error code %i", iOrigError )

        error = iOrigError;
        }

    // Get the state of the connection for this Iap Id.
    // 
    TConnectionState state;
    iSession.MyServer().GetConnectionState( connId, state );

    // Get the current connection IapId for this connId 
    //
    TUint32 currentIap = iSession.MyServer().GetBMIap( connId );

    if ( state == EStarting )
        {
        MPMLOGSTRING2( "CMPMDisconnectDlg::UserSelectedCancelL: \
Tell BM to end the client connection with error code %i", error )

        neededAction = EPropagateError;
        errorPtr = &error;
        }
    else if ( state == ERoaming )
        {
        // New notification will be attempted with latest
        // available IAPs
        //
        iSession.StoredIapInfo().ResetStoredIapInfo();
        neededAction = EIgnoreError;

        TConnMonIapInfo availableIAPs;
        availableIAPs = iSession.GetAvailableIAPs();

        iSession.MyServer().HandleServerBlackListIap( connId, 
                                                      currentIap, 
                                                      ETemporary );
        TRAP( error, iSession.PrefIAPNotificationL( availableIAPs, 
                                                    EBearerMan ) );
        if ( error == KErrNotFound )
            {
            neededAction = EPropagateError;

            errorPtr = &error;
            TRAP_IGNORE( iSession.MobilityErrorNotificationL( KErrNotFound ) )
            MPMLOGSTRING2(
                "CMPMDisconnectDlg::UserSelectedCancelL: \
Tell BM to end the client connection with error code %i", error )
            }
        else
            {
            MPMLOGSTRING(
                "CMPMDisconnectDlg::UserSelectedCancelL: \
Tell BM to ignore error and let MPM notify application about preferred IAP" )
            }
        }
    else
        {
        MPMLOGSTRING2( "CMPMDisconnectDlg::UserSelectedCancelL: \
Unsupported state %d", state )
        }

    iSession.MyServer().HandlePendingMsgs( currentIap,
                                           KErrNone,
                                           errorPtr,
                                           &neededAction );

    iSession.ProcessErrorComplete( KErrNone,
                                   errorPtr,
                                   &neededAction );
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::RunError
// -----------------------------------------------------------------------------
//
TInt CMPMDisconnectDlg::RunError(TInt aError)
    {
    // Handles a leave occurring in the request completion event handler 
    // RunL(). The active scheduler calls this function if this active 
    // object's RunL() function leaves. This gives this active object 
    // the opportunity to perform any necessary cleanup.
    // 
    MPMLOGSTRING2(
        "CMPMDisconnectDlg::RunError: RunL made a leave with error = %i", 
        aError )

    // Get the current connection IapId for this connId 
    TUint32 iapId = iSession.MyServer().GetBMIap( iSession.ConnectionId() );

    iSession.MyServer().HandlePendingMsgs( iapId, aError, NULL, NULL );

    iSession.ProcessErrorComplete( aError, NULL, NULL );
	
    // Return KErrNone to prevent panic 
    // 
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::DoCancel()
    {
    MPMLOGSTRING( "CMPMDisconnectDlg::DoCancel" )
    iDlgServ.CancelDisconnectDlg();
    }


// -----------------------------------------------------------------------------
// CMPMDisconnectDlg::Start
// -----------------------------------------------------------------------------
//
void CMPMDisconnectDlg::Start( CMPMDialogBase* aDlg )
    {
    // If there are no outstanding CMPMDisconnectDlg, then 
    // start CMPMDisconnectDlg immediately.
    //
    if ( aDlg )
        {
        CMPMDisconnectDlg* dlg = (CMPMDisconnectDlg*) aDlg;
        iUserStatus = dlg->iUserStatus;
        iUserIap = dlg->iUserIap;
        }
    if ( iUserStatus == KErrNone && iUserIap != 0 )
        {
        UserSelectedOk( iUserStatus );
        }
    else if ( iUserStatus != KErrNone )
        {
        UserSelectedCancel( iUserStatus );
        }
    else
        {
        iDlgServ.DisconnectDlg( iStatus, iIapId, ETrue );
        SetActive();
        }
    }


//  End of File
