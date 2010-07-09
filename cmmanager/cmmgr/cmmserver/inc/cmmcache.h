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
* Database cache manager.
*
*/


#ifndef CMMCACHE_H_
#define CMMCACHE_H_

#include <e32base.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>
#include <cmgenconnsettings.h>

#include "cmmserverdefs.h"
#include "cmmbearerpriority.h"
#include "cmmconnmethodstruct.h"
#include "cmmconnmethoditem.h"

// Forward declarations.
class CCmmDestinationStruct;
class CCmPluginBaseEng;
class CCmmListenerManager;
class CCmmInstanceMapping;

class CCmmDestinationInstance;
class CCmmConnMethodInstance;

class CCmmTransactionHandler;
class CCmManagerImpl;
class CCmmBearerPriorityCache;


/**
 * A simple class to group a temporary ID and a database assigned valid ID
 * together as a single structure.
 * 
 * Used e.g. when updating a newly received real ID to datastructures
 * containing a certain temporary ID.
 */
NONSHARABLE_CLASS( TCmmIdStruct )
    {
public:
    inline TCmmIdStruct() : iRealId( 0 ), iTemporaryId( 0 )
        {
        }
    inline TCmmIdStruct( TUint32 aRealId, TUint32 aTemporaryId )
            :
            iRealId( aRealId ),
            iTemporaryId( aTemporaryId )
        {
        }
public:
    TUint32 iRealId;
    TUint32 iTemporaryId;
    };


/**
 * The CmmCache-class oversees all database operations.
 * 
 * It manages the cached database data and handles all interactions between
 * that data and the client sessions.
 * 
 * It provides access to the CCmmInstanceMapping-class, that keeps track of the
 * overall structure of destinations and connection methods in the database.
 * 
 * It also manages the database listeners that keep track of database changes
 * caused by outside sources.
 */
