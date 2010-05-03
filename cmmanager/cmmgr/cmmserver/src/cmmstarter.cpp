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
* Creates and starts Connection Method Manager server.
*
*/


#include "cmmstarter.h"
#include "cmmserverdefs.h"
#include "cmmserver.h"


CServer2* CCmmStarter::CreateAndStartServerL()
    {
    return CCmmServer::NewL();
    }

TPtrC CCmmStarter::ServerName()
    {
    return KCmmServer().Mid( 0 );
    }

// End of file
