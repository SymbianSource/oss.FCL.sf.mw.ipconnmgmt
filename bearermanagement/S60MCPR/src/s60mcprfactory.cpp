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
* Description: S60 MCPR factory implementation
*
*/

/**
@file s60mcprfactory.cpp
S60 MCPR factory implementation
*/

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_msgintercept.h>

#include "s60mcprfactory.h"
#include "s60mcpr.h"

using namespace ESock;

#ifdef __CFLOG_ACTIVE
#define KS60MCprFactoryTag KESockMetaConnectionTag
_LIT8(KS60MCprFactorySubTag, "s60mcprfactory");
#endif

// -----------------------------------------------------------------------------
// CS60MetaConnectionProviderFactory::NewL
// -----------------------------------------------------------------------------
//
CS60MetaConnectionProviderFactory* CS60MetaConnectionProviderFactory::NewL( TAny* aParentContainer )
    {
    __CFLOG_VAR((KS60MCprFactoryTag, KS60MCprFactorySubTag, _L8("CS60MetaConnectionProviderFactory::NewL(%08x)"), aParentContainer));
     return new (ELeave) CS60MetaConnectionProviderFactory( TUid::Uid(CS60MetaConnectionProviderFactory::iUid),
                                                            *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)) );
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProviderFactory::CS60MetaConnectionProviderFactory
// -----------------------------------------------------------------------------
//
CS60MetaConnectionProviderFactory::CS60MetaConnectionProviderFactory( TUid aFactoryUid, 
                                                                      CMetaConnectionFactoryContainer& aParentContainer )
    :   CMetaConnectionProviderFactoryBase( aFactoryUid, aParentContainer )
    {
    }

// -----------------------------------------------------------------------------
// CS60MetaConnectionProviderFactory::DoCreateObjectL
// -----------------------------------------------------------------------------
//
ACommsFactoryNodeId* CS60MetaConnectionProviderFactory::DoCreateObjectL( TFactoryQueryBase& aQuery )
    {
    const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
    CMetaConnectionProviderBase* provider = CS60MetaConnectionProvider::NewL( *this, query.iProviderInfo );

    ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);

    return provider;
    }

