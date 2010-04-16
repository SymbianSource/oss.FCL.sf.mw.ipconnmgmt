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

#ifndef CELLULARINDICATORPLUGIN_H
#define CELLULARINDICATORPLUGIN_H

#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>
#include <QtCore/QProcess>
#include <QtCore/QVariant>


/**
 * This class describes cellular indicator plug-in
 */
class CellularIndicatorPlugin : public HbIndicatorInterface,
    public HbIndicatorPluginInterface
{

    Q_OBJECT
    Q_INTERFACES(HbIndicatorPluginInterface)

public:

    CellularIndicatorPlugin();
    ~CellularIndicatorPlugin();

public:

    //from HbIndicatorPluginInterface
    
    // Return indicator type of indicator
    QStringList indicatorTypes() const;
    
    // Access is allowed allways
    bool accessAllowed(const QString &indicatorType,
                       const HbSecurityInfo *securityInfo) const;
    
    // Create indicator. Currently installs translation.
    HbIndicatorInterface* createIndicator(const QString &indicatorType);
    
    // Return error if any
    int error() const;

public:

    //from HbIndicatorInterface
    
    // Client requests are directed to here
    bool handleClientRequest (RequestType type, const QVariant &parameter);
    
    // This called when item is selected in indicator menu
    bool handleInteraction(InteractionType type);
    
    // Used to get indicator data (icon or text)
    QVariant indicatorData(int role) const;

private slots:
    // handler for error codes
    void processError(QProcess::ProcessError err);

private: // data

    // Disable copy constructor
    Q_DISABLE_COPY(CellularIndicatorPlugin)
    // Indicator type
    QStringList mIndicatorTypes;
    // Error stored to member variable
    int mError;
    // Used to store the connection info
    QVariant mParameter;
    // Process to start external programs
    QProcess process;
};




#endif // CELLULARINDICATORPLUGIN_H
