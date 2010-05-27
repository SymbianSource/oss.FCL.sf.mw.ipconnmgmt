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
*     Defines CApNetSelPopupList for access point selection.
*
*/


// INCLUDE FILES

#include <ApNetworkItem.h>
#include <featmgr.h>

#include "ApSettingsHandlerUI.hrh"
 

#include "ApNetSelPopupList.h"
#include "ApNetSelectorListBoxModel.h"
#include "ApSettingsModel.h"
#include <ApSetUI.rsg>
#include "ApsettingshandleruiImpl.h"


#include "ApSettingsHandlerLogger.h"

#include "AccessPointTextSettingPage.h"



#include <ApProtHandler.h>  // for protection of settings


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApNetSelPopupList::NewL
// ---------------------------------------------------------
//
CApNetSelPopupList* CApNetSelPopupList::NewL( CApSettingsModel& aModel,
                                       CApSettingsHandlerImpl& aHandler,
                                       TInt& aCurrentSelectionIndex,
                                       TInt& aPreferredUid,
                                       TUint32& aEventStore,
                                       CDesCArrayFlat* aItemArray,
                                       TBool aNeedsNone )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::NewL")
    
    CApNetSelPopupList* self = 
        new ( ELeave ) CApNetSelPopupList(
                                         aModel, aHandler,
                                         aCurrentSelectionIndex,
                                         aPreferredUid, aItemArray,
                                         aEventStore, aNeedsNone
                                         );
//  till that point, object is only partially constructed!
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::NewL")
    return self;
    }


// ---------------------------------------------------------
// CApNetSelPopupList::ConstructL
// ---------------------------------------------------------
//
void CApNetSelPopupList::ConstructL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::ConstructL")

    FeatureManager::InitializeLibL();
    iHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    iProtectionSupported = FeatureManager::FeatureSupported( 
                                           KFeatureIdSettingsProtection );
    FeatureManager::UnInitializeLib();
    
    CAknRadioButtonSettingPage::ConstructL();
    iList = ListBoxControl();
    iModel = new( ELeave )CApNetSelectorListboxModel();
    iList->Model()->SetItemTextArray( iModel );
    FillListBoxWithDataL();
    iDataModel->Database()->AddObserverL( this );
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::ConstructL")
    }


// Destructor
CApNetSelPopupList::~CApNetSelPopupList()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::~CApNetSelPopupList")
    
    if ( iDataModel )
        {
        if ( iDataModel->Database() )
            {
            // must remove observer here as there is a chance that we do not exit
            // through the standard way but through Leave(KErrLeaveWithoutAlert)...
            iDataModel->Database()->RemoveObserver( this );            
            }
        }
    if ( iModel )
        {
        iModel->ResetAndDestroy();
        delete iModel;
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::~CApNetSelPopupList")
    }



// Constructor
CApNetSelPopupList::CApNetSelPopupList(
                                 CApSettingsModel& aModel,
                                 CApSettingsHandlerImpl& aHandler,
                                 TInt& aIndex, TInt& aPreferredUid,
                                 CDesCArrayFlat* aItemArray,
                                 TUint32& aEventStore, TBool aNeedsNone
                                 )
:CAknRadioButtonSettingPage( R_POPUP_LIST_NETWORK_SETTING_PAGE, 
                             aIndex, aItemArray ),
iDataModel( &aModel ),
iSelected( &aPreferredUid ),
iHandler( &aHandler ),
iEventStore( &aEventStore ),
iNeedsNone( aNeedsNone ),
iNeedUnlock( EFalse )
    {
    }





// ---------------------------------------------------------
// CApNetSelPopupList::SetHighlighted()
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApNetSelPopupList::SetHighlighted()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::SetHighlighted")
    
    if ( iPreferredUid )
        {
        TInt err( KErrNone );
        TInt idx = iModel->Item4Uid( iPreferredUid, err );
        // if UID exists, set as current, else set 0 as current.
        if ( err == KErrNone )
            {
            iList->SetCurrentItemIndexAndDraw( idx );
            iPreferredUid = 0;
            }
        else
            {
            iList->SetCurrentItemIndexAndDraw( 0 );
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::SetHighlighted")
    }


// ---------------------------------------------------------
// CApNetSelPopupList::GetHelpContext()
// ---------------------------------------------------------
//
void CApNetSelPopupList::GetHelpContext(TCoeHelpContext& aContext) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::GetHelpContext")
    
    aContext.iMajor = iHandler->iHelpMajor;
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::GetHelpContext")
    }


