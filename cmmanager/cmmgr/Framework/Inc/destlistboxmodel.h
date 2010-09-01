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
* Description:  Declaration of class CDestListboxModel.
*
*/

#ifndef DESTLISTBOX_LISTBOX_MODEL_H
#define DESTLISTBOX_LISTBOX_MODEL_H

// INCLUDE FILES
#include <eikdialg.h>
#include "destlistitemlist.h"
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif

// CONSTANTS
// Granularity of the model array.
const TInt KGranularity = 8;
_LIT( KListItemFormatDestProt, "%d\t%S\t%S\t%d" );
_LIT( KListItemFormatDest, "%d\t%S\t%S" );
const TInt KMaxFormatSize = sizeof( KListItemFormatDest );
const TInt KMaxListItemNameLength = CommsDat::KMaxTextLength;
const TInt KMaxDestListboxName = KMaxListItemNameLength+KMaxFormatSize;

/**
 *  Model for the destinations listbox.
 *  pointers; items are owned.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CDestListboxModel ) : public CDestListItemList
                                       , public MDesCArray
    {
    public:     // Construct / Destruct

        /**
        * Constructor.
        * @param aGraphicType   The used graphic type
        */
        CDestListboxModel();

        /**
        * Destructor.
        */
        virtual ~CDestListboxModel();

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
        * Sets the icon offset
        * @param aOffset    The offset of our icons in the iconarray
        */
        void SetOwnIconOffset( TInt aOffset );

    private:    // data
    
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
        void FormatListboxTextL( TInt aIndex,
                                                TDes& aBuf ) const;
        /**
        * MdcaPoint() cannot leave, so this buffer is allocated
        * to allow formatting. Fortunately, doesn't have to be very
        * large.
        * We have to use MUTABLE CAST, as MdcaPoint is const (odd enough),
        * so wouldn't allow formatting the text in a member...
        */
        __MUTABLE TBuf<KMaxDestListboxName> iBuf;
        TInt iOffset;
    };

#endif
