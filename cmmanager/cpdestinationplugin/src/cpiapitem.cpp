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
*   Data item for representing access points in UI.
*/

// System includes
#include <QObject>
#include <HbMenu>
#include <HbAction>
#include <HbDocumentLoader>
#include <HbRadioButtonList>
#include <HbDialog>
#include <HbMessageBox>
#include <HbPopup>
#include <HbLabel>
#include <HbDataFormViewItem>
#include <HbDataForm>
#include <HbDataFormModel>
#include <cpbasesettingview.h>
#include <cpsettingformitemdata.h>
#include <cpsettingformentryitemdata.h>
#include <cpitemdatahelper.h>
#include <cmdestination_shim.h>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes
#include "cpiapitem.h"
#include "cpdestinationentryitem.h"
#include "cpbearerapplugininterface.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpiapitemTraces.h"
#endif
/*!
    \class  CpIapItem
    \brief  This class represents access point in data model. 
            It takes care of all access point related operations.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!         
   Constructor.
   
   @param[in] itemDataHelper Helper for connecting signals and slots.
   @param[in] iapId Unique ID if the access point this object represents.
   @param[in] iapName Name of the access point this object represents.
   @param[in] destId Unique ID of the destination that contains reference to 
              the access point this object represents.
   @param[in] bearerPlugin Pointer to the object that implements settings view
              for this access point.
 */
CpIapItem::CpIapItem(
    CpItemDataHelper &itemDataHelper,  
    int iapId, 
    const QString &iapName, 
    int destId,
    bool apProtected,
    CpBearerApPluginInterface *bearerPlugin) :
    CpSettingFormEntryItemData(CpSettingFormEntryItemData::ButtonEntryItem, itemDataHelper), 
    mIapId(iapId), 
    mIapName(iapName),
    mDestId(destId),
    mItemDataHelper(&itemDataHelper),
    mBearerPlugin(bearerPlugin),
    mMoveOngoing(false),
    mDialog(0),
    mList(0)
{
    OstTraceFunctionEntry0(CPIAPITEM_CPIAPITEM_ENTRY);
    try {
        mCmm = new CmManagerShim();
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPIAPITEM_CPIAPITEM, "CpIapItem::CpIapItem: Exception caught");
        mCmm = NULL;
    }
    
    // Fix connections
    itemDataHelper.removeConnection(this,SIGNAL(pressed()),this,SLOT(onLaunchView()));
    if (!apProtected) {
        itemDataHelper.addConnection(
            this,
            SIGNAL(longPress(QPointF)),
            this,
            SLOT(showItemMenu(QPointF)));
        itemDataHelper.addConnection(this,SIGNAL(clicked()),this,SLOT(onLaunchView()));
    }
    OstTraceFunctionExit0(CPIAPITEM_CPIAPITEM_EXIT);
}

/*!
    Destructor
 */
CpIapItem::~CpIapItem()
{
    OstTraceFunctionEntry0(DUP1_CPIAPITEM_CPIAPITEM_ENTRY);
    delete mCmm;
    OstTraceFunctionExit0(DUP1_CPIAPITEM_CPIAPITEM_EXIT);
}

/*!
    Shows user the item specific menu. The menu is triggered by long pressing
    the access point item.
    
    @param[in] position Preferred position for the item specific menu.
 */
void CpIapItem::showItemMenu(QPointF position)
{
    OstTraceFunctionEntry0(CPIAPITEM_SHOWITEMMENU_ENTRY);
    if (isCmManagerAvailable()) {
        bool settingsReadSuccessful = true;
        bool cmConnected = false;
        try {
            CmConnectionMethodShim *cm = mCmm->connectionMethod(mIapId);
            cmConnected = cm->getBoolAttribute(CMManagerShim::CmConnected);
            delete cm;
        } 
        catch (const std::exception&)  {
            OstTrace0(TRACE_NORMAL, CPIAPITEM_SHOWITEMMENU, "CpIapItem::showItemMenu: Exception caught");
            settingsReadSuccessful = false;
        }
        if (settingsReadSuccessful) {
            // Settings could be read from commsdat: show menu.
            createItemMenu(cmConnected, position)->show();
        }    
    }
    OstTraceFunctionExit0(CPIAPITEM_SHOWITEMMENU_EXIT);
}

/*!
    Prompts user for destination where access point is to be 
    added and then makes the move in commsdat.
 */
void CpIapItem::moveIap()
{
    OstTraceFunctionEntry0(CPIAPITEM_MOVEIAP_ENTRY);
    mMoveOngoing = true;
    queryDestination();
    OstTraceFunctionExit0(CPIAPITEM_MOVEIAP_EXIT);
}

