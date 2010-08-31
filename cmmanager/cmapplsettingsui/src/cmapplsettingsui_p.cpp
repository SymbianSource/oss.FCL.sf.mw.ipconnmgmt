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
* CM Manager Application Settings UI private implementation.  
*/

// System includes

#include <HbTranslator>
#include <HbApplication>
#include <HbLabel>
#include <cmmanager_shim.h>

// User includes

#include "cmradiodialog.h"
#include "cmapplsettingsui_p.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmapplsettingsui_pTraces.h"
#endif


/*!
    \class CmApplSettingsUiPrivate
    \brief Private implementation class for Application Settings UI.

    Displays a radio button list dialog for an application that can be used
    to select a destination or connection method.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param q_pointer Parent object.
 */
CmApplSettingsUiPrivate::CmApplSettingsUiPrivate(CmApplSettingsUi *q_pointer) :
    QObject(q_pointer),
    q_ptr(q_pointer),
    mTranslator(new HbTranslator("cmapplsettingsui")),
    mListItems(
        CmApplSettingsUi::ShowDestinations |
        CmApplSettingsUi::ShowConnectionMethods),
    mSettingData(),
    mDestinationDialog(0),
    mConnMethodDialog(0),
    mSelection(),
    mDestinations(),
    mConnMethods()
{   
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_CMAPPLSETTINGSUIPRIVATE_ENTRY);
    
    // Initialize to default selection
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 0;
        
    // Install also common localization
    mTranslator->loadCommon();

    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_CMAPPLSETTINGSUIPRIVATE_EXIT);
}

/*!
    Destructor.
 */
CmApplSettingsUiPrivate::~CmApplSettingsUiPrivate()
{
    OstTraceFunctionEntry0(DUP1_CMAPPLSETTINGSUIPRIVATE_CMAPPLSETTINGSUIPRIVATE_ENTRY);
    OstTraceFunctionExit0(DUP1_CMAPPLSETTINGSUIPRIVATE_CMAPPLSETTINGSUIPRIVATE_EXIT);
}

/*!
    Options setter. 
     
    @param[in] listItems Dialog list items configuration.
    @param[in] bearerFilter Dialog Connection Method bearer filter.
 */
void CmApplSettingsUiPrivate::setOptions(
    const QFlags<CmApplSettingsUi::SelectionDialogItems> &listItems,
    const QSet<CmApplSettingsUi::BearerTypeFilter> &bearerFilter)
{
    // Store the options
    mListItems = listItems;
    mSettingData.setBearerFilter(bearerFilter);
}

/*!
    Dialog selection setter.
    
    @param[in] selection Dialog selection to set.
 */
void CmApplSettingsUiPrivate::setSelection(
    const CmApplSettingsUi::SettingSelection &selection)
{
    // Set the selection.
    // If the selection is invalid, it is ignored later on,
    // and default is used instead.
    mSelection = selection;
}

/*!
    Dialog selection getter. Returns the current selection.
    
    @return Current dialog selection.
 */
CmApplSettingsUi::SettingSelection CmApplSettingsUiPrivate::selection() const
{
    return mSelection;
}

/*!
    This function runs the dialog asynchronously. Use the setters to
    configure the dialog before running. After the dialog has been closed
    (accepted or cancelled), the signal finished() is emitted.
 */
void CmApplSettingsUiPrivate::open()
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_OPEN_ENTRY);

    // Fetch filtered Destinations and Connection Methods
    bool settingsRead = true;
    if (mListItems.testFlag(CmApplSettingsUi::ShowDestinations)) {
        settingsRead = mSettingData.fetchDestinations(mDestinations);
    }
    if (mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)) {
        settingsRead &= mSettingData.fetchConnMethods(mConnMethods);
    }

    if (!settingsRead) {
        // Settings reading failed
        emitFinished(CmApplSettingsUi::ApplSettingsErrorSettingsRead);
    } else if (mListItems.testFlag(CmApplSettingsUi::ShowDestinations)) {
        // Start with destination view
        runDestinationDialog();
    } else if (mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)) {
        // Start directly with connection method view
        runConnectionMethodDialog();
    } else {
        // Nothing to show, so finish
        emitFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
    }
    
    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_OPEN_EXIT);
}

