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
*     Defines CApSelPopupList for access point selection.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <ApListItem.h>
#include <apsetui.rsg>
#include <featmgr.h>

#include "ApSelectorListBoxModel.h"
#include "ApSettingsModel.h"
#include "ApSettingsHandlerUI.hrh"
#include "ApSelPopupList.h"
#include "ApsettingshandleruiImpl.h"


#include "ApSettingsHandlerLogger.h"

#include <ApProtHandler.h>  // for protection of settings

#include <AknsUtils.h>
#include <apsettings.mbg>
#include <data_caging_path_literals.hrh>


// CONSTANTS
// Drive and file name of the MBM file containing icons for Protection
_LIT( KFileIcons, "z:ApSettings.mbm" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSelPopupList::NewL
// ---------------------------------------------------------
//
CApSelPopupList* CApSelPopupList::NewL( CApSettingsModel& aModel,
                                       CApSettingsHandlerImpl& aHandler,
                                       TInt& aCurrentSelectionIndex,
                                       TInt& aPreferredUid,
                                       TSelectionMenuType aSelMenuType,
                                       TInt aIspFilter, TInt aBearerFilter,
                                       TInt aSortType, TUint32& aEventStore,
                                       CDesCArrayFlat* aItemArray,
                                       TBool aNeedsNone,
                                       TBool aNoEdit,
                                       TInt aReqIpvType,
                                       TVpnFilterType aVpnFilterType
                                       )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::NewL")
    CApSelPopupList* self = 
        new ( ELeave ) CApSelPopupList( aModel, aHandler,
                                        aCurrentSelectionIndex,
                                        aPreferredUid, aSelMenuType,
                                        aIspFilter, aBearerFilter,
                                        aSortType, aItemArray,
                                        aEventStore, aNeedsNone,
                                        aNoEdit,
                                        aReqIpvType,
                                        aVpnFilterType
                                        );
//  till that point, object is only partially constructed!
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::NewL")
    return self;
    }





// ---------------------------------------------------------
// CApSelPopupList::NewL
// ---------------------------------------------------------
//
CApSelPopupList* CApSelPopupList::NewL( CApSettingsModel& aModel,
                                       CApSettingsHandlerImpl& aHandler,
                                       TInt& aCurrentSelectionIndex,
                                       TInt& aPreferredUid,
                                       TSelectionMenuType aSelMenuType,
                                       TInt aIspFilter, TInt aBearerFilter,
                                       TInt aSortType, TUint32& aEventStore,
                                       CDesCArrayFlat* aItemArray,
                                       TBool aNeedsNone,
                                       TInt aReqIpvType,
                                       TVpnFilterType aVpnFilterType,
                                       TBool aIncludeEasyWlan,
                                       TBool aNoEdit
                                       )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::NewL2")
    
    CApSelPopupList* self = 
        new ( ELeave ) CApSelPopupList( aModel, aHandler,
                                        aCurrentSelectionIndex,
                                        aPreferredUid, aSelMenuType,
                                        aIspFilter, aBearerFilter,
                                        aSortType, aItemArray,
                                        aEventStore, aNeedsNone,
                                        aReqIpvType,
                                        aVpnFilterType,
                                        aIncludeEasyWlan,
                                        aNoEdit
                                        );
//  till that point, object is only partially constructed!
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::NewL2")
    return self;
    }
    
    

