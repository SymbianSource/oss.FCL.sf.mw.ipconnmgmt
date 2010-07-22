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
* Control Panel packet data AP advanced settings view implementation.  
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
#include <etelpckt.h>

// User includes
#include "cpipv4filter.h"
#include "cpipv6filter.h"
#include "cppacketdataapadvancedview.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cppacketdataapadvancedviewTraces.h"
#endif

/*!
    \class CpPacketDataApAdvancedView
    \brief Implements the advanced settings view for packet data bearer
           access points.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpPacketDataApAdvancedView::CpPacketDataApAdvancedView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mCmConnectionMethod(cmConnectionMethod),
        mIpGroup(0),
        mNetworkType(0),
        mIpv4Automatic(0),
        mIpv4Address(0),
        mIpv4DnsAutomatic(0),
        mIpv4DnsAddress1(0),
        mIpv4DnsAddress2(0),
        mIpv6DnsAutomatic(0),
        mIpv6DnsAddress1(0),
        mIpv6DnsAddress2(0),
        mProxyGroup(0),
        mProxyServer(0),
        mProxyPort(0),
        mMessageBox(0),
        mIpv6Supported(false),
        mCurrentNetworkType(NetworkTypeIpv4)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CPPACKETDATAAPADVANCEDVIEW_ENTRY);
    
    // Construct packet data AP advanced settings UI
    mForm = new HbDataForm();
    this->setWidget(mForm);
    mModel = new HbDataFormModel(mForm);
    mForm->setModel(mModel);

    mIpv6Supported = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::CmIPv6Supported);
        
    // Add advanced settings groups
    createAdvancedSettings();
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CPPACKETDATAAPADVANCEDVIEW_EXIT);
}

/*!
    Destructor.
*/
CpPacketDataApAdvancedView::~CpPacketDataApAdvancedView()
{
    OstTraceFunctionEntry0(DUP1_CPPACKETDATAAPADVANCEDVIEW_CPPACKETDATAAPADVANCEDVIEW_ENTRY);
    
    OstTraceFunctionExit0(DUP1_CPPACKETDATAAPADVANCEDVIEW_CPPACKETDATAAPADVANCEDVIEW_EXIT);
}

/*!
    Creates all advanced settings groups.
*/
void CpPacketDataApAdvancedView::createAdvancedSettings()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CREATEADVANCEDSETTINGS_ENTRY);
    
    // Create settings groups
    createIpGroup();
    createProxyGroup();
    
    // Read values and update UI
    updateIpGroup();
    updateProxyGroup();
    
    // Connect signal to initialize settings item widgets
    bool status = connect(
        mForm,
        SIGNAL(itemShown(const QModelIndex)),
        this,
        SLOT(setEditorPreferences(const QModelIndex)));
    Q_ASSERT(status);
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CREATEADVANCEDSETTINGS_EXIT);
}

/*!
    Creates the IP settings group.
*/
void CpPacketDataApAdvancedView::createIpGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPGROUP_ENTRY);
    
    // IP settings group
    mIpGroup = new CpSettingFormItemData(
        HbDataFormModelItem::GroupItem,
        hbTrId("txt_occ_subhead_ip_settings"));
    mModel->appendDataFormItem(mIpGroup);
    
    if (mIpv6Supported) {
        // IPv6 supported
        // Network type
        mNetworkType = new CpSettingFormItemData(
            HbDataFormModelItem::ComboBoxItem,
            hbTrId("txt_occ_setlabel_network_type"));
        QStringList networkTypeList;
        networkTypeList
            << hbTrId("txt_occ_setlabel_network_type_val_ipv4")
            << hbTrId("txt_occ_setlabel_network_type_val_ipv6");
        mNetworkType->setContentWidgetData("items", networkTypeList);
        mForm->addConnection(
            mNetworkType,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(changeNetworkType(int)));
        mIpGroup->appendChild(mNetworkType);
        
        mCurrentNetworkType = getNetworkType();
        if (mCurrentNetworkType == NetworkTypeIpv4) {
            // IPv4
            createIpv4SettingItems();
        } else {
            // IPv6
            createIpv6SettingItems();
        }
    } else {
        // IPv6 not supported, create IPv4 settings items
        createIpv4SettingItems();
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPGROUP_EXIT);
}

