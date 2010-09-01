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
* Description:  Implementation of CActiveConnectViaNote.
*
*/


// INCLUDE FILES
#include "ActiveConnectViaNote.h"
#include "ConnectViaNoteNotif.h"
#include "NoteDlgSuppressingSwitch.h"
#include "ConnectionDialogsLogger.h"

#include <ConnUiUtilsNotif.rsg>
#include <StringLoader.h>

#include <eikimage.h>
#include <gulicon.h>

#include <cmconnectionmethod.h>
#include <cmdestination.h>

#ifndef __WINS__
#include <featmgr.h>
#include <wlanmgmtclient.h>
#include <cmmanagerext.h>
#endif  // !__WINS__

// CONSTANTS

LOCAL_D const TInt KStringsGranularity = 2;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveConnectViaNote::CActiveConnectViaNote()
// ---------------------------------------------------------
//
CActiveConnectViaNote::CActiveConnectViaNote( 
                           CConnectViaNoteNotif* aNotif, 
                           const TConnUiUiDestConnMethodNoteId aData ) 
: CActive( KErrNone ), 
  iNotif( aNotif ), 
  iIcon( NULL ), 
  iData( aData )
    {
    CActiveScheduler::Add( this );            
    }
    
// ---------------------------------------------------------
// CActiveConnectViaNote::DoCancel()
// ---------------------------------------------------------
// 
void CActiveConnectViaNote::DoCancel()
    {
    }
 
// ---------------------------------------------------------
// CActiveConnectViaNote::RunL()
// ---------------------------------------------------------
//     
void CActiveConnectViaNote::RunL()
    {
    CLOG_ENTERFN( "CActiveConnectViaNote::RunL" );  
    
    if( iStatus == KErrNone )
        {
        iCmManager.OpenL();
        
        HBufC* noteText = GetDialogTextL();
        CleanupStack::PushL( noteText );
                
        CNoteDlgSuppressingSwitch* dialog = new (ELeave) 
                            CNoteDlgSuppressingSwitch( 
                            REINTERPRET_CAST( CEikDialog**, &dialog ) );
        CleanupStack::PushL( dialog );
        
        CEikImage* image = CreateEikImageFromCGulIconLC();
        dialog->SetIconL( image ); //The dialog takes ownership of the pointer
        CleanupStack::Pop( image );
        
        CleanupStack::Pop( dialog );

        TInt resId = R_CONNECTING_VIA_INFO_NOTE;    
        switch( iData.iNoteId )
            {
            case EConnectedViaDestConnMethodConfirmationNote:
            case EConnectedViaConnMethodConfirmationNote:
                {
                resId = R_CONNECTED_VIA_CONFIRM_NOTE;
                break;
                }
            case EConnectingToConnMethodInfoNote:
            default :
                {                
                break;
                }
            }
            
        dialog->PrepareLC( resId );
        dialog->ActivateL();
        dialog->SetCurrentLabelL( EGeneralNote, noteText->Des() );
        
        dialog->RunDlgLD( CAknNoteDialog::ELongTimeout, 
                          CAknNoteDialog::ENoTone );
                                         
        CleanupStack::PopAndDestroy( noteText );
        
        CLOG_WRITE( "CActiveConnectViaNote::RunL: Completing with KErrNone" );

        iNotif->CompleteL( KErrNone );                     
        }     
    
    CLOG_LEAVEFN( "CActiveConnectViaNote::RunL" );  
    }
    
// ---------------------------------------------------------
// CActiveConnectViaNote::LaunchActiveConnectViaNote()
// ---------------------------------------------------------
//    
void CActiveConnectViaNote::LaunchActiveConnectViaNote()
    {
    CLOG_ENTERFN( "CActiveConnectViaNote::LaunchActiveConnectViaNote" );  
    
    SetActive();    
    iClientStatus = &iStatus;
    User::RequestComplete( iClientStatus, KErrNone );    
    }
    
    
// ---------------------------------------------------------
// CActiveConnectViaNote::~CActiveConnectViaNote()
// ---------------------------------------------------------
//    
CActiveConnectViaNote::~CActiveConnectViaNote()
    {
    Cancel();  
    iCmManager.Close();  
       
    delete iIcon;
    iIcon = NULL;      
    }
    
