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
* Dialog implementation for the CM Manager Application Settings UI. 
*
*/

// System includes

#include <HbDialog>
#include <HbLabel>
#include <HbRadioButtonList>
#include <HbAction>
#include <HbDocumentLoader>

// User includes

#include "cmradiodialog.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmradiodialogTraces.h"
#endif


/*!
    \class CmRadioDialog
    \brief Radio dutton dialog class for Application Settings UI.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param parent Parent object.
*/
CmRadioDialog::CmRadioDialog(QObject *parent) :
    QObject(parent),
    mType(CmRadioDialogTypeDestination),
    mLoader(),
    mListItems(),
    mDialog(),
    mHeading(0),
    mList(0),
    mOkAction(0)
{    
    OstTraceFunctionEntry0(CMRADIODIALOG_CMRADIODIALOG_ENTRY);
    
    mLoader = QSharedPointer<HbDocumentLoader>(new HbDocumentLoader());
    loadDocml();
    
    OstTraceFunctionExit0(CMRADIODIALOG_CMRADIODIALOG_EXIT);
}

/*!
    Destructor.
*/
CmRadioDialog::~CmRadioDialog()
{
    OstTraceFunctionEntry0(DUP1_CMRADIODIALOG_CMRADIODIALOG_ENTRY);
    OstTraceFunctionExit0(DUP1_CMRADIODIALOG_CMRADIODIALOG_EXIT);
}

/*!
    Heading setter.
    
    @param type Dialog type.
*/
void CmRadioDialog::setType(CmRadioDialogType type)
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SETTYPE_ENTRY);
    
    mType = type;
    
    OstTraceFunctionExit0(CMRADIODIALOG_SETTYPE_EXIT);
}

/*!
    Options setter.
    
    @param options Dialog options.
 */
void CmRadioDialog::setOptions(const QSet<CmRadioDialogOpt> &options)
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SETOPTIONS_ENTRY);
    
    mOptions = options;

    OstTraceFunctionExit0(CMRADIODIALOG_SETOPTIONS_EXIT);
}

/*!
    Radio button list items setter.
    
    @param list List of radio button item strings.
*/
void CmRadioDialog::setItems(const QStringList &list)
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SETITEMS_ENTRY);
    
    // Store the new items
    mListItems = list;
    
    // Append "Dedicated access point"
    if (mOptions.contains(CmRadioDialogOptDedicatedAP)) {
        mListItems.append(hbTrId("txt_occ_list_dedicated_access_point"));
    }

    // Set the items to the radio list
    mList->setItems(mListItems);
    
    OstTraceFunctionExit0(CMRADIODIALOG_SETITEMS_EXIT);
}
    
/*!
    Setter for current selection.
    
    @param index Radio list item current selection setter. Indexing from zero.
*/
void CmRadioDialog::setSelected(int index)
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SETSELECTED_ENTRY);
    
    OstTrace1(
        TRACE_NORMAL,
        CMRADIODIALOG_SETSELECTED,
        "CmRadioDialog::setSelected;index=%d",
        index);
    
    // The index must be valid
    Q_ASSERT(index < mList->items().count());
    
    // Set the current selection
    mList->setSelected(index);
    
    OstTraceFunctionExit0(CMRADIODIALOG_SETSELECTED_EXIT);
}
    
/*!
    Getter for current selection.
    
    @return Current selection index. Indexing from zero.
*/
int CmRadioDialog::selected() const
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SELECTED_ENTRY);
    
    int selection = mList->selected();
    
    OstTrace1(
        TRACE_NORMAL,
        CMRADIODIALOG_SELECTED,
        "CmRadioDialog::selected;index=%d",
        index);    
    
    OstTraceFunctionExit0(CMRADIODIALOG_SELECTED_EXIT);
    return selection;
}
    
/*!
    Run the dialog asynchronously. When the dialog has finished, the signal
    finished(bool) is sent.
*/
void CmRadioDialog::open()
{
    OstTraceFunctionEntry0(CMRADIODIALOG_EXEC_ENTRY);
    
    setHeading();
    mDialog->open(this, SLOT(dialogClosed(HbAction*)));
    
    OstTraceFunctionExit0(CMRADIODIALOG_EXEC_EXIT);
}

/*!
    Dialog result handling slot.
    
    @param action Selected action. 
 */
void CmRadioDialog::dialogClosed(HbAction *action)
{
    OstTraceFunctionEntry0(CMRADIODIALOG_DIALOGCLOSED_ENTRY);
    
    // Extract the selection status
    bool success;
    if (action == mOkAction) {
        success = true;
    } else {
        success = false;
    }
    emit finished(success);
    
    OstTraceExt1(
        TRACE_NORMAL,
        CMRADIODIALOG_EXEC_RESULT,
        "CmRadioDialog::exec;success=%hhu",
        success);

    OstTraceFunctionExit0(CMRADIODIALOG_DIALOGCLOSED_EXIT);
}

/*!
    Dialog heading setter.
 */
void CmRadioDialog::setHeading()
{
    OstTraceFunctionEntry0(CMRADIODIALOG_SETHEADING_ENTRY);
    
    QString heading;
    
    // Select correct header based on dialog type
    switch (mType) {
    case CmRadioDialogTypeDestination:
        heading = hbTrId("txt_occ_title_network_connection");
        break;
        
    case CmRadioDialogTypeAccessPoint:
        heading = hbTrId("txt_occ_title_access_point");
        break;
        
#ifndef QT_NO_DEBUG
    default:
        // Unsupported dialog type detected
        Q_ASSERT(0);
        break;
#endif        
    }
    
    mHeading->setPlainText(heading);
    
    OstTraceFunctionExit0(CMRADIODIALOG_SETHEADING_EXIT);
}

/*!
    Docml loading for the dialog.
*/
void CmRadioDialog::loadDocml()
{
    OstTraceFunctionEntry0(CMRADIODIALOG_LOADDOCML_ENTRY);
    bool ok = false;

    // Load the common section
    mLoader->load(":/docml/cmradiodialog.docml", &ok);
    Q_ASSERT(ok);

    mDialog = QSharedPointer<HbDialog>(qobject_cast<HbDialog *>(mLoader->findWidget("dialog")));
    Q_ASSERT(mDialog);
    
    mHeading = qobject_cast<HbLabel *>(mLoader->findWidget("heading"));
    Q_ASSERT(mHeading);
    
    mList = qobject_cast<HbRadioButtonList *>(mLoader->findWidget("radioButtonList")); 
    Q_ASSERT(mList);
    
    mOkAction = qobject_cast<HbAction *>(mLoader->findObject("okAction")); 
    Q_ASSERT(mOkAction);
    
    OstTraceFunctionExit0(CMRADIODIALOG_LOADDOCML_EXIT);
}
