/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CApMultiLineDataQueryDialog.
*
*/


// INCLUDE FILES


#include    "ApMultiLineDataQueryDialog.h"
#include    "ApSettingsHandlerLogger.h"


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CApMultiLineDataQueryDialog::NewL
// ---------------------------------------------------------
//
CApMultiLineDataQueryDialog* CApMultiLineDataQueryDialog::NewL(
                           TDes&  aText1, TDes&  aText2,
                           TTone aTone )
    {
    APSETUILOGGER_ENTERFN( EOther,"MultiQuery::NewL")
    
    CApMultiLineDataQueryDialog* self = 
        new (ELeave) CApMultiLineDataQueryDialog(aTone);
    CleanupStack::PushL(self);

    self->SetDataL( aText1, aText2 );

    CleanupStack::Pop(self);
    
    APSETUILOGGER_LEAVEFN( EOther,"MultiQuery::NewL")
    return self;
    }


// ---------------------------------------------------------
// CApMultiLineDataQueryDialog::CApMultiLineDataQueryDialog
// ---------------------------------------------------------
//
CApMultiLineDataQueryDialog::CApMultiLineDataQueryDialog( const TTone& aTone )
: CAknMultiLineDataQueryDialog( aTone )
    {
    APSETUILOGGER_ENTERFN( EOther,"MultiQuery::CApMultiLineDataQueryDialog<->")
    }


// ---------------------------------------------------------
// CApMultiLineDataQueryDialog::UpdateLeftSoftKeyL
// ---------------------------------------------------------
//
void CApMultiLineDataQueryDialog::UpdateLeftSoftKeyL()
    {
    APSETUILOGGER_ENTERFN( EOther,"MultiQuery::UpdateLeftSoftKeyL")
    
    CAknMultiLineDataQueryDialog::UpdateLeftSoftKeyL();
    MakeLeftSoftkeyVisible( ETrue );
    
    APSETUILOGGER_LEAVEFN( EOther,"MultiQuery::UpdateLeftSoftKeyL")
    }

// End of File
