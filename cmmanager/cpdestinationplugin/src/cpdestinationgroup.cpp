/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* CpDestinationGroup contains all destination UI items as well as 
* items for uncategorized access points and adding new destinations.
*/

// System includes

#include <QDir>
#include <QPluginLoader>
#include <QStringList>

#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormViewItem>
#include <HbParameterLengthLimiter>

#include <cpbasesettingview.h>
#include <cpitemdatahelper.h>
#include <cpsettingformentryitemdataimpl.h>
#include <cpsettingformitemdata.h>
#include <cmmanager_shim.h>
#include <cmdestination_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cmmanagerdefines_shim.h>

// User includes

#include "cpdestinationgroup.h"
#include "cpdestinationentryitem.h"
#include "cpadddestinationentryitemdata.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdestinationgroupTraces.h"
#endif

/*!
    \class  CpDestinationGroup
    \brief  This class contains all data items representing destinations
    in commsdat. 
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    
    Constructor.
    
    @param[in] itemDataHelper Helper for connecting signals and slots.
 */
CpDestinationGroup::CpDestinationGroup(CpItemDataHelper &itemDataHelper) :
    CpSettingFormItemData(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_occ_subhead_destinations_access_points")),
    mItemDataHelper(&itemDataHelper),
    mUncategorisedShown(false),
    mCmManager(new CmManagerShim()),
    mBearerPlugins(new QList<CpBearerApPluginInterface *>())
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_CPDESTINATIONGROUP_ENTRY);
    
    this->loadBearerPlugins();
    
    QList<QSharedPointer<CmDestinationShim> > destinationList;
    fetchDestinations(destinationList);

    // Create Destination list
    for (int i = 0; i < destinationList.count(); i++) {
        QString iapCount = 
			getDestinationAdditionalText(destinationList.at(i));
        
        // Create new destination entry
        CpDestinationEntryItemData *destDataItem;
        destDataItem = new CpDestinationEntryItemData(itemDataHelper);
        destDataItem->setContentWidgetData(QString("text"), destinationList.at(i)->name());
        destDataItem->setContentWidgetData(QString("additionalText"),iapCount);
        destDataItem->setDestinationId(destinationList.at(i)->id());
        destDataItem->setDestinationName(destinationList.at(i)->name());
        HbIcon destIcon(resolveDestinationIcon(destinationList.at(i)));
        destDataItem->setEntryItemIcon(destIcon);
        bool connected = connect(
            destDataItem, 
            SIGNAL(destChanged()), 
            this, 
            SLOT(updateDestinationInformation()));
        Q_ASSERT(connected);
        this->appendChild(destDataItem);
    }
    
    // Add "Uncategorised Iaps" button
    createUncategorisedDestination();
    
    // "Add Destination" button
    CpSettingFormEntryItemData *addDest = new CpAddDestinationEntryItemData(
        itemDataHelper,
        this);
    addDest->setContentWidgetData(
        QString("text"),
        hbTrId("txt_occ_button_add_destination"));
    this->appendChild(addDest);

    OstTraceFunctionExit0(CPDESTINATIONGROUP_CPDESTINATIONGROUP_EXIT);
}

/*!
    Destructor
 */
CpDestinationGroup::~CpDestinationGroup()
{
    OstTraceFunctionEntry0(DUP1_CPDESTINATIONGROUP_CPDESTINATIONGROUP_ENTRY);
    
    delete mBearerPlugins;
    delete mCmManager;
    
    OstTraceFunctionExit0(DUP1_CPDESTINATIONGROUP_CPDESTINATIONGROUP_EXIT);
}

/*!
    addDestination() creates new destination item to Destinations group item.
    The new destination is shown in the UI immediately.
 */
void CpDestinationGroup::addDestination(
    const QString &name,
    QSharedPointer<CmDestinationShim> dest)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_ADDDESTINATION_ENTRY);
    
    // Create UI item for new destination
    CpDestinationEntryItemData *destDataItem;
    destDataItem = new CpDestinationEntryItemData(*mItemDataHelper);
    destDataItem->setContentWidgetData(QString("text"), name);
    destDataItem->setContentWidgetData(
        QString("additionalText"), 
        hbTrId("txt_occ_dblist_internet_val_no_access_points"));
    destDataItem->setDestinationId(dest->id());
    destDataItem->setDestinationName(name);
    HbIcon destIcon(resolveDestinationIcon(dest));
    destDataItem->setEntryItemIcon(destIcon);
    connect(
        destDataItem,
        SIGNAL(destChanged()),
        this,
        SLOT(updateDestinationInformation()));
    
    // Insert Child to correct position
    QList<QSharedPointer<CmDestinationShim> > destinationList;
    fetchDestinations(destinationList);
    insertChild(destinationList.count() - 1, destDataItem);
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_ADDDESTINATION_EXIT);
}

/*!
    Deletes destination from commsdat and removes it from UI.
 */
