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
* Control Panel WLAN AP settings view implementation.
*
*/

// System includes
#include <QString>
#include <QVariant>
#include <QDir>
#include <QPluginLoader>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormViewItem>
#include <HbLineEdit>
#include <HbEditorInterface>
#include <HbUrlFilter>
#include <HbPopup>
#include <HbMessageBox>
#include <HbAction>
#include <HbStringUtil>
#include <wlanmgmtcommon.h>
#include <cpitemdatahelper.h>
#include <cpsettingformitemdata.h>
#include <cppluginutility.h>
#include <cmconnectionmethod_shim.h>
#include <cpwlansecurityplugininterface.h>

// User includes
#include "cpwlanapplugin.h"
#include "cpwlanapview.h"
#include "cpwlanapadvancedview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlanapviewTraces.h"
#endif

/*!
    \class CpWlanApView
    \brief This class implements the WLAN AP Control Panel settings view.
*/

// External function prototypes

// Local constants

static const QString wlanSecurityPluginsDir =
    "\\resource\\qt\\plugins\\controlpanel\\wlansecurity";

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpWlanApView::CpWlanApView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mItemDataHelper(0),
        mApSettingsGroupItem(0),
        mConnectionNameItem(0),
        mWlanNetworkNameItem(0),
        mNetworkStatusItem(0),
        mNetworkModeItem(0),
        mAdHocChannelItem(0),
        mSecurityModeItem(0),
        mHomepageItem(0),
        mSecuritySettingsGroupItem(0),
        mAdvancedSettingsAction(0),
        mCmConnectionMethod(cmConnectionMethod),
        mMessageBox(0)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_CPWLANAPVIEW_ENTRY);
    
    // Add "Advanced settings" menu item
    HbMenu *menu = this->menu();
    mAdvancedSettingsAction = menu->addAction(
        hbTrId("txt_occ_opt_advanced_settings"));
    bool status = connect(
        menu,
        SIGNAL(triggered(HbAction*)),
        this,
        SLOT(menuActionTriggered(HbAction*)));
    Q_ASSERT(status);

    // Construct WLAN AP settings UI
    mForm = new HbDataForm();
    this->setWidget(mForm);
    CpPluginUtility::addCpItemPrototype(mForm);
    mModel = new HbDataFormModel(mForm);
    
    // The parameter given as 0 is a HbDataForm pointer, not parent
    mItemDataHelper = new CpItemDataHelper(0);
    mItemDataHelper->setParent(this);

    // Add access point settings group
    createAccessPointSettingsGroup();
        
    mItemDataHelper->bindToForm(mForm);
    mForm->setModel(mModel);

    status = connect(
        mForm,
        SIGNAL(itemShown(const QModelIndex)),
        this,
        SLOT(setEditorPreferences(const QModelIndex)));
    Q_ASSERT(status);

    // Expand access point settings group
    mForm->setExpanded(mModel->indexFromItem(mApSettingsGroupItem), TRUE);
        
    // Add security settings group if necessary
    updateSecurityGroup(
        mSecurityModeItem->contentWidgetData("currentIndex").toInt());
    
    OstTraceFunctionExit0(CPWLANAPVIEW_CPWLANAPVIEW_EXIT);
}

/*!
    Destructor.
*/
CpWlanApView::~CpWlanApView()
{
    OstTraceFunctionEntry0(DUP1_CPWLANAPVIEW_CPWLANAPVIEW_ENTRY);
    
    OstTraceFunctionExit0(DUP1_CPWLANAPVIEW_CPWLANAPVIEW_EXIT);
}

