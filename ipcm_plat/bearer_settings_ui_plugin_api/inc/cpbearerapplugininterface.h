/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*   Control Panel plugin interface for bearer specific AP settings plugins.
*
*/

#ifndef CPBEARERAPPLUGININTERFACE_H
#define CPBEARERAPPLUGININTERFACE_H

// System includes
#include <QtPlugin>

// User includes

// Forward declarations
class CpBaseSettingView;

// External data types

// Constants

// Class declaration

/*!
    \class CpBearerApPluginInterface
    \brief This class is an interface for bearer specific AP settings plugins
           which can be displayed in the Control Panel application.
    
    Plugin stubs must be located in
    /resource/qt/plugins/controlpanel/bearerap directory.
 */

class CpBearerApPluginInterface
{
public:
    /*!
        Destructor.
     */
    virtual ~CpBearerApPluginInterface() {}
    
    /*!
        Getter for bearer type. Bearer types are defined in the Connection
        Settings Shim API cmmanagerdefines_shim.h.
        
        \return Bearer type handled by the plugin.
     */
    virtual uint bearerType() const = 0;
    
    /*!
        Creates the settings view. Caller owns the object.
        
        \param connectionMethod Connection method ID.
        \return Pointer to a settings view object.
     */
    virtual CpBaseSettingView *createSettingView(
        uint connectionMethod) = 0;
        
signals:
    
public slots:

protected:

protected slots:

private:

private slots:

private: // data

};

Q_DECLARE_INTERFACE(
    CpBearerApPluginInterface,
    "com.nokia.plugin.controlpanel.bearerap.platform.interface/1.0");

#endif // CPBEARERAPPLUGININTERFACE_H
