/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  
 * Wrapper for CM Manager Symbian interface.
 */

// System includes

#include <e32base.h>
#include <cmdestination.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cmdestination_shim.h>

// User includes

#include "cmmanager_shim_s60_p.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CmManagerShim::CmManagerShim()
{
    d_ptr = new CmManagerShimPrivate();
}

CmManagerShim::~CmManagerShim()
{
    delete d_ptr;
}

CmDestinationShim *CmManagerShim::createDestination(QString name)
{
    return d_ptr->CreateDestination(name);
}
    
CmConnectionMethodShim *CmManagerShim::createConnectionMethod(uint bearerType)
{
    return d_ptr->CreateConnectionMethod(bearerType);
}
    
CmConnectionMethodShim *CmManagerShim::connectionMethod(
    uint connectionMethodId) const
{
    return d_ptr->ConnectionMethod(connectionMethodId);
}
    
void CmManagerShim::connectionMethod(
    QList<uint> &cmArray,
    bool legacyOnly) const
{
    return d_ptr->ConnectionMethod(
        cmArray,
        legacyOnly);
}

CmDestinationShim *CmManagerShim::destination(uint destinationId) const
{
    return d_ptr->Destination(destinationId);
}
            
void CmManagerShim::allDestinations(QList<uint> &destArray) const
{
    return d_ptr->AllDestinations(destArray);
}
