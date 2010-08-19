/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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

//This include contains full path because there is 
//an other version of the same file available in the public
//SDK and we want to make sure that the internal header is used.
#include <platform/mw/cmsettingsui.h> 
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
    
//-----------------------------------------------------------------------------
//  CCmSettingsUi::AddCmL()
//-----------------------------------------------------------------------------
//    

EXPORT_C TUint32 CCmSettingsUi::AddCmL( TUint32& aDestUid, TUint32 aBearerType )
    {
    return iImpl->AddCmL( aDestUid, aBearerType );
    }

//-----------------------------------------------------------------------------
//  CCmSettingsUi::EditCmL()
//-----------------------------------------------------------------------------
//

EXPORT_C CCmSettingsUi::TResult CCmSettingsUi::EditCmL( TUint32 aCmId )
    {
    if ( iImpl->EditCmL( aCmId ) == KDialogUserExit )
        {
        return EExit;
        }
     else
        {
        return EBack;
        }
    }
