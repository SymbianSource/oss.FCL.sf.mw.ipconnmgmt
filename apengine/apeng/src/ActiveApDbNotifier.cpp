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
* Description:  Implementation of CActiveApDbNotifier.
*
*/


// INCLUDE FILES

#include "ActiveApDb.h"
#include "ActiveApDbNotifier.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CActiveApDbNotifier::CActiveApDbNotifier
// ---------------------------------------------------------
//
CActiveApDbNotifier::CActiveApDbNotifier( CActiveApDb& aDb )
: CActive( EPriorityStandard )
    {
    iActiveDb = &aDb;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::~CActiveApDbNotifier
// ---------------------------------------------------------
//
CActiveApDbNotifier::~CActiveApDbNotifier()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::RunL
// ---------------------------------------------------------
//
void CActiveApDbNotifier::RunL()
    {
    if ( iStatus == KErrCancel )
        {
        // Do nothing.
        }
    else
        {
        // first store status, because re-schedule will be earlier 
        // than notifying clients. Reason:
        // clients can leave but re-schedule MUST happen anyway,
        // to keep the notifier alive even if any client leaves
        TRequestStatus tempstatus( iStatus );

        if ( iStatus == RDbNotifier::EClose )
            {
            // Finish if the database is closed.
            NotifyChange();
            }
        else
            {
            // Reschedule automatically (as long as the database is open).
            NotifyChange();
            }
        // and now let clients handle their stuff...
        TRAP_IGNORE( iActiveDb->HandleDbEventL( tempstatus.Int() ) );
        }
    }


// ---------------------------------------------------------
// CActiveApDbNotifier::DoCancel
// ---------------------------------------------------------
//
void CActiveApDbNotifier::DoCancel()
    {
    Stop();
    }


// ---------------------------------------------------------
// CActiveApDbNotifier::Stop
// ---------------------------------------------------------
//
void CActiveApDbNotifier::Stop()
    {
    iActiveDb->Database()->CancelRequestNotification();
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::Start
// ---------------------------------------------------------
//
void CActiveApDbNotifier::Start()
    {
    NotifyChange();
    }

// ---------------------------------------------------------
// CActiveApDbNotifier::NotifyChange
// ---------------------------------------------------------
//
void CActiveApDbNotifier::NotifyChange()
    {
    if ( !IsActive() )
        {
        iActiveDb->Database()->RequestNotification( iStatus );
        SetActive();
        }
    }

// End of File
