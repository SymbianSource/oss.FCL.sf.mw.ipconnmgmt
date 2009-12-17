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
* Description:  Implementation of CApSelectorListbox.
*
*/


// INCLUDE FILES

#include <eikclbd.h>
#include <AknIconArray.h>
//#include <EIKON.mbg>
#include <avkon.mbg>
#include <aknkeys.h>
#include <ApSetUI.rsg>
#include <featmgr.h>

#include "ApSelectorListbox.h"
#include "ApSelectorListBoxModel.h"
#include "ApListItem.h"

#include <AknsUtils.h>
#include <apsettings.mbg>
#include <data_caging_path_literals.hrh>

#include "ApSettingsHandlerLogger.h"

// CONSTANTS
// Drive and file name of the MBM file containing icons for Protection
_LIT( KFileIcons, "z:ApSettings.mbm" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApSelectorListbox::NewL
// ---------------------------------------------------------
//
CApSelectorListbox* CApSelectorListbox::NewL
( const CCoeControl* aParent )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::NewL")
    
    CApSelectorListbox* listbox =
        new ( ELeave ) CApSelectorListbox();
    CleanupStack::PushL( listbox );
    listbox->ConstructL( aParent, EAknListBoxSelectionList );
    CleanupStack::Pop();    // listbox
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::NewL")
    return listbox;
    }


// ---------------------------------------------------------
// CApSelectorListbox::CApSelectorListbox
// ---------------------------------------------------------
//
CApSelectorListbox::CApSelectorListbox()
    {
    }


// ---------------------------------------------------------
// CApSelectorListbox::~CApSelectorListbox
// ---------------------------------------------------------
//
CApSelectorListbox::~CApSelectorListbox()
    {

    }


// ---------------------------------------------------------
// CApSelectorListbox::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelectorListbox::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::OfferKeyEventL")
    
    TKeyResponse retval;
    if (    aKeyEvent.iCode == EKeyOK &&
            !( aKeyEvent.iModifiers & EModifierShift )
       )
        {
        // Enter pressed (not Shift-Enter). This will report an
        // EEnterKeyPressed event sent to the observer (the view), which may
        // delete this listbox. The code which processes keypresses by
        // default, will continue (on the already deleted listbox), and
        // will crash. So we grab this keypress here, and generate the
        // same event, but after that, quit immediately!
        ReportListBoxEventL( MEikListBoxObserver::EEventEnterKeyPressed );
        // By now the listbox may have been deleted!
        // Do not access it after this point!
        retval = EKeyWasConsumed;
        }
    else
        {
        retval = CAknSingleGraphicStyleListBox::OfferKeyEventL
            ( aKeyEvent, aType );
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::OfferKeyEventL")
    return retval;
    }



// ---------------------------------------------------------
// CApSelectorListbox::FocusChanged
// ---------------------------------------------------------
//
void CApSelectorListbox::FocusChanged( TDrawNow aDrawNow )
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::FocusChanged")
    
    // Do nothing until the listbox is fully constructed
    // The dialogpage sets the focus before calling ConstructL
    if ( iView )
        {
        CAknSingleGraphicStyleListBox::FocusChanged( aDrawNow );
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::FocusChanged")
    }



// ---------------------------------------------------------
// CApSelectorListbox::LoadIconsL
// ---------------------------------------------------------
//
void CApSelectorListbox::LoadIconsL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::LoadIconsL")

    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );

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
                                            

    FeatureManager::InitializeLibL();
    TBool protsupported = FeatureManager::FeatureSupported( 
                                           KFeatureIdSettingsProtection );
    FeatureManager::UnInitializeLib();
    if ( protsupported )
        {
        icons->AppendL( AknsUtils::CreateGulIconL( 
                                skinInstance, 
                                KAknsIIDQgnIndiSettProtectedAdd,
                                mbmFile.FullName(), 
                                EMbmApsettingsQgn_indi_sett_protected_add, 
                                EMbmApsettingsQgn_indi_sett_protected_add_mask ) );
        }

    ItemDrawer()->ColumnData()->SetIconArray( icons );

    CleanupStack::Pop(); // icons
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::LoadIconsL")
    }


// ---------------------------------------------------------
// CApSelectorListbox::Uid4Item
// ---------------------------------------------------------
//
TUint32 CApSelectorListbox::Uid4Item( TInt aItem ) const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::Uid4Item<->")
    
    CApSelectorListboxModel* lbmodel =
        STATIC_CAST( CApSelectorListboxModel*, Model()->ItemTextArray() );
    return lbmodel->At( aItem )->Uid();
    }


// ---------------------------------------------------------
// CApSelectorListbox::CurrentItemUid
// ---------------------------------------------------------
//
TUint32 CApSelectorListbox::CurrentItemUid() const
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::CurrentItemUid")
    
    CApSelectorListboxModel* lbmodel =
        STATIC_CAST( CApSelectorListboxModel*, Model()->ItemTextArray() );
    TInt idx = CurrentItemIndex();
    TUint32 retval( 0 );
    if ( idx >= 0 )
        {
        retval = lbmodel->At( idx )->Uid();
        }
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::CurrentItemUid")
    return retval;
    }



// ---------------------------------------------------------
// CApSelectorListbox::CurrentItemNameL
// ---------------------------------------------------------
//
const TDesC& CApSelectorListbox::CurrentItemNameL()
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::CurrentItemNameL")
    
    CApSelectorListboxModel* lbmodel = new( ELeave )CApSelectorListboxModel;
    lbmodel = 
        STATIC_CAST( CApSelectorListboxModel*, Model()->ItemTextArray() );
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::CurrentItemNameL")
    return lbmodel->At( CurrentItemIndex() )->Name();
    }


// ---------------------------------------------------------
// CApSelectorListbox::HandleResourceChange
// ---------------------------------------------------------
//
void CApSelectorListbox::HandleResourceChange(TInt aType)
    {
    APSETUILOGGER_ENTERFN( EListbox,"SelListbox::HandleResourceChange")
    
    if ( aType == KAknsMessageSkinChange )
        {
        TRAP_IGNORE( LoadIconsL() );
        SizeChanged();
        }
        
    CAknSingleGraphicStyleListBox::HandleResourceChange( aType );
    
    APSETUILOGGER_LEAVEFN( EListbox,"SelListbox::HandleResourceChange")
    }

// End of File