/*!
    Creates the WLAN "Access point settings" group.
*/
void CpWlanApView::createAccessPointSettingsGroup()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_CREATEACCESSPOINTSETTINGSGROUP_ENTRY);
    
    // Access point settings group
    mApSettingsGroupItem = new HbDataFormModelItem(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_occ_subhead_access_point_settings"));
    mModel->appendDataFormItem(mApSettingsGroupItem);
    
    // Connection name
    mConnectionNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_connection_name"));
    // Connect signal and add item to group
    mForm->addConnection(
        mConnectionNameItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(connectionNameChanged()));
    mConnectionNameItem->setContentWidgetData("objectName", "connectionNameEdit");
    mApSettingsGroupItem->appendChild(mConnectionNameItem); 
    
    // WLAN network name
    mWlanNetworkNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_wlan_network_name"));
    // Connect signal and add item to group
    mForm->addConnection(
        mWlanNetworkNameItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(wlanNetworkNameChanged()));
    mWlanNetworkNameItem->setContentWidgetData("objectName", "wlanNetworkNameEdit");
    mApSettingsGroupItem->appendChild(mWlanNetworkNameItem);
    
    // Network status
    mNetworkStatusItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_network_status"));
    // Add items to combobox
    QStringList networkStatusItems;
    networkStatusItems
        << hbTrId("txt_occ_setlabel_network_status_val_public")
        << hbTrId("txt_occ_setlabel_network_status_val_hidden");
    mNetworkStatusItem->setContentWidgetData("items", networkStatusItems);
    // Construct map to link item indexes to setting values
    mNetworkStatusMap.insert(0, false); // public
    mNetworkStatusMap.insert(1, true); // hidden
    // Connect signal and add item to group
    mForm->addConnection(
        mNetworkStatusItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(networkStatusChanged(int)));
    mNetworkStatusItem->setContentWidgetData("objectName", "networkStatusCB");
    mApSettingsGroupItem->appendChild(mNetworkStatusItem);
    
    // Network mode
    mNetworkModeItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_wlan_network_mode"));
    // Add items to combobox
    QStringList networkModeItems;
    networkModeItems
        << hbTrId("txt_occ_setlabel_wlan_network_mode_val_infrastruct")
        << hbTrId("txt_occ_setlabel_wlan_network_mode_val_adhoc");
    mNetworkModeItem->setContentWidgetData("items", networkModeItems);
    // Construct map to link item indexes to setting values
    mNetworkModeMap.insert(0, CMManagerShim::Infra); // infrastructure
    mNetworkModeMap.insert(1, CMManagerShim::Adhoc); // adhoc
    // Connect signal and add item to group
    mForm->addConnection(
        mNetworkModeItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(networkModeChanged(int)));
    mNetworkModeItem->setContentWidgetData("objectName", "networkModeCB");
    mApSettingsGroupItem->appendChild(mNetworkModeItem);
    
    // Ad-hoc channel
    // Added dynamically according to set network mode
    
    // Security mode
    mSecurityModeItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_wlan_security_mode"));
    // Security modes added dynamically according to set network mode

    // Connect signal and add item to group
    mForm->addConnection(
        mSecurityModeItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(securityModeChanged(int)));
    mSecurityModeItem->setContentWidgetData("objectName", "securityModeCB");
    mApSettingsGroupItem->appendChild(mSecurityModeItem);
    
    // Homepage
    mHomepageItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_homepage"));
    // Connect signal and add item to group
    mForm->addConnection(
        mHomepageItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(homepageChanged()));
    mHomepageItem->setContentWidgetData("objectName", "homepageEdit");
    mApSettingsGroupItem->appendChild(mHomepageItem);
    
    // Read settings from CommsDat and update widgets
    updateAccessPointSettingsGroup();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_CREATEACCESSPOINTSETTINGSGROUP_EXIT);
}

/*!
    Reads attribute values and updates "Access point settings" group settings.
*/
void CpWlanApView::updateAccessPointSettingsGroup()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_UPDATEACCESSPOINTSETTINGSGROUP_ENTRY);
    
    // Get attributes from CommsDat and set values to UI widgets
    
    // Connection name
    QString connectionName = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmName);
    mConnectionNameItem->setContentWidgetData("text", connectionName);
    
    // WLAN network name
    QString networkName = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::WlanSSID);
    mWlanNetworkNameItem->setContentWidgetData("text", networkName);
    
    // Network status
    bool scanSsid = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::WlanScanSSID);
    mNetworkStatusItem->setContentWidgetData(
        "currentIndex",
        mNetworkStatusMap.key(scanSsid));
    
    // Network mode
    CMManagerShim::WlanConnMode networkMode =
		static_cast<CMManagerShim::WlanConnMode>
      	(mCmConnectionMethod->getIntAttribute(
            CMManagerShim::WlanConnectionMode));
    mNetworkModeItem->setContentWidgetData(
        "currentIndex",
        mNetworkModeMap.key(networkMode));
    
    // Ad-hoc channel
    updateAdHocChannelItem(networkMode);
    
    // Security mode
    updateSecurityModeItem(networkMode);
    
    // Homepage
    QString homepage = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmStartPage);
    mHomepageItem->setContentWidgetData("text", homepage);
    
    OstTraceFunctionExit0(CPWLANAPVIEW_UPDATEACCESSPOINTSETTINGSGROUP_EXIT);
}

