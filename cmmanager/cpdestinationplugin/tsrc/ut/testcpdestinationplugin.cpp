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
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cpplugininterface.h>
#include <cppluginutility.h>
#include <cpitemdatahelper.h>
#include <cpsettingformitemdata.h>

//#include "cmapplsettingsui_p.h"
//#include "cmradiodialog.h"
//#include "cpdestinationplugin.h"

#include "hbautotest.h"
#include "testcpdestinationplugin.h"

// -----------------------------------------------------------------------------
// STATIC TEST DATA
// -----------------------------------------------------------------------------

static const QString pluginDir =
    "\\resource\\qt\\plugins\\controlpanel";

static const QString pluginName = "cpdestinationplugin.dll";

// Time to wait before continuing after an UI step
static const int waitTime = 10;

static const QString navigationButton = "HbNavigationButton";

// UI coordinates
static const QPoint destinationGroup(175,30);
static const QPoint internetDestination(175,100);
static const QPoint internetFirstAp(175,130);
static const QPoint internetFirstApMove(175,160);
static const QPoint internetFirstApDelete(175,210);
static const QPoint internetFirstApShare(175,260);

static const QPoint deleteDialogOk(100,300);

static const QPoint tbButton(175,550);
static const QPoint arrangeFirstAp(175,30);
static const QPoint arrangeDragEnd(175,140);

static const QPoint uncatDestination(175,455);
static const QPoint uncatAp(175,120);
static const QPoint uncatMenuMove(175,140);
static const QPoint uncatMenuDelete(175,180);

static const QPoint destDialogOk(100,370);
static const QPoint destDialogSecond(100,255);
static const QPoint destDialogThird(100,315);
static const QPoint destDialogNewDest(100,345);

static const QPoint addDestination(175,410);
static const QPoint addDialog(175,235);
static const QPoint addDialogOk(100,300);
static const QPoint newDestinationRename(175,445);
static const QPoint newDestinationDelete(175,490);

static const QPoint addDialogKb(175,155);
static const QPoint addDialogOkKb(100,225);

static const QPoint secondDestination(175,195);
static const QPoint thirdDestination(175,275);
static const QPoint fourthDestination(175,355);



#if 0
// Destination list item for Connection Method selection
static const QString dedicatedAccessPoint = "Dedicated access point";

static const QStringList allDestinations = QStringList() <<
    "Internet" <<
    "My Snap" <<
    dedicatedAccessPoint;

static const QStringList allConnectionMethods = QStringList()
    << "Home WLAN"
    << "packet data 1"
    << "packet data 2"
    << "packet data 3"
    << "Streaming"
    << "WLAN IAP 1"
    << "WLAN IAP 2"
    << "WLAN IAP 3";

static const QStringList gprsConnectionMethods = QStringList()
    << "packet data 1"
    << "packet data 2"
    << "packet data 3"
    << "Streaming";

static const QStringList wlanConnectionMethods = QStringList()
    << "Home WLAN"
    << "WLAN IAP 1"
    << "WLAN IAP 2"
    << "WLAN IAP 3";
#endif

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
    app.setApplicationName("TestCpDestinationPlugin");
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestCpDestinationPlugin.txt";
 
    TestCpDestinationPlugin tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}
#else
QTEST_MAIN(TestCpDestinationPlugin)
#endif

/**
 * This function is be called before the first test case is executed.
 */
