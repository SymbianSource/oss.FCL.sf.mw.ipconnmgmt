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
* Description:  Implementation of class WepWpaQueryDlg.
*
*/


// INCLUDE FILES
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <ConnUiUtilsNotif.rsg>
#include <uikon/eiksrvui.h>

#include "WepWpaQueryDlg.h"
#include "EasyWepDlgNotif.h"
#include "EasyWpaDlgNotif.h"
#include "ConnectionDialogsLogger.h"


// CONSTANTS
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif

const TInt KEasyWpaQueryMinLength = 8;
const TInt KEasyWpaQueryLengthHEX64 = 64;

const TInt KEasyWapiQueryMinLength = 1; 

// valid Wep key lengths
const TInt KConnUiUtilsWepLengthASCII5 = 5;
const TInt KConnUiUtilsWepLengthASCII13 = 13;
const TInt KConnUiUtilsWepLengthASCII29 = 29;
const TInt KConnUiUtilsWepLengthHEX10 = 10;
const TInt KConnUiUtilsWepLengthHEX26 = 26;
const TInt KConnUiUtilsWepLengthHEX58 = 58;

// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CWepWpaQueryDlg::CWepWpaQueryDlg
// ---------------------------------------------------------
//
CWepWpaQueryDlg::CWepWpaQueryDlg( TDes& aDataText, 
                                  CConnectionDialogsNotifBase* aNotif, 
                                  TDialogType aDialogType, 
                                  TBool& aHex )
: CAknTextQueryDialog( aDataText ),
  iNotif( aNotif ), 
  iDialogType( aDialogType ), 
  iHex( aHex )
    {
    }


void CWepWpaQueryDlg::ShowInfoNoteL( TInt aResId )
    {
    HBufC* stringLabel = StringLoader::LoadL( aResId, iEikonEnv );
    CleanupStack::PushL( stringLabel );

    CAknInformationNote* dialog = new ( ELeave )CAknInformationNote( ETrue );
    dialog->ExecuteLD( *stringLabel );

    CleanupStack::PopAndDestroy( stringLabel );   // stringLabel
    }
    
    
// ---------------------------------------------------------
// CWepWpaQueryDlg::CWepWpaQueryDlg
// ---------------------------------------------------------
//
CWepWpaQueryDlg::~CWepWpaQueryDlg()
    {
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    }


// ---------------------------------------------------------
// CWepWpaQueryDlg::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CWepWpaQueryDlg::PreLayoutDynInitL()
    {
    
    CAknTextQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );

    }

