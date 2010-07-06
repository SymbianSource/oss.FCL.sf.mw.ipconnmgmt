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
*     Defines dialog CApSelectorDialog for access point selection.
*
*/


// INCLUDE FILES
#include "APSettingsHandlerUIVariant.hrh"

#include <aknnavide.h>
#include <akntitle.h>
#include <eikmenup.h>

#include <ApListItem.h>
#include <apsetui.rsg>

#include <featmgr.h>

#include "ApSelectorDialog.h"
#include "ApSelectorListBoxModel.h"
#include "ApSelectorListbox.h"
#include "ApSettingsModel.h"
#include "ApsettingshandleruiImpl.h"
#include "ApSettingsHandlerUI.hrh"
#include "TextOverrides.h"

#include "ApSettingsHandlerLogger.h"

#include <VpnApEngine.h>
// CONSTANTS



// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CApSelectorDialog::ConstructAndRunLD
// Constructs the dialog and runs it.
// ---------------------------------------------------------
//
TInt CApSelectorDialog::ConstructAndRunLD( CApSettingsModel& aModel,
                                          CApSettingsHandlerImpl& aHandler,
                                          TUint32 aHighLight,
                                          TUint32& aSelected )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::ConstructAndRunLD")
    
    CleanupStack::PushL( this );

    FeatureManager::InitializeLibL();
    iHelpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    FeatureManager::UnInitializeLib();

#ifdef __TEST_OOMDEBUG
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
    TUint att;
    if ( fs.Att( KTestFileName, att ) == KErrNone )
        {
        iMemTestOn = ETrue;
        }
    else
        {
        iMemTestOn = EFalse;
        }
    CleanupStack::PopAndDestroy(); // fs, will also close it
#endif // __TEST_OOMDEBUG


    TInt retValue = KErrNone;

    iHighLight = aHighLight;
    iSelected = &aSelected;

    iDataModel = &aModel;
    iHandler = &aHandler;
    iVariant = iHandler->iExt->iVariant;

    TInt aMenuTitleResourceId;
    switch ( iSelMenuType )
        {
        case EApSettingsSelMenuSelectOnly:
            {
            aMenuTitleResourceId = R_APSETTINGSUI_SELECT_ONLY_MENUBAR;
            break;
            }
        case EApSettingsSelMenuSelectNormal:
            {
            aMenuTitleResourceId = R_APSETTINGSUI_SELECT_NORMAL_MENUBAR;
            break;
            }
        case EApSettingsSelMenuNormal:
        default: // defensive ! treat all error cases like normal!
            {
            aMenuTitleResourceId = R_APSETTINGSUI_NORMAL_MENUBAR;
            break;
            }
        }
    ConstructL( aMenuTitleResourceId );

    TInt aDlgResourceId;
    switch ( iListType )
        {
        case EApSettingsSelListIsPopUp:
        case EApSettingsSelListIsPopUpWithNone:
            {
            aDlgResourceId = R_APSELECTOR_POPUP_DIALOG;
            break;
            }
        case EApSettingsSelListIsListPane:
        default:// defensive ! treat all error cases like normal!
            {
            aDlgResourceId = R_APSELECTOR_PANE_DIALOG;
            break;
            }
        }

    iModel = new( ELeave )CApSelectorListboxModel;
    
    TBool islocked;
    iDataModel->AllListItemDataL( islocked, *iModel, iIspFilter,
                                  iBearerFilter, iSortType,
                                  iReqIpvType );
    

    if ( !iModel->Count() )
        {
        // no ap defined, ask query if editable,
        // show note if not
        if ( iNoEdit )
            {
            // show note first, 
            ShowNoteL( R_APUI_VIEW_NO_IAP_DEFINED_INFO );
            // then simply go back:
            User::Leave(KLeaveWithoutAlert);
            }
        else
            {        
            // no ap defined, ask query
            if ( !AskQueryL( R_APUI_IAP_CREATE_NEW ) )
                { // do not create, simply go back
                User::Leave(KLeaveWithoutAlert);
                }
            else
                { // create new AP
                // just ket it pass, later in it will be handled in CheckIfEmptyL()
                }
            }
        }
    
    CleanupStack::Pop();  // this, it will be PushL-d by executeLD...

    retValue = ExecuteLD( aDlgResourceId );

    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::ConstructAndRunLD")

    return retValue;
    }




