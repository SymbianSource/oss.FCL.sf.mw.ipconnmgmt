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

#ifndef APPLSETTESTER_H
#define APPLSETTESTER_H

#include <QObject>
#include <HbMainWindow>

#include "cmapplsettingsui.h"

class HbDocumentLoader;
class HbCheckBox;
class HbAction;
class HbLabel;
class HbLineEdit;
class HbComboBox;

class ApplSettTester : public HbMainWindow
{
    Q_OBJECT
    
public:
    ApplSettTester(QWidget *parent = 0);
    ~ApplSettTester();
        
private slots:
    void runTest();
    void docmlLoad(Qt::Orientation orientation);
    void orientationChange(Qt::Orientation);
    void showResult(uint retval);

private:
    
    HbDocumentLoader *docLoader;
    HbView *view;              
    HbAction *action;
    HbCheckBox *checkboxDest;
    HbCheckBox *checkboxCM;
    HbCheckBox *checkboxWlan;
    HbCheckBox *checkboxGprs;
    HbLabel *labelResult;
    HbLineEdit *selectionId;
    HbComboBox *selectionMode;
    
    CmApplSettingsUi *applSett;
    
    CmApplSettingsUi::SettingSelection mSelection;
};

#endif // APPLSETTESTER_H
