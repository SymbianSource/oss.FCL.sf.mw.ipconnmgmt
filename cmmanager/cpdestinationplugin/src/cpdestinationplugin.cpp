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
*   Control Panel plugin for showing destinations and access points
*   settings.
*/

// System includes
#include <QApplication>
#include <QLocale>
#include <QTranslator>

// User includes
#include "cpdestinationplugin.h"
#include "cpdestinationgroup.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdestinationpluginTraces.h"
#endif
/*!
    \class  CpDestinationPlugin
    \brief  This class is a Control Panel plugin for accessing
            destination and access point settings.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!         
    Constructor.
 */
CpDestinationPlugin::CpDestinationPlugin()
{
    OstTraceFunctionEntry0(CPDESTINATIONPLUGIN_CPDESTINATIONPLUGIN_ENTRY);
    // Install localization
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    mTranslator = new QTranslator(this);
    mTranslator->load(path + "cpdestinationplugin_" + lang);
    qApp->installTranslator(mTranslator);
    OstTraceFunctionExit0(CPDESTINATIONPLUGIN_CPDESTINATIONPLUGIN_EXIT);
}

/*!
    Destructor.
 */
CpDestinationPlugin::~CpDestinationPlugin()
{
    OstTraceFunctionEntry0(DUP1_CPDESTINATIONPLUGIN_CPDESTINATIONPLUGIN_ENTRY);
    OstTraceFunctionExit0(DUP1_CPDESTINATIONPLUGIN_CPDESTINATIONPLUGIN_EXIT);
}

/*!
    Inherited member from CpPluginInterface. 
    
    @param[in] itemDataHelper Helper for connecting signals and slots.
    
    \return Returns only one data item in QList. The item contains destination group item.
 */
QList<CpSettingFormItemData*> CpDestinationPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
    OstTraceFunctionEntry0(CPDESTINATIONPLUGIN_CREATESETTINGFORMITEMDATA_ENTRY);
    QList<CpSettingFormItemData*> settingFormData;
    settingFormData.append(new CpDestinationGroup(itemDataHelper));
    
    OstTraceFunctionExit0(CPDESTINATIONPLUGIN_CREATESETTINGFORMITEMDATA_EXIT);
    return settingFormData;
}

Q_EXPORT_PLUGIN2(cpdestinationplugin, CpDestinationPlugin);
