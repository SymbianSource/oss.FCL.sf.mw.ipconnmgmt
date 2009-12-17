/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to listen for HSxPA notifications.
*
*/

#include <e32base.h>
#include <rmmcustomapi.h>

#include "ConnMonServ.h"
#include "ConnMonSess.h"
#include "ConnMonIAP.h"
#include "ConnMonNoti.h"

#include "ConnMonHsdpaNotifier.h"
#include "ConnMonBearerNotifier.h"

#include "log.h"

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::CConnMonHsdpaNotifier
// Notifies when WCDMA bearer activity changes.
// -----------------------------------------------------------------------------
//
CConnMonHsdpaNotifier::CConnMonHsdpaNotifier(
        CConnMonBearerNotifier* aObserver,
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone )
        :
        CActive( EConnMonPriorityNormal ),
        iObserver( aObserver ),
        iServer( aServer ),
        iMobilePhone( aMobilePhone )
    {
    iPreviousHsxpaStatus = EBearerInfoWCDMA;
    iHsxpaStatus = EBearerInfoWCDMA;
    iHsupaActive = EFalse;
    iHsdpaActive = EFalse;
    iPacketServLoaded = 0;
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonHsdpaNotifier::Construct()
    {
    //LOGENTRFN("CConnMonHsdpaNotifier::ConstructL()")

    CActiveScheduler::Add( this );
    TInt err = iPacketService.Open( iMobilePhone );
    if ( KErrNone == err )
        {
        iPacketServLoaded = 1;
        }
    LOGIT("Created CConnMonHsdpaNotifier")

    //LOGEXITFN("CConnMonHsdpaNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::NewL
// -----------------------------------------------------------------------------
//
CConnMonHsdpaNotifier* CConnMonHsdpaNotifier::NewL(
    CConnMonBearerNotifier* aObserver,
    CConnMonServer* aServer,
    RMobilePhone& aMobilePhone )
    {
    CConnMonHsdpaNotifier* self = new( ELeave ) CConnMonHsdpaNotifier(
            aObserver,
            aServer,
            aMobilePhone );

    self->Construct();
    return self;
    }

// Destructor
CConnMonHsdpaNotifier::~CConnMonHsdpaNotifier()
    {
    Cancel();
    if ( iPacketServLoaded == 1 )
        {
        iPacketService.Close();
        iPacketServLoaded = 0;
        }
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::Receive
// Requests a new event (bearer WCDMA activity changed) from ETel
// -----------------------------------------------------------------------------
//
void CConnMonHsdpaNotifier::Receive()
    {
    if ( IsActive() )
        {
        return;
        }

    iPacketService.NotifyDynamicCapsChange( iStatus, iDynCaps );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::HsdpaActive
// Returns the latest state of HSDPA activity
// -----------------------------------------------------------------------------
//
TBool CConnMonHsdpaNotifier::HsdpaActive()
    {
    return iHsdpaActive;
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::HsupaActive
// Returns the latest state of HSUPA activity
// -----------------------------------------------------------------------------
//
TBool CConnMonHsdpaNotifier::HsupaActive()
    {
    return iHsupaActive;
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::HsxpaStatus
// Returns the latest state of HSxPA activity
// -----------------------------------------------------------------------------
//
TInt CConnMonHsdpaNotifier::HsxpaStatus()
    {
    return iHsxpaStatus;
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::DoCancel
// Cancels the request from ETel.
// -----------------------------------------------------------------------------
//
void CConnMonHsdpaNotifier::DoCancel()
    {
    if ( IsActive() )
        {
        LOGIT("SERVER: Cancel NotifyDynamicCapsChange()")
        iPacketService.CancelAsyncRequest( EPacketNotifyDynamicCapsChange );
        }
    }

// -----------------------------------------------------------------------------
// CConnMonHsdpaNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CConnMonHsdpaNotifier::RunL()
    {
    //LOGENTRFN("CConnMonHsdpaNotifier::RunL()")

    // All RunL():s outside CServer-derived main class MUST NOT LEAVE.
    // Use TRAPD when needed.

    LOGIT(".")
    LOGIT1("RunL: CConnMonHsdpaNotifier, status %d", iStatus.Int())

    if ( KErrNone != iStatus.Int() )
        {
        LOGIT1("ERROR, NotifyDynamicCapsChange FAILED <%d>", iStatus.Int())
        }
    else
        {
        // Let's check the bearer availability first...
        TBool byPhone( EFalse );
        TBool byCell( EFalse );

        TInt err = iServer->Iap()->GetBearerSupportInfo( EBearerIdWCDMA, byCell, byPhone );
        if ( KErrNone != err )
            {
            LOGIT1("CConnMonHsdpaNotifier: GetBearerSupportInfo returned error <%d>", err)
            Receive();
            return;
            }

        LOGIT3("CConnMonHsdpaNotifier: Bearer availability: cell %d, phone %d, dyn.caps 0x%04X",
                byCell, byPhone, iDynCaps)
        if ( byCell && byPhone ) // Wcdma bearer is available
            {
            if ( iDynCaps & RPacketService::KCapsHSDPA )
                {
                iHsdpaActive = ETrue;
                if ( iDynCaps & RPacketService::KCapsHSUPA )
                    {
                    // HSxPA
                    LOGIT("NotifyDynamicCapsChange() --> HSxPA active")
                    iHsxpaStatus = EBearerInfoHSxPA;
                    iHsupaActive = ETrue;
                    }
                else
                    {
                    // HSDPA
                    LOGIT("NotifyDynamicCapsChange() --> HSDPA active")
                    iHsxpaStatus = EBearerInfoHSDPA;
                    iHsupaActive = EFalse;
                    }
                }
            else
                {
                iHsdpaActive = EFalse;
                if ( iDynCaps & RPacketService::KCapsHSUPA )
                    {
                    // HSUPA
                    LOGIT("NotifyDynamicCapsChange() --> HSUPA active")
                    iHsxpaStatus = EBearerInfoHSUPA;
                    iHsupaActive = ETrue;
                    }
                else
                    {
                    // WCDMA
                    LOGIT("NotifyDynamicCapsChange() --> WCDMA active")
                    iHsxpaStatus = EBearerInfoWCDMA;
                    iHsupaActive = EFalse;
                    }
                }

            if ( iPreviousHsxpaStatus != iHsxpaStatus )
                {
                LOGIT2("CConnMonHsdpaNotifier: prev %d, curr %d (changed)", iPreviousHsxpaStatus, iHsxpaStatus)
                iPreviousHsxpaStatus = iHsxpaStatus;
                iObserver->HandleEvent();
                }
            else
                {
                LOGIT2("CConnMonHsdpaNotifier: prev %d, curr %d", iPreviousHsxpaStatus, iHsxpaStatus)
                }
            }
        }
    Receive();
    //LOGEXITFN("CConnMonHsdpaNotifier::RunL()")
    }

// End-of-file
