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
#include "caoconnectionmonitorimplstub.h"
#include "logger.h"
#include "maoconnectionmonitorobserver.h"
#include "maosettings.h"
#include "pdpcontextmanagerinternalpskeys.h"
#include "caoasyncwrapper.h"
#include "maoconnectionmanager.h"

// CONSTANTS
const TInt KDelay = 1;

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
    CleanupStack::Pop( self ); // self
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnectionMonitorImpl::~CAOConnectionMonitorImpl()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::~CAOConnectionMonitorImpl") );
    
    delete iNWRegStatusChangeSubscriber;
    iNWRegStatusChangeProperty.Delete(
        KConnectionMonitorCat,
        ENetworkNotRegistered );
    iNWRegStatusChangeProperty.Close();
    
    delete iExternalPDPContextSubscriber;
    iExternalPDPContextProperty.Delete(
        KConnectionMonitorCat,
        KExternalPDPContextCreated );
    iExternalPDPContextProperty.Close();
    
    delete iNewPDPContextSubscriber;
    iNewPDPContextProperty.Delete(
        KConnectionMonitorCat,
        KExternalPDPContextCreated );
    iNewPDPContextProperty.Close();
    
    delete iNWStatusFetcher;
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::FetchNetworkRegistrationStatus" ) );
    
    TCallBack cb( NWRegStatusFetchCallBack, this );
    iNWStatusFetcher->Start( KDelay, KDelay, cb );
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
    MAOConnectionMonitorObserver& aObserver,
    MAOSettings&                  aSettings ):
    iObserver( aObserver ),
    iSettings( aSettings ),
    iWNRegPckg( iWNReg ),
    iWNRegChangePckg( iWNChangeReg )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::CAOConnectionMonitorImpl") );
    
    iWNReg.iRegStatus = CTelephony::ENotRegisteredNoService;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ConstructL()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::ConstructL") );
    
    // Define NW reg status change property
    User::LeaveIfError( iNWRegStatusChangeProperty.Define(
        KConnectionMonitorCat,
        KNetworkRegStatusChange,
        RProperty::EInt ) );
    User::LeaveIfError( iNWRegStatusChangeProperty.Attach(
        KConnectionMonitorCat,
        KNetworkRegStatusChange ) );
    iNWRegStatusChangeSubscriber =
        CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
            this,
            NWChangePropertyIssueRequest,
            NWChangePropertyRunL,
            NWChangePropertyDoCancel,
            NULL );
    SubscribeNWChangeProperty();
    
    // Define external PDP context property
    User::LeaveIfError( iExternalPDPContextProperty.Define(
        KConnectionMonitorCat,
        KExternalPDPContextCreated,
        RProperty::EInt ) );
    User::LeaveIfError( iExternalPDPContextProperty.Attach(
        KConnectionMonitorCat,
        KExternalPDPContextCreated ) );
    iExternalPDPContextSubscriber =
        CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
            this,
            ExternalPDPContextIssueRequest,
            ExternalPDPContextRunL,
            ExternalPDPContextDoCancel,
            NULL );
    SubscribeExternalPDPContextProperty();
    
    // Define new PDP context property
    User::LeaveIfError( iNewPDPContextProperty.Define(
        KConnectionMonitorCat,
        KNewPDPContextCreated,
        RProperty::EInt ) );
    User::LeaveIfError( iNewPDPContextProperty.Attach(
        KConnectionMonitorCat,
        KNewPDPContextCreated ) );
    iNewPDPContextSubscriber =
        CAOAsyncWrapper<CAOConnectionMonitorImpl>::NewL(
            this,
            NewPDPContextIssueRequest,
            NewPDPContextRunL,
            NewPDPContextDoCancel,
            NULL );
    SubscribeNewPDPContextProperty();
    
    iNWStatusFetcher = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::SubscribeNWChangeProperty
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::SubscribeNWChangeProperty()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::SubscribeNWChangeProperty") );
    
    iNWRegStatusChangeSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWChangePropertyIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWChangePropertyIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWChangePropertyIssueRequest") );
    
    iNWRegStatusChangeProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWChangePropertyRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWChangePropertyRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NWChangePropertyRunL: aStatus: %d"),
        aStatus );
        
    if ( aStatus == KErrNone )
        {
        TInt value = 0;
        TInt err = iNWRegStatusChangeProperty.Get( value );
        if ( err == KErrNone )
            {
            CTelephony::TRegistrationStatus status =
                CTelephony::ERegistrationUnknown;
            switch ( value )
                {
                case ENetworkNotRegistered:
                    {
                    status = CTelephony::ENotRegisteredNoService;
                    break;
                    }
                case ENetworkHome:
                    {
                    status = CTelephony::ERegisteredOnHomeNetwork;
                    break;
                    }
                case ENeworkRoaming:
                    {
                    status = CTelephony::ERegisteredRoaming;
                    break;
                    }
                default:
                    {
                    // Nothing do
                    break;
                    }
                }
            iWNReg.iRegStatus = status;
            iObserver.HandleNWRegistrationStatusChangedL( iWNReg.iRegStatus );
            }
        }
    SubscribeNWChangeProperty();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWChangePropertyDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NWChangePropertyDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWChangePropertyDoCancel") );
    
    iNWRegStatusChangeProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::SubscribeExternalPDPContextProperty
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::SubscribeExternalPDPContextProperty()
    {
    LOG_1( _L(
        "CAOConnectionMonitorImpl::SubscribeExternalPDPContextProperty") );
    
    iExternalPDPContextSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::ExternalPDPContextIssueRequest
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ExternalPDPContextIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::ExternalPDPContextIssueRequest") );
    
    iExternalPDPContextProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::ExternalPDPContextRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ExternalPDPContextRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::ExternalPDPContextRunL: \
        aStatus: %d"),
        aStatus );
        
    if ( aStatus == KErrNone )
        {
        // Value is not relevant
        //iObserver.HandleNewConnectionCreatedL( 1, ETrue );
        }
    SubscribeExternalPDPContextProperty();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::ExternalPDPContextDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::ExternalPDPContextDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::ExternalPDPContextDoCancel") );
    
    iExternalPDPContextProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::SubscribeNewPDPContextProperty
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::SubscribeNewPDPContextProperty()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::SubscribeNewPDPContextProperty") );
    
    iNewPDPContextSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::SubscribeNewPDPContextProperty
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NewPDPContextIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NewPDPContextIssueRequest") );
    
    iNewPDPContextProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NewPDPContextRunL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NewPDPContextRunL( TInt aStatus )
    {
    LOG_2( _L("CAOConnectionMonitorImpl::NewPDPContextRunL: aStatus: %d"),
        aStatus );
        
    if ( aStatus == KErrNone )
        {
        // Value is not relevant
        //iObserver.HandleNewConnectionCreatedL( 1, EFalse );
        }
    SubscribeNewPDPContextProperty();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NewPDPContextDoCancel
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::NewPDPContextDoCancel()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NewPDPContextDoCancel") );
    
    iNewPDPContextProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::NWRegStatusFetchCallBack
// ---------------------------------------------------------------------------
//
TInt CAOConnectionMonitorImpl::NWRegStatusFetchCallBack( TAny* aSelf )
    {
    LOG_1( _L("CAOConnectionMonitorImpl::NWRegStatusFetchCallBack") );
    
    CAOConnectionMonitorImpl* self = static_cast<CAOConnectionMonitorImpl*>(
        aSelf );
    TRAPD( err, self->HandleNWRegStatusFetchedL() );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CAOConnectionMonitorImpl::HandleNWRegStatusFetchedL
// ---------------------------------------------------------------------------
//
void CAOConnectionMonitorImpl::HandleNWRegStatusFetchedL()
    {
    LOG_1( _L("CAOConnectionMonitorImpl::HandleNWRegStatusFetchedL") );

    iNWStatusFetcher->Cancel();
    iObserver.HandleNWRegistrationStatusChangedL( iWNReg.iRegStatus );
    }
