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
*     Defines dialog CApSelQueryDialog for access point selection.
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <ApListItem.h>
#include <AknIconArray.h>
#include <AknsUtils.h>

#include <apsetui.rsg>
#include <apsettings.mbg>

#include <featmgr.h>

#include "ApSelectorListBoxModel.h"
#include "ApSettingsModel.h"
#include "ApSettingsHandlerUI.hrh"
#include "ApSelQueryDialog.h"


#include "ApSettingsHandlerLogger.h"
#include <data_caging_path_literals.hrh>

// CONSTANTS
// Drive and file name of the MBM file containing icons for Protection
_LIT( KFileIcons, "z:ApSettings.mbm" );


// ================= MEMBER FUNCTIONS =======================

// Destructor
CApSelQueryDialog::~CApSelQueryDialog()
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::~CApSelQueryDialog")
    
    if ( iDataModel )
        {
        if ( iDataModel->Database() )
            {
            // iModel is deleted by the ListBox because LB owns the model
            iDataModel->Database()->RemoveObserver( this );
            }
        }

    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::~CApSelQueryDialog")
    }



// Constructor
CApSelQueryDialog::CApSelQueryDialog( CApSettingsModel& aModel,
                                      TUint32* aIndex )
:CAknListQueryDialog( (TInt*)aIndex ),
iNeedUnlock( EFalse )
    {
    iDataModel = &aModel;
    iSelected = aIndex;
    }



// ---------------------------------------------------------
// CApSelQueryDialog::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelQueryDialog::HandleApDbEventL( TEvent anEvent )
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::HandleApDbEventL")
    
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
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::HandleApDbEventL")
    }




// ---------------------------------------------------------
// CApSelQueryDialog::PostLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelQueryDialog::PreLayoutDynInitL()
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::PreLayoutDynInitL")
    
    // parent creates the private listbox
    CAknListQueryDialog::PreLayoutDynInitL();
    // and now we get access to it...
    CAknListQueryControl *control = 
            ( CAknListQueryControl* )Control( EListQueryControl );
    iList = control->Listbox();
    LoadIconsL();
    FillListBoxWithDataL();
    iDataModel->Database()->AddObserverL( this );
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::PreLayoutDynInitL")
    }


// ---------------------------------------------------------
// CApSelQueryDialog::PostLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelQueryDialog::PostLayoutDynInitL()
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::PostLayoutDynInitL")
    
    CAknListQueryDialog::PostLayoutDynInitL();
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::PostLayoutDynInitL")
    }


// ---------------------------------------------------------
// CApSelQueryDialog::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelQueryDialog::OkToExitL( TInt aButtonId )
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::OkToExitL")
    
    // Translate the button presses into commands for the appui & current
    // view to handle
    TBool retval( EFalse );
    if ( aButtonId == EAknSoftkeySelect )
        {
        TInt idx = iList->CurrentItemIndex();
        if ( idx >= 0 )
            {
            if ( iModel->At( iList->CurrentItemIndex() )->IsReadOnly() )
                { // read only, do not accept
                // show note
                ShowNoteL( R_APUI_NOTE_CANNOT_USE_PROTECTED_AP );
                retval = EFalse;
                }
            else
                {
                *iSelected = iModel->At( iList->CurrentItemIndex() )->Uid();
                retval = ETrue;
                }
            }
        }
    else
        {
        if ( aButtonId == EAknSoftkeyBack )
            {
            retval = ETrue;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::OkToExitL")
    return retval;
    }




// From CCoeControl
// ---------------------------------------------------------
// CApSelQueryDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::OfferKeyEventL")
    
    TKeyResponse retval;
    if ( aType == EEventKey )
        {
        if ( aKeyEvent.iCode == EKeyOK )
            { // process only if command is available...
            ProcessCommandL( EApSelCmdSelect );
            retval = EKeyWasConsumed;
            }
        else
            {
            retval = CAknListQueryDialog::OfferKeyEventL(
                                    aKeyEvent, aType );
            }
        }
    else
        {
        retval = CAknListQueryDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::OfferKeyEventL")
    return retval;
    }



// From MEikCommandObserver
// ---------------------------------------------------------
// CApSelQueryDialog::ProcessCommandL
// ---------------------------------------------------------
//
void CApSelQueryDialog::ProcessCommandL( TInt aCommandId )
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::ProcessCommandL")
    
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
            TryExitL( EFalse );
            break;
            }
        case EApSelCmdSelect:
            {
            TryExitL( EAknSoftkeySelect );
            break;
            }
        case EAknCmdHelp:
            {
		    FeatureManager::InitializeLibL();
		    TBool helpSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
		    FeatureManager::UnInitializeLib();
			if ( helpSupported )
				{            
            	iDataModel->LaunchHelpL();
				}
            break;
            }
        case EApSelCmdExit:
        case EEikCmdExit:
            {
            TryExitL( EFalse );
            break;
            }
        default:
            {
            // silently ignore it
            break;
            }
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::ProcessCommandL")
    }



// ---------------------------------------------------------
// CApSelQueryDialog::FillListBoxWithDataL()
// called when listbopx needs to be filled with data
// ---------------------------------------------------------
//
void CApSelQueryDialog::FillListBoxWithDataL()
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::FillListBoxWithDataL")
    
    TBool needtopop( EFalse );

    if ( ! iModel )
        {
        // we must use temp var to satisfy CodeScanner
        CApSelectorListboxModel* tmp = new( ELeave )CApSelectorListboxModel( 
                                CApSelectorListboxModel::EGraphicProtection );
        // although it will be member, but mainly owned by the listbox,
        // so as long as ownership is not transferred, MUST push it
        CleanupStack::PushL( tmp );
        iModel = tmp;
        // we need to Pop after ownership is passed to listbox
        needtopop = ETrue;
        }
    TBool isLocked( EFalse );
    iNeedUnlock = EFalse;
    iDataModel->AllListItemDataL( isLocked, *iModel, KEApIspTypeAll,
                                  EApBearerTypeAll, KEApSortNameAscending,
                                  iDataModel->RequestedIPvType(), 
                                  EVpnFilterNoVpn,
                                  EFalse );

    if ( isLocked )
        {
        iNeedUnlock = ETrue;
        }

    SetItemTextArray( iModel );
    if ( needtopop )
        { 
        // it signals that ownership of iModel had JUST been passed,
        // so we can Pop it.
        CleanupStack::Pop( iModel);
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::FillListBoxWithDataL")
    }




// ---------------------------------------------------------
// CApSelQueryDialog::LoadIconsL()
// called when listbox is constructed 
// ---------------------------------------------------------
//
TInt CApSelQueryDialog::LoadIconsL()
    {
    APSETUILOGGER_ENTERFN( EOther,"SelQuery::LoadIconsL")
    
    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KFileIcons, &KDC_APP_BITMAP_DIR, NULL ) );

    icons->AppendL( AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnIndiSettProtectedAdd,
                            mbmFile.FullName(), 
                            EMbmApsettingsQgn_indi_sett_protected_add, 
                            EMbmApsettingsQgn_indi_sett_protected_add_mask ) );

    SetIconArrayL( icons );

    CleanupStack::Pop(); // icons

    APSETUILOGGER_LEAVEFN( EOther,"SelQuery::LoadIconsL")
    return 0;
    }

// End of File
