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
* Description:  Contains and handles CConnectionInfo instances
*     
*
*/


#ifndef CONNECTIONARRAY_H_INCLUDED
#define CONNECTIONARRAY_H_INCLUDED

// INCLUDES
#include    "ConnectionInfoBase.h"


// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* Contains and handles CConnectionInfo instances.
*/
class CConnectionArray : public CBase, public MDesCArray
    {
    public:
        /**
        * Constructor.
        */
        CConnectionArray();

        /**
        * Destructor.
        */
        virtual ~CConnectionArray();

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
        * Give back a copy of connectionarray. 
        */
        virtual CConnectionArray* DeepCopyL();
    
        /**
        * Returns the index of the given connection
        * @param aConnectionId connection id
        * @return index value of connection in the array, or -1 if there
        * is no connection the given Id
        */
        TInt GetArrayIndex( TUint aConnectionId ) const;

        /**
        * Returns the index of the given connection
        * @param aConnection connection 
        * @return index value of connection in the array, or -1 if there
        * is no the given connection 
        */
        TInt GetArrayIndex( CConnectionInfoBase* aConnection ) const;

        /**
        * Add only new connection to the array
        * @param aConnection the connection to be added to the array
        */
        void AppendL( CConnectionInfoBase* aConnection );

        /**
        * Delete connection from the array
        * @param aConnectionId id of connection to be removed from the array
        */
        void Delete( TUint aConnectionId );

        /**
        * Delete connection from the array
        * @param aConnection the connection to be removed from the array
        */
        void Delete( CConnectionInfoBase* aConnection );

        /**
        * Destroys the connection array
        */
        void Reset();

        /**
        * Returns info object of connection given by index value in the array
        * @param aIndex index value of connection in the array
        * @return CConnectionInfo* pointer to the connection info object
        */
        CConnectionInfoBase* operator[]( TUint aIndex ) const;
        
        /**
        * Returns info object of connection given by index value in the array
        * Checks the boundaries of array.
        * @param aIndex index value of connection in the array
        * @return CConnectionInfoBase* pointer to the connection info object if
        * the given index is valid, NULL otherwise
        */
        CConnectionInfoBase* At( TInt aIndex ) const;
        
        /**
        * Gives back the number of active connections (created or suspended)
        * @return the number of active connections
        */
        TInt NumberOfActiveConnections() const;

        /**
        * Gives back the number of suspended connections
        * @return the number of suspended connections
        */
        TInt NumberOfSuspendedConnections() const;

    protected:
        /**
        * Contains the connections.
        */
        CArrayPtrFlat<CConnectionInfoBase>* iConnectionArray;   ///< Owned.
    };

#endif // CONNECTIONARRAY_H_INCLUDED