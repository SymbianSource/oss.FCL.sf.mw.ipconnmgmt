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
* Description:  Declaration of class CAuthenticationDialog.
*
*/


#ifndef __AUTHENTICATIONDIALOG_H__
#define __AUTHENTICATIONDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <AknQueryDialog.h>

#include "ExpiryTimerCallback.h"

// FORWARD DECLARATION
class CConnDlgAuthenticationPlugin;
class CExpiryTimer;

// CLASS DECLARATION
/**
 * Class implementing Authentication (username and password) dialog
 */
NONSHARABLE_CLASS( CAuthenticationDialog ) : 
                                            public CAknMultiLineDataQueryDialog, public MExpiryTimerCallback
    {
    public:
        /**
        * NewL function
        * @param aPlugin    plugin pointer
        * @param aDb        database pointer
        * @param aUsername  username
        * @param aPassword  password
        * return CAuthenticationDialog*
        */
        static CAuthenticationDialog* NewL( 
                                        CConnDlgAuthenticationPlugin* aPlugin,
                                        TDes& aUsername, 
                                        TDes& aPassword );

    private:
        /**
        * CAuthenticationDialog default constructor
        * @param aPlugin    plugin pointer
        * @param aDb        database pointer
        * @param aUsername  username
        * @param aPassword  password
        */
        CAuthenticationDialog( CConnDlgAuthenticationPlugin* aPlugin );

        /**
        * OkToExitL destructor
        * @param  aButtonId button exit id
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );

        /**
        * ~CAuthenticationDialog destructor
        * @param    -
        */
        ~CAuthenticationDialog();

        /**
        * HandleQueryEditorStateEventL function
        * @param    aQueryControl   Control to be queried
        * @param    aEventType      Type of event (not used)
        * @param    aStatus         Status of the control (empty or not)
        * return    TBool
        */
        TBool HandleQueryEditorStateEventL( CAknQueryControl* aQueryControl, 
                                            TQueryControlEvent aEventType, 
                                            TQueryValidationStatus aStatus );
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();

    private:
        /**
        * PreLayoutDynInitL function
        * @param    -
        */
        virtual void PreLayoutDynInitL();

        /**
        * SetInitialCurrentLine function
        * @param    -
        */
        virtual void SetInitialCurrentLine();


    private:
        // Status of username field (empty or not)
        TBool iFirstEmpty;

        // Status of password field (empty or not)
        TBool iSecondEmpty;

        // Pointer to the plugin
        CConnDlgAuthenticationPlugin* iPlugin;
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };


#endif

// End of File
