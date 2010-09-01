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
* Description: 
*     Defines dialog CActiveCChangeConnectionDlg from Connection Ui Utilities
*
*/


// INCLUDE FILES

#include <StringLoader.h>
#include <commdb.h>

#ifndef __WINS__
#include <wlanmgmtclient.h>
#endif

#include <ConnectionUiUtilities.rsg>

#include "ActiveCChangeConnectionDlg.h"
#include "ChangeConnectionDlg.h"
#include "ConnectionUiUtilitiesPrivateCRKeys.h"
#include "ConnectionDialogsLogger.h"
#include "ConnectionInfoKey.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveCChangeConnectionDlg* CActiveCChangeConnectionDlg::NewL()
// ---------------------------------------------------------
//
CActiveCChangeConnectionDlg* CActiveCChangeConnectionDlg::NewL( 
                                              TUint32* aIAPId, 
                                              const TConnectionPrefs& aPrefs,
                                              TDes& aConnectionName  )
    {
    CLOG_ENTERFN( "CActiveCChangeConnectionDlg::NewL " );  
    
    CActiveCChangeConnectionDlg* self = 
                         new ( ELeave ) CActiveCChangeConnectionDlg( aIAPId,
                                                             aPrefs, 
                                                             aConnectionName );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CActiveCChangeConnectionDlg::NewL " );      

    return self;        
    }

// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::ConstructL()
// ---------------------------------------------------------
//
void CActiveCChangeConnectionDlg::ConstructL()
    {     
    BaseConstructL();
    }

// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::CActiveCChangeConnectionDlg()
// ---------------------------------------------------------
//
CActiveCChangeConnectionDlg::CActiveCChangeConnectionDlg(
                                            TUint32* aIAPId, 
                                            const TConnectionPrefs& aPrefs,
                                            TDes& aConnectionName  ) 
: CActiveIAPListing( aPrefs ), 
  isDialogDeleting( EFalse ),
  iIAPId( aIAPId ),
  iConnectionName( aConnectionName ),
  iSelected( EFalse )
    {
    }
    
// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::~CActiveCChangeConnectionDlg()
// ---------------------------------------------------------
//    
CActiveCChangeConnectionDlg::~CActiveCChangeConnectionDlg()
    {
    }


// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::Cancel()
// ---------------------------------------------------------
// 
void CActiveCChangeConnectionDlg::Cancel()
    {
    CActiveIAPListing::Cancel();

    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();            
        }                 
    }


// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::DoCancel()
// ---------------------------------------------------------
// 
void CActiveCChangeConnectionDlg::DoCancel()
    {
    CLOG_ENTERFN( "CActiveCChangeConnectionDlg::DoCancel " );  
    
    if( isDialogDeleting )
        {
        CLOG_WRITE( "isDialogDeleting" );
        
        delete iDialog;
        iDialog = NULL;   
        
        isDialogDeleting = EFalse;         
        }
        
    CLOG_LEAVEFN( "CActiveCChangeConnectionDlg::DoCancel " );          
    }

    
// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::RunL()
// ---------------------------------------------------------
//     
void CActiveCChangeConnectionDlg::RunL()
    {
    CLOG_ENTERFN( "CActiveCChangeConnectionDlg::RunL " );  
    
    CLOG_WRITEF( _L( "iStatus.Int() : %d" ), iStatus.Int() );
    CLOG_WRITEF( _L( "&iStatus : %d" ), &iStatus );
    
    if( iStatus == KErrNone )
        {
        
        if( iIAPs ) // if iIAPs has been created
            {            
            // merge arrays and sort
            MergeArraysL( R_WLAN_SEARCH_FOR_NETWORKS );
            
            if ( iDialog )  // if dialog has been created
                {
                iDialog->RefreshDialogL();                    
                }
            else
                {
                
                TInt interval = 0;
                if ( iIsWLANFeatureSupported )
                    {                                    
                    interval = GetRefreshInterval(); 
                    CLOG_WRITEF( _L( "interval : %d" ),  interval );

                    if ( iWaitDialog )
                        {
                        iWaitDialog->ProcessFinishedL();     
                        delete iWaitDialog;
                        iWaitDialog = NULL;    
                        }
                     }

                iDialog = CChangeConnectionDlg::NewL( iIAPId, this,
                                                      iConnectionName );
                iDialog->ExecuteLD( R_CHANGE_CONN_LIST_QUERY );

                if ( iIsWLANFeatureSupported )
                    {                                           
                    if( interval )
                        {
                        StartTimerL( interval );
                        }     
                    }
                }   // else
            } // if iIAPs
        else
            {            
            CreateArraysL( R_WLAN_SEARCH_FOR_NETWORKS, ETrue, *iIAPId );
            }            
        }     
    else
        {
        Cancel();
        CLOG_WRITEF( _L( "RUNL Cancelled iStatus : %d" ), iStatus.Int() );
        }
    
    CLOG_LEAVEFN( "CActiveCChangeConnectionDlg::RunL " );      
        
    }
    
// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::StartSearchIAPsL()
// ---------------------------------------------------------
//    
TBool CActiveCChangeConnectionDlg::StartSearchIAPsL()
    {    
    //if ( IsActive() == EFalse )
        {
        CActiveIAPListing::BaseStartSearchIAPsL( R_SEARCHING_WAIT_NOTE );

        CLOG_WRITEF( _L( "iWait.IsStarted() : %b" ),  iWait.IsStarted() );
        iWait.Start();
        }
        
    return iSelected;           
    }
    
// ---------------------------------------------------------
// CActiveCChangeConnectionDlg::GetRefreshInterval()
// ---------------------------------------------------------
//    
TInt CActiveCChangeConnectionDlg::GetRefreshInterval()
    {
    return CActiveIAPListing::GetRefreshInterval( KCRUidConnectionUiUtilities,
                                    KConnectionUiUtilitiesChangeConnInterval );
    }
            
    
// End of File
