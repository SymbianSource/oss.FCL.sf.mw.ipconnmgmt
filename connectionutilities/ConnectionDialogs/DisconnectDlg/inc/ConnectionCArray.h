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
*      Declares the connection array.
*
*/


#ifndef DDLG_CONNECTION_ARRAY_H
#define DDLG_CONNECTION_ARRAY_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CConnectionInfo;

// CLASS DECLARATION


/**
* Contains CConnectionInfo instances.
*/
NONSHARABLE_CLASS( CConnectionCArray ) : public CBase
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        static CConnectionCArray* NewL();

        /**
        * Destructor.
        */
        virtual ~CConnectionCArray();

    private: // Constructors

        /**
        * Constructor.
        */
        CConnectionCArray();

        /**
        * Constructor.
        */
        void ConstructL();

    public: // New functions

        /**
        * Returns the number of connection added to the connection array
        * @return number of active connections
        */
        TInt Count() const;
        
        /**
        * Add only new connection to the array
        * @param aConnId id of connection to be added to the array
        * @return -
        */
        void AppendL( const CConnectionInfo* aInfo );
        
        /**
        * Destroys the connection array
        */
        void ResetAndDestroy();

        /**
        * Returns info object of connection given by index value in the array
        * @param aIndex index value of connection in the array
        * @return CConnectionInfo* pointer to the connection info object
        */
        CConnectionInfo* At( TInt aIndex ) const;
        
        /**
        * Returns the index of the given connection
        * @param aConnectionId connection id
        * @return index value of connection in the array, or -1 if there
        * is no connection the given Id
        */
        TInt GetArrayIndex( TUint aConnectionId ) const;  
        
        /**
        * Sorts the array according to the connection start times
        * @param -
        * @return -
        */
        void Sort() const;      

    private: // data
        /**
        * Contains CConectionInfo instances
        */
        CArrayPtrFlat<CConnectionInfo>* iConnArray; 
    };
      
#endif // DDLG_CONNECTION_ARRAY_H

// End Of File