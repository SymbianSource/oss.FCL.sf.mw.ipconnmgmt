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
* Description:  Implementation of RConnectionUiUtilitiesSession
*
*/



// INCLUDE FILES
#include "ConnectionUiUtilitiesClient.h"
#include "ConnectionUiUtilitiesCommon.h"
#include "e32ver.h"
#include "ConnectionDialogsUidDefs.h"
#include "ConnectionDialogsLogger.h"


// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::RConnectionUiUtilitiesSession()
// ---------------------------------------------------------
//
RConnectionUiUtilitiesSession::RConnectionUiUtilitiesSession()
: RSessionBase(),
  iNotifier( NULL ),
  iBool( EFalse ),
  iResponseMsgQuery( EMsgQueryCancelled ),
  iPassedInfo( TConnUiUiDestConnMethodNoteId() )
    {
    }

// ---------------------------------------------------------
// ~RConnectionUiUtilitiesSession
// ---------------------------------------------------------
//
RConnectionUiUtilitiesSession::~RConnectionUiUtilitiesSession()
    {
    delete iNotifier;
    }

// ---------------------------------------------------------
// Connect
//
// Create a session to the extended notifier framework
// ---------------------------------------------------------
//
TInt RConnectionUiUtilitiesSession::Connect()
    {
    TInt error( KErrNone );
    if ( !iNotifier )
        {
        TRAP( error, iNotifier = new RNotifier() );
        }
    if ( !error && iNotifier )
        {
        error = iNotifier->Connect();
        } 
    return error;
    }

// ---------------------------------------------------------
// Close
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::Close()
    {
    if ( iNotifier )
        {
        iNotifier->Close();
        }
    }

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::SearchWLANNetwork()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::SearchWLANNetwork(
                            TPckgBuf<TConnUiUiWlanNetworkPrefs>& aNetworkPrefs,
                            TRequestStatus& aStatus,
                            TBool aIsAsyncVersion )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::SearchWLANNetwork" );

    if ( iNotifier )
        {
        ::new( &iBool ) TPckg<TBool>( aIsAsyncVersion );
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidSelectWLanDlg,
                                                iBool, aNetworkPrefs );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::SearchWLANNetwork" );
    }

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelSearchWLANNetwork()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelSearchWLANNetwork()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelSearchWLANNetwork" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidSelectWLanDlg );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelSearchWLANNetwork" );
    }
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::OffLineWlanNote()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::OffLineWlanNote( TRequestStatus& aStatus,
                                                     TBool aIsAsyncVersion )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::OffLineWlanNote" );

    if ( iNotifier )
        {
        ::new( &iBool ) TPckg<TBool>( aIsAsyncVersion );
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                                KUidCOfflineWlanNoteDlg, 
                                                iBool, 
                                                iResponseStrOffLineWlanNote );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::OffLineWlanNote" );
    }    
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelOffLineWlanNote()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelOffLineWlanNote()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelOffLineWlanNote" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCOfflineWlanNoteDlg );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelOffLineWlanNote" );
    }    

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::OffLineWlanDisabledNote()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::OffLineWlanDisabledNote(
                                                     TRequestStatus& aStatus )
    {    
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::OffLineWlanDisabledNote" );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                        KUidCOfflineWlanDisabledNoteDlg, 
                                        KNullDesC8(), 
                                        iResponseStrOffLineWlanDisabledNote );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::OffLineWlanDisabledNote" );
    }

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::EasyWepDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::EasyWepDlg( TPckgBuf< TWepKeyData >& aKey,
                                                TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::EasyWepDlg" );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidEasyWepDlg,
                                                aKey, aKey );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::EasyWepDlg" );
    }    
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelEasyWepDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelEasyWepDlg()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelEasyWepDlg" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidEasyWepDlg );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelEasyWepDlg" );
    }    

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::EasyWpaDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::EasyWpaDlg( 
                            TPckgBuf< TBuf< KEasyWpaQueryMaxLength > >& aKey,
                            TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::EasyWpaDlg" );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidEasyWpaDlg,
                                                aKey, aKey );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::EasyWpaDlg" );
    }    
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelEasyWpaDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelEasyWpaDlg()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelEasyWpaDlg" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidEasyWpaDlg );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelEasyWpaDlg" );
    }    

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::WLANNetworkUnavailableNote()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::WLANNetworkUnavailableNote(
                                                     TRequestStatus& aStatus )
    {    
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::WLANNetworkUnavailableNote" );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                    KUidWLANNetworkUnavailableNoteDlg, 
                                    KNullDesC8(), 
                                    iResponseStrWLANNetworkUnavailableNote );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::WLANNetworkUnavailableNote" );
    }
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::ConfirmMethodUsageQuery()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::ConfirmMethodUsageQuery( 
                                               TMsgQueryLinkedResults& aResult,
                                               TBool aHomeNetwork,
                                               TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::ConfirmMethodUsageQuery" );

    ::new( &iResponseMsgQuery ) TPckg<TMsgQueryLinkedResults>( aResult );    
    ::new( &iPassedInfo ) TPckgBuf<TConnUiUiDestConnMethodNoteId>( 
                                            TConnUiUiDestConnMethodNoteId() );
    
    iPassedInfo().iNoteId = EConfirmMethodUsageQuery;
    
    // Set the dialog type depending on the location
    if ( aHomeNetwork )
        {
        iPassedInfo().iNoteId = EConfirmMethodUsageQueryInHomeNetwork; 
        }
    else
        {
        iPassedInfo().iNoteId = EConfirmMethodUsageQueryInForeignNetwork; 
        }
    
    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidConfirmationQuery,
                                            iPassedInfo, iResponseMsgQuery );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::ConfirmMethodUsageQuery" );
    }    
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelConfirmMethodUsageQuery()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelConfirmMethodUsageQuery()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelConfirmMethodUsageQuery" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidConfirmationQuery );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelConfirmMethodUsageQuery" );
    }    

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CWlanPowerSaveTestNote
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::WlanPowerSaveTestNote( 
                                                    TBool&          aDisable,
                                                    TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::WlanPowerSaveTestNote" );

    ::new( &iBool ) TPckg< TBool >( aDisable );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                                KUidWlanPowerSaveTestNote,
                                                KNullDesC8(),
                                                iBool );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::WlanPowerSaveTestNote" );
    }
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelWlanPowerSaveTestNote
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelWlanPowerSaveTestNote()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelWlanPowerSaveTestNote" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidWlanPowerSaveTestNote );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelWlanPowerSaveTestNote" );
    }

// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::EasyWapiDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::EasyWapiDlg( 
                            TPckgBuf< TBuf< KEasyWapiQueryMaxLength > >& aKey,
                            TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::EasyWapiDlg" );

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidEasyWapiDlg,
                                                aKey, aKey );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::EasyWapiDlg" );
    }    
    
// ---------------------------------------------------------
// RConnectionUiUtilitiesSession::CancelEasyWapiDlg()
// ---------------------------------------------------------
//
void RConnectionUiUtilitiesSession::CancelEasyWapiDlg()
    {
    CLOG_ENTERFN( "RConnectionUiUtilitiesSession::CancelEasyWapiDlg" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidEasyWapiDlg );
        }

    CLOG_LEAVEFN( "RConnectionUiUtilitiesSession::CancelEasyWapiDlg" );
    }

// End of File
