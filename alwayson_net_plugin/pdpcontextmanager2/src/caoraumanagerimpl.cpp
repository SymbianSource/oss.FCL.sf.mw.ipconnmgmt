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
* Description:  Implements the CAORAUManagerImpl class
*
*/


// INCLUDE FILES
#include "caoraumanagerimpl.h"
#include "logger.h"
#include "maoraumanagerobserver.h"

//  UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
#ifdef __ALWAYS_ON_CUSTOM_API
namespace
    {
#ifdef _DEBUG
    // CONSTANTS
    _LIT( KPanicCat, "RAUManager" );
    
    // DATA TYPES
    enum TPanicReason
        {
        ENoReason,
        EAlreadyActive
        };
    
    // LOCAL FUNCTIONS
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
       
#endif // _DEBUG 
    }
#endif // __ALWAYS_ON_CUSTOM_API 

//    METHODS

#ifdef __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::NewL
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl* CAORAUManagerImpl::NewL(
    MAORAUManagerObserver& aObserver,
    RMmCustomAPI& aCustomAPI )
    {
    LOG_1( _L("CAORAUManagerImpl::NewL") );
    
    CAORAUManagerImpl* self = new( ELeave ) CAORAUManagerImpl(
        aObserver,
        aCustomAPI );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
#else

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::NewL
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl* CAORAUManagerImpl::NewL( MAORAUManagerObserver& aObserver )
    {
    LOG_1( _L("CAORAUManagerImpl::NewL") );
    
    CAORAUManagerImpl* self = new( ELeave ) CAORAUManagerImpl( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }
    
#endif //  __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::~CAORAUManagerImpl
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl::~CAORAUManagerImpl()
    {
    LOG_1( _L("CAORAUManagerImpl::~CAORAUManagerImpl") );
    
    Cancel();
    }

#ifdef __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::CAORAUManagerImpl
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl::CAORAUManagerImpl(
    MAORAUManagerObserver& aObserver,
    RMmCustomAPI& aCustomAPI ):
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iCustomAPI( aCustomAPI )
    {
    LOG_1( _L("CAORAUManagerImpl::CAORAUManagerImpl") );
    
    CActiveScheduler::Add( this );
    }
#else

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::CAORAUManagerImpl
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl::CAORAUManagerImpl( MAORAUManagerObserver& aObserver ):
    CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    LOG_1( _L("CAORAUManagerImpl::CAORAUManagerImpl") );
    
    CActiveScheduler::Add( this );
    }
    
#endif //  __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::ConstructL()
    {
    LOG_1( _L("CAORAUManagerImpl::ConstructL") );

    IssueRAUNotifications();
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::RunL
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::RunL()
    {
    LOG_2( _L("CAORAUManagerImpl::RunL: iStatus: %d"), iStatus.Int() );
    
#ifdef __ALWAYS_ON_CUSTOM_API

    if ( iStatus == KErrNone )
        {
        MAORAUManagerObserver::TRAUType rau;
        switch ( iRAUStatus )
            {
            case RMmCustomAPI::ERauEventActive:
                {
                rau = MAORAUManagerObserver::ENormal;
                break;
                }
            case RMmCustomAPI::ERauEventResumed:
                {
                rau = MAORAUManagerObserver::EPeriodical;
                break;
                }
            default:
                {
                rau = MAORAUManagerObserver::ENormal;
                break;
                }
            }
        iObserver.HandleSuccesfulRAUL( rau );
        }
    IssueRAUNotifications();
    
#endif //  __ALWAYS_ON_CUSTOM_API
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::DoCancel
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::DoCancel()
    {
    LOG_1( _L("CAORAUManagerImpl::DoCancel") );
    
#ifdef __ALWAYS_ON_CUSTOM_API
#ifndef __WINS__
    iCustomAPI.CancelAsyncRequest( ECustomNotifyRauEventIPC );
#endif // __WINS__
#endif // __ALWAYS_ON_CUSTOM_API
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::RunError
// ---------------------------------------------------------------------------
//
#ifdef LOGGING_ENABLED
TInt CAORAUManagerImpl::RunError( TInt aError )
#else
TInt CAORAUManagerImpl::RunError( TInt /*aError*/ )
#endif // LOGGING_ENABLED
    {
    LOG_2( _L("CAORAUManagerImpl::RunError: aError: %d"), aError );
    
    IssueRAUNotifications();
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CAORAUManagerImpl::IssueRAUNotifications
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::IssueRAUNotifications()
    {
    LOG_1( _L("CAORAUManagerImpl::IssueRAUNotifications") );
    
#ifdef __ALWAYS_ON_CUSTOM_API
    __ASSERT_DEBUG( !IsActive(), Panic( EAlreadyActive ) );
    
#ifdef __WINS__
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
#else
    iCustomAPI.NotifyRauEvent( iStatus, iRAUStatus );
    SetActive();
#endif // __WINS__
#endif // __ALWAYS_ON_CUSTOM_API
    }

// End of file
