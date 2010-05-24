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

#include <HbApplication>
#include <HbMainWindow>
#include <HbView>
#include <HbDialog>
#include <HbRadioButtonList>
#include <HbAction>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbModelIterator>
#include <QSharedPointer>
#include <QtTest/QtTest>
#include <cpbearerapplugininterface.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

#include "cpwlanapview.h"
#include "cpwlanapadvancedview.h"

#include "hbautotest.h"
#include "testcpwlanapplugin.h"

// Following flag must be defined if WLAN security settings plugins
// (WEP, WPA/WPA2 and WPA2) are available. Flag can be removed when the
// plugins are released.
#define WLAN_SECURITY_PLUGINS_AVAILABLE

// -----------------------------------------------------------------------------
// STATIC TEST DATA
// -----------------------------------------------------------------------------

// Connection method (AP) ID used for testing
static const uint testApId = 5;

static const QString pluginDir =
    "\\resource\\qt\\plugins\\controlpanel\\bearerap";

static const QString pluginName = "cpwlanapplugin.dll";

// Time to wait before continuing after an UI step
static const int waitTime = 10;

// UI coordinates
static const QPoint scrollMiddle(350, 280);
static const QPoint scrollTop(350, 30);
static const QPoint scrollBottom(350, 540);

// Use positive offset if dropdown opens under the combobox
static const QPoint comboBoxFirstItemOffset(80, 75);
static const QPoint comboBoxItemOffset(0, 51);
// Use negative offset if dropdown opens above the combobox
static const QPoint comboBoxFirstItemNegativeOffset(80, -29);
static const QPoint comboBoxItemNegativeOffset(0, -51);

static const QPoint exitEditorOffset(-10, -20);

static const QPoint messageBoxOkButtonOffset(160, 140);

// Advanced settings groups
static const QPoint ipv4Group(160, 10);
static const QPoint ipv6Group(160, 60);
static const QPoint proxyGroup(160, 110);

// -----------------------------------------------------------------------------
// FRAMEWORK FUNCTIONS
// -----------------------------------------------------------------------------

/**
 * Test main function. Runs all test cases.
 */
#ifndef TESTCMAPPLSETTINGSUI_NO_OUTPUT_REDIRECT
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    app.setApplicationName("TestCpWlanApPlugin");
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestCpWlanApPlugin.txt";
 
    TestCpWlanApPlugin tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}
#else
QTEST_MAIN(TestCpWlanApPlugin)
#endif

/**
 * This function is be called before the first test case is executed.
 */
void TestCpWlanApPlugin::initTestCase()
{
    mMainWindow = new HbAutoTestMainWindow;
    //mMainWindow = new HbMainWindow;
    mMainWindow->show();
    
    // Load plugin
    QDir dir(pluginDir);
    mPluginLoader = new QPluginLoader(dir.absoluteFilePath(pluginName));
    mPlugin = qobject_cast<CpBearerApPluginInterface *>(mPluginLoader->instance());
    QVERIFY(mPlugin != NULL);
    
    // Verify plugin bearer type
    QVERIFY(mPlugin->bearerType() == CMManagerShim::BearerTypeWlan);
    
    // Create WLAN settings view (connection method ID given)
    subCreateSettingsView(testApId);
    
    subGetUiWidgets();
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCpWlanApPlugin::cleanupTestCase()
{
    delete mMainWindow;
    mMainWindow = 0;
    
    // Force unloading of plugin
    mPluginLoader->unload();
    delete mPluginLoader;
    mPluginLoader = 0;
}

/**
 * This function is be called before each test case is executed.
 */
void TestCpWlanApPlugin::init()
{
    QTest::qWait(1000);
}

/**
 * This function is be called after each test case is executed.
 */
void TestCpWlanApPlugin::cleanup()
{
}

// -----------------------------------------------------------------------------
// TEST CASES
// -----------------------------------------------------------------------------

/**
 * Tests changing of connection name.
 */
void TestCpWlanApPlugin::tcChangeConnectionName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mConnectionNameWidget);
    
    // Erase old string
    QString text = mTestView->mConnectionNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mConnectionNameWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmName,
        mTestView->mConnectionNameItem,
        result);
}

/**
 * Test data for connection name change test case.
 */
