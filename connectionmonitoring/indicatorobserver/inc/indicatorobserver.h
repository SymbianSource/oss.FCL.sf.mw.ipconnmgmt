/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * 
 */

#ifndef INDICATOROBSERVER_H
#define INDICATOROBSERVER_H

#include <QtCore>

#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <xqsettingskey.h>

class XQSettingsManager;

QTM_USE_NAMESPACE

const int wlanNotConnected = 0;
const int wlanConnected = 1;

const QString bearerWLAN = "WLAN";
const QString bearer2G = "2G";
const QString bearerWCDMA = "WCDMA";
const QString bearerHSPA = "HSPA";
const QString bearerCDMA2000 = "CDMA2000";

class IndicatorObserver : public QCoreApplication
    {
    Q_OBJECT
    
public:
    IndicatorObserver(int argc, char* argv[]);
    
    virtual ~IndicatorObserver();
    
    void initializeIndicators();
    
    void findActiveConfigurations();
    
    void updateWlanIndicator();
    
    void updateCellularIndicator();
    
    void activateCellularIndicatorPlugin(QList<QVariant> list);
    
    void deactivateCellularIndicatorPlugin();
     
    void activateWlanIndicatorPlugin(QList<QVariant> list);
    
    void deactivateWlanIndicatorPlugin();
        
private slots:
    void updateWlanRadioStatus(const XQSettingsKey &key, const QVariant &value);

    void handleConfigurationChanged(const QNetworkConfiguration& config);
    
private:
    /*!
        Pointer to network configuration manager
    */
    QNetworkConfigurationManager* mNetConfigurationManager;
    
    /*!
        Pointer to settings manager for reading PS keys
    */
    XQSettingsManager *mSettingsManager;
    
    /*!
        Pointer to a list of active cellular configurations
    */
    QList<QNetworkConfiguration>* mActiveCellularConfigurations;
    
    /*!
        Pointer to a list of active WLAN configurations
    */
    QList<QNetworkConfiguration>* mActiveWlanConfigurations;
    
    /*!
        Indicates whether WLAN radio is switched on
    */
    bool mWlanEnabled;
    
    /*!
        Indicates whether WLAN radio is forced off
    */
    bool mWlanForceDisabled;
    
    /*!
        Indicates whether WLAN indicator plugin is activated
    */
    bool mWlanIndicatorIsActivated;
    
    /*!
        Indicates whether cellular indicator plugin is activated
    */
    bool mCellularIndicatorIsActivated;

    friend class ObserverTester;
    
    };

#endif

// End of File
