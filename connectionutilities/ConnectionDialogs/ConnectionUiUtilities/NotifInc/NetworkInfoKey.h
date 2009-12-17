/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of class CNetworkInfoKey
*
*/


#ifndef __CNETWORKINFOKEY_H__
#define __CNETWORKINFOKEY_H__


// INCLUDES
#include <e32base.h>


// FORWARD DECLARATION
class CNetworkInfoArray;


// CLASS DECLARATION
/**
 * Network information key class
 */
NONSHARABLE_CLASS( CNetworkInfoKey ) : public TKeyArrayFix
    {
    public:

        /**
        * SetPtr function
        * @param aPtr   Pointer used to initialize the class pointer
        */
        void SetPtr( CNetworkInfoArray* aPtr );

        /**
        * Compare function
        * @param aLeft  Left element to be compared
        * @param aRight Right element to be compared
        * @return TInt  Result of comparison of the elements
        */
        virtual TInt Compare( TInt aLeft, TInt aRight ) const;

        /**
        * TNetworkInfoKey constructor
        * @return -
        */
        CNetworkInfoKey();

        /**
        * ~CNetworkInfoKey() destructor
        * @param  -
        */
        ~CNetworkInfoKey();        
    };


#endif      //__CNETWORKINFOKEY_H__

// End of File
