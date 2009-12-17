/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CActiveConnectViaNote
*
*/


#ifndef __ACTIVE_CONNECT_VIA_NOTE_H__
#define __ACTIVE_CONNECT_VIA_NOTE_H__

// INCLUDES
#include "ConnectionUiUtilitiesCommon.h"
#include <e32base.h>
#include <cmmanager.h>

// FORWARD DECLARATIONS
class CConnectViaNoteNotif;
class CGulIcon;
class CEikImage;


/**
 * CActiveConnectViaNote class,
 * an active object for COfflineWlanDisabledNoteNotif
 */     
NONSHARABLE_CLASS( CActiveConnectViaNote ) : public CActive
{
public:
    /**
    * Constructor
    */    
    CActiveConnectViaNote( CConnectViaNoteNotif* aNotif, 
                           const TConnUiUiDestConnMethodNoteId aData );

    /**
    * Launch ConnectViaNote setactive and starts active object
    */    
    void LaunchActiveConnectViaNote();
    
    /**
    * Destructor
    */
    virtual ~CActiveConnectViaNote();

protected:
    /**
    * DoCancel from CActive
    */    
    virtual void DoCancel();
      
    /**
    * RunL from CActive
    */      
    virtual void RunL();
    
    /**
    * GetDialogTextL() 
    * @return 
    */
    HBufC* GetDialogTextL();

    /**
    * GetDestinationNameLC() 
    * @param  aDestId
    * @return 
    */
    HBufC* GetDestinationNameLC( const TUint32 aDestId );
    
    /**
    * GetConnectionMethodNameLC
    * @param aConnMId
    * @return
    */
    HBufC* GetConnectionMethodNameLC( const TUint32 aConnMId );
    
    /**
    * CreateEikImageFromCGulIconLC
    * @return CEikImage*
    */
    CEikImage* CreateEikImageFromCGulIconLC();    
    
protected:
    TRequestStatus* iClientStatus;   ///< used for complete runl, not owned 
    CConnectViaNoteNotif* iNotif; ///< poiter to notifer, not owned
    CGulIcon* iIcon;    
    
    TConnUiUiDestConnMethodNoteId iData;
    RCmManager iCmManager;    
};         

#endif // __ACTIVE_CONNECT_VIA_NOTE_H__

// End of File
