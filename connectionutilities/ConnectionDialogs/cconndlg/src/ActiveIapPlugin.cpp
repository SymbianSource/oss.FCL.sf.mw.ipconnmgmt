/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "ActiveIapPlugin.h"
#include "IAPDialog.h"
#include "ConnectionInfo.h"
#include "ConnectionInfoArray.h"
#include "CConnDlgPrivateCRKeys.h"
#include "ConnectionDialogsLogger.h"

#include <commdb.h>
#include <StringLoader.h>
#include <wlanmgmtcommon.h>
#ifndef __WINS__
#include <wlanmgmtclient.h>
#endif
#include <AknWaitDialog.h>

#include <CConnDlgPlugin.rsg>


// CONSTANTS

// Panic string
#if defined(_DEBUG)
_LIT( KErrNullPointer, "NULL pointer" );
#endif


///////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::CActiveCConnDlgIapPlugin()
// ---------------------------------------------------------
//
CActiveCConnDlgIapPlugin::CActiveCConnDlgIapPlugin( 
                                            CConnDlgIapPlugin* aPlugin,
                                            const TConnectionPrefs& aPrefs )
: CActiveIAPListing( aPrefs ),
  iPlugin( aPlugin ), 
  isDialogDeleting( EFalse )
    {
    }

    
// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin* CActiveCConnDlgIapPlugin::NewL()
// ---------------------------------------------------------
//
CActiveCConnDlgIapPlugin* CActiveCConnDlgIapPlugin::NewL( 
                                              CConnDlgIapPlugin* aPlugin,              
                                              const TConnectionPrefs& aPrefs )
    {
    CLOG_ENTERFN( "CActiveCConnDlgIapPlugin::NewL " );  
    
    CActiveCConnDlgIapPlugin* self = 
                         new ( ELeave ) CActiveCConnDlgIapPlugin( aPlugin,                                                                  
                                                                  aPrefs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CLOG_LEAVEFN( "CActiveCConnDlgIapPlugin::NewL " );      

    return self;        
    }


// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::ConstructL()
// ---------------------------------------------------------
//
void CActiveCConnDlgIapPlugin::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::~CActiveCConnDlgIapPlugin()
// ---------------------------------------------------------
//    
CActiveCConnDlgIapPlugin::~CActiveCConnDlgIapPlugin()
    {
    CLOG_ENTERFN( "CActiveCConnDlgIapPlugin::~CActiveCConnDlgIapPlugin" );      

    Cancel();

    CLOG_LEAVEFN( "CActiveCConnDlgIapPlugin::~CActiveCConnDlgIapPlugin" );      
    }


// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::Cancel()
// ---------------------------------------------------------
// 
void CActiveCConnDlgIapPlugin::Cancel()
    {
    CLOG_ENTERFN( "CActiveCConnDlgIapPlugin::Cancel" );      

    CActiveIAPListing::Cancel();

    CLOG_WRITE( "Returned from CActiveIAPListing::Cancel" );

    if( iDialog  && !( iDialog->GetOkToExit() ) )
        {
        CLOG_WRITEF( _L( "iDialog->GetOkToExit() : %b" ), iDialog->GetOkToExit() );
        delete iDialog;
        iDialog = NULL;
        }

    CLOG_LEAVEFN( "CActiveCConnDlgIapPlugin::Cancel" );      
    }


// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::DoCancel()
// ---------------------------------------------------------
// 
void CActiveCConnDlgIapPlugin::DoCancel()
    {
    CLOG_ENTERFN( "CActiveCConnDlgIapPlugin::DoCancel " );  
    
    if( isDialogDeleting )
        {
        CLOG_WRITE( "isDialogDeleting" );
        
        delete iDialog;
        iDialog = NULL;   
        
        isDialogDeleting = EFalse;         
        }
        
    CLOG_LEAVEFN( "CActiveCConnDlgIapPlugin::DoCancel " );          
    }

    
// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::RunL()
// ---------------------------------------------------------
//     
void CActiveCConnDlgIapPlugin::RunL()
    {
    CLOG_ENTERFN( "CActiveCConnDlgIapPlugin::RunL " );  
    
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
                iDialog->RefreshDialogL( iActIAPs, ETrue );
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

                iDialog = CIapDialog::NewL( iPlugin );
                iDialog->PrepareLC( R_LIST_QUERY );
                iDialog->RefreshDialogL( iActIAPs, EFalse );
                iDialog->RunLD();

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
            CreateArraysL( R_WLAN_SEARCH_FOR_NETWORKS, EFalse, 0 );
            }            
        }     
    
    CLOG_LEAVEFN( "CActiveCConnDlgIapPlugin::RunL " );      
        
    }

    
// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::StartSearchIAPsL()
// ---------------------------------------------------------
//    
void CActiveCConnDlgIapPlugin::StartSearchIAPsL()
    {    
    CActiveIAPListing::BaseStartSearchIAPsL( R_SEARCHING_WAIT_NOTE );
    }
   
 
// ---------------------------------------------------------
// CActiveCConnDlgIapPlugin::GetRefreshInterval()
// ---------------------------------------------------------
//    
TInt CActiveCConnDlgIapPlugin::GetRefreshInterval()
    {
    return CActiveIAPListing::GetRefreshInterval( KCRUidCConnDlg,
                                                  KCConnDlgScanInterval );
    }
            
   
// End of File
