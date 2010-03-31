/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM ConnMon request handling
*
*/

/**
@file mpmconnmonreqs.cpp
Mobility Policy Manager ConnMon request handling.
*/

// INCLUDE FILES
#include "mpmconnmonreqs.h"
#include "mpmlogger.h"
#include "mpmserversession.h"
#include "mpmiapselection.h"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::NewL
// -----------------------------------------------------------------------------
//
CMPMConnMonReqs* CMPMConnMonReqs::NewL(CMPMConnMonEvents& aParent,
                                       RConnectionMonitor& aConnMon,
                                       TUint aConnId, 
                                       CMPMServerSession& aSession )
    {
    CMPMConnMonReqs* self = new (ELeave) CMPMConnMonReqs(
                                aParent, aConnMon, aConnId, aSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CMPMConnMonReqs::CMPMConnMonReqs
// -----------------------------------------------------------------------------
//
CMPMConnMonReqs::CMPMConnMonReqs(CMPMConnMonEvents& aParent,
                                 RConnectionMonitor& aConnMon,
                                 TUint aConnId, 
                                 CMPMServerSession& aSession )
    : CActive(CActive::EPriorityStandard), 
      iParent(aParent), 
      iConnMon(aConnMon), 
      iConnId(aConnId), 
      iSession(aSession), 
      iWlanScanCallback( EWlanScanCallbackNone ), 
      iConnectionCount( 0 )
    {
    }


// -----------------------------------------------------------------------------
// CMPMConnMonReqs::~CMPMConnMonReqs
// -----------------------------------------------------------------------------
//
CMPMConnMonReqs::~CMPMConnMonReqs()
    {
    MPMLOGSTRING( "CMPMConnMonReqs::~CMPMConnMonReqs" )
    Cancel();
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::ConstructL()
    {
    MPMLOGSTRING( "CMPMConnMonReqs::ConstructL" )
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::AvailableIapsSync
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::AvailableIapsSync()
    {
    MPMLOGSTRING( "CMPMConnMonReqs::AvailableIapsSync" )
    iNextState = EGetIapAvailState;
    iLastCancelCode = EConnMonGetPckgAttribute;
    iConnMon.GetPckgAttribute( EBearerIdAll, 0, KIapAvailability,
                               iIapBuf, iStatus);
    SetActive();
    
    // Since CodeScanner does not recognise method IsStarted(), 
    // it claims that here is a problem:
    // 
    // Problem: Active object called without checking 
    // whether it is active or cancelling it first.
    // 
    if ( !iActiveSchedulerWait.IsStarted() )
        {
        iActiveSchedulerWait.Start();
        }
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::RefreshAvailableIAPs
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::RefreshAvailableIAPs( TWlanScanCallback aCallback, 
                                            TInt aForceRefreshIntervalSeconds )
    {
    MPMLOGSTRING2( "CMPMConnMonReqs::RefreshAvailableIAPs callback %d", aCallback )
    // set the type of callback to determine afterwards which function to call
    iWlanScanCallback = aCallback;
    iNextState      = EScanWLANNetworksState;   // Default value
    iLastCancelCode = EConnMonGetPckgAttribute;

    TTime time;
    time.UniversalTime();
    TTimeIntervalSeconds interval( 0 );
    
    TInt ret( KErrNone );
        
    // Calculate the elapsed time.
    ret = time.SecondsFrom( iParent.GetIAPRefreshTime(), interval );
    if ( ret != KErrNone )
        {
        MPMLOGSTRING2( "CMPMConnMonReqs::RefreshAvailableIAPs interval error %d ", ret )
        // problem with interval, force iapavailability.
        interval = 0;
        aForceRefreshIntervalSeconds = 0;
        }
    // Default behavior is to query KIapAvailability from ConnMon always
    // or if the availability information has aged at least aForceRefreshIntervalSeconds
    //
    if ( aForceRefreshIntervalSeconds == 0 || 
       ( interval.Int() >= aForceRefreshIntervalSeconds ) )
        {
        // Discard availability notifications from Connection Monitor 
        // until MPM initiated WLAN scan request has completed.
        // 
        iParent.EnableDiscarding();

        MPMLOGSTRING2( "CMPMConnMonReqs::RefreshAvailableIAPs interval %d, executing scan", 
            interval.Int() )
        iConnMon.GetPckgAttribute( EBearerIdAll, 0, KIapAvailability,
                                   iIapBuf, iStatus );
        SetActive();
        }
    // With consecutive calls we allow MPM to use cached value.
    //
    else
        {
        MPMLOGSTRING2( "CMPMConnMonReqs::RefreshAvailableIAPs interval %d, using cached value",
            interval.Int() )

        // iIapBuf is empty, thus we need a new state.
        iNextState = EScanWLANNetworksStateCached;
        TRequestStatus* statusPtr;
        statusPtr = &iStatus;
        // We can just complete the request.
        SetActive();
        User::RequestComplete( statusPtr, KErrNone );
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonReqs::CompareConnIds
// -----------------------------------------------------------------------------
//
TBool CMPMConnMonReqs::CompareConnIds( const CMPMConnMonReqs& aReq1,
                                       const CMPMConnMonReqs& aReq2 )
    {
    if ( aReq1.iConnId == aReq2.iConnId )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::MyCancel()
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::MyCancel()
    {
    // Handle cancellation here.
    MPMLOGSTRING( "CMPMConnMonReqs::MyCancel()" )
    ASSERT( iNextState != EGetIapAvailState && iNextState != EObsolete );
    // Allow the availability update when the connmon request completes.
    iNextState = EObsolete;
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::ErrorCallbackL()
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::ErrorCallbackL( TInt aStatus )
    {
    switch ( iNextState )
        {
        case EGetIapAvailState:
        case EGetConnectionCountState: 
            {
            MPMLOGSTRING( "CMPMConnMonReqs::ErrorCallbackL: EGetIapAvailState failed" )
            if ( iActiveSchedulerWait.IsStarted() )
                {
                iActiveSchedulerWait.AsyncStop();
                }
            break;
            }
        case EScanWLANNetworksState:
        case EScanWLANNetworksStateCached:
            {
            MPMLOGSTRING( "CMPMConnMonReqs::ErrorCallbackL: EScanWLANNetworksState failed" )

            // Disable discarding availability notifications from Connection Monitor.
            // 
            iParent.DisableDiscarding(); 

            // check which callback function to use and continue
            // handling the request if possible.
            // 
            if( iWlanScanCallback == EWlanScanCallbackChooseIap )
                {
                iSession.IapSelectionL()->ChooseIapWLANScanCompletedL( aStatus );
                }
            else if( iWlanScanCallback == EWlanScanCallbackProcessErr )
                {
                iSession.ProcessErrorWlanScanCompletedL();
                }
            else if( iWlanScanCallback == EWlanScanCallbackCarrierRejected )
                {
                // No fresh IAP info available but try to select new IAP
                // based on whatever existing info there is
                // 
                iSession.CompleteCarrierRejected();
                }
            else if( iWlanScanCallback == EWlanScanCallbackSortSnap )
                {
                // SortSnap was called with old availability information.
                iSession.CompleteServerSortSNAP();
                }
            else
                {
                MPMLOGSTRING(
                    "CMPMConnMonReqs::ErrorCallbackL: iWlanScanCallback EWlanScanCallbackNone" )
                }
            // reset callback type
            //
            iWlanScanCallback = EWlanScanCallbackNone;
            break;
            }
        case EObsolete:
            MPMLOGSTRING( "CMPMConnMonReqs::ErrorCallbackL: EObsolete" )
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::RunL
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::RunL()
    {
    MPMLOGSTRING( "CMPMConnMonReqs::RunL" )
    if ( iStatus != KErrNone )
        {
        // Nok
        MPMLOGSTRING2( "CMPMConnMonReqs::RunL: \
ConnMon request completed with error code = %i", iStatus.Int() )

        // Does nothing if state is obsolete.
        ErrorCallbackL( iStatus.Int() );

        // RemoveReqPtr is safe if "this" doesn't exist
        iParent.RemoveReqPtr( this );
        delete this;
        }
    else
        {
        // Ok
        switch (iNextState)
            {
            case EGetIapAvailState:
                {
                MPMLOGSTRING( "CMPMConnMonReqs::RunL: EGetIapAvailState" )
                iParent.SetAvailableIAPs( iIapBuf() );

                if ( iActiveSchedulerWait.IsStarted() )
                    {
                    iActiveSchedulerWait.AsyncStop();
                    }

                iNextState      = EGetConnectionCountState;
                iLastCancelCode = EConnMonGetConnectionCount;
                iConnMon.GetConnectionCount( iConnectionCount, iStatus );
                SetActive();
                break;
                }
            case EGetConnectionCountState: 
                {
                MPMLOGSTRING( "CMPMConnMonReqs::RunL: EGetConnectionCountState" )
                MPMLOGSTRING2( "CMPMConnMonReqs::RunL: Connection Count: %i", 
                    iConnectionCount )
                iParent.SetConnectionCounter( iConnectionCount );
                iParent.RemoveReqPtr( this );
                delete this;
                break;
                }
            case EScanWLANNetworksState:
            case EScanWLANNetworksStateCached:
                {
                MPMLOGSTRING( "CMPMConnMonReqs::RunL: EScanWLANNetworksState and EScanWLANNetworksStateCached" )
                if ( iNextState == EScanWLANNetworksState )
                    {
                    MPMLOGSTRING( "CMPMConnMonReqs::RunL: EScanWLANNetworksState fresh data available from ConnMon" )
                    iParent.SetAvailableIAPs( iIapBuf() );

                    // Disable discarding availability notifications from Connection Monitor.
                    // 
                    iParent.DisableDiscarding(); 

                    // Generate event only if we got new iaps from ConnMon.
                    // 
                    if ( iWlanScanCallback == EWlanScanCallbackGenerateEvent )
                        {
                        MPMLOGSTRING( "CMPMConnMonReqs::RunL: iWlanScanCallback EWlanScanCallbackGenerateEvent" )
                        // Generate event only if not 
                        //
                        iParent.IapAvailabilityChange( EConnMon );
                        }
                    }
                // check which callback function to use
                // 
                if( iWlanScanCallback == EWlanScanCallbackChooseIap )
                    {
                    iSession.IapSelectionL()->ChooseIapWLANScanCompletedL( iStatus.Int() );
                    }
                else if( iWlanScanCallback == EWlanScanCallbackProcessErr )
                    {
                    iSession.ProcessErrorWlanScanCompletedL();
                    }
                else if( iWlanScanCallback == EWlanScanCallbackCarrierRejected )
                    {
                    iSession.CompleteCarrierRejected();
                    }
                else if( iWlanScanCallback == EWlanScanCallbackSortSnap )
                    {
                    iSession.CompleteServerSortSNAP();
                    }
                else
                    {
                    MPMLOGSTRING(
                    "CMPMConnMonReqs::RunL: iWlanScanCallback default processing" )
                    }
                // reset callback type
                //
                iWlanScanCallback = EWlanScanCallbackNone;
                
                iParent.RemoveReqPtr( this );
                delete this;
                break;
                }
            case EObsolete:
                iParent.SetAvailableIAPs( iIapBuf() );
                delete this;
                break;
            default:
                {
                MPMLOGSTRING( "CMPMConnMonReqs::RunL: default" )
                PanicServer( KErrNotSupported );
                break;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMConnMonReqs::RunError
// -----------------------------------------------------------------------------
//

TInt CMPMConnMonReqs::RunError( TInt aError )
    {
    MPMLOGSTRING2(
        "CMPMConnMonReqs::RunError: RunL made a leave with error = %i", 
        aError )

    // Disable discarding availability notifications from Connection Monitor.
    // 
    iParent.DisableDiscarding(); 

    // check which callback function was used when leave occurred
    // 
    if( iWlanScanCallback == EWlanScanCallbackChooseIap )
        {
        iSession.ChooseIapComplete( aError, NULL );
        }
    else if( iWlanScanCallback == EWlanScanCallbackProcessErr )
        {
        TBMNeededAction neededAction( EPropagateError );
        iSession.ProcessErrorComplete( KErrNone, &aError, &neededAction );
        }
    else
        {
        MPMLOGSTRING(
        "CMPMConnMonReqs::RunError: iWlanScanCallback EWlanScanCallbackNone" )
        }

    // We are done
    iParent.RemoveReqPtr( this );
    delete this;

    // Return KErrNone to prevent panic 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPMConnMonReqs::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMConnMonReqs::DoCancel()
    {
    MPMLOGSTRING( "CMPMConnMonReqs::DoCancel" )

    // Disable discarding availability notifications from Connection Monitor.
    // 
    iParent.DisableDiscarding(); 

    iConnMon.CancelAsyncRequest( iLastCancelCode );
    
    if ( iActiveSchedulerWait.IsStarted() )
        {
        iActiveSchedulerWait.AsyncStop();
        }
    }

//  End of File
