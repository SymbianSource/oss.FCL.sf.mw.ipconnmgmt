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

#include <QtCore>
#include <QCoreApplication>

#include <hbindicator.h>
#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>
#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <xqsettingsmanager.h>
#include <wlandevicesettingsinternalcrkeys.h>

#include "indicatorobserver.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "indicatorobserverTraces.h"
#endif

QTM_USE_NAMESPACE

/*!
    IndicatorObserver::IndicatorObserver
*/
IndicatorObserver::IndicatorObserver(int argc, char* argv[]) :
    QCoreApplication(argc, argv),
    mNetConfigurationManager(new QNetworkConfigurationManager(this)),
    mSettingsManager(new XQSettingsManager(this)),
    mActiveCellularConfigurations(new QList<QNetworkConfiguration>),
    mActiveWlanConfigurations(new QList<QNetworkConfiguration>),    
    mWlanEnabled(0),
    mWlanForceDisabled(0),
    mWlanIndicatorIsActivated(false),
    mCellularIndicatorIsActivated(false),
    mWlanIndicator(NULL),
    mCellularIndicator(NULL)
    
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_CONSTRUCTOR_ENTRY, "-->");

    bool connectStatus = false;

    connectStatus = connect(
        mNetConfigurationManager,
        SIGNAL(configurationChanged(const QNetworkConfiguration&)),
        this,
        SLOT(handleConfigurationChanged(const QNetworkConfiguration&)));
    
    if (!connectStatus){
        OstTrace0(TRACE_FLOW, INDICATOROBSERVER_CONNECT_FAILED, "Connecting handleConfigurationChanged SLOT failed");     
        }
    
    connectStatus = connect(
        mSettingsManager,
        SIGNAL(valueChanged(const XQSettingsKey, const QVariant)),
        this,
        SLOT(updateWlanRadioStatus(const XQSettingsKey, const QVariant)));
    
    if (!connectStatus){
        OstTrace0(TRACE_FLOW, INDICATOROBSERVER_CONNECT_FAILED_DUP1, "Connecting updateWlanRadioStatus SLOT failed");     
        }
    
    // Subscribe for WLAN ON/OFF change indications
    XQSettingsKey wlanKey(
        XQSettingsKey::TargetCentralRepository,
        KCRUidWlanDeviceSettingsRegistryId.iUid,
        KWlanOnOff);
    
    // Also subscribe for KForceDisableWlan change indications
    XQSettingsKey wlanForceKey(
        XQSettingsKey::TargetCentralRepository,
        KCRUidWlanDeviceSettingsRegistryId.iUid, 
        KWlanForceDisable);
    
    //Read current status of WLAN radio
    mWlanEnabled = mSettingsManager->readItemValue(wlanKey).toInt() ? true : false;
    mWlanForceDisabled = mSettingsManager->readItemValue(wlanForceKey).toInt() ? true : false;

    mSettingsManager->startMonitoring(wlanKey);
    mSettingsManager->startMonitoring(wlanForceKey);

    initializeIndicators();
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_CONSTRUCTOR_EXIT, "<--");
}


/*!
    IndicatorObserver::~IndicatorObserver
*/
IndicatorObserver::~IndicatorObserver()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DESTRUCTOR_ENTRY, "-->");
     
    if(mCellularIndicatorIsActivated) {
        deactivateCellularIndicatorPlugin();
    }
    
    if (mActiveCellularConfigurations) {
        mActiveCellularConfigurations->clear();
        delete mActiveCellularConfigurations;
        }
    

    if (mWlanIndicatorIsActivated) {
        deactivateWlanIndicatorPlugin();
    }
    
    if (mActiveWlanConfigurations) {
        mActiveWlanConfigurations->clear();
        delete mActiveWlanConfigurations;
        }
    
    delete mCellularIndicator;
    delete mWlanIndicator;
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DESTRUCTOR_EXIT, "<--");
}

