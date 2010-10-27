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
* Description:  Declaration of class CApNetSelectorListboxModel.
*
*/


#ifndef APNETSELECTOR_LISTBOX_MODEL_H
#define APNETSELECTOR_LISTBOX_MODEL_H

// INCLUDE FILES
#include <eikdialg.h>
#include <ApNetworkItemList.h>


// CONSTANTS
// Granularity of the model array.
const TInt KGranularity = 8;


_LIT( KListItemFormatGraphicOnOff, "%d\t%*S" );

const TInt KMaxFormatSize = sizeof( KListItemFormatGraphicOnOff );
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
* Model for the network group selection listbox. 
* It is an array of CApNetworkItem pointers; 
* items are owned.
*/
NONSHARABLE_CLASS( CApNetSelectorListboxModel ) :
        public CApNetworkItemList,
        public MDesCArray
    {
    public:     // Construct / destruct

        /**
        * Constructor.
        */
        CApNetSelectorListboxModel( );

        /**
        * Destructor.
        */
        virtual ~CApNetSelectorListboxModel();

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
        * @param aUid UID
        * @param aErr Error code 
        * @return The index for the passed UID 
        */
        TInt Item4Uid( TUint32 aUid, TInt& aErr ) const;

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

    };


#endif

// End of file
