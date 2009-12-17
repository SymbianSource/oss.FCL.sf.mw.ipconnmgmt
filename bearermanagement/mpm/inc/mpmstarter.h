/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This header defines starter for MPM server
*
*/

/**
@file mpmstarter.h
Mobility Policy Manager server starter definitions.
*/

#ifndef MPMSTARTER_H
#define MPMSTARTER_H

//  INCLUDES
#include <e32svr.h>
class CMPMServer;
// CLASS DECLARATION
/**
*  Mobility Policy Manager server starter.
*  @lib MPMServer.exe
*  @since 3.0
*/
class MPMStarter
    {
    public: // New methods

        /**
        * Creates and starts server.
        * @since 3.0
        * @return Pointer to the server object.
        */
        static CServer2* CreateAndStartServerL();

        /**
        * Creates and starts server Default connection server.
        * @since 3.2
        * @return Pointer to the server object.
        */
        static CServer2* CreateDefaultConnServerL( CMPMServer* aMPMServer );

        /**
        * Gets server name.
        * @since 3.0
        * @return Descriptor of the server name.
        */
        static TPtrC ServerName();
    };

#endif // MPMSTARTER_H

// End of File
