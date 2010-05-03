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
*   This items represents one Destination in UI.
*/

// System includes
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbToolBar>
#include <HbAction>
#include <HbDataFormViewItem>
#include <HbMenu>
#include <HbListDialog>
#include <HbInputDialog>
#include <HbMessageBox>
#include <HbPopup>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbMainWindow>
#include <cppluginutility.h>
#include <cpitemdatahelper.h>
#include <cpbasesettingview.h>
#include <cmdestination_shim.h>
#include <cmconnectionmethod_shim.h>
#include <cmmanager_shim.h>

// User includes
#include "cpiapitem.h"
#include "cpdestinationentryitem.h"
#include "cpdestinationgroup.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdestinationentryitemTraces.h"
#endif
/*!
    \class  CpDestinationEntryItemData
    \brief  This class represents destination in data model. 
            It takes care of all destination related operations.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!         
    Constructor.
    
    @param[in] itemDataHelper Helper for connecting signals and slots.
 */
CpDestinationEntryItemData::CpDestinationEntryItemData(CpItemDataHelper &itemDataHelper)
    : CpSettingFormEntryItemData(itemDataHelper),
    mDialog(0),
    mOkAction(0)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CPDESTINATIONENTRYITEMDATA_ENTRY);
    mAps = new QList<CpIapItem*>();
    try {
        mCmm = new CmManagerShim();
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_CPDESTINATIONENTRYITEMDATA, "CpDestinationEntryItemData::CpDestinationEntryItemData: Exception caught");
        mCmm = NULL;
    }
    mList = new HbListWidget();
    bool connectionSuccessful = connect(
        mList,
        SIGNAL(released(HbListWidgetItem *)),
        this,
        SLOT(updateIndex(HbListWidgetItem*)));
    Q_ASSERT(connectionSuccessful);
    
    // Fix connections
    itemDataHelper.removeConnection(this,SIGNAL(pressed()),this,SLOT(onLaunchView()));
    itemDataHelper.addConnection(this,SIGNAL(clicked()),this,SLOT(onLaunchView()));
    itemDataHelper.addConnection(this,SIGNAL(longPress(QPointF)),this,SLOT(showItemMenu(QPointF)));
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CPDESTINATIONENTRYITEMDATA_EXIT);
}

/*!
    Destructor
 */
CpDestinationEntryItemData::~CpDestinationEntryItemData()
{
    OstTraceFunctionEntry0(DUP1_CPDESTINATIONENTRYITEMDATA_CPDESTINATIONENTRYITEMDATA_ENTRY);
    delete mList;
    delete mCmm;
    delete mAps;
    delete mOkAction;
    OstTraceFunctionExit0(DUP1_CPDESTINATIONENTRYITEMDATA_CPDESTINATIONENTRYITEMDATA_EXIT);
}

/*!
    \return unique destination ID of this item.
 */
int CpDestinationEntryItemData::destinationId()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_DESTINATIONID_ENTRY);
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_DESTINATIONID_EXIT);
    return mDestinationId;
}

/*!
    Sets destination ID to be given integer.
 */
void CpDestinationEntryItemData::setDestinationId(int destId)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SETDESTINATIONID_ENTRY);
    mDestinationId = destId;
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SETDESTINATIONID_EXIT);
}

/*!
    \return Returns destination name as QString
 */
QString CpDestinationEntryItemData::destinationName()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_DESTINATIONNAME_ENTRY);
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_DESTINATIONNAME_EXIT);
    return mDestinationName;
}

/*!
    Sets given QString to be destination name.
 */
void CpDestinationEntryItemData::setDestinationName(const QString destinationName)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SETDESTINATIONNAME_ENTRY);
    mDestinationName = destinationName;
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SETDESTINATIONNAME_EXIT);
}

/*!
    Inherited member from CpSettingFormEntryItemData. Returns view that contains
    all access points included in this destination item.
 */
