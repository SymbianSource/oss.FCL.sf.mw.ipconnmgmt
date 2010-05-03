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
* Session side object representing a destination.
*
*/


#ifndef CMMDESTINATIONINSTANCE_H_
#define CMMDESTINATIONINSTANCE_H_

#include <e32base.h>
#include <metadatabase.h>
#include <cmmanagerdef.h>

#include "cmmserverdefs.h"

class CCmmSession;
class CCmmCache;
class CCmmConnMethodInstance;
class TCmmIdStruct;
class TCmmConnMethodItem;
class CCDSNAPMetadataRecord;


/**
 * Session side object representing a destination. Keeps track of any changes
 * the client makes before they are updated to database.
 */
NONSHARABLE_CLASS( CCmmDestinationInstance ) : public CObject
    {
public:
    /**
     * NewL.
     */
    static CCmmDestinationInstance* NewL( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * NewLC.
     */
    static CCmmDestinationInstance* NewLC( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * Destructor.
     */
    ~CCmmDestinationInstance();

private:
    /**
     * Constructor.
     *
     * @param aCache CmmCache.
     */
    CCmmDestinationInstance( CCmmSession* aCmmSession, CCmmCache* aCache );

    /**
     * Second phase constructor.
     */
    void ConstructL();

public:
    void SetId( const TUint32& aId );
    TUint32 GetId() const;

    /**
     * Sets a new status value.
     */
    void SetStatus( const TCmmDestinationStatus& aStatus );

    /**
     * Gets the current status of this destination instance.
     */
    TCmmDestinationStatus GetStatus() const;

    /**
     * Get handle ID.
     */
    TInt GetHandle() const;

    /**
     * Set handle ID.
     */
    void SetHandle( const TInt& aHandle );

    /**
     * Set the record status for all records.
     */
    void SetStatusForAllRecords( const TCmmRecordStatus& aStatus );

    TUint32 GetElementIdL();
    TUint32 GetRecordTagFromApRecordL();

    TInt SetRecordPointer(
            const TCmmDbRecords& aDestRecord,
            CommsDat::CCDRecordBase* aRecordPointer );
    TInt GetRecordPointer(
            const TCmmDbRecords& aDestRecord,
            CommsDat::CCDRecordBase*& aRecordPointer );

    /**
     * Save the contents of this destination into database. All connection
     * methods including embedded destination, that the client has open handles
     * to, are also updated.
     */
    void UpdateL();

    TDesC& GetDestinationNameL();
    HBufC* GetLocalisedDestinationNameL();
    void SetDestinationNameL( const TDesC& aDestinationName );

    void GetMetadataL(
            const CMManager::TSnapMetadataField& aMetadataField,
            TUint32& aMetadata );
    void SetMetadataL(
            const CMManager::TSnapMetadataField& aMetadataField,
            const TUint32& aMetadata );

    void GetProtectionL( CMManager::TProtectionLevel& aProtectionLevel );
    void SetProtectionL( CMManager::TProtectionLevel aProtectionLevel );

    /**
     * Return true if protection level has been changed. 
     */
    TBool ProtectionChanged();

    /**
     * Return the previous valid protection level of this connection method
     * instance.
     */
    CMManager::TProtectionLevel LastProtectionLevel();

    /**
     * Return the protection level currently set into this destination instance.
     */
    CMManager::TProtectionLevel CurrentProtectionLevel();

    /**
     * Adds a connection method into this destination. Returns the index in
     * the connection method array where the connection method was added.
     */
    TInt AddConnMethodL(
            const CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Adds an embedded destination into this destination. Returns the index in
     * the connection method array where the connection method was added.
     */
    TInt AddEmbeddedDestinationL(
            const CCmmDestinationInstance& aEmbeddedDestinationInstance );

    /**
     * Delete a connection method that belongs to this destination. If the
     * connection method belongs to any other destination, it is only removed
     * from this destination and not deleted. Does not call update.
     */
    void DeleteConnMethodFromDestinationL(
            const CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Remove a connection method from this destination. If the second
     * attribute aTestIfConnected is false, the test to check if connection
     * method is in use is skipped. This is to avoid a duplicate check, so it
     * should have been already tested by the caller.
     */
    void RemoveConnMethodFromDestinationL(
            const CCmmConnMethodInstance& aConnMethodInstance,
            const TBool& aTestIfConnected = ETrue );

    /**
     * Modifies the priority of a connection method inside this destination.
     */
    void ModifyConnMethodPriorityL(
            const CCmmConnMethodInstance& aConnMethodInstance,
            TUint aIndex );

    /**
     * Finds a connection method instance that belongs to the same session and
     * matches the provided ID. Return NULL if no match is found.
     */
    CCmmConnMethodInstance* FindConnMethodInstanceFromSessionById(
            const TUint32& aConnMethodId ) const;

    /**
     * Finds a destination instance that belongs to the same session and matches
     * the provided ID. Return NULL if no match is found.
     */
    CCmmDestinationInstance* FindDestinationInstanceFromSessionById(
            const TUint32& aDestinationId ) const;

    /**
     * Check from all open destination handles in the same session if the given
     * connection method is inside any of them. The given destination is
     * skipped.
     */
    TBool ConnMethodInOtherDestinationInSession(
            const TUint32& aConnMethodId,
            const TUint32& aDestinationId ) const;

    /**
     * Check if the given connection method is in this destination. Include
     * embedded destinations.
     */
    TBool ValidConnMethodIdInDestinationIncludeEmbedded(
            const TUint32& aConnMethodId ) const;

    /**
     * Check if the given connection method is inside this destination and if
     * it is in a 'locked' status. A locked connection method can't be deleted
     * nor removed from the destination. A connection method becomes locked
     * if it is the only remaining connection method in a destination and a
     * virtual IAP points into that destination.
     */
    TBool ConnMethodInDestinationButLocked( const TUint32& aConnMethodId ) const;

    /**
     * Counts how many connection methods that require priority information are
     * currently in this destination. This excludes any virtual IAPs that point
     * to a destination or any embedded destinations from the count. Assumes
     * the connection method array is in order so all wildcard priority IAPs
     * are at the end of the list.
     */
    TUint NumberOfConnMethodsWithPriority() const;

    /**
     * Check if this destination has an embedded destination. Checks the
     * current (possibly unsaved) status of this destination handle, not the
     * current status in database.
     */
    TBool HasEmbedded() const;

    /**
     * Check if this destination has an embedded destination with the specified
     * ID. Checks the current (possibly unsaved) status of this destination
     * handle, not the current status in database.
     */
    TBool HasEmbeddedWithId( const TUint32& aDestinationId ) const;

    /**
     * After update/delete to database, refresh temporary ID to real ID if
     * necessary and refresh status information for any related handles for
     * all client sessions.
     */
    void RefreshHandlesForAllSessions( const TCmmIdStruct& aIdStruct );

    /**
     * If the destination contains a connection method with matching temporary
     * ID, updates the ID to real ID. This is done after the connection method
     * has been successfully saved into the database.
     */
    void RefreshConnMethodId( const TCmmIdStruct& aIdStruct );

    /**
     * Called after this destination has been updated and database transaction
     * has completed successfully. Sets the internal state of this destination
     * instance to reflect the new valid state.
     */
    void UpdateSuccessful();

private:
    TBool AllMandatoryRecordsContainData() const;
    void RefreshRecordL( TCmmDbRecords aRecordType );
    void LoadAllRecordsL();
    void SetMetadataInternetL( const TUint32& aMetadata );
    void SetMetadataHighlight( const TUint32& aMetadata );
    void SetMetadataHiddenAgentL( const TUint32& aMetadata );
    void SetMetadataLocalizationL( const TUint32& aMetadata );
    void SetMetadataPurposeL( const TUint32& aMetadata );

    /**
     * Checks the current metadata for this destination, and verifies there is
     * no other conflicting destinations in the database.
     */
    TBool ConflictingMetadataFoundL();

public:
    // Array of connection method items that belong to this destination. Loaded
    // and maintained in priority order.
    RArray<TCmmConnMethodItem> iConnMethodItemArray; // Owned.

    // Array of connection method IDs that should be deleted if UpdateL() is
    // called for this destination instance.
    RArray<TUint32> iConnMethodsToBeDeleted; // Owned.

private:
    // Session that owns this instance.
    CCmmSession* iCmmSession; // Not owned.

    // Database cache.
    CCmmCache* iCache; // Not owned.

    // The network record.
    CommsDat::CCDNetworkRecord* iNetworkRecord; // Owned.

    // The access point record.
    CommsDat::CCDAccessPointRecord* iDestApRecord; // Owned.

    // The metadata record.
    CCDSNAPMetadataRecord* iMetadataRecord; // Owned.

    // Status flags for each record.
    TCmmRecordStatus iNetworkRecordStatus;
    TCmmRecordStatus iDestApRecordStatus;
    TCmmRecordStatus iMetadataRecordStatus;

    // Destination handle status.
    TCmmDestinationStatus iStatus;

    // Destination ID. If real ID is not available, contains a temporary ID.
    TUint32 iId;

    // Subsession handle ID.
    TInt iHandle;

    // True if protection value has been changed. Reset to false on update.
    TBool iProtectionChanged;

    // The previous valid protection level of this connection method instance.
    // Set correctly only if iProtectionChanged is true.
    CMManager::TProtectionLevel iLastProtectionLevel;

    // The protection level currently set into this destination instance.
    CMManager::TProtectionLevel iCurrentProtectionLevel;
    };

#endif // CMMDESTINATIONINSTANCE_H_

// End of file
