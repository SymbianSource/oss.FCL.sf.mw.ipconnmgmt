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
* Description:  Implements the CAOConnectionMonitorImpl class
*
*/

// INCLUDE FILES
#include <nifvar.h>
#include "caoconnectionmonitorimpl.h"
#include "maosettings.h"
#include "logger.h"
#include "maoconnectionmonitorobserver.h"
#include "caoasyncwrapper.h"


// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTATNS
#ifdef _DEBUG
    _LIT( KPanicCat, "ConnMon" );
#endif

    const TInt KBearerNotSearched = KErrNotFound;
    
    // DATA TYPES
    enum TPanicReason
        {
        EAlreadyFetchingNWRegistration,
        EAlreadyListeningNwRegistrationChange
        //EAlreadyFetchingBearerType
        };
    
    // LOCAL FUNCTIONS
#ifdef _DEBUG
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
#endif
    }

const TInt KRetryCounter = 10;


// METHODS

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAOConnectionMonitorImpl* CAOConnectionMonitorImpl::NewL(
    MAOConnectionMonitorObserver& aObserver,
    MAOSettings& aSettings )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NewL") );
    
    CAOConnectionMonitorImpl* self = new( ELeave )
        CAOConnectionMonitorImpl( aObserver, aSettings );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnectionMonitorImpl::~CAOConnectionMonitorImpl()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::~CAOConnectionMonitorImpl") );
    
    delete iBearer;
    delete iNWRegistrationStatusChange;
    delete iNWRegistrationStatus;
    delete iTelephony;
    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus" ) );
    
    __ASSERT_DEBUG( !iNWRegistrationStatus->IsActive(),
        Panic( EAlreadyFetchingNWRegistration ) );

    iNWRegistrationStatus->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NetworkRegistration
