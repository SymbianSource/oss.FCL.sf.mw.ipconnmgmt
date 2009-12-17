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
#include "caoraumanagerimplstub.h"
#include "logger.h"
#include "raumanagerobserver.h"
#include "pdpcontextmanagerinternalpskeys.h"

// METHODS

#ifdef __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::NewL
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl* CAORAUManagerImpl::NewL( MAORAUManagerObserver& aObserver,
                                            RMmCustomAPI& /*aCustomAPI*/ )
    {
    LOG_1( _L("CAORAUManagerImpl::NewL") );
    
    CAORAUManagerImpl* self = new( ELeave ) CAORAUManagerImpl( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    
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
    CleanupStack::Pop( self ); // self
    
    return self;
    }

#endif // __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::~CAORAUManagerImpl
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl::~CAORAUManagerImpl()
    {
    LOG_1( _L("CAORAUManagerImpl::~CAORAUManagerImpl") );
    
    if ( iRAUEventSubscriber )
        {
        iRAUEventSubscriber->Cancel();
        }
    iRAUEventProperty.Delete( KRAUEventCat, KRAUGenerateEvent );
    iRAUEventProperty.Close();
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::CAORAUManagerImpl
// ---------------------------------------------------------------------------
//
CAORAUManagerImpl::CAORAUManagerImpl( MAORAUManagerObserver& aObserver ):
    iObserver( aObserver )
    {
    LOG_1( _L("CAORAUManagerImpl::CAORAUManagerImpl") );
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::ConstructL()
    {
    LOG_1( _L("CAORAUManagerImpl::ConstructL") );
    
    // Define a property and attatch to it
    User::LeaveIfError( iRAUEventProperty.Define(
        KRAUEventCat,
        KRAUGenerateEvent,
        RProperty::EInt ) );
    User::LeaveIfError(
        iRAUEventProperty.Attach( KRAUEventCat, KRAUGenerateEvent ) );
    
    // Create subscription listener
    iRAUEventSubscriber = CAOAsyncWrapper<CAORAUManagerImpl>::NewL(
        this,
        RAUEventSubscribeIssueRequest,
        RAUEventSubscribeRunL,
        RAUEventSubscribeDoCancel,
        NULL );
        
    // Subscribe RAU event
    SubscribeRAUEvents();
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::SubscribeRAUEvents
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::SubscribeRAUEvents()
    {
    LOG_1( _L("CAORAUManagerImpl::SubscribeRAUEvents") );
    
    iRAUEventSubscriber->IssueRequest();
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::RAUEventSubscribeIssueRequest
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::RAUEventSubscribeIssueRequest(
    TRequestStatus& aStatus )
    {
    LOG_1( _L("CAORAUManagerImpl::RAUEventSubscribeIssueRequest") );
    
    iRAUEventProperty.Subscribe( aStatus );
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::RAUEventSubscribeRunL
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::RAUEventSubscribeRunL( TInt aStatus )
    {
    LOG_2( _L("CAORAUManagerImpl::RAUEventSubscribeRunL: aStatus: %d"),
        aStatus );
    
    if ( aStatus == KErrNone )
        {
        TInt value = 0;
        TInt err = iRAUEventProperty.Get( value );
        if ( err == KErrNone )
            {
            MAORAUManagerObserver::TRAUType rauType =
                MAORAUManagerObserver::ENormal;
            if ( value == EPeriodicalRAU )
                {
                rauType = MAORAUManagerObserver::EPeriodical;
                }
            iObserver.HandleSuccesfulRAUL( rauType );
            }
        }
    SubscribeRAUEvents();
    }

// ---------------------------------------------------------------------------
// CAORAUManagerImpl::SubscribeRAUEvents
// ---------------------------------------------------------------------------
//
void CAORAUManagerImpl::RAUEventSubscribeDoCancel()
    {
    LOG_1( _L("CAORAUManagerImpl::RAUEventSubscribeDoCancel") );
    
    iRAUEventProperty.Cancel();
    }

// End of file
