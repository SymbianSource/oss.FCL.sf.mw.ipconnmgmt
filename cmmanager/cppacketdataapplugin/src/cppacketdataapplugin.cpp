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
* Control Panel packet data AP plugin implementation.  
*
*/

// System includes
#include <HbTranslator>
#include <cmmanager_shim.h>
#include <cmconnectionmethod_shim.h>

// User includes
#include "cppacketdataapplugin.h"
#include "cppacketdataapview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cppacketdataappluginTraces.h"
#endif

/*!
    \class CpPacketDataApPlugin
    \brief Implements the packet data bearer AP (access point) Control Panel
           plugin.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
*/
CpPacketDataApPlugin::CpPacketDataApPlugin() :
    mTranslator(new HbTranslator("cpapplugin")),
    mCmManager(0),
    mCmConnectionMethod(0)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPPLUGIN_CPPACKETDATAAPPLUGIN_ENTRY);
 
    OstTraceFunctionExit0(CPPACKETDATAAPPLUGIN_CPPACKETDATAAPPLUGIN_EXIT);
}

/*!
    Destructor.
*/
CpPacketDataApPlugin::~CpPacketDataApPlugin()
{
    OstTraceFunctionEntry0(DUP1_CPPACKETDATAAPPLUGIN_CPPACKETDATAAPPLUGIN_ENTRY);
    
    delete mCmConnectionMethod;
    delete mCmManager;
    
    OstTraceFunctionExit0(DUP1_CPPACKETDATAAPPLUGIN_CPPACKETDATAAPPLUGIN_EXIT);
}

/*!
    Returns the bearer type handled by the plugin.
*/
uint CpPacketDataApPlugin::bearerType() const
{
    OstTraceFunctionEntry0(CPPACKETDATAAPPLUGIN_BEARERTYPE_ENTRY);
    
    OstTraceFunctionExit0(CPPACKETDATAAPPLUGIN_BEARERTYPE_EXIT);
    return CMManagerShim::BearerTypePacketData;
}

/*!
    Creates the packet data AP settings view.
*/
CpBaseSettingView *CpPacketDataApPlugin::createSettingView(
    uint connectionMethod)
{
    OstTraceFunctionEntry0(CPPACKETDATAAPPLUGIN_CREATESETTINGVIEW_ENTRY);
    
    // Find the connection method
    try {
        mCmManager = new CmManagerShim();
        mCmConnectionMethod = mCmManager->connectionMethod(connectionMethod);
    }
    catch (const std::exception&) {
        // Error, don't create settings view, just return
        OstTrace1(
            TRACE_ERROR,
            CPPACKETDATAAPPLUGIN_CREATESETTINGVIEW,
            "Connection method loading failed;connectionMethod=%u",
            connectionMethod);

        OstTraceFunctionExit0(DUP1_CPPACKETDATAAPPLUGIN_CREATESETTINGVIEW_EXIT);
        return NULL;
    }

    // Connection method found, create settings view
    OstTraceFunctionExit0(CPPACKETDATAAPPLUGIN_CREATESETTINGVIEW_EXIT);
    return new CpPacketDataApView(mCmConnectionMethod);
}

Q_EXPORT_PLUGIN2(cppacketdataapplugin, CpPacketDataApPlugin);
