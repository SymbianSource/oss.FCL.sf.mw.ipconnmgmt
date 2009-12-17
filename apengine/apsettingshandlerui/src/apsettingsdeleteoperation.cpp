/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CApSettingsDeleteOperation
*
*/


// INCLUDE FILES
#include <ApDataHandler.h>
#include <ApUtils.h>
#include <ActiveApDb.h>
#include <ApSettingsHandlerCommons.h>
#include <VpnApEngine.h>
#include <AknWaitDialog.h>
#include <AknProgressDialog.h>
#include <cdbcols.h>
#include <ErrorUI.h>
#include "Apsettingsdeleteoperation.h"

#include <WEPSecuritySettingsUI.h>
#include <WPASecuritySettingsUI.h>
#include <ApAccessPointItem.h>

// Following includes are needed only for the test
// when ALL APs are deleted (checking delays, etc.)
// For testing purpose only, to make process time longer.
//  TEST STARTS
#include <ActiveApDb.h>
#include <ApSelect.h>
#include <ApListItem.h>
#include "ApSelectorListBoxModel.h"
#include "ApSettingsHandlerLogger.h"

#include <ApSetUI.rsg>
//  TEST ENDS


// Stack size of the receiver thread.
LOCAL_C const TInt KDeleteThreadStackSize = 40960;

// Name of the receiver thread.
_LIT( KDeleteThreadName, "Access Point Settings UI Delete Operation" );


// ---------------------------------------------------------
// CApSettingsDeleteOperation::NewL
// ---------------------------------------------------------
//
CApSettingsDeleteOperation* CApSettingsDeleteOperation::NewL(
                                TInt aNoteControlId,
                                TInt aResourceID,
                                TUint32 aUid,
                                const TDesC& aNoteText
                                )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::NewL")
    
    CApSettingsDeleteOperation* db =
            new ( ELeave ) CApSettingsDeleteOperation(
                            aNoteControlId,
                            aResourceID,
                            aUid
                            );
    CleanupStack::PushL( db );
    db->ConstructL( aNoteText );
    CleanupStack::Pop();
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::NewL")
    return db;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::ConstructL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::ConstructL( const TDesC& aNoteText )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::ConstructL")
    
    iErrorUi = CErrorUI::NewL( *CCoeEnv::Static() );
    iNoteText = aNoteText.AllocL();
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::ConstructL")
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::CApSettingsDeleteOperation
// ---------------------------------------------------------
//
CApSettingsDeleteOperation::CApSettingsDeleteOperation(
                                 TInt aNoteControlId,
                                 TInt aResourceID,
                                 TUint32 aUid
                                 )
: CActive( EPriorityLow ),
  iNoteControlId( aNoteControlId ),
  iResourceID( aResourceID ),
  iUid( aUid )
    {
    CActiveScheduler::Add(this);
    APSETUILOGGER_WRITE( EModel,"Thread added to scheduler");
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::~CApSettingsDeleteOperation
// ---------------------------------------------------------
//
CApSettingsDeleteOperation::~CApSettingsDeleteOperation()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::~CApSettingsDeleteOperation")
    
    if(iDeleteOperation.Handle() != 0)
        {
        // wait for deletion to finish
        TRequestStatus status;          
        iDeleteOperation.Logon(status);
        User::WaitForRequest(status);
        
        iDeleteOperation.Close();
        }
        
    Cancel();                        
    
    delete iWaitDialog;

    delete iNoteText;
    delete iErrorUi;
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::~CApSettingsDeleteOperation")
    }




