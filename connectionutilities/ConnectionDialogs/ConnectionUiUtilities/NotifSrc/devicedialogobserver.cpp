/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Prompt Dialog implementation
*
*/

#include <hb/hbcore/hbsymbianvariant.h>
#include "devicedialogobserver.h"

_LIT(KButton, "button");

// ---------------------------------------------------------
// CDeviceDialogObserver* CDeviceDialogObserver::NewL
// ---------------------------------------------------------
//
CDeviceDialogObserver* CDeviceDialogObserver::NewL( CCellularDataConfirmation* aNotifier )
    {
    CDeviceDialogObserver* self = new ( ELeave ) CDeviceDialogObserver( aNotifier );
    return self;
    }

// ---------------------------------------------------------
// CDeviceDialogObserver::CDeviceDialogObserver
// ---------------------------------------------------------
//
CDeviceDialogObserver::CDeviceDialogObserver( CCellularDataConfirmation* aNotifier ): 
    iNotifier( aNotifier )
    {
    
    }

// ---------------------------------------------------------
// CDeviceDialogObserver::~CDeviceDialogObserver()
// ---------------------------------------------------------
//
CDeviceDialogObserver::~CDeviceDialogObserver()
    {
    
    }

// ---------------------------------------------------------
// void CConfirmationQuery::SetChoices
// ---------------------------------------------------------
//
void CDeviceDialogObserver::SetChoices( RArray<TMsgQueryLinkedResults>& aChoices )
    {
    iChoiceIds = aChoices;
    }

// ---------------------------------------------------------
// void CDeviceDialogObserver::DataReceived
// ---------------------------------------------------------
//
void CDeviceDialogObserver::DataReceived( CHbSymbianVariantMap& aData )
{
    // The key that is returned by the promptdialog is "button",
    // meaning the id of the pressed button.
    TInt intData = *(TInt*)(aData.Get(KButton)->Data());
    
    TInt status = KErrNone;
    
    iNotifier->SetSelectedChoice( iChoiceIds[intData-1] );
    TRAP_IGNORE( iNotifier->CompleteL( status ));

}

// ---------------------------------------------------------
// void CDeviceDialogObserver::DeviceDialogClosed
// ---------------------------------------------------------
//
void CDeviceDialogObserver::DeviceDialogClosed( TInt /*aCompletionCode*/ )
{
    // Dialog was closed, let's complete with that error code
    TInt status = KErrCancel;
    iNotifier->SetSelectedChoice( EMsgQueryCancelled );
    TRAP_IGNORE( iNotifier->CompleteL( status ));
}


