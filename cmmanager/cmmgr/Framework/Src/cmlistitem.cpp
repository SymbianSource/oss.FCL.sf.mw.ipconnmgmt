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
* Description:  Implementation of CCmListItem
*
*/

#include <eikdef.h>
#include "cmlistitem.h"

// ---------------------------------------------------------
// CCmListItem::NewLC
// ---------------------------------------------------------
//
CCmListItem* CCmListItem::NewLC( TUint32 aUid,
                                 HBufC* aName,
                                 TInt aPriority, 
                                 TUint aIconIndex, 
                                 TBool aIsProtected, 
                                 TBool aIsVpnOverDestination,
                                 TBool aIsSingleLine,
                                 TBool aIsDefault )
    {

    CCmListItem* db = new( ELeave ) CCmListItem( aName );
    CleanupStack::PushL( db );
    db->ConstructL( aUid, 
                    aPriority, 
                    aIconIndex, 
                    aIsProtected,
                    aIsVpnOverDestination, 
                    aIsSingleLine,
                    aIsDefault );
    return db;
    }
// ---------------------------------------------------------
// CCmListItem::~CCmListItem
// ---------------------------------------------------------
//
CCmListItem::~CCmListItem()
    {
    delete iName;
    }

// ---------------------------------------------------------
// CCmListItem::CCmListItem
// ---------------------------------------------------------
//
CCmListItem::CCmListItem( HBufC* aName )
    : iName( aName )
    {
    }

// ---------------------------------------------------------
// CCmListItem::ConstructL
// ---------------------------------------------------------
//
void CCmListItem::ConstructL( TUint32 aUid,
                              TInt aPriority, 
                              TUint aIconIndex, 
                              TBool aIsProtected, 
                              TBool aIsVpnOverDestination,
                              TBool aIsSingleLine,
                              TBool aIsDefault )
    {
    iPriority = aPriority;
    iIconIndex = aIconIndex;
    iIsProtected =  aIsProtected;
    iIsSingleLine =  aIsSingleLine;
    iIsDefault =  aIsDefault;
    iIsVpnOverDestination = aIsVpnOverDestination;
        
    iUid = aUid;
    }
// ---------------------------------------------------------
// CCmListItem::Name
// ---------------------------------------------------------
//
const TDesC& CCmListItem::Name()
    {
    return *iName;
    }


// ---------------------------------------------------------
// CCmListItem::Uid
// ---------------------------------------------------------
//
TUint32 CCmListItem::Uid()
    {
    return iUid;
    }
    
// ---------------------------------------------------------
// CCmListItem::IconIndex
// ---------------------------------------------------------
//
TUint CCmListItem::IconIndex()
    {
    return iIconIndex;
    }

// ---------------------------------------------------------
// CCmListItem::Priority
// ---------------------------------------------------------
//
TInt CCmListItem::Priority()
    {
    return iPriority;
    }

// ---------------------------------------------------------
// CCmListItem::IsProtected
// ---------------------------------------------------------
//
TBool CCmListItem::IsProtected()
    {
    return iIsProtected;
    }
    
// ---------------------------------------------------------
// CCmListItem::IsSingleLine
// ---------------------------------------------------------
//
TBool CCmListItem::IsSingleLine()
    {
    return iIsSingleLine;
    }

// ---------------------------------------------------------
// CCmListItem::IsDefault
// ---------------------------------------------------------
//    
TBool CCmListItem::IsDefault()
    {
    return iIsDefault;
    }

// ---------------------------------------------------------
// CCmListItem::IsVpnOverDestination
// ---------------------------------------------------------
//    
TBool CCmListItem::IsVpnOverDestination()
    {
    return iIsVpnOverDestination;
    }

    