// ---------------------------------------------------------
// CApSelPopupList::ConstructL
// ---------------------------------------------------------
//
void CApSelPopupList::ConstructL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::ConstructL")

    FeatureManager::InitializeLibL();
    iHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    iProtectionSupported = FeatureManager::FeatureSupported( 
                                           KFeatureIdSettingsProtection );
    FeatureManager::UnInitializeLib();
    
    CAknRadioButtonSettingPage::ConstructL();
    iList = ListBoxControl();
    iModel = new( ELeave )CApSelectorListboxModel(
                          CApSelectorListboxModel::EGraphicOnOff );
    iList->Model()->SetItemTextArray( iModel );
    if ( iProtectionSupported )
        {
        TInt offset = LoadIconsL();
        iModel->SetOwnIconOffset( offset );
        }
    FillListBoxWithDataL();
    iDataModel->Database()->AddObserverL( this );
        
    if ( !iModel->Count() )
        {
        // no ap defined, ask query if editable,
        // show note if not
        if ( iNoEdit )
            {
            // show note first, 
            ShowNoteL( R_APUI_VIEW_NO_IAP_DEFINED_INFO );
            // then simply go back:
            iDoExit = ETrue;
            }
        else
            {
            if ( !AskQueryL( R_APUI_IAP_CREATE_NEW ) )
                { // do not create, simply go back
                iDoExit = ETrue;
                }
            else
                { // create new AP
                // just ket it pass, later in it will be handled in CheckIfEmptyL()
                }                        
            }
        }    
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::ConstructL")
    }



// Destructor
CApSelPopupList::~CApSelPopupList()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::~CApSelPopupList")
    
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
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::~CApSelPopupList")
    }



// Constructor
CApSelPopupList::CApSelPopupList(
                                 CApSettingsModel& aModel,
                                 CApSettingsHandlerImpl& aHandler,
                                 TInt& aIndex, TInt& aPreferredUid,
                                 TSelectionMenuType aSelMenuType,
                                 TInt aIspFilter, TInt aBearerFilter,
                                 TInt aSortType, CDesCArrayFlat* aItemArray,
                                 TUint32& aEventStore, TBool aNeedsNone,
                                 TBool aNoEdit,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType
                                 )
:CAknRadioButtonSettingPage( R_POPUP_LIST_SETTING_PAGE,
                             aIndex, aItemArray ),
iDataModel( &aModel ),
iSelected( &aPreferredUid ),
iBearerFilter( aBearerFilter ),
iIspFilter( aIspFilter ),
iSortType( aSortType ),
iSelMenuType( aSelMenuType ),
iHandler( &aHandler ),
iEventStore( &aEventStore ),
iPreferredUid( aPreferredUid ),
iNeedsNone( aNeedsNone ),
iNeedUnlock( EFalse ),
iReqIpvType( aReqIpvType ),
iVpnFilterType( aVpnFilterType ),
iVariant( aHandler.iExt->iVariant ),
iIncludeEasyWlan( EFalse ),
iNoEdit( aNoEdit )
    {
    }




CApSelPopupList::CApSelPopupList(
                                 CApSettingsModel& aModel,
                                 CApSettingsHandlerImpl& aHandler,
                                 TInt& aIndex, TInt& aPreferredUid,
                                 TSelectionMenuType aSelMenuType,
                                 TInt aIspFilter, TInt aBearerFilter,
                                 TInt aSortType, CDesCArrayFlat* aItemArray,
                                 TUint32& aEventStore, TBool aNeedsNone,
                                 TInt aReqIpvType,
                                 TVpnFilterType aVpnFilterType,
                                 TBool aIncludeEasyWlan,
                                 TBool aNoEdit
                                 )
:CAknRadioButtonSettingPage( R_POPUP_LIST_SETTING_PAGE,
                             aIndex, aItemArray ),
iDataModel( &aModel ),
iSelected( &aPreferredUid ),
iBearerFilter( aBearerFilter ),
iIspFilter( aIspFilter ),
iSortType( aSortType ),
iSelMenuType( aSelMenuType ),
iHandler( &aHandler ),
iEventStore( &aEventStore ),
iPreferredUid( aPreferredUid ),
iNeedsNone( aNeedsNone ),
iNeedUnlock( EFalse ),
iReqIpvType( aReqIpvType ),
iVpnFilterType( aVpnFilterType ),
iVariant( aHandler.iExt->iVariant ),
iIncludeEasyWlan( aIncludeEasyWlan ),
iInitialised( EFalse ),
iNoEdit( aNoEdit )
    {
    }




