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
* CM Application Settings UI unit testing.
*/

#include <QSharedPointer>

#include <HbApplication>
#include <HbMainWindow>
#include <HbDialog>
#include <HbRadioButtonList>
#include <HbAction>
#include <QtTest/QtTest>

#include "cmapplsettingsui_p.h"
#include "cmradiodialog.h"

#include "hbautotest.h"
#include "testcmapplsettingsui.h"

// -----------------------------------------------------------------------------
// STATIC TEST DATA
// -----------------------------------------------------------------------------

// Time to wait before continuing after an UI step
static const int waitTime = 10;

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
    app.setApplicationName("TestCmApplSettingsUi");
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestCmApplSettingsUi.txt";
 
    TestCmApplSettingsUi tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}
#else
QTEST_MAIN(TestCmApplSettingsUi)
#endif

/**
 * This function is be called before the first test case is executed.
 */
void TestCmApplSettingsUi::initTestCase()
{
    mMainWindow = new HbAutoTestMainWindow;
    //mMainWindow = new HbMainWindow;
    mMainWindow->show();
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 0;
    mSignalFinished = NULL;
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCmApplSettingsUi::cleanupTestCase()
{
    delete mMainWindow;
    mMainWindow = 0;
}

/**
 * This function is be called before each test case is executed.
 */
void TestCmApplSettingsUi::init()
{
    // Initialize the CmApplSettingsUI object
    mApplSettUi = new CmApplSettingsUi;

    // Initialize code references
    mApplSettingsPriv = mApplSettUi->d_ptr;
    QVERIFY(mApplSettingsPriv != NULL);

    // CmApplSettingsUI finished(uint) signal watcher
    mSignalFinished = new QSignalSpy(mApplSettUi, SIGNAL(finished(uint)));
    QVERIFY(mSignalFinished->isValid() == true);
}

/**
 * This function is be called after each test case is executed.
 */
void TestCmApplSettingsUi::cleanup()
{
    delete mApplSettUi;
    mApplSettUi = NULL;
    
    mApplSettingsPriv = NULL;
    
    QCOMPARE(mSignalFinished->count(), 0);
    delete mSignalFinished;
    mSignalFinished = NULL;
}

// -----------------------------------------------------------------------------
// TEST CASES
// -----------------------------------------------------------------------------

void TestCmApplSettingsUi::tcConfigAndDelete()
{
    // Form the configuration parameters
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 0;
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowDestinations;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
        
    // Configure the dialog
    mApplSettUi->setOptions(listItems, filter);
    mApplSettUi->setSelection(mSelection);
    
    // Skip execution, to test premature deletion
}

void TestCmApplSettingsUi::tcNoContentToShow()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    mApplSettUi->setOptions(listItems, filter);

    // Run the dialog
    mApplSettUi->open();
    
    // The dialog should fail to no content error code,
    // since no selection dialog items were selected.
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
}

void TestCmApplSettingsUi::tcDestinationSelect()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowDestinations;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    mApplSettUi->setOptions(listItems, filter);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    QStringList destinations;
    destinations
        << "Internet"
        << "My Snap";
    subCheckDialog(
        mApplSettingsPriv->mDestinationDialog,
        destinations,
        0);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mDestinationDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);

    // Selection should be Internet Destination
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 4099;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcDestinationCancel()
{
    // Run the dialog with default parameters
    mApplSettUi->open();

    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mDestinationDialog,
        allDestinations,
        0);

    // Click "Cancel"
    subDismissDialog(mApplSettingsPriv->mDestinationDialog->mDialog, false);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
}

void TestCmApplSettingsUi::tcDestinationFilterWlan()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowDestinations;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter |= CMManagerShim::BearerTypeWlan;
    mApplSettUi->setOptions(listItems, filter);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    QStringList destinations;
    destinations
        << "Internet"
        << "My Snap";
    subCheckDialog(
        mApplSettingsPriv->mDestinationDialog,
        destinations,
        0);
    
    // Select "My Snap"
    subSelectDialogItem(
        mApplSettingsPriv->mDestinationDialog,
        1);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mDestinationDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);

    // Selection should be "My Snap" Destination
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 4102;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcDestinationFilterUnknown()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowDestinations;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter |= 0x10281BB7;       // Obsolete CSD bearer type
    mApplSettUi->setOptions(listItems, filter);

    // Run the dialog with default parameters
    mApplSettUi->open();

    // The dialog should fail since there's nothing to show
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
}

void TestCmApplSettingsUi::tcDedicatedConnMethodSelect()
{
    // Configure the dialog
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 4099;
    mApplSettUi->setSelection(mSelection);
    
    // Run the dialog with default parameters
    mApplSettUi->open();

    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mDestinationDialog,
        allDestinations,
        0);
    
    // Select "Dedicated access point"
    subSelectDialogItem(
        mApplSettingsPriv->mDestinationDialog,
        2);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mDestinationDialog->mDialog, true);

    // Connection method list should open
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        allConnectionMethods,
        0);

    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);
    
    // Selection should be Connection Method "Home WLAN"
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 8;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcDedicatedConnMethodCancel()
{
    // Configure the dialog
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 1;
    mApplSettUi->setSelection(mSelection);
    
    // Run the dialog with default parameters
    mApplSettUi->open();

    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mDestinationDialog,
        allDestinations,
        2);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mDestinationDialog->mDialog, true);

    // Connection method list should open
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        allConnectionMethods,
        1);

    // Click "Cancel"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, false);

    // The dialog should be cancelled
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
}

