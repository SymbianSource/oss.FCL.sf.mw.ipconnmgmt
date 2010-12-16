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
 * Description:  Implementation of CConnectionErrorDiscreetPopupNotif.
 *
 */

// INCLUDE FILES
#include <e32property.h>
#include <ScreensaverInternalPSKeys.h>
#include "connectionerrordiscreetpopupnotif.h"
#include "connectionerrordiscreetpopup.h"
#include "ConnUiUtilsNotif.h"
#include "ConnectionDialogsUidDefs.h"
#include "ConnectionUiUtilitiesCommon.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopupNotif::TNotifierInfo 
//                               CConnectionErrorDiscreetPopupNotif::RegisterL()
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopupNotif::TNotifierInfo 
        CConnectionErrorDiscreetPopupNotif::RegisterL()
    {
    iInfo.iUid = KUidConnectionErrorDiscreetPopup;
    iInfo.iPriority = ENotifierPriorityVHigh;
    iInfo.iChannel = KUidConnectionErrorDiscreetPopup;

    return iInfo;
    }

// ---------------------------------------------------------
// void CConnectionErrorDiscreetPopupNotif::StartL()
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopupNotif::StartL( const TDesC8& aBuffer,
        TInt aReplySlot, const RMessagePtr2& aMessage )
    {
    TInt screenSaverOn( 0 );
    RProperty::Get( KPSUidScreenSaver, 
                    KScreenSaverOn, 
                    screenSaverOn );

    if ( iActiveNote || screenSaverOn > 0 )
        {
        // Note is already active or Screensaver is on.
        aMessage.Complete( KErrNone );
        return;
        }
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    iActiveNote = CConnectionErrorDiscreetPopup::NewL( this );

    TPckgBuf<TUint32> data;
    data.Copy( aBuffer );

    iActiveNote->StartL( data() );
    iCancelled = EFalse;
    }

// ---------------------------------------------------------
// void CConnectionErrorDiscreetPopupNotif::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopupNotif::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopupNotif* 
//        CConnectionErrorDiscreetPopupNotif::NewL()
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopupNotif* CConnectionErrorDiscreetPopupNotif::NewL(
        const TBool aResourceFileResponsible )
    {
    CConnectionErrorDiscreetPopupNotif* self =
            new (ELeave) CConnectionErrorDiscreetPopupNotif();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CConnectionErrorDiscreetPopupNotif::CConnectionErrorDiscreetPopupNotif()
// ---------------------------------------------------------
//
CConnectionErrorDiscreetPopupNotif::CConnectionErrorDiscreetPopupNotif() :
    CConnectionDialogsNotifBase(), iActiveNote( NULL )
    {
    }

// ---------------------------------------------------------
// void CConnectionErrorDiscreetPopupNotif::Cancel()
// ---------------------------------------------------------
//
void CConnectionErrorDiscreetPopupNotif::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            // get client thread status
            RThread clientThread;
            iMessage.Client( clientThread );
            TExitType exitType = clientThread.ExitType();
            // complete message only if client is still alive
           	if ( exitType == EExitPending )
           	  {
           	  iMessage.Complete( KErrCancel );
          		}
            clientThread.Close();
            }
        }
    if ( iActiveNote )
        {
        delete iActiveNote;
        iActiveNote = NULL;
        }
    }

// End of File
