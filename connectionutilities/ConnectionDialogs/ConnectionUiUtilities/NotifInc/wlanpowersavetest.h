/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Dialog that implements WLAN Power Save Testing notification
*
*/


#ifndef __POWERSAVETEST_QUERY_H__
#define __POWERSAVETEST_QUERY_H__

// INCLUDES
#include <aknmessagequerydialog.h>

// FORWARD DECLARATIONS
class CWlanPowerSaveQueryNotif;


// CLASS DECLARATIONS

/**
 * Class implements a query dialog.
 */
NONSHARABLE_CLASS( CWlanPowerSaveQuery ) : public CAknMessageQueryDialog 
    {
    public:
        /**
        * Constructor the CWlanPowerSaveQuery class
        * @param aNotif notifier pointer  
        * @return -
        */
        CWlanPowerSaveQuery( CWlanPowerSaveQueryNotif* aNotif );

        /**
        * Exit function the CWlanPowerSaveQuery
        * @param aButtonId 
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
    
        /**
        * Destructor
        */
        virtual ~CWlanPowerSaveQuery();     

        /** 
        * From @c CCoeControl.
        *
        * Handles key events.
        * 
        * If a control wishes to process key events, it should implement this
        * function. The implementation must ensure that the function returns 
        * @c EKeyWasNotConsumed if it does not do anything in response to a 
        * key event, otherwise, other controls or dialogs may be prevented 
        * from receiving the key event. If it is able to process the event it 
        * should return @c EKeyWasConsumed.
        * 
        * @param aKeyEvent The key event. 
        * @param aType The type of key event: @c EEventKey, @c EEventKeyUp or 
        *        @c EEventKeyDown. 
        * @return Indicates whether or not the key event was used
        *         by this control. 
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode );

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

    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();    

    private:
        CWlanPowerSaveQueryNotif* iNotif;      // Pointer to the Notifier, not owned
        TBool iButtonGroupPreviouslyChanged;   // Was ButtonGroup modified?
    };


#endif  // __POWERSAVETEST_QUERY_H__

// End of File
