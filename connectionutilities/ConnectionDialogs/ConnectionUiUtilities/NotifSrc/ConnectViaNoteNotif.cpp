/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CConnectViaNoteNotif.
*
*/


// INCLUDE FILES
#include "ConnectViaNoteNotif.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ActiveConnectViaNote.h"
#include "ConnUiUtilsNotif.h"
#include "ConnectionDialogsLogger.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectViaNoteNotif::TNotifierInfo CConnectViaNoteNotif::RegisterL()
// ---------------------------------------------------------
//
CConnectViaNoteNotif::TNotifierInfo CConnectViaNoteNotif::RegisterL()
    {
    iInfo.iUid = KUidConnectViaNote;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidConnectViaNote;

    return iInfo;
    }

// ---------------------------------------------------------
// void CConnectViaNoteNotif::StartL
// ---------------------------------------------------------
//
void CConnectViaNoteNotif::StartL( const TDesC8& aBuffer, 
                                    TInt aReplySlot,
                                    const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "CConnectViaNoteNotif::StartL" );  

    TPckgBuf<TConnUiUiDestConnMethodNoteId> input;
    input.Copy( aBuffer );
    TConnUiUiDestConnMethodNoteId info = input();
    
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    iActiveNote = new( ELeave ) CActiveConnectViaNote( this, info );
        
    iActiveNote->LaunchActiveConnectViaNote();       
    }

// ---------------------------------------------------------
// void CConnectViaNoteNotif::Cancel()
// ---------------------------------------------------------
//
void CConnectViaNoteNotif::Cancel()
    {
    CLOG_ENTERFN( "CConnectViaNoteNotif::Cancel" );  

    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {   // No need to return an error code, we are just completing!
            CLOG_WRITEF( _L( "Completing CConnectViaNoteNotif with KErrNone" ) );
            iMessage.Complete( KErrNone );
            }
        
        delete iActiveNote;
        iActiveNote = NULL;
        }

    CLOG_LEAVEFN( "CConnectViaNoteNotif::Cancel" );  
    }

// ---------------------------------------------------------
// void CConnectViaNoteNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnectViaNoteNotif::CompleteL( TInt aStatus )
    {    
    CLOG_ENTERFN( "CConnectViaNoteNotif::CompleteL" );  
    CLOG_WRITEF( _L( "aStatus: %d" ),  aStatus );

    delete iActiveNote;
    iActiveNote = NULL;
    iCancelled = ETrue;    
    if ( !iMessage.IsNull() )
        {
        CLOG_WRITEF( _L( "Completing CConnectViaNoteNotif with %d" ),  aStatus );
        iMessage.Complete( aStatus );
        }
    CLOG_LEAVEFN( "CConnectViaNoteNotif::CompleteL" );  
    }


// ---------------------------------------------------------
// CConnectViaNoteNotif* CConnectViaNoteNotif::NewL()
// ---------------------------------------------------------
//
CConnectViaNoteNotif* CConnectViaNoteNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnectViaNoteNotif* self = new ( ELeave ) CConnectViaNoteNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();
    return self;
    }


// End of File
