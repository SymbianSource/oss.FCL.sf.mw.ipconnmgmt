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
*     Declaration of the CApNetworkItemList class.
*
*/


// INCLUDE FILES

#include "ApNetworkItemList.h"
#include "ApNetworkItem.h"
#include "ApEngineLogger.h"


// CONSTANTS

LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApNetworkItemList::CApNetworkItemList
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItemList::CApNetworkItemList()
: CArrayPtrFlat<CApNetworkItem>( KGranularity )
    {
    CLOG( ( ENetworks, 0, _L( "<-> CApNetworkItem::CApNetworkItemList" ) ) );
    }

// Destructor
// ---------------------------------------------------------
// CApNetworkItemList::~CApNetworkItemList
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItemList::~CApNetworkItemList()
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::~CApNetworkItemList" ) ) );

    ResetAndDestroy();

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::~CApNetworkItemList" ) ) );
    }


// ---------------------------------------------------------
// CApNetworkItemList::ItemForUid
// ---------------------------------------------------------
//
EXPORT_C const CApNetworkItem* CApNetworkItemList::ItemForUid
( TUint32 aUid ) const
    {
    CLOG( ( ENetworks, 0, _L( "-> CApNetworkItem::ItemForUid" ) ) );

    TInt i;
    TInt count = Count();
    for ( i = 0; i < count; i++ )
        {
        if ( At( i )->Uid() == aUid )
            {
            return At( i );
            }
        }

    CLOG( ( ENetworks, 1, _L( "<- CApNetworkItem::ItemForUid" ) ) );
    return NULL;
    }

//  End of File