// ---------------------------------------------------------
// CWepWpaQueryDlg::OkToExitL
// ---------------------------------------------------------
//
TBool CWepWpaQueryDlg::OkToExitL( TInt aButtonId )
    {
    TBool result = CAknTextQueryDialog::OkToExitL( aButtonId );
    if ( result )
        {
        __ASSERT_DEBUG( iNotif, User::Panic( KErrNullPointer, KErrNone ) );
        if ( aButtonId == EAknSoftkeyOk )
            {
            TInt length = Text().Length();
            
            if ( iDialogType == EDialogWpa )
                {
                if ( length < KEasyWpaQueryMinLength )
                    {
                    ShowInfoNoteL( R_INFO_PRESHARED_KEY_TOO_SHORT );
                    result = EFalse;
                    
                    // Wpa only should be cleared
                    CLOG_WRITE( "Wpa to be cleared!" );

                    CAknQueryControl* control = QueryControl();
                    if ( control )
                        {
                        control->SetTextL( KNullDesC );
                        
                        CCoeControl* coecontrol = 
                             control->ControlByLayoutOrNull(  EMultiDataFirstEdwin );     
        
                        if( coecontrol )
                            {
                            CEikEdwin* edwin = static_cast<CEikEdwin*>( coecontrol );
                            edwin->SetCursorPosL(0, EFalse);	
                            }
                        
                        //control->DrawNow();
                        }
                    UpdateLeftSoftKeyL();
                    }
                else if ( length == KEasyWpaQueryLengthHEX64 )
                    {
                    TBool foundInvalid = EFalse;
                    
                    for ( TInt i = 0; i < length; i++ )
                        {
                        if ( ( ( Text()[ i ] < '0' ) || 
                               ( Text()[ i ] > '9' ) ) &&
                             ( ( Text()[ i ] < 'A' ) || 
                               ( Text()[ i ] > 'F' ) ) && 
                             ( ( Text()[ i ] < 'a' ) || 
                               ( Text()[ i ] > 'f' ) ) )
                            {
                            foundInvalid = ETrue;
                            break;
                            }
                        }
                    
                    if ( foundInvalid )
                        {
                        ShowInfoNoteL( R_INFO_PRESHARED_KEY_NOT_HEXS );   
                        result = EFalse;
                        }
                    else
                        {
                        iNotif->CompleteL( KErrNone );
                        }
                    }
                else
                    {
                    iNotif->CompleteL( KErrNone );
                    }
                }
            else if ( iDialogType == EDialogWep )// Wep
                {
                if ( ( length == KConnUiUtilsWepLengthASCII5 ) ||
                    ( length == KConnUiUtilsWepLengthASCII13 ) ||
                    ( length == KConnUiUtilsWepLengthASCII29 ) )
                    {
                    iHex = EFalse;
                    iNotif->CompleteL( KErrNone );
                    }
                else if ( ( length == KConnUiUtilsWepLengthHEX10 ) ||
                    ( length == KConnUiUtilsWepLengthHEX26 ) ||
                    ( length == KConnUiUtilsWepLengthHEX58 ) )
                    {

                    TBool foundInvalid = EFalse;
                    for ( TInt i = 0; i < length; i++ )
                        {
                        if ( ( ( Text()[ i ] < '0' ) || 
                               ( Text()[ i ] > '9' ) ) &&
                             ( ( Text()[ i ] < 'A' ) || 
                               ( Text()[ i ] > 'F' ) ) && 
                             ( ( Text()[ i ] < 'a' ) || 
                               ( Text()[ i ] > 'f' ) ) )
                            {
                            foundInvalid = ETrue;
                            break;
                            }
                        }
                    if ( foundInvalid )
                        {
                        ShowInfoNoteL( R_INFO_WEP_KEY_ILLEGAL_CHARS );   
                        result = EFalse;
                        }
                    else
                        {
                        iHex = ETrue;
                        iNotif->CompleteL( KErrNone );
                        }
                    }
                else
                    {
                    ShowInfoNoteL( R_INFO_WEP_KEY_INVALID );        
                    result = EFalse;
                    }  
                }
            else // Wapi
                {
                if ( length < KEasyWapiQueryMinLength )
                    {
                    ShowInfoNoteL( R_INFO_PRESHARED_KEY_TOO_SHORT );
                    result = EFalse;
                    
                    // Wpa only should be cleared
                    CLOG_WRITE( "Wapi to be cleared!" );

                    CAknQueryControl* control = QueryControl();
                    
                    if ( control )
                        {
                        control->SetTextL( KNullDesC );
                        
                        CCoeControl* coecontrol = 
                             control->ControlByLayoutOrNull(  EMultiDataFirstEdwin );     
        
                        if( coecontrol )
                            {
                            CEikEdwin* edwin = static_cast<CEikEdwin*>( coecontrol );
                            edwin->SetCursorPosL(0, EFalse);	
                            }
                        
                        //control->DrawNow();
                        }
                    UpdateLeftSoftKeyL();
                    }
                else
                    {
                    iNotif->CompleteL( KErrNone );
                    }    
                }    
            }
        else
            {
            iNotif->CompleteL( KErrCancel );
            return result;
            }
        }
        
    CLOG_WRITEF( _L( "result: %d" ), result );
    CLOG_WRITEF( _L( "iDialogType: %d" ), iDialogType );

    DrawNow();
    return result;
    }

// ---------------------------------------------------------
// CWepWpaQueryDlg::NeedToDismissQueryL
// ---------------------------------------------------------
//
TBool CWepWpaQueryDlg::NeedToDismissQueryL(const TKeyEvent& aKeyEvent)
    {
    if (aKeyEvent.iCode == EKeyPhoneSend)
        {
        TryExitL(EEikBidCancel);
        return ETrue;
        }
        
    return EFalse;
    }

// End of File