/*!
    Reads attribute values and updates the IP settings group.
*/
void CpPacketDataApAdvancedView::updateIpGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_UPDATEIPGROUP_ENTRY);
    
    // Network type
    if (mNetworkType) {
        // Every time update is called we check if network type (IPv4/IPv6)
        // has changed and update items on UI if necessary.
    
        // Get network type from CommsDat
        NetworkType newNetworkType = getNetworkType();
        if (newNetworkType != mCurrentNetworkType) {
            // Network type has changed, update UI
            if (newNetworkType == NetworkTypeIpv6) {
                // IPv4 -> IPv6
                deleteIpv4SettingItems();
                createIpv6SettingItems();
            } else {
                // IPv6 -> IPv4
                deleteIpv6SettingItems();
                createIpv4SettingItems();
            }
            mCurrentNetworkType = newNetworkType;
        }
        mNetworkType->setContentWidgetData("currentIndex", newNetworkType);
    }
    
    // Phone IP address (IPv4)
    // "Automatic" checkbox
    if (mIpv4Automatic) {
        bool ipAddressFromServer = mCmConnectionMethod->getBoolAttribute(
            CMManagerShim::PacketDataIPAddrFromServer);
        Qt::CheckState state = Qt::Unchecked;
        if (ipAddressFromServer) {
            state = Qt::Checked;
        }
        mIpv4Automatic->setContentWidgetData("checkState", state);

        // Enable/disable user defined IP address fields
        enableIpv4AddressFields(state);
    }

    // Phone IP address (IPv4)
    if (mIpv4Address) {
        QString string = getIpAddress(CMManagerShim::CmIPAddress);
        mIpv4Address->setContentWidgetData("text", string);
    }
    
    // IPv4 DNS addresses
    // "Automatic" checkbox
    if (mIpv4DnsAutomatic) {
        Qt::CheckState state = Qt::Unchecked;
        if (mCmConnectionMethod->getBoolAttribute(
            CMManagerShim::CmIPDNSAddrFromServer)) {
            state = Qt::Checked;
        }
        mIpv4DnsAutomatic->setContentWidgetData("checkState", state);

        // Enable/disable user defined DNS address fields
        enableIpv4DnsFields(state);
    }
    
    // IPv4 primary DNS address
    if (mIpv4DnsAddress1) {
        QString string = getIpAddress(CMManagerShim::CmIPNameServer1);
        mIpv4DnsAddress1->setContentWidgetData("text", string);
    }
    
    // IPv4 secondary DNS address
    if (mIpv4DnsAddress2) {
        QString string = getIpAddress(CMManagerShim::CmIPNameServer2);
        mIpv4DnsAddress2->setContentWidgetData("text", string);
    }
    
    // IPv6 DNS addresses
    // Automatic/well-known/user defined combobox
    if (mIpv6DnsAutomatic) {
        uint type = getIpv6DnsType();
        mIpv6DnsAutomatic->setContentWidgetData("currentIndex", type);

        // Enable/disable user defined DNS address fields
        enableIpv6DnsFields(type);
    }
    
    // IPv6 primary DNS address
    if (mIpv6DnsAddress1) {
        QString string = getIpAddress(CMManagerShim::CmIP6NameServer1);
        mIpv6DnsAddress1->setContentWidgetData("text", string);
    }
    
    // IPv6 secondary DNS address
    if (mIpv6DnsAddress2) {
        QString string = getIpAddress(CMManagerShim::CmIP6NameServer2);
        mIpv6DnsAddress2->setContentWidgetData("text", string);
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_UPDATEIPGROUP_EXIT);
}

/*!
    Creates the IPv4 setting items.
*/
void CpPacketDataApAdvancedView::createIpv4SettingItems()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPV4SETTINGITEMS_ENTRY);

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
    mIpGroup->appendChild(mIpv4Automatic);
    
    // Phone IP address
    mIpv4Address = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_phone_ip_address"));
    mForm->addConnection(
        mIpv4Address,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv4Address()));
    mIpGroup->appendChild(mIpv4Address);
    
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
    mIpGroup->appendChild(mIpv4DnsAutomatic);
    
    // Primary DNS address
    mIpv4DnsAddress1 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_primary_name_server"));
    mForm->addConnection(
        mIpv4DnsAddress1,
        SIGNAL(editingFinished()), 
        this,
        SLOT(changeIpv4PrimaryDnsAddress()));
    mIpGroup->appendChild(mIpv4DnsAddress1);
    
    // Secondary DNS address
    mIpv4DnsAddress2 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_secondary_name_server"));
    mForm->addConnection(
        mIpv4DnsAddress2,
        SIGNAL(editingFinished()), 
        this,
        SLOT(changeIpv4SecondaryDnsAddress()));
    mIpGroup->appendChild(mIpv4DnsAddress2);
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPV4SETTINGITEMS_EXIT);
}

