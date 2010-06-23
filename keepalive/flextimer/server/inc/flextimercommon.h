/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: 
 * Common header for flextimer client interface and server.
 *
 */
/*
 * %version: 1 %
 */
// Protection against nested includes
#ifndef FLEXTIMERCOMMON_H
#define FLEXTIMERCOMMON_H

// System includes
#include <e32base.h>

#include "flextimerpanic.h"

// Constants
/**
 * FlexTimerServer executable name used by clients when starting server
 */
_LIT( KFlexTimerServerExe, "FlexTimerServer.exe" );

/**
 * Identification name for the FlexTimerServer -server
 */
_LIT( KFlexTimerServerName,"FlexTimerServer" );

/**
 * Number of message slots reserved for each client session.
 * There can be at most one outstanding asynchronous request at a time
 * (pending timeout message). Second slot is needed for synchronous
 * communication during that (e.g. for cancellation message).
 */
const TUint KFlexTimerServerMessageSlots = 2;

/**
 *  Version identifier. Client-server message interface compatibility
 *  is maintained within a major version.
 */
const TUint KFlexTimerServMajorVersionNumber = 1;
const TUint KFlexTimerServMinorVersionNumber = 0;
const TUint KFlexTimerServBuildVersionNumber = 0;

/**
 * Default timeout window size. This is used, if client has not set the
 * used timeout window. Default window is a fraction of an interval
 * between current time and the expiration time.
 * 
 * @code
 *   interval = endTime - currentTime;
 *   defaultWindow = interval * KDefaultWindowMultiplier;
 *   startTime = endTime - defaultWindow;
 * @endcode
 */
const TReal64 KDefaultWindowMultiplier = 0.2;

/**
 *  Maximun lenght of FlexTimer  63072000000000 == 2a in microseconds.
 */
const TInt64 KFlexTimerMaxTimerLength = 63072000000000;

// Data types
/**
 * Client-server interface message types (functions)
 */
enum TFlexTimerServRqst
    {
    EFlexTimerServCancelRequest = 1,
    EFlexTimerServAtRequest,
    EFlexTimerServAtUTCRequest,
    EFlexTimerServAfterRequest,
    EFlexTimerServAfterTicksRequest,
    EFlexTimerServConfigureRequest
    };

/**
 * Configuration message (EFlexTimerServConfigureRequest) type.
 * This is stored as a first parameter in configuration request message,
 * rest of the parameters and their types depend on this.
 */
enum TFlexTimerConfigureRequestType
    {
    EConfigureRequestWindowSize = 1
    };

#endif /* FLEXTIMERCOMMON_H */
