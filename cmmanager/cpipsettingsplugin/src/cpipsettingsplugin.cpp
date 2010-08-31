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
* Control Panel "Network Settings" plugin implementation.
*
*/

// System includes
#include <HbTranslator>
#include <cpsettingformentryitemdataimpl.h>

// User includes
#include "cpipsettingsplugin.h"
#include "cpipsettingsview.h"

/*!
    \class CpIpSettingsPlugin
    \brief Class implements the "Network Settings" entry item for
    Control Panel.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpIpSettingsPlugin::CpIpSettingsPlugin() :
    mTranslator(new HbTranslator("cpipsettingsplugin"))
{
}

/*!
    Destructor.
*/
CpIpSettingsPlugin::~CpIpSettingsPlugin()
{
}

/*!
    Creates and returns the entry item.
*/
QList<CpSettingFormItemData*> CpIpSettingsPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    QList<CpSettingFormItemData*> settingFormData;
    settingFormData.append(new CpSettingFormEntryItemDataImpl<CpIpSettingsView>(
        itemDataHelper,
        hbTrId("txt_occ_dblist_network_settings"),
        "",
        HbIcon("qtg_large_network_settings")));
    return settingFormData;
}

Q_EXPORT_PLUGIN2(cpipsettingsplugin, CpIpSettingsPlugin);