/*!
    Shows confirmation query for access point deletion.
 */
void CpIapItem::showDeleteConfirmation()
{
    OstTraceFunctionEntry0(CPIAPITEM_SHOWDELETECONFIRMATION_ENTRY);
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    note->clearActions();
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText(hbTrId("txt_occ_info_delete_access_point_1").arg(mIapName));
    HbAction *primaryAction = new HbAction(hbTrId("txt_common_button_yes"));
    bool connected = connect(
        primaryAction, 
        SIGNAL(triggered()), 
        this, 
        SLOT(deleteConfirmed()));
    Q_ASSERT(connected);
    note->addAction(primaryAction);
    note->addAction(new HbAction(hbTrId("txt_common_button_no")));
    note->setTimeout(HbPopup::NoTimeout);
    note->show();
    OstTraceFunctionExit0(CPIAPITEM_SHOWDELETECONFIRMATION_EXIT);
}

/*!
    Prompts user for target destination and then adds access point 
    that this object represents to the prompted destination.
 */
void CpIapItem::shareIap()
{
    OstTraceFunctionEntry0(CPIAPITEM_SHAREIAP_ENTRY);
    mMoveOngoing = false;
    queryDestination();
    OstTraceFunctionExit0(CPIAPITEM_SHAREIAP_EXIT);
}

/*!
    Completes either saving or sharing of access point by 
    making the move in commsdat.
 */
void CpIapItem::queryDialogClosed()
{
    OstTraceFunctionEntry0(CPIAPITEM_QUERYDIALOGCLOSED_ENTRY);
    if (mMoveOngoing) {
        saveMove(mDestinationList[mList->selected()]);
    } else {
        saveShare(mDestinationList[mList->selected()]);
    }
    OstTraceFunctionExit0(CPIAPITEM_QUERYDIALOGCLOSED_EXIT);
}

/*!
    Completes the deletion process by deleting access point from
    commsdat.
 */
void CpIapItem::deleteConfirmed()
{
    OstTraceFunctionEntry0(CPIAPITEM_DELETECONFIRMED_ENTRY);
    bool deleteSuccessful = true;
    try  {
        if (mDestId != 0) {
            CmDestinationShim *destination = mCmm->destination(mDestId);
            CmConnectionMethodShim *cm = destination->connectionMethodByID(mIapId);
            destination->deleteConnectionMethod(cm);
            destination->update();
            delete cm;
            delete destination;
        } else {
            CmConnectionMethodShim *cm = mCmm->connectionMethod(mIapId);
            cm->deleteConnectionMethod();
            delete cm;
        }
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, DUP1_CPIAPITEM_DELETECONFIRMED, "CpIapItem::deleteConfirmed: Exception caught");
        deleteSuccessful = false;
    }
    if (deleteSuccessful) {
        this->deleteLater();
        OstTrace0(TRACE_NORMAL, CPIAPITEM_DELETECONFIRMED, "CpIapItem::deleteConfirmed: Emit access point changed signal");
        emit iapChanged();
    } else {
        showErrorNote(hbTrId("txt_occ_info_unable_to_save_setting"));
    }
    OstTraceFunctionExit0(CPIAPITEM_DELETECONFIRMED_EXIT);
}

/*!
    Updates access point item name when the item becomes visible.
 */
void CpIapItem::updateIap(const QModelIndex index)
{
    OstTrace0( TRACE_FLOW, CPIAPITEM_UPDATEIAP_ENTRY, "CpIapItem::updateIap entry" );
    Q_UNUSED(index);
    try {
        CmConnectionMethodShim *cm = mCmm->connectionMethod(mIapId);
        this->setContentWidgetData("text", cm->getStringAttribute(CMManagerShim::CmName));
        delete cm;
    } catch (const std::exception&) {
        OstTrace0( TRACE_NORMAL, CPIAPITEM_UPDATEIAP, "CpIapItem::updateIap: exception caught, CM name reading failed" );
    }
    // Disconnect because we need to do this only after returning
    // from accees point settings view
    CpItemDataHelper *itemDataHelper = new CpItemDataHelper();
    itemDataHelper->disconnectFromForm( 
        SIGNAL(itemShown(const QModelIndex)),
        this, 
        SLOT(updateIap(const QModelIndex)));
    delete itemDataHelper;
    OstTrace0( TRACE_FLOW, DUP1_CPIAPITEM_UPDATEIAP_EXIT, "CpIapItem::updateIap exit" );
}

/*!
    Opens Iap settings view.
 */
void CpIapItem::openIap()
{
    onLaunchView();
}

/*!
    Gets access point specific view from bearer plugin.
    
    \return Returns settings view containing access point settings.
 */
