/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/



#ifndef __ACTIVELOGIN_H__
#define __ACTIVELOGIN_H__

// INCLUDES
#include "ConnDlgPlugin.h"
#include <e32base.h>


// CLASS DECLARATION

/**
* ActiveObject for asynchronous operations
*/
NONSHARABLE_CLASS( CActiveLogin ) : public CActive
    {
    private:
        /**
        * C++ default constructor.
        * @param aUsername The address where the caller of the Authenticate
        *                  dialog needs to save the username data inserted by 
        *                  the user.
        * @param aPassword The address where the caller of the Authenticate
        *                  dialog needs to save the password data inserted by 
        *                  the user.
        */
        CActiveLogin( TDes& aUsername, TDes& aPassword );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aUsername The address where the caller of the Authenticate
        *                  dialog needs to save the username data inserted by 
        *                  the user.
        * @param aPassword The address where the caller of the Authenticate
        *                  dialog needs to save the password data inserted by 
        *                  the user.
        */
        static CActiveLogin* NewL( TDes& aUsername, TDes& aPassword );

        /**
        * Destructor.
        */
        virtual ~CActiveLogin();


    public: // From base class
        /**
        * This function is called when the scheduled function ends.
        */
        void RunL();

        /**
        * Cancel operations.
        */
        void DoCancel();


    public: // New functions
        /**
        * Add this class on the ActiveScheduler and puts itself active.
        * @param aStatus The status that is checked by the caller of the 
        *                Authenticate dialog.
        */
        void Observe( TRequestStatus &aStatus );

        /**
        * Returns the TAuthenticationPairBuff 
        * @return A pointer to iAuthenticationPairBuff.
        */
        TPckgBuf<TAuthenticationPairBuff>* GetBuffer();


    private:    // Data
        // The status that is checked by the caller of the Authentication 
        // dialog. Not owned.
        TRequestStatus* iRs;    

        // The address of the area where the caller of the Authentication 
        // dialog expects the value for username. Not owned.
        TDes* iUsername;

        // The address of the area where the caller of the Authentication 
        // dialog expects the value for password. Not owned.
        TDes* iPassword;

        // Packed buffer containing user name and password used as input and 
        // output for Authentication dialog.
        TPckgBuf<TAuthenticationPairBuff> iAuthenticationPairBuff;
    };


#endif

// End of File
