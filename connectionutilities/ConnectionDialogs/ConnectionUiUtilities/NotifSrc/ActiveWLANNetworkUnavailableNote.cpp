/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CActiveWLANNetworkUnavailableNote
*
*/


// INCLUDE FILES
#include "ActiveWLANNetworkUnavailableNote.h"

#include <connuiutilsnotif.rsg>
#include <hbdevicenotificationdialogsymbian.h>
#include <StringLoader.h>


// CONSTANTS

// Empty string
_LIT( KEmpty, "" );


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CActiveWLANNetworkUnavailableNote::CActiveWLANNetworkUnavailableNote()
// ---------------------------------------------------------
//
CActiveWLANNetworkUnavailableNote::CActiveWLANNetworkUnavailableNote( 
                                  CConnectionDialogsNotifBase* aNotif ) :
CActive( KErrNone ), iNotif( aNotif )
    {
    CActiveScheduler::Add( this );            
    }
    
// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::DoCancel()
// ---------------------------------------------------------
// 
void CActiveWLANNetworkUnavailableNote::DoCancel()
    {
    }
 
// ---------------------------------------------------------
// CActiveOfflineWlanDisabledNote::RunL()
// ---------------------------------------------------------
//     
void CActiveWLANNetworkUnavailableNote::RunL()
    {
    if( iStatus == KErrNone )
        {
        HBufC* stringLabel;
        
        if ( iNoteType == EWlanNote2 )
            {
            stringLabel =  StringLoader::LoadLC( 
                                 R_OCC_INFO_NO_WLAN_AVAILABLE );
                    
            }
        else
            {
            stringLabel =  StringLoader::LoadLC( 
                                   R_WLAN_NETWORK_UNAVAILABLE );
            }
                                          
        CHbDeviceNotificationDialogSymbian::NotificationL(
                KEmpty, stringLabel->Des(), KEmpty);
        CleanupStack::PopAndDestroy( stringLabel );                    
        }
    
    iNotif->SetCancelledFlag( ETrue );
    iNotif->CompleteL( KErrNone );
    }
    
// ---------------------------------------------------------
// CActiveWLANNetworkUnavailableNote::LaunchWLANNetworkUnavailable()
// ---------------------------------------------------------
//    
void CActiveWLANNetworkUnavailableNote::LaunchWLANNetworkUnavailable(
                                                 const TInt aNoteType )
    {
    iNoteType = aNoteType;
    SetActive();    
    iClientStatus = &iStatus;
    User::RequestComplete( iClientStatus, KErrNone );    
    }
    
    
// ---------------------------------------------------------
// CActiveWLANNetworkUnavailableNote::CActiveWLANNetworkUnavailableNote()
// ---------------------------------------------------------
//    
CActiveWLANNetworkUnavailableNote::~CActiveWLANNetworkUnavailableNote()
    {
    Cancel();    
    }


// End of File
