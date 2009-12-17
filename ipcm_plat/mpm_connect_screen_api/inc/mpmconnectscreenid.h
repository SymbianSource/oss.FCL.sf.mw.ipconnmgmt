/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines central repository key for connect screen id.
*
*/

#ifndef MPMCONNECTSCREENID_H
#define MPMCONNECTSCREENID_H

/**
 * The UID of the repository to be accessed.
 *
 * @since S60 5.1
 */
const TUid KMpmOccCenRepUid = {0x101F6D50}; 

/**
 * This key contains process uid of the connect screen. Client can set this
 * value if it has WriteDeviceData and NetworkControl capabilities. Note that
 * zero means that user connection can not be established.
 *
 * @since S60 5.1
 */
const TUint32 KMpmConnectScreenId  = 0x00000001;

#endif // MPMCONNECTSCREENID_H
