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
* Description:  Implements the TAOStateStarting class
*
*/


// INCLUDE FILES
#include "taostatestarting.h"
#include "logger.h"

//    METHODS

// ---------------------------------------------------------------------------
// TAOStateStarting::TAOStateStarting
// ---------------------------------------------------------------------------
//
TAOStateStarting::TAOStateStarting( MAOStateContext& aStateContext, 
                                    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateStarting::TAOStateStarting") );
    }

// ---------------------------------------------------------------------------
// TAOStateStarting::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateStarting::StateName() const
    {
    LOG_2( _L("TAOStateStarting::StateName: %d"),
        TAOState::EStateStarting );
    
    return TAOState::EStateStarting;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType /*aNetworkType*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleSettingsReceivedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandlePDPContextDisconnectedL( TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }
// ---------------------------------------------------------------------------
// TAOStateStarting::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateStarting::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }
    
// End of file
