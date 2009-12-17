/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides cached information on IAPs and SNAPs in CommsDat.
*
*/

#ifndef CONNMONCOMMSDATCACHE_H
#define CONNMONCOMMSDATCACHE_H

#include <e32base.h>
#include <commsdattypesv1_1.h>
#include <rconnmon.h>

class CConnMonServer;
class CConnMonIAP;
class TConnMonBearer;

NONSHARABLE_CLASS( TLanBearerEntry )
    {
public:
    TInt iId;
    TBool iWlanBearer;
    };

NONSHARABLE_CLASS( TConnMonSnapEntry )
    {
public:
    TConnMonSnapEntry();

    TConnMonSnapEntry(
            TUint aId,
            TUint aIapId,
            TUint aSnapId );

    static TInt Compare(
            const TConnMonSnapEntry& aFirst,
            const TConnMonSnapEntry& aSecond );

    static TInt FindCompare(
            const TInt* aKey,
            const TConnMonSnapEntry& aEntry );

    static TBool Match(
            const TConnMonSnapEntry& aFirst,
            const TConnMonSnapEntry& aSecond );

public:
    TUint iId;              // SNAP ID
    TUint iNextLayerIapId;  // Real or virtual IAP ID
    TUint iNextLayerSnapId; // Embeded SNAP ID
    TInt iAvailability;     // TConnMonAvailability

    // Index of linked object. SNAP or IAP depending if iNextLayerIapId or
    // iNextLayerSnapId is set.
    TInt iNextLayerIndex;
    };

NONSHARABLE_CLASS( TConnMonIapEntry )
    {
public:
    TConnMonIapEntry();

    TConnMonIapEntry(
            TUint aId,
            TUint aBearerType,
            TUint aServiceType );

    static TInt Compare(
            const TConnMonIapEntry& aFirst,
            const TConnMonIapEntry& aSecond );

    static TInt FindCompare(
            const TInt* aKey,
            const TConnMonIapEntry& aEntry );

    static TBool Match(
            const TConnMonIapEntry& aFirst,
            const TConnMonIapEntry& aSecond );

public:
    TUint iId;              // IAP ID (unique among IAPs)
    TUint iBearerType;      // Bearer type from IAP table
    TUint iServiceType;     // Service type from IAP table
    TInt iAvailability;     // TConnMonAvailability
    TUint iNextLayerIapId;  // Real or virtual IAP ID
    TUint iNextLayerSnapId; // SNAP ID
    TInt iNextLayerIndex;   // Index of linked object
    };

NONSHARABLE_CLASS( TConnMonVirtualIapEntry )
    {
public:
    TConnMonVirtualIapEntry();

    TConnMonVirtualIapEntry(
            TUint aId,
            TUint aNextLayerIapId,
            TUint aNextLayerSnapId );

    static TInt Compare(
            const TConnMonVirtualIapEntry& aFirst,
            const TConnMonVirtualIapEntry& aSecond );

    static TInt FindCompare(
            const TInt* aKey,
            const TConnMonVirtualIapEntry& aEntry );

    static TBool Match(
            const TConnMonVirtualIapEntry& aFirst,
            const TConnMonVirtualIapEntry& aSecond );

public:
    TUint iId;              // IAP ID (unique among IAPs) ID
    TUint iNextLayerIapId;  // Real or virtual IAP ID
    TUint iNextLayerSnapId; // SNAP ID
    };


/**
 * ConnMon CommsDat cache
 */
