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
* Description:  Implements the CAOGpds class
*
*/


// INCLUDE FILES
#include "caogpds.h"
#include "logger.h"

#include <rmmcustomapi.h>


//  UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
//namespace
//    {
    // CONSTANTS
//    _LIT( KPanicCat, "Gpds" );
    
    // DATA TYPES
//    enum TPanicReason
//        {
//        ENoReason,
//        EAlreadyActive
//        };
    
    // LOCAL FUNCTIONS
//    LOCAL_C void Panic( TPanicReason aReason )
//        {
//        User::Panic( KPanicCat, aReason );
//        }
//    }

//    METHODS

// ---------------------------------------------------------------------------
// CAOGpds::NewL
// ---------------------------------------------------------------------------
//
CAOGpds* CAOGpds::NewL( RMmCustomAPI& aCustomAPI )
    {
    LOG_1( _L("CAOGpds::NewL") );
    
    CAOGpds* self = new( ELeave ) CAOGpds( aCustomAPI );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CAOGpds::~CAOGpds
// ---------------------------------------------------------------------------
//
CAOGpds::~CAOGpds()
    {
    LOG_1( _L("CAOGpds::~CAOGpds") );
    
    Cancel();
    if ( iWait )
        {
        if ( iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
        delete iWait;
        }
    }

// Constructor

// ---------------------------------------------------------------------------
// CAOGpds::CAOGpds
// ---------------------------------------------------------------------------
//
CAOGpds::CAOGpds( RMmCustomAPI& aCustomAPI ):
    CActive( EPriorityStandard ),
    iCustomAPI( aCustomAPI )
    {
    LOG_1( _L("CAOGpds::CAOGpds") );
    
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CAOGpds::ConstructL
// ---------------------------------------------------------------------------
//
void CAOGpds::ConstructL()
    {
    LOG_1( _L("CAOGpds::ConstructL") );
    
    // Create active schduler wiat for synchronizing async operations    
    iWait = new( ELeave ) CActiveSchedulerWait;
    }

// ---------------------------------------------------------------------------
// CAOGpds::RunL
// ---------------------------------------------------------------------------
//
void CAOGpds::RunL()
    {
    LOG_2( _L("CAOGpds::RunL: iStatus: %d"), iStatus.Int() );
    
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CAOGpds::DoCancel
// ---------------------------------------------------------------------------
//
void CAOGpds::DoCancel()
    {
    LOG_1( _L("CAOGpds::DoCancel") );
    
#ifndef __WINS__
    iCustomAPI.CancelAsyncRequest( ECustomSetAlwaysOnMode );
#endif // __WINS__

    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CAOGpds::SetAlwaysOnModeL
// ---------------------------------------------------------------------------
//
void CAOGpds::SetAlwaysOnModeL( TAlwaysOnMode aMode )
    {
    LOG_2( _L("CAOGpds::SetAlwaysOnModeL: aMode: %d"), aMode );
    
    if ( IsActive() )
        {
        return;
        }
        
#ifdef __WINS__
    aMode = aMode;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    
    if( !iWait->IsStarted() )
    {
        iWait->Start();
    }
#else
    RMmCustomAPI::TSetAlwaysOnMode mode( RMmCustomAPI::EAlwaysModeNeither );
    switch ( aMode )
        {
        case EHPLMN:
            {
            mode = RMmCustomAPI::EAlwaysModeHPLMN;
            break;
            }
        case EVPLMN:
            {
            mode = RMmCustomAPI::EAlwaysModeVPLMN;
            break;
            }
        case EBoth:
            {
            mode = RMmCustomAPI::EAlwaysModeBoth;
            break;
            }
        case ENeither:
            {
            mode = RMmCustomAPI::EAlwaysModeNeither;
            break;
            }
        default:
            {
            mode = RMmCustomAPI::EAlwaysModeNeither;
            break;
            }
        }
        
    // Wait here until request has completed
    iCustomAPI.SetAlwaysOn( iStatus, mode );
    LOG_2( _L("iCustomAPI.SetAlwaysOn: mode: %d"), mode );
    
    SetActive();
    if( !iWait->IsStarted() )
    {
        iWait->Start();
    }
    LOG_2( _L("iCustomAPI.SetAlwaysOn: iStatus: %d"), iStatus.Int() );
#endif // __WINS__
    }
        
// End of file