CpBaseSettingView *CpDestinationEntryItemData::createSettingView() const
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CREATESETTINGVIEW_ENTRY);
    CpBaseSettingView* view = new CpBaseSettingView();
    constructSettingView(view);
    
    // Toolbar. Allow arrange if more than one AP is shown
    if (mDestinationId != 0 && mAps->count() > 1) {
        HbToolBar *tb = view->toolBar();
        HbAction *arrangeAction = tb->addAction(hbTrId("txt_occ_button_arrange"));
        bool connected = connect(
            arrangeAction, 
            SIGNAL(triggered()), 
            this, 
            SLOT(activateArrangeMode()));
        Q_ASSERT(connected);
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CREATESETTINGVIEW_EXIT);
    return view;        
}

/*!
    Worker function for createSettingView(). Adds Access points to given view.
    
     @param[out] view Target view where settings are constructed.
 */
void CpDestinationEntryItemData::constructSettingView(CpBaseSettingView *view) const
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CONSTRUCTSETTINGVIEW_ENTRY);
    mAps->clear();
    CpItemDataHelper *itemDataHelper = new CpItemDataHelper();
    HbDataForm *form = new HbDataForm();
    HbDataFormModel *model = new HbDataFormModel;
    form->setModel(model);
    model->setParent(form);
    view->setWidget(form);
    CpPluginUtility::addCpItemPrototype(form);
    QList<QSharedPointer<CmConnectionMethodShim> > apList;
    // Following won't throw exception
    fetchReferencedAps(apList, mCmm);
    form->setHeading(text());

    for (int i = 0; i < apList.count(); i++) {
        int apId = apList[i]->getIntAttribute(CMManagerShim::CmId);
        CpBearerApPluginInterface *bearerPlugin = 
                static_cast<CpDestinationGroup*>(HbDataFormModelItem::parent())->findBearerPlugin(apId);
        bool apProtected = false;
        if (mDestinationId != 0) {
            CmDestinationShim *destination = mCmm->destination(mDestinationId);
            if (destination->protectionLevel() == CMManagerShim::ProtLevel3) {
                apProtected = apList[i]->getBoolAttribute(CMManagerShim::CmProtected);
            }
            delete destination;
        }
        
        CpIapItem *iapDataItem;
        iapDataItem = new CpIapItem(
            *itemDataHelper, 
            apId, 
            apList[i]->getStringAttribute(CMManagerShim::CmName), 
            mDestinationId,
            apProtected,
            bearerPlugin);
       
        // Add name to item
        iapDataItem->setContentWidgetData(
            QString("text"), 
            apList[i]->getStringAttribute(CMManagerShim::CmName));
       
        // Add priority to item if not Uncategorised "Destination"
        if (mDestinationId  != 0) {
            // Access Points are listed in priority order.
            QString priority = hbTrId("txt_occ_dblist_val_priority_l1", i);
            iapDataItem->setContentWidgetData(QString("additionalText"), priority);
        }
        
        // Set item disabled if it is protected
        if (apProtected) {
            iapDataItem->setContentWidgetData(QString("enabled"), false);
        }
        
        // Add to model
        model->appendDataFormItem(iapDataItem);
        
        // Save and make connections
        mAps->append(iapDataItem);
        bool connected 
            = connect(iapDataItem, SIGNAL(iapChanged()), this, SLOT(updateDestinationView()));
        Q_ASSERT(connected);
    }
    
    itemDataHelper->bindToForm(form);
    delete itemDataHelper;
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CONSTRUCTSETTINGVIEW_EXIT);
}

/*!
    Rebuilds view with access points after access points have changed.
 */
void CpDestinationEntryItemData::updateDestinationView()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_UPDATEDESTINATIONVIEW_ENTRY);
    HbDataForm *form = static_cast<HbDataForm*>(model()->parent()); 
    HbMainWindow *mainWnd = form->mainWindow();
       
    if (mainWnd) {
        HbView *view = mainWnd->currentView();
        CpBaseSettingView  *cpView = static_cast<CpBaseSettingView  *>(view);
        constructSettingView(cpView);
        
        // Remove Toolbar if necessary
        if (mDestinationId != 0 && mAps->count() <= 1) {
            HbToolBar *tb = view->toolBar();
            tb->clearActions();
        }
    }
    OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_UPDATEDESTINATIONVIEW, "CpDestinationEntryItemData::updateDestinationView: Emit destination changed");
    emit destChanged();
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_UPDATEDESTINATIONVIEW_EXIT);
}

