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
* Description:   Declaration of the class CTagContainer.
*
*/



#ifndef TAG_CONTAINER_H
#define TAG_CONTAINER_H

// INCLUDE FILES

#include <e32base.h>

// CLASS DECLARATION
/**
* CTagContainer
* contains and manages the fields read form XML or CSV input file
*/
class CTagContainer : public CBase
    {
    //TYPE DECLARATION
    private:
        
        /**
        * ETag
        * contains one tag and the belonging field id
        */
        class ETag
            {
            public:
            //constructor & destructor
            ETag( HBufC16* aTag, TInt aFieldId );
            ~ETag();
            
            //attributes
            TInt        iFieldId;  ///< field id tag belongs to
            HBufC16*    iTag;      ///< tag is the data segment read from file
                                   ///< created elsewhere but owned by ETag
                                   ///< and deallocated by ETag's destructor
            };
        
    public:
        
        /**
        * Two-phase constructor.
        */
        static CTagContainer* NewL();
         
        /**
        * Destructor.
        */
         ~CTagContainer();
        
    private:
        
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();
        
        /**
        * Constructor
        */
         CTagContainer();
         
    public:
        
        /**
        * Clears the array and deallocates all reserved memory
        */
         void Clear();
        
        /**
        * Adds a new tag to the array
        * @param aTag is a pointer to the tag
        */
         void AddL(HBufC16* aTag, TInt aFieldId);
        
        /**
        * Retrieves a tag identified by its index
        * @param aIdx is the index of the tag
        * @return HBufC16* pointer to the tag
        */
         HBufC16* Tag(TUint aIdx);
        
        /**
        * Retrieves the field ID
        * @param aIdx is the index of the tag
        * @return HBufC16* pointer to the tag
        */
         TInt FieldId(TUint aIdx);
        
        /**
        * Counts the number of tags
        * @return TInt is the number of tags
        */
         TInt Count();
        
    private:

        // @var Dynamic array for holding tags
        CArrayPtrSeg<ETag>* iTags; 
    
    };


#endif //TAG_CONTAINER_H