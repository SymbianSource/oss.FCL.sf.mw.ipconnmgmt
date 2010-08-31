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

// Dummy implementation not to break the build.
// Not working at the moment due to cmmanager refactoring work.

#include "cmapplicationsettingsui.h"
//#include <cmmanager.h>
//#include "cmconnsettingsuiimpl.h"
//#include "cmmanager.hrh"

using namespace CMManager;
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::NewL()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi* CCmApplicationSettingsUi::NewL()
    {
    /*
    CCmApplicationSettingsUi* self = new (ELeave) CCmApplicationSettingsUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );   // self
    return self;
    */
    return NULL;
    }
    
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::NewLC()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi* CCmApplicationSettingsUi::NewLC()
    {
    /*
    CCmApplicationSettingsUi* self = new (ELeave) CCmApplicationSettingsUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    */
    return NULL;
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::CCmApplicationSettingsUi()
//-----------------------------------------------------------------------------
//
CCmApplicationSettingsUi::CCmApplicationSettingsUi()
    {
    iImpl = NULL;
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::~CCmApplicationSettingsUi()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmApplicationSettingsUi::~CCmApplicationSettingsUi()
    {
    // delete iImpl;
    }

//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::ConstructL()
//-----------------------------------------------------------------------------
//
void CCmApplicationSettingsUi::ConstructL()
    {
    // iImpl = CCmConnSettingsUiImpl::NewWithoutTablesL(); */
    }
    
//-----------------------------------------------------------------------------
//  CCmApplicationSettingsUi::RunSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool CCmApplicationSettingsUi::RunApplicationSettingsL( 
                                               TCmSettingSelection& aSelection )
    {
    /*
    TBearerFilterArray aFilterArray;
    return iImpl->RunApplicationSettingsL(  aSelection,
                                            EShowDefaultConnection | 
                                            EShowDestinations | 
                                            EShowConnectionMethods,
                                            aFilterArray );
                                            */
    (void) aSelection;
    return NULL;
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
    /*
    return iImpl->RunApplicationSettingsL(  aSelection,
                                            aListItems,
                                            aFilterArray );
    */
    (void) aSelection;
    (void) aListItems;
    (void) aFilterArray;
    return NULL;
    }    
