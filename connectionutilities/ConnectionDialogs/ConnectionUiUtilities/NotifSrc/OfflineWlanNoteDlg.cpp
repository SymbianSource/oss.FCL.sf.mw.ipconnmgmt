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
* Description:  Implementation of class COfflineWlanNoteDlg.
*
*/


// INCLUDE FILES
#include "OfflineWlanNoteDlg.h"
#include "OfflineWlanNoteNotif.h"
#include "ExpiryTimer.h"
#include <uikon/eiksrvui.h>



// CONSTANTS
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// COfflineWlanNoteDlg::COfflineWlanNoteDlg
// ---------------------------------------------------------
//
COfflineWlanNoteDlg::COfflineWlanNoteDlg( COfflineWlanNoteNotif* aNotif )
: iNotif( aNotif ),
  iExpiryTimer( NULL )
    {
    }
    
    
// ---------------------------------------------------------
// COfflineWlanNoteDlg::~COfflineWlanNoteDlg
// ---------------------------------------------------------
//
COfflineWlanNoteDlg::~COfflineWlanNoteDlg()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    }



// ---------------------------------------------------------
// COfflineWlanNoteDlg::OkToExitL
// ---------------------------------------------------------
//
TBool COfflineWlanNoteDlg::OkToExitL( TInt aButtonId )
    {
    TInt status = KErrCancel;
    if (aButtonId == EAknSoftkeyYes || aButtonId == EAknSoftkeyDone || 
        aButtonId == EAknSoftkeyOk )
        {
        status = KErrNone;
        }

    __ASSERT_DEBUG( iNotif, User::Panic( KErrNullPointer, KErrNone ) );
    iNotif->CompleteL( status );

    return ETrue;
    }
    
    
// ---------------------------------------------------------
// COfflineWlanNoteDlg::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void COfflineWlanNoteDlg::PreLayoutDynInitL()
    {
    
    CAknQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    }


void COfflineWlanNoteDlg::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }
// End of File
