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
    return NULL;
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
    return NULL;
    }
    
    

// ---------------------------------------------------------
// CApSelPopupList::ConstructL
// ---------------------------------------------------------
//
void CApSelPopupList::ConstructL()
    {
    }



// Destructor
CApSelPopupList::~CApSelPopupList()
    {
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
    }


// ---------------------------------------------------------
// CApSelPopupList::GetHelpContext()
// ---------------------------------------------------------
//
void CApSelPopupList::GetHelpContext(TCoeHelpContext& aContext) const
    {
    }


// ---------------------------------------------------------
// CApSelPopupList::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelPopupList::HandleApDbEventL( TEvent anEvent )
    {
    }


// ---------------------------------------------------------
// CApSelPopupList::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelPopupList::ActivateL()
    {
    }



// ---------------------------------------------------------
// CApSelPopupList::SelectCurrentItemL()
// called after the setting had been changed
// ---------------------------------------------------------
//
void CApSelPopupList::SelectCurrentItemL()
    {
    }



// ---------------------------------------------------------
// CApSelPopupList::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelPopupList::OkToExitL( TInt aButtonId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApSelPopupList::FillListBoxWithDataL()
// called when listbox needs to be filled with data
// ---------------------------------------------------------
//
void CApSelPopupList::FillListBoxWithDataL()
    {
    }


// ---------------------------------------------------------
// CApSelPopupList::SetSelectedL()
// ---------------------------------------------------------
//
void CApSelPopupList::SetSelectedL()
    {
    }


// ---------------------------------------------------------
// CApSelPopupList::CheckIfEmptyL()
// ---------------------------------------------------------
//
void CApSelPopupList::CheckIfEmptyL()
    {
    }



// ---------------------------------------------------------
// CApSelPopupList::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApSelPopupList::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    }


// From MEikCommandObserver
// ---------------------------------------------------------
// CApSelPopupList::ProcessCommandL
// ---------------------------------------------------------
//
void CApSelPopupList::ProcessCommandL( TInt aCommandId )
    {
    }


    // From MEikListBoxObserver
void CApSelPopupList::HandleListBoxEventL( CEikListBox* aListBox,
                        MEikListBoxObserver::TListBoxEvent aEventType )
    {
    }


// From CCoeControl
// ---------------------------------------------------------
// CApSelPopupList::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApSelPopupList::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    User::Leave( KErrNotSupported );
    }




// ---------------------------------------------------------
// CApSelPopupList::LoadIconsL
// ---------------------------------------------------------
//
TInt CApSelPopupList::LoadIconsL()
    {
    return 0;
    }




// ---------------------------------------------------------
// CApSelPopupList::HandleResourceChange
// ---------------------------------------------------------
//
void CApSelPopupList::HandleResourceChange(TInt aType)
    {
    }

// End of File
