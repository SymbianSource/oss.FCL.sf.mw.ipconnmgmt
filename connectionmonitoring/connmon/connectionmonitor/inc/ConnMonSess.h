/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Connection Monitor session.
*
*/

#ifndef CONNMONSESS_H
#define CONNMONSESS_H

class CConnMonServer;
class CPlugin;
class TEvent;
class TEventInfo;

/**
* TConnSettings
* Record for connection level threshold settings.
*/
class TConnSettings
    {
public:
    TConnSettings(
            TUint aActivityTimeThreshold,
            TUint aDLDataThreshold,
            TUint aULDataThreshold );

public: // Data
    // Activity time threshold (>= 5)
    TUint iActivityTimeThreshold;

    // Downlink data volume threshold (>= 4096)
    TUint iDLDataThreshold;

    // Uplink data volume threshold (>= 4096)
    TUint iULDataThreshold;
    };

/**
* TConnRequest
* Record for a pending asyncronous requst
* (KUplinkData, KDownlinkData, KConnectionActive).
* This request has not yet been completed in the server side.
*/
class TConnRequest
    {
public:
    TConnRequest(
            TUint8 aType,
            const RMessage2 aMessage );

public: // Data
    // Request type
    TUint8 iType;

    // Request
    RMessage2 iMessage;
    };

/**
* TConnParams
* A collection record for dynamic connection parameters
* (thresholds, pending async. requests)
*/
class TConnParams
    {
public:
    // Constructors and destructor
    TConnParams( TUint aConnectionId );

public:
    /**
     * Compares two TConnParams by the connection id.
     * @param aFirst The first TConnParams record.
     * @param aSecond The second TConnParams record.
     * @return ETrue if equal, EFalse if not.
     */
    static TBool MatchId(
            const TConnParams& aFirst,
            const TConnParams& aSecond );

public: // Data
    // Connection id
    TUint iConnectionId;

    // Threshold settings
    TConnSettings iSettings;

    // Pending async. requests (KUplinkData, KDownlinkData, KConnectionActive)
    RArray<TConnRequest> iRequests;
    };