/*!
    IndicatorObserver::initializeIndicators
*/
void IndicatorObserver::initializeIndicators()
{

    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_INITIALIZEINDICATORS_ENTRY, "-->");
    
    // create the indicators and make connections
    mCellularIndicator = new HbIndicator();  
    // connect the user activated slot here, so that the indicator know to start the connview in case
    // the user taps the indicator
    bool conn = QObject::connect(
                    mCellularIndicator, 
                    SIGNAL(userActivated(const QString&, const QVariantMap&)), 
                    this, 
                    SLOT(userActivateCellularIndicator(const QString&, const QVariantMap&)));
    
    mWlanIndicator = new HbIndicator();  
    // connect the user activated slot here, so that the indicator know to start the wlan sniffer in case
    // the user taps the indicator
    conn = QObject::connect(
                    mWlanIndicator, 
                    SIGNAL(userActivated(const QString&, const QVariantMap&)), 
                    this, 
                    SLOT(userActivateWlanIndicator(const QString&, const QVariantMap&)));
    
    findActiveConfigurations();
    updateWlanIndicator();
    updateCellularIndicator();
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_INITIALIZEINDICATORS_EXIT, "<--");
}

/*!
    IndicatorObserver::updateWlanRadioStatus
*/
void IndicatorObserver::updateWlanRadioStatus(const XQSettingsKey &key, const QVariant &value)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATEWLANRADIOSTATUS_ENTRY, "-->");
    
    // The change notification is received either for the WlanOnoff key, or the
    // ForceDisableWlan key
    if (KWlanOnOff == key.key()) {
        mWlanEnabled = value.toInt() ? true : false;
    } else {
        mWlanForceDisabled = value.toInt() ? true : false;
    }
    
    if (mWlanEnabled == false || mWlanForceDisabled == true) {    
        deactivateWlanIndicatorPlugin();
    } else {
        updateWlanIndicator();
    }
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATEWLANRADIOSTATUS_EXIT, "<--");
}

/*!
    IndicatorObserver::findActiveConfigurations
*/
void IndicatorObserver::findActiveConfigurations()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_FINDACTIVECONFIGURATIONS_ENTRY, "-->");
    
    mActiveWlanConfigurations->clear();
    mActiveCellularConfigurations->clear();
            
    //Let's find active connections if any    
    QList<QNetworkConfiguration> allConfigurations = mNetConfigurationManager->allConfigurations(QNetworkConfiguration::Active);
    
    for (int i=0; i<allConfigurations.count(); i++) {
        if (allConfigurations[i].type() == QNetworkConfiguration::InternetAccessPoint) {

            //Let's check the bearer type
            QString bearerName = allConfigurations[i].bearerName();
        
            if (bearerName == bearerWLAN) {
                mActiveWlanConfigurations->append(allConfigurations[i]);
            } else if (bearerName == bearer2G ||
                       bearerName == bearerWCDMA ||
                       bearerName == bearerHSPA || 
                       bearerName == bearerCDMA2000) {
                mActiveCellularConfigurations->append(allConfigurations[i]);
            }  
        }
    }
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_FINDACTIVECONFIGURATIONS_EXIT, "<--");
}

/*!
    IndicatorObserver::updateWlanIndicator
*/
void IndicatorObserver::updateWlanIndicator()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATEWLANINDICATOR_ENTRY, "-->");
    QList<QVariant> list;
    
    int count = mActiveWlanConfigurations->count();

    //We do not deactivate WlanIndicator plugin here as it is done in updateWlanRadioStatus method
    //as WLAN radio status determines whether to show indicator or not
    if ( mWlanEnabled && !mWlanForceDisabled) {
        if(count == 0) {
            list.insert(0, wlanNotConnected);
            activateWlanIndicatorPlugin(list);
        } else {
            list.insert(0, wlanConnected);
            list.insert(1, mActiveWlanConfigurations->at(0).name());          
            activateWlanIndicatorPlugin(list);
        }
    }
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATEWLANINDICATOR_EXIT, "<--");
}

/*!
    IndicatorObserver::updateCellularIndicator
*/
void IndicatorObserver::updateCellularIndicator()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATECELLULARINDICATOR_ENTRY, "-->");
    
    QList<QVariant> list;

    int count = mActiveCellularConfigurations->count();
 
    if ( count == 0 ) {
        if ( mCellularIndicatorIsActivated ){
            deactivateCellularIndicatorPlugin();
        }
    } else {
        if (count == 1) {
            list.insert(0, count);
            list.insert(1, mActiveCellularConfigurations->at(0).name());
            list.insert(2, mActiveCellularConfigurations->at(0).identifier().toInt());
        } else {
            list.insert(0, count);
        
        } 
        activateCellularIndicatorPlugin(list);        
    }
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_UPDATECELLULARINDICATOR_EXIT, "<--");
}

