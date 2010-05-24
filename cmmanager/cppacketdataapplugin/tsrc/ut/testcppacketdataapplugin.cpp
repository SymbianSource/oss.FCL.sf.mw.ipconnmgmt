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
* Control Panel packet data AP plugin unit testing.
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
#include <HbComboBox>
#include <QtTest/QtTest>
#include <etelpckt.h>
#include <cpbearerapplugininterface.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

#include "cppacketdataapview.h"
#include "cppacketdataapadvancedview.h"

#include "hbautotest.h"
#include "testcppacketdataapplugin.h"

// -----------------------------------------------------------------------------
// STATIC TEST DATA
// -----------------------------------------------------------------------------

// Connection method (AP) ID used for testing
static const uint testApId = 1;

static const QString pluginDir =
    "\\resource\\qt\\plugins\\controlpanel\\bearerap";

static const QString pluginName = "cppacketdataapplugin.dll";

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
static const QPoint ipGroup(160, 10);
static const QPoint proxyGroup(160, 60);

// Test strings
static const QString tooLongUserName =
    "too long name 123456789012345678901234567890123456THISISEXTRA";
static const QString truncatedUserName =
    "too long name 123456789012345678901234567890123456";

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
    app.setApplicationName("TestCpPacketDataApPlugin");
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestCpPacketDataApPlugin.txt";
 
    TestCpPacketDataApPlugin tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}
#else
QTEST_MAIN(TestCpPacketDataApPlugin)
#endif

/**
 * This function is be called before the first test case is executed.
 */
void TestCpPacketDataApPlugin::initTestCase()
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
    QVERIFY(mPlugin->bearerType() == CMManagerShim::BearerTypePacketData);
    
    // Write initial settings to CommsDat
    subInitializeCommsDat();
    
    // Create packet data settings view (connection method ID given)
    subCreateSettingsView(testApId);
    
    subGetUiWidgets();
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCpPacketDataApPlugin::cleanupTestCase()
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
void TestCpPacketDataApPlugin::init()
{
    QTest::qWait(1000);
}

/**
 * This function is be called after each test case is executed.
 */
void TestCpPacketDataApPlugin::cleanup()
{
}

// -----------------------------------------------------------------------------
// TEST CASES
// -----------------------------------------------------------------------------

/**
 * Tests changing of connection name.
 */
void TestCpPacketDataApPlugin::tcChangeConnectionName()
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
void TestCpPacketDataApPlugin::tcChangeConnectionName_data()
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
        << "test packet AP"
        << "test packet AP";
}

/**
 * Tests that empty connection name is not accepted.
 */
void TestCpPacketDataApPlugin::tcConnectionNameEmpty()
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
 * Tests changing of access point name.
 */
void TestCpPacketDataApPlugin::tcChangeAccessPointName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mAccessPointNameWidget);
    
    // Erase old string
    QString text = mTestView->mAccessPointNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mAccessPointNameWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataAPName,
        mTestView->mAccessPointNameItem,
        result);
}

/**
 * Test data for access point name change test case.
 */
void TestCpPacketDataApPlugin::tcChangeAccessPointName_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
    
    QTest::newRow("maximum length")
        << "really long name 12345678901234567890123456789012345678901234567890123456789012345678901234567890123"
        << "really long name 12345678901234567890123456789012345678901234567890123456789012345678901234567890123";
    QTest::newRow("too long")
        << "too long name 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345"
        << "too long name 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456";
    QTest::newRow("basic") // last one must always fit on one line in UI
        << "test AP name"
        << "test AP name";
}

/**
 * Tests that empty access point name is not accepted.
 */
void TestCpPacketDataApPlugin::tcAccessPointNameEmpty()
{
    QString previous = 
        mTestView->mAccessPointNameItem->contentWidgetData("text").toString();
    
    HbAutoTest::mouseClick(mMainWindow, mAccessPointNameWidget);
    
    // Erase old string
    QString text = mTestView->mAccessPointNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    HbAutoTest::mouseClick(mMainWindow, mAccessPointNameWidget, exitEditorOffset);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(
        mMainWindow,
        mTestView->mMessageBox.data(),
        messageBoxOkButtonOffset);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataAPName,
        mTestView->mAccessPointNameItem,
        previous);
}


/**
 * Tests handling of too long string from CommsDat (longer than UI accepts).
 */
