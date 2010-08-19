/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of plugins (base & all derived).
*
*/

// INCLUDE FILES
#include "ConnDlgPlugin.h"
#include "AuthenticationDialog.h"
#include "ReconnectDialog.h"
#include "QosDialog.h"
#include "NewIapDialog.h"
#include "ActiveIapPlugin.h"

#include "ActiveSelectConnectionPlugin.h"
#include "CConnDlgPrivateCRKeys.h"
#include <centralrepository.h>

#include "ConnectionDialogsUidDefs.h"
#include "ConnectionDialogsLogger.h"

#include <eikenv.h>
#include <bautils.h>
#include <e32property.h> 
#include <rmpm.h>
#include <startupdomainpskeys.h>

#include <CConnDlgPlugin.rsg>
#include <data_caging_path_literals.hrh>
#include <vpnapi.h>

using namespace CommsDat;

// CONSTANTS

// RSC file name.
_LIT( KResourceFileName, "CConnDlgPlugin.rsc" );

// Panic string
_LIT( KConnDlgPluginPanic, "CConnDlgPlugin" );

LOCAL_D const TInt KPluginGranularity = 4;

#if defined(_DEBUG)
_LIT( KErrActiveObjectNull, "iActivePlugin not NULL" );
#endif



LOCAL_C void CreateNotifiersL( 
                            CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers );




// ---------------------------------------------------------
// NotifierArray()
// Lib main entry point
// ---------------------------------------------------------
//

EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* array = new (ELeave) 
                    CArrayPtrFlat<MEikSrvNotifierBase2>( KPluginGranularity );

    if ( array )
        {
        TRAPD( err, CreateNotifiersL( array ) );
        if( err )
            {
            TInt count = array->Count();
            while( count-- )
                {
                (*array)[count]->Release();
                }
            delete array;
            array = NULL;
            }
        }

    return( array );
    }


// ---------------------------------------------------------
// CreateNotifiersL
// ---------------------------------------------------------
//
LOCAL_C void CreateNotifiersL( 
                              CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    MEikSrvNotifierBase2 *serNotify;
    TBool resourceFileResponsible = ETrue;

    serNotify = CConnDlgIapPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    resourceFileResponsible = EFalse; // Only the first has to load resources

    serNotify = CConnDlgAuthenticationPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgReconnectPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgNewIapPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgQosPlugin::NewL( resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );

    serNotify = CConnDlgSelectConnectionPlugin::NewL( 
                                                    resourceFileResponsible );
    CleanupStack::PushL( serNotify );
    aNotifiers->AppendL( serNotify );
    CleanupStack::Pop( serNotify );
    }



// ---------------------------------------------------------
// CConnDlgIapPlugin::TNotifierInfo CConnDlgIapPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgIapPlugin::TNotifierInfo CConnDlgIapPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgIap;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgIap;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgIapPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgIapPlugin::StartL( const TDesC8& aBuffer, TInt aReplySlot,
                                const RMessagePtr2& aMessage )
    {    
    if ( ScreenSaverOn() || AutolockOn() )
        {
        // Screen saver or Autolock is active. Cancel the dialog. 
        CLOG_WRITE( "CConnDlgAuthenticationPlugin::StartL: Screen saver or Autolock is active." );
        aMessage.Complete( KErrCancel );
        return;
        }

    iPrefs.Copy( (TPtrC8) aBuffer );

    iCancelled = EFalse;
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    
    // Check if there is a suitable active connection
    TInt iap( 0 );
    TInt bearer( ECommDbBearerWLAN ); // User conn. is always WLAN in 9.1
                                      
    // Is there active User connection
    TInt err = GetUserConnection( iap );
    
    if ( err != KErrNone || iap == 0 )
        {
        // Is there any active connection
    	err = GetActiveConnection( iap, bearer );
        }

    if ( ( err == KErrNone ) && 
         ( iap != 0 )        &&  
         ( iPrefs().iBearerSet & bearer ) )
        {
    	// a suitable connection is already active
    	iIAP = iap;
    	CompleteL( KErrNone );
        }
    else
        {
        // display a dialog
        __ASSERT_DEBUG( !iActivePlugin, 
                    User::Panic( KErrActiveObjectNull, KErrNone ) );
    
        iActivePlugin = CActiveCConnDlgIapPlugin::NewL( this, iPrefs() );
        iActivePlugin->StartSearchIAPsL();	
        }
    }



// ---------------------------------------------------------
// void CConnDlgIapPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgIapPlugin::Cancel()
    {
    delete iActivePlugin;
    iActivePlugin = NULL;
    
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        }
    }