NONSHARABLE_CLASS( CCmmCache ) : public CBase
    {
public:
    /**
     * NewL.
     */
    static CCmmCache* NewL(
            CCmManagerImpl* aCmManagerImpl,
            CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins );

    /**
     * NewLC.
     */
    static CCmmCache* NewLC(
            CCmManagerImpl* aCmManagerImpl,
            CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins );

    /**
     * Destructor.
     */
    ~CCmmCache();

private:
    /**
     * Constructor.
     *
     * @param aPlugins Reference to supported plugin array.
     */
    CCmmCache(
            CCmManagerImpl* aCmManagerImpl,
            CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins );

    /**
     * Second phase constructor.
     */
    void ConstructL();

public:
    void OpenDestinationL(
            CCmmDestinationInstance& aDestinationInstance,
            const TUint32 aId );

    /**
     * Refresh the data contained in aDestinationInstance. This means reloading
     * the data from database if necessary. After this call the contents of
     * aDestinationInstance will reflect the current state in the database. 
     */
    void RefreshDestinationL(
            CCmmDestinationInstance& aDestinationInstance );

    void CreateDestinationL(
            CCmmDestinationInstance& aDestinationInstance,
            const TDesC& aName,
            const TUint32 aId );

    void OpenConnMethodL(
            CCmmConnMethodInstance& aConnMethodInstance,
            CCmmDestinationInstance* aDestinationInstance,
            const TUint32 aId );

    /**
     * Refresh the data contained in aConnMethodInstance. This means reloading
     * the data from database if necessary. After this call the contents of
     * aConnMethodInstance will reflect the current state in the database. 
     */
    void RefreshConnMethodL(
            CCmmConnMethodInstance& aConnMethodInstance );

    void CreateConnMethodL(
            CCmmConnMethodInstance& aConnMethodInstance,
            CCmmDestinationInstance* aDestinationInstance,
            const TUint32 aBearerType,
            const TUint32 aConnMethodId );

    /**
     * Creates a copy of the given client instance (copy of the connection
     * method to the cache). The copy is created from the commsdat data
     * (changes not updated will not be shown in the copy).
     * @param aNewConnMethodInstance The copy instance to be created.
     * @param aConnMethodInstance The source instance to be copied.
     */
    void CreateCopyOfConnMethodL(
            CCmmConnMethodInstance& aNewConnMethodInstance,
            CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Reloads a destination record if needed and copies the latest version to
     * the session instance given as parameter.
     */
    void LoadDestinationRecordL(
            CCmmDestinationInstance& aDestinationInstance,
            TCmmDbRecords aDestRecordType );

    /**
     * Saves the modifications in aDestinationInstance into the database. Also
     * all connection methods inside this destination are updated (including
     * any embedded destination).
     */
    void UpdateDestinationL( CCmmDestinationInstance& aDestinationInstance );

    /**
     * Saves the modifications in aConnMethodInstance into database. The second
     * attribute aTemporaryHandle must be true if the connection method
     * instance is only a temporary one created for the duration of this update
     * process only.
     */
    void UpdateConnMethodL(
            CCmmConnMethodInstance& aConnMethodInstance,
            TBool aTemporaryHandle );

    /**
     * Deletes the destination refered by aDestinationInstance from the
     * database. In case other sessions have open handles to the same
     * destination, the status is updated to deleted state, but it is not yet
     * removed from database. (ID must be kept reserved in commsdat until all
     * handles are closed).
     */
    void DeleteDestinationL(
            CCmmDestinationInstance& aDestinationInstance,
            TBool aForced = EFalse );

    /**
     * Deletes a connection method as part of destination update operation. It
     * is already checked that the connection method given as parameter can be
     * deleted and a transaction is already open.
     */
    void DeleteConnMethodAsPartOfDestinationUpdateL( const TUint32 aConnMethodId );

    /**
     * Deletes a connection method as part of destination delete operation. It
     * is already checked that the connection method given as parameter can be
     * deleted and a transaction is already open.
     */
    void DeleteConnMethodAsPartOfDestinationDeleteL( const TUint32 aConnMethodId );

    /**
     * Basic connection method delete. Removes a connection method from any
     * destination it might belong to, and then deletes it.
     */
    void DeleteConnMethodL( CCmmConnMethodInstance& aConnMethodInstance );

    TBool DestinationExistsWithId( const TUint32 aId );
    TBool DestinationExistsWithNameL( const TDesC& aName, const TUint32 aDestinationId );
    TBool DestinationOpenWithId( const TUint32 aId );
    TBool NotSavedDestinationOpenWithNameL( const TDesC& aName, const TUint32 aDestinationId );

    TBool ConnMethodExistsWithId( const TUint32 aConnMethodId );
    TBool ConnMethodOpenWithId( const TUint32 aConnMethodId );

    TBool DestinationExistsWithMetadataLocalizedL(
            CCmmDestinationInstance& aDestinationInstance,
            const TUint32 aValue );
    TBool DestinationExistsWithMetadataPurposeL(
            CCmmDestinationInstance& aDestinationInstance,
            const TUint32 aValue );

    void DbChangeDetectedL( const TUint32 aTableId );
    void DbChangeError( const TUint32 aTableId );

    void CloseDestination( CCmmDestinationInstance& aDestinationInstance );
    void CloseConnMethod( CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Returns a reference to the CommsDat session.
     */
    CommsDat::CMDBSession& Session() const;

    /**
     * Finds out the bearer type and priority of the service type from given
     * IAP record.
     */
    void BearerInfoFromIapRecordL(
            CommsDat::CCDIAPRecord* aIapRecord,
            TUint32& aBearerType,
            TUint& aBearerPriority ) const;

    /**
     * Finds out the priority of the service type in given IAP record.
     */
    void BearerPriorityFromIapRecordL(
            CommsDat::CCDIAPRecord* aIapRecord,
            TUint& aBearerPriority ) const;

    /**
     * Finds out the bearer type of a connection method from given IAP record.
     */
    void BearerTypeFromIapRecordL(
            CommsDat::CCDIAPRecord* aIapRecord,
            TUint32& aBearerType ) const;

    /**
     * Loads and initializes a connection method instance.
     * @param aConnMethodInstance Reference to connection method instance
     * beeing initialized.
     * @param aConnMethodId ID of the connection method beeing opened.
     */
    void OpenConnectionMethodInstanceL(
            CCmmConnMethodInstance& aConnMethodInstance,
            const TUint32 aConnMethodId );

    /**
     * Returns Connection Method specific integer value. CM is loaded(if not loaded already)
     * to get the value.
     * @param aCmId Identifier of the Connection Method.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested integer value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    TUint32 GetConnectionMethodInfoIntL(
            const TUint32 aCmId,
            const TUint32 aAttribute );

    /**
     * Returns Connection Method specific boolean value. CM is loaded(if not loaded already)
     * to get the value.
     * @param aCmId Identifier of the Connection Method.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested boolean value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    TBool GetConnectionMethodInfoBoolL(
            const TUint32 aCmId,
            const TUint32 aAttribute );

    /**
     * Returns Connection Method specific 16 bit string value.
     * CM is loaded(if not loaded already) to get the value.
     * @param aCmId Identifier of the Connection Method.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested string value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    HBufC* GetConnectionMethodInfoStringL(
            const TUint32 aCmId,
            const TUint32 aAttribute );

    /**
     * Returns Connection Method specific 8 bit string value.
     * CM is loaded(if not loaded already) to get the value.
     * @param aCmId Identifier of the Connection Method.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested string value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    HBufC8* GetConnectionMethodInfoString8L(
            const TUint32 aCmId,
            const TUint32 aAttribute );

    /**
     * Returns bearer specific integer value. IAP does not need to be loaded for this.
     * @param aBearerType The type of the plugin to be asked.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested integer value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    TUint32 GetBearerInfoIntL(
            const TUint32 aBearerType,
            const TUint32 aAttribute );

    /**
     * Returns bearer specific boolean value. IAP does not need to be loaded for this.
     * @param aBearerType The type of the plugin to be asked.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested boolean value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    TBool GetBearerInfoBoolL(
            const TUint32 aBearerType,
            const TUint32 aAttribute );

    /**
     * Returns bearer specific 16 bit string value. IAP does not need to be loaded for this.
     * @param aBearerType The type of the plugin to be asked.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested string value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    HBufC* GetBearerInfoStringL(
            const TUint32 aBearerType,
            const TUint32 aAttribute );

    /**
     * Returns bearer specific 8 bit string value. IAP does not need to be loaded for this.
     * @param aBearerType The type of the plugin to be asked.
     * @param aAttribute The identifier of the requested value.
     * @return Returns the requested string value if found. Otherwise leaves with error code
     * KErrNotFound.
     */
    HBufC8* GetBearerInfoString8L(
            const TUint32 aBearerType,
            const TUint32 aAttribute );

    /**
     * Find and return a copy of a connection method item matching the given ID.
     * Returns KErrNotFound, if the connection method is not found.
     */
    TInt GetConnMethodItem(
            const TUint32 aConnMethodId,
            TCmmConnMethodItem& aConnMethodItem ) const;

    /**
     * Returns all conenction method IDs. Unsupported connection methods are
     * included if aCheckBearerType is set to EFalse.
     */
    void GetAllConnMethodsL(
            RArray<TUint32>& aConnMethodArray,
            TBool aCheckBearerType ) const;

    /**
     * Returns the number of destinations the provided connection method
     * belongs to.
     */
    TInt DestinationsContainingConnMethod( const TUint32 aConnMethodId ) const;

    /**
     * Return the EasyWLAN IAP ID, zero if not found or WLAN not supported.
     */
    TUint32 EasyWlanIdL() const;

    /**
     * Check if WLAN is supported on phone.
     */
    TBool WlanSupported() const;

    /**
     * Find out the internet destination ID. Set to 0 if not found.
     */
    void InternetDestinationIdL( TUint& aInternetDestinationId ) const;

    /**
     * Return the number of destinations in database.
     */
    TInt GetDestinationCount() const;

    /**
     * Returns all the valid Destinations
     */
    void GetDestinationsL( RArray<TUint32>& aDestinationArray ) const;

    /**
     * Returns all the valid connection methods under given destination.
     */
    void GetConnMethodsFromDestinationL(
            const TUint32 aDestinationId,
            RArray<TCmmConnMethodItem>& aConnMethodArray ) const;

    /**
     * Copies the bearer priority array's contents to aArray.
     */
    void CopyBearerPriorityArrayL( RPointerArray<CCmmBearerPriority>& aArray ) const;

    /**
     * Updates the bearer priority array with the contents of aArray.
     */
    void UpdateBearerPriorityArrayL( const RPointerArray<CCmmBearerPriority>& aArray );

    TBool ConnMethodInOtherDestination(
            const TUint32 aConnMethodId,
            const TUint32 aDestinationId );
    TUint32 NextFreeTemporaryId();

    /**
     * Read general connection settings from database.
     */
    void ReadGenConnSettingsL( TCmGenConnSettings& aGenConnSettings ) const;

    /**
     * Write general connection settings to database.
     */
    void WriteGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings );

    /**
     * Enumerates connections and checks if the given connection method is
     * connected.
     * @param aCmId Id of the connection method to be checked.
     * @return Returns ETrue if the CM is connected and EFalse if it's not
     * connected.
     */
    TBool CheckIfCmConnected( const TUint32 aCmId ) const;

    /**
     * Enumerates connections and checks if any of the connection methods in
     * the given destination is connected.
     * If pointer to destination instance is given, then information about
     * relevant connection methods is retrieved from that. Otherwise the
     * information is retrieved from instance mapping using the given ID.
     */
    TBool DestinationConnectedL(
            const TUint32 aDestinationId,
            CCmmDestinationInstance* aDestinationInstance = NULL ) const;

    /**
     * Check from database if the given destination is an embedded destination
     * in any other destination.
     */
    TBool DestinationIsEmbedded( const TUint32 aDestinationId ) const;

    /**
     * Check from database if the given destination has an embedded destination.
     */
    TBool DestinationHasEmbedded( const TUint32 aDestinationId ) const;

    /**
     * Check from database if the given destination is pointed to by any
     * virtual IAP.
     */
    TBool DestinationPointedToByVirtualIap( const TUint32 aDestinationId ) const;

    /**
     * Check from database if the given connection method is pointed to by any
     * virtual IAP.
     */
    TBool ConnMethodPointedToByVirtualIap( const TUint32 aConnMethodId ) const;

    /**
     * Check if the given connection method is the only connection method in
     * the given destination and if a virtual IAP points to that destination.
     */
    TBool ConnMethodInDestinationButLocked(
            const TUint32 aConnMethodId,
            const TUint32 aDestinationId ) const;

    /**
     * Remove all references to the given connection method from the
     * datamobility selection policy records. Then update instance mapping to
     * reflect the current database state, and notify any possible client
     * handles for the changed destinations.
     */
    void RemoveAllReferencesToConnMethodL(
            CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Update the ID of a new destination from temporary ID to real ID after a
     * successful update to database.
     */
    void RefreshDestinationId( const TCmmIdStruct& aIdStruct );

    /**
     * Update the ID of a new connection method from temporary ID to real ID
     * after a successful update to database.
     */
    void RefreshConnMethodId( const TCmmIdStruct& aIdStruct );

    /**
     * During destination update, after updating connection methods inside the
     * destination, the database records containing the information of what
     * connection methods are inside the destination need to be updated with
     * real IDs for any newly created connection methods. These real IDs are
     * held in the temporary array iUpdatedConnMethods until successful commit
     * to database. This method is used to find out those real IDs before that.
     */
    void TranslateTemporaryId( const TUint32 aTemporaryId, TUint32& aRealId ) const;

    /**
     * Check if the given connection method can have all references removed and
     * made into an uncategorized connection method.
     */
    void CheckIfConnMethodReferencesCanBeRemovedL(
            const CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Check if given connection method is referenced from any protected destination.
     */
    void CheckIfConnMethodBelongsToProtectedDestinationL(
            const CCmmConnMethodInstance& aConnMethodInstance,
            TBool& aBelongsToProtectedDestination );

    /**
     * Check if the given connection method can be deleted.
     */
    void CheckIfConnMethodCanBeDeletedL( const CCmmConnMethodInstance& aConnMethodInstance );

    /**
     * Check if the given destination can be deleted.
     */
    void CheckIfDestinationCanBeDeletedL( const CCmmDestinationInstance& aDestinationInstance );

    /**
     * Return the requested table ID.
     */
    CommsDat::TMDBElementId TableId( TCmmDbRecords aRecord );

private:
    TInt FindDestinationFromCache( const TUint32 aId );
    TInt FindNotSavedDestinationFromCacheL( const TDesC& aName, const TUint32 aDestinationId );

    TInt FindConnMethodFromCache( const TUint32 aConnMethodId );

    void ReplaceGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings );

    /**
     * Opens a transaction and deletes the given connection method.
     */
    void DeletePluginL( CCmmConnMethodStruct& aConnMethodStruct );

    /**
     * Initiate the deletion of given destination if none of the connection
     * methods inside it are connected.
     */
    void DeleteDestinationForcedL( CCmmDestinationInstance& aDestinationInstance );

    /**
     * Informs all the loaded iaps if something related to their tables
     * changed in commsdat.
     */
    void NotifyPluginsForTableChangesL( const TUint32 aTableId );

private:
    // Monitors changes in the database.
    CCmmListenerManager* iListenerManager; // Owned.

    // Contains the current destination/connection method structure.
    CCmmInstanceMapping* iInstanceMapping; // Owned.

    // Array of currently opened destinations.
    RPointerArray<CCmmDestinationStruct> iDestinationArray; // Owned.

    // Array of currently opened connection methods.
    RPointerArray<CCmmConnMethodStruct> iConnMethodArray; // Owned.

    // Temporary array to hold information about connection methods that are
    // deleted during a database transaction. This information is needed
    // immediately when the database transaction is successfully commited.
    RArray<TCmmIdStruct> iDeletedConnMethods; // Owned.

    // Temporary arrays to hold information about connection methods that are
    // updated during a database transaction. This information is needed
    // immediately when the database transaction is successfully commited.
    RArray<TCmmIdStruct> iUpdatedConnMethods; // Owned.
    RPointerArray<CCmmConnMethodInstance> iUpdatedConnMethods2; // Does not own contents.

    // Temporary arrays to hold information about destinations that are updated
    // during a database transaction. This information is needed immediately
    // when the database transaction is successfully commited.
    RArray<TCmmIdStruct> iUpdatedDestinations; // Owned.
    RPointerArray<CCmmDestinationInstance> iUpdatedDestinations2; // Does not own contents.

    // Transaction handler for CommsDat operations. Created and owned by
    // CCmManagerImpl.
    CCmmTransactionHandler* iTrans; // Not owned.

    // Pointer to bearer priority array.
    CCmmBearerPriorityCache* iBearerPriorityCache; // Owned.

    CArrayPtrFlat<const CCmPluginBaseEng>* iPlugins; // Not owned.

    // Connection method manager.
    CCmManagerImpl* iCmManagerImpl; // Not owned.

    // Counter to generate new unique IDs. These are used to link new objects
    // without valid IDs between session side and cache.
    TUint32 iCurrentTemporaryId;

    // Destination network table ID.
    CommsDat::TMDBElementId iSnapTableId;
    // Destination metadata table ID.
    CommsDat::TMDBElementId iSnapMetadataTableId;
    };

#endif // CMMCACHE_H_

// End of file
