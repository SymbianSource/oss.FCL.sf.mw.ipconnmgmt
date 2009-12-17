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
* Description:  Implementation of class CAuthenticationDialog.
*
*/


// INCLUDE FILES
#include "AuthenticationDialog.h"
#include "ConnDlgPlugin.h"
#include "ExpiryTimer.h"

#include <uikon/eiksrvui.h>


// ---------------------------------------------------------
// CAuthenticationDialog::CAuthenticationDialog
// ---------------------------------------------------------
//
CAuthenticationDialog::CAuthenticationDialog( 
                                        CConnDlgAuthenticationPlugin* aPlugin )
: CAknMultiLineDataQueryDialog( CAknQueryDialog::ENoTone ),
  iPlugin( aPlugin )
    {
    }


// ---------------------------------------------------------
// CAuthenticationDialog::~CAuthenticationDialog
// ---------------------------------------------------------
//
CAuthenticationDialog::~CAuthenticationDialog()
    {

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    
    }


// ---------------------------------------------------------
// CAuthenticationDialog::NewL
// ---------------------------------------------------------
//
CAuthenticationDialog* CAuthenticationDialog::NewL( 
                                        CConnDlgAuthenticationPlugin* aPlugin, 
                                        TDes& aUsername, TDes& aPassword )
    {
    CAuthenticationDialog* dialog = new( ELeave ) 
                                            CAuthenticationDialog( aPlugin );
    CleanupStack::PushL( dialog );

    dialog->SetDataL( aUsername, aPassword );

    CleanupStack::Pop( dialog );

    return dialog;
    }


// ---------------------------------------------------------
// CAuthenticationDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CAuthenticationDialog::PreLayoutDynInitL()
    {
    CAknMultiLineDataQueryDialog::PreLayoutDynInitL();

    if( FirstData( TPtr( 0, 0 ) ).Length() > 0 )
        {
        MakeLeftSoftkeyVisible( ETrue );
        }

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    }


// ---------------------------------------------------------
// CAuthenticationDialog::SetInitialCurrentLine()
// ---------------------------------------------------------
//
void CAuthenticationDialog::SetInitialCurrentLine()
    {
    CAknMultiLineDataQueryDialog::SetInitialCurrentLine();

    iFirstEmpty = FirstData( TPtr( 0,0 ) ).Length() == 0;
    iSecondEmpty = SecondData( TPtr( 0,0 ) ).Length() == 0;
    if( !iFirstEmpty || !iSecondEmpty )
        {
        MakeLeftSoftkeyVisible( ETrue );
        }
    }


// ---------------------------------------------------------
// CAuthenticationDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CAuthenticationDialog::OkToExitL( TInt aButtonId )
    {
    if ( CAknMultiLineDataQueryDialog::OkToExitL( aButtonId ) )
        {
        if ( aButtonId == EAknSoftkeySelect || 
             aButtonId == EAknSoftkeyOk || 
             aButtonId == EAknSoftkeyDone )
            {
            iPlugin->CompleteL( KErrNone );
            }
        else if ( aButtonId== EAknSoftkeyCancel )
            {
            iPlugin->CompleteL( KErrCancel );
            }

        return( ETrue );
        }

    return( EFalse );
    }


// ---------------------------------------------------------
// CAuthenticationDialog::HandleQueryEditorStateEventL
//
// This code is based on AknQueryDialog.cpp
// ---------------------------------------------------------
//
TBool CAuthenticationDialog::HandleQueryEditorStateEventL( 
                                            CAknQueryControl* aQueryControl,
                                            TQueryControlEvent /*aEventType*/,
                                            TQueryValidationStatus aStatus )
    {
    if( STATIC_CAST( CAknMultilineQueryControl*, aQueryControl )->IsFirst() )
        {
        iFirstEmpty = ( aStatus == EEditorEmpty );
        }

    if( STATIC_CAST( CAknMultilineQueryControl*, aQueryControl)->IsSecond() )
        {
        iSecondEmpty = ( aStatus == EEditorEmpty );
        }

    MakeLeftSoftkeyVisible( ( !iFirstEmpty ) || ( !iSecondEmpty ) );

    return EFalse;
    }
	
void CAuthenticationDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }


// End of File
