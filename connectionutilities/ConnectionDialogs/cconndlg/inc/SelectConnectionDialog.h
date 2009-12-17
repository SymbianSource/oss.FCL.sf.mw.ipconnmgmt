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
* Description:  Declaration of class CSelectConnectionDialog.
*
*/


#ifndef __SELECTCONNECTIONDIALOG_H__
#define __SELECTCONNECTIONDIALOG_H__


// INCLUDES
#include <e32base.h>
#include <aknlistquerydialog.h>
#include "ExpiryTimerCallback.h"


// FORWARD DECLARATION
class CConnDlgSelectConnectionPlugin;
class CConnectionInfoArray;
class MEikSrvNotifierBase2;
class CActiveSelectExplicit;
class CExpiryTimer;

// CLASS DECLARATION 
/**
 * Class implementing SelectConnection dialog
 */
NONSHARABLE_CLASS( CSelectConnectionDialog ) : public CAknListQueryDialog, public MExpiryTimerCallback
    {
    private:
        /**
        * CSelectConnectionDialog constructor
        * @param aPlugin    plugin pointer
        */
        CSelectConnectionDialog( MEikSrvNotifierBase2* aPlugin, 
                                 TInt aRefreshInterval,
                                 TUint32 aBearerSet, 
                                 TBool aIsWLANFeatureSupported, 
                                 TUint aDefaultCMId );

        /**
        * Symbian default constructor.
        */      
        void ConstructL();


    public:
        /**
        * NewL function
        * @param aPlugin    plugin pointer
        * return CSelectConnectionDialog*
        */
        static CSelectConnectionDialog* NewL( MEikSrvNotifierBase2* aPlugin,
                                        TInt aRefreshInterval,
                                        TUint32 aBearerSet, 
                                        TBool aIsWLANFeatureSupported, 
                                        TUint aDefaultCMId );

        /**
        * ~CSelectConnectionDialog destructor
        */
        ~CSelectConnectionDialog();

        /**
        * Exit function OF the CSelectConnectionDialog
        * @param    aButtonId button exit id
        * @return TBool exit or no
        */
        virtual TBool OkToExitL( TInt aButtonId );
        
        void PrepareAndRunLD( CConnectionInfoArray* aIAP, 
                             CArrayPtr< CGulIcon >* aIcons,
                             TBool aIsReallyRefreshing,
                             const TInt aHighlightedItem );
                             
        /**
        * Dialog refreshing
        */
        void RefreshDialogL( CConnectionInfoArray* aIAP, 
                             CArrayPtr< CGulIcon >* aIcons,
                             TBool aIsReallyRefreshing,
                             const TInt aHighlightedItem );
                             
       /**
        *
        * @return The value of iFromOkToExit.
        */
        inline TBool GetOkToExit();          
        
        /**
        * Dialog expiration timeout callback
        */
        void HandleTimedOut();

        /**
        * From CCoeControl  Handle key events. When a key event occurs, 
        *                   CONE calls this function for each control on the control stack, 
        *                   until one of them returns EKeyWasConsumed to indicate that it processed the key event.  
        * @param aKeyEvent  The key event.
        * @param aType      The type of the event: EEventKey, EEventKeyUp or EEventKeyDown.
        * @return           Indicates whether or not the key event was used by this control.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );

        TInt RunLD();

        void SetElementIDL( TUint32 aIAPId );

        void CompleteL( TInt aStatus );
        
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    private:
        /**
        * PreLayoutDynInitL
        * @param    -
        */
        virtual void PreLayoutDynInitL();

        /**
        * Initialize menu pane.
        * @param aResourceId Menu pane resource id.
        * @param CEikMenuPane Menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        void ProcessCommandL( TInt aCommand );


    private:    // Data

        // Pointer to the plugin, not owned
        CConnDlgSelectConnectionPlugin* iPlugin;     
        
        RArray<TUint> iIapIDs;
        RArray<TUint> iDestIDs;

        // For base class, unused.
        TInt iDummy;
        
        TBool iFromOkToExit; ///< closed the dialof from OkToExitL or not

        CArrayPtr< CGulIcon >* iIcons;

        // pointer to active object
        CActiveSelectExplicit* iActiveSelectExplicit;
        TInt iRefreshInterval;

        // Selected preferences
        TUint32 iBearerSet;

        TBool iIsWLANFeatureSupported;

        TUint iDefaultCMId;
        
        // Pointer for dialog expiration timer
        CExpiryTimer* iExpiryTimer;
    };


#include "SelectConnectionDialog.inl"

#endif


// End of File
