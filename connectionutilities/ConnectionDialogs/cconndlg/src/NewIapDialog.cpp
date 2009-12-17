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
* Description:  Implementation of class CNewIapDialog.
*
*/



// INCLUDE FILES
#include "NewIapDialog.h"
#include "ConnDlgPlugin.h"
#include "ExpiryTimer.h"

#include <uikon/eiksrvui.h>
#include <StringLoader.h>

#include <CConnDlgPlugin.rsg>


// CONST DECLARATIONS

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CNewIapDialog::CNewIapDialog
// ---------------------------------------------------------
//
CNewIapDialog::CNewIapDialog( CConnDlgNewIapPlugin* aPlugin, TBool& aConnect,
                              const TNewIapConnectionPrefs& aPrefs )
: iPlugin( aPlugin ),
  iConnect( aConnect ),
  iPrefs( aPrefs )
    {
    }


// ---------------------------------------------------------
// CNewIapDialog::~CNewIapDialog
// ---------------------------------------------------------
//
CNewIapDialog::~CNewIapDialog()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    }

// ---------------------------------------------------------
// CNewIapDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void CNewIapDialog::PreLayoutDynInitL()
    {
    CAknQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );

    // New connection name
    HBufC *msg = StringLoader::LoadL( R_CONNDLG_NEW_CONNECTION_ATTEMPT, 
                                      iPrefs.iName );
    CleanupStack::PushL( msg );
    SetPromptL( msg->Des() );
    CleanupStack::PopAndDestroy( msg );
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    }


// ---------------------------------------------------------
// CNewIapDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CNewIapDialog::OkToExitL( TInt aButtonId )
    {
    iConnect = EFalse;
    if ( aButtonId == EAknSoftkeyYes || aButtonId == EAknSoftkeyDone || 
         aButtonId == EAknSoftkeyOk )
        {
        iConnect = ETrue;
        }

    __ASSERT_DEBUG( iPlugin, User::Panic( KErrNullPointer, KErrNone ) );
    iPlugin->CompleteL( KErrNone );

    return( ETrue );
    }

void CNewIapDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// End of File
