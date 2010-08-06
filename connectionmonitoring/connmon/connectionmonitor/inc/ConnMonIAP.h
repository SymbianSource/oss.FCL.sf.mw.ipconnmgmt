/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Core functionality for ConnMon.
*
*/

#ifndef CONNMONIAP_H
#define CONNMONIAP_H

#include <es_sock.h>
#include <in_sock.h>
#include <commdb.h>
#include <es_enum.h>
#include <etel.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>
#include <rconnmon.h>

#include "ConnMonDef.h"
#include "CWlanSupport.h"
#include "connmondtmnoti.h"
#include "connmontelnoti.h"

class CConnMonIAP;
class CConnUpDownNotifier;
class CProgressNotifier;
class CDataNotifier;
class CNetwStatusNotifier;
class CNetwRegistrationNotifier;
class CBearerNotifier;
class CWcdmaBearerNotifier;
class CModeNotifier;
class CGsmSignalNotifier;
class CBearerAvailabilityNotifier;
class CSubConnUpDownNotifier;
class CActivityNotifier;
class CConnMonServer;
class CDataVolume;
class CCsdFax;
class CPsdFax;
class CWlanSupport;
class CConnMonBearerNotifier;

// Record name for WLAN bearer table
_LIT( KWlanBearerRecordName, "WLANBearer" );

const TInt KPhoneRetryCount = 7;
const TInt KPhoneRetryTimeout = 100000;
const TInt KConnMonSmallBufferLen = 30;
const TInt KBitsInByte = 8;
const TInt KAttributeParameterIndex = 3;

/**
* TConnInfo
* Connection information record.
*/
class TConnInfo
    {
    public: // Constructors and destructor
        TConnInfo(
                TInt aIapId,
                TInt aNetId,
                TUint aConnectionId,
                TInt aBearer,
                TBearerInfo aBearerInfo );

    public:
        void Reset();
        static TInt Compare( const TConnInfo& aFirst, const TConnInfo& aSecond );
        static TBool MatchId( const TConnInfo& aFirst, const TConnInfo& aSecond );
        static TBool MatchIap( const TConnInfo& aFirst, const TConnInfo& aSecond );
        static TBool MatchBearer( const TConnInfo& aFirst,const TConnInfo& aSecond );

    public: // Data
        // IAP Id
        TUint32 iIapId;

        // Net Id
        TUint32 iNetId;

        // Unique connection id
        TUint iConnectionId;

        // Bearer type
        TInt iBearer;

        // RConnection object that is open and attached
        RConnection* iConnAttach;

        // Connection status notifier
        CProgressNotifier* iProgressNotifier;

        // Downlink data volume notifier
        CDataNotifier* iDLDataNotifier;

        // Uplink data volume notifier
        CDataNotifier* iULDataNotifier;

        // Active object for getting KDownlinkData and KUplinkData
        CDataVolume* iDataVolumeAO;

        // Connection down notifier
        CSubConnUpDownNotifier* iConnDownNotifier;

        // Connection activity notifier
        CActivityNotifier* iActivityNotifier;

        // 0 means that connection is idle
        // 1 means that there is data moving through the connection
        TUint8 iActivity;

        // Bearer info
        TBearerInfo iBearerInfo;

        // BearerInfo/BearerGroup notifier
        CConnMonBearerNotifier* iBearerNotifier;
    };

