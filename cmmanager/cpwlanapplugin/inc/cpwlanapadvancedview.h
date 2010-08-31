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
* Control Panel WLAN AP advanced settings view header file.
*
*/

#ifndef CPWLANAPADVANCEDVIEW_H
#define CPWLANAPADVANCEDVIEW_H

// System includes
#include <QAbstractSocket>
#include <HbMessageBox>
#include <cpbasesettingview.h>
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbValidator;
class CmConnectionMethodShim;

// External data types

// Constants

// Class declaration
class CpWlanApAdvancedView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    CpWlanApAdvancedView(
        CmConnectionMethodShim *cmConnectionMethod,
        QGraphicsItem *parent = 0);
    ~CpWlanApAdvancedView();
    
signals:

public slots:

protected:

protected slots:
 
private:
    Q_DISABLE_COPY(CpWlanApAdvancedView)
   
    //! IPv6 DNS type
    enum Ipv6DnsType {
        Ipv6DnsTypeAutomatic = 0,
        Ipv6DnsTypeWellKnown,
        Ipv6DnsTypeUserDefined
    };

    void createAdvancedSettings();
    void createIpv4Group();
    void updateIpv4Group();
    void createIpv6Group();
    void updateIpv6Group();
    void createProxyGroup();
    void updateProxyGroup();
    QString getIpAddress(CMManagerShim::ConnectionMethodAttribute attribute);
    Ipv6DnsType getIpv6DnsType();
    void enableIpv4AddressFields(int state);
    void enableIpv4DnsFields(int state);
    void enableIpv6DnsFields(int selectionIndex);
    bool validateAndSaveIpAddress(
        HbDataFormModelItem *item,
        CMManagerShim::ConnectionMethodAttribute attribute,
        QAbstractSocket::NetworkLayerProtocol protocol);
    void showMessageBox(
        HbMessageBox::MessageBoxType type,
        const QString &text);
    bool tryUpdate();
    void handleUpdateError();
    
private slots:
    void setEditorPreferences(const QModelIndex index);
    void changeIpv4AddressMode(int state);
    void changeIpv4Address();
    void changeIpv4SubnetMask();
    void changeIpv4Gateway();
    void changeIpv4DnsMode(int state);
    void changeIpv4PrimaryDnsAddress();
    void changeIpv4SecondaryDnsAddress();
    void changeIpv6DnsMode(int selectionIndex);
    void changeIpv6PrimaryDnsAddress();
    void changeIpv6SecondaryDnsAddress();
    void changeProxyServer();
    void changeProxyPort();

private: // data
    //! Dataform
    HbDataForm *mForm;
    //! Dataform model
    HbDataFormModel *mModel;
    //! Connection Settings Shim connection method pointer
    CmConnectionMethodShim *mCmConnectionMethod;
    //! "IPv4 settings" group
    HbDataFormModelItem *mIpv4Group;
    //! "Automatic" setting item for phone IP address
    HbDataFormModelItem *mIpv4Automatic;
    //! "Phone IP address" setting item
    HbDataFormModelItem *mIpv4Address;
    //! "Subnet mask" setting item
    HbDataFormModelItem *mIpv4SubnetMask;
    //! "Default gateway" setting item
    HbDataFormModelItem *mIpv4Gateway;
    //! "Automatic" setting item for IPv4 DNS addresses
    HbDataFormModelItem *mIpv4DnsAutomatic;
    //! "Primary name server" setting item
    HbDataFormModelItem *mIpv4DnsAddress1;
    //! "Secondary name server" setting item
    HbDataFormModelItem *mIpv4DnsAddress2;
    //! "IPv6 settings" group
    HbDataFormModelItem *mIpv6Group;
    //! "Automatic" setting item for IPv6 DNS addresses
    HbDataFormModelItem *mIpv6DnsAutomatic;
    //! "Primary name server" setting item
    HbDataFormModelItem *mIpv6DnsAddress1;
    //! "Secondary name server" setting item
    HbDataFormModelItem *mIpv6DnsAddress2;
    //! "Proxy settings" group
    HbDataFormModelItem *mProxyGroup;
    //! "Proxy server address" setting item
    HbDataFormModelItem *mProxyServer;
    //! "Proxy port number" setting item
    HbDataFormModelItem *mProxyPort;
    //! Message box for info notes
    QSharedPointer<HbMessageBox> mMessageBox;
    //! Flag for IPv6 support
    bool mIpv6Supported;
    
    // Friend classes
    friend class TestCpWlanApPlugin;
};

#endif // CPWLANAPADVANCEDVIEW_H
