/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines central repository key for VPN toggle
*
*/

#ifndef MPMVPNTOGGLEAPI_H
#define MPMVPNTOGGLEAPI_H

/**
 * The UID of the repository to be accessed.
 */
const TUid KMpmVpnToggleCenRepUid = {0x20016A88}; 

/**
 * This key contains value, which defines if VPN connection is preferred
 * connection. Client can set this value if it has WriteDeviceData and
 * NetworkControl capabilities.
 */
const TUint32 KMpmVpnTogglePreferVpn = 0x00000001;

/**
 * This key contains VPN IAP Id value for VPN IAP which is used to establish
 * VPN connection, when VPN connection is preferred. Client can set this value
 * if it has WriteDeviceData and NetworkControl capabilities. TUint32 value
 * needs to be converted from TInt.
 */
const TUint32 KMpmVpnToggleIapId = 0x00000002;

/**
 * This key contains SNAP Id value SNAP which is used to establish VPN
 * connection, when VPN connection is preferred. Client can set this value
 * if it has WriteDeviceData and NetworkControl capabilities. TUint32 value
 * needs to be converted from TInt.
 */
const TUint32 KMpmVpnToggleSnapId = 0x00000003;

#endif // MPMVPNTOGGLEAPI_H
