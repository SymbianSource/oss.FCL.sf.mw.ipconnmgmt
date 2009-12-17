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
* Description:  Implements the TAOStateDisconnected class
*
*/


// INCLUDE FILES
#include "taostatedisconnected.h"
#include "logger.h"
#include "maotimer.h"
#include "maostatecontext.h"
#include "maostatepool.h"

// UNNAMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
    _LIT( KPanicCat, "StateDisconnected" );
    
    // DATA TYPES
    enum TPanicReason
        {
        EInvalidFailureReason
        };
    
    // LOCAL FUNCTIONS
    LOCAL_C void Panic( TPanicReason aReason )
        {
        User::Panic( KPanicCat, aReason );
        }
    }

// METHODS

// ---------------------------------------------------------------------------
// TAOStateDisconnected::TAOStateDisconnected
// ---------------------------------------------------------------------------
//
TAOStateDisconnected::TAOStateDisconnected(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateDisconnected::TAOStateDisconnected") );
    }

// ---------------------------------------------------------------------------
// TAOStateDisconnected::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateDisconnected::StateName() const
    {
    LOG_2( _L("TAOStateDisconnected::StateName: %d"),
        TAOState::EStateDisconnected );
    
    return TAOState::EStateDisconnected;
    }

// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleSwitchFromDisconnectedL(
    MAOConnectionManager::TFailureReason aReason )
    {
    LOG_1( _L("TAOStateDisconnected::HandleSwitchFromDisconnectedL") );
    
    TAOState* newState = NULL;
    MAOTimer& timer = iStateContext.Timer();
    MAOConnectionManager& connectionManager =
        iStateContext.ConnectionManager();
    
    switch ( aReason )
        {
        case MAOConnectionManager::ETemporary:
            {
            if ( !timer.RetryTimerDisabled() )
                {
                // Retry timer is not disabled on other words timer value
                // is != 0 (infinite)
                timer.StartRetryTimer();
                newState = iStatePool.ChangeState( TAOState::EStateWaiting );
                }
            else
                {
                // Retry timer disabled - Go to unconnected and wait for RAU
                newState = iStatePool.ChangeState( 
                    TAOState::EStateUnconnected );
                if( !iStateContext.Timer().UnconnectTimerDisabled() )
                    {
                    // Start unconnect timer to poll connection
                    // in unconnected state
                    timer.StartUnconnectTimer();
                    }
                }
            break;
            }
        case MAOConnectionManager::EPermanent:
            {
            newState = iStatePool.ChangeState( TAOState::EStateUnconnected );
            if( !iStateContext.Timer().UnconnectTimerDisabled() )
                {
                // Start unconnect timer to poll connection
                // in unconnected state
                timer.StartUnconnectTimer();
                }
            break;
            }
        case MAOConnectionManager::EDisconnected:
            {
            timer.StartConnectionTimer();
            connectionManager.ActivatePDPContextL();
            newState = iStatePool.ChangeState( TAOState::EStateConnecting );
            break;
            }
        default:
            {
            // Should not be here
            Panic( EInvalidFailureReason );
            break;
            }
        }
        
    return newState;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateDisconnected::HandleConnectionDeletedL") );
    
    return TAOState::DoHandleConnectionDeletedL();
    }
    
    
    // These all must be ignored in disconnected state.
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandlePDPContextDisconnectedL( 
                                TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisconnected::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisconnected::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// End of file
