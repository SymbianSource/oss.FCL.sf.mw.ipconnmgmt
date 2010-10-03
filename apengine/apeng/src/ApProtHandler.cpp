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
* Description:  Declaration of the CApProtHandler class.
*
*/


// INCLUDE FILES
#include <cdbcols.h>
#include <commdb.h>
#include <ApEngineConsts.h>

#include <comms-infras/commdb/protection/protectdb.h>
#include <comms-infras/commdb/protection/protectcpdb.h>

#include <ApProtHandler.h>
#include <ApListItem.h>
#include <ApListItemList.h>
#include <ApDataHandler.h>
#include <ApSelect.h>

#include "ApEngineCommons.h"
#include "APEngineVariant.hrh"
#include "ApEngineLogger.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CApProtHandler::IsTableProtectedL
// ---------------------------------------------------------
//
EXPORT_C TBool CApProtHandler::IsTableProtectedL( CCommsDatabase* aDb )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApProtHandler::IsApProtectedL
// ---------------------------------------------------------
//
EXPORT_C TBool CApProtHandler::IsApProtectedL( CCommsDatabase* aDb,
                                                     TUint32 aUid )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectItemL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectItemL( CCommsDatabase* aDb,
                                             TUint32 aUid )
    {
    }


// ---------------------------------------------------------
// CApProtHandler::UnprotectItemL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectItemL( CCommsDatabase* aDb,
                                               TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApProtHandler::ProtectTableL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectTableL( CCommsDatabase* aDb )
    {
    }


// ---------------------------------------------------------
// CApProtHandler::UnprotectTableL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectTableL( CCommsDatabase* aDb )
    {
    }


// ---------------------------------------------------------
// CApProtHandler::IsPreferencesProtectedL
// ---------------------------------------------------------
//
EXPORT_C TBool CApProtHandler::IsPreferencesProtectedL( CCommsDatabase* aDb )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectPreferencesL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectPreferencesL( CCommsDatabase* aDb )
    {
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectPreferencesL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectPreferencesL( CCommsDatabase* aDb )
    {
    }



// ---------------------------------------------------------
// CApProtHandler::RemoveProtectedAccessPointsL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::RemoveProtectedAccessPointsL(
                                          CCommsDatabase* aDb)
    {
    }

// ================== OTHER MEMBER FUNCTIONS ===============





// ---------------------------------------------------------
// CApProtHandler::IsRecordProtectedL
// ---------------------------------------------------------
//
TBool CApProtHandler::IsRecordProtectedL( CCommsDatabase* aDb,
                                            const TDesC& aTable,
                                            TUint32 aUid )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// CApProtHandler::ProtectRecordL
// ---------------------------------------------------------
//
void CApProtHandler::ProtectRecordL( CCommsDatabase* aDb,
                                    const TDesC& aTable, TUint32 aUid )
    {
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectRecordL
// ---------------------------------------------------------
//
void CApProtHandler::UnprotectRecordL( CCommsDatabase* aDb,
                                      const TDesC& aTable, TUint32 aUid )
    {
    }




// ---------------------------------------------------------
// CApProtHandler::IsTableProtectedL
// ---------------------------------------------------------
//
TBool CApProtHandler::IsTableProtectedL( CCommsDatabase* aDb,
                                               const TDesC& aTable )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectTableL
// ---------------------------------------------------------
//
void CApProtHandler::ProtectTableL( CCommsDatabase* aDb,
                                     const TDesC& aTable )
    {
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectTableL
// ---------------------------------------------------------
//
void CApProtHandler::UnprotectTableL( CCommsDatabase* aDb,
                                       const TDesC& aTable )
    {
    }


// ---------------------------------------------------------
// CApProtHandler::GetProtectionStateL
// ---------------------------------------------------------
//
TBool CApProtHandler::GetProtectionStateL( CCommsDbTableView& aTable )
    {
    return 0;
    }



// ---------------------------------------------------------
// CApProtHandler::DoRemoveProtectedAccessPointsL
// ---------------------------------------------------------
//
void CApProtHandler::DoRemoveProtectedAccessPointsL(
                                          CCommsDatabase& aDb)
    {
    }


//  End of File
