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
* Description:  Application Setting UI
*
*/

#include "cmapplicationsettingsui.h"
#include <cmmanager.h>
#include "cmconnsettingsuiimpl.h"
#include "cmmanager.hrh"

using namespace CMManager;
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::NewL()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi* CCmApplicationSettingsUi::NewL()
    {
    CCmApplicationSettingsUi* self = new (ELeave) CCmApplicationSettingsUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );   // self
    return self;
    }
    
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::NewLC()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi* CCmApplicationSettingsUi::NewLC()
    {
    CCmApplicationSettingsUi* self = new (ELeave) CCmApplicationSettingsUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::CCmApplicationSettingsUi()
//-----------------------------------------------------------------------------
//
CCmApplicationSettingsUi::CCmApplicationSettingsUi()
    {
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::~CCmApplicationSettingsUi()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi::~CCmApplicationSettingsUi()
    {
    delete iImpl;
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::ConstructL()
//-----------------------------------------------------------------------------
//
void CCmApplicationSettingsUi::ConstructL()
    {
    iImpl = CCmConnSettingsUiImpl::NewWithoutTablesL();
    }
    
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::RunSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CCmApplicationSettingsUi::RunApplicationSettingsL( 
                                               TCmSettingSelection& aSelection )
    {
    TBearerFilterArray aFilterArray;
    return iImpl->RunApplicationSettingsL(  aSelection,
                                            EShowDefaultConnection | 
                                            EShowDestinations | 
                                            EShowConnectionMethods,
                                            aFilterArray );
    }    
    
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::RunSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CCmApplicationSettingsUi::RunApplicationSettingsL( 
                                        TCmSettingSelection& aSelection, 
                                        TUint aListItems,
                                        TBearerFilterArray& aFilterArray)
    {
    return iImpl->RunApplicationSettingsL(  aSelection,
                                            aListItems,
                                            aFilterArray );
    }    