// ---------------------------------------------------------
// CActiveConnectViaNote::GetDialogTextL()
// ---------------------------------------------------------
//    
HBufC* CActiveConnectViaNote::GetDialogTextL()
    {
    HBufC* tempText = NULL;
    switch( iData.iNoteId )
        {
        case EConnectedViaDestConnMethodConfirmationNote:
            {
            HBufC* destName = GetDestinationNameLC( iData.iDestination );
            HBufC* cmName = GetConnectionMethodNameLC( 
                                                    iData.iConnectionMethod );
            CDesCArrayFlat* strings = new( ELeave ) CDesCArrayFlat( 
                                                        KStringsGranularity );
            CleanupStack::PushL( strings );
            
            strings->AppendL( *destName );   
            strings->AppendL( *cmName );
            tempText = StringLoader::LoadL( 
                                        R_QTN_NETW_CONSET_CONF_CONNECTED_TO,
                                        *strings );

            CleanupStack::PopAndDestroy( strings );  
            CleanupStack::PopAndDestroy( cmName );
            CleanupStack::PopAndDestroy( destName );
            break;
            }

        case EConnectedViaConnMethodConfirmationNote:
        case EConnectingToConnMethodInfoNote:
        default:
            {
            HBufC* cmNname = GetConnectionMethodNameLC( 
                                                    iData.iConnectionMethod );
            tempText = StringLoader::LoadL( 
                ( iData.iNoteId == EConnectedViaConnMethodConfirmationNote ) ?
                R_QTN_NETW_CONSET_CONF_CONNECTED_VIA_METHOD : 
                R_QTN_NETW_CONSET_INFO_ROAMING_TO,
                *cmNname );
            CleanupStack::PopAndDestroy( cmNname ); 
            break;
            }
        }

    return tempText;
    }

// ---------------------------------------------------------
// CActiveConnectViaNote::GetDestinationNameLC()
// ---------------------------------------------------------
//    
HBufC* CActiveConnectViaNote::GetDestinationNameLC( const TUint32 aDestId )
    {
    HBufC* tempDestName( NULL );
        
    RCmDestination destination = iCmManager.DestinationL( aDestId );    
    CleanupClosePushL( destination );

    tempDestName = destination.NameLC();
    CleanupStack::Pop( tempDestName );  // tempDestName

    CleanupStack::PopAndDestroy();  // destination
    CleanupStack::PushL( tempDestName );
            
    return tempDestName;
    }
    
// ---------------------------------------------------------
// CActiveConnectViaNote::GetConnectionMethodNameLC()
// ---------------------------------------------------------
//    
HBufC* CActiveConnectViaNote::GetConnectionMethodNameLC( 
                                                    const TUint32 aConnMId )
    {
    HBufC* tempCMName( NULL );
    TBool isEasyWlan = EFalse;

#ifndef __WINS__
    
    FeatureManager::InitializeLibL();
    TBool isWlanSupported = 
                    FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    FeatureManager::UnInitializeLib();

    if ( isWlanSupported )
        {
        if ( iCmManager.EasyWlanIdL() == iData.iConnectionMethod )
            {
            TWlanSsid ssidConn;
            CWlanMgmtClient *wlanMgmtClient = CWlanMgmtClient::NewL();
            CleanupStack::PushL( wlanMgmtClient );

            if ( !wlanMgmtClient->GetConnectionSsid( ssidConn ) )
                {
                tempCMName = HBufC::NewL( ssidConn.Length() );
                tempCMName->Des().Copy( ssidConn ); 
                isEasyWlan = ETrue;
                }

            wlanMgmtClient->CancelNotifications();
            CleanupStack::PopAndDestroy( wlanMgmtClient );
            }
        }
#endif  // !__WINS__
    
    CLOG_WRITEF( _L( "Connection Method id in CActiveConnectViaNote: %d" ), aConnMId );

    RCmConnectionMethod connMethod = iCmManager.ConnectionMethodL( aConnMId );
    CleanupClosePushL( connMethod );

    TInt tempIcon = connMethod.GetIntAttributeL( CMManager::ECmBearerIcon );
    CLOG_WRITEF( _L( "tempIcon in CActiveConnectViaNote: %d" ), tempIcon );
    iIcon = REINTERPRET_CAST( CGulIcon*, tempIcon );
    
    if ( !isEasyWlan )
        {
        tempCMName = connMethod.GetStringAttributeL( CMManager::ECmName );
        }
    
    CleanupStack::PopAndDestroy();  // connMethod
    CleanupStack::PushL( tempCMName );
    
    return tempCMName;
    }
    
// ---------------------------------------------------------
// CActiveConnectViaNote::CreateEikImageFromCGulIconLC()
// ---------------------------------------------------------
//   
CEikImage* CActiveConnectViaNote::CreateEikImageFromCGulIconLC()
    {
    CEikImage* tempImage = new ( ELeave ) CEikImage();
    
    if( iIcon )
        {                
        CFbsBitmap* bm = iIcon->Bitmap();
        CFbsBitmap* m = iIcon->Mask();
        tempImage->SetPicture( bm, m );
        iIcon->SetBitmapsOwnedExternally( ETrue );                
        }
    
    CleanupStack::PushL( tempImage );
    return tempImage;    
    }



// End of File
