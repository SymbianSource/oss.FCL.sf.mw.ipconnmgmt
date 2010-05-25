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
* Description:  Implementation of CEasyWpaDlgNotif
*
*/


// INCLUDE FILES
#include "EasyWpaDlgNotif.h"
#include "WepWpaQueryDlg.h"
#include "ConnUiUtilsNotif.h"

#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CEasyWpaDlgNotif::TNotifierInfo CEasyWpaDlgNotif::RegisterL()
// ---------------------------------------------------------
//
CEasyWpaDlgNotif::TNotifierInfo CEasyWpaDlgNotif::RegisterL()
    {
    iInfo.iUid = KUidEasyWpaDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidEasyWpaDlg;

    return iInfo;
    }

// ---------------------------------------------------------
// void CEasyWpaDlgNotif::StartL
// ---------------------------------------------------------
//
void CEasyWpaDlgNotif::StartL( const TDesC8& /*aBuffer*/, 
                                    TInt aReplySlot,
                                    const RMessagePtr2& aMessage )
    {
    if ( ScreenSaverOn() || AutolockOn() )
        {
        // Screen saver or Autolock is active. Cancel the dialog. 
        aMessage.Complete( KErrCancel );
        return;
        }
    
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    aMessage.ReadL( aReplySlot, iKey );

    HBufC* msg = CCoeEnv::Static()->AllocReadResourceLC( 
                                                    R_EASY_WPA_QUERY_TEXT );
    iDialog = new ( ELeave ) CWepWpaQueryDlg( iKey(), this, EDialogWpa, iHex );
    iDialog->SetPromptL( *msg );
    iDialog->SetMaxLength( KEasyWpaQueryMaxLength );
    iDialog->ExecuteLD( R_EASY_WEP_WPA_DLG );
    CleanupStack::PopAndDestroy( msg );
    }

// ---------------------------------------------------------
// void CEasyWpaDlgNotif::Cancel()
// ---------------------------------------------------------
//
void CEasyWpaDlgNotif::Cancel()
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
// void CEasyWpaDlgNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CEasyWpaDlgNotif::CompleteL( TInt aStatus )
    {    
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, iKey );
            }

        iMessage.Complete( aStatus );
        }
    Cancel(); // Temporary solution
    }


// ---------------------------------------------------------
// CEasyWpaDlgNotif* CEasyWpaDlgNotif::NewL()
// ---------------------------------------------------------
//
CEasyWpaDlgNotif* CEasyWpaDlgNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CEasyWpaDlgNotif* self = new ( ELeave ) CEasyWpaDlgNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

// End of File
