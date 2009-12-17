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
* Description:  ConnMon Telephony watcher.
*
*/

#include <mmtsy_names.h>
#include <pcktcs.h>

#include "connmontelnoti.h"
#include "ConnMonDef.h"
#include "log.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonTelNotifier::NewL
// -----------------------------------------------------------------------------
//
CConnMonTelNotifier* CConnMonTelNotifier::NewL(
        MConnMonTelephonyObserver& aObserver,
        RMobilePhone* aMobilePhone )
    {
    CConnMonTelNotifier* self = new( ELeave ) CConnMonTelNotifier( aObserver, aMobilePhone );

    self->Construct();
    return self;
    }


// -----------------------------------------------------------------------------
// CConnMonTelNotifier::CConnMonTelNotifier
// -----------------------------------------------------------------------------
//
CConnMonTelNotifier::CConnMonTelNotifier(
        MConnMonTelephonyObserver& aObserver,
        RMobilePhone* aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iMobilePhone( aMobilePhone ),
        iVoiceCallActive( EFalse ),
        iObserver( aObserver ),
        iSendEvents( EFalse )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CConnMonTelNotifier::~CConnMonTelNotifier
// -----------------------------------------------------------------------------
//
CConnMonTelNotifier::~CConnMonTelNotifier()
    {
    //LOGENTRFN("CConnMonTelNotifier::~CConnMonTelNotifier()")
    Cancel();
    iLine.Close();
    //LOGEXITFN("CConnMonTelNotifier::~CConnMonTelNotifier()")
    }


// -----------------------------------------------------------------------------
// CConnMonTelNotifier::Construct
// -----------------------------------------------------------------------------
//
void CConnMonTelNotifier::Construct()
    {
    //LOGENTRFN("CConnMonTelNotifier::Construct()")

    TInt ret = iLine.Open( *iMobilePhone, KMmTsyVoice1LineName );
    iLine.GetStatus( iCallStatus );

    if ( iCallStatus == RCall::EStatusDialling   ||
         iCallStatus == RCall::EStatusRinging    ||
         iCallStatus == RCall::EStatusAnswering  ||
         iCallStatus == RCall::EStatusConnecting ||
         iCallStatus == RCall::EStatusConnected  ||
         iCallStatus == RCall::EStatusHangingUp )
        {
        LOGIT1("Voice line status: active (%d)", iCallStatus)
        iVoiceCallActive = ETrue;
        }
    else if ( iCallStatus == RCall::EStatusIdle ||
              iCallStatus == RCall::EStatusUnknown )
        {
        LOGIT1("Voice line status: not active (%d)", iCallStatus)
        iVoiceCallActive = EFalse;
        }
    else
        {
        LOGIT1("Voice line status: unknown (%d)", iCallStatus )
        iVoiceCallActive = EFalse;
        }
    iLine.NotifyStatusChange( iStatus, iCallStatus );
    SetActive();

    LOGIT("Created CConnMonTelNotifier")
    //LOGEXITFN("CConnMonTelNotifier::Construct()")
    }


// -----------------------------------------------------------------------------
// CConnMonTelNotifier::IsCallActive
// -----------------------------------------------------------------------------
//
TBool CConnMonTelNotifier::IsCallActive() const
    {
    LOGIT("CConnMonTelNotifier::IsCallActive")

    return iVoiceCallActive;
    }

// -----------------------------------------------------------------------------
// CConnMonTelNotifier::Receive
// -----------------------------------------------------------------------------
//
void CConnMonTelNotifier::Receive()
    {
    iSendEvents = ETrue;
    }

// -----------------------------------------------------------------------------
// CConnMonTelNotifier::CancelNotificatications
// -----------------------------------------------------------------------------
//
void CConnMonTelNotifier::CancelNotifications()
    {
    iSendEvents = EFalse;
    }

// -----------------------------------------------------------------------------
// CConnMonTelNotifier::RunL
// -----------------------------------------------------------------------------
//
void CConnMonTelNotifier::RunL()
    {
    LOGENTRFN("CConnMonTelNotifier::RunL()")

    TBool stateChanged( EFalse );
    if ( KErrNone == iStatus.Int() )
        {
        LOGIT1("Server: CConnMonTelNotifier::RunL - Call status <%d>", iCallStatus )

        if ( iCallStatus == RCall::EStatusDialling   ||
             iCallStatus == RCall::EStatusRinging    ||
             iCallStatus == RCall::EStatusAnswering  ||
             iCallStatus == RCall::EStatusConnecting ||
             iCallStatus == RCall::EStatusConnected  ||
             iCallStatus == RCall::EStatusHangingUp )
            {
            // Do not notify, if state doesn't change
            if ( EFalse == iVoiceCallActive )
                {
                stateChanged = ETrue;
                iVoiceCallActive = ETrue;

                LOGIT("Server: CConnMonTelNotifier::RunL - Voice Call Active")
                }
            }
        else
            {
            // Do not notify, if state doesn't change
            if ( EFalse != iVoiceCallActive )
                {
                stateChanged = ETrue;
                iVoiceCallActive = EFalse;

                LOGIT("Server: CConnMonTelNotifier::RunL - Voice Call Not Active")
                }
            }
        // Does the observer want to listen to the events
        // and has the state changed from the last known state
        if ( iSendEvents && stateChanged )
            {
            LOGIT("Server: CConnMonTelNotifier::RunL - Notifying observer")

            iObserver.PhoneLineStatusChange();
            }
        iLine.NotifyStatusChange( iStatus, iCallStatus );
        SetActive();
        }
    else
        {
        // Log error and stop
        LOGIT1("Server: CConnMonTelNotifier::RunL - Error <%d>", iStatus.Int() )
        }
    LOGEXITFN("CConnMonTelNotifier::RunL()")
    }

// -----------------------------------------------------------------------------
// CConnMonTelNotifier::DoCancel
// -----------------------------------------------------------------------------
//
void CConnMonTelNotifier::DoCancel()
    {
    LOGIT("CConnMonTelNotifier::DoCancel")

    iLine.NotifyStatusChangeCancel();
    }

// End-of-file
