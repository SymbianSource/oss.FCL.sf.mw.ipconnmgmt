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
* Description:  General connectivity settings.
*
*/
#ifndef CMGENCONNSETTINGS_H
#define CMGENCONNSETTINGS_H


// Usage types for new WLAN
enum TCmUsageOfWlan
    {
    ECmUsageOfWlanKnown,
    ECmUsageOfWlanKnownAndNew, // default 
    ECmUsageOfWlanManual   
    };

// Seamlessness values
enum TCmSeamlessnessValue
        {
        ECmSeamlessnessNotRoamedTo,   // IAP can be used in start, but not roamed to unless already active
        ECmSeamlessnessConfirmFirst,  // Confirm first
        ECmSeamlessnessShowprogress,  // Show progress
        ECmSeamlessnessFullySeamless, // Same as previous
        ECmSeamlessnessDisabled       // IAP blacklisted and not used at all
        };

// Table for connection settings other than default connection
struct TCmGenConnSettings
    {
    TCmUsageOfWlan         iUsageOfWlan;
    TCmSeamlessnessValue   iSeamlessnessHome;
    TCmSeamlessnessValue   iSeamlessnessVisitor;
    };

#endif

