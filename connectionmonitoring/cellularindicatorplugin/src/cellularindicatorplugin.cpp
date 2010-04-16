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
#include <QTranslator>
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
    HbIndicatorInterface(IndicatorType, GroupPriorityAverage,
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
    const HbSecurityInfo *securityInfo) const
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
    QTranslator *translator = new QTranslator(this);

    QString lang = QLocale::system().name(); 
    QString path = "Z:/resource/qt/translations/"; 
    translator->load("cellularindicatorplugin_" + lang, path);
    qApp->installTranslator(translator);
    
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
    
    switch(type) {
    case InteractionActivated:
        //connect error() to slot processError() to get error, 
        QObject::connect( &process, SIGNAL(error(QProcess::ProcessError)),                       
                          this, SLOT(processError(QProcess::ProcessError)));

        // Show connection view
        process.start("connview");
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
                QString iapName;
                iapName = indicatorInfo[1].toString();
                QString str = QString(hbTrId("txt_occ_dblist_cellular_data_val_1_connected"));
                ret = str.arg(iapName);
            }
        }
        break;

    case IconNameRole:
    case DecorationNameRole:
        // Return the icon
        ret = HbIcon("qtg_small_gprs");
        break;
    
    default:
        // Do nothing
        break;
    }
    
    OstTraceFunctionExit0( CELLULARINDICATORPLUGIN_INDICATORDATA_EXIT );
    return ret;
}

/*!
    The processError is a handler for error codes.
*/
void CellularIndicatorPlugin::processError(QProcess::ProcessError err)
    {
    OstTraceFunctionEntry1(CELLULARINDICATORPLUGIN_PROCESSERROR_ENTRY, this);
  
    switch (err) {   
        case QProcess::FailedToStart: 
        case QProcess::Crashed: 
        case QProcess::Timedout: 
        case QProcess::ReadError: 
        case QProcess::WriteError: 
        case QProcess::UnknownError:
            OstTrace1( CELLULARINDICATORPLUGIN_ERR,PROCESSERROR_KNOWN,"Process Error %u", err);
            break;  
        default:
        OstTrace1( CELLULARINDICATORPLUGIN_ERR,PROCESSERROR_UNKNOWN,"Unknown Process Error %u", err);
            break;
        }
    OstTraceFunctionExit1(CELLULARINDICATORPLUGIN_PROCESSERROR_EXIT, this);
    }

