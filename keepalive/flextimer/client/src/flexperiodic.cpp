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
 * Description:  Implementation of CFlexPeriodic class
 *
 */
/*
 * %version: 1 %
 */
// System include files
// User include files go here:
#include "flexperiodic.h"
#include "flextimercommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "flexperiodicTraces.h"
#endif

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexPeriodic* CFlexPeriodic::NewL( TInt aPriority )
    {

    CFlexPeriodic* self = new ( ELeave ) CFlexPeriodic( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructs the object.
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexPeriodic::~CFlexPeriodic()
    {
    OstTrace1( TRACE_NORMAL, CFLEXPERIODIC_CFLEXPERIODIC,
            "CFlexPeriodic::~CFlexPeriodic;this=%x", this );

    }

// ---------------------------------------------------------------------------
// Starts the periodic timer. 32-bit delay and interval parameters.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexPeriodic::Start( TTimeIntervalMicroSeconds32 aDelay,
                                    TTimeIntervalMicroSeconds32 anInterval,
                                    TCallBack aCallBack,
                                    TCallBack aCallBackError )
    {
    OstTraceExt4( TRACE_NORMAL, CFLEXPERIODIC_START32,
            "CFlexPeriodic::Start32;this=%x;aDelay=%d;"
            "anInterval=%d;aCallBack=%x", ( TUint )this,
            aDelay.Int(), anInterval.Int(), ( TUint )&( aCallBack ) );

    TTimeIntervalMicroSeconds32 zero( 0 );
    __ASSERT_ALWAYS(aDelay >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicDelayLessThanZero));
    __ASSERT_ALWAYS(anInterval > zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicIntervalTooSmall));
    __ASSERT_ALWAYS( aCallBack.iFunction != NULL,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicCallbackFunctionIsNull));
    // aCallBackError is left unasserted on purpose.
    // if error occurs and callback is null client is paniced.

    // Interval value is saved for later use, delay is sent immediately
    // to the server.
    iInterval = MAKE_TINT64( 0, anInterval.Int() );
    iCallBack = aCallBack;
    iCallBackError = aCallBackError;
    CFlexTimer::After( aDelay );
    }

// ---------------------------------------------------------------------------
// Starts the periodic timer. 64-bit delay and interval parameters.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexPeriodic::Start( TTimeIntervalMicroSeconds aDelay,
                                    TTimeIntervalMicroSeconds anInterval,
                                    TCallBack aCallBack,
                                    TCallBack aCallBackError )
    {
    OstTraceExt4( TRACE_NORMAL, CFLEXPERIODIC_START64,
            "CFlexPeriodic::Start64;this=%x;aDelay=%lld;"
            "anInterval=%lld;aCallBack=%x", ( TUint )this,
            aDelay.Int64(), anInterval.Int64(), ( TUint )&( aCallBack ) );

    TTimeIntervalMicroSeconds zero( 0 );
    __ASSERT_ALWAYS(aDelay >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicDelayLessThanZero));
    __ASSERT_ALWAYS(anInterval > zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicIntervalTooSmall));
    __ASSERT_ALWAYS( aCallBack.iFunction != NULL,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicCallbackFunctionIsNull));
    // aCallBackError is left unasserted on purpose.
    // if error occurs and callback is null client is paniced.
    
    // Interval value is saved for later use, delay is sent immediately
    // to the server.
    iInterval = anInterval.Int64();
    iCallBack = aCallBack;
    iCallBackError = aCallBackError;
    CFlexTimer::After( aDelay );
    }

