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
* Description:  Declaration of class CApSelectorListboxModel.
*
*/


#ifndef APSELECTOR_LISTBOX_MODEL_H
#define APSELECTOR_LISTBOX_MODEL_H

// INCLUDE FILES
#include <eikdialg.h>
#include <ApListItemList.h>


// CONSTANTS
// Granularity of the model array.
const TInt KGranularity = 8;

// const TInt KErrInvalidBitmapType = -300;

_LIT( KListItemFormatGraphicBearer, "%d\t%*S\t\t" );
_LIT( KListItemFormatGraphicNone, "%*S" );
_LIT( KListItemFormatGraphicOnOff, "%d\t%*S" );

_LIT( KListItemFormatGraphicProtected, "%d\t%*S\t%d\t" );
_LIT( KListItemFormatGraphicOnOffWithLock, "%d\t%*S\t\t%d\t" );
_LIT( KListItemFormatGraphicWithLock, "%*S\t\t%d\t" );

const TInt KMaxFormatSize = sizeof( KListItemFormatGraphicBearer );
const TInt KMaxListItemNameLength = 50; // = KCommsDbSvrMaxFieldLength

/**
* Buffer size for formatting listbox text. Maximum item length
* plus listbox internals (tabulators for icons etc.) must fit into it.
*
* Important note:
* The model of the listbox is a CAknFilteredTextListBoxModel.
* The *itemarray* of that model is this class. That is, whatever is in
* the model (this class), it may not be visible, if it is filtered out.
* We always must use the listbox's Model() to get visible ones.
*/
const TInt KMaxApSelectorListboxName = KMaxListItemNameLength+KMaxFormatSize;


// CLASS DECLARATION

/**
* Model for the access point listbox. It is an array of CApSelectorItem
* pointers; items are owned.
*/
NONSHARABLE_CLASS( CApSelectorListboxModel ) :
        public CApListItemList,
        public MDesCArray
    {
    public:     // Construct / destruct
        enum TGraphicType       /// Events reported to the database observer.
            {
            EGraphicNone,       ///< No graphic or icon.
            EGraphicProtection, ///< Only protected icon.
            EGraphicBearer,     ///< Bearer specific icon.
            EGraphicOnOff       ///< Selection specific icon.
            };


        /**
        * Constructor.
        * @param aGraphicType   The used graphic type
        */
        CApSelectorListboxModel( TGraphicType aGraphicType = EGraphicBearer );


        /**
        * Destructor.
        */
        virtual ~CApSelectorListboxModel();

    public:     // from MDesCArray

        /**
        * Get number of items in the model.
        * @return Number of items.
        */
        TInt MdcaCount() const;


        /**
        * Text to be displayed for this item.
        * @param aIndex Index of item.
        * @return TPtrC for this item text (text owned by the model).
        */
        TPtrC MdcaPoint( TInt aIndex ) const;


    public:
        
        /**
        * index <--> Uid conversion
        * @param aUid   The UID of the item 
        * @param aErr   Error code
        * @return       The index of the item 
        */
        TInt Item4Uid( TUint32 aUid, TInt& aErr ) const;

        
        /**
        * Sets the icon offset
        * @param aOffset    The offset of our icons in the iconarray
        */
        void SetOwnIconOffset( TInt aOffset );


    private:    // data
        /**
        * Formats the text of the listbox.
        * @param aIndex Index of item.
        * @param aBuf Buffer to store formatted text.
        */
        void FormatListboxText( TInt aIndex, TDes& aBuf ) const;


        /**
        * MdcaPoint() cannot leave, so this buffer is allocated
        * to allow formatting. Fortunately, doesn't have to be very
        * large.
        * We have to use MUTABLE CAST, as MdcaPoint is const (odd enough),
        * so wouldn't allow formatting the text in a member...
        */
        __MUTABLE TBuf<KMaxApSelectorListboxName> iBuf;


        TGraphicType    iGraphicType;
        TInt            iOffset;
        TBool           iFeatureInitialised;
        TBool           iProtectionSupported;
    };


#endif

// End of file
