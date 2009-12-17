/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for bearer abstraction class.
*
*/

#include "ConnMonBearer.h"
#include "ConnMonIAP.h"
#include "connmoncommsdatcache.h"
#include "log.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
TConnMonBearer::TConnMonBearer(
        CConnMonIAP* aConnMonIAP,
        CConnMonCommsDatCache* aCommsDatCache )
        :
        iConnMonIAP( aConnMonIAP ),
        iCommsDatCache( aCommsDatCache )
    {
    __ASSERT_DEBUG( NULL != aConnMonIAP, User::Panic( KPanicBadParameter, KErrBadHandle ) );
    }

// End-of-file
