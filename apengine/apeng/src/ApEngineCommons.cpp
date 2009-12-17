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
    _LIT( kApSet, "APEngine" ) ;
    User::Panic( kApSet, aPanic ) ;
    }


// ---------------------------------------------------------
// ApCommons::StartPushedTransactionL
// ---------------------------------------------------------
//
TBool ApCommons::StartPushedTransactionLC( CCommsDatabase& aDb, TBool aRetry )
    {
    CLOG( ( ETransaction, 0, 
            _L( "-> ApCommons::StartPushedTransactionLC" ) ) );

    TBool retval( EFalse );
    if ( !aDb.InTransaction() )
        {
        TInt retriesLeft( aRetry ? KRetryCount : 1 );
        TInt err = aDb.BeginTransaction();
        while ( ( ( err == KErrLocked ) || ( err == KErrAccessDenied ) )
                && --retriesLeft )
            {
            User::After( KRetryWait );
            err = aDb.BeginTransaction();
            }
        User::LeaveIfError( err );
        CleanupStack::PushL
                (
                TCleanupItem( ApCommons::RollbackTransactionOnLeave, &aDb )
                );
        retval = ETrue;
        }
    else
        {
        retval = EFalse;
        }
    CLOG( ( ETransaction, 1, 
            _L( "<- ApCommons::StartPushedTransactionLC" ) ) );
    return retval;
    }



// ---------------------------------------------------------
// ApCommons::RollbackTransactionOnLeave
// ---------------------------------------------------------
//
void ApCommons::RollbackTransactionOnLeave( TAny* aDb )
    {
    CLOG( ( ETransaction, 0, 
            _L( "-> ApCommons::RollbackTransactionOnLeave" ) ) );
    
    CCommsDatabase* db = STATIC_CAST( CCommsDatabase*, aDb );
    db->RollbackTransaction();

    CLOG( ( ETransaction, 1, 
            _L( "<- ApCommons::RollbackTransactionOnLeave" ) ) );
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
    CLOG( ( ECommons, 0, _L( "-> ApCommons::DoMakeValidNameL" ) ) );

    aChanged = EFalse;
    HBufC* temp = HBufC::NewLC( KApMaxConnNameLength );
    HBufC* temp2 = HBufC::NewLC( KApMaxConnNameLength );

    HBufC* corrname = ApCommons::EnsureMaxLengthLC( aName, aChanged );
    *temp = *corrname;
    TInt postfix( 0 );
    TInt pf( 0 );
    TInt i( 0 );
    TBool valid ( EFalse );

    TPtrC prefix = ApCommons::GetPrefix( *corrname );
    
    CApUtils* utils = CApUtils::NewLC( aDb );

    if ( aIsNew )
        {
        aUid = 0;
        }

    postfix = GetPostfix( *temp, prefix );
    postfix = -1;
    do
        {       
        valid = utils->IsValidNameL( *temp, aUid );
        if ( !valid )
            {
            aChanged = ETrue;
            postfix++;
            // check the length of postfix, check text length accordingly
            pf = postfix;
            for (i=1; i<10; i++)
                {
                pf /= 10;
                if ( !pf )
                    break;
                }
            TPtr sgdptr( temp->Des() );
            TPtr sgdptr2( temp2->Des() );
            if ( postfix )
                {
                if ( postfix < 10 )
                    {
                    sgdptr2 = prefix.Left( KApMaxConnNameLength - i - 3 );
                    }
                else
                    {
                    sgdptr2 = prefix.Left( KApMaxConnNameLength - i - 2 );
                    }
                }
            else
                {
                sgdptr2 = prefix.Left( KApMaxConnNameLength );
                }
            if ( postfix )
                {
                if ( postfix > 9 )
                    {
                    sgdptr.Format( KFormatLargePostfix, &sgdptr2,
                                       postfix );
                    }
                else
                    {
                    sgdptr.Format( KFormatPostfix, &sgdptr2,
                                       postfix );
                    }
                }
            else
                {
                sgdptr.Format( KFormatNoPostfix, &sgdptr2 );
                }
            }
        } while ( !valid );

    CleanupStack::PopAndDestroy( utils );
    CleanupStack::PopAndDestroy( corrname );
    CleanupStack::PopAndDestroy( temp2 );

    if ( aChanged )
        {
        CleanupStack::Pop( temp );
        }
    else
        {
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }

    CLOG( ( ECommons, 1, _L( "<- ApCommons::DoMakeValidNameL" ) ) );

    return temp;
    }





