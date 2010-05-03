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
* Handles client requests.
*
*/


#ifndef CMMSESSION_H_
#define CMMSESSION_H_

#include <e32base.h>

#include "cmmcache.h"

class CCmmServer;
class CCmmConnMethodInstance;
class CCmmDestinationInstance;


/**
 * A handler for the service requests received from clients.
 */
class CCmmSession : public CSession2
    {
public:
    /**
     * Creates a CVpnManagerSession object using two phase construction,
     * and returns a pointer to the created object
     *
     * @param Client's thread
     * @param The server
     *
     * @result Pointer to the new session
     */
    static CCmmSession* NewL( CCmmServer& aServer, CCmmCache& aCache );

    /**
     * Creates a CVpnManagerSession object using two phase construction,
     * and returns a pointer to the created object
     * (leaving a pointer to the object on the cleanup stack)
     *
     * @param Client's thread
     * @param The server
     *
     * @result Pointer to the new session
     */
    static CCmmSession* NewLC( CCmmServer& aServer, CCmmCache& aCache );

    /**
     * Destroy the object and release all memory objects.
     */
    ~CCmmSession();

public:
    /**
     * Service request from client.
     *
     * @param aMessage Message from client (containing requested operation and
     * any relevant data).
     */
    void ServiceL( const RMessage2& aMessage );

    /**
     * Finds a connection method instance that belongs to this session and
     * matches the provided ID. Return NULL if no match is found.
     */
    CCmmConnMethodInstance* FindConnMethodInstanceById(
            const TUint32& aConnMethodId );

    /**
     * Finds a destination instance that belongs to this session and matches
     * the provided handle.
     */
    CCmmDestinationInstance* FindDestinationInstanceByHandleL(
            const TInt& aDestinationHandle );

    /**
     * Finds a destination instance that belongs to this session and matches
     * the provided ID. Return NULL if no match is found.
     */
    CCmmDestinationInstance* FindDestinationInstanceById(
            const TUint32& aDestinationId );

    /**
     * Check from all open destination handles in this session if the given
     * connection method is inside any of them. The given destination is
     * skipped.
     */
    TBool ConnMethodInOtherDestination(
            const TUint32& aConnMethodId,
            const TUint32& aDestinationId );

    /**
     * Check for restrictions for adding an embedded destination from destination
     * instances of all clients.
     * @param aDestinationId The ID of the destination where a destination is
     * beeing embedded.
     * @param aEmbeddedDestinationId The ID of the destination that is beeing
     * embedded.
     */
    TBool EmbeddedDestinationConflictsFromAllSessions(
            const TUint32& aDestinationId,
            const TUint32& aEmbeddedDestinationId );

    /**
     * Check for restrictions for adding an embedded destination from destination
     * instances of this session.
     * @param aDestinationId The ID of the destination where a destination is
     * beeing embedded.
     * @param aEmbeddedDestinationId The ID of the destination that is beeing
     * embedded.
     */
    TBool EmbeddedDestinationConflicts(
            const TUint32& aDestinationId,
            const TUint32& aEmbeddedDestinationId );

    /**
     * After update to database, refresh temporary ID to real ID if necessary
     * and refresh status information for any related handles for all client
     * sessions.
     */
    void RefreshHandlesForAllSessions( const TCmmIdStruct& aIdStruct );

    /**
     * Removes a connection method from any open destination handle in this
     * session.
     */
    void RemoveConnMethodFromDestinationHandles(
            const TUint32& aConnMethodId );

    /**
     * Notify this sessions destination/connection method handles about an
     * updated/deleted destination/connection method.
     */
    void RefreshHandles( const TUint32& aId ) const;

private:
    /**
     * The first phase of two phase construction.
     *
     * @param Client's thread
     * @param The server
     */
    CCmmSession( CCmmServer& aServer, CCmmCache& aCache );

    /**
     * Destination specific service dispatcher.
     * @param aMessage Message sent by the plugin
     */
    void ServiceDestinationL( const RMessage2& aMessage );

    /**
     * Connection Method specific service dispatcher.
     * @param aMessage Message sent by the plugin
     */
    void ServiceConnMethodL( const RMessage2& aMessage );

    /**
     * The second phase of two phase construction.
     */
    void ConstructL() ;

    void GetBearerInfoIntL( const RMessage2& aMessage );
    void GetBearerInfoBoolL( const RMessage2& aMessage );
    void GetBearerInfoStringL( const RMessage2& aMessage );
    void GetBearerInfoString8L( const RMessage2& aMessage );
    void GetConnMethodInfoIntL( const RMessage2& aMessage );
    void GetConnMethodInfoBoolL( const RMessage2& aMessage );
    void GetConnMethodInfoStringL( const RMessage2& aMessage );
    void GetConnMethodInfoString8L( const RMessage2& aMessage );
    void GetConnMethodArrayL( const RMessage2& aMessage );
    void GetAllDestinationsL( const RMessage2& aMessage );
    void GetEasyWLANIdL( const RMessage2& aMessage );
    void GetSupportedBearersL( const RMessage2& aMessage );
    void ReadDefaultConnectionL( const RMessage2& aMessage );
    void ReadGenConnSettingsL( const RMessage2& aMessage );
    void WriteGenConnSettingsL( const RMessage2& aMessage );
    void GetBearerPriorityArrayL( const RMessage2& aMessage );
    void UpdateBearerPriorityArrayL( const RMessage2& aMessage );
    void CopyConnMethodL( const RMessage2& aMessage );
    void MoveConnMethodL( const RMessage2& aMessage );
    void RemoveConnMethodL( const RMessage2& aMessage );

    void RemoveAllReferencesL( const RMessage2& aMessage );

    void GetDestinationL( const RMessage2& aMessage );
    void RefreshDestinationL( const RMessage2& aMessage );
    void CreateDestinationWithNameL( const RMessage2& aMessage );
    void CreateDestinationWithNameAndIdL( const RMessage2& aMessage );
    void CloseDestination( const RMessage2& aMessage );
    void GetConnectionMehodCountL( const RMessage2& aMessage );
    void GetConnMethodPriorityL( const RMessage2& aMessage );
    void GetDestinationNameL( const RMessage2& aMessage );
    void GetDestinationIdL( const RMessage2& aMessage );
    void GetDestinationElementIdL( const RMessage2& aMessage );
    void GetDestinationMetadataL( const RMessage2& aMessage );
    void GetDestinationProtectionLevelL( const RMessage2& aMessage );
    void IsDestinationConnectedL( const RMessage2& aMessage );
    void IsDestinationHiddenL( const RMessage2& aMessage );
    void DestinationIsEqualL( const RMessage2& aMessage );

    void DestAddConnMethodL( const RMessage2& aMessage );
    void DestAddEmbeddedDestinationL( const RMessage2& aMessage );
    void DestDeleteConnMethodL( const RMessage2& aMessage );
    void DestRemoveConnMethodL( const RMessage2& aMessage );

    void ModifyConnMethodPriorityL( const RMessage2& aMessage );
    void SetDestinationNameL( const RMessage2& aMessage );
    void SetDestinationMetadataL( const RMessage2& aMessage );
    void SetDestinationProtectionL( const RMessage2& aMessage );
    void SetDestinationHiddenL( const RMessage2& aMessage );
    void UpdateDestinationL( const RMessage2& aMessage );
    void DeleteDestinationL( const RMessage2& aMessage );

    void CreateConnMethodL( const RMessage2& aMessage );
    void UpdateConnMethodL( const RMessage2& aMessage );
    void CloseConnMethod( const RMessage2& aMessage );
    void DeleteConnMethodL( const RMessage2& aMessage );
    void GetConnMethodWithIdL( const RMessage2& aMessage );
    void RefreshConnMethodL( const RMessage2& aMessage );
    void GetIntAttributeL( const RMessage2& aMessage );
    void GetBoolAttributeL( const RMessage2& aMessage );
    void GetStringAttributeL( const RMessage2& aMessage );
    void GetString8AttributeL( const RMessage2& aMessage );
    void SetIntAttributeL( const RMessage2& aMessage );
    void SetBoolAttributeL( const RMessage2& aMessage );
    void SetStringAttributeL( const RMessage2& aMessage );
    void SetString8AttributeL( const RMessage2& aMessage );
    void CreateConnMethodWithIdL( const RMessage2& aMessage );
    void GetConnMethodFromDestWithIndexL( const RMessage2& aMessage );
    void GetConnMethodFromDestWithIdL( const RMessage2& aMessage );
    void CreateConnMethodToDestL( const RMessage2& aMessage );
    void CreateConnMethodToDestWithIdL( const RMessage2& aMessage );
    void CmIsEqualL( const RMessage2& aMessage );
    void CreateCopyOfExistingL( const RMessage2& aMessage );
    void GetEmbeddedDestinationL( const RMessage2& aMessage );

private:
    RMessage2 iMessage;

    CCmmServer& iServer;

    // Object container for this session (our handles)
    CObjectCon* iDestinationContainer;

    // Object index which stores objects for this session
    CObjectIx* iDestinationObjects;

    // Object container for this session (our handles)
    CObjectCon* iConnMethodContainer;

    // Object index which stores objects for this session
    CObjectIx* iConnMethodObjects;

    CCmmCache& iCache; // Not owned
    };

#endif // CMMSESSION_H_

// End of file
