/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TUN Driver plug-in IF implementation class.
*
*/

#include "cmplugintundriver.h"
#include <e32std.h>
#include <ecom/implementationproxy.h>

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KPluginTUNDriverBearerTypeUid, CCmPluginTUNDriver::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
* ImplementationGroupProxy
* This is function is invoked by E-Com Plugin framework for TUNDriver plugin UID when needs to be loaded.
* @param 
* @return 
*/
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
