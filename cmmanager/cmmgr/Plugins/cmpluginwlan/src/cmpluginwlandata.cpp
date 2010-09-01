/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of WLan network data holder class
*
*/

#include <e32std.h>
#include "cmpluginwlandata.h"
#include "cmlogger.h"




// --------------------------------------------------------------------------
// CCmPluginWlanData::NewLC
// --------------------------------------------------------------------------
//
CCmPluginWlanData* CCmPluginWlanData::NewLC()
    {
    CCmPluginWlanData* self = new( ELeave ) CCmPluginWlanData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }
    
    

// --------------------------------------------------------------------------
// CCmPluginWlanData::~CCmPluginWlanData
// --------------------------------------------------------------------------
//
CCmPluginWlanData::~CCmPluginWlanData()
    {
    LOGGER_ENTERFN( "CCmPluginWlanData::~CCmPluginWlanData" );
    
    delete iNetworkName;
    iNetworkName = NULL;
    CLOG_CLOSE;
    }
    
    


// --------------------------------------------------------------------------
// CCmPluginWlanData::CCmPluginWlanData
// --------------------------------------------------------------------------
//
CCmPluginWlanData::CCmPluginWlanData()
    {
    CLOG_CREATE;
    }




// --------------------------------------------------------------------------
// CCmPluginWlanData::ConstructL
// --------------------------------------------------------------------------
//
void CCmPluginWlanData::ConstructL()
    {
    LOGGER_ENTERFN( "CCmPluginWlanData::ConstructL" );
    
    iNetworkName = NULL;
    }