// ---------------------------------------------------------
// ApCommons::EnsureMaxLengthLC
// ---------------------------------------------------------
//
HBufC* ApCommons::EnsureMaxLengthLC( const TDesC* aName, TBool& aChanged )
    {
    CLOG( ( ECommons, 0, _L( "-> ApCommons::EnsureMaxLengthLC" ) ) );

    TInt length = aName->Length();
    
    aChanged = EFalse;
    
    if ( !length )
        {
        // Name is required.
        User::Leave(KErrInvalidName);
        }

    HBufC* corrname;
    if ( KApMaxConnNameLength < length )
        { // name too long, truncate.
        corrname = aName->Left( KApMaxConnNameLength ).AllocLC();
        aChanged = ETrue;
        }
    else
        {
        corrname = aName->AllocLC();
        corrname->Des().Trim();
        if ( corrname->Length() == 0 )
            {
            User::Leave(KErrInvalidName);
            }
        // comes here only if name is valid
        if ( corrname->Length() != aName->Length() )
            {
            aChanged = ETrue;
            }
        }
    CLOG( ( ECommons, 1, _L( "<- ApCommons::EnsureMaxLengthLC" ) ) );

    return corrname;
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
    TPtrC prefix = aName;
    TInt lastBrace = aName.LocateReverse('(');
    if ( lastBrace != KErrNotFound )
        {
        // aName looks like "<prefix><brace><something>".
        // See if <something> is an integer number.
        TPtrC num = aName.Right( aName.Length() - lastBrace - 1 );
        TInt val;
        TLex lex( num );
        if ( lex.Val( val ) == KErrNone )
            {
            // Yes, the trailer is an integer.
            prefix.Set( aName.Left( lastBrace ) );
            }
        }
    return prefix;
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
    TInt postfix( KErrNotFound );
    TInt nameLength = aName.Length();
    TInt prefixLength = aPrefix.Length();
    if ( nameLength >= prefixLength && aName.FindF( aPrefix ) == 0 )
        {
        // aName is longer or equal length, and
        // aPrefix can be found in the beginning of aName.
        if ( nameLength == prefixLength )
            {
            // They have the same length; they equal.
            postfix = 0;
            }
        else
            {
            if ( aName[ prefixLength ] == '(' )
                {
                // (Now we know that aName is longer than aPrefix.)
                // aName looks like "aPrefix<brace><something>".
                // See if <something> is an integer number.
                TPtrC num = aName.Right( nameLength - prefixLength - 1 );
                TInt val;
                TLex lex( num );
                if ( lex.Val( val ) == KErrNone )
                    {
                    // Yes, the trailer is an integer.
                    if ( val > 0 )
                        {
                        postfix = val;
                        }
                    else
                        {
                        // signal that it is invalid...
                        postfix = -1;
                        }
                    }
                }
            }
        }
    return postfix;
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
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    TRAPD( err, atable->ReadUintL( aColumn, aValue ) );
    if ( err != KErrNone ) 
        { // do not leave if readed value is not present in table...
        if ( ( err != KErrUnknown ) && ( err != KErrNotFound ) )
            User::Leave( err );
        }
    return err;
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
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    TRAPD( err, atable->ReadBoolL( aColumn, aValue ) );
    if ( err != KErrNone )
        { // do not leave if readed value is not present in table...
        if ( err != KErrUnknown )
            User::Leave( err );
        }
    return err;
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
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    HBufC* value =  NULL;
    // because of Trap, we need to call a func
    // which pops it from the stack & push it back afterwards...
    TRAP( aErrCode, value = ReadLongTextL( atable, aColumn ) );
    CleanupStack::PushL( value );
    if ( aErrCode != KErrNone )
        { // do not leave if readed value is not present in table...
        if ( aErrCode != KErrUnknown )
            User::Leave( aErrCode );
        }
    return value;
    }



