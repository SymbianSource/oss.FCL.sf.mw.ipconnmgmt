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
* Description:  Implementation of CCmDestinationIconMap
*
*/

#include <eikenv.h>
#include <eiksbfrm.h>
#include <eikscrlb.h>
#include <bidivisual.h>
#include <avkon.rsg>
#include <AknPanic.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknLayout.lag>
#include <data_caging_path_literals.hrh>
#include <aknappui.h>
#include <aknconsts.h>
#include <AknDef.h>
#include <PUAcodes.hrh>
#include <s32file.h>
#include <eikscrlb.h>   // TEikScrollBarModelType
#include <AknsDrawUtils.h>
#include <featmgr.h>
#include <AknsFrameBackgroundControlContext.h>
#include <cmmanager.mbg>
#include <cmcommonconstants.h>
#include <touchfeedback.h>
#include "cmdesticonmap.h"
#include "cmmanagerimpl.h"

// The offset because of CEikDialogPage
const TInt KHorizontalDialogMargin = 0;
const TInt KVerticalDialogMargin = 0;
const TInt KAknSctCBaButtonDirections = 3; // bottom, right and left

/**
 * Extension class to CCmDestinationIconMap
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmDestinationIconMapExtension ) : public CBase, 
                                                      public MObjectProvider
    {
    public:
    
        CCmDestinationIconMapExtension();
        ~CCmDestinationIconMapExtension();
    
    protected:
    
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        MObjectProvider* MopNext();
    
    public: // data
        
        MCoeControlObserver *iObserver;
        
        TUint iFlags;
        MObjectProvider* iIconMap;       
        TInt iMaxVisibleRows;
        CAknsFrameBackgroundControlContext* iBgContext;
    };

// -----------------------------------------------------------------------------
// CCmDestinationIconMapExtension::CCmDestinationIconMapExtension
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCmDestinationIconMapExtension::CCmDestinationIconMapExtension()
    : iMaxVisibleRows( 0 )
    {
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMapExtension::MopSupplyObject()
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCmDestinationIconMapExtension::MopSupplyObject( TTypeUid aId )
    {
    return MAknsControlContext::SupplyMopObject( aId, iBgContext );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMapExtension::MopNext()
// -----------------------------------------------------------------------------
//    
MObjectProvider* CCmDestinationIconMapExtension::MopNext()
    {
    return iIconMap;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMapExtension::~CCmDestinationIconMapExtension()
// -----------------------------------------------------------------------------
//
CCmDestinationIconMapExtension::~CCmDestinationIconMapExtension()
    {
    delete iBgContext;
    iBgContext = NULL;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CCmDestinationIconMap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCmDestinationIconMap::CCmDestinationIconMap() 
    : iCursorPos( TPoint( 0,0 ) )
    , iOldCursorPos( TPoint( 0,0 ) )
    , iMaxColumns( -1 )
    , iDragAlreadyActive( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCmDestinationIconMap* CCmDestinationIconMap::NewL()
    {
    CCmDestinationIconMap* self = new( ELeave ) CCmDestinationIconMap();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;    
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::ConstructL()
    {
    // Must be created here to get the member variables available
    iExtension = new ( ELeave ) CCmDestinationIconMapExtension;
    iExtension->iIconMap = this;
        
    iConsArray = new( ELeave ) CAknIconArray( KNumOfIcons );
    
    iExtension->iFlags = 0x00;
        
    DoLayout();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::~CCmDestinationIconMap()
// -----------------------------------------------------------------------------
//
CCmDestinationIconMap::~CCmDestinationIconMap()
    {
    delete iSBFrame; iSBFrame = NULL;
    
    delete iOffscreenBg;
    delete iBitmapDevice;
    delete iBitmapGc;

    delete iExtension; 
    iExtension = ( CCmDestinationIconMapExtension* )NULL;
    
    if ( iConsArray )
        {
        iConsArray->ResetAndDestroy();
        delete iConsArray;
        iConsArray = NULL;
        }
   }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::DoLayout()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::DoLayout() 
    {
    TRAPD( err,LoadIconL() );
    if ( err )
        {
        return;
        }
    
    iIsMirrored = AknLayoutUtils::LayoutMirrored();
    iDrawnBefore = EFalse;
    CountMaxColumnsAndCellSizes();
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::ConstructFromResourceL()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::ConstructFromResourceL( 
                                                TResourceReader& /*aReader*/ )
    {
    LoadIconL();
    CreateScrollBarAndIconRowL();

    iExtension->iBgContext = CAknsFrameBackgroundControlContext::NewL( 
                                                            KAknsIIDQsnFrPopup, 
                                                            TRect( 0, 0, 1, 1 ), 
                                                            TRect( 0, 0, 1, 1 ), 
                                                            EFalse );

    if ( DrawableWindow() && AknLayoutUtils::PenEnabled() )
        {
        EnableDragEvents();       
        SetGloballyCapturing( ETrue );
        SetPointerCapture( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::LoadIcons()
// This actually load the all icons to be shown on dialog from icon file
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::LoadIconL()
    { 
    iConsArray->ResetAndDestroy();
    // Draw all the Icons.
    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                                     &KDC_APP_BITMAP_DIR, NULL ) );
        
    // Create icon bitmap and mask.
    for ( TInt i( 0 ); i < ( KNumOfIcons * 2 ); i++ )
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* bitmapMask = NULL;
        AknIconUtils::CreateIconLC( 
                   bitmap, 
                   bitmapMask, mbmFile.FullName(),
                   EMbmCmmanagerQgn_prop_set_conn_dest_internet_small + i, 
                   EMbmCmmanagerQgn_prop_set_conn_dest_internet_small + i + 1 );
        i++;
        AknIconUtils::SetSize( bitmap, 
                               TSize( iGridItemWidth, iGridItemHeight ) );
        AknIconUtils::SetSize( bitmapMask, 
                               TSize( iGridItemWidth, iGridItemHeight ) );
        CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
        CleanupStack::PushL( icon );
        iConsArray->AppendL( icon );       
        CleanupStack::Pop( 3, bitmap );
        }        
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::HeightInRows()
// This actually returns the no of rows to be shown on a page
// Depends upon the Layout size
// -----------------------------------------------------------------------------
//
TInt CCmDestinationIconMap::HeightInRows()
    {
    return ( iRows );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CreateScrollBarAndIconRowL()
// This actually creates the scroll bar sets the number of 
// pages and rows on a page to be shown
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::CreateScrollBarAndIconRowL()
    {
    iRows = ( ( iConsArray->Count() - 1 ) / iMaxColumns ) + 1 ;
    iFirstVisibleRow = 0;
    iAnimated = EFalse;
    iCursorPos = TPoint( 0, 0 );
    
    // Create and set the scb visible even though there is nothing to scroll   
    delete iSBFrame; iSBFrame=NULL;

    if ( AknLayoutUtils::PenEnabled() )
        {
        iSBFrame=new( ELeave ) CEikScrollBarFrame( this, this, ETrue ); 
        }
    else
        {
        iSBFrame=new( ELeave ) CEikScrollBarFrame( this, NULL, ETrue );
        }
    // Decide which type of scrollbar is shown
    CAknAppUi* appUi = iAvkonAppUi;
    if ( AknLayoutUtils::DefaultScrollBarType( appUi ) == 
         CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        if ( AknLayoutUtils::PenEnabled() )
            {
            // window owning scrollbar
            iSBFrame->CreateDoubleSpanScrollBarsL( ETrue, 
                                                   EFalse, 
                                                   ETrue, 
                                                   EFalse );
            }
        else
            {
            // non-window owning scrollbar
            iSBFrame->CreateDoubleSpanScrollBarsL( EFalse, 
                                                   EFalse, 
                                                   ETrue, 
                                                   EFalse ); 
            }
        iSBFrame->SetTypeOfVScrollBar( CEikScrollBarFrame::EDoubleSpan );
        }
    else
        {
        // For EArrowHead type scrollbar
        iSBFrame->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
        }
    
    iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                       CEikScrollBarFrame::EAuto );
    iSBFrame->VerticalScrollBar()->SetMopParent( iExtension );
    UpdateScrollIndicatorL();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::SetIndex
// set the reference of the selected icon index from the table
// -----------------------------------------------------------------------------
// 
void CCmDestinationIconMap::SetIndex( TInt& aIconIndex )
    {
    iIconIndex = &aIconIndex;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::MinimumSize
// 
// -----------------------------------------------------------------------------
//
TSize CCmDestinationIconMap::MinimumSize()
    {    
    iIsMirrored = AknLayoutUtils::LayoutMirrored();
    CountMaxColumnsAndCellSizes();

    TRect rect;
    // Used the set rect, but resolution changes cannot be handled when it is used
              
    TAknLayoutRect dialogLayRect;
        
    // Main pane without softkeys
    TRect mainPaneRect;
    if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect ) )
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }
    
    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety = 
               AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
    
    // Calc the variety
    TInt maxVariety = iconMapDialogVariety.LastVariety();
    
    // Check the CBA, if the orientation is not landscape 
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    // the offset for the certain cba location variety
    TInt maxVarietyOffset = 0; 
    // the number of varieties
    TInt varietyOffset = maxVariety + 1;
    
    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the 
    // variety
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
       {
       // the offset for one variety
       varietyOffset = ( maxVariety + 1 ) / KAknSctCBaButtonDirections;
       }
    
    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending order
    // e.g the biggest sized layout first, the smallest last
    if ( location == AknLayoutUtils::EAknCbaLocationRight )
       {
       maxVarietyOffset = varietyOffset;
       }
    else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
       {
       maxVarietyOffset = varietyOffset + varietyOffset; // 2*
       }
    
    TInt varietyNumber = varietyOffset - iRows - 1; 
    
    // if more lines than possible to show, use the default 
    // ( the biggest grid ) variety
    if ( varietyNumber < 0 )
        {
        varietyNumber = 0;
        }
    // if zero rows, use the minimum
    else if ( iRows<=0 )
        {
        varietyNumber -= 1;
        }
    
    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if ( Layout_Meta_Data::IsLandscapeOrientation() && 
        location == AknLayoutUtils::EAknCbaLocationRight )
       {
       varietyNumber = 10;
       }
    else
        {
        if( iRows == 0 )
            {
            varietyNumber = 5;
            }
        else
            {
            varietyNumber = 3;
            }
        }       
    // Layout the dialog size
    dialogLayRect.LayoutRect( mainPaneRect, 
       AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );
       
    // Layout the grid
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect( dialogLayRect.Rect(), 
       AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
    
    return TSize( dialogLayRect.Rect().Width(), 
        gridWithScrollLayRect.Rect().Height() );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::ActivateL()
// This method is needed to set correct initial value to scroll indicator.
// -----------------------------------------------------------------------------
//
 void CCmDestinationIconMap::ActivateL()
    {
    CCoeControl::ActivateL();
    if ( iRows > iExtension->iMaxVisibleRows )
        {
        UpdateScrollIndicatorL();
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::OfferKeyEventL
// Handles all the Keypad events
// -----------------------------------------------------------------------------
//
TKeyResponse CCmDestinationIconMap::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                    TEventCode /*aModifiers*/ )
    {
    TUint code=aKeyEvent.iCode;
    
    switch ( code )
        {
        case EKeyLeftArrow:
        case '4':
            MoveCursorL( -1,0 );
            break;
        case EKeyRightArrow:
        case '6':
            MoveCursorL( 1,0 );
            break;
        case EKeyUpArrow:
        case '2':
            MoveCursorL( 0,-1 );
            break;
        case EKeyDownArrow:
        case '8':
            MoveCursorL( 0,1 );
            break;
        case EKeyOK:
        case '5':  
        case EKeySpace:
            {
            if ( iConsArray )
                {
                TInt ret = iMaxColumns * 
                           ( iFirstVisibleRow + iCursorPos.iY ) + 
                           iCursorPos.iX;
                if ( ret <= iConsArray->Count() )
                    {
                    *iIconIndex = ret;    
                    }
                else
                    {
                    *iIconIndex = -1;
                    }
                }
               
            }
            break;
        default:
            return EKeyWasNotConsumed;

        }
    return EKeyWasConsumed;
    }

#ifdef RD_SCALABLE_UI_V2 

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::HandlePointerEventL
// Handles all the Keypad events
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::HandlePointerEventL(
                                const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        TInt xInPixels = aPointerEvent.iPosition.iX - iGridTopLeft.iX;
        TInt yInPixels = aPointerEvent.iPosition.iY - iGridTopLeft.iY;
        TInt newGridX = xInPixels / iGridItemWidth;
        TInt newGridY = yInPixels / iGridItemHeight;
        TInt globalY = newGridY + iFirstVisibleRow;

        // The pointer has been set down or dragged into the area of the grid. 
        // Also, it may NOT be in the "empty slots" at the end of the grid.
        if ( xInPixels >= 0 &&
                yInPixels >= 0 && 
                newGridX < iMaxColumns &&
                yInPixels < iGridItemHeight * iExtension->iMaxVisibleRows &&
                globalY * iMaxColumns + newGridX + 1 <= iConsArray->Count() )
            {
            // For any action to be taken, the pointer event must either be a
            // Button1Down or a drag event which has originated from a
            // Button1Down in to the grid.
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down ||
                 aPointerEvent.iType == TPointerEvent::EDrag )
                {
                if ( aPointerEvent.iType == TPointerEvent::EDrag ) 
                    {
                    iDragAlreadyActive = ETrue;
                    }

                // The user tapps a cell which has no icon. it is ignored.
                if ( iConsArray->Count() > globalY * iMaxColumns + newGridX )
                    {
              	    // If icon is already selected then on single tap it
              	    // should select the icon.
                    if( iCursorPos.iY == newGridY && 
                        iCursorPos.iX == newGridX &&
                        aPointerEvent.iType != TPointerEvent::EDrag )
                        {
                        if ( iExtension->iObserver )
                            {
                            iExtension->iObserver->HandleControlEventL( 
                                  this, 
                                  MCoeControlObserver::EEventStateChanged );
                            } 
                        }
                    else if ( iCursorPos.iX + ( iCursorPos.iY + 
                              iFirstVisibleRow ) * iMaxColumns  < 
                              iConsArray->Count() )
                        {
                        iOldCursorPos = iCursorPos;
                        iCursorPos.iY = newGridY;
                        iCursorPos.iX = newGridX;
                        DrawCursor();
                        }
                    else
                        {
                        iOldCursorPos.iX = 0;
                        iOldCursorPos.iY = 0;
                        }
				    }
                }
            else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                // The pointer has been set and dragged into the area of 
                // the grid.
                if ( yInPixels >= 0 && yInPixels < iGridItemHeight * 
                     iExtension->iMaxVisibleRows && 
                     iDragAlreadyActive && 
                     iOldCursorPos == iCursorPos &&
                     (iConsArray->Count() > (globalY * iMaxColumns + newGridX)))
                    {
                    if ( iExtension->iObserver )
                        {
                        iExtension->iObserver->HandleControlEventL( 
                                this, 
                                MCoeControlObserver::EEventStateChanged );
                        }
                	iDragAlreadyActive = EFalse;    
                    }
                }

            MTouchFeedback* feedback = MTouchFeedback::Instance();
        
            if ( feedback )
                {
                if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackBasic );
                    }
                else if ( aPointerEvent.iType == TPointerEvent::EDrag && iCursorPos != iOldCursorPos )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackSensitive );	
                    }
                }
            }
        else // For a non window owning scrollbar.
            {
            if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                iDragAlreadyActive = EFalse;
                }
            }
        
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationIconMap::SetObserver
// Sets the observer.
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::SetObserver( MCoeControlObserver* aObserver  )
    {
    if ( iExtension )
        {
        iExtension->iObserver = aObserver;
        }
    }

