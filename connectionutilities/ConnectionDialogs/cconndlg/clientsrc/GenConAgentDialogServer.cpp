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
* Description:  Implementation of Generic Connection Dialog Client
*
*/



// INCLUDE FILES
#include "GenConAgentDialogServer.h"
#include "ConnectionDialogsUidDefs.h"
#include "ActiveLogin.h"
#include "ConnectionDialogsLogger.h"

#include <agentdialog.h>


// ---------------------------------------------------------
// ThreadFunction
//
// Required by Symbian's def file, stub
// ---------------------------------------------------------
//
EXPORT_C TInt ThreadFunction(TAny* /*aArg*/)
    {
    return 0;
    }


// ---------------------------------------------------------
// StartDialogThread
//
// Attempt to start a thread for the socket server in the C32 process.
// ---------------------------------------------------------
//
EXPORT_C TInt StartDialogThread()
    {
    return KErrNone;
    }


// ---------------------------------------------------------
// RGenConAgentDialogServer
// ---------------------------------------------------------
//
EXPORT_C RGenConAgentDialogServer::RGenConAgentDialogServer()
: RSessionBase(),
  iNotifier( NULL ),
  iIAP( 0 ),
  iModemId( 0 ),
  iLocationId( 0 ),
  iConNames( TIspConnectionNames() ),
  iPrefs( TConnectionPrefs() ),
  iBool( EFalse ),
  iPctBuffer( 0 ),
  iPctResponse( TPctResponse() ),
  iAuthenticationPair( TAuthenticationPair() ),
  iNewIapPrefsBuffer( TNewIapConnectionPrefs() ),
  iNotUsed( 0 ),
  iAccessPoint( 0 )
    {
    }


// ---------------------------------------------------------
// ~RGenConAgentDialogServer
// ---------------------------------------------------------
//
EXPORT_C RGenConAgentDialogServer::~RGenConAgentDialogServer()
    {
    delete iNotifier;
    }


// ---------------------------------------------------------
// Connect
//
// Create a session to the extended notifier framework
// ---------------------------------------------------------
//
EXPORT_C TInt RGenConAgentDialogServer::Connect()
    {
    TInt error( KErrNone );
    if ( !iNotifier )
        {
        TRAP( error, iNotifier = new RDialogNotifier() );
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
EXPORT_C void RGenConAgentDialogServer::Close()
    {
    if ( iNotifier )
        {
        iNotifier->Close();
        }
    }


// ---------------------------------------------------------
// Version
// ---------------------------------------------------------
//
EXPORT_C TVersion RGenConAgentDialogServer::Version( void ) const
    {
    return( TVersion( KConnDlgMajorVersionNumber, KConnDlgMinorVersionNumber,
                      KConnDlgBuildVersionNumber ) );
    }


// ---------------------------------------------------------
// IapConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::IapConnection( TUint32& aIAP, 
                    const TConnectionPrefs& aPrefs, TRequestStatus& aStatus)
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::IapConnection 1" );

    IapConnection( aIAP, aPrefs, KErrNone, aStatus );

    CLOG_LEAVEFN( "RGenConAgentDialogServer::IapConnection 1" );
    }


// ---------------------------------------------------------
// IapConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::IapConnection( TUint32& aIAP, 
                    const TConnectionPrefs& aPrefs, TInt /*aLastError*/, 
                    TRequestStatus& aStatus)
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::IapConnection 2" );

    ::new( &iIAP ) TPckg<TUint32>( aIAP );
    iPrefs() = aPrefs;

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidCConnDlgIap,
                                                iPrefs, iIAP );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::IapConnection 2" );
    }


