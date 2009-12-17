/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Declares the connection info sort key
*
*/


#ifndef DDLG_CONNECTIONINFOSORTKEY_H
#define DDLG_CONNECTIONINFOSORTKEY_H

#include <e32base.h>

#include "ConnectionInfo.h"

NONSHARABLE_CLASS( TConnectionInfoSortKey ) : public TKeyArrayFix
    {
    public:

        /**
        * Constructor
        */
        TConnectionInfoSortKey();
        
        /**
        * SetPtr function
        * @param aPtr   Pointer used to initialize the class pointer
        */
        void SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr );

        /**
        * Compare function
        * @param aLeft  Left element to be compared
        * @param aRight Right element to be compared
        * @return TInt  Result of comparison of the elements
        */
        virtual TInt Compare( TInt aLeft, TInt aRight ) const;

        /**
        * At function
        * @param nIndex Index of the element ot be returned
        * @return TAny* The element at the given index
        */
        virtual TAny* At( TInt anIndex ) const;
                
    protected:
        // Pointer to class containing info over the connection 
        CArrayPtrFlat<CConnectionInfo> *iPtr;
    };
    
    
#endif //DDLG_CONNECTIONINFOSORTKEY_H

// End Of File