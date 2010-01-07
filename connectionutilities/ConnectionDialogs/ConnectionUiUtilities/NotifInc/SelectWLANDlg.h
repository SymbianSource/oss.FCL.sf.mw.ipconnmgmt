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
*     Declares CSelectWLAN dialog.
*
*/


#ifndef SELECTWLANDLG_H
#define SELECTWLANDLG_H

// INCLUDES
#include <aknlistquerydialog.h>
#include <wlanmgmtcommon.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATION
class CSelectWLanDlgPlugin;
class CNetworkInfoArray;
class CAknWaitDialog;
class CExpiryTimer;

// CLASS DECLARATION
/**
* CSelectWLANDlg dialog class
*
*/
NONSHARABLE_CLASS( CSelectWLANDlg ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    public: // Constructors and destructor

        /**
        * Two-phase construction.
        */
        static CSelectWLANDlg* NewL( CSelectWLanDlgPlugin* aPlugin,
                                     CNetworkInfoArray* *const aNetworkArray );

        /**
        * Destructor.
        */
        ~CSelectWLANDlg();
        
        
        /**
        *
        */
        void RefreshDialogL();
        
        /**
        *
        * @return The value of iFromOkToExit.
        */
        inline TBool GetOkToExit();


    protected:
        /**
        * Constructor.
        */
        CSelectWLANDlg( CSelectWLanDlgPlugin* aPlugin, TInt aIndex, 
                        CNetworkInfoArray* *const aNetworkArray );


        /**
        * Symbian default constructor.
        */      
        void ConstructL();

    private:
        // From CAknListQueryDialog
        TBool OkToExitL( TInt aButtonId );
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();

        // From CAknListQueryDialog
        virtual void PreLayoutDynInitL();

        /**
        * Handle resource change events. 
        * @param aType: The type of resources that have changed
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * from CCoeControl
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType);
                                     
        void ShowInfoNoteL( TInt aResId );


    private:  // New functions

        /**
        * Sets graphic icons
        * @param    -
        */
        void SetIconsL();

        TBool ScanForWlanNwL( TWlanSsid& aSsid, 
                              TWlanConnectionMode& aConnectionMode,
                              TWlanConnectionSecurityMode& aSecurityMode,
                              TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                              TBool& aProtectedSetupSupported );

        void DestroyWaitDialog();

    private: //data

        // Pointer to the plugin
        CSelectWLanDlgPlugin*       iPlugin;

        // not owned
        CNetworkInfoArray* *const   iNetworkArray;

         // closed the dialof from OkToExitL or not
        TBool iFromOkToExit;

        CAknWaitDialog* iWaitDialog;
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };

#include "SelectWLANDlg.inl"  

#endif

// End of File
