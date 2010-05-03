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
* Data handling encapsulation for CM Application Settings UI.
*
*/

// System includes
#include <HbStringUtil>

// User includes
#include "cmsettingdata.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmsettingdataTraces.h"
#endif


/*!
    \class CmSettingData
    \brief Cm Manager settings data wrapper class for Application Settings UI.

    This class fetches, catches exceptions, filters and orders the
    destinations and connection methods needed by Application Settings UI.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CmSettingData::CmSettingData() :
    mBearers(),
    mCmManager()
{    
    OstTraceFunctionEntry0(CMSETTINGDATA_CMSETTINGDATA_ENTRY);
    OstTraceFunctionExit0(CMSETTINGDATA_CMSETTINGDATA_EXIT);
}

/*!
    Destructor.
*/
CmSettingData::~CmSettingData()
{
    OstTraceFunctionEntry0(DUP1_CMSETTINGDATA_CMSETTINGDATA_ENTRY);
    OstTraceFunctionExit0(DUP1_CMSETTINGDATA_CMSETTINGDATA_EXIT);
}

/*!
    Bearer filter setter function.
    
    @param[in] bearers Connection Method bearer filter.
*/
void CmSettingData::setBearerFilter(const QSet<uint> &bearers)
{
    OstTraceFunctionEntry0(CMSETTINGDATA_SETBEARERFILTER_ENTRY);
    
    mBearers = bearers;
    
    OstTraceFunctionExit0(CMSETTINGDATA_SETBEARERFILTER_EXIT);
}

/*!
    Fetches destinations suitable for Application Selection UI Dialog.
    
    @param[out] destinations Destination list.
    
    @return Settings fetching status - true if reading succeeded.
*/
bool CmSettingData::fetchDestinations(
    QList< QSharedPointer<Destination> > &destinations)
{
    OstTraceFunctionEntry0(CMSETTINGDATA_FETCHDESTINATIONS_ENTRY);
    
    bool success = false;

    // Clear the list just in case
    destinations.clear();
    
    // Catch all CM Manager exceptions
    try {
        getCmManager();
        
        // Fetch all destinations
        QList<uint> destArray;
        mCmManager->allDestinations(destArray);
        int count = destArray.count();
        
        OstTrace1(
            TRACE_NORMAL,
            CMSETTINGDATA_FETCHDESTINATIONS_RETRIEVED,
            "CmSettingData::fetchDestinations retrieved;count=%d",
            count);
        
        // Go through all destinations
        for (int i=0; i < count; i++) {
            QSharedPointer<CmDestinationShim> destination;
            destination = QSharedPointer<CmDestinationShim>(
                mCmManager->destination(destArray[i]));
            // Append to list if destination is OK
            if (validateDestination(destination.data())) {
                QSharedPointer<Destination> dest(new Destination);
                dest->id = destination->id();
                dest->name = destination->name();
                destinations.append(dest);
            }
        }
        
        // Destinations fetched successfully 
        success = true;
    } catch (const std::exception &ex) {
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            CMSETTINGDATA_FETCHDESTINATIONS_EXCEPTION,
            "CmSettingData::fetchDestinations exception;error=%d",
            error);
    }

    OstTraceFunctionExit0(CMSETTINGDATA_FETCHDESTINATIONS_EXIT);
    return success;
}

/*!
    Fetching of connection methods suitable for Application Selection UI Dialog.
    
    @param[out] connMethods Connection Method list.
    
    @return Settings fetching status - true if reading succeeded.
 */
bool CmSettingData::fetchConnMethods(
    QList< QSharedPointer<ConnMethod> > &connMethods)
{
    OstTraceFunctionEntry0(CMSETTINGDATA_FETCHCONNMETHODS_ENTRY);
    
    bool success = false;

    // Clear the list just in case
    connMethods.clear();
    
    // Catch all CM Manager exceptions
    try {
        getCmManager();
        
        // Fetch all connection methods
        QList<uint> connMethodList;
        mCmManager->connectionMethod(connMethodList, false);
        int count = connMethodList.count();
        
        OstTrace1(
            TRACE_NORMAL,
            CMSETTINGDATA_FETCHCONNMETHODS_RETRIEVED,
            "CmSettingData::fetchConnMethods retrieved;count=%d",
            count);

        // Go through all connection methods
        for (int i=0; i < count; i++) {
            QSharedPointer<CmConnectionMethodShim> connMethod;
            connMethod = QSharedPointer<CmConnectionMethodShim>(
                mCmManager->connectionMethod(connMethodList[i]));
            // Append to list if connection method is OK
            if (validateConnMethod(connMethod.data())) {
                QSharedPointer<ConnMethod> connectionMethod(new ConnMethod);
                connectionMethod->id = connMethod->getIntAttribute(
                    CMManagerShim::CmId);
                connectionMethod->name = connMethod->getStringAttribute(
                    CMManagerShim::CmName);
                connMethods.append(connectionMethod);
            }
        }

        // Destinations fetched successfully 
        success = true;
    } catch (const std::exception &ex) {
        int error = qt_symbian_exception2Error(ex);
        OstTrace1(
            TRACE_NORMAL,
            CMSETTINGDATA_FETCHCONNMETHODS_EXCEPTION,
            "CmSettingData::fetchDestinations exception;error=%d",
            error);
    }
    
    // Order the list by connection method name
    qSort(
        connMethods.begin(),
        connMethods.end(),
        CmSettingData::connMethodLessThan);
    
    OstTraceFunctionExit0(CMSETTINGDATA_FETCHCONNMETHODS_EXIT);
    return success;
}

