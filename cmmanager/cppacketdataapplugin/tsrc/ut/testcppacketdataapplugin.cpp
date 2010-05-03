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
#include <QtTest/QtTest>
#include <cpbearerapplugininterface.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

#include "cppacketdataapview.h"

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
static const QPoint sideTop(350, 60);
static const QPoint scrollStart(350, 300);
static const QPoint scrollStop(350, 240);

static const QPoint messageBoxOkButton(170, 320);

// These are measured when view is scrolled to top
static const QPoint connectionNameLineEdit(330, 110);

static const QPoint accessPointNameLineEdit(330, 190);

static const QPoint userNameLineEdit(330, 265);

// These are measured when view is scrolled to bottom
static const QPoint passwordPromptCheckbox(50, 295);
static const QPoint passwordLineEdit(330, 380);

static const QPoint authenticationComboBox(175, 470);
static const QPoint authenticationSecure(100, 420);
static const QPoint authenticationNormal(100, 365);

static const QPoint homepageLineEdit(330, 555);

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
    QPluginLoader loader(dir.absoluteFilePath(pluginName));
    mPlugin = qobject_cast<CpBearerApPluginInterface *>(loader.instance());
    QVERIFY(mPlugin != NULL);
    
    // Verify plugin bearer type
    QVERIFY(mPlugin->bearerType() == CMManagerShim::BearerTypePacketData);
    
    // Create packet data settings view (connection method ID given)
    subCreateSettingsView(testApId);
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCpPacketDataApPlugin::cleanupTestCase()
{
    delete mMainWindow;
    mMainWindow = 0;
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::CmNameLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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
        << "really long name 1234567890123"
        << "really long name 1234567890123";
    QTest::newRow("too long")
        << "too long name 123456789012345678901234567890"
        << "too long name 1234567890123456";
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, connectionNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::CmNameLength);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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
 * Tests changing of access point name.
 */
void TestCpPacketDataApPlugin::tcChangeAccessPointName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, accessPointNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::PacketDataAPNameLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, accessPointNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::PacketDataAPNameLength);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

    QTest::qWait(100);
    // Dismiss messagebox
    HbAutoTest::mouseClick(mMainWindow, mTestView, messageBoxOkButton);
    
    // Verify both commsdat and UI widget
    subVerifyString(
        CMManagerShim::PacketDataAPName,
        mTestView->mAccessPointNameItem,
        previous);
}

/**
 * Tests changing of user name.
 */
void TestCpPacketDataApPlugin::tcChangeUserName()
{
    QFETCH(QString, string);
    QFETCH(QString, result);
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, userNameLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::PacketDataIFAuthNameLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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

// Long strings don't work, Orbit bug? Screen goes blank
//    QTest::newRow("maximum length")
//        << "really long name 123456789012345678901234567890123"
//        << "really long name 123456789012345678901234567890123";
//    QTest::newRow("too long")
//        << "too long name 1234567890123456789012345678901234567890123"
//        << "too long name 123456789012345678901234567890123456";
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
    // Scroll to the bottom of the view
    HbAutoTest::mousePress(mMainWindow, mTestView, scrollStart);
    QTest::qWait(500);
    HbAutoTest::mouseMove(mMainWindow, mTestView, scrollStop);
    HbAutoTest::mouseRelease(mMainWindow, mTestView, scrollStop);
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
        HbAutoTest::mouseClick(
            mMainWindow,
            mTestView,
            passwordPromptCheckbox);
    }
    
    // Enable prompt for password and verify
    HbAutoTest::mouseClick(mMainWindow, mTestView, passwordPromptCheckbox);
    subVerifyBool(
        CMManagerShim::PacketDataIFPromptForAuth,
        true);
    
    // Verify that password lineedit is disabled, following steps will
    // fail if editing is allowed
    HbAutoTest::mouseClick(mMainWindow, mTestView, passwordLineEdit);
    QTest::qWait(waitTime);

    // Disable prompt for password and verify
    HbAutoTest::mouseClick(mMainWindow, mTestView, passwordPromptCheckbox);
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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, passwordLineEdit);
    
    // Erase old string
    subClearLineEdit(CMManagerShim::PacketDataIFAuthPassLength);
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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

// Long strings don't work, Orbit bug? Screen goes blank
//    QTest::newRow("maximum length")
//        << "really long name 123456789012345678901234567890123"
//        << "really long name 123456789012345678901234567890123";
//    QTest::newRow("too long")
//        << "too long name 1234567890123456789012345678901234567890123"
//        << "too long name 123456789012345678901234567890123456";
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
    // Set authentication mode to secure
    HbAutoTest::mouseClick(mMainWindow, mTestView, authenticationComboBox, 100);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, authenticationSecure, 100);

    subVerifyBool(
        CMManagerShim::PacketDataDisablePlainTextAuth,
        true);
    
    // Set authentication mode to normal
    HbAutoTest::mouseClick(mMainWindow, mTestView, authenticationComboBox, 100);
    QTest::qWait(100);
    HbAutoTest::mouseClick(mMainWindow, mTestView, authenticationNormal, 100);

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
    
    HbAutoTest::mouseClick(mMainWindow, mTestView, homepageLineEdit);
    
    // Erase old string
    QString text = mTestView->mHomepageItem->contentWidgetData("text").toString();
    subClearLineEdit(text.size());
    
    // Enter new string
    HbAutoTest::keyClicks(mMainWindow, string, 0, waitTime);

    HbAutoTest::mouseClick(mMainWindow, mTestView, sideTop);

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
void TestCpPacketDataApPlugin::tcAdvancedSettings()
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
    // Read attribute value from CommsDat
    QScopedPointer<CmManagerShim> cmManager(new CmManagerShim);
    QScopedPointer<CmConnectionMethodShim> connectionMethod( 
        cmManager->connectionMethod(testApId));
    bool commsdat = connectionMethod->getBoolAttribute(attribute);
    
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
