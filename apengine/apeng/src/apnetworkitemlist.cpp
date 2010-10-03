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
    }

// Destructor
// ---------------------------------------------------------
// CApNetworkItemList::~CApNetworkItemList
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItemList::~CApNetworkItemList()
    {
    }


// ---------------------------------------------------------
// CApNetworkItemList::ItemForUid
// ---------------------------------------------------------
//
EXPORT_C const CApNetworkItem* CApNetworkItemList::ItemForUid
( TUint32 aUid ) const
    {
    return NULL;
    }

//  End of File