void CpDestinationGroup::deleteDestination(int destId)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_DELETEDESTINATION_ENTRY);
    
    // "Add Destination" child is removed from count (childCount() -1)
    for (int i = 0; i < this->childCount() - 1; i++) { 
        CpDestinationEntryItemData  *destDataItem = 
            static_cast<CpDestinationEntryItemData*>(this->childAt(i));

        if (destDataItem->destinationId() == destId) {
            int index = this->indexOf(destDataItem);
            this->removeChild(index);
            
            if (!mUncategorisedShown) {
            // Return "Uncategorised" item to UI if necessary
            createUncategorisedDestination();
            }
        }
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_DELETEDESTINATION_EXIT);
}

/*!
    Finds correct access point settings implementation according to
    access point bearer type.
    
    \return Returns pointer to CpBearerApPluginInterface which implements
            access point settins.
 */
CpBearerApPluginInterface *CpDestinationGroup::findBearerPlugin(int apId)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_FINDBEARERPLUGIN_ENTRY);
    
    CpBearerApPluginInterface *retVal = NULL;
    try {
        CmConnectionMethodShim *cm = mCmManager->connectionMethod(apId);
        int apBearerType = cm->getIntAttribute(CMManagerShim::CmBearerType);
    
        for (int i = 0; i < mBearerPlugins->count(); i++) {
            if (mBearerPlugins->at(i)->bearerType() == apBearerType) {
                retVal = mBearerPlugins->at(i);
                break;
            }
        }
        delete cm;
    }
    catch (const std::exception&) {
        OstTrace0(
            TRACE_NORMAL,
            CPDESTINATIONGROUP_FINDBEARERPLUGIN,
            "CpDestinationGroup::findBearerPlugin: Exception caught");
        // Let return value be NULL.
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_FINDBEARERPLUGIN_EXIT);
    return retVal;
}

/*!
    Updates all destination group items' destinations' information. 
    (Name and access point count)
 */
void CpDestinationGroup::updateDestinationInformation()
{
	OstTraceFunctionEntry0(CPDESTINATIONGROUP_UPDATEDESTINATIONINFORMATION_ENTRY);
	
	// "Add Destination" child is removed from count (childCount() -1)
    for (int i = 0; i < childCount() - 1; i++) {
        CpDestinationEntryItemData  *destDataItem = 
            static_cast<CpDestinationEntryItemData*>(this->childAt(i));
        
        if (destDataItem->destinationId() == 0) {
            // Uncategrised Iaps
	        QList<uint> apList;
            mCmManager->connectionMethod(apList);
            if (apList.count() == 0) {
                // delete empty uncategorised IAPs destination
                int index = this->indexOf(destDataItem);
                this->removeChild(index);
                mUncategorisedShown = false;
                continue;
            } 
        }
        QString iapCount = getDestinationAdditionalText(destDataItem->destinationId());
        destDataItem->setContentWidgetData(QString("additionalText"), iapCount);
        destDataItem->setContentWidgetData(QString("text"), destDataItem->destinationName());
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_UPDATEDESTINATIONINFORMATION_EXIT);
}

/*!
    Creates item for uncategorized access points to destination group item
    if there is any.
 */
void CpDestinationGroup::createUncategorisedDestination()
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_CREATEUNCATEGORISEDDESTINATION_ENTRY);
    
    QList<uint> apList;
    mCmManager->connectionMethod(apList);
    if (apList.count() > 0) {
        // Create new destination entry
        CpDestinationEntryItemData *destDataItem;
        destDataItem = new CpDestinationEntryItemData(*mItemDataHelper);
        destDataItem->setContentWidgetData(QString("text"), hbTrId("txt_occ_dblist_uncategorized"));
        QString iapCount = getDestinationAdditionalText(0);
        destDataItem->setContentWidgetData(QString("additionalText"),iapCount);
        destDataItem->setDestinationId(0);
        destDataItem->setDestinationName(hbTrId("txt_occ_dblist_uncategorized"));
        HbIcon destIcon(mCmManager->getUncategorizedIcon());
        destDataItem->setEntryItemIcon(destIcon);
        bool connected = connect(
            destDataItem, 
            SIGNAL(destChanged()), 
            this, 
            SLOT(updateDestinationInformation()));
        Q_ASSERT(connected);
        QList<QSharedPointer<CmDestinationShim> > destinationList;
        fetchDestinations(destinationList);
        
        insertChild(destinationList.count(), destDataItem);
        mUncategorisedShown = true;
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_CREATEUNCATEGORISEDDESTINATION_EXIT);
}

/*!
    Searches all destinations from commsdat. List of representing 
    CmDestinationShim * items returned on completion.
 */
void CpDestinationGroup::fetchDestinations(
    QList<QSharedPointer<CmDestinationShim> > &destinationList)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_FETCHDESTINATIONS_ENTRY);
    
    try {
        QList<uint> destArray;
        mCmManager->allDestinations(destArray);
    
        for (int i = 0; i < destArray.count(); i++) {
            CmDestinationShim *destination;
            destination = mCmManager->destination(destArray[i]);
            if (!destination->isHidden()) {
                destinationList.append(QSharedPointer<CmDestinationShim>(destination));
            } else {
                delete destination;
            }
        }
    } catch (const std::exception&) {
        OstTrace0(
            TRACE_NORMAL,
            CPDESTINATIONGROUP_FETCHDESTINATIONS,
            "CpDestinationGroup::fetchDestinations: exception caught, Reading destinations");
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_FETCHDESTINATIONS_EXIT);
}

