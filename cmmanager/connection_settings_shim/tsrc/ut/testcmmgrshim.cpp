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

#include <QList>
#include <QString>

#include <HbApplication>
#include <HbMainWindow>
#include <HbView>
#include <HbLabel>

#include <QtTest/QtTest>

#include "cmdestination_shim.h"
#include "cmconnectionmethod_shim.h"

#include "testcmmgrshim.h"

// -----------------------------------------------------------------------------
// STATIC TEST DATA
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// TEST MACROES
// -----------------------------------------------------------------------------

// Test macro for verifying an exception throwing code block
#define TEST_CATCH_AND_VERIFY(code, error) \
    { \
    int error_code; \
    QT_TRYCATCH_ERROR(error_code, code); \
    QCOMPARE(error_code, error); \
    }

// -----------------------------------------------------------------------------
// FRAMEWORK FUNCTIONS
// -----------------------------------------------------------------------------

/**
 * Test main function. Runs all test cases.
 */
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    
    char *pass[3];  
    pass[0] = argv[0];
    pass[1] = "-o"; 
    pass[2] = "c:\\data\\TestCmMgrShim.txt";
 
    TestCmMgrShim tc;
    int res = QTest::qExec(&tc, 3, pass);
 
    return res;
}

/**
 * This function is be called before the first test case is executed.
 */
void TestCmMgrShim::initTestCase()
{
}

/**
 * This function is be called after the last test case was executed.
 */
void TestCmMgrShim::cleanupTestCase()
{
}

/**
 * This function is be called before each test case is executed.
 */
void TestCmMgrShim::init()
{
    // Initialize the CmManagerShim object
    mCmManagerShim = new CmManagerShim;
}

/**
 * This function is be called after each test case is executed.
 */
void TestCmMgrShim::cleanup()
{
    // There should be no destinations
    QList<uint> destinations;
    mCmManagerShim->allDestinations(destinations);
    int destinationCount = destinations.count(); 
    
    // Delete the destinations if there were any
    deleteDestinations();

    // There should be no connection methods
    QList<uint> connMethods;
    mCmManagerShim->connectionMethod(connMethods, false);
    int connMethodCount = connMethods.count(); 
    
    // Delete the connection methods if there were any
    deleteConnectionMethods();

    // Validate after the deletions are done
    QCOMPARE(destinationCount, 0);
    QCOMPARE(connMethodCount, 0);

    delete mCmManagerShim;
}

// -----------------------------------------------------------------------------
// TEST CASES
// -----------------------------------------------------------------------------

/**
 * Test case for testing initial state.
 * -Reads AND DELETES all destinations and connection methods.
 * -Verifies that there are no destinations and connection methods.
 */
void TestCmMgrShim::tcResetSettings()
{
    // Read all destinations
    QList<uint> destinations;
    mCmManagerShim->allDestinations(destinations);
    
    // Delete the destinations
    for (int i=0; i < destinations.count(); i++) {
        CmDestinationShim *dest = mCmManagerShim->destination(destinations[i]);
        dest->deleteDestination();
        
        // Update should leave
        TEST_CATCH_AND_VERIFY(
            dest->update(),
            KErrBadHandle);
        
        delete dest;
    }
    
    // Check that there no longer are any destinations
    mCmManagerShim->allDestinations(destinations);
    QCOMPARE(destinations.count(), 0);
    
    // Read all connection methods
    QList<uint> connMethods;
    mCmManagerShim->connectionMethod(connMethods, false);
    
    // Remove possible uncategorized connection methods
    for (int i=0; i < connMethods.count(); i++){
        CmConnectionMethodShim *cm = 
            mCmManagerShim->connectionMethod(connMethods[i]);
        QVERIFY(cm != NULL);
        bool ok = cm->deleteConnectionMethod();
        QVERIFY(ok == true);
        cm->update();
        delete cm;
    }
    
    // Check that tere is no longer any connection methods
    mCmManagerShim->connectionMethod(connMethods, false);
    QCOMPARE(connMethods.count(), 0);
    
    // Try reading a Connection Method with an erroneous ID
    TEST_CATCH_AND_VERIFY(
        mCmManagerShim->connectionMethod(0),
        KErrArgument);
    
    // Try reading a non-existent Connection Method
    TEST_CATCH_AND_VERIFY(
        mCmManagerShim->connectionMethod(1),
        KErrNotFound);
}

