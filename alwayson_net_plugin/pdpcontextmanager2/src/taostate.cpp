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
* Description:  Implements the TAOState class
*
*/


// INCLUDE FILES
#include <e32property.h>

#include "taostate.h"
#include "logger.h"
#include "maoconnectionmanager.h"
#include "maostatecontext.h"
#include "maotimer.h"
#include "maostatepool.h"
#include "maosettings.h"
#include "maogpds.h"
#include "pdpcontextmanagerpskeys.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
TAOState::TAOState( MAOStateContext& aStateContext, 
                    MAOStatePool& aStatePool ):
    iStateContext( aStateContext ),
    iStatePool( aStatePool ),
    iFlags( 0 )
    {
    LOG_1( _L("TAOState::TAOState") );
    }

// --------------------------------------------------------------------------
// TAOState::HandlePDPContextActivatedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandlePDPContextActivatedL()
    {
    LOG_1( _L("TAOState::HandlePDPContextActivatedL") );

    return NULL;    
    }

// --------------------------------------------------------------------------
// TAOState::HandlePDPContextActivationFailedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandlePDPContextActivationFailedL()
    {
    LOG_1( _L("TAOState::HandlePDPContextActivationFailedL") );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOState::HandlePDPContextDisconnectedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandlePDPContextDisconnectedL( TInt /*aReason*/ )
    {
    LOG_1( _L("TAOState::HandlePDPContextDisconnectedL") );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOState::HandleNetworkChangedL
// --------------------------------------------------------------------------
//
#ifdef LOGGING_ENABLED
TAOState* TAOState::HandleNetworkChangedL(
    MAOConnectionManager::TNetworkType aNetworkType )
#else
TAOState* TAOState::HandleNetworkChangedL(
    MAOConnectionManager::TNetworkType /*aNetworkType*/ )
#endif
    {
    LOG_2( _L("TAOState::HandleNetworkChangedL: aNetworkType: %d"), 
              aNetworkType );

    // PLMN change notification
    // Cancel all requests
    CancelAll();
    return iStatePool.ChangeState( EStateStarting );
    }

// --------------------------------------------------------------------------
// TAOState::HandleSuccesfulRAUEventL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleSuccesfulRAUEventL()
    {
    LOG_1( _L("TAOState::HandleSuccesfulRAUEventL") );
    
    TAOState* newState = NULL;
    
    // If we don't have PDP context active try to activate it
    if ( !iStateContext.ConnectionManager().IsPDPContextActive() )
        {
        CancelAll();
        newState = DoActivatePDPContextL();
        }
    
    return newState;
    }

// --------------------------------------------------------------------------
// TAOState::HandleRetryTimerTriggeredL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleRetryTimerTriggeredL()
    {
    LOG_1( _L("TAOState::HandleRetryTimerTriggeredL") );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOState::HandleConnectionTimerTriggeredL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleConnectionTimerTriggeredL()
    {
    LOG_1( _L("TAOState::HandleConnectionTimerTriggeredL") );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOState::HandleSettingsReceivedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleSettingsReceivedL()
    {
    LOG_1( _L("TAOState::HandleSettingsReceivedL") );

    // By default if settings are changed OTA
    // cancel all requests and switch to starting state    
    CancelAll();
    
    // Set Always-On mode to cellmo side
    SetAlwaysOnModeL();
        
    return iStatePool.ChangeState( EStateStarting );
    }

// --------------------------------------------------------------------------
// TAOState::HandlePDPContextTemporarilyBlockedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandlePDPContextTemporarilyBlockedL()
    {
    LOG_1( _L("TAOState::HandlePDPContextTemporarilyBlockedL") );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOState::HandleGeneralError
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleGeneralError()
    {
    LOG_1( _L("TAOState::HandleGeneralError") );
    
    // Error occured:
    // Cancel all operations and go to disconnected state
    CancelAll();
    return iStatePool.ChangeState( EStateDisconnected );
    }

// --------------------------------------------------------------------------
// TAOState::CancelAll
// --------------------------------------------------------------------------
//
void TAOState::CancelAll()
    {
    LOG_1( _L("TAOState::CancelAll") );

    // Cancel timers
    CancelTimers();
    
    // Close current connection    
    iStateContext.ConnectionManager().CloseConnection();
    }

// --------------------------------------------------------------------------
// TAOState::SetAlwaysOnModeL
// --------------------------------------------------------------------------
//
void TAOState::SetAlwaysOnModeL()
    {
    LOG_1( _L("TAOState::SetAlwaysOnModeL") );
    
    MAOGpds::TAlwaysOnMode mode = MAOGpds::ENeither;
    MAOSettings& settings = iStateContext.Settings();
    if ( settings.AlwaysOnSupportedInHPLMN() )
        {
        mode = MAOGpds::EHPLMN;
        }
    if ( settings.AlwaysOnSupportedInVPLMN() )
        {
        if ( mode == MAOGpds::EHPLMN )
            {
            mode = MAOGpds::EBoth;
            }
        else
            {
            mode = MAOGpds::EVPLMN;
            }
        }
    iStateContext.Gpds().SetAlwaysOnModeL( mode );
    }

// --------------------------------------------------------------------------
// TAOState::HandleEnableAlwaysOnL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleEnableAlwaysOnL()
    {
    LOG_1( _L("TAOState::HandleEnableAlwaysOnL") );
    
    TAOState* newState = NULL;
    if ( EnableAlwaysOn() )
        {
        // start PDP connection
        newState = DoActivatePDPContextL();
        }
    else
        {
        newState = iStatePool.ChangeState( TAOState::EStateDisabled );
        }
        
    return newState;
    }

// --------------------------------------------------------------------------
// TAOState::HandleSwitchFromDisconnectedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleSwitchFromDisconnectedL(
    MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    LOG_1( _L("TAOState::HandleSwitchFromDisconnectedL") );
    
    return NULL;    
    }

// --------------------------------------------------------------------------
// TAOState::HandleConnectionDeletedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOState::HandleConnectionDeletedL") );
    
    return NULL;    
    }

// --------------------------------------------------------------------------
// TAOState::HandleResetFactorySettingsL
// --------------------------------------------------------------------------
//
TAOState* TAOState::HandleResetFactorySettingsL( TBool aReset )
    {
    LOG_1( _L("TAOState::HandleResetFactorySettingsL") );
    
    TAOState* newState = NULL;
    if ( aReset )
        {
        // Close connection
        CancelAll();
        
        // Respond to Rfs that connection has been torn down and rfs sequence
        // can continue
        RProperty::Set(
            KPDPContextManager2,
            KPDPContextManagerFactorySettingsReset,
            EPDPContextManagerFactorySettingsResetStartReply );
            
        newState = iStatePool.ChangeState( EStateResetFactorySettings );
        }
    else
        {
        newState = DoActivatePDPContextL();
        }
    return newState;
    }

// --------------------------------------------------------------------------
// TAOState::EnableAlwaysOn
// --------------------------------------------------------------------------
//
TBool TAOState::EnableAlwaysOn()
    {
    LOG_1( _L("TAOState::EnableAlwaysOn") );
    
    TBool enabled = EFalse;
    MAOConnectionManager& connectionManager =
        iStateContext.ConnectionManager();
    MAOSettings& settings = iStateContext.Settings();
    
    // If new state is in starting state, we'll check
    // whether Always-On should be supported in current
    // network
    MAOConnectionManager::TNetworkType nwType =
        connectionManager.NetworkType();
    switch ( nwType )
        {
        case MAOConnectionManager::EHPLMN:
            {
            if ( settings.AlwaysOnSupportedInHPLMN() )
                {
                enabled = ETrue;
                }
            break;
            }
        case MAOConnectionManager::EVPLMN:
            {
            if ( settings.AlwaysOnSupportedInVPLMN() )
                {
                enabled = ETrue;
                }
            break;
            }
        default:
            {
            // Not enabled
            break;
            }
        }
        
    LOG_2( _L("TAOState::EnableAlwaysOn >> Enabled: %d"), enabled );
    
    return enabled;
    }

// --------------------------------------------------------------------------
// TAOState::CancelTimers
// --------------------------------------------------------------------------
//
void TAOState::CancelTimers()
    {
    LOG_1( _L("TAOState::CancelTimers") );
    
    iStateContext.Timer().StopConnectionTimer();
    iStateContext.Timer().StopRetryTimer();
    iStateContext.Timer().StopUnconnectTimer();
    }

// --------------------------------------------------------------------------
// TAOState::DoActivatePDPContextL
// --------------------------------------------------------------------------
//
TAOState* TAOState::DoActivatePDPContextL()
    {
    LOG_1( _L("TAOState::DoActivatePDPContextL") );
    
    iStateContext.Timer().StartConnectionTimer();
    iStateContext.ConnectionManager().ActivatePDPContextL();
    
    return iStatePool.ChangeState( TAOState::EStateConnecting );
    }

// --------------------------------------------------------------------------
// TAOState::DoHandleConnectionDeletedL
// --------------------------------------------------------------------------
//
TAOState* TAOState::DoHandleConnectionDeletedL()
    {
    LOG_1( _L("TAOState::DoHandleConnectionDeletedL") );
    
    MAOConnectionManager& connectionManager =
        iStateContext.ConnectionManager();
    TAOState* newState = NULL;
        
    if ( !connectionManager.IsPDPContextActive() )
        {
        CancelAll();
        newState = DoActivatePDPContextL();
        }
        
    return newState;
    }

// ---------------------------------------------------------------------------
// TAOState::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOState::HandleExternalConnectionCreatedL()
    {
    LOG_1( _L("TAOState::HandleExternalConnectionCreatedL") );
    
    CancelAll();
    return iStatePool.ChangeState( TAOState::EStateExternalConnection );
    }

// ---------------------------------------------------------------------------
// TAOState::HandleUnconnectTimerExpiredL
// ---------------------------------------------------------------------------
//
TAOState* TAOState::HandleUnconnectTimerExpiredL()
    {
    LOG_1( _L("TAOState::HandleUnconnectTimerExpiredL") );
    
    return NULL;
    }

// End of file
