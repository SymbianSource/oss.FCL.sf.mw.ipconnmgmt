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
* Description:  Notier part for WAPI PSK query dialog
*
*/

// INCLUDE FILES
#include "easywapidlgnotif.h"
#include "WepWpaQueryDlg.h"
#include "ConnUiUtilsNotif.h"

#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CEasyWapiDlgNotif::TNotifierInfo CEasyWapiDlgNotif::RegisterL()
// ---------------------------------------------------------
//
CEasyWapiDlgNotif::TNotifierInfo CEasyWapiDlgNotif::RegisterL()
    {
    iInfo.iUid      = KUidEasyWapiDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel  = KUidEasyWapiDlg;

    return iInfo;
    }

// ---------------------------------------------------------
// void CEasyWapiDlgNotif::StartL
// ---------------------------------------------------------
//
void CEasyWapiDlgNotif::StartL( const TDesC8&      /*aBuffer*/, 
                                TInt                aReplySlot,
                                const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage   = aMessage;
    iCancelled = EFalse;

    aMessage.ReadL( aReplySlot, iKey );

    // Using the same dialog resource as WPA-PSK
    HBufC* msg = CCoeEnv::Static()->AllocReadResourceLC( 
                                       R_EASY_WPA_QUERY_TEXT );
      
    // Using the same dialog implementation as WEP/WPA                                   
    iDialog = new ( ELeave ) CWepWpaQueryDlg( iKey(), 
                                              this, 
                                              EDialogWapi, 
                                              iHex );
    iDialog->SetPromptL( *msg );
    iDialog->SetMaxLength( KEasyWapiQueryMaxLength );
    
    iDialog->ExecuteLD( R_EASY_WEP_WPA_DLG );
    CleanupStack::PopAndDestroy( msg );
    }

// ---------------------------------------------------------
// void CEasyWapiDlgNotif::Cancel()
// ---------------------------------------------------------
//
void CEasyWapiDlgNotif::Cancel()
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
// void CEasyWapiDlgNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CEasyWapiDlgNotif::CompleteL( TInt aStatus )
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
// CEasyWapiDlgNotif* CEasyWapiDlgNotif::NewL()
// ---------------------------------------------------------
//
CEasyWapiDlgNotif* CEasyWapiDlgNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CEasyWapiDlgNotif* self = new ( ELeave ) CEasyWapiDlgNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

// End of File
