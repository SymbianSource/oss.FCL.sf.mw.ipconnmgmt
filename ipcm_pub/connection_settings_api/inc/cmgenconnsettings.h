/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


// Usage types for WLAN
enum TCmUsageOfWlan
    {
    // Known WLANs will be used, and they will be switched to
    // if they become available during existing connections.
    ECmUsageOfWlanKnown,
    // Known and new WLANs will be used. 
    ECmUsageOfWlanKnownAndNew, 
    // WLANs will be used but they will not be switched to
    // from existing connections.
    ECmUsageOfWlanManual   
    };

// Cellular data usage values
enum TCmCellularDataUsage
    {
    // Cellular data connection is not used automatically but a confirmation
    // is requested from the user.
    ECmCellularDataUsageConfirm,
    // Cellular data connection is automatically used without querying
    // permission from the user.
    ECmCellularDataUsageAutomatic,
    // Cellular data usage is disabled. Only WLAN will be used.
    ECmCellularDataUsageDisabled,
    // Cellular data connection is automatically used without querying
    // permission from the user in home network.
    // NOTE: This value indicates that cellular data is automatically used
    // in home network only, not in other networks within home country.
    // That is, outside home network in home country, user permission is
    // queried for cellular data usage.
    // This value is only applicable in parameter
    // TCmGenConnSettings.iCellularDataUsageHome when used with
    // RCmManager::ReadGenConnSettingsL() or
    // RCmManager::WriteGenConnSettingsL() methods.
    ECmCellularDataUsageAutomaticInHomeNetwork
    };

// Table for connection settings other than default connection
struct TCmGenConnSettings
    {
    // WLAN usage.
    TCmUsageOfWlan         iUsageOfWlan;
    // Cellular data usage in home country.
    TCmCellularDataUsage   iCellularDataUsageHome;
    // Cellular data usage abroad.
    // NOTE: This parameter is never set to
    // ECmCellularDataUsageAutomaticInHomeNetwork.
    TCmCellularDataUsage   iCellularDataUsageVisitor;
    };

#endif
