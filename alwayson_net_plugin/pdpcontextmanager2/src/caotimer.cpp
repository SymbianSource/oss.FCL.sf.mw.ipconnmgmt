/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CAOTimer class
*
*/


// INCLUDE FILES
#include "caotimer.h"
#include "logger.h"
#include "maotimerobserver.h"
#include "maosettings.h"
#include "pdpcontextmanagerinternalcrkeys.h"

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
#ifdef _DEBUG
    _LIT( KPanicCat, "Timer" );
#endif
    const TInt KSecondsToMicro = 1000000; // one second
    const TInt KMaxTimerInSeconds = 1800; // 30 minutes
    
    // DATA TYPES
    enum TPanicReason
        {
        ERetryTimerAlreadyActive,
        EConnectionTimerAlreadyActive,
        EUnconnectTimerAlreadyActive
        };
    
    // LOCAL FUNCTIONS
#ifdef _DEBUG
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
#endif
    }

// METHODS

// ---------------------------------------------------------------------------
// CAOTimer::NewL
// ---------------------------------------------------------------------------
//
CAOTimer* CAOTimer::NewL( MAOTimerObserver& aObserver,
                          MAOSettings& aSettings )
    {
    LOG_1( _L("CAOTimer::NewL") );
    
    CAOTimer* self = new( ELeave ) CAOTimer( aObserver, aSettings );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CAOTimer::~CAOTimer
// ---------------------------------------------------------------------------
//
CAOTimer::~CAOTimer()
    {
    LOG_1( _L("CAOTimer::~CAOTimer") );
    
    delete iUnconnectTimer;
    delete iConnectionTimer;
    delete iRetryTimer;
    }

// ---------------------------------------------------------------------------
// CAOTimer::HandleSettingsChangedL
// ---------------------------------------------------------------------------
//
void CAOTimer::HandleSettingsChangedL()
    {
    LOG_1( _L("CAOTimer::HandleSettingsChangedL") );
    
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// CAOTimer::StartRetryTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StartRetryTimer()
    {
    LOG_1( _L("CAOTimer::StartRetryTimer") );
    
    __ASSERT_DEBUG( !iRetryTimer->IsActive(),
        Panic( ERetryTimerAlreadyActive ) );
        
    iRetryTimerCount = 0;
    
    // First check if the current retry timer value is more than 30 minutes
    iCurrentRetryTimerInterval = 0;
    if( iSettings.RetryTimerValue() > KMaxTimerInSeconds )
        {
        // First interval is 30 minutes
        iCurrentRetryTimerInterval = KMaxTimerInSeconds * KSecondsToMicro;
        }
    else
        {
        // use current value
        iCurrentRetryTimerInterval =
            iSettings.RetryTimerValue() * KSecondsToMicro;
        }
    
    TCallBack cb( RetryTimerCallBack, this );
    iRetryTimer->Start(
        iCurrentRetryTimerInterval, iCurrentRetryTimerInterval, cb );
    }

// ---------------------------------------------------------------------------
// CAOTimer::StartConnectionTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StartConnectionTimer()
    {
    LOG_1( _L("CAOTimer::StartConnectionTimer") );
    
    __ASSERT_DEBUG( !iConnectionTimer->IsActive(),
        Panic( EConnectionTimerAlreadyActive ) );
        
    TInt interval = ConfigureInterval( iSettings.ConnectionTimerValue() );
    TCallBack cb( ConnectionTimerCallBack, this );
    iConnectionTimer->Start( interval, interval, cb );
    }

// ---------------------------------------------------------------------------
// CAOTimer::StopRetryTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StopRetryTimer()
    {
    LOG_1( _L("CAOTimer::StopRetryTimer") );
    
    iRetryTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CAOTimer::StopConnectionTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StopConnectionTimer()
    {
    LOG_1( _L("CAOTimer::StopConnectionTimer") );
    
    iConnectionTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CAOTimer::StartUnconnectTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StartUnconnectTimer()
    {
    LOG_1( _L("CAOTimer::StartUnconnectTimer") );

    __ASSERT_DEBUG( !iUnconnectTimer->IsActive(),
        Panic( EUnconnectTimerAlreadyActive ) );
    
    TInt interval = ConfigureInterval( iSettings.UnconnectTimerValue() );
    TCallBack cb( UnconnectTimerCallBack, this );
    iUnconnectTimer->Start( interval, interval, cb );
    }

// ---------------------------------------------------------------------------
// CAOTimer::StopUnconnectTimer
// ---------------------------------------------------------------------------
//
void CAOTimer::StopUnconnectTimer()
    {
    LOG_1( _L("CAOTimer::StopUnconnectTimer") );
    
    iUnconnectTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CAOTimer::RetryTimerDisabled
// ---------------------------------------------------------------------------
//
TBool CAOTimer::RetryTimerDisabled() const
    {
    LOG_1( _L("CAOTimer::RetryTimerDisabled") );
    
    return iSettings.RetryTimerValue() == 0;
    }

// ---------------------------------------------------------------------------
// CAOTimer::UnconnectTimerDisabled
// ---------------------------------------------------------------------------
//
TBool CAOTimer::UnconnectTimerDisabled() const
    {
    LOG_1( _L("CAOTimer::UnconnectTimerDisabled") );
    
    if ( iSettings.IsCellularAllowedByUser() )
        {
        return iSettings.UnconnectTimerValue() == 0;
        }
    else
        {
        // There should be no retries because user
        // does not allow cellular connections.
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CAOTimer::CAOTimer
// ---------------------------------------------------------------------------
//
CAOTimer::CAOTimer( MAOTimerObserver& aObserver, MAOSettings& aSettings ):
    iObserver( aObserver ),
    iSettings( aSettings )
    {
    LOG_1( _L("CAOTimer::CAOTimer") );
    }

// ---------------------------------------------------------------------------
// CAOTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CAOTimer::ConstructL()
    {
    LOG_1( _L("CAOTimer::ConstructL") );
    
    // Create timers
    iRetryTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iConnectionTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iUnconnectTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// CAOTimer::RetryTimerCallBack
// ---------------------------------------------------------------------------
//
TInt CAOTimer::RetryTimerCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOTimer::RetryTimerCallBack") );
    
    CAOTimer* self = static_cast<CAOTimer*>( aSelf );
    TRAPD( err, self->HandleRetryTimerCallBackL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CAOTimer::ConnectionTimerCallBack
// ---------------------------------------------------------------------------
//
TInt CAOTimer::ConnectionTimerCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOTimer::ConnectionTimerCallBack") );
    
    CAOTimer* self = static_cast<CAOTimer*>( aSelf );
    self->StopConnectionTimer();
    TRAPD( err, self->iObserver.HandleConnectionTimerExpiredL() );
    return err;
    }
    
// ---------------------------------------------------------------------------
// CAOTimer::HandleRetryTimerCallBackL
// ---------------------------------------------------------------------------
//
void CAOTimer::HandleRetryTimerCallBackL()
    {
    LOG_1( _L("CAOTimer::HandleRetryTimerCallBackL") );
    
    // Stop retry timer and increase timer count
    iRetryTimerCount++;
    StopRetryTimer();
    
    // Check if we still need to continue
    // If retry timer has been set longer than 30min
    TInt currentlyRanInSec = ( iCurrentRetryTimerInterval / KSecondsToMicro ) 
         + ( ( iRetryTimerCount - 1 ) * KMaxTimerInSeconds );
    if( iSettings.RetryTimerValue() != currentlyRanInSec )
        {
        // Get remaining time in seconds
        TInt timeRemainingInSec =
            iSettings.RetryTimerValue() - currentlyRanInSec;
            
        // Check if remaining time is longer than 30 minutes
        if ( timeRemainingInSec > KMaxTimerInSeconds )
            {
            // Current interval is 30 minutes
            iCurrentRetryTimerInterval = KMaxTimerInSeconds * KSecondsToMicro;
            }
        else
            {
            // use time that is left
            iCurrentRetryTimerInterval = timeRemainingInSec * KSecondsToMicro;
            }
            
        // Restart timer
        TCallBack cb( RetryTimerCallBack, this );
        if( !iRetryTimer->IsActive())
            {
            
            iRetryTimer->Start(
                iCurrentRetryTimerInterval, iCurrentRetryTimerInterval, cb );
            }
        }
    else
        {
        // Retry timer expired, notify observer
        iObserver.HandleRetryTimerExpiredL();
        }
    }

// ---------------------------------------------------------------------------
// CAOTimer::UnconnectTimerCallBack
// ---------------------------------------------------------------------------
//
TInt CAOTimer::UnconnectTimerCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOTimer::UnconnectTimerCallBack") );
    
    CAOTimer* self = static_cast<CAOTimer*>( aSelf );
    self->StopUnconnectTimer();
    TRAPD( err, self->iObserver.HandleUnconnectTimerExpiredL() );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CAOTimer::ConfigureInterval
// ---------------------------------------------------------------------------
//
TInt CAOTimer::ConfigureInterval( const TInt& aIntervalInSec )
    {
    LOG_2( _L("CAOTimer::ConfigureInterval: aIntervalInSec: %d"),
        aIntervalInSec );
        
    // Interval maximum value is 30 minutes
    TInt interval = 0;
    if( aIntervalInSec > KMaxTimerInSeconds )
        {
        interval = KMaxTimerInSeconds * KSecondsToMicro;
        }
    else
        {
        interval = aIntervalInSec * KSecondsToMicro;
        }
        
    return interval;
    }

// End of file
