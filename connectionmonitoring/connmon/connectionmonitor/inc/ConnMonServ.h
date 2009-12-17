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
* Description:  Connection Monitor server.
*
*/

#ifndef CONNMONSERV_H
#define CONNMONSERV_H

#include <e32base.h>

_LIT( KConnMonDLL, "ConnMon.dll" );
_LIT( KConnMonStartupSemaphore, "ConnMonStartupSemaphore" );

// Security policy
const TUint KConnMonPolicyRangeCount = 11;

const TInt KConnMonPolicyRanges[KConnMonPolicyRangeCount] =
    {
    0,   //range is 0-2 inclusive
    3,   //range is 3-7 inclusive
    8,   //range is 8-12 inclusive
    13,  //range is 13
    14,  //range is 14
    15,  //range is 15
    16,  //range is 16-499 inclusive
    500, //range is 500-507 inclusive
    508, //range is 508-599 inclusive
    600, //range is 600-601 inclusive
    602  //range is 602-KMaxTInt inclusive
    };

const TUint8 KConnMonPolicyElementsIndex[KConnMonPolicyRangeCount] =
    {
    CPolicyServer::EAlwaysPass,     // Applies to  0th range (request number: 0-2)
                                            //EReqGetConnectionCount
                                            //EReqGetConnectionInfo
                                            //EReqGetSubConnectionInfo

    CPolicyServer::ECustomCheck,    // Applies to  1st range (request number: 3-7)
                                            //EReqGetIntAttribute
                                            //EReqGetUintAttribute
                                            //EReqGetBoolAttribute
                                            //EReqGetStringAttribute
                                            //EReqGetPckgAttribute

    CPolicyServer::ECustomCheck,    // Applies to  2nd range (request number: 8-12)
                                            //EReqSetIntAttribute
                                            //EReqSetUintAttribute
                                            //EReqSetBoolAttribute
                                            //EReqSetStringAttribute
                                            //EReqSetPckgAttribute

    CPolicyServer::EAlwaysPass,     // Applies to  3rd range (request number: 13)
                                            //EReqCancelAsyncRequest

    CPolicyServer::EAlwaysPass,     // Applies to  4th range (request number: 14)
                                            //EReqReceiveEvent

    CPolicyServer::EAlwaysPass,     // Applies to  5th range (request number: 15)
                                            //EReqCancelReceiveEvent

    CPolicyServer::ENotSupported,   // Applies to  6th range (request number: 16-499)

    4,                              // Applies to  7th range (request number: 500-507)
                                            //EReqPluginCreateSubSession
                                            //EReqPluginCloseSubSession
                                            //EReqPluginRegisterAttribute
                                            //EReqPluginCancelRegisterAttribute
                                            //EReqPluginEvent
                                            //EReqPluginGetQuery
                                            //EReqPluginCancelGetQuery
                                            //EReqPluginAttribute

    CPolicyServer::ENotSupported,    // Applies to  8th range (request number: 508-599)

    CPolicyServer::ENotSupported,    // Applies to  9th range (request number: 600-601)
                                     // Used internally in communicating with plugin
                                            //EReqInternalSetThreshold
                                            //EReqInternalResetThreshold

    CPolicyServer::ENotSupported     // Applies to 10th range (request number: 602-KMaxTInt)
    };

const CPolicyServer::TPolicyElement KConnMonPolicyElements[] =
    {
    { _INIT_SECURITY_POLICY_C1( ECapabilityNetworkServices ), CPolicyServer::EFailClient },

    { _INIT_SECURITY_POLICY_C1( ECapabilityNetworkControl ), CPolicyServer::EFailClient },

    { _INIT_SECURITY_POLICY_C1( ECapabilityReadDeviceData ), CPolicyServer::EFailClient },

    { _INIT_SECURITY_POLICY_C1( ECapabilityReadUserData ), CPolicyServer::EFailClient },

    { _INIT_SECURITY_POLICY_C2( ECapabilityNetworkServices,
                                ECapabilityNetworkControl ), CPolicyServer::EFailClient }
    };

const CPolicyServer::TPolicy KConnMonPolicy =
    {
    CPolicyServer::EAlwaysPass, // Specifies all connect attempts should pass
    KConnMonPolicyRangeCount,
    KConnMonPolicyRanges,
    KConnMonPolicyElementsIndex,
    KConnMonPolicyElements,
    };

// Function prototypes
void PanicServer( TInt aPanic );

// Forward declarations
class CConnMonScheduler;
class CConnMonDelayedShutdown;
class CConnMonIAP;
class CEventQueue;
class TConnMonBearer;
class CConnMonCommsDatCache;
class CConnMonAvailabilityManager;
class CConnMonBearerGroupManager;
class CConnMonCommsDatNotifier;
class TEvent;
class TConnMonIapInfo;
class TConnMonSNAPInfo;
class TConnMonId;

