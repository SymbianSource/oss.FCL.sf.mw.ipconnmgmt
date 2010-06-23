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
 * Description:  Implementation of CFlexTimerSession class.
 *
 */
/*
 * %version: 1 %
 */

// System include files
#include <e32def.h>
#include <e32cmn.h>
#include <hal.h>

// User include files go here:
#include "flextimercommon.h"
#include "flextimersession.h"
#include "mflextimerservice.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flextimersessionTraces.h"
#endif

// Constants

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CFlexTimerSession::CFlexTimerSession( MFlexTimerService* aFlexTimerService )
:   iService( aFlexTimerService ),
    iTimerWinSize( 0 ),
    iUseDefaultWin( ETrue )
    {
    OstTrace1( TRACE_INTERNAL,
               CFLEXTIMERSESSION,
               "CFlexTimerSession::CFlexTimerSession; this=%x",
               ( TUint )this );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CFlexTimerSession::~CFlexTimerSession()
    {
    OstTrace1( TRACE_INTERNAL,
               CFLEXTIMERSESSIOND,
               "CFlexTimerSession::~CFlexTimerSession; this=%x",
               ( TUint )this );
    }

// --------------------------------------------------------------------------
// Function called from CFlexTimerService::RunL for messages targeted to
// this session
// --------------------------------------------------------------------------
//
void CFlexTimerSession::ServiceL( const RMessage2& aMessage )
    {
    TInt msgType( aMessage.Function() );

    OstTraceExt2( TRACE_INTERNAL,
                  CFLEXTIMERSESSION_MSG,
                  "CFlexTimerSession::ServiceL; this=%x; msgType=%d",
                  ( TInt )this,
                  msgType );

    // Check message type and pass it to correct handler. Handler either 
    // stores the message or completes it immediately.
    switch ( msgType )
        {
        case EFlexTimerServCancelRequest:
            CancelTimer( aMessage );
            break;
        case EFlexTimerServAtRequest:
            NewAtTimer( aMessage );
            break;
        case EFlexTimerServAtUTCRequest:
            NewAtUtcTimer( aMessage );
            break;
        case EFlexTimerServAfterRequest:
            NewAfterTimer( aMessage );
            break;
        case EFlexTimerServAfterTicksRequest:
            NewAfterTicksTimer( aMessage );
            break;
        case EFlexTimerServConfigureRequest:
            ConfigureParameters( aMessage );
            break;
        default:
            OstTrace1(
                    TRACE_INTERNAL,
                    CFLEXTIMERSESSION_ERROR,
                    "CFlexTimerSession::ServiceL: Unknown message; this=%x",
                    ( TUint )this );
            aMessage.Complete( KErrNotSupported );
        }
    }

// --------------------------------------------------------------------------
// Timeout handler for triggering timers
// --------------------------------------------------------------------------
//
void CFlexTimerSession::Timeout() const
    {
    // Check for the validity of pending timeout request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_TIMEOUT,
                      "CFlexTimerSession::Timeout; this=%x; msg=%x",
                      ( TUint )this,
                      ( TUint )iPendingTimeoutMsg.Handle() );
        // Complete message, this sets message pointer to NULL
        iPendingTimeoutMsg.Complete( KErrNone );
        }
    else
        {
        // No valid message pointer, nothing we can do here
        OstTrace1( TRACE_INTERNAL,
                   CFLEXTIMERSESSION_TIMEOUT_ERROR,
                   "CFlexTimerSession::Timeout: No Pending message; this=%x",
                   ( TUint )this );
        }
    }

// --------------------------------------------------------------------------
// Function for aborting At-timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::Abort( TInt aReason ) const
    {
    // Check for the validity of pending timeout request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_ABORT,
                      "CFlexTimerSession::Abort; this=%x; msg=%x",
                      ( TUint )this,
                      ( TUint )iPendingTimeoutMsg.Handle() );
        // Complete message, this sets message pointer to NULL
        iPendingTimeoutMsg.Complete( aReason );
        }
    else
        {
        // No valid message pointer, nothing we can do here
        OstTrace1(
                TRACE_INTERNAL,
                CFLEXTIMERSESSION_ABORT_ERROR,
                "CFlexTimerSession::Abort: No Pending message; this=%x",
                ( TUint )this );

        }
    }

// --------------------------------------------------------------------------
// Handler function for starting At() -timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::NewAtTimer( const RMessage2& aMessage )
    {

    // Check that we do not have a pending timer request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_ATPANIC,
                      "CFlexTimerSession::NewAtTimer: Already pending; "
                      "this=%x; msg=%x",
                      ( TUint )this,
                      ( TUint )iPendingTimeoutMsg.Handle() );

        aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerErrorPendingTimer );
        return;
        }

    // Get interval from current time to the requested timestamp
    TTime refTime;
    refTime.HomeTime();
    TTimeIntervalMicroSeconds interval;
    GetIntervalToMsgTime( aMessage, refTime, interval );

    OstTraceExt2( TRACE_INTERNAL,
                  CFLEXTIMERSESSION_ATTIMER,
                  "CFlexTimerSession::NewAtTimer; this=%x; interval=%lld",
                  ( TUint )this,
                  interval.Int64() );

    // Add the timer to engine
    DoAddTimer( aMessage, interval, ETrue );

    }