CpBaseSettingView *CpIapItem::createSettingView() const
{
    OstTraceFunctionEntry0(CPIAPITEM_CREATESETTINGVIEW_ENTRY);
    CpBaseSettingView *view = NULL;
    CpItemDataHelper *itemDataHelper = new CpItemDataHelper();
    if (mBearerPlugin != NULL) {
        CmConnectionMethodShim *cm = mCmm->connectionMethod(mIapId);
        bool cmConnected = cm->getBoolAttribute(CMManagerShim::CmConnected);
        delete cm;
        
        // Do not open connected AP
        if (!cmConnected) {
            CpIapItem *iap = const_cast<CpIapItem*>(this);
            itemDataHelper->connectToForm( 
                SIGNAL(itemShown(const QModelIndex)),
                iap, 
                SLOT(updateIap(const QModelIndex)));
            view = mBearerPlugin->createSettingView(mIapId);
        }
    }
    delete itemDataHelper;
    OstTraceFunctionExit0(CPIAPITEM_CREATESETTINGVIEW_EXIT);
    return view;
}

/*!
    Prompts user for destination.
 */
void CpIapItem::queryDestination()
{
    OstTraceFunctionEntry0(CPIAPITEM_QUERYDESTINATION_ENTRY);
    bool readingSuccessful = true;
    QStringList destinations;
    try {
        mCmm->allDestinations(mDestinationList);
        
        for (int i = 0; i < mDestinationList.count(); i++) {
            CmDestinationShim *destination = mCmm->destination(mDestinationList[i]);
            if ((destination->id() != mDestId)
                && !destination->isHidden()) {

                QString dest = destination->name();
                destinations.append(dest);
            } else {
                // Remove this destination from list to sync both lists
                mDestinationList.removeAt(i);
                i--;
            }
            delete destination;
        }
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, CPIAPITEM_QUERYDESTINATION, "CpIapItem::queryDestination: exception caught");
        readingSuccessful = false;
    }
    
    if (readingSuccessful) {
        // Load DocML 
        bool ok = false;
        HbDocumentLoader *loader = new HbDocumentLoader();
        loader->load(":/docml/cpdestinationplugindialogs.docml", &ok);
        mDialog = qobject_cast<HbDialog *>(loader->findWidget("dialog"));
        HbLabel *heading = qobject_cast<HbLabel *>(loader->findWidget("heading"));
        heading->setPlainText(hbTrId("txt_occ_dialog_select_network_destination_to_be_ac"));
        mList = qobject_cast<HbRadioButtonList *>(loader->findWidget("radioButtonList")); 
        mList->setItems(destinations);
        mList->setSelected(0);
        mOk = qobject_cast<HbAction *>(loader->findObject("okAction"));
        bool connected = connect(mOk, 
                                 SIGNAL(triggered()), 
                                 this, 
                                 SLOT(queryDialogClosed()));
        Q_ASSERT(connected);
        mDialog->show();
        delete loader;
    } else {
        OstTrace0(TRACE_NORMAL, DUP1_CPIAPITEM_QUERYDESTINATION, " CpIapItem::queryDestination: exception caught");
        showErrorNote(hbTrId("txt_occ_info_unable_to_read_settings"));
    }
    OstTraceFunctionExit0(CPIAPITEM_QUERYDESTINATION_EXIT);
}

/*!
    Shares access point to given destination.
    
    @param[in] id Target destination's ID
 */
void CpIapItem::saveShare(int id)
{
    OstTraceFunctionEntry0(CPIAPITEM_SAVESHARE_ENTRY);
    try {
        CmConnectionMethodShim *cm;
        CmDestinationShim *source = mCmm->destination(mDestId);
        cm = source->connectionMethodByID(mIapId);
        delete source;
        CmDestinationShim *target = mCmm->destination(id);
        target->addConnectionMethod(cm);
        target->update();
        delete cm;
        delete target;
        OstTrace0(TRACE_NORMAL, CPIAPITEM_SAVESHARE, "CpIapItem::saveShare: Emit access point changed signal");
        emit iapChanged();
    } catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, DUP1_CPIAPITEM_SAVESHARE, "CpIapItem::saveShare: Exception caught");
        showErrorNote(hbTrId("txt_occ_info_unable_to_read_settings"));
    }
    OstTraceFunctionExit0(CPIAPITEM_SAVESHARE_EXIT);
}

/*!
    Moves access point from current destination to given destination.
    
    @param[in] id Target destination's ID
 */