/**
 * Test case for testing basic legacy WLAN connection method handling.
 * -Creates a legacy WLAN connection method (i.e. does not belong to
 *  any destination).
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcLegacyConnMethodWlan()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();
    
    // Check bearer type
    int bearer = cm->getIntAttribute(CMManagerShim::CmBearerType);
    QCOMPARE(bearer, (int)CMManagerShim::BearerTypeWlan);
    
    // Check ID
    int id = cm->getIntAttribute(CMManagerShim::CmId);
    QCOMPARE(id, 1);
    
    // Delete the connection method reference
    delete cm;
    
    // Refetch the connection method
    cm = mCmManagerShim->connectionMethod(id);
    QVERIFY(cm != NULL);
    
    // Delete the connection method
    cm->deleteConnectionMethod();
    
    // Check that double deletion throws an exception
    TEST_CATCH_AND_VERIFY(
        cm->deleteConnectionMethod(),
        KErrBadHandle);
    
    delete cm;
}

/**
 * Test case for testing basic legacy GPRS connection method handling.
 * -Creates a legacy GPRS connection method (i.e. does not belong to
 *  any destination).
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcLegacyConnMethodGprs()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypePacketData);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();
    
    // Check ID
    int id = cm->getIntAttribute(CMManagerShim::CmId);
    QCOMPARE(id, 1);
    
    // Check bearer type
    int bearer = cm->getIntAttribute(CMManagerShim::CmBearerType);
    QCOMPARE(bearer, (int)CMManagerShim::BearerTypePacketData);
    
    // Delete the connection method reference
    delete cm;
    
    // Refetch the connection method
    cm = mCmManagerShim->connectionMethod(id);
    QVERIFY(cm != NULL);
    
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Basic error case testing for CmManagerShim.
 * All test call should throw an exception since they are invalid.
 */
void TestCmMgrShim::tcCmManagerBasicFails()
{
    // Try creating a destination with an invalid name
    CmDestinationShim *dest = 0;
    TEST_CATCH_AND_VERIFY(
        dest = mCmManagerShim->createDestination(QString("")),
        KErrArgument);
    QVERIFY(dest == NULL);
    
    // Try creating a connection method with an invalid bearer type
    CmConnectionMethodShim *cm = 0;
    TEST_CATCH_AND_VERIFY(
        cm = mCmManagerShim->createConnectionMethod(0),
        KErrArgument);
    QVERIFY(cm == NULL);
    
    // Try fetching a connection method with an invalid ID
    cm = 0;
    TEST_CATCH_AND_VERIFY(
        cm = mCmManagerShim->connectionMethod(42),
        KErrNotFound);
    QVERIFY(cm == NULL);
    
    // Try fetching a destination with an invalid ID
    dest = 0;
    TEST_CATCH_AND_VERIFY(
        dest = mCmManagerShim->destination(42),
        KErrArgument);
    QVERIFY(dest == NULL);
}

/**
 * Test case for testing basic destination handling
 * -Create a destination
 * -Validate the default destination content
 * -Delete the destination
 */
void TestCmMgrShim::tcBasicDestination()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    
    // Update to CommsDat
    dest->update();
    
    // Check the name
    QString name(dest->name());
    QCOMPARE(name, QString("TestDestination"));

    // Check ID
    uint destinationId = dest->id();
    QVERIFY(destinationId >= 4000);
    
    // Check protection level default value
    CMManagerShim::CmmProtectionLevel prot = dest->protectionLevel();
    QCOMPARE(prot, CMManagerShim::ProtLevel0);
    
    // Check hidden flag default value
    bool isHidden = dest->isHidden(); 
    QCOMPARE(isHidden, false);
    
    // Check destination content
    int connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Delete the destination
    dest->deleteDestination();

    // Check that double deletion throws an exception
    TEST_CATCH_AND_VERIFY(
        dest->deleteDestination(),
        KErrBadHandle);

    delete dest;
}

