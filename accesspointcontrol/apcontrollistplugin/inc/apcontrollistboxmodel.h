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
* Description:  Declaration of class CApControlListboxModel.
*
*/


#ifndef APCONTROL_LISTBOX_MODEL_H
#define APCONTROL_LISTBOX_MODEL_H

// INCLUDES
#include <aknlists.h>

// CLASS DECLARATION

/**
* Data model for the access point listbox. It is an array of CApListItem
* pointers; items are owned.
*/
NONSHARABLE_CLASS( CApControlListboxModel ):
        public CTextListBoxModel,
        public MDesCArray
    {
    public:     // Construct / destruct

        /**
        * Constructor.
        * @param aGraphicType Graphic type
        */
        CApControlListboxModel( );

        /**
        * Destructor.
        */
        virtual ~CApControlListboxModel();

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

    };


#endif

// End of file
