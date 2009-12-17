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
* Description:  Declaration of CActiveOfflineWlanDisabledNote
*
*/


#ifndef __ACTIVE_OFFLINE_WLAN_DISABLED_NOTE_H__
#define __ACTIVE_OFFLINE_WLAN_DISABLED_NOTE_H__

// INCLUDES
#include "OfflineWlanDisabledNoteNotif.h"

/**
 * CActiveOfflineWlanDisabledNote class,
 * an active object for COfflineWlanDisabledNoteNotif
 */     
NONSHARABLE_CLASS( CActiveOfflineWlanDisabledNote ) : public CActive
{
public:
    /**
    * Constructor
    */    
    CActiveOfflineWlanDisabledNote( COfflineWlanDisabledNoteNotif* aNotif );

    /**
    * Launch OfflineWlanDisabledNote setactive and starts active object
    */    
    void LaunchOfflineWlanDisabledNote();
    
    /**
    * Destructor
    */
    virtual ~CActiveOfflineWlanDisabledNote();

protected:
    /**
    * DoCancel from CActive
    */    
      virtual void DoCancel();
      
    /**
    * RunL from CActive
    */      
    virtual void RunL();
    
protected:
    TRequestStatus* iClientStatus;   ///< used for complete runl, not owned 
    COfflineWlanDisabledNoteNotif* iNotif; ///< poiter to notifer, not owned
              
};         

#endif // __ACTIVE_OFFLINE_WLAN_DISABLED_NOTE_H__

// End of File
