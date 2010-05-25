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
* Description:  Implementation of CEasyWepDlgNotif
*
*/


// INCLUDE FILES
#include "EasyWepDlgNotif.h"
#include "WepWpaQueryDlg.h"
#include "ConnUiUtilsNotif.h"

#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>
#include <featmgr.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CEasyWepDlgNotif::TNotifierInfo CEasyWepDlgNotif::RegisterL()
// ---------------------------------------------------------
//
CEasyWepDlgNotif::TNotifierInfo CEasyWepDlgNotif::RegisterL()
    {
    iInfo.iUid = KUidEasyWepDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidEasyWepDlg;

    return iInfo;
    }

// ---------------------------------------------------------
// void CEasyWepDlgNotif::StartL
// ---------------------------------------------------------
//
void CEasyWepDlgNotif::StartL( const TDesC8& /*aBuffer*/, 
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
                                                    R_EASY_WEP_QUERY_TEXT );
    iDialog = new ( ELeave ) CWepWpaQueryDlg( 
                                            iKey().iKey, this, EDialogWep, iHex );
    iDialog->SetPromptL( *msg );

    TBool isWEP256Enabled = ETrue;
    
    FeatureManager::InitializeLibL();
    isWEP256Enabled = FeatureManager::FeatureSupported( 
                                                KFeatureIdWlanWep256Enabled );
    FeatureManager::UnInitializeLib();

    if ( isWEP256Enabled )
        {
        iDialog->SetMaxLength( KEasyWepQuery256BitMaxLength );
        }
    else
        {
        iDialog->SetMaxLength( KEasyWepQueryNon256BitMaxLength );
        }

    iDialog->ExecuteLD( R_EASY_WEP_WPA_DLG );
    CleanupStack::PopAndDestroy( msg );
    }

// ---------------------------------------------------------
// void CEasyWepDlgNotif::Cancel()
// ---------------------------------------------------------
//
void CEasyWepDlgNotif::Cancel()
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
// void CEasyWepDlgNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CEasyWepDlgNotif::CompleteL( TInt aStatus )
    {    
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iKey().iHex = iHex;
            iMessage.WriteL( iReplySlot, iKey );
            }

        iMessage.Complete( aStatus );
        }
    Cancel(); // Temporary solution
    }


// ---------------------------------------------------------
// CEasyWepDlgNotif* CEasyWepDlgNotif::NewL()
// ---------------------------------------------------------
//
CEasyWepDlgNotif* CEasyWepDlgNotif::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CEasyWepDlgNotif* self = new ( ELeave ) CEasyWepDlgNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

// End of File
