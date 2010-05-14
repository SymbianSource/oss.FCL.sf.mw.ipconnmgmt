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
* Control Panel WLAN AP advanced settings view implementation.
*
*/

// System includes
#include <QHostAddress>
#include <HbCheckBox>
#include <HbLineEdit>
#include <HbDataFormViewItem>
#include <HbWidget>
#include <HbEditorInterface>
#include <HbUrlFilter>
#include <HbInputDialog>
#include <HbDataForm>
#include <HbDataFormModel>
#include <cmconnectionmethod_shim.h>
#include <cmmanagerdefines_shim.h>
#include <cpsettingformitemdata.h>

// User includes
#include "cpipv4filter.h"
#include "cpipv6filter.h"
#include "cpwlanapadvancedview.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlanapadvancedviewTraces.h"
#endif

/*!
    \class CpWlanApAdvancedView
    \brief This class implements the WLAN AP Control Panel advanced
           settings view.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpWlanApAdvancedView::CpWlanApAdvancedView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mCmConnectionMethod(cmConnectionMethod),
        mIpv4Group(0),
        mIpv4Automatic(0),
        mIpv4Address(0),
        mIpv4SubnetMask(0),
        mIpv4Gateway(0),
        mIpv4DnsAutomatic(0),
        mIpv4DnsAddress1(0),
        mIpv4DnsAddress2(0),
        mIpv6Group(0),
        mIpv6DnsAutomatic(0),
        mIpv6DnsAddress1(0),
        mIpv6DnsAddress2(0),
        mProxyGroup(0),
        mProxyServer(0),
        mProxyPort(0),
        mMessageBox(0),
        mIpv6Supported(false)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CPWLANAPADVANCEDVIEW_ENTRY);
    
    // Construct WLAN AP advanced settings UI
    mForm = new HbDataForm();
    this->setWidget(mForm);
    mModel = new HbDataFormModel(mForm);
    mForm->setModel(mModel);

    mIpv6Supported = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::CmIPv6Supported);
        
    // Add advanced settings groups
    createAdvancedSettings();
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CPWLANAPADVANCEDVIEW_EXIT);
}

/*!
    Destructor.
*/
CpWlanApAdvancedView::~CpWlanApAdvancedView()
{
    OstTraceFunctionEntry0(DUP1_CPWLANAPADVANCEDVIEW_CPWLANAPADVANCEDVIEW_ENTRY);
    
    OstTraceFunctionExit0(DUP1_CPWLANAPADVANCEDVIEW_CPWLANAPADVANCEDVIEW_EXIT);
}

/*!
    Creates all advanced settings groups.
*/
void CpWlanApAdvancedView::createAdvancedSettings()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CREATEADVANCEDSETTINGS_ENTRY);
    
    // Create settings groups
    createIpv4Group();
    if (mIpv6Supported) {
        createIpv6Group();
    }
    createProxyGroup();
    
    // Read values and update UI
    updateIpv4Group();
    if (mIpv6Supported) {
        updateIpv6Group();
    }
    updateProxyGroup();
    
    // Connect signal to initialize settings item widgets
    bool status = connect(
        mForm,
        SIGNAL(itemShown(const QModelIndex)),
        this,
        SLOT(setEditorPreferences(const QModelIndex)));
    Q_ASSERT(status);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CREATEADVANCEDSETTINGS_EXIT);
}

