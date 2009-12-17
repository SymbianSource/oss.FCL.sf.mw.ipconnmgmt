/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to listen for EGPRS notifications.
*
*/

#include <e32base.h>
#include <rmmcustomapi.h>

#include "ConnMonEGprsNotifier.h"
#include "ConnMonBearerNotifier.h"
#include "log.h"

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::CConnMonEGprsNotifier
// Notifies when GPRS<-->EGPRS bearer changes.
// -----------------------------------------------------------------------------
//
CConnMonEGprsNotifier::CConnMonEGprsNotifier(
        CConnMonBearerNotifier* aObserver,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iObserver( aObserver ),
        iMobilePhone( aMobilePhone ),
        iGprsInfo(),
        iGprsInfoPckg( iGprsInfo )
    {
    iEGprsActive = EFalse;
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonEGprsNotifier::ConstructL()
    {
    //LOGENTRFN("CConnMonEGprsNotifier::ConstructL()")

    CActiveScheduler::Add( this );
    // Open ETel custom API
    TInt ret = iCustomApi.Open( iMobilePhone );

    if ( ret != KErrNone )
        {
        LOGIT1("GPRS Bearer notifier NOT started. CustomAPI.Open() - err <%d>", ret )
        User::Leave( ret );
        }
    LOGIT("Created CConnMonEGprsNotifier")

    //LOGEXITFN("CConnMonEGprsNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::NewL
// -----------------------------------------------------------------------------
//
CConnMonEGprsNotifier* CConnMonEGprsNotifier::NewL(
        CConnMonBearerNotifier* aObserver,
        RMobilePhone& aMobilePhone )
    {
    CConnMonEGprsNotifier* self = new( ELeave ) CConnMonEGprsNotifier( aObserver, aMobilePhone );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CConnMonEGprsNotifier::~CConnMonEGprsNotifier()
    {
    Cancel();

    // Close ETel custom API
    iCustomApi.Close();
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::Receive
// Requests a new event (bearer GPRS) from ETel
// -----------------------------------------------------------------------------
//
void CConnMonEGprsNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iCustomApi.NotifyEGprsInfoChange( iStatus, iGprsInfoPckg );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::EGprsActive
// Returns the latest state of the EGPRS activity
// -----------------------------------------------------------------------------
//
TBool CConnMonEGprsNotifier::EGprsActive()
    {
    return iEGprsActive;
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CConnMonEGprsNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        LOGIT("SERVER: Cancel NotifyEGprsInfoChange()")
        iCustomApi.CancelAsyncRequest( ECustomNotifyEGprsInfoChange );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonEGprsNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CConnMonEGprsNotifier::RunL()
    {
    //LOGENTRFN("CConnMonEGprsNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CConnMonEGprsNotifier, status %d", iStatus.Int())

    if ( KErrNone != iStatus.Int() )
        {
        LOGIT1("ERROR, NotifyEGprsInfoChange FAILED <%d>", iStatus.Int())
        }
    else
        {
        if ( iGprsInfoPckg().iGprsInfo == RMmCustomAPI::EEdgeGprs )
            {
            LOGIT("SERVER: NotifyEGprsInfoChange()--> EGPRS is active")
            iEGprsActive = ETrue;
            }
        else
            {
            LOGIT("SERVER: NotifyEGprsInfoChange()--> GPRS is active")
            iEGprsActive = EFalse;
            }
        iObserver->HandleEvent();
        // New request
        Receive();
        }
    //LOGEXITFN("CConnMonEGprsNotifier::RunL()")
    }

// End-of-file
