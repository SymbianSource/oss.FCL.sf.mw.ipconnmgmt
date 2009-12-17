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
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::NewL")
    
    CTextOverrides* db = new( ELeave ) CTextOverrides();
    CleanupStack::PushL( db );
    db->ConstructL( );
    CleanupStack::Pop(); // db
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::NewL")
    return db;
    }



// Destructor
CTextOverrides::~CTextOverrides()
    {
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::~CTextOverrides")
    
    if ( iTxtArray )
        {
        iTxtArray->ResetAndDestroy();
        }
    delete iTxtArray;
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::~CTextOverrides")
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
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::ConstructL")
    
    iTxtArray = new( ELeave ) CArrayPtrFlat<HBufC>( KTxtOverrideGran );
    for (TInt i=0; i<=KTTextIdMax; i++ )
        {
        iTxtArray->AppendL( NULL);
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::ConstructL")
    }


// ---------------------------------------------------------
// CTextOverrides::SetTextOverridesL
// ---------------------------------------------------------
//
void CTextOverrides::SetTextOverrideL( TTextID aText2Change,
                                                const TDesC& aNewText )
    {
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::SetTextOverrideL")
    
    ValidateIdL( aText2Change );

    HBufC* a = aNewText.AllocL();
    iTxtArray->Delete( aText2Change );
    iTxtArray->InsertL( aText2Change, a );

    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::SetTextOverrideL")
    }


// ---------------------------------------------------------
// CTextOverrides::TextOverridesL
// ---------------------------------------------------------
//
const TDesC& CTextOverrides::TextOverrideL( TTextID aTextId )
    {
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::TextOverrideL")
    
    ValidateIdL( aTextId );
    if ( !IsTextOverridenL( aTextId ) )
        {
        User::Leave( KTxtNotOverriden );
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::TextOverrideL")
    return *iTxtArray->At( aTextId );
    }


// ---------------------------------------------------------
// CTextOverrides::IsTextOverridenL
// ---------------------------------------------------------
//
TBool CTextOverrides::IsTextOverridenL( TTextID aTextId )
    {
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::IsTextOverridenL")
    
    TBool retval( EFalse );
    ValidateIdL( aTextId );
    if ( iTxtArray->At( aTextId ) )
        {
        retval = ETrue;
        }
    else
        {
        retval = EFalse;
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::IsTextOverridenL")
    return retval;
    }


// ---------------------------------------------------------
// CTextOverrides::ValidateIdL
// ---------------------------------------------------------
//
void CTextOverrides::ValidateIdL( TTextID aTextId )
    {
    APSETUILOGGER_ENTERFN( EOther,"TxtOverride::ValidateIdL")
    
    if ( ( aTextId < KTTextIdMin ) || ( aTextId > KTTextIdMax ) )
        {
        User::Leave( KErrInvalidTextId );
        }
    
    APSETUILOGGER_LEAVEFN( EOther,"TxtOverride::ValidateIdL")
    }


// End of File