/**
 * Test case for testing basic modifications for a destination.
 * -Create a destination
 * -Add a GPRS connection method to it.
 * -Add a WLAN connection method to it.
 * -Delete the GPRS connection method.
 * -Delete the Destination (and thus also the WLAN connection method).
 */
void TestCmMgrShim::tcDestinationModify()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    uint destId = dest->id();
    QVERIFY(destId >= 4000);
    
    // Check destination initial content
    int connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Create a GPRS connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypePacketData);
    QVERIFY(cm != NULL);

    // Update to CommsDat
    cm->update();
    
    // Add the connection method to the destination
    int index = dest->addConnectionMethod(cm);
    QCOMPARE(index, 0);

    // Update to CommsDat
    dest->update();
    
    // Create a WLAN connection method
    CmConnectionMethodShim *cmWlan = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cmWlan != NULL);

    // Update to CommsDat
    cmWlan->update();

    // Add the connection method to the destination
    index = dest->addConnectionMethod(cmWlan);
    QCOMPARE(index, 0);

    // Update to CommsDat
    dest->update();
    
    // Delete the WLAN connection method reference
    delete cmWlan;
    cmWlan = 0;
    
    // Delete the GPRS connection method
    dest->deleteConnectionMethod(cm);
    delete cm;
    cm = 0;
    
    // Update to CommsDat
    dest->update();
    
    // Verify the delete
    connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 1);
    
    // Delete the whole destination including the remaining WLAN conn method.
    dest->deleteDestination();
    delete dest;
    
    // Check that the destination is gone
    dest = NULL;
    TEST_CATCH_AND_VERIFY(
        dest = mCmManagerShim->destination(destId),
        KErrNotFound);
    QCOMPARE(dest, (CmDestinationShim *)0);
}

/**
 * Test case for adding and reading boolean attribute for a connection method.
 * -Creates a legacy WLAN connection method (i.e. does not belong to
 *  any destination).
 * -Sets a boolean attribute.
 * -Gets the boolean attribute.
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcConnMethodSetBoolAttribute()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();
    
    // Set bool attribute value
    cm->setBoolAttribute(CMManagerShim::CmProxyUsageEnabled, true);
        
    // Update to CommsDat
    cm->update();
     
    // Get bool attribute value
    bool testBool = false;
    testBool = cm->getBoolAttribute(CMManagerShim::CmProxyUsageEnabled);
    QCOMPARE(testBool, true);
        
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for adding and reading integer attribute for a connection method.
 * -Creates a legacy WLAN connection method (i.e. does not belong to
 *  any destination).
 * -Sets an int attribute.
 * -Gets the int attribute.
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcConnMethodSetIntAttribute()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();
    
    // Set int attribute value
    uint testInt = 99;
    cm->setIntAttribute(CMManagerShim::CmProxyPortNumber, testInt);
    
    // Update to CommsDat
    cm->update();
     
    // Get int attribute value
    uint resultInt = cm->getIntAttribute(CMManagerShim::CmProxyPortNumber);
    QCOMPARE(resultInt, testInt);    
    
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for adding and reading string attributes for a connection method.
 * -Creates a legacy WLAN connection method (i.e. does not belong to
 *  any destination).
 * -Sets a string attribute.
 * -Gets the string attribute.
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcConnMethodSetStringAttribute()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();

    // Set string attribute value
    QString testString("TestProxyServerName");
    cm->setStringAttribute(CMManagerShim::CmProxyServerName, testString);

    // Set string8 attribute value
    QString testString2("key12");
    cm->setString8Attribute(CMManagerShim::WlanWepKey1InAscii, testString2);
    
    // Update to CommsDat
    cm->update();
     
    // Check string attribute value
    QString resultString = cm->getStringAttribute(CMManagerShim::CmProxyServerName);
    QCOMPARE(resultString, testString);

    // Check string8 attribute value
    resultString = cm->getString8Attribute(CMManagerShim::WlanWepKey1InAscii);
    QCOMPARE(resultString, testString2);

    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for testing attribute reads using a wrong attribute type.
 * All test reads in this case should fail to an exception.
 */
