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


#ifndef APENGINE_COMMONS_H
#define APENGINE_COMMONS_H

//  INCLUDES
#include <commdb.h>
#include <ApEngineVer.h>


// CONSTANTS
_LIT(KSqlSelectBase, "SELECT * from %s WHERE %s='%S'");
_LIT( KEmpty, "" );
_LIT( KFormat, "%S" );
_LIT8( KFormat8,"%S" );


LOCAL_C const TInt KR_ApEngineDefaultValue    = 0;

// DATA TYPES

enum TApEnginePanicCodes
    {
    ENullPointer,
    EInvalidDbType,
    EWrongResourceFormat,
    EActiveApDbObserverNotFound,
    EInvalidColumn,
    EInvalidSpeed,
    ESqlError,
    ESanityCheckFailed,
    ENotSupported,
    EMenuCommandUnknown
    };


// CLASS DECLARATION
/**
* ApCommons is a collection of commonly used 'utility' functions.
*/
NONSHARABLE_CLASS( ApCommons )
    {
    public:
        /**
        * Panics.
        */
        static void Panic( TApEnginePanicCodes aPanic );

        /**
        * Starts a transaction on the given database if not already in one.
        * In case of error, leaves with CommDb error codes.
        * In case of KErrLocked, if retry is asked, it retries the operation
        * after a short ( KRetryWait ) period. If still locked, leaves with it
        * to allow caller module to handle the situation
        * (e.g. giving up a Retry/Cancel query).
        * In case of successfull transaction starting, pushes
        * RollbackTransactionOnLeave to the CleanUpStack.
        * @param aDb A reference to a CCommsDatabase object to start
        * the transaction.
        * @param aRetry A TBool indicating whether it shall retry
        * the operation or not.
        * @return Boolean indicating whether it has started a
        * transaction or not (it also means whether it has pushed it or not.)
        * It is used after DB operations to decide whether we have to
        * commit or it is the caller's task.
        */
        static TBool StartPushedTransactionLC( CCommsDatabase& aDb,
                                               TBool aRetry = ETrue );


        /**
        * Rolls back transaction on leave; called in leave processing.
        * @param aDb The database (CCommsDatabase*) as TAny*.
        */
        static void RollbackTransactionOnLeave( TAny* aDb );




        /**
        * Creates a valid name from the passed aName.
        * @param aDb A reference to a CCommsDatabase object 
        * @param aChanged A reference to a boolean to hold whether the name 
        *   had been changed or not.
        * @param aUid The Uid of the access point whose name we are checking
        *   to be able to handle updates and ignore self-match only situation
        * @param aIsNew a boolean specifying whether tha access point whose 
        *   name we are checking is new or not.
        * @return A valid name
        */
        static HBufC* DoMakeValidNameL( CCommsDatabase& aDb,
                                        TBool& aChanged,
                                        HBufC* aName,
                                        TUint32 aUid,
                                        TBool aIsNew );

        /**
        * Ensures that the passed name is valid, length > 0 & length < max.
        * If length == 0, leaves with KErrInvalidName
        * If name is only whitespace, leaves with KErrInvalidName
        * If name is longer than max, name is truncated
        * It copies the name, pushez it onto the CleanupStack and passes
        * ownership.
        * @param aName The name
        * @param aChanged A reference to a boolean to hold whether the name 
        *   had been changed or not.        
        * @return The valid-length name
        */
        static HBufC* EnsureMaxLengthLC( const TDesC* aName, TBool& aChanged );






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
        * @param aName  The name to get the postfix from
        * @param aPrefix The prefix
        * @return The postfix
        */
        static TInt GetPostfix( const TDesC& aName, const TDesC& aPrefix );


        /**
        * Given aName in the format <prefix> or
        * <prefix><brace><integer><brace>, return a
        * pointer to the leading part. That is, if there is
        * trailing <space><integer>, then that is excluded;
        * if there is no trailing part, then the original
        * decriptor is returned.
        * Examples:
        *   - "Foo" returns "Foo";
        *   - "Foo 12" returns "Foo 12";
        *   - "Foo(12)" returns "Foo";
        *   - "Foo 12 (34)" returns "Foo 12 ";
        *   - "Foo bar" returns "Foo bar";
        *   - "Foo " returns "Foo ".
        * @param aName  The name to get the prefix from
        * @return The prefix
        */
        static TPtrC GetPrefix( const TDesC& aName );


        /**
        * Reads up a TUint32 from the given tables given column.
        * Leaves in case of error.
        * Leave with KErrUnknown is captured and returned as
        * function return value.
        * The main aim is not to Leave if some value is not written to the DB.
        * @param atable The table to read from
        * @param aColumn The column to read
        * @param aValue A reference to hold the readed integer
        * @return Error code
        */
        static TInt ReadUintL
                        (
                        CCommsDbTableView* atable,
                        const TDesC& aColumn,
                        TUint32& aValue
                        );

        /**
        * Reads up a TBool from the given tables given column.
        * Leaves in case of error.
        * Leave with KErrUnknown is captured and returned as
        * function return value.
        * The main aim is not to Leave if some value is not written to the DB.
        * @param atable The table to read from
        * @param aColumn The column to read
        * @param aValue A reference to hold the readed boolean
        * @return Error code
        */
        static TInt ReadBoolL
                        (
                        CCommsDbTableView* atable,
                        const TDesC& aColumn,
                        TBool& aValue
                        );


        /**
        * Reads up a long text from the given tables given column.
        * Leaves in case of error.
        * Leave with KErrUnknown is captured and returned as
        * function return value.
        * Ownership of the returned text is passed to the caller.
        * The main aim is not to Leave if some value is not written to the DB.
        * @param atable The table to read from
        * @param aColumn The column to read
        * @param aValue A reference to hold the error code
        * @return The readed long text
        */
        static HBufC* ReadLongTextLC
                        (
                        CCommsDbTableView* atable,
                        const TDesC& aColumn,
                        TInt& aErrCode
                        );
                        

        /**
        * Reads up a long text from the given tables given column.
        * Leaves in case of error.
        * Ownership of the returned text is passed to the caller.
        * Readed data is poped from the CleanuptStack to be able
        * to TRAP it in ReadLongTextLC (above) function...
        * @param atable The table to read from
        * @param aColumn The column to read
        * @return The readed long text
        */
        static HBufC* ReadLongTextL( CCommsDbTableView* atable,
                                     const TDesC& aColumn );


        /**
        * Returns the escaped aLiteral.
        * @param aLiteral The lietral to be escaped
        * @return the escaped aLiteral.
        */
        static HBufC* EscapeTextLC( const TDesC& aLiteral );


        /**
        * Reads up the specified text columns value from the database.
        * In case of error, leaves with CommDb error codes.
        */
        static HBufC16* ReadText16ValueLC( CCommsDbTableView* atable,
                                           const TDesC& aCol );

        /**
        * Reads up the specified text columns value from the database.
        * In case of error, leaves with CommDb error codes.
        * @param atable The table to read from
        * @param aColumn The column to read
        * @return The readed text
        */
        static HBufC8* ReadText8ValueLC( CCommsDbTableView* atable,
                                         const TDesC& aCol );

        /**
        * Reads up the read-only flag for the current record
        * in the passed table
        * In case of error, leaves with CommDb error codes.
        * The function expects the passed table to have a valid
        * current record, that is, after creating the table,
        * record cursor must be setted to the desired record...
        * @param aTable The table to read current records read-only flag.
        * @return The current records read-only flag
        */
        static TBool DoGetReadOnlyL( CCommsDbTableView& aTable );


        /**
        * Commits a transaction on the given database.
        * In case of error, returns the CommDb error codes.
        * @param aDb The database to commit the transaction on
        * @return Error code
        */
        static TInt CommitTransaction( CCommsDatabase& aDb );


        /**
        * Gets the local variant value 
        * @return A TInt stating the variant
        */
        static TInt GetVariantL();


        /**
        * Gives back the TInt Value of the given shared data key for the given 
        * UId. Does't use notify the given shared data, just gives back the
        * actual value.
        * @param aUid actual uid to identify the chunk of shared data
        * @param aKey actual shared data key
        * @return The actual value of the shared data
        */
        static TInt IsGivenSharedDataSupportL( const TUid aUid, 
                                               const TUint32 aKey );

    };

#endif      // CAPENGINE_COMMONS_H

// End of File
