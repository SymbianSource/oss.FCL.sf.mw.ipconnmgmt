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
* Description:  Declaration of class CApSelectorListbox.
*
*/


#ifndef APSELECTOR_LISTBOX_H
#define APSELECTOR_LISTBOX_H

// INCLUDE FILES
#include <aknlists.h>


// FORWARD DECLARATION
class CApSelectorListboxModel;


// CLASS DECLARATION

/**
* Model for the access point listbox. It is an array of CApSelectorItem
* pointers; items are owned.
*/
NONSHARABLE_CLASS( CApSelectorListbox ) : public CAknSingleGraphicStyleListBox
    {
    public:     // Construct / destruct


        /**
        * Two-phased constructor. Leaves on failure.
        * @param aParent Parent control.
        * @return The constructed listbox.
        */
        static CApSelectorListbox* NewL
            ( const CCoeControl* aParent );


        /**
        * Destructor.
        */
        virtual ~CApSelectorListbox();


        /**
        * Constructor.
        */
        CApSelectorListbox();

    public :
        /**
        * Handle key event.
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return Response (was the key event consumed?).
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handle focus change.
        * @param aDrawNow Draw now?
        */
        void FocusChanged( TDrawNow aDrawNow );


        /**
        * Loads the icons
        */
        void LoadIconsL();


        /**
        * Gets the UID from a given item
        * @param    The index of the item
        * @return   The UID of the item given by itemindex
        */
        TUint32 Uid4Item( TInt aItem ) const;


        /**
        * Gets the UID of the current item
        * @return  The UID of the current item
        */
        TUint32 CurrentItemUid() const;


        /**
        * Gets the name of the current item
        * @return The name of the current item
        */
        const TDesC& CurrentItemNameL();
        

        // from CCoeControl
        void HandleResourceChange(TInt aType);

    };


#endif

// End of file
