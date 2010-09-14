/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM Server class definition
*
*/

/**
@file mpmserver.h
Mobility Policy Manager server class definitions.
*/

#ifndef MPMSERVER_H
#define MPMSERVER_H

//  INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#include <etel.h>           // RTelServer
#include <etelmm.h>         // RMobilePhone
#include <etelpckt.h>       // RPacketService

#include "mpmcommon.h"
#include "rmpm.h"
#include "mpmroamingwatcher.h"
#include "mpmdatausagewatcher.h"


class CMPMCommsDatAccess;
class CMpmCsIdWatcher;
class CMpmDataUsageWatcher;
class CMpmOfflineWatcher;

// CONSTANTS
_LIT( KMPMPanicCategory, "Mobility Policy Manager Server" );

const TInt    KPhoneRetryCount   = 7;
// 0.1 seconds
//
const TInt    KPhoneRetryTimeout = 100000;

const TUint32 KMaxOpenTransAttempts = 5;
const TUint32 KRetryAfter = 200000;

// The granularity with which iDisconnectQueue will allocate memory chunks. 
// If set to two there will be space for two instances of CMPMDisconnectDlg
// before new memory will be allocated.
const TInt    KGranularity       = 2;

// Security policy definitions

//Total number of ranges
const TUint KMPMPolicyRangeCount = 2;

// roaming to connected wlan wait interval
// in micro seconds (10 sec)
const TInt KRoamingToWlanUpdateInterval = 10000000; 

// roaming to connected hotspot wlan wait interval
// in micro seconds (120 sec)                    
const TInt KRoamingToHotspotWlanUpdateInterval = 120000000; 
 
//Definition of the ranges of IPC numbers
const TInt KMPMPolicyRanges[KMPMPolicyRangeCount] = 
    {
    0, // 0 -> 19 ; EMPMServerChooseIap,
       //           EMPMServerCancelRequest,
       //           EMPMServerApplicationLeavesConnection,
       //           EMPMServerIapConnectionStopped,
       //           EMPMServerProcessError,
       //           EMPMServerRegisterPrefIAPNotif,
       //           EMPMServerUnregisterPrefIAPNotif,
       //           EMPMServerWaitNotification, 
       //           EMPMServerSortSNAP, 
       //           EMPMServerApplicationJoinsConnection,
       //           EMPMServerIapConnectionActivated,
       //           EMPMServerIapConnectionStarted, 
       //           EMPMServerApplicationConnectionEnds,
       //           EMPMServerApplicationMigratesToCarrier, 
       //           EMPMServerApplicationIgnoredTheCarrier, 
       //           EMPMServerApplicationAcceptedTheCarrier, 
       //           EMPMServerApplicationRejectedTheCarrier, 
       //           EMPMServerEasyWlanSsid,
       //           EMPMServerReselectIap
#ifndef _DEBUG
    19 // 19 up ; non implemented function end of range check ; ENotSupported
#else
    600 // 20 up ; non implemented function end of range check ; ENotSupported
#endif
    };
    
//Policy to implement for each of the above ranges        
const TUint8 KMPMPolicyElementsIndex[KMPMPolicyRangeCount] = 
    {
    0, //applies to 0th range
    CPolicyServer::ENotSupported //applies to 1st range (out of range IPC)
    };

//Specific capability checks
const CPolicyServer::TPolicyElement KMPMPolicyElements[] = 
    {
    //policy "0"; fail call if NetworkControl not present
    {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl),
      CPolicyServer::EFailClient
    }
    };

//Package all the above together into a policy
const CPolicyServer::TPolicy KMPMPolicy =
    {
    0, //specifies policy "0" for connect attempts
    KMPMPolicyRangeCount, //number of ranges
    KMPMPolicyRanges, //ranges array
    KMPMPolicyElementsIndex, //elements<->ranges index
    KMPMPolicyElements, //array of elements
    };

// DATA TYPES 
// Connection state types
enum TConnectionState
    {
    EIdle = 0, 
    EStarting, 
    EStarted, 
    ERoaming 
    };

