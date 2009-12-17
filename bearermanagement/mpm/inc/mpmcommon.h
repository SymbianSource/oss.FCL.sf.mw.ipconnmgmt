/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This header defines common definitions for MPM
*
*/

/**
@file mpmcommon.h
Mobility Policy Manager common definitions.
*/

#ifndef MPMCOMMON_H
#define MPMCOMMON_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// Server name
_LIT( KMPMServerName,"!MPMServer" );
// MPM Default Connection server name
_LIT( KMPMDefaultConnectionServerName,"!MPMDefaultConnectionServer" );

// EXE name
_LIT( KMPMServerImg,"MPMServer" );

const TUid KServerUid2={0x1000008d};
const TUid KServerUid3={0x101f6d12};

const TInt KMPMServerStackSize    = 0x2000;    //  8KB
const TInt KMPMServerInitHeapSize = 0x1000;    //  4KB
const TInt KMPMServerMaxHeapSize  = 0x1000000; // 16MB

// The server version. A version must be specified when 
// creating a session with the server
const TUint KMPMServerMajorVersionNumber = 5;
const TUint KMPMServerMinorVersionNumber = 2;
const TUint KMPMServerBuildVersionNumber = 0;

// The number of message slots allocated for a client session
// Number of necessary message slots is determined as follows. 
// If these conditions change number of message slots may need to be increased. 
// 3 for asyncronous methods (ChooseBestIAP, ProcessError, WaitNotification)
// 1 for cancelling asynchronous requests
// 5 for synchronous requests. Following functions use Send and could 
// theoretically be called before previous syncronous call has completed: 
//
// ApplicationLeavesConnection, UnregisterPrefIAPNotif, 
// IAPConnectionActivated, ApplicationMigratesToCarrier,
//  
// and one of following (These use SendReceive):
// RegisterPrefIAPNotif, ApplicationIgnoredTheCarrier, 
// ApplicationAcceptedTheCarrier, ApplicationRejectedTheCarrier, 
// ApplicationConnectionEnds, SortSNAP, ApplicationJoinsConnection, 
// IAPConnectionStopped, IAPConnectionStarted, EasyWlanSsid
//
const TUint KNumMessageSlots = 9;

// The number of message slots allocated for MPM Default Connection 
// client session
const TUint KNumDefConnMessageSlots = 1;

// MACROS

// DATA TYPES
// Opcodes used in message passing between client and server
enum TMPMServerRequests
    {
    EMPMServerChooseIap,
    EMPMServerCancelRequest,
    EMPMServerApplicationLeavesConnection,
    EMPMServerIapConnectionStopped,
    EMPMServerProcessError,
    EMPMServerRegisterPrefIAPNotif,
    EMPMServerUnregisterPrefIAPNotif,
    EMPMServerWaitNotification, 
    EMPMServerSortSNAP,
    EMPMServerApplicationJoinsConnection,
    EMPMServerIapConnectionActivated, 
    EMPMServerIapConnectionStarted, 
    EMPMServerApplicationConnectionEnds, 
    EMPMServerApplicationMigratesToCarrier, 
    EMPMServerApplicationIgnoredTheCarrier, 
    EMPMServerApplicationAcceptedTheCarrier, 
    EMPMServerApplicationRejectedTheCarrier, 
    EMPMServerEasyWlanSsid,
    EMPMServerReselectIap
    };

/**  Request codes for MPM Default Connection server */
enum TMPMDefaultConnectionRequests
    {
    EMPMDefaultConnectionSetDefaultIap,
    EMPMDefaultConnectionClearDefaultIap
    };

// Panic codes used for server and client
// Code values selected just for being different
// than some normal common error or return values. 
enum TMPMPanic
    {
    // Server
    EMPMReceiveAlreadyActive     = 100,
    
    // Client
    ERMPMPanicClientNotConnected = 150    
    };
      
#endif // MPMCOMMON_H

// End of File
