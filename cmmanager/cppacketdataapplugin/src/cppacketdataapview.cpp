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
* Control Panel packet data AP settings view header file.  
*
*/

// System includes
#include <QString>
#include <QStringList>
#include <QVariant>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormViewItem>
#include <HbLineEdit>
#include <HbEditorInterface>
#include <HbUrlFilter>
#include <HbPopup>
#include <HbMessageBox>
#include <HbAction>
#include <cpsettingformitemdata.h>
#include <cmconnectionmethod_shim.h>

// User includes
#include "cppacketdataapplugin.h"
#include "cppacketdataapview.h"
#include "cppacketdataapadvancedview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cppacketdataapviewTraces.h"
#endif

/*!
    \class CpPacketDataApView
    \brief Implements the settings view for packet data bearer access points.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpPacketDataApView::CpPacketDataApView(
    CmConnectionMethodShim *cmConnectionMethod,
    QGraphicsItem *parent) :
        CpBaseSettingView(0, parent),
        mForm(0),
        mModel(0),
        mApSettingsGroupItem(0),
        mConnectionNameItem(0),
        mAccessPointNameItem(0),
        mUserNameItem(0),
        mPromptForAuthItem(0),
        mPasswordItem(0),
        mAuthenticationItem(0),
        mHomepageItem(0),
        mAdvancedSettingsAction(0),
        mCmConnectionMethod(cmConnectionMethod),
        mMessageBox(0)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_CPPACKETDATAAPVIEW_ENTRY);
    
    HbMenu *menu = this->menu();
    mAdvancedSettingsAction = menu->addAction(
        hbTrId("txt_occ_opt_advanced_settings"));
    bool status = connect(
        menu,
        SIGNAL(triggered(HbAction*)),
        this,
        SLOT(menuActionTriggered(HbAction*)));
    Q_ASSERT(status);

    // Construct packet data AP settings UI
    mForm = settingForm();
    if (mForm) {
        mModel = new HbDataFormModel(mForm);

        // Add access point settings group
        createAccessPointSettingsGroup();
        
        mForm->setModel(mModel);
        
        status = connect(
            mForm,
            SIGNAL(itemShown(const QModelIndex)),
            this,
            SLOT(setEditorPreferences(const QModelIndex)));
        Q_ASSERT(status);

        // Expand Access point settings group
        mForm->setExpanded(mModel->indexFromItem(mApSettingsGroupItem), true);
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_CPPACKETDATAAPVIEW_EXIT);
}

/*!
    Destructor.
*/
CpPacketDataApView::~CpPacketDataApView()
{
    OstTraceFunctionEntry0(DUP1_CPPACKETDATAAPVIEW_CPPACKETDATAAPVIEW_ENTRY);
    
    OstTraceFunctionExit0(DUP1_CPPACKETDATAAPVIEW_CPPACKETDATAAPVIEW_EXIT);
}

