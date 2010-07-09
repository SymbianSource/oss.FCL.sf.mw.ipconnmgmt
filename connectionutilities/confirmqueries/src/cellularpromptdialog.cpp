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
* Description: Prompt Dialog implementation
*
*/

#include <HbTranslator>
#include <QLocale>
#include <QList>
#include <HbLabel>
#include <HbDialog>
#include <HbDocumentLoader>
#include <HbPushButton>
#include <HbView>
#include <HbApplication>
#include "cellularpromptdialog.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cellularpromptdialogTraces.h"
#endif



// The index numbers of the buttons of the dialog
const int firstButtonIndex = 1;
const int middleButtonIndex = 2;
const int cancelButtonIndex = 3;

/**
 * The constructor
 */
CellularPromptDialog::CellularPromptDialog(const QVariantMap &parameters)
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_CELLULARPROMPTDIALOG_ENTRY );

    // Install localization
    mTranslator = QSharedPointer<HbTranslator>(new HbTranslator("cellularpromptdialog"));

    createDialog(parameters);
    mClose = false;
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_CELLULARPROMPTDIALOG_EXIT );
}


/**
 * The construction of the dialog
 */ 
void CellularPromptDialog::createDialog(const QVariantMap &parameters)
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_CREATEDIALOG_ENTRY );
    
    // Set the button and label texts according to the network (home or abroad)
    QString labelText;
    QString button1Text;
    QString button2Text;
    QString button3Text(hbTrId("txt_occ_button_cellular_cancel"));
    
    // There is only one value in the QVariantMap that we are interested in,
    // whether we are in home network or not
    QList<QVariant> list = parameters.values();
    bool homeNetwork = true;
    
    if (list.count() > 0) {
        homeNetwork = list[0].toBool();
    }
    
    if (homeNetwork) {
        labelText = QString(hbTrId("txt_occ_title_connect_to_internet_using_cellular_d"));
        button1Text = QString(hbTrId("txt_occ_button_connect_automatically"));
        button2Text = QString(hbTrId("txt_occ_button_connect_this_time"));
    } else {
        labelText = QString(hbTrId("txt_occ_title_connect_to_internet_in_this_country"));
        button1Text = QString(hbTrId("txt_occ_button_connect_this_time"));
    }
    
    HbDocumentLoader loader;
    bool ok = true;

    if (homeNetwork) {
        loader.load(":/xml/prompt_home.docml", &ok);
    } else {
        loader.load(":/xml/prompt_abroad.docml", &ok);
    }

    if ( !ok ) {
        // send information about cancelling to the observer, the xml loading failed
        cancelPressed();
        OstTraceFunctionExit0( CELLULARPROMPTDIALOG_CREATEDIALOG_EXIT );
        return;
    }
    
    // store the pointer to the dialog in order to be able to delete
    // it in the destructor
    mDialog = qobject_cast<HbDialog*>( loader.findWidget ("dialog"));
    Q_ASSERT(mDialog != NULL);
    
    // fetch the needed widgets, update their texts and connect the
    // clicked signals of the buttons to correct slots.
    HbWidget *container = qobject_cast<HbWidget*>( loader.findWidget ("container"));
    this->setContentWidget(container);
    
    // Set the dialog to be on the screen for 30 seconds, unless
    // the user reacts earlier
    this->setModal(true);
    this->setTimeout(30000);
    this->setDismissPolicy(HbPopup::NoDismiss);
    
    HbLabel* label = qobject_cast<HbLabel*>( loader.findWidget("label") );
    Q_ASSERT(label != NULL);
    label->setPlainText(labelText);
    label->setTextWrapping(Hb::TextWordWrap);

    HbPushButton* firstButton = qobject_cast<HbPushButton*>( loader.findWidget("topButton") );
    Q_ASSERT(firstButton != NULL);
    firstButton->setText(button1Text);
    
    HbPushButton* middleButton = NULL;
    if ( homeNetwork ) {
    middleButton = qobject_cast<HbPushButton*>( loader.findWidget("middleButton") );
    Q_ASSERT(middleButton != NULL); 
    middleButton->setText(button2Text);
    } 
    
    HbPushButton* cancelButton = qobject_cast<HbPushButton*>( loader.findWidget("cancelButton") );
    Q_ASSERT(cancelButton != NULL);
    cancelButton->setText(button3Text);
      
    // Connect the button clicks to slots, assert if connecting fails
    bool connected = connect( firstButton, SIGNAL(clicked()), this, SLOT( firstButtonPressed() ));
    Q_ASSERT(connected == true);
    
    if ( homeNetwork ) {
    connected = connect( middleButton, SIGNAL(clicked()), this, SLOT( middleButtonPressed() ));
    Q_ASSERT(connected == true);
    }
    
    connected = connect( cancelButton, SIGNAL(clicked()), this, SLOT( cancelPressed() ));
    Q_ASSERT(connected == true);
    // Connect the about to close and hide signals, so that we are able to inform 
    // the caller that the dialog was closed, for example due to timeout
    connected = connect(this, SIGNAL(aboutToClose()), this, SLOT(closingDialog()));
    Q_ASSERT(connected == true);
    connected = connect(this, SIGNAL(aboutToHide()), this, SLOT(closingDialog()));
    Q_ASSERT(connected == true);
   
    OstTraceFunctionExit0( DUP1_CELLULARPROMPTDIALOG_CREATEDIALOG_EXIT );
}