// ---------------------------------------------------------------------------
//
CTelephony::TRegistrationStatus
    CAOConnectionMonitorImpl::NetworkRegistration() const
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NetworkRegistration") );
    
    return iWNReg.iRegStatus;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnectionMonitorImpl::CAOConnectionMonitorImpl(
    MAOConnectionMonitorObserver& aObserver, MAOSettings& aSettings ):
    iObserver( aObserver ),
    iSettings( aSettings ),
    iTelephony( NULL ),
    iWNRegPckg( iWNReg ),
    iWNRegChangePckg( iWNChangeReg ),
    iNWRegistrationStatus( NULL ),
    iNWRegistrationStatusChange( NULL ),
    iBearer( NULL ), 
    iMyConnectionId( 0 ),
    iConnectionAlive( EFalse ),
    iBearerValue( KBearerNotSearched ),
    iCounter( 0 )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::CAOConnectionMonitorImpl") );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ConstructL()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::ConstructL") );
    
    // Create connection monitor
    iConnectionMonitor.ConnectL();
    User::LeaveIfError( iConnectionMonitor.NotifyEventL( *this ) );
    
    // Create telephony
    iTelephony = CTelephony::NewL();
    
    // Network registration async wrapper
    iNWRegistrationStatus = CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
        this,
        &CAOConnectionMonitorImpl::NWRegStatusIssueRequest,
        &CAOConnectionMonitorImpl::NWRegStatusRunL,
        &CAOConnectionMonitorImpl::NWRegStatusDoCancel,
        &CAOConnectionMonitorImpl::NWRegStatusRunError );
        
    // Network registration status change async wrapper
    iNWRegistrationStatusChange = 
        CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
        this,
        &CAOConnectionMonitorImpl::NWRegStatusChangeIssueRequest,
        &CAOConnectionMonitorImpl::NWRegStatusChangeRunL,
        &CAOConnectionMonitorImpl::NWRegStatusChangeDoCancel,
        &CAOConnectionMonitorImpl::NWRegStatusChangeRunError );
        
    // Bearer type fetcher
    iBearer = CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
        this,
        &CAOConnectionMonitorImpl::BearerIssueRequest,
        &CAOConnectionMonitorImpl::BearerRunL,
        &CAOConnectionMonitorImpl::BearerDoCancel,
        &CAOConnectionMonitorImpl::BearerRunError );
        
    // Start listening NW reg status change
    ListenNetworkRegistrationStatusChange();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::ListenNetworkRegistrationStatusChange
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ListenNetworkRegistrationStatusChange()
    {
    LOG_1( _L(
        "CAOConnectionMonitorImpl::ListenNetworkRegistrationStatusChange" ) );
    
    __ASSERT_DEBUG( !iNWRegistrationStatusChange->IsActive(),
        Panic( EAlreadyListeningNwRegistrationChange ) );

    iNWRegistrationStatusChange->IssueRequest();    
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::HandleError
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::HandleError( TInt aError )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::HandleError" ) );
    
    iObserver.HandleError( aError );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWRegStatusIssueRequest" ) );
    
    iTelephony->GetNetworkRegistrationStatus( aStatus, iWNRegPckg );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NWRegStatusRunL: aStatus: %d" ),
        aStatus );
        
    if ( aStatus == KErrNone )
        {
        LOG_2( _L("> Current iWNReg.iRegStatus: %d"), iWNReg.iRegStatus );
	
        if ( ( iWNReg.iRegStatus < iWNChangeReg.iRegStatus ) && 
        	   ( iCounter < KRetryCounter ) )
            {
            iCounter++;	
            LOG_2( _L("> Different from events iWNChangeReg.iRegStatusv: %d"), 
                   iWNChangeReg.iRegStatus );	
            iNWRegistrationStatus->IssueRequest();	
            }
        else
            {
            iCounter = 0;	
            iObserver.HandleNWRegistrationStatusChangedL( 
                iWNRegPckg().iRegStatus );	
            }        
        } 
    else
        {
        HandleError( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWRegStatusDoCancel" ) );
    
    iTelephony->CancelAsync( 
        CTelephony::EGetNetworkRegistrationStatusCancel );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusRunError
// ---------------------------------------------------------------------------
//
TInt CAOConnectionMonitorImpl::NWRegStatusRunError( TInt aError )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NWRegStatusRunError: aError: %d" ),
        aError );

    HandleError( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusChangeIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusChangeIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWRegStatusChangeIssueRequest" ) );
    
    iTelephony->NotifyChange(
        aStatus,
        CTelephony::ENetworkRegistrationStatusChange,
        iWNRegChangePckg );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusChangeRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusChangeRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NWRegStatusChangeRunL: \
        aStatus: %d" ),
        aStatus );
        
    // Handle event and continue listening
    if ( aStatus == KErrNone )
        {
        LOG_3( _L("> NW reg. old: %d, WN reg. new: %d"),
            iWNReg.iRegStatus, iWNChangeReg.iRegStatus );
            
        // Notify only if the value really changed
        if ( iWNReg.iRegStatus != iWNChangeReg.iRegStatus )
            {
            iWNReg = iWNChangeReg;
            iObserver.HandleNWRegistrationStatusChangedL(
                iWNRegChangePckg().iRegStatus );
            }
        }
    else
        {
        HandleError( aStatus );
        }
    
    // Continue listening
    ListenNetworkRegistrationStatusChange();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusChangeDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWRegStatusChangeDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWRegStatusChangeDoCancel" ) );
    
    iTelephony->CancelAsync(
        CTelephony::ENetworkRegistrationStatusChangeCancel );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusChangeRunError
// ---------------------------------------------------------------------------
//
#ifdef LOGGING_ENABLED
TInt CAOConnectionMonitorImpl::NWRegStatusChangeRunError( TInt aError )
#else
TInt CAOConnectionMonitorImpl::NWRegStatusChangeRunError( TInt /*aError*/ )
#endif
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NWRegStatusChangeRunError: \
        aError: %d" ),
        aError );
    
    // Continue listening
    ListenNetworkRegistrationStatusChange();
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::BearerIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::BearerIssueRequest( TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::BearerIssueRequest" ) );
    
    iBearerValue = KBearerNotSearched;
    
    iConnectionMonitor.GetIntAttribute(
        iMyConnectionId,
        0,
        KBearer,
        iBearerValue,
        aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::CancelFetchBearerType
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::CancelFetchBearerType()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::CancelFetchBearerType" ) );
    
    iBearerValue = KBearerNotSearched;
    iBearer->Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::BearerRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::BearerRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::BearerRunL: aStatus: %d" ), 
            aStatus );
    
    if( aStatus != KErrNone )
        {
        iBearerValue = EBearerUnknown;
        }
    LOG_2( _L("> Bearer type: %d"), iBearerValue );
       
    switch( iBearerValue )
        {
        case EBearerExternalCSD:
        case EBearerExternalWCDMA:
        case EBearerExternalLAN:
        case EBearerExternalCDMA2000:
        case EBearerExternalGPRS:
        case EBearerExternalHSCSD:
        case EBearerExternalEdgeGPRS:
        case EBearerExternalWLAN:
        case EBearerExternalBluetooth:
        case EBearerExternalWcdmaCSD:   
        case EBearerGPRS:
        case EBearerEdgeGPRS:
        case EBearerWCDMA:    
            {
            iConnectionAlive = ETrue;
            iObserver.HandleExternalConnectionCreatedL();    
            
            break;
            }
        
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::BearerDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::BearerDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::BearerDoCancel" ) );

    iConnectionMonitor.CancelAsyncRequest( KBearer );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::BearerRunError
// ---------------------------------------------------------------------------
//
TInt CAOConnectionMonitorImpl::BearerRunError( TInt /*aError*/ )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::BearerRunError" ) );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::FetchBearerType
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::FetchBearerType()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::FetchBearerType" ) );

    iBearer->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::EventL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::EventL
     ( const CConnMonEventBase &aConnMonEvent )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::EventL") );
    LOG_2( _L("> Event type: %d"), aConnMonEvent.EventType() );
    LOG_2( _L("> Connection id: %d"), aConnMonEvent.ConnectionId() );
        
    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonCreateConnection:
            {
            LOG_1( _L("> Connection created") );
            if( iConnectionAlive )
                {
                LOG_1( _L("Connection has been alive! Break") );
                break;
                }
            
            iMyConnectionId = aConnMonEvent.ConnectionId();
            
            // Get iap id
            TUint           iapId( 0 );
            TRequestStatus  status1( KErrNone );
            
            iConnectionMonitor.GetUintAttribute( iMyConnectionId, 
                                                 0, 
                                                 KIAPId, 
                                                 iapId, 
                                                 status1 );
            User::WaitForRequest( status1 );
    
            if ( iSettings.AccessPointId() != iapId )
                {
                // Check the if the connection is external
                if( !iBearer->IsActive() )
                    {
                    FetchBearerType();
                    // iConnectionAlive can be true if the bearer could be retrieved 
                    }
                }
            break;
            }
        case EConnMonDeleteConnection:
            {
            LOG_1( _L("> Connection deleted") );
            
            if( iMyConnectionId != aConnMonEvent.ConnectionId() )
                {
                LOG_1( _L("Other connection was closed Break") );
                  
                iObserver.HandleConnectionDeletedL( aConnMonEvent.ConnectionId(), EFalse );
                break;
                }
            
            if ( iBearer->IsActive() )
                {
                CancelFetchBearerType();    
                }
            
            iConnectionAlive = EFalse;
            iObserver.HandleConnectionDeletedL( aConnMonEvent.ConnectionId(), ETrue );
            break;
            }
            
       case EConnMonConnectionStatusChange:
           {
           const CConnMonConnectionStatusChange* eventStatus; 
           eventStatus  = static_cast<const CConnMonConnectionStatusChange*>( 
                                                         &aConnMonEvent );
           TInt connId = eventStatus->ConnectionId(); 

           if ( eventStatus->ConnectionStatus() == KLinkLayerOpen )
               {
               LOG_1( _L("> Connection status KlinkLayerOpen") );
               	
               TUint iapId( 0 );
               TUint netId( 0 );
               TRequestStatus  status1( KErrNone );
               TRequestStatus  status2( KErrNone );
            
                iConnectionMonitor.GetUintAttribute( connId, 
                                                     0, 
                                                     KIAPId, 
                                                     iapId, 
                                                     status1 );
                User::WaitForRequest( status1 );
		    
		    
                iConnectionMonitor.GetUintAttribute( connId, 
                                                     0, 
                                                     KNetworkIdentifier, 
                                                     netId, 
                                                     status2 );
                User::WaitForRequest( status2 );
		    
                if ( status1 == KErrNone && status2 == KErrNone )
                    {    
                    // if this is a not-always-on iap
                    TInt bearer( 0 );
                    TRequestStatus status( KErrNotReady );
       
                    // Get the bearer info from CM server.
                    iConnectionMonitor.GetIntAttribute( connId, 
                                                        0, 
                                                        KBearer, 
                                                        bearer, 
                                                        status );
                    User::WaitForRequest( status ); 

                    if ( status.Int() != KErrNone ) 
                        {
                        // write to log
                        //LOG( Log::Printf( _L("KBearer FAILED: %d\n"), status.Int()));
                        break;
                        }

                    // Check that it is GPRS or WCDMA.
                    if ( bearer == EBearerGPRS      || 
                         bearer == EBearerEdgeGPRS  ||
                         bearer == EBearerWCDMA )
                        {     
                        // Notify internal packet connections
                        //if ( iSettings.AccessPointId() != iapId )
                        //    {
                        //    iConnectionAlive = ETrue;    
                        //    }
                        
                        iObserver.HandleInternalConnectionCreatedL( connId,
                                                                    iapId, 
                                                                    netId );
                        }  
                    }
                }
            }
        default:
            {
            LOG_1( _L("> Do nothing") );
            // Nothing to do
            break;
            }
        }

    LOG_1( _L("CAOConnectionMonitorImpl::EventL end") );

    }
