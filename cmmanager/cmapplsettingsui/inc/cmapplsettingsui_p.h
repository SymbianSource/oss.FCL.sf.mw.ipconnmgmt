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
* CM Manager Application Settings UI private interface.  
*
*/

#ifndef CMAPPLSETTINGSUI_P_H
#define CMAPPLSETTINGSUI_P_H

// System includes

#include <QObject>
#include <QFlags>
#include <QSet>
#include <QList>
#include <QSharedPointer>

// User includes

#include "cmconnectionmethod_shim.h"
#include "cmdestination_shim.h"
#include "cmsettingdata.h"
#include "cmapplsettingsui.h"

// Forward declarations

class QTranslator;
class CmManagerShim;
class CmRadioDialog;
class TestCmApplSettingsUi;

// External data types

// Constants

// Class declaration

class CmApplSettingsUiPrivate : QObject
{
    Q_OBJECT

public:

    // Data types

    explicit CmApplSettingsUiPrivate(CmApplSettingsUi *q_pointer);
    
    virtual ~CmApplSettingsUiPrivate();

    void setOptions(
        const QFlags<CmApplSettingsUi::SelectionDialogItems> &listItems,
        const QSet<CmApplSettingsUi::BearerTypeFilter> &bearerFilter);
    
    void setSelection(
        const CmApplSettingsUi::SettingSelection &selection);
    
    CmApplSettingsUi::SettingSelection selection() const;
    
    void open();
    
signals:

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CmApplSettingsUiPrivate)

    void runDestinationDialog();
    
    void runConnectionMethodDialog();
        
    int findDestination(uint destinationId) const;

    int findConnectionMethod(uint connMethodId) const;

    void emitFinished(uint status) const;
    
private slots:

    void destinationDialogResult(bool success);
    
    void connMethodDialogResult(bool success);

private: // data
    
    //! "Not found" search index value. 
    static const int ItemNotFound = -1;

    // General member data
    CmApplSettingsUi *q_ptr;                //!< Public implementation
    QSharedPointer<CmManagerShim> mCmManagerShim;   //!< CM Manager client instance
    QTranslator *mAppTranslator;            //!< Translator for application text ID's       
    QTranslator *mCommonTranslator;         //!< Translator for common text ID's

    // Configuration
    QFlags<CmApplSettingsUi::SelectionDialogItems> mListItems;  //!< Wanted dialog type's
    
    // State data
    CmSettingData mSettingData;             //!< Setting data
    CmRadioDialog *mDestinationDialog;      //!< Destination dialog
    CmRadioDialog *mConnMethodDialog;       //!< Connection method dialog
    CmApplSettingsUi::SettingSelection mSelection;          //!< Current selection
    QList< QSharedPointer<CmSettingData::Destination> > mDestinations;  //! Destinations
    QList< QSharedPointer<CmSettingData::ConnMethod> > mConnMethods;    //! Connection methods
    
    // Friend classes
    
    // Test class needs direct access
    friend class TestCmApplSettingsUi;
};

#endif // CMAPPLSETTINGSUI_P_H
