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

//#include "cmapplsettingsui.h"

class HbDialog;
class HbMainWindow;
class HbAutoTestMainWindow;
class HbDataFormModelItem;
class CpBearerApPluginInterface;
class CpPluginInterface;
class HbView;

class TestCpDestinationPlugin : public QObject
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
        void tcArrangeAps();
        void tcMoveAp();
        void tcShareDeleteAp();
        void tcAddDestination();
        void tcDestinationRename();
        void tcDestinationDelete();
        void tcCreateAndDeleteDestination();
        void tcDeleteUncat();
        void tcOpenDestinations();

    private:
        // Sub test cases
        void subCreateSettingsView();
        void subClickWidget(const QString &name);
        
        // Not Run cases
        void tcMoveProtected();
        

    private:
        // Code references
        
        // Test data
        HbAutoTestMainWindow              *mMainWindow;
        QSharedPointer<CpPluginInterface> mPlugin;
        HbView                            *mView;
};
