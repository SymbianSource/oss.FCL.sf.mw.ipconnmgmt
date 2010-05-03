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
class CpWlanApView;
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
        void tcChangeHomepage();
        void tcChangeHomepage_data();
        void tcAdvancedSettings();

    private:
        // Sub test cases
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

    private:
        // Code references
        CpWlanApView *mTestView;
        
        // Test data
        HbAutoTestMainWindow *mMainWindow;
        CpBearerApPluginInterface *mPlugin;
};
