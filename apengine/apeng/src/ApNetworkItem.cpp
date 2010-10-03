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
    return NULL;
    }


// ---------------------------------------------------------
// CApNetworkItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem* CApNetworkItem::NewLC( CApNetworkItem* aItem )
    {
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApNetworkItem::~CApNetworkItem
// ---------------------------------------------------------
//
EXPORT_C CApNetworkItem::~CApNetworkItem()
    {
    }



// ---------------------------------------------------------
// CApNetworkItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::CopyFromL( const CApNetworkItem& aCopyFrom )
    {
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
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApNetworkItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::ConstructL( TUint32 aUid,
                                          const TDesC& aName )
    {
    }



// ---------------------------------------------------------
// CApNetworkItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApNetworkItem::Name() const
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApNetworkItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApNetworkItem::Uid() const
    {
    return 0;
    }



// ---------------------------------------------------------
// CApNetworkItem::SetUid
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::SetUid( TUint32 aUid )
    {
    }


// ---------------------------------------------------------
// CApNetworkItem::SetUidL
// ---------------------------------------------------------
//
EXPORT_C void CApNetworkItem::SetNameL( const TDesC& aName )
    {
    }

//  End of File
