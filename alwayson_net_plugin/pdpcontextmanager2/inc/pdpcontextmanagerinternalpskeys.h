/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Publish & Subscribe keys only for testing
*
*/


#ifndef PDPCONTEXTMANAGERINTERNALPSKEYS_H
#define PDPCONTEXTMANAGERINTERNALPSKEYS_H

// NOTE:
//
// These PS keys are only internal to PDP context manager and are
// used only by the stub implementations.
//
// USE FOR TESTING PURPOSES ONLY!

// RAU event keys
const TUid KRAUEventCat = {0x00000001};
const TUint KRAUGenerateEvent = 1;
enum
    {
    ENormalRAU,
    EPeriodicalRAU
    };

// Connection monitor keys
const TUid KConnectionMonitorCat = {0x00000002};
const TUint KNetworkRegStatusChange     = 1;
const TUint KExternalPDPContextCreated  = 2;
const TUint KNewPDPContextCreated       = 3;
enum
    {
    ENetworkNotRegistered,
    ENetworkHome,
    ENeworkRoaming
    };

// Connection keys
const TUid KConnectionCat = {0x00000003};
const TUint KPDPContextActivation       = 1;
const TUint KPDPContextDisconnection    = 2;

#endif // PDPCONTEXTMANAGERINTERNALPSKEYS_H
