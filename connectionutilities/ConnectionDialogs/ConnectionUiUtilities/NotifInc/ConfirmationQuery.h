/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CConfirmationQuery.
*
*/


#ifndef __CONFIRMATION_QUERY_H__
#define __CONFIRMATION_QUERY_H__

// INCLUDES
#include <aknmessagequerydialog.h>
#include <ConnectionUiUtilities.h>
#include "ExpiryTimerCallback.h"
// FORWARD DECLARATIONS
class CConfirmationQueryNotif;
class CExpiryTimer;

// CLASS DECLARATIONS

/**
 * Class implements a query dialog.
 */
NONSHARABLE_CLASS( CConfirmationQuery ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    public:
        /**
        * Constructor the CConfirmationQuery class
        * @param aNotif notifier pointer
        * @param aNameOfConnMethod 
        * @param aUIId    
        * @return -
        */
        CConfirmationQuery( CConfirmationQueryNotif* aNotif );

        /**
        * Exit function the CConfirmationQuery
        * @param aButtonId 
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
    
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();
        
        /**
        * Destructor
        */
        virtual ~CConfirmationQuery();     

        /**
        * Tries to exit the dialog when the specified button is pressed, if this 
        * button should exit the dialog.
        *
        * See @c OkToExitL() to determine which buttons can exit the dialog.
        * 
        * This will fail if user exit is prevented by the 
        * @c EEikDialogFlagNoUserExit flag. If the @c EEikDialogFlagNotifyEsc flag
        * is not set and the dialog has been cancelled it immediately deletes 
        * itself.
        * 
        * @param aButtonId The id of the pressed button.
        */
        void TryExitL( TInt aButtonId );
        
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);
        
        /**
        * Sets the available choices for the query into the class variable
        * @param    aChoices RArray<TInt>
        */
        void SetChoices( RArray<TMsgQueryLinkedResults> aChoices );

    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();    

    private:
        CConfirmationQueryNotif* iNotif;  // Pointer to the Notifier, not owned
        TBool iButtonGroupPreviouslyChanged;    // Was ButtonGroup modified?
        
        RArray<TMsgQueryLinkedResults> iChoiceIds;
        // For base class, unused.
        TInt iDummy;
		
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };


#endif  // __CONFIRMATION_QUERY_H__

// End of File
