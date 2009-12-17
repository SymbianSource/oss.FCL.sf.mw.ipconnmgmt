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
@file mpmconfirmdlgroaming.h
UI Note Handler definition.
*/

#ifndef MPMCONFIRMDLGROAMING_H
#define MPMCONFIRMDLGROAMING_H

// INCLUDES
#include <e32base.h>
#include "mpmconfirmdlg.h"

// FORWARD DECLARATIONS
class CMPMServerSession;

// CLASS DECLARATION
/**
*  MPM Confirmation Dialog for roaming connection class
*
*  @lib MPMServer.exe
*  @since 3.2
*/
NONSHARABLE_CLASS( CMPMConfirmDlgRoaming ) : public CBase, 
                                             public MMPMConfirmDlg
  {
    public:

        /**
        * Two-phased constructor.
        */
        static CMPMConfirmDlgRoaming* NewL( 
                        CMPMServerSession&                  aSession,
                        const TUint32                       aSnapId,
                        const TUint32                       aIAP,
                        const CMPMConfirmDlg::TDialogType   aDialogType, 
                        const TBool                         aReconnect );

        /**
        * Destructor.
        */
        ~CMPMConfirmDlgRoaming();

    private:

        /**
        * Handles confirmation query response.
        * @since 3.2
        * @param aError generic Symbian error response code
        * @param aResult Result of user selection
        */
        void HandleResponse( const TInt                      aError,
                             const TMsgQueryLinkedResults    aResult );

        /**
        * C++ default constructor.
        */
        CMPMConfirmDlgRoaming( 
                        CMPMServerSession&                  aSession,
                        const TUint32                       aSnapId,
                        const TUint32                       aIAP,
                        const CMPMConfirmDlg::TDialogType   aDialogType, 
                        const TBool                         aReconnect );

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
        CMPMServerSession&          iSession;
        TUint32                     iSnapId;
        TUint32                     iIAP;
        CMPMConfirmDlg::TDialogType iDialogType;
        CMPMConfirmDlg*             iConfirmDlg;
        TMsgQueryLinkedResults      iMsgQuery;
        TInt                        iError;
        TBool                       iReconnect;

    };

#endif // MPMCONFIRMDLGROAMING_H
