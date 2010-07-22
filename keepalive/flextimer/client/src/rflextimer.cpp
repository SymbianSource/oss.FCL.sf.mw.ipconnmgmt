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
 * Description:  Implementation of RFlexTimer class
 *
 */

/*
 * %version: 1 %
 */
#include "rflextimer.h"
#include "flextimercommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "rflextimerTraces.h"
#endif


_LIT( KFlexTimerSemaphoreName, "FlexTimerSemaphore" );

// Semaphore count value initialization to 1. The 1st to call Wait() will
// pass the semaphore. Other processes attempting entry will wait until the
// 1st one has called Signal() on the semaphore.
const TInt KPassFirstWaitEntry = 1;

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C RFlexTimer::RFlexTimer()
    {
    OstTrace1( TRACE_NORMAL, RFLEXTIMER_RFLEXTIMER,
               "RFlexTimer::RFlexTimer;this=%x", this );
    
    }

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C RFlexTimer::~RFlexTimer()
    {
    OstTrace1( TRACE_NORMAL, DUP1_RFLEXTIMER_RFLEXTIMER,
               "RFlexTimer::~RFlexTimer;this=%x", ( TUint )this );
    
    Close();
    }

// ---------------------------------------------------------------------------
// Connects to the server and create a session.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimer::Connect()
    {
    OstTrace1( TRACE_NORMAL, RFLEXTIMER_CONNECT,
               "RFlexTimer::Connect;this=%x", ( TUint )this );
    
    // Not opened handle-number
    const TInt kHandleNotOpened( 0 );
    
    // Starts the server, if it does not already exist in the system.
    TInt ret = StartServer();

    if ( ret == KErrNone )
        { // No session, create it

        // Handle() is zero when initialized RHandleBase. Close() also zeroes 
        // the handle. If CreateSession() fails, Handle() is still zero.
        // If session is created ok, the Handle() contains the (non zero)
        // handle-number. 
        //
        // Handle() can be used for checking is the handle opened or not.
        //
        if ( Handle() == kHandleNotOpened )
            {
            // Creates the session for this client.
            ret = CreateSession( KFlexTimerServerName,
                                 Version(),
                                 KFlexTimerServerMessageSlots );
            }
        else
            { // Session already exists - panic.

            OstTrace1( 
                TRACE_NORMAL, 
                DUP1_RFLEXTIMER_CONNECT,
                "RFlexTimer::Connect already connected - PANIC;this=%x", 
                ( TUint )this );
        
            User::Panic( KRFlexTimerPanicCat, EFlexTimerAlreadyConnected );
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Cancels any outstanding request to the server.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::Cancel()
    {
    OstTrace1( TRACE_NORMAL, RFLEXTIMER_CANCEL,
               "RFlexTimer::Cancel;this=%x", ( TUint )this );
    
    SendReceive( EFlexTimerServCancelRequest );
    }

// ---------------------------------------------------------------------------
// Fire timer at latest on the given interval. 32-bit interval.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::After( TRequestStatus& aStatus,
                                 TTimeIntervalMicroSeconds32 aInterval )
    {
    OstTraceExt3( TRACE_NORMAL, RFLEXTIMER_AFTER32,
                  "RFlexTimer::After32;this=%x;aStatus=%x;aInterval=%d",
                  ( TUint )this, ( TUint )&( aStatus ), aInterval.Int() );
    
    const TTimeIntervalMicroSeconds32 ZERO_INTERVAL32(0);
    __ASSERT_ALWAYS(aInterval >= ZERO_INTERVAL32,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerAfterIntervalLessThanZero));
    
    TIpcArgs args( aInterval.Int(), 0 );
    //asynchronous request for timer creation and timeout request.    
    SendReceive( EFlexTimerServAfterRequest, args, aStatus );
    }

// ---------------------------------------------------------------------------
// Fire timer at latest on the given interval. 64-bit interval.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::After( TRequestStatus& aStatus,
                                 TTimeIntervalMicroSeconds aInterval )
    {
    OstTraceExt3( TRACE_NORMAL, RFLEXTIMER_AFTER64,
                  "RFlexTimer::After64;this=%x;aStatus=%x;aInterval=%lld",
                  ( TUint )this, ( TUint )&( aStatus ), aInterval.Int64() );
    
    const TTimeIntervalMicroSeconds ZERO_INTERVAL(0);
    __ASSERT_ALWAYS(aInterval >= ZERO_INTERVAL,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerAfterIntervalLessThanZero));
    
    TIpcArgs args( I64LOW(aInterval.Int64()), I64HIGH(aInterval.Int64()) );
    //asynchronous request for timer creation and timeout request.    
    SendReceive( EFlexTimerServAfterRequest, args, aStatus );
    }

