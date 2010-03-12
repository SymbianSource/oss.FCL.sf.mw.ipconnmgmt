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
* Description:  Implementation of class CSelectConnectionDialog.
*
*/


// INCLUDE FILES
#include "SelectConnectionDialog.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "ConnectionDialogsLogger.h"
#include "ActiveSelectExplicit.h"
#include "ExpiryTimer.h"
#include "ConnectionDialogs.hrh"


#include "ConnDlgPlugin.h"
#include <CConnDlgPlugin.rsg>

#include <uikon/eiksrvui.h>

#include <AknsFrameBackgroundControlContext.h>
#include <aknsoundsystem.h>
#include <AknsDrawUtils.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <eiklbi.h>

// CONST DECLARATIONS

#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif



// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CSelectConnectionDialog::CSelectConnectionDialog
// ---------------------------------------------------------
//
CSelectConnectionDialog::CSelectConnectionDialog( 
                                        MEikSrvNotifierBase2* aPlugin,
                                        TInt aRefreshInterval,
                                        TUint32 aBearerSet, 
                                        TBool aIsWLANFeatureSupported,
                                        TUint aDefaultCMId )
: CAknListQueryDialog( &iDummy ),
  iPlugin( ( CConnDlgSelectConnectionPlugin* )aPlugin ),
  iFromOkToExit( EFalse ),
  iActiveSelectExplicit( NULL ),
  iRefreshInterval( aRefreshInterval ),
  iBearerSet( aBearerSet ),
  iIsWLANFeatureSupported( aIsWLANFeatureSupported ),
  iDefaultCMId( aDefaultCMId )
    {
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::NewL
// ---------------------------------------------------------
//
CSelectConnectionDialog* CSelectConnectionDialog::NewL( 
                                                MEikSrvNotifierBase2* aPlugin,
                                                TInt aRefreshInterval,
                                                TUint32 aBearerSet, 
                                                TBool aIsWLANFeatureSupported, 
                                                TUint aDefaultCMId )
    {
    CSelectConnectionDialog* self = new( ELeave ) CSelectConnectionDialog( 
                                                    aPlugin,
                                                    aRefreshInterval,
                                                    aBearerSet, 
                                                    aIsWLANFeatureSupported, 
                                                    aDefaultCMId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::ConstructL()
// ---------------------------------------------------------
//
void CSelectConnectionDialog::ConstructL()
    {
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::~CSelectConnectionDialog()
// ---------------------------------------------------------
//
CSelectConnectionDialog::~CSelectConnectionDialog()
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::~CSelectConnectionDialog " );
    iIapIDs.Close();
    iDestIDs.Close();
    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( EFalse );
    delete iExpiryTimer;
    CLOG_LEAVEFN( "CSelectConnectionDialog::~CSelectConnectionDialog " );
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CSelectConnectionDialog::PreLayoutDynInitL()
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::PreLayoutDynInitL " );  
    
    CAknListQueryDialog::PreLayoutDynInitL();

    SetOwnershipType( ELbmDoesNotOwnItemArray );
    SetIconArrayL( iIcons );

    STATIC_CAST( CEikServAppUi*, 
                CCoeEnv::Static()->AppUi() )->SuppressAppSwitching( ETrue );
    
    // Hide Options button if single-click enabled
    //
    if ( static_cast< CAknAppUi* >( iCoeEnv->AppUi() )->IsSingleClickCompatible() )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        cba->MakeCommandVisible( EAknSoftkeyOptions, EFalse );
        cba->DrawDeferred();
        }

    iExpiryTimer = CExpiryTimer::NewL( *this );
    iExpiryTimer->Start();
    
    CLOG_LEAVEFN( "CSelectConnectionDialog::PreLayoutDynInitL " );      
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CSelectConnectionDialog::OkToExitL( TInt aButtonId )
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::OkToExitL " );      
    
    TBool result( EFalse );
    TInt completeCode( KErrNone );
    
    if ( aButtonId == EAknSoftkeySelect || aButtonId == EAknSoftkeyOk ||
         aButtonId == EAknSoftkeyDone )
        {
        CLOG_WRITE( "OkToExitL Ok-ed" );  
        TInt index = ListBox()->CurrentItemIndex();
        if ( iIapIDs[index] == 0 && iDestIDs[index] == 0 ) 
            {      // Uncategorized
            ProcessCommandL( ESelectConnectionCmdChooseMethod );
            }
        else
            {
            iPlugin->SetElementIDL( iIapIDs[index], iDestIDs[index] );
            result = ETrue;
            }
        }
    else if ( aButtonId == EAknSoftkeyCancel )
        {
        CLOG_WRITE( "OkToExitL Cancel" );  
        completeCode = KErrCancel;
        result = ETrue;
        }
    else if ( aButtonId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        }
        
    if ( result )
        {
        iFromOkToExit = ETrue;        
        
        __ASSERT_DEBUG( iPlugin, User::Panic( KErrNullPointer, KErrNone ) );
        CLOG_WRITE( "OkToExitL Going to CompleteL" );  
        iPlugin->CompleteL( completeCode );
        CLOG_WRITE( "OkToExitL Returned from CompleteL" );  
        }

    CLOG_LEAVEFN( "CSelectConnectionDialog::OkToExitL " );      

    return result;
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::PrepareAndRunLD
// ---------------------------------------------------------
//    
void CSelectConnectionDialog::PrepareAndRunLD( CConnectionInfoArray* aIAP,
                                               CArrayPtr< CGulIcon >* aIcons,
                                               TBool aIsReallyRefreshing,
                                               const TInt aHighlightedItem )
    {
    PrepareLC( R_SELECT_CONNECTION_QUERY );
    RefreshDialogL( aIAP, aIcons, aIsReallyRefreshing, aHighlightedItem );
    RunLD();
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::RefreshDialogL
// ---------------------------------------------------------
//    
void CSelectConnectionDialog::RefreshDialogL( CConnectionInfoArray* aIAP, 
                                              CArrayPtr< CGulIcon >* aIcons,
                                              TBool aIsReallyRefreshing,
                                              const TInt aHighlightedItem )
    {  
    CLOG_ENTERFN( "CSelectConnectionDialog::RefreshDialogL " );  
     
    if ( aIAP->Count() == 0 )
        {
        ListBox()->Reset();	
        }
    else
        {
        SetItemTextArray( aIAP );	
        } 
    
    iIcons = aIcons;

    iIapIDs.Reset();
    iDestIDs.Reset();

    // Determine the preferred IAP
    TInt numAPs = aIAP->Count();
    for( TInt ii = 0; ii < numAPs; ii++ )
        {
        iIapIDs.AppendL( aIAP->At( ii )->Id() );
        iDestIDs.AppendL( aIAP->At( ii )->DestinationId() );
        }

    if ( aHighlightedItem >= 0 && aHighlightedItem < numAPs )
        {
        ListBox()->SetCurrentItemIndex( aHighlightedItem );
        if ( aHighlightedItem == 1 )
            {
            // To have the Search for Wlan also visible. But this is valid only
            // if the highlighted item can be the first or the second, 
            // otherwise we risk to hide the highlighted item!
            ListBox()->ScrollToMakeItemVisible( 0 );    
            }
        }

    ListBox()->HandleItemAdditionL();

    if ( aHighlightedItem >= 0 && aHighlightedItem < numAPs )
        {
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        HBufC* label;

        if ( iIapIDs[aHighlightedItem] != 0 )
            {       // Easy Wlan
            label = StringLoader::LoadL( R_QTN_MSK_SELECT );
            }
        else if ( iDestIDs[aHighlightedItem] != 0 )
            {       // Destination
            label = StringLoader::LoadL( R_QTN_MSK_CONNECT );
            }
        else        // if ( iIapIDs[aHighlightedItem] == 0 && iDestIDs[aHighlightedItem] == 0 )
            {      // Uncategorized
            label = StringLoader::LoadL( R_QTN_MSK_OPEN );
            }

        CleanupStack::PushL( label );
        cba.SetCommandL( EAknSoftkeyDone, label->Des() );
        CleanupStack::PopAndDestroy(); // label
        }

    if ( aIsReallyRefreshing )
        {   // If we are really making a refresh, we need to recalculate the 
            // layout, because the number of items could be different and the
            // dialog should be resized accordingly
        Layout();   
        SizeChanged();
        DrawDeferred();
        }

    CLOG_LEAVEFN( "CSelectConnectionDialog::RefreshDialogL " );  
    }
    

// ----------------------------------------------------------------------------
// CSelectConnectionDialog::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CSelectConnectionDialog::DynInitMenuPaneL( TInt aResourceId, 
                                             CEikMenuPane* aMenuPane )
    {
    CAknListQueryDialog::DynInitMenuPaneL( aResourceId, aMenuPane );

    if ( aResourceId == R_SELECT_CONNECTION_MENU_PANE )
        {
        if ( aMenuPane )
            {
            TInt index = ListBox()->CurrentItemIndex();

            if ( iIapIDs[index] != 0 )   
                {      // Easy Wlan
                aMenuPane->DeleteMenuItem( ESelectConnectionCmdChooseMethod );
                aMenuPane->DeleteMenuItem( ESelectConnectionCmdConnect );
                }
            else if ( iDestIDs[index] != 0 )
                {       // Destination
                aMenuPane->DeleteMenuItem( ESelectConnectionCmdSelect );
                }
            else // if ( iIapIDs[index] == 0 && iDestIDs[index] == 0 ) 
                {      // Uncategorized
                aMenuPane->DeleteMenuItem( ESelectConnectionCmdSelect );
                aMenuPane->DeleteMenuItem( ESelectConnectionCmdConnect );
                }
            }
        }
    }


// ----------------------------------------------------------------------------
// CSelectConnectionDialog::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CSelectConnectionDialog::ProcessCommandL( TInt aCommand )
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::ProcessCommandL" );  
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch( aCommand )
        {
        case ESelectConnectionCmdSelect:
        case ESelectConnectionCmdConnect:
            {
            TryExitL( EAknSoftkeySelect );
            break;
            }

        case ESelectConnectionCmdChooseMethod:
            {
            if ( !iActiveSelectExplicit )
                {
                iActiveSelectExplicit = CActiveSelectExplicit::NewL( this, 
                                        iDestIDs[ListBox()->CurrentItemIndex()],
                                        iRefreshInterval,
                                        iBearerSet, 
                                        iIsWLANFeatureSupported,
                                       iDefaultCMId );
                iActiveSelectExplicit->ShowSelectExplicitL();
                }
            break;
            }

        default:
            {
            break;
            }
        }
    CLOG_LEAVEFN( "CSelectConnectionDialog::ProcessCommandL" );  
    }


// ---------------------------------------------------------
// CNetworksViewContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CSelectConnectionDialog::OfferKeyEventL( 
                                                   const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::OfferKeyEventL" );
    
    if( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        // Let's not obscure the Dialer in the background
        if ( iExpiryTimer && !iActiveSelectExplicit )
            {
            iExpiryTimer->Cancel();
            iExpiryTimer->StartShort();    
            }
        }

    TKeyResponse result( EKeyWasNotConsumed );
    TInt indexBefore = -1;
    TInt indexAfter = -1;

    if ( aKeyEvent.iScanCode == EStdKeyDownArrow || 
         aKeyEvent.iScanCode == EStdKeyUpArrow )
        {
        CLOG_WRITE( "Arrow pressed" );
        indexBefore = ListBox()->CurrentItemIndex();
        }

    result = CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
    
    if ( iFromOkToExit )
        {     
        return result;
        }
    
    CLOG_WRITEF( _L ( "Returned from CAknListQueryDialog::OfferKeyEventL: %d" ), result );

    if ( aKeyEvent.iScanCode == EStdKeyDownArrow || 
         aKeyEvent.iScanCode == EStdKeyUpArrow )
        {
        indexAfter = ListBox()->CurrentItemIndex();
        }

    if ( indexBefore != indexAfter )
        {
        CLOG_WRITEF( _L ( "IndexBefore = %d, IndexAfter = %d" ), indexBefore, indexAfter );
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        HBufC* label;
        if ( iIapIDs[indexAfter] != 0 )
            {       // Easy Wlan
            label = StringLoader::LoadL( R_QTN_MSK_SELECT );
            }
        else if ( iDestIDs[indexAfter] != 0 )
            {       // Destination
            label = StringLoader::LoadL( R_QTN_MSK_CONNECT );
            }
        else        // if ( iIapIDs[indexAfter] == 0 && iDestIDs[indexAfter] == 0 )
            {      // Uncategorized
            label = StringLoader::LoadL( R_QTN_MSK_OPEN );
            }
        CleanupStack::PushL( label );
        cba.SetCommandL( EAknSoftkeyDone, label->Des() );
        CleanupStack::PopAndDestroy(); // label
        cba.DrawDeferred();
        }

    // Show Options button if some listbox row gets highlighted
    //
    if ( static_cast< CAknAppUi* >( iCoeEnv->AppUi() )->IsSingleClickCompatible() )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();

        if ( ( ListBox()->View()->ItemDrawer()->Flags()
               & CListItemDrawer::ESingleClickDisabledHighlight )
              )
            {
            cba->MakeCommandVisible( EAknSoftkeyOptions, EFalse );
            }
        else
            {
            cba->MakeCommandVisible( EAknSoftkeyOptions, ETrue );
            }

        cba->DrawDeferred();
        }
    
    CLOG_LEAVEFN( "CSelectConnectionDialog::OfferKeyEventL" );  

    return result;
    }


