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
* Description: Prompt Dialog plugin implementation
*
*/


#include <hbdevicedialoginterface.h>
#include <QVariantMap>
#include "cellularpromptdialogplugin.h"
#include "cellularpromptdialog.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cellularpromptdialogpluginTraces.h"
#endif


/* Initializes the resource file */
inline void initMyResource() { Q_INIT_RESOURCE(promptdialog); }
/* removes the resource file */
inline void cleanupMyResource() { Q_CLEANUP_RESOURCE(promptdialog); }
 

Q_EXPORT_PLUGIN2(cellularpromptdialogplugin, CellularPromptDialogPlugin)


// This plugin implements one device dialog type
static const struct {
    const char *mTypeString;
} dialogInfos[] = {
    {"com.nokia.ipconnmgmt.promptdialog/1.0"}
};

/**
 * Constructor
 */ 
CellularPromptDialogPlugin::CellularPromptDialogPlugin()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_CELLULARPROMPTDIALOGPLUGIN_ENTRY );
    
    // Initializes the resource file which contains the
    // docml building up the dialog
    initMyResource();
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_CELLULARPROMPTDIALOGPLUGIN_EXIT );
}

/**
 * Destructor
 */ 
CellularPromptDialogPlugin::~CellularPromptDialogPlugin()
{
    OstTraceFunctionEntry0( DUP1_CELLULARPROMPTDIALOGPLUGIN_DCELLULARPROMPTDIALOGPLUGIN_ENTRY );
    
    // Remove the loaded resources
    cleanupMyResource();
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_DCELLULARPROMPTDIALOGPLUGIN_EXIT );
}

/**
 * Create device dialog widget
 */ 
HbDeviceDialogInterface *CellularPromptDialogPlugin::createDeviceDialog(
    const QString &deviceDialogType,
    const QVariantMap &parameters)
{  
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_CREATEDEVICEDIALOG_ENTRY );
    
    Q_UNUSED(deviceDialogType)
    CellularPromptDialog* dialog = new CellularPromptDialog(parameters);
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_CREATEDEVICEDIALOG_EXIT );
    return dialog;
}

/**
 * Check if client is allowed to use device dialog widget
 */
bool CellularPromptDialogPlugin::accessAllowed(const QString &deviceDialogType,
    const QVariantMap &parameters, const QVariantMap &securityInfo) const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_ACCESSALLOWED_ENTRY );
    
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use.
    return true;
}

/**
 * Return information of device dialog the plugin creates
 */ 
bool CellularPromptDialogPlugin::deviceDialogInfo(const QString &deviceDialogType,
    const QVariantMap &parameters, DeviceDialogInfo *info) const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_DEVICEDIALOGINFO_ENTRY );
    
    Q_UNUSED(parameters)
    Q_UNUSED(deviceDialogType)
    
    info->group = GenericDeviceDialogGroup;
    info->flags = NoDeviceDialogFlags;
    info->priority = DefaultPriority;
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_DEVICEDIALOGINFO_EXIT );
    return true;
}

/**
 * Return device dialog types this plugin implements
 */ 
QStringList CellularPromptDialogPlugin::deviceDialogTypes() const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_DEVICEDIALOGTYPES_ENTRY );
    
    QStringList types;
    const int numTypes = sizeof(dialogInfos) / sizeof(dialogInfos[0]);
    for(int i = 0; i < numTypes; i++) {
        types.append(dialogInfos[i].mTypeString);
    }
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_DEVICEDIALOGTYPES_EXIT );
    return types;
}

/**
 * Return plugin flags
 */ 
CellularPromptDialogPlugin::PluginFlags CellularPromptDialogPlugin::pluginFlags() const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_PLUGINFLAGS_ENTRY );   
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_PLUGINFLAGS_EXIT );
    return NoPluginFlags;
}

/**
 * The last error is not stored, not supported
 */ 
int CellularPromptDialogPlugin::error() const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOGPLUGIN_ERROR_ENTRY );
    OstTraceFunctionExit0( CELLULARPROMPTDIALOGPLUGIN_ERROR_EXIT );
    return 0;
}

