/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens to changes in a Central Repository key 
*               KCurrentCellularDataUsage in Repository KCRUidCmManager.
*/

#include <commsdat.h>
#include <centralrepository.h>
#include <cmmanagerkeys.h>

#include "cenrepobserver.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CAOCenRepObserver::NewL
// -----------------------------------------------------------------------------
//
CAOCenRepObserver* CAOCenRepObserver::NewL( MAOCenRepObserver& aObserver )
    {
    CAOCenRepObserver* self = 
             CAOCenRepObserver::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::NewLC
// -----------------------------------------------------------------------------
//
CAOCenRepObserver* CAOCenRepObserver::NewLC( MAOCenRepObserver& aObserver )
    {
    CAOCenRepObserver* self = 
             new( ELeave ) CAOCenRepObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::~CAOCenRepObserver
// -----------------------------------------------------------------------------
//
CAOCenRepObserver::~CAOCenRepObserver()
    {
    // Cancel outstanding request, if exists
    Cancel();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::CAOCenRepObserver
// -----------------------------------------------------------------------------
//
CAOCenRepObserver::CAOCenRepObserver( MAOCenRepObserver& aObserver )
        :
        CActive( CActive::EPriorityStandard ),
        iObserver( aObserver ),
        iRepository( NULL ), 
        iErrorCounter( 0 )
                
    {
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CAOCenRepObserver::ConstructL()
    {    
    TRAPD( err, iRepository = CRepository::NewL( KCRUidCmManager ) )
     
    if ( err == KErrNone )
        {
        CActiveScheduler::Add( this );
        RequestNotifications();
        }
    else
        {
        LOG_2( _L("FAILED to open KCRUidCmManager repository <%d>"), err ); 
        }
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::RequestNotifications
// -----------------------------------------------------------------------------
//
void CAOCenRepObserver::RequestNotifications()
    {
    LOG_1( _L("CAOCenRepObserver::RequestNotifications") );

    TInt err = iRepository->NotifyRequest( KCurrentCellularDataUsage, iStatus );

    if ( err == KErrNone )
        {
        SetActive();
        }
    else
        {
        LOG_2( _L("ERROR, iRepository->NotifyRequest() %d"), err );	
        }
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::DoCancel
// -----------------------------------------------------------------------------
//
void CAOCenRepObserver::DoCancel()
    {
    iRepository->NotifyCancel( KCurrentCellularDataUsage );
    }

// -----------------------------------------------------------------------------
// CAOCenRepObserver::RunL
// -----------------------------------------------------------------------------
//
void CAOCenRepObserver::RunL()
    {
    LOG_2( _L("CAOCenRepObserver::RunL() %d"), iStatus.Int() );

    if ( iStatus.Int() < KErrNone )
        {
        iErrorCounter++;
        if ( iErrorCounter > KCenRepErrorRetryCount )
            {
            LOG_2( _L("Over %d consecutive errors, stopping notifications permanently"), 
                   KCenRepErrorRetryCount );	
            return;
            }
        }
    else
        {
        iErrorCounter = 0;
        
        TInt value( 0 );
        TInt err = iRepository->Get( KCurrentCellularDataUsage, value );
            
        if ( err == KErrNone )
            {
            LOG_2( _L("KCurrentCellularDataUsage: %d"), value );
                   	
            TRAPD( leaveCode, iObserver.CurrentCellularDataUsageChangedL( value ) )
            
            if ( leaveCode )
                {
                LOG_2( _L("iObserver->CurrentCellularDataUsageChangedL FAILED: %d"), 
                       leaveCode );
                }
            }
        else
            {
            LOG_2( _L("ERROR, iRepository->Get(KCurrentCellularDataUsage) %d"), err );
            }
        }

    RequestNotifications();
    }

// End-of-file
