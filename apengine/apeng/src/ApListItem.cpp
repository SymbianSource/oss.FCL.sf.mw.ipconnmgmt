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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    CApListItem* db = new( ELeave ) CApListItem;
    CleanupStack::PushL( db );
    db->ConstructL();

    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
    return db;
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    CApListItem* db = new( ELeave ) CApListItem;
    CleanupStack::PushL( db );
    db->ConstructL( aIspType, aUid, aName, aBearerType );

    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
    return db;
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    CApListItem* db = new( ELeave ) CApListItem;
    CleanupStack::PushL( db );
    db->ConstructL( aIspType, aUid, aName, aBearerType, aStartPage );
    db->iExt->iReadOnly = aReadOnly;
    
    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
    return db;
    }

// ---------------------------------------------------------
// CApListItem::NewLC
// ---------------------------------------------------------
//
EXPORT_C CApListItem* CApListItem::NewLC( CApListItem* aItem )
    {
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    CApListItem* db = new( ELeave ) CApListItem;
    CleanupStack::PushL( db );
    db->ConstructL( aItem->iIspType, aItem->iUid, *aItem->iName,
                    aItem->iBearerType, *aItem->iStartPage );
    db->iExt->iReadOnly = aItem->IsReadOnly();
    
    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
    return db;
    }


// Destructor
// ---------------------------------------------------------
// CApListItem::~CApListItem
// ---------------------------------------------------------
//
EXPORT_C CApListItem::~CApListItem()
    {
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    delete iName;
    delete iStartPage;
    if ( iExt )
        {
        delete iExt;
        }

    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
    }



// ---------------------------------------------------------
// CApListItem::CopyFromL
// ---------------------------------------------------------
//
EXPORT_C void CApListItem::CopyFromL( const CApListItem& aCopyFrom )
    {
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    HBufC* temp = NULL;
    HBufC* temp2 = NULL;
    temp = aCopyFrom.iName->AllocL();
    delete iName;
    iName = temp;

    temp2 = aCopyFrom.iStartPage->AllocL();
    delete iStartPage;
    iStartPage = temp2;

    iUid = aCopyFrom.iUid;
    iIspType =  aCopyFrom.iIspType;
    iBearerType = aCopyFrom.iBearerType;
    iExt->iReadOnly = aCopyFrom.iExt->iReadOnly;

    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    iName = HBufC::NewL( 0 );
    iStartPage = HBufC::NewL( 0 );
    CreateExtraL();
    
    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::NewLC" ) ) );

    ConstructL();
    SetItemL( aIspType, aUid, aName, aBearerType );
    
    CLOG( ( EUtil, 1, _L( "<- CApListItem::NewLC" ) ) );
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::ConstructL" ) ) );

    ConstructL();
    SetItemL( aIspType, aUid, aName, aBearerType, aStartPage );

    CLOG( ( EUtil, 1, _L( "<- CApListItem::ConstructL" ) ) );
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::SetItemL(4)" ) ) );

    SetItemL( aIspType, aUid, aName, aBearerType, KEmpty );

    CLOG( ( EUtil, 1, _L( "<- CApListItem::SetItemL(4)" ) ) );
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
    CLOG( ( EUtil, 0, _L( "-> CApListItem::SetItemL" ) ) );

    HBufC* temp = NULL;
    temp = aName.AllocL();
    delete iName;
    iName = temp;

    HBufC* temp2 = NULL;
    temp2 = aStartpage.AllocL();
    delete iStartPage;
    iStartPage = temp2;

    iIspType = aIspType;
    iUid = aUid;
    iBearerType = aBearerType;
    
    CLOG( ( EUtil, 1, _L( "<- CApListItem::SetItemL" ) ) );
    }


// ---------------------------------------------------------
// CApListItem::Name
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApListItem::Name()
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::Name" ) ) );

    return *iName;
    }


// ---------------------------------------------------------
// CApListItem::Uid
// ---------------------------------------------------------
//
EXPORT_C TUint32 CApListItem::Uid()
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::Uid" ) ) );

    return iUid;
    }


// ---------------------------------------------------------
// CApListItem::Type
// ---------------------------------------------------------
//
EXPORT_C TCommsDbIspType CApListItem::Type()
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::Type" ) ) );

    return iIspType;
    }



// ---------------------------------------------------------
// CApListItem::BearerType
// ---------------------------------------------------------
//
EXPORT_C TApBearerType CApListItem::BearerType()
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::BearerType" ) ) );

    return iBearerType;
    }


// ---------------------------------------------------------
// CApListItem::StartPage
// ---------------------------------------------------------
//
EXPORT_C const TDesC& CApListItem::StartPage() const
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::StartPage" ) ) );

    return *iStartPage;
    }


// ---------------------------------------------------------
// CApListItem::IsReadOnly
// ---------------------------------------------------------
//
EXPORT_C TBool CApListItem::IsReadOnly() const
    {
    CLOG( ( EUtil, 0, _L( "<-> CApListItem::IsReadOnly" ) ) );

    return iExt->iReadOnly;
    }


// ---------------------------------------------------------
// CApListItem::IsVpnAp
// ---------------------------------------------------------
//
TBool CApListItem::IsVpnAp() const
    {
    return iExt->iIsVpn;
    }


// ---------------------------------------------------------
// CApListItem::CreateExtraL
// ---------------------------------------------------------
//
void CApListItem::CreateExtraL()
    {
    iExt = new ( ELeave )TApListItemExtra;
    iExt->iIapId = 0;
    iExt->iIsVpn = EFalse;
    iExt->iReadOnly = EFalse;
    }


// ---------------------------------------------------------
// CApListItem::SetVpnFlag
// ---------------------------------------------------------
//
void CApListItem::SetVpnFlag( TBool aIsVpn )
    {
    iExt->iIsVpn = aIsVpn;
    }

//  End of File
