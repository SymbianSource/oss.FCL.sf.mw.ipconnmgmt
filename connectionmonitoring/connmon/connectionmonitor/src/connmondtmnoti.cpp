/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConnMon Dual Transfer Mode watcher.
*
*/

#include "connmondtmnoti.h"
#include "ConnMonDef.h"
#include "log.h"

#include <pcktcs.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::NewL
// -----------------------------------------------------------------------------
//
CConnMonDtmNotifier* CConnMonDtmNotifier::NewL(
        RPacketService& aPacketService,
        MConnMonDtmNotifierObserver& aObserver )
    {
    CConnMonDtmNotifier* self = new( ELeave ) CConnMonDtmNotifier(
            aPacketService,
            aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::CConnMonDtmNotifier
// -----------------------------------------------------------------------------
//
CConnMonDtmNotifier::CConnMonDtmNotifier(
        RPacketService& aPacketService,
        MConnMonDtmNotifierObserver& aObserver )
        :
        CActive( EConnMonPriorityNormal ),
        iPacketService( aPacketService ),
        iMsClass( RPacketService::EMSClassUnknown ),
        iObserver( aObserver ),
        iSendEvents( EFalse )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::~CConnMonDtmNotifier
// -----------------------------------------------------------------------------
//
CConnMonDtmNotifier::~CConnMonDtmNotifier()
    {
    //LOGENTRFN("CConnMonDtmNotifier::~CConnMonDtmNotifier()")
    Cancel();
    //LOGEXITFN("CConnMonDtmNotifier::~CConnMonDtmNotifier()")
    }


// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonDtmNotifier::ConstructL()
    {
    //LOGENTRFN("CConnMonDtmNotifier::ConstructL()")

    // Request current MS class from ETel
    TRequestStatus status;
    iPacketService.GetMSClass( status, iMsClass, iMaxMsClass );
    User::WaitForRequest( status );

    if ( status.Int() != KErrNone )
        {
        LOGIT1("CConnMonDtmNotifier: ERROR getting MSClass from ETel <%d>", status.Int())
        User::LeaveIfError( status.Int() );
        }

    // Request notification from ETel about MS class change
    iPacketService.NotifyMSClassChange( iStatus, iMsClass );
    SetActive();

    LOGIT("Created CConnMonDtmNotifier")
    //LOGEXITFN("CConnMonDtmNotifier::ConstructL()")
    }


// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::IsInDualMode
// -----------------------------------------------------------------------------
//
TBool CConnMonDtmNotifier::IsInDualMode() const
    {
    LOGIT1("CConnMonDtmNotifier::IsInDualMode: %d", iMsClass == RPacketService::EMSClassDualMode)

    return ( iMsClass == RPacketService::EMSClassDualMode );
    }

// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::Receive
// -----------------------------------------------------------------------------
//
void CConnMonDtmNotifier::Receive()
    {
    iSendEvents = ETrue;
    }

// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::CancelNotificatications
// -----------------------------------------------------------------------------
//
void CConnMonDtmNotifier::CancelNotifications()
    {
    iSendEvents = EFalse;
    }

// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::RunL
// -----------------------------------------------------------------------------
//
void CConnMonDtmNotifier::RunL()
    {
    //LOGENTRFN("CConnMonDtmNotifier::RunL()")

    LOGIT(".")
    LOGIT1("RunL: CConnMonDtmNotifier, status <%d>", iStatus.Int())

    if ( KErrNone == iStatus.Int() )
        {
        if ( iSendEvents )
            {
            LOGIT("Server: CConnMonTelNotifier::RunL - Notifying observer")
            iObserver.DtmStateChanged();
            }
        // Request notification from ETel about MS class change
        iPacketService.NotifyMSClassChange( iStatus, iMsClass );
        SetActive();
        }
    else
        {
        // Log error and stop
        LOGIT1("Server: CConnMonDtmNotifier::RunL - Error <%d>", iStatus.Int())
        }
    //LOGEXITFN("CConnMonDtmNotifier::RunL()")
    }


// -----------------------------------------------------------------------------
// CConnMonDtmNotifier::DoCancel
// -----------------------------------------------------------------------------
//
void CConnMonDtmNotifier::DoCancel()
    {
    //LOGENTRFN("CConnMonDtmNotifier::DoCancel()")
    iPacketService.CancelAsyncRequest( EPacketNotifyMSClassChange );
    //LOGEXITFN("CConnMonDtmNotifier::DoCancel()")
    }

// End-of-file
