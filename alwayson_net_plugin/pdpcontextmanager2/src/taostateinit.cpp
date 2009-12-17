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
* Description:  Implements the TAOStateInit class
*
*/


// INCLUDE FILES
#include "taostateinit.h"
#include "logger.h"
#include "maostatepool.h"

//    METHODS

// ---------------------------------------------------------------------------
// TAOStateInit::TAOStateInit
// ---------------------------------------------------------------------------
//
TAOStateInit::TAOStateInit(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateInit::TAOStateInit") );
    }

// ---------------------------------------------------------------------------
// TAOStateInit::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateInit::StateName() const
    {
    LOG_2( _L("TAOStateInit::StateName: %d"),
        TAOState::EStateInit );
    
    return TAOState::EStateInit;
    }

// ---------------------------------------------------------------------------
// TAOStateInit::HandleNetworkChangedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleNetworkChangedL(
    MAOConnectionManager::TNetworkType aNetworkType )
    {
    LOG_1( _L("TAOStateInit::HandleNetworkChangedL") );
    
    TAOState* newState = NULL;
    if ( aNetworkType == MAOConnectionManager::EHPLMN ||
        aNetworkType == MAOConnectionManager::EVPLMN )
        {
        SetFlag( ENWRegistrationReceived );
        if ( CheckFlag( ESettingsReceived ) )
            {
            // Conditions met to change 'connecting' state
            ClearFlags();
            newState = iStatePool.ChangeState( EStateStarting );
            }
        }
    return newState;
    }
            
// ---------------------------------------------------------------------------
// TAOStateInit::HandleSettingsReceivedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleSettingsReceivedL()
    {
    LOG_1( _L("TAOStateInit::HandleSettingsReceivedL") );
    
    TAOState* newState = NULL;
    
    // Set always on mode to TSY
    SetAlwaysOnModeL();
    
    // Set flag that settings have been received
    SetFlag( ESettingsReceived );
    if ( CheckFlag( ENWRegistrationReceived ) )
        {
        // Conditions met to change 'connecting' state
        ClearFlags();
        newState = iStatePool.ChangeState( EStateStarting );
        }
    
    return newState;
    }
    
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandlePDPContextDisconnectedL( TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }    
// ---------------------------------------------------------------------------
// TAOStateInit::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateInit::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleSwitchFromDisconnectedL(
    MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateInit::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateInit::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }
        
// End of file
