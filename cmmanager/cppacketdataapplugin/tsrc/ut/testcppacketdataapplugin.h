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

class HbDialog;
class HbMainWindow;
class HbAutoTestMainWindow;
class HbDataFormModelItem;
class HbAction;
class CpPacketDataApView;
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
        void tcChangeUserName();
        void tcChangeUserName_data();
        void tcScrollToBottom();
        void tcChangePromptPassword();
        void tcChangePassword();
        void tcChangePassword_data();
        void tcChangeAuthenticationMode();
        void tcChangeHomepage();
        void tcChangeHomepage_data();
        void tcAdvancedSettings();

    private:
        // Sub test cases
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
        void subClickWidget(const QString &name);

    private:
        // Code references
        CpPacketDataApView *mTestView;
        
        // Test data
        HbAutoTestMainWindow *mMainWindow;
        CpBearerApPluginInterface *mPlugin;
};
