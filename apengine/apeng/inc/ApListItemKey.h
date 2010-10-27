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


#ifndef TAPLISTITEMKEY_H
#define TAPLISTITEMKEY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <ApEngineVer.h>

// CLASS DECLARATION
/**
*  A key for sorting CApListItems according to their name
*  The pointer to the list is not owned
*/
NONSHARABLE_CLASS( TApListItemNameKey ) : public TKeyArrayFix
    {
public:
    /**
    * Sets the list to sort
    * @param aPtr A pointer to the l;ist to sort
    */
    void SetPtr( CApListItemList* aPtr );
    
    // From TKeyArrayFix
    virtual TInt Compare( TInt aLeft,TInt aRight ) const;
    
    // From TKeyArrayFix
    virtual TAny* At( TInt anIndex ) const;
    
    /**
    * Constructor
    */
    TApListItemNameKey( TBool aAscending = ETrue );
    
protected:
    CApListItemList* iList;
    TBool            iAscending;
    };




/**
*  A key for sorting CApListItems according to their UID
*  The pointer to the list is not owned
*/
NONSHARABLE_CLASS( TApListItemUidKey ) : public TKeyArrayFix
    {
public:
    /**
    * Sets the list to sort
    * @param aPtr A pointer to the l;ist to sort
    */
    void SetPtr( CApListItemList* aPtr );
        
    // From TKeyArrayFix
    virtual TInt Compare( TInt aLeft,TInt aRight ) const;
    
    // From TKeyArrayFix
    virtual TAny* At( TInt anIndex ) const;
    
    /**
    * Constructor
    */
    TApListItemUidKey( TBool aAscending = ETrue );

    
protected:
    CApListItemList* iList;
    TBool            iAscending;
    };

#endif

// End of File
