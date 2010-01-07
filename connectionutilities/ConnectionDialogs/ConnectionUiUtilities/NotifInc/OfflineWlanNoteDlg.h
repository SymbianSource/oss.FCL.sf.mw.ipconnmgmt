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
* Description:  Implementation of class COfflineWlanNoteDlg.
*
*/


#ifndef __COFFLINEWLANNOTEDLG_H__
#define __COFFLINEWLANNOTEDLG_H__

// INCLUDES
#include <AknQueryDialog.h>
#include "ExpiryTimerCallback.h"

// FORWARD DECLARATIONS
class COfflineWlanNoteNotif;
class CExpiryTimer;

// CLASS DECLARATIONS

/**
 * Class implements a query dialog.
 */
NONSHARABLE_CLASS( COfflineWlanNoteDlg ) : public CAknQueryDialog, public MExpiryTimerCallback
    {
public:
    /**
    * Constructor the COfflineWlanNoteDlg class
    * @param aNotif notifier pointer
    * @return -
    */
    COfflineWlanNoteDlg( COfflineWlanNoteNotif* aNotif );

    /**
    * Exit function the COfflineWlanNoteDlg
    * @param aButtonId 
    * @return TBool exit or no
    */
    virtual TBool OkToExitL( TInt aButtonId );
    
    /**
    * Dialog expiration timeout callback
    */
    void HandleTimedOut();
    
    /**
    * Destructor
    */
    virtual ~COfflineWlanNoteDlg();

private:
    /**
    * PreLayoutDynInitL
    * @param    -
    */
    virtual void PreLayoutDynInitL();    

private:
    COfflineWlanNoteNotif* iNotif;  // Pointer to the Notifier
    
    // Pointer for dialog expiration timer
    CExpiryTimer* iExpiryTimer;
    };


#endif  // __COFFLINEWLANNOTEDLG_H__

// End of File
