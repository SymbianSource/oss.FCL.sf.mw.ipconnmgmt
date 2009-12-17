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
* Description:  Implements the TAOStateDisabled class
*
*/


// INCLUDE FILES
#include "taostatedisabled.h"
#include "logger.h"
#include "maostatepool.h"

//    METHODS

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TAOStateDisabled::TAOStateDisabled( MAOStateContext& aStateContext, 
                                    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateDisabled::TAOStateDisabled") );
    }

// --------------------------------------------------------------------------
// TAOStateDisabled::StateName
// --------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateDisabled::StateName() const
    {
    LOG_2( _L("TAOStateDisabled::StateName: %d"),
        TAOState::EStateDisabled );
    
    return TAOState::EStateDisabled;
    }

// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleNetworkChangedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleNetworkChangedL(
    MAOConnectionManager::TNetworkType aNetworkType )
    {
    LOG_1( _L("TAOStateDisabled::HandleNetworkChangedL") );
    
    TAOState* newState = NULL;
    
    if ( aNetworkType == MAOConnectionManager::EHPLMN ||
        aNetworkType == MAOConnectionManager::EVPLMN )
        {
        newState = iStatePool.ChangeState( TAOState::EStateStarting );
        }
    
    return newState;
    }
    
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
    TAOState* TAOStateDisabled::HandlePDPContextDisconnectedL(
                                TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateDisabled::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateDisabled::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }
    
// End of file