/**
* CConnMonSession
* Server side session.
*/
NONSHARABLE_CLASS( CConnMonSession ) : public CSession2
    {
public:
    CConnMonSession( CConnMonServer* aServer );
    void ConstructL();
    ~CConnMonSession();

public:
    /**
     * Gets a pointer to the server module.
     * @return A pointer to the server module.
     */
    inline CConnMonServer* CmServer() { return iCmServer; }

    /**
     * Tells if the client is listening to the events.
     * @return ETrue if session is listening, EFalse if not.
     */
    TBool IsListening() const;

    /**
     * Tells if the client can receive the event.
     * @return ETrue if client has receive active, EFalse if not.
     */
    TBool CanReceiveEvent() const;

    /**
     * Sends an event to the client.
     * @param aEvent The event.
     * @return ETrue if client accepted the event, EFalse if not.
     */
    TBool SendEventToClient( const TEvent& aEvent );

    /**
     * Sends data volumes to the client if client has any pending requests.
     * @param aConnectionId Connection id.
     * @param aDlVolume Downlink data volume.
     * @param aUlVolume Uplink data volume.
     * @param aRequestStatus Status to be sent to the client.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt CompleteDataVolumeRequests(
            const TUint& aConnectionId,
            const TUint& aDlVolume,
            const TUint& aUlVolume,
            const TInt& aRequestStatus );

    /**
     * Sends activity information to the client if client has any pending requests.
     * @param aConnectionId Connection id.
     * @param aActivity Activity information.
     * @param aRequestStatus Status to be sent to the client.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt CompleteActivityRequests(
            const TUint& aConnectionId,
            const TBool& aActivity,
            const TInt& aRequestStatus );

    /**
     * Sets connection level thresholds.
     * @param aConnId, The connection id.
     * @param aSettings The thresholds record.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetConnectionSettings(
            const TUint aConnId,
            const TConnSettings& aSettings );

    /**
     * Gets connection level thresholds.
     * @param aConnId, The connection id.
     * @param aSettings On return, contains the thresholds.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetConnectionSettings( const TUint aConnId, TConnSettings& aSettings );

    /**
     * Saves parameters of a pending request.
     * @param aConnId, The connection id.
     * @param aConnRequest The request.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetConnectionRequest(
            const TUint aConnId,
            const TConnRequest& aRequest );

    /**
     * Removes the connection parameters.
     * @param aConnId, The connection id.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt RemoveConnectionParams( const TUint aConnId );

    /**
     * Gets a bearer specific threshold.
     * These are ON/OFF type e.g signal strength and bearer availability.
     * @param aThresholdType, The type of the threshold.
     * @param aThreshold, On return, contains the value of the threshold.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetBearerThreshold(
            const TInt& aThresholdType,
            TUint& aThreshold ) const;

    /**
     * Plugin specific service dispatcher.
     * @param aMessage Message sent by the plugin
     */
    void ServicePluginL( const RMessage2& aMessage );

    /**
     * Finds plug-in subsession object matching the handle.
     * @param aHandle, Plugin subsession handle.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    CPlugin* PluginFromHandle( const TUint aHandle );

    /**
     * Creates a new plug-in subsession object.
     * @param aConnection Generic bearer specific connection id that identifies the plug-in,
     */
    void NewPluginL( const TUint aConnectionId );

    /**
     * Deletes the plug-in subsession object.
     * @param aHandle, Plugin subsession handle.
     */
    void DeletePlugin( const TUint aHandle );

    /**
     * Sets/gets an attribute to/from a plugin.
     * @param aType The type of the request.
     * @param aMessage Message sent by the client
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SendRequestToPlugin( const TInt aType, const RMessage2& aMessage );

    /**
     * Cancels client's attribute request.
     * @param aType The type of the attribute.
     */
    void CancelAttributeRequestFromPlugin(
            const RThread& aClient,
            const TInt aType );

    /**
     * Sets a new threshold to the plugin.
     * @param aType The type of the threshold.
     * @param aValue The new value for the threshold.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetPluginThreshold( const TInt aType, const TUint aValue );

public:
    /**
     * From CSharableSession Service dispatcher.
     * @param aMessage Message sent by the client
     */
    void ServiceL( const RMessage2& aMessage );

private:
    /**
     * Completes the client message if status != pending or bad descriptor
     * @param aStatus Status from actual request
     */
    void CompleteMessage( const TInt aStatus );

    /**
     * Gets the number of active connections and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetConnectionCountL();

    /**
     * Gets the connection id and subconnection count and
     * writes them to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetConnectionInfo();

    /**
     * Gets the subconnection id and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetSubConnectionInfo() const;

    /**
     * Gets the TInt attribute and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetIntAttributeL();

    /**
     * Gets the TUint attribute and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetUintAttributeL();

    /**
     * Gets the TBool attribute and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetBoolAttributeL();

    /**
     * Gets the TDes attribute and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetStringAttributeL();

    /**
     * Gets the packaged attribute and
     * writes it to the client's address space.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt GetPckgAttributeL();

    /**
     * Sets the TInt attribute to the server.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetIntAttributeL();

    /**
     * Sets the TUint attribute to the server.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetUintAttributeL();

    /**
     * Sets the TBool attribute to the server (KConnectionStop & KConnectionStopAll).
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetBoolAttribute();

    /**
     * Sets the String attribute to the server.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt SetStringAttributeL();

    // Not implemented
    TInt SetPckgAttribute() const;

    /**
     * Cancels the asyncronous request(s) sent by the client.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt CancelAsyncRequest();

    /**
     * Sets 'receive request' active and checks the event queue for a new event.
     * @param aBuffer Client memory area where to an event can be written.
     */
    void ReceiveEventL( const TAny* aBuffer );

    /**
     * Cancels the event request.
     * @param aBuffer, Client memory area.
     */
    void CancelReceiveEvent();

    /**
     * Checks if connection id is still valid.
     * @param aConnId, The connection id.
     * @return ETrue if connection id is valid, EFalse if not valid.
     */
    TBool IsConnectionValid( const TUint aConnId );

    /**
     * Removes the obsolete connection parameters from iConnParams array.
     */
    void RemoveObsoleteConnectionParams();

    /**
     * Calculates the adjusted threshold for data volume events.
     * @param aEvent, Event information.
     * @param aAdjustedTh, On return, contains the adjusted threshold.
     * @param aSmallestTh, On return, contains the smallest threshold among the clients.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt CalculateAdjustedThreshold(
            const TEventInfo& aEvent,
            TUint& aAdjustedTh,
            TUint& aSmallestTh );

    /**
     * Adjusts a given threshold value to allowed range according to type.
     * @since MCL 9.1
     * @param aConnId The connection ID in case bearer needs to be discovered.
     * @param aThresholdType The type of the threshold.
     * @param aThreshold The threshold value. Will contain adjusted value on return.
     * @return KErrNone if successfull, otherwise a system wide error code.
     */
    TInt AdjustThresholdToAllowedRange(
            const TInt& aConnId,
            const TInt& aThresholdType,
            TUint& aThreshold ) const;

    /**
     * Returns a reference to the current client message.
     * @return A reference to the current client message.
     */
    RMessage2 Message() const { return iMessage2; }

private: // Data
    // Pointer to the server module
    CConnMonServer* iCmServer;

    // Enumerated connection ids
    CArrayFixFlat<TUint>* iEnumConnInfo;

    // Connection parameters (data volume and activity time thresholds)
    // and pending requests
    RArray<TConnParams> iConnParams;

    // True if client is listening to the events
    TBool iListening;

    // True if client has the event request active
    TBool iReceiveActive;

    // The event request message client has sent
    RMessage2 iReceiveMessage;

    // Pointer to the client memory area for the event
    const TAny* iReceiveBuffer;

    // Time of the latest event delivered to the client
    TTime iLastEventTime;

    // Object container for this session (our handles)
    CObjectCon* iContainer;

    // Object index which stores objects for this session
    CObjectIx* iPlugins;

    // Bearer availability event threshold (0=OFF or 1=ON)
    TUint iBearerAvailabilityThreshold;

    // Signal strength event threshold (0=OFF or 1=ON)
    TUint iSignalStrengthThreshold;

    // New bearer system event threshold (0=OFF or 1=ON)
    TUint iBearerGroupThreshold;

    // Reference to the current client message
    RMessage2 iMessage2;

private: // Friend classes
        friend class CPlugin;
    };

#endif // CONNMONSESS_H

// End-of-file