void TestCpWlanApPlugin::tcChangeConnectionName_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("maximum length")
        << "really long name 123456789012345678901234567890123"
        << "really long name 123456789012345678901234567890123";
    QTest::newRow("too long")
        << "too long name 12345678901234567890123456789012345678901234"
        << "too long name 123456789012345678901234567890123456";
    QTest::newRow("basic") // last one must always fit on one line in UI
        << "test WLAN AP"
        << "test WLAN AP";
}

/**
 * Tests that empty connection name is not accepted.
 */
void TestCpWlanApPlugin::tcConnectionNameEmpty()
{
    QString previous = 
        mTestView->mConnectionNameItem->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mConnectionNameWidget);

    // Erase old string
    QString text = mTestView->mConnectionNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    HbAutoTest::mouseClick(mMainWindow, mConnectionNameWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestView->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmName,
        mTestView->mConnectionNameItem,
        previous);
}

/**
 * Tests changing of WLAN network name.
 */
void TestCpWlanApPlugin::tcChangeWlanNetworkName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mWlanNetworkNameWidget);
    
    // Erase old string
    QString text = mTestView->mWlanNetworkNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mWlanNetworkNameWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::WlanSSID,
        mTestView->mWlanNetworkNameItem,
        result);
}

/**
 * Test data for WLAN network name change test case.
 */
void TestCpWlanApPlugin::tcChangeWlanNetworkName_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("maximum length")
        << "really long name 123456789012345"
        << "really long name 123456789012345";
    QTest::newRow("too long")
        << "too long name 123456789012345678901234567890"
        << "too long name 123456789012345678";
    QTest::newRow("basic") // last one must always fit on one line in UI
        << "test SSID"
        << "test SSID";
}

/**
 * Tests that empty WLAN network name is not accepted.
 */
void TestCpWlanApPlugin::tcWlanNetworkNameEmpty()
{
    QString previous = 
        mTestView->mWlanNetworkNameItem->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mWlanNetworkNameWidget);
    
    // Erase old string
    QString text = mTestView->mWlanNetworkNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    HbAutoTest::mouseClick(mMainWindow, mWlanNetworkNameWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestView->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::WlanSSID,
        mTestView->mWlanNetworkNameItem,
        previous);
}

/**
 * Tests changing of WLAN network status.
 */
void TestCpWlanApPlugin::tcChangeNetworkStatus()
{
    QPointF publicPointOffset = comboBoxFirstItemOffset;
    QPointF hiddenPointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    
    // Set network status to hidden
    HbAutoTest::mouseClick(mMainWindow, mNetworkStatusWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkStatusWidget, hiddenPointOffset, 100);
    subVerifyNetworkStatus(HiddenStatus);

    // Set network status to public
    HbAutoTest::mouseClick(mMainWindow, mNetworkStatusWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkStatusWidget, publicPointOffset, 100);
    subVerifyNetworkStatus(PublicStatus);
}

/**
 * Tests changing of WLAN network mode.
 */
void TestCpWlanApPlugin::tcChangeNetworkMode()
{
    QPointF infraPointOffset = comboBoxFirstItemOffset;
    QPointF adHocPointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    
    // Set network mode to ad-hoc
    HbAutoTest::mouseClick(mMainWindow, mNetworkModeWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkModeWidget, adHocPointOffset, 100);
    subVerifyUint(CMManagerShim::WlanConnectionMode, CMManagerShim::Adhoc);

    // Set network mode to infrastructure
    HbAutoTest::mouseClick(mMainWindow, mNetworkModeWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkModeWidget, infraPointOffset, 100);
    subVerifyUint(CMManagerShim::WlanConnectionMode, CMManagerShim::Infra);
}

/**
 * Tests changing of WLAN security mode.
 */
