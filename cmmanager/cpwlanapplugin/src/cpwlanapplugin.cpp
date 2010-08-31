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
* Control Panel WLAN AP plugin implementation.
*
*/

// System includes
#include <HbTranslator>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes
#include "cpwlanapplugin.h"
#include "cpwlanapview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpwlanappluginTraces.h"
#endif

/*!
    \class CpWlanApPlugin
    \brief Implements the WLAN bearer AP (access point) Control Panel plugin.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpWlanApPlugin::CpWlanApPlugin() :
    mTranslator(new HbTranslator("cpapplugin")),
    mCmManager(0),
    mCmConnectionMethod(0)
{
    OstTraceFunctionEntry0(CPWLANAPPLUGIN_CPWLANAPPLUGIN_ENTRY);

    OstTraceFunctionExit0(CPWLANAPPLUGIN_CPWLANAPPLUGIN_EXIT);
}

/*!
    Destructor.
*/
CpWlanApPlugin::~CpWlanApPlugin()
{
    OstTraceFunctionEntry0(DUP1_CPWLANAPPLUGIN_CPWLANAPPLUGIN_ENTRY);
    
    delete mCmConnectionMethod;
    delete mCmManager;

    OstTraceFunctionExit0(DUP1_CPWLANAPPLUGIN_CPWLANAPPLUGIN_EXIT);
}

/*!
    Returns the bearer type handled by the plugin.
*/
uint CpWlanApPlugin::bearerType() const
{
    OstTraceFunctionEntry0(CPWLANAPPLUGIN_BEARERTYPE_ENTRY);
    
    OstTraceFunctionExit0(CPWLANAPPLUGIN_BEARERTYPE_EXIT);
    return CMManagerShim::BearerTypeWlan;
}

/*!
    Creates the WLAN AP settings view.
*/
CpBaseSettingView *CpWlanApPlugin::createSettingView(uint connectionMethod)
{
    OstTraceFunctionEntry0(CPWLANAPPLUGIN_CREATESETTINGVIEW_ENTRY);

    // Find the connection method
    try {
        mCmManager = new CmManagerShim();
        mCmConnectionMethod = mCmManager->connectionMethod(connectionMethod);
    }
    catch (const std::exception&) {
        // Error, don't create settings view, just return
        OstTrace1(
            TRACE_ERROR,
            CPWLANAPPLUGIN_CREATESETTINGVIEW,
            "Connection method loading failed;connectionMethod=%u",
            connectionMethod);

        OstTraceFunctionExit0(DUP1_CPWLANAPPLUGIN_CREATESETTINGVIEW_EXIT);
        return NULL;
    }
    
    // Connection method found, create settings view
    OstTraceFunctionExit0(CPWLANAPPLUGIN_CREATESETTINGVIEW_EXIT);
    return new CpWlanApView(mCmConnectionMethod);
}

Q_EXPORT_PLUGIN2(cpwlanapplugin, CpWlanApPlugin);