// ---------------------------------------------------------
// CApSelPopupList::SetHighlighted()
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApSelPopupList::SetHighlighted()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::SetHighlighted")
    
    TInt err( KErrNone );
    TInt idx = iModel->Item4Uid( iPreferredUid, err );
    
    // if UID exists, set as current, else set 0 as current.
    iList->SetCurrentItemIndexAndDraw( err == KErrNone ? idx : 0);
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::SetHighlighted")
    }


// ---------------------------------------------------------
// CApSelPopupList::GetHelpContext()
// ---------------------------------------------------------
//
void CApSelPopupList::GetHelpContext(TCoeHelpContext& aContext) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::GetHelpContext")
    
    aContext.iMajor = iHandler->iHelpMajor;
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::GetHelpContext")
    }


// ---------------------------------------------------------
// CApSelPopupList::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelPopupList::HandleApDbEventL( TEvent anEvent )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::HandleApDbEventL")
    
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
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::HandleApDbEventL")
    }


// ---------------------------------------------------------
// CApSelPopupList::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelPopupList::ActivateL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::ActivateL")
    
    CAknRadioButtonSettingPage::ActivateL();

    if (iDoExit)
        {
        User::Leave(KErrNone);
        return;
        }

    // put empty list checking & required actions here.
    CheckIfEmptyL();
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::ActivateL")
    }



// ---------------------------------------------------------
// CApSelPopupList::SelectCurrentItemL()
// called after the setting had been changed
// ---------------------------------------------------------
//
void CApSelPopupList::SelectCurrentItemL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::SelectCurrentItemL")
    
    CAknRadioButtonSettingPage::SelectCurrentItemL();
    TInt idx = iList->CurrentItemIndex();
    if ( idx >= 0 )
        {
        *iSelected = iModel->At( idx )->Uid();
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::SelectCurrentItemL")
    }



// ---------------------------------------------------------
// CApSelPopupList::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelPopupList::OkToExitL( TInt aButtonId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::OkToExitL")
    
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
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::OkToExitL")
    return ETrue;
    }



// ---------------------------------------------------------
// CApSelPopupList::FillListBoxWithDataL()
// called when listbox needs to be filled with data
// ---------------------------------------------------------
//
void CApSelPopupList::FillListBoxWithDataL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::FillListBoxWithDataL")
    
    iPreferredLine = iList->CurrentItemIndex();
    if ( iPreferredLine < 0 )
        {
        iPreferredLine = 0;
        }

    TBool isLocked(EFalse);
    iNeedUnlock = EFalse;
    iDataModel->AllListItemDataL( isLocked, *iModel, iIspFilter, 
                                  iBearerFilter, iSortType, iReqIpvType,
                                  iVpnFilterType, iIncludeEasyWlan );
    if ( isLocked )
        {
        iNeedUnlock = ETrue;
        }

    iList->View()->SetDisableRedraw( ETrue );

    if ( iNeedsNone )
        {
        HBufC* buf = iEikonEnv->AllocReadResourceLC( R_APUI_VIEW_AP_NONE );
        CApListItem* NoneItem = CApListItem::NewLC
                        (
                        EIspTypeInternetAndWAP, KApNoneUID,
                        *buf,
                        EApBearerTypeHSCSD
                        );
        iModel->InsertL( 0, NoneItem );
        // as InsertL passes ownership of item to array,
        // remove from cleanupstack
        CleanupStack::Pop(); // NoneItem, as ownership is passed to array
        CleanupStack::PopAndDestroy(); // buf, as item has copied it...
        }

    iList->HandleItemAdditionL();

    iList->View()->SetDisableRedraw( EFalse );
    iList->HandleItemAdditionL();

    SetSelectedL();
    iPreferredUid = *iSelected;
    
    SetHighlighted();

    SizeChanged();
    DrawNow();
    
    CheckAndSetDataValidity();
    UpdateCbaL();
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::FillListBoxWithDataL")
    }


