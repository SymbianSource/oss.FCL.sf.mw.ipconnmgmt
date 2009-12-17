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
* Description:  Implements the TAOStateConnecting class
*
*/


// INCLUDE FILES
#include "taostateconnecting.h"
#include "logger.h"
#include "maostatepool.h"
#include "maostatecontext.h"
#include "maotimer.h"

// UNNAMNED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    // CONSTANTS
    const TInt KMaxAttempts = 5;
    }

// METHODS

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TAOStateConnecting::TAOStateConnecting(
    MAOStateContext& aStateContext, MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool ),
    iConnectionCounter( 0 )
    {
    LOG_1( _L("TAOStateConnecting::TAOStateConnecting") );
    }

// ---------------------------------------------------------------------------
// TAOStateConnecting::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateConnecting::StateName() const
    {
    LOG_2( _L("TAOStateConnecting::StateName: %d"),
        TAOState::EStateConnecting );
    
    return TAOState::EStateConnecting;
    }

// ---------------------------------------------------------------------------
// TAOStateConnecting::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandlePDPContextActivatedL()
    {
    LOG_1( _L("TAOStateConnecting::HandlePDPContextActivatedL") );
    
    iStateContext.Timer().StopConnectionTimer();
    return iStatePool.ChangeState( TAOState::EStateConnected );
    }

// ---------------------------------------------------------------------------
// TAOStateConnecting::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandlePDPContextActivationFailedL()
    {
    LOG_1( _L("TAOStateConnecting::HandlePDPContextActivationFailedL") );
    
    iStateContext.Timer().StopConnectionTimer();
    return iStatePool.ChangeState( TAOState::EStateDisconnected );
    }

// ---------------------------------------------------------------------------
// TAOStateConnecting::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleConnectionTimerTriggeredL()
    {
    LOG_1( _L("TAOStateConnecting::HandleConnectionTimerTriggeredL") );
    
    TAOState* newState = NULL;
    
    // Connection took too long -> disconnect and retry
    CancelAll();
    iConnectionCounter++;
    if ( iConnectionCounter >= KMaxAttempts )
        {
        // Maximum attempts achieved -> change to disconnected
        iConnectionCounter = 0;
        newState = iStatePool.ChangeState( TAOState::EStateDisconnected );
        }
    else
        {
        // Try to reconnect and start connection timer
        iStateContext.Timer().StartConnectionTimer();
        }
    return newState;
    }

// ---------------------------------------------------------------------------
// TAOStateConnecting::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateConnecting::HandleConnectionDeletedL") );
    
    TAOState* newState = NULL;
    
    // If we are already connecting to default IAP
    // we can stay in connected state
    if ( iStateContext.ConnectionManager().IsPDPContextActive() )
        {
        newState = iStatePool.CurrentState();
        }
    // Not connected to default IAP, cancel all and go to disconnected state
    else
        {
        CancelAll();
        newState = iStatePool.ChangeState( TAOState::EStateDisconnected );
        }
    
    return newState;
    }
    
    
// ---------------------------------------------------------------------------
// TAOStateConnecting::HandlePDPContextDisconnectedL
// This is ignored in connecting state. Always returns NULL;
// ---------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandlePDPContextDisconnectedL( TInt 
                                                             /*aReason*/ )
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandleSuccesfulRAUEventL
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandleRetryTimerTriggeredL
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandlePDPContextTemporarilyBlockedL
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandleGeneralError
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleGeneralError()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandleEnableAlwaysOnL
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnecting::HandleSwitchFromDisconnectedL
// This is ignored in connecting state. Always returns NULL;
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnecting::HandleSwitchFromDisconnectedL(
    MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }


// End of file
