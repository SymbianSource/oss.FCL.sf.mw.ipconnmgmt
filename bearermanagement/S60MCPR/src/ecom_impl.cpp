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
* Description: ECOM plugin
*
*/

/**
@file ecom_impl.cpp
ECOM plugin
*/

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

#include "s60mcprfactory.h"
#include "s60tiermanagerfactory.h"

//
// ECOM Implementation
//

const TImplementationProxy ImplementationTable[] =
    {   
    IMPLEMENTATION_PROXY_ENTRY(CS60MetaConnectionProviderFactory::iUid, CS60MetaConnectionProviderFactory::NewL),
    IMPLEMENTATION_PROXY_ENTRY(CS60TierManagerFactory::iUid, CS60TierManagerFactory::NewL), 
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

