/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Issues RConnection::DataTransferredRequest() and sends the
*                data volumes to sessions.
*
*/

#include <es_sock.h>

#include "CDataVolume.h"
#include "ConnMonServ.h"
#include "ConnMonDef.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataVolume::CDataVolume
// -----------------------------------------------------------------------------
//
CDataVolume::CDataVolume(
        CConnMonServer* aServer,
        RConnection* aConnection,
        const TUint& aConnectionId )
        :
        CActive( EConnMonPriorityNormal ),
        iServer( aServer ),
        iConnection( aConnection ),
        iConnectionId( aConnectionId ),
        iDlData( 0 ),
        iPckgDlData( iDlData ),
        iUlData( 0 ),
        iPckgUlData( iUlData )
    {
    }

// -----------------------------------------------------------------------------
// CDataVolume::Construct
// -----------------------------------------------------------------------------
//
void CDataVolume::Construct()
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CDataVolume::~CDataVolume()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CDataVolume::Receive
// Requests a new event (activity changed) from RConnection
// -----------------------------------------------------------------------------
//
void CDataVolume::Receive()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    iUlData = 0;
    iDlData = 0;

    iConnection->DataTransferredRequest( iPckgUlData, iPckgDlData, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CDataVolume::DoCancel
// Cancels the request from RConnection.
// -----------------------------------------------------------------------------
//
void CDataVolume::DoCancel()
    {
    if ( IsActive() )
        {
        // Complete all outstanding data volume requests on this connection
        // Use the latest data volumes got from the ESOCK.
        iServer->SendDataVolumesToSessions(
                iConnectionId,
                iPckgDlData(),
                iPckgUlData(),
                KErrCancel );

        // Cancel the request from RConnection
        iConnection->DataTransferredCancel();
        }
    }

// -----------------------------------------------------------------------------
// CDataVolume::RunL
// Handles the event that has arrived from RConnection
// -----------------------------------------------------------------------------
//
void CDataVolume::RunL()
    {
    //LOGENTRFN("CDataVolume::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CDataVolume, status %d", iStatus.Int())

    if ( iStatus.Int() != KErrNone )
        {
        LOGIT1("SERVER: Data volume request FAILED <%d>", iStatus.Int())
        }
    else
        {
        LOGIT2("SERVER: Data volume request completed: d:%d, u:%d", iPckgDlData(), iPckgUlData())
        }

    // Complete all outstanding data volume requests on this connection
    iServer->SendDataVolumesToSessions(
            iConnectionId,
            iPckgDlData(),
            iPckgUlData(),
            iStatus.Int() );

    // This is a one-shot active object. The object is connection specific,
    // constructed when needed for the first time, and reused as necessary.
    // Delete happens only when the connection specific attribute tables are
    // cleaned up.

    //LOGEXITFN("CDataVolume::RunL()")
    }

// End-of-file