// ---------------------------------------------------------------------------
// Configures the flex window sizes for both the initial delay and the
// consequent intervals after that.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexPeriodic::Configure(
                                 TTimeIntervalMicroSeconds32 aDelayWindow,
                                 TTimeIntervalMicroSeconds32 aIntervalWindow )
    {

    OstTraceExt3( TRACE_NORMAL, CFLEXPERIODIC_CONFIGURE,
            "CFlexPeriodic::Configure32;this=%x;"
            "aDelayWindow=%d;aIntervalWindow=%d", ( TUint )this,
            aDelayWindow.Int(), aIntervalWindow.Int() );

    TTimeIntervalMicroSeconds32 zero( 0 );
    __ASSERT_ALWAYS(aDelayWindow >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicDelayWindowLessThanZero));
    __ASSERT_ALWAYS(aIntervalWindow >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicIntervalWindowLessThanZero));

    // interval window is saved for later use. Delay window is sent
    // immediately to server. 
    TInt ret = CFlexTimer::Configure( aDelayWindow );
    if ( ret == KErrNone )
        {
        // Interval window is changed only, if configuration is successful.
        iIntervalWindow = MAKE_TINT64( 0, aIntervalWindow.Int() );
        iSendConfigure = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Configures the flex window sizes for both the initial delay and the
// consequent intervals after that. 64-bit version.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFlexPeriodic::Configure( 
                                   TTimeIntervalMicroSeconds aDelayWindow,
                                   TTimeIntervalMicroSeconds aIntervalWindow )
    {
    OstTraceExt3( TRACE_NORMAL, DUP1_CFLEXPERIODIC_CONFIGURE,
            "CFlexPeriodic::Configure64;this=%x;"
            "aDelayWindow=%lld;aIntervalWindow=%lld", ( TUint )this,
            aDelayWindow.Int64(), aIntervalWindow.Int64() );

    TTimeIntervalMicroSeconds zero( 0 );
    __ASSERT_ALWAYS(aDelayWindow >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicDelayWindowLessThanZero));
    __ASSERT_ALWAYS(aIntervalWindow >= zero,
            User::Panic(KCFlexPeriodicPanicCat,
                    EFlexPeriodicIntervalWindowLessThanZero));

    // interval window is saved for later use. Delay window is sent
    // immediately to server. 
    TInt ret = CFlexTimer::Configure( aDelayWindow );
    if ( ret == KErrNone )
        {
        // Interval window is changed only, if configuration is successful.
        iIntervalWindow = aIntervalWindow;
        
        // This is set to true only, if the server is able to receive the
        // delay window configuration.
        iSendConfigure = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Handles the active objects request completion event.
// ---------------------------------------------------------------------------
//
void CFlexPeriodic::RunL()
    {
    if ( KErrNone == iStatus.Int() )
        {
        if ( iSendConfigure )
            {
            OstTrace1( TRACE_NORMAL, CFLEXPERIODIC_RUNL,
                    "CFlexPeriodic::RunL;this=%x", ( TUint )this );
    
            CFlexTimer::Configure( iIntervalWindow );
            // Reset the iSendConfigure to false so that this is sent only once.
            iSendConfigure = EFalse;
            }
        CFlexTimer::After( iInterval );
        iCallBack.CallBack();
        }
    // Error happended in server. timer is not restarted and
    // iCallBackError is called to inform user
    else
        {
        if ( NULL != iCallBackError.iFunction )
            {
            iCallBackError.CallBack();
            }
        // User has not defined iCallBackError
        // Panic client.
        else
            {
            OstTrace1( TRACE_NORMAL, CFLEXPERIODIC_RUNL_PANIC,
            "CFlexPeriodic::RunL;this=%x;"
            "Error returned from FlexTimerServer and no Error CallBack"
            " is defined. Panicing client.", ( TUint )this );
            User::Panic( KCFlexPeriodicPanicCat,
                        EFlexPeriodicErrorCallbackFunctionIsNull );
            }
        }
    }
// ---------------------------------------------------------------------------
// Second part of the two-phase construction.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFlexPeriodic::ConstructL()
    {
    CFlexTimer::ConstructL();

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Private constructor with priority.
// ---------------------------------------------------------------------------
//
EXPORT_C CFlexPeriodic::CFlexPeriodic( TInt aPriority ) :
    CFlexTimer( aPriority ), iSendConfigure( EFalse )
    {

    }
