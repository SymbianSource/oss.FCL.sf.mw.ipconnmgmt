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

#ifndef CMRADIODIALOG_H
#define CMRADIODIALOG_H

// System includes

#include <QObject>
#include <QSet>
#include <QStringList>
#include <QSharedPointer>

// User includes

// Forward declarations

class HbDialog;
class HbLabel;
class HbRadioButtonList;
class HbAction;
class HbDocumentLoader;
class TestCmApplSettingsUi;

// External data types

// Constants

// Supported dialog types
enum CmRadioDialogType {
    CmRadioDialogTypeDestination = 1,       //!< Destination selection dialog
    CmRadioDialogTypeAccessPoint            //!< Access point selection dialog
};

// Supported options
enum CmRadioDialogOpt {
    CmRadioDialogOptDedicatedAP = 1         //!< Append "Dedicated access point"
};

// Class declaration

class CmRadioDialog : public QObject
{
    Q_OBJECT

public:

    // Data types

    explicit CmRadioDialog(QObject *parent = 0);
    
    ~CmRadioDialog();
    
    void setType(CmRadioDialogType type);
    
    void setOptions(const QSet<CmRadioDialogOpt> &options);
    
    void setItems(const QStringList &list);
    
    void setSelected(int index);
    
    int selected() const;
    
    void open();
    
signals:

    /*!
     * Signal used for informing that the dialog has finished.
     * 
     * @param[in] success Dialog run success. This is true if OK was pressed.
     */
    void finished(bool success);

public slots:

    void dialogClosed(HbAction *action);

protected:

protected slots:

private:

    void setHeading();

private slots:
    
    void loadDocml();

private: // data

    // Owned data
    CmRadioDialogType mType;                    //!< Radio dialog type
    QSet<CmRadioDialogOpt> mOptions;            //!< Radio dialog options
    QSharedPointer<HbDocumentLoader> mLoader;   //!< Dialog document loader
    QStringList mListItems;                     //!< Dialog radio button items
    QSharedPointer<HbDialog> mDialog;           //!< Dialog reference
    
    // Not owned data
    HbLabel *mHeading;                          //!< Dialog title
    HbRadioButtonList *mList;                   //!< Dialog's radio button list
    HbAction *mOkAction;                        //!< Dialog's OK button action
    
    // Friend classes
    
    // Test class needs direct access
    friend class TestCmApplSettingsUi;
};

#endif // CMRADIODIALOG_H