void TestCpWlanApPlugin::tcChangeSecurityMode()
{
#ifdef WLAN_SECURITY_PLUGINS_AVAILABLE
    QPointF openPointOffset = comboBoxFirstItemNegativeOffset + comboBoxItemNegativeOffset * 3;
    QPointF wepPointOffset = comboBoxFirstItemNegativeOffset + comboBoxItemNegativeOffset * 2;
    QPointF wpaPointOffset = comboBoxFirstItemNegativeOffset + comboBoxItemNegativeOffset;
    QPointF wpa2PointOffset = comboBoxFirstItemNegativeOffset;
#else
    QPointF openPointOffset = comboBoxFirstItemNegativeOffset;
#endif
    
#ifdef WLAN_SECURITY_PLUGINS_AVAILABLE
    // Set security mode to WEP
    HbAutoTest::mouseClick(mMainWindow, mSecurityModeWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mSecurityModeWidget, wepPointOffset, 100);
    subVerifyUint(
        CMManagerShim::WlanSecurityMode,
        CMManagerShim::WlanSecModeWep);
    
    QTest::qWait(1000);
#endif
    // Set security mode to open
    HbAutoTest::mouseClick(mMainWindow, mSecurityModeWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mSecurityModeWidget, openPointOffset, 100);
    subVerifyUint(
        CMManagerShim::WlanSecurityMode,
        CMManagerShim::WlanSecModeOpen);
}

/**
 * Scrolls the tested view to the bottom.
 */
void TestCpWlanApPlugin::tcScrollToBottom()
{
    subScrollToBottom();
}

/**
 * Gets UI widget pointers because homepage view item may have been
 * created before it was visible on the UI.
 */
void TestCpWlanApPlugin::tcGetUiWidgets()
{
    subGetUiWidgets();
}

/**
 * Tests changing of homepage.
 */
void TestCpWlanApPlugin::tcChangeHomepage()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mHomepageWidget);
    
    // Erase old string
    QString text = mTestView->mHomepageItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mHomepageWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmStartPage,
        mTestView->mHomepageItem,
        result);
}

/**
 * Test data for homepage change test case.
 */
void TestCpWlanApPlugin::tcChangeHomepage_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("long")
        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html"
        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html";
    QTest::newRow("basic") // last one should always fit on one line in UI
        << "http://www.symbian.org/"
        << "http://www.symbian.org/";
    QTest::newRow("empty")
        << ""
        << "";
}

/**
 * Opens advanced settings view.
 */
void TestCpWlanApPlugin::tcOpenAdvancedSettingsView()
{
    // Launch advanced settings view
    bool status = connect(
        this,
        SIGNAL(menuActionTriggered(HbAction *)),
        mTestView,
        SLOT(menuActionTriggered(HbAction *)));
    Q_ASSERT(status);
    emit menuActionTriggered(mTestView->mAdvancedSettingsAction);
    
    QTest::qWait(1000);
    
    mTestViewAdvanced = static_cast<CpWlanApAdvancedView *>(mMainWindow->currentView());
}

/**
 * Expands the IPv4 settings group and gets UI widget pointers.
 */
void TestCpWlanApPlugin::tcExpandIpv4SettingsAndGetUiWidgets()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipv4Group);
    
    QTest::qWait(500);

    subGetAdvancedUiWidgets(0);
}

/**
 * Tests enabling of automatic phone IP address.
 */
void TestCpWlanApPlugin::tcEnableAutomaticIpv4Address()
{
    Qt::CheckState state = static_cast<Qt::CheckState>
        (mTestViewAdvanced->mIpv4Automatic->contentWidgetData("checkState").toInt());
    if (state == Qt::Checked) {
        // Disable automatic IP address
        HbAutoTest::mouseClick(mMainWindow, mIpv4AddressAutomaticWidget);
    }
    // Enable automatic IP address
    HbAutoTest::mouseClick(mMainWindow, mIpv4AddressAutomaticWidget);
    subVerifyBool(
        CMManagerShim::CmIPAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv4AddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv4SubnetMaskWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv4GatewayWidget);
}

/**
 * Tests disabling of automatic phone IP address.
 */
void TestCpWlanApPlugin::tcEnableUserDefinedIpv4Address()
{
    Qt::CheckState state = static_cast<Qt::CheckState>
        (mTestViewAdvanced->mIpv4Automatic->contentWidgetData("checkState").toInt());
    if (state == Qt::Checked) {
        // Disable automatic IP address
        HbAutoTest::mouseClick(mMainWindow, mIpv4AddressAutomaticWidget);
    }
    // Can't verify the setting from CommsDat here, because CMManager will
    // set it back to true if no valid IP address is yet defined. The flag
    // is verified in tcChangeIpAddress().
}