/*!
    Loads all WLAN security plugins.
*/
void CpWlanApView::loadSecurityPlugins(
    CMManagerShim::WlanConnMode networkMode)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_LOADSECURITYPLUGINS_ENTRY);
    
    // Load security plugins
    QList<CpWlanSecurityPluginInterface *> plugins;
    QDir pluginsDir(wlanSecurityPluginsDir);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        CpWlanSecurityPluginInterface *plugin = 
            qobject_cast<CpWlanSecurityPluginInterface *>(loader.instance());
        if (plugin) {
            // Sort items based on the orderNumber()
            QList<CpWlanSecurityPluginInterface *>::iterator i;
            for (i = plugins.begin(); i != plugins.end(); ++i) {
                if ((*i)->orderNumber() > plugin->orderNumber()) {
                    plugins.insert(i, plugin);
                    plugin = NULL;
                    break;
                }
            }
            if (plugin) {
                plugins.append(plugin);
            }
        }
    }
    
    // Add security plugins to map. If network mode is ad-hoc, only WEP
    // is allowed.
    int i;
    i = mSecurityModeMap.size();
    foreach (CpWlanSecurityPluginInterface *plugin, plugins) {
        if (networkMode != CMManagerShim::Adhoc
            || plugin->securityMode() == CMManagerShim::WlanSecModeWep) {
            mSecurityModeMap.insert(i, plugin);
            i++;
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_LOADSECURITYPLUGINS_EXIT);
}

/*!
    Updates the "Security settings" group.
*/
void CpWlanApView::updateSecurityGroup(int index)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_UPDATESECURITYGROUP_ENTRY);
    
    // Remove old security settings group
    if (mSecuritySettingsGroupItem) {
        mModel->removeItem(mSecuritySettingsGroupItem);
        mSecuritySettingsGroupItem = NULL;
    }

    // Add new security settings group
    if (index > 0) {
        // Get correct security plugin
        CpWlanSecurityPluginInterface *plugin = mSecurityModeMap.value(index);
        if (plugin) {
            // Ask plugin to create the security group
            plugin->setReference(
                mCmConnectionMethod,
                mCmConnectionMethod->getIntAttribute(CMManagerShim::CmId));
            mSecuritySettingsGroupItem = plugin->uiInstance(*mItemDataHelper);
            if (mSecuritySettingsGroupItem) {
                // And add it to dataform
                mModel->appendDataFormItem(mSecuritySettingsGroupItem);
            }
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_UPDATESECURITYGROUP_EXIT);
}

/*!
    Shows message box with "OK" button using given text.
*/
void CpWlanApView::showMessageBox(
    HbMessageBox::MessageBoxType type,
    const QString &text)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_SHOWMESSAGEBOX_ENTRY);
    
    // Create a message box
    mMessageBox = QSharedPointer<HbMessageBox>(new HbMessageBox(type));
    mMessageBox->setText(text);
    mMessageBox->setModal(true);
    mMessageBox->setTimeout(HbPopup::NoTimeout);
    mMessageBox->open();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_SHOWMESSAGEBOX_EXIT);
}

/*!
    Tries to update connection method changes to CommsDat.
    Returns "true" if success, "false" if some error happened. 
*/
bool CpWlanApView::tryUpdate()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_TRYUPDATE_ENTRY);
    
    // Try update
    try {
        mCmConnectionMethod->update();
    }
    catch (const std::exception&) {
        // Handle error
        handleUpdateError();
        
        OstTraceFunctionExit0(CPWLANAPVIEW_TRYUPDATE_EXIT);
        return false;
    }

    OstTraceFunctionExit0(DUP1_CPWLANAPVIEW_TRYUPDATE_EXIT);
    return true;
}

/*!
    Handles failed CommsDat update.
 */
