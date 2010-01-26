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
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_partner.h>
#endif
#include <es_enum.h>
#include <etelpckt.h>
#include <in_sock.h>

#include "caoconnectionimpl.h"
#include "maosettings.h"
#include "logger.h"
#include "maoconnectionobserver.h"

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
    _LIT( KPanicCat, "ConnectionImpl" );
    
    // DATA TYPES
    enum TPanicCode
        {
        ENone,
        EConnectionAlreadyActive,
        ENotActive,
        EInvalidState,
        EAlreadyActive,
        EReuqestNotPending
        };
    
    // LOCAL FUNCTIONS
    LOCAL_C void Panic( TPanicCode aCode )
        {
        User::Panic( KPanicCat, aCode );
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
    RSocketServ*           aSocketServ )
    {
    LOG_1( _L("CAOConnectionImpl::NewL") );
    
    CAOConnectionImpl* self = new( ELeave ) CAOConnectionImpl(
        aObserver,
        aSettings,
        aSocketServ );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
        
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
    
    delete iCommsDatabase;
    iConnection.Close();
    
    delete iExtPrefs;
    delete iPrefsList;
    }


// ---------------------------------------------------------------------------
// CAOConnectionImpl::ActivateConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ActivateConnection()
    {
    LOG_1( _L("CAOConnectionImpl::ActivateConnection") );
        
    __ASSERT_DEBUG( !IsActive() && iState == EDisconnected,
        Panic( EConnectionAlreadyActive ) );
        
    // Change state to EOpeningLink
    ChangeState( EOpeningLink );
    
    // Check IAP
    TUint iap = iSettings.AccessPointId();
    if ( IsValidIAP( iap ) && iSettings.IsCellularAllowedByUser() )
        {
        // Check that we really have RConnection handle opened
        TInt err = KErrNone;
        if( !iConnection.SubSessionHandle() )
            {
            // Socket serv is closed only in destructor
            err = iConnection.Open( *iSocketServ );
            }
    
        if( err == KErrNone )
            {    		
            // Start as a silent connection
            iExtPrefs->SetIapId( iap );
            iExtPrefs->SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnSilent );
    		
            // Clean iPrefsList and append new prefs
            iPrefsList->Remove( 0 );
            TRAPD( err, iPrefsList->AppendL( iExtPrefs ) )
    		
            if ( err == KErrNone )
                {
    		    iConnection.Start( *iPrefsList, iStatus );
                SetActive();
                }
            else
                {
                LOG_1( _L("iPrefsList->AppendL( iExtPrefs ) FAILED") );
                ActivateSelf( KErrNotSupported );
                }
            }
        else
            {
            ActivateSelf( KErrNotSupported );
            }
        }
    else
        {
        // Something is wrong. Complete self with error KErrNotSupported ->
        // leads to permanent failure.
        ActivateSelf( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::CancelConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::CancelConnection()
    {
    LOG_1( _L("CAOConnectionImpl::CancelConnection") );
    Cancel();
    LOG_2( _L("CAOConnectionImpl::CancelConnection: Cur. state: %d"),
        iState );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::IsConnectedToDefaultIAP
// ---------------------------------------------------------------------------
//
TBool CAOConnectionImpl::IsConnectedToDefaultIAP()
    {
    LOG_1( _L("CAOConnectionImpl::IsConnectedToDefaultIAP") );
    
    // Check if we have PDP context open to default IAP
    TBool connected = EFalse;
    TUint connectionCount = 0;
    TInt err = iConnection.EnumerateConnections( connectionCount );
    if ( err == KErrNone )
        {
        TPckgBuf<TConnectionInfoV2> connectionInfo;
        for ( TInt i = 0; i < connectionCount; i++ )
            {
            err = iConnection.GetConnectionInfo( i + 1, connectionInfo );
            if ( err == KErrNone )
                {
                if ( connectionInfo().iIapId == iSettings.AccessPointId() )
                    {
                    LOG_1( _L("CAOConnectionImpl::IsConnectedToDefaultIAP: \
                               Connected to default IAP") );
                    connected = ETrue;
                    break;
                    }
                }
            }
        }
        
    LOG_2( _L("CAOConnectionImpl::IsConnectedToDefaultIAP: Connected: %d "),
        connected );
        
    return connected;
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::NumberOfConnections
// ---------------------------------------------------------------------------
//
TInt CAOConnectionImpl::NumberOfConnections()
    {
    LOG_1( _L("CAOConnectionImpl::NumberOfConnections") );
    
    TUint connectionCount = 0;
    TInt err = iConnection.EnumerateConnections( connectionCount );
    if( err != KErrNone )
        {
        connectionCount = 0;
        }
    LOG_2( _L("> Number of connections: %d"), connectionCount );
        
    return static_cast<TInt>( connectionCount );
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionImpl::CloseConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::CloseConnection()
    {
    LOG_1( _L("CAOConnectionImpl::CloseConnection") );
    
    if ( iState == EListeningAccessPoint )
        {
        if ( IsActive() )
            {
            iConnection.CancelProgressNotification();	
            }
        	
    	iConnection.Close();
        
        // To really close: re-open
        TInt err = iConnection.Open( *iSocketServ );
        LOG_2( _L("> Re-opened connection handle: Err: %d"), err );

        ChangeState( EDisconnected );
        }
    }
        
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnectionImpl::CAOConnectionImpl(
                                      MAOConnectionObserver& aObserver,
                                      MAOSettings&           aSettings,
                                      RSocketServ*           aSocketServ ):
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iSettings( aSettings ),
    iSocketServ( aSocketServ ),
    iState( EDisconnected )
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
    
    User::LeaveIfError( iConnection.Open( *iSocketServ, KAfInet ) );
    iCommsDatabase = CCommsDatabase::NewL( EDatabaseTypeUnspecified );
    
    iExtPrefs = new (ELeave) TExtendedConnPref;
    iPrefsList = TConnPrefList::NewL();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::IsValidIAP
// ---------------------------------------------------------------------------
//
TBool CAOConnectionImpl::IsValidIAP( TUint32 aId ) const
    {
    LOG_1( _L("CAOConnectionImpl::IsValidIAP") );
    
    CCommsDbTableView* table = NULL;
    
    // E32USER Cbase panic 79 is raised if OpenViewMatchingUintLC does not
    // leave inside trap. Therefore pop table out from cleanup stack before
    // exiting from TRAP harness.
    TRAPD( error, table = iCommsDatabase->OpenViewMatchingUintLC(
        TPtrC( IAP ),
        TPtrC( COMMDB_ID ),
        aId );
        CleanupStack::Pop( table ) );
    
    if ( error == KErrNone )
        {
        error = table->GotoFirstRecord();
        }
    delete table;
    
    return error == KErrNone;
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::CompleteSelf
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::CompleteSelf( TRequestStatus& aStatus, TInt aReason )
    {
    LOG_1( _L("CAOConnectionImpl::CompleteSelf") );
    
    __ASSERT_DEBUG( IsActive(), Panic( ENotActive ) );
    __ASSERT_DEBUG( aStatus == KRequestPending, Panic( EReuqestNotPending ) );
    
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, aReason );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleOpeningLinkL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleOpeningLinkL( TInt aStatus )
    {
    LOG_1( _L("CAOConnectionImpl::HandleOpeningLinkL") );
    
    if ( aStatus == KErrNone )
        {
        // First check current status
        TInt err = iConnection.Progress( iProgressBuf() );
        if ( err == KErrNone && iProgressBuf().iError == KErrNone )
            {
            IssueProgressNotification();
            if ( iProgressBuf().iStage == KLinkLayerOpen )
                {
                // Link open
                ChangeState( EListeningAccessPoint );
                iObserver.HandlePDPContextActivatedL();
                }
            }
        else
            {
            BrodcastActivationFailedError( err );
            }
        }
    else
        {
        BrodcastActivationFailedError( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::HandleListeningAccessPointL
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::HandleListeningAccessPointL( TInt aStatus )
    {
    LOG_1( _L("CAOConnectionImpl::HandleListeningAccessPointL") );
    LOG_2( _L("> aStatus: %d"), aStatus );
    LOG_2( _L("> iProgressBuf().iError: %d"), iProgressBuf().iError );
    
    if ( aStatus == KErrNone && iProgressBuf().iError == KErrNone )
        {
        // Check connection status
        switch ( iProgressBuf().iStage )
            {
            case KLinkLayerClosed:
                {
                LOG_1( _L("> Link closed") );
                
                // Connection disconnected
                // Stop connection just in case
                StopConnection();
                
                // Notify and change state
                BrodcastDisconnectedError( KErrDisconnected );
                break;
                }
            case KLinkLayerOpen:
                {
                LOG_1( _L("> Link open") );
                
                // everything ok -> continue listening
                IssueProgressNotification();
                break;
                }
            case KDataTransferTemporarilyBlocked:
                {
                LOG_1( _L("> Link temporarily blocked") );
                
                // Data transfer temporarily blocked
                // Continue listening and notify
                IssueProgressNotification();
                iObserver.HandlePDPContextTemporarilyBlockedL();
                break;
                }
            case KConnectionClosed:
                {
                LOG_1( _L("> Connection closed") );
                    
                // Notify and change state
                BrodcastDisconnectedError( KErrDisconnected );
                break;
                }                
            default:
                {
                LOG_2( _L("iProgressBuf().iStage: %d"),
                        iProgressBuf().iStage );
                
                // Just continue listening
                IssueProgressNotification();
                break;
                }
            }
        }
    else
        {
        // Handle error
        BrodcastDisconnectedError( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::ChangeState
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ChangeState( TInternalState aNewState )
    {
#ifdef LOGGING_ENABLED
    TInternalState old = iState;
    iState = aNewState;
    LOG_3( _L("CAOConnectionImpl::ChangeState >> Changing state %d -> %d"),
        old, iState );
#else
    iState = aNewState;
#endif // LOGGING_ENABLED
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::IssueProgressNotification
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::IssueProgressNotification()
    {
    LOG_1( _L("CAOConnectionImpl::IssueProgressNotification") );
    
    __ASSERT_DEBUG( !IsActive(), Panic( EConnectionAlreadyActive ) );
    
    iConnection.ProgressNotification( iProgressBuf, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::BrodcastDisconnectedError
// ---------------------------------------------------------------------------
//
#ifdef LOGGING_ENABLED
void CAOConnectionImpl::BrodcastDisconnectedError( TInt aError )
#else
void CAOConnectionImpl::BrodcastDisconnectedError( TInt /*aError*/ )
#endif
    {
    LOG_2( _L("CAOConnectionImpl::BrodcastDisconnectedError: aError: %d"),
           aError );
    
    ChangeState( EDisconnected );
    TRAP_IGNORE( iObserver.HandlePDPContextDisconnectedL(
        MAOConnectionManager::EDisconnected ) );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::BrodcastActivationFailedError
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::BrodcastActivationFailedError( TInt aError )
    {
    LOG_2( _L("CAOConnectionImpl::BrodcastActivationFailedError: aError: %d"),
        aError );
    
    ChangeState( EDisconnected );
    TRAP_IGNORE( iObserver.HandlePDPContextActivationFailedL(
        FailureReason( aError ) ) );
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
        case KErrConnectionTerminated:
            {
            LOG_1( _L("> Temporary failure") );
            reason = MAOConnectionManager::ETemporary;
            break;
            }
        // Permanent cause
        default:
            {
            LOG_1( _L("> Permanent failure") );
            reason = MAOConnectionManager::EPermanent;
            break;
            }
        }
        
    LOG_2( _L("CAOConnectionImpl::FailureReason: Reason: %d"), reason );
            
    return reason;
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionImpl::ActivateSelf
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::ActivateSelf( TInt aReason )
    {
    LOG_2( _L("CAOConnectionImpl::ActivateSelf: aReason: %d"), aReason );
    
    __ASSERT_DEBUG( !IsActive(), Panic( EAlreadyActive ) );
    
    iStatus = KRequestPending;
    SetActive();
    CompleteSelf( iStatus, aReason );
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::StopConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionImpl::StopConnection()
    {
    LOG_1( _L("CAOConnectionImpl::StopConnection") );
    
    TInt err = iConnection.Stop( RConnection::EStopAuthoritative );
    LOG_2( _L("> Stopped connection: Err: %d"), err );
    
    iConnection.Close();
    LOG_1( _L("> Closed connection handle") );
    
    err = iConnection.Open( *iSocketServ );
    LOG_2( _L("> Re-opened connection handle: Err: %d"), err );
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
            // nothing to do
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
        default:
            {
            LOG_1( _L("CAOConnectionImpl::RunL >> Invalid state!") );
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
    
    TInternalState currentState = iState;
    ChangeState( EDisconnected );
    switch ( currentState )
        {
        case EDisconnected:
            {
            // Nothing to do
            break;
            }
        case EOpeningLink:
        case EListeningAccessPoint:
            {
            // Ignore error code since there is nothing that can be done if
            // these fail
            iConnection.CancelProgressNotification();
            StopConnection();
            break;
            }
        default:
            {
            LOG_1( _L("CAOConnectionImpl::DoCancel >> Invalid state!") );
            Panic( EInvalidState );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CAOConnectionImpl::RunError( TInt aError )
    {
    LOG_2( _L("CAOConnectionImpl::RunError: aError: %d"), aError );
    
    switch ( iState )
        {
        case EDisconnected:
        case EOpeningLink:
        case EListeningAccessPoint:
            {
            BrodcastDisconnectedError( aError );
            break;
            }
        default:
            {
            LOG_1( _L("CAOConnectionImpl::RunError >> Invalid state!") );
            Panic( EInvalidState );
            break;
            }
        }
    ChangeState( EDisconnected );
    
    return KErrNone;
    }

