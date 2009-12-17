/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CConnMonEventHandler is a hidden active object to receive
*                notifications on behalf of a client application.
*
*/

#include <rconnmon.h>

#include "ConnMonDef.h"
#include "ConnMonCli.h"
#include "log.h"

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panic
// Panics the client in case of programming error.
// -----------------------------------------------------------------------------
//
void Panic( TInt aPanic )
    {
    _LIT( KPanicCategory, "ConnectionMonitor Client" );
    User::Panic( KPanicCategory, aPanic );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnMonEventHandler::CConnMonEventHandler
// -----------------------------------------------------------------------------
//
CConnMonEventHandler::CConnMonEventHandler(
        MConnectionMonitorObserver* aObserver,
        RConnectionMonitor& aSession )
        :
        CActive( EConnMonPriorityNormal ),
        iSession( aSession ),
        iObserver( aObserver ),
        iBuf( NULL, 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CConnMonEventHandler::Construct
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::Construct()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CConnMonEventHandler::~CConnMonEventHandler()
    {
    Cancel();
    delete iConnMonEvent;

    iObserver = NULL;
    }


// -----------------------------------------------------------------------------
// CConnMonEventHandler::ReceiveNotification
// Request a new event from Connection Monitor server.
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::ReceiveNotification()
    {
    if ( iPause )
        {
        return;
        }

    if ( IsActive() )
        {
        Cancel();
        }

    // Must be passed as a descriptor
    iEventInfo.Reset();
    iBuf.Set( reinterpret_cast<TUint8*>( &iEventInfo ),
              sizeof( TEventInfo ),
              sizeof( TEventInfo ) );

    iSession.ReceiveEvent( iBuf, iExtraBuf, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CConnMonEventHandler::Pause
// Pauses receiving events.
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::Pause()
    {
    iPause = ETrue;
    }

// -----------------------------------------------------------------------------
// CConnMonEventHandler::Continue
// Continues receiving events.
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::Continue( MConnectionMonitorObserver* aObserver )
    {
    iPause = EFalse;
    iObserver = aObserver;
    ReceiveNotification();
    }

// -----------------------------------------------------------------------------
// CConnMonEventHandler::ReceiveNotification
// Receives the new event from Connection Monitor server and passes it to the
// client interface.
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::RunL()
    {
    if ( KErrServerBusy == iStatus.Int() )
        {
        // Message slot was reserved
        // Try again
        LOGIT("Client [%d]: CConnMonEventHandler::RunL() KErrServerBusy, trying again")
        ReceiveNotification();
        }
    else if ( KErrNone == iStatus.Int() )
        {
        // A new event has arrived
        switch ( iEventInfo.iEventType )
            {
            case EConnMonCreateConnection :
                iConnMonEvent = new (ELeave) CConnMonCreateConnection(
                        iEventInfo.iConnectionId );
                break;

            case EConnMonDeleteConnection :
                iConnMonEvent = new (ELeave) CConnMonDeleteConnection(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData,
                        iEventInfo.iData2,
                        iEventInfo.iData3 );
                break;

            case EConnMonDownlinkDataThreshold :
                iConnMonEvent = new (ELeave) CConnMonDownlinkDataThreshold(
                        iEventInfo.iConnectionId,
                        iEventInfo.iSubConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonUplinkDataThreshold :
                iConnMonEvent = new (ELeave) CConnMonUplinkDataThreshold(
                        iEventInfo.iConnectionId,
                        iEventInfo.iSubConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonNetworkStatusChange :
                iConnMonEvent = new (ELeave) CConnMonNetworkStatusChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonConnectionStatusChange :
                iConnMonEvent = new (ELeave) CConnMonConnectionStatusChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iSubConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonConnectionActivityChange :
                iConnMonEvent = new (ELeave) CConnMonConnectionActivityChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iSubConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonNetworkRegistrationChange :
                iConnMonEvent = new (ELeave) CConnMonNetworkRegistrationChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonBearerChange :
                iConnMonEvent = new (ELeave) CConnMonBearerChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonSignalStrengthChange :
                iConnMonEvent = new (ELeave) CConnMonSignalStrengthChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonBearerAvailabilityChange :
                iConnMonEvent = new (ELeave) CConnMonBearerAvailabilityChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonIapAvailabilityChange :
                iConnMonEvent = new (ELeave) CConnMonIapAvailabilityChange(
                        iEventInfo.iConnectionId,
                        reinterpret_cast<const TConnMonIapInfo*>( iExtraBuf.Ptr() ) );
                break;

            case EConnMonTransmitPowerChange :
                iConnMonEvent = new (ELeave) CConnMonTransmitPowerChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonSNAPsAvailabilityChange :
                iConnMonEvent = new (ELeave) CConnMonSNAPsAvailabilityChange(
                        iEventInfo.iConnectionId, iEventInfo.iData,
                        reinterpret_cast< const TConnMonSNAPInfo* >( iExtraBuf.Ptr() ) );
                break;

            case EConnMonNewWLANNetworkDetected :
                iConnMonEvent = new (ELeave) CConnMonNewWLANNetworkDetected(
                        iEventInfo.iConnectionId );
                break;

            case EConnMonOldWLANNetworkLost :
                iConnMonEvent = new (ELeave) CConnMonOldWLANNetworkLost(
                        iEventInfo.iConnectionId );
                break;

            case EConnMonPacketDataUnavailable :
                iConnMonEvent = new (ELeave) CConnMonPacketDataUnavailable(
                        iEventInfo.iConnectionId );
                break;

            case EConnMonPacketDataAvailable :
                iConnMonEvent = new (ELeave) CConnMonPacketDataAvailable(
                        iEventInfo.iConnectionId );
                break;

            case EConnMonBearerInfoChange :
                iConnMonEvent = new (ELeave) CConnMonBearerInfoChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData );
                break;

            case EConnMonBearerGroupChange :
                iConnMonEvent = new (ELeave) CConnMonBearerGroupChange(
                        iEventInfo.iConnectionId,
                        iEventInfo.iData2,
                        iEventInfo.iData3,
                        iEventInfo.iData );
                break;

            default:
                if ( iEventInfo.iEventType >= EConnMonPluginEventBase )
                    {
                    // Size of the data is in 'iEventInfo.iData2'
                    iConnMonEvent = new (ELeave) CConnMonGenericEvent(
                            iEventInfo.iEventType,
                            iEventInfo.iConnectionId,
                            reinterpret_cast<TAny*>( &( iEventInfo.iData ) ) );
                    }
                else
                    {
                    iConnMonEvent = new (ELeave) CConnMonEventBase(
                            iEventInfo.iEventType,
                            iEventInfo.iConnectionId );
                    }
            }

        // Deliver the event to client handler
        TRAPD( leaveCode,
                iObserver->EventL( reinterpret_cast<CConnMonEventBase&>( *iConnMonEvent ) ) );

        delete iConnMonEvent;
        iConnMonEvent = NULL;

        LOGIT6("Client [%d]: GOT EVENT: type %d, id %d, data1 %d, data2 %d, data3 %d",
                &iSession,
                iEventInfo.iEventType,
                iEventInfo.iConnectionId,
                iEventInfo.iData,
                iEventInfo.iData2,
                iEventInfo.iData3 )

        // Initiate the next receive
        ReceiveNotification();

        // If leave occurs in EventL, log and ignore
        if ( leaveCode )
            {
            LOGIT2("Client [%d]: CConnMonEventHandler::RunL() iObserver->EventL() call left <%d>",
                    &iSession, leaveCode)
            }
        }
    else
        {
        LOGIT2("Client [%d]: CConnMonEventHandler::RunL() failed <%d>", &iSession, iStatus.Int())
        }
    }

// -----------------------------------------------------------------------------
// CConnMonEventHandler::DoCancel
// Cancels the request from Connection Monitor server.
// -----------------------------------------------------------------------------
//
void CConnMonEventHandler::DoCancel()
    {
    if ( IsActive() )
        {
        iSession.CancelReceiveEvent();
        }
    }

// End-of-file
