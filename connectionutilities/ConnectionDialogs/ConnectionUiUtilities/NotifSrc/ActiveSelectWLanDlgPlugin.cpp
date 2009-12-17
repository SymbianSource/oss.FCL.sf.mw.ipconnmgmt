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
* Description:  Implementation of CActiveSelectWLanDlgPlugin.
*
*/


// INCLUDE FILES
#include <AknWaitDialog.h>
#include <ConnUiUtilsNotif.rsg>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <AknSgcc.h>
#include <AknCapServerClient.h>
//#include <aknnotewrappers.h>

#include "ActiveSelectWLanDlgPlugin.h"
#include "SelectWLANDlg.h"
#include "ConnectionDialogsLogger.h"
#include "ConnectionUiUtilitiesPrivateCRKeys.h"


// CONSTANTS

/**
* For iPeriodic Timer, 10 seconds
* The interval between refreshing available wlan networks.
*/
LOCAL_D const TInt KTickDefaultInterval = 10000000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin* CActiveSelectWLanDlgPlugin::NewL()
// ---------------------------------------------------------
//
CActiveSelectWLanDlgPlugin* CActiveSelectWLanDlgPlugin::NewL( 
                                              CSelectWLanDlgPlugin* aPlugin )
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::NewL " );  
    
    CActiveSelectWLanDlgPlugin* self = 
                         new ( ELeave ) CActiveSelectWLanDlgPlugin( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::NewL " );      

    return self;        
    }

// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::ConstructL()
// ---------------------------------------------------------
//
void CActiveSelectWLanDlgPlugin::ConstructL()
    {     
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::ConstructL " );
       
    CActiveScheduler::Add( this );            
    iWlanDataProv = CWlanNetworkDataProvider::NewL();
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::ConstructL " );
    }

// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::CActiveSelectWLanDlgPlugin()
// ---------------------------------------------------------
//
CActiveSelectWLanDlgPlugin::CActiveSelectWLanDlgPlugin( 
                                                CSelectWLanDlgPlugin* aPlugin )
: CActive( EPriorityStandard ), 
  iPlugin( aPlugin )
    {
    }
    
// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::~CActiveSelectWLanDlgPlugin()
// ---------------------------------------------------------
//    
CActiveSelectWLanDlgPlugin::~CActiveSelectWLanDlgPlugin()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::~CActiveSelectWLanDlgPlugin " );
    Cancel();

    StopTimer();
    DestroyWaitDialog();

    if ( iResultsListDialog  && !( iResultsListDialog->GetOkToExit() ) )
        {
        CLOG_WRITEF( _L( "OkToExit %b" ), iResultsListDialog->GetOkToExit() );
        delete iResultsListDialog;
        iResultsListDialog = NULL;
        }
    
    delete iWlanDataProv;

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::~CActiveSelectWLanDlgPlugin " );
    }


// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::DoCancel()
// ---------------------------------------------------------
// 
void CActiveSelectWLanDlgPlugin::DoCancel()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::DoCancel " );

    CLOG_WRITE( "stopping timer" );
    StopTimer();

    CLOG_WRITE( "deleting results list dialog" );
    DestroyResultsListDialog();

    CLOG_WRITE( "deleting wait dialog" );
    DestroyWaitDialog();

    CLOG_WRITE( "deleting wlan data provider" );
    delete iWlanDataProv;
    iWlanDataProv = NULL;

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::DoCancel " );
    }
    
// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::StartTimerL
// ---------------------------------------------------------
//    
void CActiveSelectWLanDlgPlugin::StartTimerL( TInt aTickInterval )
    {    
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::StartTimerL " );  
    
    if( !iPeriodic )
        {
        iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard ); 
        }
        
    iPeriodic->Start( aTickInterval, aTickInterval, TCallBack( Tick, this ) );
    
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::StartTimerL " );
    }


// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::StopTimer
// ---------------------------------------------------------
//
void CActiveSelectWLanDlgPlugin::StopTimer()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::StopTimer " );  
    
    if( iPeriodic )
        {
        CLOG_WRITE( "iPeriodic" );
        iPeriodic->Cancel();
        CLOG_WRITE( "Cancel" );
        
        delete iPeriodic;
        CLOG_WRITE( "delete" );
        
        iPeriodic = NULL;
        }        
    
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::StopTimer " );
    }
    
// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::DoTick
// ---------------------------------------------------------
//
void CActiveSelectWLanDlgPlugin::DoTick()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::DoTick " );

    if ( IsActive() == EFalse )
        {
        CLOG_WRITE( "not active anymore - making another scan request..." );

        SetActive();

#ifdef __WINS__
        TRequestStatus* clientStatus = &iStatus;
        User::RequestComplete( clientStatus, KErrNone );
#else
        iWlanDataProv->RefreshScanInfo( iStatus );
#endif
        }
    }

// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::Tick
// ---------------------------------------------------------
//
TInt CActiveSelectWLanDlgPlugin::Tick( TAny* aObject )
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::Tick " );

    CActiveSelectWLanDlgPlugin* myself =
                        static_cast<CActiveSelectWLanDlgPlugin*>( aObject );
    myself->DoTick();

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::Tick " );

    return 1;
    }


// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::RunL()
// ---------------------------------------------------------
//     
void CActiveSelectWLanDlgPlugin::RunL()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::RunL " );  
    
    if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();     
        delete iWaitDialog;
        iWaitDialog = NULL;    
        }
    
    CLOG_WRITEF( _L( "iStatus.Int() : %d" ), iStatus.Int() );
    
    if( iStatus == KErrNone )
        {             
        iWlanDataProv->OnTimerExpiredL();
        
        CNetworkInfoArray* *const networkArray = 
                                   iWlanDataProv->GetNetworkInfoArray();
         
        const TInt resultsCount = ( *networkArray )->MdcaCount();

        CLOG_WRITEF( _L( "resultsCount : %d" ),resultsCount );

        if ( resultsCount > 0 )
            {
            if( iResultsListDialog )
                {
                iResultsListDialog->RefreshDialogL();
                }
            else
                {
                iResultsListDialog = CSelectWLANDlg::NewL( iPlugin, 
                                                           networkArray );

                iResultsListDialog->ExecuteLD( R_SELECT_WLAN_LIST_QUERY );

                TInt interval = GetRefreshInterval();
                CLOG_WRITEF( _L( "interval : %d" ),  interval );

                if( interval )
                    {
                    StartTimerL( interval );
                    }
                }
            }
        else
            {
            StopTimer();
            
            DestroyWaitDialog();
            DestroyResultsListDialog();

            HBufC* stringLabel = StringLoader::LoadLC( 
                                                    R_INFO_NO_NETWORKS_FOUND );
/*** Deadlock                                              
            CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
            globalNote->ShowNoteL( EAknGlobalInformationNote, *stringLabel );

            CleanupStack::PopAndDestroy( globalNote );
            CleanupStack::PopAndDestroy( stringLabel );
***/

//// 3.1 solution: prevent deadlock, does not work in 3.0 because of avkon 
            RAknUiServer* globalNote = CAknSgcClient::AknSrv();
            if ( globalNote->Handle() )
                {
                globalNote->ShowGlobalNoteL( *stringLabel, 
                                            EAknGlobalInformationNote );
                }
            CleanupStack::PopAndDestroy( stringLabel );

//// End of 3.1 solution

/*** 3.0 solution
            CAknInformationNote* myLocalGlobalNote = new ( ELeave ) 
                                                CAknInformationNote( EFalse );
            myLocalGlobalNote->ExecuteLD( *stringLabel );
            CleanupStack::PopAndDestroy( stringLabel );

*** End of 3.0 solution    ***/
            
            iPlugin->SetCancelledFlag( ETrue );
            iPlugin->CompleteL( KErrNotFound );            
            }
        }
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::RunL " );      
        
    }
    
// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::StartSearchWlanNetworkL()
// ---------------------------------------------------------
//    
void CActiveSelectWLanDlgPlugin::StartSearchWlanNetworkL()
    {    
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::StartSearchWlanNetworkL " );  
    
    DestroyWaitDialog();

    iWaitDialog = new( ELeave )CAknWaitDialog
                    ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ), ETrue );
    iWaitDialog->ExecuteLD( R_SEARCHING_WLAN_WAIT_NOTE ); 
    
    SetActive();    

#ifdef __WINS__
    TRequestStatus* clientStatus = &iStatus;
    User::RequestComplete( clientStatus, KErrNone );
#else
    iWlanDataProv->RefreshScanInfo( iStatus );
#endif

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::StartSearchWlanNetworkL " );
    }
    
// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::GetRefreshInterval()
// ---------------------------------------------------------
//    
TInt CActiveSelectWLanDlgPlugin::GetRefreshInterval()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::GetRefreshInterval " );

    CRepository* repository = NULL;
    TInt err( KErrNone );

    TRAP( err, 
          repository = CRepository::NewL( KCRUidConnectionUiUtilities ) );
          
    TInt variant( 0 );  
    CLOG_WRITEF( _L( "err : %d" ), err );
      
    if ( err == KErrNone )
        {
        TInt retval = repository->Get( KConnectionUiUtilitiesScanInterval, 
                                       variant ); 
                                       
        CLOG_WRITEF( _L( "retval : %d" ), retval );
                                                                             
        if ( retval == KErrNotFound )
            {
            variant = KTickDefaultInterval;
            }
        }
     else
        {
        variant = KTickDefaultInterval;
        }

    delete repository;

    CLOG_WRITEF( _L( "variant : %d" ), variant );
    
    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::GetRefreshInterval " );
    
    return variant;            
    }


// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::DestroyWaitDialog()
// ---------------------------------------------------------
//
void CActiveSelectWLanDlgPlugin::DestroyWaitDialog()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::DestroyWaitDialog " );

    delete iWaitDialog;
    iWaitDialog = NULL;

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::DestroyWaitDialog " );
    }


// ---------------------------------------------------------
// CActiveSelectWLanDlgPlugin::DestroyResultsListDialog()
// ---------------------------------------------------------
//
void CActiveSelectWLanDlgPlugin::DestroyResultsListDialog()
    {
    CLOG_ENTERFN( "CActiveSelectWLanDlgPlugin::DestroyResultsListDialog " );

    if ( iResultsListDialog  && !( iResultsListDialog->GetOkToExit() ) )
        {
        delete iResultsListDialog;
        iResultsListDialog = NULL;
        }

    CLOG_LEAVEFN( "CActiveSelectWLanDlgPlugin::DestroyResultsListDialog " );
    }

// End of File