// ---------------------------------------------------------
// void CConnDlgIapPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgIapPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgIapPlugin::CompleteL" );      

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TUint32>( iIAP ) );
            }
        
        CLOG_WRITEF( _L( "CConnDlgIapPlugin::CompleteL iIAP : %d" ), iIAP );
                
        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgIapPlugin::CompleteL" );   
    }


// ---------------------------------------------------------
// CConnDlgIapPlugin* CConnDlgIapPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgIapPlugin* CConnDlgIapPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgIapPlugin* self = new (ELeave) CConnDlgIapPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CConnDlgIapPlugin::SetPreferredIapIdL
// ---------------------------------------------------------
//    
void CConnDlgIapPlugin::SetPreferredIapIdL( TUint32 aIAPId )
    {
    CLOG_ENTERFN( "CConnDlgIapPlugin::SetPreferredIapIdL " );
    
    iIAP = aIAPId;
        
    CLOG_LEAVEFN( "CConnDlgIapPlugin::SetPreferredIapIdL " );
    }
    
    
// ---------------------------------------------------------
// CConnDlgIapPlugin::GetActiveConnection
// ---------------------------------------------------------
//    
TInt CConnDlgIapPlugin::GetActiveConnection( TInt& aIapId, 
                                             TInt& aBearer )
    {
    aIapId  = 0;
    aBearer = 0;
    
    TInt err = RProperty::Get( KMPMActiveConnectionCategory, 
                               KMPMPSKeyActiveConnectionIap, 
                               aIapId );
                               
    CLOG_WRITEF( _L( "KMPMPSKeyActiveConnectionIap : %d" ), aIapId );                           
    CLOG_WRITEF( _L( "err : %d" ), err );                           
                               
    if ( err == KErrNone )
        {
    	err = RProperty::Get( KMPMActiveConnectionCategory, 
                              KMPMPSKeyActiveConnectionBearer, 
                              aBearer );
        }
        
    CLOG_WRITEF( _L( "KMPMPSKeyActiveConnectionBearer : %d" ), aBearer );                           
    CLOG_WRITEF( _L( "err : %d" ), err );
    CLOG_WRITEF( _L( "iPrefs().iBearerSet : %d" ), iPrefs().iBearerSet );
    
    return err;
    }
    
    
// ---------------------------------------------------------
// CConnDlgIapPlugin::GetUserConnection
// ---------------------------------------------------------
//    
TInt CConnDlgIapPlugin::GetUserConnection( TInt& aIapId )
    {
    aIapId  = 0;
    
    TInt err = RProperty::Get( KMPMUserConnectionCategory, 
                               KMPMPSKeyUserConnectionIap, 
                               aIapId );
                
    CLOG_WRITEF( _L( "KMPMPSKeyUserConnectionIap : %d" ), aIapId );                           
    CLOG_WRITEF( _L( "err : %d" ), err );                            
        
    return err;    
    }
            

// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin::TNotifierInfo
//                                  CConnDlgAuthenticationPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin::TNotifierInfo 
                                    CConnDlgAuthenticationPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgAuthentication;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgAuthentication;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgAuthenticationPlugin::StartL( const TDesC8& aBuffer, 
                                           TInt aReplySlot,
                                           const RMessagePtr2& aMessage )
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::StartL" );

    if ( ScreenSaverOn() || AutolockOn() )
        {
        // Screen saver or Autolock is active. Cancel the dialog. 
        CLOG_WRITE( "CConnDlgAuthenticationPlugin::StartL: Screen saver or Autolock is active." );
        aMessage.Complete( KErrCancel );
        return;
        }

    if ( aBuffer.Length() > iAuthPairBuff.Length() )
        {
        CLOG_WRITE( "User::Panic, EConnDlgIllegalRequest" );
        User::Panic( KConnDlgPluginPanic, EConnDlgIllegalRequest );
        }

    iAuthPairBuff.Copy( aBuffer );

    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    GetAuthenticationL();

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::StartL" );
    }



// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgAuthenticationPlugin::Cancel()
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::Cancel" );

    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        delete iDialog;
        iDialog = NULL;
        }

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::Cancel" );
    }


// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::GetAuthenticationL()
// ---------------------------------------------------------
//
void CConnDlgAuthenticationPlugin::GetAuthenticationL()
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::GetAuthenticationL" );

    iDialog = CAuthenticationDialog::NewL( this, iAuthPairBuff().iUsername, 
                                           iAuthPairBuff().iPassword );


    iDialog->ExecuteLD( R_CONNDLG_AUTHENTICATION );

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::GetAuthenticationL" );
    }


