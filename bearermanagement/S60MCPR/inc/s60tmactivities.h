/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR's activities.
*
*/

/**
@file s60mcpractivities.h
S60 MCPR's activities.
*/

#ifndef S60TM_ACTIVITIES_H
#define S60TM_ACTIVITIES_H

#include <comms-infras/mobilitymcpractivities.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>

#include "s60mcprstates.h"
#include "s60tiermanager.h"

// Forward declarations
class CS60MetaConnectionProvider;

namespace S60TierManagerActivities
    {
    DECLARE_ACTIVITY_MAP(S60TierManagerMap)
    }

namespace S60TmProviderActivitySuper
    {

    typedef MeshMachine::TNodeContext<CCoreTierManager, CoreNetStates::TContext> TContext;

    DECLARE_SMELEMENT_HEADER(TSelectProviderSuper,
                             MeshMachine::TStateTransition<TContext>,
                             NetStateMachine::MStateTransition,
                             TMStates::TContext)
	
	virtual void DoL();

    DECLARE_SMELEMENT_FOOTER( TSelectProviderSuper )
    }

#endif // S60TM_ACTIVITIES_H

