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
* Description:  Implementation of CCmSettingsUi.
*
*/

#include <cmsettingsui.h>
#include "cmsettingsuiimpl.h"
#include <cmcommonconstants.h>

//-----------------------------------------------------------------------------
//  CCmSettingsUi::NewL()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmSettingsUi* CCmSettingsUi::NewL()
    {
    CCmSettingsUi* self = new (ELeave) CCmSettingsUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );   // self
    return self;
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUi::CCmSettingsUi()
//-----------------------------------------------------------------------------
//
CCmSettingsUi::CCmSettingsUi()
    {
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUi::~CCmSettingsUi()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmSettingsUi::~CCmSettingsUi()
    {
    delete iImpl;
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUi::ConstructL()
//-----------------------------------------------------------------------------
//
void CCmSettingsUi::ConstructL()
    {
    iImpl = CCmSettingsUiImpl::NewL();
    }
    
//-----------------------------------------------------------------------------
//  CCmSettingsUi::RunSettingsL()
//-----------------------------------------------------------------------------
//
EXPORT_C CCmSettingsUi::TResult CCmSettingsUi::RunSettingsL()
    {
    if ( iImpl->RunSettingsL() == KDialogUserExit )
        {
        return EExit;
        }
     else
        {
        return EBack;
        }
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUi::SelectDestinationDlgL()
//-----------------------------------------------------------------------------
//    
EXPORT_C TBool CCmSettingsUi::SelectDestinationDlgL( TUint32& aDestinationId )
    {
    return iImpl->SelectDestinationDlgL( aDestinationId );
    }