void CpWlanApView::handleUpdateError()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_HANDLEUPDATEERROR_ENTRY);
    
    // Show error note to user
    showMessageBox(
        HbMessageBox::MessageTypeWarning,
        hbTrId("txt_occ_info_unable_to_save_setting"));
    // Reload settings from CommsDat and update UI
    try {
        mCmConnectionMethod->refresh();
    }
    catch (const std::exception&) {
        // Ignore error from refresh. Most likely this will not happen, but
        // if it does, there isn't very much we can do.
        OstTrace0(
            TRACE_ERROR,
            CPWLANAPPLUGIN_HANDLEUPDATEERROR,
            "Refresh failed");
    };
    updateAccessPointSettingsGroup();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_HANDLEUPDATEERROR_EXIT);
}

/*!
    Updates Ad-hoc channel item. Setting item is shown if network mode is
    ad-hoc, otherwise it is not shown.
 */
void CpWlanApView::updateAdHocChannelItem(
    CMManagerShim::WlanConnMode networkMode)
{
    if (networkMode == CMManagerShim::Infra) {
        // Infrastructure
        if (mAdHocChannelItem) {
            mApSettingsGroupItem->removeChild(
                mApSettingsGroupItem->indexOf(mAdHocChannelItem));
            mAdHocChannelItem = NULL;
        }
    } else {
        // Ad-hoc
        if (!mAdHocChannelItem) {
            // Ad-hoc channel item not shown, add it
            mAdHocChannelItem = new CpSettingFormItemData(
                HbDataFormModelItem::ComboBoxItem,
                hbTrId("txt_occ_setlabel_adhoc_channel"));
            // Add items to combobox
            QStringList adHocChannelItems;
            adHocChannelItems
                << hbTrId("txt_occ_setlabel_adhoc_channel_val_automatic");
            for (int i = WlanAdHocChannelMinValue;
                i <= WlanAdHocChannelMaxValue;
                i++) {
                QString channel;
                channel.setNum(i);
                adHocChannelItems << HbStringUtil::convertDigits(channel);
            }
            mAdHocChannelItem->setContentWidgetData(
                "items",
                adHocChannelItems);
            // Connect signal and add item to group
            mForm->addConnection(
                mAdHocChannelItem,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(adHocChannelChanged(int)));
            // Add item after the network mode item
            mApSettingsGroupItem->insertChild(
                mApSettingsGroupItem->indexOf(mNetworkModeItem) + 1,
                mAdHocChannelItem);
        }
        // Update ad-hoc item
        int adHocChannelId = mCmConnectionMethod->getIntAttribute(
            CMManagerShim::WlanChannelID);
        mAdHocChannelItem->setContentWidgetData(
            "currentIndex",
            adHocChannelId);
    }
}

/*!
    Updates security mode item.
 */
void CpWlanApView::updateSecurityModeItem(
    CMManagerShim::WlanConnMode networkMode)
{
    // Load WLAN security plugins and construct map
    mSecurityModeMap.clear();
    mSecurityModeMap.insert(0, NULL); // open mode
    loadSecurityPlugins(networkMode);
    
    // Add items to combobox
    QStringList securityModeItems;
    foreach (CpWlanSecurityPluginInterface *plugin, mSecurityModeMap) {
        if (plugin) {
            // Add security mode from plugin
            securityModeItems
                << hbTrId(plugin->securityModeTextId().toLatin1());
        } else {
            // Add open mode
            securityModeItems
                << hbTrId("txt_occ_setlabel_wlan_security_mode_val_open");
        }
    }
    // Remove connection before setting new content, because combobox
    // will emit currentIndexChanged signal and we don't want that.
    mForm->removeConnection(
        mSecurityModeItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(securityModeChanged(int)));
    mSecurityModeItem->setContentWidgetData("items", securityModeItems);
    mForm->addConnection(
        mSecurityModeItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(securityModeChanged(int)));
    
    uint securityMode = mCmConnectionMethod->getIntAttribute(
        CMManagerShim::WlanSecurityMode);
    // Iterate through the map to find correct security plugin and set
    // mode index
    int securityModeIndex = 0;
    QMapIterator<int, CpWlanSecurityPluginInterface *> i(mSecurityModeMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()
            && i.value()->securityMode() == securityMode) {
            securityModeIndex = i.key();
        }
    }
    mSecurityModeItem->setContentWidgetData("currentIndex",
        securityModeIndex);
    
    if (securityMode != CMManagerShim::WlanSecModeOpen
        && securityModeIndex == 0) {
        // Security plugin implementing the selected security mode not found,
        // set security mode to open also in CommsDat  
        mCmConnectionMethod->setIntAttribute(
            CMManagerShim::WlanSecurityMode,
            CMManagerShim::WlanSecModeOpen);
        // Try update
        try {
            mCmConnectionMethod->update();
        }
        catch (const std::exception&) {
            // Error in update, but can't show error notes at this point
        }
    }
}

