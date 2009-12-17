/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CDestListbox
*
*/

#include <eikclbd.h>
#include <avkon.mbg>
#include <aknkeys.h>
#include <data_caging_path_literals.hrh>
#include <cmmanager.mbg> 

#include "destlistbox.h"
#include "destlistboxmodel.h"
#include "destlistitem.h"

// ---------------------------------------------------------------------------
// CDestListbox::NewL
// ---------------------------------------------------------------------------
//
CDestListbox* CDestListbox::NewL( const CCoeControl* aParent )
    {
    CDestListbox* listbox = new ( ELeave ) CDestListbox();
    CleanupStack::PushL( listbox );
    listbox->ConstructL( aParent, EAknListBoxSelectionList );
    CleanupStack::Pop( listbox );
    return listbox;
    }

// ---------------------------------------------------------------------------
// CDestListbox::CDestListbox
// ---------------------------------------------------------------------------
//
CDestListbox::CDestListbox()
    {
    }

// ---------------------------------------------------------------------------
// CDestListbox::~CDestListbox
// ---------------------------------------------------------------------------
//
CDestListbox::~CDestListbox()
    {
    }

// ---------------------------------------------------------------------------
// CDestListbox::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CDestListbox::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyOK &&
         !( aKeyEvent.iModifiers & EModifierShift ) )
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
        return EKeyWasConsumed;
        }
    else
        {
        return CAknDoubleLargeStyleListBox::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------------------------
// CDestListbox::FocusChanged
// ---------------------------------------------------------------------------
//
void CDestListbox::FocusChanged( TDrawNow aDrawNow )
    {
    // Do nothing until the listbox is fully constructed
    // The dialogpage sets the focus before calling ConstructL
    if ( iView )
        {
        CAknDoubleLargeStyleListBox::FocusChanged( aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CDestListbox::Uid4Item
// ---------------------------------------------------------------------------
//
TUint32 CDestListbox::Uid4Item( TInt aItem ) const
    {
    CDestListboxModel* lbmodel =
        STATIC_CAST( CDestListboxModel*, Model()->ItemTextArray() );
    return lbmodel->At( aItem )->Uid();
    }

// ---------------------------------------------------------------------------
// CDestListbox::CurrentItemUid
// ---------------------------------------------------------------------------
//
TUint32 CDestListbox::CurrentItemUid() const
    {
    CDestListboxModel* lbmodel =
        STATIC_CAST( CDestListboxModel*, Model()->ItemTextArray() );
    TInt idx = CurrentItemIndex();
    TUint32 retval( 0 );
    if ( idx >= 0 )
        {
        retval = lbmodel->At( idx )->Uid();
        }
    return retval;
    }

// ---------------------------------------------------------------------------
// CDestListbox::CurrentItemNameL
// ---------------------------------------------------------------------------
//
const TDesC& CDestListbox::CurrentItemNameL()
    {
    CDestListboxModel* lbmodel = new( ELeave )CDestListboxModel;
    lbmodel = STATIC_CAST( CDestListboxModel*, Model()->ItemTextArray() );
    return lbmodel->At( CurrentItemIndex() )->Name();
    }

// ---------------------------------------------------------------------------
// CDestListbox::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CDestListbox::HandleResourceChange(TInt aType)
    {
    if ( aType == KAknsMessageSkinChange )
        {
        SizeChanged();
        }
        
    CAknDoubleLargeStyleListBox::HandleResourceChange( aType );     
    }