/**
 * Tests changing of IP address.
 */
void TestCpWlanApPlugin::tcChangeIpAddress()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4AddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4Address->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4AddressWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPAddress,
        mTestViewAdvanced->mIpv4Address,
        result);
    subVerifyBool(
        CMManagerShim::CmIPAddrFromServer,
        false);
}

/**
 * Test data for IP address change test case.
 */
void TestCpWlanApPlugin::tcChangeIpAddress_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("too long")
        << "255.255.255.2551234"
        << "255.255.255.255";
    QTest::newRow("normal")
        << "192.168.0.1"
        << "192.168.0.1";
}

/**
 * Tests changing of subnet mask.
 */
void TestCpWlanApPlugin::tcChangeSubnetMask()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4SubnetMaskWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4SubnetMask->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4SubnetMaskWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPNetmask,
        mTestViewAdvanced->mIpv4SubnetMask,
        result);
}

/**
 * Test data for subnet mask change test case.
 */
void TestCpWlanApPlugin::tcChangeSubnetMask_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("empty")
        << ""
        << "0.0.0.0";
    QTest::newRow("zero")
        << "0.0.0.0"
        << "0.0.0.0";
    QTest::newRow("too long")
        << "255.255.255.2551234"
        << "255.255.255.255";
    QTest::newRow("normal")
        << "192.168.0.1"
        << "192.168.0.1";
}

/**
 * Tests invalid subnet mask.
 */
void TestCpWlanApPlugin::tcInvalidSubnetMask()
{
    QString previous = 
        mTestViewAdvanced->mIpv4SubnetMask->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4SubnetMaskWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4SubnetMask->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, "999.999.999.999", 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4SubnetMaskWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestViewAdvanced->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPNetmask,
        mTestViewAdvanced->mIpv4SubnetMask,
        previous);
}

/**
 * Tests changing of gateway.
 */
void TestCpWlanApPlugin::tcChangeGateway()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4GatewayWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4Gateway->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4GatewayWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPGateway,
        mTestViewAdvanced->mIpv4Gateway,
        result);
}

/**
 * Test data for gateway change test case.
 */
void TestCpWlanApPlugin::tcChangeGateway_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("empty")
        << ""
        << "0.0.0.0";
    QTest::newRow("zero")
        << "0.0.0.0"
        << "0.0.0.0";
    QTest::newRow("too long")
        << "255.255.255.2551234"
        << "255.255.255.255";
    QTest::newRow("normal")
        << "192.168.0.1"
        << "192.168.0.1";
}

/**
 * Tests invalid gateway address.
 */
void TestCpWlanApPlugin::tcInvalidGateway()
{
    QString previous = 
        mTestViewAdvanced->mIpv4Gateway->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4GatewayWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4Gateway->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, "999.999.999.999", 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4GatewayWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestViewAdvanced->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPGateway,
        mTestViewAdvanced->mIpv4Gateway,
        previous);
}

/**
 * Scrolls the tested view to the bottom.
 */
void TestCpWlanApPlugin::tcScrollToBottom2()
{
    subScrollToBottom();
}

/**
 * Tests enabling of automatic IPv4 DNS addresses.
 */
void TestCpWlanApPlugin::tcEnableAutomaticIpv4DnsAddress()
{
    Qt::CheckState state = static_cast<Qt::CheckState>
        (mTestViewAdvanced->mIpv4DnsAutomatic->contentWidgetData("checkState").toInt());
    if (state == Qt::Checked) {
        // Disable automatic IPv4 DNS address
        HbAutoTest::mouseClick(mMainWindow, mIpv4DnsAddressAutomaticWidget);
    }
    // Enable automatic IPv4 DNS address
    HbAutoTest::mouseClick(mMainWindow, mIpv4DnsAddressAutomaticWidget);
    subVerifyBool(
        CMManagerShim::CmIPDNSAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv4SecondaryDnsAddressWidget);
}

/**
 * Tests disabling of automatic IPv4 DNS addresses.
 */