// ----------------------------------------------------------------------------
// CSelectConnectionDialog::RunLD
// ----------------------------------------------------------------------------
//
TInt CSelectConnectionDialog::RunLD()
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::RunLD" );  
    CAknDialog::ConstructL( R_SELECT_CONNECTION_MENUBAR );
    User::ResetInactivityTime();

    CAknKeySoundSystem* soundSystem = NULL;
    if ( iSoundSystem )
        {
        iSoundSystem->PushContextL( R_AVKON_DEFAULT_SKEY_LIST );
        // Need to store local copy of iSoundSystem, since it will have been
        // destroyed when RunLD returns
        soundSystem = iSoundSystem;
        }

    CAknsFrameBackgroundControlContext* cc = 
        ( CAknsFrameBackgroundControlContext* )AknsDrawUtils::ControlContext(
                                                                        this );
    cc->SetCenter( KAknsIIDQsnFrPopupCenterQuery );

    TInt ret = CAknDialog::RunLD();
    if ( soundSystem )
        {
        soundSystem->PopContext();
        }
    CLOG_LEAVEFN( "CSelectConnectionDialog::RunLD" );  
    return ret;
    }


// ---------------------------------------------------------
// CSelectConnectionDialog::SetElementIDL
// ---------------------------------------------------------
//    
void CSelectConnectionDialog::SetElementIDL( TUint32 aIAPId )
    {
    iPlugin->SetElementIDL( aIAPId, 0 );
    }


