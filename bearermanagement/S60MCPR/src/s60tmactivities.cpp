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
* Description: S60 MCPR activities implementation
*
*/

/**
@file s60mcpractivities.cpp
S60 MCPR activities implementation
*/

#include "s60tmactivities.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace TMActivities;

// -----------------------------------------------------------------------------
// S60TmProviderActivitySuper
// -----------------------------------------------------------------------------
//
namespace S60TmProviderActivitySuper
{ //Simple parallel activity provider selection, waits untill selection completes (via ISelectionNotify), then gets destroyed
    DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivitySelect,
                                       TMSelectProvider,
                                       TCFSelector::TSelect,
                                       CSelectProviderActivity::NewL)
                                       
	FIRST_NODEACTIVITY_ENTRY(TMStates::TAwaitingSelectProviderSuper,
	                         MeshMachine::TNoTag)
	
	NODEACTIVITY_ENTRY(KNoTag,
	                   S60TmProviderActivitySuper::TSelectProviderSuper,
	                   CoreStates::TNeverAccept,
	                   MeshMachine::TNoTag)
	
	LAST_NODEACTIVITY_ENTRY(KNoTag,
	                        MeshMachine::TDoNothing)

    NODEACTIVITY_END()
}

// -----------------------------------------------------------------------------
// S60TierManagerActivities - activitymap
// -----------------------------------------------------------------------------
//
namespace S60TierManagerActivities
{
    DEFINE_ACTIVITY_MAP(S60TierManagerMap)
    ACTIVITY_MAP_ENTRY(S60TmProviderActivitySuper, TMSelectProvider)
    ACTIVITY_MAP_END_BASE(TMActivities, coreTMActivities)
}

//  End of File