void TestCpPacketDataApPlugin::tcTooLongUserNameInCommsDat()
{
    // Verify UI text, the widget contains the too long string, even though
    // it is not shown completely on UI
    QString text = mTestView->mUserNameItem->contentWidgetData("text").toString();
    QCOMPARE(tooLongUserName, text);

    QTest::qWait(1000);
    
    // Visit editing state, truncated string should be saved to CommsDat
    HbAutoTest::mouseClick(mMainWindow, mUserNameWidget);
    
    QTest::qWait(1000);
    
    HbAutoTest::mouseClick(mMainWindow, mUserNameWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataIFAuthName,
        mTestView->mUserNameItem,
        truncatedUserName);
}

/**
 * Tests changing of user name.
 */
void TestCpPacketDataApPlugin::tcChangeUserName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mUserNameWidget);
    
    // Erase old string
    QString text = mTestView->mUserNameItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mUserNameWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataIFAuthName,
        mTestView->mUserNameItem,
        result);
}

/**
 * Test data for user name change test case.
 */
void TestCpPacketDataApPlugin::tcChangeUserName_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("maximum length")
        << "really long name 123456789012345678901234567890123"
        << "really long name 123456789012345678901234567890123";
    QTest::newRow("too long")
        << "too long name 1234567890123456789012345678901234567890123"
        << "too long name 123456789012345678901234567890123456";
    QTest::newRow("basic")
        << "username"
        << "username";
    QTest::newRow("empty") // last one must always fit on one line in UI
        << ""
        << "";
}

/**
 * Scrolls the tested view to the bottom.
 */
void TestCpPacketDataApPlugin::tcScrollToBottom()
{
    subScrollToBottom();
}

/**
 * Tests "prompt" password checkbox.
 */
void TestCpPacketDataApPlugin::tcChangePromptPassword()
{
    // Ensure prompt for password is unchecked
    bool prompt = subGetBool(CMManagerShim::PacketDataIFPromptForAuth);
    if (prompt) {
        // Disable prompt for password
        HbAutoTest::mouseClick(mMainWindow, mPasswordPromptWidget);
    }
    
    // Enable prompt for password and verify
    HbAutoTest::mouseClick(mMainWindow, mPasswordPromptWidget);
    subVerifyBool(
        CMManagerShim::PacketDataIFPromptForAuth,
        true);
    
    // Verify that password lineedit is disabled, following steps will
    // fail if editing is allowed
    HbAutoTest::mouseClick(mMainWindow, mPasswordWidget);
    QTest::qWait(waitTime);

    // Disable prompt for password and verify
    HbAutoTest::mouseClick(mMainWindow, mPasswordPromptWidget);
    subVerifyBool(
        CMManagerShim::PacketDataIFPromptForAuth,
        false);
}

/**
 * Tests changing of password.
 */
void TestCpPacketDataApPlugin::tcChangePassword()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mPasswordWidget);
    
    // Erase old string
    QString text = mTestView->mPasswordItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mPasswordWidget, exitEditorOffset);

    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataIFAuthPass,
        mTestView->mPasswordItem,
        result);
}

/**
 * Test data for password change test case.
 */
void TestCpPacketDataApPlugin::tcChangePassword_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("maximum length")
        << "really long name 123456789012345678901234567890123"
        << "really long name 123456789012345678901234567890123";
    QTest::newRow("too long")
        << "too long name 1234567890123456789012345678901234567890123"
        << "too long name 123456789012345678901234567890123456";
    QTest::newRow("basic")
        << "password"
        << "password";
    QTest::newRow("empty") // last one must always fit on one line in UI
        << ""
        << "";
}

/**
 * Tests changing of authentication mode.
 */
void TestCpPacketDataApPlugin::tcChangeAuthenticationMode()
{
    QPointF normalPointOffset = comboBoxFirstItemOffset;
    QPointF securePointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    
    // Set authentication mode to secure
    HbAutoTest::mouseClick(mMainWindow, mAuthenticationWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mAuthenticationWidget, securePointOffset);

    subVerifyBool(
        CMManagerShim::PacketDataDisablePlainTextAuth,
        true);
    
    // Set authentication mode to normal
    HbAutoTest::mouseClick(mMainWindow, mAuthenticationWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mAuthenticationWidget, normalPointOffset);

    subVerifyBool(
        CMManagerShim::PacketDataDisablePlainTextAuth,
        false);
}

/**
 * Tests changing of homepage.
 */