void TestCpWlanApPlugin::tcEnableUserDefinedIpv4DnsAddress()
{
    Qt::CheckState state = static_cast<Qt::CheckState>
        (mTestViewAdvanced->mIpv4DnsAutomatic->contentWidgetData("checkState").toInt());
    if (state == Qt::Checked) {
        // Disable automatic IPv4 DNS address
        HbAutoTest::mouseClick(mMainWindow, mIpv4DnsAddressAutomaticWidget);
    }
    // Can't verify the setting from CommsDat here, because CMManager will
    // set it back to true if no valid IP address is yet defined. The flag
    // is verified in tcChangeIpv4DnsAddress().
}

/**
 * Tests changing of IPv4 DNS addresses.
 */
void TestCpWlanApPlugin::tcChangeIpv4DnsAddress()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    // Primary DNS address
    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4DnsAddress1->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPNameServer1,
        mTestViewAdvanced->mIpv4DnsAddress1,
        result);
    
    // Secondary DNS address
    HbAutoTest::mouseClick(mMainWindow, mIpv4SecondaryDnsAddressWidget);
    
    // Erase old string
    text = mTestViewAdvanced->mIpv4DnsAddress2->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4SecondaryDnsAddressWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPNameServer2,
        mTestViewAdvanced->mIpv4DnsAddress2,
        result);
    
    // Verify user defined address is in use
    subVerifyBool(
        CMManagerShim::CmIPDNSAddrFromServer,
        false);
}

/**
 * Test data for IPv4 DNS address change test case.
 */
void TestCpWlanApPlugin::tcChangeIpv4DnsAddress_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("too long")
        << "255.255.255.2551234"
        << "255.255.255.255";
    QTest::newRow("normal")
        << "192.168.0.1"
        << "192.168.0.1";
}

/**
 * Tests invalid IPv4 DNS address.
 */
void TestCpWlanApPlugin::tcInvalidIpv4DnsAddress()
{
    QString previous = 
        mTestViewAdvanced->mIpv4DnsAddress1->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv4DnsAddress1->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, "999.999.999.999", 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestViewAdvanced->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIPNameServer1,
        mTestViewAdvanced->mIpv4DnsAddress1,
        previous);
}

/**
 * Scrolls the tested view to the top.
 */
void TestCpWlanApPlugin::tcScrollToTop()
{
    subScrollToTop();
}

/**
 * Collapses the IPv4 settings group.
 */
void TestCpWlanApPlugin::tcCollapseIpv4Settings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipv4Group);
}

/**
 * Expands the IPv6 settings group and gets UI widget pointers.
 */
void TestCpWlanApPlugin::tcExpandIpv6SettingsAndGetUiWidgets()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipv6Group);

    QTest::qWait(500);
    
    subGetAdvancedUiWidgets(1);
}

/**
 * Tests enabling of automatic IPv6 DNS addresses.
 */
void TestCpWlanApPlugin::tcEnableAutomaticIpv6DnsAddress()
{
    // Enable automatic IPv6 DNS address
    QPointF automaticPointOffset = comboBoxFirstItemOffset;
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget, automaticPointOffset, 100);
    QTest::qWait(100);
    subVerifyBool(
        CMManagerShim::CmIP6DNSAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget);
}

/**
 * Tests enabling of well-known IPv6 DNS addresses.
 */
void TestCpWlanApPlugin::tcEnableWellKnownIpv6DnsAddress()
{
    // Enable well-known IPv6 DNS address
    QPointF wellKnownPointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget, wellKnownPointOffset, 100);
    QTest::qWait(100);
    subVerifyBool(
        CMManagerShim::CmIP6DNSAddrFromServer,
        false);
    subVerifyString(
        CMManagerShim::CmIP6NameServer1,
        mTestViewAdvanced->mIpv6DnsAddress1,
        "fec0:000:0000:ffff::1");
    subVerifyString(
        CMManagerShim::CmIP6NameServer2,
        mTestViewAdvanced->mIpv6DnsAddress2,
        "fec0:000:0000:ffff::2");
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget);
}

/**
 * Tests enabling of user defined IPv6 DNS addresses.
 */
void TestCpWlanApPlugin::tcEnableUserDefinedIpv6DnsAddress()
{
    // Select user defined IPv6 DNS address
    QPointF userDefinedPointOffset = comboBoxFirstItemOffset + comboBoxItemOffset * 2;
    //QPointF userDefinedPointOffset(0, 160);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget, userDefinedPointOffset, 100);
    QTest::qWait(100);
    // Can't verify the setting from CommsDat here, because CMManager will
    // set it back to true if no valid IP address is yet defined. The flag
    // is verified in tcChangeIpv4DnsAddress().
}

