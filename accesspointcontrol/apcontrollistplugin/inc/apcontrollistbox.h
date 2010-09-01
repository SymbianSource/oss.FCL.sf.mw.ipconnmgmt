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
* Description:  Declaration of class CApControlListbox.
*
*/


#ifndef APSELECTOR_LISTBOX_H
#define APSELECTOR_LISTBOX_H

// INCLUDE FILES
#include <aknlists.h>
class CAPControlListPluginContainer;


// CLASS DECLARATION

/**
* Listbox to display the list of access points.
* 
*/
NONSHARABLE_CLASS(CApControlListbox): public CAknSingleStyleListBox
    {
    public:     // Construct / destruct

        /**
        * Destructor.
        */
        virtual ~CApControlListbox();


        /**
        * Constructor.
        */
        CApControlListbox();

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
        * From CCoeControl. Handles a change to the control's resources.
        * The types of resources handled are those which are shared across 
        * the environment, e.g. colours or fonts.
        * Called if Skinning is changed.
        * @param aType A message UID value.
        */
        virtual void HandleResourceChange(TInt aType); 

        /**
        * Set EmptyText in the listbox
        */
        void SetListEmptyTextL();
    };


#endif

// End of file
