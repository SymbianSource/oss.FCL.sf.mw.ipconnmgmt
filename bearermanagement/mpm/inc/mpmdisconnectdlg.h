/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM class used for disconnect dialog initiation
*
*/

/**
@file mpmdisconnectdlg.h
Mobility Policy Manager disconnect dialog initiation class.
*/

#ifndef MPMDISCONNECTDLG_H
#define MPMDISCONNECTDLG_H

//  INCLUDES
#include <e32base.h>
#include <DisconnectDlgClient.h>
#include "mpmserversession.h"
#include "mpmdialogbase.h"

// CLASS DECLARATION
/**
*  MPM disconnect dialog initiation class definition.
*
*  @lib MPMServer.exe
*  @since 3.1
*/
class CMPMDisconnectDlg : protected CMPMDialogBase
    {
    public:    // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CMPMDisconnectDlg* NewL(
		    CMPMServerSession&                aSession,
            TInt                              aOrigError,
            CArrayPtrFlat<CMPMDisconnectDlg>& aDisconnectQueue );

        /**
        * Destructor.
        */
        virtual ~CMPMDisconnectDlg();

    protected: // New functions

        /**
         * Start Dlg.
         * @param aDlg previous same type of dlg in case its response could
		 *             be utilized.
         */
        virtual void Start( CMPMDialogBase* aDlg );

        /**
        * User selected Ok in Disconnect Dialog.
        * @since 3.2
        * @param aError Error code from dialog
        * @param aIapId Id of the Iap from terminated connection
        */
        void UserSelectedOk( TInt aError );

        /**
        * User selected Cancel in Disconnect Dialog.
        * @since 3.2
        * @param aError Error code from dialog
        */
        void UserSelectedCancel( TInt aError );

    protected: // Functions from base classes

        /**
        * From CActive. Cancels UI dialog.
        * @since 3.1
        */        
        void DoCancel();

        /**
        * From CActive. Run when UI dialog finished.
        * @since 3.1
        */               
        void RunL();

        /**
        * From CActive. Run if RunL leaves.
        * @since 3.1
        * @param aError Error that caused leave.
        * @return KErrNone
        */
        TInt RunError (TInt aError);

    private: // New methods

        /**
        * C++ default constructor.
        */
        CMPMDisconnectDlg(
		    CMPMServerSession&                aSession,
            TInt                              aOrigError,
            CArrayPtrFlat<CMPMDisconnectDlg>& aDisconnectQueue );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
    protected:
        TInt        iUserStatus;
        TInt        iUserIap;
        
    private: // Data
        // Disconnect Dialog Server
        RDisconnectDlgServer        iDlgServ;

        // Session object reference
        CMPMServerSession&          iSession;

        // IAP of the closed connection
        TUint32                     iIapId;

        // Original error code reported by Bearer Manager
        TInt                        iOrigError;
    };


#endif // MPMDISCONNECTDLG_H

// End of file
