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

#ifndef CMCONNECTIONMETHOD_SHIM_H
#define CMCONNECTIONMETHOD_SHIM_H

// System includes

#include <QString>
#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations

class CmConnectionMethodShimPrivate;

// External data types

// Constants

// Class declaration

/*!
    \class CmConnectionMethodShim
    \brief Implementation class for CM Manager Connection Method Shim.

    Wrapper for CM Manager Symbian interface.
    
    NOTE! This shim is supposed to be used only in middleware code. Usage
    must be agreed with ipconnmgmt package owner. Applications should use
    the Qt Bearer Management API instead.
*/

class CONNECTIONSETTINGSSHIMDLL_EXPORT CmConnectionMethodShim
{

public:

    // Data types

    /*!
     *  Constructor.
     *  
     *  @param[in] cm CM Manager connection method object.
     */
    CmConnectionMethodShim(RCmConnectionMethod &cm);

    /*!
     *  Destructor.
     */
    ~CmConnectionMethodShim();
    
    /*!
     * Gets the value for a uint attribute.
     * Note: even though the name suggests int attribute, return value is
     * actually uint. This is how the function is specified in public
     * RCmConnectionMethod class as well.
     * 
     * @param attribute Identifies the attribute to be retrived.
     * @return Contains the requested uint attribute.
     */
    uint getIntAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;

    /*!
     * Gets the value for a bool attribute.
     * 
     * @param attribute Identifies the attribute to be retrived.
     * @return Contains the requested bool attribute.
     */
    bool getBoolAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;

    /*!
     * Gets the value for a 16 bit QString attribute.
     * 
     * @param attribute Identifies the attribute to be retrived.
     * @return Contains the requested QString attribute.
     */
    QString getStringAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;
    
    /*!
     * Gets the value for an 8 bit QString attribute.
     * 
     * @param attribute Identifies the attribute to be retrived.
     * @return Contains the requested QString attribute.
     */
    QString getString8Attribute(
        CMManagerShim::ConnectionMethodAttribute attribute) const;
    
    /*!
     * Sets the value for an uint attribute.
     * Note: even though the name suggests int attribute, parameter is
     * actually uint. This is how the function is specified in public
     * RCmConnectionMethod class as well.
     * 
     * @param attribute Identifies the attribute to be set.
     * @param value The value to be set.
     */
    void setIntAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute,
        uint value);

    /*!
     * Sets the value for a bool attribute.
     * 
     * @param attribute Identifies the attribute to be set.
     * @param value The value to be set.
     */
    void setBoolAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute,
        bool value);

    /*!
     * Sets the value for a 16 bit QString attribute.
     * 
     * @param attribute Identifies the attribute to be set.
     * @param value The value to be set.
     */
    void setStringAttribute(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);
    
    /*!
     * Sets the value for an 8 bit QString attribute.
     * 
     * @param attribute Identifies the attribute to be set.
     * @param value The value to be set.
     */
    void setString8Attribute(
        CMManagerShim::ConnectionMethodAttribute attribute, 
        QString value);
    
    /*!
     * Try to delete the connection method. If it is referenced from 
     * any destination, then the references are removed and the connection
     * method is deleted.
     * 
     * @return True if connection method deleted.
     */
    bool deleteConnectionMethod();
    
    /*!
     * Update in the database.
     */        
    void update();
    
    /*!
     * Gets the icon identifier(name) of the connection method. It can
     * contain path information.
     * @return Contains string representing icon
     */
    QString getIcon() const;
    
    /*!
     * Sets the icon identifier(name) of the destination. It can
     * contain path information.
     * 
     * @param icon New icon
     */      
    void setIcon(QString icon);
    
    /*!
     * Reload the contents of the connection method from database.
     * Any current changes made to the connection method are lost.
     */
    void refresh();
    
protected:
    
private:
    
private: // data
    
    CmConnectionMethodShimPrivate *d_ptr;  //!< Private implementation
    
    // Friend classes
    
    /*
     * CmDestinationShimPrivate::AddConnectionMethodL needs direct access
     * to Connection Methods it is adding to the Destination.
     */
    friend class CmDestinationShimPrivate;
};

#endif /* CMCONNECTIONMETHOD_SHIM_H */
