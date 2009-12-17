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
* Description:  Updates P&S variables: KPSUidGprsStatus and KPSUidWcdmaStatus.
*
*/

#include <NIFVAR.H>
#include <e32svr.h>
#include <PSVariables.h>

#include "saeobserver.h"
#include "dcl_log.h"

// Used to internally initialize network status to unknwon -state.
const TInt ENtwkStatusNotKnown( -1 );

// ============================ MEMBER FUNCTIONS ===========================

// -------------------------------------------------------------------------
// CSaeObserver::CSaeObserver
// -------------------------------------------------------------------------
CSaeObserver::CSaeObserver( const RConnectionMonitor& aMonitor ) :
    iMonitor( aMonitor ),
    iNetworkIsUnAttached( ETrue ),
    iNetworkIsSuspended ( EFalse ),
    iCurrentBearerType ( ESAEUnknownConnectionType )
    {
    }

// -------------------------------------------------------------------------
// CSaeObserver::NewL
// -------------------------------------------------------------------------
CSaeObserver* CSaeObserver::NewL( const RConnectionMonitor& aMonitor )
    {
    CSaeObserver* self = new (ELeave) CSaeObserver( aMonitor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -------------------------------------------------------------------------
// CSaeObserver::~CSaeObserver
// -------------------------------------------------------------------------
CSaeObserver::~CSaeObserver()
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::~CSaeObserver() start->" )));
    iGPRSConnections.Close();
    iWCDMAConnections.Close();
    iSAProperty.Close();
    LOG( Log::Printf( _L("DCL: CSaeObserver::~CSaeObserver() end." )));
    }

// -------------------------------------------------------------------------
// CSaeObserver::ConstructL
// -------------------------------------------------------------------------
void CSaeObserver::ConstructL()
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::ConstructL() start  " )));

    // Define PubSub values
    _LIT_SECURITY_POLICY_PASS( KGeneralReadPolicy  );
    _LIT_SECURITY_POLICY_PASS( KGeneralWritePolicy );

    iSAProperty.Define( KUidSystemCategory,
                        KPSUidGprsStatusValue,
                        RProperty::EInt,
                        KGeneralReadPolicy,
                        KGeneralWritePolicy );

    iSAProperty.Define( KUidSystemCategory,
                        KPSUidWcdmaStatusValue,
                        RProperty::EInt,
                        KGeneralReadPolicy,
                        KGeneralWritePolicy );

    iGPRSConnections.Reset();
    iWCDMAConnections.Reset();

    // Initialize PubSub values
    UpdateBearerNetworkStatusL( ESAEGPRSConnectionType );  // GPRS status
    UpdateBearerNetworkStatusL( ESAEWCDMAConnectionType ); // WCDMA status

    LOG( Log::Printf( _L("DCL: CSaeObserver::ConstructL() end." )));
    }

// -------------------------------------------------------------------------
// CSaeObserver::EventL
// Handles events sent by Connection Monitor
// -------------------------------------------------------------------------
void CSaeObserver::EventL( const CConnMonEventBase &aConnMonEvent )
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::EventL() start->" )));

    switch( aConnMonEvent.EventType() )
        {
        case EConnMonDeleteConnection:
            {
            LOG( Log::Printf( _L("DCL: EConnMonDeleteConnection." )));
            const CConnMonDeleteConnection* event =
                          ( CConnMonDeleteConnection* ) &aConnMonEvent;
            HandleEConnMonDeleteConnectionL( *event );
            }
            break;

        case EConnMonNetworkStatusChange:
            {
            LOG( Log::Printf( _L("DCL: EConnMonNetworkStatusChange." )));
            const CConnMonNetworkStatusChange* event =
                        ( CConnMonNetworkStatusChange* ) &aConnMonEvent;
            HandleEConnMonNetworkStatusChangeL( *event );
            }
            break;

        case EConnMonConnectionStatusChange:
            {
            LOG( Log::Printf( _L("DCL: EConnMonConnectionStatusChange." )));
            const CConnMonConnectionStatusChange* event =
                      ( CConnMonConnectionStatusChange* ) &aConnMonEvent;
            HandleEConnMonConnectionStatusChangeL( *event );
            }
            break;

        case EConnMonBearerChange:
            {
            LOG( Log::Printf( _L("DCL: EConnMonBearerChange." )));
            const CConnMonBearerChange* event =
                                 ( CConnMonBearerChange* ) &aConnMonEvent;
            HandleEConnMonBearerChangeL( *event );
            }
            break;

        default:
            // Unhandled event
            LOG( Log::Printf( _L("DCL: Unhandled event type (%d)."),
                 aConnMonEvent.EventType() ));
            break;
        }
    LOG( Log::Printf( _L("DCL: CSaeObserver::EventL() end." )));
    }

