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
* Description:  Implementation of class CReconnectDialog.
*
*/


#ifndef __RECONNECTDIALOG_H__
#define __RECONNECTDIALOG_H__

// INCLUDES
#include <e32base.h>
#include <AknQueryDialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CConnDlgReconnectPlugin;
class CExpiryTimer;

// CLASS DECLARATIONS

/**
 * Class implementing Reconnect dialog
 */
NONSHARABLE_CLASS( CReconnectDialog ) : public CAknQueryDialog, public MExpiryTimerCallback
    {
    public:
        /**
        * Constructor of the CReconnetDialog class
        * @param aPlugin plugin pointer
        * @param aBool reconnect to the IAP
        * @return -
        */
        CReconnectDialog( CConnDlgReconnectPlugin* aPlugin, TBool& aBool );

        /**
        * Exit function the CReconnectDialog
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
    virtual ~CReconnectDialog();


    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();


    private:
        // Pointer to the plugin
        CConnDlgReconnectPlugin* iPlugin;

        // Specifies way of closing of the dialog. ETrue when 'Yes' or 'Done'
        // softkeys pressed.
        TBool& iBool;
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };

#endif  // __RECONNECTDIALOG_H__


// End of File