/*!
    Prompts user for action after long tap. Choices for actions are "Rename" and "Delete".
    
    @param[in] position The item specific menu is shown in given position.
 */
void CpDestinationEntryItemData::showItemMenu(QPointF position)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SHOWITEMMENU_ENTRY);
    if (mDestinationId != 0) {
        CmDestinationShim *destination = NULL;
        CMManagerShim::CmmProtectionLevel protLvl = CMManagerShim::ProtLevel1;
        
        try {
            destination = mCmm->destination(mDestinationId);
            protLvl = destination->protectionLevel();
            delete destination;
        } catch (const std::exception&) {
            OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_SHOWITEMMENU, "CpDestinationEntryItemData::showItemMenu: Exception caught");
            if (destination != NULL) {
                delete destination;
            }
        }
        
        HbMenu *menu = createItemMenu(protLvl, position);
        menu->show();
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SHOWITEMMENU_EXIT);
}

/*!
    Prompts user for new destination name and makes 
    the change in commsdat if new name is valid.
    
    \sa isDestinationNameValid()
 */
void CpDestinationEntryItemData::renameDestination()
{    
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_RENAMEDESTINATION_ENTRY);
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
                             SLOT(saveNewDestinationName()));
    Q_ASSERT(connected);
    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"));
    mDialog->addAction(mOkAction);
    mDialog->addAction(cancelAction);
    mDialog->show();
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_RENAMEDESTINATION_EXIT);
}

/*!
    Shows confirmation query for user before deleteting destination.
    \sa deleteDestination()
 */
void CpDestinationEntryItemData::confirmDestinationDelete()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CONFIRMDESTINATIONDELETE_ENTRY);
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText(hbTrId("txt_occ_info_delete_snap").arg(mDestinationName));
    note->clearActions();
    HbAction *okAction = new HbAction(hbTrId("txt_common_button_yes"));
    note->addAction(okAction);
    bool connected = connect(
        okAction, 
        SIGNAL(triggered()), 
        this, 
        SLOT(deleteDestination()));
    Q_ASSERT(connected);
    HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_no"));
    note->addAction(cancelAction);
    note->setTimeout(HbPopup::NoTimeout);
    note->show();
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CONFIRMDESTINATIONDELETE_EXIT);
}

/*!
    Destination is deleted after user chooses "OK" from destination deletion
    confirmation query.
    \sa confirmDestinationDelete()
 */
void CpDestinationEntryItemData::deleteDestination()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_DELETEDESTINATION_ENTRY);
    CmDestinationShim *destination = NULL;
    CmConnectionMethodShim *cm = NULL;
    try {
        destination = mCmm->destination(mDestinationId);
        int cmCount = destination->connectionMethodCount();
        while (cmCount > 0) {
            cm = destination->connectionMethod(0);
            destination->removeConnectionMethod(cm);
            cmCount--;
            delete cm;
            cm = NULL;
        }
        destination->update();
        destination->deleteDestination();
        CpDestinationGroup *parent 
            = static_cast<CpDestinationGroup *>(HbDataFormModelItem::parent());
        parent->deleteDestination(mDestinationId);
        delete destination;
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_DELETEDESTINATION, "CpDestinationEntryItemData::deleteDestination: Exception caught");
        if (destination != NULL) {
            delete destination;
        }
        if (cm != NULL) {
            delete cm;
        }
        showErrorNote(hbTrId("txt_occ_info_unable_to_save_setting"));
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_DELETEDESTINATION_EXIT);
}

/*!
    Creates new view where access point priorities can 
    be changed by dragging and dropping and shows it to user.
 */