// -------------------------------------------------------------------------
// CSaeObserver::HandleEConnMonBearerChangeL
// -------------------------------------------------------------------------
void CSaeObserver::HandleEConnMonBearerChangeL( const CConnMonBearerChange& aEvent )
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::HandleEConnMonBearerChangeL()" )));
    TInt newConnType( aEvent.ConnectionId() ); // Connection ID tells the new bearer type
#ifdef _DEBUG
    TInt oldConnType( ESAEUnknownConnectionType );
#endif
    TInt newSAEConnType(ESAEUnknownConnectionType);
    TInt oldSAEConnType(ESAEUnknownConnectionType);


    // Check what handover we are executing.
    RArray<TUint>* fromArray;

    if ( newConnType == EBearerIdGPRS )
        {
        // No way to check whether we actually allready are at EdgeGPRS...
#ifdef _DEBUG
        oldConnType = EBearerIdWCDMA;
#endif
        newSAEConnType = ESAEGPRSConnectionType;
        oldSAEConnType = iCurrentBearerType;
        fromArray = &iWCDMAConnections;
        }
    else if ( newConnType == EBearerIdWCDMA )
        {
#ifdef _DEBUG
        oldConnType = EBearerIdGPRS;
#endif
        newSAEConnType = ESAEWCDMAConnectionType;
        oldSAEConnType = iCurrentBearerType;
        fromArray = &iGPRSConnections;
        }
    else
        {
        LOG( Log::Printf( _L("DCL: Connection type %d not supported."),
             newConnType ));
        }

    LOG( Log::Printf( _L("DCL: NewConnType %d, OldConnType %d"),
         newSAEConnType, oldSAEConnType ));

    if ( newSAEConnType != ESAEUnknownConnectionType &&
         newSAEConnType != iCurrentBearerType )
        {

        TInt fromArrayCount( fromArray->Count() );
        LOG( Log::Printf( _L("DCL: Handover from %d to %d bearer, %d connections."),
             oldConnType, newConnType, fromArrayCount ));

        --fromArrayCount; // Indexing of array starts at 0...

        // Move all connections from old bearer specific array to the new one...
        while (fromArrayCount >= 0)
            {
            TUint connectionID(0);

            connectionID = (*fromArray)[fromArrayCount];

            LOG( Log::Printf( _L("DCL: Moving Connection ID %d from old array index %d to new array."),
                 connectionID, fromArrayCount ));

            RemoveFromConnArray( STATIC_CAST(TSAEConnectionType, oldSAEConnType), connectionID );
            AddToConnArray( STATIC_CAST(TSAEConnectionType, newSAEConnType), connectionID );
            --fromArrayCount;
            }

        iCurrentBearerType = newSAEConnType;

        if (!iNetworkIsSuspended) // we are suspended, don't update the new network state.
            {
            UpdateBearerNetworkStatusL( STATIC_CAST(TSAEConnectionType, newSAEConnType) );
            }

        // Update all context activity
        UpdateContextActivity( STATIC_CAST(TSAEConnectionType, oldSAEConnType) );
        UpdateContextActivity( STATIC_CAST(TSAEConnectionType, newSAEConnType) );

        }
    else
        {
        LOG( Log::Printf( _L("DCL: Unknown connection type %d, or this connection type is allready active."),
             newConnType ));
        // No such connection ID...
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::UpdateBearerNetworkStatusL
// -------------------------------------------------------------------------
void CSaeObserver::UpdateBearerNetworkStatusL( const TSAEConnectionType& connType )
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::UpdateBearerNetworkStatusL(connType %d, bearer %d, suspended %d)"),
         connType, iCurrentBearerType, iNetworkIsSuspended ));

    // No connections active, so check network status and update if
    // UnAttached. This prevents the state to hang to active in case
    // of no network events...
    TInt ntwStatus( ENtwkStatusNotKnown );
    TRequestStatus status;
    TInt subConId( 0 );

    if ( connType == ESAEGPRSConnectionType )
        {
        iMonitor.GetIntAttribute( EBearerIdGPRS, subConId, KNetworkStatus, ntwStatus, status );
        User::WaitForRequest( status );
        LOG( Log::Printf( _L("+ DCL: Async call status==(%d)."), status.Int() ));

        if ( (ntwStatus == EConnMonStatusNotAvailable) || (ntwStatus == EConnMonStatusUnattached) )
            {
            LOG( Log::Printf( _L("DCL: GPRS ntwk is UnAttached." )));
            iNetworkIsUnAttached = ETrue;
            NotifyPubSub( KPSUidGprsStatus, EPSGprsUnattached );
            }
        else if ( ntwStatus == ENtwkStatusNotKnown )
            {
            LOG( Log::Printf( _L("DCL: Async request for GPRS ntwk status failed %d!"),
                 status.Int() ));
            }
        else
            {
            LOG( Log::Printf( _L("DCL: GPRS ntwk is Attached." )));
            iNetworkIsUnAttached = EFalse;
            NotifyPubSub( KPSUidGprsStatus, EPSGprsAttach );
            }
        }
    else if ( connType == ESAEWCDMAConnectionType )
        {
        iMonitor.GetIntAttribute( EBearerIdWCDMA, subConId, KNetworkStatus, ntwStatus, status );
        User::WaitForRequest( status );
        LOG( Log::Printf( _L("+ DCL: Async call status==(%d)."), status.Int() ));

        if ( (ntwStatus == EConnMonStatusNotAvailable) || (ntwStatus == EConnMonStatusUnattached) )
            {
            LOG( Log::Printf( _L("DCL: WCDMA ntwk is UnAttached." )));
            iNetworkIsUnAttached = ETrue;
            NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaUnattached );
            }
        else if ( ntwStatus == ENtwkStatusNotKnown )
            {
            LOG( Log::Printf( _L("DCL: Async request for WCDMA ntwk status failed %d!"),
                 status.Int() ));
            }
        else
            {
            LOG( Log::Printf( _L("DCL: WCDMA ntwk is Attached." )));
            iNetworkIsUnAttached = EFalse;
            NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaAttach );
            }
        }
    else
        {
        LOG( Log::Printf( _L("DCL: !ERROR! Unknown connection type %d !ERROR!"),
             connType ));
        User::Leave(KErrGeneral);
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::HandleEConnMonDeleteConnectionL
// -------------------------------------------------------------------------
void CSaeObserver::HandleEConnMonDeleteConnectionL( const CConnMonDeleteConnection& aEvent )
    {
    TUint connId( aEvent.ConnectionId() );
    // Connection type cannot be read from the Delete -event, but is needed
    // later...
    TSAEConnectionType connType( ESAEUnknownConnectionType );
    IdentifyTypeFromArrays( connId, connType );
    RArray<TUint>* conns = MatchingConnArray( connType, connId );

    if ( conns )
        {
        TInt connCount( conns->Count() );
        if ( connCount > 0 )
            {
            // If the connection ID is found from the connection array,
            // simply remove it.
            if (RemoveFromConnArray( connType, connId ) == KErrNone)
                {
                LOG( Log::Printf( _L("DCL: Connection %d deleted (%d connections remaining)."),
                     connId, conns->Count() ));

                if( --connCount == 0 )
                    {
                    UpdateBearerNetworkStatusL( connType );
                    }
                else
                    {
                    // Some connection(s) still active, so update status.
                    UpdateContextActivity( connType );
                    }
                }
            }
        else if ( iNetworkIsUnAttached && (connType == ESAEGPRSConnectionType) )
            {
            // No active connections and network is UnAttached, but this still is
            // a GPRS event... this case can occur for instance when trying to
            // establish a GPRS connection with a SIM that does not have the GPRS
            // enabled by operator. If this would not be handled the state would
            // hang to activating (no UnAttach event received since network's state
            // does not change).
            LOG( Log::Printf( _L("DCL: No connections have been active and ntwk is UnAttached." )));
            NotifyPubSub( KPSUidGprsStatus, EPSGprsUnattached );
            }
        else if ( iNetworkIsUnAttached && (connType == ESAEWCDMAConnectionType) )
            {
            // Same case here as above but with WCDMA event...
            LOG( Log::Printf( _L("DCL: No connections have been active and ntwk is UnAttached." )));
            NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaUnattached );
            }
        else
            {
            LOG( Log::Printf( _L("DCL: No connections of type %d active."), connType ));
            }
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::HandleEConnMonNetworkStatusChangeL
// -------------------------------------------------------------------------
void CSaeObserver::HandleEConnMonNetworkStatusChangeL( const CConnMonNetworkStatusChange& aEvent )
    {
    TSAEConnectionType connType( GetConnectionType( aEvent.ConnectionId() ) );
    LOG( Log::Printf( _L("DCL: connection type: %d."), connType ));

    if ( (connType == ESAEGPRSConnectionType) || (connType == ESAEWCDMAConnectionType) )
        {
        switch ( aEvent.NetworkStatus() )
            {
            case EConnMonStatusUnattached:
                {
                if ( connType == ESAEGPRSConnectionType )
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusUnattached, KUidGprsStatus, ESAGprsUnattached." )));
                    iNetworkIsUnAttached = ETrue;
                    iNetworkIsSuspended = EFalse;
                    NotifyPubSub( KPSUidGprsStatus, EPSGprsUnattached );
                    }
                else // ESAEWCDMAConnectionType
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusUnattached, KUidWcdmaStatus, ESAWcdmaUnattached." )));
                    iNetworkIsUnAttached = ETrue;
                    iNetworkIsSuspended = EFalse;
                    NotifyPubSub( KPSUidWcdmaStatus, EPSGprsUnattached );
                    }
                // Just in case; there shouldn't be any active connections by now anyway:
                RemoveAllFromConnArray( connType );
                }
                break;

            case EConnMonStatusAttached:
                {
                if ( connType == ESAEGPRSConnectionType )
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusAttached, KUidGprsStatus, ESAGprsAttach." )));
                    iNetworkIsUnAttached = EFalse;
                    iNetworkIsSuspended = EFalse;
                    NotifyPubSub( KPSUidGprsStatus, EPSGprsAttach );
                    }
                else // ESAEWCDMAConnectionType
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusAttached, KUidWcdmaStatus, ESAWcdmaAttach." )));
                    iNetworkIsUnAttached = EFalse;
                    iNetworkIsSuspended = EFalse;
                    NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaAttach );
                    }
                }
                break;

            case EConnMonStatusSuspended:
                {
                LOG( Log::Printf( _L("DCL: EConnMonStatusSuspended, -> ntwk to suspend." )));
                iNetworkIsSuspended = ETrue;
                NotifyPubSub( KPSUidGprsStatus, EPSGprsSuspend );
                NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaSuspend );
                }
                break;

            case EConnMonStatusActive:
                {
                if ( connType == ESAEGPRSConnectionType )
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusActive, KUidGprsStatus, current activity..." )));
                    iNetworkIsUnAttached = EFalse;
                    iNetworkIsSuspended = EFalse;
                    }
                else // ESAEWCDMAConnectionType
                    {
                    LOG( Log::Printf( _L("DCL: EConnMonStatusActive, KUidWcdmaStatus, current activity..." )));
                    iNetworkIsUnAttached = EFalse;
                    iNetworkIsSuspended = EFalse;
                    }
                UpdateContextActivity( connType );
                }
                break;

            default:
                LOG( Log::Printf( _L("DCL: Unhandled NtwStatusChange(%d)."), aEvent.NetworkStatus() ));
                break;
            }
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::HandleEConnMonConnectionStatusChangeL
// -------------------------------------------------------------------------
void CSaeObserver::HandleEConnMonConnectionStatusChangeL(const CConnMonConnectionStatusChange& aEvent )
    {
    TInt connStatus = aEvent.ConnectionStatus();
    if ( connStatus == KPsdStartingActivation ||
            connStatus == KPsdFinishedActivation ||
            connStatus == KPsdFinishedDeactivation )
        {
        TUint connId( aEvent.ConnectionId() );
        LOG( Log::Printf( _L("DCL: connection ID got: %d."), connId ));
        TSAEConnectionType connType( GetConnectionType( connId ) );
        LOG( Log::Printf( _L("DCL: connection type: %d."), connType ));

        if ( (connType == ESAEGPRSConnectionType) || (connType == ESAEWCDMAConnectionType) )
            {
            LOG( Log::Printf( _L("DCL: trying to get connection status..."), connId));
            switch ( connStatus )
                {
                case KPsdStartingActivation :
                    {
                    if ( connType == ESAEGPRSConnectionType )
                        {
                        LOG( Log::Printf( _L("DCL: KPsdStartingActivation : KUidGprsStatus, ESAGprsContextActivating." )));
                        NotifyPubSub( KPSUidGprsStatus, EPSGprsContextActivating );
                        iCurrentBearerType = ESAEGPRSConnectionType;
                        }
                    else // ESAEWCDMAConnectionType
                        {
                        LOG( Log::Printf( _L("DCL: KPsdStartingActivation : KUidWcdmaStatus, ESAWcdmaContextActivating." )));
                        NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaContextActivating );
                        iCurrentBearerType = ESAEWCDMAConnectionType;
                        }
                    AddToConnArray( connType, connId );
                    }
                    break;

                case KPsdFinishedActivation :
                    {
                    LOG( Log::Printf( _L("DCL: KPsdFinishedActivation." )));
                    // Connection opened, so add it to the array of open connections:
                    AddToConnArray( connType, connId );
                    UpdateContextActivity( connType );
                    LOG( Log::Printf( _L("DCL: Connection %d of type %d finished activation."),
                         connId, connType ));
                    }
                    break;

                case KPsdFinishedDeactivation :
                    {
                    LOG( Log::Printf( _L("DCL: KPsdFinishedDeactivation." )));
                    RemoveFromConnArray( connType, connId );
                    LOG( Log::Printf( _L("DCL: Connection %d of type %d finished de-activation."),
                         connId, connType ));

                    if (iGPRSConnections.Count() == 0 && iWCDMAConnections.Count() == 0)
                        {
                        // No active contexts...
                        UpdateBearerNetworkStatusL(connType);
                        }
                    else
                        {
                        // There is at least one active context
                        UpdateContextActivity(connType);
                        }
                    }
                    break;

                default :
                    LOG( Log::Printf( _L("DCL: Unhandled context connection status (%d)."),
                         aEvent.ConnectionStatus() ));
                    break;
                }
            }
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::NotifyPubSub
// -------------------------------------------------------------------------
void CSaeObserver::NotifyPubSub( const TUid aVariable, const TInt aState )
    {
    LOG( Log::Printf( _L("DCL: CSaeObserver::NotifyPubSub : aVariable == %d aState == %d"),
         aVariable.iUid, aState ));

    TInt returnValue = iSAProperty.Set(KUidSystemCategory, aVariable.iUid, aState);

    LOG( Log::Printf( _L("DCL: CSaeObserver::NotifyPubSub : returnValue == %d"),
         returnValue ));
    }

// -------------------------------------------------------------------------
// CSaeObserver::GetConnectionType
// -------------------------------------------------------------------------
TSAEConnectionType CSaeObserver::GetConnectionType( const TUint &aConnectionId )
    {
    TRequestStatus status;
    TInt connType( EBearerUnknown );
    TInt subConId( 0 );
    TSAEConnectionType ret( ESAEUnknownConnectionType );

    // EBearerIdGPRS and -WCDMA means bearer specific Connection Id.
    // This is a fix to get for instance network unattached state,
    // since the actual connection Id is not necessarily available anymore.
    switch ( aConnectionId )
        {
        case EBearerIdGPRS :
            {
            ret = ESAEGPRSConnectionType;
            }
            break;

        case EBearerIdWCDMA :
            {
            ret = ESAEWCDMAConnectionType;
            }
            break;

        default :
            {
            iMonitor.GetIntAttribute( aConnectionId, subConId, KBearer, connType, status );
            User::WaitForRequest( status );
            LOG( Log::Printf( _L("+ DCL: Async call status==(%d)."), status.Int() ));
            if ( status == KErrNone )
                {
                // If bearer status can not be read the event is not handled, since
                // connType == EBearerUnknown and ret == ESAEUnknownConnectionType.

                if ( connType == EBearerGPRS ||
                     connType == EBearerExternalGPRS ||
                     connType == EBearerEdgeGPRS ||
                     connType == EBearerExternalEdgeGPRS )
                    {
                    ret = ESAEGPRSConnectionType;
                    }
                else if ( connType == EBearerWCDMA ||
                          connType == EBearerExternalWCDMA )
                      {
                    ret = ESAEWCDMAConnectionType;
                    }
                }
            else
                {
                // Finally try to check if allready saved to some connection
                // type specific connection array...
                IdentifyTypeFromArrays( aConnectionId, ret );
                }
            }
            break;
        }

#ifdef _DEBUG
        switch ( ret )
        {
        case ESAEGPRSConnectionType :
            {
            LOG( Log::Printf( _L("DCL: This is a GPRS connection event." )));
            }
            break;
        case ESAEWCDMAConnectionType :
            {
            LOG( Log::Printf( _L("DCL: This is a WCDMA connection event." )));
            }
            break;
        default :
            {
            LOG( Log::Printf( _L("DCL: The event is not of any known type." )));
            }
            break;
        }
#endif //_DEBUG

    return ret;
    }

// -------------------------------------------------------------------------
// CSaeObserver::IdentifyTypeFromArrays
// -------------------------------------------------------------------------
void CSaeObserver::IdentifyTypeFromArrays( TUint aConnectionId,
                                           TSAEConnectionType& aConnType )
    {
    // When, for instance, handling Delete Connection -event we can't get the
    // connection type from the event, so we can search for the unique
    // connection ID from the ones we allready have in the connection type
    // specific member lists. We can then figure the connection type
    // based on that.
    if ( iGPRSConnections.Find(aConnectionId) != KErrNotFound )
        {
        LOG( Log::Printf( _L("DCL: IdentifyTypeFromArrays: Found from GPRS array" )));
        aConnType = ESAEGPRSConnectionType;
        }
    else if ( iWCDMAConnections.Find(aConnectionId) != KErrNotFound )
        {
        LOG( Log::Printf( _L("DCL: IdentifyTypeFromArrays: Found from WCDMA array" )));
        aConnType = ESAEWCDMAConnectionType;
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::MatchingConnArray
// -------------------------------------------------------------------------
RArray<TUint>* CSaeObserver::MatchingConnArray( const TSAEConnectionType& aConnType,
                                                TUint aConnectionId )
    {
    LOG( Log::Printf( _L("DCL: MatchingConnArray: type:%d, id:%d"),
         aConnType, aConnectionId ));
    RArray<TUint>* ret = NULL;
    switch ( aConnType )
        {
        case ESAEGPRSConnectionType :
            {
            LOG( Log::Printf( _L("DCL: MatchingConnArray: Returning GPRS array" )));
            ret =  &iGPRSConnections;
            }
            break;
        case ESAEWCDMAConnectionType :
            {
            LOG( Log::Printf( _L("DCL: MatchingConnArray: Returning WCDMA array" )));
            ret = &iWCDMAConnections;
            }
            break;
        case ESAEUnknownConnectionType :
            {
            // Check if connection ID allready in some type specific array
            TSAEConnectionType type( ESAEUnknownConnectionType );
            IdentifyTypeFromArrays( aConnectionId, type);
            if ( type != ESAEUnknownConnectionType )
                {
                // Try to match again with the implicitly found out type...
                ret = MatchingConnArray( type, aConnectionId );
                }
            }
            break;
        default :
            LOG( Log::Printf( _L("DCL: MatchingConnArray: No array found, returning NULL" )));
            break;
        }
    return ret;
    }

// -------------------------------------------------------------------------
// CSaeObserver::UpdateContextActivity
// -------------------------------------------------------------------------
void CSaeObserver::UpdateContextActivity( const TSAEConnectionType& aConnType )
{
    // Check if network is UnAttached or unknown, do not update to active in
    // this case since connection can not be active if ntwk is not attached.
    // Also check if we are currently in suspend state. Only network events can
    // trigger returning from suspend (the i*NetworkIsSuspended is unset only
    // during handling of these events).
    if ( aConnType == ESAEGPRSConnectionType )
        {
        TInt count( iGPRSConnections.Count() );
        LOG( Log::Printf( _L("DCL: UpdateContextActivity (GPRS conns %d)"), count ));
        if ( !iNetworkIsUnAttached && !iNetworkIsSuspended)
            {
            if ( count == 1 )
                {
                // One connection active
                LOG( Log::Printf( _L("DCL: ESAGprsContextActive" )));
                NotifyPubSub( KPSUidGprsStatus, EPSGprsContextActive );
                iCurrentBearerType = ESAEGPRSConnectionType;
                }
            else if( count > 1 )
                {
                // Multiple connections active
                LOG( Log::Printf( _L("DCL: ESAGprsMultibleContextActive" )));
                NotifyPubSub( KPSUidGprsStatus, EPSGprsMultibleContextActive );
                iCurrentBearerType = ESAEGPRSConnectionType;
                }
            }
        }
    else if ( aConnType == ESAEWCDMAConnectionType )
        {
        TInt count( iWCDMAConnections.Count() );
        LOG( Log::Printf( _L("DCL: UpdateContextActivity (WCDMA conns %d)"), count ));
        if ( !iNetworkIsUnAttached && !iNetworkIsSuspended)
            {
            if ( count == 1 )
                {
                // One connection active
                LOG( Log::Printf( _L("DCL: ESAWcdmaContextActive" )));
                NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaContextActive );
                iCurrentBearerType = ESAEWCDMAConnectionType;
                }
            else if( count > 1 )
                {
                // Multiple connections active
                LOG( Log::Printf( _L("DCL: ESAWcdmaMultipleContextActive" )));
                NotifyPubSub( KPSUidWcdmaStatus, EPSWcdmaMultipleContextActive );
                iCurrentBearerType = ESAEWCDMAConnectionType;
                }
            }
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::AddToConnArray
// -------------------------------------------------------------------------
void CSaeObserver::AddToConnArray( const TSAEConnectionType& aConnType,
                                   const TUint aConnId )
    {
    RArray<TUint>* conns = MatchingConnArray( aConnType );

    LOG( Log::Printf( _L("DCL: AddToConnArray(type %d)"), aConnType ));

    // No duplicate connections. Also do not add bearer specific connection Id.
    if ( conns && (conns->Find(aConnId) == KErrNotFound) &&
        ((aConnId != EBearerIdGPRS) && (aConnId != EBearerIdWCDMA)) )
        {
        conns->Append( aConnId );
        LOG( Log::Printf( _L("DCL: Contexts: GPRS %d, WCDMA %d"),
             iGPRSConnections.Count(), iWCDMAConnections.Count() ));
        }
    }

// -------------------------------------------------------------------------
// CSaeObserver::RemoveFromConnArray
// -------------------------------------------------------------------------
TInt CSaeObserver::RemoveFromConnArray( const TSAEConnectionType& aConnType,
                                        const TUint aConnId )
    {
    LOG( Log::Printf( _L("DCL: RemoveFromConnArray, type: %d, id: %d."),
         aConnType, aConnId ));
    TInt ret(KErrNotFound);
    TInt index(0);
    RArray<TUint>* conns = MatchingConnArray( aConnType );

    if ( conns )
        {
        index = conns->Find( aConnId );
        if ( index != KErrNotFound )
            {
            conns->Remove( index );
            LOG( Log::Printf( _L("DCL: Context %d of type %d removed."),
                 aConnId, aConnType ));
            ret = KErrNone;
            }
        }
    return ret;
    }

// -------------------------------------------------------------------------
// CSaeObserver::RemoveAllFromConnArray
// -------------------------------------------------------------------------
void CSaeObserver::RemoveAllFromConnArray( const TSAEConnectionType& aConnType )
    {
    RArray<TUint>* conns = MatchingConnArray( aConnType );

    if ( conns )
        {
        LOG( Log::Printf( _L("DCL: RemoveAllFromConnArray(), reseting ConnType %d array."),
             aConnType ));
        conns->Reset();
        }
    }
