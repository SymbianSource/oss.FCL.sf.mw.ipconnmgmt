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
* Description:  Declaration of class CSelectExplicitDialog.
*
*/


#ifndef __SELECTEXPLICITDIALOG_H__
#define __SELECTEXPLICITDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <aknlistquerydialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATION
class CConnectionInfoArray;
class CSelectConnectionDialog;
class CExpiryTimer;

// CLASS DECLARATION 
/**
 * Class implementing SelectExplicit dialog
 */
NONSHARABLE_CLASS( CSelectExplicitDialog ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    private:
        /**
        * CSelectExplicitDialog constructor
        * @param aPlugin    plugin pointer
        */
        CSelectExplicitDialog( CSelectConnectionDialog* aCallerDialog );

        
        /**
        * Symbian default constructor.
        */      
        void ConstructL();


    public:
        /**
        * NewL function
        * @param aPlugin    plugin pointer
        * return CSelectExplicitDialog*
        */
        static CSelectExplicitDialog* NewL( 
                                    CSelectConnectionDialog* aCallerDialog );


        /**
        * ~CSelectExplicitDialog destructor
        */
        ~CSelectExplicitDialog();

        /**
        * Exit function OF the CSelectExplicitDialog
        * @param    aButtonId button exit id
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
        
        void PrepareAndRunLD( CConnectionInfoArray* aIAP, 
                             CArrayPtr< CGulIcon >* aIcons,
                             TBool aIsReallyRefreshing );
                             
        /**
        * Dialog refreshing
        */
        void RefreshDialogL( CConnectionInfoArray* aIAP, 
                             CArrayPtr< CGulIcon >* aIcons,
                             TBool aIsReallyRefreshing );
                             
       /**
        *
        * @return The value of iFromOkToExit.
        */
        inline TBool GetOkToExit();
        
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);
        
        TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);                                

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


    private:    // Data

        // Pointer to the plugin, not owned
        CSelectConnectionDialog* iCallerDialog;

        RArray<TUint> iIAPIds;

        TBool iFromOkToExit; ///< closed the dialof from OkToExitL or not
        
        TInt iDummy;
        
        CArrayPtr< CGulIcon >* iIcons;
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;

    };


#include "SelectExplicitDialog.inl"

#endif


// End of File