/**
* CConnMonServer
*/
NONSHARABLE_CLASS( CConnMonServer ) : public CPolicyServer
    {
public:
    CConnMonServer();
    void ConstructL();
    ~CConnMonServer();

public:
    /**
     * Increments the session counter.
     */
    void IncrementSessions();

    /**
     * Decrements the session counter.
     * @param aEventInfo Event to be added.
     */
    void DecrementSessions();

    /**
     * Offers the event to all clients that are listening.
     * @param aEvent Event to be sent to the clients.
     * @param aNumOffered On return, the number of clients that actually
     *        have issued a receive reques.
     */
    void SendEventToSessions( const TEvent& aEvent, TInt& aNumOffered );

    /**
     * Offers the latest data volume information to all clients.
     * @param aConnectionId Connection Id.
     * @param aDlData Downlink data in bytes.
     * @param aUIData Uplink data in bytes.
     * @param aStatus Status received with data volumes from RConnection.
     */
    void SendDataVolumesToSessions(
            const TUint aConnectionId,
            const TUint aDlData,
            const TUint aUlData,
            const TInt aStatus );

    /**
     * Offers the latest activity information to all clients.
     * @param aConnectionId Connection Id.
     * @param aActivity Connection activity information.
     * @param aStatus Status received with activity from RConnection.
     */
    void SendActivityToSessions(
            const TUint aConnectionId,
            const TBool aActivity,
            const TInt aStatus );

    /**
     * Removes connection settings (thresholds) from sessions.
     * @param aConnectionId Connection Id.
     */
    void RemoveConnSettingsFromSessions( const TUint aConnectionId );

    /**
     * Gets the number of clients that are listening to the events.
     * @return Number of clients listening.
     */
    TInt NumberOfListeners();

    /**
     * Calculates the new (smallest) threshold
     * @param aConnectionId Connection Id.
     * @param aThresholdType Type of the threshold. See TConnMonThreshold.
     * @param aThreshold On return, the new threshold value.
     */
    void CalculateThreshold(
            const TUint aConnectionId,
            const TInt& aThresholdType,
            TUint& aThreshold );

    /**
     * Panics the client thread (because client gave a bad descriptor).
     * @param aPanic The panic code.
     */
    void PanicClient( TInt aPanic ) const;

    /**
     * Panics the client thread (because client gave a bad descriptor).
     * @param aMsg Reference to the client request message.
     * @param aPanic The panic code.
     */
    void PanicClient( const RMessage2& aMsg, const TInt aPanic ) const;

    /**
     * Scheduler calls Error() method when server's RunL() has a leave.
     * @param aError The error code.
     */
    void Error( TInt aError );

    /**
     * Custom security checking for IPCs marked with TSpecialCase::ECustomCheck.
     * @param aMsg The IPC message from the client.
     * @return The result of the security check (TCustomResult::EPass/TCustomResult::EFail).
     */
    TCustomResult CustomSecurityCheckL(
            const RMessage2& aMsg,
            TInt& aAction,
            TSecurityInfo& aMissing );

    /**
     * Gets a pointer to the iap module.
     * @return A pointer to the iap module.
     */
    inline CConnMonIAP* Iap() { return iIap; }

    /**
     * Get available IAP IDs for the IAPs with bearer aBearerId
     */
    TInt GetAvailableIaps( const TUint aBearerId, TConnMonIapInfo& aIapInfo );

    /**
     * Get available SANP IDs
     */
    void GetAvailableSnaps( TConnMonSNAPInfo& aSnapInfo );

    /**
     * Get available SANP IDs
     */
    TInt GetAvailableSnaps( RArray<TConnMonId>& aSnapIds );

    /**
     * Gets a pointer to event queue.
     * @return A pointer to the event queue.
     */
    inline CEventQueue* EventQueue() { return iEventQueue; }

    /**
     * Produces object containers (needed by subsessions) with unique ids within the server.
     * @return A object container.
     */
    CObjectCon* NewContainerL();

    /**
     * Removes from index and deletes the object container.
     * @param aContainer The container to be deleted.
     * @return A object container.
     */
    void RemoveContainer( CObjectCon* aContainer );

    /**
     * Tries to find plug-in supporting the attribute.
     * @param aType The type of the request.
     * @param aMessage The client message.
     * @return KErrRequestPending if plug-in was found, KErrNotSupported if not.
     */
    TInt SendRequestToPlugin(
            const TInt aType,
            const RMessage2& aMessage,
            const TBool aToAllPlugins = EFalse );

    /**
     * Cancels attribute requests issued by a client from plug-ins..
     * @param aType The type of the attribute (TInt, TUint, TBool...).
     * @return Void.
     */
    void CancelAttributeRequestsFromPlugins(
            const RThread& aClient,
            const TInt aType );

    /**
     * Gets Availability Manager.
     * @return Availability Manager.
     */
    inline CConnMonAvailabilityManager* AvailabilityManager()
        {
        return iAvailabilityManager;
        }

    /**
     * Gets bearers.
     * @return Bearers.
     */
    inline const RPointerArray<TConnMonBearer> Bearers()
        {
        return iBearers;
        }

    /**
     * Gets the bearer group manager.
     * @return Iaps availability manager.
     */
    inline CConnMonBearerGroupManager* BearerGroupManager()
        {
        return iBearerGroupManager;
        }

public:
    /**
     * From CServer Creates a new session for a client.
     * @param aVersion Version information.
     * @return A pointer to the new session.
     */
    CSession2* NewSessionL(
            const TVersion& aVersion,
            const RMessage2& aMessage ) const;

private: // Data
    // Pointer to the Iap module
    CConnMonIAP* iIap;

    // Pointer to the event queue
    CEventQueue* iEventQueue;

    // The number of the sessions (=clients)
    TUint8 iSessionCount;

    // Pointer to the shutdown timer object
    CConnMonDelayedShutdown* iShutdown;

    // Object container index
    CObjectConIx* iContainerIndex;

    // Array of bearers
    RPointerArray<TConnMonBearer> iBearers;

    // CommsDat cache
    CConnMonCommsDatCache* iCommsDatCache;

    // Availability manager
    CConnMonAvailabilityManager* iAvailabilityManager;

    // Notifiers to listen for specific CommsDat tables
    CConnMonCommsDatNotifier* iIapTableNotifier;
    CConnMonCommsDatNotifier* iSnapTableNotifier;
    CConnMonCommsDatNotifier* iVirtualTableNotifier;

    CConnMonBearerGroupManager* iBearerGroupManager;
    };