// ---------------------------------------------------------
// CApNetSelPopupList::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApNetSelPopupList::HandleApDbEventL( TEvent anEvent )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::HandleApDbEventL")
    
    switch ( anEvent )
        {
        case EDbChanged:
            {
            FillListBoxWithDataL();
            break;
            }
        case EDbClosing:
            {
            break;
            }
        case EDbAvailable:
            {
            if ( iNeedUnlock )
                {
                FillListBoxWithDataL();
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::HandleApDbEventL")
    }


// ---------------------------------------------------------
// CApNetSelPopupList::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApNetSelPopupList::ActivateL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::ActivateL")
    
    CAknRadioButtonSettingPage::ActivateL();
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::ActivateL")
    }



// ---------------------------------------------------------
// CApNetSelPopupList::SelectCurrentItemL()
// called after the setting had been changed
// ---------------------------------------------------------
//
void CApNetSelPopupList::SelectCurrentItemL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::SelectCurrentItemL")
    
    CAknRadioButtonSettingPage::SelectCurrentItemL();
    TInt idx = iList->CurrentItemIndex();
    if ( idx >= 0 )
        {
        *iSelected = iModel->At( idx )->Uid();
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::SelectCurrentItemL")
    }



// ---------------------------------------------------------
// CApNetSelPopupList::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::OkToExitL( TInt aButtonId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::OkToExitL")
    
    // Translate the button presses into commands for the appui & current
    // view to handle.
    // This is a very good thing, that aButtonId is a TInt,
    // but the caller ( CAknSettingPage ) simply uses a TBool.
    // The value of ETrue means the button usually for Yes, Ok, Options, etc
    // and EFalse means Back, Cancel, etc.
    // so we must act according to a boolean value and not
    // according to the real button id...
    TBool IsAccepted = aButtonId;

    if ( IsAccepted )
        {
        TInt idx = iList->CurrentItemIndex();
        if ( idx >= 0 )
            {
            *iSelected = iModel->At( idx )->Uid();
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::OkToExitL")
    return ETrue;
    }



// ---------------------------------------------------------
// CApNetSelPopupList::FillListBoxWithDataL()
// called when listbox needs to be filled with data
// ---------------------------------------------------------
//
void CApNetSelPopupList::FillListBoxWithDataL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::FillListBoxWithDataL")
    
    iPreferredLine = iList->CurrentItemIndex();
    if ( iPreferredLine < 0 )
        {
        iPreferredLine = 0;
        }

    if ( !iPreferredUid )
        { // if not specified explicitly, get current and use as 'preferred'
        if ( iPreferredLine >= 0 )
            {
            if ( iModel->Count() )
                {
                iPreferredUid = iModel->At( iPreferredLine )->Uid();
                }
            }
        }

    TBool isLocked(EFalse);
    iNeedUnlock = EFalse;

    iDataModel->NetWorkListDataL( isLocked, *iModel );

    if ( isLocked )
        {
        iNeedUnlock = ETrue;
        }

    iList->View()->SetDisableRedraw( ETrue );

    if ( iNeedsNone )
        {
        HBufC* buf = iEikonEnv->AllocReadResourceLC( R_APUI_VIEW_AP_NONE );
        CApNetworkItem* NoneItem = CApNetworkItem::NewLC();
        NoneItem->SetUid( KApNoneUID );
        NoneItem->SetNameL( *buf );
        iModel->InsertL( 0, NoneItem );
        // as InsertL passes ownership of item to array,
        // remove from cleanupstack
        CleanupStack::Pop(); // NoneItem, as ownership is passed to array
        CleanupStack::PopAndDestroy(); // buf, as item has copied it...
        }

    iList->HandleItemAdditionL();

    SetSelectedL();

    SetHighlighted();

    iList->View()->SetDisableRedraw( EFalse );
    iList->HandleItemAdditionL();

    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::FillListBoxWithDataL")
    }


// ---------------------------------------------------------
// CApNetSelPopupList::SetSelectedL()
// ---------------------------------------------------------
//
void CApNetSelPopupList::SetSelectedL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::SetSelectedL")
    
    TInt i( 0 );
    for( i=0; i<iModel->Count(); i++ )
        {
        if ( iModel->At( i )->Uid() == TUint32( *iSelected ) )
            {
            iList->SetCurrentItemIndexAndDraw( i );
            HandleListBoxEventL(iList,
                                MEikListBoxObserver::EEventItemSingleClicked);
          /*  HandleListBoxEventL(iList,
                                            MEikListBoxObserver::EEventItemDoubleClicked);*/
            return; //exit here
            }
        }
    iList->SetCurrentItemIndexAndDraw( 0 );//set something that does no harm...
    HandleListBoxEventL(iList,
        MEikListBoxObserver::EEventItemSingleClicked ); // simulate selection...
    /*HandleListBoxEventL(iList,
        MEikListBoxObserver::EEventItemDoubleClicked);*/
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::SetSelectedL")
    }