// ---------------------------------------------------------
// CApSelectorDialog::~CApSelectorDialog
// Destructor
// ---------------------------------------------------------
//
CApSelectorDialog::~CApSelectorDialog()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::~CApSelectorDialog")
    
    if ( iDataModel )
        {
        if ( iDataModel->Database() )
            {
            iDataModel->Database()->RemoveObserver( this );
            }
        }

    // iModel is deleted by the ListBox because LB owns the model...
    // of course only after we passed ownership...
    if ( !iModelPassed )
        {
        delete iModel;
        }
    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        }
    if ( iTitlePane )
        {
        // set old text back, if we have it...
        if ( iOldTitleText )
            {
            if ( iExitReason != EShutDown )
                {
                TRAP_IGNORE( iTitlePane->SetTextL( *iOldTitleText ) );
                }
            delete iOldTitleText;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::~CApSelectorDialog")
    }



// ---------------------------------------------------------
// CApSelectorDialog::NewLC
// Two-phase dconstructor, second phase is ConstructAndRunLD
// ---------------------------------------------------------
//
CApSelectorDialog* CApSelectorDialog::NewL(
                                        TSelectionListType aListType,
                                        TSelectionMenuType aSelMenuType,
                                        TInt aIspFilter,
                                        TInt aBearerFilter,
                                        TInt aSortType,
                                        TUint32& aEventStore,
                                        TBool aNoEdit,
                                        TInt aReqIpvType
                                        )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::NewL")
    
    CApSelectorDialog* db =
        new ( ELeave )CApSelectorDialog( aListType, aSelMenuType,
                                         aIspFilter, aBearerFilter,
                                         aSortType, aEventStore,
                                         aNoEdit,
                                         aReqIpvType );

    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::NewL")
    return db;
    }




// ---------------------------------------------------------
// CApSelectorDialog::CApSelectorDialog
// Constructor
// ---------------------------------------------------------
//
CApSelectorDialog::CApSelectorDialog( TSelectionListType aListType,
                                      TSelectionMenuType aSelMenuType,
                                      TInt aIspFilter,
                                      TInt aBearerFilter,
                                      TInt aSortType,
                                      TUint32& aEventStore,
                                      TBool aNoEdit,
                                      TInt aReqIpvType
                                     )
:iListType( aListType ),
iSelMenuType( aSelMenuType ),
iIspFilter( aIspFilter ),
iBearerFilter( aBearerFilter ),
iSortType( aSortType ),
iEventStore( &aEventStore ),
iNeedUnlock( EFalse ),
iModelPassed( EFalse ),
iExitReason( EExitNone ),
iReqIpvType( aReqIpvType ),
iInitialised( EFalse ),
iNoEdit(aNoEdit)
    {

    }



// ---------------------------------------------------------
// CApSelectorDialog::InitTextsL
// called before the dialog is shown
// to initialize localized textual data
// ---------------------------------------------------------
//
void CApSelectorDialog::InitTextsL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::InitTextsL")
    
    // set pane text if neccessary...
    // pane text needed if not pop-up...
    __ASSERT_DEBUG( iTextOverrides, Panic( ENullPointer ) );
    if ( iListType == EApSettingsSelListIsListPane )
        {
        iStatusPane = iEikonEnv->AppUiFactory()->StatusPane();
        iTitlePane =
            ( CAknTitlePane* )iStatusPane->ControlL(
                                TUid::Uid( EEikStatusPaneUidTitle ) );

        iOldTitleText = iTitlePane->Text()->AllocL();
        iTitlePane->SetTextL( 
                        *iEikonEnv->AllocReadResourceLC( R_APUI_NAVI_AP ) );

        CleanupStack::PopAndDestroy(); // resource R_APUI_NAVI_AP

        iNaviPane = ( CAknNavigationControlContainer* ) 
                        iStatusPane->ControlL( 
                                TUid::Uid( EEikStatusPaneUidNavi ) );
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KEmpty );
        iNaviPane->PushL( *iNaviDecorator );
        }
        
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::InitTextsL")
    }



