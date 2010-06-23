/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Defines the pop-up selection list with the active connections.
*/


// INCLUDE FILES
#include "DisconnectDlgDialog.h"
#include "DisconnectDlgUi.h"
#include "ConnectionInfo.h"
#include "ConnectionCArray.h"
#include "ConnectionModel.h"
#include "ConnectionDialogsLogger.h"
#include "ExpiryTimer.h"

#include <disconnectdlg.rsg>
#include <AknIconArray.h>
#include <StringLoader.h>
#include <uikon/eiksrvui.h>


// CONSTANTS
/**
* Listbox item format mask
*/
_LIT( KFormat, "%d\t%S\t%S" );

/**
* Max size of Listbox item text
*/
const TInt KListItemTextBuf = 512;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDisconnectDlgDialog::NewL
// ---------------------------------------------------------
//
CDisconnectDlgDialog* CDisconnectDlgDialog::NewL( 
                                     CDisconnectDialogUi* aDisconnectDialogUi,
                                     CConnectionModel* aConnModel,
                                     TInt* aIndex,
                                     TUint32& aIAPId )
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::NewL");    
    CDisconnectDlgDialog* dialog = new( ELeave ) CDisconnectDlgDialog( 
                            aDisconnectDialogUi, aConnModel, aIndex, aIAPId );
    CleanupStack::PushL( dialog );
    dialog->ConstructL();
    CleanupStack::Pop( dialog ); // dialog
    CLOG_LEAVEFN("CDisconnectDlgDialog::NewL");    

    return dialog;
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::CDisconnectDlgDialog
// ---------------------------------------------------------
//
CDisconnectDlgDialog::CDisconnectDlgDialog( 
                                    CDisconnectDialogUi* aDisconnectDialogUi,
                                    CConnectionModel* aConnModel,
                                    TInt* aIndex,
                                    TUint32& aIAPId )
: CAknListQueryDialog( aIndex ), 
  iIAPId( aIAPId )  
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::CDisconnectDlgDialog");    
    
    iDisconnectDialogUi = aDisconnectDialogUi;
    iConnModel = aConnModel;

    CLOG_LEAVEFN("CDisconnectDlgDialog::CDisconnectDlgDialog");
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::RunDlgLD
// ---------------------------------------------------------
//
void CDisconnectDlgDialog::RunDlgLD()
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::RunDlgLD");    
    
    iPrompt = StringLoader::LoadL( R_QTN_NETW_HEAD_CONN_TO_CLOSE );
    ExecuteLD( R_DISCONNECTDLG_DIALOG, iPrompt->Des() );

    CLOG_LEAVEFN("CDisconnectDlgDialog::RunDlgLD");    
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::ConstructL
// ---------------------------------------------------------
//
void CDisconnectDlgDialog::ConstructL()
    {
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::~CDisconnectDlgDialog
// ---------------------------------------------------------
//
CDisconnectDlgDialog::~CDisconnectDlgDialog()
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::~CDisconnectDlgDialog");    
    
    delete iPrompt;
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    
    delete iExpiryTimer;

    CLOG_LEAVEFN("CDisconnectDlgDialog::~CDisconnectDlgDialog");    
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void CDisconnectDlgDialog::PreLayoutDynInitL()
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::PreLayoutDynInitL");    
    
    CAknListQueryDialog::PreLayoutDynInitL();
    
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    
    CConnectionCArray* connArray = iConnModel->GetConnArray();

    // items: application and IAP names
    CDesCArrayFlat* items = new( ELeave )CDesCArrayFlat( connArray->Count() );
    CleanupStack::PushL( items );
    // icons: application icons
    CArrayPtr< CGulIcon >* icons = new( ELeave )CAknIconArray( 
                                                        connArray->Count() );
    CleanupStack::PushL( icons );

    CGulIcon* iconTemp = NULL;
    CGulIcon* icon = NULL;

    TInt count = connArray->Count();
    CLOG_WRITEF(_L( "count: %d" ), count );     

    // Get the info of every connection and initialize the list of dialog
    for( TInt i = 0; i < count; i++ )
        {
        CConnectionInfo* info = connArray->At( i );

        TBool useSharedIcon = EFalse;
        // Application and IAP name
        HBufC* listItem = HBufC::NewLC( KListItemTextBuf );
        TPtrC iap = info->GetIapNameL();
        
        TPtrC app = info->GetAppNameL( useSharedIcon ); 

        // Application icon
        iconTemp = info->GetAppIconL( useSharedIcon );
        icon = CGulIcon::NewL();
        icon->SetBitmapsOwnedExternally( ETrue );
        icon->SetBitmap( iconTemp->Bitmap() );
        icon->SetMask( iconTemp->Mask() );

        icons->AppendL( icon );

        listItem->Des().Format( KFormat, i, &iap, &app );
        items->AppendL( listItem->Des() );

        CleanupStack::PopAndDestroy( listItem ); // listItem
        }

    // set new item list
    SetItemTextArray( items );
    // set icon list
    SetIconArrayL( icons );
    // we don't have to take care of delete items
    SetOwnershipType( ELbmOwnsItemArray );

    CleanupStack::Pop( icons );
    CleanupStack::Pop( items );
    
    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();

    CLOG_LEAVEFN("CDisconnectDlgDialog::PreLayoutDynInitL");
    }


// ---------------------------------------------------------
// CDisconnectDlgDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CDisconnectDlgDialog::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN("CDisconnectDlgDialog::OkToExitL");    
    
    TBool retVal( ETrue );
    CLOG_WRITEF(_L( "aButtonId: %d" ), aButtonId );     
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeyDone )
        {
        TInt toBeClosedItem = ListBox()->CurrentItemIndex();
        TInt result = iConnModel->EndConnection( toBeClosedItem );
        iDisconnectDialogUi->iDialogRun = EFalse;
        if ( result )
            {            
            if( KNullUnit == result )
                {
                result = KErrNone;
                }
            else
                {               
                iConnModel->InfoNoteL( R_QTN_NETW_INFO_DISCONNECT_FAILED );
                result = KErrGeneral;
                }
            
            iDisconnectDialogUi->CompleteL( result );
            }
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        iConnModel->CancelNotification();
        iDisconnectDialogUi->iDialogRun = EFalse;        
        iDisconnectDialogUi->CompleteL( KErrCancel );
        }
    else
        {
        retVal = EFalse;
        }

    CLOG_WRITEF(_L( "retVal: %d" ), retVal );    
    CLOG_LEAVEFN("CDisconnectDlgDialog::OkToExitL");    

    return retVal;
    }

void CDisconnectDlgDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// End of File