// ---------------------------------------------------------
// CApSelPopupList::SetSelectedL()
// ---------------------------------------------------------
//
void CApSelPopupList::SetSelectedL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::SetSelectedL")
    
    TInt i( 0 );
    TInt idx( 0 );//set something that does no harm...
    TInt count = iModel->Count(); 
    for( i=0; i<count; i++ )
        {
        if ( iModel->At( i )->Uid() == TUint32( *iSelected ) )
            {
            idx = i;
            i = count;
            }
        }
    iList->SetCurrentItemIndexAndDraw( idx );
    SelectCurrentItemL();
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::SetSelectedL")
    }


// ---------------------------------------------------------
// CApSelPopupList::CheckIfEmptyL()
// ---------------------------------------------------------
//
void CApSelPopupList::CheckIfEmptyL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::CheckIfEmptyL")
    
    if ( !iModel->Count() )
        {
        // no ap defined, create new AP
        ProcessCommandL( EApSelCmdNewBlank );
        }
    iInitialised = ETrue;
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::CheckIfEmptyL")
    }



// ---------------------------------------------------------
// CApSelPopupList::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApSelPopupList::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::DynInitMenuPaneL")
    
    CAknSettingPage::DynInitMenuPaneL( aResourceId, aMenuPane );

    TInt itemCount = iList->Model()->NumberOfItems();
    TInt idx = iList->CurrentItemIndex();
    TUint32 itemUid(0);
    if ( itemCount )
        {
        itemUid = iModel->At( idx )->Uid();
        }

    if ( iHandler->iModel->Need2DeleteSelect( aResourceId, itemCount ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdSelect );
        }

    if ( iHandler->iModel->Need2DeleteOpen( aResourceId, itemCount, 
                                            itemUid, iNeedsNone ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdOpen );
        }
    if ( iHandler->iModel->Need2DeleteDeleteL( aResourceId, itemCount, 
                                              itemUid, iNeedsNone ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdDelete );
        }

    if ( iHandler->iModel->Need2DeleteNewUseExisting( 
                        aResourceId, itemCount ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdNewUseExisting );
        }
    if ( iHandler->iModel->Need2DeleteNewL( aResourceId ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdNewBlank );
        }

    if ( iHandler->iModel->Need2DeleteHelp( aResourceId ) )
        {
        aMenuPane->DeleteMenuItem( EAknCmdHelp );                
        }

    if ( aResourceId == R_APSETTINGSUI_MENU_SELECT_NORMAL )
        {
        if ( iSelMenuType == EApSettingsSelMenuSelectOnly )
            {
            // Disallow other menues by deleting them
            aMenuPane->DeleteMenuItem( EApSelCmdExit );
            }
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::DynInitMenuPaneL")
    }


// From MEikCommandObserver
// ---------------------------------------------------------
// CApSelPopupList::ProcessCommandL
// ---------------------------------------------------------
//
void CApSelPopupList::ProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::ProcessCommandL")
    
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
        case EAknSoftkeyOk:
        case EApSelCmdSelect:
            {
            *iEventStore |= KApUiEventSelected;
            
            TUint32 currUid = iModel->At( iList->CurrentItemIndex() )->Uid();
            if( iPreferredUid != currUid )
                {
                iPreferredUid = currUid;
                SetHighlighted();
                }

            CAknRadioButtonSettingPage::SelectCurrentItemL();
            AttemptExitL( ETrue );
            break;
            }
        case EApSelCmdOpen:
            {// as we must be standing on an item, it must exist...
            // So no problem on iModel->At( ...)
            iPreferredUid = iModel->At( iList->CurrentItemIndex() )->Uid();
            iHandler->DoRunViewerL( iPreferredUid );
            SetHighlighted();
            break;
            }
        case EApSelCmdNew:
            {
            break; // starts a submenu, nothing to do here
            }
        case EApSelCmdNewBlank:
            {
            iPreferredUid = iHandler->HandleApBlankNewL(
                                            iBearerFilter, iIspFilter );
            SetHighlighted();
            break;
            }
        case EApSelCmdNewUseExisting:
            {
            TUint32 currid = iModel->At( iList->CurrentItemIndex() )->Uid();
            iPreferredUid = iHandler->HandleDuplicateL( currid );
            SetHighlighted();
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
                iHandler->HandleApDeleteCmdL(
                            iModel->At( iList->CurrentItemIndex() )->Uid(),
                            iModel->Count() == (1+iNeedsNone) );
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
        if ( iInitialised )
            {                        
            AttemptExitL( EFalse );
            }
        else
            {
            *iEventStore |= KApUiEventInternal;
            User::Leave(KLeaveWithoutAlert);
            }            
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
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::ProcessCommandL")
    }


    // From MEikListBoxObserver
