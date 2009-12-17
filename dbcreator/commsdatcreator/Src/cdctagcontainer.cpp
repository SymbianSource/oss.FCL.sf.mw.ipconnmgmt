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
* Description:  Method definitions of the class CTagContainer.
*
*/



#include "cdctagcontainer.h"
#include "cdcerrors.h"

//CONSTANTS
const TInt KGranularity = 20;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor of ETag
// ---------------------------------------------------------
CTagContainer::ETag::ETag( HBufC16* aTag, TInt aFieldId ):
    iFieldId( aFieldId ),
    iTag( aTag )
    {
    //no implementation needed    
    }

// ---------------------------------------------------------
// Destructor of ETag
// ---------------------------------------------------------
CTagContainer::ETag::~ETag() 
    {
    delete iTag;    
    }
    
// ---------------------------------------------------------
// TTagContainer NewL
// ---------------------------------------------------------
//    
CTagContainer* CTagContainer::NewL()
    {
    CTagContainer* self = new ( ELeave ) CTagContainer();
    CleanupStack::PushL( self );

    // From Base class
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }
    
// ---------------------------------------------------------
// TTagContainer ConstructL
// ---------------------------------------------------------
//        
void CTagContainer::ConstructL()
    {
    iTags = new (ELeave) CArrayPtrSeg<ETag>( KGranularity );    
    }
    
// ---------------------------------------------------------
// TTagContainer Constructor
// ---------------------------------------------------------
//
CTagContainer::CTagContainer()
    {
    }
    
// ---------------------------------------------------------
// TTagContainer Destructor
// ---------------------------------------------------------
//
CTagContainer::~CTagContainer()
    {
    Clear();
    delete iTags;
    }
        
// ---------------------------------------------------------
// CTagContainer::Clear
// ---------------------------------------------------------
//
void CTagContainer::Clear()
    {
    iTags->ResetAndDestroy();
    }
    
// ---------------------------------------------------------
// CTagContainer::Add
// ---------------------------------------------------------
//    
void CTagContainer::AddL( HBufC16* aTag, TInt aFieldId )
    {
    ETag* tag = new (ELeave) ETag( aTag, aFieldId );
    //lint -sem( iTags->AppendL, custodial(1) )
    
    CleanupStack::PushL( tag );
    iTags->AppendL( tag );
    CleanupStack::Pop( tag );
    }
    
// ---------------------------------------------------------
// CTagContainer::Tag
// ---------------------------------------------------------
//    
HBufC16* CTagContainer::Tag( TUint aIdx )
    {
    __ASSERT_ALWAYS( aIdx < Count(), 
                 User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );
                 
    return iTags->At(aIdx)->iTag;
    }

// ---------------------------------------------------------
// CTagContainer::FieldId
// ---------------------------------------------------------
//    
TInt CTagContainer::FieldId( TUint aIdx )
    {
    __ASSERT_ALWAYS( aIdx < Count(), 
                 User::Panic ( KPanicOutOfRange, KErrOutOfRange ) );

    return iTags->At( aIdx )->iFieldId;
    }
    
// ---------------------------------------------------------
// CTagContainer::Count
// ---------------------------------------------------------
//    
TInt CTagContainer::Count()
    {
    return iTags->Count();
    }

// End of File.
