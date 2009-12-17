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
* Description:  Implements the TAOStateWaiting class
*
*/


// INCLUDE FILES
#include "taostatewaiting.h"
#include "logger.h"
#include "maostatecontext.h"
#include "maostatepool.h"
#include "maotimer.h"

//    METHODS

// ---------------------------------------------------------------------------
// TAOStateWaiting::TAOStateWaiting
// ---------------------------------------------------------------------------
//
TAOStateWaiting::TAOStateWaiting(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateWaiting::TAOStateWaiting") );
    }

// ---------------------------------------------------------------------------
// TAOStateWaiting::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateWaiting::StateName() const
    {
    LOG_2( _L("TAOStateWaiting::StateName: %d"),
        TAOState::EStateWaiting );
    
    return TAOState::EStateWaiting;
    }

// ---------------------------------------------------------------------------
// TAOStateWaiting::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleSuccesfulRAUEventL()
    {
    LOG_1( _L("TAOStateWaiting::HandleSuccesfulRAUEventL") );
    
    return DoActivatePDPContextL();
    }

// ---------------------------------------------------------------------------
// TAOStateWaiting::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleRetryTimerTriggeredL()
    {
    LOG_1( _L("TAOStateWaiting::HandleRetryTimerTriggeredL") );

    return DoActivatePDPContextL();
    }

// ---------------------------------------------------------------------------
// TAOStateWaiting::DoActivatePDPContextL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::DoActivatePDPContextL()
    {
    LOG_1( _L("TAOStateWaiting::DoActivatePDPContextL") );
    
    iStateContext.Timer().StartConnectionTimer();
    iStateContext.ConnectionManager().ActivatePDPContextL();
    return iStatePool.ChangeState( TAOState::EStateConnecting );
    }

// ---------------------------------------------------------------------------
// TAOStateWaiting::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateWaiting::HandleConnectionDeletedL") );
    
    return TAOState::DoHandleConnectionDeletedL();
    }

// These all must be ignored in waiting state.
// @see TAOState
        
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandlePDPContextActivationFailedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandlePDPContextDisconnectedL( TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateWaiting::
// ---------------------------------------------------------------------------
//
TAOState* TAOStateWaiting::HandleSwitchFromDisconnectedL(
            MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }


// End of file