void CpIapItem::saveMove(int id)
{
    OstTraceFunctionEntry0(CPIAPITEM_SAVEMOVE_ENTRY);
    try {
        // Make move in commsdat
        CmConnectionMethodShim *cm = NULL;
        if (mDestId != 0) {
            CmDestinationShim *source = mCmm->destination(mDestId);
            cm = source->connectionMethodByID(mIapId);
            source->removeConnectionMethod(cm);
            source->update();
            delete source;
        } else {
            cm = mCmm->connectionMethod(mIapId);
        }
        CmDestinationShim *target = mCmm->destination(id);
        target->addConnectionMethod(cm);
        target->update();
        delete target;
        delete cm;
    }
    catch (const std::exception&) {
        OstTrace0(TRACE_NORMAL, DUP1_CPIAPITEM_SAVEMOVE, "CpIapItem::saveMove: Exception caught");
        showErrorNote(hbTrId("txt_occ_info_unable_to_save_setting"));
    }
    OstTrace0(TRACE_NORMAL, CPIAPITEM_SAVEMOVE, "CpIapItem::saveMove: Emit access point changed signal");
    emit iapChanged(); 
    OstTraceFunctionExit0(CPIAPITEM_SAVEMOVE_EXIT);
}

/*!
    Tests if CmManagerShim has been created successfully
    earlier. If not, it retries.
 */
bool CpIapItem::isCmManagerAvailable()
{
    OstTraceFunctionEntry0(CPIAPITEM_ISCMMANAGERAVAILABLE_ENTRY);
    bool retval = false;
    if (mCmm == NULL) {
        try {
            mCmm = new CmManagerShim();
            retval = true;
        }
        catch (const std::exception&) {
            OstTrace0(TRACE_NORMAL, CPIAPITEM_ISCMMANAGERAVAILABLE, "CpIapItem::isCmManagerAvailable: Exception caught");
            mCmm = NULL;
        }
    } else {
        retval = true;
    }
    OstTraceFunctionExit0(CPIAPITEM_ISCMMANAGERAVAILABLE_EXIT);
    return retval;
}

/*!
    Helper function for creating item specific menu.
    
    @param[in] protLvl Effective protection level for this
                       access point.
 */
HbMenu *CpIapItem::createItemMenu(
    bool cmConnected,
    const QPointF &position)
{
    OstTraceFunctionEntry0(CPIAPITEM_CREATEITEMMENU_ENTRY);
    HbMenu* menu = new HbMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    HbAction* openIapAction 
            = menu->addAction(hbTrId("txt_common_menu_open"));
    bool connected = connect(openIapAction, SIGNAL(triggered()), this, SLOT(openIap()));
    Q_ASSERT(connected);
    HbAction* moveIapAction 
        = menu->addAction(hbTrId("txt_occ_menu_move_to_other_destination"));
    connected = connect(moveIapAction, SIGNAL(triggered()), this, SLOT(moveIap()));
    Q_ASSERT(connected);
    HbAction* deleteIapAction 
        = menu->addAction(hbTrId("txt_common_menu_delete"));
    connected = connect(
        deleteIapAction, 
        SIGNAL(triggered()), 
        this, 
        SLOT(showDeleteConfirmation()));
    Q_ASSERT(connected);
    HbAction* shareIapAction 
        = menu->addAction(hbTrId("txt_occ_menu_share_to_other_destination"));
    connected = connect(shareIapAction, SIGNAL(triggered()), this, SLOT(shareIap()));
    Q_ASSERT(connected);
       
    if (cmConnected) {
        // Disable operations for connected APs
        openIapAction->setDisabled(true);
        moveIapAction->setDisabled(true);
        deleteIapAction->setDisabled(true);
        shareIapAction->setDisabled(true);
    }
    
    // Can't share uncategorised APs
    if (mDestId == 0)
    {
        shareIapAction->setDisabled(true);
    }
       
    menu->setPreferredPos(position);
    OstTraceFunctionExit0(CPIAPITEM_CREATEITEMMENU_EXIT);
    return menu;
}

/*!
    Helper function for showing error notes.
    
    @param[in] info Info string to be shown in note.
 */
void CpIapItem::showErrorNote(const QString &info)
{
    OstTraceFunctionEntry0(CPIAPITEM_SHOWERRORNOTE_ENTRY);
    HbMessageBox *note = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    note->clearActions();
    note->setAttribute(Qt::WA_DeleteOnClose);
    note->setText(info);
    note->setTimeout(HbPopup::NoTimeout);
    HbAction *errorOk = new HbAction(hbTrId("txt_common_button_ok"));                       
    note->addAction(errorOk);
    note->show();    
    OstTraceFunctionExit0(CPIAPITEM_SHOWERRORNOTE_EXIT);
}