/*!
    IndicatorObserver::handleConfigurationChanged
*/
void IndicatorObserver::handleConfigurationChanged(const QNetworkConfiguration& config)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_HANDLECONFIGURATIONCHANGED_ENTRY, "-->");
    
    switch (config.state())
    {   
        case QNetworkConfiguration::Defined: //To handle situation where we have active connection and it is lost due to bad radio conditions 
        case QNetworkConfiguration::Discovered:            
        case QNetworkConfiguration::Active:
            {
                findActiveConfigurations();

                QString bearerName = config.bearerName();
                
                if (bearerName == bearerWLAN) {
                    updateWlanIndicator();
                } else if (bearerName == bearer2G ||
                           bearerName == bearerWCDMA ||
                           bearerName == bearerHSPA  ||
                           bearerName == bearerCDMA2000) {                   
                    updateCellularIndicator();
                }
            }
            break;
            
        default:
            break;
    }
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_HANDLECONFIGURATIONCHANGED_EXIT, "<--");
}

void IndicatorObserver::userActivateCellularIndicator(const QString &type, const QVariantMap &data)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_USERACTIVATECELLULARINDICATOR_ENTRY, "-->");
    
    Q_UNUSED(data);
    Q_UNUSED(type);
    // Show connection view
    QProcess::startDetached("connview");
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_USERACTIVATECELLULARINDICATOR_EXIT, "<--");
}

void IndicatorObserver::userActivateWlanIndicator(const QString &type, const QVariantMap &data)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_USERACTIVATEWLANINDICATOR_ENTRY, "-->");
    
    Q_UNUSED(data);
    Q_UNUSED(type);
    // Show wlan list view
    QProcess::startDetached("WlanSniffer");

    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_USERACTIVATEWLANINDICATOR_EXIT, "<--");
}

/*!
    IndicatorObserver::activateCellularIndicatorPlugin
*/
void IndicatorObserver::activateCellularIndicatorPlugin(QList<QVariant> list)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_ACTIVATECELLULARINDICATORPLUGIN_ENTRY, "-->");
    
    bool success = mCellularIndicator->activate("com.nokia.hb.indicator.connectivity.cellularindicatorplugin/1.0", list);
     
    if (!success) {
        mCellularIndicatorIsActivated = false;
        OstTrace0(TRACE_FLOW, INDICATOROBSERVER_CELLULAR_INDICATOR_ACTIVATION_FAILED, "Cellular indicator activation failed"); 
    } else {
        mCellularIndicatorIsActivated = true;
    }
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_ACTIVATECELLULARINDICATORPLUGIN_EXIT, "<--");
}

/*!
    IndicatorObserver::deactivateCellularIndicatorPlugin
*/
void IndicatorObserver::deactivateCellularIndicatorPlugin()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DEACTIVATECELLULARINDICATORPLUGIN_ENTRY, "-->");
    
    mCellularIndicator->deactivate("com.nokia.hb.indicator.connectivity.cellularindicatorplugin/1.0");
    mCellularIndicatorIsActivated = false;
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DEACTIVATECELLULARINDICATORPLUGIN_EXIT, "<--");
}

/*!
    IndicatorObserver::activateWlanIndicatorPlugin
*/
void IndicatorObserver::activateWlanIndicatorPlugin(QList<QVariant> list)
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_ACTIVATEWLANINDICATORPLUGIN_ENTRY, "-->");
    
    bool success = mWlanIndicator->activate("com.nokia.hb.indicator.connectivity.wlanindicatorplugin/1.0", list);
   
    if (!success) {
        mWlanIndicatorIsActivated = false;
        OstTrace0(TRACE_FLOW, INDICATOROBSERVER_WLAN_INDICATOR_ACTIVATION_FAILED, "WLAN indicator activation failed"); 
    } else {
        mWlanIndicatorIsActivated = true;
    }
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_ACTIVATEWLANINDICATORPLUGIN_EXIT, "<--");    
}

/*!
    IndicatorObserver::deactivateWlanIndicatorPlugin
*/
void IndicatorObserver::deactivateWlanIndicatorPlugin()
{
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DEACTIVATEWLANINDICATORPLUGIN_ENTRY, "-->");

    mWlanIndicator->deactivate("com.nokia.hb.indicator.connectivity.wlanindicatorplugin/1.0");
    mWlanIndicatorIsActivated = false;
    
    OstTrace0(TRACE_FLOW, INDICATOROBSERVER_DEACTIVATEWLANINDICATORPLUGIN_EXIT, "<--");   
}
