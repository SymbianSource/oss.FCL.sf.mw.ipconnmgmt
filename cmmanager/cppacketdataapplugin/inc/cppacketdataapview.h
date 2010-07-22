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

#ifndef CPPACKETDATAAPVIEW_H
#define CPPACKETDATAAPVIEW_H

// System includes
#include <QSharedPointer>
#include <QMap>
#include <HbMessageBox>
#include <cpbasesettingview.h>

// User includes

// Forward declarations
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbMessageBox;
class CmConnectionMethodShim;
class CpPacketDataApPlugin;

// External data types

// Constants

// Class declaration
class CpPacketDataApView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    CpPacketDataApView(
        CmConnectionMethodShim *cmConnectionMethod,
        QGraphicsItem *parent = 0);
    ~CpPacketDataApView();
    
signals:

public slots:

protected:

protected slots:
 
private:
    Q_DISABLE_COPY(CpPacketDataApView)
    
    void createAccessPointSettingsGroup();
    void updateAccessPointSettingsGroup();
    void showMessageBox(
        HbMessageBox::MessageBoxType type,
        const QString &text);
    bool tryUpdate();
    void handleUpdateError();
    
private slots:
    void connectionNameChanged();
    void accessPointNameChanged();
    void userNameChanged();
    void passwordChanged();
    void authenticationChanged(int index);
    void homepageChanged();
    void menuActionTriggered(HbAction *action);
    void restoreCurrentView();
    void setEditorPreferences(const QModelIndex modelIndex);
    
private: // data
    //! Dataform
    HbDataForm *mForm;
    //! Dataform model
    HbDataFormModel *mModel;
    //! "Access point settings" group
    HbDataFormModelItem *mApSettingsGroupItem;
    //! "Connection name" setting item
    HbDataFormModelItem *mConnectionNameItem;
    //! "Access point name" setting item
    HbDataFormModelItem *mAccessPointNameItem;
    //! "User name" setting item
    HbDataFormModelItem *mUserNameItem;
    //! "Password" setting item
    HbDataFormModelItem *mPasswordItem;
    //! "Authentication" setting item
    HbDataFormModelItem *mAuthenticationItem;
    //! "Homepage" setting item
    HbDataFormModelItem *mHomepageItem;
    //! "Advanced settings" action for view menu
    HbAction *mAdvancedSettingsAction;
    //! Connection Settings Shim connection method pointer
    CmConnectionMethodShim *mCmConnectionMethod;
    //! Message box for info notes
    QSharedPointer<HbMessageBox> mMessageBox;
    //! Maps authentication combobox index to CMManagerShim::PacketDataDisablePlainTextAuth value
    QMap<int, bool> mAuthenticationMap;
    
    // Friend classes
    friend class TestCpPacketDataApPlugin;
};

#endif // CPPACKETDATAAPVIEW_H
