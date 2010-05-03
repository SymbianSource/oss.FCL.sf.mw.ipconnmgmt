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
 * Private implementation of Wrapper for CM Manager Symbian interface.
 */

#ifndef CMCONNECTIONMETHOD_SHIM_S60_P_H
#define CMCONNECTIONMETHOD_SHIM_S60_P_H

// System includes

#include <QString>
#include <cmconnectionmethod.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class CmConnectionMethodShimPrivate
{

public:
    
    // Data types

    CmConnectionMethodShimPrivate(RCmConnectionMethod &cm);

    ~CmConnectionMethodShimPrivate();
    
    uint GetIntAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;

    bool GetBoolAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;

    QString GetStringAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;
    
    QString GetString8Attribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;
    
    void SetIntAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute,
        uint value);

    void SetBoolAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute,
        bool value);

    void SetStringAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);

    void SetString8Attribute(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);
    
    bool DeleteConnectionMethod();

    void Update();
    
    void Refresh();
    
protected:

private:

    Q_DISABLE_COPY(CmConnectionMethodShimPrivate)

    void GetStringAttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute,
        QString &value) const;
   
    void GetString8AttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute,
        QString &value) const;
   
    void SetIntAttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute,
        uint value);

    void SetBoolAttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute,
        bool value);

    void SetStringAttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);
   
    void SetString8AttributeL(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);
    
    void TraceIfError(TInt error) const;
    
private: // data    
    
    RCmConnectionMethod iCm;  //!< CmManager Connection Method instance
    
    // Friend classes

    /*
     * CmDestinationShimPrivate::AddConnectionMethodL needs direct access
     * to Connection Methods it is adding to the Destination.
     */
    friend class CmDestinationShimPrivate;
};

#endif // CMCONNECTIONMETHOD_SHIM_S60_P_H