// ---------------------------------------------------------
// CApSelectorDialog::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------
//
void CApSelectorDialog::HandleListboxDataChangeL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::HandleListboxDataChangeL")
    
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
    iDataModel->AllListItemDataL( isLocked, *iModel, iIspFilter,
                                  iBearerFilter, iSortType,
                                  iReqIpvType );
    if ( isLocked )
        {
        iNeedUnlock = ETrue;
        }

    iList->View()->SetDisableRedraw( ETrue );
    iList->HandleItemAdditionL();

    SetHighlighted();

    iList->View()->SetDisableRedraw( EFalse );
    iList->HandleItemAdditionL();
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::HandleListboxDataChangeL")
    }



// ---------------------------------------------------------
// CApSelectorDialog::SetTextOverrides
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::SetTextOverrides( CTextOverrides *aOverrides )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::SetTextOverrides<->")
    
    __ASSERT_DEBUG( aOverrides, Panic( ENullPointer ) );
    iTextOverrides = aOverrides;
    }




// ---------------------------------------------------------
// CApSelectorDialog::CheckIfEmptyL()
// called on activation and after the db has changed
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelectorDialog::CheckIfEmptyL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::CheckIfEmptyL")
    
    if ( !iModel->Count() )
        {
        ProcessCommandL( EApSelCmdNewBlank );
        }
    iInitialised = ETrue;
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::CheckIfEmptyL")
    }


// ---------------------------------------------------------
// CApSelectorDialog::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelectorDialog::ActivateL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::ActivateL")
    
    CAknDialog::ActivateL();
    // put empty list checking & required actions here.
    CheckIfEmptyL();
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::ActivateL")
    }



// ---------------------------------------------------------
// CApSelectorDialog::GetHelpContext()
// ---------------------------------------------------------
//
void CApSelectorDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::GetHelpContext<->")
    
    aContext.iMajor = iHandler->iHelpMajor;
    }



// ---------------------------------------------------------
// CApSelectorDialog::HandleApDbEventLw
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelectorDialog::HandleApDbEventL( TEvent anEvent )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::HandleApDbEventL")
    
    switch ( anEvent )
        {
        case EDbChanged:
            {
            HandleListboxDataChangeL();
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
                HandleListboxDataChangeL();
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EUnknownCase ) );
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::HandleApDbEventL")
    }



// From MEikCommandObserver
void CApSelectorDialog::ProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::ProcessCommandL")
    
    if ( MenuShowing() )
        {
        HideMenu();
        }
    DoProcessCommandL( aCommandId );
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::ProcessCommandL")
    }




// From MEikListBoxObserver
void CApSelectorDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                             TListBoxEvent aEventType )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::HandleListBoxEventL")
    
    switch ( aEventType )
        {
        case EEventItemSingleClicked:            
        case EEventEnterKeyPressed:
            // both handled in the same way for now...
        case EEventItemDoubleClicked:
            {
            ProcessCommandL( EApSelCmdOpen );
            break;
            }
        case EEventItemClicked:
            {
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
            };
        };
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::HandleListBoxEventL")
    }



//----------------------------------------------------------
// CApSelectorDialog::CreateCustomControlL
//----------------------------------------------------------
//
SEikControlInfo CApSelectorDialog::CreateCustomControlL
( TInt aControlType )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::CreateCustomControlL")
    
    SEikControlInfo controlInfo;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    if ( aControlType == KApSelectorListboxType )
        {
        controlInfo.iControl = new ( ELeave ) CApSelectorListbox;
        }
    else
        {
        controlInfo.iControl = NULL;
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::CreateCustomControlL")
    return controlInfo;
    }


//----------------------------------------------------------
// CApSelectorDialog::DynInitMenuPaneL
//----------------------------------------------------------
//
void CApSelectorDialog::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::DynInitMenuPaneL")
    
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );

    TInt itemCount = iList->Model()->NumberOfItems();
//    TUint32 itemUid = iModel->At( iList->CurrentItemIndex() )->Uid();

    TUint32 itemUid(0);
    if ( itemCount > 0 )
        {
        itemUid = iModel->At( iList->CurrentItemIndex() )->Uid();
        }

    if ( iHandler->iModel->Need2DeleteSelect( aResourceId, itemCount ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdSelect );
        }
    if ( iHandler->iModel->Need2DeleteOpen( aResourceId, itemCount, 
                                            itemUid, EFalse ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdOpen );
        }
    if ( iHandler->iModel->Need2DeleteDeleteL( aResourceId, itemCount, 
                                               itemUid, EFalse ) )
        {
        aMenuPane->DeleteMenuItem( EApSelCmdDelete );
        }
    if ( iHandler->iModel->Need2DeleteNewUseExisting( aResourceId, 
                                                      itemCount ) )
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
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::DynInitMenuPaneL")
    }