/*!
    Searches all bearer plugins from fixed location and loads them.
    
    \sa findBearerPlugin()
 */
void CpDestinationGroup::loadBearerPlugins()
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_LOADBEARERPLUGINS_ENTRY);
    
    // Load bearer plugins
    QDir pluginsDir("\\resource\\qt\\plugins\\controlpanel\\bearerap");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        CpBearerApPluginInterface *plugin = 
            qobject_cast<CpBearerApPluginInterface *>(loader.instance());
        if (plugin) {
            mBearerPlugins->append(plugin);
        }
    }
    
    OstTraceFunctionExit0(CPDESTINATIONGROUP_LOADBEARERPLUGINS_EXIT);
}

/*!
    Helper function to be used in localisation. (Overloaded)
    
    \return Returns correct localized QString according to access point count.
 */
QString CpDestinationGroup::getDestinationAdditionalText(QSharedPointer<CmDestinationShim> destination)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_GETDESTINATIONADDITIONALTEXT_ENTRY);
    
    QString result = "";
    int iapCount = destination->connectionMethodCount();
    int counter = 0;
    for (int i = 0; i < iapCount; i++) {
        CmConnectionMethodShim *cm = destination->connectionMethod(i);
        if (cm->getBoolAttribute(CMManagerShim::CmDestination)
            || cm->getBoolAttribute(CMManagerShim::CmHidden)) {
            counter++;
        }
        delete cm; 
    }
    iapCount = iapCount - counter;
    if (iapCount > 0) {
        result = hbTrId("txt_occ_dblist_internet_val_ln_access_points", iapCount);
    } else {
        result = hbTrId("txt_occ_dblist_internet_val_no_access_points");
    }
    
    OstTrace0(TRACE_FLOW, CPDESTINATIONGROUP_GETDESTINATIONADDITIONALTEXT_EXIT, "Exit");
    return result;
}

/*!
    Helper function to be used in localisation. (Overloaded)
    
    \return Returns correct localized QString according to access point count.
 */
QString CpDestinationGroup::getDestinationAdditionalText(int destinationId)
{
    OstTraceFunctionEntry0(CPDESTINATIONGROUP_GETDESTINATIONADDITIONALTEXT2_ENTRY);
    
    QString result = "";
    int iapCount = 0;
    int counter = 0;
    if (destinationId == 0) {
        QList<uint> apList;
        mCmManager->connectionMethod(apList);
        iapCount = apList.count();
        for (int i = 0; i < iapCount; i++) {
            CmConnectionMethodShim *cm = mCmManager->connectionMethod(apList[i]);
            if (cm->getBoolAttribute(CMManagerShim::CmDestination)
                || cm->getBoolAttribute(CMManagerShim::CmHidden)) {
                counter++;
            }
            delete cm; 
        }
    } else {
        CmDestinationShim *destination = mCmManager->destination(destinationId);
        iapCount = destination->connectionMethodCount();
        for (int i = 0; i < iapCount; i++) {
            CmConnectionMethodShim *cm = destination->connectionMethod(i);
            if (cm->getBoolAttribute(CMManagerShim::CmDestination)
                || cm->getBoolAttribute(CMManagerShim::CmHidden)) {
                counter++;
            }
            delete cm; 
        }
        delete destination;
    }
    iapCount = iapCount - counter;
    if (iapCount > 0) {
        result = hbTrId("txt_occ_dblist_internet_val_ln_access_points", iapCount);
    } else {
        result = hbTrId("txt_occ_dblist_internet_val_no_access_points");
    }
    
    OstTrace0(TRACE_FLOW, CPDESTINATIONGROUP_GETDESTINATIONADDITIONALTEXT2_EXIT, "Exit");
    return result;
}

/*!
    Helper function for showing icons.
    
    \return Returns string representing given destination's icon
 */
QString CpDestinationGroup::resolveDestinationIcon(
    QSharedPointer<CmDestinationShim> destination) const
{
    QString result(destination->getIcon());
    
    if (result.isEmpty()) {
        uint metaData = destination->metadata(CMManagerShim::SnapMetadataPurpose);
        switch (metaData) {
            case CMManagerShim::SnapPurposeInternet:
                result = "qtg_small_internet";
                break;
                
            case CMManagerShim::SnapPurposeIntranet:
                result = "qtg_small_intranet";
                break;
                
            case CMManagerShim::SnapPurposeMMS:
                result = "qtg_small_mms";
                break;
                
            case CMManagerShim::SnapPurposeOperator:
                result = "qtg_small_operator";
                break;
                
            default:
                // CMManagerShim::SnapPurposeUnknown
                result = "qtg_small_favorite";
                break;
        }
    }
    
    return result;
}
