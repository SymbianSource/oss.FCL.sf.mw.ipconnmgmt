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
* Header for Connection Settings Shim Unit Test program.
*/

#include <QtTest/QtTest>

#include <cmmanager_shim.h>

class HbMainWindow;

class TestCmMgrShim : public QObject
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
        void tcResetSettings();
        void tcLegacyConnMethodWlan();
        void tcLegacyConnMethodGprs();
        void tcCmManagerBasicFails();
        void tcBasicDestination();
        void tcDestinationModify();
        void tcConnMethodSetBoolAttribute();
        void tcConnMethodSetIntAttribute();
        void tcConnMethodSetStringAttribute();
        void tcConnMethodWrongTypeAttributeRead();
        void tcConnMethodWrongTypeAttributeWrite();
        void tcConnectionMethodRefresh();
        void tcDestinationReadConnectionMethods();
        void tcDestinationRemoveConnectionMethod();
        void tcDestinationPriority();
        void tcDestinationMisc();
        void tcDestinationRefresh();
        void tcDestinationInvalidParams();
        
    private:
        // Sub test cases
        void deleteDestinations();
        void deleteConnectionMethods();

    private:
        // Test data
        CmManagerShim *mCmManagerShim;
};