void TestCmMgrShim::tcConnMethodWrongTypeAttributeRead()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();

    // Try reading name erroneously as an int attribute
    uint intResult = 0;
    TEST_CATCH_AND_VERIFY(
        intResult = cm->getIntAttribute(CMManagerShim::CmName),
        KErrNotSupported);
    QCOMPARE(intResult, (uint)0);
    
    // Try reading name erroneously as a bool attribute
    bool boolResult = false;
    TEST_CATCH_AND_VERIFY(
        boolResult = cm->getBoolAttribute(CMManagerShim::CmName),
        KErrNotSupported);
    QCOMPARE(boolResult, false);
    
    // Try reading ID erroneously as a string attribute
    QString stringResult;
    TEST_CATCH_AND_VERIFY(
        stringResult = cm->getStringAttribute(CMManagerShim::CmId),
        KErrNotSupported);
    QCOMPARE(stringResult, QString(""));
    
    // Try reading ID erroneously as a string 8 attribute
    stringResult = "";
    TEST_CATCH_AND_VERIFY(
        stringResult = cm->getString8Attribute(CMManagerShim::CmId),
        KErrNotSupported);
    QCOMPARE(stringResult, QString(""));
    
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for testing attribute reads using a wrong attribute type.
 * All test reads in this case should fail to an exception.
 */
void TestCmMgrShim::tcConnMethodWrongTypeAttributeWrite()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();

    // Try writing name erroneously as an int attribute
    TEST_CATCH_AND_VERIFY(
        cm->setIntAttribute(CMManagerShim::CmName, 0),
        KErrNotSupported);
    
    // Try writing name erroneously as a bool attribute
    TEST_CATCH_AND_VERIFY(
        cm->setBoolAttribute(CMManagerShim::CmName, false),
        KErrNotSupported);
    
    // Try writing ID erroneously as a string attribute
    TEST_CATCH_AND_VERIFY(
        cm->setStringAttribute(CMManagerShim::CmId, QString("dada")),
        KErrNotSupported);
    
    // Try reading ID erroneously as a string 8 attribute
    TEST_CATCH_AND_VERIFY(
        cm->setString8Attribute(CMManagerShim::CmId, QString("dada")),
        KErrNotSupported);
    
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for testing connection method refreshing.  
 * -Create a connection method
 * -Read the name of the connection method
 * -Change the name, but don't update CommsDat
 * -Refsesh connection method
 * -Check that the name is the original one
 * -Delete the connection method
 */
void TestCmMgrShim::tcConnectionMethodRefresh()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
        
    // Update to CommsDat
    cm->update();

    // Check ID
    int id = cm->getIntAttribute(CMManagerShim::CmId);
    QCOMPARE(id, 1);
        
    // Check the default name
    QString name = cm->getStringAttribute(CMManagerShim::CmName);
    QVERIFY(name != QString(""));
        
    // Change name
    cm->setStringAttribute(CMManagerShim::CmName, "WlanBlaaBlaa");
    // Do not update CommsDat

    // Check the changed name
    QString newName = cm->getStringAttribute(CMManagerShim::CmName);
    QCOMPARE(newName, QString("WlanBlaaBlaa"));

    // Refresh connection method
    cm->refresh();

    // Delete the connection method reference
    delete cm;

    // Refetch the connection method
    cm = mCmManagerShim->connectionMethod(id);
    QVERIFY(cm != NULL);        

    // Check that the bearer name in database is the original one
    newName = cm->getStringAttribute(CMManagerShim::CmName);
    QCOMPARE(newName, name);
        
    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
}

/**
 * Test case for testing reading destination's connection methods.
 * -Create a destination
 * -Add a GPRS connection method to it.
 * -Add a WLAN connection method to it.
 * -Read number of connection methods.
 * -Read connection method by index.
 * -Read connection method by unknown index.
 * -Read connection method by id.
 * -Read connection method by unknown id.
 * -Delete the Destination (and connection methods).
 */
