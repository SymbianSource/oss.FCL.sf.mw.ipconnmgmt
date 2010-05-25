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
* Description:  Destination icon map
*
*/

#ifndef CMDESTICONMAP_H
#define CMDESTICONMAP_H

#include <coecntrl.h>
#include <AknUtils.h>
#include <gulicon.h>
#include <AknIconArray.h>


class CCmDestinationIconMapExtension;
class CEikScrollBarFrame;
class CCmDestinationIconMapExtension;


/**
 * Change Icon Dialog table Destination UI.
 *
 */
NONSHARABLE_CLASS(CCmDestinationIconMap) : public CCoeControl
                                         , public MEikScrollBarObserver
    {
    public: 
        
        /** 
        * CCmDestinationIconMap two phase constructor
        */
        static CCmDestinationIconMap* NewL();    
        
        /** 
        * CCmDestinationIconMap destructor
        */
         ~CCmDestinationIconMap();
        
        /** 
        * CCmDestinationIconMap resource constructor.
        * @param aReader Icon Dialog table Resource
        */
         void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * Number of rows in the icon table.
        * @return Number of rows in icon table.
        */
         TInt HeightInRows();
        
        /**
        * Create the scrollbar Ptr and set the No of Rows in the 
        * Icon table depends upon the Icon array count.
        */
         void CreateScrollBarAndIconRowL();
        
        /**
        * Sets reference to the index where selected icon index is put.
        * @param aIconIndex Reference to the integer where selected icon 
        * index is put.
        */
         void SetIndex(TInt& aIconIndex);

    private:
    
        /** 
        * CCmDestinationIconMap constructor
        */
         CCmDestinationIconMap();

        /** 
        * CCmDestinationIconMap two phase constructor
        */
        void ConstructL();
        
    public: // from CCoeControl
    
         TSize MinimumSize();
         TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode);
         TCoeInputCapabilities InputCapabilities() const;
         virtual void ActivateL();

#ifdef RD_SCALABLE_UI_V2

 	    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
 	    void SetObserver( MCoeControlObserver* aObserver  );
 	    
#endif //RD_SCALABLE_UI_V2

    protected: // from CCoeControl
    
         void SizeChanged();
         void HandleResourceChange(TInt aType);
         TInt CountComponentControls() const;
         CCoeControl* ComponentControl(TInt aIndex) const;

    public: 

        /**
        * DrawOffscreenBackgroundIfRequired().
        *
        * @Description:Draw the offscreen bitmap if applicable.
        *              Specially in case of Skin change
        */
        void DrawOffscreenBackgroundIfRequired() const;

        /**
        * DoLayout()
        * @Description: Sets the Layout of the Icon table.
        */
        void DoLayout();

    private:
    
        /**
        * Draw().
        *
        * @Description:
        */
        void Draw(const TRect& aRect) const;

        /**
        * DrawItem().
        *
        * @Description:
        */
        void DrawItem( CWindowGc& aGc,
                       TInt index,
                       TInt aIconIndex,
                       TBool aHighlighted,
                       TBool aDrawBackground ) const;

        /**
        * DrawCursor().
        *
        * @Description:
        */
        void DrawCursor() const;
        
        /**
        * MoveCursorL().
        *
        * @Description:
        */
        void MoveCursorL( TInt aX, TInt aY );

        /**
        * DrawCell().
        *
        * @Description:
        */
        void DrawCell( TInt aCursorPos, TBool aHighLighted ) const;
        
        /**
        * CursorRect().
        *
        * @Description:
        */
        TRect CursorRect( TInt aCursorPos ) const;
        
        /**
        * CursorPoint().
        *
        * @Description:
        */
        TPoint CursorPoint( TInt aCursorPos ) const;

        /**
        * UpdateScrollIndicatorL().
        *
        * @Description:
        */
        void UpdateScrollIndicatorL();

        /**
        * CountMaxColumnsAndCellSizes().
        *
        * @Description:
        */
        void CountMaxColumnsAndCellSizes();

        /**
        * LoadIconL().
        * This fucton leaves if the client does not have a valid UI context
        *
        * @Description:
        */
        void LoadIconL();

        /**
        * Extension().
        *
        * @Description:
        */
        CCmDestinationIconMapExtension* Extension() const;        
     
    private: //from CCoeContrl

        void Reserved_1();
        void Reserved_2();
        
    public:

        void HandleScrollEventL( CEikScrollBar* aScrollBar, 
                                 TEikScrollEvent aEventType);

    private:
        TInt                iGridItemWidth;
        TInt                iGridItemHeight;
        TBool               iIsMirrored;
        TInt                iRows;
        TInt                iFirstVisibleRow;
        TPoint              iOffset;
        TPoint              iGridTopLeft;

        TPoint              iCursorPos;
        TPoint              iOldCursorPos;
        mutable TBool       iDrawnBefore;

        CEikScrollBarFrame* iSBFrame;
        TInt*               iIconIndex;

        TBool               iAnimated;

        CFbsBitmap*         iOffscreenBg;
        CFbsBitmapDevice*   iBitmapDevice;
        CFbsBitGc*          iBitmapGc;
        TBool               iHasBitmapBackground;
        mutable TBool       iOffscreenBgDrawn;

        TInt                iMaxColumns;
        CCmDestinationIconMapExtension*   iExtension;
        CArrayPtr<CGulIcon>*  iConsArray;
        TBool               iDragAlreadyActive;
    };

#endif // __CMDESTICONMAP_H
