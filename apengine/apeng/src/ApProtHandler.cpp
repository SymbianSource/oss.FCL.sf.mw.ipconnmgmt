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

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ProtectDB.h>
#include <ProtectcpDb.h>
#else
#include <comms-infras/commdb/protection/protectdb.h>
#include <comms-infras/commdb/protection/protectcpdb.h>
#endif

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
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::IsTableProtectedL" ) ) );

    TBool retval( EFalse );
    retval = IsTableProtectedL( aDb, TPtrC(WAP_ACCESS_POINT) );

    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::IsTableProtectedL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApProtHandler::IsApProtectedL
// ---------------------------------------------------------
//
EXPORT_C TBool CApProtHandler::IsApProtectedL( CCommsDatabase* aDb,
                                                     TUint32 aUid )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::IsApProtectedL" ) ) );

    TBool retval( EFalse );
    retval = IsRecordProtectedL( aDb, TPtrC(WAP_ACCESS_POINT), aUid );
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::IsApProtectedL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectItemL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectItemL( CCommsDatabase* aDb,
                                             TUint32 aUid )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::ProtectItemL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    ProtectRecordL( aDb, TPtrC(WAP_ACCESS_POINT), aUid );
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::ProtectItemL" ) ) );
    }


// ---------------------------------------------------------
// CApProtHandler::UnprotectItemL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectItemL( CCommsDatabase* aDb,
                                               TUint32 aUid )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::UnprotectItemL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    UnprotectRecordL( aDb, TPtrC(WAP_ACCESS_POINT), aUid );
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::UnprotectItemL" ) ) );
    }



// ---------------------------------------------------------
// CApProtHandler::ProtectTableL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectTableL( CCommsDatabase* aDb )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::ProtectTableL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    ProtectTableL( aDb, TPtrC(WAP_ACCESS_POINT) );
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::ProtectTableL" ) ) );
    }


// ---------------------------------------------------------
// CApProtHandler::UnprotectTableL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectTableL( CCommsDatabase* aDb )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::UnprotectTableL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    UnprotectTableL( aDb, TPtrC(WAP_ACCESS_POINT) );

    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::UnprotectTableL" ) ) );
    }


// ---------------------------------------------------------
// CApProtHandler::IsPreferencesProtectedL
// ---------------------------------------------------------
//
EXPORT_C TBool CApProtHandler::IsPreferencesProtectedL( CCommsDatabase* aDb )
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::IsPreferencesProtectedL" ) ) );

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }

    CCommsDbProtectConnectPrefTableView* table = 
        STATIC_CAST( CCommsDbProtectConnectPrefTableView*, 
                     db->OpenConnectionPrefTableLC());

    TInt err = table->GotoFirstRecord();
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    RDbRowSet::TAccess access;
    User::LeaveIfError( table->GetTableAccess( access ) ); 
    CleanupStack::PopAndDestroy( table ); 
    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); //db
        }
    
    //access is EUpdatable if record is updatable
    TBool retval( !(access == RDbRowSet::EUpdatable) );

    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::IsPreferencesProtectedL" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectPreferencesL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::ProtectPreferencesL( CCommsDatabase* aDb )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::ProtectPreferencesL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }

    CCommsDbProtectConnectPrefTableView* table = 
        STATIC_CAST( CCommsDbProtectConnectPrefTableView*, 
                     db->OpenConnectionPrefTableLC());

    User::LeaveIfError( table->GotoFirstRecord() );
    User::LeaveIfError( table->ProtectTable() );
    
    CleanupStack::PopAndDestroy( table ); 

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); //db
        }

    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::ProtectPreferencesL" ) ) );
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectPreferencesL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::UnprotectPreferencesL( CCommsDatabase* aDb )
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::UnprotectPreferencesL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
    CCommsDbProtectConnectPrefTableView* table = 
        STATIC_CAST( CCommsDbProtectConnectPrefTableView*, 
                     db->OpenConnectionPrefTableLC());

    User::LeaveIfError( table->GotoFirstRecord() );
    User::LeaveIfError( table->UnprotectTable() );
    
    CleanupStack::PopAndDestroy( table );
    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); //db
        }
    
    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::UnprotectPreferencesL" ) ) );
    }



