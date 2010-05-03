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
* Description:  Implementation of ecom plugin
*
*/


// INCLUDE FILES
#include "ConnectionDialogsUidDefs.h"
#include "ConnectionDialogsLogger.h"

#include <ecom/implementationproxy.h>
#include <AknNotifierWrapper.h> // link against aknnotifierwrapper.lib
#include <wifiprotuiddefs.h>


// CONSTANTS
const TInt KMyPriority =  MEikSrvNotifierBase2::ENotifierPriorityLow;
const TInt KArrayGranularity = 5;
 

// ---------------------------------------------------------
// CleanupArray()
// ---------------------------------------------------------
//
void CleanupArray( TAny* aArray )
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>*     
        subjects = static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>( aArray );
    TInt lastInd = subjects->Count()-1;
    for ( TInt i = lastInd; i >= 0; i-- )
        {
        subjects->At( i )->Release();
        }

    delete subjects;
    }
    
// ---------------------------------------------------------
// DoCreateNotifierArrayL()
// ---------------------------------------------------------
//
CArrayPtr<MEikSrvNotifierBase2>* DoCreateNotifierArrayL()
    {
    CLOG_CREATE;
    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects =
        new ( ELeave )CArrayPtrFlat<MEikSrvNotifierBase2>( KArrayGranularity );
    
    CleanupStack::PushL( TCleanupItem( CleanupArray, subjects ) );

    // Create Wrappers

    _LIT( KCConnDlgPluginName, "CCONNDLGPLUGIN.DLL" );
    _LIT( KDisconnectDlgName, "DISCONNECTDLG.DLL" );
    _LIT( KConnUiUtilsNotifName, "ConnUiUtilsNotif.DLL" );

    // Session owning notifier(if default implementation is enough)
    CAknCommonNotifierWrapper* master = 
        CAknCommonNotifierWrapper::NewL( KUidCConnDlgIap,
                                         KUidCConnDlgIap,
                                         KMyPriority,
                                         KCConnDlgPluginName,
                                         1 ); // we don't use synch reply

    CleanupStack::PushL( master );   
    subjects->AppendL( master );
    CleanupStack::Pop( master );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCConnDlgNewIap,
                                            KUidCConnDlgNewIap,
                                            KMyPriority ) );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCConnDlgAuthentication,
                                            KUidCConnDlgAuthentication,
                                            KMyPriority ) );
                                   
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCConnDlgReconnect,
                                            KUidCConnDlgReconnect,
                                            KMyPriority ) );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCConnDlgQos,
                                            KUidCConnDlgQos,
                                            KMyPriority ) );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCConnDlgSelectConn,
                                            KUidCConnDlgSelectConn,
                                            KMyPriority ) );

_LIT( KWiFiProtSetupPluginName , "wifiprotplugin.dll" );
   master = 
        CAknCommonNotifierWrapper::NewL( KUidWiFiProtSetup,
                                         KUidWiFiProtSetup,
                                         KMyPriority,
                                         KWiFiProtSetupPluginName,
                                         1 ); // we don't use synch reply

    CleanupStack::PushL( master );   
    subjects->AppendL( master );
    CleanupStack::Pop( master );


    // Disconnect dialog
    subjects->AppendL( CAknCommonNotifierWrapper::NewL( KUidDisconnectDlg,
                                                        KUidDisconnectDlg,
                                                        KMyPriority,
                                                        KDisconnectDlgName,
                                                        1 ) );

    // WLAN wrappers
    master = CAknCommonNotifierWrapper::NewL( KUidSelectWLanDlg,
                                              KUidSelectWLanDlg,
                                              KMyPriority,
                                              KConnUiUtilsNotifName,
                                              1 );
    
    CleanupStack::PushL( master );   
    subjects->AppendL( master );
    CleanupStack::Pop( master );


    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCOfflineWlanNoteDlg,
                                            KUidCOfflineWlanNoteDlg,
                                            KMyPriority ) );
    
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidCOfflineWlanDisabledNoteDlg,
                                            KUidCOfflineWlanDisabledNoteDlg,
                                            KMyPriority ) );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidEasyWepDlg,
                                            KUidEasyWepDlg,
                                            KMyPriority ) );

    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidEasyWpaDlg,
                                            KUidEasyWpaDlg,
                                            KMyPriority ) );
    
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidWLANNetworkUnavailableNoteDlg,
                                            KUidWLANNetworkUnavailableNoteDlg,
                                            KMyPriority ) );
                                            
    // ALR WRAPPERS
                                            
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                                        KUidConfirmationQuery,
                                                        KUidConfirmationQuery,
                                                        KMyPriority ) );
                                            
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                                        KUidConnectViaNote,
                                                        KUidConnectViaNote,
                                                        KMyPriority ) );
                                                        
    // WLAN wrappers
                                                        
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidWlanPowerSaveTestNote,
                                            KUidWlanPowerSaveTestNote,
                                            KMyPriority ) );
                                            
    subjects->AppendL( new( ELeave ) CAknNotifierWrapperLight( *master,
                                            KUidEasyWapiDlg,
                                            KUidEasyWapiDlg,
                                            KMyPriority ) );

    CleanupStack::Pop();    // array cleanup
    return( subjects );
    }

// ---------------------------------------------------------
// NotifierArray()
// ---------------------------------------------------------
//
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    // old Lib main entry point
    {
    CArrayPtr<MEikSrvNotifierBase2>* array = 0;
    TRAP_IGNORE( array = DoCreateNotifierArrayL() );
    return array;
    }

// ---------------------------------------------------------
// ImplementationTable
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    {{0x10207375}, ( TFuncPtr )NotifierArray}
#else
    {{0x10207375}, NotifierArray}
#endif
    };

// ---------------------------------------------------------
// ImplementationGroupProxy
// entry point
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                            TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
                  sizeof( TImplementationProxy ) ;
    return ImplementationTable;
    }


// End of File