void CpDestinationEntryItemData::activateArrangeMode()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_ACTIVATEARRANGEMODE_ENTRY);
    mList = new HbListWidget();
    HbDataForm *form = static_cast<HbDataForm*>(model()->parent());             
    HbMainWindow *mainWnd = form->mainWindow();
    HbView *view = new HbView();
    createArrangeModeView(view);
    
    if (mainWnd && view) {
        mPreView = mainWnd->currentView();
        mainWnd->addView(view);
        mainWnd->setCurrentView(view, false);
        HbAction *arrangeViewBackAction = new HbAction(Hb::BackAction, view);
        bool connected = connect(
            arrangeViewBackAction, 
            SIGNAL(triggered()), 
            this, 
            SLOT(viewCancel()));
        Q_ASSERT(connected);
        view->setNavigationAction(arrangeViewBackAction);
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_ACTIVATEARRANGEMODE_EXIT);
}

/*!
    This function is called when user returns from arrange mode with
    toolbar command "Done" thus accapting the changes. New priorities
    are saved to commsdat according to order of the items in the previous view.
 */
void CpDestinationEntryItemData::viewDone()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_VIEWDONE_ENTRY);
    try {
        // Arrange items
        CmDestinationShim *destination = mCmm->destination(mDestinationId);
        for (int i = 0; i < mList->count(); i++) {
            int apId = mList->item(i)->data(Hb::IndexFeedbackRole).toInt();
            CmConnectionMethodShim *cm = mCmm->connectionMethod(apId);
            destination->modifyPriority(cm,i);
            delete cm;
        }
        destination->update();
        delete destination;
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_VIEWDONE, "CpDestinationEntryItemData::viewDone: Exception caught");
        viewCancel();
        showErrorNote(QString("txt_occ_info_unable_to_save_setting"));
        OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_VIEWDONE_EXIT);
        return;
    }
    
    HbDataForm *form = static_cast<HbDataForm*>(model()->parent());
    HbMainWindow *mainWnd = form->mainWindow();
    HbView* view = mainWnd->currentView();
    
    if (mainWnd && view)  {
        //restore previous status
        mainWnd->removeView(view);
        mainWnd->setCurrentView(mPreView);
        mPreView = NULL;
    }
    updateDestinationView();
    OstTraceFunctionExit0(DUP1_CPDESTINATIONENTRYITEMDATA_VIEWDONE_EXIT);
}

/*!
    User returns from view without accepting the changes to priorities.
 */
void CpDestinationEntryItemData::viewCancel()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_VIEWCANCEL_ENTRY);
    HbDataForm *form = static_cast<HbDataForm*>(model()->parent());
    HbMainWindow *mainWnd = form->mainWindow();
    HbView* view = mainWnd->currentView();
    
    if (mainWnd && view) {
        //restore previous status
        mainWnd->removeView(view);
        view->deleteLater();
        mainWnd->setCurrentView(mPreView);
        mPreView = NULL;
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_VIEWCANCEL_EXIT);
}

/*!
    This function updates access points priorities shown in UI
    when user is in arrange mode.
    
    @param[in] widgetItem Unused.
    
    \sa activateArrangeMode()
 */
void CpDestinationEntryItemData::updateIndex(HbListWidgetItem *widgetItem)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_UPDATEINDEX_ENTRY);
    Q_UNUSED(widgetItem);
    for (int i = 0; i < mList->count(); i++) {
        HbListWidgetItem *item = mList->item(i);
        QString priority = hbTrId("txt_occ_dblist_val_priority_l1", i);
        item->setSecondaryText(priority);
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_UPDATEINDEX_EXIT);
}

/*!
    This function searches all connection methods from commsdat that are
    connected to this destination item. Connection methods are returned in
    apList reference parameter.
    
    @param[out] apList Constains list of this destination's accesspoints.
    @param[in] cmm Pointer to CmManagerShim instance to avoid multiple session openings.
 */