// ---------------------------------------------------------------------------
// Fire timer at latest after the given number of ticks.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::AfterTicks( TRequestStatus& aStatus, TInt aTicks )
    {
    OstTraceExt3( TRACE_NORMAL, RFLEXTIMER_AFTERTICKS,
                  "RFlexTimer::AfterTicks;this=%x;aStatus=%d;aTicks=%d",
                  ( TUint )this, ( TUint )&( aStatus ), aTicks );
    
    __ASSERT_ALWAYS(aTicks >= 0,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerAfterTicksIntervalLessThanZero));

    TIpcArgs args( aTicks );
    //asynchronous request for timer creation and timeout request.    
    SendReceive( EFlexTimerServAfterTicksRequest, args, aStatus );
    }

// ---------------------------------------------------------------------------
// Fire timer between at latest by the given time value.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::At( TRequestStatus& aStatus, const TTime& aTime )
    {
    OstTraceExt3( TRACE_NORMAL, RFLEXTIMER_AT,
                  "RFlexTimer::At;this=%x;aStatus=%d;aTime=%lld",
                  ( TUint )this, ( TUint )&( aStatus ), aTime.Int64() );
    
    TTime nowTime;
    nowTime.HomeTime();
    
    __ASSERT_ALWAYS(aTime >= nowTime,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerAtIntervalLessThanZero));
    
    TIpcArgs args( I64LOW(aTime.Int64()), I64HIGH(aTime.Int64()) );
    //asynchronous request for timer creation and timeout request.    
    SendReceive( EFlexTimerServAtRequest, args, aStatus );
    }

// ---------------------------------------------------------------------------
// Fire timer between at latest by the given time value.
// ---------------------------------------------------------------------------
//
EXPORT_C void RFlexTimer::AtUTC( TRequestStatus& aStatus, const TTime& aTime )
    {
    OstTraceExt3( TRACE_NORMAL, RFLEXTIMER_ATUTC,
                  "RFlexTimer::AtUTC;this=%x;aStatus=%d;aTime=%lld",
                  ( TUint )this, ( TUint )&( aStatus ), aTime.Int64() );
    
    TTime nowTime;
    nowTime.UniversalTime();
    
    __ASSERT_ALWAYS(aTime >= nowTime,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerAtUTCIntervalLessThanZero));
    
    TIpcArgs args( I64LOW(aTime.Int64()), I64HIGH(aTime.Int64()) );
    //asynchronous request for timer creation and timeout request.    
    SendReceive( EFlexTimerServAtUTCRequest, args, aStatus );
    }

// ---------------------------------------------------------------------------
// Sets the window size in which alignment is possible for the timer.
// This is a synchronous command. 32-bit function version.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimer::Configure( TTimeIntervalMicroSeconds32 aWindowSize )
    {
    OstTraceExt2( TRACE_NORMAL, RFLEXTIMER_CONFIGURE,
                  "RFlexTimer::Configure32;this=%x;aWindowSize=%d",
                  ( TUint )this, aWindowSize.Int() );
    
    const TTimeIntervalMicroSeconds32 ZERO_INTERVAL32(0);
    __ASSERT_ALWAYS(aWindowSize >= ZERO_INTERVAL32,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerWindowLessThanZero));
    
    TInt64 transfer = MAKE_TINT64( 0, aWindowSize.Int() );
        
    // Regardless of the user function, the window size is always transmitted
    // as a 64-bit integer.
    TIpcArgs args( EConfigureRequestWindowSize, 
                   I64LOW(transfer), I64HIGH(transfer) );

    return SendReceive( EFlexTimerServConfigureRequest, args );
    }

