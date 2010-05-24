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
class CpPacketDataApView;
class CpPacketDataApAdvancedView;
class CpBearerApPluginInterface;

class TestCpPacketDataApPlugin : public QObject
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
        void tcChangeAccessPointName();
        void tcChangeAccessPointName_data();
        void tcAccessPointNameEmpty();
        void tcTooLongUserNameInCommsDat();
        void tcChangeUserName();
        void tcChangeUserName_data();
        void tcScrollToBottom();
        void tcChangePromptPassword();
        void tcChangePassword();
        void tcChangePassword_data();
        void tcChangeAuthenticationMode();
        void tcChangeHomepage();
        void tcChangeHomepage_data();
        void tcOpenAdvancedSettingsView();
        void tcExpandIpSettings();
        void tcSetIpv4NetworkType();
        void tcEnableAutomaticIpv4Address();
        void tcEnableUserDefinedIpv4Address();
        void tcChangeIpAddress();
        void tcChangeIpAddress_data();
        void tcScrollToBottom2();
        void tcEnableAutomaticIpv4DnsAddress();
        void tcEnableUserDefinedIpv4DnsAddress();
        void tcChangeIpv4DnsAddress();
        void tcChangeIpv4DnsAddress_data();
        void tcInvalidIpv4DnsAddress();
        void tcScrollToTop();
        void tcSetIpv6NetworkType();
        void tcEnableAutomaticIpv6DnsAddress();
        void tcEnableWellKnownIpv6DnsAddress();
        void tcEnableUserDefinedIpv6DnsAddress();
        void tcChangeIpv6DnsAddress();
        void tcChangeIpv6DnsAddress_data();
        void tcInvalidIpv6DnsAddress();
        void tcCollapseIpSettings();
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
        void subInitializeCommsDat();
        void subCreateSettingsView(uint connetionMethodId);
        void subVerifyString(
            CMManagerShim::ConnectionMethodAttribute attribute,
            HbDataFormModelItem *item,
            QString expected);
        void subClearLineEdit(uint length);
        bool subGetBool(CMManagerShim::ConnectionMethodAttribute attribute);
        void subVerifyBool(
            CMManagerShim::ConnectionMethodAttribute attribute,
            bool expected);
        void subVerifyUint(
            CMManagerShim::ConnectionMethodAttribute attribute,
            uint expected);
        void subClickWidget(const QString &name);
        void subScrollToBottom();
        void subScrollToTop();

    private:
        // Code references
        CpPacketDataApView *mTestView;
        CpPacketDataApAdvancedView *mTestViewAdvanced;
        // "Access point settings" UI widgets
        HbWidget *mConnectionNameWidget;
        HbWidget *mAccessPointNameWidget;
        HbWidget *mUserNameWidget;
        HbWidget *mPasswordPromptWidget;
        HbWidget *mPasswordWidget;
        HbWidget *mAuthenticationWidget;
        HbWidget *mHomepageWidget;
        // "Advanced settings" UI widgets
        HbDataFormViewItem *mIpGroupViewItem;
        HbDataFormViewItem *mProxyGroupViewItem;
        HbWidget *mNetworkTypeWidget;
        HbWidget *mIpv4AddressAutomaticWidget;
        HbWidget *mIpv4AddressWidget;
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
