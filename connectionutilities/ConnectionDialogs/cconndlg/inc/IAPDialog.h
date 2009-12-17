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
* Description:  Declaration of class CIapDialog.
*
*/


#ifndef __IAPDIALOG_H__
#define __IAPDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <aknlistquerydialog.h>

#include "ExpiryTimerCallback.h"

// FORWARD DECLARATION
class CConnDlgIapPlugin;
class CConnectionInfoArray;
class CExpiryTimer;

// CLASS DECLARATION 
/**
 * Class implementing IAP dialog
 */
NONSHARABLE_CLASS( CIapDialog ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    private:
        /**
        * CIapDialog constructor
        * @param aPlugin    plugin pointer
        */
        CIapDialog( CConnDlgIapPlugin* aPlugin );

        /**
        * Symbian default constructor.
        */      
        void ConstructL();


    public:
        /**
        * NewL function
        * @param aPlugin    plugin pointer
        * return CIapDialog*
        */
        static CIapDialog* NewL( CConnDlgIapPlugin* aPlugin );

        /**
        * ~CIapDialog destructor
        */
        ~CIapDialog();

        /**
        * Exit function OF the CIapDialog
        * @param    aButtonId button exit id
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
        
        /**
        * Dialog refreshing
        */
        void RefreshDialogL( CConnectionInfoArray* aIAP, 
                             TBool aIsReallyRefreshing );
                             
       /**
        *
        * @return The value of iFromOkToExit.
        */
        inline TBool GetOkToExit();
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();


    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();


    private:
        /**
        * Sets graphic icons
        * @param    -
        */
        void SetIconsL();

        
    private:    // Data

        // Pointer to the plugin, not owned
        CConnDlgIapPlugin* iPlugin;     
        
        RArray<TUint> iIAPIds;

        // For base class, unused.
        TInt iDummy;
        
        TBool iFromOkToExit; ///< closed the dialof from OkToExitL or not
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };


#include "IapDialog.inl"

#endif


// End of File
