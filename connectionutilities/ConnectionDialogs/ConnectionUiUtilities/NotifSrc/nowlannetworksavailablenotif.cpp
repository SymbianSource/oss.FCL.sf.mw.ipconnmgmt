/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CNoWLANNetworksAvailableNotif.
*
*/

// INCLUDE FILES
#include "ConnectionDialogsUidDefs.h"
#include "nowlannetworksavailablenotif.h"
#include "nowlansdiscreetpopup.h"
#include "ConnUiUtilsNotif.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNoWLANNetworksAvailableNotif::TNotifierInfo 
//                 CNoWLANNetworksAvailableNotif::RegisterL()
// ---------------------------------------------------------
//
CNoWLANNetworksAvailableNotif::TNotifierInfo 
                   CNoWLANNetworksAvailableNotif::RegisterL()
    {
    iInfo.iUid      = KUidNoWLANNetworksAvailableNote;
    iInfo.iPriority = ENotifierPriorityVHigh;
    iInfo.iChannel  = KUidNoWLANNetworksAvailableNote;

    return iInfo;
    }


// ---------------------------------------------------------
// void CNoWLANNetworksAvailableNotif::StartL()
// ---------------------------------------------------------
//
void CNoWLANNetworksAvailableNotif::StartL( const TDesC8& /*aBuffer*/, 
                                            TInt aReplySlot,
                                            const RMessagePtr2& aMessage )
    {
    if ( iActiveNote )
        {
        // Note is already active
        aMessage.Complete( KErrNone );
        return;
        }
    
    iReplySlot = aReplySlot;
    iMessage   = aMessage;
    iCancelled = EFalse;
            
    iActiveNote = CNoWlansDiscreetPopup::NewL( this );
        
    iActiveNote->StartL();       
    }
    
// ---------------------------------------------------------
// void CNoWLANNetworksAvailableNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CNoWLANNetworksAvailableNotif::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    }    

// ---------------------------------------------------------
// CNoWLANNetworksAvailableNotif* CNoWLANNetworksAvailableNotif::NewL()
// ---------------------------------------------------------
//
CNoWLANNetworksAvailableNotif* CNoWLANNetworksAvailableNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CNoWLANNetworksAvailableNotif* self = 
                            new( ELeave ) CNoWLANNetworksAvailableNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }
    

// ---------------------------------------------------------
// CNoWLANNetworksAvailableNotif::CNoWLANNetworksAvailableNotif()
// ---------------------------------------------------------
//
CNoWLANNetworksAvailableNotif::CNoWLANNetworksAvailableNotif() 
: CConnectionDialogsNotifBase(), 
  iActiveNote( NULL )
{    
}    

// ---------------------------------------------------------
// void CNoWLANNetworksAvailableNotif::Cancel()
// ---------------------------------------------------------
//
void CNoWLANNetworksAvailableNotif::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    
    if ( iActiveNote )
        {
        delete iActiveNote;
        iActiveNote = NULL;
        }
    }

// End of File
