/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Server allows registering IAP to be used as default connection
*
*/

#ifndef CMPMDEFAULTCONNSERVER_H
#define CMPMDEFAULTCONNSERVER_H

#include <e32base.h>

#include "mpmserver.h"

class CMPMServer;

/**
 *  Server allows registering IAP to be used as default connection.
 *  Server allows WLAN Active Idle plug-in to register the started 
 *  WLAN IAP as default connection. Only one client is served at the time.
 *
 *
 *  @lib MPMServer.exe
 *  @since S60 v3.2
 */
class CMPMDefaultConnServer : public CPolicyServer
    {

public:

    /**
     * Two-phased constructor.
     */
    static CMPMDefaultConnServer* NewL( CMPMServer* aMPMServer );

    /**
    * Destructor.
    */
    virtual ~CMPMDefaultConnServer();

    /**
     * Creates new session
     *
     * @since S60 v3.2
     * @return MPMServer object
     */
    CMPMServer* MPMServer();

// from base class CServer2


    /**
     * From CServer2
     * Creates new session
     *
     * @since S60 v3.2
     * @param aVersion Version information
     * @param aMessage Message, not used here
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage) const;

private:

    CMPMDefaultConnServer( CMPMServer* aMPMServer );

    void ConstructL();


private: // data

    /**
     * Reference to MPM Server for storing the default IAP. 
     * Not own.
     */
    CMPMServer* iMPMServer;

    };

#endif // CMPMDEFAULTCONNSERVER_H
