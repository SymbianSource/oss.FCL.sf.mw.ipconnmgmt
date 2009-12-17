/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 IP TM factory implementation
*
*/


/**
@file s60tiermanagerfactory.cpp
S60 IP TM factory implementation
*/

#include <comms-infras/ss_log.h>

#include "s60tiermanagerfactory.h"
#include "s60tiermanager.h"

#ifdef __CFLOG_ACTIVE
#define KS60TierMgrFactoryTag KESockMetaConnectionTag
_LIT8(KS60TierMgrSubTag, "s60tiermanager");
#endif // __CFLOG_ACTIVE

using namespace ESock;

// -----------------------------------------------------------------------------
// CS60TierManagerFactory::NewL
// -----------------------------------------------------------------------------
//
CS60TierManagerFactory* CS60TierManagerFactory::NewL(TAny* aParentContainer)
    {
    __CFLOG_VAR((KS60TierMgrFactoryTag, KS60TierMgrSubTag, _L8("S60MCPR::CS60TierManagerFactory::\tNewL(%08x)"), aParentContainer));
     return new (ELeave) CS60TierManagerFactory(
		 TUid::Uid(CS60TierManagerFactory::iTypeId),
		 TUid::Uid(CS60TierManagerFactory::iUid),
		 *(reinterpret_cast<CTierManagerFactoryContainer*>(aParentContainer)) );
    }

// -----------------------------------------------------------------------------
// CS60TierManagerFactory::CS60TierManagerFactory
// -----------------------------------------------------------------------------
//
CS60TierManagerFactory::CS60TierManagerFactory( TUid aTierTypeId, TUid aFactoryUid, CTierManagerFactoryContainer& aParentContainer )
    :   CTierManagerFactoryBase( aTierTypeId, aFactoryUid, aParentContainer )
    {
    }

// -----------------------------------------------------------------------------
// CS60TierManagerFactory::DoCreateObjectL
// -----------------------------------------------------------------------------
//
ESock::ACommsFactoryNodeId* CS60TierManagerFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */)
    {
    return CS60TierManager::NewL(*this);
    }
