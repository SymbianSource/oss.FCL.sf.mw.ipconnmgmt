/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PDPContextManager Settings API 
*               Provides access to the PDPContextManager settings 
*
*/

#ifndef PDPCONTEXTMANAGERINTERNALCRKEYS_H
#define PDPCONTEXTMANAGERINTERNALCRKEYS_H

const TUid KCRUidPDPContextManager = {0x102071F5};


/** 
* Defines whether always online functionality is used when user is roaming. 
*
* Integer type
*
* Default value: 0
*/
const TUint32 KPDPContextManagerEnableWhenRoaming        = 0x00000001;

/**
* Access point ID that is used as always online context.
*
* Integer type
*
* Default value: 0
*/
const TUint32 KPDPContextManagerDefaultUid                = 0x00000002;

/**
* Retry interval in minutes which is used if connection fails.
*
* Integer type
*
* Default value: ""
**/
const TUint32 KPDPContextManagerRetryTimer                = 0x00000003;

/** 
* Defines whether always online functionality is used when user 
* is in home network.
*
* Integer type
*
* Default value: 0
**/
const TUint32 KPDPContextManagerEnableWhenHome            = 0x00000004;

/** 
* Accesspoint related linger value.
*
* Binary type
*
* Default value: 0
**/
const TUint32  KPDPContextManagerLinger                   = 0x00000005;

/**
* Connection timer value.
*
* Integer type
*
* Default value: 30
**/
const TUint32 KPDPContextManagerConnectionTimer           = 0x00000006;

/**
* Always-on feature dynamic flag.
*
* Integer type
*
* Default value: 0
*
* 0 = Disabled
* 1 = Enabled
**/
const TUint32 KPDPContextAlwaysOnEnabled                  = 0x00000007;

/**
* Polling timer in unconnected state.
* A new connection will be tried after interval specified in the timer.
* Timer value in seconds.
*
* Integer type
*
* Default value: 10
*/
const TUint32 KPDPContextManagerUnconnectTimer           = 0x00000008;

/**
* Number of iap id & linger interval entries in dynamic table.
*
* Integer type
*
* Default value: 0
*/
const TUint32 KPdpContextManagerLingerArrayCount         = 0x00000009;

#endif
