/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class WepWpaQueryDlg.
*
*/


#ifndef __CWEPWPAQUERYDLG_H__
#define __CWEPWPAQUERYDLG_H__


// INCLUDES
#include <AknQueryDialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class CConnectionDialogsNotifBase;
class CExpiryTimer;

enum TDialogType
    {
    EDialogWep     =0,
    EDialogWpa     =1,
    EDialogWapi    =2
    };

// CLASS DECLARATIONS

/**
 * Class implements a query dialog.
 */
NONSHARABLE_CLASS( CWepWpaQueryDlg ) : public CAknTextQueryDialog, public MExpiryTimerCallback
    {
public:
    /**
    * Constructor the COfflineWlanNoteDlg class
    * @param aDataText returned password
    * @param aNotif notifier pointer
    * @param aWpa determines Wpa or Wep query dialog will be created
    * @param aHex Returns ETrue is password is given in hexadecimal, 
    * EFalse if ASCII
    * @return -
    */
    CWepWpaQueryDlg( TDes& aDataText, CConnectionDialogsNotifBase* aNotif, 
                     TDialogType aDialogType, TBool& aHex );

    /**
    * Exit function the COfflineWlanNoteDlg
    * @param aButtonId 
    * @return TBool exit or no
    */
    virtual TBool OkToExitL( TInt aButtonId );
    
    /**
    * Destructor
    */
    virtual ~CWepWpaQueryDlg();
    
    /**
    * from CCoeControl
    * @param aKeyEvent Event to handled.
    * @param aType Type of the key event. 
    * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                 TEventCode aType);
    
    /**
     * Dialog expiration timeout callback
     */
    void HandleTimedOut();

private:

    /**
    * Draws an info note
    * @param aResId resource id
    * @return -
    */
    void ShowInfoNoteL( TInt aResId );
    
    /**
    * PreLayoutDynInitL
    * @param    -
    */
    virtual void PreLayoutDynInitL();      

private:
    CConnectionDialogsNotifBase* iNotif;  // Pointer to the Notifier
    TInt   iDialogType;
    TBool& iHex;
    // Pointer for dialog expiration timer
    CExpiryTimer* iExpiryTimer;
    };


#endif  // __CWEPWPAQUERYDLG_H__

// End of File
