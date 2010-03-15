/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Handling of the MPM interface.
 *
*/


/**
 @file s60mpmrequests.cpp
 Handling of the MPM interface.
 */

// Project includes
#include "s60mpmrequests.h"
#include "s60mcpr.h"
#include "s60mpmnotifications.h"

// Namespace
using namespace S60MCPRMPMOperations;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMPMPolicyRequests::NewL
// -----------------------------------------------------------------------------
//
CMPMPolicyRequests* CMPMPolicyRequests::NewL( MMPMPolicyNotificationUser& aUser )
    {
    CMPMPolicyRequests* self = new ( ELeave ) CMPMPolicyRequests();
    CleanupStack::PushL( self );
    self->ConstructL( aUser );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::PolicyNotification
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::PolicyNotification( TMpmNotification& aNotification )
    {
    if ( !iCommitedToDeleteItself )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::PolicyNotification() forwarding call tocalling MCPR", this )
        iPolicyNotificationsUser->PolicyNotification( aNotification );
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::RequestDelete
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::RequestDelete()
    {
    iCommitedToDeleteItself = ETrue;
    // Either we have pending requests
    // (we always have at least notifications if we're connected.)
    //
    if ( iPolicyRequests.Count() > 0 )
        {
        S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::RequestDelete() %d pending requests,  IssueRequest", this, iPolicyRequests.Count() )
        IssueRequest();
        }
    // Or we are free to delete this.
    //
    else
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RequestDelete() IssueDeleteRequest", this )
        IssueDeleteRequest();
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CMPMPolicyRequests
// -----------------------------------------------------------------------------
//
CMPMPolicyRequests::CMPMPolicyRequests() :
                                    CActive( EPriorityStandard ),
                                    iCommitedToDeleteItself( EFalse ),
                                    iReconnectTimerRunning( EFalse ),
                                    iReconnectTimeout( KMPMInitialReconnectTimeout )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CMPMPolicyRequests()", this )
    CActiveScheduler::Add( this );
    iConnId = (TConnectionId)this;
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::~CMPMPolicyRequests
// -----------------------------------------------------------------------------
//
CMPMPolicyRequests::~CMPMPolicyRequests()
    {
    //LOG( ESockLog::Printf(KESockMetaConnectionTag, _L8("CMPMPolicyRequests<%x>::~CMPMPolicyRequests"), this ); )
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::~CMPMPolicyRequests()", this )

    __ASSERT_ALWAYS(!IsActive(), User::Panic(KS60MCprPanic, KPanicInvalidCActiveUsage));
    __ASSERT_DEBUG(iPolicyRequests.Count() == 0, User::Panic(KS60MCprPanic, KPanicInvalidCActiveUsage));

    // Clean up all objects in the list
    // Normally there shouldn't be any.
    TInt count = iPolicyRequests.Count();
    for (TInt i = 0; i < count; i++)
        {
        S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::~CMPMPolicyRequests() deleting request %d", 
                           this, (TInt)iPolicyRequests[i].iRequestType )
        // Delete prefs
        delete iPolicyRequests[i].iConnPref;
        delete iPolicyRequests[i].iPolicyPref;
        // PolicyRequest owns this class and can delete it.
        // MCPR must have cancelled its activity when this happens.
        delete iPolicyRequests[i].iUser;
        }

    // Reset the array.
    iPolicyRequests.Close();

    // Close the MPM handle 
    if ( iMpm.Connected() )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::~CMPMPolicyRequests() closing MPM session", this )
        iMpm.Close();
        }
    
    if ( iReconnectTimerRunning )
        {
        Cancel();
        iReconnectTimer.Close();
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ConstructL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ConstructL( MMPMPolicyNotificationUser& aUser )
    {
    // Force MPMNotifications to go through CMPMPolicyRequests
    iPolicyNotificationsUser = &aUser;
    iPolicyNotifications = CMPMPolicyNotifications::NewL( *(MMPMPolicyNotificationUser*)this, 
                                                          iMpm );
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::ConstructL() using iPolicyNotifications<%x>", 
                       this, iPolicyNotifications )
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::IAPConnectionStartedL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::IAPConnectionStartedL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::IAPConnectionStartedL() IAP %d", this, aIapId )
    CreatePolicyRequestL( EPolicyRequestIAPConnectionStarted, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::IAPConnectionStoppedL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::IAPConnectionStoppedL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::IAPConnectionStoppedL() IAP %d", this, aIapId )
    CreatePolicyRequestL( EPolicyRequestIAPConnectionStopped, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationJoinsConnectionL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationJoinsConnectionL( const TUint32 aIapId )
    {
    // Can't use CreatePolicyRequest() here because this could
    // be the first call to MPM.

    PolicyRequest policydata = PolicyRequest();
    policydata.iRequestType = EPolicyRequestApplicationJoinsConnection;
    policydata.iConnId      = iConnId;
    policydata.iIapId       = aIapId;

    // Add request to array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationJoinsConnectionL() IAP %d", this, aIapId )
    
    // Submit it.
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationLeavesConnectionL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationLeavesConnectionL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationLeavesConnectionL() IAP %d", this, aIapId )
    CreatePolicyRequestL( EPolicyRequestApplicationLeavesConnection, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationLeavesConnectionL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationConnectionEndsL()
    {
    // This is only a notification. Thus, if the session doesn't exist, don't create one
    if ( !iMpm.Connected() )
        {
        return;
        }

    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationConnectionEndsL() IAP %d", this )
    
    PolicyRequest policydata = PolicyRequest();
    policydata.iRequestType  = EPolicyRequestApplicationConnectionEnds;
    policydata.iConnId       = iConnId;
//    policydata.iIapId               = aIapId;
    // Nothing to cleanup so it's ok to Leave in AppendL

    // Add request to array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    // Submit it.
    IssueRequest();
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::RegisterPreferredIAPNotificationL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::RegisterPreferredIAPNotificationL()
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RegisterPreferredIAPNotificationL()", this )

    PolicyRequest policydata = PolicyRequest();
    
    policydata.iRequestType  = EPolicyRequestRegisterPrefIAPNotif;
    policydata.iConnId       = iConnId;

    // Add request to array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    // Submit it.
    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::UnregisterPreferredIAPNotificationL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::UnregisterPreferredIAPNotificationL()
    {
    // Sanity check that we're still in business. 
    if ( !iMpm.Connected() )
        {
        return;
        }

    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::UnregisterPreferredIAPNotificationL()", this )

    PolicyRequest policydata = PolicyRequest();
    
    policydata.iRequestType  = EPolicyRequestUnregisterPrefIAPNotif;
    policydata.iConnId       = iConnId;

    // Add request to array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    // Submit it.
    IssueRequest();
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationIgnoredTheCarrierL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationIgnoredTheCarrierL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationIgnoredTheCarrierL()", this )
    CreatePolicyRequestL( EPolicyRequestApplicationIgnoredTheCarrier, aIapId );
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationMigratesToCarrierL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationMigratesToCarrierL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationMigratesToCarrierL()", this )
    CreatePolicyRequestL( EPolicyRequestApplicationMigratesToCarrier, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationRejectedTheCarrierL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationRejectedTheCarrierL( const TUint32 aIapId )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationRejectedTheCarrierL()", this )
    CreatePolicyRequestL( EPolicyRequestApplicationRejectedTheCarrier, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ApplicationAcceptedTheCarrierL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ApplicationAcceptedTheCarrierL( const TUint32 aIapId)
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ApplicationAcceptedTheCarrierL()", this )
    CreatePolicyRequestL( EPolicyRequestApplicationAcceptedTheCarrier, aIapId );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ChooseBestIAP
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ChooseBestIAPL( TConnPref*             aConnPref,
                                        const TUid&             aAppUid,
                                        TConnPref*              aPolicyPref,
                                        MMPMPolicyRequestsUser* aUser )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ChooseBestIAPL()", this )
    
    // Define policydata
    PolicyRequest policydata = PolicyRequest();
    policydata.iRequestType         = EPolicyRequestChooseBestIAP;
    policydata.iConnId              = iConnId;
    policydata.iAppUid              = aAppUid;

    // Heap allocated objects
    policydata.iConnPref            = aConnPref;
    policydata.iPolicyPref          = aPolicyPref;
    policydata.iUser                = aUser;

    // Nothing to cleanup so it's ok to Leave in AppendL
    // Heap allocated objects are on callers responsibility if we Leave.
    
    // Add it to the array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CancelReselectBestIAP()
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::CancelReselectBestIAP()
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelReselectBestIAPL()", this )
    ASSERT( iPolicyRequests.Count() > 0 );
    CancelRequest( EMPMReselectBestIAP );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CancelChooseBestIAPL()
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::CancelChooseBestIAPL()
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelChooseBestIAPL()", this )
    ASSERT( iPolicyRequests.Count() > 0 );
    CancelRequest( EMPMChooseBestIAP );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CancelProcessError()
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::CancelProcessError()
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelProcessErrorL()", this )
    ASSERT( iPolicyRequests.Count() > 0 );
    CancelRequest( EMPMProcessError );
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ReselectIAPL()
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ReselectIAPL( TConnPref* aPolicyPref,
                                       MMPMPolicyRequestsUser* aUser )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::ReselectIAPL()", this )
    //Call MPM to get the list of potential IAPs for the connection
    //
    PolicyRequest policydata = PolicyRequest();

    policydata.iRequestType = EPolicyRequestReselectBestIAP;
    policydata.iConnId      = iConnId;
    policydata.iPolicyPref  = aPolicyPref;
    policydata.iUser        = aUser;

    // Add it to the array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::ProcessErrorL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::ProcessErrorL( TInt& anError,
                                        MMPMPolicyRequestsUser* aUser )
    {
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::ProcessErrorL() %d", this, anError )

    // insert into array of pending policy request
    PolicyRequest policydata = PolicyRequest();

    policydata.iRequestType = EPolicyRequestProcessError;
    policydata.iError       = anError;
    policydata.iConnId      = iConnId;
    policydata.iUser        = aUser;

    // Add it to the array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    IssueRequest();
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::IssueDeleteRequest
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::IssueDeleteRequest()
    {
    ASSERT( !IsActive() );
    // Always cancel & delete policynotifications.
    if ( iMpm.Connected() && iPolicyNotifications->IsActive() )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::IssueDeleteRequest() Asynch", this )
        iPolicyNotifications->CancelRequestAndDestroyD( &iStatus );
        iPolicyNotifications = NULL;
        SetActive();
        }
    else if ( iPolicyNotifications )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::IssueDeleteRequest() Immediate", this )
        iPolicyNotifications->CancelRequestAndDestroyD();
        iPolicyNotifications = NULL;
        delete this;
        }
    else
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::IssueDeleteRequest() Invalid state", this )
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::IssueRequest
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::IssueRequest()
    {
    // If we have a pending request, back off.
    //
    if ( IsActive() || iPolicyRequests.Count() == 0 )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::IssueRequest() Unnecessary call, back off", this )
        return;
        }
    
    if ( !iMpm.Connected() )
        {
        PolicyRequest policydata = PolicyRequest();
        policydata.iRequestType  = EPolicyRequestConnect;
        // OK because default granularity is 8 and this is done when first request arrives.
        iPolicyRequests.Insert(policydata, 0);
        }

    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::IssueRequest() iRequestType %d", this, (TInt)iPolicyRequests[0].iRequestType )
    //
    // else we submit a new one.
    switch ( iPolicyRequests[0].iRequestType )
        {
        case EPolicyRequestConnect:
            {
            iMpm.Connect( iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestChooseBestIAP:
            {
            //LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("\tcall MPM ChooseBestIAP")));
            iMpm.ChooseBestIAP( ( *iPolicyRequests[0].iConnPref ),
                                iPolicyRequests[0].iConnId,
                                iPolicyRequests[0].iAppUid,
                                ( *iPolicyRequests[0].iPolicyPref ),
                                iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestReselectBestIAP:
            {
            iMpm.ReselectBestIAP( iPolicyRequests[0].iConnId,
                                  ( *iPolicyRequests[0].iPolicyPref ),
                                  iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestIAPConnectionStarted:
            {
            iMpm.IAPConnectionStarted( iPolicyRequests[0].iIapId,
                                       iPolicyRequests[0].iConnId,
                                       iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestIAPConnectionStopped:
            {
            iMpm.IAPConnectionStopped( iPolicyRequests[0].iIapId,
                                       iPolicyRequests[0].iConnId,
                                       iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationJoinsConnection:
            {
            iMpm.ApplicationJoinsConnection( iPolicyRequests[0].iIapId,
                                             iPolicyRequests[0].iConnId,
                                             iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationLeavesConnection:
            {
            iMpm.ApplicationLeavesConnection( iPolicyRequests[0].iIapId,
                                              iPolicyRequests[0].iConnId,
                                              iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationConnectionEnds:
            {
            iMpm.ApplicationConnectionEnds( iPolicyRequests[0].iConnId,
                                            iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestRegisterPrefIAPNotif:
            {
            iMpm.RegisterPrefIAPNotif( iPolicyRequests[0].iConnId,
                                       iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestUnregisterPrefIAPNotif:
            {
            iMpm.UnregisterPrefIAPNotif( iPolicyRequests[0].iConnId,
                                         iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationIgnoredTheCarrier:
            {
            iMpm.ApplicationIgnoredTheCarrier( iPolicyRequests[0].iIapId,
                                               iPolicyRequests[0].iConnId,
                                               iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationMigratesToCarrier:
            {
            iMpm.ApplicationMigratesToCarrier( iPolicyRequests[0].iIapId,
                                               iPolicyRequests[0].iConnId,
                                               iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationRejectedTheCarrier:
            {
            iMpm.ApplicationRejectedTheCarrier( iPolicyRequests[0].iIapId,
                                                iPolicyRequests[0].iConnId,
                                                iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestApplicationAcceptedTheCarrier:
            {
            iMpm.ApplicationAcceptedTheCarrier( iPolicyRequests[0].iIapId,
                                                iPolicyRequests[0].iConnId,
                                                iStatus );
            SetActive();
            break;
            }
        case EPolicyRequestProcessError:
            {
            //LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("\tcall MPM ProcessError")));
            iMpm.ProcessError( iPolicyRequests[0].iError, 
                               iPolicyRequests[0].iConnId,
                               iPolicyRequests[0].iNeededAction, 
                               iStatus );
            SetActive();
            break;
            }
        default:
            {
#ifdef _DEBUG
            // In debug builds, panic
            _LIT( KPanicMsg, "CMPMPolicyRequests::IssueRequestL: Invalid case" );
            User::Panic( KPanicMsg, KErrNotSupported );
#endif
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPMPolicyRequests::RunL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::RunL()
    {
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::RunL() iStatus %d", this, iStatus.Int() )

    // At first, check if this is a timer activity
    if ( iReconnectTimerRunning )
        {
        // iMpm.Connect() failed at least once. Timeout has expired, try again.
#ifdef _DEBUG    
        TBool isConnected = iMpm.Connected();
        __ASSERT_DEBUG( !isConnected,  User::Panic( KS60MCprPanic, KErrGeneral ) );
#endif
        iReconnectTimerRunning = EFalse;
        iReconnectTimer.Close();
        iMpm.Connect( iStatus );
        SetActive();
        } 
    else if ( iPolicyRequests.Count() > 0 && !iCommitedToDeleteItself)
        {
        // there's a request waiting and is now completed
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RunL() Standard processing", this )
        PolicyRequest& policydata = iPolicyRequests[0];
        // Process the response
        //
        switch ( policydata.iRequestType )
            {
            case EPolicyRequestConnect:
                {
                if ( iStatus.Int() != KErrNone )
                    {
                    // MPM connection failed, set MPM connection required again.
                    iMpm.Close();
                    // Go to RunError
                    User::Leave( iStatus.Int() );
                    }
                else
                    {
                    // Otherwise MPM ok and start listening for notifications
                    iPolicyNotifications->Activate();
                    }
                break;
                }
            case EPolicyRequestProcessError:
            case EPolicyRequestChooseBestIAP:
            case EPolicyRequestReselectBestIAP:
                {
                if ( policydata.iUser )
                    {
                    // ChooseBest IAP is completed by iUser.
                    //
                    policydata.iStatus = iStatus.Int();
                    policydata.iUser->PolicyResponse( policydata );
                    }
                break;
                }
            // With these messages we ignore the result.
            case EPolicyRequestRegisterPrefIAPNotif:
            case EPolicyRequestUnregisterPrefIAPNotif:
            case EPolicyRequestApplicationIgnoredTheCarrier:
            case EPolicyRequestApplicationMigratesToCarrier:
            case EPolicyRequestApplicationRejectedTheCarrier:
            case EPolicyRequestApplicationAcceptedTheCarrier:
            case EPolicyRequestIAPConnectionStarted:
            case EPolicyRequestIAPConnectionStopped:
            case EPolicyRequestApplicationJoinsConnection:
            case EPolicyRequestApplicationLeavesConnection:
            case EPolicyRequestApplicationConnectionEnds:
                {
#ifdef _DEBUG                
                if ( iStatus.Int() != KErrNone )
                    {
                    S60MCPRLOGSTRING3( "S60MCPR::CMPMPolicyRequests<%x>::RunL() Request %d failed %d", 
                                       this, (TInt) policydata.iRequestType, iStatus.Int() )
                    }
#endif // _DEBUG
                break;
                }
            default:
                {
#ifdef _DEBUG
                // In debug builds, panic
                _LIT( KPanicMsg, "CMPMPolicyRequests::RunL: Invalid case" );
                User::Panic( KPanicMsg, KErrNotSupported );
#endif
                break;
                }
            }
        // Then cleanup
        //
        policydata.Cleanup();
        iPolicyRequests.Remove( 0 ); // remove request from the array
        // And if there are still pending request, cotinue with the next one
        //
        if ( iPolicyRequests.Count() > 0 )
             {
             IssueRequest();
             }
        }
    // We have pending requests but we have commited to delete itself
    // 
    else if ( iPolicyRequests.Count() > 0 && iCommitedToDeleteItself )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RunL() Processing pending requests before delete", this )
        // Submit pending requests to keep MPM in consistent state.
        // First cleanup the old one
        //
        iPolicyRequests[0].Cleanup();
        iPolicyRequests.Remove( 0 ); // remove request from the array
        // And if there are still pending request, cotinue with the next one
        //
        if ( iPolicyRequests.Count() > 0 )
            {
            IssueRequest();
            }
        // In this case we've processed everything and can close.
        //
        else
            {
            IssueDeleteRequest();
            }
        }
    else if ( iPolicyRequests.Count() == 0 && iCommitedToDeleteItself )
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RunL() About to delete itself", this )
        ASSERT( !iPolicyNotifications );
        delete this;
        }
    else
        {
        // INVALID STATE
#ifdef _DEBUG
        // In debug builds, panic
        _LIT( KPanicMsg, "CMPMPolicyRequests::RunL: Invalid state" );
        User::Panic( KPanicMsg, KErrNotSupported );
#endif
        // Even still, make sure we don't leak memory 
        //
        iPolicyNotifications->CancelRequestAndDestroyD();
        iPolicyNotifications = NULL;
        delete this;
        }
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::DoCancel
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::DoCancel()
    {
    _LIT( KPanicMsg, "CMPMPolicyRequests::DoCancel" );
    if ( iReconnectTimerRunning )
        {
        iReconnectTimer.Cancel();
        }
    else
        {
        // Not allowed! Cancel() may cause deadlock!
        User::Panic( KPanicMsg, KErrNotSupported );
        }
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::RunError
// -----------------------------------------------------------------------------
//
TInt CMPMPolicyRequests::RunError( TInt aError )
    {
    S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::RunError()", this )
    // Connect has failed, cleanup and deliver errors
    if ( aError != KErrServerBusy || iReconnectTimeout >= KMPMMaxReconnectTimeout )
        {
        TInt count = iPolicyRequests.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iPolicyRequests[i].iUser )
                {
                S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::RunError() Sending error %d in PolicyResponse", this, aError )
                iPolicyRequests[i].iStatus = aError;
                iPolicyRequests[i].iUser->PolicyResponse( iPolicyRequests[i] );
                }
            
            // Delete prefs
            iPolicyRequests[i].Cleanup();
            }
        iPolicyRequests.Reset();
        }
    else
        {
        // KErrServerBusy received, try reconnecting after a brief pause
        iReconnectTimeout *= KMPMReconnectTimeoutMultiplier;
        TTimeIntervalMicroSeconds32 timeout( iReconnectTimeout );
        iReconnectTimer.CreateLocal();
        iReconnectTimer.After( iStatus, timeout );
        iReconnectTimerRunning = ETrue;
        SetActive();
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CancelRequest
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::CancelRequest( TMpmAsynchCodes aRequestCode )
    {
    // Semantics of asynchronous cancellations:
    //
    // The biggest difference is that asynchronous cancellation will
    // cause RunL() -call for original request. Because of this the 
    // PolicyResponse() calls must be prevented.
    //
    // Asynch Cancellation cannot be queued along with the other requests.
    // Precondition for cancellation call to MPM is that there's an active request
    // and that request code matches to the given parameter.
    //
    // Cancellation of the unsent requests is simple. Just remove it from the queue.
    //
    // Asynchronous cancellation adds message to MPMServer's IPC queue but since we
    // queue all requests we can safely always call the SendCancelRequest()
    
    // If the cancelled request is the active one.
    //
    S60MCPRLOGSTRING2( "S60MCPR::CMPMPolicyRequests<%x>::CancelRequest() %d", this, (TInt)aRequestCode )
    if ( iPolicyRequests[0].iRequestType == aRequestCode )
        {
        // Cancel the query if active
        //
        if ( IsActive() )
            {
            S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelRequest() Cancelling active request", this )
#ifdef _DEBUG
            TInt ret = iMpm.SendCancelRequest( aRequestCode );
#else
            iMpm.SendCancelRequest( aRequestCode );
#endif
            __ASSERT_DEBUG( ret == KErrNone,  User::Panic( KS60MCprPanic, ret ) );
            // Prevent the callback.
            iPolicyRequests[0].Cancel();
            }
        // Completed or not started. Just remove the request.
        //
        else
            {
            S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelRequest() Cancelling non-active request", this )
            iPolicyRequests[0].Cleanup();
            // remove it.
            iPolicyRequests.Remove(0);
            }
        }
    // Else we need to find it from the queue
    //
    else 
        {
        S60MCPRLOGSTRING1( "S60MCPR::CMPMPolicyRequests<%x>::CancelRequest() Cancelling pending request", this )
        TInt count = iPolicyRequests.Count();
        for (TInt i = 1; i < count; i++)
            {
            if ( iPolicyRequests[i].iRequestType == aRequestCode )
                {
                // Cancellable request found, clean it up
                iPolicyRequests[i].Cleanup();
                // remove it.
                iPolicyRequests.Remove(i);
                break; // first occurence is enough.
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPMPolicyRequests::CreatePolicyRequestL
// -----------------------------------------------------------------------------
//
void CMPMPolicyRequests::CreatePolicyRequestL( TPolicyServerOperations aRequestCode, 
                                               const TUint32& aIapId )
    {
    // Sanity check that we're still in business. 
    if ( !iMpm.Connected() )
        {
        return;
        }

    PolicyRequest policydata = PolicyRequest();
    
    policydata.iRequestType         = aRequestCode;
    policydata.iConnId              = iConnId;
    policydata.iIapId               = aIapId;

    // Add request to array of uncompleted requests
    iPolicyRequests.AppendL( policydata );

    // Submit it.
    IssueRequest();
    }

// End of file
