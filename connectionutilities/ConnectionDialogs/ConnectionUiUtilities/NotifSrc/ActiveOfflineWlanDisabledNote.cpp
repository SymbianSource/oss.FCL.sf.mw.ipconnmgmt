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
* Description:  Implementation of CActiveOfflineWlanDisabledNote.
*
*/


// INCLUDE FILES
#include "ActiveOfflineWlanDisabledNote.h"

#include <connuiutilsnotif.rsg>
#include <hbdevicenotificationdialogsymbian.h>
#include <StringLoader.h>

// CONSTANTS

// Empty string
_LIT( KEmpty, "" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::CActiveOfflineWlanDisabledNote()
// ---------------------------------------------------------
//
CActiveOfflineWlanDisabledNote::CActiveOfflineWlanDisabledNote( 
                                COfflineWlanDisabledNoteNotif* aNotif )
: CActive( KErrNone ), 
  iNotif( aNotif )
    {
    CActiveScheduler::Add( this );            
    }
    
    
// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::~CActiveOfflineWlanDisabledNote()
// ---------------------------------------------------------
//    
CActiveOfflineWlanDisabledNote::~CActiveOfflineWlanDisabledNote()
    {
    Cancel();    
    }


// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::RunL()
// ---------------------------------------------------------
//     
void CActiveOfflineWlanDisabledNote::RunL()
    {
    if( iStatus == KErrNone )
        {
        HBufC* stringLabel =  StringLoader::LoadLC( R_OFFLINE_NOT_POSSIBLE );
        CleanupStack::PushL( stringLabel );
                                          
        CHbDeviceNotificationDialogSymbian::NotificationL(
                KEmpty, stringLabel->Des(), KEmpty);
        CleanupStack::PopAndDestroy( stringLabel );
        
        iNotif->SetCancelledFlag( ETrue );
        iNotif->CompleteL( KErrNone );                     
        }                
    }

    
// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::DoCancel()
// ---------------------------------------------------------
// 
void CActiveOfflineWlanDisabledNote::DoCancel()
    {
    }

 
// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::LaunchOfflineWlanDisabledNote()
// ---------------------------------------------------------
//    
void CActiveOfflineWlanDisabledNote::LaunchOfflineWlanDisabledNote()
    {
    SetActive();    
    iClientStatus = &iStatus;
    User::RequestComplete( iClientStatus, KErrNone );    
    }


// End of File
