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
* Description:  Declaration of class CConnectionInfoArray
*
*/


#ifndef __CCONNECTIONINFOARRAY_H__
#define __CCONNECTIONINFOARRAY_H__

// INCLUDES
#include <e32base.h>
#include <bamdesca.h>


// FORWARD DECLARATION
class CConnectionInfo;


// CLASS DECLARATION
/**
* CConnectionInfoArray class
*/
NONSHARABLE_CLASS( CConnectionInfoArray ) : 
                                        public CArrayPtrFlat<CConnectionInfo>,
                                        public MDesC16Array
{
    public:    
        /**
        * Default constructor
        */
        CConnectionInfoArray();        

        
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
};                             

#endif

// End of File
