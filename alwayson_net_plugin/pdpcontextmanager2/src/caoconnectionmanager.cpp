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
* Description:  Implements the CAOConnectionManager class
*
*/

// INCLUDE FILES
#include "caoconnectionmanager.h"
#include "logger.h"
#include "caoconnection.h"
#include "caoconnectionmonitor.h"
#include "maoconnectionmanagerobserver.h"

// METHODS

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAOConnectionManager* CAOConnectionManager::NewL(
    MAOConnectionManagerObserver& aObserver,
    MAOSettings& aSettings )
    {
    LOG_1( _L("CAOConnectionManager::NewL") );
    
    CAOConnectionManager* self = new( ELeave ) 
        CAOConnectionManager( aObserver, aSettings );
    
    CleanupStack::PushL( self );
    self->ConstructL( aSettings );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAOConnectionManager::~CAOConnectionManager()
    {
    LOG_1( _L("CAOConnectionManager::~CAOConnectionManager") );
    
    for ( TInt i=0; i < iLingerArray.Count(); i++ )
        {
        delete iLingerArray[ i ];    
        }
    
    iLingerArray.Reset();
    iLingerArray.Close();
            
    delete iConnectionMonitor;
    delete iConnection;
    iSocketServ.Close();
    }


// ---------------------------------------------------------------------------
// CAOConnectionManager::FetchNetworkRegistrationStatus
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::FetchNetworkRegistrationStatus()
    {
    LOG_1( _L("CAOConnectionManager::FetchNetworkRegistrationStatus") );
    
    iConnectionMonitor->FetchNetworkRegistrationStatus();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::ActivatePDPContext
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::ActivatePDPContextL()
    {
    LOG_1( _L("CAOConnectionManager::ActivatePDPContextL") );
    
    iConnection->ActivateConnection();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::IsPDPContextActive
// ---------------------------------------------------------------------------
//
TBool CAOConnectionManager::IsPDPContextActive() const
    {
    LOG_1( _L("CAOConnectionManager::IsPDPContextActive") );
    
    return iConnection->IsConnectedToDefaultIAP();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::CloseConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::CloseConnection()
    {
    LOG_1( _L("CAOConnectionManager::CloseConnection") );
    
    iConnection->CancelConnection();
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionManager::DetachConnection
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::DetachConnection()
    {
    LOG_1( _L("CAOConnectionManager::DetachConnection") );
    
    iConnection->CloseConnection();
    }    

// ---------------------------------------------------------------------------
// CAOConnectionManager::NetworkType
// ---------------------------------------------------------------------------
//
MAOConnectionManager::TNetworkType CAOConnectionManager::NetworkType() const
    {
    LOG_1( _L("CAOConnectionManager::NetworkType") );
    
    MAOConnectionManager::TNetworkType nwType = ENotRegistered;
    switch ( iConnectionMonitor->NetworkRegistration() )
        {
        case CTelephony::ERegisteredOnHomeNetwork:
            {
            nwType = EHPLMN;
            break;
            }
        case CTelephony::ERegisteredRoaming:
            {
            nwType = EVPLMN;
            break;
            }
        default:
            {
            // Other types are considered as not registered.
            break;
            }
        }
    LOG_2( _L("CAOConnectionManager::NetworkType: NWType: %d"),
        nwType );
    
    return nwType;
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::NumberOfConnections
// ---------------------------------------------------------------------------
//
TInt CAOConnectionManager::NumberOfConnections()
    {
    LOG_1( _L("CAOConnectionManager::NumberOfConnections") );
    
    TInt numOfConnections = iConnection->NumberOfConnections();
    LOG_2( _L("> Connection count: %d"), numOfConnections );
    
    return numOfConnections;
    }
    
// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleSettingsChangedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandleSettingsChangedL()
    {
    LOG_1( _L("CAOConnectionManager::HandleSettingsChangedL") );
    
       
    // Check if always-on iap should stop linger
    for ( TInt i=0; i < iLingerArray.Count(); i++ )
        {
        if  ( iLingerArray[ i ]->IapId() == iSettings.AccessPointId() )
            {
            // Always-on IAP
            if ( IsAlwaysOnEnabled() )
                {	
                iLingerArray[ i ]->StopLinger();    
                }
            else
                {
                iLingerArray[ i ]->HandleSettingsChanged();	
                }	     
            }
        else
            {
            // common IAP
            iLingerArray[ i ]->HandleSettingsChanged();    
            }    
        }    
    }    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAOConnectionManager::CAOConnectionManager(
                                        MAOConnectionManagerObserver& aObserver,
                                        MAOSettings& aSettings ):
    iObserver( aObserver ),
    iSettings( aSettings )
    {
    LOG_1( _L("CAOConnectionManager::CAOConnectionManager") );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::ConstructL( MAOSettings& aSettings )
    {
    LOG_1( _L("CAOConnectionManager::ConstructL") );
    
    User::LeaveIfError( iSocketServ.Connect() );
     
    iConnection = CAOConnection::NewL( *this, aSettings, &iSocketServ );
    iConnectionMonitor = CAOConnectionMonitor::NewL( *this, aSettings );
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::IsAlwaysOnEnabled()
// ---------------------------------------------------------------------------
//
TBool CAOConnectionManager::IsAlwaysOnEnabled()
    {
    LOG_1( _L("CAOConnectionManager::IsAlwaysOnEnabled") );
    
    TBool alwaysOnEnabled( EFalse );
        
    MAOConnectionManager::TNetworkType nwType = NetworkType();
    
    if ( nwType == MAOConnectionManager::EHPLMN )
        {
        if ( iSettings.AlwaysOnSupportedInHPLMN() )
            {
            alwaysOnEnabled = ETrue;
            }
        }
    else if ( nwType == MAOConnectionManager::EVPLMN )
        {
        if ( iSettings.AlwaysOnSupportedInVPLMN() )
            {
            alwaysOnEnabled = ETrue;
            }
        }
    else
        {
        alwaysOnEnabled = EFalse;    
        }
            
    return alwaysOnEnabled;     
    }
       
// ---------------------------------------------------------------------------
// CAOConnectionManager::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandlePDPContextActivatedL()
    {
    LOG_1( _L("CAOConnectionManager::HandlePDPContextActivatedL") );
    
    iObserver.HandlePDPContextActivatedL();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandlePDPContextActivationFailedL(
    MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("CAOConnectionManager::HandlePDPContextActivationFailedL") );
    
    iObserver.HandlePDPContextActivationFailedL( aReason );
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandlePDPContextDisconnectedL(
    MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("CAOConnectionManager::HandlePDPContextDisconnectedL") );
    
    iObserver.HandlePDPContextDisconnectedL( aReason );
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandlePDPContextTemporarilyBlockedL()
    {
    LOG_1( _L("CAOConnectionManager::HandlePDPContextTemporarilyBlockedL") );
    
    iObserver.HandlePDPContextTemporarilyBlockedL();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleNWRegistrationStatusChangedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandleNWRegistrationStatusChangedL(
    CTelephony::TRegistrationStatus aNetworkRegistration )
    {
    LOG_1( _L("CAOConnectionManager::HandleNWRegistrationStatusChangedL") );
    
    MAOConnectionManager::TNetworkType nwType = NetworkType();
                 
    // Check if always-on iap should start/stop linger
    for ( TInt i=0; i < iLingerArray.Count(); i++ )
        {
        if  ( iLingerArray[ i ]->IapId() == iSettings.AccessPointId() )
            {
            if ( IsAlwaysOnEnabled() )
                {
                iObserver.HandleNWRegistrationStatusChangedL( aNetworkRegistration );	
                iLingerArray[ i ]->StopLinger();
                return;    
                }
            else if ( nwType != MAOConnectionManager::ENotRegistered )
                {
                iLingerArray[ i ]->StartLinger();
                iObserver.HandleNWRegistrationStatusChangedL( aNetworkRegistration );
                return;	
                }
                	    
            break; 
            }
        }
        
    iObserver.HandleNWRegistrationStatusChangedL( aNetworkRegistration );        
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandleExternalConnectionCreatedL()
    {
    LOG_1( _L("CAOConnectionManager::HandleExternalConnectionCreatedL") );
    
    iObserver.HandleExternalConnectionCreatedL();
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandleConnectionDeletedL( const TUint aConnectionId,
                                                     const TBool aForward )
    {
    LOG_1( _L("CAOConnectionManager::HandleConnectionDeletedL") );
    
    // Check if this is a linger connection
    for ( TInt i=0; i < iLingerArray.Count(); i++ )
        {
        if  ( iLingerArray[ i ]->ConnectionId() == aConnectionId )
            {
            delete iLingerArray[ i ];    
            iLingerArray.Remove( i );
            break; 
            }
        }
    
    if ( aForward )
        {
        iObserver.HandleConnectionDeletedL( aConnectionId, aForward );
        }
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleInternalConnectionCreatedL
// ---------------------------------------------------------------------------
//    
void CAOConnectionManager::HandleInternalConnectionCreatedL( 
                                                      const TUint aConnectionId,
                                                      const TUint aIapId, 
                                                      const TUint aNetId )
    {
    LOG_1( _L("CAOConnectionManager::HandleInternalConnectionCreatedL") );
    
    CLingerConnection* ptr = NULL;
    TInt               err( KErrNone );
    
    // Check if there is already a linger object for this connection
    for ( TInt i=0; i < iLingerArray.Count(); i++ )
        {
        if  ( iLingerArray[ i ]->ConnectionId() == aConnectionId )
            {
            delete iLingerArray[ i ];    
            iLingerArray.Remove( i );    
            break; 
            }
        }
    
    if ( ptr == NULL )
        {
        TConnectionInfo    connInfo;
        
        connInfo.iIapId =  aIapId;
        connInfo.iNetId =  aNetId;
    
        ptr = CLingerConnection::NewL( aConnectionId, 
                                       connInfo, 
                                       iSettings, 
                                       &iSocketServ );
                                   
        iLingerArray.Append( ptr );
        }
                                   
                                           
    if ( iSettings.AccessPointId() == aIapId )
        {
        // Always-on-iap
        if ( !IsAlwaysOnEnabled() )
            {
            // Start linger since iap is not in always-on mode
            err = ptr->StartLinger();
            
            if ( err )
                {
                // write to log
                LOG_2( _L("ptr->StartLinger: err: %d"), err );
                }
            }
        }
    else
        {
        // common iap
        err = ptr->StartLinger();

        if ( err )
            {
            // write to log
            LOG_2( _L("ptr->StartLinger: err: %d"), err );
            }
        }    
    }

// ---------------------------------------------------------------------------
// CAOConnectionManager::HandleError
// ---------------------------------------------------------------------------
//
void CAOConnectionManager::HandleError( TInt /*aError*/ )
    {
    LOG_1( _L("CAOConnectionManager::HandleError") );
    
    // Notify that PDP context has disconnected since there is something wrong
    // with network
    TRAP_IGNORE( iObserver.HandlePDPContextDisconnectedL(
        MAOConnectionManager::EDisconnected ) );
    }
