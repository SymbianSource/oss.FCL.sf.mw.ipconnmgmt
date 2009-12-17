/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Contains private Central Repository keys.
*
*/


#ifndef MPMPRIVATECRKEYS_H
#define MPMPRIVATECRKEYS_H


const TUid KCRUidMPM = {0x101F6D12}; 

// Wlan scan feature on or off.
const TUint32 KMpmWlanScanEnabled  = 0x00000001;

// Wlan scan interval in low mode
const TUint32 KMpmWlanScanLowInterval = 0x00000002;

// Wlan scan interval in high mode
const TUint32 KMpmWlanScanHighInterval = 0x00000003;

// First entry for the dedicated clients
const TUint32 KMpmDedicatedClientBase = 0x00001000;

#endif      // MPMPRIVATECRKEYS_H

// End of File
