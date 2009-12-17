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
* Description:  Implementation of COfflineWlanDisabledNoteNotif
*
*/


// INCLUDE FILES
#include "OfflineWlanDisabledNoteNotif.h"
#include "ActiveOfflineWlanDisabledNote.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnUiUtilsNotif.h"

    
// ---------------------------------------------------------
// COfflineWlanDisabledNoteNotif::COfflineWlanDisabledNoteNotif()
// ---------------------------------------------------------
//
COfflineWlanDisabledNoteNotif::COfflineWlanDisabledNoteNotif()
: CConnectionDialogsNotifBase(), 
  iActiveNote( NULL )
    {    
    }    


// ---------------------------------------------------------
// COfflineWlanDisabledNoteNotif* COfflineWlanDisabledNoteNotif::NewL()
// ---------------------------------------------------------
//
COfflineWlanDisabledNoteNotif* COfflineWlanDisabledNoteNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    COfflineWlanDisabledNoteNotif* self = 
                                new (ELeave) COfflineWlanDisabledNoteNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }    
    

// ---------------------------------------------------------
// COfflineWlanDisabledNoteNotif::TNotifierInfo 
//                                   COfflineWlanDisabledNoteNotif::RegisterL()
// ---------------------------------------------------------
//
COfflineWlanDisabledNoteNotif::TNotifierInfo 
                                     COfflineWlanDisabledNoteNotif::RegisterL()
    {
    iInfo.iUid = KUidCOfflineWlanDisabledNoteDlg;
    iInfo.iPriority = ENotifierPriorityVHigh;
    iInfo.iChannel = KUidCOfflineWlanDisabledNoteDlg;

    return iInfo;
    }


// ---------------------------------------------------------
// void COfflineWlanDisabledNoteNotif::StartL()
// ---------------------------------------------------------
//
void COfflineWlanDisabledNoteNotif::StartL( const TDesC8& /*aBuffer*/, 
                                              TInt aReplySlot,
                                              const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;
            
    iActiveNote = new( ELeave ) CActiveOfflineWlanDisabledNote( this );
    iActiveNote->LaunchOfflineWlanDisabledNote();       
    }

    
// ---------------------------------------------------------
// void COfflineWlanDisabledNoteNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void COfflineWlanDisabledNoteNotif::CompleteL( TInt aStatus )
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
// void COfflineWlanDisabledNoteNotif::Cancel()
// ---------------------------------------------------------
//
void COfflineWlanDisabledNoteNotif::Cancel()
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