/*!
    Creates the IPv4 settings group.
*/
void CpWlanApAdvancedView::createIpv4Group()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CREATEIPV4GROUP_ENTRY);
    
    // IPv4 settings group
    mIpv4Group = new CpSettingFormItemData(
        HbDataFormModelItem::GroupItem,
        hbTrId("txt_occ_subhead_ipv4_settings"));
    mModel->appendDataFormItem(mIpv4Group);
    
    // Phone IP address
    // "Automatic" checkbox
    mIpv4Automatic = new CpSettingFormItemData(
        HbDataFormModelItem::CheckBoxItem,
        hbTrId("txt_occ_setlabel_phone_ip_address"));
    mIpv4Automatic->setContentWidgetData(
        "text",
        hbTrId("txt_occ_setlabel_val_automatic"));
    mForm->addConnection(
        mIpv4Automatic,
        SIGNAL(stateChanged(int)),
        this,
        SLOT(changeIpv4AddressMode(int)));
    mIpv4Group->appendChild(mIpv4Automatic);
    
    // Phone IP address
    mIpv4Address = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_phone_ip_address"));
    mForm->addConnection(
        mIpv4Address,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv4Address()));
    mIpv4Group->appendChild(mIpv4Address);
    
    // Subnet mask
    mIpv4SubnetMask = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_subnet_mask"));
    mForm->addConnection(
        mIpv4SubnetMask,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv4SubnetMask()));
    mIpv4Group->appendChild(mIpv4SubnetMask);
    
    // Gateway
    mIpv4Gateway = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_default_gateway"));
    mForm->addConnection(
        mIpv4Gateway,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv4Gateway()));
    mIpv4Group->appendChild(mIpv4Gateway);
    
    // DNS addresses
    // "Automatic" checkbox
    mIpv4DnsAutomatic = new CpSettingFormItemData(
        HbDataFormModelItem::CheckBoxItem,
        hbTrId("txt_occ_setlabel_dns_addresses"));
    mIpv4DnsAutomatic->setContentWidgetData(
        "text",
        hbTrId("txt_occ_setlabel_val_automatic"));
    mForm->addConnection(
        mIpv4DnsAutomatic,
        SIGNAL(stateChanged(int)), 
        this,
        SLOT(changeIpv4DnsMode(int)));
    mIpv4Group->appendChild(mIpv4DnsAutomatic);
    
    // Primary DNS address
    mIpv4DnsAddress1 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_primary_name_server"));
    mForm->addConnection(
        mIpv4DnsAddress1,
        SIGNAL(editingFinished()), 
        this,
        SLOT(changeIpv4PrimaryDnsAddress()));
    mIpv4Group->appendChild(mIpv4DnsAddress1);
    
    // Secondary DNS address
    mIpv4DnsAddress2 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_secondary_name_server"));
    mForm->addConnection(
        mIpv4DnsAddress2,
        SIGNAL(editingFinished()), 
        this,
        SLOT(changeIpv4SecondaryDnsAddress()));
    mIpv4Group->appendChild(mIpv4DnsAddress2);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CREATEIPV4GROUP_EXIT);
}

/*!
    Reads attribute values and updates the IPv4 settings group.
*/
void CpWlanApAdvancedView::updateIpv4Group()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_UPDATEIPV4GROUP_ENTRY);
    
    // Phone IP address
    // "Automatic" checkbox
    bool ipAddressFromServer = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::CmIPAddrFromServer);
    Qt::CheckState state = Qt::Unchecked;
    if (ipAddressFromServer) {
        state = Qt::Checked;
    }
    mIpv4Automatic->setContentWidgetData("checkState", state);

    // Enable/disable user defined IP address fields
    enableIpv4AddressFields(state);
    
    // Phone IP address
    QString string = getIpAddress(CMManagerShim::CmIPAddress);
    mIpv4Address->setContentWidgetData("text", string);
    
    // Subnet mask
    string = getIpAddress(CMManagerShim::CmIPNetmask);
    mIpv4SubnetMask->setContentWidgetData("text", string);
    
    // Gateway
    string = getIpAddress(CMManagerShim::CmIPGateway);
    mIpv4Gateway->setContentWidgetData("text", string);
    
    // DNS addresses
    // "Automatic" checkbox
    state = Qt::Unchecked;
    if (mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::CmIPDNSAddrFromServer)) {
        state = Qt::Checked;
    }
    mIpv4DnsAutomatic->setContentWidgetData("checkState", state);

    // Enable/disable user defined DNS address fields
    enableIpv4DnsFields(state);
    
    // Primary DNS address
    string = getIpAddress(CMManagerShim::CmIPNameServer1);
    mIpv4DnsAddress1->setContentWidgetData("text", string);
    
    // Secondary DNS address
    string = getIpAddress(CMManagerShim::CmIPNameServer2);
    mIpv4DnsAddress2->setContentWidgetData("text", string);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_UPDATEIPV4GROUP_EXIT);
}

