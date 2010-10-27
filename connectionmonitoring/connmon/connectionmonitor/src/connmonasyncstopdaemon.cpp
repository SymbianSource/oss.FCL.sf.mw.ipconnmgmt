/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object that starts a connection stop operation
*               in a new thread and waits for it to complete.
*/

#include "ConnMonIAP.h"
#include "CPsdFax.h"
#include "connmonasyncstopdaemon.h"
#include "log.h"

TAsyncStopThreadData::TAsyncStopThreadData(
        TUint32 aIapId,
        TUint32 aNetId )
        :
        iIapId( aIapId ),
        iNetId( aNetId )
    {
    iStep = 0;
    iErrorCode = 0;
    }


TInt ConnectionStopThreadFunction( TAny* aPtr )
    {
    TInt err( KErrNone );
    TAsyncStopThreadData* threadData = (TAsyncStopThreadData*)aPtr;

    RSocketServ socketServer;
    RConnection connection;
    TConnectionInfo info;

    err = socketServer.Connect( KCmESockMessageSlots );
    threadData->iErrorCode = err;
    if ( !err )
        {
        err = connection.Open( socketServer, KAfInet );
        threadData->iStep++; // Step 1
        threadData->iErrorCode = err;
        if ( !err )
            {
            info.iIapId = threadData->iIapId;
            info.iNetId = threadData->iNetId;
            err = connection.Attach(
                    TPckg<TConnectionInfo>( info ),
                    RConnection::EAttachTypeNormal );
            threadData->iStep++; // Step 2
            threadData->iErrorCode = err;
            if ( !err )
                {
                err = connection.Stop( RConnection::EStopAuthoritative );
                threadData->iStep++; // Step 3
                threadData->iErrorCode = err;
                }
            connection.Close();
            }
        socketServer.Close();
        }

    return err;
    }

CConnMonAsyncStopDaemon* CConnMonAsyncStopDaemon::NewL( CConnMonIAP* aIap )
    {
    CConnMonAsyncStopDaemon* self = CConnMonAsyncStopDaemon::NewLC( aIap );
    CleanupStack::Pop( self );
    return self;
    }

CConnMonAsyncStopDaemon* CConnMonAsyncStopDaemon::NewLC( CConnMonIAP* aIap )
    {
    CConnMonAsyncStopDaemon* self = new( ELeave ) CConnMonAsyncStopDaemon( aIap );
    CleanupStack::PushL( self );
    self->Construct();
    return self;
    }

CConnMonAsyncStopDaemon::~CConnMonAsyncStopDaemon()
    {
    LOGENTRFN("~CConnMonAsyncStopDaemon()")
    // Must not be active at this point. Can't be cancelled.

    delete iData;
    iData = NULL;

    LOGEXITFN("~CConnMonAsyncStopDaemon()")
    }

CConnMonAsyncStopDaemon::CConnMonAsyncStopDaemon( CConnMonIAP* aIap )
        :
        CActive( EConnMonPriorityHigh ),
        iIap( aIap ),
        iPsdFax( NULL ),
        iData( NULL ),
        iConnectionType( EConnMonStopTypeUnknown ),
        iConnectionId( 0 )
    {
    }

void CConnMonAsyncStopDaemon::Construct()
    {
    //LOGENTRFN("CConnMonAsyncStopDaemon::Construct()")
    CActiveScheduler::Add( this );
    //LOGEXITFN("CConnMonAsyncStopDaemon::Construct()")
    }

TInt CConnMonAsyncStopDaemon::Start( TUint aConnectionId, TUint32 aIapId, TUint32 aNetId )
    {
    LOGENTRFN("CConnMonAsyncStopDaemon::Start()")
    TInt err( KErrNone );
    iConnectionType = EConnMonStopTypeInternal;
    iConnectionId = aConnectionId;

    LOGIT3("Starting async stop daemon, id %d, iap id %d, net id %d", aConnectionId, aIapId, aNetId)
    iData = new TAsyncStopThreadData( aIapId, aNetId );
    if ( !iData )
        {
        err = KErrNoMemory;
        LOGEXITFN1("CConnMonAsyncStopDaemon::Start()", err)
        return err;
        }

    _LIT( KTempName, "connmonstop%d" );
    TBuf<KConnMonSmallBufferLen> name;
    name.Format( KTempName(), iConnectionId );

    RThread thread;
    err = thread.Create(
            name,
            ConnectionStopThreadFunction,
            KCmStopThreadStackSize,
            NULL,
            reinterpret_cast<TAny*>( iData ) );
    LOGIT1("CConnMonAsyncStopDaemon::Start(): Thread created <%d>", err)

    if ( !err )
        {
        thread.Logon( iStatus );
        SetActive();
        thread.Resume();
        }

    LOGEXITFN1("CConnMonAsyncStopDaemon::Start()", err)
    return err;
    }

TInt CConnMonAsyncStopDaemon::Start( const TUint aConnectionId, CPsdFax* aPsdFax )
    {
    LOGENTRFN("CConnMonAsyncStopDaemon::Start()")
    TInt err( KErrNone );
    iConnectionType = EConnMonStopTypeExternalPsd;
    iConnectionId = aConnectionId;
    iPsdFax = aPsdFax;

    err = iPsdFax->Stop( aConnectionId, iStatus );
    if ( !err )
        {
        SetActive();
        }

    LOGEXITFN1("CConnMonAsyncStopDaemon::Start()", err)
    return err;
    }

void CConnMonAsyncStopDaemon::DoCancel()
    {
    LOGENTRFN("CConnMonAsyncStopDaemon::DoCancel()")
    
    // The cancel process is synchronous from OS side, and can't be cancelled.
    // There could also be multiple clients waiting for the same connection stop process.
    
    LOGEXITFN("CConnMonAsyncStopDaemon::DoCancel()")
    }

// -----------------------------------------------------------------------------
// CConnMonAsyncStopDaemon::RunL
// When the separate thread closes the connection and finishes executing,
// logon-operation completes and this RunL is executed by active scheduler.
// Since this is a oneshot active object, the RunL will delete itself at the end.
// -----------------------------------------------------------------------------
//
void CConnMonAsyncStopDaemon::RunL()
    {
    LOGIT(".")
    LOGIT2("RunL: CConnMonAsyncStopDaemon, status %d, stop type %d", iStatus.Int(), iConnectionType)

    if ( iData )
        {
        LOGIT2("Thread steps done %d/3, err <%d>", iData->iStep, iData->iErrorCode)
        }

    iIap->CompleteAsyncStopReqs( iConnectionId, iStatus.Int() );

    switch ( iConnectionType )
        {
        case EConnMonStopTypeInternal:
            iIap->CleanupConnectionInfo( iConnectionId );
            break;
        case EConnMonStopTypeExternalPsd:
            iPsdFax->CleanupConnectionInfo( iConnectionId );
            break;
        default:
            // error
            break;
        }

    delete this;
    }

// End of file