// ---------------------------------------------------------------------------
// Sets the window size in which alignment is possible for the timer.
// This is a synchronous command. 64-bit function version.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RFlexTimer::Configure( TTimeIntervalMicroSeconds aWindowSize )
    {
    OstTraceExt2( TRACE_NORMAL, DUP1_RFLEXTIMER_CONFIGURE,
                  "RFlexTimer::Configure64;this=%x;aWindowSize=%lld",
                  ( TUint )this, aWindowSize.Int64() );
    
    const TTimeIntervalMicroSeconds ZERO_INTERVAL64(0);
    __ASSERT_ALWAYS(aWindowSize >= ZERO_INTERVAL64,
                    User::Panic(KRFlexTimerPanicCat,
                                EFlexTimerWindowLessThanZero));
    
    // Regardless of the user function, the window size is always transmitted
    // as a 64-bit integer.
    TIpcArgs args( EConfigureRequestWindowSize,
                   I64LOW(aWindowSize.Int64()),
                   I64HIGH(aWindowSize.Int64()) );

    return SendReceive( EFlexTimerServConfigureRequest, args  );
    }

// ---------------------------------------------------------------------------
// Gets the version number.
// ---------------------------------------------------------------------------
//
TVersion RFlexTimer::Version() const
    {
    return ( TVersion( KFlexTimerServMajorVersionNumber,
                       KFlexTimerServMinorVersionNumber,
                       KFlexTimerServBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
// Connects to the server. If server does not exist, it is created.
// ---------------------------------------------------------------------------
//
TInt RFlexTimer::StartServer()
    {
    OstTrace1( TRACE_NORMAL, RFLEXTIMER_STARTSERVER,
               "RFlexTimer::StartServer;this=%x", ( TUint )this );
    
    TFindServer findServer( KFlexTimerServerName );
    TFullName serverName;

    // See if the server is already started. 
    TInt ret = findServer.Next( serverName );
    
    if ( ret != KErrNone )
        {
        //Server was not found so create one.
        RProcess serverProcess;
        TBuf<1> serverParameters;

        // Semaphore to guard the opening of the server process.
        RSemaphore semaphore;

        // Prevent two clients from  creating the serverProcess at the same
        // time.
        if ( ( ret = semaphore.CreateGlobal( 
                KFlexTimerSemaphoreName, KPassFirstWaitEntry ) ) != KErrNone )
            {
            ret = semaphore.OpenGlobal( KFlexTimerSemaphoreName );
            }

        if ( ret == KErrNone )
            {
            semaphore.Wait();

            // See again if the server is already started. This doublechecking
            // is necessary, since two or processes may have resolved the
            // first findserver before any server was created.

            // TFindServer sets its content only when instantiated.
            // Just using findServer.Next() does not work in here.
            TFindServer findServerAgain( KFlexTimerServerName );
            if ( findServerAgain.Next( serverName ) != KErrNone )
                {
                // Load the executable for the server.
                ret = serverProcess.Create( KFlexTimerServerExe,
                                            serverParameters,
                                            EOwnerThread );

                if ( ret == KErrNone )
                    {
                    // Server has been created successfully. It is initially 
                    // in suspended state. Now resume the server process.
                    serverProcess.Resume();

                    // Wait until the server process has been started.
                    TRequestStatus status;
                    serverProcess.Rendezvous( status );
                    User::WaitForRequest( status );

                    // Check if server has panicked during initialization.
                    ret = serverProcess.ExitType();
                    if ( ret == EExitPanic )
                        {
                        OstTrace1( TRACE_NORMAL, DUP1_RFLEXTIMER_STARTSERVER,
                                "RFlexTimer::StartServer;this=%x; "
                                "ERROR: Server paniced",
                                ( TUint )this );

                        ret = KErrServerTerminated;
                        }
                    else
                        {
                        ret = status.Int();
                        }

                    // The server process stands on its own. This handle can
                    // be closed. 
                    serverProcess.Close();
                    }
                else
                    {
                    OstTrace1( TRACE_NORMAL, DUP2_RFLEXTIMER_STARTSERVER,
                            "RFlexTimer::StartServer;this=%x; "
                            "ERROR: Server creation failed",
                            ( TUint )this );
                    }
                }
            
            semaphore.Signal();
            semaphore.Close();
            }
        }

    return ret;
    }

