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
* Description:  Implementation of class CConnectionInfoKey.
*
*/


// INCLUDE FILES
#include "ConnectionInfoKey.h"
#include "ConnectionInfo.h"


// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// CConnectionInfoKey::CConnectionInfoKey()
// ---------------------------------------------------------
//
CConnectionInfoKey::CConnectionInfoKey( TBool aIsWLANFeatureSupported )
: TKeyArrayFix( 0, ECmpCollated ),
  iPtr( NULL ),
  iIsWLANFeatureSupported( aIsWLANFeatureSupported ),
  iNameEasyWLAN( NULL )
    {
    }


// ---------------------------------------------------------
// CConnectionInfoKey::~CConnectionInfoKey()
// ---------------------------------------------------------
//
CConnectionInfoKey::~CConnectionInfoKey()
    {
    if ( iIsWLANFeatureSupported )
        {
        delete iNameEasyWLAN;
        }
    }


// ---------------------------------------------------------
// CConnectionInfoKey::NewL()
// ---------------------------------------------------------
//
CConnectionInfoKey* CConnectionInfoKey::NewL( TDesC &aNameEasyWLAN )
    {
    CConnectionInfoKey* self = new (ELeave) CConnectionInfoKey( ETrue );
    CleanupStack::PushL( self );
    self->ConstructL( aNameEasyWLAN );
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CConnectionInfoKey::ConstructL()
// ---------------------------------------------------------
//
void CConnectionInfoKey::ConstructL( TDesC &aNameEasyWLAN )
    {
    iNameEasyWLAN = aNameEasyWLAN.AllocL();
    }


// ---------------------------------------------------------
// CConnectionInfoKey::NewL()
// ---------------------------------------------------------
//
CConnectionInfoKey* CConnectionInfoKey::NewL()
    {
    CConnectionInfoKey* self = new (ELeave) CConnectionInfoKey( EFalse );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CConnectionInfoKey::ConstructL()
// ---------------------------------------------------------
//
void CConnectionInfoKey::ConstructL()
    {
    }



// ---------------------------------------------------------
// CConnectionInfoKey::SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr )
// ---------------------------------------------------------
//
void CConnectionInfoKey::SetPtr( CArrayPtrFlat<CConnectionInfo>* aPtr )
    {
    iPtr = aPtr;
    }


// ---------------------------------------------------------
// CConnectionInfoKey::Compare( TInt aLeft, TInt aRight ) const
// ---------------------------------------------------------
//
TInt CConnectionInfoKey::Compare( TInt aLeft, TInt aRight ) const
    {
    TInt retval;

    if ( iIsWLANFeatureSupported &&
         ! iPtr->At( aLeft )->Name().CompareC( *iNameEasyWLAN ) )
        {
        retval = -1;
        }
    else if ( iIsWLANFeatureSupported &&
              ! iPtr->At( aRight )->Name().CompareC( *iNameEasyWLAN ) )
        {
        retval = 1;
        }
    else
        {
        retval = iPtr->At( aLeft )->Name().CompareC( 
                                                iPtr->At( aRight )->Name() );
        }

    return retval;
    }

// End of File