void TestCpPacketDataApPlugin::tcChangeHomepage()
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
void TestCpPacketDataApPlugin::tcChangeHomepage_data()
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
void TestCpPacketDataApPlugin::tcOpenAdvancedSettingsView()
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
    
    mTestViewAdvanced = static_cast<CpPacketDataApAdvancedView *>(mMainWindow->currentView());
}

/**
 * Expands the IP settings group.
 */
void TestCpPacketDataApPlugin::tcExpandIpSettings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipGroup);
    
    subGetAdvancedUiWidgets(0);
}

/**
 * Sets the network type to IPv4 and gets UI widget pointers.
 */
void TestCpPacketDataApPlugin::tcSetIpv4NetworkType()
{
    QPointF ipv4PointOffset = comboBoxFirstItemOffset;
    
    // Set network type to IPv4
    HbAutoTest::mouseClick(mMainWindow, mNetworkTypeWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkTypeWidget, ipv4PointOffset, 100);
    subVerifyUint(CMManagerShim::PacketDataPDPType, RPacketContext::EPdpTypeIPv4);
    
    subGetAdvancedUiWidgets(0);
}

/**
 * Tests enabling of automatic phone IP address.
 */
void TestCpPacketDataApPlugin::tcEnableAutomaticIpv4Address()
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
        CMManagerShim::PacketDataIPAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv4AddressWidget);
}

/**
 * Tests disabling of automatic phone IP address.
 */
void TestCpPacketDataApPlugin::tcEnableUserDefinedIpv4Address()
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
void TestCpPacketDataApPlugin::tcChangeIpAddress()
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
        CMManagerShim::PacketDataIPAddr,
        mTestViewAdvanced->mIpv4Address,
        result);
    subVerifyBool(
        CMManagerShim::PacketDataIPAddrFromServer,
        false);
}

/**
 * Test data for IP address change test case.
 */
void TestCpPacketDataApPlugin::tcChangeIpAddress_data()
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
 * Scrolls the tested view to the bottom.
 */
void TestCpPacketDataApPlugin::tcScrollToBottom2()
{
    subScrollToBottom();
    
    subGetAdvancedUiWidgets(0);
}

/**
 * Tests enabling of automatic IPv4 DNS addresses.
 */
void TestCpPacketDataApPlugin::tcEnableAutomaticIpv4DnsAddress()
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
        CMManagerShim::PacketDataIPDNSAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv4PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv4SecondaryDnsAddressWidget);
}

/**
 * Tests disabling of automatic IPv4 DNS addresses.
 */
void TestCpPacketDataApPlugin::tcEnableUserDefinedIpv4DnsAddress()
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
void TestCpPacketDataApPlugin::tcChangeIpv4DnsAddress()
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
        CMManagerShim::PacketDataIPNameServer1,
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
        CMManagerShim::PacketDataIPNameServer2,
        mTestViewAdvanced->mIpv4DnsAddress2,
        result);
    
    // Verify user defined address is in use
    subVerifyBool(
        CMManagerShim::PacketDataIPDNSAddrFromServer,
        false);
}

/**
 * Test data for IPv4 DNS address change test case.
 */
void TestCpPacketDataApPlugin::tcChangeIpv4DnsAddress_data()
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
void TestCpPacketDataApPlugin::tcInvalidIpv4DnsAddress()
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
        CMManagerShim::PacketDataIPNameServer1,
        mTestViewAdvanced->mIpv4DnsAddress1,
        previous);
}

/**
 * Scrolls the tested view to the top.
 */
void TestCpPacketDataApPlugin::tcScrollToTop()
{
    subScrollToTop();
}

/**
 * Sets the network type to IPv6 and gets UI widget pointers.
 */
void TestCpPacketDataApPlugin::tcSetIpv6NetworkType()
{
    QPointF ipv6PointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    
    // Set network type to IPv6
    HbAutoTest::mouseClick(mMainWindow, mNetworkTypeWidget);
    HbAutoTest::mouseClick(mMainWindow, mNetworkTypeWidget, ipv6PointOffset, 100);
    subVerifyUint(CMManagerShim::PacketDataPDPType, RPacketContext::EPdpTypeIPv6);

    subGetAdvancedUiWidgets(0);
}

/**
 * Tests enabling of automatic IPv6 DNS addresses.
 */
void TestCpPacketDataApPlugin::tcEnableAutomaticIpv6DnsAddress()
{
    // Enable automatic IPv6 DNS address
    QPointF automaticPointOffset = comboBoxFirstItemOffset;
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget, automaticPointOffset, 100);
    QTest::qWait(100);
    subVerifyBool(
        CMManagerShim::PacketDataIPIP6DNSAddrFromServer,
        true);
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget);
}

