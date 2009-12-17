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
* Description:  Implements the TAOStateUnconnected class
*
*/


// INCLUDE FILES
#include "taostateunconnected.h"
#include "logger.h"
#include "maostatecontext.h"
#include "maostatepool.h"
#include "maoconnectionmanager.h"
#include "maotimer.h"

//    METHODS

// Constructor
TAOStateUnconnected::TAOStateUnconnected(
    MAOStateContext& aStateContext, MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateUnconnected::TAOStateUnconnected") );
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateUnconnected::StateName() const
    {
    LOG_2( _L("TAOStateUnconnected::StateName: %d"),
        TAOState::EStateUnconnected );
    
    return TAOState::EStateUnconnected;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateUnconnected::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateUnconnected::HandleConnectionDeletedL") );
    
    return TAOState::DoHandleConnectionDeletedL();
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleUnconnectTimerExpiredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateUnconnected::HandleUnconnectTimerExpiredL()
    {
    LOG_1( _L("TAOStateUnconnected::HandleUnconnectTimerExpiredL") );
    
    // First check that we don't have a connection ongoing.
    // Browser acts funny if we try to open a connection while browsing
    TAOState* newState = NULL;
    if( !iStateContext.ConnectionManager().NumberOfConnections() )
        {
        // No ongoing connections, try to activate always on connection
        newState = DoActivatePDPContextL();
        }
    else
        {
        // Already connected to some APN.
        // Do not try to open a new connection but restart timer
        iStateContext.Timer().StartUnconnectTimer();
        }
    return newState;
    }
    
// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandlePDPContextActivatedL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandlePDPContextActivationFailedL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandlePDPContextDisconnectedL( 
                               TInt /*aReason*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleGeneralError
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandleGeneralError()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandleEnableAlwaysOnL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//    
TAOState* TAOStateUnconnected::HandleSwitchFromDisconnectedL(
    MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateUnconnected::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
// 
TAOState* TAOStateUnconnected::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }
    
// End of file