// ---------------------------------------------------------
// CApNetSelPopupList::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApNetSelPopupList::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::DynInitMenuPaneL")
    
    CAknSettingPage::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_APSETTINGSUI_MENU_NETWORK )
        {
        if ( Need2DeleteSelect( aResourceId ) )
            {
            aMenuPane->DeleteMenuItem( EApSelCmdSelect );
            }
        if ( Need2DeleteOpenDelete( aResourceId ) )
            {
            aMenuPane->DeleteMenuItem( EApSelCmdDelete );
            }
        if ( iProtectionSupported )
            {
            if ( CApProtHandler::IsTableProtectedL( 
                  iDataModel->Database()->Database() ) )
                {            
                aMenuPane->DeleteMenuItem( EApSelCmdNew );                
                }
            }
		if ( !iHelpSupported )
			{
			aMenuPane->DeleteMenuItem( EAknCmdHelp );
			}        
        }

    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::DynInitMenuPaneL")
    }


// From MEikCommandObserver
// ---------------------------------------------------------
// CApNetSelPopupList::ProcessCommandL
// ---------------------------------------------------------
//
void CApNetSelPopupList::ProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::ProcessCommandL")
    
    if ( MenuShowing() )
        {
        HideMenu();
        }

    switch ( aCommandId )
        {
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }
        case EAknSoftkeyBack:
            {
            AttemptExitL( EFalse );
            break;
            }
        case EAknSoftkeyCancel:
            {
            *iEventStore |= KApUiEventShutDownRequested;
            AttemptExitL( EFalse );
            break;
            }
        case EApSelCmdSelect:
            {
            *iEventStore |= KApUiEventSelected;
            AttemptExitL( ETrue );
            break;
            }
        case EApSelCmdNew:
            {
            // create a new network group here
            if ( HandleCreateNewNetworkGroupL() )
                {
                SetHighlighted();
                }
            break; 
            }
        case EApSelCmdDelete:
            {
            // As it might have been started by the Clear key,
            // we must check if there is any...
            TInt cnt = iModel->Count();
            if ( ( iNeedsNone & ( cnt > 1 ) ) || ( !iNeedsNone && cnt ) )
                {
                // as we must be standing on an item, it must exists...
                // So no problem on iModel->At( ...)
                if ( iList->CurrentItemIndex() < ( iModel->Count()-1 ) )
                    {// move to next one if possible
                    iPreferredUid = iModel->At(
                            iList->CurrentItemIndex()+1 )->Uid();
                    }
                else
                    { // if it is the last, move to previous if possible.
                    // if not, set to 0
                    if ( iModel->Count()> 1 )
                        {// move to prev. one if possible
                        iPreferredUid = iModel->At(
                            iList->CurrentItemIndex()-1 )->Uid();
                        }
                    else
                        {
                        iPreferredUid = 0;
                        }
                    }
                iHandler->HandleNetworkDeleteCmdL(
                            iModel->At( iList->CurrentItemIndex() )->Uid() );
                }
            break;
            }
        case EAknCmdHelp:
            {
			if ( iHelpSupported )
				{
				iDataModel->LaunchHelpL();	
				}
            break;
            }
        default:
            {
            // silently ignore it
            break;
            }
        }
    // check if user wants to Exit....
    if ( *iEventStore & KApUiEventExitRequested )
        {
        AttemptExitL( EFalse );
        }
    else
        {
        switch ( aCommandId )
            {
            case EApSelCmdExit:
                {
                *iEventStore |= KApUiEventExitRequested;
                AttemptExitL( EFalse );
                break;
                }
            case EEikCmdExit:
                {
                *iEventStore |= KApUiEventShutDownRequested;
                AttemptExitL( EFalse );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::ProcessCommandL")
    }



// From CCoeControl
// ---------------------------------------------------------
// CApNetSelPopupList::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApNetSelPopupList::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::OfferKeyEventL")
    TKeyResponse retval;
    if ( aType == EEventKey )
        {
        if ( ( aKeyEvent.iCode == EKeyDelete ) ||
            ( aKeyEvent.iCode == EKeyBackspace ) )
            {
            ProcessCommandL( EApSelCmdDelete );
            retval = EKeyWasConsumed;
            }
        else
            {
            if ( aKeyEvent.iCode == EKeyOK )
                { // process only if command is available...
                ProcessCommandL( EApSelCmdSelect );
                retval = EKeyWasConsumed;
                }
            else
                {
                retval = CAknRadioButtonSettingPage::OfferKeyEventL(
                                        aKeyEvent, aType );
                }
            }
        }
    else
        {
        retval = CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEvent, aType );
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::OfferKeyEventL")
    return retval;
    }



// ---------------------------------------------------------
// CApNetSelPopupList::Need2DeleteSelect
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::Need2DeleteSelect( TInt aResourceId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::Need2DeleteSelect<->")
    return ( ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL ) &&
             ( iList->Model()->NumberOfItems() == 0 ) );
    }