/**
 * Tests changing of IPv6 DNS addresses.
 */
void TestCpWlanApPlugin::tcChangeIpv6DnsAddress()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    // Primary DNS address
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv6DnsAddress1->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIP6NameServer1,
        mTestViewAdvanced->mIpv6DnsAddress1,
        result);
    
    // Secondary DNS address
    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget);
    
    // Erase old string
    text = mTestViewAdvanced->mIpv6DnsAddress2->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIP6NameServer2,
        mTestViewAdvanced->mIpv6DnsAddress2,
        result);
    
    // Verify user defined address is in use
    subVerifyBool(
        CMManagerShim::CmIP6DNSAddrFromServer,
        false);
}

/**
 * Test data for IPv6 DNS address change test case.
 */
void TestCpWlanApPlugin::tcChangeIpv6DnsAddress_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("normal")
        << "2001:db8:85a3::8a2e:370:7334"
        << "2001:db8:85a3:0:0:8a2e:370:7334";
}

/**
 * Tests invalid IPv6 DNS address.
 */
void TestCpWlanApPlugin::tcInvalidIpv6DnsAddress()
{
    QString previous = 
        mTestViewAdvanced->mIpv6DnsAddress1->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mIpv6DnsAddress1->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, "abcdef:fedcba", 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestViewAdvanced->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmIP6NameServer1,
        mTestViewAdvanced->mIpv6DnsAddress1,
        previous);
}

/**
 * Collapses the IPv6 settings group.
 */
void TestCpWlanApPlugin::tcCollapseIpv6Settings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipv6Group);
}

/**
 * Expands the proxy settings group and gets UI widget pointers.
 */
void TestCpWlanApPlugin::tcExpandProxySettingsAndGetUiWidgets()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, proxyGroup);

    QTest::qWait(500);
    
    subGetAdvancedUiWidgets(2);
}

/**
 * Tests changing of proxy server address.
 */
void TestCpWlanApPlugin::tcChangeProxyServerAddress()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mProxyServerAddressWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mProxyServer->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mProxyServerAddressWidget, exitEditorOffset);

    QTest::qWait(100);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::CmProxyServerName,
        mTestViewAdvanced->mProxyServer,
        result);
    // Verify user defined address flag is set correctly
    if (result.isEmpty()) {
        subVerifyBool(
            CMManagerShim::CmProxyUsageEnabled,
            false);
    } else {
        subVerifyBool(
            CMManagerShim::CmProxyUsageEnabled,
            true);
    }
}

/**
 * Test data for proxy server address change test case.
 */
void TestCpWlanApPlugin::tcChangeProxyServerAddress_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("long")
        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html"
        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html";
    QTest::newRow("empty")
        << ""
        << "";
    QTest::newRow("basic") // last one should always fit on one line in UI
        << "http://www.symbian.org/"
        << "http://www.symbian.org/";
}

/**
 * Tests changing of proxy port number.
 */
void TestCpWlanApPlugin::tcChangeProxyPortNumber()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mProxyPortNumberWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mProxyPort->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mProxyPortNumberWidget, exitEditorOffset);

    QTest::qWait(100);

    // Verify both commsdat and UI widget
    subVerifyUint(
        CMManagerShim::CmProxyPortNumber,
        result.toInt());
    QCOMPARE(
        mTestViewAdvanced->mProxyPort->contentWidgetData("text").toInt(),
        result.toInt());
}

/**
 * Test data for proxy port number change test case.
 */
void TestCpWlanApPlugin::tcChangeProxyPortNumber_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
  
    QTest::newRow("basic")
        << "8080"
        << "8080";
    QTest::newRow("too long")
        << "1234567890"
        << "12345";
    QTest::newRow("zero")
        << "0"
        << "";
    QTest::newRow("empty")
        << ""
        << "";
}

/**
 * Tests invalid proxy port number.
 */