void CpDestinationEntryItemData::fetchReferencedAps(
    QList<QSharedPointer<CmConnectionMethodShim> > &apList, 
    const CmManagerShim *cmm) const
{   
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_FETCHREFERENCEDAPS_ENTRY);
    CmDestinationShim *destination = NULL;
    try {
        if (mDestinationId == 0) {
            QList<uint> apIds;
            cmm->connectionMethod(apIds);
            for (int i = 0; i < apIds.count(); i++) {
                CmConnectionMethodShim *cm;
                cm = cmm->connectionMethod(apIds.at(i));
                apList.append(QSharedPointer<CmConnectionMethodShim>(cm));
            }
        } else {
            destination = cmm->destination(mDestinationId);
            int apCount = destination->connectionMethodCount();
            for (int i = 0; i < apCount; i++) {
                CmConnectionMethodShim *cm = NULL;
                cm = destination->connectionMethod(i);
                apList.append(QSharedPointer<CmConnectionMethodShim>(cm));
            }
            delete destination;
        }
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_FETCHREFERENCEDAPS, "CpDestinationEntryItemData::fetchReferencedAps: Exception caught");
        if (destination != NULL) {
            delete destination;
        }
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_FETCHREFERENCEDAPS_EXIT);
}

/*!
    Function for checking if the given destination name is valid. Duplicate and
    and empty names are rejected.
    
    @param[out] destination Constains the destination name to be validated. If name 
                is valid it remains unchanged. If the given name is invalid, appropriate
                error text is set the variable.
    @param[in] cmm Pointer to CmManagerShim instance to avoid multiple session openings.
    
    \return true if name is valid.
 */
bool CpDestinationEntryItemData::isDestinationNameValid(
    QString &destination, 
    const CmManagerShim *cmm)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_ISDESTINATIONNAMEVALID_ENTRY);
    bool retVal = true;
    
    if (destination.length() > 0) {
        QList<uint> destinationList;
        cmm->allDestinations(destinationList);
    
        for (int i = 0; i < destinationList.count(); i ++) {
            CmDestinationShim *dest = cmm->destination(destinationList[i]);
            if (0 == destination.compare(dest->name())) {
                destination = hbTrId("txt_occ_info_name_already_in_use");
                retVal = false;
                break;
            }
            delete dest;
        }
    } else {
        destination = hbTrId("txt_occ_info_invalid_name");
        retVal = false;
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_ISDESTINATIONNAMEVALID_EXIT);
    return retVal;
}

/*!
    Worker function for activateArrangeMode(). Adds list of access points
    to given view.
    
    @param[out] view Target view where the arrange mode is constructed.

    \sa activateArrangeMode()
 */
void CpDestinationEntryItemData::createArrangeModeView(HbView *view)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CREATEARRANGEMODEVIEW_ENTRY);
    QList<QSharedPointer<CmConnectionMethodShim> >  apList;
    fetchReferencedAps(apList, mCmm);   
    CmDestinationShim *destination = NULL;
    
    try {
        destination = mCmm->destination(mDestinationId);
        for (int i = 0; i < apList.count(); i++) {
            HbListWidgetItem *item = new HbListWidgetItem();
            item->setText(apList[i]->getStringAttribute(CMManagerShim::CmName));
            item->setData(apList[i]->getIntAttribute(CMManagerShim::CmId), Hb::IndexFeedbackRole);

            uint pri = destination->priority(apList[i].data());
            QString priority = hbTrId("txt_occ_dblist_val_priority_l1", pri);
            item->setSecondaryText(priority);
        
            mList->addItem(item);
        }
        mList->setArrangeMode(true);
        view->setWidget(mList);
                
        // Toolbar
        HbToolBar *tb = view->toolBar();
        HbAction *doneAction = tb->addAction(hbTrId("txt_common_button_ok"));
        bool connected = connect(
            doneAction, 
            SIGNAL(triggered()), 
            this, 
            SLOT(viewDone()));
        Q_ASSERT(connected);
        delete destination;
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_CREATEARRANGEMODEVIEW, "CpDestinationEntryItemData::createArrangeModeView: Exception caught");
        // return empty view
        if (destination != NULL) {
            delete destination;
        }
        while (mList->count() > 0) {
            HbListWidgetItem *item = mList->item(0);
            delete item;
        }
        mList->clear();
    }
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CREATEARRANGEMODEVIEW_EXIT);
}

/*!
    Helper function for creating item specific menu.
    
    @param[in] protLvl Effective protection level for this
                       access point.
 */