/*!
    Creates the IPv6 settings group.
*/
void CpWlanApAdvancedView::createIpv6Group()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CREATEIPV6GROUP_ENTRY);
    
    // IPv6 settings group
    mIpv6Group = new CpSettingFormItemData(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_occ_subhead_ipv6_settings"));
    mModel->appendDataFormItem(mIpv6Group);
    
    // DNS addresses
    // Automatic/well-known/user defined combobox
    mIpv6DnsAutomatic = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_dns_addresses"));
    QStringList dnsModeList;
    // Order of the list must match the Ipv6DnsType enum
    dnsModeList
        << hbTrId("txt_occ_setlabel_val_automatic")
        << hbTrId("txt_occ_setlabel_dns_addresses_val_wellknown")
        << hbTrId("txt_occ_setlabel_dns_addresses_val_user_defined");
    mIpv6DnsAutomatic->setContentWidgetData("items", dnsModeList);
    mForm->addConnection(
        mIpv6DnsAutomatic,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(changeIpv6DnsMode(int)));
    mIpv6Group->appendChild(mIpv6DnsAutomatic);
    
    // Primary DNS address
    mIpv6DnsAddress1 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_primary_name_server"));
    mForm->addConnection(
        mIpv6DnsAddress1,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv6PrimaryDnsAddress()));
    mIpv6Group->appendChild(mIpv6DnsAddress1);
    
    // Secondary DNS address
    mIpv6DnsAddress2 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_secondary_name_server"));
    mForm->addConnection(
        mIpv6DnsAddress2,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv6SecondaryDnsAddress()));  
    mIpv6Group->appendChild(mIpv6DnsAddress2);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CREATEIPV6GROUP_EXIT);
}

/*!
    Reads attribute values and updates the IPv6 settings group.
*/
void CpWlanApAdvancedView::updateIpv6Group()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_UPDATEIPV6GROUP_ENTRY);
    
    // DNS addresses
    // Automatic/well-known/user defined combobox
    uint type = getIpv6DnsType();
    mIpv6DnsAutomatic->setContentWidgetData("currentIndex", type);

    // Enable/disable user defined DNS address fields
    enableIpv6DnsFields(type);
    
    // Primary DNS address
    QString string = getIpAddress(CMManagerShim::CmIP6NameServer1);
    mIpv6DnsAddress1->setContentWidgetData("text", string);
    
    // Secondary DNS address
    string = getIpAddress(CMManagerShim::CmIP6NameServer2);
    mIpv6DnsAddress2->setContentWidgetData("text", string);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_UPDATEIPV6GROUP_EXIT);
}

/*!
    Creates the proxy settings group.
*/
void CpWlanApAdvancedView::createProxyGroup()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CREATEPROXYGROUP_ENTRY);
    
    // Proxy settings group
    mProxyGroup = new CpSettingFormItemData(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_occ_subhead_proxy_settings"));
    mModel->appendDataFormItem(mProxyGroup);
    
    // Proxy server address
    mProxyServer = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_proxy_server_address"));
    mForm->addConnection(
        mProxyServer,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeProxyServer()));
    mProxyGroup->appendChild(mProxyServer);
    
    // Proxy port number
    mProxyPort = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_proxy_port_number"));
    mForm->addConnection(
        mProxyPort,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeProxyPort()));
    mProxyGroup->appendChild(mProxyPort);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CREATEPROXYGROUP_EXIT);
}

