/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the TAOStateExternalConnection class
*
*/

// INCLUDE FILES
#include <e32property.h>

#include "taostateexternalconnection.h"
#include "logger.h"
#include "pdpcontextmanagerpskeys.h"

//	METHODS

// Constructor
TAOStateExternalConnection::TAOStateExternalConnection(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
	{
	LOG_1( _L("TAOStateExternalConnection::TAOStateExternalConnection") );
	}

// ----------------------------------------------------------------------------
// TAOStateExternalConnection::StateName
// ----------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateExternalConnection::StateName() const
    {
    LOG_2( _L("TAOStateExternalConnection::StateName: %d"),
        TAOState::EStateExternalConnection );
    
    return TAOState::EStateExternalConnection;
    }

// ----------------------------------------------------------------------------
// TAOStateExternalConnection::HandleConnectionDeletedL
// ----------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleConnectionDeletedL()
    {
    LOG_1( _L("TAOStateExternalConnection::HandleConnectionDeletedL") );
        
    return DoActivatePDPContextL();
    }

// ----------------------------------------------------------------------------
// TAOStateExternalConnection::HandleResetFactorySettingsL
// ----------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleResetFactorySettingsL(
    TBool /*aReset*/ )
    {
    LOG_1( _L("TAOStateExternalConnection::HandleResetFactorySettingsL") );
        
    // Respond to Rfs
    RProperty::Set(
        KPDPContextManager2,
        KPDPContextManagerFactorySettingsReset,
        EPDPContextManagerFactorySettingsResetStartReply );
        
    return NULL;
    }
    
      
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandlePDPContextActivatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandlePDPContextActivationFailedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandlePDPContextDisconnectedL( 
                                        TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleNetworkChangedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType /*aNetworkType*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleSettingsReceivedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleSettingsReceivedL()
    {
    return NULL;
    }    
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }    

// ---------------------------------------------------------------------------
// TAOStateExternalConnection::HandleUnconnectTimerExpiredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateExternalConnection::HandleUnconnectTimerExpiredL()
    {
    return NULL;
    }
    

// End of file
