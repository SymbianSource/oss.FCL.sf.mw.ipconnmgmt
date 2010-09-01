/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  commonly used constants for the WLAN plugin
*
*/

#ifndef CMWLANCOMMONCONSTANTS_H
#define CMWLANCOMMONCONSTANTS_H

// CONSTANTS
_LIT( KPluginWlanResDirAndFileName, "z:cmwlanui.rsc" );
_LIT( KEmptyAddress, "http://" );

/// 'Dynamic' IP address to use in comparisons
_LIT( KDynIpAddress, "0.0.0.0" );

_LIT( KDynIpv6Address, "0:0:0:0:0:0:0:0" );

const TInt KCmArrayGranularity = 10;
const TInt KCmAdhocChannelMaxLength = 2;
const TInt KCmAdhocChannelMinValue = 1;
const TInt KCmAdhocChannelDefaultValue = 7;
const TInt KCmAdhocChannelMaxValue = 11;
const TInt KCmWlanNWNameMaxLength = 32;

#endif  // CMWLANCOMMONCONSTANTS_H
