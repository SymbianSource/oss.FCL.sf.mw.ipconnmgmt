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
* This items represents one Destination in UI.
*
*/

#ifndef CPDESTINATIONENTRYITEM_H
#define CPDESTINATIONENTRYITEM_H

// System includes
#include <QSharedPointer>
#include <cpsettingformentryitemdata.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations
class QString;
class HbMenu;
class HbView;
class HbListWidget;
class HbListWidgetItem;
class HbMainWindow;
class HbAction;
class HbInputDialog;
class CpItemDataHelper;
class CpIapItem;
class CmConnectionMethodShim;
class CmManagerShim;

// External data types

// Constants

// Class declaration
class CpDestinationEntryItemData : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
    explicit CpDestinationEntryItemData(CpItemDataHelper &itemDataHelper);
    virtual ~CpDestinationEntryItemData();
    
    int destinationId();
    void setDestinationId( int destId );
    QString destinationName();
    void setDestinationName(const QString destinationName);
    
signals:
    void destChanged();
    
public slots:
    void updateDestinationView();
    void showItemMenu(QPointF position);
    void openDestination();
    void renameDestination();
    void confirmDestinationDelete();
    void deleteDestination();
    void activateArrangeMode();
    void viewDone();
    void viewCancel();
    void updateIndex(HbListWidgetItem *widgetItem);
    void saveNewDestinationName();
    
protected:
    
protected slots:
    
private:
    virtual CpBaseSettingView *createSettingView() const;
    void fetchReferencedAps(
        QList<QSharedPointer<CmConnectionMethodShim> > &apList, const CmManagerShim *cmm) const;
    bool isDestinationNameValid(QString &destination, const CmManagerShim *cmm);
    void createArrangeModeView(HbView *view);
    void constructSettingView(CpBaseSettingView *view) const;
    HbMenu *createItemMenu(
        CMManagerShim::CmmProtectionLevel protLvl,
        const QPointF position);
    void lauchNewDestinationNameQuery();
    void showRenameError(const QString &info);
    void showErrorNote(const QString &info);
            
private slots:
    
private: // data
    
    //! Helper for connecting signals to underlying widgets
    CpItemDataHelper    *mItemDataHelper;
    //! Unique Destination ID
    int                 mDestinationId;
    //! Destination Name
    QString             mDestinationName;
    //! List of access points contained by this object
    QList<CpIapItem*>   *mAps;
    //! Pointer to CmManager for commsdat operations
    CmManagerShim       *mCmm;
    //! Previous view when arrange mode view is shown
    HbView              *mPreView;
    //! List of access points for arrange mode
    HbListWidget        *mList;
    //! New Destination name query dialog
    HbInputDialog      *mDialog;
    //! New Destination name query's OK action
    HbAction           *mOkAction;
};

#endif /* CPDESTINATIONENTRYITEM_H */
