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
* Description: This header defines server launcher for MPM client.
*
*/

/**
@file mpmlauncher.h
Mobility Policy Manager server laucher for the client.
*/

#ifndef MPMLAUNCHER_H
#define MPMLAUNCHER_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS
_LIT( KMPMLauncherSemaphore, "MPMLauncherSemaphore" );

// CLASS DECLARATION
/**
*  Mobility Policy Manager server laucher for client.
*  @lib MPM.lib
*  @since 3.0
*/
NONSHARABLE_CLASS(MPMLauncher)
    {
    public: // New methods

        /**
        * Initiates MPM server starting.
        * @since 3.0
        * @param aServerFileName
        * @param aServerUid2
        * @param aServerUid3
        * @return KErrNone if starting OK.
        */
        static TInt LaunchServer(
            const TDesC& aServerFileName,
            const TUid& aServerUid2,
            const TUid& aServerUid3);

    };

#endif // MPMLAUNCHER_H

// End of File
