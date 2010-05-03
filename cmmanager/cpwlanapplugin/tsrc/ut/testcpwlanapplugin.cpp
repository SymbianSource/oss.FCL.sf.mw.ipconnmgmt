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
#include <QtTest/QtTest>
#include <cpbearerapplugininterface.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

#include "cpwlanapview.h"

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
static const QPoint connectionNameLabel(175, 70);

static const QPoint connectionNameLineEdit(330, 110);

static const QPoint wlanNetworkNameLineEdit(330, 190);

static const QPoint networkStatusComboBox(175, 270);
static const QPoint networkStatusPublic(175, 325);
static const QPoint networkStatusHidden(175, 375);

static const QPoint networkModeComboBox(175, 365);
static const QPoint networkModeIntrastructure(175, 415);
static const QPoint networkModeAdHoc(175, 465);

static const QPoint securityModeComboBox(175, 460);
#ifndef WLAN_SECURITY_PLUGINS_AVAILABLE
static const QPoint securityModeOpen(175, 510);
#else
static const QPoint securityModeOpen(175, 260);
static const QPoint securityModeWep(175, 305);
static const QPoint securityModeWpaWpa2(175, 355);
static const QPoint securityModeWpa2(175, 405);
#endif

static const QPoint homepageLineEdit(330, 545);

static const QPoint messageBoxOkButton(170, 320);

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
    QPluginLoader loader(dir.absoluteFilePath(pluginName));
    mPlugin = qobject_cast<CpBearerApPluginInterface *>(loader.instance());
    QVERIFY(mPlugin != NULL);
    
    // Verify plugin bearer type
    QVERIFY(mPlugin->bearerType() == CMManagerShim::BearerTypeWlan);
    
    // Create WLAN settings view (connection method ID given)
    subCreateSettingsView(testApId);
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCpWlanApPlugin::cleanupTestCase()
{
    delete mMainWindow;
    mMainWindow = 0;
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::CmNameLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLabel);

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
        << "really long name 1234567890123"
        << "really long name 1234567890123";
    QTest::newRow("too long")
        << "too long name 123456789012345678901234567890"
        << "too long name 1234567890123456";
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLineEdit);    
    // Erase old string
    subClearLineEdit(CMManagerShim::CmNameLength);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLabel);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(mMainWindow, mTestView, messageBoxOkButton);
    
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, wlanNetworkNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::WlanSSIDLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLabel);

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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, wlanNetworkNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::WlanSSIDLength);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLabel);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(mMainWindow, mTestView, messageBoxOkButton);
    
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
    // Set network status to hidden
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkStatusComboBox, 100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkStatusHidden, 100);
    subVerifyNetworkStatus(HiddenStatus);

    // Set network status to public
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkStatusComboBox, 100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkStatusPublic, 100);
    subVerifyNetworkStatus(PublicStatus);
}

/**
 * Tests changing of WLAN network mode.
 */
void TestCpWlanApPlugin::tcChangeNetworkMode()
{
    // Set network mode to ad-hoc
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkModeComboBox, 100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkModeAdHoc, 100);
    subVerifyUint(CMManagerShim::WlanConnectionMode, CMManagerShim::Adhoc);

    // Set network mode to infrastructure
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkModeComboBox, 100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, networkModeIntrastructure, 100);
    subVerifyUint(CMManagerShim::WlanConnectionMode, CMManagerShim::Infra);
}

/**
 * Tests changing of WLAN security mode.
 */
void TestCpWlanApPlugin::tcChangeSecurityMode()
{
#ifdef WLAN_SECURITY_PLUGINS_AVAILABLE
    // Ensure security mode is open
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeComboBox, 100);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeOpen, 100);
    subVerifyUint(
        CMManagerShim::WlanSecurityMode,
        CMManagerShim::WlanSecModeOpen);

    QTest::qWait(1000);

    // Set security mode to WEP
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeComboBox, 100);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeWep, 100);
    subVerifyUint(
        CMManagerShim::WlanSecurityMode,
        CMManagerShim::WlanSecModeWep);
    
    QTest::qWait(1000);
#endif
    
    // Set security mode to open
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeComboBox, 100);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, securityModeOpen, 100);
    subVerifyUint(
        CMManagerShim::WlanSecurityMode,
        CMManagerShim::WlanSecModeOpen);
}

/**
 * Tests changing of homepage.
 */
void TestCpWlanApPlugin::tcChangeHomepage()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, homepageLineEdit);
    
    // Erase old string
    QString text = mTestView->mHomepageItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLabel);

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
  
// Doesn't work always, view goes blank sometimes, Orbit bug?
//    QTest::newRow("long")
//        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html"
//        << "http://developer.symbian.org/main/documentation/reference/s^3/doc_source/AboutSymbianOSLibrary9.6/index.html";
    QTest::newRow("basic") // last one should always fit on one line in UI
        << "http://www.symbian.org/"
        << "http://www.symbian.org/";
    QTest::newRow("empty")
        << ""
        << "";
}

/**
 * Tests advanced settings view (which is currently empty).
 */
void TestCpWlanApPlugin::tcAdvancedSettings()
{
    // Launch advanced settings view
    bool status = connect(
        this,
        SIGNAL(menuActionTriggered(HbAction *)),
        mTestView,
        SLOT(menuActionTriggered(HbAction *)));
    Q_ASSERT(status);
    emit menuActionTriggered(mTestView->mAdvancedSettingsAction);

    QTest::qWait(2000);
    
    // Return from advanced settings view
    subClickWidget("HbNavigationButton");
}

// -----------------------------------------------------------------------------
// SUB TEST CASES
// -----------------------------------------------------------------------------

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
    // Erase old string
    QTest::qWait(5000); // TODO: Remove this when item specific menu doesn't pop up anymore

    // Move cursor to end of string
    //HbAutoTest::keyClick(mMainWindow, Qt::Key_End, 0, waitTime); // doesn't seem to do anything? 
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    HbAutoTest::keyClick(mMainWindow, Qt::Key_Down, 0, waitTime);
    for (int i=0; i<25; i++) {
        HbAutoTest::keyClick(mMainWindow, Qt::Key_Right, 0, waitTime);
    }
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
