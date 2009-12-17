/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Defines the QueryDisconnectDlgDialog.
*
*/


// INCLUDE FILES
#include "QueryDisconnectDlgDialog.h"
#include "DisconnectDlgUi.h"
#include "ConnectionDialogsLogger.h"
#include "ExpiryTimer.h"

#include <aknnotedialog.h>
#include <uikon/eiksrvui.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CQueryDisconnectDlgDialog::CQueryDisconnectDlgDialog
// ---------------------------------------------------------
//
CQueryDisconnectDlgDialog::CQueryDisconnectDlgDialog( 
                                    CDisconnectDialogUi* aDisconnectDialogUi )
: CAknQueryDialog( ENoTone )
    {
    CLOG_ENTERFN("CQueryDisconnectDlgDialog::CQueryDisconnectDlgDialog");
    iDisconnectDialogUi = aDisconnectDialogUi;
    CLOG_LEAVEFN("CQueryDisconnectDlgDialog::CQueryDisconnectDlgDialog");
    }
    
    
// ---------------------------------------------------------
// CQueryDisconnectDlgDialog::~CQueryDisconnectDlgDialog
// ---------------------------------------------------------
//
CQueryDisconnectDlgDialog::~CQueryDisconnectDlgDialog()
    {
    CLOG_ENTERFN("CQueryDisconnectDlgDialog::~CQueryDisconnectDlgDialog");
    
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    
    delete iExpiryTimer;

    CLOG_LEAVEFN("CQueryDisconnectDlgDialog::~CQueryDisconnectDlgDialog");
    }


// ---------------------------------------------------------
// CQueryDisconnectDlgDialog::PreLayoutDynInitL()
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CQueryDisconnectDlgDialog::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CQueryDisconnectDlgDialog::PreLayoutDynInitL " );  
    
    CAknQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );  
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    CLOG_LEAVEFN( "CQueryDisconnectDlgDialog::PreLayoutDynInitL " );      
    }   


// ---------------------------------------------------------
// CQueryDisconnectDlgDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CQueryDisconnectDlgDialog::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN("CQueryDisconnectDlgDialog::OkToExitL");        
    
    TBool retVal( ETrue );

    CLOG_WRITEF( _L( "aButtonId: %d" ), aButtonId );     

    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeyDone )
        {
        iDisconnectDialogUi->iQueryRun = EFalse;        
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        iDisconnectDialogUi->iQueryRun = EFalse;
        iDisconnectDialogUi->CompleteL( KErrCancel );
        }
    else
        {
        retVal = EFalse;
        }
        
    CLOG_WRITEF( _L( "v: %d" ), retVal );             
    CLOG_LEAVEFN("CQueryDisconnectDlgDialog::OkToExitL");    

    return retVal;    
    }

void CQueryDisconnectDlgDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }
    
// End of File
