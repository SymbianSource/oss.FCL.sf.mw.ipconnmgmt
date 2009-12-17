/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Declares the TConnectionInfoSortKey
*
*/


#include "connectioninfosortkey.h"


// ---------------------------------------------------------
// TConnectionInfoSortKey::TConnectionInfoSortKey
// ---------------------------------------------------------
//
TConnectionInfoSortKey::TConnectionInfoSortKey() : 
    TKeyArrayFix( 0, ECmpCollated )
    {
    }

    
// ---------------------------------------------------------
// TConnectionInfoSortKey::SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr )
// ---------------------------------------------------------
//
void TConnectionInfoSortKey::SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr )
    {
    iPtr = aPtr;
    }


// ---------------------------------------------------------
// TConnectionInfoSortKey::Compare( TInt aLeft, TInt aRight ) const
// ---------------------------------------------------------
//
TInt TConnectionInfoSortKey::Compare( TInt aLeft, TInt aRight ) const
    {
    TInt retval;
    TTime left = iPtr->At( aLeft )->GetStartTime();
    TTime right = iPtr->At( aRight )->GetStartTime();

    if( left < right )
        {
        retval = -1;
        }
    else if( left > right )
        {
        retval = 1;
        }
    else 
        {
        retval = 0;
        } 
    
    return retval;
    }


// ---------------------------------------------------------
// TConnectionInfoSortKey::At( TInt anIndex ) const
// ---------------------------------------------------------
//
TAny* TConnectionInfoSortKey::At( TInt anIndex ) const
    {
    return iPtr->At( anIndex );
    }
