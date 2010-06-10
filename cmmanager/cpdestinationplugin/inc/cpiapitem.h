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
*
*/

#ifndef CPIAPITEM_H
#define CPIAPITEM_H

// System includes
#include <QObject>
#include <QPoint>
#include <QModelIndex>
#include <cpsettingformentryitemdata.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations
class HbDialog;
class HbRadioButtonList;
class HbAction;
class HbMenu;
class CmManagerShim;
class CpBearerApPluginInterface;

// External data types

// Constants

// Class declaration
class CpIapItem : public CpSettingFormEntryItemData
    {
    Q_OBJECT
    
public:
    CpIapItem(
        CpItemDataHelper &itemDataHelper, 
        int iapId, 
        const QString &iapName, 
        int destId,
        bool apProtected,
        CpBearerApPluginInterface *bearerPlugin);
    
    ~CpIapItem();
    
signals:
    void iapChanged();
    
public slots:
    void showItemMenu(QPointF position);
    void moveIap();
    void showDeleteConfirmation();
    void shareIap();
    void queryDialogClosed();
    void deleteConfirmed();
    void updateIap();
    void openIap();

protected:
    
protected slots:

private:
    virtual CpBaseSettingView *createSettingView() const;
    void queryDestination();
    void saveMove(int id);
    void saveShare(int id);
    bool isCmManagerAvailable();
    HbMenu *createItemMenu(
        bool cmConnected,
        const QPointF &position);
    void showErrorNote(const QString &info);

private slots:

private: // data 
    //! Unique ID for access point
    int                         mIapId;
    //! Access point name
    QString                     mIapName;
    //! ID of the destination referencing this access point
    int                         mDestId;
    //! Helper for connecting signals to underlying widgets
    CpItemDataHelper            *mItemDataHelper;
    //! Pointer to CmManager for commsdat operations
    CmManagerShim               *mCmm;
    //! Plugin that implements settings view for this access point
    CpBearerApPluginInterface   *mBearerPlugin;
    
    //! Variable showing if access point move transaction is ongoing
    bool                        mMoveOngoing;
    //! Target destination query dialog. Used for move and share
    HbDialog                    *mDialog;
    //! Dialog's OK action
    HbAction                    *mOk;
    //! List of available destinations' IDs
    QList<uint>                 mDestinationList;
    //! List of available destinations
    HbRadioButtonList           *mList;
};

#endif // CPIAPITEM_H