void TestCmMgrShim::tcDestinationReadConnectionMethods()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    uint destId = dest->id();
    QVERIFY(destId >= 4000);
    
    // Check destination initial content
    int connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Create a GPRS connection method
    CmConnectionMethodShim *cmGprs = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypePacketData);
    QVERIFY(cmGprs != NULL);
    // Update to CommsDat
    cmGprs->update();
    
    // Add the connection method to the destination
    int index = dest->addConnectionMethod(cmGprs);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest->update();
    
    delete cmGprs;
    cmGprs = 0;
    
    // Create a WLAN connection method
    CmConnectionMethodShim *cmWlan = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cmWlan != NULL);
    // Update to CommsDat
    cmWlan->update();

    // Add the connection method to the destination
    index = dest->addConnectionMethod(cmWlan);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest->update();

    
    // Check number of destination's connection methods
    connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 2);

    // Read connection method by index.
    CmConnectionMethodShim *cm = dest->connectionMethod(1);
    QVERIFY(cm != NULL);
    
    // Try to read connection method by unknown index.
    TEST_CATCH_AND_VERIFY(
            dest->connectionMethod(2),
            KErrArgument);
    
    // Read connection method by id.
    uint id = cmWlan->getIntAttribute(CMManagerShim::CmId);
    cm = dest->connectionMethodByID(id);
    QVERIFY(cm != NULL);
    
    delete cmWlan;
    cmWlan = 0;
    
    delete cm;
    cm = 0;
    
    // Try to read connection method by unknown id.    
    TEST_CATCH_AND_VERIFY(
        dest->connectionMethodByID(100),
        KErrNotFound);
   
    // Delete the destination (and connection methods).
    dest->deleteDestination();
    delete dest;
}

/**
 * Test case for testing removing connection method from single destination.
 * -Create 2 destinations
 * -Add a WLAN connection method to both.
 * -Remove the WLAN connection method from the first destination.
 * -Remove the WLAN connection method from the other destination.
 * -Delete the Destinations.
 * -Delete connection method.
 */
void TestCmMgrShim::tcDestinationRemoveConnectionMethod()
{
    // Create a new destination
    CmDestinationShim *dest1;
    dest1 = mCmManagerShim->createDestination("TestDestination1");
    QVERIFY(dest1 != NULL);
    uint destId1 = dest1->id();
    QVERIFY(destId1 >= 4000);

    // Create another destination
    CmDestinationShim *dest2;
    dest2 = mCmManagerShim->createDestination("TestDestination2");
    QVERIFY(dest2 != NULL);
    uint destId2 = dest2->id();
    QVERIFY(destId2 >= 4000);
    
    // Check destinations initial content
    int connMethodCount = dest1->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    connMethodCount = dest2->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Create a WLAN connection method
    CmConnectionMethodShim *cmWlan = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cmWlan != NULL);
    // Update to CommsDat
    cmWlan->update();

    uint cmWlanId = cmWlan->getIntAttribute(CMManagerShim::CmId);
    QCOMPARE(cmWlanId, (uint)1);

    // Add the connection method to the destination 1
    int index = dest1->addConnectionMethod(cmWlan);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest1->update();

    // Add the connection method to the destination 2
    index = dest2->addConnectionMethod(cmWlan);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest2->update();
        
    // Remove the WLAN connection method from destination 1
    dest1->removeConnectionMethod(cmWlan);
    // Update to CommsDat
    dest1->update();
    
    // Verify the delete
    connMethodCount = dest1->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Check that WLAN connection method still exists for destination 2
    connMethodCount = dest2->connectionMethodCount();
    QCOMPARE(connMethodCount, 1);

    // Remove the WLAN connection method from destination 2
    dest2->removeConnectionMethod(cmWlan);
    // Update to CommsDat
    dest2->update();
    
    // Verify the delete
    connMethodCount = dest2->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);

    // Delete destinations
    dest1->deleteDestination();
    delete dest1;
    dest2->deleteDestination();
    delete dest2;
    
    // Check that WLAN connection method still exists
    delete cmWlan;
    cmWlan = mCmManagerShim->connectionMethod(cmWlanId);
    QVERIFY(cmWlan != NULL);
    
    // Remove WLAN connection method.
    cmWlan->deleteConnectionMethod();
    delete cmWlan;
}