/*!
    Destination selection dialog execution.
 */
void CmApplSettingsUiPrivate::runDestinationDialog()
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_RUNDESTINATIONDIALOG_ENTRY);

    // Check that there is some content to show:
    // -At least one Destination
    // -Or at least one Connection Method under "Dedicated access point"
    if (mDestinations.isEmpty()
        && (!mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)
            || mConnMethods.isEmpty())) {
        // Nothing to show, so finish
        emitFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
        OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_RUNDESTINATIONDIALOG_EXIT);
        return;
    }

    // Create a new Destination dialog
    delete mDestinationDialog;
    mDestinationDialog = new CmRadioDialog(this);
    mDestinationDialog->setType(CmRadioDialogTypeDestination);
    
    // Add Dedicated access point option if requested & supported
    if (mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)
        && !mConnMethods.isEmpty()) {
        QSet<CmRadioDialogOpt> options;
        options.insert(CmRadioDialogOptDedicatedAP);
        mDestinationDialog->setOptions(options);
    }
    
    // Add destinations to the dialog
    QStringList destinations;
    for (int i = 0; i < mDestinations.count(); i++) {
        destinations << mDestinations[i]->name;
    }
    mDestinationDialog->setItems(destinations);
    
    // Set selection
    int index = 0;          // Default index is the first
    if (mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)
        && mSelection.result == CmApplSettingsUi::SelectionTypeConnectionMethod
        && !mConnMethods.isEmpty()) {
        // Dedicated access point
        index = destinations.count();
    } else {
        int destinationIndex = findDestination(mSelection.id);
        if (destinationIndex != ItemNotFound) {
            index = destinationIndex;
        }
    }
    mDestinationDialog->setSelected(index);
    
    // Execute the dialog
    bool connected = connect(
        mDestinationDialog,
        SIGNAL(finished(bool)),
        this,
        SLOT(destinationDialogResult(bool)));
    Q_ASSERT(connected);
    mDestinationDialog->open();
    
    OstTraceFunctionExit0(DUP1_CMAPPLSETTINGSUIPRIVATE_RUNDESTINATIONDIALOG_EXIT);
}

/*!
    Connection method selection dialog execution.
 */
void CmApplSettingsUiPrivate::runConnectionMethodDialog()
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_RUNCONNECTIONMETHODDIALOG_ENTRY);
    
    // Check that there is at least one Connection Method to show
    if (mConnMethods.count() == 0) {
        // Nothing to show, so finish
        emitFinished(CmApplSettingsUi::ApplSettingsErrorNoContent);
        OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_RUNCONNECTIONMETHODDIALOG_EXIT);
        return;
    }

    // Create a dialog & configure it
    delete mConnMethodDialog;
    mConnMethodDialog = new CmRadioDialog(this);
    mConnMethodDialog->setType(CmRadioDialogTypeAccessPoint);

    // Add connection methods to the dialog
    QStringList connectionMethods;
    for (int i = 0; i < mConnMethods.count(); i++) {
        connectionMethods << mConnMethods[i]->name;
    }
    mConnMethodDialog->setItems(connectionMethods);

    // Set selection
    int index = findConnectionMethod(mSelection.id);
    if (index == ItemNotFound) {
        // Connection Method not found
        index = 0;
    }
    mConnMethodDialog->setSelected(index);
    
    // Execute the dialog
    bool connected = connect(
        mConnMethodDialog,
        SIGNAL(finished(bool)),
        this,
        SLOT(connMethodDialogResult(bool)));
    Q_ASSERT(connected);
    mConnMethodDialog->open();

    OstTraceFunctionExit0(DUP1_CMAPPLSETTINGSUIPRIVATE_RUNCONNECTIONMETHODDIALOG_EXIT);
}

