/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the bearer priority array cache.
*
*/


#include "cmmbearerpriority.h"


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmBearerPriority* CCmmBearerPriority::NewL(
        const TDesC& aServiceType,
        const TUint& aPriority,
        const TUint& aUiPriority )
    {
    CCmmBearerPriority* self = CCmmBearerPriority::NewLC( aServiceType, aPriority, aUiPriority );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmBearerPriority* CCmmBearerPriority::NewLC(
        const TDesC& aServiceType,
        const TUint& aPriority,
        const TUint& aUiPriority )
    {
    CCmmBearerPriority* self = new( ELeave ) CCmmBearerPriority( aPriority, aUiPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aServiceType );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor. Note: aServiceType is not copied here because constructor
// cannot leave.
// ---------------------------------------------------------------------------
//
CCmmBearerPriority::CCmmBearerPriority(
        const TUint& aPriority,
        const TUint& aUiPriority )
        :
        iPriority( aPriority ),
        iUiPriority( aUiPriority )
    {
    iServiceType = NULL;
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmBearerPriority::ConstructL( const TDesC& aServiceType )
    {
    iServiceType = aServiceType.AllocL();
    }

// ---------------------------------------------------------------------------
// Get the service type.
// ---------------------------------------------------------------------------
//
HBufC* CCmmBearerPriority::ServiceType() const
    {
    return iServiceType;
    }

// ---------------------------------------------------------------------------
// Get the priority.
// ---------------------------------------------------------------------------
//
TUint CCmmBearerPriority::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------------------------
// Get the UI priority.
// ---------------------------------------------------------------------------
//
TUint CCmmBearerPriority::UiPriority() const
    {
    return iUiPriority;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmBearerPriority::~CCmmBearerPriority()
    {
    delete iServiceType;
    }

// End of file