// ---------------------------------------------------------
// CApSettingsDeleteOperation::RunL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::RunL()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::RunL")
    
    switch ( iState )
        {
        case EProcOnDialogOn:
            { // two chances :
            // 1. Process finished normally
            // 2. Process leaved with error
            iState = EProcOffDialogOn;
            if ( iWaitDialog )
                {
                iWaitDialog->ProcessFinishedL();
                }
            if ( iStatus.Int() != KErrNone )
                {
                // some error has occured
                iErr = iStatus.Int();
                }
            Next();
            break;
            }
        case EProcOffDialogOn:
            {
            // now we wait for the dialog to be dismissed
            // just to be able to delete it
            // stays in this state as long as DialogDismissedL()
            // is not called, it will move it out into
            // EProcOffDialogOff and it also deletes
            Next();
            break;
            }
        case EProcOnDialogOff:
            { // this can't be, as dialog MUST stay as long as operation...
            iState = EProcOffDialogOff;
            Next();
            break;
            }
        case EProcOffDialogOff:
            { // now we can safely delete the dialog
            if ( iWaitDialog )
                {
                delete iWaitDialog;
                }
            if ( iErr != KErrNone )
                {
                // some error has occured
                iErrorUi->ShowGlobalErrorNoteL( iErr );
                }
            iDeleteOperation.Kill( iErr );
            iDeleteOperation.Close();            
//            delete this;
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidState ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::RunL")
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::DoCancel
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::DoCancel()
    { // must be here as it is pure virtual in base...    
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::DoCancel")
    
    if (iDeleteOperation.Handle() != 0 && iStatus == KRequestPending)
        {
        iDeleteOperation.LogonCancel(iStatus);
        } 
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::DoCancel")
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::DialogDismissedL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::DialogDismissedL( TInt /*aButtonId*/ )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::DialogDismissedL")
    
    switch ( iState )
        {
        case EProcOnDialogOn:
            { // can't be, it means that dialog is dismissed by user
            // but process is still runing...
            iState = EProcOnDialogOff;
            break;
            }
        case EProcOffDialogOn:
            {
            // set dialog off
            iState = EProcOffDialogOff;
            // as proc is already off, it circles in RunL,
            break;
            }
        case EProcOnDialogOff:
            { // can't be, it is already off...
            iState = EProcOnDialogOff;
            Next();
            break;
            }
        case EProcOffDialogOff:
            {// can't be, it is already off...
            iState = EProcOffDialogOff;
            Next();
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidState ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::DialogDismissedL")
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartL()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::StartL")
    
    iStatus = KRequestPending;
    // set the internal state as both is running
    iState = EProcOnDialogOn;

    // now we can start wait note...
    StartWaitNoteL();

    // now start new thread with operation
    StartDeleteOperationThreadL();

    SetActive();
    // now we can wait on it...
    // wait until the started thread completes (delete operation completes)
    // and it will enter RunL. RunL will notify the wait dialog about the
    // completion of the process.
    // As the dialog shall wait ~0.5 sec till display AND must be visible
    // for ~1.5 sec. as a minimum, but must stay up as long as the 'process'
    // on it is waiting for does not completes, we have the following chances:
    // if the process is shorter than 0.5,
        // after operation completes, state moves to EProcOffDialogOn,
        // dialog will be notified about completion by RunL, and RunL starts
        // circle and wait for the dialog to finish, multiply re-entering
        // and leaving RunL, as long as the state is EProcOffDialogOn.
        // As time was shorter than 0.5, thus dialog will not show itself
        // and will call DialogDismissedL, which will change the
        // state to EProcOffDialogOff, which, on entering
        // RunL again, will delete the dialog and kill the thread.
    // if the process was longer than 0.5 but shorter than 0.5+1.5sec,
        // after operation completes, state moves to EProcOffDialogOn,
        // dialog will be notified about completion by RunL, and RunL starts
        // circle and wait for the dialog to finish, multiply re-entering
        // and leaving RunL, as long as the state is EProcOffDialogOn.
        // As time was longer than 0.5, thus dialog will show itself
        // and will stay up for 1.5 sec, as process ended between
        // 0.5 and (0.5+1.5). After it dialog will call DialogDismissedL,
        // which will change the state to EProcOffDialogOff, which, on
        // entering RunL again, will delete the dialog and kill the thread.
    // if the process was longer than 0.5+1.5
        // after operation completes, state moves to EProcOffDialogOn,
        // dialog will be notified about completion by RunL, and RunL starts
        // circle and wait for the dialog to finish, multiply re-entering
        // and leaving RunL, as long as the state is EProcOffDialogOn.
        // As time was longer than 0.5+1.5, thus dialog will show itself
        // and will stay up for as long as 'process/operation' is not finished.
        // As thread completes, notifies dialog, after it dialog will
        // call back DialogDismissedL, which will change the state
        // to EProcOffDialogOff, which, on entering RunL again,
        // will delete the dialog and kill the thread.
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::StartL")
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::Next
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::Next()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::Next")
    
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus *status = &iStatus;
    User::RequestComplete( status, KErrNone );  // Invoke RunL()
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::Next")
    }




// ---------------------------------------------------------
// CApSettingsDeleteOperation::RunDeleteThread
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::RunDeleteThread( TAny* aPtr )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::RunDeleteThread")
    
    CApSettingsDeleteOperation* aop = 
            STATIC_CAST( CApSettingsDeleteOperation*, aPtr );


    CTrapCleanup* TheTrapCleanup = CTrapCleanup::New(); // get cleanup stack

    TRAPD(r, aop->DoRunDeleteThreadL( aop ) );

    delete TheTrapCleanup; // destroy cleanup stack

    aop->iErr = r;
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::RunDeleteThread")
    return ( r );
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::DoRunDeleteThreadL
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::DoRunDeleteThreadL( 
                                    CApSettingsDeleteOperation* aop )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::DoRunDeleteThreadL")
    
    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( db );
    
    APSETUILOGGER_WRITE( EDelete,"Commsdb created");

    CApDataHandler* handler = CApDataHandler::NewLC( *db );
    
    APSETUILOGGER_WRITE( EDelete,"CApDataHandler created");
    
    CVpnApEngine* vpnengine = CVpnApEngine::NewLC( db );

    APSETUILOGGER_WRITE( EDelete,"CVpnApEngine created");

    TBool ownTransaction = StartTransactionLC( db, 
                                               /*aWrite*/ETrue, 
                                               /*aRetry*/ETrue );

    APSETUILOGGER_WRITE( EDelete,"Tr.started");

//  NORMAL OPERATION STARTS
    TBool isvpn = vpnengine->IsVpnApL( aop->iUid );

    APSETUILOGGER_WRITE( EDelete,"IsVpnApL returned");
    
    // if linked to a VPN, the VPN also must be deleted
    // deleting the VPN part is the delete operation's responsibility...
    if ( isvpn )
        {
        vpnengine->DeleteVpnL( aop->iUid );
        }
    else
        {
        handler->RemoveAPL( aop->iUid );
        }
    
    APSETUILOGGER_WRITE( EDelete,"RemoveAPL returned");


    if ( ownTransaction )
        {
        TInt err = db->CommitTransaction();
        User::LeaveIfError( err );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    APSETUILOGGER_WRITE( EDelete," delete finished");

    CleanupStack::PopAndDestroy( 3, db ); //db,handler,vpnengine

//  NORMAL OPERATION ENDS


    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::DoRunDeleteThreadL")
    return KErrNone;
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartWaitNoteL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartWaitNoteL()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::StartWaitNoteL")
    
    if ( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
    iWaitDialog =
        new ( ELeave ) CAknWaitDialog
                ( REINTERPRET_CAST(CEikDialog**,&iWaitDialog) );
    iWaitDialog->PrepareLC( iResourceID );
    iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
    iWaitDialog->SetTextL( *iNoteText );
    iWaitDialog->SetCallback( this );
    iWaitDialog->RunLD();
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::StartWaitNoteL")
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartDeleteOperationThreadL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartDeleteOperationThreadL()
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::StartDeleteOperationThreadL")
    
    TInt err = iDeleteOperation.Create
            (
            KDeleteThreadName, // aName
            RunDeleteThread, // aFunction
            KDeleteThreadStackSize, // aStackSize
            NULL,         // Use this heap.
            this // aPtr
            );
    APSETUILOGGER_WRITE_FORMAT( EDelete, "iDeleteOperation create returned, code:%d", err);    
    
    User::LeaveIfError( err );
    // on termination/death of this thread, it will call RunL
    iDeleteOperation.Logon( iStatus );
    // let's start the thread
    iDeleteOperation.Resume();
    // set the internal state as both is running
    iState = EProcOnDialogOn;
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::StartDeleteOperationThreadL")
    }





// ---------------------------------------------------------
// CApSettingsDeleteOperation::StarTransactionLC
// ---------------------------------------------------------
//
TBool CApSettingsDeleteOperation::StartTransactionLC( CCommsDatabase* aDb,
                                                      TBool aWrite, 
                                                      TBool aRetry /*ETrue*/ )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::StartTransactionLC")
    
    TBool retval( EFalse );
    if ( !aDb->InTransaction() )
        {
        TInt retriesLeft( aRetry ? KRetryCount : 1 );
        TInt err = aDb->BeginTransaction();
        while ( ( err == KErrLocked ) && --retriesLeft )
            {
            User::After( KRetryWait );
            err = aDb->BeginTransaction();
            if (aWrite )
                {
                TRAP_IGNORE( GetWriteLockL( aDb ) );
                }
            }
        if ( err )
            {
            ShowNoteL( R_APUI_VIEW_TEXT_CANNOT_ACCESS_DB );
            }
        User::LeaveIfError( err );
        CleanupStack::PushL( TCleanupItem( RollbackTransactionOnLeave, aDb ) );
        retval = ETrue;
        }
    else
        {
        retval = EFalse;
        }
        
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::StartTransactionLC")
    return retval;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::GetWriteLockL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::GetWriteLockL( CCommsDatabase* aDb )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::GetWriteLockL")
    
    CCommsDbTableView* v = aDb->OpenTableLC( TPtrC(WAP_ACCESS_POINT ) );
    TUint32 aDummyUid( KApNoneUID );
    TInt err = v->InsertRecord( aDummyUid );
    User::LeaveIfError( err );
    v->CancelRecordChanges();
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy(); // v
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::GetWriteLockL")
    }

// ---------------------------------------------------------
// CApSettingsDeleteOperation::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::RollbackTransactionOnLeave( TAny* aDb )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::RollbackTransactionOnLeave")
    
    CCommsDatabase* db = STATIC_CAST( CCommsDatabase*, aDb );
    db->RollbackTransaction();
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::RollbackTransactionOnLeave")
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::ReadUintL
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::ReadUintL( CCommsDbTableView* atable,
                                            const TDesC& aColumn,
                                            TUint32& aValue )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::ReadUintL")
    
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    TRAPD( err, atable->ReadUintL( aColumn, aValue ) );
    if ( err != KErrNone )
        { // do not leave if readed value is not present in table...
        if ( err != KErrUnknown )
            User::Leave( err );
        }
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::ReadUintL")
    return err;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::SetNoteTextL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::SetNoteTextL( const TDesC& aNoteText )
    {
    APSETUILOGGER_ENTERFN( EDelete,"DeleteOp::SetNoteTextL")
    
    delete iNoteText;
    iNoteText = NULL; // must NULL it to satisfy CodeScanner...
    iNoteText = aNoteText.AllocL();
    
    APSETUILOGGER_LEAVEFN( EDelete,"DeleteOp::SetNoteTextL")
    }
// End of File
