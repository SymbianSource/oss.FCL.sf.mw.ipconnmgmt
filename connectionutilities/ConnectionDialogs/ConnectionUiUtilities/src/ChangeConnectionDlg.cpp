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
*     Defines dialog CChangeConnectionDlg from Connection Ui Utilities
*
*/


// INCLUDE FILES

#include <AknIconArray.h>
#include <AknsUtils.h>
#include <aknmessagequerycontrol.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <WlanCdbCols.h>

#include <data_caging_path_literals.hrh>
#include <connectionuiutilities.rsg>

#include "ChangeConnectionDlg.h"
#include "ActiveCChangeConnectionDlg.h"
#include "ConnectionDialogsLogger.h"
#include "ExpiryTimer.h"

// CONSTANTS

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


// ================= MEMBER FUNCTIONS =======================

// Constructor
CChangeConnectionDlg::CChangeConnectionDlg( TInt aIndex,
                                    TUint32* aIAPId, 
                                    CActiveCChangeConnectionDlg* aActiveDlg,
                                    TDes& aConnectionName )
: CAknListQueryDialog( &aIndex ),
  iIAPId( aIAPId ),
  iConnectionName( aConnectionName ),
  iIsWLANFeatureSupported( EFalse ),
  iActiveDlg( aActiveDlg ),
  iExpiryTimer( NULL )
    {
    }


// Destructor
CChangeConnectionDlg::~CChangeConnectionDlg()
    {
    //delete iExpiryTimer;
    }


// ---------------------------------------------------------
// CChangeConnectionDlg::NewL
// ---------------------------------------------------------
//
CChangeConnectionDlg* CChangeConnectionDlg::NewL( TUint32* aIAPId, 
                                      CActiveCChangeConnectionDlg* aActiveDlg,
                                      TDes& aConnectionName)
    {
    CChangeConnectionDlg* temp = new ( ELeave )CChangeConnectionDlg(
                                    0, aIAPId, aActiveDlg, aConnectionName );
    CleanupStack::PushL( temp ); 
    temp->ConstructL();                                    
    CleanupStack::Pop( temp );
    return temp;
    }


// ---------------------------------------------------------
// CChangeConnectionDlg::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------
//
void CChangeConnectionDlg::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iIsWLANFeatureSupported = 
                    FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();
    }



// ---------------------------------------------------------
// CChangeConnectionDlg::OkToExitL( TInt aButtonId)
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CChangeConnectionDlg::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CChangeConnectionDlg::OkToExitL " );  
    
    TBool result( EFalse );
    __ASSERT_DEBUG( iActiveDlg, User::Panic( KErrNullPointer, KErrNone ) );
    
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk )
        {
        TInt index = ListBox()->CurrentItemIndex();
        CConnectionInfo* tempInfo = iActiveDlg->ActIAPs()->At( index );
        *iIAPId = tempInfo->Id();

        CLOG_WRITEF( _L( "*iIAPId : %d" ), *iIAPId );
        
        iActiveDlg->Cancel();
        iActiveDlg->SetSelected( ETrue );
                
        result = ETrue;
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        iActiveDlg->Cancel();  
                  
        result = ETrue;
        }
    
    CLOG_LEAVEFN( "CChangeConnectionDlg::OkToExitL " );  
    
    return result;
    }



// ---------------------------------------------------------
// CChangeConnectionDlg::PreLayoutDynInitL()
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CChangeConnectionDlg::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CChangeConnectionDlg::PreLayoutDynInitL " );  
    
    CAknListQueryDialog::PreLayoutDynInitL();

    HBufC *desc = StringLoader::LoadL( R_CHANGE_CONNECTION_DESC, 
                                       iConnectionName );
    CleanupStack::PushL( desc );
    MessageBox()->SetMessageTextL( desc );
    CleanupStack::PopAndDestroy( desc );

    SetOwnershipType( ELbmOwnsItemArray );
    SetItemTextArray( iActiveDlg->ActIAPs() );       
    SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Timer not in use due the unresolved crash
    //iExpiryTimer = CExpiryTimer::NewL( *this );
    //iExpiryTimer->Start();
    CLOG_LEAVEFN( "CChangeConnectionDlg::PreLayoutDynInitL " );  
    
    }


// ---------------------------------------------------------
// CChangeConnectionDlg::RefreshDialogL
// ---------------------------------------------------------
//    
void CChangeConnectionDlg::RefreshDialogL()
    {  
    CLOG_ENTERFN( "CChangeConnectionDlg::RefreshDialogL " );  
     
    SetItemTextArray( iActiveDlg->ActIAPs() );
    ListBox()->HandleItemAdditionL();  
    
    Layout();   
    SizeChanged();
    DrawNow();
    
    CLOG_LEAVEFN( "CChangeConnectionDlg::RefreshDialogL " );  
              
    }


// ----------------------------------------------------------------------------
// void CChangeConnectionDlg::HandleResourceChange( TInt aType )
// Handle resource change events. 
// ----------------------------------------------------------------------------
//
void CChangeConnectionDlg::HandleResourceChange( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        CAknListQueryDialog::HandleResourceChange( aType );

        SizeChanged();
        }
    else
        {
        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            TRect mainPaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                               mainPaneRect );

            TAknLayoutRect layoutRect;
            layoutRect.LayoutRect( TRect( TPoint( 0, 0 ), 
                                   mainPaneRect.Size() ),
                                   AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );

            ListBox()->SetRect( layoutRect.Rect() );
            }

        // Base call
        CAknListQueryDialog::HandleResourceChange( aType );
        }
    }

void CChangeConnectionDlg::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }


// End of File
