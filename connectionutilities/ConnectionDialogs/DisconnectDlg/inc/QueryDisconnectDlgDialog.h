/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Declares the QueryDisconnectDlgDialog.
*
*/


#ifndef DDLG_QUERY_DIALOG_H
#define DDLG_QUERY_DIALOG_H

// INCLUDES
#include <AknQueryDialog.h>
#include <aknlists.h> 
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CDisconnectDialogUi;
class CExpiryTimer;


// CLASS DECLARATION
NONSHARABLE_CLASS( CQueryDisconnectDlgDialog ) : public CAknQueryDialog, public MExpiryTimerCallback
    {    
    public:
        /**
        * Contructor
        */
        CQueryDisconnectDlgDialog( CDisconnectDialogUi* aDisconnectDialogUi );
        
        /**
        * Destructor
        */
        ~CQueryDisconnectDlgDialog();
        
        /**
        *
        */
        void PreLayoutDynInitL();

    protected: //  from CAknQueryDialog
        /**
        * This function is called by the EIKON framework 
        * if the user activates a button in the button panel. 
        * It is not called if the Cancel button is activated, 
        * unless the EEikDialogFlagNotifyEsc flag is set.
        * @param aButtonId  The ID of the button that was activated
        * @return Should return ETrue if the dialog should exit, 
        * and EFalse if it should not.
        */
        TBool OkToExitL( TInt aButtonId );
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();
    
    private:        
        CDisconnectDialogUi* iDisconnectDialogUi;  ///< Not owned
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;

    };

#endif // DDLG_QUERY_DIALOG_H

// End of file