// ---------------------------------------------------------
// CApProtHandler::RemoveProtectedAccessPointsL
// ---------------------------------------------------------
//
EXPORT_C void CApProtHandler::RemoveProtectedAccessPointsL(
                                          CCommsDatabase* aDb)
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::RemoveProtectedAccessPointsL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
    else
        {
        db = aDb;
        }
    DoRemoveProtectedAccessPointsL( *db );

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }

    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::RemoveProtectedAccessPointsL" ) ) );
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
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::IsRecordProtectedL" ) ) );

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }

    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *db, ETrue );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenViewMatchingUintLC
                                             (
                                              aTable,
                                              TPtrC(COMMDB_ID),
                                              aUid
                                             );

    TInt err = view->GotoFirstRecord();
    User::LeaveIfError( err );

    TInt prot( EFalse );
    User::LeaveIfError( view->GetRecordAccess( prot ) );

    CleanupStack::PopAndDestroy( view ); // view

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *db );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }
        
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::IsRecordProtectedL" ) ) );
    return prot;
    }



// ---------------------------------------------------------
// CApProtHandler::ProtectRecordL
// ---------------------------------------------------------
//
void CApProtHandler::ProtectRecordL( CCommsDatabase* aDb,
                                    const TDesC& aTable, TUint32 aUid )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::ProtectRecordL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *db, ETrue );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenViewMatchingUintLC
                                                (
                                                 aTable,
                                                 TPtrC(COMMDB_ID),
                                                 aUid
                                                );

    TInt err = view->GotoFirstRecord();
    User::LeaveIfError( err );

    User::LeaveIfError( view->ProtectRecord() );

    CleanupStack::PopAndDestroy( view ); // view

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *db );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::ProtectRecordL" ) ) );
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectRecordL
// ---------------------------------------------------------
//
void CApProtHandler::UnprotectRecordL( CCommsDatabase* aDb,
                                      const TDesC& aTable, TUint32 aUid )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::UnprotectRecordL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *db, ETrue );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenViewMatchingUintLC
                                             (
                                              aTable,
                                              TPtrC(COMMDB_ID),
                                              aUid
                                             );

    TInt err = view->GotoFirstRecord();
    User::LeaveIfError( err );

    User::LeaveIfError( view->UnprotectRecord() );

    CleanupStack::PopAndDestroy( view ); // view

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *db );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }

    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::UnprotectRecordL" ) ) );
    }




// ---------------------------------------------------------
// CApProtHandler::IsTableProtectedL
// ---------------------------------------------------------
//
TBool CApProtHandler::IsTableProtectedL( CCommsDatabase* aDb,
                                               const TDesC& aTable )
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::IsTableProtectedL(table)" ) ) );

    TBool retval( EFalse );
    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *db, ETrue );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenTableLC( aTable );

    RDbRowSet::TAccess aAccessType( RDbRowSet::EUpdatable );
    
    User::LeaveIfError( view->GetTableAccess( aAccessType) );
    
    if ( aAccessType == RDbRowSet::EUpdatable )
        {
        retval = EFalse;
        }
    else
        { // EReadOnly, EInsertOnly
        retval = ETrue;
        }

    CleanupStack::PopAndDestroy( view ); // view

    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *db );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }
    
    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::IsTableProtectedL(table)" ) ) );
    return retval;
    }


// ---------------------------------------------------------
// CApProtHandler::ProtectTableL
// ---------------------------------------------------------
//
void CApProtHandler::ProtectTableL( CCommsDatabase* aDb,
                                     const TDesC& aTable )
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::ProtectTableL(table)" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenTableLC( aTable );

    TInt err = view->GotoFirstRecord();
    User::LeaveIfError( err );

    User::LeaveIfError( view->ProtectTable() );

    CleanupStack::PopAndDestroy( view ); // view

    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); //db
        }

    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::ProtectTableL(table)" ) ) );
    }



