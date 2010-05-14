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
* Keeps track of the general destination/connection method structure in
* database.
*
*/


#ifndef CMMINSTANCEMAPPING_H_
#define CMMINSTANCEMAPPING_H_

#include <e32base.h>

#include "cmmcache.h"
#include "cmmconnmethoditem.h"


NONSHARABLE_CLASS( CDestination ) : public CBase
    {
public:
    static CDestination* NewL();
    static CDestination* NewLC();
    ~CDestination();

private:
    CDestination();
    void ConstructL();

public:
    // The destination ID.
    TUint32 iId;

    // Array of connection methods inside this destination. Includes also
    // embedded destinations.
    RArray<TCmmConnMethodItem> iConnMethodItemArray;

    // IDs for unsupported connection methods inside this destination.
    RArray<TUint32> iUnsupportedConnMethods;
    
    // The destination metadata.
    TUint32 iMetadata;
    };


NONSHARABLE_CLASS( CCmmInstanceMapping ) : public CBase
    {
public:
    /**
     * NewL.
     */
    static CCmmInstanceMapping* NewL( CCmmCache& aCache );

    /**
     * NewLC.
     */
    static CCmmInstanceMapping* NewLC( CCmmCache& aCache );

    /**
     * Destructor.
     */
    ~CCmmInstanceMapping();

private:
    /**
     * Constructor.
     *
     * @param aCache database cache object.
     */
    CCmmInstanceMapping( CCmmCache& aCache );

    /**
     * Second phase constructor.
     */
    void ConstructL();

public: //TODO, sort methods, they are currently in mixed order in .cpp file.
    /**
     * Update the connection method and destination structures to current state.
     */
    void RefreshL();

    /**
     * Check if the given ID is a valid existing destination ID.
     */
    TBool ValidDestinationId( const TUint32& aId ) const;

    /**
     * Check if the given ID is a valid existing connection method ID.
     */
    TBool ValidConnMethodId( const TUint32& aId ) const;

    /**
     * Check from database if the given destination is an embedded destination
     * in any other destination.
     */
    TBool DestinationIsEmbedded( const TUint32& aDestinationId ) const;

    /**
     * Check from database if the given destination has an embedded destination.
     */
    TBool DestinationHasEmbedded( const TUint32& aDestinationId ) const;

    /**
     * Check from database if the given destination is pointed to by any
     * virtual IAP.
     */
    TBool DestinationPointedToByVirtualIap( const TUint32& aDestinationId ) const;

    /**
     * Check from database if the given connection method is pointed to by any
     * virtual IAP.
     */
    TBool ConnMethodPointedToByVirtualIap( const TUint32& aConnMethodId ) const;

    /**
     * Check if the given connection method is the only connection method in
     * the given destination and if a virtual IAP points to that destination.
     */
    TBool ConnMethodInDestinationButLocked(
            const TUint32& aConnMethodId,
            const TUint32& aDestinationId ) const;

    /**
     * Get bearer type of connection method matching given ID.
     * Return KErrNone if ID is found, KErrNotFound if not.
     */
    TInt GetConnMethodBearerType( const TUint32& aConnMethodId, TUint32& bearerType ) const;

    /**
     * Returns the number of destinations the provided connection method
     * belongs to.
     */
    TInt DestinationsContainingConnMethod( const TUint32& aConnMethodId ) const;

    /**
     * Returns the destination ids containing the connection method given as
     * parameter.
     */
    void DestinationsContainingConnMethodL(
            const TUint32 aConnMethodId,
            RArray<TUint32>& aDestinationIds ) const;

    /**
     * Returns all conenction method IDs. Unsupported connection methods are
     * included if aCheckBearerType is set to EFalse.
     */
    void GetAllConnMethodsL(
            RArray<TUint32>& aConnMethodArray,
            TBool aCheckBearerType = ETrue ) const;

    /**
     * Returns the number of destinations.
     */
    TInt GetDestinationCount() const;

    /**
     * Returns all the valid destinations.
     */
    void GetDestinationsL( RArray<TUint32>& aDestinationArray ) const;

    /**
     * Inserts all the valid connection methods inside the given destination
     * into the provided connection method item array. The array is reset first.
     */
    void GetConnMethodsFromDestinationL(
            const TUint32& aDestinationId,
            RArray<TCmmConnMethodItem>& aConnMethodArray ) const;

    /**
     * Returns ETrue if the provided connection method belongs to any other
     * destination than the one given.
     */
    TBool ConnMethodInOtherDestination(
            const TUint32& aConnMethodId,
            const TUint32& aDestinationId );

    /**
     * Return the EasyWLAN IAP ID, zero if not found or WLAN not supported.
     */
    TUint32 EasyWlanIdL();

    /**
     * Find out the internet destination ID. ID is set to 0 if not found.
     */
    void InternetDestinationIdL( TUint& aInternetDestinationId );

    /**
     * Add a connection method ID to deleted list. Ignores any duplicates. Also
     * removes the connection method from destination/connection method
     * structures so Refresh()-call is not needed.
     */
    void AddConnMethodToDeletedListL( const TUint& aConnMethodId );

    /**
     * Remove a connection method ID from deleted list. Nothing happens if ID
     * is not found from the list.
     */
    void RemoveConnMethodFromDeletedList( const TUint& aConnMethodId );

    /**
     * Add a destination ID to deleted list. Ignores any duplicates. Also
     * removes the destination from destination/connection method structures
     * so Refresh()-call is not needed.
     */
    void AddDestinationToDeletedListL( const TUint& aDestinationId );

    /**
     * Remove a destination ID from deleted list. Nothing happens if ID is not
     * found from the list.
     */
    void RemoveDestinationFromDeletedList( const TUint& aDestinationId );

    /**
     * Remove the connection method from current destination/connection method
     * structures. This is a lot faster than calling Refresh().
     */
    void RemoveConnMethod( const TUint32& aConnMethodId );

    /**
     * Remove the connection method from current destination/connection method
     * structures. This is a lot faster than calling Refresh(). The ID of any
     * changed destination is added to the aChangedDestinations-array.
     */
    void RemoveConnMethod(
            const TUint32& aConnMethodId,
            RArray<TUint32>& aChangedDestinations );

    /**
     * Remove the destination from current destination/connection method
     * structures. This is a lot faster than calling Refresh().
     */
    void RemoveDestination( const TUint32& aDestinationId );

    /**
     * Remove the connection method from all destinations in the current
     * destination/connection method structures. This is a lot faster than
     * calling Refresh(). The ID of any changed destination is added to the
     * aChangedDestinations-array.
     */
    void RemoveConnMethodFromDestinations(
            const TUint32& aConnMethodId,
            RArray<TUint32>& aChangedDestinations );
    
    /**
     * Get the metadata of the destination identified with given id.
     */
    TUint32 DestinationMetadata( const TUint32 aDestinationId ) const;

private:
    /**
     * Reads all connection methods from database.
     */
    void ReadAndValidateConnMethodsL();

    /**
     * Reads all destinations and the connection methods inside them.
     */
    void ReadAndValidateDestinationsL();

    /**
     * Returns the reference to shared CommsDat session handle.
     */
    CommsDat::CMDBSession& Session() const;

private:
    // The database cache object.
    CCmmCache& iCache;

    // All supported connection methods. Does not include any embedded destinations.
    RArray<TCmmConnMethodItem> iConnMethodItemArray;

    // IDs for all unsupported connection methods.
    RArray<TUint32> iUnsupportedConnMethods;

    // All destinations.
    RPointerArray<CDestination> iDestinations;

    // Connection methods that are deleted, but not removed from database yet
    // because some handles are still open to them and ID needs to be kept
    // reserved. Kept in unsigned integer order.
    RArray<TUint> iDeletedConnMethods;

    // Destinations that are deleted, but not removed from database yet because
    // some handles are still open to them and ID needs to be kept reserved.
    // Kept in unsigned integer order.
    RArray<TUint> iDeletedDestinations;

    // EasyWLAN IAP ID, KMaxTUint32 if not known.
    TUint32 iEasyWlanId;
    };

#endif // CMMINSTANCEMAPPING_H_

// End of file