void TestCpDestinationPlugin::initTestCase()
{
    mMainWindow = new HbAutoTestMainWindow;
    mMainWindow->show();
    
    // Load plugin
    QDir dir(pluginDir);
    QPluginLoader loader(dir.absoluteFilePath(pluginName));
    mPlugin = QSharedPointer<CpPluginInterface>
        (qobject_cast<CpPluginInterface *>(loader.instance()));
    //QVERIFY(mPlugin != NULL);

    
    // Create destinations view
    subCreateSettingsView();
    
    HbAutoTest::mouseClick(mMainWindow, mView, destinationGroup, waitTime);
    QTest::qWait(200);
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCpDestinationPlugin::cleanupTestCase()
{
    delete mMainWindow;
    mMainWindow = 0;
}

/**
 * This function is be called before each test case is executed.
 */
void TestCpDestinationPlugin::init()
{
    // Expand settings group
   // mTestView->mForm->setExpanded(
   //     mTestView->mModel->indexFromItem(mTestView->mApSettingsGroupItem),
    //    true);
}

/**
 * This function is be called after each test case is executed.
 */
void TestCpDestinationPlugin::cleanup()
{

}

// -----------------------------------------------------------------------------
// TEST CASES
// -----------------------------------------------------------------------------
void TestCpDestinationPlugin::tcArrangeAps()
{   
    // Open Internet destination
    HbAutoTest::mouseClick(mMainWindow, mView, internetDestination, waitTime);
    QTest::qWait(500);
    
    // Activate arrange mode
    HbAutoTest::mouseClick(mMainWindow, mView, tbButton, waitTime);
    QTest::qWait(500);
    
    // Drag
    HbAutoTest::mousePress(mMainWindow, mView, arrangeFirstAp, waitTime);
    QTest::qWait(300);
    HbAutoTest::mouseMove(mMainWindow, mView, arrangeDragEnd, waitTime);
    QTest::qWait(300);
    HbAutoTest::mouseRelease(mMainWindow, mView, arrangeDragEnd, waitTime);
    
    HbAutoTest::mouseClick(mMainWindow, mView, arrangeDragEnd, waitTime);
    QTest::qWait(500);
    
    // Cancel
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
    // Activate arrange mode again
    HbAutoTest::mouseClick(mMainWindow, mView, tbButton, waitTime);
    QTest::qWait(500);
    
    // Drag
    HbAutoTest::mousePress(mMainWindow, mView, arrangeFirstAp, waitTime);
    QTest::qWait(300);
    HbAutoTest::mouseMove(mMainWindow, mView, arrangeDragEnd, waitTime);
    QTest::qWait(300);
    // HbAutoTest::mouseRelease(mMainWindow, mView, arrangeDragEnd, waitTime);
    
    HbAutoTest::mouseClick(mMainWindow, mView, arrangeDragEnd, waitTime);
    QTest::qWait(500);
    
    // OK
    HbAutoTest::mouseClick(mMainWindow, mView, tbButton, waitTime);
    QTest::qWait(500);
    
    // Back to main view
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
}

void TestCpDestinationPlugin::tcMoveAp()
{   
    // Open uncategorised aps
    HbAutoTest::mouseClick(mMainWindow, mView, uncatDestination, waitTime);
    QTest::qWait(500);
    
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, uncatAp, 700);
    QTest::qWait(50);
    
    // Select move
    HbAutoTest::mouseClick(mMainWindow, mView, uncatMenuMove, waitTime);
    QTest::qWait(300);
    
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogOk, waitTime);
    QTest::qWait(2000);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcShareDeleteAp()
{
    // Open Internet destination
    HbAutoTest::mouseClick(mMainWindow, mView, internetDestination, waitTime);
    QTest::qWait(500);
      
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstAp, 700);
    QTest::qWait(50);
      
    // Select share
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstApShare, waitTime);
    QTest::qWait(300);
      
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogOk, waitTime);
    QTest::qWait(2000);
    
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstAp, 700);
    QTest::qWait(50);
          
    // Select share
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstApDelete, waitTime);
    QTest::qWait(300);
          
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, deleteDialogOk, waitTime);
    QTest::qWait(2000);
      
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcAddDestination()
{
    // Add Destination
    HbAutoTest::mouseClick(mMainWindow, mView, addDestination, waitTime);
    QTest::qWait(500);
      
    // Click OK -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Click OK -> errorNote disappears
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Activate keyboard
    HbAutoTest::mouseClick(mMainWindow, mView, addDialog, waitTime);
    QTest::qWait(500);
        
    // Activate line-edit
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogKb, waitTime);
    QTest::qWait(500);
        
    // Input text
    HbAutoTest::keyClicks(mMainWindow, "Internet", 0, waitTime);
    QTest::qWait(2000);
          
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOkKb, waitTime);
    QTest::qWait(500);
        
    // Select ok again -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
        
    // Click OK -> errorNote disappears
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Activate keyboard
    HbAutoTest::mouseClick(mMainWindow, mView, addDialog, waitTime);
    QTest::qWait(500);
    
    // Activate line-edit
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogKb, waitTime);
    QTest::qWait(500);
    
    // Input text
    HbAutoTest::keyClicks(mMainWindow, "foo", 0, waitTime);
    QTest::qWait(2000);
      
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOkKb, waitTime);
    QTest::qWait(500);
    
    // Select ok again
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcDestinationRename()
{
    // Open context menu
    HbAutoTest::mouseClick(mMainWindow, mView, addDestination, 700);
    QTest::qWait(50);
      
    // Click OK -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, newDestinationRename, waitTime);
    QTest::qWait(300);
    
    // Click OK -> error note shown
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(300);
    
    // Click OK -> errorNote disappears
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Activate keyboard
    HbAutoTest::mouseClick(mMainWindow, mView, addDialog, waitTime);
    QTest::qWait(500);
    
    // Activate line-edit
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogKb, waitTime);
    QTest::qWait(500);
    
    // Input text
    HbAutoTest::keyClicks(mMainWindow, "Internet", 0, waitTime);
    QTest::qWait(2000);
      
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOkKb, waitTime);
    QTest::qWait(500);
    
    // Select ok again -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Click OK -> errorNote disappears
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
        
    // Activate keyboard
    HbAutoTest::mouseClick(mMainWindow, mView, addDialog, waitTime);
    QTest::qWait(500);
        
    // Activate line-edit
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogKb, waitTime);
    QTest::qWait(500);
        
    // Input text
    HbAutoTest::keyClicks(mMainWindow, "bar", 0, waitTime);
    QTest::qWait(2000);
          
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOkKb, waitTime);
    QTest::qWait(500);
        
    // Select ok again
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // TODO: remove
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcDestinationDelete()
{
    // Open context menu
    HbAutoTest::mouseClick(mMainWindow, mView, addDestination, 700);
    QTest::qWait(50);
      
    // Click OK -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, newDestinationDelete, waitTime);
    QTest::qWait(300);
    
    // Click OK -> error note shown
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(1000);  
  
    // TODO: remove
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcCreateAndDeleteDestination()
{
    // Add Destination
    HbAutoTest::mouseClick(mMainWindow, mView, addDestination, waitTime);
    QTest::qWait(500);
    
    // Activate keyboard
    HbAutoTest::mouseClick(mMainWindow, mView, addDialog, waitTime);
    QTest::qWait(500);
    
    // Activate line-edit
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogKb, waitTime);
    QTest::qWait(500);
    
    // Input text
    HbAutoTest::keyClicks(mMainWindow, "foo", 0, waitTime);
    QTest::qWait(2000);
      
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOkKb, waitTime);
    QTest::qWait(500);
    
    // Select ok again
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(500);
    
    // Open Internet destination
    HbAutoTest::mouseClick(mMainWindow, mView, internetDestination, waitTime);
    QTest::qWait(500);
          
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstAp, 700);
    QTest::qWait(50);
          
    // Select move
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstApMove, waitTime);
    QTest::qWait(300);
    
    // Select destination
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogNewDest, waitTime);
    QTest::qWait(300);
    
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogOk, waitTime);
    QTest::qWait(2000);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
    // Open context menu
    HbAutoTest::mouseClick(mMainWindow, mView, addDestination, 700);
    QTest::qWait(50);
      
    // Click OK -> Error note
    HbAutoTest::mouseClick(mMainWindow, mView, newDestinationDelete, waitTime);
    QTest::qWait(300);
    
    // Click OK -> error note shown
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(1000);  
  
    // TODO: remove
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcOpenDestinations()
{
    // Open uncategorised aps
    HbAutoTest::mouseClick(mMainWindow, mView, secondDestination, waitTime);
    QTest::qWait(500);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
    // Open uncategorised aps
    HbAutoTest::mouseClick(mMainWindow, mView, thirdDestination, waitTime);
    QTest::qWait(500);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
    // Open uncategorised aps
    HbAutoTest::mouseClick(mMainWindow, mView, fourthDestination, waitTime);
    QTest::qWait(500);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
    
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, uncatAp, 700);
    QTest::qWait(50);
    
    // TODO: remove
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcDeleteUncat()
{   
    // Open uncategorised aps
    HbAutoTest::mouseClick(mMainWindow, mView, uncatDestination, waitTime);
    QTest::qWait(500);
    
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, uncatAp, 700);
    QTest::qWait(50);
    
    // Select move
    HbAutoTest::mouseClick(mMainWindow, mView, uncatMenuDelete, waitTime);
    QTest::qWait(500);
    
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(2000);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