void CApSelPopupList::HandleListBoxEventL( CEikListBox* aListBox,
                        MEikListBoxObserver::TListBoxEvent aEventType )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::HandleListBoxEventL")
    
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            ProcessCommandL( EApSelCmdSelect );
            break;
            }
        case EEventEditingStarted:
            {
            break;
            }
        case EEventEditingStopped:
            {
            break;
            }
        default:
            {
            CAknRadioButtonSettingPage::HandleListBoxEventL( aListBox, aEventType );
            };
        };
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::HandleListBoxEventL")
    }


// From CCoeControl
// ---------------------------------------------------------
// CApSelPopupList::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelPopupList::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::OfferKeyEventL")
    
    TKeyResponse retval;
    
    if ( aType == EEventKey )
        {
        if ( ( aKeyEvent.iCode == EKeyDelete ) ||
            ( aKeyEvent.iCode == EKeyBackspace ) )
            {
            if (!iNoEdit)
                {            
                ProcessCommandL( EApSelCmdDelete );
                }
            retval = EKeyWasConsumed;
            }
        else
            {
            if ( ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) &&
                ( ( iSelMenuType == EApSettingsSelMenuSelectOnly ) ||
                    ( iSelMenuType == EApSettingsSelMenuSelectNormal ) ) )
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
        
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::OfferKeyEventL")
    return retval;
    }




// ---------------------------------------------------------
// CApSelPopupList::LoadIconsL
// ---------------------------------------------------------
//
TInt CApSelPopupList::LoadIconsL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::LoadIconsL")
    
    CArrayPtr< CGulIcon >* icons = 
                        iList->ItemDrawer()->FormattedCellData()->IconArray();

    TInt count = icons->Count();

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KFileIcons, &KDC_APP_BITMAP_DIR, NULL ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWmlGprs,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wml_gprs, 
                            EMbmApsettingsQgn_prop_wml_gprs_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWmlCsd,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wml_csd, 
                            EMbmApsettingsQgn_prop_wml_csd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWmlHscsd,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wml_hscsd, 
                            EMbmApsettingsQgn_prop_wml_hscsd_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWmlSms,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wml_sms, 
                            EMbmApsettingsQgn_prop_wml_sms_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWlanBearer,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wlan_bearer, 
                            EMbmApsettingsQgn_prop_wlan_bearer_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropWlanBearer,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_prop_wlan_easy, 
                            EMbmApsettingsQgn_prop_wlan_easy_mask ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnIndiSettProtectedAdd,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_indi_sett_protected_add, 
                            EMbmApsettingsQgn_indi_sett_protected_add_mask ) );

    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::LoadIconsL")
    return count;
    }




// ---------------------------------------------------------
// CApSelPopupList::HandleResourceChange
// ---------------------------------------------------------
//
void CApSelPopupList::HandleResourceChange(TInt aType)
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelPopupList::HandleResourceChange")
    
    if ( aType == KAknsMessageSkinChange )
        {
        iList->HandleResourceChange( aType );
        if ( iProtectionSupported )
            {
            TRAP_IGNORE( LoadIconsL() );
            }
        SizeChanged();
        }    
        
    CAknRadioButtonSettingPage::HandleResourceChange( aType );          
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelPopupList::HandleResourceChange")
    }

// End of File
