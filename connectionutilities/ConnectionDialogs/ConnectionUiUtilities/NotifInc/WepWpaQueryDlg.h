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

// FORWARD DECLARATIONS
class CConnectionDialogsNotifBase;

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
NONSHARABLE_CLASS( CWepWpaQueryDlg ) : public CAknTextQueryDialog
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
    
    TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);    

private:

    /**
    * Draws an info note
    * @param aResId resource id
    * @return -
    */
    void CWepWpaQueryDlg::ShowInfoNoteL( TInt aResId );
    
    /**
    * PreLayoutDynInitL
    * @param    -
    */
    virtual void PreLayoutDynInitL();      

private:
    CConnectionDialogsNotifBase* iNotif;  // Pointer to the Notifier
    TInt   iDialogType;
    TBool& iHex;
    };


#endif  // __CWEPWPAQUERYDLG_H__

// End of File
