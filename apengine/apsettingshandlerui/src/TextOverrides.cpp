/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Declares the CTextOverrides class.
*
*/


// INCLUDE FILES
#include "TextOverrides.h"
#include <ApSettingsHandlerCommons.h>
#include "ApSettingsHandlerLogger.h"

// CLASS DECLARATION

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTextOverrides::NewL
// ---------------------------------------------------------
//
CTextOverrides* CTextOverrides::NewL( )
    {
    return NULL;
    }



// Destructor
CTextOverrides::~CTextOverrides()
    {
    }


// Constructor
CTextOverrides::CTextOverrides()
    {
    }


// ---------------------------------------------------------
// CTextOverrides::ConstructL
// ---------------------------------------------------------
//
void CTextOverrides::ConstructL( )
    {
    }


// ---------------------------------------------------------
// CTextOverrides::SetTextOverridesL
// ---------------------------------------------------------
//
void CTextOverrides::SetTextOverrideL( TTextID aText2Change,
                                                const TDesC& aNewText )
    {
    }


// ---------------------------------------------------------
// CTextOverrides::TextOverridesL
// ---------------------------------------------------------
//
const TDesC& CTextOverrides::TextOverrideL( TTextID aTextId )
    {
    User::Leave( KErrNotSupported );
    }


// ---------------------------------------------------------
// CTextOverrides::IsTextOverridenL
// ---------------------------------------------------------
//
TBool CTextOverrides::IsTextOverridenL( TTextID aTextId )
    {
    return EFalse;
    }


// ---------------------------------------------------------
// CTextOverrides::ValidateIdL
// ---------------------------------------------------------
//
void CTextOverrides::ValidateIdL( TTextID aTextId )
    {
    }


// End of File
