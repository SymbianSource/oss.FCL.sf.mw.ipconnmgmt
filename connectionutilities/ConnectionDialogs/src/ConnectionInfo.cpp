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
* Description:  Implementation of class CConnectionInfo.
*
*/


// INCLUDE FILES
#include "ConnectionInfo.h"


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CConnectionInfo::CConnectionInfo
// ---------------------------------------------------------
//
CConnectionInfo::CConnectionInfo( TUint32 aId, TBool aIsVPN, 
                                  TUint32 aVPN_iap_id, TUint32 aDestinationId )
: iName( NULL ),
  iId( aId ),
  iListItem( NULL ),
  iIsVPN( aIsVPN ),
  iVPN_iap_id( aVPN_iap_id ),
  iDestinationId( aDestinationId )
    {
    }


// ---------------------------------------------------------
// CConnectionInfo::~CConnectionInfo
// ---------------------------------------------------------
//
CConnectionInfo::~CConnectionInfo()
    {
    delete iName;
    delete iListItem;
    }


// ---------------------------------------------------------
// CConnectionInfo::NewL( TDesC &aName, TUint32 aId, 
//                        TDesC &aListItem )
// ---------------------------------------------------------
//
CConnectionInfo* CConnectionInfo::NewL( TDesC &aName, TUint32 aId, 
                                        TDesC &aListItem )
    {
    CConnectionInfo* self = new (ELeave) CConnectionInfo( aId );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aListItem );
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CConnectionInfo::NewL()
// ---------------------------------------------------------
//
CConnectionInfo* CConnectionInfo::NewL( TDesC &aName, TUint32 aId, 
                                        TUint32 vpn_iap_id, TDesC &aListItem )
    {
    CConnectionInfo* self = new( ELeave )CConnectionInfo( aId, ETrue, 
                                                          vpn_iap_id );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aListItem );
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CConnectionInfo::NewL()
// ---------------------------------------------------------
//
CConnectionInfo* CConnectionInfo::NewL( TUint32 aId, TUint32 aDestId, 
                                        TDesC &aListItem )
    {
    CConnectionInfo* self = new( ELeave )CConnectionInfo( aId, EFalse, 0,
                                                          aDestId );
    CleanupStack::PushL( self );
    self->ConstructL( aListItem );
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CConnectionInfo::ConstructL()
// ---------------------------------------------------------
//
void CConnectionInfo::ConstructL( TDesC &aName, TDesC &aListItem )
    {
    iName = aName.AllocL();
    iListItem = aListItem.AllocL();
    }


// ---------------------------------------------------------
// CConnectionInfo::ConstructL()
// ---------------------------------------------------------
//
void CConnectionInfo::ConstructL( TDesC &aListItem )
    {
    iListItem = aListItem.AllocL();
    }


// End of File