#endif //RD_SCALABLE_UI_V2    

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::InputCapabilities()
//
// -----------------------------------------------------------------------------
//
TCoeInputCapabilities CCmDestinationIconMap::InputCapabilities() const
    {
    return TCoeInputCapabilities( TCoeInputCapabilities::EAllText );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::SizeChanged()
// Control position of this control is registered for skin library when 
// necessary in CEikDialogPage::SetDataPosition, so we do not do that in this 
// method.
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::SizeChanged()
    {
    // Get the layout
    
    // Main pane without softkeys
    TRect mainPaneRect;
   
    if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                             mainPaneRect ) )
       {
       mainPaneRect = iAvkonAppUi->ClientRect();
       }
    
    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety = 
               AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
    
    TInt maxVariety = iconMapDialogVariety.LastVariety();
    
    // Check the CBA, if the orientation is not landscape 
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    // the offset for the certain cba location variety
    TInt maxVarietyOffset = 0;
    TInt varietyOffset = maxVariety + 1;
    
    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the 
    // variety
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
       {
       // the offset for one variety
       varietyOffset = ( maxVariety + 1 ) / KAknSctCBaButtonDirections;
       }
    
    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending 
    // order e.g the biggest sized layout first, the smallest last
    if ( location == AknLayoutUtils::EAknCbaLocationRight )
       {
       maxVarietyOffset = varietyOffset;
       }
    else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
       {
       maxVarietyOffset = varietyOffset + varietyOffset; // 2*
       }
    
    TInt varietyNumber = varietyOffset - iRows - 1; 
    
    // if more lines than possible to show, use the default 
    // ( the biggest grid ) variety
    if ( varietyNumber < 0 )
        {
        varietyNumber = 0;
        }
    // if zero rows, use the minimum
    else if ( iRows<=0 )
        {
        varietyNumber -= 1;
        }

    //add the varietyoffset
    varietyNumber += maxVarietyOffset;
    
    if ( Layout_Meta_Data::IsLandscapeOrientation() && 
         location == AknLayoutUtils::EAknCbaLocationRight )
       {
       varietyNumber = 10;
       }
    else
        {
        if ( iRows == 0 )
            {
            varietyNumber = 5;
            }
        else
            {
            varietyNumber = 3;
            }
        }

    TAknLayoutRect popupGridLayRect;
    popupGridLayRect.LayoutRect( mainPaneRect, 
          AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );
    
    // Calculate the size relatively
    TRect relativeDialog( TPoint( 0,0 ),popupGridLayRect.Rect().Size() );
    
    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect( relativeDialog, 
       AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
            
    // Then the grid area without scrollbar
    // NOTE: The grid with scroll bar is used as reference
    TAknLayoutRect gridLayRect;
    gridLayRect.LayoutRect( gridWithScrollLayRect.Rect(), 
       AknLayoutScalable_Avkon::grid_graphic_popup_pane( 0 ) );

    // Different parent if SCT inside editing menu.
    TRect contentRect = gridLayRect.Rect();

    // The x coordinate is 3 pixels to right and y coordinate 3 pixels up 
    // so substract from x coordinate and add to y coordinate
    if ( iIsMirrored ) 
        {
        iOffset = TPoint( 
            contentRect.iBr.iX - KHorizontalDialogMargin - iGridItemWidth + 1,  
            contentRect.iTl.iY - KVerticalDialogMargin + 1 );
        
        iGridTopLeft.iX = contentRect.iBr.iX - 
                          KHorizontalDialogMargin - 
                          ( iMaxColumns * iGridItemWidth );
        iGridTopLeft.iY = contentRect.iTl.iY - KVerticalDialogMargin;
        }
    else  // not mirrored
        {
        iOffset = TPoint( contentRect.iTl.iX - KHorizontalDialogMargin + 1, 
                         contentRect.iTl.iY - KVerticalDialogMargin + 1 );
        iGridTopLeft.iX = contentRect.iTl.iX - KHorizontalDialogMargin;
        iGridTopLeft.iY = contentRect.iTl.iY - KVerticalDialogMargin;
        }
    
       
    // The last, update background context
    if ( iExtension->iBgContext )
       {            
        TInt bgVariety = 0;
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            bgVariety = 1;
            }
            
        TAknLayoutRect innerRect;
        innerRect.LayoutRect( 
                relativeDialog, 
                AknLayoutScalable_Avkon::bg_popup_window_pane_g1( bgVariety ) );

       iExtension->iBgContext->SetFrameRects( relativeDialog, 
                                              innerRect.Rect() );
       }
    
    TRAPD( err,UpdateScrollIndicatorL() );
    if ( err )
        {
        return;
        }
    }


