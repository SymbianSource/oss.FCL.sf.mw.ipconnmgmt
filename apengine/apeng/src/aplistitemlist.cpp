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
*     Declaration of the CApSelect class.
*
*/


// INCLUDE FILES

#include "ApListItemList.h"
#include "ApListItem.h"
#include "ApEngineLogger.h"


// CONSTANTS

LOCAL_D const TInt KGranularity = 4;    ///< Granularity of the list.

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApListItemList::CApListItemList
// ---------------------------------------------------------
//
EXPORT_C CApListItemList::CApListItemList()
: CArrayPtrFlat<CApListItem>( KGranularity )
    {
    }

// Destructor
// ---------------------------------------------------------
// CApListItemList::~CApListItemList
// ---------------------------------------------------------
//
EXPORT_C CApListItemList::~CApListItemList()
    {
    }

// ---------------------------------------------------------
// CApListItemList::ItemByUid
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItemList::ItemForUid( TUint32 aUid ) const
    {
    return NULL;
    }

//  End of File