/*!
    Adds settings group with all settings items to the model.
*/
void CpPacketDataApView::createAccessPointSettingsGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_CREATEACCESSPOINTSETTINGSGROUP_ENTRY);
    
    // Access point settings group
    mApSettingsGroupItem = new HbDataFormModelItem(
        HbDataFormModelItem::GroupItem, 
        hbTrId("txt_occ_subhead_access_point_settings"));
    mModel->appendDataFormItem(mApSettingsGroupItem);
    
    // Connection name
    mConnectionNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_connection_name"));
    // Connect signal and add item to group
    mForm->addConnection(
        mConnectionNameItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(connectionNameChanged()));
    mApSettingsGroupItem->appendChild(mConnectionNameItem);
    
    // Access point name
    mAccessPointNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_access_point_name"));
    // Connect signal and add item to group
    mForm->addConnection(
        mAccessPointNameItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(accessPointNameChanged()));
    mApSettingsGroupItem->appendChild(mAccessPointNameItem);
    
    // User name
    mUserNameItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_user_name"));
    // Connect signal and add item to group
    mForm->addConnection(
        mUserNameItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(userNameChanged()));
    mApSettingsGroupItem->appendChild(mUserNameItem);
    
    // Prompt for password
    mPromptForAuthItem = new CpSettingFormItemData(
        HbDataFormModelItem::CheckBoxItem,
        hbTrId("txt_occ_setlabel_password"));
    mPromptForAuthItem->setContentWidgetData("text",
        hbTrId("txt_occ_setlabel_password_val_prompt"));
    // Construct map to link item values to setting values
    mPromptForAuthMap.insert(Qt::Unchecked, false); // Do not prompt
    mPromptForAuthMap.insert(Qt::Checked, true); // Prompt for password
    // Connect signal and add item to group
    mForm->addConnection(
        mPromptForAuthItem,
        SIGNAL(stateChanged(int)),
        this,
        SLOT(promptForAuthChanged(int)));
    mApSettingsGroupItem->appendChild(mPromptForAuthItem);
    
    // Password
    mPasswordItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_password"));
    mPasswordItem->setContentWidgetData("echoMode", "Password");
    // Connect signal and add item to group
    mForm->addConnection(
        mPasswordItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(passwordChanged()));
    mApSettingsGroupItem->appendChild(mPasswordItem);
    
    // Authentication
    mAuthenticationItem = new CpSettingFormItemData(
        HbDataFormModelItem::ComboBoxItem,
        hbTrId("txt_occ_setlabel_authentication"));
    // Add items to combobox
    QStringList authenticationItems;
    authenticationItems
        << hbTrId("txt_occ_setlabel_authentication_val_normal")
        << hbTrId("txt_occ_setlabel_authentication_val_secure");
    mAuthenticationItem->setContentWidgetData("items", authenticationItems);
    // Construct map to link item indexes to setting values
    mAuthenticationMap.insert(0, false); // normal (plaintext allowed)
    mAuthenticationMap.insert(1, true); // secure (plaintext disabled)
    // Connect signal and add item to group
    mForm->addConnection(
        mAuthenticationItem,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(authenticationChanged(int)));
    mApSettingsGroupItem->appendChild(mAuthenticationItem);
    
    // Homepage
    mHomepageItem = new CpSettingFormItemData(
        HbDataFormModelItem::TextItem,
        hbTrId("txt_occ_setlabel_homepage"));
    // Connect signal and add item to group
    mForm->addConnection(
        mHomepageItem,
        SIGNAL(editingFinished()),
        this,
        SLOT(homepageChanged()));
    mApSettingsGroupItem->appendChild(mHomepageItem);
    
    // Read settings from CommsDat and update widgets
    updateAccessPointSettingsGroup();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_CREATEACCESSPOINTSETTINGSGROUP_EXIT);
}

/*!
    Reads attribute values and updates "Access point settings" group settings.
*/
void CpPacketDataApView::updateAccessPointSettingsGroup()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_UPDATEACCESSPOINTSETTINGSGROUP_ENTRY);

    // Get attributes from CommsDat and set values to UI widgets
    
    // Connection name
    QString connectionName = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmName);
    mConnectionNameItem->setContentWidgetData("text", connectionName);
    
    // Access point name
    QString accessPointName = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::PacketDataAPName);
    mAccessPointNameItem->setContentWidgetData("text", accessPointName);
    
    // User name
    QString userName = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::PacketDataIFAuthName);
    mUserNameItem->setContentWidgetData("text", userName);
    
    // Prompt for password
    bool promptForAuth = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::PacketDataIFPromptForAuth);
    mPromptForAuthItem->setContentWidgetData("checkState",
        mPromptForAuthMap.key(promptForAuth));
    if (promptForAuth) {
        mPasswordItem->setEnabled(false);
    } else {
        mPasswordItem->setEnabled(true);
    }
    
    // Password
    QString password = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::PacketDataIFAuthPass);
    mPasswordItem->setContentWidgetData("text", password);

    // Authentication
    bool disablePlainText = mCmConnectionMethod->getBoolAttribute(
        CMManagerShim::PacketDataDisablePlainTextAuth);
    mAuthenticationItem->setContentWidgetData("currentIndex",
        mAuthenticationMap.key(disablePlainText));
    
    // Homepage
    QString homepage = mCmConnectionMethod->getStringAttribute(
        CMManagerShim::CmStartPage);
    mHomepageItem->setContentWidgetData("text", homepage);
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_UPDATEACCESSPOINTSETTINGSGROUP_EXIT);
}

/*!
    Shows message box with "OK" button using given text.
*/
void CpPacketDataApView::showMessageBox(
    HbMessageBox::MessageBoxType type,
    const QString &text)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_SHOWMESSAGEBOX_ENTRY);
    
    // Create a message box
    mMessageBox = QSharedPointer<HbMessageBox>(new HbMessageBox(type));
    mMessageBox->setText(text);
    mMessageBox->setModal(true);
    mMessageBox->setTimeout(HbPopup::NoTimeout);
    mMessageBox->open();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_SHOWMESSAGEBOX_EXIT);
}

/*!
    Tries to update connection method changes to CommsDat.
    Returns "true" if success, "false" if some error happened. 
*/
bool CpPacketDataApView::tryUpdate()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_TRYUPDATE_ENTRY);
    
    // Try update
    try {
        mCmConnectionMethod->update();
    }
    catch (const std::exception&) {
        // Handle error
        handleUpdateError();
        
        OstTraceFunctionExit0(CPPACKETDATAAPVIEW_TRYUPDATE_EXIT);
        return false;
    }

    OstTraceFunctionExit0(DUP1_CPPACKETDATAAPVIEW_TRYUPDATE_EXIT);
    return true;
}