/**
* CConnMonScheduler
* Active scheduler for the server thread.
*/
NONSHARABLE_CLASS( CConnMonScheduler ) : public CActiveScheduler
    {
public:
    static void ConstructL();
    ~CConnMonScheduler();

public:

    // TSignal is needed to allow a client thread to pass TRequestStatus
    // in a way that it survives the thread launch or process launch
    class TSignal
        {
    public:
        inline TSignal();
        inline TInt Set( const TDesC& aData );
        inline TSignal( TRequestStatus& aStatus );
        inline TPtrC Get() const;
    public:
        TRequestStatus* iStatus;
        TThreadId iId;
        };

    /**
     * Launches the server.
     * @return KErrNone if OK, otherwise a system wide error code.
     */
    static TInt LaunchFromClient();

    /**
     * Starts the active scheduler.
     * @return KErrNone if OK, otherwise a system wide error code.
     */
    IMPORT_C static TInt ThreadStart( TSignal& aSignal );

public:
    /**
     * From CActiveScheduler Handles leave situations.
     * @param aError System wide error code.
     */
    void Error( TInt aError ) const;

private: // Data
    CConnMonServer* iServer;
    };

/**
* CConnMonDelayedShutdown
* Shutdown timer that closes Connection Monitor server after a period.
*/
NONSHARABLE_CLASS( CConnMonDelayedShutdown ) : public CActive
    {
public:
    CConnMonDelayedShutdown( CConnMonServer* aServer );
    void ConstructL();
    ~CConnMonDelayedShutdown();

public:
    /**
     * Starts shutdown timer.
     * @return KErrNone if OK, otherwise a system wide error code.
     */
    void Start();

private:
    /**
     * From CActive Cancels the shutdown timer.
     */
    void DoCancel();

    /**
     * From CActive Shuts the server down.
     */
    void RunL();

private: // Data
    RTimer iTimer;
    CConnMonServer* iServer;
    };

// INLINES
inline CConnMonScheduler::TSignal::TSignal()
        :
        iStatus( KErrNone ),
        iId()
    {
    }

inline CConnMonScheduler::TSignal::TSignal(
        TRequestStatus& aStatus )
        :
        iStatus( &aStatus ),
        iId( RThread().Id() )
    {
    aStatus = KRequestPending;
    }

inline TPtrC CConnMonScheduler::TSignal::Get() const
    {
    return TPtrC( reinterpret_cast<const TText*>( this ), sizeof( *this ) / sizeof( TText ) );
    }

inline TInt CConnMonScheduler::TSignal::Set( const TDesC& aData )
    {
    if ( aData.Size() != sizeof( *this ) )
        {
        return KErrGeneral;
        }
    else
        {
        Mem::Copy( this, aData.Ptr(), sizeof( *this ) );
        return KErrNone;
        }
    }

#endif // CONNMONSERV_H

// End-of-file
