/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CConfirmationQuery.
*
*/


// INCLUDE FILES
#include "ConfirmationQuery.h"
#include "ConfirmationQueryNotif.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnectionDialogsLogger.h"
#include "ExpiryTimer.h"

#include <uikon/eiksrvui.h>
#include <StringLoader.h>
#include <ConnUiUtilsNotif.rsg>



// CONSTANTS
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CConfirmationQuery::CConfirmationQuery
// ---------------------------------------------------------
//
CConfirmationQuery::CConfirmationQuery( CConfirmationQueryNotif* aNotif )
: CAknListQueryDialog( &iDummy ),
  iNotif( aNotif ),
  iButtonGroupPreviouslyChanged( EFalse )
   {
   }
    
    
// ---------------------------------------------------------
// CConfirmationQuery::~CConfirmationQuery
// ---------------------------------------------------------
//
CConfirmationQuery::~CConfirmationQuery()
    {
    STATIC_CAST( CEikServAppUi*, 
                 CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    }



// ---------------------------------------------------------
// CConfirmationQuery::OkToExitL
// ---------------------------------------------------------
//
TBool CConfirmationQuery::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CConfirmationQuery::OkToExitL" );
    TBool result( EFalse );
    TInt status = KErrCancel;
    if ( aButtonId == EAknSoftkeySelect || 
         aButtonId == EAknSoftkeyDone || aButtonId == EAknSoftkeyOk )
        {
        iNotif->SetSelectedChoiceL( iChoiceIds[ListBox()->CurrentItemIndex()] );
        result = ETrue;
        status = KErrNone;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        status = KErrCancel;
        result = ETrue;
        }

    if ( result )
        {
        CLOG_WRITEF( _L( "aButtonId = %d" ), aButtonId );
        __ASSERT_DEBUG( iNotif, User::Panic( KErrNullPointer, KErrNone ) );
        iNotif->CompleteL( status );
        }

    CLOG_LEAVEFN( "CConfirmationQuery::OkToExitL" );

    return result;  
    }
    
    
// ---------------------------------------------------------
// CConfirmationQuery::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CConfirmationQuery::PreLayoutDynInitL()
    {      
    CAknListQueryDialog::PreLayoutDynInitL();

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );

    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    }


// ---------------------------------------------------------
// CConfirmationQuery::TryExitL()
// ---------------------------------------------------------
//
void CConfirmationQuery::TryExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CConfirmationQuery::TryExitL" );
    CAknListQueryDialog::TryExitL( aButtonId );
    CLOG_LEAVEFN( "CConfirmationQuery::TryExitL" );
    }

// ---------------------------------------------------------
// CConfirmationQuery::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CConfirmationQuery::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                 TEventCode aType)
    {
    if( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        // Let's not obscure the Dialer in the background
        if ( iExpiryTimer )
            {
            iExpiryTimer->Cancel();
            iExpiryTimer->StartShort();    
            }
        }
    
    return CAknListQueryDialog::OfferKeyEventL( aKeyEvent,aType ); 
    } 

// ---------------------------------------------------------
// CConfirmationQuery::SetChoices
// ---------------------------------------------------------
//
void CConfirmationQuery::SetChoices( RArray<TMsgQueryLinkedResults> aChoices )
    {
    iChoiceIds = aChoices;
    }

void CConfirmationQuery::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }


// End of File