void TestCpWlanApPlugin::tcInvalidProxyPortNumber()
{
    int previous = 
        mTestViewAdvanced->mProxyPort->contentWidgetData("text").toInt();
    
    HbAutoTest::mouseClick(mMainWindow, mProxyPortNumberWidget);
    
    // Erase old string
    QString text = mTestViewAdvanced->mProxyPort->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, "65536", 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mProxyPortNumberWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestViewAdvanced->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyUint(
        CMManagerShim::CmProxyPortNumber,
        previous);
    QCOMPARE(
        mTestViewAdvanced->mProxyPort->contentWidgetData("text").toInt(),
        previous);
}

/**
 * Collapses the proxy settings group.
 */
void TestCpWlanApPlugin::tcCollapseProxySettings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, proxyGroup);
}

/**
 * Returns from advanced settings view.
 */
void TestCpWlanApPlugin::tcCloseAdvancedSettingsView()
{
    // Return from advanced settings view
    subClickWidget("HbNavigationButton");
}

// -----------------------------------------------------------------------------
// SUB TEST CASES
// -----------------------------------------------------------------------------

/**
 * Gets UI widget pointers.
 */
void TestCpWlanApPlugin::subGetUiWidgets()
{
    HbModelIterator iterator(mTestView->mModel);
    
    // Get "Access point settings" group item
    QModelIndex apGroupIndex = iterator.index(0);
    
    // Get UI widgets
    mConnectionNameWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(0, apGroupIndex));
    mWlanNetworkNameWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(1, apGroupIndex));
    mNetworkStatusWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(2, apGroupIndex));
    mNetworkModeWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(3, apGroupIndex));
    mSecurityModeWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(4, apGroupIndex));
    mHomepageWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(5, apGroupIndex));
}

/**
 * Gets advanced settings view UI widget pointers by group index.
 */
void TestCpWlanApPlugin::subGetAdvancedUiWidgets(
    uint index)
{
    // Get the group item
    HbModelIterator iterator(mTestViewAdvanced->mModel);
    QModelIndex groupIndex = iterator.index(index);
    
    // Get UI widgets
    if (index == 0) {
        // IPv4 settings group
        mIpv4AddressAutomaticWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(0, groupIndex));
        mIpv4AddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(1, groupIndex));
        mIpv4SubnetMaskWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(2, groupIndex));
        mIpv4GatewayWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(3, groupIndex));
        mIpv4DnsAddressAutomaticWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(4, groupIndex));
        mIpv4PrimaryDnsAddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(5, groupIndex));
        mIpv4SecondaryDnsAddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(6, groupIndex));
    } else if (index == 1) {
        // IPv6 settings group
        mIpv6DnsAddressAutomaticWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(0, groupIndex));
        mIpv6PrimaryDnsAddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(1, groupIndex));
        mIpv6SecondaryDnsAddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(2, groupIndex));    
    } else {
        // Proxy settings group
        mProxyServerAddressWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(0, groupIndex));
        mProxyPortNumberWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(1, groupIndex));
    }
}

/**
 * Gets an UI widget from HbDataForm by index. 
 */
HbWidget *TestCpWlanApPlugin::subGetWidgetByIndex(
    HbDataForm *form,
    const QModelIndex &index)
{
    HbDataFormViewItem *viewItem = qobject_cast<HbDataFormViewItem *>
        (form->itemByIndex(index));
    HbWidget *widget = viewItem->dataItemContentWidget();
    //QString widgetClassName(widget->metaObject()->className());
    //qDebug() << widgetClassName;
    return widget;
}

/**
 * Verifies that given string is correctly stored in CommsDat and shown on UI. 
 */
void TestCpWlanApPlugin::subVerifyString(
    CMManagerShim::ConnectionMethodAttribute attribute,
    HbDataFormModelItem *item,
    QString expected)
{
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    QString commsdat = connectionMethod->getStringAttribute(attribute);

    QCOMPARE(commsdat, expected);

    // Get value from UI widget
    QString widget = item->contentWidgetData("text").toString();

    QCOMPARE(widget, expected);
}

/**
 * Verifies that given attribute contains expected integer value in CommsDat. 
 */
void TestCpWlanApPlugin::subVerifyUint(
    CMManagerShim::ConnectionMethodAttribute attribute,
    uint expected)
{
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    uint commsdat = connectionMethod->getIntAttribute(attribute);
    
    QCOMPARE(commsdat, expected);
}

