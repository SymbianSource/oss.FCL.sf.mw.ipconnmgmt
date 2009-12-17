/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Updates P&S variables: KPSUidGprsStatus and KPSUidWcdmaStatus.
*
*/

#ifndef SAEOBSERVER_H
#define SAEOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <rconnmon.h>

// DEFINITIONS
namespace
{
    enum TSAEConnectionType
    {
    ESAEUnknownConnectionType,
    ESAEGPRSConnectionType,
    ESAEWCDMAConnectionType
    };
}

// FORWARD DECLARATIONS

// CLASS DECLARATION

NONSHARABLE_CLASS( CSaeObserver ) : public CBase
{
public:

    /**
    * Two-phased constructor.
    */
    static CSaeObserver* NewL( const RConnectionMonitor& aMonitor );

    /**
    * Destructor.
    */
    virtual ~CSaeObserver();

    /**
    * Events from Connection Monitor are handled in this function.
    *
    * @param aConnMonEvent The event information.
    * @return void
    */
    void EventL( const CConnMonEventBase &aConnMonEvent );

private:

    /**
    * C++ default constructor
    */
    CSaeObserver( const RConnectionMonitor& aMonitor );

    /**
    * Second phase constructor.
    */
    void ConstructL();

    /**
    * Handles CConnMonDeleteConnection-events.
    *
    * @param aEvent the event to be handled
    * @return void
    */
    void HandleEConnMonDeleteConnectionL( const CConnMonDeleteConnection& aEvent );

    /**
    * Handles CConnMonNetworkStatusChange-events.
    *
    * @param aEvent the event to be handled
    * @return void
    */
    void HandleEConnMonNetworkStatusChangeL( const CConnMonNetworkStatusChange& aEvent );

    /**
    * Handles CConnMonConnectionStatusChange-events.
    *
    * @param aEvent the event to be handled
    * @return void
    */
    void HandleEConnMonConnectionStatusChangeL( const CConnMonConnectionStatusChange& aEvent );

    /**
    * Handles CConnMonBearerChange-events.
    *
    * @param aEvent the event to be handled
    * @return void
    */
    void HandleEConnMonBearerChangeL( const CConnMonBearerChange& aEvent );

    /**
    * Updates P&S variables
    *
    * @param aVariable PS variable uid
    * @param aState new state to be written to P&S
    * @return void
    */
    void NotifyPubSub( const TUid aVariable, const TInt aState );

    /**
    * Helper function that checks if the connection is of supported
    * connection type.
    *
    * @param  aConnectionId the connection ID value
    * @return type of the connection
    */
    TSAEConnectionType GetConnectionType( const TUint &aConnId );

    /**
    * Helper function that checks if the connection ID is already in some
    * connection type -specific member array. This way we can implicitly figure
    * out the connection type of the event.
    *
    * @param  aConnectionId connection ID
    * @param  aConnType type of the connection
    * @return void
    */
    void IdentifyTypeFromArrays( TUint aConnectionId,
                                 TSAEConnectionType& aConnType );

    /**
    * Return the connection array specific to connections of passed type.
    *
    * @param aConnType Reference to connection type. The corresponding array
    *                  is identified primarily based on this parameter.
    * @param aConnId   Connection ID is used in the identification of the array
    *                  only when type is ESAEUnknownConnectionType.
    * @return A pointer to corresponding connection array.
    */
    RArray<TUint>* MatchingConnArray( const TSAEConnectionType& aConnType,
                                      TUint aConnId = 0 );

    /**
    * Update current context activity to System Agent according
    * to connections stored in the member array:
    *      1 connection:     KUid*Status == ESA*Active
    *     >1 connections:    KUid*Status == ESA*MultipleActive
    *
    * @param  aConnType reference to connection type.
    * @return void
    */
    void UpdateContextActivity( const TSAEConnectionType& aConnType );

    /**
    * If not allready defined, add connection ID to the member array.
    *
    * @param  aConnType Reference to connection type.
    * @param  aConnId   Connection ID.
    * @return void
    */
    void AddToConnArray( const TSAEConnectionType& aConnType,
                         const TUint aConnId );

    /**
    * If found, remove connection ID from the member array.
    *
    * @param  aConnType  Reference to connection type.
    * @param  aConnId    Connection ID.
    * @return KErrNone if ID removed from array. Otherwise KErrNotFound.
    */
    TInt RemoveFromConnArray( const TSAEConnectionType& aConnType,
                              const TUint aConnId );

    /**
    * Removes all connection ID:s from the member array.
    *
    * @param  aConnType connection type (ie. bearer).
    * @return void
    */
    void RemoveAllFromConnArray( const TSAEConnectionType& aConnType );

    /**
    * Helper function that is used when connection count in some bearer
    * specific array reduces to zero,
    * Checks the state of the network and updates SA state variable accordingly.
    *
    * @param  aConnType connection type (ie. bearer).
    * @return void
    */
    void UpdateBearerNetworkStatusL( const TSAEConnectionType& connType );

private: // data

    // The Connection Monitor instance to be used
    RConnectionMonitor iMonitor;

    // Count of sub connections (contexts)
    RArray< TUint > iGPRSConnections;
    RArray< TUint > iWCDMAConnections;

    // State indicator; whether the network is UnAttached/unknown or not.
    // This is needed to sync the gprs and network states better.
    // Connection Monitor receives them from different sources, so
    // they do not necessarily correlate.
    TBool iNetworkIsUnAttached;

    // State indicator; whether the network is suspended or not.
    // This is needed to be able to return from suspend only on network
    // events, i.e. for instance EConnMonDeleteConnection does not mean
    // that the state of the network is not suspended anymore.
    TBool iNetworkIsSuspended;

    // State indicator of current bearer type. This is used during handling of
    // bearer changed notifications. We have to know from what bearer we are
    // moving to the new bearer type.
    TInt iCurrentBearerType;

    //Publish and Subscribe property for KPSUidGprsStatus and KPSUidWcdmaStatus
    RProperty iSAProperty;
};

#endif // SAEOBSERVER_H

// End of File
