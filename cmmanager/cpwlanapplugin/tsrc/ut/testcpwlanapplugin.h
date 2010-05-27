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
* Control Panel WLAN AP plugin unit testing.
*/

#include <QStringList>
#include <QtTest/QtTest>
#include <cmmanagerdefines_shim.h>

class HbWidget;
class HbDialog;
class HbMainWindow;
class HbAutoTestMainWindow;
class HbDataForm;
class HbDataFormModelItem;
class HbDataFormViewItem;
class HbAction;
class CpWlanApView;
class CpWlanApAdvancedView;
class CpBearerApPluginInterface;

enum NetworkStatus {
    HiddenStatus,
    PublicStatus
};

class TestCpWlanApPlugin : public QObject
{
    Q_OBJECT

    signals:
        // Test signals
        void menuActionTriggered(HbAction *);
    
    public slots:
        // Test framework functions
        void initTestCase();
        void cleanupTestCase();
        void init();
        void cleanup();

    private slots:
        // Test cases
        void tcChangeConnectionName();
        void tcChangeConnectionName_data();
        void tcConnectionNameEmpty();
        void tcChangeWlanNetworkName();
        void tcChangeWlanNetworkName_data();
        void tcWlanNetworkNameEmpty();
        void tcChangeNetworkStatus();
        void tcChangeNetworkMode();
        void tcChangeSecurityMode();
        void tcScrollToBottom();
        void tcGetUiWidgets();
        void tcChangeHomepage();
        void tcChangeHomepage_data();
        void tcOpenAdvancedSettingsView();
        void tcExpandIpv4SettingsAndGetUiWidgets();
        void tcEnableAutomaticIpv4Address();
        void tcEnableUserDefinedIpv4Address();
        void tcChangeIpAddress();
        void tcChangeIpAddress_data();
        void tcChangeSubnetMask();
        void tcChangeSubnetMask_data();
        void tcInvalidSubnetMask();
        void tcChangeGateway();
        void tcChangeGateway_data();
        void tcInvalidGateway();
        void tcScrollToBottom2();
        void tcEnableAutomaticIpv4DnsAddress();
        void tcEnableUserDefinedIpv4DnsAddress();
        void tcChangeIpv4DnsAddress();
        void tcChangeIpv4DnsAddress_data();
        void tcInvalidIpv4DnsAddress();
        void tcScrollToTop();
        void tcCollapseIpv4Settings();
        void tcExpandIpv6SettingsAndGetUiWidgets();
        void tcEnableAutomaticIpv6DnsAddress();
        void tcEnableWellKnownIpv6DnsAddress();
        void tcEnableUserDefinedIpv6DnsAddress();
        void tcChangeIpv6DnsAddress();
        void tcChangeIpv6DnsAddress_data();
        void tcInvalidIpv6DnsAddress();
        void tcCollapseIpv6Settings();
        void tcExpandProxySettingsAndGetUiWidgets();
        void tcChangeProxyServerAddress();
        void tcChangeProxyServerAddress_data();
        void tcChangeProxyPortNumber();
        void tcChangeProxyPortNumber_data();
        void tcInvalidProxyPortNumber();
        void tcCollapseProxySettings();
        void tcCloseAdvancedSettingsView();

    private:
        // Sub test cases
        void subGetUiWidgets();
        void subGetAdvancedUiWidgets(uint index);
        HbWidget *subGetWidgetByIndex(
            HbDataForm *form,
            const QModelIndex &index);
        void subVerifyString(
            CMManagerShim::ConnectionMethodAttribute attribute,
            HbDataFormModelItem *item,
            QString expected);
        void subVerifyUint(
            CMManagerShim::ConnectionMethodAttribute attribute,
            uint expected);
        void subVerifyNetworkStatus(
            NetworkStatus expected);
        void subClearLineEdit(uint length);
        void subCreateSettingsView(uint connetionMethodId);
        void subClickWidget(const QString &name);
        QPointF subMiddlePointOfWidget(const HbWidget *widget);
        void subVerifyBool(
            CMManagerShim::ConnectionMethodAttribute attribute,
            bool expected);
        void subScrollToBottom();
        void subScrollToTop();

    private:
        // Code references
        CpWlanApView *mTestView;
        CpWlanApAdvancedView *mTestViewAdvanced;
        // "Access point settings" UI widgets
        HbWidget *mConnectionNameWidget;
        HbWidget *mWlanNetworkNameWidget;
        HbWidget *mNetworkStatusWidget;
        HbWidget *mNetworkModeWidget;
        HbWidget *mSecurityModeWidget;
        HbWidget *mHomepageWidget;
        // "Advanced settings" UI widgets
        HbDataFormViewItem *mIpv4GroupViewItem;
        HbDataFormViewItem *mIpv6GroupViewItem;
        HbDataFormViewItem *mProxyGroupViewItem;
        HbWidget *mIpv4AddressAutomaticWidget;
        HbWidget *mIpv4AddressWidget;
        HbWidget *mIpv4SubnetMaskWidget;
        HbWidget *mIpv4GatewayWidget;
        HbWidget *mIpv4DnsAddressAutomaticWidget;
        HbWidget *mIpv4PrimaryDnsAddressWidget;
        HbWidget *mIpv4SecondaryDnsAddressWidget;
        HbWidget *mIpv6DnsAddressAutomaticWidget;
        HbWidget *mIpv6PrimaryDnsAddressWidget;
        HbWidget *mIpv6SecondaryDnsAddressWidget;
        HbWidget *mProxyServerAddressWidget;
        HbWidget *mProxyPortNumberWidget;
        
        // Test data
        HbAutoTestMainWindow *mMainWindow;
        QPluginLoader *mPluginLoader;
        CpBearerApPluginInterface *mPlugin;
};
