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
* Description:  Declaration of some common global functions.
*
*/


// INCLUDE FILES
#include <eikdef.h>
#include <ApEngineConsts.h>
#include <commdb.h>
#include <ApUtils.h>
#include <centralrepository.h> // link against centralrepository.lib

#include "ApEngineCommons.h"
#include "ApProtHandler.h"
#include "ApEngineLogger.h"
#include "ApEnginePrivateCRKeys.h"    

// CONSTANTS


// LOCAL CONSTANTS AND MACROS
_LIT(KFormatPostfix, "%S(%02d)");
_LIT(KFormatNoPostfix, "%S");
_LIT(KFormatLargePostfix, "%S(%d)");

void ApCommons::Panic( TApEnginePanicCodes aPanic )
    {
    }


// ---------------------------------------------------------
// ApCommons::StartPushedTransactionL
// ---------------------------------------------------------
//
TBool ApCommons::StartPushedTransactionLC( CCommsDatabase& aDb, TBool aRetry )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// ApCommons::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void ApCommons::RollbackTransactionOnLeave( TAny* aDb )
    {
    }




// ---------------------------------------------------------
// ApCommons::DoMakeValidNameL
// ---------------------------------------------------------
//
HBufC* ApCommons::DoMakeValidNameL
                            (
                            CCommsDatabase& aDb,
                            TBool& aChanged,
                            HBufC* aName,
                            TUint32 aUid,
                            TBool aIsNew
                            )
    {
    return NULL;
    }





// ---------------------------------------------------------
// ApCommons::EnsureMaxLengthLC
// ---------------------------------------------------------
//
HBufC* ApCommons::EnsureMaxLengthLC( const TDesC* aName, TBool& aChanged )
    {
    return NULL;
    }




/**
* Given aName in the format <prefix> or <prefix><brace><integer><brace>,
* return a pointer to the leading part.
* That is, if there is trailing <space><integer>,
* then that is excluded; if there is no trailing part, then the original
* decriptor is returned.
* Examples:
*   - "Foo" returns "Foo";
*   - "Foo 12" returns "Foo 12";
*   - "Foo(12)" returns "Foo";
*   - "Foo 12 (34)" returns "Foo 12 ";
*   - "Foo bar" returns "Foo bar";
*   - "Foo " returns "Foo ".
*/
TPtrC ApCommons::GetPrefix( const TDesC& aName )
    {
    User::Leave( KErrNotSupported );
    }


/**
* If aName is constructed from aPrefix with a postfix, get the numeric
* value of the postfix, e.g:
*   - GetPostfix( "Foo (3)", "Foo" ) == 3
*   - GetPostfix( "Foo 23 (45)", "Foo 23" ) == 45
* If aName is the same as aPrefix, return 0, e.g.:
*   - GetPostfix( "Foo", "Foo" ) == 0
* If aName is not constructed from aPrefix, return -1, e.g.:
*   - GetPostfix( "Foobar", "Foo" ) == -1
*   - GetPostfix( "Fo 23 45", "Foo" ) == -1
*/
TInt ApCommons::GetPostfix( const TDesC& aName, const TDesC& aPrefix )
    {
    return 0;
    }




// ---------------------------------------------------------
// ApCommons::ReadUintL
// ---------------------------------------------------------
//
TInt ApCommons::ReadUintL
                    (
                    CCommsDbTableView* atable,
                    const TDesC& aColumn,
                    TUint32& aValue
                    )
    {
    return KErrUnknown;
    }


// ---------------------------------------------------------
// ApCommons::ReadBoolL
// ---------------------------------------------------------
//
TInt ApCommons::ReadBoolL
                    (
                    CCommsDbTableView* atable,
                    const TDesC& aColumn,
                    TBool& aValue
                    )
    {
    return KErrUnknown;
    }



// ---------------------------------------------------------
// ApCommons::ReadLongTextLC
// ---------------------------------------------------------
//
HBufC* ApCommons::ReadLongTextLC
                        (
                        CCommsDbTableView* atable,
                        const TDesC& aColumn,
                        TInt& aErrCode
                        )
    {
    return NULL;
    }



// ---------------------------------------------------------
// ApCommons::ReadLongTextL
// ---------------------------------------------------------
//
HBufC* ApCommons::ReadLongTextL( CCommsDbTableView* atable,
                                  const TDesC& aColumn )
    {
    return NULL;
    }



// ---------------------------------------------------------
// ApCommons::EscapeTextLC
// ---------------------------------------------------------
//
HBufC* ApCommons::EscapeTextLC( const TDesC& aLiteral )
    {
    return NULL;
    }



// ---------------------------------------------------------
// ApCommons::ReadText16ValueL
// ---------------------------------------------------------
//
HBufC16* ApCommons::ReadText16ValueLC( CCommsDbTableView* atable,
                                 const TDesC& aCol )
    {
    return NULL;
    }


// ---------------------------------------------------------
// ApCommons::ReadText8ValueLC
// ---------------------------------------------------------
//
HBufC8* ApCommons::ReadText8ValueLC( CCommsDbTableView* atable,
                                 const TDesC& aCol )
    {
    return NULL;
    }


// ---------------------------------------------------------
// ApCommons::DoGetReadOnlyL
// ---------------------------------------------------------
//
TBool ApCommons::DoGetReadOnlyL( CCommsDbTableView& aTable )
    {
    return EFalse;
    }



// ---------------------------------------------------------
// ApCommons::CommitTransaction
// ---------------------------------------------------------
//
TInt ApCommons::CommitTransaction( CCommsDatabase& aDb )
    {
    return KErrNotSupported;
    }




//----------------------------------------------------------
// ApCommons::GetVariantL
//----------------------------------------------------------
//
TInt ApCommons::GetVariantL()
    {
    return 0;
    }



// ---------------------------------------------------------
// ApCommons::IsGivenSharedDataSupportL
// ---------------------------------------------------------
//
TInt ApCommons::IsGivenSharedDataSupportL( const TUid aUid, 
                                           const TUint32 aKey )
    {
    return 0;    
    }


// End of File
