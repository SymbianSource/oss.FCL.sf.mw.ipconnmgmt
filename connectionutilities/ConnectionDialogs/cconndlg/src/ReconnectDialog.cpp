/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CReconnectDialog.
*
*/


// INCLUDE FILES
#include "ReconnectDialog.h"
#include "ConnDlgPlugin.h"
#include "ExpiryTimer.h"

#include <uikon/eiksrvui.h>


// CONSTANTS
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CReconnectDialog::CReconnectDialog
// ---------------------------------------------------------
//
CReconnectDialog::CReconnectDialog( CConnDlgReconnectPlugin* aPlugin, 
                                    TBool& aBool )
: iPlugin( aPlugin ),
  iBool( aBool )
    {
    }


// ---------------------------------------------------------
// CReconnectDialog::~CReconnectDialog
// ---------------------------------------------------------
//
CReconnectDialog::~CReconnectDialog()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    }

// ---------------------------------------------------------
// CReconnectDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CReconnectDialog::PreLayoutDynInitL()
    {
    CAknQueryDialog::PreLayoutDynInitL();
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    }


// ---------------------------------------------------------
// CReconnectDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CReconnectDialog::OkToExitL( TInt aButtonId )
    {
    iBool = EFalse;
    if ( aButtonId == EAknSoftkeyYes || aButtonId == EAknSoftkeyDone || 
         aButtonId == EAknSoftkeyOk )
        {
        iBool = ETrue;
        }


    __ASSERT_DEBUG( iPlugin, User::Panic( KErrNullPointer, KErrNone ) );
    iPlugin->CompleteL( KErrNone );

    return ETrue;
    }

void CReconnectDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// End of File