/**
* CConnMonIAP
*/
NONSHARABLE_CLASS( CConnMonIAP )
        :
        public CBase,
        public MConnMonTelephonyObserver,
        public MConnMonDtmNotifierObserver
    {
    public: // Constructors and destructor
        CConnMonIAP( CConnMonServer* aServer );
        void ConstructL();
        ~CConnMonIAP();

    public: // New methods
        /**
         * Enumerates active connections.
         * @since
         * @param aCount On return, contains the number of the active connection
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt EnumerateConnections( TUint& aCount );

        /**
         * Enumerates active connections.
         * @since
         * @param aCount On return, contains the number of the active connection
         * @param aEnumInfo On return, contains the enumarated connection ids.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt EnumerateConnectionsL( TUint& aCount, CArrayFixFlat<TUint>& aEnumInfo );

        /**
         * Checks if given connection id is valid.
         * @since
         * @param aConnectionId Connection id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TBool ConnectionExists( const TUint& aConnectionId );

        /**
         * Gets the count of the subconnections.
         * @since
         * @param aConnectionId  Connection id.
         * @param aSubConnectionCount On return, contains the count of the subconnections.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetConnectionInfo( const TUint& aConnectionId, TUint& aSubConnectionCount );

        /**
         * Gets a TInt attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue On return, contains the value.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetIntAttributeL( const RMessage2& aMessage, TInt& aValue );

        /**
         * Gets a TUint attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue On return, contains the value.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetUintAttributeL( const RMessage2& aMessage, TUint& aValue );

        /**
         * Gets a TBool attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue On return, contains the value.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetBoolAttributeL( const RMessage2& aMessage, TBool& aValue );


        /**
         * Gets a TDes attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue On return, contains the value.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetStringAttribute( const RMessage2& aMessage, TDes& aValue );

        /**
         * Gets the UIDs of the clients using a connection. UIDs are 0 in WINS.
         * @since
         * @param aConnectionId  Connection id.
         * @param aClientEnum On return, contains the count of the clients and their uids.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetUids( const TUint aConnectionId, TConnMonClientEnum& aClientEnum );

        /**
         * Sets a TUint attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue Value to be set.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt SetUintAttribute( const RMessage2& aMessage, const TUint aValue );

        /**
         * Sets a TBool attribute.
         * @since
         * @param aMessage Reference to client message, containing the attribute information.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt SetBoolAttribute( const RMessage2& aMessage );

        /**
         * Sets a String attribute.
         * @since
         * @param aMessage Reference to client message.
         * @param aValue Value to be set.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt SetStringAttributeL( const RMessage2& aMessage, const TDes& aValue );

        /**
         * Turns notifications ON.
         */
        void ListenL();

        /**
         * Turns notifications OFF.
         * @since
         * @return void
         */
        void CancelListen();

        /**
         * Adds a connection to the global array and starts notifications for it.
         * @since
         * @param aConnection New connection information.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt AddConnectionL( TConnInfo& aConnection );

        /**
         * Removes connection from global array and stops notifications.
         * @since
         * @param aConnection Connection information.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt RemoveConnection( TConnInfo& aConnection );

        /**
         * Gets a pointer to the RConnection handle.
         * @since
         * @return A pointer to the RConnection handle.
         */
        inline RConnection* Rconnection()
            {
            return &iConnection;
            }

        /**
         * Gets a pointer to the packet service handle.
         * @since
         * @return A pointer to the packet service handle.
         */
        inline const RPacketService* PacketService() const
            {
            return &iPacketService;
            }

        /**
         * Gets a pointer to the WLAN management services.
         * @since
         * @return A pointer to the WLAN management services.
         */
        inline CWlanSupport* WlanSupport()
            {
            return iWlanSupport;
            }

        /**
         * Gets a pointer to the RMobilePhone handle.
         * @since
         * @return A pointer to the iMobilePhone member.
         */
        inline const RMobilePhone* MobilePhone() const
            {
            return &iMobilePhone;
            }

        /**
         * Gets TConnMonNetworkStatus matching ETel network status.
         * @since
         * @param aPacketStatus Packet network status returned by ETel server.
         * @return TConnMonNetworkStatus.
         */
        TInt CalculateNetworkStatus(
                const RPacketService::TStatus& aPacketStatus ) const;

        /**
         * Gets TConnMonNetworkRegistration matching ETel network registration status.
         * @since
         * @param aRegistration Network registration status returned by ETel server.
         * @return TConnMonNetworkRegistration.
         */
        TInt CalculateNetworkRegistration(
                const RMobilePhone::TMobilePhoneRegistrationStatus& aRegistration ) const;

        /**
         * Gets TConnMonNetworkRegistration_v2 matching ETel network registration status.
         * @since
         * @param aRegistration Network registration status returned by ETel server.
         * @return TConnMonNetworkRegistration_v2.
         */
        TInt CalculateNetworkRegistration_v2(
                const RMobilePhone::TMobilePhoneRegistrationStatus& aRegistration ) const;

        /**
         * Tells whether this cell supports EdgeGPRS or GPRS.
         * @since
         * @return ETrue if cell supports Edge, EFalse otherwise.
         */
        TBool IsEdgeCell();

        /**
         * Tells whether this cell supports Hsdpa or WCDMA.
         * @since
         * @return ETrue if cell supports Hsdpa, EFalse otherwise.
         */
        TBool IsHsdpaCell();

        /**
         * Tells whether this cell supports Hsdpa and/or Hsupa.
         * @since MCL 9.1
         * @return EBearerInfoWCDMA if no high speed features detected, or
         * EBearerInfoHSDPA / EBearerInfoHSUPA / EBearerInfoHSxPA.
         */
        TInt HsxpaStatus();

        /**
         * Gets the time when the connection has started.
         * @since
         * @param aConnectionId The connection id.
         * @param aTime On return, contains the starting time of the connection.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetStartTime( const TUint& aConnectionId, TTime& aTime );

        /**
         * Gets the bearer info for an internal connection.
         * @since
         * @param aConnectionId  Connection id.
         * @param aBearer On return, contains the bearer.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetBearer(
                const TUint& aConnectionId,
                TInt& aBearer,
                TBearerInfo& aBearerInfo );

        /**
         * Updates activity information to connection array.
         * @since
         * @param aConnectionId The connection id.
         * @param aActivity 0 means idle, 1 means active.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt UpdateActivity( const TUint& aConnectionId, const TUint& aActivity );

        /**
         * Checks if subconnection level notifier has already sent connection
         * closed event.
         * @since
         * @param aConnection The connection info. On return, will contain connection id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TBool GetDeleteNotifyStatus( TConnInfo& aConnection );

        /**
         * Gets a pointer to the activity notifier object for the given connection id.
         * @since
         * @param aConnectionId The connection id.
         * @param aActivityNotifier On return, pointer to the activity notifier.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetActivityNotifier(
                const TUint& aConnectionId,
                CActivityNotifier** aActivityNotifier );

        /**
         * Gets a pointer to the progress notifier object for the given connection id.
         * @since
         * @param aConnectionId The connection id.
         * @param aProgressNotifier On return, pointer to the progress notifier.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetProgressNotifier(
                const TUint& aConnectionId,
                CProgressNotifier** aProgressNotifier );

        /**
         * Gets a pointer to the subconnection up/down notifier object for the
         * given connection id.
         * @since
         * @param aConnectionId The connection id.
         * @param aSubConnUpDownNotifier On return, pointer to the subconnection
         * up/down notifier.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetSubConnUpDownNotifier(
                const TUint& aConnectionId,
                CSubConnUpDownNotifier** aSubConnUpDownNotifier );

        /**
         * Wakes up CONNECTION SPECIFIC notifications that depend on threshold values.
         * @since
         * @param aConnectionId Connection Id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt WakeUpNotifications( const TUint& aConnectionId );

        /**
         * Wakes up GENERAL notifications that depend on threshold values.
         * @since
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt WakeUpNotifications();

        /**
         * Starts activity notifier for the given connection id.
         * @since
         * @param aConnectionId Connection Id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt LaunchActivityNotifierL( const TUint& aConnectionId );

        /**
         * Starts data notifiers (uplink & downlink) for the given connection id.
         * @since
         * @param aConnectionId Connection Id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt LaunchDataNotifiers( const TUint& aConnectionId );

        /**
         * Starts bearer notifiers for the given connection id.
         * @since
         * @param aConnectionId Connection Id.
         * @return KErrNone if successfull, otherwise KErrNotFound.
         */
        TInt LaunchBearerNotifier( const TUint& aConnectionId );

        /**
         * Appends active connections (iapID & netID) with matching aBearerId
         * to aIdArray.
         *
         * @param aBearerId Identifies the bearer.
         * @param aIdArray On return contains the use IAP IDs for the active
         * connections matching the given aBearerId parameter.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        void GetActiveConnectionsIds( const TUint& aBearerId, RArray<TUint>& aIdArray );

        /**
         * Return ETrue if one or more active packetdata connections are found,
         * EFalse otherwise.
         */
        TBool ActivePacketdataConnectionsFound();

        /**
         * Tells whether bearer is supported or not.
         * @since
         * @param aConnectionId Generic bearer id (EBearerIdGPRS, EBEarerIdCSD).
         * @param aByCell On return, ETrue if the bearer is supported by the cell, EFalse otherwise.
         * @param aByPhone On return, ETrue if the bearer is supported by the phone, EFalse otherwise.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetBearerSupportInfo(
                const TUint aConnectionId,
                TBool& aByCell,
                TBool& aByPhone ) const;

        /**
         * Gets the bearer is matching the phone's mode.
         * @since
         * @param aBeareId On return, the bearer id matching the phone's mode.
         * @param aCsd ETrue is CS bearer wanted, EFalse is PS bearer wanted.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetBearerId( TUint& aBearerId, TBool aCsd=EFalse ) const;

       /**
         * Destroys the old record and creates a new one because connection has reconnected.
         * @since
         * @param aConnectionId The old connectionId. On return it will be replaced by the new Id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt Reconnect( TUint& aConnectionId );

        /**
         * Saves network ids selected by the end user.
         * @since
         * @param aLanPreference A string of comma separated network id e.g. 1,2,5.
         * @return void
         */
        void SetEndUserLanPreference( const TDes8& aLanPreference )
            {
            iEndUserNetIds = aLanPreference;
            }

        /**
         * Gets network ids selected by the end user.
         * @since
         * @param aLanPreference On return contains a string of comma separated network id e.g. 1,2,5.
         * @return void
         */
        void GetEndUserLanPreference( TDes8& aLanPreference )
            {
            aLanPreference = iEndUserNetIds;
            }

        /**
         * Resets network ids.
         * @since
         * @return void
         */
        void ResetEndUserLanPreference()
            {
            iEndUserNetIds.Zero();
            iEndUserNetIds.FillZ();
            }

        /**
         * Check if WLAN is supported and connection ID is correct.
         *
         * @param aConnectionId Connection ID of the connection.
         * @return KErrNone when successfull, otherwise a system wide error code.
         */
        TInt WlanSupportCheck( const TUint& aConnectionId );

        /**
         * Appends available WLAN IAPs. IAPs are queried from WLAN engine, if
         * WLAN scanning flag is set, otherwise cache is used.
         *
         * @param aIapBuf On return, contains the available WLAN IAPs.
         * @return KErrNone when successfull, otherwise a system wide error code.
         */
        TInt AppendAvailableWLANIaps( RArray<TUint>& aIdArray );

        /**
         * Appends available WLAN IAPs. IAP info is read from CommsDat. Those
         * IAPs with a matching SSID with the current WLAN connection are
         * marked available.
         * @since 3.2
         * @param aIapBuf On return, contains the available WLAN IAPs.
         * @return KErrNone when successfull, otherwise a system wide error code.
         */
        TInt AppendAvailableWLANIapsBySsid( RArray<TUint>& aIdArray );

        /**
         * Enables WLAN scan for the next iap availability query.
         * @since
         * @return void
         */
        void EnableWlanScan();

        /**
         * Callback function from iDualTransferModeNotifier
         */
        virtual void DtmStateChanged();

        /**
         * Callback function from iTelephonyNotifier
         */
        virtual void PhoneLineStatusChange();

        /**
         * Gets the network registration status.
         * @since
         * @param aNetworkStatus On return, contains the current network registration status.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetNetworkRegistrationMode( TInt& aRegistration ) const;

        /**
         * Gets the bearer for the connection.
         * @since 3.2
         * @param aConnectionId Id of the connection.
         * @return KErrNone if successfull, otherwise KErrNotFound.
         */
        TInt GetBearerInfo( const TUint aConnectionId, TBearerInfo& aBearerInfo );

        /**
         * Sends a bearer group changed event.
         * @since
         * @param aBearerinfo New bearer to find out the new bearer
         * groups.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        void SendConnectionEvents( TConnMonBearerInfo& aBearerinfo );

        /**
         * Gets the last error value returned with connection status.
         * Error code indicates the reason of connection closure.
         * @since
         * @param aConnectionId  Connection id.
         * @param aProgress On return, contains the last progress status and error code.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt LastProgressError( const TUint&  aConnectionId, TNifProgress& aProgress );

        /**
         * Check if the WLAN connection received signal strength is strong
         * enough to maintain a decent quality connection.
         *
         * @since S60 v5.2
         * @return ETrue if RSS is strong enough, EFalse in any other situation
         * (weak signal, no WLAN connection, WLAN not supported... ).
         */
        TBool WlanRssGoodEnough();
        
        /**
         * Gets the network roaming status.
         * @since
         * @param aNetworkStatus On return, contains the current network registration status.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetNetworkRegistration_v2( TInt& aRegistration ) const;


    private: // New methods
        /**
         * Opens and attaches to a connection.
         * @since
         * @param aConnInfo  Connection info record.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt OpenAndAttachTo( TConnInfo& aConnInfo );

        /**
         * Stops a connection.
         * @since
         * @param aIndex  iConnInfos index for the connection.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt StopConnection( const TInt aIndex );

        /**
         * Creates a new unique connection id.
         * @since
         * @return New connection id.
         */
        TUint NewConnectionId();

        /**
         * Gets the iConnInfos array index for a connection id.
         * @since
         * @param aConnectionId  Connection id.
         * @return iConnInfos index for connection id.
         */
        TInt Index( const TUint& aConnectionId );

        /**
         * Starts connection specific notifications (status, data volume, activity).
         * @since
         * @param aIndex  iConnInfos index for the connection.
         * @return void
         */
        void AddConnNotificationsL( const TInt& aIndex );

        /**
         * Stops connection specific notifications (status, data volume, activity).
         * @since
         * @param aIndex  iConnInfos index for the connection.
         * @return void
         */
        void DeleteConnNotifications( const TInt& aIndex );

        /**
         * Stops all the notifications.
         * @since
         * @return void
         */
        void DeleteNotifications();

        /**
         * Gets the access point name from commDB.
         * @since
         * @param aConnectionId  Connection id.
         * @param aName On return, contains the access point name.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetAccessPointName( const TUint& aConnectionId, TDes& aName );

        /**
         * Gets the telephone number from commDB.
         * @since
         * @param aConnectionId  Connection id.
         * @param aTelNum On return, contains the tel number.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetTelNumber( const TUint& aConnectionId, TDes& aTelNum );

        /**
         * Gets the connection status.
         * @since
         * @param aIndex Index of the connection info in iConnInfos array.
         * @param aConnectionStatus On return, contains the status of the connection.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetConnectionStatus( const TInt& aIndex, TInt& aConnectionStatus );

        /**
         * Gets the packet network status.
         * @since
         * @param aNetworkStatus On return, contains the current psd network status.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetPsdNetworkStatus( TInt& aNetworkStatus ) const;

        /**
         * Gets the network registration status.
         * @since
         * @param aNetworkStatus On return, contains the current network registration status.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetNetworkRegistration( TInt& aRegistration ) const;

        /**
         * Gets the GSM signal strength.
         * @since
         * @param aSignalStrength On return, contains the GSM signal strength in dBs.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetGSMSignalStrength( TInt& aSignalStrength ) const;

        /**
         * Gets the protocol type.
         * @since
         * @param aProtocolType On return, contains type the protocol (ip4,ip6).
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetProtocolTypeL( const TUint& aConnectionId, TInt& aProtocolType );

        /**
         * Gets the connection activity information asynchronously.
         * @since
         * @param aIndex Index of the connection info in iConnInfos array.
         * @return void
         */
        void GetActivityOneShotL( const TInt& aIndex );

        /**
         * Gets the bearer info CSD/HSCSD from ETel.
         * @since
         * @param aNumber The dial up number of the call that will be used
         * to match RConnection object to ETel object.
         * @param aBearer On return, contains the bearer.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt GetBearerFromETel(
                const TDes& aNumber,
                TInt& aBearer,
                TBearerInfo& aBearerInfo );

        /**
         * Compares two access point names.
         * @since
         * @param aNameFromCommDB The access point name (UNICODE) from commDB.
         * @param aNameFromETel   The access point name (UNICODE) that ETel object is using.
         * @return 0 if access point names are equal.
         */
        TInt CompareAccessPointNames(
                const TDesC& aNameFromCommDB,
                const TDesC& aNameFromETel ) const;

        /**
         * Compares two access point names.
         * @since
         * @param aNameFromCommDB The access point name (UNICODE) from commDB.
         * @param aNameFromETel   The access point name (UTF8) that ETel object is using.
         * @return 0 if access point names are equal.
         */
        TInt CompareAccessPointNames(
                const TDesC& aNameFromCommDB,
                const TDesC8& aNameFromETel ) const;

        /**
         * In case that a new connection has arrived with the same IAP
         * id as one in the connection info array (that is closing but
         * some event has not yet occured) the previous connection is
         * removed and the needed event are sent.
         * @since
         * @param aIndex Index number in connection info array.
         */
        void CompleteConnectionClosing( TInt& aIndex );

        /**
         * Takes a connection specific or a bearer specific connection id and
         * calculates the respective bearer specific connection id.
         * @since MCL 9.1
         * @param aConnId The connection id.
         * @param aBearerId On return, contains the propper bearer specific connection id.
         * @return KErrNone if successfull, otherwise a system wide error code.
         */
        TInt BearerIdForBearerAvailability( const TUint& aConnId, TUint& aBearerId );

    private: // Data
        // Array that stores connection information records
        RArray<TConnInfo> iConnInfos;

        // Pointer to server module
        CConnMonServer* iServer;

        // Handle to socket server
        RSocketServ iSocketServer;

        // Handle to RConnection
        RConnection iConnection;

        // Used when allocating new connection ids
        TUint iIdCounter;

        // Handle to the telephony server
        RTelServer iTelServer;

        // Handle to the packet service
        RPacketService iPacketService;

        // RMobilePhone handle
        RMobilePhone iMobilePhone;

        // Pointer to CSD fax handler module
        CCsdFax* iCsdFax;

        // Pointer to PSD fax handler module
        CPsdFax* iPsdFax;

        // Connection up/down event watcher
        CConnUpDownNotifier* iConnUpDownNotifier;

        // PSD network status event watcher
        CNetwStatusNotifier* iPsdNetwStatusNotifier;

        // Network registration status event watcher
        CNetwRegistrationNotifier* iNetwRegistrationNotifier;

        // Bearer change (GPRS/Edge GPRS) event watcher
        CBearerNotifier* iEdgeNotifier;

        // Bearer change (Wcdma/Hsdpa) event watcher
        CWcdmaBearerNotifier* iWcdmaNotifier;

        // Mode change (2G <-> 3G)
        CModeNotifier* iModeNotifier;

        // GSM signal strength event watcher
        CGsmSignalNotifier* iGSMSignalNotifier;

        // GPRS / CSD bearer availability changed event watcher
        CBearerAvailabilityNotifier* iGSMBearerAvailabilityNotifier;

        // Number of clients that are listening to events
        TUint8 iCountListeners;

        // 1 = phone tsy is loaded, 0 = tsy is not loaded
        TUint8 iTSYLoaded;

        // 1 = packet services are loaded, 0 = not loaded
        TUint8 iPacketServLoaded;

        // End user LAN network selection
        TBuf8<KCommsDbSvrMaxFieldLength> iEndUserNetIds;

        // Pointer to WLan management service
        CWlanSupport* iWlanSupport;

        // Phone call status watcher.
        CConnMonTelNotifier* iTelephonyNotifier;

        // DTM status watcher.
        CConnMonDtmNotifier* iDualTransferModeNotifier;
    };

#endif // CONNMONIAP_H

// End-of-file
