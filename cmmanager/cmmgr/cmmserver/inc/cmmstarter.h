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
* Creates and starts the connection method manager (CMM) server.
*
*/


#ifndef CMMSTARTER_H_
#define CMMSTARTER_H_

#include <e32base.h>


NONSHARABLE_CLASS( CCmmStarter ) : public CBase
    {
public:
    static CServer2* CreateAndStartServerL();
    static TPtrC ServerName();
    };

#endif // CMMSTARTER_H_

// End of file
