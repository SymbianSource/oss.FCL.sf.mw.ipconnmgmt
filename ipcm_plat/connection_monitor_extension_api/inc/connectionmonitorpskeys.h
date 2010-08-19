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
* Description:  Connection Monitor Publish & Subscribe keys.
*
*/


#ifndef CONNECTIONMONITORPSKEYS_H
#define CONNECTIONMONITORPSKEYS_H

/**
 * Connection Monitor RProperty category.
 */
const TUid KConnectionMonitorPS = {0x101F6CF0};

/**
 * Dial-Up override key.
 * Used only when dial-up override feature is enabled. The purpose for this
 * feature is to stop and block any packetdata connections while a dial-up
 * connection is beeing established in single PDP context networks.
 */
const TUint KDialUpConnection = 0x00000001;

/**
 * Values for KDialUpConnection key.
 */
enum TDialUpStates
    {
    /**
     * Can be used to indicate to Connection Monitor Server that a dial-up
     * connection has ended. If the dial-up connection setup fails, this can be
     * used to restore normal packetdata connectivity a little faster in some
     * error situations.
     */
    EConnMonDialUpClosed,
    /**
     * Indicates to Connection Monitor Server that a Dial-Up connection is
     * starting. Connection Monitor will stop any active packetdata connection
     * and block further packetdata connections during the dial-up connection
     * setup.
     */
    EConnMonDialUpInit,
    /**
     * Connection Monitor Server will indicate with this value that any
     * preparations for an upcoming dial-up connection are completed.
     */
    EConnMonReady
    };

#endif // CONNECTIONMONITORPSKEYS_H
