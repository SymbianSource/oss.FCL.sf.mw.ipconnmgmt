/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Execute command without blocking the process. Needed for
*                touch UI implementation.
*
*/

#include <e32base.h>
#include <mcmdexec.h>
#include "ccmdexec.h"

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CCmdExec::CCmdExec(MCmdExec& aOwner) :
    CActive( CActive::EPriorityStandard ),
    iOwner(aOwner)
    {
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CCmdExec::~CCmdExec()
    {
    Cancel();
    }

void CCmdExec::Execute()
    {
    // Cancel possible previous request
    Cancel();
    
    // Do request
    TRequestStatus *requestStatus = &iStatus;
    User::RequestComplete(requestStatus, KErrNone);
    SetActive();
    }
            
void CCmdExec::RunL()
    {
    iOwner.Execute();
    }

TInt CCmdExec::RunError( TInt aError )
    {
    if (aError != 0)
        iOwner.HandleLeaveError( aError );
    return KErrNone;
    }

void CCmdExec::DoCancel()
    {
    
    }