/*!
    Stores connection name.
*/
void CpWlanApView::connectionNameChanged()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_CONNECTIONNAMECHANGED_ENTRY);
    
    QString connectionName =
        mConnectionNameItem->contentWidgetData("text").toString();
    if (!connectionName.isEmpty()) {
        // Update to CommsDat
        mCmConnectionMethod->setStringAttribute(
            CMManagerShim::CmName,
            connectionName);
        if (tryUpdate()) {
            // Update successful
            // Read name because in case the name already exists it will
            // be made unique by CMManager
            connectionName = mCmConnectionMethod->getStringAttribute(
                CMManagerShim::CmName);
            mConnectionNameItem->setContentWidgetData("text", connectionName);
        }
    } else {
        // Inform user of invalid name
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_name"));

        // Empty name not allowed, revert back to old value
        connectionName = mCmConnectionMethod->getStringAttribute(
            CMManagerShim::CmName);
        mConnectionNameItem->setContentWidgetData("text", connectionName);
    }

    OstTraceFunctionExit0(CPWLANAPVIEW_CONNECTIONNAMECHANGED_EXIT);
}

/*!
    Stores WLAN network name.
*/
void CpWlanApView::wlanNetworkNameChanged()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_WLANNETWORKNAMECHANGED_ENTRY);
    
    // Update to CommsDat
    QString wlanNetworkName =
        mWlanNetworkNameItem->contentWidgetData("text").toString();
    if (!wlanNetworkName.isEmpty()) {
        mCmConnectionMethod->setStringAttribute(
            CMManagerShim::WlanSSID,
            wlanNetworkName);
        (void)tryUpdate();
    } else {
        // Inform user of invalid name
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_name"));

        // Empty name not allowed, revert back to old value
        wlanNetworkName = mCmConnectionMethod->getStringAttribute(
            CMManagerShim::WlanSSID);
        mWlanNetworkNameItem->setContentWidgetData("text", wlanNetworkName);
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_WLANNETWORKNAMECHANGED_EXIT);
}

/*!
    Stores WLAN network status.
*/
void CpWlanApView::networkStatusChanged(int index)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_NETWORKSTATUSCHANGED_ENTRY);
    
    // Update to CommsDat
    mCmConnectionMethod->setBoolAttribute(
        CMManagerShim::WlanScanSSID,
        mNetworkStatusMap.value(index));
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_NETWORKSTATUSCHANGED_EXIT);
}

/*!
    Stores WLAN network mode.
*/
void CpWlanApView::networkModeChanged(int index)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_NETWORKMODECHANGED_ENTRY);
    
    // Update to CommsDat
    mCmConnectionMethod->setIntAttribute(
        CMManagerShim::WlanConnectionMode,
        mNetworkModeMap.value(index));
    if (tryUpdate()) {
        // If mode is ad-hoc, ad-hoc channel item must be shown
        CMManagerShim::WlanConnMode networkMode =
            static_cast<CMManagerShim::WlanConnMode>
                (mNetworkModeMap.value(index));
        updateAdHocChannelItem(networkMode);
		// Also security mode list may need updating
        updateSecurityModeItem(networkMode);
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_NETWORKMODECHANGED_EXIT);
}

/*!
    Stores ad-hoc channel ID.
*/
void CpWlanApView::adHocChannelChanged(int index)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_ADHOCCHANNELCHANGED_ENTRY);
    
    // Update to CommsDat
    mCmConnectionMethod->setIntAttribute(
        CMManagerShim::WlanChannelID,
        index);
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_ADHOCCHANNELCHANGED_EXIT);
}

