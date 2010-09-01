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
* Description:  Implementation of class CQosDialog.
*
*/


// INCLUDE FILES
#include "QosDialog.h"
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
// CQosDialog::CQosDialog
// ---------------------------------------------------------
//
CQosDialog::CQosDialog( CConnDlgQosPlugin* aPlugin, TBool& aBool )
: iPlugin( aPlugin ),
  iBool( aBool )
    {
    }
    
    
// ---------------------------------------------------------
// CQosDialog::~CQosDialog
// ---------------------------------------------------------
//
CQosDialog::~CQosDialog()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    }


// ---------------------------------------------------------
// CQosDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CQosDialog::PreLayoutDynInitL()
    {
    CAknQueryDialog::PreLayoutDynInitL();
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    }


// ---------------------------------------------------------
// CQosDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CQosDialog::OkToExitL( TInt aButtonId )
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

void CQosDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// End of File