/*!
    Deletes the IPv4 setting items.
*/
void CpPacketDataApAdvancedView::deleteIpv4SettingItems()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_DELETEIPV4SETTINGITEMS_ENTRY);

    // Phone IP address
    // "Automatic" checkbox
    if (mIpv4Automatic) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv4Automatic));
        mIpv4Automatic = NULL;
    }
    
    // Phone IP address
    if (mIpv4Address) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv4Address));
        mIpv4Address = NULL;
    }
    
    // DNS addresses
    // "Automatic" checkbox
    if (mIpv4DnsAutomatic) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv4DnsAutomatic));
        mIpv4DnsAutomatic = NULL;
    }
    
    // Primary DNS address
    if (mIpv4DnsAddress1) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv4DnsAddress1));
        mIpv4DnsAddress1 = NULL;
    }
    
    // Secondary DNS address
    if (mIpv4DnsAddress2) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv4DnsAddress2));
        mIpv4DnsAddress2 = NULL;
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_DELETEIPV4SETTINGITEMS_EXIT);
}

/*!
    Creates the IPv6 setting items.
*/
void CpPacketDataApAdvancedView::createIpv6SettingItems()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPV6SETTINGITEMS_ENTRY);
    
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
    mIpGroup->appendChild(mIpv6DnsAutomatic);
    
    // Primary DNS address
    mIpv6DnsAddress1 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_primary_name_server"));
    mForm->addConnection(
        mIpv6DnsAddress1,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv6PrimaryDnsAddress()));
    mIpGroup->appendChild(mIpv6DnsAddress1);
    
    // Secondary DNS address
    mIpv6DnsAddress2 = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_secondary_name_server"));
    mForm->addConnection(
        mIpv6DnsAddress2,
        SIGNAL(editingFinished()),
        this,
        SLOT(changeIpv6SecondaryDnsAddress()));  
    mIpGroup->appendChild(mIpv6DnsAddress2);
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CREATEIPV6SETTINGITEMS_EXIT);
}

/*!
    Deletes the IPv6 setting items.
*/
void CpPacketDataApAdvancedView::deleteIpv6SettingItems()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_DELETEIPV6SETTINGITEMS_ENTRY);
    
    // DNS addresses
    // Automatic/well-known/user defined combobox
    if (mIpv6DnsAutomatic) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv6DnsAutomatic));
        mIpv6DnsAutomatic = NULL;
    }
    
    // Primary DNS address
    if (mIpv6DnsAddress1) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv6DnsAddress1));
        mIpv6DnsAddress1 = NULL;
    }
    
    // Secondary DNS address
    if (mIpv6DnsAddress2) {
        mIpGroup->removeChild(mIpGroup->indexOf(mIpv6DnsAddress2));
        mIpv6DnsAddress2 = NULL;
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_DELETEIPV6SETTINGITEMS_EXIT);
}

/*!
    Creates the proxy settings group.
*/
void CpPacketDataApAdvancedView::createProxyGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CREATEPROXYGROUP_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CREATEPROXYGROUP_EXIT);
}

/*!
    Reads attribute values and updates the proxy settings group.
*/
void CpPacketDataApAdvancedView::updateProxyGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_UPDATEPROXYGROUP_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_UPDATEPROXYGROUP_EXIT);
}

/*!
    Reads network type (IPv4/IPv6) from CommsDat.
*/
CpPacketDataApAdvancedView::NetworkType CpPacketDataApAdvancedView::getNetworkType()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_GETNETWORKTYPE_ENTRY);

    uint networkType = mCmConnectionMethod->getIntAttribute(
        CMManagerShim::PacketDataPDPType);
    
    if (networkType == RPacketContext::EPdpTypeIPv6) {
        OstTraceFunctionExit0(DUP1_CPPACKETDATAAPADVANCEDVIEW_GETNETWORKTYPE_EXIT);
        return NetworkTypeIpv6;
    }

    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_GETNETWORKTYPE_EXIT);
    return NetworkTypeIpv4;
}

/*!
    Reads an IP address from CommsDat with given attribute. If string is
    empty, a zero address (0.0.0.0 or 0:0:0:0:0:0:0:0) is returned.
*/
QString CpPacketDataApAdvancedView::getIpAddress(
    CMManagerShim::ConnectionMethodAttribute attribute)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_GETIPADDRESS_ENTRY);
    
    QString ipAddress = mCmConnectionMethod->getStringAttribute(attribute);
    
    if (ipAddress.isEmpty()) {
        // String is empty, return a zero address instead
        switch (attribute) {
            case CMManagerShim::CmIPAddress:
            case CMManagerShim::CmIPNameServer1:
            case CMManagerShim::CmIPNameServer2:
                // IPv4 address
                ipAddress = CMManagerShim::UnspecifiedIpv4Address; 
                break;
            
            case CMManagerShim::CmIP6NameServer1:
            case CMManagerShim::CmIP6NameServer2:
                // IPv6 address
                ipAddress = CMManagerShim::DynamicIpv6Address;
                break;
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_GETIPADDRESS_EXIT);
    return ipAddress;
}