// --------------------------------------------------------------------------
// Handler function for starting AtUTC() -timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::NewAtUtcTimer( const RMessage2& aMessage )
    {

    // Check that we do not have a pending timer request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_ATUTCPANIC,
                      "CFlexTimerSession::NewAtUtcTimer: Already pending; "
                      "this=%x; msg=%x",
                      ( TUint )this,
                      ( TUint )iPendingTimeoutMsg.Handle() );

        aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerErrorPendingTimer );
        return;
        }

    TTime refTime;
    refTime.UniversalTime();
    TTimeIntervalMicroSeconds interval;
    GetIntervalToMsgTime( aMessage, refTime, interval );
    
    OstTraceExt2( 
            TRACE_INTERNAL,
            CFLEXTIMERSESSION_ATUTCTIMER,
            "CFlexTimerSession::NewAtUtcTimer; this=%x; interval=%lld",
            ( TUint )this,
            interval.Int64() );

    // Add the timer to engine
    DoAddTimer( aMessage, interval, ETrue );

    }

// --------------------------------------------------------------------------
// Handler function for starting After()-timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::NewAfterTimer( const RMessage2& aMessage )
    {

    // Check that we do not have a pending timer request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_AFTERPANIC,
                      "CFlexTimerSession::NewAfterTimer: Already pending; "
                      "this=%x; msg=%x",
                      ( TUint )this,
                      ( TUint )iPendingTimeoutMsg.Handle() );

        aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerErrorPendingTimer );
        return;
        }

    TInt64 timeStamp = MAKE_TINT64( aMessage.Int1(), aMessage.Int0() );
    TTimeIntervalMicroSeconds interval( timeStamp );

    OstTraceExt2(
            TRACE_INTERNAL,
            CFLEXTIMERSESSION_AFTERTIMER,
            "CFlexTimerSession::NewAfterTimer; this=%x; interval=%llu",
            ( TUint )this,
            interval.Int64() );

    // Add the timer to engine, request cancellation if the secure time
    // is not available
    DoAddTimer( aMessage, interval, EFalse );
    }

// --------------------------------------------------------------------------
// Handler function for starting AfterTicks()-timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::NewAfterTicksTimer( const RMessage2& aMessage )
    {

    // Check that we do not have a pending timer request
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTraceExt2(
                TRACE_INTERNAL,
                CFLEXTIMERSESSION_AFTERTICKSPANIC,
                "CFlexTimerSession::NewAfterTicksTimer: Already pending; "
                "this=%x; msg=%x",
                ( TUint )this,
                ( TUint )iPendingTimeoutMsg.Handle() );

        aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerErrorPendingTimer );
        return;
        }

    // Get the tick period from HAL and convert ticks to microseconds
    TInt tickUs;
    HAL::Get( HAL::ESystemTickPeriod, tickUs );

    TTimeIntervalMicroSeconds interval =
            static_cast<TInt64> ( aMessage.Int0() ) * tickUs;

    OstTraceExt2(
            TRACE_INTERNAL,
            CFLEXTIMERSESSION_AFTERTICKSTIMER,
            "CFlexTimerSession::NewAfterTicksTimer; this=%x; interval=%llu",
            ( TUint )this,
            interval.Int64() );

    // Add the timer to engine
    DoAddTimer( aMessage, interval, EFalse );
    }

// --------------------------------------------------------------------------
// Handler function for cancelling running timer
// --------------------------------------------------------------------------
//
void CFlexTimerSession::CancelTimer( const RMessage2& aMessage )
    {
    TInt ret;
    // Check that we have a pending timer request
    if ( iPendingTimeoutMsg.IsNull() )
        {
        OstTrace1(
                TRACE_INTERNAL,
                CFLEXTIMERSESSION_CANCELERROR,
                "CFlexTimerSession::CancelTimer: no pending msg; this=%x",
                ( TUint )this );
        ret = KErrNotFound;
        }
    else
        {
        ret = iService->CancelTimer( this );
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_CANCEL,
                      "CFlexTimerSession::CancelTimer; this=%x; ret=%d",
                      ( TUint )this,
                      ret );

        // Complete pending message, this sets message pointer to NULL
        iPendingTimeoutMsg.Complete( KErrCancel );
        }

    // Complete cancel message
    aMessage.Complete( ret );
    }