// ---------------------------------------------------------
// WarnNewIapConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::WarnNewIapConnection(
                                    const TConnectionPrefs& aPrefs,
                                    TInt aLastError, const TDesC& aNewIapName, 
                                    TBool& aResponse, TRequestStatus& aStatus)
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::WarnNewIapConnection" );

    ::new( &iBool ) TPckg<TBool>( aResponse );
    ::new( &iNewIapPrefsBuffer ) TPckgBuf<TNewIapConnectionPrefs>( 
                                                    TNewIapConnectionPrefs() );

    iNewIapPrefsBuffer().iPrefs = aPrefs;
    iNewIapPrefsBuffer().iLastError = aLastError;
    iNewIapPrefsBuffer().iName = aNewIapName;

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidCConnDlgNewIap,
                                                iNewIapPrefsBuffer, iBool );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::WarnNewIapConnection" );
    }


// ---------------------------------------------------------
// Login
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::Login( TDes& aUsername, 
                                               TDes& aPassword, 
                                               TBool aIsReconnect, 
                                               TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::Login" );

    Authenticate( aUsername, aPassword, aIsReconnect, aStatus );

    CLOG_LEAVEFN( "RGenConAgentDialogServer::Login" );
    }


// ---------------------------------------------------------
// Authenticate
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::Authenticate( TDes& aUsername,
                                                      TDes& aPassword, 
                                                      TBool /*aIsReconnect*/, 
                                                      TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::Authenticate" );

    CActiveLogin* activeLogin = NULL;

    TRAPD( err, activeLogin = CActiveLogin::NewL( aUsername, aPassword ) );

    iNotUsed() = ( TUint32 )activeLogin;
    
    if ( err != KErrNone )
        {
        TRequestStatus* pS = &aStatus;
        User::RequestComplete( pS, err );
        }
    else
        {
        activeLogin->Observe( aStatus );

        TPckgBuf<TAuthenticationPairBuff>* authenticationPairBuff = 
                                                    activeLogin->GetBuffer();

        if ( iNotifier )
            {
            iNotifier->StartNotifierAndGetResponse( activeLogin->iStatus,
                                                    KUidCConnDlgAuthentication,
                                                    *authenticationPairBuff,
                                                    *authenticationPairBuff );
            }
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::Authenticate" );
    }


// ---------------------------------------------------------
// Reconnect
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::Reconnect( TBool& aBool, 
                                                   TRequestStatus& aStatus )
    {
    // Reconnect dialog has not to be showed
    aBool = EFalse;
    TRequestStatus* pS = &aStatus;
    User::RequestComplete( pS, KErrNone );
    }


// ---------------------------------------------------------
// OpenPct
// ---------------------------------------------------------
//
EXPORT_C TInt RGenConAgentDialogServer::OpenPct()
    {
    return( KErrNotSupported );
    }


// ---------------------------------------------------------
// WritePct
// ---------------------------------------------------------
//
EXPORT_C TInt RGenConAgentDialogServer::WritePct( const TDesC& /*aData*/ )
    {
    return( KErrNone );
    }


// ---------------------------------------------------------
// ReadPct
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::ReadPct( TDes& /*aData*/, 
                                                 TRequestStatus& aStatus )
    {
    TRequestStatus* s = &aStatus;
    User::RequestComplete( s, KErrNotSupported );
    }


// ---------------------------------------------------------
// DestroyPctNotification
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::DestroyPctNotification( 
                                                TRequestStatus& /*aStatus*/ )
    {
    }


// ---------------------------------------------------------
// ClosePct
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::ClosePct()
    {
    }


// ---------------------------------------------------------
// QoSWarning
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::QoSWarning( TBool& aResponse, 
                                                    TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::QoSWarning" );

    ::new( &iBool ) TPckg<TBool>( aResponse );
    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidCConnDlgQos,
                                                iBool, iBool );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::QoSWarning" );
    }


