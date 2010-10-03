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
* Description:  Implementation of class CApSettingsDeleteOperation
*
*/


// INCLUDE FILES
#include <ApDataHandler.h>
#include <ApUtils.h>
#include <ActiveApDb.h>
#include <ApSettingsHandlerCommons.h>
#include <VpnApEngine.h>
#include <AknWaitDialog.h>
#include <AknProgressDialog.h>
#include <cdbcols.h>
#include <ErrorUI.h>
#include "Apsettingsdeleteoperation.h"

#include <ApAccessPointItem.h>

// Following includes are needed only for the test
// when ALL APs are deleted (checking delays, etc.)
// For testing purpose only, to make process time longer.
//  TEST STARTS
#include <ActiveApDb.h>
#include <ApSelect.h>
#include <ApListItem.h>
#include "ApSelectorListBoxModel.h"
#include "ApSettingsHandlerLogger.h"

#include <apsetui.rsg>
//  TEST ENDS


// Stack size of the receiver thread.
LOCAL_C const TInt KDeleteThreadStackSize = 40960;

// Name of the receiver thread.
_LIT( KDeleteThreadName, "Access Point Settings UI Delete Operation" );


// ---------------------------------------------------------
// CApSettingsDeleteOperation::NewL
// ---------------------------------------------------------
//
CApSettingsDeleteOperation* CApSettingsDeleteOperation::NewL(
                                TInt aNoteControlId,
                                TInt aResourceID,
                                TUint32 aUid,
                                const TDesC& aNoteText
                                )
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::ConstructL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::ConstructL( const TDesC& aNoteText )
    {
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::CApSettingsDeleteOperation
// ---------------------------------------------------------
//
CApSettingsDeleteOperation::CApSettingsDeleteOperation(
                                 TInt aNoteControlId,
                                 TInt aResourceID,
                                 TUint32 aUid
                                 )
: CActive( EPriorityLow ),
  iNoteControlId( aNoteControlId ),
  iResourceID( aResourceID ),
  iUid( aUid )
    {
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::~CApSettingsDeleteOperation
// ---------------------------------------------------------
//
CApSettingsDeleteOperation::~CApSettingsDeleteOperation()
    {
    }




// ---------------------------------------------------------
// CApSettingsDeleteOperation::RunL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::RunL()
    {
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::DoCancel
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::DoCancel()
    { // must be here as it is pure virtual in base...    
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::DialogDismissedL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::DialogDismissedL( TInt /*aButtonId*/ )
    {
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartL()
    {
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::Next
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::Next()
    {
    }




// ---------------------------------------------------------
// CApSettingsDeleteOperation::RunDeleteThread
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::RunDeleteThread( TAny* aPtr )
    {
    return ( 0 );
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::DoRunDeleteThreadL
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::DoRunDeleteThreadL( 
                                    CApSettingsDeleteOperation* aop )
    {
    return KErrNotSupported;
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartWaitNoteL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartWaitNoteL()
    {
    }



// ---------------------------------------------------------
// CApSettingsDeleteOperation::StartDeleteOperationThreadL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::StartDeleteOperationThreadL()
    {
    }





// ---------------------------------------------------------
// CApSettingsDeleteOperation::StarTransactionLC
// ---------------------------------------------------------
//
TBool CApSettingsDeleteOperation::StartTransactionLC( CCommsDatabase* aDb,
                                                      TBool aWrite, 
                                                      TBool aRetry /*ETrue*/ )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::GetWriteLockL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::GetWriteLockL( CCommsDatabase* aDb )
    {
    }

// ---------------------------------------------------------
// CApSettingsDeleteOperation::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::RollbackTransactionOnLeave( TAny* aDb )
    {
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::ReadUintL
// ---------------------------------------------------------
//
TInt CApSettingsDeleteOperation::ReadUintL( CCommsDbTableView* atable,
                                            const TDesC& aColumn,
                                            TUint32& aValue )
    {
    return KErrNotSupported;
    }


// ---------------------------------------------------------
// CApSettingsDeleteOperation::SetNoteTextL
// ---------------------------------------------------------
//
void CApSettingsDeleteOperation::SetNoteTextL( const TDesC& aNoteText )
    {
    }
// End of File
