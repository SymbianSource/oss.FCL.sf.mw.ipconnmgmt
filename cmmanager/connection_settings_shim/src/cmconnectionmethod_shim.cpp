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

#include <cmconnectionmethod.h>
#include <cmconnectionmethod_shim.h>

// User includes

#include "cmconnectionmethod_shim_s60_p.h"

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

CmConnectionMethodShim::CmConnectionMethodShim(
    RCmConnectionMethod &cm)
{
    d_ptr = new CmConnectionMethodShimPrivate(cm);
}

CmConnectionMethodShim::~CmConnectionMethodShim()
{
    delete d_ptr;
}
   
uint CmConnectionMethodShim::getIntAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return d_ptr->GetIntAttribute(attribute);
}

bool CmConnectionMethodShim::getBoolAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return d_ptr->GetBoolAttribute(attribute);
}

QString CmConnectionMethodShim::getStringAttribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return d_ptr->GetStringAttribute(attribute);
}

QString CmConnectionMethodShim::getString8Attribute( 
    CMManagerShim::ConnectionMethodAttribute attribute) const
{
    return d_ptr->GetString8Attribute(attribute);
}

void CmConnectionMethodShim::setIntAttribute(
    CMManagerShim::ConnectionMethodAttribute attribute,
    uint value)    
{
    d_ptr->SetIntAttribute(attribute, value); 
}

void CmConnectionMethodShim::setBoolAttribute(
    CMManagerShim::ConnectionMethodAttribute attribute,
    bool value)    
{
    d_ptr->SetBoolAttribute(attribute, value);
}

void CmConnectionMethodShim::setStringAttribute(
    CMManagerShim::ConnectionMethodAttribute attribute, 
    QString value)    
{
    d_ptr->SetStringAttribute(attribute, value);
}

void CmConnectionMethodShim::setString8Attribute(
    CMManagerShim::ConnectionMethodAttribute attribute, 
    QString value)    
{
    d_ptr->SetString8Attribute(attribute, value);
}

bool CmConnectionMethodShim::deleteConnectionMethod()
{
    return d_ptr->DeleteConnectionMethod();
}
    
void CmConnectionMethodShim::update()
{
    d_ptr->Update();
}

QString CmConnectionMethodShim::getIcon() const
{
    return d_ptr->GetIcon();
}

void CmConnectionMethodShim::setIcon(
    QString icon)    
{
    d_ptr->SetIcon(icon);
}

void CmConnectionMethodShim::refresh()
{
    d_ptr->Refresh();
}