/*!
    Reads attribute values and updates the proxy settings group.
*/
void CpWlanApAdvancedView::updateProxyGroup()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_UPDATEPROXYGROUP_ENTRY);
    
    // Proxy server address
    QString serverAddress = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmProxyServerName);
    mProxyServer->setContentWidgetData("text", serverAddress);
    
    // Proxy port number
    uint portNumber = mCmConnectionMethod->getIntAttribute(
        CMManagerShim::CmProxyPortNumber);
    if (portNumber > 0) {
        mProxyPort->setContentWidgetData("text", QVariant(portNumber));
    } else {
        mProxyPort->setContentWidgetData("text", "");
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_UPDATEPROXYGROUP_EXIT);
}

/*!
    Reads an IP address from CommsDat with given attribute. If string is
    empty, a zero address (0.0.0.0 or 0:0:0:0:0:0:0:0) is returned.
*/
QString CpWlanApAdvancedView::getIpAddress(
    CMManagerShim::ConnectionMethodAttribute attribute)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_GETIPADDRESS_ENTRY);
    
    QString ipAddress = mCmConnectionMethod->getStringAttribute(attribute);
    
    if (ipAddress.isEmpty()) {
        // String is empty, return a zero address instead
        switch (attribute) {
        case CMManagerShim::CmIP6NameServer1:
        case CMManagerShim::CmIP6NameServer2:
            // IPv6 address
            ipAddress = CMManagerShim::DynamicIpv6Address;
            break;
        default:
            // IPv4 address
            ipAddress = CMManagerShim::UnspecifiedIpv4Address; 
            break;
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_GETIPADDRESS_EXIT);
    return ipAddress;
}

/*!
    Resolves IPv6 DNS address type.
*/
CpWlanApAdvancedView::Ipv6DnsType CpWlanApAdvancedView::getIpv6DnsType()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_GETIPV6DNSTYPE_ENTRY);
    
    // Read "DNS address from server" flag
    bool dnsAddressFromServer = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::CmIP6DNSAddrFromServer);
    
    // Read IPv6 DNS addresses from CommsDat
    QString primaryAddress = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmIP6NameServer1);
    QString secondaryAddress = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmIP6NameServer2);

    // Resolve IPv6 DNS address type
    Ipv6DnsType type = Ipv6DnsTypeUserDefined;
    if (dnsAddressFromServer) {
        // Automatic
        type = Ipv6DnsTypeAutomatic;
    } else if (primaryAddress.toLower() == CMManagerShim::KnownIpv6NameServer1
        && secondaryAddress.toLower() == CMManagerShim::KnownIpv6NameServer2) {
        // Well-known IPv6 DNS addresses
        type = Ipv6DnsTypeWellKnown;
    }

    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_GETIPV6DNSTYPE_EXIT);
    return type;
}

/*!
    Sets IPv4 address field enabled/disabled.
*/
void CpWlanApAdvancedView::enableIpv4AddressFields(int state)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_ENABLEIPV4ADDRESSFIELDS_ENTRY);
    
    if (state == Qt::Checked) {
        // Automatic, disable text field
        mIpv4Address->setEnabled(false);
        mIpv4SubnetMask->setEnabled(false);
        mIpv4Gateway->setEnabled(false);
    } else {
        // Manual, enable text field
        mIpv4Address->setEnabled(true);
        mIpv4SubnetMask->setEnabled(true);
        mIpv4Gateway->setEnabled(true);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_ENABLEIPV4ADDRESSFIELDS_EXIT);
}

/*!
    Sets IPv4 DNS address fields enabled/disabled.
*/
void CpWlanApAdvancedView::enableIpv4DnsFields(int state)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_ENABLEIPV4DNSFIELDS_ENTRY);
    
    if (state == Qt::Checked) {
        // Automatic, disable text fields
        mIpv4DnsAddress1->setEnabled(false);
        mIpv4DnsAddress2->setEnabled(false);
    } else {
        // Manual, enable text fields
        mIpv4DnsAddress1->setEnabled(true);
        mIpv4DnsAddress2->setEnabled(true);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_ENABLEIPV4DNSFIELDS_EXIT);
}