HbMenu *CpDestinationEntryItemData::createItemMenu(
    CMManagerShim::CmmProtectionLevel protLvl,
    const QPointF position)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_CREATEITEMMENU_ENTRY);
    HbMenu *menu = new HbMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    HbAction *renameDestAction = menu->addAction(hbTrId("txt_common_menu_rename_item"));
    bool connected = 
        connect(renameDestAction, SIGNAL(triggered()), this, SLOT(renameDestination()));
    Q_ASSERT(connected);
    HbAction *deleteDestAction = menu->addAction(hbTrId("txt_common_menu_delete"));
    connected = 
        connect(deleteDestAction, SIGNAL(triggered()), this, SLOT(confirmDestinationDelete()));
    Q_ASSERT(connected);
                     
    if (protLvl == CMManagerShim::ProtLevel1
         || protLvl == CMManagerShim::ProtLevel2 ) {
    
        // Disable operations for protected destinations
        renameDestAction->setDisabled(true);
        deleteDestAction->setDisabled(true);
    }
        
    menu->setModal(true);
    menu->setPreferredPos(position);
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_CREATEITEMMENU_EXIT);
    return menu;
}

/*!
    This function is called when user selects OK from destination
    name query popup. The given name is valited and if the name is
    valid, new destination is created in commsdat with given name.
    If validation fails user is promted again for destination name.
 */
void CpDestinationEntryItemData::saveNewDestinationName()
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SAVENEWDESTINATIONNAME_ENTRY);
    QString destinationName = mDialog->value().toString();
    bool destinationNameInvalid = true;
    CmManagerShim *cmm = NULL;
    CmDestinationShim *destination = NULL;
    
    try {
        cmm = new CmManagerShim();
        if (isDestinationNameValid(destinationName, cmm)) {
            cmm = new CmManagerShim();
            cmm->destination(mDestinationId)->setName(destinationName);
            mDestinationName = destinationName;
            destinationNameInvalid = false;
        }
    } catch (const std::exception&) {
        OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SAVENEWDESTINATIONNAME_EXIT);
        return;
    }
    delete destination;
    delete cmm;
    
    if (destinationNameInvalid) {   
        // Validation function has modified destination name 
        // to be error string
        showRenameError(destinationName);
    } else {
        OstTrace0(TRACE_NORMAL, CPDESTINATIONENTRYITEMDATA_SAVENEWDESTINATIONNAME, "CpDestinationEntryItemData::saveNewDestinationName: emit destination changed");
        emit destChanged();
    }
    OstTraceFunctionExit0(DUP1_CPDESTINATIONENTRYITEMDATA_SAVENEWDESTINATIONNAME_EXIT);
}

/*!
 * Helper function for showing error note when user inputs
 * invalid destination name.
 */
void CpDestinationEntryItemData::showRenameError(const QString &info)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SHOWRENAMEERROR_ENTRY);
    // Destination name NOK. Inform user and ask again.
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    note->clearActions();
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText(info);
    note->setTimeout(HbPopup::NoTimeout);
    HbAction *errorOk = new HbAction(hbTrId("txt_common_button_ok"));
    bool connected = connect(
        errorOk,
        SIGNAL(triggered()),
        this,
        SLOT(renameDestination()));
    Q_ASSERT(connected);                        
    note->addAction(errorOk);
    note->show();
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SHOWRENAMEERROR_EXIT);
}

/*!
    Helper function for showing error notes.
    
    @param[in] info Info string to be shown in note.
 */
void CpDestinationEntryItemData::showErrorNote(const QString &info)
{
    OstTraceFunctionEntry0(CPDESTINATIONENTRYITEMDATA_SHOWERRORNOTE_ENTRY);
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    note->clearActions();
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText(info);
    note->setTimeout(HbPopup::NoTimeout);
    HbAction *errorOk = new HbAction(hbTrId("txt_common_button_ok"));                       
    note->addAction(errorOk);
    note->show();    
    OstTraceFunctionExit0(CPDESTINATIONENTRYITEMDATA_SHOWERRORNOTE_EXIT);
}