/*!
    Stores WLAN security mode.
*/
void CpWlanApView::securityModeChanged(int index)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_SECURITYMODECHANGED_ENTRY);
    
    // Get security plugin
    CpWlanSecurityPluginInterface *plugin = mSecurityModeMap.value(index);
    // Update to CommsDat
    if (plugin) {
        mCmConnectionMethod->setIntAttribute(
            CMManagerShim::WlanSecurityMode,
            plugin->securityMode());
    } else {
        mCmConnectionMethod->setIntAttribute(
            CMManagerShim::WlanSecurityMode,
            CMManagerShim::WlanSecModeOpen);
    }
    (void)tryUpdate();
    
    // Update UI
    updateSecurityGroup(
        mSecurityModeItem->contentWidgetData("currentIndex").toInt());
    
    OstTraceFunctionExit0(CPWLANAPVIEW_SECURITYMODECHANGED_EXIT);
}

/*!
    Stores homepage.
*/
void CpWlanApView::homepageChanged()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_HOMEPAGECHANGED_ENTRY);
    
    // Update to CommsDat
    QString homepage = mHomepageItem->contentWidgetData("text").toString();
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::CmStartPage,
        homepage);
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPWLANAPVIEW_HOMEPAGECHANGED_EXIT);
}

/*!
    Handles view menu actions.
*/
void CpWlanApView::menuActionTriggered(HbAction *action)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_MENUACTIONTRIGGERED_ENTRY);
    
    if (action == mAdvancedSettingsAction) {
        HbMainWindow *mainWindow = this->mainWindow();

        // Create the advanced settings view
        HbView *newView = new CpWlanApAdvancedView(mCmConnectionMethod);
        // Connect signal to return back to the previous view
        bool status = QObject::connect(
            newView,
            SIGNAL(aboutToClose()),
            this,
            SLOT(restoreCurrentView()));
        Q_ASSERT(status);
        
        mainWindow->addView(newView);
        mainWindow->setCurrentView(newView);
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_MENUACTIONTRIGGERED_EXIT);
}

/*!
    Removes current view from main window and sets this view as the
    current view. Used when "back" button is pressed in "Advanced settings"
    view.
*/
void CpWlanApView::restoreCurrentView()
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_RESTORECURRENTVIEW_ENTRY);
    
    HbMainWindow *mainWindow = this->mainWindow();
    
    // Remove the previous view and delete it
    HbView *prevView = mainWindow->currentView();
    mainWindow->removeView(prevView);
    prevView->deleteLater();
    
    // Set this view on top
    mainWindow->setCurrentView(this);
    
    OstTraceFunctionExit0(CPWLANAPVIEW_RESTORECURRENTVIEW_EXIT);
}

/*!
    Sets editor preferences for all HbLineEdit items.
*/
void CpWlanApView::setEditorPreferences(const QModelIndex modelIndex)
{
    OstTraceFunctionEntry0(CPWLANAPVIEW_SETEDITORPREFERENCES_ENTRY);
    
    HbDataFormViewItem *viewItem = qobject_cast<HbDataFormViewItem *>
        (mForm->itemByIndex(modelIndex));
    HbDataFormModelItem *modelItem = mModel->itemFromIndex(modelIndex);
    
    if (modelItem == mConnectionNameItem
        || modelItem == mWlanNetworkNameItem
        || modelItem == mHomepageItem ) {
        // HbLineEdit items, get editor and editor interface
        HbLineEdit *edit = qobject_cast<HbLineEdit *>
            (viewItem->dataItemContentWidget());
        HbEditorInterface editInterface(edit);
        
        if (modelItem == mConnectionNameItem) {
            // Setup editor for connection name
            editInterface.setInputConstraints(HbEditorConstraintLatinAlphabetOnly);
            edit->setInputMethodHints(Qt::ImhNoPredictiveText); 
            edit->setMaxLength(CMManagerShim::CmNameLength);
        } else if (modelItem == mWlanNetworkNameItem) {
            // Setup editor for WLAN SSID
            editInterface.setMode(HbInputModeNone);
            editInterface.setInputConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setEditorClass(HbInputEditorClassNetworkName); 
            editInterface.setDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::WlanSSIDLength);
        } else { /* mHomepageItem */
            // Setup editor for URL
            editInterface.setMode(HbInputModeNone);
            editInterface.setInputConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setFilter(HbUrlFilter::instance());
            editInterface.setEditorClass(HbInputEditorClassUrl);
            editInterface.setDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::CmStartPageLength);
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPVIEW_SETEDITORPREFERENCES_EXIT);
}