/**
 * Test case for testing destination's cm priority reading and changing. 
 * -Create a destination
 * -Add a GPRS connection method to it.
 * -Add a WLAN connection method to it.
 * -Read priority values of the connection methods
 * -Change the priority values
 * -Read the changed priority values
 * -Try to change the priority value to unsupported value
 * -Delete the destination (and connection methods)
 */
void TestCmMgrShim::tcDestinationPriority()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    uint destId = dest->id();
    QVERIFY(destId >= 4000);
      
    // Create a GPRS connection method
    CmConnectionMethodShim *cmGprs = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypePacketData);
    QVERIFY(cmGprs != NULL);
    // Update to CommsDat
    cmGprs->update();
    
    // Add the connection method to the destination
    int index = dest->addConnectionMethod(cmGprs);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest->update();
    
    // Create a WLAN connection method
    CmConnectionMethodShim *cmWlan = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cmWlan != NULL);
    // Update to CommsDat
    cmWlan->update();

    // Add the connection method to the destination
    index = dest->addConnectionMethod(cmWlan);
    QCOMPARE(index, 0);
    // Update to CommsDat
    dest->update();
        
    // Get the priority of connection methods
    int priority = dest->priority(cmWlan);
    QCOMPARE(priority, 1);
    priority = dest->priority(cmGprs);
    QCOMPARE(priority, 2);
    
    // Modify the priority of GPRS connection method
    dest->modifyPriority(cmGprs, 0);
    dest->modifyPriority(cmWlan, 1);
    // Update to CommsDat
    dest->update();
    
    // Check the priorities
    priority = dest->priority(cmGprs);
    QCOMPARE(priority, 1);
    priority = dest->priority(cmWlan);
    QCOMPARE(priority, 2);

    // Try to modify the priority to forbidden value  
    TEST_CATCH_AND_VERIFY(
        dest->modifyPriority(cmGprs, 2),
        KErrArgument);

    // Delete the whole destination including the remaining WLAN conn method.
    dest->deleteDestination();
    
    delete dest;
    delete cmGprs;
    delete cmWlan;
}

/**
 * Test case for testing miscellaneous methods for destination. 
 * -Create a destination
 * -Change the name of the destination
 * -Check if the destination is hidden
 * -Check if destination is "internet" with metadata.
 * -Delete the destination
 */
void TestCmMgrShim::tcDestinationMisc()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    uint destId = dest->id();
    QVERIFY(destId >= 4000);
          
    // Change the name of the destination
    dest->setName("NewName");
    // Update to CommsDat
    dest->update();
    
    // Check the name
    QString name(dest->name());
    QCOMPARE(name, QString("NewName"));

    // Check if destination is hidden
    bool hidden = dest->isHidden();
    QCOMPARE(hidden, false);
    
    // Check "internet" metadata field
    uint metadata = dest->metadata(CMManagerShim::SnapMetadataInternet);
    QCOMPARE(metadata, (uint)false);
    
    // Delete the destination
    dest->deleteDestination();
    delete dest;
}

/**
 * Test case for testing destination refreshing method.  
 * -Create a destination
 * -Add a GPRS connection method, but don't update CommsDat
 * -Change the name of the destination, but don't update CommsDat
 * -Refresh destination
 * -Check that the changes are not valid anymore
 * -Delete the destination
 * -Delete the connection method
 */