/*!
    Handles failed CommsDat update.
 */
void CpPacketDataApView::handleUpdateError()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_HANDLEUPDATEERROR_ENTRY);
    
    // Show error note to user
    showMessageBox(
        HbMessageBox::MessageTypeWarning,
        hbTrId("txt_occ_info_unable_to_save_setting"));
    // Reload settings from CommsDat and update UI
    try {
        mCmConnectionMethod->refresh();
    }
    catch (const std::exception&) {
        // Ignore error from refresh. Most likely this will not happen, but
        // if it does, there isn't very much we can do.
        OstTrace0(
            TRACE_ERROR,
            CPPACKETDATAAPVIEW_HANDLEUPDATEERROR,
            "Refresh failed");
    };
    updateAccessPointSettingsGroup();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_HANDLEUPDATEERROR_EXIT);
}

/*!
    Updates connection name to CommsDat.
*/
void CpPacketDataApView::connectionNameChanged()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_CONNECTIONNAMECHANGED_ENTRY);
    
    QString connectionName =
        mConnectionNameItem->contentWidgetData("text").toString();
    if (!connectionName.isEmpty()) {
        // Update to CommsDat
        mCmConnectionMethod->setStringAttribute(
            CMManagerShim::CmName,
            connectionName);
        if (tryUpdate()) {
            // Update successful
            // Read name because in case the name already exists it will
            // be made unique by CMManager
            connectionName = mCmConnectionMethod->getStringAttribute(
                CMManagerShim::CmName);
            mConnectionNameItem->setContentWidgetData("text", connectionName);
        }
    } else {
        // Inform user of invalid name
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_name"));

        // Empty name not allowed, revert back to old value in CommsDat
        connectionName = mCmConnectionMethod->getStringAttribute(
            CMManagerShim::CmName);
        mConnectionNameItem->setContentWidgetData("text", connectionName);
     }
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_CONNECTIONNAMECHANGED_EXIT);
}

/*!
    Updates access point name to CommsDat.
*/
void CpPacketDataApView::accessPointNameChanged()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_ACCESSPOINTNAMECHANGED_ENTRY);
    
    // Update to CommsDat
    QString accessPointName =
        mAccessPointNameItem->contentWidgetData("text").toString();
    if (!accessPointName.isEmpty()) {
        mCmConnectionMethod->setStringAttribute(
            CMManagerShim::PacketDataAPName,
            accessPointName);
        (void)tryUpdate();
    } else {
        // Inform user of invalid name
        showMessageBox(
            HbMessageBox::MessageTypeInformation,
            hbTrId("txt_occ_info_invalid_name"));
    
        // Empty name not allowed, revert back to old value in CommsDat
        accessPointName = mCmConnectionMethod->getStringAttribute(
            CMManagerShim::PacketDataAPName);
        mAccessPointNameItem->setContentWidgetData("text", accessPointName);
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_ACCESSPOINTNAMECHANGED_EXIT);
}

/*!
    Updates user name to CommsDat.
*/
void CpPacketDataApView::userNameChanged()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_USERNAMECHANGED_ENTRY);
    
    // Update to CommsDat
    QVariant userName = mUserNameItem->contentWidgetData("text");
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::PacketDataIFAuthName,
        userName.toString());
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_USERNAMECHANGED_EXIT);
}

/*!
    Updates prompt for password setting to CommsDat.
*/
void CpPacketDataApView::promptForAuthChanged(int state)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_PROMPTFORAUTHCHANGED_ENTRY);
    
    // If prompt for auth is selected, password edit should be disabled.
    if (state == Qt::Checked) {
        mPasswordItem->setEnabled(false);
    } else {
        mPasswordItem->setEnabled(true);
    }
    // Update to CommsDat
    mCmConnectionMethod->setBoolAttribute(
        CMManagerShim::PacketDataIFPromptForAuth,
        mPromptForAuthMap.value(static_cast<Qt::CheckState>(state)));
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_PROMPTFORAUTHCHANGED_EXIT);
}

/*!
    Updates password to CommsDat.
*/
void CpPacketDataApView::passwordChanged()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_PASSWORDCHANGED_ENTRY);
    
    // Update to CommsDat
    QVariant password = mPasswordItem->contentWidgetData("text");
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::PacketDataIFAuthPass,
        password.toString());
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_PASSWORDCHANGED_EXIT);
}

