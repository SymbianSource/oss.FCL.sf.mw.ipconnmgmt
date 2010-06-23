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
* Control Panel "Network Settings" view implementation.
*
*/

// System includes
#include <QtCore>
#include <HbDataForm>
#include <HbDataFormModel>
#include <cpplugininterface.h>
#include <cppluginutility.h>
#include <cpsettingformitemdata.h>
#include <cpitemdatahelper.h>

// User includes
#include "cpipsettingsview.h"

/*!
    \class CpIpSettingsView
    \brief Class implements the "Network Settings" Control Panel view.
*/

// External function prototypes

// Local constants
static const QString ipSettingsPlugins[] = {
    "cpwlansettingsplugin.qtplugin",
    "cpdestinationplugin.qtplugin",
    "videosettingsplugin.qtplugin",
    "cpvpnmanagementplugin.qtplugin",
    "" // empty item terminates the list
};

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpIpSettingsView::CpIpSettingsView(QGraphicsItem *parent) :
    CpBaseSettingView(0, parent)
{
    HbDataForm *form = new HbDataForm();
    this->setWidget(form);
    CpPluginUtility::addCpItemPrototype(form);
    HbDataFormModel *model = new HbDataFormModel(form);

    // The parameter given as 0 is a HbDataForm pointer, not parent
    mItemDataHelper = new CpItemDataHelper(0);
    mItemDataHelper->setParent(this);
    
    HbDataFormModelItem *modelItem;
    CpPluginInterface *plugin = NULL;
    QDir pluginsDir("\\resource\\qt\\plugins\\controlpanel");
    QPluginLoader pluginLoader;
        
    // Load listed child plugins
    for (int i = 0; !ipSettingsPlugins[i].isEmpty(); i++) {
        pluginLoader.setFileName(pluginsDir.absoluteFilePath(ipSettingsPlugins[i]));
        plugin = qobject_cast<CpPluginInterface *>(pluginLoader.instance());
        if (plugin) {
            QList<CpSettingFormItemData*> formDataItemList =
                plugin->createSettingFormItemData(*mItemDataHelper);
            for (int j = 0; j < formDataItemList.count(); j++) {
                modelItem = formDataItemList.at(j);
                if (modelItem) {
                    model->appendDataFormItem(modelItem);
                }
            }
        }
    }

    mItemDataHelper->bindToForm(form);
    form->setModel(model);
}

/*!
    Destructor.
*/
CpIpSettingsView::~CpIpSettingsView()
{
}