// ---------------------------------------------------------
// void CConnDlgAuthenticationPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgAuthenticationPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgAuthenticationPlugin::CompleteL" );

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, iAuthPairBuff );
            }

        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgAuthenticationPlugin::CompleteL" );
    }


// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin* CConnDlgAuthenticationPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin* CConnDlgAuthenticationPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgAuthenticationPlugin* self = new (ELeave) 
                                                CConnDlgAuthenticationPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------
// CConnDlgAuthenticationPlugin::CConnDlgAuthenticationPlugin()
// ---------------------------------------------------------
//
CConnDlgAuthenticationPlugin::CConnDlgAuthenticationPlugin()
: iAuthPair( TAuthenticationPair() )
    {
    }


// ---------------------------------------------------------
// CConnDlgQosPlugin::TNotifierInfo CConnDlgQosPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgQosPlugin::TNotifierInfo CConnDlgQosPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgQos;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgQos;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::StartL
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::StartL( const TDesC8& /*aBuffer*/, TInt aReplySlot,
                                const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    GetReconnectL();
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        delete iDialog;
        iDialog = NULL;
        }
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::GetReconnectL()
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::GetReconnectL()
    {
    iDialog = new (ELeave) CQosDialog( this, iBool );
    iDialog->ExecuteLD( R_CONNDLG_QOS );
    }


// ---------------------------------------------------------
// void CConnDlgQosPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgQosPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            TRAP_IGNORE( iMessage.WriteL( iReplySlot, TPckg<TBool>( iBool ) ) );
            }

        iMessage.Complete( aStatus );
        }
    Cancel();
    }


// ---------------------------------------------------------
// CConnDlgQosPlugin* CConnDlgQosPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgQosPlugin* CConnDlgQosPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgQosPlugin* self = new (ELeave) CConnDlgQosPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------
// CConnDlgReconnectPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgReconnectPlugin::TNotifierInfo CConnDlgReconnectPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgReconnect;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgReconnect;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::StartL
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::StartL( const TDesC8& /*aBuffer*/, 
                                      TInt aReplySlot,
                                      const RMessagePtr2& aMessage )
    {
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    GetReconnectL();
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        delete iDialog;
        iDialog = NULL;
        }
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::GetReconnectL()
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::GetReconnectL()
    {
    iDialog = new (ELeave) CReconnectDialog( this, iBool );
    iDialog->ExecuteLD( R_CONNDLG_RECONNECT );
    }


// ---------------------------------------------------------
// void CConnDlgReconnectPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgReconnectPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TBool>( iBool ) );
            }

        iMessage.Complete( aStatus );
        }
    Cancel();
    
    }


// ---------------------------------------------------------
// CConnDlgReconnectPlugin* CConnDlgReconnectPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgReconnectPlugin* CConnDlgReconnectPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgReconnectPlugin* self = new (ELeave) CConnDlgReconnectPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------
// CConnDlgNewIapPlugin::TNotifierInfo CConnDlgNewIapPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgNewIapPlugin::TNotifierInfo CConnDlgNewIapPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgNewIap;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgNewIap;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgNewIapPlugin::StartL( const TDesC8& aBuffer, TInt aReplySlot,
                                   const RMessagePtr2& aMessage )
    {
    if ( aBuffer.Length() > iPrefs.Length() )
        {
        User::Panic( KConnDlgPluginPanic, EConnDlgIllegalRequest );
        }

    iPrefs.Copy( aBuffer );
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    iCancelled = EFalse;

    GetNewIapL();
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgNewIapPlugin::Cancel()
    {
    if ( !iCancelled )
        {
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            iMessage.Complete( KErrCancel );
            }
        delete iDialog;
        iDialog = NULL;
        }
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::GetNewIapL()
// ---------------------------------------------------------
//
void CConnDlgNewIapPlugin::GetNewIapL()
    {
    iDialog = new (ELeave) CNewIapDialog( this, iConnect, iPrefs() );
    iDialog->ExecuteLD( R_CONNDLG_NEW_IAP );
    }


// ---------------------------------------------------------
// void CConnDlgNewIapPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgNewIapPlugin::CompleteL( TInt aStatus )
    {
    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TBool>( iConnect ) );
            }

        iMessage.Complete( aStatus );
        }
    Cancel();
    }