/*!
    Sets IPv6 DNS address fields enabled/disabled.
*/
void CpWlanApAdvancedView::enableIpv6DnsFields(int selectionIndex)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_ENABLEIPV6DNSFIELDS_ENTRY);
    
    // Check combobox value
    if (selectionIndex == Ipv6DnsTypeUserDefined) {
        // User-defined, enable text fields
        mIpv6DnsAddress1->setEnabled(true);
        mIpv6DnsAddress2->setEnabled(true);
    } else {
        // Automatic or well-known, disable text fields
        mIpv6DnsAddress1->setEnabled(false);
        mIpv6DnsAddress2->setEnabled(false);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_ENABLEIPV6DNSFIELDS_EXIT);
}

/*!
    Validates an IP address and saves it to CommsDat in case it was a valid
    address or an empty string. In case the address is not valid, an error
    note is shown to the user and previous setting is restored to the UI.
*/
bool CpWlanApAdvancedView::validateAndSaveIpAddress(
    HbDataFormModelItem *item,
    CMManagerShim::ConnectionMethodAttribute attribute,
    QAbstractSocket::NetworkLayerProtocol protocol)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_VALIDATEANDSAVEIPADDRESS_ENTRY);
    
    bool success = false;
    
    // Check address
    QString address = item->contentWidgetData("text").toString();
    QHostAddress hostAddress;
    if (hostAddress.setAddress(address)
        && hostAddress.protocol() == protocol) {
        // Proper address, convert to lower case (IPv6)
        address = hostAddress.toString().toLower();
    } else if (address.isEmpty()) {
        // Empty string, replace with 0.0.0.0 or 0:0:0:0:0:0:0:0
        if (protocol == QAbstractSocket::IPv4Protocol) {
            address = CMManagerShim::UnspecifiedIpv4Address;
        } else {
            address = CMManagerShim::DynamicIpv6Address;
        }
    } else {
        // Invalid address
        address.clear();
    }

    if (!address.isEmpty()) {
        // Save to CommsDat
        mCmConnectionMethod->setStringAttribute(attribute, address);
        if (tryUpdate()) {
            success = true;
            item->setContentWidgetData("text", address);
        }
    } else {
        // Inform user of invalid name
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_input"));
        // Restore previous setting
        address = getIpAddress(attribute);
        item->setContentWidgetData("text", address);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_VALIDATEANDSAVEIPADDRESS_EXIT);
    return success;
}

/*!
    Shows message box with "OK" button using given text.
*/
void CpWlanApAdvancedView::showMessageBox(
    HbMessageBox::MessageBoxType type,
    const QString &text)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_SHOWMESSAGEBOX_ENTRY);
    
    // Create a message box
    mMessageBox = QSharedPointer<HbMessageBox>(new HbMessageBox(type));
    mMessageBox->setText(text);
    mMessageBox->setModal(true);
    mMessageBox->setTimeout(HbPopup::NoTimeout);
    mMessageBox->setDismissPolicy(HbPopup::NoDismiss);
    mMessageBox->open();
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_SHOWMESSAGEBOX_EXIT);
}

/*!
    Tries to update connection method changes to CommsDat.
    Returns "true" if success, "false" if some error happened. 
*/
bool CpWlanApAdvancedView::tryUpdate()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_TRYUPDATE_ENTRY);
    
    // Try update
    bool success = true;
    try {
        mCmConnectionMethod->update();
    }
    catch (const std::exception&) {
        // Handle error
        handleUpdateError();
        
        success = false;
    }

    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_TRYUPDATE_EXIT);
    return success;
}

/*!
    Handles failed CommsDat update.
 */
void CpWlanApAdvancedView::handleUpdateError()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_HANDLEUPDATEERROR_ENTRY);
    
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
            CPWLANAPPADVANCEDVIEW_HANDLEUPDATEERROR,
            "Refresh failed");
    };
    updateIpv4Group();
    if (mIpv6Supported) {
        updateIpv6Group();
    }
    updateProxyGroup();
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_HANDLEUPDATEERROR_EXIT);
}