NONSHARABLE_CLASS( CConnMonCommsDatCache ) : public CBase
    {
private:
    // Availability status for cache tables
    enum TConnMonAvailability
        {
        EConnMonAvailabilityUnknown = 0,    // Not solved yet
        EConnMonAvailabilityProcessing,     // Availability is beeing solved (loop detection)
        EConnMonAvailabilityUnavailable,    // Not available
        EConnMonAvailabilityAvailable       // Available
        };
    enum TConnMonCacheInitState
        {
        EConnMonCacheInitNotStarted = 0,
        EConnMonCacheInitInProgress,
        EConnMonCacheInitCompleted
        };
    enum TConnMonCacheBearerType
        {
        EConnMonCacheBearerTypeUnknown = 0,
        EConnMonCacheBearerTypeAny,
        EConnMonCacheBearerTypeVirtual
        };
    enum TConnMonCacheServiceType
        {
        EConnMonCacheServiceTypeUnknown = 0,
        EConnMonCacheServiceTypeAny,
        EConnMonCacheServiceTypeCsd,
        EConnMonCacheServiceTypeGprs,
        EConnMonCacheServiceTypeLan,
        EConnMonCacheServiceTypeWlan
        };

public:
    /**
     * Two-phased constructor.
     */
    static CConnMonCommsDatCache* NewL();

    /**
     * Two-phased constructor.
     */
    static CConnMonCommsDatCache* NewLC();

    /**
    * Destructor.
    */
    virtual ~CConnMonCommsDatCache();

    /**
     * Initializes the SNAP and IAP cache. Reads all relevant IAP and SNAP
     * information from CommsDat and writes it to cache. This should be called
     * before the first client has connected to ConnMon.
     *
     * @return error code.
     */
    void Init(
            CConnMonServer* aServer,
            CConnMonIAP* aConnMonIap,
            RPointerArray<TConnMonBearer>* aBearers );

    inline TUint32 GetIapRecordTableId() { return iIapRecordTableId; }
    inline TUint32 GetSnapRecordTableId() { return iSnapRecordTableId; }
    inline TUint32 GetVirtualRecordTableId() { return iVirtualRecordTableId; }

    /**
     * Reads a changed CommsDat table and if changes are detected, updates the
     * internal cache.
     *
     * @param aTableId Table ID of the changed CommsDat table
     */
    void RefreshCommsDatCacheL( const TUint32 aTableId );

    /**
     * Updates the internal structures of the cache and recalculates all SNAP
     * and IAP availability information.
     * If aCanSendEvents is true, will send IAP and/or SNAP availability
     * changed events if changes detected.
     */
    void RefreshAvailabilityInfo( const TBool aCanSendEvents );

    /**
     * Sets the availability status to available for all IAPs with matching
     * bearer ID / service type.
     */
    void SetAvailableIapsWithBearerId( const TUint aServiceType );

    /**
     * Sets the availability status to available for the IAP with given ID.
     */
    void SetAvailableIapWithId( const TUint aId );

    /**
     * Get available IAP IDs related to given bearer ID.
     *
     * @param aBearerId Bearer ID to identify between all bearers and a
     * specific one.
     * @param aIapInfo A TConnMonIapInfo object to contain the IDs.
     * @return error code.
     */
    TInt GetAvailableIaps( const TUint aBearerId, TConnMonIapInfo& aIapInfo );

    /**
     * Get available SNAP IDs.
     *
     * @param aSnapInfo A TConnMonSNAPInfo object to contain the IDs.
     */
    void GetAvailableSnaps( TConnMonSNAPInfo& aSnapInfo );

    /**
     * Get available SNAP IDs.
     *
     * @param aSnapIds Array to contain the IDs.
     * @return KErrNoMemory if the RArray fails to allocate memory for itself,
     * KErrNone otherwise.
     */
    TInt GetAvailableSnaps( RArray<TConnMonId>& aSnapIds );

private:

    /**
     * Constructor.
     */
    CConnMonCommsDatCache();

    /**
     * 2nd level constructor.
     */
    void ConstructL();

    /**
     * Reads all relevant CommsDat information to cache and initializes the
     * SNAP and virtual IAP table IDs.
     */
    void InitCommsDatCacheL();

    /**
     * Reads all IAPinformation from CommsDat and updates the cache if
     * necessary.
     *
     * @return ETrue if cache information was changed, EFalse otherwise.
     */
    TBool RefreshCommsDatIapCacheL( CommsDat::CMDBSession& aDb );

    /**
     * Reads all IAP table information from CommsDat into an array.
     */
    void ReadCommsDatIapTableL(
            CommsDat::CMDBSession& aDb,
            RArray<TConnMonIapEntry>& aCurrentIapData,
            RArray<TUint>& aCurrentWlanIapIds );

    /**
     * Reads one record from IAP table and finds out the service and bearer
     * types.
     */
    void ReadCommsDatIapEntryL(
            CommsDat::CCDIAPRecord* aIapEntry,
            RArray<TLanBearerEntry>& aLanBearerTableCache,
            TUint& aBearerType,
            TUint& aServiceType );

    /**
     * Reads all records from the LAN bearer table and checks if they are WLAN
     * bearer type. Results are stored in an array.
     */
    void ReadCommsDatLanBearerTableL(
            CommsDat::CMDBSession& aDb,
            RArray<TLanBearerEntry>& aLanBearerTableCache );

    /**
     * Reads all SNAP information from CommsDat and updates the cache if
     * necessary.
     *
     * @return ETrue if cache information was changed, EFalse otherwise.
     */
    TBool RefreshCommsDatSnapCacheL( CommsDat::CMDBSession& aDb );

    /**
     * Reads all SNAP information from CommsDat to an array.
     */
    void ReadCommsDatSnapTableL(
            CommsDat::CMDBSession& aDb,
            RArray<TConnMonSnapEntry>& aCurrentSnapData );

    /**
     * Reads all virtual IAP link information from CommsDat and updates the
     * cache if necessary. CommsDat is not read if IAP table did not contain
     * any virtual IAPs.
     *
     * @return ETrue if cache information was changed, EFalse otherwise.
     */
    TBool RefreshCommsDatVirtualIapCacheL( CommsDat::CMDBSession& aDb );

    /**
     * Reads all virtual IAP link information from CommsDat to an array.
     */
    void ReadCommsDatVirtualIapTableL(
            CommsDat::CMDBSession& aDb,
            RArray<TConnMonVirtualIapEntry>& aCurrentVirtualIapData );

    /**
     * Compares to sorted RArray<TUint> arrays. Returns ETrue if the arrays are not identical.
     */
    TBool CompareSortedArrays(
            const RArray<TUint>& aFirstArray,
            const RArray<TUint>& aSecondArray );

    /**
     * Compares to sorted RArray<TConnMonIapEntry> arrays. Returns ETrue if the arrays are not identical.
     */
    TBool CompareSortedArrays(
            const RArray<TConnMonIapEntry>& aFirstArray,
            const RArray<TConnMonIapEntry>& aSecondArray );

    /**
     * Compares to sorted RArray<TConnMonSnapEntry> arrays. Returns ETrue if
     * the arrays are not identical.
     */
    TBool CompareSortedArrays(
            const RArray<TConnMonSnapEntry>& aFirstArray,
            const RArray<TConnMonSnapEntry>& aSecondArray );

    /**
     * Compares to sorted RArray<TConnMonVirtualIapEntry> arrays. Returns ETrue
     * if the arrays are not identical.
     */
    TBool CompareSortedArrays(
            const RArray<TConnMonVirtualIapEntry>& aFirstArray,
            const RArray<TConnMonVirtualIapEntry>& aSecondArray );

    /**
     * Copy new data into a RArray<TUint> array.
     */
    void DeepCopy(
            const RArray<TUint>& aSourceArray,
            RArray<TUint>& aTargetArray );

    /**
     * Updates the index links for SNAPs and virtual IAPs.
     */
    void UpdateSnapAndVirtualIapLinks();

    /**
     * Sets the availability flag to false for all IAPs and SNAPs contained in
     * cache.
     */
    void ResetAllAvailabilityInfo();

    /**
     * Solve SNAP and virtual IAP availability. The results depend on real IAP
     * availability, so those need to be set when this method is called.
     */
    void SolveSnapAndVirtualIapAvailability();

    /**
     * Create a EConnMonIapAvailabilityChange event and add it to the event
     * queue to be sent to clients.
     */
    TInt SendIapAvailabilityEvent();

    /**
     * Create a EConnMonSNAPsAvailabilityChange event and add it to the event
     * queue to be sent to clients.
     */
    TInt SendSnapAvailabilityEvent();

    /**
     * Finds the index in cache for IAP with id aId. Returns KErrNotFound (-1)
     * if no match is found.
     */
    TInt FindIapIndex( const TUint aId );

    /**
     * Finds the first index in cache for SNAP with ID aId. Returns
     * KErrNotFound (-1) if no match is found.
     */
    TInt FindSnapIndex( const TUint aId );

    /**
     * Takes a SNAP cache index and reads the ID from that entry. Then adjusts
     * the availability for each cache entry related to that ID.
     */
    void SetSnapAvailabilityAtIndex( TUint aIndex, TInt aAvailability );

    /**
     * Check IAP availability by index
     */
    TInt CheckIapAvailability( TUint aIndex );

    /**
     * Check SNAP availability by index
     */
    TInt CheckSnapAvailability( TUint aIndex );

    /**
     * Updates the internal list of available IAP IDs and returns ETrue if it
     * has changed.
     */
    TBool UpdateAvailableIaps();

    /**
     * Updates the internal list of available SNAP IDs and returns ETrue if it
     * has changed.
     */
    TBool UpdateAvailableSnaps();

    /**
     * Checks from LAN bearer table array if given ID has a WLAN bearer.
     */
    TBool HasWlanBearer(
            TUint32 aId,
            RArray<TLanBearerEntry>& aLanBearerTableCache );

    /**
     * Convert a bearer ID to (cache internal) service type
     */
    TInt ConvertBearerIdToServiceType(
            const TUint aBearerId,
            TUint& aServiceType );

    /**
     * Checks from CommsDat if WLAN background scanning is enabled.
     *
     * @return ETrue if WLAN background scanning is enabled, EFalse otherwise.
     */
    TBool IsWlanBackgroundScanningEnabledL();

private: // Data
    // To keep track of Init()-method call.
    TInt iInitStatus;

    // Flags set when changes in CommsDat (compared to cache) detected.
    TBool iIapsChanged;
    TBool iSnapsChanged;
    TBool iVirtualIapsChanged;

    // Set in init phase, is WLAN bearer available or not.
    TBool iWlanSupportEnabled;

    // Table IDs for CommsDat record types, will be set during init phase.
    TUint32 iIapRecordTableId;
    TUint32 iSnapRecordTableId;
    TUint32 iVirtualRecordTableId;

    // Counter for virtual IAPs. Whwn zero, virtual IAP link table is not read.
    TInt iVirtualIapCount;

    // Cache for WLAN IAP IDs, to efficiently detect any changes. WLAN scan
    // enabled when changes occur.
    RArray<TUint> iWlanIapIdCache;

    // Cache for available IAP IDs, to efficiently detect availability changes.
    RArray<TUint> iIapIdCache;

    // Cache for available SNAP IDs, to efficiently detect availability changes.
    RArray<TUint> iSnapIdCache;

    // Cache tables, own
    RArray<TConnMonIapEntry>* iIapCache;
    RArray<TConnMonSnapEntry>* iSnapCache;
    RArray<TConnMonVirtualIapEntry>* iVirtualIapCache;

    // Pointer to server module, not own
    CConnMonServer* iServer;

    // Pointer to IAP module, not own
    CConnMonIAP* iIap;

    // Bearers, not own
    RPointerArray<TConnMonBearer>* iBearers;
    };

#endif // CONNMONCOMMSDATCACHE_H

// End-of-file
