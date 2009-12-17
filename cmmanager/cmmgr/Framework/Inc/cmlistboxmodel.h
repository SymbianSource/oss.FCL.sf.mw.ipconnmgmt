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
* Description:  Declaration of class CCmListboxModel.
*
*/

#ifndef CMLISTBOX_LISTBOX_MODEL_H
#define CMLISTBOX_LISTBOX_MODEL_H

// INCLUDE FILES
#include <eikdialg.h>
#include "cmlistitemlist.h"
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif

// CONSTANTS
// Granularity of the model array.
const TInt KCMGranularity = 8;

//"1\tFirstLabel\tSecondLabel\t0\t0" /**/
_LIT( KListItemFormatCmProt, "%d\t%S\t%S\t%d" );
//"0\tFirstLabel\tSecondLabel" /*\t%d*/
_LIT( KListItemFormatCm, "%d\t%S\t%S" );
//"0\tFirstLabel" /*\t%d*/
_LIT( KListItemFormatEmbDest, "%d\t%S\t" );
//"0\tFirstLabel\t0\t0" /*\t%d*/
_LIT( KListItemFormatEmbDestProt, "%d\t%S\t\t%d" );

const TInt KCMMaxFormatSize = sizeof( KListItemFormatCm );
const TInt KCMMaxListItemNameLength = CommsDat::KMaxTextLength;
const TInt KMaxCmListboxName = KCMMaxListItemNameLength+KCMMaxFormatSize;


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


// CLASS DECLARATION

/**
 * Model for the connection methods listbox.
 * pointers; items are owned.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CCmListboxModel ) : public CCmListItemList
                                     , public MDesCArray
    {
    public:     // Construct

        /**
        * Constructor.
        */
        CCmListboxModel();

        /**
        * CCmListboxructor.
        */
        virtual ~CCmListboxModel();

    public:     // from MDesCArray

        /**
        * Get number of items in the model.
        * @return number of items.
        */
        TInt MdcaCount() const;

        /**
        * Text to be displayed for this item.
        * @param aIndex Index of item.
        * @return for this item text (text owned by the model).
        */
        TPtrC MdcaPoint( TInt aIndex ) const;

    public:        
        
        /**
        * Sets the icon offset
        * @param aOffset the offset of our icons in the iconarray
        */
        void SetOwnIconOffset( TInt aOffset );

    protected:    // data
    
        /**
        * Formats the text of the listbox, calls the leaving version trapped.
        * @param aIndex Index of item.
        * @param aBuf Buffer to store formatted text.
        */
        void FormatListboxText( TInt aIndex, TDes& aBuf ) const;

        /**
        * Formats the text of the listbox, leaving version.
        * @param aIndex Index of item.
        * @param aBuf Buffer to store formatted text.
        */
        void FormatListboxTextL( TInt aIndex, TDes& aBuf ) const;

        /**
        * MdcaPoint() cannot leave, so this buffer is allocated
        * to allow formatting. Fortunately, doesn't have to be very
        * large.
        * We have to use MUTABLE CAST, as MdcaPoint is const (odd enough),
        * so wouldn't allow formatting the text in a member...
        */
        __MUTABLE TBuf<KMaxCmListboxName> iBuf;
        TInt iOffset;
    };

#endif // CMLISTBOX_LISTBOX_MODEL_H