// ---------------------------------------------------------
// ModemAndLocationSelection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::ModemAndLocationSelection( 
                                                    TUint32& aModemId, 
                                                    TUint32& aLocationId, 
                                                    TRequestStatus& aStatus)
    {
    ::new( &iModemId ) TPckg<TUint32>( aModemId );
    ::new( &iLocationId ) TPckg<TUint32>( aLocationId );

    TIspConnectionNames modemLoc;
    modemLoc.iModemName.Num( ( TInt )aModemId );
    modemLoc.iLocationName.Num( ( TInt )aLocationId );
    iConNames = modemLoc;

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                                KUidCConnDlgModemAndLocation,
                                                iConNames, iConNames);
        }
    }


// ---------------------------------------------------------
// ClosePct
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelModemAndLocationSelection()
    {
    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgModemAndLocation );
        }
    }


// ---------------------------------------------------------
// CancelIapConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelIapConnection()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelIapConnection" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgIap );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelIapConnection" );
    }


// ---------------------------------------------------------
// CancelWarnNewIapConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelWarnNewIapConnection()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelWarnNewIapConnection" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgNewIap );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelWarnNewIapConnection" );
    }


// ---------------------------------------------------------
// CancelLogin
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelLogin()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelLogin" );

    CancelAuthenticate();

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelLogin" );
    }


// ---------------------------------------------------------
// CancelAuthenticate
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelAuthenticate()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelAuthenticate" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgAuthentication );
        }

    CActiveLogin* activeLogin = ( CActiveLogin* )iNotUsed();

    activeLogin->Cancel();
    delete activeLogin;

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelAuthenticate" );
    }


// ---------------------------------------------------------
// CancelReconnect
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelReconnect()
    {
    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgReconnect );
        }
    }


// ---------------------------------------------------------
// CancelReadPct
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelReadPct()
    {
    }


// ---------------------------------------------------------
// CancelDestroyPctNotification
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelDestroyPctNotification()
    {
    }


// ---------------------------------------------------------
// CancelQoSWarning
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelQoSWarning()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelQoSWarning" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgQos );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelQoSWarning" );
    }


// ---------------------------------------------------------
// AccessPointConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::AccessPointConnection( 
                                    TUint32& /*aAccessPoint*/, 
                                    TInt /*aAccessPointGroup*/, 
                                    TRequestStatus& /*aStatus*/ )
    {
    }


// ---------------------------------------------------------
// CancelAccessPointConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::CancelAccessPointConnection()
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::CancelAccessPointConnection" );

    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidCConnDlgSelectConn );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::CancelAccessPointConnection" );
    }


// ---------------------------------------------------------
// AccessPointConnection
// ---------------------------------------------------------
//
EXPORT_C void RGenConAgentDialogServer::AccessPointConnection(
                                                  TUint32& /*aAP*/, 
                                                  TUint32 aAPType, 
                                                  TUint32& aBearerAPInd, 
                                                  TUint32 aBearerAPType, 
                                                  TRequestStatus& aStatus )
    {
    CLOG_ENTERFN( "RGenConAgentDialogServer::AccessPointConnection" );

    ::new( &iIAP ) TPckg<TUint32>( aBearerAPInd );
    ::new( &iPrefs ) TPckgBuf<TConnectionPrefs>( TConnectionPrefs() );

    // Using the unused iRank to pass the ElementId
    iPrefs().iRank = aBearerAPInd;
    iPrefs().iBearerSet = aBearerAPType;
    
    // By setting aAPType to 1 application can define that
    // dialog is displayed even though there was a user connection.
    if ( aAPType == 1 )
        {
    	iPrefs().iDirection = ECommDbConnectionDirectionOutgoing;
        }
    else
        {
    	iPrefs().iDirection = ECommDbConnectionDirectionUnknown;
        }

    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, 
                                                KUidCConnDlgSelectConn,
                                                iPrefs, iIAP );
        }

    CLOG_LEAVEFN( "RGenConAgentDialogServer::AccessPointConnection" );
    }


// ---------------------------------------------------------
// Connect
//
// Connect to the  server
// ---------------------------------------------------------
//
TInt RDialogNotifier::Connect()
    {
    TInt ret = RNotifier::Connect();
    return ret;
    }


// End of File
