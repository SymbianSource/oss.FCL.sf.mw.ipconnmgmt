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
* Description:  Implementation of CSelectWLanDlgPlugin
*
*/


// INCLUDE FILES
#include "SelectWLanDlgPlugin.h"
#include "SelectWLANDlg.h"

#include "ActiveSelectWLanDlgPlugin.h"
#include "ConnectionDialogsLogger.h"
#include "ConnUiUtilsNotif.h"

#include <e32property.h>         // For RProperty 
#include <UikonInternalPSKeys.h> // For KPSUidUikon and KUikGlobalNotesAllowed.

#include <cmmanager.h>
#include <cmmanagerext.h>
#include <cmconnectionmethod.h>
#include <cmpluginwlandef.h>


// ================= MEMBER FUNCTIONS =======================  

// ---------------------------------------------------------
// CSelectWLanDlgPlugin CSelectWLanDlgPlugin()
// ---------------------------------------------------------
//
CSelectWLanDlgPlugin::CSelectWLanDlgPlugin() 
: CConnectionDialogsNotifBase(), 
  iActivePlugin( NULL )
    {
    }


// ---------------------------------------------------------
// CSelectWLanDlgPlugin* CSelectWLanDlgPlugin::NewL()
// ---------------------------------------------------------
//
CSelectWLanDlgPlugin* CSelectWLanDlgPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CSelectWLanDlgPlugin* self = new (ELeave) CSelectWLanDlgPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------
// CSelectWLanDlgPlugin::TNotifierInfo CSelectWLanDlgPlugin::RegisterL()
// ---------------------------------------------------------
//
CSelectWLanDlgPlugin::TNotifierInfo CSelectWLanDlgPlugin::RegisterL()
    {
    iInfo.iUid = KUidSelectWLanDlg;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidSelectWLanDlg;

    return iInfo;
    }

// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::StartL()
// ---------------------------------------------------------
//
void CSelectWLanDlgPlugin::StartL( const TDesC8& aBuffer, TInt aReplySlot,
                                   const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::StartL " );  
    
    TPckgBuf<TBool> asyncVersion;
    asyncVersion.Copy( aBuffer );

    iCancelled = EFalse;
    
    // We are about to display the Offline note.
    // Since this part of the code can be executed during the bootup, check if
    // the UI has really started up to display notes/dialogs.
    TInt notesAllowed = 0;
    User::LeaveIfError ( RProperty::Get( KPSUidUikon, KUikGlobalNotesAllowed,
                                        notesAllowed ) );

    iReplySlot = aReplySlot;
    iMessage = aMessage;

    if ( notesAllowed )
        {
        aMessage.ReadL( aReplySlot, iNetworkPrefs );
    
        TInt err( KErrNone );
        TRAP( err, iActivePlugin = CActiveSelectWLanDlgPlugin::NewL( this ) ); 
        if ( err )
            {
            delete iActivePlugin;
            iActivePlugin = NULL;
            iCancelled = ETrue; 
            User::Leave( err );           
            }
        
        iActivePlugin->StartSearchWlanNetworkL(); 
        CLOG_LEAVEFN( "CSelectWLanDlgPlugin::StartL " );  
        }
    else if ( asyncVersion() )
        {
        CLOG_WRITE( "Completing with KErrNotReady" );
        CompleteL( KErrNotReady );
        }
    else
        {
        iCancelled = ETrue; 
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::Cancel()
// ---------------------------------------------------------
//
void CSelectWLanDlgPlugin::Cancel()
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::Cancel " );  
    
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        
        delete iActivePlugin;
        iActivePlugin = NULL;
        }   
    CLOG_LEAVEFN( "CSelectWLanDlgPlugin::Cancel " );  
                     
    }

// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CSelectWLanDlgPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::CompleteL " );  
    
    if ( ( ( aStatus == KErrNone ) || ( aStatus == KErrNotFound ) ) && !iMessage.IsNull() )
        {
        HBufC* ssid16 = HBufC::NewLC( iNetworkPrefs().iSsId.Length() );
        ssid16->Des().Copy( iNetworkPrefs().iSsId ); 
        CLOG_WRITEF( _L( "iSSID : %S" ), ssid16 );    
        CleanupStack::PopAndDestroy( ssid16 );
        CLOG_WRITEF( _L( "iCntMode : %d" ), iNetworkPrefs().iNetworkMode );
        CLOG_WRITEF( _L( "iSecurityMode : %d" ), iNetworkPrefs().iSecMode );
    
        iMessage.WriteL( iReplySlot, iNetworkPrefs );
        }

    CLOG_WRITEF( _L( "aStatus in CompleteL: %d" ), aStatus );

    delete iActivePlugin;
    iActivePlugin = NULL;        

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( aStatus );
        }
    Cancel();
    
    CLOG_LEAVEFN( "CSelectWLanDlgPlugin::CompleteL " );  
    }
    
// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::StopTimer
// ---------------------------------------------------------
//    
void CSelectWLanDlgPlugin::StopTimer()
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::StopTimer " );  
    
    iActivePlugin->Cancel();  

    CLOG_LEAVEFN( "CSelectWLanDlgPlugin::StopTimer " );  
      
    }
    
    
// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::SetNetworkPrefsL
// ---------------------------------------------------------
//
void CSelectWLanDlgPlugin::SetNetworkPrefsL( TWlanSsid& aSSID,
                      TWlanConnectionMode& aConnectionMode,
                      TWlanConnectionSecurityMode& aSecurityMode,
                      TWlanConnectionExtentedSecurityMode& aExtSecurityMode,
                      TBool& aProtectedSetupSupported )
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::SetNetworkPrefsL " );  
    iNetworkPrefs().iSsId = aSSID;
    iNetworkPrefs().iNetworkMode = aConnectionMode;
    iNetworkPrefs().iSecMode = aSecurityMode;
    iNetworkPrefs().iExtSecMode = aExtSecurityMode;
    iNetworkPrefs().iProtectedSetupSupported = aProtectedSetupSupported;
          
    HBufC* ssid16 = HBufC::NewLC( aSSID.Length() );
    ssid16->Des().Copy( aSSID ); 
    CLOG_WRITEF( _L( "aSSID : %S" ), ssid16 );    
    CleanupStack::PopAndDestroy( ssid16 );
    CLOG_WRITEF( _L( "aConnectionMode : %d" ),  aConnectionMode );    
    CLOG_WRITEF( _L( "aSecurityMode : %d" ),  aSecurityMode );
    CLOG_WRITEF( _L( "aExtSecurityMode : %d" ),  aExtSecurityMode );             
     
    CLOG_LEAVEFN( "CSelectWLanDlgPlugin::SetNetworkPrefsL " );
    }


// ---------------------------------------------------------
// void CSelectWLanDlgPlugin::SetEasyWlanToHiddenL()
// ---------------------------------------------------------
//
void CSelectWLanDlgPlugin::SetEasyWlanToHiddenL( const TBool aValue )
    {
    CLOG_ENTERFN( "CSelectWLanDlgPlugin::SetEasyWlanToHiddenL" );

    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    CleanupClosePushL( cmManagerExt );

    TUint32 easyWlanId = cmManagerExt.EasyWlanIdL();
    CLOG_WRITEF( _L( "easyWlanId %d" ), easyWlanId );
   
    if ( easyWlanId )
        {
        RCmConnectionMethodExt cmConnMethodExt;
        cmConnMethodExt = cmManagerExt.ConnectionMethodL( easyWlanId );
        CleanupClosePushL( cmConnMethodExt );

        cmConnMethodExt.SetBoolAttributeL( CMManager::EWlanScanSSID, aValue );
        cmConnMethodExt.UpdateL();

        CleanupStack::PopAndDestroy(); // cmConnMethodExt;
        }

    CleanupStack::PopAndDestroy(); // cmManagerExt

    CLOG_LEAVEFN( "CSelectWLanDlgPlugin::SetEasyWlanToHiddenL" );
    }
    
// End of File
