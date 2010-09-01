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
* Description:  Implementation of class CQosDialog.
*
*/


#ifndef __QOSDIALOG_H__
#define __QOSDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <AknQueryDialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CConnDlgQosPlugin;
class CExpiryTimer;

// CLASS DECLARATIONS

/**
 * Class implementing Quality of services dialog.
 */
NONSHARABLE_CLASS( CQosDialog ) : public CAknQueryDialog, public MExpiryTimerCallback
    {
    public:
        /**
        * Constructor the CQosDialog class
        * @param aPlugin plugin pointer
        * @param aBool reconnect to the Qos
        * @return -
        */
        CQosDialog( CConnDlgQosPlugin* aPlugin, TBool& aBool );

        /**
        * Exit function the CQosDialog
        * @param aButtonId plugin pointer
        * @param aBool reconnect to the IAP
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
    virtual ~CQosDialog();


    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();


    private:
        // Pointer to the plugin
        CConnDlgQosPlugin* iPlugin;

        // Specifies way of closing of the dialog. ETrue when 'Yes' or 'Done'
        // softkeys pressed.
        TBool& iBool;     

        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
        
    };

#endif  // __QOSDIALOG_H__


// End of File
