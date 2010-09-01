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
* Description:  Implementation of CCmListbox
*
*/

#include <eikclbd.h>
#include <avkon.mbg>
#include <aknkeys.h>
#include <data_caging_path_literals.hrh>

#include "cmlistbox.h"
#include "cmlistboxmodel.h"
#include "cmlistitem.h"


// ---------------------------------------------------------------------------
// CCmListbox::NewL
// ---------------------------------------------------------------------------
//
CCmListbox* CCmListbox::NewL( const CCoeControl* aParent )
    {
    CCmListbox* listbox = new ( ELeave ) CCmListbox();
    CleanupStack::PushL( listbox );
    listbox->ConstructL( aParent, EAknListBoxSelectionList );
    CleanupStack::Pop( listbox );    
    return listbox;
    }

// ---------------------------------------------------------------------------
// CCmListbox::CCmListbox
// ---------------------------------------------------------------------------
//
CCmListbox::CCmListbox()
    {
    }

// ---------------------------------------------------------------------------
// CCmListbox::~CCmListbox
// ---------------------------------------------------------------------------
//
CCmListbox::~CCmListbox()
    {
    }

// ---------------------------------------------------------------------------
// CCmListbox::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCmListbox::OfferKeyEventL
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
        return CAknDoubleGraphicStyleListBox::OfferKeyEventL
            ( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------------------------
// CCmListbox::FocusChanged
// ---------------------------------------------------------------------------
//
void CCmListbox::FocusChanged( TDrawNow aDrawNow )
    {
    // Do nothing until the listbox is fully constructed
    // The dialogpage sets the focus before calling ConstructL
    if ( iView )
        {
        CAknDoubleGraphicStyleListBox::FocusChanged( aDrawNow );
        }
    }

// ---------------------------------------------------------------------------
// CCmListbox::Uid4Item
// ---------------------------------------------------------------------------
//
TUint32 CCmListbox::Uid4Item( TInt aItem ) const
    {
    CCmListboxModel* lbmodel =
        STATIC_CAST( CCmListboxModel*, Model()->ItemTextArray() );
    return lbmodel->At( aItem )->Uid();
    }

// ---------------------------------------------------------------------------
// CCmListbox::CurrentItemUid
// ---------------------------------------------------------------------------
//
TUint32 CCmListbox::CurrentItemUid() const
    {
    CCmListboxModel* lbmodel =
        STATIC_CAST( CCmListboxModel*, Model()->ItemTextArray() );
    TInt idx = CurrentItemIndex();
    TUint32 retval( 0 );
    if ( idx >= 0 )
        {
        retval = lbmodel->At( idx )->Uid();
        }
    return retval;
    }

// ---------------------------------------------------------------------------
// CCmListbox::CurrentItemNameL
// ---------------------------------------------------------------------------
//
const TDesC& CCmListbox::CurrentItemNameL()
    {
    CCmListboxModel* lbmodel = new( ELeave )CCmListboxModel;
    lbmodel = STATIC_CAST( CCmListboxModel*, Model()->ItemTextArray() );
    return lbmodel->At( CurrentItemIndex() )->Name();
    }

// ---------------------------------------------------------------------------
// CCmListbox::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CCmListbox::HandleResourceChange(TInt aType)
    {
    if ( aType == KAknsMessageSkinChange )
        {
        SizeChanged();
        }
        
    CAknDoubleGraphicStyleListBox::HandleResourceChange( aType );     
    }
