/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CDestListItem.
*
*/

#include <eikdef.h>
#include "destlistitem.h"
#include <cmmanagerdef.h>

using namespace CMManager;

// ---------------------------------------------------------------------------
// CDestListItem::NewLC
// ---------------------------------------------------------------------------
//
CDestListItem* CDestListItem::NewLC( TUint32 aUid,
                                     HBufC* aName,
                                     TInt aNumOfCMs, 
                                     TUint aIconIndex, 
                                     TProtectionLevel aProtectionLevel,
                                     TBool aIsDefault )
    {
    CDestListItem* db = new( ELeave ) CDestListItem( aName );
    CleanupStack::PushL( db );
    db->ConstructL( aUid, aNumOfCMs, aIconIndex , 
                    aProtectionLevel, aIsDefault );
    return db;
    }
    
// ---------------------------------------------------------------------------
// CDestListItem::~CDestListItem
// ---------------------------------------------------------------------------
//
CDestListItem::~CDestListItem()
    {
    delete iName;
    }

// ---------------------------------------------------------------------------
// CDestListItem::CDestListItem
// ---------------------------------------------------------------------------
//
CDestListItem::CDestListItem( HBufC* aName )
    : iName( aName )
    {
    }

// ---------------------------------------------------------------------------
// CDestListItem::ConstructL
// ---------------------------------------------------------------------------
//
void CDestListItem::ConstructL( TUint32 aUid,
                                TInt aNumOfCMs, 
                                TUint aIconIndex, 
                                TProtectionLevel aProtectionLevel,
                                TBool aIsDefault )
    {
    iProtectionLevel = aProtectionLevel;
    if (aProtectionLevel == EProtLevel0)
        {
        iIsProtected = EFalse;        
        }
    else
        {
        iIsProtected = ETrue;        
        }
    iIconIndex = aIconIndex;
    iNumOfCMs = aNumOfCMs;
    iIsDefault = aIsDefault;
    iUid = aUid;
    }
// ---------------------------------------------------------------------------
// CDestListItem::Name
// ---------------------------------------------------------------------------
//
const TDesC& CDestListItem::Name()
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CDestListItem::Uid
// ---------------------------------------------------------------------------
//
TUint32 CDestListItem::Uid()
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// CDestListItem::NumOfCMs
// ---------------------------------------------------------------------------
//
TInt CDestListItem::NumOfCMs()
    {
    return iNumOfCMs;
    }

// ---------------------------------------------------------------------------
// CDestListItem::IconIndex
// ---------------------------------------------------------------------------
//
TUint CDestListItem::IconIndex()
    {
    return iIconIndex;
    }

// ---------------------------------------------------------------------------
// CDestListItem::IsProtected
// ---------------------------------------------------------------------------
//
TUint CDestListItem::IsProtected()
    {
    return iIsProtected;
    }
    
// ---------------------------------------------------------------------------
// CDestListItem::IsProtected
// ---------------------------------------------------------------------------
//
TProtectionLevel CDestListItem::GetProtectionLevel()
    {
    return iProtectionLevel;
    }
    
// ---------------------------------------------------------------------------
// CDestListItem::IsDefault
// ---------------------------------------------------------------------------
//
TUint CDestListItem::IsDefault()
    {
    return iIsDefault;
    }

