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
* Control Panel WLAN AP settings view header file.
*
*/

#ifndef CPWLANAPVIEW_H
#define CPWLANAPVIEW_H

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
class CpWlanApPlugin;
class CpWlanSecurityPluginInterface;

// External data types

// Constants

// Class declaration
class CpWlanApView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    CpWlanApView(
        CmConnectionMethodShim *cmConnectionMethod,
        QGraphicsItem *parent = 0);
    ~CpWlanApView();
    
signals:

public slots:

protected:

protected slots:
 
private:
    void createAccessPointSettingsGroup();
    void updateAccessPointSettingsGroup();
    void loadSecurityPlugins();
    void updateSecurityGroup(int index);
    void showMessageBox(
        HbMessageBox::MessageBoxType type,
        const QString &text);
    bool tryUpdate();
    void handleUpdateError();
       
private slots:
    void connectionNameChanged();
    void wlanNetworkNameChanged();
    void networkStatusChanged(int index);
    void networkModeChanged(int index);
    void securityModeChanged(int index);
    void homepageChanged();
    void menuActionTriggered(HbAction *action);
    void restoreCurrentView();
    void setEditorPreferences(const QModelIndex modelIndex);
    
private: // data
    //! Dataform
    HbDataForm *mForm;
    //! Dataform model
    HbDataFormModel *mModel;
    //! Control Panel item data helper for WLAN security plugins
    CpItemDataHelper *mItemDataHelper;
    //! "Access point settings" group
    HbDataFormModelItem *mApSettingsGroupItem;
    //! "Connection name" setting item
    HbDataFormModelItem *mConnectionNameItem;
    //! "WLAN network name" setting item
    HbDataFormModelItem *mWlanNetworkNameItem;
    //! "Network status" setting item
    HbDataFormModelItem *mNetworkStatusItem;
    //! "Network mode" setting item
    HbDataFormModelItem *mNetworkModeItem;
    //! "Security mode" setting item
    HbDataFormModelItem *mSecurityModeItem;
    //! "Homepage" setting item
    HbDataFormModelItem *mHomepageItem;
    //! "Security settings" group
    HbDataFormModelItem *mSecuritySettingsGroupItem;
    //! "Advanced settings" action for view menu
    HbAction *mAdvancedSettingsAction;
    //! Connection Settings Shim connection method pointer
    CmConnectionMethodShim *mCmConnectionMethod;
    //! Message box for info notes
    QSharedPointer<HbMessageBox> mMessageBox;
    //! Maps network status combobox index to CMManagerShim::WlanScanSSID value
    QMap<int, bool> mNetworkStatusMap;
    //! Maps network mode combobox index to CMManagerShim::WlanConnectionMode value
    QMap<int, int> mNetworkModeMap;
    //! Maps security mode combobox index to corresponding security plugin pointer
    QMap<int, CpWlanSecurityPluginInterface *> mSecurityModeMap;
    
    // Friend classes
    friend class TestCpWlanApPlugin;
};

#endif // CPWLANAPVIEW_H