// Which component initiated sending preferred IAP notifications
enum TPrefIAPNotifCaller
    {
    EConnMon = 0,
    EConnMonEvent,
    EBearerMan, 
    EConfirmDlgRoaming, 
    EConfirmDlgStarting
    };

// Blacklisting expires when connection is terminated (user/application) 
// or when connection error is no longer a problem (IapConnectionStarted). 
// 
enum TBlacklistCategory
    {
    EConnectionLifetime = 0, 
    ETemporary
    };

//  Types of callback to be used after wlan scan has been completed
enum TWlanScanCallback
    {
    EWlanScanCallbackNone,
    EWlanScanCallbackChooseIap,
    EWlanScanCallbackProcessErr,
    EWlanScanCallbackCarrierRejected,
    EWlanScanCallbackSortSnap,
    EWlanScanCallbackGenerateEvent
    };

// Enums used with active connection selection
enum TMPMBearerType
    {
    // Set to none when there are no active connections
    EMPMBearerTypeNone,
    // VPN bearer
    EMPMBearerTypeVpn,
    // WLAN bearer
    EMPMBearerTypeWlan,
    // Packet data bearer
    EMPMBearerTypePacketData,
    // Something else than previous ones
    EMPMBearerTypeOther
    };

enum TOfflineWlanQueryResponse
    {
    EOfflineResponseUndefined,
    EOfflineResponseYes,
    EOfflineResponseNo
    };

// FUNCTION PROTOTYPES
void PanicServer( TInt aPanic );

// FORWARD DECLARATIONS
class CMPMConnMonEvents;
class CMPMServerSession;
class CMPMDtmWatcher;
class CMPMDisconnectDlg;
class CMPMConfirmDlgRoaming;
class CMPMConfirmDlgStarting;
class CMPMDefaultConnection;
class CMPMWlanQueryDialog;
class CMPMDialog;
class CMPMConnPermQueryTimer;
class CMPMOfflineQueryTimer;
class CMPMConnSelectionDlgTimer;

// CLASS DECLARATION
/**
*  Class structure to associate Connection Id 
*  and the list of blacklisted IAPs 
* 
*  @since 3.1
*/
class TMPMBlackListConnId
    {
    public:
        void Append( TUint32 aIap, TBlacklistCategory aCategory );
        void Remove( TInt index );
        void Close();
        TInt Count() const;
        inline TUint32 Iap( TInt aIndex) const;
        inline TBlacklistCategory Category( TInt aIndex) const;

    public:
        TConnectionId               iConnId;
    private:
        RArray<TUint32>             iBlackListIap;
        RArray<TBlacklistCategory>  iCategory;
    };
    
// CLASS DECLARATION
/**
*  Class structure containing application information.
*
*  @since 3.1
*/
class TConnectionInfo
    {
    public:
        TConnectionInfo();
        static TBool MatchId( const TConnectionInfo& aFirst,
                              const TConnectionInfo& aSecond );
                               
        static TBool MatchIdSnap( const TConnectionInfo& aFirst,
                                  const TConnectionInfo& aSecond );        
    public:
        TConnectionId       iConnId;
        TUint32             iSnap; 
        TUint32             iIapId;
        TConnectionState    iState;
        TUint32             iAppUid;
    };

// CLASS DECLARATION
/**
*  Class structure to associate IAP with 
*  the list of application information.
*  Used for book-keeping of active BM connections.
*
*  @since 3.1
*/
class TActiveBMConn
    {
    public:
        TActiveBMConn();
        static TBool MatchIapId( const TActiveBMConn& aFirst,
                                 const TActiveBMConn& aSecond );

        static TBool MatchConnInfo( const TActiveBMConn& aFirst,
                                    const TActiveBMConn& aSecond );
    public:
        TConnectionInfo     iConnInfo;
    };

