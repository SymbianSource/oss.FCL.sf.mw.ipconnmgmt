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
* Description:  Declaration of the CApProtHandler class.
*
*/


#ifndef CAPPROTHANDLER_H
#define CAPPROTHANDLER_H

// Deprecation warning
#warning This header file has been deprecated and will be fully removed between weeks 12 and 18 of 2010. See Polonium CR #153 for details.
 
 
//  INCLUDES
#include <e32base.h>
#include <ApEngineVer.h>




// FORWARD DECLARATION
class CCommsDatabase;
class CCommsDbTableView;

// CLASS DECLARATION
/**
*  A collection of access point protection handling routines.
*/
NONSHARABLE_CLASS( CApProtHandler )
    {
    public:  // New functions

        /**
        * Returns whether the Access Point Table is protected or not.
        * If the table is protected, new access points can not be added,
        * existing ones can not be modified and can not be deleted.
        * @param aDb The database object
        * @return TBool indicating whether the table is protected or not.
        *
        * @deprecated
        */
        IMPORT_C static TBool IsTableProtectedL( CCommsDatabase* aDb );


        /**
        * Returns whether the given Access Point is protected or not.
        * If the access point is protected, it can not be modified and
        * can not be deleted.
        * @param aDb    The database object
        * @param aUid   The uid of the access point
        * @return TBool indicating whether the access point is protected or not
        *
        * @deprecated
        */
        IMPORT_C static TBool IsApProtectedL( CCommsDatabase* aDb,
                                             TUint32 aUid );


        /**
        * Sets a given Access Point to protected.
        * If the access point is protected, it can not be modified and
        * can not be deleted.
        * @param aDb    The database object
        * @param aUid   The uid of the access point
        *
        * @deprecated
        */
        IMPORT_C static void ProtectItemL( CCommsDatabase* aDb, TUint32 aUid );


        /**
        * Sets a given Access Point to not-protected.
        * If the access point is not protected, it can be modified and
        * can be deleted.
        * @param aDb    The database object
        * @param aUid   The uid of the access point
        *
        * @deprecated
        */
        IMPORT_C static void UnprotectItemL( CCommsDatabase* aDb, 
                                             TUint32 aUid );

        /**
        * Sets the access point table to protected.
        * If the table is protected, new access points can not be added,
        * existing ones can not be modified and can not be deleted.
        * @param aDb    The database object
        *
        * @deprecated
        */
        IMPORT_C static void ProtectTableL( CCommsDatabase* aDb );


        /**
        * Sets the access point table to not-protected.
        * If the table is not protected, new access points can be added,
        * existing ones can be modified and can be deleted.
        * @param aDb    The database object
        *
        * @deprecated
        */
        IMPORT_C static void UnprotectTableL( CCommsDatabase* aDb );


        /**
        * Gets whether the preferences are protected or not
        * @param aDb    The database object
        * @returns TBool indicating whether the preferences are protected
        * or not
        *
        * @deprecated
        */
        IMPORT_C static TBool IsPreferencesProtectedL( CCommsDatabase* aDb );


        /**
        * Sets the preferences to protected.
        * @param aDb    The database object
        *
        * @deprecated
        */
        IMPORT_C static void ProtectPreferencesL( CCommsDatabase* aDb );


        /**
        * Sets the preferences to not-protected.
        * @param aDb    The database object
        *
        * @deprecated
        */
        IMPORT_C static void UnprotectPreferencesL( CCommsDatabase* aDb );


        // The following are not exported, for internal use only!
        /**
        * Returns whether the given record in the given table
        * is protected or not.
        * If the record is protected, it can not be modified and
        * can not be deleted.
        * @param aDb The database object in use
        * @param the table to use
        * @param the uid of the record
        * @return whether the given record in the given table
        * is protected or not.
        */
        static TBool IsRecordProtectedL( CCommsDatabase* aDb,
                                    const TDesC& aTable, TUint32 aUid );

        /**
        * Protects a given record in a given table.
        * @param aDb The database object in use
        * @param the table to use
        * @param the uid of the record
        */
        static void ProtectRecordL( CCommsDatabase* aDb,
                                 const TDesC& aTable, TUint32 aUid );


        /**
        * Unprotects a given record in a given table.
        * @param aDb The database object in use
        * @param the table to use
        * @param the uid of the record
        */
        static void UnprotectRecordL( CCommsDatabase* aDb,
                                    const TDesC& aTable, TUint32 aUid );


        /**
        * Returns whether the given table in a given database is
        * protected or not.
        * @param aDb The database object in use
        * @param the table we need the info on
        * @returns TBool indicating whether the given table in a given
        * database is protected or not.
        */
        static TBool IsTableProtectedL( CCommsDatabase* aDb,
                                        const TDesC& aTable );


        /**
        * Protects a given table in a given database.
        * @param aDb    The database object in use
        * @param aTable The table to protect
        */
        static void ProtectTableL( CCommsDatabase* aDb, const TDesC& aTable );

        /**
        * Unprotects a given table in a given database.
        * @param aDb The database object in use
        * @param aTable The table to unprotect
        */
        static void UnprotectTableL( CCommsDatabase* aDb,
                                     const TDesC& aTable );

        /**
        * Gets the current records' protection state
        * aTable MUST be open and record cursor set to desired record
        * @param aTable The table containign the record whose protection 
        * sate we would like to get
        * @return The records' protection state
        */
        static TBool GetProtectionStateL( CCommsDbTableView& aTable );



        /**
        * Remove all protected AP-s and sets the table to be 'updatable'.
        * @param aDb The database object in use
        *
        * @deprecated
        */
        IMPORT_C static void RemoveProtectedAccessPointsL(CCommsDatabase* aDb);


        /**
        * Remove all protected AP-s and sets the table to be 'updatable'.
        * @param aDb The database object in use
        */
        static void DoRemoveProtectedAccessPointsL( CCommsDatabase& aDb );
    

    };

#endif

// End of File
