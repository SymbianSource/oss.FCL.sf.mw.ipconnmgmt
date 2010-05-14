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
 *   Data item for representing "Add Destination" button in UI.
 */

// System includes
#include <HbInputDialog>
#include <HbAction>
#include <HbMessageBox>
#include <HbPopup>
#include <cpitemdatahelper.h>
#include <cmdestination_shim.h>
#include <cmmanager_shim.h>

// User includes
#include "cpadddestinationentryitemdata.h"
#include "cpdestinationgroup.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpadddestinationentryitemdataTraces.h"
#endif
/*!
    \class  CpAddDestinationEntryItemData
    \brief  This class is a dummy destination. It does not contain 
            access points but clicking it starts new destination 
            creation process.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
            
    Constructor.
    
    @param[in] itemDataHelper Helper from Control Panel for making connections.
    @param[in] parent Parent object.
 */
CpAddDestinationEntryItemData::CpAddDestinationEntryItemData(CpItemDataHelper &itemDataHelper,
                                                             CpDestinationGroup *parent)
    : CpSettingFormEntryItemData(CpSettingFormEntryItemData::ButtonEntryItem, itemDataHelper),
    mParent(parent),
    mDialog(0)
{
    OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_CPADDDESTINATIONENTRYITEMDATA_ENTRY);
    // Fix connections
    itemDataHelper.removeConnection(this,SIGNAL(pressed()),this,SLOT(onLaunchView()));
    itemDataHelper.addConnection(this,SIGNAL(clicked()),this,SLOT(onLaunchView()));
    OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_CPADDDESTINATIONENTRYITEMDATA_EXIT);
}

/*!
    Destructor.
 */
CpAddDestinationEntryItemData::~CpAddDestinationEntryItemData()
{
    OstTraceFunctionEntry0(DUP1_CPADDDESTINATIONENTRYITEMDATA_CPADDDESTINATIONENTRYITEMDATA_ENTRY);
    OstTraceFunctionExit0(DUP1_CPADDDESTINATIONENTRYITEMDATA_CPADDDESTINATIONENTRYITEMDATA_EXIT);
}

/*!
    Inherited member from CpSettingFormEntryItemData. When this item is clicked
    new dialog is started for creating new destination.
 */
void CpAddDestinationEntryItemData::onLaunchView()
{
    OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_ONLAUNCHVIEW_ENTRY);
    mDialog = new HbInputDialog();
    mDialog->setAttribute(Qt::WA_DeleteOnClose);
    mDialog->lineEdit()->setMaxLength(DestinationNameMaxLength);
    mDialog->clearActions();
    mDialog->setPromptText(hbTrId("txt_occ_dialog_destination_name"));
    mDialog->setInputMode(HbInputDialog::TextInput);
    mOkAction = new HbAction(hbTrId("txt_common_button_ok"));
    bool connected = connect(mOkAction, 
                             SIGNAL(triggered()), 
                             this, 
                             SLOT(setNewDestinationName()));
    Q_ASSERT(connected);
    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"));
    mDialog->addAction(mOkAction);
    mDialog->addAction(cancelAction);
    mDialog->show();
    OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_ONLAUNCHVIEW_EXIT);
}

/*!
    This function is called when user selects OK from destination
    name query popup. The given name is valited and if the name is
    valid, new destination is created in commsdat with given name.
    If validation fails user is promted again for destination name.
 */
void CpAddDestinationEntryItemData::setNewDestinationName()
{
    OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_SETNEWDESTINATIONNAME_ENTRY);
    QString destinationName = mDialog->value().toString();
    bool destinationNameInvalid = true;
    CmManagerShim *cmm = NULL;
    CmDestinationShim *destination = NULL;
    
    try {
        cmm = new CmManagerShim();
        if (isDestinationNameValid(destinationName, cmm)) {
            // Destination name OK. Create new destination.
            destination = cmm->createDestination(destinationName);
            destinationNameInvalid = false;
        }
    } catch (const std::exception&) {
        OstTrace0( TRACE_NORMAL, DUP2_CPADDDESTINATIONENTRYITEMDATA_SETNEWDESTINATIONNAME, "CpAddDestinationEntryItemData::setNewDestinationName: exception caught" );
        return;
    }

    if (!destinationNameInvalid) {
        // Update view
        if (mParent != 0) {
            mParent->addDestination(destinationName, destination->id());
        }                
    } else {
        showErrorNote();
    }
    delete destination;
    delete cmm;
    OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_SETNEWDESTINATIONNAME_EXIT);
}

/*!
    Inherited member from CpSettingFormEntryItemData. This item does not
    create new view because it does not need one.
    
    \return NULL
 */
CpBaseSettingView *CpAddDestinationEntryItemData::createSettingView() const
{
	OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_CREATESETTINGVIEW_ENTRY);
	OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_CREATESETTINGVIEW_EXIT);
	return NULL;
}

/*!
    Function for checking if the given destination name is valid. Duplicate and
    and empty names are rejected.
    
    @param[in] dest Name which user has entered to be the name of the new destination.
    @param[in] cmm Pointer to CmManagerShim for accessing data in commsdat.
    \return true if name is valid.
 */
bool CpAddDestinationEntryItemData::isDestinationNameValid(const QString dest, CmManagerShim *cmm) const
{
    OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_ISDESTINATIONNAMEVALID_ENTRY);
    bool retVal = true;
    
    if (dest.length() > 0) {
        QList<uint> destinationList;
        cmm->allDestinations(destinationList);
    
        for (int i = 0; i < destinationList.count(); i ++) {
            CmDestinationShim *destination = cmm->destination(destinationList[i]);
            if (0 == dest.compare(destination->name())) {
                retVal = false;
                break;
            }
            delete destination;
        }
    } else {
        retVal = false;
    }
    OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_ISDESTINATIONNAMEVALID_EXIT);
    return retVal;
}

/*!
 * Helper function for showing error note when user inputs
 * invalid destination name.
 */
void CpAddDestinationEntryItemData::showErrorNote()
{
    OstTraceFunctionEntry0(CPADDDESTINATIONENTRYITEMDATA_SHOWERRORNOTE_ENTRY);
    // Destination name NOK. Inform user and ask again.
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    note->clearActions();
    note->setAttribute(Qt::WA_DeleteOnClose);
    QString info = hbTrId("txt_occ_info_invalid_name");
    note->setText(info);
    note->setTimeout(HbPopup::NoTimeout);
    HbAction *errorOk = new HbAction(hbTrId("txt_common_button_ok"));
    bool connected = connect(
        errorOk,
        SIGNAL(triggered()),
        this,
        SLOT(onLaunchView()));
    Q_ASSERT(connected);                        
    note->addAction(errorOk);
    note->show();
    OstTraceFunctionExit0(CPADDDESTINATIONENTRYITEMDATA_SHOWERRORNOTE_EXIT);
}
