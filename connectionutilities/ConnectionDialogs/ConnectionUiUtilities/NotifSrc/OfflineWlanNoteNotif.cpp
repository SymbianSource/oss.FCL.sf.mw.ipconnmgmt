/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of COfflineWlanNoteNotif.
*
*/


// INCLUDE FILES
#include "OfflineWlanNoteNotif.h"
#include "OfflineWlanNoteDlg.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnUiUtilsNotif.h"
#include "ConnectionDialogsLogger.h"

#include <e32property.h>         // For RProperty 
#include <UikonInternalPSKeys.h> // For KPSUidUikon and KUikGlobalNotesAllowed.
#include <connuiutilsnotif.rsg>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// COfflineWlanNoteNotif::TNotifierInfo COfflineWlanNoteNotif::RegisterL()
// ---------------------------------------------------------
//
COfflineWlanNoteNotif::TNotifierInfo COfflineWlanNoteNotif::RegisterL()
    {
    iInfo.iUid = KUidCOfflineWlanNoteDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCOfflineWlanNoteDlg;

    return iInfo;
    }

// ---------------------------------------------------------
// void COfflineWlanNoteNotif::StartL
// ---------------------------------------------------------
//
void COfflineWlanNoteNotif::StartL( const TDesC8& aBuffer, 
                                    TInt aReplySlot,
                                    const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "COfflineWlanNoteNotif::StartL" );

    TPckgBuf<TBool> asyncVersion;
    asyncVersion.Copy( aBuffer );

    iCancelled = EFalse;

    // We are about to display the Offline note.
    // Since this part of the code can be executed during the bootup, check if
    // the UI has really started up to display notes/dialogs.
    TInt notesAllowed = 0;
    User::LeaveIfError ( RProperty::Get( KPSUidUikon, KUikGlobalNotesAllowed,
                                        notesAllowed ) );

    iReplySlot = aReplySlot;
    iMessage = aMessage;

    CLOG_WRITEF( _L( "notesAllowed : %d" ), notesAllowed );

    if ( notesAllowed )
        {
        GetDialogL();
        }
    else if ( asyncVersion() )
        {
        CLOG_WRITE( "Completing with KErrNotReady" );
        CompleteL( KErrNotReady );
        }
    else
        {
        CLOG_WRITE( "Leaving with KErrNotReady" );
        iCancelled = ETrue;
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------
// void COfflineWlanNoteNotif::Cancel()
// ---------------------------------------------------------
//
void COfflineWlanNoteNotif::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        
        delete iDialog;
        iDialog = NULL;
        }
    }


// ---------------------------------------------------------
// void COfflineWlanNoteNotif::GetDialogL()
// ---------------------------------------------------------
//
void COfflineWlanNoteNotif::GetDialogL()
    {
    iDialog = new ( ELeave ) COfflineWlanNoteDlg( this );
    iDialog->ExecuteLD( R_QUERY_DIALOG );
    }


// ---------------------------------------------------------
// void COfflineWlanNoteNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void COfflineWlanNoteNotif::CompleteL( TInt aStatus )
    {    
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    }


// ---------------------------------------------------------
// COfflineWlanNoteNotif* COfflineWlanNoteNotif::NewL()
// ---------------------------------------------------------
//
COfflineWlanNoteNotif* COfflineWlanNoteNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    COfflineWlanNoteNotif* self = new ( ELeave ) COfflineWlanNoteNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// End of File