/*!
    Resolves IPv6 DNS address type.
*/
CpPacketDataApAdvancedView::Ipv6DnsType CpPacketDataApAdvancedView::getIpv6DnsType()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_GETIPV6DNSTYPE_ENTRY);
    
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

    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_GETIPV6DNSTYPE_EXIT);
    return type;
}

/*!
    Sets IPv4 address field enabled/disabled.
*/
void CpPacketDataApAdvancedView::enableIpv4AddressFields(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV4ADDRESSFIELDS_ENTRY);
    
    if (state == Qt::Checked) {
        // Automatic, disable text field
        if (mIpv4Address) {
            mIpv4Address->setEnabled(false);
        }
    } else {
        // User defined, enable text field
        if (mIpv4Address) {
            mIpv4Address->setEnabled(true);
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV4ADDRESSFIELDS_EXIT);
}

/*!
    Sets IPv4 DNS address fields enabled/disabled.
*/
void CpPacketDataApAdvancedView::enableIpv4DnsFields(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV4DNSFIELDS_ENTRY);
    
    if (state == Qt::Checked) {
        // Automatic, disable text fields
        if (mIpv4DnsAddress1) {
            mIpv4DnsAddress1->setEnabled(false);
        }
        if (mIpv4DnsAddress2) {
            mIpv4DnsAddress2->setEnabled(false);
        }
    } else {
        // User defined, enable text fields
        if (mIpv4DnsAddress1) {
            mIpv4DnsAddress1->setEnabled(true);
        }
        if (mIpv4DnsAddress2) {
            mIpv4DnsAddress2->setEnabled(true);
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV4DNSFIELDS_EXIT);
}

/*!
    Sets IPv6 DNS address fields enabled/disabled.
*/
void CpPacketDataApAdvancedView::enableIpv6DnsFields(int selectionIndex)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV6DNSFIELDS_ENTRY);
    
    // Check combobox value
    if (selectionIndex == Ipv6DnsTypeUserDefined) {
        // User-defined, enable text fields
        if (mIpv6DnsAddress1) {
            mIpv6DnsAddress1->setEnabled(true);
        }
        if (mIpv6DnsAddress2) {
            mIpv6DnsAddress2->setEnabled(true);
        }
    } else {
        // Automatic or well-known, disable text fields
        if (mIpv6DnsAddress1) {
            mIpv6DnsAddress1->setEnabled(false);
        }
        if (mIpv6DnsAddress2) {
            mIpv6DnsAddress2->setEnabled(false);
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_ENABLEIPV6DNSFIELDS_EXIT);
}

/*!
    Validates an IP address and saves it to CommsDat in case it was a valid
    address or an empty string. In case the address is not valid, an error
    note is shown to the user and previous setting is restored to the UI.
*/
bool CpPacketDataApAdvancedView::validateAndSaveIpAddress(
    HbDataFormModelItem *item,
    CMManagerShim::ConnectionMethodAttribute attribute,
    QAbstractSocket::NetworkLayerProtocol protocol)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_VALIDATEANDSAVEIPADDRESS_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_VALIDATEANDSAVEIPADDRESS_EXIT);
    return success;
}

/*!
    Shows message box with "OK" button using given text.
*/
void CpPacketDataApAdvancedView::showMessageBox(
    HbMessageBox::MessageBoxType type,
    const QString &text)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_SHOWMESSAGEBOX_ENTRY);
    
    // Create a message box
    mMessageBox = QSharedPointer<HbMessageBox>(new HbMessageBox(type));
    mMessageBox->setText(text);
    mMessageBox->setModal(true);
    mMessageBox->setTimeout(HbPopup::NoTimeout);
    mMessageBox->open();
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_SHOWMESSAGEBOX_EXIT);
}

/*!
    Tries to update connection method changes to CommsDat.
    Returns "true" if success, "false" if some error happened. 
*/
bool CpPacketDataApAdvancedView::tryUpdate()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_TRYUPDATE_ENTRY);
    
    // Try update
    try {
        mCmConnectionMethod->update();
    }
    catch (const std::exception&) {
        // Handle error
        handleUpdateError();
        
        OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_TRYUPDATE_EXIT);
        return false;
    }

    OstTraceFunctionExit0(DUP1_CPPACKETDATAAPADVANCEDVIEW_TRYUPDATE_EXIT);
    return true;
}

