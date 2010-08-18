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

#include <QtPlugin>
#include <HbTranslator>
#include <QLocale>
#include <QList>
#include <HbLabel>
#include <HbDialog>
#include <HbDocumentLoader>
#include <HbPushButton>
#include <HbView>
#include <HbApplication>
#include "cellularindicatorplugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cellularindicatorpluginTraces.h"
#endif

Q_EXPORT_PLUGIN(CellularIndicatorPlugin)

const static char IndicatorType[] =
    "com.nokia.hb.indicator.connectivity.cellularindicatorplugin/1.0";

/*!
    CellularIndicatorPlugin::CellularIndicatorPlugin
 */
CellularIndicatorPlugin::CellularIndicatorPlugin() :
    HbIndicatorInterface(IndicatorType, SettingCategory,
        InteractionActivated),
    mError(0)
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_CELLULARINDICATORPLUGIN_ENTRY );
    mIndicatorTypes <<
        "com.nokia.hb.indicator.connectivity.cellularindicatorplugin/1.0";
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_CELLULARINDICATORPLUGIN_EXIT );
}

/*!
    CellularIndicatorPlugin::~CellularIndicatorPlugin
 */
CellularIndicatorPlugin::~CellularIndicatorPlugin()
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_DCELLULARINDICATORPLUGIN_ENTRY );
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_DCELLULARINDICATORPLUGIN_EXIT );
}

/*!
    CellularIndicatorPlugin::indicatorTypes
 */
QStringList CellularIndicatorPlugin::indicatorTypes() const
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_INDICATORTYPES_ENTRY );
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_INDICATORTYPES_EXIT );
    return mIndicatorTypes;
}

/*!
    CellularIndicatorPlugin::accessAllowed
 */
bool CellularIndicatorPlugin::accessAllowed(const QString &indicatorType,
    const QVariantMap &securityInfo) const
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_ACCESSALLOWED_ENTRY );
    Q_UNUSED(indicatorType)
    Q_UNUSED(securityInfo)

    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_ACCESSALLOWED_EXIT );
    return true;
}

/*!
    CellularIndicatorPlugin::createIndicator
 */
HbIndicatorInterface* CellularIndicatorPlugin::createIndicator(
    const QString &indicatorType)
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_CREATEINDICATOR_ENTRY );
    
    Q_UNUSED(indicatorType)
    
    // Install localization
    mTranslator = QSharedPointer<HbTranslator>(new HbTranslator("cellularindicatorplugin"));
        
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_CREATEINDICATOR_EXIT );
    return this;
}

/*!
    CellularIndicatorPlugin::error
 */
int CellularIndicatorPlugin::error() const
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_ERROR_ENTRY );
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_ERROR_EXIT );
    return mError;
}

/*!
    CellularIndicatorPlugin::handleClientRequest
 */
bool CellularIndicatorPlugin::handleClientRequest(RequestType type,
    const QVariant &parameter)
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_HANDLECLIENTREQUEST_ENTRY );
    
    bool handled(false);

    switch (type) {
    case RequestActivate:
        if (mParameter != parameter) {
            mParameter = parameter;
            emit dataChanged();
        }
        handled =  true;
        break;
    
    case RequestDeactivate:
        mParameter.clear();
        break;
        
    default:
        // Do nothing
        break;
    }

    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_HANDLECLIENTREQUEST_EXIT );
    return handled;
}

/*!
    CellularIndicatorPlugin::handleInteraction
 */
bool CellularIndicatorPlugin::handleInteraction(InteractionType type)
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_HANDLEINTERACTION_ENTRY );
    
    Q_UNUSED(type)
    bool handled = false;
    QVariantMap data;
    
    switch(type) {
    case InteractionActivated:
        // Emit a signal that will inform the client to start the connectionview client
        emit userActivated(data);

        handled = true;
        break;
    
    default:
        break;
    }

    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_HANDLEINTERACTION_EXIT );
    return handled;
}

/*!
    CellularIndicatorPlugin::indicatorData
*/
QVariant CellularIndicatorPlugin::indicatorData(int role) const
{
    OstTraceFunctionEntry0( CELLULARINDICATORPLUGIN_INDICATORDATA_ENTRY );
    QVariant ret = NULL;
    
    switch(role) {
    case PrimaryTextRole:
        ret = QString(hbTrId("txt_occ_dblist_cellular_data"));
        break;

    case SecondaryTextRole:
        if (mParameter.isValid() && (mParameter.type() == QVariant::List)) {

            QList<QVariant> indicatorInfo; 
            indicatorInfo = mParameter.toList();

            if (!indicatorInfo.count()) {
                // No entries on the list -> don't even try to use this
                break;
            }
            
            if ( (indicatorInfo[0].toInt() > 1) && indicatorInfo.count() ) {
                
                // More than one connection -> show number of connections
                QString str = QString(hbTrId("txt_occ_dblist_cellular_data_val_l1_connections"));
                ret = str.arg(indicatorInfo[0].toInt());
            }
            else if ( indicatorInfo[0].toInt() && (indicatorInfo.count() >= 2)) {
                
                // Only one connection -> show name of the iap
                ret = indicatorInfo[1].toString();
            }
        }
        break;

    case DecorationNameRole:
        // Return the icon
        ret = QString("qtg_small_gprs");
        break;
    
    default:
        // Do nothing
        break;
    }
    
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_INDICATORDATA_EXIT );
    return ret;
}