/*!
    Updates authentication setting to CommsDat.
*/
void CpPacketDataApView::authenticationChanged(int index)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_AUTHENTICATIONCHANGED_ENTRY);
    
    // Update to CommsDat
    mCmConnectionMethod->setBoolAttribute(
        CMManagerShim::PacketDataDisablePlainTextAuth,
        mAuthenticationMap.value(index));
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_AUTHENTICATIONCHANGED_EXIT);
}

/*!
    Updates homepage to CommsDat.
*/
void CpPacketDataApView::homepageChanged()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_HOMEPAGECHANGED_ENTRY);
    
    // Update to CommsDat
    QString homepage = mHomepageItem->contentWidgetData("text").toString();
    mCmConnectionMethod->setStringAttribute(
        CMManagerShim::CmStartPage, homepage);
    (void)tryUpdate();
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_HOMEPAGECHANGED_EXIT);
}

/*!
    Handles menu actions.
*/
void CpPacketDataApView::menuActionTriggered(HbAction *action)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_MENUACTIONTRIGGERED_ENTRY);
    
    if (action == mAdvancedSettingsAction) {
        // "Advanced settings" selected
        HbMainWindow *mainWindow = this->mainWindow();

        // Create the advanced settings view
        HbView *newView = new CpPacketDataApAdvancedView(mCmConnectionMethod);
        // Connect signal to return back to the previous view
        bool status = QObject::connect(
            newView,
            SIGNAL(aboutToClose()),
            this,
            SLOT(restoreCurrentView()));
        Q_ASSERT(status);

        // Show the advanced settings view
        mainWindow->addView(newView);
        mainWindow->setCurrentView(newView);
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_MENUACTIONTRIGGERED_EXIT);
}

/*!
    Deletes the current view and sets this view as the current one.
*/
void CpPacketDataApView::restoreCurrentView()
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_RESTORECURRENTVIEW_ENTRY);
    
    HbMainWindow *mainWindow = this->mainWindow();
    
    // Remove the previous view and delete it
    HbView *prevView = mainWindow->currentView();
    mainWindow->removeView(prevView);
    prevView->deleteLater();
    
    // Set this view on top
    mainWindow->setCurrentView(this);
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_RESTORECURRENTVIEW_EXIT);
}

/*!
    Sets editor preferences for all HbLineEdit items.
*/
void CpPacketDataApView::setEditorPreferences(const QModelIndex modelIndex)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPVIEW_SETEDITORPREFERENCES_ENTRY);
    
    HbDataFormViewItem *viewItem = mForm->dataFormViewItem(modelIndex);
    HbDataFormModelItem *modelItem = mModel->itemFromIndex(modelIndex);
    
    if (modelItem == mConnectionNameItem
        || modelItem == mAccessPointNameItem
        || modelItem == mUserNameItem
        || modelItem == mPasswordItem
        || modelItem == mHomepageItem ) {
        // HbLineEdit items, get editor and editor interface
        HbLineEdit *edit = qobject_cast<HbLineEdit *>
            (viewItem->dataItemContentWidget());
        HbEditorInterface editInterface(edit);
        
        if (modelItem == mConnectionNameItem) {
            // Setup editor for connection name
            editInterface.setConstraints(HbEditorConstraintLatinAlphabetOnly);
            edit->setInputMethodHints(Qt::ImhNoPredictiveText); 
            edit->setMaxLength(CMManagerShim::CmNameLength);
        } else if (modelItem == mAccessPointNameItem) {
            // Setup editor for packet data AP name
            editInterface.setInputMode(HbInputModeNone);
            editInterface.setConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setLocalDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::PacketDataAPNameLength);
        } else if (modelItem == mUserNameItem) {
            // Setup editor for user name
            editInterface.setInputMode(HbInputModeNone);
            editInterface.setConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setEditorClass(HbInputEditorClassUsername);
            editInterface.setLocalDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::PacketDataIFAuthNameLength);
        } else if (modelItem == mPasswordItem) {
            // Setup editor for password
            editInterface.setInputMode(HbInputModeNone);
            editInterface.setConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setEditorClass(HbInputEditorClassPassword);
            editInterface.setLocalDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::PacketDataIFAuthPassLength);
        } else { /* mHomepageItem */
            // Setup editor for URL
            editInterface.setInputMode(HbInputModeNone);
            editInterface.setConstraints(HbEditorConstraintLatinAlphabetOnly);
            editInterface.setFilter(HbUrlFilter::instance());
            editInterface.setEditorClass(HbInputEditorClassUrl);
            editInterface.setLocalDigitType(HbDigitTypeNone);
            edit->setInputMethodHints(
                Qt::ImhNoPredictiveText
                | Qt::ImhPreferLowercase);
            edit->setMaxLength(CMManagerShim::CmStartPageLength);
        }
    }
    
    OstTraceFunctionExit0(CPPACKETDATAAPVIEW_SETEDITORPREFERENCES_EXIT);
}
