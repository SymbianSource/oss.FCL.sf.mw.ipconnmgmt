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
* Description: 
*     
*
*/


#ifndef NETWORKINFOARRAY_NOTIF_H_INCLUDED
#define NETWORKINFOARRAY_NOTIF_H_INCLUDED

// INCLUDES
#include    "NetworkInfoBase.h"


// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* Contains and handles CConnectionInfo instances.
*/
NONSHARABLE_CLASS( CNetworkInfoArray ) : public CBase, public MDesCArray
    {
    public:
        /**
        * Static constructor
        * @return A pointer to a CNetworkInfoArray instance.
        */
        static CNetworkInfoArray* NewL();

        /**
        * Constructor.
        */
        CNetworkInfoArray();

        /**
        * Destructor.
        */
        virtual ~CNetworkInfoArray();

        /**
        * Contructs dynamic data members
        */
        virtual void ConstructL();
   
    public: // from MDesCArray
        /** 
        * Returns the number of descriptor elements in a descriptor array.
        * @return The number of descriptor elements in a descriptor array. 
        */
        virtual TInt MdcaCount() const;

        /** 
        * Indexes into a descriptor array.    
        * @param aIndex The position of the descriptor element within a 
        * descriptor array. 
        * The position is relative to zero; i.e. zero implies the first 
        * descriptor element in a descriptor array. 
        * @return A  non-modifiable pointer descriptor representing 
        * the descriptor element located at position aIndex within a 
        * descriptor array. 
        */
        virtual TPtrC MdcaPoint( TInt aIndex ) const;

    public:
        /**
        * Returns the index of the given network
        * @param aNetworkName name of network 
        * @param aTo the index while searching    
        * @return index value of network in the array, or -1 if there
        * is no network the given Id
        */
        TInt GetArrayIndex( TName& aNetworkName ) const;

        /**
        * Add only new network to the array
        * @param aNetwork the network to be added to the array
        */
        void AppendL( CNetworkInfoBase* aNetwork );

        /**
        * Destroys the network array
        */
        void Reset();

        /**
        * Returns info object of network given by index value in the array
        * @param aIndex index value of network in the array
        * @return CNetworkInfoBase* pointer to the network info object
        */
        CNetworkInfoBase* operator[]( TUint aIndex ) const;
        
        /**
        * Returns info object of network given by index value in the array
        * Checks the boundaries of array.
        * @param aIndex index value of network in the array
        * @return CNetworkInfoBase* pointer to the network info object if
        * the given index is valid, NULL otherwise
        */
        CNetworkInfoBase* At( TInt aIndex ) const;
        
        
        /**
        * calls sort funcion of CArrayPtrFlat.
        */
        TInt Sort( TKeyArrayFix& aKey );
        
    protected:
        /**
        * Contains the connections.
        */
        CArrayPtrFlat<CNetworkInfoBase>* iNetworkInfoArray;   ///< Owned.
    };

#endif // NETWORKINFOARRAY_NOTIF_H_INCLUDED
