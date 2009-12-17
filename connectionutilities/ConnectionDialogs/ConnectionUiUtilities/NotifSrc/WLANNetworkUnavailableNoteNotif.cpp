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
* Description:  Implementation of CWLANNetworkUnavailableNoteNotif.
*
*/


// INCLUDE FILES
#include "WLANNetworkUnavailableNoteNotif.h"
#include "ActiveWLANNetworkUnavailableNote.h"
#include "ConnUiUtilsNotif.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWLANNetworkUnavailableNoteNotif::TNotifierInfo 
//                               CWLANNetworkUnavailableNoteNotif::RegisterL()
// ---------------------------------------------------------
//
CWLANNetworkUnavailableNoteNotif::TNotifierInfo 
                                 CWLANNetworkUnavailableNoteNotif::RegisterL()
    {
    iInfo.iUid = KUidWLANNetworkUnavailableNoteDlg;
    iInfo.iPriority = ENotifierPriorityVHigh;
    iInfo.iChannel = KUidWLANNetworkUnavailableNoteDlg;

    return iInfo;
    }


// ---------------------------------------------------------
// void CWLANNetworkUnavailableNoteNotif::StartL()
// ---------------------------------------------------------
//
void CWLANNetworkUnavailableNoteNotif::StartL( const TDesC8& /*aBuffer*/, 
                                              TInt aReplySlot,
                                              const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;
            
    iActiveNote = new( ELeave ) CActiveWLANNetworkUnavailableNote( this );
        
    iActiveNote->LaunchWLANNetworkUnavailable( 
                    CActiveWLANNetworkUnavailableNote::EWlanNote1 );       
    }
    
// ---------------------------------------------------------
// void CWLANNetworkUnavailableNoteNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CWLANNetworkUnavailableNoteNotif::CompleteL( TInt aStatus )
    {    
    delete iActiveNote;
    iActiveNote = NULL;
    
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    }    

// ---------------------------------------------------------
// CWLANNetworkUnavailableNoteNotif* CWLANNetworkUnavailableNoteNotif::NewL()
// ---------------------------------------------------------
//
CWLANNetworkUnavailableNoteNotif* CWLANNetworkUnavailableNoteNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CWLANNetworkUnavailableNoteNotif* self = 
                            new( ELeave ) CWLANNetworkUnavailableNoteNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }
    

// ---------------------------------------------------------
// CWLANNetworkUnavailableNoteNotif::CWLANNetworkUnavailableNoteNotif()
// ---------------------------------------------------------
//
CWLANNetworkUnavailableNoteNotif::CWLANNetworkUnavailableNoteNotif() 
: CConnectionDialogsNotifBase(), 
  iActiveNote( NULL )
{    
}    

// ---------------------------------------------------------
// void CWLANNetworkUnavailableNoteNotif::Cancel()
// ---------------------------------------------------------
//
void CWLANNetworkUnavailableNoteNotif::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        
        delete iActiveNote;
        iActiveNote = NULL;
        }
    }

// End of File