// ---------------------------------------------------------
// CApProtHandler::UnprotectTableL
// ---------------------------------------------------------
//
void CApProtHandler::UnprotectTableL( CCommsDatabase* aDb,
                                       const TDesC& aTable )
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::UnprotectTableL(table)" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    CCommsDatabase* db = aDb;
    if ( !aDb )
        {
        // create DB object
        db = CCommsDatabase::NewL( ETrue );
        CleanupStack::PushL( db );
        }
//    Follow related error rep. and act accordingly:
//    JBAH-58FCHH
//    TBool ownTransaction = ApCommons::StartPushedTransactionLC( *db, ETrue );

    CCommsDbProtectTableView* view =
        (CCommsDbProtectTableView*)db->OpenTableLC( aTable );


    TInt err = view->GotoFirstRecord();
    // If not found, no problem, Unprotect will be carried out correctly
    // But if no GotoFirstRecord, and no records, it will Panic...
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }

    User::LeaveIfError( view->UnprotectTable() );

    CleanupStack::PopAndDestroy( view ); // view


/*
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *db );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
*/
    if ( !aDb )
        {
        CleanupStack::PopAndDestroy( db ); // db
        }

    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::UnprotectTableL(table)" ) ) );
    }


// ---------------------------------------------------------
// CApProtHandler::GetProtectionStateL
// ---------------------------------------------------------
//
TBool CApProtHandler::GetProtectionStateL( CCommsDbTableView& aTable )
    {
    CLOG( ( EProtection, 0, _L( "-> CApProtHandler::GetProtectionStateL" ) ) );

    TInt aAccess( 0 );
    User::LeaveIfError(
        ( (CCommsDbProtectTableView*) &aTable )->GetRecordAccess( aAccess ) );
    
    CLOG( ( EProtection, 1, _L( "<- CApProtHandler::GetProtectionStateL" ) ) );
    return aAccess;
    }



// ---------------------------------------------------------
// CApProtHandler::DoRemoveProtectedAccessPointsL
// ---------------------------------------------------------
//
void CApProtHandler::DoRemoveProtectedAccessPointsL(
                                          CCommsDatabase& aDb)
    {
    CLOG( ( EProtection, 0, 
        _L( "-> CApProtHandler::DoRemoveProtectedAccessPointsL" ) ) );

    if ( ApCommons::GetVariantL() & KApUiEditOnlyVPNs )
        {
        User::Leave( KErrNotSupported );
        }
#ifdef __TEST_CDMA_WRITE_PROTECT
    User::Leave( KErrNotSupported );
#endif // __TEST_CDMA_WRITE_PROTECT

    // TRANSACTION STARTING IS REMOVED UNTIL ERROR 
    // JBAH-58FCHH IS CORRECTED BY SYMBIAN
    // start a transaction, just to make sure
//    TBool ownTransaction = ApCommons::StartPushedTransactionLC( aDb, ETrue );
    
    // make AP table updatable
    UnprotectTableL( &aDb );

    // remove all protected AP
    // first get a list of current AP-s
    CApListItemList* list = new( ELeave )CApListItemList();
    CleanupStack::PushL( list );
    CApDataHandler* handler = CApDataHandler::NewLC( aDb );
    CApSelect* sel = 
        CApSelect::NewLC( aDb, KEApIspTypeAll, EApBearerTypeAllBearers,
                         KEApSortNameAscending);
    sel->AllListItemDataL( *list );
    // now get protected ones and delete them
    TInt count( list->Count() );
    for( TInt i = 0; i<count; i++)
        {
        if ( list->At( i )->IsReadOnly() )
            {
            UnprotectItemL( &aDb, list->At( i )->Uid() );
            handler->RemoveAPL( list->At( i )->Uid() );
            }
        }
    CleanupStack::PopAndDestroy( 3, list ); // sel, handler, list

/*
    if ( ownTransaction )
        {
        ApCommons::CommitTransaction( *aDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
*/
    
    CLOG( ( EProtection, 1, 
        _L( "<- CApProtHandler::DoRemoveProtectedAccessPointsL" ) ) );
    }


//  End of File
