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
* Dialog implementaton for test application 
* for testing CM Manager Application Settings Ui.
*/

#include <QGraphicsWidget>
#include <QString>

#include <HbDialog>
#include <HbApplication>
#include <HbMainWindow>
#include <HbView>
#include <HbDocumentLoader>
#include <HbInstance>
#include <HbAction>
#include <HbCheckBox>
#include <HbLabel>
#include <HbLineEdit>
#include <HbComboBox>


#include "cmapplsettingsui.h"
#include "cmmanagerdefines_shim.h"

#include "applsettester.h"

#define TEST_DOCML ":/docml/tst_applsettingsui.docml"

ApplSettTester::ApplSettTester(QWidget *parent) : HbMainWindow(parent)
{
    applSett = new CmApplSettingsUi(this);

    // Clear initial selection
    mSelection.result = CmApplSettingsUi::SelectionTypeDestination;
    mSelection.id = 0;
    
    bool status = connect(
        this,
        SIGNAL(orientationChanged(Qt::Orientation)),
        this,
        SLOT(orientationChange(Qt::Orientation)));
    Q_ASSERT(status);
    
    docLoader = new HbDocumentLoader();
    docmlLoad(this->orientation()); 
}

ApplSettTester::~ApplSettTester()
{
    delete docLoader;
}

void ApplSettTester::docmlLoad(Qt::Orientation orientation)
{
    bool ok = true;
    
    docLoader->reset();
    // First load the common section
    docLoader->load(TEST_DOCML, &ok);

    if (ok) {    
        // Then load the orientation specific section
        orientationChange(orientation);
        
        QGraphicsWidget *widget = docLoader->findWidget(QString("testApplView"));
        if (widget) {
            view = qobject_cast<HbView*>(widget);
            this->addView(view);
            this->setCurrentView(view);

            checkboxDest = qobject_cast<HbCheckBox*>(docLoader->findWidget("checkbox_dest"));
            Q_ASSERT(checkboxDest);
            checkboxCM = qobject_cast<HbCheckBox*>(docLoader->findWidget("checkbox_cm"));
            Q_ASSERT(checkboxCM);
            
            checkboxWlan = qobject_cast<HbCheckBox*>(docLoader->findWidget("checkbox_wlan"));
            Q_ASSERT(checkboxWlan);
            checkboxGprs = qobject_cast<HbCheckBox*>(docLoader->findWidget("checkbox_gprs"));
            Q_ASSERT(checkboxGprs);
           
            selectionMode = qobject_cast<HbComboBox*>(docLoader->findWidget("mode"));
            Q_ASSERT(selectionMode);
            selectionId = qobject_cast<HbLineEdit*>(docLoader->findWidget("id"));
            Q_ASSERT(selectionId);
           
            labelResult = qobject_cast<HbLabel*>(docLoader->findWidget("result"));
            Q_ASSERT(labelResult);
            
            action = qobject_cast<HbAction *>(docLoader->findObject("testAction"));
            Q_ASSERT(action);
            
            bool status = connect(action, SIGNAL(triggered()), this, SLOT(runTest()));
            Q_ASSERT(status);
        }
    }
}

void ApplSettTester::orientationChange(Qt::Orientation orientation)
{
    bool ok = true;
    if (orientation == Qt::Vertical) {
        // reading "portrait" section
        docLoader->load(TEST_DOCML, "portrait", &ok);
    } else {
        // reading "landscape" section
        docLoader->load(TEST_DOCML, "landscape", &ok);
    }   
}

void ApplSettTester::runTest()
{
    QFlags<CmApplSettingsUi::SelectionDialogItems> listItems;
    QSet<CmApplSettingsUi::BearerTypeFilter> filter;
    
    // Dialog type selection
    if (checkboxDest->checkState()) {
        listItems |= CmApplSettingsUi::ShowDestinations;
    }
    if (checkboxCM->checkState()) {
        listItems |= CmApplSettingsUi::ShowConnectionMethods;
    }

    // Bearer filtering
    if (checkboxWlan->checkState()) {
        filter |= CMManagerShim::BearerTypeWlan;
    }
    if (checkboxGprs->checkState()) {
        filter |= CMManagerShim::BearerTypePacketData;
    }
    
    // Setting selection value
    mSelection.result = 
        (CmApplSettingsUi::SelectionType)selectionMode->currentIndex();
    mSelection.id = selectionId->text().toUInt();;

    applSett->setOptions(listItems, filter);
    applSett->setSelection(mSelection);
    
    // Connect finished(uint) signal and handle result via it
    connect(applSett, SIGNAL(finished(uint)), this, SLOT(showResult(uint)));
    
    // Start CmApplSettingsUi
    applSett->open();
}

void ApplSettTester::showResult(uint retval)
{
    if (retval == CmApplSettingsUi::ApplSettingsErrorNone) {
        mSelection = applSett->selection();
        selectionMode->setCurrentIndex(mSelection.result);    
        selectionId->setText(QString::number(mSelection.id));
    }
    labelResult->setNumber(int(retval));
}