// ---------------------------------------------------------
// CConnDlgNewIapPlugin* CConnDlgNewIapPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgNewIapPlugin* CConnDlgNewIapPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgNewIapPlugin* self = new (ELeave) CConnDlgNewIapPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }

    
// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin::TNotifierInfo 
//                                  CConnDlgSelectConnectionPlugin::RegisterL()
// ---------------------------------------------------------
//
CConnDlgSelectConnectionPlugin::TNotifierInfo 
                                    CConnDlgSelectConnectionPlugin::RegisterL()
    {
    iInfo.iUid = KUidCConnDlgSelectConn;
    iInfo.iPriority = ENotifierPriorityHigh;
    iInfo.iChannel = KUidCConnDlgSelectConn;

    return iInfo;
    }


// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::StartL
// ---------------------------------------------------------
//

void CConnDlgSelectConnectionPlugin::StartL( const TDesC8& aBuffer, 
                                             TInt aReplySlot,
                                             const RMessagePtr2& aMessage )
    {
     CLOG_ENTERFN( "CConnDlgSelectConnectionPlugin::StartL" );      	
    	
    if ( iActivePlugin )
        {
        aMessage.Complete( KErrServerBusy );
        return;
        }
     
    TInt iStartUpPhaseValue( EStartupUiPhaseUninitialized );
    RProperty::Get( KPSUidStartup, KPSStartupUiPhase, iStartUpPhaseValue );
    CLOG_WRITEF( _L( "CConnDlgSelectConnectionPlugin::Phone start up phase: %d" ), 
    									iStartUpPhaseValue );           
    // Don't show the dialog if phone boot up isn't complete or screen saver or Autolock is on.
    if ( iStartUpPhaseValue != EStartupUiPhaseAllDone || ScreenSaverOn() || AutolockOn() )
        { 
        CLOG_WRITE( "CConnDlgSelectConnectionPlugin::StartL: Screen saver or Autolock is active or start up phase not completed yet." );
        aMessage.Complete( KErrCancel );
        return;
        }

    iPrefs.Copy( ( TPtrC8 ) aBuffer );

    iCancelled = ETrue; // This method could leave before displaying the dialog.
    iReplySlot = aReplySlot;
    iMessage = aMessage;
    
    __ASSERT_DEBUG( !iActivePlugin, 
                     User::Panic( KErrActiveObjectNull, KErrNone ) );
    
    iElementID = iPrefs().iRank;
    iActivePlugin = CActiveSelectConnectionPlugin::NewL( this,
                                                         iElementID,
                                                         iPrefs().iBearerSet );
    // Check if there is a suitable active connection
    TInt snap( 0 );
    TInt iap( 0 );
    TInt bearer( ECommDbBearerWLAN ); // User conn. is always WLAN in 9.1
    
    // Is there active User connection
    TInt err = GetUserConnection( iap, snap );
    
    if ( ( err != KErrNone ) || ( iap == 0 && snap == 0 ) )
        {
        // Is there any active connection
    	err = GetActiveConnection( iap, snap, bearer );
        }
                                      
    RVpnServ vpn;
    TInt vpnErr( KErrNone );
    TInt policyCount( 0 );
    TBool forceDialogForIntranetUsers( EFalse );
    
    vpnErr = vpn.Connect();
    if ( !vpnErr ) 
    	  {
    	  vpnErr = vpn.EnumeratePolicies(policyCount);
    	  if ( !vpnErr && policyCount > 0 )
    	  	  {
    	  	  forceDialogForIntranetUsers = ETrue;
    	  	  }
        vpn.Close();
        }
        
    if ( !forceDialogForIntranetUsers &&
    	 ( err == KErrNone ) && 
         ( iap != 0 || snap != 0 ) && 
         ( iPrefs().iBearerSet & bearer ) &&
         ( iPrefs().iDirection == 0 ) )     // JavaVM sets iDirection to "1"
        {
    	// a suitable connection is already active
    	SetElementIDL( iap, snap );
    	CompleteL( KErrNone );
        }
    else
        {
        // display dialog
        iActivePlugin->ShowSelectConnectionL();
        iCancelled = EFalse; // Dialog is now up and running
        }
    }



// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::Cancel()
// ---------------------------------------------------------
//
void CConnDlgSelectConnectionPlugin::Cancel()
    {
    CLOG_ENTERFN( "CConnDlgSelectConnectionPlugin::Cancel" );      

    delete iActivePlugin;
    CLOG_WRITE( "iActivePlugin deleted" );      
    iActivePlugin = NULL;
    CLOG_WRITE( "iActivePlugin NULLed" );      
    
    if ( !iCancelled )
        {
        CLOG_WRITE( "iCancelled == EFalse" );      
        iCancelled = ETrue;
        if ( !iMessage.IsNull() )
            {
            CLOG_WRITE( "Going to Complete iMessage (KErrCancel)" );      
            iMessage.Complete( KErrCancel );
            CLOG_WRITE( "iMessage completed" );
            }
        }

    CLOG_LEAVEFN( "CConnDlgSelectConnectionPlugin::Cancel" );      
    }


