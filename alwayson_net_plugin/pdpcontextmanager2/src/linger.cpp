/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module implements linger functionality for not-always-on 
*                connections.
*
*/


#include "linger.h"
#include "logger.h"
#include "maosettings.h"

const TInt KSecondsToMicro         = 1000000; // one second
const TInt KDataInactivityInterval = 30;      // 30 seconds
const TInt KMaxTimerInSeconds      = 1800;    // 30 minutes


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CLingerConnection::CLingerConnection
// ---------------------------------------------------------------------------
//
CLingerConnection::CLingerConnection( TUint           aConnectionId,
                                      TConnectionInfo aConnectionInfo,
                                      MAOSettings&    aSettings, 
                                      RSocketServ*    aSocketServ) :
    iConnectionId( aConnectionId ),
    iConnectionInfo( aConnectionInfo ),
    iSettings( aSettings ),
    iSocketServ( aSocketServ ),
    iLingering( EFalse ),
    iAttached( EFalse ),
    iDlData( 0 ),
    iPckgDlData( iDlData ),
    iUlData( 0 ),
    iPckgUlData( iUlData ),
    iTimer( NULL ), 
    iLingerInterval( 0 ), 
    iLingerTimerCount( 0 ),
    iCurrentTimerInterval( 0 ) 
    {
    }


// ---------------------------------------------------------------------------
// CLingerConnection::ConstructL
// ---------------------------------------------------------------------------
//
void CLingerConnection::ConstructL()
    {
    LOG_1( _L("CLingerConnection::ConstructL") );         
    
    // Create timer
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }


