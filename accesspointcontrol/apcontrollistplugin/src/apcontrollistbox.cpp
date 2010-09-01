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
* Description:  Implementation of ApControlListbox.
*
*/


// INCLUDE FILES

#include <eikclbd.h>
#include <avkon.mbg>
#include <aknkeys.h>
#include <apcontrollistpluginrsc.rsg>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>

#include "apcontrollistbox.h"
#include "apcontrollistboxmodel.h"
#include "apcontrollistpluginlogger.h"
#include "apcontrollistplugin.h"
#include "apcontrollistapiwrapper.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CApControlListbox::CApControlListbox
// ---------------------------------------------------------
//
CApControlListbox::CApControlListbox() 
    {
    CLOG( ( ESelector, 0, 
        _L( "<-> CApControlListbox::CApControlListbox" ) ) );
    }


// ---------------------------------------------------------
// CApControlListbox::~CApControlListbox
// ---------------------------------------------------------
//
CApControlListbox::~CApControlListbox()
    {
    CLOG( ( ESelector, 0, 
        _L( "<-> CApControlListbox::~CApControlListbox" ) ) );
    }


// ---------------------------------------------------------
// CApControlListbox::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CApControlListbox::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    CLOG( ( ESelector, 0, _L( "-> CApControlListbox::OfferKeyEventL" ) ) );

    TKeyResponse retval ( EKeyWasConsumed );

    if (    aKeyEvent.iCode == EKeyOK &&
            !( aKeyEvent.iModifiers & EModifierShift )
       )
        {
        // Enter pressed (not Shift-Enter). This will report an
        // EEnterKeyPressed event sent to the observer (the view), which may
        // delete this listbox. The code which processes keypresses by
        // default, will continue (on the already deleted listbox), and
        // will crash. So we grab this keypress here, and generate the
        // same event, but after that, quit immediately.
        ReportListBoxEventL( MEikListBoxObserver::EEventEnterKeyPressed );
        // By now the listbox may have been deleted.
        // Do not access it after this point.
        }
    else if (    aKeyEvent.iCode == EKeyBackspace )
        {
        static_cast<CAPControlListPlugin*>(iListBoxObserver)->APIWrapper()->
            RemoveAPN();
        }
    else
        {
        retval = CAknSingleStyleListBox::OfferKeyEventL
                            ( aKeyEvent, aType );
        }

    CLOG( ( ESelector, 0, _L( "<- CApControlListbox::OfferKeyEventL" ) ) );

    return retval;
    }



// ---------------------------------------------------------
// CApControlListbox::FocusChanged
// ---------------------------------------------------------
//
void CApControlListbox::FocusChanged( TDrawNow aDrawNow )
    {
    CLOG( ( ESelector, 0, _L( "-> CApControlListbox::FocusChanged" ) ) );

    // Do nothing until the listbox is fully constructed
    // The dialogpage sets the focus before calling ConstructL
    if ( iView )
        {
        CAknSingleStyleListBox::FocusChanged( aDrawNow );
        }

    CLOG( ( ESelector, 0, _L( "<- CApControlListbox::FocusChanged" ) ) );
    }


// ---------------------------------------------------------
// CApControlListbox::HandleResourceChange
// ---------------------------------------------------------
//
void CApControlListbox::HandleResourceChange(TInt aType)
    {
    CLOG( ( ESelector, 0, _L( 
    				"-> CApControlListbox::HandleResourceChange" ) ) );
    if ( aType == KAknsMessageSkinChange )
        {        CAknSingleStyleListBox::HandleResourceChange( aType );
        SizeChanged();
        }    
        
    CAknSingleStyleListBox::HandleResourceChange( aType );
    CLOG( ( ESelector, 0, _L( 
    				"<- CApControlListbox::HandleResourceChange" ) ) );
    }
// ---------------------------------------------------------
// CApControlListbox::AddToListBoxL
// ---------------------------------------------------------
//
void CApControlListbox::SetListEmptyTextL()
    {
    CLOG( ( ESelector, 0, _L( 
        "-> CApControlListbox::SetListEmptyTextL" ) ) );
    HBufC* primary = 
              iEikonEnv->AllocReadResourceLC( R_QTN_ACL_EMPTY_VIEW_PRIMARY );
//    _LIT( KCmNewLine, "\n" );    
//    primary->Des().Append( KCmNewLine );    
    HBufC* secondary = 
              iEikonEnv->AllocReadResourceLC( R_QTN_ACL_EMPTY_VIEW_SECONDARY );
    CDesCArrayFlat* items = new (ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(items);
    items->AppendL(primary->Des()); 
    items->AppendL(secondary->Des());      
    HBufC* emptyText = 
          StringLoader::LoadLC( R_TWO_STRING_FOR_EMPTY_VIEW , *items);              
    View()->SetListEmptyTextL( *emptyText );    
    CleanupStack::PopAndDestroy( emptyText );
    CleanupStack::PopAndDestroy( items );
    CleanupStack::PopAndDestroy( secondary );
    CleanupStack::PopAndDestroy( primary );
    
    CLOG( ( ESelector, 0, _L( 
        "<- CApControlListbox::SetListEmptyTextL" ) ) );
    }

// End of File