/**
 * Destructor
 */
CellularPromptDialog::~CellularPromptDialog()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_DCELLULARPROMPTDIALOG_ENTRY );
    
    // The dialog widgets are deleted as the dialog is deleted
    if (mDialog != NULL) {
        delete mDialog;
    }
    mDialog = NULL;
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_DCELLULARPROMPTDIALOG_EXIT );
}

/**
 * Function is called when the first button is pressed and the 
 * index of the button is emitted
 */
void CellularPromptDialog::firstButtonPressed()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_FIRSTBUTTONPRESSED_ENTRY );
    
    QVariantMap data;
    QVariant variant(firstButtonIndex);
    data.insert("button", variant);
    // emit the data of the selected button and close the dialog
    emit deviceDialogData(data);
    close();

    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_FIRSTBUTTONPRESSED_EXIT );
}

/**
 * Function is called when the second button is pressed and the 
 * index of the button is emitted
 */
void CellularPromptDialog::middleButtonPressed()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_MIDDLEBUTTONPRESSED_ENTRY );
    
    QVariantMap data;
    QVariant variant(middleButtonIndex);
    data.insert("button", variant);
    // emit the data of the selected button and close the dialog
    emit deviceDialogData(data);
    close();
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_MIDDLEBUTTONPRESSED_EXIT );
}

/**
 * Function is called when the third button is pressed and the 
 * index of the button is emitted
 */
void CellularPromptDialog::cancelPressed()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_CANCELPRESSED_ENTRY );
    
    QVariantMap data;
    QVariant variant(cancelButtonIndex);
    data.insert("button", variant);
    // emit the data of the selected button and close the dialog
    emit deviceDialogData(data);
    close();
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_CANCELPRESSED_EXIT );
}

/**
 * Function is called when the dialog is about to close
 */
void CellularPromptDialog::closingDialog()
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_CLOSINGDIALOG_ENTRY );
    
    if (!mClose) {
        mClose = true;
        closeDeviceDialog(false);
    }
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_CLOSINGDIALOG_EXIT );
}


/**
 * Updating the dialog during its showing is not allowed.
 */ 
bool CellularPromptDialog::setDeviceDialogParameters
                (const QVariantMap &parameters)
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_SETDEVICEDIALOGPARAMETERS_ENTRY );
    
    Q_UNUSED(parameters)
    // changing the dialog after presenting it is not supported.
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_SETDEVICEDIALOGPARAMETERS_EXIT );
    return true;
}

/**
 * Not supported, 0 always returned
 */
int CellularPromptDialog::deviceDialogError() const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_DEVICEDIALOGERROR_ENTRY );
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_DEVICEDIALOGERROR_EXIT);
    return 0;
}

/**
 * Dialog is closed and the signal about closing is emitted
 */
void CellularPromptDialog::closeDeviceDialog(bool byClient)
{   
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_CLOSEDEVICEDIALOG_ENTRY );
    
    Q_UNUSED(byClient)
    close();
    // If the user closes the dialog, then the deviceDialogClosed is emitted
    emit deviceDialogClosed();
    
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_CLOSEDEVICEDIALOG_EXIT );
}

/**
 * This dialog widget is returned to the caller
 */
HbPopup *CellularPromptDialog::deviceDialogWidget() const
{
    OstTraceFunctionEntry0( CELLULARPROMPTDIALOG_DEVICEDIALOGWIDGET_ENTRY );
    OstTraceFunctionExit0( CELLULARPROMPTDIALOG_DEVICEDIALOGWIDGET_EXIT );
    
    return const_cast<CellularPromptDialog*>(this);
}