// ---------------------------------------------------------------------------
// CLingerConnection::NewL
// ---------------------------------------------------------------------------
//
CLingerConnection* CLingerConnection::NewL( TUint           aConnectionId,
                                            TConnectionInfo aConnectionInfo, 
                                            MAOSettings&    aSettings, 
                                            RSocketServ*    aSocketServ)
    {
    LOG_1( _L("CLingerConnection::NewL") );
    
    CLingerConnection* self = CLingerConnection::NewLC( aConnectionId,
                                                        aConnectionInfo,
                                                        aSettings, 
                                                        aSocketServ );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CLingerConnection::NewLC
// ---------------------------------------------------------------------------
//
CLingerConnection* CLingerConnection::NewLC( TUint           aConnectionId,
                                             TConnectionInfo aConnectionInfo,
                                             MAOSettings&    aSettings, 
                                             RSocketServ*    aSocketServ)
    {
    LOG_1( _L("CLingerConnection::NewLC") );
    
    CLingerConnection* self = new( ELeave ) CLingerConnection ( aConnectionId,
                                                                aConnectionInfo,
                                                                aSettings, 
                                                                aSocketServ );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ~CLingerConnection
// ---------------------------------------------------------------------------
//
CLingerConnection::~CLingerConnection()
    {
    LOG_1( _L("CLingerConnection::~CLingerConnection") );
    
    // Cancel timer
    CancelTimer();
    
    // Delete timer
    delete iTimer;
    
    // Close RConnection object
    CloseConnection();
    }

// ---------------------------------------------------------------------------
// CLingerConnection::StartLinger
// ---------------------------------------------------------------------------
//
TInt CLingerConnection::StartLinger()
    {
    LOG_1( _L("CLingerConnection::StartLinger") );
    
    if ( !iAttached )
        {    
        // Read settings    
        ReadSettings();
         
        if ( iLingerInterval != 0 )
            {
            TInt err = OpenAndAttach();

            if ( err != KErrNone )
                {
                // Write to log
                LOG_2( _L("OpenAndAttach: err: %d"), err );    
                return( err );
                }
        
            if ( iLingerInterval > 0 )
                {
                // Start timer
                StartTimer( KDataInactivityInterval );
            
                LOG_1( _L("Linger timer started OK") );    
                }
            
            }
        }
        
    return KErrNone;                  
    }
 
// ---------------------------------------------------------------------------
// CLingerConnection::StopLinger
// ---------------------------------------------------------------------------
//
void CLingerConnection::StopLinger()
    {
    LOG_1( _L("CLingerConnection::StopLinger") );
    
    if ( iAttached )
        {    
        CancelTimer();
        CloseConnection();           
        iLingering = EFalse;
        
        LOG_1( _L("Linger timer stopped") );
        }      
    }
    
// ---------------------------------------------------------------------------
// CLingerConnection::StopConnection
// ---------------------------------------------------------------------------
//
TInt CLingerConnection::StopConnection()
    {
    LOG_1( _L("CLingerConnection::StopConnection") );
    
    if ( !iAttached )
        {
        return KErrNotSupported;
        }
    
    // Must first stop connection    
    TInt err = iConnection.Stop( RConnection::EStopAuthoritative );

    // Stop linger & close handles
    if ( err == KErrNone )
        {
        StopLinger();
        }
        
    return err;                
    }
    
// ---------------------------------------------------------------------------
// CLingerConnection::HandleSettingsChanged
// ---------------------------------------------------------------------------
//
void CLingerConnection::HandleSettingsChanged()
    {
    LOG_1( _L("CLingerConnection::HandleSettingsChangedL") );
    
    TInt oldLingerInterval( iLingerInterval );
    
    ReadSettings();
   
    if ( iLingerInterval != oldLingerInterval )
        {
        if ( iLingerInterval == 0 )
            {
            // Linger was turned off
            StopLinger();
            return; 
            }
        
        if ( iLingering )
            {          
            if ( iLingerInterval > 0 )
                {
                // Linger timer has a new positive value
                CancelTimer();
                StartTimer( iLingerInterval );
                }
            else
                {
                // Linger timer has a new negative value
                // -> linger forever
                CancelTimer();
                iLingering = EFalse; 
                }        
            }
        else if ( iAttached )
            {
            if ( ( oldLingerInterval > 0 ) && ( iLingerInterval < 0 ) )
                {
                // Linger timer has a new negative value
                // -> linger forever
                CancelTimer();     
                }
            else if ( ( oldLingerInterval < 0 ) && ( iLingerInterval > 0 ) )
                {
                // Linger timer has a new positive value
                CancelTimer();
                StartTimer( KDataInactivityInterval );
                }      
            }    
        else // not attached, not lingering
            {
            StartLinger();
            }        
        }
    }
    
// ---------------------------------------------------------------------------
// CLingerConnection::Status
// ---------------------------------------------------------------------------
//
TInt CLingerConnection::Status()
    {        
    if ( iLingering )
        {
        return ELingerRunning;    
        }
    else if ( iAttached )
        {
        return ELingerAttached;    
        }
    else
        {
        return ELingerStopped;    
        }    
    }

// ---------------------------------------------------------------------------
// CLingerConnection::OpenAndAttach
// ---------------------------------------------------------------------------
//
TInt CLingerConnection::OpenAndAttach()
    {
    LOG_1( _L("CLingerConnection::OpenAndAttach") );
    
    if ( !iAttached )
        {        
        // Open
        TInt err = iConnection.Open( *iSocketServ, KAfInet );

        if ( err != KErrNone )
            {
            return ( err );
            }
    
        // Attach to keep the connection open
        err = iConnection.Attach( TPckg< TConnectionInfo >( iConnectionInfo ), 
                                  RConnection::EAttachTypeNormal );
                              
        if ( err != KErrNone )
            {
            iConnection.Close();
            return ( err );
            }
        
        iAttached = ETrue;    
        }
            
    return KErrNone;    
    }
        
// ---------------------------------------------------------------------------
// CLingerConnection::CloseConnection
// ---------------------------------------------------------------------------
//
void CLingerConnection::CloseConnection()
    {
    LOG_1( _L("CLingerConnection::CloseConnection") );
    
    // Close RConnection object
    if ( iAttached )
        {
        iConnection.Close();
        
        // To really close: re-open & re-close
        TInt err = iConnection.Open( *iSocketServ, KAfInet );

        if ( err == KErrNone )
            {
            iConnection.Close();
            }
            
        iAttached = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CLingerConnection::IsConnectionIdle
// ---------------------------------------------------------------------------
//
TBool CLingerConnection::IsConnectionIdle()
    {
    LOG_1( _L("CLingerConnection::IsConnectionIdle") );
    
    TUint currentDlData( iDlData );
    TUint currentUlData( iUlData );
    
    if ( !iAttached )
        {
        return ETrue;
        }
        
    LOG_2( _L("old Uplink data volume: %d"), currentUlData );
    LOG_2( _L("old Downlink data volume: %d"), currentDlData );    
               
    // get new data volumes from ESock
    iDlData = 0;
    iUlData = 0;
    
    TRequestStatus status( KErrNone );
           
    iConnection.DataTransferredRequest( iPckgUlData, iPckgDlData, status );
    
    User::WaitForRequest( status );
    
    LOG_2( _L("new Uplink data volume: %d"), iUlData );
    LOG_2( _L("new Downlink data volume: %d"), iDlData );    
    
    if ( status == KErrNone )
        {
        if ( ( iUlData == currentUlData ) && ( iDlData == currentDlData  ) )
            {
            return ETrue;    
            }
        else
            {
            return EFalse;    
            }    
        }
    else
        {
        return EFalse;   
        }    
    }

// ---------------------------------------------------------------------------
// CLingerConnection::ReadSettings
// ---------------------------------------------------------------------------
//
void CLingerConnection::ReadSettings()
    {
    LOG_1( _L("CLingerConnection::ReadSettings") );
    
    iLingerInterval = iSettings.LingerTimerValue( iConnectionInfo.iIapId );
    }


// ---------------------------------------------------------------------------
// CLingerConnection::StartTimer()
// ---------------------------------------------------------------------------
//
void CLingerConnection::StartTimer( TInt aTimerInterval, TBool aReset )
    {
    LOG_1( _L("CLingerConnection::StartTimer") );
    
    if ( aReset )
        {
        iLingerTimerCount = 0;
        }
    
    TCallBack cb( TimerCallBack, this );

    // if linger timer is negative we should not start timer but linger forever...
    if ( iLingerInterval > 0 )
        {
        LOG_2( _L("iTimer->Start: aTimerInterval: %d"), aTimerInterval );
        
        if( aTimerInterval > KMaxTimerInSeconds )
            {
            // Maximum allowed interval is 30 minutes.
            // This restriction comes from TTimeIntervalMicroSeconds32
            iCurrentTimerInterval = KMaxTimerInSeconds;
            }
        else
            {
            // use current value
            iCurrentTimerInterval = aTimerInterval;
            }    
    
        LOG_2( _L("iTimer->Start: iCurrentTimerInterval: %d"), iCurrentTimerInterval );
        CancelTimer();
        iTimer->Start( ( iCurrentTimerInterval * KSecondsToMicro ), 
                       ( iCurrentTimerInterval * KSecondsToMicro ), 
                       cb );    
        }
    }

// ---------------------------------------------------------------------------
// CLingerConnection::CancelTimer()
// ---------------------------------------------------------------------------
//
void CLingerConnection::CancelTimer()
    {
    LOG_1( _L("CLingerConnection::CancelTimer") );
    
    if( iTimer->IsActive() )
        {
        iTimer->Cancel();  
        }
    }

// ---------------------------------------------------------------------------
// CLingerConnection::TimerCallBack
// ---------------------------------------------------------------------------
//
TInt CLingerConnection::TimerCallBack( TAny* aSelf )
    {
    LOG_1( _L("CLingerConnection::TimerCallBack") );
    
    CLingerConnection* self = static_cast< CLingerConnection* >( aSelf );
    
    if ( self->iLingering )
        {
        // Maximum allowed timer interval is 30 min
        // This restriction comes from TTimeIntervalMicroSeconds32 
        self->iLingerTimerCount++;
        
        TInt currentlyRanInSec = 
        self->iCurrentTimerInterval + ( ( self->iLingerTimerCount - 1 ) * KMaxTimerInSeconds );
         
        if( self->iLingerInterval != currentlyRanInSec )
            {
            // Get remaining time in seconds
            TInt timeRemainingInSec = self->iLingerInterval - currentlyRanInSec;
                
            // Check if remaining time is longer than 30 minutes
            if ( timeRemainingInSec > KMaxTimerInSeconds )
                {
                // Current interval is 30 minutes
                self->iCurrentTimerInterval = KMaxTimerInSeconds;
                }
            else
                {
                // use time that is left
                self->iCurrentTimerInterval = timeRemainingInSec;
                }
            
            // Restart timer to reach iLingerInterval    
            self->iTimer->Cancel();
            self->StartTimer( self->iCurrentTimerInterval, EFalse );
            
            return  KErrNone;       
            }
        }        
    
    if ( self->IsConnectionIdle() )
        {
        if ( self->iLingering )
            {
            // Connection has been idle during lingering period e.g. 60 min.
            // Stop connection.
            TInt err = self->StopConnection();
    
            if ( err == KErrNone )
                {
                self->iTimer->Cancel();
                self->iLingering = EFalse;
                }    
            }
        else
            {
            // Connection has been idle during data inactivity period e.g 30 s. 
            // Start lingering timer.
            self->iTimer->Cancel();
            self->StartTimer( self->iLingerInterval );
            self->iLingering = ETrue;   
            }    
        }
    else
        {
        // There was data trasfer through the connection
        if ( self->iLingering )
            {
            // Start monitoring data transfer with KDataInactivityInterval
            // if connection was already in lingering mode. 
            self->iTimer->Cancel();
            self->StartTimer( KDataInactivityInterval );
            self->iLingering = EFalse;   
            }        
        }    
    
    return KErrNone;
    }
