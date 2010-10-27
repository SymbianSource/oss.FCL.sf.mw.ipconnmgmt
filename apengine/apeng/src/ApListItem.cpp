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
#include <eikdef.h>

#include "ApListItem.h"
#include "ApEngineLogger.h"
#include "ApEngineCommons.h"

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================
// Two-phased constructor.
// ---------------------------------------------------------
// CApListItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItem::NewLC()
    {
    return NULL;
    }


// ---------------------------------------------------------
// CApListItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItem::NewLC
                                        (
                                        TCommsDbIspType aIspType,
                                        TUint32 aUid,
                                        const TDesC& aName,
                                        TApBearerType aBearerType
                                        )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    return NULL;
    }


EXPORT_C CApListItem* CApListItem::NewLC
                                        (
                                        TCommsDbIspType aIspType,
                                        TUint32 aUid,
                                        const TDesC& aName,
                                        TApBearerType aBearerType,
                                        const TDesC& aStartPage,
                                        TBool aReadOnly
                                        )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    (void)aStartPage;
    (void)aReadOnly;
    return NULL;
    }

// ---------------------------------------------------------
// CApListItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItem::NewLC( CApListItem* aItem )
    {
    (void)aItem;
    return NULL;
    }


// Destructor
// ---------------------------------------------------------
// CApListItem::~CApListItem
// ---------------------------------------------------------
//
EXPORT_C CApListItem::~CApListItem()
    {
    }



// ---------------------------------------------------------
// CApListItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::CopyFromL( const CApListItem& aCopyFrom )
    {
    (void)aCopyFrom;
    }


// C++ default constructor can NOT contain any code that
// might leave.
//
// ---------------------------------------------------------
// CApListItem::CApListItem
// ---------------------------------------------------------
//
EXPORT_C CApListItem::CApListItem()
:iIspType( EIspTypeInternetAndWAP ),
iUid( 0 ),
iBearerType( EApBearerTypeGPRS )
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApListItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::ConstructL()
    {
    }


// Symbian OS default constructor can leave.
// ---------------------------------------------------------
// CApListItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::ConstructL( TCommsDbIspType aIspType,
                                      TUint32 aUid,
                                      const TDesC& aName,
                                      TApBearerType aBearerType )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    }



// ---------------------------------------------------------
// CApListItem::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::ConstructL( TCommsDbIspType aIspType,
                                      TUint32 aUid,
                                      const TDesC& aName,
                                      TApBearerType aBearerType,
                                      const TDesC& aStartPage
                                      )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    (void)aStartPage;
    }


// ---------------------------------------------------------
// CApListItem::SetItemL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::SetItemL( TCommsDbIspType aIspType,
                                    TUint32 aUid,
                                    const TDesC& aName,
                                    TApBearerType aBearerType )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    }


// ---------------------------------------------------------
// CApListItem::SetItemL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::SetItemL( TCommsDbIspType aIspType,
                                    TUint32 aUid,
                                    const TDesC& aName,
                                    TApBearerType aBearerType,
                                    const TDesC& aStartpage
                                    )
    {
    (void)aIspType;
    (void)aUid;
    (void)aName;
    (void)aBearerType;
    }


// ---------------------------------------------------------
// CApListItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApListItem::Name()
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApListItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApListItem::Uid()
    {
    return 0;
    }


// ---------------------------------------------------------
// CApListItem::Type
// ---------------------------------------------------------
//
EXPORT_C TCommsDbIspType CApListItem::Type()
    {
    User::Leave( KErrNotSupported );
    }



// ---------------------------------------------------------
// CApListItem::BearerType
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApListItem::BearerType()
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApListItem::StartPage
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApListItem::StartPage() const
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CApListItem::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApListItem::IsReadOnly() const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApListItem::IsVpnAp
// ---------------------------------------------------------
//
TBool CApListItem::IsVpnAp() const
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CApListItem::CreateExtraL
// ---------------------------------------------------------
//
void CApListItem::CreateExtraL()
    {
    }


// ---------------------------------------------------------
// CApListItem::SetVpnFlag
// ---------------------------------------------------------
//
void CApListItem::SetVpnFlag( TBool aIsVpn )
    {
    (void)aIsVpn;
    }

//  End of File
