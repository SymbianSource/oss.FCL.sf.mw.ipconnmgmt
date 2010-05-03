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

#include <cmdestination.h>
#include <cmconnectionmethod.h>
#include <cmdestination_shim.h>

// User includes

#include "cmdestination_shim_s60_p.h"
#include "cmconnectionmethod_shim_s60_p.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CmDestinationShim::CmDestinationShim(RCmDestination &destination)
{
    d_ptr = new CmDestinationShimPrivate(destination);
}

CmDestinationShim::~CmDestinationShim()
{
    delete d_ptr;
}
                 
int CmDestinationShim::connectionMethodCount() const
{
    return d_ptr->ConnectionMethodCount();
}
                
CmConnectionMethodShim *CmDestinationShim::connectionMethod(int index) const
{
    return d_ptr->ConnectionMethod(index);
}
           
CmConnectionMethodShim *CmDestinationShim::connectionMethodByID(
    uint cmId) const
{    
    return d_ptr->ConnectionMethodByID(cmId);
}

uint CmDestinationShim::priority(CmConnectionMethodShim *cmItem) const
{
    return d_ptr->Priority(cmItem);
}
               
QString CmDestinationShim::name() const
{    
    return d_ptr->Name();
}
                             
uint CmDestinationShim::id() const
{
    return d_ptr->Id();
}

uint CmDestinationShim::metadata(
    CMManagerShim::SnapMetadataField metadataField) const
{
    return d_ptr->Metadata(metadataField);
}

CMManagerShim::CmmProtectionLevel CmDestinationShim::protectionLevel() const
{
    return d_ptr->ProtectionLevel();
}
           
bool CmDestinationShim::isHidden() const
{
    return d_ptr->IsHidden();
}
           
int CmDestinationShim::addConnectionMethod(CmConnectionMethodShim *cm)
{
    return d_ptr->AddConnectionMethod(cm);
}

void CmDestinationShim::deleteConnectionMethod(CmConnectionMethodShim *cm)
{
    d_ptr->DeleteConnectionMethod(cm);
}
       
void CmDestinationShim::removeConnectionMethod(CmConnectionMethodShim *cm)
{
    d_ptr->RemoveConnectionMethod(cm);
}
             
void CmDestinationShim::modifyPriority(CmConnectionMethodShim *cm, int index)
{
    d_ptr->ModifyPriority(cm, index);
}
           
void CmDestinationShim::setName(QString name)
{
    d_ptr->SetName(name);
}
              
void CmDestinationShim::update()
{
    d_ptr->Update();
}

void CmDestinationShim::refresh()
{
    d_ptr->Refresh();
}

void CmDestinationShim::deleteDestination()
{
    d_ptr->DeleteDestination();
}