/*!
    Initializes all settings groups.
*/
void CpWlanApAdvancedView::setEditorPreferences(const QModelIndex index)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_SETEDITORPREFERENCES_ENTRY);
    
    HbDataFormViewItem *viewItem = qobject_cast<HbDataFormViewItem *>
        (mForm->itemByIndex(index));
    HbDataFormModelItem *modelItem = mModel->itemFromIndex(index);

    if (modelItem == mIpv4Address
        || modelItem == mIpv4SubnetMask
        || modelItem == mIpv4Gateway
        || modelItem == mIpv4DnsAddress1
        || modelItem == mIpv4DnsAddress2
        || modelItem == mIpv6DnsAddress1
        || modelItem == mIpv6DnsAddress2
        || modelItem == mProxyServer
        || modelItem == mProxyPort) {
        // HbLineEdit items, get editor and editor interface
        HbLineEdit *edit = qobject_cast<HbLineEdit *>
            (viewItem->dataItemContentWidget());
        HbEditorInterface editorInterface(edit);

        if (modelItem == mIpv4Address
            || modelItem == mIpv4SubnetMask
            || modelItem == mIpv4Gateway
            || modelItem == mIpv4DnsAddress1
            || modelItem == mIpv4DnsAddress2) {
            // IPv4 IP address
            editorInterface.setMode(HbInputModeNumeric);
            editorInterface.setFilter(CpIpv4Filter::instance());
            edit->setMaxLength(CMManagerShim::CmIP4AddressLength);
        } else if (modelItem == mIpv6DnsAddress1
            || modelItem == mIpv6DnsAddress2) {
            // IPv6 IP address
            editorInterface.setInputConstraints(
                HbEditorConstraintLatinAlphabetOnly);
            editorInterface.setFilter(CpIpv6Filter::instance());
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::CmIP6NameServerLength);
        } else if (modelItem == mProxyServer) {                
            // Proxy server address
            editorInterface.setMode(HbInputModeNone);
            editorInterface.setInputConstraints(
                HbEditorConstraintLatinAlphabetOnly);
            editorInterface.setFilter(HbUrlFilter::instance());
            editorInterface.setEditorClass(HbInputEditorClassUrl);
            editorInterface.setDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::CmProxyServerNameLength);
        } else { /* mProxyPort */                
            // Proxy port number
            editorInterface.setMode(HbInputModeNumeric);
            edit->setInputMethodHints(Qt::ImhDigitsOnly);
            edit->setMaxLength(CMManagerShim::CmProxyPortNumberLength);
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_SETEDITORPREFERENCES_EXIT);
}

/*!
    Handles change to IPv4 address mode (automatic/user defined).
*/
void CpWlanApAdvancedView::changeIpv4AddressMode(int state)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4ADDRESSMODE_ENTRY);
    
    bool ipv4AddressFromServer;
    if (state == Qt::Checked) {
        ipv4AddressFromServer = true;
    } else {
        ipv4AddressFromServer = false;
    }
    
    if (!ipv4AddressFromServer
        && getIpAddress(CMManagerShim::CmIPAddress) ==
            CMManagerShim::UnspecifiedIpv4Address) {
        // User defined IP address selected, but no valid address is stored.
        // Just enable the UI IP editors, because CMManager will update the
        // "IP address from server" flag by itself when a valid IP address
        // is stored.
        enableIpv4AddressFields(state);
    } else {
        // Save setting to CommsDat
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIPAddrFromServer,
            ipv4AddressFromServer);
        if (tryUpdate()) {
            enableIpv4AddressFields(state);
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4ADDRESSMODE_EXIT);
}

