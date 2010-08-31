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
* Cache side object representing a destination.
*
*/


#ifndef CMMDESTINATIONSTRUCT_H_
#define CMMDESTINATIONSTRUCT_H_

#include <e32base.h>
#include <commsdattypesv1_1.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#include <commsdattypeinfov1_1_internal.h>
#endif

#include "cmmserverdefs.h"

class CCmmTransactionHandler;
class CCmmDestinationInstance;
class CCmmCache;
class CCDSNAPMetadataRecord;


/**
 * Cache side object representing a destination. Reflects the current state in
 * database.
 */
NONSHARABLE_CLASS( CCmmDestinationStruct ) : public CBase
    {
public:
    static CCmmDestinationStruct* NewL(
            CCmmCache* aCache,
            CCmmTransactionHandler* aTransactionHandler,
            const TUint32 aId );
    static CCmmDestinationStruct* NewL(
            CCmmCache* aCache,
            CCmmTransactionHandler* aTransactionHandler,
            const TDesC& aName,
            const TUint32 aId );

    static CCmmDestinationStruct* NewLC(
            CCmmCache* aCache,
            CCmmTransactionHandler* aTransactionHandler,
            const TUint32 aId );
    static CCmmDestinationStruct* NewLC(
            CCmmCache* aCache,
            CCmmTransactionHandler* aTransactionHandler,
            const TDesC& aName,
            const TUint32 aId );

    virtual ~CCmmDestinationStruct();

private:
    CCmmDestinationStruct( CCmmCache* aCache, CCmmTransactionHandler* aTransactionHandler );

    // Opens an existing destination, based on ID.
    void ConstructL( const TUint32 aDestinationId );

    // Creates a new destination. With name and optionally also with ID.
    void ConstructL( const TDesC& aName, const TUint32 aId );

public:
    void SetId( const TUint32 aId );
    TUint32 GetId() const;

    /**
     * Get the destination ID from network record. To get a valid ID, this must
     * be called only during or after a call to UpdateL().
     */
    TUint32 GetRealId() const;
    TDesC& GetDestinationNameL();

    /**
     * Gets the current status of this destination struct.
     */
    TCmmDestinationStatus GetStatus() const;
    TInt GetReferenceCount() const;

    /**
     * Sets a new status value.
     */
    void SetStatus( const TCmmDestinationStatus& aStatus );

    /**
     * Notify about a possible change in database on specified record table.
     */
    void NotifyRecordChange( const TCmmDbRecords aRecordType );

    /**
     * Set the record status for all records.
     */
    void SetStatusForAllRecords( const TCmmRecordStatus aStatus );

    /**
     * Copies the data for this destination to a session instance and
     * increments the reference counter by one.
     */
    void CreateDestinationInstanceL(
            CCmmDestinationInstance& aDestinationInstance );

    /**
     * Refresh the data contained in aDestinationInstance. This means reloading
     * the data from database if necessary. After this call the contents of
     * aDestinationInstance will reflect the current state in the database. 
     */
    void RefreshDestinationInstanceL(
            CCmmDestinationInstance& aDestinationInstance );

    /**
     * Reloads a destination record if needed and copies the latest version to
     * the session instance given as parameter.
     */
    void LoadRecordL(
            CCmmDestinationInstance& aDestinationInstance,
            TCmmDbRecords aRecordType );

    /**
     * Save the contents of a session side destination handle into database.
     */
    void UpdateL(
            CCmmDestinationInstance& aDestinationInstance,
            CCmmCache* aCache );

    /**
     * Delete all database records of this destination. Also removes any
     * records making this destination an embedded destination in another
     * destination.
     */
    void DeleteL();

    /**
     * 
     * This should be called when a client session closes a destination handle.
     * Reference counter is decremented by one and the remaining number of
     * references is returned.
     */
    TInt DestinationInstanceClosed();

    /**
     * Called after this destination has been updated and database transaction
     * has completed successfully. Sets the internal state of this destination
     * structure to reflect the new valid state.
     */
    void UpdateSuccessful();

private:
    void InitializeDestApRecordL();
    void SetDefaultTierManagerL();
    void SetDefaultMCprL();
    void SetDefaultCprL();
    void SetDefaultSCprL();
    void SetDefaultProtocolL();

    /**
     * Set attribute flag on the given record.
     * @param aRecord Record to be set.
     * @param aAttribute Attribute to be set.
     * @param aSet ETrue to set, EFalse to clear.
     */
    void SetAttribute( CommsDat::CCDRecordBase* aRecord, TUint32 aAttribute, TBool aSet );

    CommsDat::CCDRecordBase* CopyRecordL(
            TCmmDbRecords aRecordType,
            CommsDat::CCDRecordBase* aSource );

private:
    // Database cache.
    CCmmCache* iCache; // Not owned.

    // The transaction handler.
    CCmmTransactionHandler* iTransactionHandler; // Not owned.

    // The access point record.
    CommsDat::CCDAccessPointRecord* iDestApRecord; // Owned.
    // The network record.
    CommsDat::CCDNetworkRecord* iNetworkRecord; // Owned.
    // The metadata record.
    CCDSNAPMetadataRecord* iMetadataRecord; // Owned.

    // Record status variables.
    TCmmRecordStatus iDestApRecordStatus;
    TCmmRecordStatus iNetworkRecordStatus;
    TCmmRecordStatus iMetadataRecordStatus;

    // Counts number of session side objects referring to this destination.
    TInt iReferenceCounter;

    // Destination status.
    TCmmDestinationStatus iStatus;

    // Destination ID. If real ID is not available, contains a temporary ID.
    TUint32 iId;

    // Destination name for newly created destination.
    // Only valid until destination is saved to database (status is ECmmDestinationStatusNotSaved).
    HBufC* iName; // Owned.
    };

#endif // CMMDESTINATIONSTRUCT_H_

// End of file