// ---------------------------------------------------------
// ApCommons::ReadLongTextL
// ---------------------------------------------------------
//
HBufC* ApCommons::ReadLongTextL( CCommsDbTableView* atable,
                                  const TDesC& aColumn )
    {
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    HBufC* value = atable->ReadLongTextLC( aColumn );
    CleanupStack::Pop( value );
    return value;
    }



// ---------------------------------------------------------
// ApCommons::EscapeTextLC
// ---------------------------------------------------------
//
HBufC* ApCommons::EscapeTextLC( const TDesC& aLiteral )
    {
    TInt l( aLiteral.Length() );
    //> 2*, to ensure correct esc. even if ALL chars are quotes...
    HBufC* retbuf = HBufC::NewLC( 2*l );
    TPtr ret = retbuf->Des();
    TUint quote( '\'' );  // TChar gives warnings in THUMB & ARMI
    TInt i( 0 );

    for( i=0; i<l; i++ )
        {
        ret.Append( aLiteral[i] );
        if ( aLiteral[i] == quote )
            {
            // Duplicate quote.
            ret.Append( quote );
            }
        }
    return retbuf;
    }



// ---------------------------------------------------------
// ApCommons::ReadText16ValueL
// ---------------------------------------------------------
//
HBufC16* ApCommons::ReadText16ValueLC( CCommsDbTableView* atable,
                                 const TDesC& aCol )
    {
    // Second variant, if always keeping mem usage to minimum.
    // This case code is a bit slower and more 'complicated'.
    // First version simpler, faster, less chance for errors
    // (e.g. if memory is exhausted, can not even create object
    // that may not be able to hold the values.
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;

    TInt aLength(0);
    atable->ReadColumnLengthL(aCol, aLength);

    HBufC* buf = HBufC::NewLC(aLength);
    TPtr ptr( buf->Des() );
    atable->ReadTextL(aCol, ptr);
    return buf;
    }


// ---------------------------------------------------------
// ApCommons::ReadText8ValueLC
// ---------------------------------------------------------
//
HBufC8* ApCommons::ReadText8ValueLC( CCommsDbTableView* atable,
                                 const TDesC& aCol )
    {
    __ASSERT_DEBUG( (atable), Panic( ENullPointer ) ) ;
    TInt aLength(0);
    atable->ReadColumnLengthL(aCol, aLength);

    HBufC8* buf = HBufC8::NewLC(aLength);
    TPtr8 ptr( buf->Des() );
    atable->ReadTextL(aCol, ptr);
    return buf;
    }


// ---------------------------------------------------------
// ApCommons::DoGetReadOnlyL
// ---------------------------------------------------------
//
TBool ApCommons::DoGetReadOnlyL( CCommsDbTableView& aTable )
    {
    return CApProtHandler::GetProtectionStateL( aTable );
    }



// ---------------------------------------------------------
// ApCommons::CommitTransaction
// ---------------------------------------------------------
//
TInt ApCommons::CommitTransaction( CCommsDatabase& aDb )
    {
    CLOG( ( ETransaction, 0, _L( "-> ApCommons::CommitTransaction" ) ) );
    TInt err = aDb.CommitTransaction();
    CLOG( ( ETransaction, 1, _L( "<- ApCommons::CommitTransaction" ) ) );
    return err;
    }




//----------------------------------------------------------
// ApCommons::GetVariantL
//----------------------------------------------------------
//
TInt ApCommons::GetVariantL()
    {
    CLOG( ( ETransaction, 0, _L( "-> ApCommons::GetVariantL" ) ) );
    TInt variant( 0 );
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL( KCRUidApEngineLV );
    repository->Get( KApEngineLVFlags, variant );
    delete repository;

    CLOG( ( ETransaction, 1, _L( "<- ApCommons::GetVariantL" ) ) );

    return variant;
    }



// ---------------------------------------------------------
// ApCommons::IsGivenSharedDataSupportL
// ---------------------------------------------------------
//
TInt ApCommons::IsGivenSharedDataSupportL( const TUid aUid, 
                                           const TUint32 aKey )
    {
    TInt value( 0 );
    // Connecting and initialization:
    CRepository* repository = CRepository::NewL( aUid );
    repository->Get( aKey, value );
    delete repository;
    return value;    
    }


// End of File