/**
 * Tests enabling of well-known IPv6 DNS addresses.
 */
void TestCpPacketDataApPlugin::tcEnableWellKnownIpv6DnsAddress()
{
    // Enable well-known IPv6 DNS address
    QPointF wellKnownPointOffset = comboBoxFirstItemOffset + comboBoxItemOffset;
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mIpv6DnsAddressAutomaticWidget, wellKnownPointOffset, 100);
    QTest::qWait(100);
    subVerifyBool(
        CMManagerShim::PacketDataIPIP6DNSAddrFromServer,
        false);
    subVerifyString(
        CMManagerShim::PacketDataIPIP6NameServer1,
        mTestViewAdvanced->mIpv6DnsAddress1,
        "fec0:000:0000:ffff::1");
    subVerifyString(
        CMManagerShim::PacketDataIPIP6NameServer2,
        mTestViewAdvanced->mIpv6DnsAddress2,
        "fec0:000:0000:ffff::2");
    
    // Ensure that editing the IP address is not allowed
    HbAutoTest::mouseClick(mMainWindow, mIpv6PrimaryDnsAddressWidget);
    HbAutoTest::mouseClick(mMainWindow, mIpv6SecondaryDnsAddressWidget);
}

/**
 * Tests enabling of user defined IPv6 DNS addresses.
 */
void TestCpPacketDataApPlugin::tcEnableUserDefinedIpv6DnsAddress()
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
void TestCpPacketDataApPlugin::tcChangeIpv6DnsAddress()
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
        CMManagerShim::PacketDataIPIP6NameServer1,
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
        CMManagerShim::PacketDataIPIP6NameServer2,
        mTestViewAdvanced->mIpv6DnsAddress2,
        result);
    
    // Verify user defined address is in use
    subVerifyBool(
        CMManagerShim::PacketDataIPIP6DNSAddrFromServer,
        false);
}

/**
 * Test data for IPv6 DNS address change test case.
 */
void TestCpPacketDataApPlugin::tcChangeIpv6DnsAddress_data()
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
void TestCpPacketDataApPlugin::tcInvalidIpv6DnsAddress()
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
        CMManagerShim::PacketDataIPIP6NameServer1,
        mTestViewAdvanced->mIpv6DnsAddress1,
        previous);
}

/**
 * Collapses the IP settings group.
 */
void TestCpPacketDataApPlugin::tcCollapseIpSettings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, ipGroup);
}

/**
 * Expands the proxy settings group and gets UI widget pointers.
 */
void TestCpPacketDataApPlugin::tcExpandProxySettingsAndGetUiWidgets()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, proxyGroup);

    QTest::qWait(500);
    
    subGetAdvancedUiWidgets(1);
}

/**
 * Tests changing of proxy server address.
 */
void TestCpPacketDataApPlugin::tcChangeProxyServerAddress()
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
void TestCpPacketDataApPlugin::tcChangeProxyServerAddress_data()
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
void TestCpPacketDataApPlugin::tcChangeProxyPortNumber()
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
void TestCpPacketDataApPlugin::tcChangeProxyPortNumber_data()
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
void TestCpPacketDataApPlugin::tcInvalidProxyPortNumber()
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
void TestCpPacketDataApPlugin::tcCollapseProxySettings()
{
    HbAutoTest::mouseClick(mMainWindow, mTestViewAdvanced, proxyGroup);
}

/**
 * Returns from advanced settings view.
 */
void TestCpPacketDataApPlugin::tcCloseAdvancedSettingsView()
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
void TestCpPacketDataApPlugin::subGetUiWidgets()
{
    HbModelIterator iterator(mTestView->mModel);
    
    // Get "Access point settings" group item
    QModelIndex apGroupIndex = iterator.index(0);
    
    // Get UI widgets
    mConnectionNameWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(0, apGroupIndex));
    mAccessPointNameWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(1, apGroupIndex));
    mUserNameWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(2, apGroupIndex));
    mPasswordPromptWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(3, apGroupIndex));
    mPasswordWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(4, apGroupIndex));
    mAuthenticationWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(5, apGroupIndex));
    mHomepageWidget = subGetWidgetByIndex(
        mTestView->mForm,
        iterator.index(6, apGroupIndex));
}

