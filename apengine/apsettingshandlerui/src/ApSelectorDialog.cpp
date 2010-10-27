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
    return 0;
    }




// ---------------------------------------------------------
// CApSelectorDialog::~CApSelectorDialog
// Destructor
// ---------------------------------------------------------
//
CApSelectorDialog::~CApSelectorDialog()
    {
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
    return NULL;
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
    }



// ---------------------------------------------------------
// CApSelectorDialog::HandleListboxDataChangeL
// called before the dialog is shown to initialize listbox data
// ---------------------------------------------------------
//
void CApSelectorDialog::HandleListboxDataChangeL()
    {
    }



// ---------------------------------------------------------
// CApSelectorDialog::SetTextOverrides
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::SetTextOverrides( CTextOverrides *aOverrides )
    {
    }




// ---------------------------------------------------------
// CApSelectorDialog::CheckIfEmptyL()
// called on activation and after the db has changed
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelectorDialog::CheckIfEmptyL()
    {
    }


// ---------------------------------------------------------
// CApSelectorDialog::ActivateL()
// called after the dialog is shown
// used to handle empty list - query
// ---------------------------------------------------------
//
void CApSelectorDialog::ActivateL()
    {
    }



// ---------------------------------------------------------
// CApSelectorDialog::GetHelpContext()
// ---------------------------------------------------------
//
void CApSelectorDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    }



// ---------------------------------------------------------
// CApSelectorDialog::HandleApDbEventLw
// called by the active access point framework
// ---------------------------------------------------------
//
void CApSelectorDialog::HandleApDbEventL( TEvent anEvent )
    {
    }



// From MEikCommandObserver
void CApSelectorDialog::ProcessCommandL( TInt aCommandId )
    {
    }




// From MEikListBoxObserver
void CApSelectorDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                             TListBoxEvent aEventType )
    {
    }



//----------------------------------------------------------
// CApSelectorDialog::CreateCustomControlL
//----------------------------------------------------------
//
SEikControlInfo CApSelectorDialog::CreateCustomControlL
( TInt aControlType )
    {
    User::Leave( KErrNotSupported );
    }


//----------------------------------------------------------
// CApSelectorDialog::DynInitMenuPaneL
//----------------------------------------------------------
//
void CApSelectorDialog::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    }



// ---------------------------------------------------------
// CApSelectorDialog::OfferKeyEventL();
// ---------------------------------------------------------
//
TKeyResponse CApSelectorDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                               TEventCode aType)
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApSelectorDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::PreLayoutDynInitL()
    {
    }


// ---------------------------------------------------------
// CApSelectorDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------
//
void CApSelectorDialog::PostLayoutDynInitL()
    {
    }


// ---------------------------------------------------------
// CApSelectorDialog::OkToExitL( TInt aButtonId )
// called by framework when the OK button is pressed
// ---------------------------------------------------------
//
TBool CApSelectorDialog::OkToExitL( TInt aButtonId )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSelectorDialog::SetHighlighted()
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApSelectorDialog::SetHighlighted()
    {
    }






// ---------------------------------------------------------
// CApSelectorDialog::DoProcessCommandL
// called when needs to change the highlighting
// ---------------------------------------------------------
//
void CApSelectorDialog::DoProcessCommandL( TInt aCommandId )
    {
    }



// End of File

