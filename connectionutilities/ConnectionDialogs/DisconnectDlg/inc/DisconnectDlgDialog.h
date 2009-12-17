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
*     Declares the pop-up selection list with the active connections.
*
*/


#ifndef DDLG_DIALOG_H
#define DDLG_DIALOG_H

// INCLUDES
#include <AknQueryDialog.h>
#include <aknlists.h> 
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CConnectionModel;
class CDisconnectDialogUi;
class CExpiryTimer;

// CLASS DECLARATION


/*
* The dialog of  DisconnectDlg
*/
NONSHARABLE_CLASS( CDisconnectDlgDialog ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        * @param CDisconnectDialogUi* notifier will be completed if 
        * connection is closed
        * @param CConnectionModel* handles connections
        * @param TInt* After the query is dismissed, the index will hold
        *               the value of selected item.
        * @param aIAPId return the selected iap id
        */
        static CDisconnectDlgDialog* NewL( 
                                     CDisconnectDialogUi* aDisconnectDialogUi,
                                     CConnectionModel* aConnModel,
                                     TInt* aIndex,
                                     TUint32& aIAPId );

        /**
        * Launch and destroy dialog.
        */
        void RunDlgLD();

        /**
        * Destructor.
        */
        virtual ~CDisconnectDlgDialog();

    private: // Constructors

        /**
        * Constructor.
        * @param CDisconnectDialogUi* notifier will be completed if 
        * connection is closed
        * @param CConnectionModel* handles connections
        * @param TInt* After the query is dismissed, the index will hold
        *               the value of selected item.
        * @param aIAPId return the selected iap id        
        */
        CDisconnectDlgDialog( CDisconnectDialogUi* aDisconnectDialogUi,
                              CConnectionModel* aConnModel,
                              TInt* aIndex,
                              TUint32& aIAPId );

        /**
        * Constructor.
        */
        void ConstructL();

    protected: //  from CAknListQueryDialog

        /**
        * This function is called by the EIKON dialog framework just
        * before the dialog is activated, but before it is sized,
        * and before PostLayoutDynInitL() is called.
        * @param -
        * @return -        
        */
        void PreLayoutDynInitL();

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
        
    private: //data

        /**
        * Contains connection related functions
        */
        CConnectionModel*              iConnModel;  ///< Not Owned.

        CDisconnectDialogUi*           iDisconnectDialogUi;  ///< Not owned

        HBufC*                         iPrompt; ///< Prompt value of dialog
        
        // ID of IAP
        TUint32& iIAPId;      
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };

#endif /* DDLG_DIALOG_H */

// End of File