/*!
    Instantiates Cm Manager Shim for this setting data object.
    This is a member function since this can throw an exception and it
    needs to be catched.
 */
void CmSettingData::getCmManager()
{
    if (!mCmManager) {
        mCmManager = QSharedPointer<CmManagerShim>(new CmManagerShim());    
    }
}

/*!
    Validates the given destination.
    This function checks that the following criterias match:
    -The destination is not empty (i.e. it has least one Connection Method).
     This covers also the bearer filter, so there must be at least one
     Connection Method after the bearer filtering.
    -The destination is not hidden.
    
    @param[in] destination Destination to validate.

    @return Validation result - true if valid.
 */
bool CmSettingData::validateDestination(CmDestinationShim *destination) const
{
    OstTraceFunctionEntry0(CMSETTINGDATA_VALIDATEDESTINATION_ENTRY);
    
    bool valid = true;

    // Destination must not be hidden or empty
    if (destination->isHidden() ||
        destination->connectionMethodCount() == 0) {
        valid = false;
    } else if (!mBearers.isEmpty()) {
        // If filter is empty, all bearers are supported

        // Check that there is at least one Connection Method that
        // matches after bearer filtering
        valid = false;
        for (int i=0; i < destination->connectionMethodCount(); i++) {
            QSharedPointer<CmConnectionMethodShim> connMethod;
            connMethod = QSharedPointer<CmConnectionMethodShim>(
                destination->connectionMethod(i));
            int bearer = connMethod->getIntAttribute(CMManagerShim::CmBearerType);
            if (mBearers.contains(bearer)) {
                // Matching bearer found from this Connection Method
                valid = true;
                break;
            }
        }
    }

    OstTraceExt2(
        TRACE_NORMAL,
        CMSETTINGDATA_VALIDATEDESTINATION_RESULT,
        "CmSettingData::validateDestination result;destination=%d;valid=%hhd",
        (TInt32)destination->id(),
        valid);

    OstTraceFunctionExit0(CMSETTINGDATA_VALIDATEDESTINATION_EXIT);
    return valid;
}

/*!
    Validates the given connection method.
    This function checks that the following criterias match:
    -The bearer type of the connection method matches the set filter.
    -The connection method is not hidden.
    
    @param[in] connMethod Connection method to validate.

    @return Validation result - true if valid.
 */
bool CmSettingData::validateConnMethod(CmConnectionMethodShim *connMethod) const
{
    OstTraceFunctionEntry0(CMSETTINGDATA_VALIDATECONNMETHOD_ENTRY);

    bool valid = true;

    int bearer = connMethod->getIntAttribute(CMManagerShim::CmBearerType);
    bool hidden = connMethod->getBoolAttribute(CMManagerShim::CmHidden);
    if (hidden || (!mBearers.isEmpty() && !mBearers.contains(bearer))) {
        valid = false;
    }
    
    OstTraceExt4(
        TRACE_NORMAL,
        CMSETTINGDATA_VALIDATECONNMETHOD_RESULT,
        "CmSettingData::validateConnMethod result;id=%d;valid=%hhd;hidden=%hhd;bearer=%d",
        connMethod->getIntAttribute(CMManagerShim::CmId),
        valid,
        hidden,
        bearer);

    OstTraceFunctionEntry0(CMSETTINGDATA_VALIDATECONNMETHOD_EXIT);
    return valid;
}

/*!
    Compares two Connection Methods.
    
    @param[in] connMethod1 Connection method to compare with.
    @param[in] connMethod2 Connection method to compare.
    
    @return Returns true if the first connection method is "less than"
    the second one.
 */
bool CmSettingData::connMethodLessThan(
    const QSharedPointer<ConnMethod> connMethod1,
    const QSharedPointer<ConnMethod> connMethod2)
{
    int result = HbStringUtil::compareC(connMethod1->name, connMethod2->name);
    return (result < 0) ? true : false;
}
