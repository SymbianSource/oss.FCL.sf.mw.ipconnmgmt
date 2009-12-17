/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to listen for EGPRS and HSxPA notifications.
*
*/

#include <e32base.h>
#include <etelmm.h>

#include "ConnMonBearerNotifier.h"
#include "ConnMonServ.h"
#include "CEventQueue.h"
#include "ConnMonBearerGroupManager.h"
#include "ConnMonIAP.h"
#include "ConnMonEGprsNotifier.h"
#include "ConnMonHsdpaNotifier.h"
#include "log.h"


// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::CConnMonBearerNotifier
// Notifies when the bearer changes from EGPRS to GPRS on vice versa.
// This information is cell specific.
// -----------------------------------------------------------------------------
//
CConnMonBearerNotifier::CConnMonBearerNotifier(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone,
        TUint& aConnectionId )
        :
        iServer( aServer ),
        iMobilePhone( aMobilePhone ),
        iConnectionId( aConnectionId )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CConnMonBearerNotifier::ConstructL()
    {
    //LOGENTRFN("CConnMonBearerNotifier::ConstructL()")

    iEGprsNotifier = CConnMonEGprsNotifier::NewL( this, iMobilePhone );
    iHsdpaNotifier = CConnMonHsdpaNotifier::NewL( this, iServer, iMobilePhone );
    iEventInfo.iData = EBearerUnknown;
    LOGIT("Created CConnMonBearerNotifier")

    //LOGEXITFN("CConnMonBearerNotifier::ConstructL()")
    }

// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::NewL
// -----------------------------------------------------------------------------
//
CConnMonBearerNotifier* CConnMonBearerNotifier::NewL(
        CConnMonServer* aServer,
        RMobilePhone& aMobilePhone,
        TUint& aConnectionId )
    {
    CConnMonBearerNotifier* self = new( ELeave ) CConnMonBearerNotifier(
            aServer,
            aMobilePhone,
            aConnectionId );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CConnMonBearerNotifier::~CConnMonBearerNotifier()
    {
    if ( iEGprsNotifier )
        {
        iEGprsNotifier->Cancel();
        delete iEGprsNotifier;
        iEGprsNotifier = NULL;
        }

    if ( iHsdpaNotifier )
        {
        iHsdpaNotifier->Cancel();
        delete iHsdpaNotifier;
        iHsdpaNotifier = NULL;
        }

    iServer = NULL;
    }

void CConnMonBearerNotifier::Listen()
    {
    iEGprsNotifier->Receive();
    iHsdpaNotifier->Receive();
    }

void CConnMonBearerNotifier::CancelListen()
    {
    iEGprsNotifier->Cancel();
    iHsdpaNotifier->Cancel();
    }

TBool CConnMonBearerNotifier::Listening()
    {
    // It is enough to know that atleast 1 active object is active.
    if ( iEGprsNotifier->IsActive() )
        {
        return ETrue;
        }
    if ( iHsdpaNotifier->IsActive() )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::RunL
// Handles the event that has arrived from ETel
// -----------------------------------------------------------------------------
//
void CConnMonBearerNotifier::HandleEvent()
    {
    LOGIT("SERVER: CConnMonBearerNotifier::HandleEvent()")

    TBool eventSent( EFalse );
    iEventInfo.Reset();
    TConnMonBearerInfo bearerInfo( EBearerInfoUnknown );

    // Check the mode (GPRS or WCDMA).
    RMobilePhone::TMobilePhoneNetworkMode mode( RMobilePhone::ENetworkModeUnknown );
    TInt ret = iMobilePhone.GetCurrentMode( mode );
    iEventInfo.iEventType = EConnMonBearerInfoChange;
    iEventInfo.iConnectionId = iConnectionId;

    if ( KErrNone == ret
         && mode == RMobilePhone::ENetworkModeGsm
         && iEGprsNotifier->IsActive() == EFalse )
        {
        LOGIT("SERVER: CConnMonBearerNotifier::HandleEvent(): NetworkMode is GSM")
        if ( iEGprsNotifier->EGprsActive() )
            {
            iEventInfo.iData = EBearerInfoEdgeGPRS;
            }
        else
            {
            iEventInfo.iData = EBearerInfoGPRS;
            }

        iServer->EventQueue()->Add( iEventInfo );

        LOGIT2("SERVER: EVENT -> Bearer changed: BearerId %d, data %d", iEventInfo.iConnectionId, iEventInfo.iData)
        }

    else if ( KErrNone == ret
              && mode == RMobilePhone::ENetworkModeWcdma
              && iHsdpaNotifier->IsActive() == EFalse )
        {
        LOGIT("SERVER: NetworkMode is Wcdma")

        bearerInfo = (TConnMonBearerInfo)iHsdpaNotifier->HsxpaStatus();
        iEventInfo.iData = bearerInfo;

        iServer->EventQueue()->Add( iEventInfo );

        LOGIT2("SERVER: EVENT -> Bearer changed: BearerId %d, data %d", iEventInfo.iConnectionId, iEventInfo.iData)
        eventSent = ETrue;
        }

    if ( eventSent )
        {
        SendBearerGroupEvent( bearerInfo );
        }

    // New request
    Listen();
    }

// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::SendBearerGroupEvent
// This function was made for sending the dynamic caps information
// about the bearer in sync with the bearer group event.
// -----------------------------------------------------------------------------
//
void CConnMonBearerNotifier::SendBearerGroupEvent( TConnMonBearerInfo& aBearerInfo )
    {
    LOGENTRFN("CConnMonBearerNotifier::SendBearerGroupEvent()")

    if ( aBearerInfo == EBearerInfoWLAN )
        {
        LOGIT("SERVER: WLAN connection-->bearer group changed event NOT sent")
        LOGIT("SERVER: CConnMonBearerNotifier::SendBearerGroupEvent() OUT")
        return;
        }

    TUint bearerMask( 0 );
    TUint bearerMask2( 0 );

    TInt err = iServer->BearerGroupManager()->GetBearerGroups(
            aBearerInfo,
            bearerMask,
            bearerMask2 );

    if ( KErrNone != err )
        {
        LOGIT1("SERVER: CConnMonBearerNotifier::SendBearerGroupEvent(): GetBearerGroups() error <%d>", err)
        return;
        }

    iEventInfo.iConnectionId = iConnectionId;
    iEventInfo.iEventType = EConnMonBearerGroupChange;

    iEventInfo.iData = 1;
    iEventInfo.iData2 = bearerMask;
    iEventInfo.iData3 = bearerMask2;

    iServer->EventQueue()->Add( iEventInfo );

    LOGIT3("SERVER: EVENT -> Bearer group info changed: connId %d, internal %d, groups %d",
            iEventInfo.iConnectionId, iEventInfo.iData, iEventInfo.iData2)
    LOGEXITFN("CConnMonBearerNotifier::SendBearerGroupEvent()")
    }


// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::SendBearerGroupEvent
//
// -----------------------------------------------------------------------------
//
void CConnMonBearerNotifier::SendBearerGroupEvent()
    {
    LOGENTRFN("CConnMonBearerNotifier::SendBearerGroupEvent()")

    TInt bearer( EBearerUnknown );
    TBearerInfo bearerInfo;

    TInt err = iServer->Iap()->GetBearer(
            iConnectionId,
            bearer,
            bearerInfo );

    if ( KErrNone != err )
        {
        LOGIT1("SERVER: CConnMonBearerNotifier::SendBearerGroupEvent(): GetBearer() error <%d>", err)
        return;
        }

    if ( bearerInfo.iBearer == EBearerInfoWLAN )
        {
        LOGIT("SERVER: WLAN connection-->bearer group changed event NOT sent")
        LOGIT("SERVER: CConnMonBearerNotifier::SendBearerGroupEvent() OUT")
        return;
        }

    TUint bearerMask( 0 );
    TUint bearerMask2( 0 );

    err = iServer->BearerGroupManager()->GetBearerGroups(
            (TConnMonBearerInfo)bearerInfo.iBearer,
            bearerMask,
            bearerMask2 );

    if ( KErrNone != err )
        {
        LOGIT1("SERVER: CConnMonBearerNotifier::SendBearerGroupEvent(): GetBearerGroups() error <%d>", err)
        return;
        }

    iEventInfo.iConnectionId = iConnectionId;
    iEventInfo.iEventType = EConnMonBearerGroupChange;

    iEventInfo.iData = bearerInfo.iInternal;
    iEventInfo.iData2 = bearerMask;
    iEventInfo.iData3 = bearerMask2;

    iServer->EventQueue()->Add( iEventInfo );

    LOGIT3("SERVER: EVENT -> Bearer group info changed: connId %d, internal %d, groups %d",
            iEventInfo.iConnectionId, iEventInfo.iData, iEventInfo.iData2)
    LOGEXITFN("CConnMonBearerNotifier::SendBearerGroupEvent()")
    }

// -----------------------------------------------------------------------------
// CConnMonBearerNotifier::SendBearerInfoEvent
//
// -----------------------------------------------------------------------------
//
void CConnMonBearerNotifier::SendBearerInfoEvent()
    {
    TInt bearer( EBearerUnknown );
    TBearerInfo bearerInfo;

    TInt err = iServer->Iap()->GetBearer(
            iConnectionId,
            bearer,
            bearerInfo );

    if ( KErrNone != err )
        {
        LOGIT1("SERVER: CConnMonBearerNotifier::SendBearerInfoEvent(): GetBearer() error <%d>", err)
        return;
        }

    iEventInfo.Reset();

    iEventInfo.iEventType = EConnMonBearerInfoChange;
    iEventInfo.iConnectionId = iConnectionId;
    iEventInfo.iData = bearerInfo.iBearer;

    iServer->EventQueue()->Add( iEventInfo );

    LOGIT2("SERVER: EVENT -> Bearer info changed: connId %d, bearerInfo %d", iEventInfo.iConnectionId, iEventInfo.iData)
    }

// End-of-file
