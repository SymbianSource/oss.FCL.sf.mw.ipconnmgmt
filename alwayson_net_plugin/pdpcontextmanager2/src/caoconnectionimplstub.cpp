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
* Description:  Implements the CAOConnectionImpl class
*
*/


// INCLUDE FILES
#include <es_enum.h>
#include <ETELPCKT.H>
#include <in_sock.h>

#include "caoconnectionimplstub.h"
#include "logger.h"
#include "maoconnectionobserver.h"
#include "pdpcontextmanagerinternalpskeys.h"
#include "caoasyncwrapper.h"

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
    _LIT( KPanicCat, "ConnImplStub" );
    const TInt KDelay = 2000000; // 2 seconds
    
    // DATA TYPES
    enum TPanicReason
        {
        EAlreadyConnected,
        ENotActive,
        EInvalidState
        };
    
    // LOCAL FUNCTIONS
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
    }

// METHODS

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAOConnectionImpl* CAOConnectionImpl::NewL(
    MAOConnectionObserver& aObserver,
    MAOSettings&           aSettings,
    RSocketServ*           aSocketServ  )
    {
    LOG_1( _L("CAOConnectionImpl::NewL") );
    
    CAOConnectionImpl* self = new( ELeave ) CAOConnectionImpl(
        aObserver,
        aSettings,
        aSocketServ );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnectionImpl::~CAOConnectionImpl()
    {
    LOG_1( _L("CAOConnectionImpl::~CAOConnectionImpl") );
    
    Cancel();
    delete iConnectionTimer;
    
    delete iPDPContextActivationSubscriber;
    iPDPContextActivationProperty.Delete(
        KConnectionCat,
        KPDPContextActivation );
    iPDPContextActivationProperty.Close();
    
    delete iPDPContextDisconnectionSubscriber;
    iPDPContextDisconnectionProperty.Delete(
        KConnectionCat,
        KPDPContextDisconnection );
    iPDPContextDisconnectionProperty.Close();
    }


// ---------------------------------------------------------------------------
// CAOConnectionImpl::ActivateConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ActivateConnection()
    {
    LOG_1( _L("CAOConnectionImpl::ActivateConnection") );
    
    __ASSERT_DEBUG( !IsActive() && iState == EDisconnected,
        Panic( EAlreadyConnected ) );

    // Change state -> EOpeningLink
    ChangeState( EOpeningLink );
    
    // Activate connection
    DoActivateConnection();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::AttachToExistingConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::AttachToExistingConnection( TUint /*aConnectionId*/ )
    {
    LOG_1( _L("CAOConnectionImpl::AttachToExistingConnection") );
         
    __ASSERT_DEBUG( !IsActive() && iState == EDisconnected,
        Panic( EAlreadyConnected ) );
        
    // Change state -> EAttachingExistingConnection
    ChangeState( EAttachingExistingConnection );
    
    // Activate connection
    DoActivateConnection();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::CancelConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::CancelConnection()
    {
    LOG_1( _L("CAOConnectionImpl::CancelConnection") );
    
    Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::IsConnectedToDefaultIAP
// ---------------------------------------------------------------------------
//
TBool CAOConnectionImpl::IsConnectedToDefaultIAP()
    {
    LOG_1( _L("CAOConnectionImpl::IsConnectedToDefaultIAP") );
    
    return iState != EDisconnected;
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::NumberOfConnections
// ---------------------------------------------------------------------------
//
TInt CAOConnectionImpl::NumberOfConnections()
    {
    LOG_1( _L("CAOConnectionImpl::NumberOfConnections") );
    
    return 0;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnectionImpl::CAOConnectionImpl(
    MAOConnectionObserver& aObserver,
    MAOSettings&           aSettings,
    RSocketServ*           aSocketServ  ):
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iSettings( aSettings ),
    iSocketServ( aSocketServ )
    {
    LOG_1( _L("CAOConnectionImpl::CAOConnectionImpl") );
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ConstructL()
    {
    LOG_1( _L("CAOConnectionImpl::ConstructL") );
    
    iConnectionTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    
    // Define PDP context activation property
    User::LeaveIfError( iPDPContextActivationProperty.Define(
        KConnectionCat,
        KPDPContextActivation,
        RProperty::EInt ) );
    User::LeaveIfError( iPDPContextActivationProperty.Attach(
        KConnectionCat,
        KPDPContextActivation ) );
    iPDPContextActivationSubscriber =
        CAOAsyncWrapper<CAOConnectionImpl>::NewL(
            this,
            PDPContextActivationIssueRequest,
            PDPContextActivationRunL,
            PDPContextActivationDoCancel,
            NULL );
    SubscribePDPContextActivation();

    // Define PDP context activation property
    User::LeaveIfError( iPDPContextDisconnectionProperty.Define(
        KConnectionCat,
        KPDPContextDisconnection,
        RProperty::EInt ) );
    User::LeaveIfError( iPDPContextDisconnectionProperty.Attach(
        KConnectionCat,
        KPDPContextDisconnection ) );
    iPDPContextDisconnectionSubscriber =
        CAOAsyncWrapper<CAOConnectionImpl>::NewL(
            this,
            PDPContextDisconnectionIssueRequest,
            PDPContextDisconnectionRunL,
            PDPContextDisconnectionDoCancel,
            NULL );
    SubscribePDPContextDisconnection();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::ChangeState
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ChangeState( TInternalState aNewState )
    {
    TInternalState old = iState;
    iState = aNewState;
    LOG_3( _L("CAOConnectionImpl::ChangeState >> Changing state %d -> %d"),
        old, iState );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::CompleteSelf
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::CompleteSelf( TRequestStatus& aStatus, TInt aReason )
    {
    LOG_2( _L("CAOConnectionImpl::CompleteSelf: aReason: %d"), aReason );
    
    __ASSERT_DEBUG( IsActive(), Panic( ENotActive ) );
    
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, aReason );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::ConnectionCallBack
// ---------------------------------------------------------------------------
//
TInt CAOConnectionImpl::ConnectionCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOConnectionImpl::ConnectionCallBack") );
    
    CAOConnectionImpl* self = static_cast<CAOConnectionImpl*>( aSelf );
    TRAPD( err, self->HandleConnectionCallBackL() );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleConnectionCallBackL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleConnectionCallBackL()
    {
    LOG_1( _L("CAOConnectionImpl::HandleConnectionCallBackL") );
    
    iConnectionTimer->Cancel();
    CompleteSelf( iStatus, iActivationError );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::DoActivateConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::DoActivateConnection()
    {
    LOG_1( _L("CAOConnectionImpl::DoActivateConnection") );
    
    TCallBack cb( ConnectionCallBack, this );
    iConnectionTimer->Start( KDelay, KDelay, cb );
    iStatus = KRequestPending;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleOpeningLinkL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleOpeningLinkL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionImpl::HandleOpeningLinkL: aStatus: %d"),
            aStatus );
    
    if ( aStatus == KErrNone )
        {
        // Everything ok - Link opened
        BroadcastPDPContextActivatedL();
        }
    else
        {
        // Report error
        BroadcastPDPContextActivationFailureL( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::FailureReason
// ---------------------------------------------------------------------------
//
MAOConnectionManager::TFailureReason CAOConnectionImpl::FailureReason(
    TInt aError )
    {
    LOG_2( _L("CAOConnectionImpl::FailureReason: aError: %d"), aError );
    
    // Determine error cause
    MAOConnectionManager::TFailureReason reason =
        MAOConnectionManager::EPermanent;
    switch ( aError )
        {
        // Temporary cause
        case KErrGprsUserAuthenticationFailure:
        case KErrGprsActivationRejectedByGGSN:
        case KErrGprsActivationRejected:
        case KErrGprsServiceOptionTemporarilyOutOfOrder:
            {
            reason = MAOConnectionManager::ETemporary;
            break;
            }
        // Permanent cause
        default:
            {
            reason = MAOConnectionManager::EPermanent;
            break;
            }
        }
        
    LOG_2( _L("CAOConnectionImpl::FailureReason: Reason: %d"), reason );
            
    return reason;
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleListeningAccessPointL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleListeningAccessPointL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionImpl::HandleListeningAccessPointL: aStatus: %d"),
        aStatus );
    
    if ( aStatus == KErrNone )
        {
        // Everything ok - Continue listening
        iStatus = KRequestPending;
        SetActive();
        }
    else
        {
        // Report error
        BroadcastPDPContextDisconnectionL( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::BroadcastPDPContextActivationFailureL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::BroadcastPDPContextActivationFailureL( TInt aError )
    {
    LOG_2( _L("CAOConnectionImpl::BroadcastPDPContextActivationFailureL: \
        aError: %d"), aError );
    
    ChangeState( EDisconnected );
    iObserver.HandlePDPContextActivationFailedL(
        FailureReason( aError ) );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::BroadcastPDPContextActivationFailureL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::BroadcastPDPContextDisconnectionL( TInt aError )
    {
    LOG_2( _L("CAOConnectionImpl::BroadcastPDPContextDisconnectionL: \
        aError: %d"), aError );
        
    ChangeState( EDisconnected );
    iObserver.HandlePDPContextDisconnectedL(
        MAOConnectionManager::ETemporary );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleAttachingExistingConnectionL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleAttachingExistingConnectionL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionImpl::HandleAttachingExistingConnectionL:\
        aStatus: %d"), aStatus );
    
    if ( aStatus == KErrNone )
        {
        // Broadcast PDP context activated
        BroadcastPDPContextActivatedL();
        }
    else
        {
        // Report error
        BroadcastPDPContextActivationFailureL( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::SubscribePDPContextActivation
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::SubscribePDPContextActivation()
    {
    LOG_1( _L("CAOConnectionImpl::SubscribePDPContextActivation") );

    iPDPContextActivationSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextActivationIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextActivationIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionImpl::PDPContextActivationIssueRequest") );
    
    iPDPContextActivationProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextActivationRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextActivationRunL( TInt aStatus )
    {
    LOG_1( _L("CAOConnectionImpl::PDPContextActivationIssueRequest") );
    
    if ( aStatus == KErrNone )
        {
        TInt value = 0;
        TInt err = iPDPContextActivationProperty.Get( value );
        if ( err == KErrNone )
            {
            iActivationError = value;
            if ( IsActive() &&
                ( iState == EOpeningLink || 
                  iState == EAttachingExistingConnection ) )
                {
                CompleteSelf( iStatus, iActivationError );
                }
            }
        }
    SubscribePDPContextActivation();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextActivationDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextActivationDoCancel()
    {
    LOG_1( _L("CAOConnectionImpl::PDPContextActivationDoCancel") );
    
    iPDPContextActivationProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::SubscribePDPContextDisconnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::SubscribePDPContextDisconnection()
    {
    LOG_1( _L("CAOConnectionImpl::SubscribePDPContextDisconnection") );
    
    iPDPContextDisconnectionSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextDisconnectionIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextDisconnectionIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionImpl::PDPContextDisconnectionIssueRequest") );
    
    iPDPContextDisconnectionProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextDisconnectionRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextDisconnectionRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionImpl::PDPContextDisconnectionRunL: aStatus: %d"),
        aStatus );
    
    if ( aStatus == KErrNone )
        {
        TInt value = 0;
        TInt err = iPDPContextDisconnectionProperty.Get( value );
        if ( err == KErrNone )
            {
            iDisconnectionError = value;
            if ( IsActive() && iState == EListeningAccessPoint )
                {
                CompleteSelf( iStatus, iDisconnectionError );
                }
            }
        }
    SubscribePDPContextDisconnection();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::PDPContextDisconnectionDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::PDPContextDisconnectionDoCancel()
    {
    LOG_1( _L("CAOConnectionImpl::PDPContextDisconnectionIssueRequest") );
    
    iPDPContextDisconnectionProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::BroadcastPDPContextActivatedL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::BroadcastPDPContextActivatedL()
    {
    LOG_1( _L("CAOConnectionImpl::BroadcastPDPContextActivatedL") );
    
    ChangeState( EListeningAccessPoint );
    iStatus = KRequestPending;
    SetActive();
    iObserver.HandlePDPContextActivatedL();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::RunL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::RunL()
    {
    LOG_2( _L("CAOConnectionImpl::RunL: iStatus: %d"), iStatus.Int() );

    TInt status = iStatus.Int();
    switch ( iState )
        {
        case EDisconnected:
            {
            // Nothing to do
            break;
            }
        case EOpeningLink:
            {
            HandleOpeningLinkL( status );
            break;
            }
        case EListeningAccessPoint:
            {
            HandleListeningAccessPointL( status );
            break;
            }
        case EAttachingExistingConnection:
            {
            HandleAttachingExistingConnectionL( status );
            break;
            }
        default:
            {
            // Should not be here
            Panic( EInvalidState );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::DoCancel()
    {
    LOG_1( _L("CAOConnectionImpl::DoCancel") );
    
    ChangeState( EDisconnected );
    CompleteSelf( iStatus, KErrCancel );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CAOConnectionImpl::RunError( TInt aError )
    {
    LOG_1( _L("CAOConnectionImpl::RunError") );
    
    return aError;
    }

