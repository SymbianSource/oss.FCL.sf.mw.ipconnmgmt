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
* Description:  Declaration of the TApListItemKey class.
*
*/


// INCLUDE FILES

#include "ApListItemList.h"
#include "ApListItem.h"
#include "ApListItemKey.h"

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// TApListItemNameKey::SetPtr()
// ---------------------------------------------------------
//
void TApListItemNameKey::SetPtr( CApListItemList* aPtr )
    {
    }


// ---------------------------------------------------------
// TApListItemNameKey::Compare()
// ---------------------------------------------------------
//
TInt TApListItemNameKey::Compare( TInt aLeft,TInt aRight ) const
    {
    return 0;
    }


// ---------------------------------------------------------
// TApListItemNameKey::At()
// ---------------------------------------------------------
//
TAny* TApListItemNameKey::At( TInt anIndex ) const
    {
    return NULL;
    }



// ---------------------------------------------------------
// TApListItemNameKey::TApListItemNameKey()
// ---------------------------------------------------------
//
TApListItemNameKey::TApListItemNameKey(  TBool aAscending )
:TKeyArrayFix( 0, ECmpCollated )
        {
        }






// ---------------------------------------------------------
// TApListItemUidKey 
// ---------------------------------------------------------


// ---------------------------------------------------------
// TApListItemUidKey::SetPtr
// ---------------------------------------------------------
//
void TApListItemUidKey::SetPtr( CApListItemList* aPtr )
    {
    }


// ---------------------------------------------------------
// TApListItemUidKey::Compare
// ---------------------------------------------------------
//
TInt TApListItemUidKey::Compare( TInt aLeft,TInt aRight ) const
    {
    return 0;
    }


// ---------------------------------------------------------
// TApListItemUidKey::At
// ---------------------------------------------------------
//
TAny* TApListItemUidKey::At( TInt anIndex ) const
    {
    return NULL;
    }



// ---------------------------------------------------------
// TApListItemUidKey::TApListItemUidKey
// ---------------------------------------------------------
//
TApListItemUidKey::TApListItemUidKey(  TBool aAscending )
:TKeyArrayFix( 0, ECmpCollated )
        {
        }


//  End of File