/*!
    Destination search function.
    
    @param destinationId Destination ID.
    
    @return Returns index if found, and ItemNotFound if not. 
 */
int CmApplSettingsUiPrivate::findDestination(uint destinationId) const
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_FINDDESTINATION_ENTRY);
    
    int result = ItemNotFound;
    
    for (int i = 0; i < mDestinations.count(); i++) {
        if (mDestinations[i]->id == destinationId) {
            result = i;
            break;
        }
    }
    
    OstTraceExt2(
        TRACE_NORMAL,
        CMAPPLSETTINGSUIPRIVATE_FINDDESTINATION,
        "CmApplSettingsUiPrivate::findDestination;destinationId=%u;result=%d",
        destinationId,
        result);
    
    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_FINDDESTINATION_EXIT);
    return result;
}

/*!
    Connection Method search function.
    
    @param connMethodId Connection Method ID.
    
    @return Returns index if found, and ItemNotFound if not. 
 */
int CmApplSettingsUiPrivate::findConnectionMethod(uint connMethodId) const
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_FINDCONNECTIONMETHOD_ENTRY);

    int result = ItemNotFound;
    
    for (int i = 0; i < mConnMethods.count(); i++) {
        if (mConnMethods[i]->id == connMethodId) {
            result = i;
            break;
        }
    }
    
    OstTraceExt2(
        TRACE_NORMAL,
        CMAPPLSETTINGSUIPRIVATE_FINDCONNECTIONMETHOD,
        "CmApplSettingsUiPrivate::findConnectionMethod;connMethodId=%u;result=%d",
        connMethodId,
        result);
    
    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_FINDCONNECTIONMETHOD_EXIT);
    return result;
}

/*!
   Emits the completion signal "finished(uint)".
   
   @param status Finishing status code.
 */
void CmApplSettingsUiPrivate::emitFinished(uint status) const
{
    OstTrace1(
        TRACE_NORMAL,
        CMAPPLSETTINGSUIPRIVATE_EMITFINISHED,
        "CmApplSettingsUiPrivate::emitFinished;status=%u",
        status);
    emit q_ptr->finished(status);
}

/*!
    Destination dialog result handling slot.
    
    @param success Destination dialog result - TRUE if accepted.
 */
void CmApplSettingsUiPrivate::destinationDialogResult(bool success)
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_DESTINATIONDIALOGRESULT_ENTRY);
    
    if (success) {
        // Dialog was accepted - user clicked "OK"
        int index = mDestinationDialog->selected();    
        if (mListItems.testFlag(CmApplSettingsUi::ShowConnectionMethods)
            && index == mDestinations.count()) {
            // Dedicated access point was selected
            runConnectionMethodDialog();
        } else {
            // Destination was selected
            mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
            mSelection.id = mDestinations[index]->id;
            emitFinished(CmApplSettingsUi::ApplSettingsErrorNone);
        }
    } else {
        // Dialog was cancelled - user clicked "Cancel"
        emitFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
    }
    
    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_DESTINATIONDIALOGRESULT_EXIT);
}

/*!
    Connection method dialog result handling slot.
    
    @param success Connection method dialog result - TRUE if accepted.
 */
void CmApplSettingsUiPrivate::connMethodDialogResult(bool success)
{
    OstTraceFunctionEntry0(CMAPPLSETTINGSUIPRIVATE_CONNMETHODDIALOGRESULT_ENTRY);
    
    if (success) {
        // Dialog was accepted - user clicked "OK"
        int index = mConnMethodDialog->selected();
        mSelection.result = CmApplSettingsUi::SelectionTypeConnectionMethod;
        mSelection.id = mConnMethods[index]->id;
        emitFinished(CmApplSettingsUi::ApplSettingsErrorNone);
    } else {
        // Dialog was cancelled - user clicked "Cancel"
        emitFinished(CmApplSettingsUi::ApplSettingsErrorCancel);
    }
    
    OstTraceFunctionExit0(CMAPPLSETTINGSUIPRIVATE_CONNMETHODDIALOGRESULT_EXIT);
}