// ---------------------------------------------------------
// void CSelectConnectionDialog::CompleteL
// ---------------------------------------------------------
//
void CSelectConnectionDialog::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CSelectConnectionDialog::CompleteL" );  
    delete iActiveSelectExplicit;
    iActiveSelectExplicit = NULL;
    iPlugin->CompleteL( aStatus );
    CLOG_LEAVEFN( "CSelectConnectionDialog::CompleteL" );  
    }

void CSelectConnectionDialog::HandleTimedOut()
    {
    TRAP_IGNORE( TryExitL(EAknSoftkeyCancel) );
    }

// ---------------------------------------------------------
// void CSelectConnectionDialog::HandleListBoxEventL
// ---------------------------------------------------------
//    
void CSelectConnectionDialog::HandleListBoxEventL(CEikListBox* aListBox, 
												TListBoxEvent aEventType)
	{	
    if( aEventType == EEventItemClicked )
        {
        // Do not forward to the base class. 
        // The first click must only select the item.        
        return;
        }
    else if( aEventType == EEventItemDoubleClicked )
        {
        // Forward as EEventItemClicked to open the item on the second click.
        CAknListQueryDialog::HandleListBoxEventL( aListBox, EEventItemClicked );
        }
	else
        {
        // normal forward
        CAknListQueryDialog::HandleListBoxEventL( aListBox, aEventType );
        }
    }

// ---------------------------------------------------------
// void CSelectConnectionDialog::HandlePointerEventL
// ---------------------------------------------------------
//    
void CSelectConnectionDialog::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if( ListBox()->Rect().Contains( aPointerEvent.iPosition ) )
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            TInt index;
            TInt oldIndex = ListBox()->CurrentItemIndex();
            if ( ListBox()->View()->XYPosToItemIndex( aPointerEvent.iPosition, index ) &&
                   ( index == oldIndex ) )
                {
                // Pressed Down Effect
                ListBox()->View()->ItemDrawer()->SetFlags( CListItemDrawer::EPressedDownState );
                }
            }
        CAknControl::HandlePointerEventL(aPointerEvent);
        }
    else
        {
        CAknListQueryDialog::HandlePointerEventL( aPointerEvent );
        }
    }

// End of File
