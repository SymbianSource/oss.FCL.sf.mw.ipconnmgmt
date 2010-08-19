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
* Description:  CmManager central repository keys.
*
*/

#ifndef CMMANAGERKEYS_H
#define CMMANAGERKEYS_H

/**
 * UID of CmManager repository
 *
 * @since S60 5.2
 */
const TUid KCRUidCmManager = {0x10207376}; 

/**
 * Cellular data usage setting in current network.
 *
 * This means that the client does not need to take into account 
 * whether the device is in the home country or in a foreign country 
 * but the value is provided for the country the device is at the moment.
 * See TCmCellularDataUsage in cmgenconnsettings.h for more info.
 *
 * @since S60 5.2
 */
const TUint32 KCurrentCellularDataUsage  = 0x00000001;

/**
 * Dial-up override setting.
 *
 * When this key is enabled dial-up connection overrides all
 * internal cellular connections. Internal cellular connections 
 * are automatically disconnected when dial-up connection starts.
 *
 * @since S60 5.2
 */
const TUint32 KDialUpOverride  = 0x00000002;
    
#endif // CMMANAGERKEYS_H
