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
* Description:  Contains private Central Repository keys.
*
*/

#ifndef DCLPRIVATECRKEYS_H
#define DCLPRIVATECRKEYS_H

#include <dclcrkeys.h>

const TUid KCRUidDCL = {0x101F6CFE};

// GPRS data volume threshold (volume mode).
const TUint32 KDCLGprsThreshold  = 0x00000001;

// WCDMA data volume threshold (volume mode).
const TUint32 KDCLWcdmaThreshold = 0x00000002;

// WLAN data volume threshold (volume mode).
const TUint32 KDCLWlanThreshold  = 0x00000003;

// LAN data volume threshold (volume mode).
const TUint32 KDCLLanThreshold   = 0x00000004;

// Timer interval (timer mode).
const TUint32 KDCLTimerInterval  = 0x00000005;

// Force timer mode.
const TUint32 KDCLForceTimerMode = 0x00000006;

#endif // DCLPRIVATECRKEYS_H

// End of File