// ---------------------------------------------------------
// CApNetSelPopupList::Need2DeleteOpenDelete
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::Need2DeleteOpenDelete( TInt aResourceId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::Need2DeleteOpenDelete<->")
    
    return ( ( aResourceId == R_APSETTINGSUI_MENU_NETWORK ) &&
             (
               (
                 ( iList->Model()->NumberOfItems() == 0) && 
                 ( iNeedsNone == EFalse )
               )
               ||
               ( ( iNeedsNone ) &&
                 ( iModel->At( iList->CurrentItemIndex() )->Uid() 
                                  == KApNoneUID )
             ) ) );
    }



// ---------------------------------------------------------
// CApNetSelPopupList::HandleCreateNewNetworkGroupL
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::HandleCreateNewNetworkGroupL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"NetSelPopup::HandleCreateNewNetworkGroupL")
    
    TBool retval( EFalse );

    // must be prepared for the worst case...
    TBuf<KModifiableLongTextLength> textToChange;    

    TInt TextSettingPageFlags( EAknSettingPageNoOrdinalDisplayed );

    CAccessPointTextSettingPage* dlg =
        new( ELeave )CAccessPointTextSettingPage( 
                        R_TEXT_SETTING_PAGE_NETW_NAME, 
                        textToChange,
                        TextSettingPageFlags,
                        EFalse );

    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        CApNetworkItem* network = CApNetworkItem::NewLC();
        network->SetUid( 0 ); // will be assigned when written toi disk
        network->SetNameL( textToChange );
        iDataModel->DataHandler()->CreateNetworkL( *network );
        iPreferredUid = network->Uid();
        CleanupStack::PopAndDestroy( network );
        retval = ETrue;
        }
    *iEventStore |= KApUiEventEdited;
    
    APSETUILOGGER_LEAVEFN( EListbox,"NetSelPopup::HandleCreateNewNetworkGroupL")
    return retval;
    }

// End of File
