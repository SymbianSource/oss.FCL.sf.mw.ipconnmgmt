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
* Description: 
*     Declaration of the CApNetworkItem class.
*
*/


// INCLUDE FILES
#include <eikdef.h>

#include "ApNetworkItem.h"
#include "ApEngineLogger.h"


// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApNetworkItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem* CApNetworkItem::NewLC()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::NewLC()" ) ) );

    CApNetworkItem* db = new( ELeave ) CApNetworkItem;
    CleanupStack::PushL( db );
    db->ConstructL();

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::NewLC()" ) ) );
    return db;
    }


// ---------------------------------------------------------
// CApNetworkItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem* CApNetworkItem::NewLC( CApNetworkItem* aItem )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::NewLC" ) ) );

    CApNetworkItem* db = new( ELeave ) CApNetworkItem;
    CleanupStack::PushL( db );
    db->ConstructL( aItem->iUid, *aItem->iName );

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::NewLC" ) ) );
    return db;
    }


// Destructor
// ---------------------------------------------------------
// CApNetworkItem::~CApNetworkItem
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem::~CApNetworkItem()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::~CApNetworkItem" ) ) );

    delete iName;
    }



// ---------------------------------------------------------
// CApNetworkItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::CopyFromL( const CApNetworkItem& aCopyFrom )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::CopyFromL" ) ) );

    HBufC* temp = NULL;
    temp = aCopyFrom.iName->AllocL();
    delete iName;
    iName = temp;

    iUid = aCopyFrom.iUid;

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::CopyFromL" ) ) );
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApNetworkItem::CApNetworkItem
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem::CApNetworkItem()
:iUid( 0 )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApNetworkItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::ConstructL()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::ConstructL()" ) ) );

    iName = HBufC::NewL( 0 );
    
    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::ConstructL()" ) ) );
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApNetworkItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::ConstructL( TUint32 aUid,
                                          const TDesC& aName )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::ConstructL" ) ) );

    ConstructL();
    SetUid( aUid );
    SetNameL( aName );

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::ConstructL" ) ) );
    }



// ---------------------------------------------------------
// CApNetworkItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApNetworkItem::Name() const
    {
    CLOG( ( ENetworks, 0, _L( "<-> CApNetworkItem::Name" ) ) );

    return *iName;
    }


// ---------------------------------------------------------
// CApNetworkItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApNetworkItem::Uid() const
    {
    CLOG( ( ENetworks, 0, _L( "<-> CApNetworkItem::Uid" ) ) );

    return iUid;
    }



// ---------------------------------------------------------
// CApNetworkItem::SetUid
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::SetUid( TUint32 aUid )
    {
    CLOG( ( ENetworks, 0, _L( "<-> CApNetworkItem::SetUid" ) ) );

    iUid = aUid;
    
    }


// ---------------------------------------------------------
// CApNetworkItem::SetUidL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::SetNameL( const TDesC& aName )
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::SetNameL" ) ) );

    HBufC* temp = NULL;
    temp = aName.AllocL();
    delete iName;
    iName = temp;
    
    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::SetNameL" ) ) );
    }

//  End of File
