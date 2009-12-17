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
 * Description:  Implementation of CConnectingViaDiscreetPopupNotif.
 *
 */

// INCLUDE FILES
#include "connectingviadiscreetpopupnotif.h"
#include "connectingviadiscreetpopup.h"
#include "ConnUiUtilsNotif.h"
#include "ConnectionDialogsUidDefs.h"
#include "ConnectionUiUtilitiesCommon.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectingViaDiscreetPopupNotif::TNotifierInfo 
//                               CConnectingViaDiscreetPopupNotif::RegisterL()
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopupNotif::TNotifierInfo CConnectingViaDiscreetPopupNotif::RegisterL()
    {
    iInfo.iUid = KUidConnectingViaDiscreetPopup;
    iInfo.iPriority = ENotifierPriorityVHigh;
    iInfo.iChannel = KUidConnectingViaDiscreetPopup;

    return iInfo;
    }

// ---------------------------------------------------------
// void CConnectingViaDiscreetPopupNotif::StartL()
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopupNotif::StartL(const TDesC8& aBuffer,
        TInt aReplySlot, const RMessagePtr2& aMessage)
    {
    if ( iActiveNote )
        {
        // Note is already active
        aMessage.Complete( KErrNone );
        return;
        }
    
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = ETrue;
    iActiveNote = CConnectingViaDiscreetPopup::NewL(this);

    TPckgBuf<TConnUiConnectingViaDiscreetPopup> data;
    data.Copy(aBuffer);

    iActiveNote->StartL( data().iIapId, data().iConnectionAlreadyActive );
    iCancelled = EFalse;
    }

// ---------------------------------------------------------
// void CConnectingViaDiscreetPopupNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopupNotif::CompleteL(TInt aStatus)
    {
    iCancelled = ETrue;
    if (!iMessage.IsNull())
        {
        iMessage.Complete(aStatus);
        }
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopupNotif* CConnectingViaDiscreetPopupNotif::NewL()
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopupNotif* CConnectingViaDiscreetPopupNotif::NewL(
        const TBool aResourceFileResponsible)
    {
    CConnectingViaDiscreetPopupNotif* self =
            new (ELeave) CConnectingViaDiscreetPopupNotif();
    CleanupStack::PushL(self);
    self->ConstructL(KResourceFileName, aResourceFileResponsible);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CConnectingViaDiscreetPopupNotif::CConnectingViaDiscreetPopupNotif()
// ---------------------------------------------------------
//
CConnectingViaDiscreetPopupNotif::CConnectingViaDiscreetPopupNotif() :
    CConnectionDialogsNotifBase(), iActiveNote(NULL)
    {
    }

// ---------------------------------------------------------
// void CConnectingViaDiscreetPopupNotif::Cancel()
// ---------------------------------------------------------
//
void CConnectingViaDiscreetPopupNotif::Cancel()
    {
    if (!iCancelled)
        {
        iCancelled = ETrue;
        if (!iMessage.IsNull())
            {
            iMessage.Complete(KErrCancel);
            }
        }
    if ( iActiveNote )
        {
        delete iActiveNote;
        iActiveNote = NULL;
        }
    }

// End of File