/**
 * Gets advanced settings view UI widget pointers by group index.
 */
void TestCpPacketDataApPlugin::subGetAdvancedUiWidgets(
    uint index)
{
    // Get the group item
    HbModelIterator iterator(mTestViewAdvanced->mModel);
    QModelIndex groupIndex = iterator.index(index);
    
    // Get UI widgets
    if (index == 0) {
        // IP settings group
        mNetworkTypeWidget = subGetWidgetByIndex(
            mTestViewAdvanced->mForm,
            iterator.index(0, groupIndex));
        if (qobject_cast<HbComboBox *>(mNetworkTypeWidget)->currentIndex() == 0) {
            // IPv4
            mIpv4AddressAutomaticWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(1, groupIndex));
            mIpv4AddressWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(2, groupIndex));
            mIpv4DnsAddressAutomaticWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(3, groupIndex));
            mIpv4PrimaryDnsAddressWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(4, groupIndex));
            mIpv4SecondaryDnsAddressWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(5, groupIndex));
        } else {
            // IPv6
            mIpv6DnsAddressAutomaticWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(1, groupIndex));
            mIpv6PrimaryDnsAddressWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(2, groupIndex));
            mIpv6SecondaryDnsAddressWidget = subGetWidgetByIndex(
                mTestViewAdvanced->mForm,
                iterator.index(3, groupIndex));
        }
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
HbWidget *TestCpPacketDataApPlugin::subGetWidgetByIndex(
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
 * Writes initial settings to CommsDat needed by some test cases.
 */
void TestCpPacketDataApPlugin::subInitializeCommsDat()
{
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    
    // Initial settings
    connectionMethod->setStringAttribute(
        CMManagerShim::PacketDataIFAuthName,
        tooLongUserName);
    
    connectionMethod->update();
}

/**
 * Creates the settings view and shows it.
 */
void TestCpPacketDataApPlugin::subCreateSettingsView(uint connectionMethodId)
{
    // Create settings view
    HbView *view = mPlugin->createSettingView(connectionMethodId);
    QVERIFY(view != NULL);
    
    // Display the view
    mMainWindow->addView(view);
    mMainWindow->setCurrentView(view);
    // Store pointer to settings view class
    mTestView = static_cast<CpPacketDataApView *>(view);    
}

/**
 * Verifies that given string is correctly stored in CommsDat and shown on UI. 
 */
void TestCpPacketDataApPlugin::subVerifyString(
    CMManagerShim::ConnectionMethodAttribute attribute,
    HbDataFormModelItem *item,
    QString expected)
{
    QTest::qWait(waitTime);

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
 * Clears a HbLineEdit.
 */
void TestCpPacketDataApPlugin::subClearLineEdit(
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
 * Gets value of a boolean attribute from CommsDat.
 */
bool TestCpPacketDataApPlugin::subGetBool(
    CMManagerShim::ConnectionMethodAttribute attribute)
{
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    return connectionMethod->getBoolAttribute(attribute);
}

/**
 * Verifies that given attribute contains expected boolean value in CommsDat. 
 */
void TestCpPacketDataApPlugin::subVerifyBool(
    CMManagerShim::ConnectionMethodAttribute attribute,
    bool expected)
{
    QTest::qWait(waitTime);
    
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    bool commsdat = connectionMethod->getBoolAttribute(attribute);
    
    QCOMPARE(commsdat, expected);
}

/**
 * Verifies that given attribute contains expected integer value in CommsDat. 
 */
void TestCpPacketDataApPlugin::subVerifyUint(
    CMManagerShim::ConnectionMethodAttribute attribute,
    uint expected)
{
    QTest::qWait(waitTime);

    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    uint commsdat = connectionMethod->getIntAttribute(attribute);
    
    QCOMPARE(commsdat, expected);
}

/**
 * Clicks a widget currently on UI by class name.
 */
void TestCpPacketDataApPlugin::subClickWidget(const QString &name)
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
 * Scrolls the tested view to the bottom.
 */
void TestCpPacketDataApPlugin::subScrollToBottom()
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
void TestCpPacketDataApPlugin::subScrollToTop()
{
    // Scroll to the top of the view
    HbAutoTest::mousePress(mMainWindow, mTestView, scrollMiddle);
    QTest::qWait(1000);
    HbAutoTest::mouseMove(mMainWindow, mTestView, scrollBottom);
    HbAutoTest::mouseRelease(mMainWindow, mTestView, scrollBottom);
}
