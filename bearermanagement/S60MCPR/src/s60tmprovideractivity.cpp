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
* Description: S60 MCPR simple select activity implementation
*
*/

/**
@file s60mcprsimpleselectactivity.cpp
S60 MCPR simple select activity implementation
*/

#include "s60mcpractivities.h"
#include "s60mcprstates.h"
#include "s60extendedtmselector.h"
#include "s60tmactivities.h"

#include <comms-infras/ss_api_ext.h>

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace MCprActivities;
using namespace TMActivities;

// -----------------------------------------------------------------------------
// S60TmProviderActivitySuper
// -----------------------------------------------------------------------------
//
namespace S60TmProviderActivitySuper
    {
    DEFINE_SMELEMENT(TSelectProviderSuper, NetStateMachine::MStateTransition, TMStates::TContext)
    void TSelectProviderSuper::DoL() // codescanner::leave
	    {
	    /*__ASSERT_DEBUG(iContext.iNodeActivity, User::Panic(KCoreTMPanic, KPanicNoActivity));*/
	    CSelectProviderActivity* act = static_cast<CSelectProviderActivity*>(iContext.iNodeActivity);

	    //Creates the core selector - this selector is not required, it is always the
	    //same selector created so the code could be moving into this transition.
	    //The method FindOrCreateProviderL would need to be recreated somewhere else
	    //for this to be possible (currently inherited off ASimpleSelectorBase.

        act->iSelector = new (ELeave /*codescanner::leave*/) CExtendedS60SelectorBase( 
		    iContext.Node().TierId(),
		    message_cast<TCFSelector::TSelect>(iContext.iMessage).iConnPrefList
		    ); 

	    ISelectionNotify selectionNotify(act,act->InterfaceVTable());
	    act->iSelector->SelectL(selectionNotify); // codescanner::leave
	    //Do not do any further processing here as selectors may be returning synchronously (via ISelectionNotify).
	    }
    }

//  End of File
