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
* Description:  Declaration of the CApNetworks class.
*
*/


// INCLUDE FILES
#include <cdbcols.h>
#include <commdb.h>
#include <eikdef.h>
#include <d32dbms.h>
#include <ApNetworkItem.h>
#include <ApNetworkItemList.h>
#include <ApEngineConsts.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ProtectDb.h>
#else
#include <comms-infras/commdb/protection/protectdb.h>
#endif

#include "ApNetworks.h"
#include "ApEngineLogger.h"
#include "ApEngineCommons.h"




// LOCAL CONSTANTS AND MACROS

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApNetworks::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApNetworks* CApNetworks::NewLC( CCommsDatabase& aDb )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::NewLC" ) ) );

    CApNetworks* db = new( ELeave ) CApNetworks;
    CleanupStack::PushL( db );
    db->ConstructL( aDb );

    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::NewLC" ) ) );
    return db;
    }


// Destructor
// ---------------------------------------------------------
// CApNetworks::~CApNetworks
// ---------------------------------------------------------
//
EXPORT_C CApNetworks::~CApNetworks()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::~CApNetworks" ) ) );

    if ( iApList )
        {
        iApList->ResetAndDestroy();
        delete iApList;
        }

    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::~CApNetworks" ) ) );
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApNetworks::CApNetworks
// ---------------------------------------------------------
//
EXPORT_C CApNetworks::CApNetworks()
:iCount( 0 )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApNetworks::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworks::ConstructL( CCommsDatabase& aDb )

    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::ConstructL" ) ) );

    iApList = new( ELeave )CApNetworkItemList();
    iDb = &aDb;
    DoUpdateL();

    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::ConstructL" ) ) );
    }



// ---------------------------------------------------------
// CApNetworks::Count
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApNetworks::Count() const
    {
    CLOG( ( ENetworks, 0, _L( "<-> CApNetworks::Count" ) ) );

    return iApList->Count( );
    }


// ---------------------------------------------------------
// CApNetworks::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApNetworks::NameL( TUint32 aUid ) const
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::NameL" ) ) );

    CApNetworkItem* item = 
        CONST_CAST( CApNetworkItem*, iApList->ItemForUid( aUid ) );
    if ( item == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::NameL" ) ) );
    return (item->Name() );
    }






// ---------------------------------------------------------
// CApNetworks::AllListItemDataL
// ---------------------------------------------------------
//
EXPORT_C TInt CApNetworks::AllListItemDataL( CApNetworkItemList& aList )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::AllListItemDataL" ) ) );

    aList.ResetAndDestroy();

    CApNetworkItem* item=NULL;

    TInt i;
    TInt count = iApList->Count();
    for ( i=0; i<count; i++ )
        {
        item = CApNetworkItem::NewLC( ( *iApList )[i] );
        aList.AppendL( item );
        CleanupStack::Pop( item ); // item owned by aList !
        }
    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::AllListItemDataL" ) ) );

    return aList.Count();
    }




// ================= OTHER EXPORTED FUNCTIONS ==============

// ================== OTHER MEMBER FUNCTIONS ===============



// ---------------------------------------------------------
// CApNetworks::DoUpdate
// ---------------------------------------------------------
//
TInt CApNetworks::DoUpdateL()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworks::DoUpdateL" ) ) );

    TInt err( KErrNone );

    TBool OwnTransaction = ApCommons::StartPushedTransactionLC( *iDb );

    CCommsDbTableView* networkt =  iDb->OpenTableLC( TPtrC(NETWORK) );

    err = networkt->GotoFirstRecord();

    if ( ( err == KErrNone ) || ( err == KErrNotFound ) )
        { // even if it is empty, we must clear the list.
        // otherwise, Leave
        // fill up list. First empty it
        iApList->ResetAndDestroy();
        iCount = 0;
        TUint32 temp( 0 );
        TInt length( 0 );

        CApNetworkItem* item = NULL;
        CApNetworkItemList* tmpnetlist = new( ELeave )CApNetworkItemList();
        HBufC* tmp = NULL;
        CleanupStack::PushL( tmpnetlist );
        
        if ( err == KErrNone )
            { // now get the names and UIDs
            TBool goon( ETrue );
            do
                {
                err = ApCommons::ReadUintL( networkt, TPtrC(COMMDB_ID), temp );

                networkt->ReadColumnLengthL( TPtrC(COMMDB_NAME), length );
                tmp = HBufC::NewLC( length );
                TPtr tmpptr( tmp->Des() );
                networkt->ReadTextL( TPtrC(COMMDB_NAME), tmpptr );
                item = CApNetworkItem::NewLC( );
                item->SetUid( temp );
                item->SetNameL( tmpptr );
                tmpnetlist->AppendL( item );
                CleanupStack::Pop( item ); // item owned by list !

                err = networkt->GotoNextRecord();
                if ( err == KErrNotFound )
                    {
                    goon = EFalse;
                    }
                else
                    {
                    User::LeaveIfError( err );
                    }
                CleanupStack::PopAndDestroy( tmp );
                }
                while ( goon );
            }
        // and now move items to real array
        CleanupStack::Pop( tmpnetlist );
        delete iApList;
        iApList = tmpnetlist;
        }
    else
        {
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( networkt );
    
    if ( OwnTransaction )
        {
        ApCommons::CommitTransaction( *iDb );
        CleanupStack::Pop(); // RollbackTransactionOnLeave
        }
    

    CLOG( ( ENetworks, 1, _L( "<- CApNetworks::DoUpdateL" ) ) );
    return err;
    }



// End of File