/*!
    Handles failed CommsDat update.
 */
void CpPacketDataApAdvancedView::handleUpdateError()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_HANDLEUPDATEERROR_ENTRY);
    
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
            CPPACKETDATAAPPADVANCEDVIEW_HANDLEUPDATEERROR,
            "Refresh failed");
    };
    updateIpGroup();
    updateProxyGroup();
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_HANDLEUPDATEERROR_EXIT);
}

/*!
    Initializes all settings groups.
*/
void CpPacketDataApAdvancedView::setEditorPreferences(const QModelIndex index)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_SETEDITORPREFERENCES_ENTRY);
    
    HbDataFormViewItem *viewItem = qobject_cast<HbDataFormViewItem *>
        (mForm->itemByIndex(index));
    HbDataFormModelItem *modelItem = mModel->itemFromIndex(index);

    if (modelItem == mIpv4Address
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_SETEDITORPREFERENCES_EXIT);
}

/*!
    Handles change to network type (IPv4/IPv6).
*/
void CpPacketDataApAdvancedView::changeNetworkType(int type)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGENETWORKTYPE_ENTRY);

    // Save network type to CommsDat
    int pdpType;
    if (type == NetworkTypeIpv6) {
        pdpType = RPacketContext::EPdpTypeIPv6;
    } else {
        pdpType = RPacketContext::EPdpTypeIPv4;
    }
    mCmConnectionMethod->setIntAttribute(
        CMManagerShim::PacketDataPDPType,
        pdpType);
    if (tryUpdate())
        {
        // This will handle updating the UI with correct setting items 
        updateIpGroup();
        }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGENETWORKTYPE_EXIT);
}

/*!
    Handles change to IPv4 address mode (automatic/user defined).
*/
void CpPacketDataApAdvancedView::changeIpv4AddressMode(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4ADDRESSMODE_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4ADDRESSMODE_EXIT);
}

/*!
    Saves IPv4 address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeIpv4Address()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4ADDRESS_ENTRY);
    
    if (validateAndSaveIpAddress(
        mIpv4Address,
        CMManagerShim::CmIPAddress,
        QAbstractSocket::IPv4Protocol)) {
        // In case the IP address is empty or 0.0.0.0, CMManager changes the
        // "IP address from server" flag to true. We need to refresh the UI to
        // make sure we are in sync.
        updateIpGroup();
    }

    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4ADDRESS_EXIT);
}

/*!
    Handles change to IPv4 DNS mode (automatic/user defined).
*/
void CpPacketDataApAdvancedView::changeIpv4DnsMode(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4DNSMODE_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4DNSMODE_EXIT);
}

/*!
    Saves IPv4 primary DNS address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeIpv4PrimaryDnsAddress()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4PRIMARYDNSADDRESS_ENTRY);
    
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
            updateIpGroup();            
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4PRIMARYDNSADDRESS_EXIT);
}

/*!
    Saves IPv4 secondary DNS address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeIpv4SecondaryDnsAddress()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4SECONDARYDNSADDRESS_ENTRY);
    
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
            updateIpGroup();
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV4SECONDARYDNSADDRESS_EXIT);
}

/*!
    Handles change to IPv6 DNS mode (automatic/well-known/user defined).
*/
void CpPacketDataApAdvancedView::changeIpv6DnsMode(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6DNSMODE_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6DNSMODE_EXIT);
}

/*!
    Saves IPv6 primary DNS address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeIpv6PrimaryDnsAddress()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6PRIMARYDNSADDRESS_ENTRY);
    
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
            updateIpGroup();            
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6PRIMARYDNSADDRESS_EXIT);
}

/*!
    Saves IPv6 secondary DNS address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeIpv6SecondaryDnsAddress()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6SECONDARYDNSADDRESS_ENTRY);
    
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
            updateIpGroup();
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEIPV6SECONDARYDNSADDRESS_EXIT);
}

/*!
    Saves proxy server address to CommsDat.
*/
void CpPacketDataApAdvancedView::changeProxyServer()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEPROXYSERVER_ENTRY);
    
    QString proxyServer = mProxyServer->contentWidgetData("text").toString();
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::CmProxyServerName,
        proxyServer);
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEPROXYSERVER_EXIT);
}

/*!
    Saves proxy port to CommsDat.
*/
void CpPacketDataApAdvancedView::changeProxyPort()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPADVANCEDVIEW_CHANGEPROXYPORT_ENTRY);
    
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
    
    OstTraceFunctionExit0(CPPACKETDATAAPADVANCEDVIEW_CHANGEPROXYPORT_EXIT);
}
