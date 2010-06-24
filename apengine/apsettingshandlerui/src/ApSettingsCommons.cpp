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
* Description: 
*    Defines common methods.
*
*/



// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include <eikenv.h>
#include <ApSettingsHandlerCommons.h>
#include "ApSettingsHandlerUI.hrh"
#include <apsetui.rsg>
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <StringLoader.h>

#include <AknProgressDialog.h>
#include <AknWaitDialog.h>

#include "ApSettingsHandlerLogger.h"

void Panic( TApSettingsHandlerUiPanicCodes aPanic )
    {
    APSETUILOGGER_ENTERFN( EOther,"Commons::Panic")
    
    _LIT( kApSet, "APSettingsHandlerUi" ) ;
    User::Panic( kApSet, aPanic ) ;
    }


// ---------------------------------------------------------
// AskQueryL
// ---------------------------------------------------------
//
TInt AskQueryL( TInt aResId, TDesC* aVar )
    {
    APSETUILOGGER_ENTERFN( EOther,"Commons::AskQueryL")
    
    HBufC* temp;
    if ( aVar )
        {
        temp = StringLoader::LoadL( aResId, *aVar );
        CleanupStack::PushL( temp );
        }
    else
        {
        temp = CEikonEnv::Static()->AllocReadResourceLC( aResId );
        }
    CAknQueryDialog* qd = 
            CAknQueryDialog::NewL( CAknQueryDialog::EConfirmationTone );
    TInt retval = qd->ExecuteLD( R_CONFIRMATION_QUERY, *temp );
    CleanupStack::PopAndDestroy();  // temp
    
    APSETUILOGGER_LEAVEFN( EOther,"Commons::AskQueryL")
    return retval;
    }



// ---------------------------------------------------------
// ShowNoteL
// ---------------------------------------------------------
//
TInt ShowNoteL( TInt aResId, TDesC* aVar )
    {
    APSETUILOGGER_ENTERFN( EOther,"Commons::ShowNoteL")
    
    HBufC* temp;
    if ( aVar )
        {
        temp = StringLoader::LoadL( aResId, *aVar );
        CleanupStack::PushL( temp );
        }
    else
        {
        temp = CEikonEnv::Static()->AllocReadResourceLC( aResId );
        }
    CAknNoteDialog* dlg =
        new ( ELeave ) CAknNoteDialog
                (
                 REINTERPRET_CAST( CEikDialog**, &dlg ),
                 CAknNoteDialog::ENoTone,
                 CAknNoteDialog::EShortTimeout
                );
    dlg->PrepareLC( R_GENERAL_NOTE );
    dlg->SetTextL( *temp );
    TInt retval = dlg->RunLD();
    CleanupStack::PopAndDestroy();  // temp
    
    APSETUILOGGER_LEAVEFN( EOther,"Commons::ShowNoteL")
    return retval;
    }


#ifdef __TEST_OOMDEBUG
    TInt GetTestStateL()
        {
        APSETUILOGGER_ENTERFN( EOther,"Commons::GetTestStateL")
        
        TInt retval( 0 );
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL<RFs>( fs );
        TUint att;
        if ( fs.Att( KTestFileName, att ) == KErrNone )
            {
            RFile file;
            TInt err = file.Open( fs, KTestFileName,
                                  EFileShareAny | EFileStreamText |
                                  EFileRead
                                );
            if ( err == KErrNone )
                {
                TBuf8<256> buf;
                err = file.Read( buf );
                if ( err == KErrNone )
                    {
                    TInt val;
                    TLex8 lex( buf );
                    err = lex.Val( val );
                    if ( err == KErrNone )
                        {
                        retval = val;
                        }
                    }
                }
            file.Close();
            }
        CleanupStack::PopAndDestroy(); // fs, will also close it
        
        APSETUILOGGER_LEAVEFN( EOther,"Commons::GetTestStateL")
        return retval;
        }
#endif // __TEST_OOMDEBUG

// End of File
