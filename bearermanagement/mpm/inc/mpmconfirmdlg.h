/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Confirm Dialog definition.
*
*/

/**
@file mpmconfirmdlg.h
UI Note Handler definition.
*/

#ifndef MPMCONFIRMDLG_H
#define MPMCONFIRMDLG_H

// INCLUDES
#include <e32base.h>
#include <ConnectionUiUtilities.h>

#include "mpmdialogbase.h"

// CLASS DECLARATION
/**
*  MPM Confirmation Dialog abstract base class
*
*  @lib MPMServer.exe
*  @since 3.2
*/
class MMPMConfirmDlg
    {
    public:
        /**
        * Confirmation Dialog response
        * @since 3.2
        * @param aError generic Symbian error response code
        * @param aResult Result of user selection
        * @return void
        */
        virtual void ConfirmDlgResponse( const TInt                   aError, 
                                         const TMsgQueryLinkedResults aResult ) = 0;
        /**
        * Confirmation Dialog response
        * @since 5.2
        * @param aDlg previously completed aDlg
        * @return ETrue if response accepted
        */
        virtual TBool ConfirmDlgResponse( MMPMConfirmDlg* aDlg ) = 0;
    };

// CLASS DECLARATION
/**
*  MPM Confirmation Dialog base class
*
*  @lib MPMServer.exe
*  @since 3.2
*/
NONSHARABLE_CLASS( CMPMConfirmDlg ) : protected CMPMDialogBase
    {
    public:

        enum TDialogType
            {
            EConfirmDlgHomeNetwork = 0,
            EConfirmDlgVisitorNetwork,
            EConfirmDlgNationalRoaming
            };

        /**
        * Two-phased constructor.
        */
        static CMPMConfirmDlg* NewL( CArrayPtrFlat<CMPMDialogBase>& aQueue,
                                     MMPMConfirmDlg&       aConfirmDlgUser,
                                     const TUint32         aNetId,
                                     const TUint32         aIAP,
                                     const TDialogType     aDialogType );

        /**
        * Destructor.
        */
        ~CMPMConfirmDlg();
    protected:
        /**
        * For starting the active object.
        * @since 3.1
        * @param aDlg Dialog.
         */
        virtual void Start( CMPMDialogBase* aDlg );
        
    private:

        /**
        * C++ default constructor.
        */
        CMPMConfirmDlg( CArrayPtrFlat<CMPMDialogBase>& aQueue,
                        MMPMConfirmDlg&       aConfirmDlgUser,
                        const TUint32         aNetId,
                        const TUint32         aIAP,
                        const TDialogType     aDialogType );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // from CActive

        /**
        * Active Object RunL.
        * @since 3.2
        */
        void RunL();

        /**
        * Active Object DoCancel.
        * @since 3.2
        */
        void DoCancel();

    private:
        MMPMConfirmDlg&         iConfirmDlgUser;
        TUint32                 iSnapId;
        TUint32                 iIAP;
        TDialogType             iDialogType;
        CConnectionUiUtilities* iUtils;
        TMsgQueryLinkedResults  iResult;
    };

#endif // MPMCONFIRMDLG_H