void TestCpDestinationPlugin::tcMoveProtected()
{   
    // Open Internet destination
    HbAutoTest::mouseClick(mMainWindow, mView, internetDestination, waitTime);
    QTest::qWait(500);
          
    // Activate context menu
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstAp, 700);
    QTest::qWait(50);
          
    // Select move
    HbAutoTest::mouseClick(mMainWindow, mView, internetFirstApMove, waitTime);
    QTest::qWait(300);
    
    // Select destination
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogSecond, waitTime);
    QTest::qWait(300);
    
    // Select ok
    HbAutoTest::mouseClick(mMainWindow, mView, destDialogOk, waitTime);
    QTest::qWait(2000);
    
    HbAutoTest::mouseClick(mMainWindow, mView, addDialogOk, waitTime);
    QTest::qWait(2000);
    
    // Back to destination view
    subClickWidget(navigationButton);
    QTest::qWait(500);
}

// -----------------------------------------------------------------------------
// SUB TEST CASES
// -----------------------------------------------------------------------------

void TestCpDestinationPlugin::subCreateSettingsView()
{
    // Create settings view
    HbView *view = new HbView();
    CpItemDataHelper *itemDataHelper = new CpItemDataHelper();
    itemDataHelper->setParent(view);
    HbDataForm *form = new HbDataForm();
    HbDataFormModel *model = new HbDataFormModel;
    CpPluginUtility::addCpItemPrototype(form);
    QList<CpSettingFormItemData *> list = mPlugin->createSettingFormItemData(*itemDataHelper);
    HbDataFormModelItem *modelItem = NULL;
    modelItem = list[0];
    model->appendDataFormItem(modelItem);
    form->setModel(model);
    model->setParent(form);
    view->setWidget(form);
    itemDataHelper->bindToForm(form);
    
    // Display the view
    // TODO: connect aboutToClose() signal
    mMainWindow->addView(view);
    mMainWindow->setCurrentView(view); 
    
    mView = view;
}

void TestCpDestinationPlugin::subClickWidget(const QString &name)
{
    QList<QGraphicsItem *> itemList = mMainWindow->scene()->items();

    QGraphicsItem *target = 0;
    foreach (QGraphicsItem* item, itemList) {
        if (item->isWidget()) {
            QString widgetClassName(static_cast<QGraphicsWidget*>(item)->metaObject()->className());
            qDebug() << widgetClassName;
            
            if (widgetClassName == name) {
                target = item;
                //break;
            }
        }
    }

    Q_ASSERT(target);
    HbAutoTest::mouseClick(mMainWindow, static_cast<HbWidget *>(target));
}
