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
    iList = aPtr;
    }


// ---------------------------------------------------------
// TApListItemNameKey::Compare()
// ---------------------------------------------------------
//
TInt TApListItemNameKey::Compare( TInt aLeft,TInt aRight ) const
    {
    TInt retval = 
        iList->At( aLeft )->Name().CompareC( iList->At( aRight )->Name() );
    if ( !iAscending )
        {
        retval *= -1;
        }
    return retval;
    }


// ---------------------------------------------------------
// TApListItemNameKey::At()
// ---------------------------------------------------------
//
TAny* TApListItemNameKey::At( TInt anIndex ) const
    {
    TAny* ptr = MUTABLE_CAST( TDesC16*, &(iList->At( anIndex )->Name()));
    return ptr;
    }



// ---------------------------------------------------------
// TApListItemNameKey::TApListItemNameKey()
// ---------------------------------------------------------
//
TApListItemNameKey::TApListItemNameKey(  TBool aAscending )
:TKeyArrayFix( 0, ECmpCollated )
        {
        iList = NULL;
        iAscending = aAscending;
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
    iList = aPtr;
    }


// ---------------------------------------------------------
// TApListItemUidKey::Compare
// ---------------------------------------------------------
//
TInt TApListItemUidKey::Compare( TInt aLeft,TInt aRight ) const
    {
    TInt retval( 1 );
    if ( iList->At( aLeft )->Uid() < iList->At( aRight )->Uid() )
        {
        retval = -1;
        }
    else
        {
        if ( iList->At( aLeft )->Uid() == iList->At( aRight )->Uid() )
            {
            retval = 0;
            }
        }
    if ( !iAscending )
        {
        retval *= -1;
        }
    return retval;
    }


// ---------------------------------------------------------
// TApListItemUidKey::At
// ---------------------------------------------------------
//
TAny* TApListItemUidKey::At( TInt anIndex ) const
    {
    TAny* ptr = MUTABLE_CAST( TUint32*, &(iList->At( anIndex )->iUid) );
    return ptr;
    }



// ---------------------------------------------------------
// TApListItemUidKey::TApListItemUidKey
// ---------------------------------------------------------
//
TApListItemUidKey::TApListItemUidKey(  TBool aAscending )
:TKeyArrayFix( 0, ECmpCollated )
        {
        iList = NULL;
        iAscending = aAscending;
        }


//  End of File
