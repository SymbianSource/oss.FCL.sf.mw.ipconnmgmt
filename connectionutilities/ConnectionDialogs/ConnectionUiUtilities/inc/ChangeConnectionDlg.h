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
* Description: 
*     Declares dialog.
*
*/


#ifndef CHANGECONNECTIONDLG_H
#define CHANGECONNECTIONDLG_H


// INCLUDES
#include <commdb.h>
#include <aknlistquerydialog.h>
#include <agentdialog.h>
#include <AknWaitDialog.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif

#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "ActiveCChangeConnectionDlg.h"
#include "ExpiryTimerCallback.h"

class CExpiryTimer;
// CLASS DECLARATION

/**
* CChangeConnectionDlg dialog class
*
*/
NONSHARABLE_CLASS( CChangeConnectionDlg ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    public: // Constructors and destructor

        /**
        * Two-phase construction.
        * @param 
        * @param 
        */
        static CChangeConnectionDlg* NewL( TUint32* aIAPId, 
                                       CActiveCChangeConnectionDlg* aActiveDlg,
                                       TDes& aConnectionName );

        /**
        * Destructor.
        */
        ~CChangeConnectionDlg();

        /**
        * Refersh items of dialog
        */
        void RefreshDialogL();
        
    protected:
        /**
        * Constructor.
        */
        CChangeConnectionDlg( TInt aIndex, 
                              TUint32* aIAPId,
                              CActiveCChangeConnectionDlg* aActiveDlg, 
                              TDes& aConnectionName );


    private:

        // From CAknListQueryDialog
        virtual void PreLayoutDynInitL();

        // From CAknListQueryDialog
        TBool OkToExitL( TInt aButtonId );

        /**
        * Handle resource change events. 
        * @param aType: The type of resources that have changed
        */
        void HandleResourceChange( TInt aType );

        /**
        * Create dialog.
        */
        void ConstructL();

        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();

    private: //data
        TUint32*            iIAPId;
        TBuf<CommsDat::KMaxTextLength> iConnectionName;
        TBool               iIsWLANFeatureSupported;
        
        CActiveCChangeConnectionDlg* iActiveDlg; // not owned
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };




#endif

// End of File
