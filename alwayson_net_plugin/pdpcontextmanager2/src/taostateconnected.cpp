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
* Description:  Implements the TAOStateConnected class
*
*/


// INCLUDE FILES
#include "taostateconnected.h"
#include "logger.h"
#include "maostatepool.h"
#include "maostatecontext.h"
#include "maotimer.h"
#include "maosettings.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
TAOStateConnected::TAOStateConnected(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateConnected::TAOStateConnected") );
    }

// --------------------------------------------------------------------------
// TAOStateConnected::StateName
// --------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateConnected::StateName() const
    {
    LOG_2( _L("TAOStateConnected::StateName: %d"),
        TAOState::EStateConnected );
    
    return TAOState::EStateConnected;
    }

// --------------------------------------------------------------------------
// TAOStateConnected::HandlePDPContextDisconnectedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandlePDPContextDisconnectedL( 
                             TInt /*aReason*/ )
    {
    LOG_1( _L("TAOStateConnected::HandlePDPContextDisconnectedL") );
    
    return iStatePool.ChangeState( TAOState::EStateDisconnected );
    }

// --------------------------------------------------------------------------
// TAOStateConnected::HandleConnectionDeletedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateConnected::HandleConnectionDeletedL") );
    
    TAOState* newState = NULL;
    
    // If we are already connected to default IAP
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


// --------------------------------------------------------------------------
// TAOStateConnected::HandlePDPContextActivatedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandlePDPContextActivatedL()
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// TAOStateConnected::HandleNetworkChangedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleNetworkChangedL(
    MAOConnectionManager::TNetworkType aNetworkType )
    {
    LOG_2( _L("TAOStateConnected::HandleNetworkChangedL: aNetworkType: %d"), 
              aNetworkType );

    // PLMN change notification
    if ( aNetworkType != MAOConnectionManager::ENotRegistered )
        {
	    if ( EnableAlwaysOn() )
	        {
	    	// Stay connected
	    	return NULL;
	        }
	    else
	        {
	        // Is linger enabled
	        if ( iStateContext.Settings().LingerTimerValue( 
	             iStateContext.Settings().AccessPointId() ) > 0 )
	            {
	        	// Go to disabled state but do not stop the connection. 
	        	// Linger timer will stop connection later.
	        	iStateContext.ConnectionManager().DetachConnection();
	        	CancelTimers();
	        	return iStatePool.ChangeState( EStateDisabled );
	            }
	        }	
        }
    
    // Either aNetworkType == MAOConnectionManager::ENotRegistered
    // or PLMN changed and always-on is disabled & linger is off
        
    // Cancel all requests
    CancelAll();
    return iStatePool.ChangeState( EStateStarting );	
    }
            
// --------------------------------------------------------------------------
// TAOStateConnected::HandleSuccesfulRAUEventL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandleRetryTimerTriggeredL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandleConnectionTimerTriggeredL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandlePDPContextTemporarilyBlockedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandleGeneralError
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleGeneralError()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandleEnableAlwaysOnL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// TAOStateConnected::HandleSwitchFromDisconnectedL
// --------------------------------------------------------------------------
//
TAOState* TAOStateConnected::HandleSwitchFromDisconnectedL(
          MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }


// End of file