/**
 * Verifies that WLAN network status in CommsDat is correct.
 */
void TestCpWlanApPlugin::subVerifyNetworkStatus(
    NetworkStatus expected)
{
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    bool commsdatScanSsid = connectionMethod->getBoolAttribute(
        CMManagerShim::WlanScanSSID);
    
    if (expected == HiddenStatus) {
        QVERIFY(commsdatScanSsid == true);
    } else if (expected == PublicStatus) {
        QVERIFY(commsdatScanSsid == false);
    } else {
        Q_ASSERT(false);
    }
}

/**
 * Clears a HbLineEdit.
 */
void TestCpWlanApPlugin::subClearLineEdit(
    uint length)
{
    // Move cursor to end of string
    //HbAutoTest::keyClick(mMainWindow, Qt::Key_End, Qt::ControlModifier, waitTime); // doesn't seem to do anything? 
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    for (int i=0; i<25; i++) {
        HbAutoTest::keyClick(mMainWindow, Qt::Key_Right, 0, waitTime);
    }
    // Erase string
    for (int i=0; i<length; i++) {
        HbAutoTest::keyClick(mMainWindow, Qt::Key_Backspace, 0, waitTime);
    }
}

/**
 * Creates the settings view and shows it.
 */
void TestCpWlanApPlugin::subCreateSettingsView(uint connectionMethodId)
{
    // Create settings view
    HbView *view = mPlugin->createSettingView(connectionMethodId);
    QVERIFY(view != NULL);
    
    // Display the view
    mMainWindow->addView(view);
    mMainWindow->setCurrentView(view);
    // Store pointer to settings view class
    mTestView = static_cast<CpWlanApView *>(view);    
}

/**
 * Clicks a widget currently on UI by class name.
 */
void TestCpWlanApPlugin::subClickWidget(const QString &name)
{
    QList<QGraphicsItem *> itemList = mMainWindow->scene()->items();

    QGraphicsItem *target = 0;
    foreach (QGraphicsItem* item, itemList) {
        if (item->isWidget()) {
            QString widgetClassName(static_cast<QGraphicsWidget*>(item)->metaObject()->className());
            //qDebug() << widgetClassName;
            
            if (widgetClassName == name) {
                target = item;
                break;
            }
        }
    }

    Q_ASSERT(target);
    HbAutoTest::mouseClick(mMainWindow, static_cast<HbWidget *>(target));
}

/**
 * Returns the middle point of a widget.
 */
QPointF TestCpWlanApPlugin::subMiddlePointOfWidget(const HbWidget *widget)
{
    QRectF widgetRect = widget->rect();
    QRectF widgetSceneRect = widget->mapRectToScene(widgetRect);
    qreal middleX = ((widgetSceneRect.right() - widgetSceneRect.left()) / 2) + widgetSceneRect.left();
    qreal middleY = ((widgetSceneRect.bottom() - widgetSceneRect.top()) / 2) + widgetSceneRect.top();
    
    return QPointF(middleX, middleY);
}

/**
 * Verifies that given attribute contains expected boolean value in CommsDat. 
 */
void TestCpWlanApPlugin::subVerifyBool(
    CMManagerShim::ConnectionMethodAttribute attribute,
    bool expected)
{
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    bool commsdat = connectionMethod->getBoolAttribute(attribute);
    
    QCOMPARE(commsdat, expected);
}

/**
 * Scrolls the tested view to the bottom.
 */
void TestCpWlanApPlugin::subScrollToBottom()
{
    // Scroll to the bottom of the view
    HbAutoTest::mousePress(mMainWindow, mTestView, scrollMiddle);
    QTest::qWait(1000);
    HbAutoTest::mouseMove(mMainWindow, mTestView, scrollTop);
    HbAutoTest::mouseRelease(mMainWindow, mTestView, scrollTop);
}

/**
 * Scrolls the tested view to the top.
 */
void TestCpWlanApPlugin::subScrollToTop()
{
    // Scroll to the top of the view
    HbAutoTest::mousePress(mMainWindow, mTestView, scrollMiddle);
    QTest::qWait(1000);
    HbAutoTest::mouseMove(mMainWindow, mTestView, scrollBottom);
    HbAutoTest::mouseRelease(mMainWindow, mTestView, scrollBottom);
}
