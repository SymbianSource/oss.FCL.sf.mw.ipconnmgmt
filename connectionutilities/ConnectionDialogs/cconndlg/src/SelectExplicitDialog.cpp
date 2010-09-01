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
* Description:  Implementation of class CSelectExplicitDialog.
*
*/


// INCLUDE FILES
#include "SelectExplicitDialog.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "ConnDlgPlugin.h"
#include "ConnectionDialogsLogger.h"
#include "SelectConnectionDialog.h"
#include "ExpiryTimer.h"

#include <AknIconArray.h>
#include <AknsUtils.h>
#include <uikon/eiksrvui.h>

#include <data_caging_path_literals.hrh>
#include <apsettings.mbg>

#include <CConnDlgPlugin.rsg>


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CSelectExplicitDialog::CSelectExplicitDialog
// ---------------------------------------------------------
//
CSelectExplicitDialog::CSelectExplicitDialog( 
                                    CSelectConnectionDialog* aCallerDialog )
: CAknListQueryDialog( &iDummy ),
  iCallerDialog( aCallerDialog ),
  iFromOkToExit( EFalse )
    {
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::NewL
// ---------------------------------------------------------
//
CSelectExplicitDialog* CSelectExplicitDialog::NewL( 
                                    CSelectConnectionDialog* aCallerDialog )
    {
    CSelectExplicitDialog* self = new( ELeave ) 
                                        CSelectExplicitDialog( aCallerDialog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::ConstructL()
// ---------------------------------------------------------
//
void CSelectExplicitDialog::ConstructL()
    {
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::~CSelectExplicitDialog()
// ---------------------------------------------------------
//
CSelectExplicitDialog::~CSelectExplicitDialog()
    {
    CLOG_ENTERFN( "CSelectExplicitDialog::~CSelectExplicitDialog " );
    iIAPIds.Close();
    delete iExpiryTimer;
    CLOG_LEAVEFN( "CSelectExplicitDialog::~CSelectExplicitDialog " );
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CSelectExplicitDialog::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CSelectExplicitDialog::PreLayoutDynInitL " );  
    
    CAknListQueryDialog::PreLayoutDynInitL();

    SetOwnershipType( ELbmDoesNotOwnItemArray );
    SetIconArrayL( iIcons );

    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    CLOG_LEAVEFN( "CSelectExplicitDialog::PreLayoutDynInitL " );      
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CSelectExplicitDialog::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CSelectExplicitDialog::OkToExitL " );      
    
    TBool result( EFalse );
    TInt completeCode( KErrNone );
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeyDone )
        {
        iCallerDialog->SetElementIDL( iIAPIds[ListBox()->CurrentItemIndex()] );
        result = ETrue;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        completeCode = KErrCancel;
        result = ETrue;
        }
        
    if ( result )
        {
        iFromOkToExit = ETrue;        
        
        iCallerDialog->CompleteL( completeCode );
        }

    CLOG_LEAVEFN( "CSelectExplicitDialog::OkToExitL " );      

    return result;
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::PrepareAndRunLD
// ---------------------------------------------------------
//    
void CSelectExplicitDialog::PrepareAndRunLD( CConnectionInfoArray* aIAP,
                                               CArrayPtr< CGulIcon >* aIcons,
                                               TBool aIsReallyRefreshing )
    {
    PrepareLC( R_LIST_EXPLICIT_QUERY );
    RefreshDialogL( aIAP, aIcons, aIsReallyRefreshing );
    RunLD();
    }


// ---------------------------------------------------------
// CSelectExplicitDialog::RefreshDialogL
// ---------------------------------------------------------
//    
void CSelectExplicitDialog::RefreshDialogL( CConnectionInfoArray* aIAP, 
                                              CArrayPtr< CGulIcon >* aIcons,
                                              TBool aIsReallyRefreshing )
    {  
    CLOG_ENTERFN( "CSelectExplicitDialog::RefreshDialogL " );  
     
    SetItemTextArray( aIAP );
    iIcons = aIcons;

    iIAPIds.Reset();

    // Determine the preferred IAP
    TInt numAPs = aIAP->Count();
    for( TInt ii = 0; ii < numAPs; ii++ )
        {
        iIAPIds.AppendL( aIAP->At( ii )->Id() );
        }

    if ( numAPs )
        {
        ListBox()->SetCurrentItemIndex( 0 );
        }
    ListBox()->HandleItemAdditionL();

    if ( aIsReallyRefreshing )
        {   // If we are really making a refresh, we need to recalculate the 
            // layout, because the number of items could be different and the
            // dialog should be resized accordingly
        Layout();   
        SizeChanged();
        DrawNow();
        }

    CLOG_LEAVEFN( "CSelectExplicitDialog::RefreshDialogL " );  
    }
    
// ---------------------------------------------------------
// CSelectExplicitDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CSelectExplicitDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
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
	
void CSelectExplicitDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }
// End of File