// ---------------------------------------------------------
// CApSelectorDialog::OfferKeyEventL();
// ---------------------------------------------------------
//
TKeyResponse CApSelectorDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                               TEventCode aType)
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::OfferKeyEventL")
    
    TKeyResponse retval( EKeyWasConsumed );
    if ( ( aType == EEventKey ) &&
         ( ( aKeyEvent.iCode == EKeyDelete ) ||
           ( aKeyEvent.iCode == EKeyBackspace ) ) )
        {
        if (!iNoEdit)
            {
            ProcessCommandL( EApSelCmdDelete );
            }
        }
    else
        {
        if ( ( aType == EEventKey ) && ( aKeyEvent.iCode == EKeyOK ) )
            { // process only if command is available...
            if ( iList->Model()->NumberOfItems() > 0)
                {
                if (!iNoEdit)
                    {
                    ProcessCommandL( EApSelCmdOpen );
                    }
                }
            }
        else
            {
            if ( iList )
                {
                // as list IS consuming, must handle because
                // it IS the SHUTDOWN...
                // or, a view switch is shutting us down...
                if ( aKeyEvent.iCode == EKeyEscape )
                    {
                    ProcessCommandL( EEikCmdExit );
                    retval = EKeyWasConsumed;
                    }
                else
                    {
                    retval = iList->OfferKeyEventL( aKeyEvent, aType );
                    }
                }
            else
                {
                retval = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::OfferKeyEventL")
    return retval;
    }


// ---------------------------------------------------------
// CApSelectorDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::PreLayoutDynInitL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::PreLayoutDynInitL")
    
    iList =
        STATIC_CAST( CApSelectorListbox*, Control( KApSelectorListboxId ) );
    iList->CreateScrollBarFrameL( ETrue );
    iList->ScrollBarFrame()->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iList->SetListBoxObserver( this );

    iList->Model()->SetItemTextArray( iModel );
    iModelPassed = ETrue;
    iList->LoadIconsL();
    HandleListboxDataChangeL();
    iList->HandleItemAdditionL();
    InitTextsL();
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::PreLayoutDynInitL")
    }


// ---------------------------------------------------------
// CApSelectorDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::PostLayoutDynInitL()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::PostLayoutDynInitL")
    
    iDataModel->Database()->AddObserverL( this );
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::PostLayoutDynInitL")
    }


// ---------------------------------------------------------
// CApSelectorDialog::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelectorDialog::OkToExitL( TInt aButtonId )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::OkToExitL")
    
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );

    if ( aButtonId == EAknSoftkeyOptions )
        {
        if ( iSelMenuType == EApSettingsSelMenuSelectOnly )
            {
            *iSelected = iList->CurrentItemUid();
            retval = ETrue;
            }
        else
            {
            DisplayMenuL();
            }
        }
    else if (aButtonId == EApSelCmdOpen)
        {
        ProcessCommandL(aButtonId);
        retval = EFalse; // don't exit the dialog
        }
    else
        {
        retval = ETrue;
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::OkToExitL")
    return retval;
    }


