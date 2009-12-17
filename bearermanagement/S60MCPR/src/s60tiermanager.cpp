/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 IP TM implementation
*
*/

/**
@file s60tiermanager.cpp
S60 IP TM implementation
*/

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_log.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>

#include "s60tiermanager.h"
#include "s60tiermanagerselector.h"
#include "s60tmactivities.h"

#ifdef SYMBIAN_TRACE_ENABLE
    #define KS60TierMgrTag KESockMetaConnectionTag
    // _LIT8(KS60TierMgrSubTag, "iptiermgr");
#endif

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;

// -----------------------------------------------------------------------------
// CS60TierManager::NewL
// -----------------------------------------------------------------------------
//
CS60TierManager* CS60TierManager::NewL( ESock::CTierManagerFactoryBase& aFactory )
    {
     return new (ELeave) CS60TierManager( aFactory, 
                                          S60TierManagerActivities::S60TierManagerMap::Self() );
    }


// -----------------------------------------------------------------------------
// CS60TierManager::CS60TierManager
// -----------------------------------------------------------------------------
//
CS60TierManager::CS60TierManager( ESock::CTierManagerFactoryBase& aFactory,
                                  const MeshMachine::TNodeActivityMap& aActivityMap )
    :    CCoreTierManager( aFactory, aActivityMap )
    {
    LOG_NODE_CREATE(KS60TierMgrTag, CS60TierManager);
    }

// -----------------------------------------------------------------------------
// CS60TierManager::~CS60TierManager
// -----------------------------------------------------------------------------
//
CS60TierManager::~CS60TierManager()
    {
    LOG_NODE_DESTROY(KS60TierMgrTag, CS60TierManager);
    }

// -----------------------------------------------------------------------------
// CS60TierManager::DoCreateProviderSelectorL
// -----------------------------------------------------------------------------
//
MProviderSelector* CS60TierManager::DoCreateProviderSelectorL( const Meta::SMetaData& aSelectionPreferences )
    {
    return TS60ProviderSelectorFactory::NewSelectorL( aSelectionPreferences );
    }

// -----------------------------------------------------------------------------
// CS60TierManager::ReceivedL
// -----------------------------------------------------------------------------
//
void CS60TierManager::ReceivedL( const TRuntimeCtxId& aSender, 
                                 const TNodeId& aRecipient, 
                                 TSignatureBase& aMessage )
    {
    TNodeContext<CS60TierManager> ctx( *this, aMessage, aSender, aRecipient );
    CCoreTierManager::Received( ctx );
    User::LeaveIfError( ctx.iReturn );
    }