void TestCmMgrShim::tcDestinationRefresh()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    uint destId = dest->id();
    QVERIFY(destId >= 4000);
    dest->update();

    // Create a GPRS connection method
    CmConnectionMethodShim *cmGprs = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypePacketData);
    QVERIFY(cmGprs != NULL);
    // Update to CommsDat
    cmGprs->update();
    
    // Add the connection method to the destination
    int index = dest->addConnectionMethod(cmGprs);
    QCOMPARE(index, 0);
    // Don't update CommsDat
    
    // Change the name of the destination
    dest->setName("NewName");
    // Don't update to CommsDat

    // Refresh destination
    dest->refresh();
    
    // Delete the destination reference
    delete dest;
    
    // Refetch the destination
    dest = mCmManagerShim->destination(destId);
    QVERIFY(dest != NULL);
    
    // Check the name that it is the original one
    QString name(dest->name());
    QCOMPARE(name, QString("TestDestination"));

    // Check destination content
    int connMethodCount = dest->connectionMethodCount();
    QCOMPARE(connMethodCount, 0);
    
    // Delete the destination
    dest->deleteDestination();
    delete dest;

    // Delete connection method
    bool ok = cmGprs->deleteConnectionMethod();
    delete cmGprs;
    QCOMPARE(ok, true);
}

/**
 * Test case for testing data reads using a invalid input data.
 * All test reads in this case should fail to an exception.
 */
void TestCmMgrShim::tcDestinationInvalidParams()
{
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
    
    // Update to CommsDat
    dest->update();

    // Try reading with an invalid connection method index
    CmConnectionMethodShim *cm = 0;
    TEST_CATCH_AND_VERIFY(
        dest->connectionMethod(42),
        KErrArgument);
    QVERIFY(cm == 0);
    
    // Try reading with an invalid connection method ID
    cm = 0;
    TEST_CATCH_AND_VERIFY(
        dest->connectionMethodByID(42),
        KErrNotFound);
    QVERIFY(cm == 0);
    
    // Delete the destination
    dest->deleteDestination();
    delete dest;
}

/**
 * Test case for setting and getting icon from CM Manager
 * -Creates a legacy WLAN connection method (i.e. does not belong to
 *  any destination).
 * -Sets & Gets Icon
 * -Creates destination
 * -Sets & Gets Icon
 * -Deletes the connection method. 
 */
void TestCmMgrShim::tcIconOperations()
{
    // Create the connection method
    CmConnectionMethodShim *cm = mCmManagerShim->createConnectionMethod(
        CMManagerShim::BearerTypeWlan);
    QVERIFY(cm != NULL);
    
    // Update to CommsDat
    cm->update();
    
    // Create a new destination
    CmDestinationShim *dest;
    dest = mCmManagerShim->createDestination("TestDestination");
    QVERIFY(dest != NULL);
       
    // Update to CommsDat
    dest->update();

    // Set icon
    QString testString("TestIconName");
    cm->setIcon(testString);
    
    // Update to CommsDat
    cm->update();
    
    // Set icon
    dest->setIcon(testString);
    
    // Update to CommsDat
    dest->update();
     
    // Check icon
    QString resultString = cm->getIcon();
    QCOMPARE(resultString, testString);

    // Check destination icon
    QString resultDestString = dest->getIcon();
    QCOMPARE(resultDestString, testString);

    // Delete the connection method
    cm->deleteConnectionMethod();
    delete cm;
    
    // Delete the destination
    dest->deleteDestination();
    delete dest;
}

// -----------------------------------------------------------------------------
// SUB TEST CASES
// -----------------------------------------------------------------------------

/**
 * Case for deleting all destinations.
 */
void TestCmMgrShim::deleteDestinations()
{
    // Read all destinations
    QList<uint> destinations;
    mCmManagerShim->allDestinations(destinations);
    
    // Delete the destinations
    for (int i=0; i < destinations.count(); i++) {
        CmDestinationShim *dest = mCmManagerShim->destination(destinations[i]);
        dest->deleteDestination();
        delete dest;
    }
}

/**
 * Case for deleting all connection methods.
 */
void TestCmMgrShim::deleteConnectionMethods()
{
    // Read all connection methods
    QList<uint> connMethods;
    mCmManagerShim->connectionMethod(connMethods, false);
    
    // Remove all connection methods
    for (int i=0; i < connMethods.count(); i++){
        CmConnectionMethodShim *cm = 
            mCmManagerShim->connectionMethod(connMethods[i]);
        QVERIFY(cm != NULL);
        bool ok = cm->deleteConnectionMethod();
        QVERIFY(ok == true);
        cm->update();
        delete cm;
    }
}
