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
* Description:
* Launches the Connection Method Manager Server.
*
*/


#ifndef CMMCLISTATIC_H
#define CMMCLISTATIC_H

#include <e32std.h>

const TUint KCmmDefaultMinHeapSize =  0x1000;  //  4K
const TUint KCmmDefaultMaxHeapSize = 0x10000;  // 64K

const TInt KCmmDefaultMessageSlots = 2;

_LIT( KCmManagerStartupMutex, "CmManagerStartupMutex" );


/**
 * Class to handle CmManager server startup.
 */
class TCmManagerLauncher
    {
public:
    static TInt LaunchServer(
            const TDesC& aServerName,
            const TDesC& aServerFileName,
            const TUid& aServerUid3,
            const TUint aWinsMinHeapSize = KCmmDefaultMinHeapSize,
            const TUint aWinsMaxHeapSize = KCmmDefaultMaxHeapSize,
            const TUint aWinsStackSize = KDefaultStackSize );

private:
    static TBool ServerStartupLock( RMutex& mutex, TInt& err );
    static void ServerStartupUnlock( RMutex& mutex );
    };

#endif // CMMCLISTATIC_H

// End of file
