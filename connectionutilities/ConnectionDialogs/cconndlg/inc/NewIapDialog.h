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
* Description:  Declaration of class CNewIapDialog.
*
*/


#ifndef __NEWIAPDIALOG_H__
#define __NEWIAPDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <AknQueryDialog.h>
#include <agentdialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CConnDlgNewIapPlugin;
class CExpiryTimer;

// CLASS DECLARATIONS

/**
 * Class implementing New IAP warning dialog.
 */
NONSHARABLE_CLASS( CNewIapDialog ) : public CAknQueryDialog, public MExpiryTimerCallback
    {
    public:

        /**
        * Constructor the CNewIapDialog class
        * @param aPlugin plugin pointer
        * @param aConnect connect to the IAP
        * @param aPrefs preferences infomation
        * @return -
        */
        CNewIapDialog( CConnDlgNewIapPlugin* aPlugin, 
                       TBool& aConnect,
                       const TNewIapConnectionPrefs& aPrefs );

    /**
    * Destructor
    */
    virtual ~CNewIapDialog();

        /**
        * CBA handling function
        * @param aButtonId button Uid
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();

    private:
        /**
        * PreLayoutDynInitL function
        * @param -
        * @return -
        */
        void PreLayoutDynInitL();

    private:
        // Pointer to the plugin
        CConnDlgNewIapPlugin* iPlugin;      

        // Specifies way of closing of the dialog. ETrue when 'Yes' or 'Done'
        // softkeys pressed.
        TBool& iConnect;                    

        // Selected preferences
        const TNewIapConnectionPrefs& iPrefs;

        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };


#endif

// End of File