// -----------------------------------------------------------------------------
// CCmDestinationIconMap::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // save the old info for the magnitudes of the SCT grid
        TInt oldMaxColumns = iMaxColumns;

        // calculate the new magnitudes
        DoLayout();
        
        // then calculate the index position of the cursor in the icon table
        // and update the x and y positions for the new grid with it
        
        TInt oldCursorPosition = ( iFirstVisibleRow + iOldCursorPos.iY ) * 
                                 oldMaxColumns +
                                 iOldCursorPos.iX;
        
        TInt currentCursorPosition = ( iFirstVisibleRow + iCursorPos.iY ) * 
                                     oldMaxColumns +
                                     iCursorPos.iX;
        
        // the new first row is the top row on the page where the new focus is
        iFirstVisibleRow = iExtension->iMaxVisibleRows * 
                           ( currentCursorPosition / 
                           ( iMaxColumns * iExtension->iMaxVisibleRows ) );
        
        // the cursor positions are relative to current page
        iCursorPos.iY = ( currentCursorPosition - 
                        ( iMaxColumns * iFirstVisibleRow ) ) / 
                        iMaxColumns;
        iCursorPos.iX = currentCursorPosition - 
                        ( iMaxColumns * iFirstVisibleRow ) - 
                        ( iMaxColumns * iCursorPos.iY );
           
        iOldCursorPos.iY = ( oldCursorPosition - 
                           ( iMaxColumns * iFirstVisibleRow ) ) / 
                           iMaxColumns;
        iOldCursorPos.iX = oldCursorPosition - 
                           ( iMaxColumns * iFirstVisibleRow ) - 
                           ( iMaxColumns * iOldCursorPos.iY );
        }

    if ( aType == KAknsMessageSkinChange )
        {
        iOffscreenBgDrawn = EFalse;
        }
        
    CCoeControl::HandleResourceChange( aType );   
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::Draw()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::Draw( const TRect& /*aRect*/ ) const
    {    
    TInt cursorPos = 0;
    CWindowGc& gc=SystemGc();
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    
    TRect rect = Rect();
    
    // Main pane without softkeys
    TRect mainPaneRect;
    if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                             mainPaneRect ) )
       {
       mainPaneRect = iAvkonAppUi->ClientRect();
       }
    
    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety = 
       AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
       
    // The variety starts from 0 so add +1
    TInt maxVariety = iconMapDialogVariety.LastVariety();
    
    // Check the CBA, if the orientation is not landscape 
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    // the offset for the certain cba location variety
    TInt maxVarietyOffset = 0;
    TInt varietyOffset = maxVariety + 1;
    
    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the 
    // variety
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
       {
       // the offset for one variety
       varietyOffset = ( maxVariety + 1 )/KAknSctCBaButtonDirections;
       }
    
    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending 
    // order e.g the biggest sized layout first, the smallest last
    if ( location == AknLayoutUtils::EAknCbaLocationRight )
       {
       maxVarietyOffset = varietyOffset;
       }
    else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
       {
       maxVarietyOffset = varietyOffset + varietyOffset; // 2*
       }
    
    TInt varietyNumber = varietyOffset - iRows - 1; 
    
    // if more lines than possible to show, use the default 
    // ( the biggest grid ) variety
    if ( varietyNumber < 0 )
        {
        varietyNumber = 0;
        }
    // if zero rows, use the minimum
    else if ( iRows<=0 )
        {
        varietyNumber -= 1;
        }
    
    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if ( Layout_Meta_Data::IsLandscapeOrientation() && 
         location == AknLayoutUtils::EAknCbaLocationRight )
        {
        varietyNumber = 10;
        }
    else
        {
        varietyNumber = 3;
        }    
    // Layout the dialog size
    TAknLayoutRect dialogLayRect;
    dialogLayRect.LayoutRect( 
           mainPaneRect, 
          AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );
    
    // Get the missing height for the background
    TInt backgroundHeightOffset = dialogLayRect.Rect().Height() - rect.iBr.iY;
    
    rect.iBr.iY += backgroundHeightOffset;

    // Check if we got an offscreen bitmap allocated for skin background and
    // there is bitmap background in the current skin.
    if ( iOffscreenBg && iHasBitmapBackground )
        {
        DrawOffscreenBackgroundIfRequired();
        gc.BitBlt( rect.iTl, iOffscreenBg );
        }
    else
        {
        AknsDrawUtils::Background( skin, cc, this, gc, rect );
        }

    TInt numberOfIconsToBeDrawn = iConsArray->Count();
    numberOfIconsToBeDrawn -= ( iFirstVisibleRow * iMaxColumns );
    if ( numberOfIconsToBeDrawn > 0 )
        {
        if ( numberOfIconsToBeDrawn > 
            ( iExtension->iMaxVisibleRows * iMaxColumns ) )
            {
            numberOfIconsToBeDrawn = iExtension->iMaxVisibleRows * iMaxColumns;
            }

        gc.SetPenStyle( CGraphicsContext::ESolidPen );    
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        gc.SetPenSize( TSize( 1,1 ) );

        // 2 ) Draw the grid
        const TSize gridItemRectSize( iGridItemWidth + 1, iGridItemHeight + 1 );
             
        TInt numberOfGridCellsToBeDrawn = numberOfIconsToBeDrawn;

        TRgb colorLine = AKN_LAF_COLOR( 219 );
        AknsUtils::GetCachedColor( skin, 
                                   colorLine, 
                                   KAknsIIDQsnLineColors, 
                                   EAknsCIQsnLineColorsCG5 );
        TRgb colorRecentLine = AKN_LAF_COLOR( 215 );
        AknsUtils::GetCachedColor( skin, 
                                   colorRecentLine, 
                                   KAknsIIDQsnLineColors, 
                                   EAknsCIQsnLineColorsCG7 );

        // default pen color
        gc.SetPenColor( colorLine );
        
        TInt fullRows = numberOfGridCellsToBeDrawn / iMaxColumns;

        // how many left after the full rows
        numberOfGridCellsToBeDrawn -= fullRows * iMaxColumns;

        TPoint pos = iGridTopLeft;

        TInt endX = pos.iX + iGridItemWidth * iMaxColumns;
        TInt endY = pos.iY + iGridItemHeight * fullRows;

        TInt ii = 0;

        if ( fullRows )
            {
            // Draw full vertical lines
            for ( ii = 0 ; ii <= iMaxColumns ; ii++ )
                {
                gc.SetPenColor( colorLine );
                gc.SetPenSize( TSize( 1,1 ) );
                gc.DrawLine( pos, TPoint( pos.iX, endY ) );
                pos.iX += iGridItemWidth;
                }

            pos = iGridTopLeft;

            // Draw full horizontal lines
            for ( ii = 0 ; ii <= fullRows ; ii++ )
                {
                gc.SetPenSize( TSize( 1,1 ) );    
                gc.SetPenColor( colorLine );
                gc.DrawLine( pos, TPoint( endX, pos.iY ) );
                pos.iY += iGridItemHeight;
                }
            gc.SetPenColor( colorLine );
            gc.SetPenSize( TSize( 1,1 ) );    
            }

        if ( numberOfGridCellsToBeDrawn )
            {
            // Remaining cells in the last, non-full row
            pos = iOffset;
            // iOffset is cell area topLeft, grid is not included in it
            pos.iX--;
            pos.iY--;

            pos.iY += iGridItemHeight * fullRows;

            for ( ii = 0 ; ii < numberOfGridCellsToBeDrawn ; ii++ )
                {
                gc.DrawRect( TRect( pos, gridItemRectSize ) );

                if ( iIsMirrored ) 
                    pos.iX -= iGridItemWidth;
                else // not mirrored
                    pos.iX += iGridItemWidth; 
                }
            }
        
        TInt iconIndex = iFirstVisibleRow * iMaxColumns;
        TInt lCnt = iConsArray->Count(); 
        cursorPos = iCursorPos.iX + iCursorPos.iY * iMaxColumns;
        if ( lCnt > 0 )
            {
            TRect cellRect( TPoint( 0,0 ), 
                            TSize( iGridItemWidth - 1, iGridItemHeight - 1 ) );
            for ( TInt j = iconIndex, i = 0; 
                  j < lCnt && i < numberOfIconsToBeDrawn;
                  j++, i++ )
                {
                DrawItem( gc, i, j, cursorPos == i, EFalse );        
                }     
            }
            
        }
    iDrawnBefore = ETrue;
    gc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::DrawItem()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::DrawItem( 
    CWindowGc& aGc,
    TInt index,
    TInt aIconIndex,
    TBool aHighlighted,
    TBool aDrawBackground ) const
    {
    TRect sctPosition = CursorRect( index );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    
    TBool skins = AknsDrawUtils::Background( skin,cc,aGc, sctPosition );    
    TRgb color;
    if ( !skins )
        {
        aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        }
    
    if ( aHighlighted )
        {
        TRgb colorHightLightRect = AKN_LAF_COLOR( 215 );
        AknsUtils::GetCachedColor( skin, 
                                   colorHightLightRect, 
                                   KAknsIIDQsnLineColors, 
                                   EAknsCIQsnLineColorsCG7 );
        aGc.SetPenColor( colorHightLightRect );
        aGc.DrawRect( sctPosition );

        // Shrink by one pixel in all directions.
        TRect innerRect = sctPosition;
        innerRect.Shrink( 1,1 );

        color = AKN_LAF_COLOR( 210 );
        AknsUtils::GetCachedColor( skin, 
                                   color, 
                                   KAknsIIDQsnHighlightColors, 
                                   EAknsCIQsnHighlightColorsCG1 ); 
        aGc.SetBrushColor( color );       
        aGc.Clear( innerRect );
        }
    else if ( aDrawBackground )
        {
        TRect innerRect = sctPosition;
        aGc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
        if ( !skins )
            {
            aGc.Clear( innerRect );
            }
        else 
            {
            AknsDrawUtils::Background( skin, cc, this, aGc, innerRect );
            }
        }
    
    if ( iConsArray )
        {
        TInt lCnt =  iConsArray->Count(); 
        if ( lCnt > 0 && aIconIndex < lCnt && aIconIndex >= 0 )
            {
            TRect cellRect( TPoint( 0,0 ), 
                            TSize( iGridItemWidth - 1, iGridItemHeight - 1 ) );

            CGulIcon* bitmap = NULL;
            bitmap = iConsArray->At( aIconIndex );

            aGc.BitBltMasked( CursorPoint( index ),
                            bitmap->Bitmap(),
                            cellRect,
                            bitmap->Mask(),
                            EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::DrawCursor()
// Optimizes drawing. Only cursor is drawn.
// -----------------------------------------------------------------------------
void CCmDestinationIconMap::DrawCursor() const
    {
    // Whole Icon Map has to be drawn at least once. 
    // If the user presses arrow key before Icon Map has been drawn,
    // only cursor position is drawn without this check.
    if ( !iDrawnBefore )
        {
        DrawNow();
        }
    // Only redraw old and new cursor position cells
    ActivateGc();
    CWindowGc& gc = SystemGc();

    TInt cursorPos = iOldCursorPos.iX + iOldCursorPos.iY * iMaxColumns;   
    DrawCell( cursorPos, EFalse );

    cursorPos = iCursorPos.iX + iCursorPos.iY * iMaxColumns;
    DrawCell( cursorPos, ETrue );

    gc.DiscardFont();
    DeactivateGc();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::DrawCell
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::DrawCell( 
    TInt aCursorPos,
    TBool aHighlighted ) const
    {
    // calculate icon index
    TInt iconIndex = aCursorPos + iFirstVisibleRow * iMaxColumns;

    // If we are only redrawing for animations, no need to draw non-animated
    // items.
    TRect rect = CursorRect( aCursorPos );

    Window().Invalidate( rect );
    Window().BeginRedraw( rect );
    DrawItem( SystemGc(), aCursorPos, iconIndex, aHighlighted, ETrue );
    Window().EndRedraw();
    SystemGc().DiscardFont();
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CursorRect
// -----------------------------------------------------------------------------
//
TRect CCmDestinationIconMap::CursorRect( TInt aCursorPos ) const
    {
    TPoint pos = iOffset;

    if ( iIsMirrored )
        {
        pos.iX -= ( aCursorPos % iMaxColumns ) * iGridItemWidth;
        }
    else // Not mirrored
        {
        pos.iX += ( aCursorPos % iMaxColumns ) * iGridItemWidth;
        }

    pos.iY += ( aCursorPos / iMaxColumns ) * iGridItemHeight; 
    return TRect( pos, TSize( iGridItemWidth - 1, iGridItemHeight - 1 ) );
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CursorPoint
// -----------------------------------------------------------------------------
//
TPoint CCmDestinationIconMap::CursorPoint( TInt aCursorPos ) const
    {
    TPoint pos = iOffset;

    if ( iIsMirrored )
        {
        pos.iX -= ( aCursorPos % iMaxColumns ) * iGridItemWidth;
        }
    else // Not mirrored
        {
        pos.iX += ( aCursorPos % iMaxColumns ) * iGridItemWidth;
        }

    pos.iY += ( aCursorPos / iMaxColumns ) * iGridItemHeight; 
    return pos;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::MoveCursorL
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::MoveCursorL( TInt aDeltaX, TInt aDeltaY )
    {           
    if ( iIsMirrored )
        {
        aDeltaX = -aDeltaX;
        }
    
    if ( ( iConsArray->Count() < 8 ) )
        {
        return;
        }

    iOldCursorPos = iCursorPos;
    TInt oldFirstVisibleRow = iFirstVisibleRow;

    TInt globalYPos = iCursorPos.iY + iFirstVisibleRow;
    TInt lastColumnOnLastRow = ( ( iConsArray->Count() - 1 ) % iMaxColumns );

    TInt skipicon = aDeltaX != 0 ? 1: 0;

    if ( aDeltaX < 0 )
        {
        // Cursor was moved to left.
        if ( iCursorPos.iX > skipicon - 1 )
            {
            iCursorPos.iX -= skipicon;
            }
        else
            {
            if ( skipicon > iMaxColumns )
                {
                globalYPos--;
                iCursorPos.iX = iMaxColumns;
                }
            // Go to previous line
            globalYPos--;
            if ( globalYPos < 0 )
                {
                // Cursor was on the first line - go to last line.
                globalYPos = iRows - 1;
                // x - position to the last item on the last row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            else
                {
                // x - position to last column.
                iCursorPos.iX = iMaxColumns - skipicon;
                }
            }
        }

    if ( aDeltaX > 0 )
        {
        // Cursor was moved to right.
        if ( globalYPos < iRows - 1 )
            {
            // Not in the last row.
            if ( iCursorPos.iX < iMaxColumns - skipicon )
                {
                // If not on the last columns, move cursor to next column.
                iCursorPos.iX += skipicon;
                }
            else
                {
                // Cursor was on last column,
                // move to first column of the next line.
                iCursorPos.iX = 0;
                globalYPos++;
                }
            }
        else
            {
            // Currently on the last row.
            if ( iCursorPos.iX < lastColumnOnLastRow )
                {
                // If there are more items on this row, move cursor to next item.
                iCursorPos.iX++;
                }
            else
                {
                // No more item on the current row.
                // Move to first item on the first row.
                iCursorPos.iX = 0;
                globalYPos = 0;
                }
            }
        }

    if ( aDeltaY < 0 )
        {
        iCursorPos.iX -= skipicon;
        if ( iCursorPos.iX < 0 )
            {
            iCursorPos.iX += ( iMaxColumns - 1 );
            globalYPos--;
            }
        // Cursor was moved to up.
        if ( globalYPos > 0 )
            {
            // Cursor was not on the first line - move it to previous row.
            globalYPos--;
            }
        else
            {
            // Move cursot to last to row.
            globalYPos = iRows - 1;
            if ( iCursorPos.iX > lastColumnOnLastRow )
                {
                // No items in the current column on the last row - 
                // move cursor to last item on the row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            }
        }
    
    if ( aDeltaY > 0 )
        {
        iCursorPos.iX = ( iCursorPos.iX + skipicon ) % iMaxColumns;
        globalYPos += ( iCursorPos.iX + skipicon ) / iMaxColumns;
        // Cursor was moved to down.
        if ( globalYPos < iRows - 1 )
            {
            // Cursor is not on the last row. Move cursor to next row.
            globalYPos++;
            if ( globalYPos == iRows - 1 && 
                 iCursorPos.iX > lastColumnOnLastRow )
                {
                // No items in the current column on the last row - 
                // move cursor to last item on the row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            }
        else 
            {
            // Cursor was at the last row - move it to the first row.
            globalYPos = 0;
            }        
        }
    iCursorPos.iY = globalYPos - iFirstVisibleRow;

    if ( globalYPos < iFirstVisibleRow )
        {
        // Cursor was moved from the top row.
        if ( globalYPos <= 0 )
            {
            iFirstVisibleRow = 0;
            iCursorPos = TPoint( iCursorPos.iX, 0 );
            }
        else
            {
            // If cursor was moved up out of the visible area - show it again.
            iFirstVisibleRow -= iExtension->iMaxVisibleRows;
            iCursorPos = TPoint( iCursorPos.iX, 
                                 iExtension->iMaxVisibleRows - 1 );
            }
        }
    
    if ( globalYPos > iFirstVisibleRow + iExtension->iMaxVisibleRows - 1 )
        {
        if ( globalYPos == iRows - 1 )
            {
            // When cursor has moved from the top line,
            // it is adjusted to a page boundary.
            iCursorPos = TPoint( 
                iCursorPos.iX,
                ( iRows - 1 ) % iExtension->iMaxVisibleRows
                );
            iFirstVisibleRow = ( ( iRows - 1 ) / iExtension->iMaxVisibleRows ) * 
                               iExtension->iMaxVisibleRows;
            }
        else
            {
            // If cursor was moved down out of the visible area - show it again.
            iFirstVisibleRow += iExtension->iMaxVisibleRows;
            iCursorPos = TPoint( iCursorPos.iX, 0 );
            }
        }
       
    if ( iRows > iExtension->iMaxVisibleRows && 
        ( iOldCursorPos.iY + oldFirstVisibleRow != 
          iCursorPos.iY + iFirstVisibleRow ) )
        {
        UpdateScrollIndicatorL();
        }

    if ( oldFirstVisibleRow == iFirstVisibleRow )
        {
        // Draw only cursor if the view to the content was not scrolled.
        DrawCursor();
        }
    else
        {
        DrawNow();
        }

    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::UpdateScrollIndicatorL()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::UpdateScrollIndicatorL()
    {    
    if ( !iSBFrame )
        {
        return;
        }
    TEikScrollBarModel hSbarModel;
    TEikScrollBarModel vSbarModel;
    
    TEikScrollBarFrameLayout layout;
    
    // Main pane without softkeys
    TRect mainPaneRect;
    if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                             mainPaneRect ) )
       {
       mainPaneRect = iAvkonAppUi->ClientRect();
       }
        
    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety = 
       AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
    
    TInt maxVariety = iconMapDialogVariety.LastVariety();
    
    // Check the CBA, if the orientation is not landscape 
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    // the offset for the certain cba location variety
    TInt maxVarietyOffset = 0;
    TInt varietyOffset = maxVariety + 1;
    
    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the 
    // variety
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
       {
       // the offset for one variety
       varietyOffset = ( maxVariety + 1 )/KAknSctCBaButtonDirections;
       }
    
    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending 
    // order e.g the biggest sized layout first, the smallest last
    if ( location == AknLayoutUtils::EAknCbaLocationRight )
       {
       maxVarietyOffset = varietyOffset;
       }
    else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
       {
       maxVarietyOffset = varietyOffset + varietyOffset; // 2*
       }
    
    TInt varietyNumber = varietyOffset - iRows - 1; 
    
    // if more lines than possible to show, use the default 
    // ( the biggest grid ) variety
    if ( varietyNumber < 0 )
        {
        varietyNumber = 0;
        }
    // if zero rows, use the minimum
    else if ( iRows <= 0 )
        {
        varietyNumber -= 1;
        }
    
    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if ( Layout_Meta_Data::IsLandscapeOrientation() && 
         location == AknLayoutUtils::EAknCbaLocationRight )
       {
       varietyNumber = 10;
       }
    else
        {
        varietyNumber = 3;
        }     
    // Layout the dialog size
    TAknLayoutRect dialogLayRect;
    dialogLayRect.LayoutRect( mainPaneRect, 
       AknLayoutScalable_Avkon::popup_grid_graphic_window( varietyNumber ) );
    
    TRect dialogRect = dialogLayRect.Rect();
    
    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;
       
    gridWithScrollLayRect.LayoutRect( 
                    TRect( TPoint( 0,0 ),
                    TSize( dialogRect.Size() ) ), 
                    AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );

    // Calculate the relative rect for the grid
    TRect parent = gridWithScrollLayRect.Rect();
    
    TAknWindowComponentLayout scrollbarLayout = 
                                    AknLayoutScalable_Avkon::scroll_pane_cp5();
    
    vSbarModel.iScrollSpan = iRows;
    vSbarModel.iThumbSpan = iExtension->iMaxVisibleRows;
    
    if ( iSBFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {    
        // For EDoubleSpan type scrollbar
        vSbarModel.iThumbPosition = iFirstVisibleRow;
        TAknDoubleSpanScrollBarModel hDsSbarModel( hSbarModel );
        TAknDoubleSpanScrollBarModel vDsSbarModel( vSbarModel );
        
        // The y coordinate must be sifted 3 pixels up and x 3 to left
        parent.iTl.iY -= KVerticalDialogMargin;
        parent.iBr.iY -= KVerticalDialogMargin;        
        parent.iTl.iX -= KHorizontalDialogMargin;
        parent.iBr.iX -= KHorizontalDialogMargin;
        
        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
        iSBFrame->Tile( &vDsSbarModel ); 
        AknLayoutUtils::LayoutVerticalScrollBar( iSBFrame, 
                                                 parent, 
                                                 scrollbarLayout );       
        iSBFrame->SetVFocusPosToThumbPos( vDsSbarModel.FocusPosition() );
        }
    else
        {
        // For EArrowHead type scrollbar
        vSbarModel.iThumbPosition = iCursorPos.iY + iFirstVisibleRow;
        iSBFrame->TileL( &hSbarModel,&vSbarModel,parent,parent,layout );        
        iSBFrame->SetVFocusPosToThumbPos( vSbarModel.iThumbPosition );
        }
    }


// -----------------------------------------------------------------------------
// CCmDestinationIconMap::Reserved_1()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::Reserved_1()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::Reserved_2()
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::Reserved_2()
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::DrawOffscreenBackgroundIfRequired
// 
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::DrawOffscreenBackgroundIfRequired() const
    {
    if ( iOffscreenBg && iHasBitmapBackground )
        {
        if ( !iOffscreenBgDrawn )
            {
            TRect mainPaneRect;
            if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                                     mainPaneRect ) )
               {
               mainPaneRect = iAvkonAppUi->ClientRect();
               }
            
            // Dialog layout, check variety first
            TAknLayoutScalableParameterLimits iconMapDialogVariety = 
               AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
            
            TInt maxVariety = iconMapDialogVariety.LastVariety();
            
            // Check the CBA, if the orientation is not landscape 
            // there is not so much varieties
            AknLayoutUtils::TAknCbaLocation location = 
                                                  AknLayoutUtils::CbaLocation();
            // the offset for the certain cba location variety
            TInt maxVarietyOffset = 0;
            TInt varietyOffset = maxVariety + 1;
            
            // landscape variety number must be calculated offset == number of 
            // varieties same applies to the variety number for the biggest 
            // sized layout for the variety
            if ( Layout_Meta_Data::IsLandscapeOrientation() )
               {
               // the offset for one variety
               varietyOffset = ( maxVariety + 1 )/KAknSctCBaButtonDirections;
               }
            
            // for right and left cba buttons the max variety is not zero
            // the varities are ordered by the location of the cba and the 
            // descending order e.g the biggest sized layout first, the smallest last
            if ( location == AknLayoutUtils::EAknCbaLocationRight )
               {
               maxVarietyOffset = varietyOffset;
               }
            else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
               {
               maxVarietyOffset = varietyOffset + varietyOffset; // 2*
               }

            TInt varietyNumber = varietyOffset - iRows - 1; 
            
            // if more lines than possible to show, use the default 
            // ( the biggest grid ) variety
            if ( varietyNumber < 0 )
                {
                varietyNumber = 0;
                }
            // if zero rows, use the minimum
            else if ( iRows<=0 )
                {
                varietyNumber -= 1;
                }
            
            //add the varietyoffset
            varietyNumber += maxVarietyOffset;
                   
            TAknLayoutRect popupGridLayRect;
            popupGridLayRect.LayoutRect( mainPaneRect, 
               AknLayoutScalable_Avkon::popup_grid_graphic_window( 5 ) );
            
            TRect popupGridRect = popupGridLayRect.Rect();
            
            // set the top left height as the control starting point
            popupGridRect.iTl.iY = Rect().iTl.iY;
            
            //if ( popupGridRect.iBr.iY < mainPaneRect.iBr.iY )
            //    popupGridRect.iBr.iY = mainPaneRect.iBr.iY
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
            
            // draw to upper left corner, and normalize the retangle to 
            // fact that the dialog starts from coordinates ( 0,0 ), 
            // so the y-coordinate is correct ( heading pane ) 
            // but x must be set to zero
            TPoint point = TPoint( 0, 0 );
            popupGridRect.Move( -popupGridRect.iTl.iX,0 );
            
            AknsDrawUtils::DrawBackground( skin,
                                           cc,
                                           this,
                                           *iBitmapGc,
                                           point,
                                           popupGridRect,
                                           KAknsDrawParamDefault );

            iOffscreenBgDrawn = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::ComponentControl( TInt aIndex ) const
// Return the controll pointer
// -----------------------------------------------------------------------------
//
CCoeControl* CCmDestinationIconMap::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 && 
         iSBFrame && 
         iSBFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {    
        return iSBFrame->VerticalScrollBar();
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CountComponentControls()
// Return no of controll to be placed on the container control
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationIconMap::CountComponentControls() const
    {
    if ( iSBFrame && 
         iSBFrame->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {    
        return 1;
        }
    else
        {
        return 0;
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::CountMaxColumnsAndCellSizes
// Counts no of columns and the cell size will be displayed in the icon table
// -----------------------------------------------------------------------------
//
void CCmDestinationIconMap::CountMaxColumnsAndCellSizes()
    {    
    TRect cellRect; // retangle of one item in grid
    TRect gridRect; // retangle of the grid contaning the items
    
    // 1. Get the layout 
    
    // Get the parent rect
    TRect mainPaneRect;
    if ( !AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                             mainPaneRect ) )
       {
       mainPaneRect = iAvkonAppUi->ClientRect();
       }
    
    // Calculate the layout of the whole popup with the biggest possible -> 0
    // Dialog layout, check variety first
    
    // Get the layout rect of the dialog
    
    // Check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety = 
        AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();
    
    TInt maxVariety = iconMapDialogVariety.LastVariety();
    
    // Check the CBA, if the orientation is not landscape 
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    // the offset for the certain cba location variety
    TInt maxVarietyOffset = 0;
    TInt varietyOffset = maxVariety + 1; // the number of varieties
        
    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the 
    // variety
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
       {
       // the offset for one variety
       varietyOffset = ( maxVariety + 1 ) / KAknSctCBaButtonDirections;
       }
       
    if ( location == AknLayoutUtils::EAknCbaLocationRight )
       {
       maxVarietyOffset = varietyOffset;
       }
    else if ( location == AknLayoutUtils::EAknCbaLocationLeft )
       {
       maxVarietyOffset = varietyOffset + varietyOffset; // 2*
       }
    
    TAknLayoutRect popupGridLayRect;
    popupGridLayRect.LayoutRect( mainPaneRect,
       AknLayoutScalable_Avkon::popup_grid_graphic_window( maxVarietyOffset ) );
    
    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect( popupGridLayRect.Rect(), 
       AknLayoutScalable_Avkon::listscroll_popup_graphic_pane() );
    
    // Then the grid area without scrollbar
    // NOTE: The grid with scroll bar is used as reference
    TAknLayoutRect gridLayRect;
    gridLayRect.LayoutRect( gridWithScrollLayRect.Rect(), 
       AknLayoutScalable_Avkon::grid_graphic_popup_pane( 0 ) );

    // Different parent if SCT inside editing menu.
    TRect rect = Rect();
    gridRect = gridLayRect.Rect();
    // cell size, AGAIN 7 item
    TAknLayoutRect cellLayRect;
    cellLayRect.LayoutRect( 
                    gridRect, 
                    AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0,0,0 ) );

    cellRect = cellLayRect.Rect();
        
    // 2. Calculate width related
    // - item width
    // - max number of columns
    
    // Width of the items area
    TInt gridWidth = gridRect.Width();
    
    // Width of one item
    TInt cellWidth = cellRect.Width();
  
     // ensure the item width and store it
    TAknLayoutRect secondCellLayRect;
    secondCellLayRect.LayoutRect( gridRect, 
                    AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0,1,0 ) );
       
    iGridItemWidth = secondCellLayRect.Rect().iTl.iX - 
                     cellLayRect.Rect().iTl.iX;
    
    if ( iGridItemWidth < 0 )
        {
        // The result is negative with lay file data if the layout is mirrored.
        iGridItemWidth = -iGridItemWidth;
        }
    
    // Calculate the amount of columns
    iMaxColumns = gridWidth / cellWidth;
    
    
    // 2. Calculate height related
    // - item height
    // - max number of rows
        
    // Get the grid height
    // NOTE: in landscape the LAF is wrong so use the main panes height
    TInt gridHeight = gridRect.Height();
    
    // get the item height
    TInt cellHeight = cellRect.Height();
    
    // calculate the number of items fitting to grid
    iExtension->iMaxVisibleRows = 3; //gridHeight / cellHeight;
    
    // Store the item height
    TAknLayoutRect secondRowLayRect;
    secondRowLayRect.LayoutRect( gridLayRect.Rect(), 
       AknLayoutScalable_Avkon::cell_graphic_popup_pane( 0,0,1 ) );

    iGridItemHeight = secondRowLayRect.Rect().iTl.iY - 
                      cellLayRect.Rect().iTl.iY;
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::HandleScrollEventL
// Handles the different scroll events so that the map reacts accordingly.
// -----------------------------------------------------------------------------
// 
void CCmDestinationIconMap::HandleScrollEventL( CEikScrollBar* aScrollBar, 
                                                TEikScrollEvent aEventType )
    {
    TBool update = EFalse;
    
    switch ( aEventType )
        {       
        case EEikScrollUp:
        case EEikScrollPageUp:
            {           
            // nothing done if we are already on the first page.
            if ( iFirstVisibleRow > 0 )
                {
                iFirstVisibleRow -= iExtension->iMaxVisibleRows;
                if ( iFirstVisibleRow < 0 )
                    {
                    iFirstVisibleRow = 0;
                    }
                update = ETrue;
                }
            UpdateScrollIndicatorL();
            }
            break;
        
        case EEikScrollDown:
        case EEikScrollPageDown:
            {           
            // nothing done if we are already on the last page.
            if ( iFirstVisibleRow < iRows - iExtension->iMaxVisibleRows ) 
                {
                iFirstVisibleRow += iExtension->iMaxVisibleRows;
                if ( iFirstVisibleRow > iRows - iExtension->iMaxVisibleRows )
                    {   
                    iFirstVisibleRow = iRows - iExtension->iMaxVisibleRows;
                    }
                update = ETrue;
                }
            UpdateScrollIndicatorL();
            }
            break;   
               
        case EEikScrollThumbDragVert:
            {        	
            // Ask which type of scrollbar is shown
            TInt thumbPosition;
            TBool isDoubleSpan = ( CEikScrollBarFrame::EDoubleSpan == iSBFrame->TypeOfVScrollBar() );
            if ( isDoubleSpan )
                {
                thumbPosition = static_cast <const TAknDoubleSpanScrollBarModel*>( 
                                            aScrollBar->Model() )->FocusPosition();
                }
            else
                {
                thumbPosition = aScrollBar->Model()->iThumbPosition;
                }
            
            if ( thumbPosition != iFirstVisibleRow )
                {
                iFirstVisibleRow = thumbPosition;
                update = ETrue;
                }
            }
            break;
        
        case EEikScrollThumbReleaseVert:
            {
            // Ask which type of scrollbar is shown
            TBool isDoubleSpan = 
                CEikScrollBarFrame::EDoubleSpan == iSBFrame->TypeOfVScrollBar();
            if ( isDoubleSpan )
                {
                iFirstVisibleRow = static_cast <const TAknDoubleSpanScrollBarModel*>( 
                                            aScrollBar->Model() )->FocusPosition();
                }
            else
                {
                iFirstVisibleRow = aScrollBar->Model()->iThumbPosition;
                }
            update = ETrue;
            UpdateScrollIndicatorL();
            }
            break;
        
        case EEikScrollLeft: // flow through            
        case EEikScrollRight: // flow through
        case EEikScrollPageLeft: // flow through
        case EEikScrollPageRight: // flow through
        case EEikScrollThumbDragHoriz: // flow through
        case EEikScrollThumbReleaseHoriz: // flow through
        default: 
            // do nothing
            break; 
        }
                
    // to avoid flicker we draw only if there really was something new to draw.
    if ( update )
        {              
        if ( iExtension->iObserver )
            {
            iExtension->iObserver->HandleControlEventL( 
                                      this, 
                                      MCoeControlObserver::EEventRequestFocus );
            } 
        DrawDeferred();    
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationIconMap::Extension
// -----------------------------------------------------------------------------
//
CCmDestinationIconMapExtension* CCmDestinationIconMap::Extension() const
    {
    return iExtension;
    }    
