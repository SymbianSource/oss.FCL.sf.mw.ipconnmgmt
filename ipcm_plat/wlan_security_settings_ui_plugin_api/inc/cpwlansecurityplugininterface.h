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
*   Control Panel plugin interface for WLAN security settings plugins.
*
*/

#ifndef CPWLANSECURITYPLUGININTERFACE_H
#define CPWLANSECURITYPLUGININTERFACE_H

// System includes
#include <QtPlugin>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations
class CmConnectionMethodShim;
class CpSettingFormItemData;
class CpItemDataHelper;

// External data types

// Constants

// Class declaration

/*!
    \class CpWlanSecurityPluginInterface
    \brief This class is an interface for WLAN security settings plugins
           which can be displayed in Control Panel application.
           
    Plugin stubs must be located in
    /resource/qt/plugins/controlpanel/wlansecurity directory.
 */

class CpWlanSecurityPluginInterface
{
public:
    /*!
        Destructor.
     */
    virtual ~CpWlanSecurityPluginInterface() {}

    /*!
        Getter for security mode. Security modes are defined in Connection
        Settings Shim WlanSecMode enum, e.g. WlanSecModeWep.

        \return Security mode identifier.
     */
    virtual CMManagerShim::WlanSecMode securityMode() const = 0;
    
    /*!
        Getter for localization text identifier for the security mode,
        e.g. "txt_occ_setlabel_wlan_security_mode_val_wep". This localized
        text will be shown in the UI.

        \return Localization text ID.
     */
    virtual QString securityModeTextId() const = 0;

    /*! 
        Sets the database reference. Plugin can either use Connection
        Settings Shim to access the settings or some other way using the
        provided connection method ID.

        \param cmConnectionMethod Pointer to CmConnectionMethodShim instance
               which can be used for reading and writing of the settings. Not
               owned by the plugin.
        \param id Connection method ID.
     */
    virtual void setReference(
        CmConnectionMethodShim *cmConnectionMethod,
        uint id) = 0;

    /*!
        Getter for order number. This order number is used to put the
        security modes in correct order in the security mode list. Security
        mode with the lowest order number will be first and so on.

        E.g. WEP = 10, 802.1X = 20, WPA = 30, WPA2 = 40, WAPI = 50

        \return Order number.
     */
    virtual int orderNumber() const = 0;

    /*!
        Creates an UI instance ("Security settings" group with all settings
        for the security mode). Caller owns the object. The security plugin
        doesn't need to handle deletion of the UI instance, it will be
        handled by the caller and Control Panel framework. Every call shall
        create a new UI instance.

        \param dataHelper Item data helper.
        \return Pointer to an UI object.
     */
    virtual CpSettingFormItemData* uiInstance(
        CpItemDataHelper &dataHelper) = 0;

signals:

public slots:

protected:

protected slots:

private:

private slots:

private: // data

};

Q_DECLARE_INTERFACE(
    CpWlanSecurityPluginInterface,
    "com.nokia.plugin.controlpanel.wlansecurity.platform.interface/1.0");

#endif // CPWLANSECURITYPLUGININTERFACE_H