// ---------------------------------------------------------
// void CConnDlgSelectConnectionPlugin::CompleteL( TInt aStatus )
// ---------------------------------------------------------
//
void CConnDlgSelectConnectionPlugin::CompleteL( TInt aStatus )
    {
    CLOG_ENTERFN( "CConnDlgSelectConnectionPlugin::CompleteL" );      

    iCancelled = ETrue;
    if ( !iMessage.IsNull() )
        {
        if ( aStatus == KErrNone )
            {
            iMessage.WriteL( iReplySlot, TPckg<TUint32>( iElementID ) );
            }
        
        CLOG_WRITEF( _L( "CConnDlgSelectConnectionPlugin::CompleteL iElementID : %d" ), iElementID );
                
        iMessage.Complete( aStatus );
        }
    Cancel();

    CLOG_LEAVEFN( "CConnDlgSelectConnectionPlugin::CompleteL" );   
    }


// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin* CConnDlgSelectConnectionPlugin::NewL()
// ---------------------------------------------------------
//
CConnDlgSelectConnectionPlugin* CConnDlgSelectConnectionPlugin::NewL( 
                                        const TBool aResourceFileResponsible )
    {
    CConnDlgSelectConnectionPlugin* self = 
                            new (ELeave) CConnDlgSelectConnectionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL( KResourceFileName, aResourceFileResponsible );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin::SetSelectedIDsL
// ---------------------------------------------------------
//    
void CConnDlgSelectConnectionPlugin::SetElementIDL( TUint32 aIAPId, 
                                                    TUint32 aDestinationId )
    {
    iElementID = iActivePlugin->GetElementIDL( aIAPId, aDestinationId );
    }
    
    
// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin::GetUserConnection
// ---------------------------------------------------------
//    
TInt CConnDlgSelectConnectionPlugin::GetUserConnection( TInt& aIapId, 
                                                        TInt& aSnapId )
    {
    aIapId  = 0;
    aSnapId = 0;
    
    TInt err = RProperty::Get( KMPMUserConnectionCategory, 
                               KMPMPSKeyUserConnectionSnap, 
                               aSnapId );
    
    if ( err != KErrNone || aSnapId == 0 )
        {
        err = RProperty::Get( KMPMUserConnectionCategory, 
                              KMPMPSKeyUserConnectionIap, 
                              aIapId );
        }
       
    CLOG_WRITEF( _L( "KMPMPSKeyUserConnectionSnap : %d" ), aSnapId );                      
    CLOG_WRITEF( _L( "KMPMPSKeyUserConnectionIap : %d" ), aIapId );                           
    CLOG_WRITEF( _L( "err : %d" ), err );
        
    return err;    
    }


// ---------------------------------------------------------
// CConnDlgSelectConnectionPlugin::GetActiveConnection
// ---------------------------------------------------------
//    
TInt CConnDlgSelectConnectionPlugin::GetActiveConnection( TInt& aIapId, 
                                                          TInt& aSnapId,
                                                          TInt& aBearer )
    {
    aIapId  = 0;
    aSnapId = 0;
    aBearer = 0;
    
    TInt err = RProperty::Get( KMPMActiveConnectionCategory, 
                               KMPMPSKeyActiveConnectionSnap, 
                               aSnapId );
                                  
    if ( err != KErrNone || aSnapId == 0 )
        {
        err = RProperty::Get( KMPMActiveConnectionCategory, 
                              KMPMPSKeyActiveConnectionIap, 
                              aIapId );
        }
         
    CLOG_WRITEF( _L( "KMPMPSKeyActiveConnectionSnap : %d" ), aSnapId );                      
    CLOG_WRITEF( _L( "KMPMPSKeyActiveConnectionIap : %d" ), aIapId );                           
    CLOG_WRITEF( _L( "err : %d" ), err );                           
                               
    if ( err == KErrNone )
        {
    	err = RProperty::Get( KMPMActiveConnectionCategory, 
                              KMPMPSKeyActiveConnectionBearer, 
                              aBearer );
        }

    CLOG_WRITEF( _L( "KMPMPSKeyActiveConnectionBearer : %d" ), aBearer );                           
    CLOG_WRITEF( _L( "iPrefs().iBearerSet : %d" ), iPrefs().iBearerSet );
    CLOG_WRITEF( _L( "err : %d" ), err );
    
    return err;
    }

// End of File