void TestCmApplSettingsUi::tcConnMethodSelect()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    mApplSettUi->setOptions(listItems, filter);
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 1;
    mApplSettUi->setSelection(mSelection);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        allConnectionMethods,
        1);
    
    // Select "WLAN IAP 1"
    subSelectDialogItem(
        mApplSettingsPriv->mConnMethodDialog,
        3);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);

    // Selection should be Connection Method "packet data 3"
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 4;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcConnMethodCancel()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    mApplSettUi->setOptions(listItems, filter);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        allConnectionMethods,
        0);
    
    // Click "Cancel"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, false);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
}

void TestCmApplSettingsUi::tcConnMethodFilterGprs()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter |= CMManagerShim::BearerTypePacketData;
    mApplSettUi->setOptions(listItems, filter);
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 9;
    mApplSettUi->setSelection(mSelection);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        gprsConnectionMethods,
        3);
    
    // Select "packet data 3"
    subSelectDialogItem(
        mApplSettingsPriv->mConnMethodDialog,
        2);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);

    // Selection should be Connection Method "packet data 3"
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 4;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcConnMethodFilterWlan()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter |= CMManagerShim::BearerTypeWlan;
    mApplSettUi->setOptions(listItems, filter);
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 6;
    mApplSettUi->setSelection(mSelection);
    
    // Run the dialog
    mApplSettUi->open();
    
    // Validate the dialog content
    subCheckDialog(
        mApplSettingsPriv->mConnMethodDialog,
        wlanConnectionMethods,
        2);
    
    // Select "WLAN IAP 1"
    subSelectDialogItem(
        mApplSettingsPriv->mConnMethodDialog,
        1);
    
    // Click "OK"
    subDismissDialog(mApplSettingsPriv->mConnMethodDialog->mDialog, true);

    // The dialog should succeed
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNone);

    // Selection should be Connection Method "WLAN IAP 1"
    mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
    mSelection.id = 5;
    subVerifySelection(mSelection);
}

void TestCmApplSettingsUi::tcConnMethodFilterUnknown()
{
    // Configure the dialog
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    listItems |= CmApplSettingsUi::ShowConnectionMethods;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    filter |= 0x10281BB7;       // Obsolete CSD bearer type
    mApplSettUi->setOptions(listItems, filter);
    
    // Run the dialog
    mApplSettUi->open();
    
    // The dialog should fail since there's nothing to show
    subCatchSignalFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
}

// -----------------------------------------------------------------------------
// SUB TEST CASES
// -----------------------------------------------------------------------------

/**
 * Catch and verify the signal "finished(int)" emitted by mSignalFinished.
 */
void TestCmApplSettingsUi::subCatchSignalFinished(uint status)
{
    QTest::qWait(waitTime);
    QCOMPARE(mSignalFinished->count(), 1);      // A sole signal
    QList<QVariant> arguments = mSignalFinished->takeFirst();
    // Verify the first result status
    QVERIFY(arguments.at(0).toUInt() == status);
}

/**
 * Verify the current selection against given selection
 */
void TestCmApplSettingsUi::subVerifySelection(CmApplSettingsUi::SettingSelection selection)
{
    mSelection = mApplSettUi->selection();
    QCOMPARE(mSelection.result, selection.result);
    QCOMPARE(mSelection.id, selection.id);    
}

/**
 * Application settings ui dialog content validation subtestcase.
 * Checks:
 * -The dialog's radiobutton list content.
 * -Dialog's current selection.
 */
void TestCmApplSettingsUi::subCheckDialog(
    CmRadioDialog *radioDialog,
    QStringList items,
    int selected)
{
    // Wait for the dialog to show properly
    QTest::qWait(waitTime);
    
    QVERIFY(radioDialog != NULL);
    QVERIFY(radioDialog->mList != NULL);
    QCOMPARE(radioDialog->mList->items(), items);
    QCOMPARE(radioDialog->mList->selected(), selected);
}

/**
 * Select an item from the radio dialog.
 */
void TestCmApplSettingsUi::subSelectDialogItem(
    CmRadioDialog *radioDialog,
    int selected)
{
    // Wait for the dialog to show properly
    QTest::qWait(waitTime);
    
    QVERIFY(radioDialog != NULL);
    QVERIFY(radioDialog->mList != NULL);
    HbRadioButtonList *radioList = radioDialog->mList;

    // Calculate radio list item center location inside the
    // radio list widget
    QSizeF radioListSize = radioList->size();
    qDebug(
        "Radio List size height(%f) width(%f)",
        radioListSize.height(),
        radioListSize.width());
    int itemCount = radioList->items().count();
    qDebug("Radio List item count: %d", itemCount);
    qreal buttonHeight = radioListSize.height() / itemCount;
    QPoint point(
        radioListSize.width() / 2,
        buttonHeight * (selected + 0.5));
    
    // Click the button
    HbAutoTest::mouseClick(mMainWindow, radioList, point, waitTime);

    // Verify that the selection succeeded
    QCOMPARE(radioDialog->mList->selected(), selected);
}

/**
 * Dismiss the dialog by pressing either "OK" or "Cancel".
 */
void TestCmApplSettingsUi::subDismissDialog(
    QSharedPointer<HbDialog> dialog,
    bool accept)
{
    // Wait for the dialog to show properly
    QTest::qWait(waitTime);
    
    QVERIFY(dialog != NULL);
    
    // Calculate button center location inside the dialog
    QSizeF dialogSize = dialog->size();
    qDebug(
        "Dialog size height(%f) width(%f)",
        dialogSize.height(),
        dialogSize.width());
    QPoint point;
    if (accept) {
        // Left button: "OK"
        point.setX(dialogSize.width() / 4);
    } else {
        // Right button: "Cancel"
        point.setX(dialogSize.width() - (dialogSize.width() / 4));
    }
    point.setY(dialogSize.height() - 30);
    
    // Click the button
    HbAutoTest::mouseClick(mMainWindow, dialog.data(), point, waitTime);
}