// ---------------------------------------------------------
// CApSelectorDialog::SetHighlighted()
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApSelectorDialog::SetHighlighted()
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::SetHighlighted")
    
    TBool done( EFalse );

    if ( iPreferredUid )
        {
        TInt err( KErrNone );
        TInt idx = iModel->Item4Uid( iPreferredUid, err );
        // if UID exists, set as current, else set 0 as current.
        if ( err == KErrNone )
            {
            iList->SetCurrentItemIndexAndDraw( idx );
            iPreferredUid = 0;
            done = ETrue;
            }
        }
    if ( !done )
        { // try to use iPreferredLine, as previous selection had been deleted
        TInt count( iModel->Count() );
        if ( count <= iPreferredLine )
            {
            // less elements, preferred line is out of the screen, 
            // select last one
            if ( count )
                {
                iList->SetCurrentItemIndexAndDraw( count - 1 );
                }
            else
                {
                iList->SetCurrentItemIndexAndDraw( 0 );
                }
            done = ETrue;
            }
        else
            { // count > iPreferredLine, select preferred line
            iList->SetCurrentItemIndexAndDraw( iPreferredLine );
            done = ETrue;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::SetHighlighted")
    }






// ---------------------------------------------------------
// CApSelectorDialog::DoProcessCommandL
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApSelectorDialog::DoProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( EDialog,"SelDIalog::DoProcessCommandL")
    
    switch ( aCommandId )
        {
        case EApSelCmdSelect:
            {
            *iSelected = iList->CurrentItemUid();
            *iEventStore |= KApUiEventSelected;
            iPreferredUid = iList->CurrentItemUid();
            SetHighlighted();
            // Can not exit here, as it would delete this,
            // Exit will be handled at the end of this function!
            // TryExitL( EAknSoftkeyBack );
            break;
            }
        case EApSelCmdOpen:
            {
            // when using __UHEAP_MARK;/MARKEND, TitlePane old text setback is
            // reported as a mem. leak, but it is not as the lifetime of 
            // the text is longer than this part.
            iPreferredUid = iList->CurrentItemUid();
            iHandler->DoRunViewerL( iPreferredUid );
            break;
            }
        case EApSelCmdNewBlank:
            { // put the new's UID into iPreferredUid to set highlight onto it
            // when using __UHEAP_MARK;/MARKEND, TitlePane old text setback is
            // reported as a mem. leak, but it is not as the lifetime of 
            // the text is longer than this part.
            iPreferredUid =
                    iHandler->HandleApBlankNewL( iBearerFilter, iIspFilter );
            SetHighlighted();
            break;
            }
        case EApSelCmdNewUseExisting:
            { // put the new's UID into iPreferredUid to set highlight onto it
            // when using __UHEAP_MARK;/MARKEND, TitlePane old text setback is
            // reported as a mem. leak, but it is not as the lifetime of 
            // the text is longer than this part.
            iPreferredUid = 
                    iHandler->HandleDuplicateL( iList->CurrentItemUid() );
            SetHighlighted();
            break;
            }
        case EApSelCmdDelete:
            { 
            // put the UID of the next Ap into iPreferredUid to set 
            // highlight onto it...
            // if next does not exists, put it onto prev.
            // if no next & no prev. exists, set it to 0 (don't care).
            // As it might have been started by the Clear key,
            // we must check if there is any...
            if ( iModel->Count() )
                {
                // As we must be standing on an item, it must exist...
                // So no problem on iModel->At( ...)
                if ( iList->CurrentItemIndex() < ( iModel->Count()-1 ) )
                    {
                    // move to next one if possible
                    iPreferredUid =
                        iList->Uid4Item( iList->CurrentItemIndex()+1 );
                    }
                else
                    {
                    // if it is the last, move to previous if possible.
                    // if not, set to 0
                    if ( iModel->Count()> 1 )
                        {
                        // move to prev. one if possible
                        iPreferredUid =
                            iList->Uid4Item( iList->CurrentItemIndex()-1 );
                        }
                    else
                        {
                        iPreferredUid = 0;
                        }
                    }
                iHandler->HandleApDeleteCmdL( iList->CurrentItemUid(),
                                              iModel->Count() == 1 );
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
    // check if user wants to exit...
    if ( *iEventStore & KApUiEventExitRequested )
        {
        if ( iInitialised )
            {                        
            TryExitL( EAknSoftkeyBack );
            }
        else
            {
            *iEventStore |= KApUiEventInternal;
            User::Leave(KLeaveWithoutAlert);
            }
        }
    else
        { // handle possible exit reasons/request here.
        switch ( aCommandId )
            {
            case EApSelCmdSelect:
                {
                TryExitL( EAknSoftkeyBack );
                break;
                }
            case EApSelCmdExit:
                {
                iExitReason = EExit;
                *iEventStore |= KApUiEventExitRequested;
                TryExitL( EAknSoftkeyBack );
                break;
                }
            case EEikCmdExit:
                {
                iExitReason = EShutDown;
                *iEventStore |= KApUiEventShutDownRequested;
                TryExitL( EAknSoftkeyBack );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    
    APSETUILOGGER_LEAVEFN( EDialog,"SelDIalog::DoProcessCommandL")
    }



// End of File

