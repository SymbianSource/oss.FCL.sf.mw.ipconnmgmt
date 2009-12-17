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
* Description:  Implementation of application setting UI.
*
*/

#include <coemain.h>
#include <featmgr.h>
#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>

#include "cmapplicationsettingsui.h"
#include <cmmanager.h>
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include "cmapplicationsettingsuiimpl.h"

using namespace CMManager;

typedef struct
    {
    TInt                iCmIapId;
    TInt                iCmDefaultPriority;
    }TCmListItem;

//---------------------------------------------------------------------------
//  CCmApplicationSettingsUiImpl::NewL
//---------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUiImpl* CCmApplicationSettingsUiImpl::NewL()
    {
    return NULL;
    }


//---------------------------------------------------------------------------
//  CCmApplicationSettingsUiImpl::RunSettingsL()
//---------------------------------------------------------------------------
//
EXPORT_C TBool CCmApplicationSettingsUiImpl::RunApplicationSettingsL( 
                                          TCmSettingSelection& /*aSelection*/ )
    {
    return EFalse;
    }    

