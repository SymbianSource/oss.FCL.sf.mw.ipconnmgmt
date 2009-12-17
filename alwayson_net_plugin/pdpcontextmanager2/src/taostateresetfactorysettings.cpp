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
* Description:  Implements the TAOStateResetFactorySettings class
*
*/


// INCLUDE FILES
#include <e32property.h>

#include "taostateresetfactorysettings.h"
#include "logger.h"
#include "pdpcontextmanagerpskeys.h"

//    METHODS

// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::TAOStateResetFactorySettings
// ---------------------------------------------------------------------------
//
TAOStateResetFactorySettings::TAOStateResetFactorySettings(
    MAOStateContext& aStateContext,
    MAOStatePool& aStatePool ):
    TAOState( aStateContext, aStatePool )
    {
    LOG_1( _L("TAOStateResetFactorySettings::TAOStateResetFactorySettings") );
    }

// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::StateName
// ---------------------------------------------------------------------------
//
TAOState::TAOStateName TAOStateResetFactorySettings::StateName() const
    {
    LOG_2( _L("TAOStateResetFactorySettings::StateName: %d"),
        TAOState::EStateInit );
    
    return TAOState::EStateResetFactorySettings;
    }

// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleResetFactorySettingsL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleResetFactorySettingsL(
    TBool aReset )
    {
    LOG_1( _L("TAOStateResetFactorySettings::HandleResetFactorySettingsL") );
    
    TAOState* newState = NULL;
    if ( !aReset )
        {
        newState = DoActivatePDPContextL();
        }
        
    return newState;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::TAOStateResetFactorySettings
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandlePDPContextActivatedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandlePDPContextActivationFailedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandlePDPContextActivationFailedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandlePDPContextDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandlePDPContextDisconnectedL( 
                                        TInt /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleNetworkChangedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleNetworkChangedL(
        MAOConnectionManager::TNetworkType /*aNetworkType*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleSuccesfulRAUEventL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleSuccesfulRAUEventL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleRetryTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleRetryTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleConnectionTimerTriggeredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleConnectionTimerTriggeredL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleSettingsReceivedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleSettingsReceivedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandlePDPContextTemporarilyBlockedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandlePDPContextTemporarilyBlockedL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleGeneralError
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleGeneralError()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleEnableAlwaysOnL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleEnableAlwaysOnL()
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleSwitchFromDisconnectedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleSwitchFromDisconnectedL(
        MAOConnectionManager::TFailureReason /*aReason*/ )
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleConnectionDeletedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleConnectionDeletedL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleExternalConnectionCreatedL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleExternalConnectionCreatedL()
    {
    return NULL;
    }    

// ---------------------------------------------------------------------------
// TAOStateResetFactorySettings::HandleUnconnectTimerExpiredL
// ---------------------------------------------------------------------------
//
TAOState* TAOStateResetFactorySettings::HandleUnconnectTimerExpiredL()
    {
    return NULL;
    }

// End of file
