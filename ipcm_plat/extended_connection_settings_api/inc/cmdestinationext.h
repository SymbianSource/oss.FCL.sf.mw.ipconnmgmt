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
* Destination extended interface class.
*
*/


#ifndef CMDESTINATIONEXT_H
#define CMDESTINATIONEXT_H

// Deprecation warning
#warning This header file is deprecated and shall not be used anymore. Please use cmdestination.h instead. See Polonium CR #461 for details.

#include <e32base.h>
#include <metadatabase.h>
#include <cmmanagerdef.h>

// Forward declarations
class RCmConnectionMethodExt;
class CCmDestinationWrapper;


/**
 *  RCmDestination is for getting/setting values of a network destination.
 *  @lib cmmanager.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RCmDestinationExt )
    {
    public:
        /** Default constructor. */
        IMPORT_C RCmDestinationExt();

    public:
        /**
        * Close the session.
        */
        IMPORT_C void Close();

        IMPORT_C ~RCmDestinationExt();

        IMPORT_C RCmDestinationExt( const RCmDestinationExt& aDestination );

        /**
        * Return the number of connection methods in the destination
        * @param None
        * @return number of the connection method in this destination
        */
        IMPORT_C TInt ConnectionMethodCount();

        /**
        * Return a reference to the connection method.
        * The index must be less than the return value of
        * ConnectionMethodCount()
        * @param anIndex index of the connection method to be returned
        * @return connection method
        */
        IMPORT_C RCmConnectionMethodExt ConnectionMethodL( TInt aIndex );

        /**
        * Returns the connection method with the ECmId.
        * Leaves with KErrNotFound if not found.
        * @param aCmId unique id of the requested connection method.
        * @return connection method
        */
        IMPORT_C RCmConnectionMethodExt ConnectionMethodByIDL(
                TInt aConnMethodId );

        /**
        * Return the priority of the passed connection method
        * @param aCCMItem connection method
        * @return the priority of the queried connection method
        */
        IMPORT_C TUint PriorityL(
                const RCmConnectionMethodExt& aConnectionMethod );

        /**
        * Return the name of the destination. Ownership is passed.
        * HBuf ownership is passed to the caller
        * @return name of the destination.
        */
        IMPORT_C HBufC* NameLC();

        /**
        * Return the destination's Id attribute
        * @return id of the destination
        */
        IMPORT_C TUint32 Id();

        /**
        * Return the destination's ElementId attribute
        * @return TUint32
        */
        IMPORT_C TUint32 ElementId();

        /**
        * Return the Metadata
        * @return TUint32
        */
        IMPORT_C TUint32 MetadataL(
                CMManager::TSnapMetadataField aMetadataField ) const;

        /**
        * Return the protection level of the destination.
        * @return protection level
        */
        IMPORT_C CMManager::TProtectionLevel ProtectionLevel();

        /**
        * Return that the destination is hidden or not.
        * @return ETrue if the destination is hidden
        */
        IMPORT_C TBool IsHidden();

        /**
        * Create a connection method that belongs to this destination.
        * @param aImplementationUid - implementation uid (bearer type) of the
        * connection method to be created
        * @return newly created connection method
        */
        IMPORT_C RCmConnectionMethodExt CreateConnectionMethodL(
                TUint32 aBearerType );

        /*
        * Adds an existing connection method to a destination
        * @param aConnectionMethod Connection method to be added
        * @return index in the Connection Method list
        */
        IMPORT_C TInt AddConnectionMethodL(
                RCmConnectionMethodExt aConnectionMethod );

        /*
        * Embeds an existing destination into this destination.
        * @param RCmDestinationExt - Destination to be embedded
        * @return TInt - index in the Connection Method list
        */
        IMPORT_C TInt AddEmbeddedDestinationL(
                const RCmDestinationExt& aDestination );

        /**
        * Remove a connection method from a destination and delete it
        * on update.
        * Exception: conenction method is not deleted  if it's referenced
        * from any other destination.
        * @param aConnectionMethod the connection method to be deleted.
        */
        IMPORT_C void DeleteConnectionMethodL(
                RCmConnectionMethodExt& aConnectionMethod );

        /**
        * Remove connection method from the destination
        * @param aConnectionMethod connection method to be removed
        */
        IMPORT_C void RemoveConnectionMethodL(
                RCmConnectionMethodExt aConnectionMethod );

        /**
        * Set the connection method's priority based on the passed
        * index. Indexing starts from zero(the highest priority).
        * @param aCCMItem the connection method item
        * @param aIndex the new priority of the connection method in the
        * destination
        */
        IMPORT_C void ModifyPriorityL(
                RCmConnectionMethodExt& aConnectionMethod,
                TUint aIndex );

        /**
        * Set the destination's name
        * @param aDestinationName new name of the destination
        */
        IMPORT_C void SetNameL( const TDesC& aName );

        /**
        * Set the Metadata
        * @param aMetaField field to be modified
        * @param aValue value to be set
        */
        IMPORT_C void SetMetadataL(
                CMManager::TSnapMetadataField aMetadataField,
                TUint32 aValue );

        /**
        * Set the protection level of the destination.
        * @param aProtLevel new protection level
        */
        IMPORT_C void SetProtectionL( CMManager::TProtectionLevel aProtLevel );

        /**
        * Set if the destination is hidden or not.
        * @param aHidden - ETrue if destination is hidden
        */
        IMPORT_C void SetHiddenL( TBool aHidden );

        /**
        * Update all values of the destination and its connection method.
        * Nothing if stored, if update leaves due to any reason.
        */
        IMPORT_C void UpdateL();

        /**
        * Delete destination and its connection methods from CommsDat.
        * Connection methods that belong to other destination, too, are
        * not deleted.
        */
        IMPORT_C void DeleteLD();

        /**
        * Returns if there's connection created with any of the destination's
        * connection method.
        * @return ETrue if there's connection with any of the destination's
        * connection method.
        */
        IMPORT_C TBool IsConnectedL() const;

        /**
         * Compares whether two objects content is the same.
         * @return ETrue if the content is same.
         */
        IMPORT_C TBool operator==(
                const RCmDestinationExt& aDestination ) const;

        /**
         * Compares whether two objects content is different.
         * @return ETrue if the content is different.
         */
        IMPORT_C TBool operator!=(
                const RCmDestinationExt& aDestination ) const;

        /**
        * assignment operator
        *
        * @since S60 3.2
        * @param aDestination the destination being compared
        * @return RCmDestination
        */
        IMPORT_C RCmDestinationExt& operator=(
                const RCmDestinationExt& aDestination );

        /**
        * Create a connection method with given id that belongs to this
        * destination.
        * @param aImplementationUid - implementation uid (bearer type) of the
        * connection method to be created
        * @param aConnMethdId Predefined id for the connection method
        * (id range is from 1 to max IAP record amount in CommsDat).
        * @return newly created connection method or leaves with
        * KErrAlreadyExists if there exists CM with the same Id.
        */
        IMPORT_C RCmConnectionMethodExt CreateConnectionMethodL(
                TUint32 aBearerType,
                TUint32 aConnMethodId );

    private:
        friend class RCmManagerExt;
        friend class RCmConnectionMethodExt;

    private:
        CCmDestinationWrapper* iCmDestinationWrapper;
    };

#endif // CMDESTINATIONEXT_H

// End of file