// --------------------------------------------------------------------------
// Handler function for configuring timer parameters
// --------------------------------------------------------------------------
//
void CFlexTimerSession::ConfigureParameters( const RMessage2& aMessage )
    {
    TInt ret( KErrNone );

    // Configuration is not allowed when there is a pending message
    if ( !iPendingTimeoutMsg.IsNull() )
        {
        OstTrace1( TRACE_INTERNAL,
                   CFLEXTIMERSESSION_CONFIGERROR,
                   "CFlexTimerSession::ConfigureParameters: Timer pending; "
                   "this=%x",
                   ( TUint )this );
        ret = KErrInUse;
        }
    else
        {
        TInt paramType( aMessage.Int0() );
        OstTraceExt2( TRACE_INTERNAL,
                      CFLEXTIMERSESSION_CONFIG,
                      "CFlexTimerSession::ConfigureParameters; this=%x; "
                      "paramType=%d",
                      ( TUint )this,
                      paramType );

        switch ( paramType )
            {
            case EConfigureRequestWindowSize:
                {
                TInt winLo = aMessage.Int1();
                TInt winHi = aMessage.Int2();

                iTimerWinSize = MAKE_TINT64( winHi, winLo );
                iUseDefaultWin = EFalse;

                OstTraceExt1( TRACE_INTERNAL,
                              CFLEXTIMERSESSION_CONFIGWIN,
                              "CFlexTimerSession::ConfigureParameters; "
                              "iTimerWinSize=%lld",
                              iTimerWinSize.Int64() );
                
                if ( iTimerWinSize.Int64() < 0 ||
                    iTimerWinSize.Int64() > KFlexTimerMaxTimerLength )
                    {
                    aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerIllegalTimerValue );
                    // Return after panicing the client -- completing the
                    // message afterwards panics the server with USER 70.
                    return;
                    }
                break;
                }
            default:
                OstTrace0( TRACE_INTERNAL,
                           CFLEXTIMERSESSION_CONFIGUNKNOWN,
                           "CFlexTimerSession::ConfigureParameters: "
                           "ERROR - Invalid parameter type" );
                ret = KErrNotSupported;
                break;
            }
        }
    aMessage.Complete( ret );
    }

// --------------------------------------------------------------------------
// Function for adding timer to engine and handling its return value
// --------------------------------------------------------------------------
//
void CFlexTimerSession::DoAddTimer( const RMessage2& aMessage,
                                    TTimeIntervalMicroSeconds& aWinEnd,
                                    TBool aRequestAbort )
    {
    OstTraceExt4( TRACE_INTERNAL,
               CFLEXTIMERSESSION_ADDTIMER,
               "CFlexTimerSession::DoAddTimer; this=%x; msg=%x; aWinEnd=%llu;"
               " aRequestAbort=%d",
               ( TUint )this,
               ( TUint )aMessage.Handle(),
               aWinEnd.Int64(),
               aRequestAbort );  
    
    // Check that the timer has valid values.
    if ( aWinEnd.Int64() < 0 || aWinEnd.Int64() > KFlexTimerMaxTimerLength)
        {
        OstTraceExt4( TRACE_INTERNAL, 
                      CFLEXTIMERSESSION_DOADDTIMER, 
                      "CFlexTimerSession::DoAddTimer;Invalid parameters "
                      "- panicing client;this=%x;aMessage=%x;aWinEnd=%llu;"
                      "aRequestAbort=%d", 
                      ( TUint )this, 
                      ( TUint )aMessage.Handle(), 
                      aWinEnd.Int64(),
                      aRequestAbort );
        aMessage.Panic( KRFlexTimerPanicCat,
                        EFlexTimerServerIllegalTimerValue );
        return;
        }
    
    TTimeIntervalMicroSeconds winStart;
    TTimeIntervalMicroSeconds window = iTimerWinSize;
    
    if ( iUseDefaultWin )
        {
        window = static_cast<TInt64> ( static_cast<TReal64> ( aWinEnd.Int64() )
            * KDefaultWindowMultiplier );
        }

    winStart = aWinEnd.Int64() - window.Int64();

    // Store pending message. Not completed until timer expires.
    // Note that in case of zero or negative timer value, message can
    // be completed through Timeout() even before AddTimer returns. This
    // implementation saves some checks and timer stopping/starting in engine.
    iPendingTimeoutMsg = aMessage;

    // Add timer to engine
    TInt ret = iService->AddTimer( winStart, aWinEnd, aRequestAbort, this );

    if ( KErrNone != ret )
        {
        // Failed, pass error code to client side
        OstTraceExt2( TRACE_INTERNAL,
                   CFLEXTIMERSESSION_ADDERROR,
                   "CFlexTimerSession::DoAddTimer: Error; this=%x, ret=%d",
                   ( TUint )this,
                   ret );
        // Complete pending message, this sets message pointer to NULL
        iPendingTimeoutMsg.Complete( ret );
        }
    }

// --------------------------------------------------------------------------
// From CSession2
// Function for handling unintentional termination of the client
// --------------------------------------------------------------------------
//
void CFlexTimerSession::Disconnect( const RMessage2 &aMessage )
    {
    // The client of this session has died.
    
    // Remove timer from engine.
    iService->CancelTimer( this );

    // Disconnect() must end with a call to the base class implementation, 
    // which will delete the session object and complete the disconnect 
    // message.
    CSession2::Disconnect( aMessage );
    }
