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
#include <QStringList>
#include <QtTest/QtTest>

#include "cmapplsettingsui.h"

class HbDialog;
class CmRadioDialog;
class HbMainWindow;
class HbAutoTestMainWindow;

class TestCmApplSettingsUi : public QObject
{
    Q_OBJECT

    public slots:
        // Test framework functions
        void initTestCase();
        void cleanupTestCase();
        void init();
        void cleanup();

    private slots:
        // Test cases
        void tcConfigAndDelete();
        void tcNoContentToShow();
        void tcDestinationSelect();
        void tcDestinationCancel();
        void tcDestinationFilterWlan();
        void tcDestinationFilterUnknown();
        void tcDedicatedConnMethodSelect();
        void tcDedicatedConnMethodCancel();
        void tcConnMethodSelect();
        void tcConnMethodCancel();
        void tcConnMethodFilterGprs();
        void tcConnMethodFilterWlan();
        void tcConnMethodFilterUnknown();

    private:
        // Sub test cases
        void subCatchSignalFinished(uint status);
        void subVerifySelection(CmApplSettingsUi::SettingSelection selection);
        void subCheckDialog(
            CmRadioDialog *radioDialog,
            QStringList items,
            int selection);
        void subSelectDialogItem(
            CmRadioDialog *radioDialog,
            int selected);
        void subDismissDialog(
            QSharedPointer<HbDialog> dialog,
            bool accept);

    private:
        // Code references
        CmApplSettingsUiPrivate *mApplSettingsPriv;
        
        // Test data
        HbAutoTestMainWindow *mMainWindow;
        CmApplSettingsUi::SettingSelection mSelection;
        CmApplSettingsUi *mApplSettUi;
        QSignalSpy *mSignalFinished;
};