/*!
    Saves IPv4 address to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv4Address()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4ADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv4Address,
        CMManagerShim::CmIPAddress,
        QAbstractSocket::IPv4Protocol)) {
        // In case the IP address is empty or 0.0.0.0, CMManager changes the
        // "IP address from server" flag to true. We need to refresh the UI to
        // make sure we are in sync.
        updateIpv4Group();
    }

    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4ADDRESS_EXIT);
}

/*!
    Saves IPv4 subnet mask to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv4SubnetMask()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4SUBNETMASK_ENTRY);
    
    (void)validateAndSaveIpAddress(
        mIpv4SubnetMask,
        CMManagerShim::CmIPNetmask,
        QAbstractSocket::IPv4Protocol);
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4SUBNETMASK_EXIT);
}

/*!
    Saves IPv4 gateway to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv4Gateway()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4GATEWAY_ENTRY);
    
    (void)validateAndSaveIpAddress(
        mIpv4Gateway,
        CMManagerShim::CmIPGateway,
        QAbstractSocket::IPv4Protocol);

    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4GATEWAY_EXIT);
}

/*!
    Handles change to IPv4 DNS mode (automatic/user defined).
*/
void CpWlanApAdvancedView::changeIpv4DnsMode(int state)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4DNSMODE_ENTRY);
    
    bool dnsAddressFromServer;
    if (state == Qt::Checked) {
        dnsAddressFromServer = true;
    } else {
        dnsAddressFromServer = false;
    }
    // Save setting to CommsDat
    mCmConnectionMethod->setBoolAttribute(
        CMManagerShim::CmIPDNSAddrFromServer,
        dnsAddressFromServer);
    if (tryUpdate()) {
        enableIpv4DnsFields(state);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4DNSMODE_EXIT);
}

/*!
    Saves IPv4 primary DNS address to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv4PrimaryDnsAddress()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4PRIMARYDNSADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv4DnsAddress1,
        CMManagerShim::CmIPNameServer1,
        QAbstractSocket::IPv4Protocol)) {
        // Address saved successfully, update "DNS address from server" flag
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIPDNSAddrFromServer,
            false);
        if (tryUpdate()) {
            // In case primary DNS address is empty or 0.0.0.0, CMManager will
            // check secondary DNS address and in case it is a valid address,
            // move it to the primary DNS address. We need to refresh the
            // UI to make sure we are in sync. If tryUpdate() fails it does
            // the refresh automatically, so only success case is handled
            // here.
            updateIpv4Group();            
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4PRIMARYDNSADDRESS_EXIT);
}

/*!
    Saves IPv4 secondary DNS address to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv4SecondaryDnsAddress()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV4SECONDARYDNSADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv4DnsAddress2,
        CMManagerShim::CmIPNameServer2,
        QAbstractSocket::IPv4Protocol)) {
        // Address saved successfully, update "DNS address from server" flag
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIPDNSAddrFromServer,
            false);
        if (tryUpdate()) {
            // In case primary DNS address is empty or 0.0.0.0, CMManager will
            // write the address there instead of secondary DNS address, so we
            // need to refresh the UI to make sure we are in sync. If
            // tryUpdate() fails it does the refresh automatically, so only
            // success case is handled here.
            updateIpv4Group();
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV4SECONDARYDNSADDRESS_EXIT);
}

/*!
    Handles change to IPv6 DNS mode (automatic/well-known/user defined).
*/
void CpWlanApAdvancedView::changeIpv6DnsMode(int state)
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV6DNSMODE_ENTRY);
    
    // Update "DNS address from server" flag
    if (state == Ipv6DnsTypeAutomatic) {
        // Automatic
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIP6DNSAddrFromServer,
            true);
    } else {
        // User defined or well-known address
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIP6DNSAddrFromServer,
            false);
        
        if (state == Ipv6DnsTypeWellKnown) {
            // Well-known address, set DNS addresses and update UI
            mCmConnectionMethod->setStringAttribute(
                CMManagerShim::CmIP6NameServer1,
                CMManagerShim::KnownIpv6NameServer1);
            mIpv6DnsAddress1->setContentWidgetData(
                "text",
                CMManagerShim::KnownIpv6NameServer1);
            mCmConnectionMethod->setStringAttribute(
                CMManagerShim::CmIP6NameServer2,
                CMManagerShim::KnownIpv6NameServer2);
            mIpv6DnsAddress2->setContentWidgetData(
                "text",
                CMManagerShim::KnownIpv6NameServer2);        
        }
    }

    if (tryUpdate()) {
        enableIpv6DnsFields(state);
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV6DNSMODE_EXIT);
}

