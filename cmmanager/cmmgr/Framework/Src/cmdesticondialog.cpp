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
* Description:  Implementation of CCmDestinationIconDialog
*
*/

// INCLUDE FILES
#include <uikon.hrh>
#include <e32std.h>
#include <eikcapc.h>

#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>

#include <aknborders.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <aknPopupHeadingPane.h>
#include <AknSettingCache.h>

#include <AknLayout.lag>
#include <e32property.h>

#include <cmmanager.rsg>

#include "cmmanager.hrh"
#include "cmdesticondialog.h"
#include "cmdesticonmap.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCmDestinationIconDialog class
// CCmDestinationIconDialog() - constructor
// ----------------------------------------------------------------------------
//
CCmDestinationIconDialog::CCmDestinationIconDialog( TInt& aIconIndex )
    : iIconIndex(&aIconIndex)
    {
    CEikDialog::SetBorder( AknBorderId::EAknBorderNotePopup );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::~CCmDestinationIconDialog()
// -----------------------------------------------------------------------------
//
CCmDestinationIconDialog::~CCmDestinationIconDialog()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::HandleResourceChange(TInt aType)
    {
    CAknDialog::HandleResourceChange(aType);

    if (aType==KEikDynamicLayoutVariantSwitch)
        {
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::OkToExitL()
// -----------------------------------------------------------------------------
//
TBool CCmDestinationIconDialog::OkToExitL(TInt aButtonId)
    {
    CCmDestinationIconMap* iconmapControl = 
        STATIC_CAST(CCmDestinationIconMap*, Control(ECmDestIconMapContentId));
    // Selection key / select softkey adds a iconacter only if 5-key was not 
    // used to select multiple icons before.
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect )
        {
        TKeyEvent key;
        key.iCode=EKeyOK;
        key.iModifiers=0;
        iconmapControl->OfferKeyEventL(key, EEventKey);
        }
    return(ETrue);
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::PreLayoutDynInitL()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::PreLayoutDynInitL()
    {
    CCmDestinationIconMap* iconmapControl = 
       STATIC_CAST( CCmDestinationIconMap*, Control( ECmDestIconMapContentId ));
    CAknPopupHeadingPane* headingPane =
       STATIC_CAST( CAknPopupHeadingPane*, Control( EAknSCTQueryHeadingId ));

    iconmapControl->SetIndex(*iIconIndex);    

    SetLineNonFocusing(EAknSCTQueryHeadingId);

    // add title    
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* title = env->AllocReadResourceL( R_CMMANAGERUI_PRMPT_SELECT_ICON );
    CleanupStack::PushL( title );
    headingPane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    title = NULL;

#ifdef RD_SCALABLE_UI_V2

	iconmapControl->SetObserver(this);
	
#endif // RD_SCALABLE_UI_V2
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::SetSizeAndPosition()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::SetSizeAndPosition( const TSize& aSize )
    {
    SetBorder( TGulBorder::ENone ); // No Dialog borders in SCT.
    CAknDialog::SetSizeAndPosition( aSize );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CCmDestinationIconDialog::OfferKeyEventL(
                             const TKeyEvent& aKeyEvent, TEventCode aModifiers )
    {
    if ( aModifiers == EEventKey )
        {
        TUint code=aKeyEvent.iCode;
        CCmDestinationIconMap* iconmapControl =
                            STATIC_CAST( CCmDestinationIconMap*, 
                                         Control( ECmDestIconMapContentId ));

        switch ( code )
            {
            case EKeyLeftArrow:
            case EKeyRightArrow:
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                TKeyResponse res(
                    iconmapControl->OfferKeyEventL( aKeyEvent, aModifiers ));
                return res;
                }
                
            case EKeyEnter:
                // change fro EKeyEnter to EKeyOK
                {
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyOK;
                keyEvent.iScanCode = aKeyEvent.iScanCode;
                keyEvent.iModifiers = aKeyEvent.iModifiers;
                keyEvent.iRepeats = aKeyEvent.iRepeats;
                return CEikDialog::OfferKeyEventL( keyEvent, aModifiers );
                }
                
            default:
                break;
            }
        }
        
    return CEikDialog::OfferKeyEventL( aKeyEvent, aModifiers );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::ExecuteLD()
// -----------------------------------------------------------------------------
//
TInt CCmDestinationIconDialog::ExecuteLD()
    {
    // Prepare real icon map
    CEikDialog::PrepareLC( R_ICONSEL_DIALOG );
    return(RunLD());
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::CEikDialog_Reserved_1()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::CEikDialog_Reserved_1()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::CEikDialog_Reserved_2()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::CEikDialog_Reserved_2()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::CCmDestinationIconDialog_Reserved()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::CAknIconMapDialog_Reserved()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::CreateCustomControlL()
// Virtual function override from the base class to implement the custom control
// -----------------------------------------------------------------------------
//    
SEikControlInfo CCmDestinationIconDialog::CreateCustomControlL(
    TInt aControlType)
    {
    CCmDestinationIconMap *control= NULL;
    if ( aControlType == KCmDestIconMapType )
        {
        control = CCmDestinationIconMap::NewL();
        }
    SEikControlInfo info;
    info.iControl = control;
    info.iFlags = 0;
    info.iTrailerTextId = 0;
    return info;
    }

#ifdef RD_SCALABLE_UI_V2

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::HandleControlEventL
// Handles the event.
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::HandleControlEventL( 
                                                CCoeControl* /*aControl*/,
                                                TCoeEvent aEventType )
    {
    if( aEventType == EEventStateChanged)
		{
	    iConSelected = ETrue;
		}
    } 

// -----------------------------------------------------------------------------
// CCmDestinationIconDialog::HandleDialogPageEventL
// Handles the event.
// -----------------------------------------------------------------------------
//
void CCmDestinationIconDialog::HandleDialogPageEventL(TInt aEventID)
	{
    if ( AknLayoutUtils::PenEnabled() && ( aEventID == 
         MEikDialogPageObserver::EDialogPageTapped && iConSelected ) )
	    {
	    TryExitL( EAknSoftkeyOk );
	    }
	}
	
#endif //RD_SCALABLE_UI_V2

//  End of File
