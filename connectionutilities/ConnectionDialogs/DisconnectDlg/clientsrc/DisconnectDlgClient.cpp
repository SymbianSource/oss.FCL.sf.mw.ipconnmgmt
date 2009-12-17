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
* Description:  Implementation of Disconnect Dialog Client
*
*/



// INCLUDE FILES
#include <bautils.h>

#include <DisconnectDlgClient.h>
#include "DisconnectDlgUi.h"
#include "ConnectionDialogsUidDefs.h"
#include "e32ver.h"

#include "ConnectionDialogsLogger.h"


// CONSTANTS


// ---------------------------------------------------------
// RDisconnectDlgServer
// ---------------------------------------------------------
//
EXPORT_C RDisconnectDlgServer::RDisconnectDlgServer()
: RSessionBase(), 
  iNotifier( NULL ), 
  iPassedInfo( TDisconnectConnectionPrefs() ),
  iIapId( 0 )
    {
    }


// ---------------------------------------------------------
// ~RDisconnectDlgServer
// ---------------------------------------------------------
//
EXPORT_C RDisconnectDlgServer::~RDisconnectDlgServer()
    {
    }


// ---------------------------------------------------------
// Connect
//
// Create a session to the extended notifier framework
// ---------------------------------------------------------
//
EXPORT_C TInt RDisconnectDlgServer::Connect()
    {
    TInt error( KErrNone );

    if ( !iNotifier )
        {
        TRAP( error, iNotifier = new RNotifier() );
        }

    if ( error == KErrNone && iNotifier )
        {
        error = iNotifier->Connect();
        }

    return error;
    }


// ---------------------------------------------------------
// Close
// ---------------------------------------------------------
//
EXPORT_C void RDisconnectDlgServer::Close()
    {
    if ( iNotifier )
        {
        iNotifier->Close();
        }

    delete iNotifier;
    iNotifier = NULL;
    }


// ---------------------------------------------------------
// Version
// ---------------------------------------------------------
//
EXPORT_C TVersion RDisconnectDlgServer::Version( void ) const
    {
    return( TVersion( KDisconnectDlgMajorVersionNumber, 
                      KDisconnectDlgMinorVersionNumber,
                      KDisconnectDlgBuildVersionNumber ) );
    }


// ---------------------------------------------------------
// DisconnectDlg
// ---------------------------------------------------------
//
EXPORT_C void RDisconnectDlgServer::DisconnectDlg( TRequestStatus& aStatus, 
                                       const TBool aShowQuery /* = EFalse */ )
    {
    CLOG_ENTERFN("RDisconnectDlgServer::DisconnectDlg");
    
    TDisconnectConnectionPrefs connectionPrefs;
    
    connectionPrefs.iPrompt = aShowQuery;
    iPassedInfo() = connectionPrefs;
    
    if ( iNotifier )
        {
        iNotifier->StartNotifierAndGetResponse( aStatus, KUidDisconnectDlg,
                                                iPassedInfo, iIapId );
        }

    CLOG_WRITEF(_L( "aStatus: %d" ), aStatus.Int() );     
    CLOG_LEAVEFN("RDisconnectDlgServer::DisconnectDlg");
    }

    
// ---------------------------------------------------------
// DisconnectDlg
// ---------------------------------------------------------
//
EXPORT_C void RDisconnectDlgServer::DisconnectDlg( TRequestStatus& aStatus, 
                                       TUint32& aIapId,
                                       const TBool aShowQuery /* = EFalse */ )
    {
    CLOG_ENTERFN("RDisconnectDlgServer::DisconnectDlg 3");
    ::new( &iIapId ) TPckg<TUint32>( aIapId );        
    DisconnectDlg( aStatus, aShowQuery );
    CLOG_LEAVEFN("RDisconnectDlgServer::DisconnectDlg 3");        
    }
    

// ---------------------------------------------------------
// CancelDisconnectDlg
// ---------------------------------------------------------
//
EXPORT_C void RDisconnectDlgServer::CancelDisconnectDlg()
    {
    if ( iNotifier )
        {
        iNotifier->CancelNotifier( KUidDisconnectDlg );
        }
    }

// End of File
