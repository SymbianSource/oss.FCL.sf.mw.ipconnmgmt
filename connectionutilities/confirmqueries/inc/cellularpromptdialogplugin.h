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
* Description: 
*
*/


#ifndef ____CELLULARPROMPTDIALOG_H__PLUGIN_H__
#define ____CELLULARPROMPTDIALOG_H__PLUGIN_H__

#include <QObject>
#include <hbdevicedialogplugin.h>

class CellularPromptDialog;

class CellularPromptDialogPlugin : public HbDeviceDialogPlugin
{
    Q_OBJECT

public:
    
    /* Constructor */
    CellularPromptDialogPlugin();
    /* Destructor */
    ~CellularPromptDialogPlugin();
    
    /* Check whether access is allowed, true is always returned */
    bool accessAllowed(const QString &deviceDialogType,
        const QVariantMap &parameters, const QVariantMap &securityInfo) const;
    
    /* Creates the dialog widget */
    HbDeviceDialogInterface *createDeviceDialog(const QString &deviceDialogType,
        const QVariantMap &parameters);
    
    /* Returns the device dialog type */
    bool deviceDialogInfo(const QString &deviceDialogType,
        const QVariantMap &parameters, DeviceDialogInfo *info) const;
    
    /* Returns a list of dialog types that this plugin implements.
     * Only one dialog type is supported.
     */
    QStringList deviceDialogTypes() const;
    
    /* Returns the possible plugin flags, there are none */
    PluginFlags pluginFlags() const;
    
    /* Not supported */
    int error() const;

private:
    Q_DISABLE_COPY(CellularPromptDialogPlugin)

};

#endif // ___CELLULARPROMPTDIALOG_H__
