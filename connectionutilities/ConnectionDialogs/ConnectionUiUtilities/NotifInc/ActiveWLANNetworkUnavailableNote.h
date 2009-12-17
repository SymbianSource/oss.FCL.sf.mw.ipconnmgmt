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
* Description:  Declaration of CActiveWLANNetworkUnavailableNote
*
*/



#ifndef __ACTVE_WLAN_UNAVAILABLE_NOTE_H__
#define __ACTVE_WLAN_UNAVAILABLE_NOTE_H__


// INCLUDES
#include "ConnectionUiUtilitiesCommon.h"
#include "ConnectionDialogsNotifBase.h"
//#include "WLANNetworkUnavailableNoteNotif.h"

/**
 * CActiveOfflineWlanDisabledNote class,
 * an active object for COfflineWlanDisabledNoteNotif
 */     
NONSHARABLE_CLASS( CActiveWLANNetworkUnavailableNote ) : public CActive
{
public:
    
    enum
        {
        EWlanNote1     = 0, // R_WLAN_NETWORK_UNAVAILABLE
        EWlanNote2     = 1  // R_OCC_INFO_NO_WLAN_AVAILABLE
        };

    /**
    * Constructor
    */    
    CActiveWLANNetworkUnavailableNote( CConnectionDialogsNotifBase* aNotif );

    /**
    * Launch OfflineWlanDisabledNote setactive and starts active object
    */    
    void LaunchWLANNetworkUnavailable( const TInt aNoteType );
    
    /**
    * Destructor
    */
    virtual ~CActiveWLANNetworkUnavailableNote();

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
    CConnectionDialogsNotifBase* iNotif; ///< poiter to notifer, not owned
    TInt                         iNoteType; // Type of the text to be shown
              
};         

#endif // __ACTVE_WLAN_UNAVAILABLE_NOTE_H__

// End of File
