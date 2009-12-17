/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
@file mpmconfirmdlgstarting.h
UI Note Handler definition.
*/

#ifndef MPMCONFIRMDLGSTARTING_H
#define MPMCONFIRMDLGSTARTING_H

// INCLUDES
#include <e32base.h>
#include "mpmconfirmdlg.h"
#include "mpmiapselection.h"

// FORWARD DECLARATIONS
class CMPMIapSelection;
class CMPMServer;
class CMPMServerSession;

// CLASS DECLARATION
/**
*  MPM Confirmation Dialog for establishing connection class
*
*  @lib MPMServer.exe
*  @since 3.2
*/
NONSHARABLE_CLASS( CMPMConfirmDlgStarting ) : public CBase, 
                                              public MMPMConfirmDlg
  {
    public:

        /**
        * Two-phased constructor.
        */
        static CMPMConfirmDlgStarting* NewL( 
                    CMPMIapSelection&                   aIapSelection,
                    const TUint32                       aConnId,
                    const TUint32                       aSnapId,
                    const TUint32                       aIAP,
                    const CMPMConfirmDlg::TDialogType   aDialogType, 
                    const TMpmConnPref&                 aPref,
                    CMPMServer&                         aServer,
                    CMPMServerSession&                  aSession,
                    const TInt                          aIapState );

        /**
        * Destructor.
        */
        ~CMPMConfirmDlgStarting();

    private:

        /**
        * User selected Cancel in Starting Dialog.
        * @since 3.2
        * @param aError generic Symbian error response code
        */
        void UserSelectedCancel( const TInt       aError );
        
        /**
        * User selected "Connect this time" in Starting Dialog.
        * @since 5.2
        */
        void UserSelectedConnectThisTime();
        
        /**
        * User selected "Connect automatically" in Starting Dialog.
        * @since 5.2
        */
        void UserSelectedConnectAutomatically();
        
        /**
        * Starts displaying Starting Dialog.
        * @since 3.2
        */
        void StartL();

        /**
        * C++ default constructor.
        */
        CMPMConfirmDlgStarting( 
                    CMPMIapSelection&                   aIapSelection,
                    const TUint32                       aConnId,
                    const TUint32                       aSnapId,
                    const TUint32                       aIAP,
                    const CMPMConfirmDlg::TDialogType   aDialogType, 
                    const TMpmConnPref&                 aPref,
                    CMPMServer&                         aServer,
                    CMPMServerSession&                  aSession,
                    const TInt                          aIapState );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // from MMPMConfirmDlg

        /**
        * Confirmation Dialog response
        * @since 3.2
        * @param aError generic Symbian error response code
        * @param aResult Result of user selection
        * @return void
        */
    	void ConfirmDlgResponse( const TInt                     aError, 
    	                         const TMsgQueryLinkedResults   aResult );

    	/**
        * Confirmation Dialog response
        * @since 5.2
        * @param aDlg previously completed aDlg
        * @return ETrue if response accepted
        */
        TBool ConfirmDlgResponse( MMPMConfirmDlg* aDlg );

    private:
        CMPMIapSelection&           iIapSelection;
        TUint32                     iConnId;
        TUint32                     iSnapId;
        TUint32                     iIAP;
        CMPMConfirmDlg::TDialogType iDialogType;
        CMPMConfirmDlg*             iConfirmDlg;
        TMsgQueryLinkedResults      iMsgQuery;
        TMpmConnPref                iPref;
        CMPMServer&                 iServer;
        CMPMServerSession&          iSession;
        TInt                        iError;
        TInt                        iIapState;
    };

#endif // MPMCONFIRMDLGSTARTING_H
