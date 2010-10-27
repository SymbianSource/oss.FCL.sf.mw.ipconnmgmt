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
#include <apsetui.rsg>
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
    return NULL;
    }


// ---------------------------------------------------------
// CApNetSelPopupList::ConstructL
// ---------------------------------------------------------
//
void CApNetSelPopupList::ConstructL()
    {
    }


// Destructor
CApNetSelPopupList::~CApNetSelPopupList()
    {
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
    }


// ---------------------------------------------------------
// CApNetSelPopupList::GetHelpContext()
// ---------------------------------------------------------
//
void CApNetSelPopupList::GetHelpContext(TCoeHelpContext& aContext) const
    {
    }


// ---------------------------------------------------------
// CApNetSelPopupList::HandleApDbEventL
// called by the active access point framework
// ---------------------------------------------------------
//
void CApNetSelPopupList::HandleApDbEventL( TEvent anEvent )
    {
    }


// ---------------------------------------------------------
// CApNetSelPopupList::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApNetSelPopupList::ActivateL()
    {
    }



// ---------------------------------------------------------
// CApNetSelPopupList::SelectCurrentItemL()
// called after the setting had been changed
// ---------------------------------------------------------
//
void CApNetSelPopupList::SelectCurrentItemL()
    {
    }



// ---------------------------------------------------------
// CApNetSelPopupList::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::OkToExitL( TInt aButtonId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApNetSelPopupList::FillListBoxWithDataL()
// called when listbox needs to be filled with data
// ---------------------------------------------------------
//
void CApNetSelPopupList::FillListBoxWithDataL()
    {
    }


// ---------------------------------------------------------
// CApNetSelPopupList::SetSelectedL()
// ---------------------------------------------------------
//
void CApNetSelPopupList::SetSelectedL()
    {
    }




// ---------------------------------------------------------
// CApNetSelPopupList::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CApNetSelPopupList::DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane )
    {
    }


// From MEikCommandObserver
// ---------------------------------------------------------
// CApNetSelPopupList::ProcessCommandL
// ---------------------------------------------------------
//
void CApNetSelPopupList::ProcessCommandL( TInt aCommandId )
    {
    }



// From CCoeControl
// ---------------------------------------------------------
// CApNetSelPopupList::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApNetSelPopupList::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApNetSelPopupList::Need2DeleteSelect
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::Need2DeleteSelect( TInt aResourceId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApNetSelPopupList::Need2DeleteOpenDelete
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::Need2DeleteOpenDelete( TInt aResourceId )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApNetSelPopupList::HandleCreateNewNetworkGroupL
// ---------------------------------------------------------
//
TBool CApNetSelPopupList::HandleCreateNewNetworkGroupL()
    {
    return EFalse;
    }

// End of File
