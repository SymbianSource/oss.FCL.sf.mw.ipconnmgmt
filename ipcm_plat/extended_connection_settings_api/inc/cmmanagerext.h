/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Connection manager IF class.
*
*/


#ifndef CMMANAGEREXT_H
#define CMMANAGEREXT_H

// Deprecation warning
#warning This header file is deprecated and shall not be used anymore. Please use cmmanager.h instead. See Polonium CR #461 for details.

#include <e32base.h>
#include <metadatabase.h>
#include <cmmanagerdef.h>
#include <cmconnectionmethodext.h>
#include <cmdefconnvalues.h>

// Forward declarations
class RCmDestinationExt;
class RCmManagerApi;


/**
 *  RCmManager is used to query and modify network destinations and connection
 *  methods.
 *
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RCmManagerExt )
    {
    public:
        /** Inline constructor */
        inline RCmManagerExt();

    public:
        /** Open session */
        IMPORT_C void OpenL();

        /** Open session and create tables if they don't exist */
        IMPORT_C void CreateTablesAndOpenL();

        /**
        * Symbian constructor
        * Pushes the object on the cleanup stack
        *
        * @since S60 3.2
        */
        IMPORT_C void OpenLC();

        /** Close session */
        IMPORT_C void Close();

        /**
        * GetBearerInfoXXX function can be used to query bearer informantion
        * that doesn't belong to a specific connection method, such as
        * ECmCoverage or ECmDefaultPriority etc.
        * NULL pointer, returned from string functions, means attribute exists
        * but has no value.
        * HBuf ownership is passed to the caller.
        */
        IMPORT_C TUint32 GetBearerInfoIntL(
                TUint32 aBearerType,
                TUint32 aAttribute ) const;
        IMPORT_C TBool GetBearerInfoBoolL(
                TUint32 aBearerType,
                TUint32 aAttribute ) const;
        IMPORT_C HBufC* GetBearerInfoStringL(
                TUint32 aBearerType,
                TUint32 aAttribute ) const;
        IMPORT_C HBufC8* GetBearerInfoString8L(
                TUint32 aBearerType,
                TUint32 aAttribute ) const;

        /**
        * GetConnectionMethodInfoXXX function can be used to query any,
        * non-bearer specific information about a given connection method. This
        * can be e.g. ECmBearerType, ECmName, ECmStartPage etc.
        * NULL pointer, returned from string functions, means attribute exists
        * but has no value.
        * HBuf ownership is passed to the caller
        */
        IMPORT_C TUint32 GetConnectionMethodInfoIntL(
                TUint32 aIapId,
                TUint32 aAttribute ) const;
        IMPORT_C TBool GetConnectionMethodInfoBoolL(
                TUint32 aIapId,
                TUint32 aAttribute ) const;
        IMPORT_C HBufC* GetConnectionMethodInfoStringL(
                TUint32 aIapId,
                TUint32 aAttribute ) const;
        IMPORT_C HBufC8* GetConnectionMethodInfoString8L(
                TUint32 aIapId,
                TUint32 aAttribute ) const;

        /**
        * Creates a Destination with the passed name.
        * @param aName the name of the new destination.
        * @return RCmDestinationExt new destination. ownership is passed to the
        * caller
        */
        IMPORT_C RCmDestinationExt CreateDestinationL( const TDesC& aName );

        /**
        * Creates a connection method does not belong to any destination
        * @param aImplementationUid - bearer type of the new connection method
        * @return RCmConnectionMethodExt newly created connection method.
        */
        IMPORT_C RCmConnectionMethodExt CreateConnectionMethodL(
                TUint32 aBearerType );

        /**
        * Copies a connection method belonging to one destination to another.
        * The connection method becomes shared. Calls UpdateL on the
        * destination.
        * @param aTargetDestination
        * @param aConnectionMethodId
        * @return TInt - index in the Connection Method list
        */
        IMPORT_C TInt CopyConnectionMethodL(
                RCmDestinationExt& aTargetDestination,
                RCmConnectionMethodExt& aConnectionMethod );

        /**
        * Moves a connection method belonging to one destination to another. The
        * connection method is removed from the source destination.
        * @param aSourceDestination
        * @param aTargetDestination
        * @param aConnectionMethod
        * @return TInt - index in the Connection Method list
        */
        IMPORT_C TInt MoveConnectionMethodL(
                RCmDestinationExt& aSourceDestination,
                RCmDestinationExt& aTargetDestination,
                RCmConnectionMethodExt& aConnectionMethod );

        /**
        * Remove connection method from the destination
        * @param aDestination destination the connection method is attached to
        * @param aConnectionMethod connection method to be removed
        */
        IMPORT_C void RemoveConnectionMethodL(
                RCmDestinationExt& aDestination,
                RCmConnectionMethodExt& aConnectionMethod );

        /**
        * Removes connection method from every destination and make it
        * uncategorized.
        * @param aConnectionMethod connection method to be removed
        */
        IMPORT_C void RemoveAllReferencesL(
                RCmConnectionMethodExt& aConnectionMethod );

    public:
        /**
        * Returns the connection method queried by its ID.
        * @param aId if of the connection method
        * @return RCmConnectionMethodExt connection method instance
        */
        IMPORT_C RCmConnectionMethodExt ConnectionMethodL(
                TUint32 aConnectionMethodId );

        /**
        * Returns the list of connection methods that do not belong to any
        * destination.
        * @param aCMArray on return it is the array of IAP ids.
        * @param aCheckBearerType If ETrue only connection methods with
        * supported bearer types are returned.
        * @param aLegacyOnly when set to ETrue only Legacy IAP ids will be
        * returned.
        * @param aEasyWlan when set to ETrue EasyWlan id is returned, too.
        */
        IMPORT_C void ConnectionMethodL(
                RArray<TUint32>& aCmArray,
                TBool aCheckBearerType = ETrue,
                TBool aLegacyOnly = ETrue,
                TBool aEasyWlan = EFalse );

        /**
        * Returns the destination to which a specified connection method
        * belongs, search is by destination ID.
        * @param aDestinationId id of the destination
        * @return RCmDestinationExt network destination
        */
        IMPORT_C RCmDestinationExt DestinationL( TUint32 aDestinationId );

        /**
        * Returns an array of all destination id's
        * @param aDestArray array of all available network destination
        */
        IMPORT_C void AllDestinationsL( RArray<TUint32>& aDestArray );

        /**
        * Returns the copy of the bearer priority array. Ownership is passed.
        * @param aArray bearer priority array
        */
        IMPORT_C void BearerPriorityArrayL(
                RArray<TBearerPriority>& aArray ) const;

        /**
        * Update bearer priority array
        * @param aArray array with new global bearer priority
        */
        IMPORT_C void UpdateBearerPriorityArrayL(
                const RArray<TBearerPriority>& aArray );

        /**
        * Clean up passed global bearer priority array. Delete
        * the elements and calls Reset and Close on the array.
        * @param aArray array to be cleaned up
        */
        IMPORT_C void CleanupGlobalPriorityArray(
                RArray<TBearerPriority>& aArray ) const;

        /**
        * Query all of the supported bearer types.
        * @param aArray list of all supported bearer types.
        */
        IMPORT_C void SupportedBearersL( RArray<TUint32>& aArray ) const;

        /**
        * Returns the id of the EasyWlan connection method.
        *
        * @since S60 3.2
        * @return id of the EasyWLan connection method. 0 if not found.
        */
        IMPORT_C TUint32 EasyWlanIdL();

        /**
        * Returns the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        IMPORT_C void ReadDefConnL( TCmDefConnValue& aDefConnSetting );

        /**
        * Stores the default connection method/SNAP.
        *
        * @since S60 3.2
        */
        IMPORT_C void WriteDefConnL( const TCmDefConnValue& aDefConnSetting );

        /**
        * Creates a Destination with the passed name and id.
        * @param aName the name of the new destination.
        * @param aDestId Predefined id of the destination.
        * @return RCmDestinationExt newly created destination or leaves with
        * KErrAlreadyExists if there exists CM with the same Id.
        * Ownership is passed to the caller.
        */
        IMPORT_C RCmDestinationExt CreateDestinationL(
                const TDesC& aName,
                TUint32 aDestId );

        /**
        * Creates a connection method does not belong to any destination
        * @param aImplementationUid - bearer type of the new connection
        * method
        * @param aConnMethodId Predefined id for the connection method
        * (id range is from 1 to max IAP record amount in CommsDat).
        * @return RCmConnectionMethodExt newly created connection
        * method or leaves with KErrAlreadyExists if there exists CM
        * with the same Id.
        */
        IMPORT_C RCmConnectionMethodExt CreateConnectionMethodL(
                TUint32 aBearerType,
                TUint32 aConnMethodId );

    private:
        /**
        * This is a private copy constructor without iplemetation.
        * The sole purpose of this declaration is to forbid the copying of
        * the objects of this class.
        */
        RCmManagerExt( RCmManagerExt& );

        /**
        * This is a private copy constructor without iplemetation.
        * The sole purpose of this declaration is to forbid the copying of
        * the objects of this class.
        */
        RCmManagerExt& operator=( RCmManagerExt& );

    private:
        friend class CCmManagerImpl;

    private:
        RCmManagerApi* iCmManagerApi;
    };


inline RCmManagerExt::RCmManagerExt() : iCmManagerApi( NULL )
    {
    }


#endif // CMMANAGEREXT_H

// End of file