/*!
    Saves IPv6 primary DNS address to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv6PrimaryDnsAddress()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV6PRIMARYDNSADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv6DnsAddress1,
        CMManagerShim::CmIP6NameServer1,
        QAbstractSocket::IPv6Protocol)) {
        // Address saved successfully, update "DNS address from server" flag
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIP6DNSAddrFromServer,
            false);
        if (tryUpdate()) {
            // In case primary DNS address is empty or 0:0:0:0:0:0:0:0,
            // CMManager will check secondary DNS address and in case it is a
            // valid address, move it to the primary DNS address. We need to
            // refresh the UI to make sure we are in sync. If tryUpdate()
            // fails it does the refresh automatically, so only success case
            // is handled here.
            updateIpv6Group();            
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV6PRIMARYDNSADDRESS_EXIT);
}

/*!
    Saves IPv6 secondary DNS address to CommsDat.
*/
void CpWlanApAdvancedView::changeIpv6SecondaryDnsAddress()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEIPV6SECONDARYDNSADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv6DnsAddress2,
        CMManagerShim::CmIP6NameServer2,
        QAbstractSocket::IPv6Protocol)) {
        // Address saved successfully, update "DNS address from server" flag
        mCmConnectionMethod->setBoolAttribute(
            CMManagerShim::CmIP6DNSAddrFromServer,
            false);
        if (tryUpdate()) {
            // In case primary DNS address is empty or 0:0:0:0:0:0:0:0,
            // CMManager will write the address there instead of secondary DNS
            // address, so we need to refresh the UI to make sure we are in
            // sync. If tryUpdate() fails it does the refresh automatically,
            // so only success case is handled here.
            updateIpv6Group();
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEIPV6SECONDARYDNSADDRESS_EXIT);
}

/*!
    Saves proxy server address to CommsDat.
*/
void CpWlanApAdvancedView::changeProxyServer()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEPROXYSERVER_ENTRY);
    
    QString proxyServer = mProxyServer->contentWidgetData("text").toString();
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::CmProxyServerName,
        proxyServer);
    if (tryUpdate()) {
        // If proxy server name is empty, CMManager will also clear the
        // proxy port. Therefore UI needs to be updated.
        updateProxyGroup();
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEPROXYSERVER_EXIT);
}

/*!
    Saves proxy port to CommsDat.
*/
void CpWlanApAdvancedView::changeProxyPort()
{
    OstTraceFunctionEntry0(CPWLANAPADVANCEDVIEW_CHANGEPROXYPORT_ENTRY);
    
    uint proxyPort = mProxyPort->contentWidgetData("text").toInt();
    if (proxyPort <= 65535) {
        if (proxyPort == 0) {
            // Don't show zero
            mProxyPort->setContentWidgetData("text", "");
        }
        mCmConnectionMethod->setIntAttribute(
            CMManagerShim::CmProxyPortNumber,
            proxyPort);
        (void)tryUpdate();
    } else {
        // Inform user of invalid value
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_input"));
        // Restore previous setting
        proxyPort = mCmConnectionMethod->getIntAttribute(
            CMManagerShim::CmProxyPortNumber);
        if (proxyPort > 0) {
            mProxyPort->setContentWidgetData("text", QVariant(proxyPort));
        } else {
            mProxyPort->setContentWidgetData("text", "");
        }
    }
    
    OstTraceFunctionExit0(CPWLANAPADVANCEDVIEW_CHANGEPROXYPORT_EXIT);
}