// CLASS DECLARATION
/**
*  MPM Server class definition.
*
*  @lib MPMServer.exe
*  @since 3.0
*/
class CMPMServer : public CPolicyServer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPMServer* NewL();

        /**
        * Destructor.
        */
        virtual ~CMPMServer();

    public: // New functions
        
        /**
        * Panics the client thread in case of a wrong client request.
        * @since 3.0
        * @param aPanic Panic code
        */
        void PanicClient( const TInt aPanic ) const;

        /**
        * Adding info about active BM connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aSnap Snap Id of the connection
        * @param aIapId IAP Id of the connection
        * @param aState State of the connection
        * @param aSession Handle to session class
        */
        void AppendBMConnection( const TConnectionId aConnId,
                                 const TUint32       aSnap,
                                 const TUint32       aIapId, 
                                 TConnectionState    aState,
                                 CMPMServerSession&  aSession );

        /**
        * Reset the Iap Id as zero and update connection state as EIdle.
        * @since 3.2
        * @param aConnId Connection Id
        * @param aIapId IAP Id of the connection
        * @param aSession Handle to session class        
        */
        void ResetBMConnection( const TConnectionId aConnId, 
                                const TUint32       aIapId,
                                CMPMServerSession&  aSession );

        /**
        * Removing info about an active BM connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aSession Handle to session class        
        */
        void RemoveBMConnection( const TConnectionId aConnId,
                                 CMPMServerSession&  aSession );

        /**
        * Getting Iap Id of an active BM connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @return Iap Id if connection found, else 0
        */
        TUint32 GetBMIap( const TConnectionId aConnId );

        /**
        * Getting SNAP of an active BM connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @return SNAP if connection found, else 0
        */
        TUint32 GetBMSnap( const TConnectionId aConnId );

        /**
        * Getting the connection state for the Connection Id.
        * @since 3.2
        * @param aConnId Connection Id
        * @param aState State of the connection
        */
        void GetConnectionState( const TConnectionId aConnId,
                                 TConnectionState&   aState );

        /**
        * Checks if the connection is started for the Iap Id,
        * returning the state.
        * @since 3.2
        * @param aIapId IAP Id of the connection
        * @param aConnId Connection Id
        * @return Connection State, EIdle if not found
        */
        TConnectionState CheckUsageOfIap( const TUint32 aIapId, 
                              const TConnectionId aConnId );

        /**
        * Checks if any WLAN connection is started.
        * @since 3.2
        * @param aCdbAccess
		* @return IAP ID of the started WLAN connection, zero if not found
        */
        TUint32 IsWlanConnectionStartedL( const CMPMCommsDatAccess* aCdbAccess );
        
        /**
        * Appending the whole IAP info about an active BM connection.
        * @since 3.2 
        * @param aIapId IAP Id of the connection 
        * @param aConnId Connection Id
        * @param aSession Handle to session class        
        */
        void AppendBMIAPConnectionL( const TUint32       aIapId, 
                                     const TConnectionId aConnId,
                                     CMPMServerSession&  aSession );

        /**
        * Removing the whole IAP info about an active BM connection.
        * @since 3.1 
        * @param aIapId IAP Id of the connection
        * @param aConnId Connection Id
        * @param aSession Handle to session class        
        */
        void RemoveBMIAPConnection( const TUint32       aIapId, 
                                    const TConnectionId aConnId,
                                    CMPMServerSession&  aSession );

        /**
        * Adding info about active BM preferred IAP notification registration.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aSnap Snap Id of the connection
        * @param aCurrentIapId IAP Id of the connection
        */
        void AppendBMPrefIAP( const TConnectionId aConnId,
                              const TUint32       aSnap,
                              const TUint32       aCurrentIapId );

        /**
        * Removing info about active BM preferred IAP notification 
        * registration.
        * @since 3.1
        * @param aConnId Connection Id
        */
        void RemoveBMPrefIAP( const TConnectionId aConnId );

        /**
        * Resets current IAP Id info of an active BM preferred IAP 
        * notification registration.
        * @since 3.2
        * @param aConnId Connection Id
        */
        void ResetBMPrefIAP( const TConnectionId aConnId );

        /**
        * Checks whether given connection has registered for 
        * preferred IAP notifications.
        * @since 3.2
        * @param aConnId Connection Id
        * @return ETrue if registered
        */
        TBool RegisteredForBMPrefIAP( const TConnectionId aConnId );

        /**
        * Sets last notified IAP for BMPrefIAPNotif related to a connection
        * 
        * @since 3.2
        * @param aConnId Connection Id
        * @param aIapId Last notified IAP id.
        * @return KErrNone if successfull. otherwise one of the system wide error
        * codes
        */
        TInt SetConnectionBMPrefLastNotifedIap( TConnectionId aConnId, TUint32 aIapId );

        /**
        * Adding session pointer.
        * @since 3.2
        * @param aSession Pointer to session
        */
        void AppendSessionL(const CMPMServerSession* aSession);

        /**
        * Removing session pointer.
        * @since 3.2
        * @param aSession Pointer to session
        */
        void RemoveSession( const CMPMServerSession* aSession );

        /**
        * Notify each session about IAP availability change.
        * @since 3.1
        * @param aIapInfo Info about available IAPs
        * @param aCaller Identifies the calling context 
        */
        void NotifyBMPrefIapL( const TConnMonIapInfo& aIapInfo,
                               const TPrefIAPNotifCaller aCaller );

        /**
        * Update Connection dialog of each session
        * @since 3.2
        */
        void UpdateSessionConnectionDlgL();

        /**
        * Completes pending messages
        * @since 5.2
        * @param aIapId IAP Id of the connection.
        * @param aError Message status
        * @param aErrorReturned Error value to be returned, NULL if no 
        * value should be passed
        * @param aNeededAction Needed action to be returned, NULL if no 
        * value should be passed
        */
        void HandlePendingMsgs( TUint aIapId,
                                TInt aError,
                                TInt* aErrorReturned,
                                TBMNeededAction* aNeededAction );

        /**
        * Handling of blacklisting certain IAP and the presumed IAP for 
        * the certain connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aIapId IAP Id of the connection.
        * @param aCategory Either connection lifetime or temporary. 
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes 
        */
        TInt HandleServerBlackListIap( const TConnectionId   aConnId, 
                                       TUint32               aIapId, 
                                       TBlacklistCategory    aCategory );

        /**
        * Handling of blacklisting certain IAP for the certain connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aIapId IAP Id of the connection.
        * @param aCategory Either connection lifetime or temporary. 
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes 
        */
        TInt BlackListIap( const TConnectionId   aConnId, 
                           TUint32               aIapId, 
                           TBlacklistCategory    aCategory );

        /**
        * Handling of unblacklisting certain IAP or all IAPs 
        * for the certain connection.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aIapId IAP Id of the connection.
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes 
        */
        TInt HandleServerUnblackListIap( const TConnectionId aConnId, 
                                         TUint32             aIapId );

        /**
        * Handling of unblacklisting all IAPs for certain category.
        * @since 3.2
        * @param aCategory Either connection lifetime or temporary. 
        */
        void HandleServerUnblackListIap( TBlacklistCategory  aCategory );

        /**
        * Get blacklisted iap based on the Connection Id.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aBlacklistedIAP returns blacklisted iaps.
        * @return KErrNone if successful.
        */
        TInt GetBlacklistedIAP( TConnectionId    aConnId, 
                                RArray<TUint32> &aBlacklistedIAP );

        /**
        * Get all blacklisted IAPs.
        * @since 3.2
        * @param aBlacklistedIAP returns blacklisted iaps.
        * @return KErrNone if successful.
        */
        TInt GetBlacklistedIAP( RArray<TUint32> &aBlacklistedIAP );

        /**
        * Set the ConnMonEvents object.
        * @since 3.1
        * @param aEvents Pointer to the ConnMonEvents object.
        */
        inline void SetEvents( CMPMConnMonEvents* aEvents );

        /**
        * Get the ConnMonEvents object.
        * @since 3.1
        * @return Pointer to the ConnMonEvents object.
        */
        inline CMPMConnMonEvents* Events();

        /**
        * Find the correct Connection Id from the list of blacklisted 
        * Connection Id's.
        * @since 3.1
        * @param aConnId Connection Id
        * @param aIndex returns index of Connection Id in the list.
        * @return ETrue if successful.
        */
        TBool FindBlacklistedConnIndex( const TConnectionId aConnId, TInt &aIndex );

        /**
        * Checks if voice call is active or not.
        * @since 3.1
        * @return ETrue if voice call is active, otherwise EFalse.
        */
        TBool IsVoiceCallActiveL() const;

        /**
        * Checks if mode is GSM or not.
        * @since 3.1
        * @return ETrue if mode is GSM, otherwise EFalse.
        */
        TBool IsModeGSM() const;

        /**
        * Checks if phone supports Dual Transfer Mode or not.
        * @since 3.1
        * @return ETrue if phone supports DTM, otherwise EFalse.
        */
        TBool IsDTMSupported() const;
                
        /**
        * Dump array of active connections to log in order to support 
        * testing.
        * @since 3.2
        */
        void DumpActiveBMConns();

        /**
        * Sets WLAN scan required On/Off.
        * @since 3.2
        * @param aValue True or false.
        */
        inline void SetWLANScanRequired( const TBool aValue );

        /**
        * Checks if there is a need to scan WLAN networks.
        * @since 3.2
        * @return ETrue if yes, otherwise EFalse.
        */
        inline TBool IsWLANScanRequired() const;

        /**
        * Get the DisconnectQueue.
        * @since 3.2
        * @return Pointer to the DisconnectQueue.
        */
        inline CArrayPtrFlat<CMPMDisconnectDlg>* DisconnectQueue();

        /**
        * Get the RoamingQueue.
        * @since 3.2
        * @return Pointer to the RoamingQueue.
        */
        inline CArrayPtrFlat<CMPMConfirmDlgRoaming>* RoamingQueue();

        /**
        * Get the StartingQueue.
        * @since 3.2
        * @return Pointer to the StartingQueue.
        */
        inline CArrayPtrFlat<CMPMConfirmDlgStarting>* StartingQueue();

        /**
        * Appends aDlg to the iWlanQueryQueue.
        * @since 3.2
        * @param aDlg Pointer to the CMPMConfirmDlgWlanQuery object.
        */
        void AppendWlanQueryQueueL( CMPMWlanQueryDialog* aDlg );

        /**
        * Removes the dialog from the iWlanQueryQueue.
        * @since 3.2
        * @param aDlg dialog to be removed.
        */
        inline void RemoveFromWlanQueryQueue( CMPMWlanQueryDialog* aDlg );

        /**
        * Get the WlanQueryQueue.
        * @since 3.2
        * @return Pointer to the WlanQueryQueue.
        */
        inline CArrayPtrFlat<CMPMWlanQueryDialog>* WlanQueryQueue();

        /**
        * Get the first item in iWlanQueryQueue.
        * @since 3.2
        * @return Pointer to the first item in iWlanQueryQueue.
        */
        inline CMPMWlanQueryDialog* FirstInWlanQueryQueue();

        /**
        * Get the ConnectDialogQueue.
        * @since 3.2
        * @return Pointer to the CMPMDialogQueue.
        */
        inline CArrayPtrFlat<CMPMDialog>* ConnectDialogQueue();

        /**
        * Get the Default Connection object.
        * @since 3.2
        * @return Pointer to the Default Connection object.
        */
        CMPMDefaultConnection* DefaultConnection(); 

        /**
        * Returns true if there is a started connection
        * 
        * @since 3.2
        * @param aIapId Optional searched IapId. If it doesn't exist,
        * returns first found and started connection iapid
        * @return IapId, KErrNotFound if doesn't exist
        */
        TInt StartedConnectionExists( TInt aIapId = KErrNotFound ); 
        
        /**
        * Increments connection count.
        * @since 3.2
        */
        inline void IncrementConnections();

        /**
        * Decrements connection count.
        * @since 3.2
        */
        inline void DecrementConnections(); 

        /**
        * Returns connection count.
        * @since 3.2
        * @return Connection count.
        */
        inline TUint ConnectionCounter(); 

        /**
        * Sets connection count.
        * @since 3.2
        * @param aValue Connection count value.
        */
        inline void SetConnectionCounter( const TUint aValue );

        /**
        * Returns array of dedicated clients.
        *
        * @since 5.0
        * @return Reference to array of dedicated clients.
        */
        inline RArray<TUint32>& DedicatedClients();

        /**
        * Returns the Commsdat access instance
        *
        * @since 3.2
        * @return Commsdat access instance.
        */
        inline CMPMCommsDatAccess* CommsDatAccess();

        /**
        * Starts forced roaming sequence to wlan
        *
        * @param aIapInfo Info about available IAPs
        * @since 5.2
        */
        void StartForcedRoamingToWlanL( const TConnMonIapInfo& aIapInfo );
        
        /**
        * Starts forced roaming sequence to connected wlan
        *
        * @param aIapInfo Info about available IAPs
        * @return Always zero...
        * @since 5.2
        */
        static TInt StartForcedRoamingToConnectedWlanL( TAny* aUpdater );
        
        /**
        * Starts forced roaming sequence to connected hotspot wlan
        *
        * @param aIapInfo Info about available IAPs
        * @return Always zero...
        * @since 5.2
        */
        static TInt StartForcedRoamingToConnectedHotspotWlanL( TAny* aUpdater );

        /**
        * Starts forced roaming sequence from WLAN if necessary
        *
        * @param aIapInfo Info about available IAPs
        * @since 5.2
        */
        void StartForcedRoamingFromWlanL( const TConnMonIapInfo& aIapInfo );
        
        /**
        * Checks whether phone is in visitor network.
        *
        * @since 5.2
		* @return ETrue if phone is in visitor network, EFalse otherwise
        */
        TBool IsVisitorNetwork() const;
        
        /**
        * Returns the RoamingWatcher pointer
        *
        * @since 5.2
		* @return RoamingWatcher pointer
        */
        inline CMPMRoamingWatcher* RoamingWatcher() const;

         /**
        * Returns the DataUsageWatcher pointer
        *
        * @since 5.2
        * @return DataUsageWatcher pointer
        */
        inline CMpmDataUsageWatcher* DataUsageWatcher() const;        
        
        
    public: // Functions from base classes

        /**
        * From CServer2. Creates a new session for a client.
        * @since 3.0
        * @param aVersion Version information
        * @param aMessage Not really used here
        * @return Pointer to the new client session
        */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage) const;

        /**
		* Stops either all connections or a certain IAP
		*
        * @param IAP ID to be stopped, zero for all connections
		*/
        void StopConnections( TInt aIapId = 0 );

    public:

        /**
         * Mark that there is an active user connection.
         * @since 5.1
         */
        inline void SetUserConnection();
        
        /**
         * Mark that user connection is not active
         * @since 5.1
         */
        inline void ClearUserConnection();
        
        /**
         * Check if user connection is active
         * @since 5.1
         * @return ETrue if user connection is active.
         */
        inline TBool UserConnection() const;

        /**
         * Set connection preferences used by user connection
         * @since 5.1
         * @param aUserConnPref User connection prefernces.
         */
        inline void SetUserConnPref( const TMpmConnPref& aUserConnPref );
        
        /**
         * Returns connection preferences used by user connection
         * @since 5.1
         * @return User connection preferences.
         */
        inline const TMpmConnPref* UserConnPref() const;
        
        /**
         * Checks whether user connection is in internet snap.
         * @since 5.1
         * @return ETrue if user connection is in internet snap
         */
        TBool UserConnectionInInternet() const;
                
        /**
         * Handle to central repository watcher
         * @since 5.1
         * @return Pointer to watcher object.
         */
        inline CMpmCsIdWatcher* CsIdWatcher();

        /**
         * Change state of the P&S keys according to active connection.
         * Non-leaving version. Resets variables in case error happens.
         * @since 5.2         
         * @param aSession Handle to session used by connection.
        */
        void UpdateActiveConnection( CMPMServerSession& aSession );
        
        /**
        * Change state of the P&S keys according to active connection.
        * @since 5.0
        * @param aSession Handle to session used by connection.
        */
        void UpdateActiveConnectionL( CMPMServerSession& aSession );

        /**
        * Map bearer type between MPM and commsdat.
        * @since 5.0
        * @param aBearerType Bearer type used by MPM for active connection selection.
        * @return Bearer type used by commsdat
        */
        TUint32 MapBearerType( TMPMBearerType aBearerType );

        /**
        * Writes new active connection to the P&S keys
        * @since 5.0
        */    
        void PublishActiveConnection();
        
        /**
        * Returns number of active iaps.
        * @since 5.0
        * @param aKeysUpToDate Tells whether P&S keys are valid or not.
        * @return Number of active connections
        */
        TInt NumberOfActiveConnections( TBool& aKeysUpToDate );

        /**
        * Returns server session instance that corresponds to given
        * connection id.
        * @since 5.2
        * @param aConnId Id of the connection. The id must belong to an
        *                existing server session.
        * @return Server session instance
        */
        CMPMServerSession* GetServerSession( TConnectionId aConnId ) const;
        
        /**
        * Stops cellular connections, except MMS
        * @param aSilentOnly stop only silent cellular connections
        * @since 5.2
        */
        void StopCellularConns( TBool aSilentOnly = EFalse );

        /**
        * Offline mode watcher updates the mode variable stored by MPM server.
        * @since 5.2
        * @param aNewModeValue New offline mode value
        */
        void UpdateOfflineMode( TInt aNewModeValue );

        /**
        * Returns true if the phone is in offline mode.
        * @since 5.2
        * @return ETrue if phone is in offline mode, EFalse otherwise.
        */
        TBool IsPhoneOffline();

        /**
        * Tells the "Use WLAN in offline mode" query response.
        * @since 5.2
        * @return Response of offline mode query (Undefined if not executed)
        */
        TOfflineWlanQueryResponse OfflineWlanQueryResponse();

        /**
        * Called when the "Use WLAN in offline mode" query has been responded.
        * @since 5.2
        * @param aResponse Sets the Offline mode query response value.
        */
        void SetOfflineWlanQueryResponse( TOfflineWlanQueryResponse aResponse);

        /**
        * Starts the connection permission query timer.
        * During the timer, no connection permission query can be initiated.
        * @since 5.2
        */
        void StartConnPermQueryTimer();

        /**
        * Resets the connection permission query timer.
        * @since 5.2
        */
        void ResetConnPermQueryTimer();

        /**
        * Returns true if the connection permission query timer is running.
        * During the timer, no connection permission query can be initiated.
        * @since 5.2
        * @return ETrue if timer is running, EFalse otherwise.
        */
        TBool IsConnPermQueryTimerOn();
        
        /**
        * Starts the offline query timer.
        * During the timer, offline query can not be initiated.
        * @since 5.2
        */
        void StartOfflineQueryTimer();

        /**
        * Resets the offline query timer.
        * @since 5.2
        */
        void ResetOfflineQueryTimer();

        /**
        * Returns true if the offline query timer is running.
        * During the timer, offline query can not be initiated.
        * @since 5.2
        * @return ETrue if timer is running, EFalse otherwise.
        */
        TBool IsOfflineQueryTimerOn();
        
        /**
        * Starts the connection selection dialog timer.
        * During the timer, connection selection dialog can not be initiated.
        * @since 5.2
        */
         void StartConnSelectionDlgTimer();

        /**
        * Resets the connection selection dialog timer.
        * @since 5.2
        */
         void ResetConnSelectionDlgTimer();

        /**
        * Returns true if the connection selection dialog timer is running.
        * During the timer, connection selection dialog can not be initiated.
        * @since 5.2
        * @return ETrue if timer is running, EFalse otherwise.
        */
         TBool IsConnSelectionDlgTimerOn();
         
    private:

        /**
        * C++ default constructor.
        */
        CMPMServer();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Send error notification with KErrDisconnect error code.
        * @since 5.2
        * @param aConnIndex Active connection index
        * @param aAvailWlanIapIds Array of available wlan iap ids
        * @param aIapList Array of all available iap ids
        * @param aCheckForBestIap Whether check if current iap is already
        *                         the best should be done
        * @param aDestinationBearerType Destination iap bearer type
        */
        void NotifyDisconnectL( TInt aConnIndex, 
                                RArray<TUint32>& aAvailWlanIapIds,
                                RAvailableIAPList& aIapList,
                                TBool aCheckForBestIap,
                                TMPMBearerType aDestinationBearerType );
        
        /**
         * Check if iap can be disconnected, and disconnect it
         * @since 5.2
         * @param aIapId Iap id for checking
         */
        void CheckIapForDisconnect( TInt aIapId );
        

    private: // Data
        // Pointer to the ConnMonEvents object
        CMPMConnMonEvents* iEvents;

        // Array of active connections requested by BM
        RArray<TActiveBMConn> iActiveBMConns;
        
        // Array of all sessions
        //
        RPointerArray<CMPMServerSession> iSessions;

        // Array of blacklisted IAPs
        // 
        RArray<TMPMBlackListConnId>      iBlackListIdList;

        // Handle to the telephony server
        RTelServer iTelServer;

        // RMobilePhone handle
        RMobilePhone iMobilePhone;

        // Handle to the packet service
        RPacketService iPacketService;

        // True if mmtsy is loaded
        TBool iTSYLoaded;

        // True if packet services are loaded
        TBool iPacketServLoaded;

        // Dual Transfer Mode watcher
        CMPMDtmWatcher* iDtmWatcher;
        
		//Cellular network roaming watcher
        CMPMRoamingWatcher* iRoamingWatcher;

        // Is WLAN scan required or not before displaying Connection Dialog
        TBool iWLANScanRequired;

        // Solves problem with overlapping Disconnect Dialogs
        CArrayPtrFlat<CMPMDisconnectDlg>* iDisconnectQueue;

        // Solves problem with overlapping Roaming Dialogs
        CArrayPtrFlat<CMPMConfirmDlgRoaming>* iRoamingQueue;

        // Solves problem with overlapping Starting Dialogs
        CArrayPtrFlat<CMPMConfirmDlgStarting>* iStartingQueue;

        // Solves problem with overlapping Wlan Queries
        CArrayPtrFlat<CMPMWlanQueryDialog>* iWlanQueryQueue;

        // Solves problem with overlapping Connection Dialogs
        CArrayPtrFlat<CMPMDialog>* iConnectDialogQueue;
        
        // Handles Default Connection selection
        CMPMDefaultConnection* iDefaultConnection;

        // Keeps track of the number of connections
        TUint iConnectionCounter;

        // Set when there is user connection exist
        TBool iUserConnection;

        // Preferences selected for the user connection
        TMpmConnPref iUserConnPref;

        // Set when user connection in internet snap
        TBool iUserConnectionInInternet;
        
        /**
         * Handle to central repository watcher
         * Own.
         */
        CMpmCsIdWatcher* iMpmCsIdWatcher;

        /**
         * Handle to central repository watcher
         */
        CMpmDataUsageWatcher* iMpmDataUsageWatcher;

        /**
         * Handle to central repository watcher
         */
        CMpmOfflineWatcher* iMpmOfflineWatcher;

        // Iap id of the active connection
        TUint32 iActiveIapId;
        
        // Snap id of the active connection
        TUint32 iActiveSnapId;

        // Bearer type of the active connection
        TMPMBearerType iActiveBearerType;

        // Dedicated clients
        RArray<TUint32> iDedicatedClients;

        // Used for commsdat related functionalities
        CMPMCommsDatAccess* iCommsDatAccess;
        
        // Offline mode.
        TInt iOfflineMode;
        
        // Is WLAN usage already accepted in this offline session.
        TOfflineWlanQueryResponse iOfflineWlanQueryResponse;
        
        // Timer to start roaming to connected WLAN network 
        CPeriodic* iRoamingToWlanPeriodic;
        
        // Timer to start roaming to connected WLAN network 
        CPeriodic* iRoamingToHotspotWlanPeriodic;
        
        // TConnMonIapInfo Info about available IAPs
        TConnMonIapInfo iConnMonIapInfo;
        
        // Connection permission query cancellation delay timer
        CMPMConnPermQueryTimer* iConnPermQueryTimer;
        
        // Offline query cancellation delay timer
        CMPMOfflineQueryTimer* iOfflineQueryTimer;
        
        // Connection selection dialog cancellation delay timer         
        CMPMConnSelectionDlgTimer* iConnSelectionDlgTimer;
      
    };

#include "mpmserver.inl"


#endif // MPMSERVER_H

// End of File
