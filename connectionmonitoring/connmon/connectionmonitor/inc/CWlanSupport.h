/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides an interface to WLAN Engine.
*
*/

#ifndef CONNMONCWLANSUPPORT_H
#define CONNMONCWLANSUPPORT_H

#include <wlanmgmtclient.h>
#include <wlanscaninfo.h>
#include <commsdat.h>
#include <wlancontainer.h>
#include <rconnmon.h>

#include "ConnMonServ.h"
#include "connmonwlannetwork.h"


// Information Element ID for SSID as in 802.11.
const TUint8 KWlan802Dot11SsidIE = 0;

// Information Element ID for Rsn as in 802.11
const TUint8 KWlan802Dot11RsnIE = 48;

// Information Element ID for Wpa as in 802.11
const TUint8 KWlan802Dot11WpaIE = 221;

// Information Element ID for Vendor data as in 802.11
const TUint8 KWlan802Dot11VendorSpecificIE = 221;

// Bit mask for Capability info field to get type (Infra/AdHoc).
const TUint8 KWlan802Dot11CapabilityEssMask = 0x0001;

// Bit mask for Capability info field to get type (Infra/AdHoc).
const TUint8 KWlan802Dot11CapabilityPrivacyMask = 0x0010;

// Control header length for a beacon raw buffer in CWlanScanInfo.
const TInt KWlanProbeControlHeaderLength = 12;

enum TConnMonWlanRssClass
     {
     EConnMonWlanRssClassUnknown = 0,
     EConnMonWlanRssClassWeak,
     EConnMonWlanRssClassNormal
     };

class CConnMonServer;
class CConnMonSession;
class CWlanEvent;
class CWlanSession;
class CWlanGetScanResults;
class CWlanGetSsidScanResults;


/**
* CWlanSupport
*/
NONSHARABLE_CLASS( CWlanSupport ) : public CBase
    {
public:
    static CWlanSupport* NewL( CConnMonServer* aServer );
    virtual ~CWlanSupport();

public:
    /**
     * Gets the WLAN bearer availability information.
     * @param aAvailability On return, contains WLAN availability (yes/no) information.
     * @param aDoScan If this is set to ETrue scan results will be used instead of event info.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    void GetBearerAvailabilityL( TBool& aAvailability );

    TInt GetBearerAvailabilityL( const RMessage2& aMessage, TBool& aAvailability );

    /**
     * Gets the the signal strength of the active WLAN connection.
     * When there are no connections, gets the signal strength of
     * the best available WLAN.
     * @param aSignalStrength On return, contains the signal strength.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetSignalStrengthL( const RMessage2& aMessage, TInt& aSignalStrength );

    /**
     * Gets information on available WLAN networks (SSID, signal strength, type).
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetNetworkNamesL( const RMessage2& aMessage );

    /**
     * Parse WLAN network names from scan results.
     * @since 5.0
     * @param aScanInfo Contains WLAN scan results
     * @param aNetInfo On return, contains information on available WLAN networks.
     */
    void ParseNetworkNamesL( CWlanScanInfo* aScanInfo, TConnMonNetworkNames& aNetInfo );

    /**
     * Appends available WLAN IAPs.
     * @param aIapBuf On return, contains the available WLAN IAPs.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt AppendAvailableIaps( RArray<TUint>& aIdArray );

    /**
     * When a WLAN connection is up, appends all WLAN IAP IDs that have the same
     * SSID as the current connection (SSID sharing), to the array given as
     * argument. IAP info is read from CommsDat.
     * @since 3.2
     * @param aIapBuf On return, contains the available WLAN IAP IDs.
     * @return KErrNone when successfull, KErrDisconnected if there is no active
     * WLAN connection and otherwise a system wide error code.
     */
    TInt AppendAvailableIapsBySsidL( RArray<TUint>& aIdArray );

    /**
     * Gets network name as UNICODE.
     * @param aSsid On return, contains the SSID as UNICODE.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetConnectionSsid( TDes& aSsid );

    /**
     * Gets connection mode (e.g. infra/ad hoc).
     * @param aConnectionMode On return, contains the connection mode.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetConnectionMode( TInt& aConnectionMode );

    /**
     * Gets connection security mode.
     * @param aConnectionSecurity On return, contains the connection security mode.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetConnectionSecurity( TInt& aConnectionSecurity );

    /**
     * Gets connection signal quality.
     * @param aSignalQuality On return, contains the connection signal quality.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetConnectionSignalQuality( TInt& aSignalQuality );

    /**
     * Gets connection BSSID.
     * @param aBssid On return, contains the connection BSSID.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetConnectionBssid( TBuf8<CConnMonWlanNetwork::KWlanBssId>& aBssid );

    /**
     * Gets the used transmit power.
     * @param aPower On return, contains the used transmit power in mW.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetTransmitPower( TUint& aPower );

    /**
     * Activates WLAN Management API events.
     * @return void.
     */
    void ActivateNotificationsL();

    /**
     * Cancels WLAN Management API events.
     */
    void CancelNotifications();

    /**
     * Gets information on available WLAN networks (SSID, signal strength, type).
     * @param aWlanNetworks On return, contains information on available WLAN networks.
     */
    void GetWLANNetworksL( RConnMonWLANNetworksArray& aWlanNetworks );

    /**
     * Gets information on available WLAN networks (SSID, signal strength, type, BssId).
     * @since 3.2
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetWlanNetworksL( const RMessage2& aMessage );

    /**
     * Parse WLAN networks information (SSID, signal strength, type, BssId).
     * @since 5.0
     * @param aScanInfo Contains WLAN scan results
     * @param aWlanNetworks On return, contains information on available WLAN networks.
     */
    void ParseWlanNetworksL(
            CWlanScanInfo* aScanInfo,
            RConnMonWlanNetworksPtrArray& aWlanNetworks );

    /**
     * Ordering method of TConnMonNetwork class objects.
     * @param aFirst First object for comparison
     * @param aSecond Second object for comparison
     * @return -1 if aFirst.iSignalStrength >  aSecond.iSignalStrength
     *          1 if aFirst.iSignalStrength <  aSecond.iSignalStrength
     *          0 if aFirst.iSignalStrength == aSecond.iSignalStrength
     */
    static TInt CompareNetworkStrengths(
            const TConnMonNetwork& aFirst,
            const TConnMonNetwork& aSecond );

    /**
     * Gets current WLAN networks information (SSID, signal strength, type, BssId).
     * @since 5.0
     * @param aWlanNetwork On return, contains information on current WLAN network.
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetCurrentWlanNetworkL( RConnMonWlanNetworksPtrArray& aWlanNetwork );

    /**
     * Gets WLAN probe raw buffers.
     * @since 5.0
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetWlanProbeRawBuffersL( const RMessage2& aMessage );

    /**
     * Parse WLAN networks probe raw buffers.
     * @since 5.0
     * @param aScanInfo Contains WLAN scan results
     * @param aWlanProbeRawBuffers On return, contains WLAN probe raw buffers.
     */
    void ParseWlanProbeRawBuffersL(
            CWlanScanInfo* aScanInfo,
            RConnMonWlanProbeRawBuffersPtrArray& aWlanProbeRawBuffers );

    /**
     * Gets SSID specific WLAN network information (SSID, signal strength, type, BssId).
     * @since 5.0
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetWlanSsidNetworksL( const RMessage2& aMessage );

    /**
     * Enables WLAN scan for the next iap availability query.
     */
    void EnableWlanScan() { iEnableScan = ETrue; };

    /**
     * Check WLAN session index.
     * @since 5.0
     * @param aSessionId Session specific ID to compare array IDs
     * @return index number or system wide error code.
     */
    TInt WlanSessionIndex( CConnMonSession* aSessionId );

    /**
     * Add WLAN session.
     * @since 5.0
     * @param aMessage Client message
     * @return index number for the session in the WLAN sessions array
     */
    TInt AddWlanSessionL( const RMessage2& aMessage );

    /**
     * Delete WLAN session.
     * @since 5.0
     * @param aMessage Client message
     */
    void DeleteWlanSession( CConnMonSession* aSessionId );

    /**
     * Sets WLAN TInt attribute (KWlanScanCacheLifeTime).
     * @since 5.0
     * @param aAttribute Attribute to be set
     * @param aValue Value to be set
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt SetIntAttributeL( const RMessage2& aMessage, const TInt aValue );

    /**
     * Sets WLAN TUint attribute (KWlanScanMaxDelay).
     * @since 5.0
     * @param aAttribute Attribute to be set
     * @param aValue Value to be set
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt SetUintAttributeL( const RMessage2& aMessage, const TUint aValue );

    /**
     * Gets WLAN TInt attribute (KWlanScanCacheLifeTime).
     * @since 5.0
     * @param aAttribute Attribute to be get
     * @param aValue On return contains the value of attribute
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetIntAttributeL( const RMessage2& aMessage, TInt& aValue );

    /**
     * Gets WLAN TUInt attribute (KWlanScanMaxDelay).
     * @since 5.0
     * @param aAttribute Attribute to be get
     * @param aValue On return contains the value of attribute
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetUintAttributeL( const RMessage2& aMessage, TUint& aValue );

    /**
     * Sets WLAN String attribute (KWlanSsid etc).
     * @since 5.0
     * @param aAttribute Attribute to be set
     * @param aValue Value to be set
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt SetStringAttributeL( const RMessage2& aMessage, const TDes& aValue );

    /**
     * Gets WLAN String attribute (KWlanSsid etc).
     * @since 5.0
     * @param aAttribute Attribute to be get
     * @param aValue Value to be get
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt GetStringAttributeL( const RMessage2& aMessage, TDes& aValue );

    /**
     * Cancel all client asynch requests for WLAN engine.
     * @since 5.0
     * @param aMessage Client message
     * @return KErrNone when successfull, otherwise a system wide error code.
     */
    TInt CancelAsyncRequest( const RMessage2& aMessage );

    /**
     * Enables sending of WLAN events to clients. Set when there is a client
     * listening to events.
     *
     * @since S60 v5.2
     */
    void EnableEventsToClients();

    /**
     * Disables sending of WLAN events to clients. Set when there is no longer
     * any clients listening to events.
     *
     * @since S60 v5.2
     */
    void DisableEventsToClients();

    /**
     * Checks if sending of events to clients is enabled.
     *
     * @since S60 v5.2
     * @return ETrue if events are enabled, EFalse otherwise.
     */
    TBool EventsToClientsEnabled();

    /**
     * Checks the class of the WLAN received signal strength, based on WLAN
     * engine events. Returns EFalse only if a weak signal event has been
     * received from WLAN engine.
     *
     * @since S60 v5.2
     * @return EFalse if signal level is weak (for certain), ETrue otherwise.
     */
    TBool WlanRssGoodEnough();

private:
    CWlanSupport( CConnMonServer* aServer );

    /**
     * Second phase construction.
     */
    void ConstructL();
    
    /*
     * Converts WLAN's security modes into ConnMon's.
     * @since 5.2
     * @param scanInfo Wlan scan info
     * @return Connection monitor's security mode (TConnMonSecurityMode).
     */
    TUint ConvertWlan2ConnMonExtSecMode(CWlanScanInfo* scanInfo);

private: // Data
    // Pointer to server module
    CConnMonServer* iServer;

    // WLAN Management API handle
    CWlanMgmtClient* iWlanMgmt;

    // WLAN event watcher object
    CWlanEvent* iEventWatcher;

    // Stores available WLAN iaps until next scan
    RArray<TUint> iAvailableWlanIapsCache;

    // Scan is allowed when ETrue. Initialized to ETrue in constructor.
    TBool iEnableScan;

    // Pointer table to session specific WLAN data objects
    RPointerArray<CWlanSession> iWlanSessionsPtrArray;

    // Event sending status
    TBool iEnableEventsToClients;
    };

/**
* CWlanEvent
*/
NONSHARABLE_CLASS( CWlanEvent ) : public CBase, public MWlanMgmtNotifications
    {
public:
    static CWlanEvent* NewL( CWlanSupport* aWlanSupport, CConnMonServer* aServer );
    virtual ~CWlanEvent();

public:
    TBool WlanAvailability() { return iWlanAvailable; }
    TBool HasAvailabilityKnowledge() { return iHasAvailabilityKnowledge; }
    TUint TxPower() { return iTxPower; }
    TUint RssClass() { return iRssClass; }
    TUint Rss() { return iRss; }

public: // From MWlanMgmtNotifications
    /**
     * New networks have been detected during scan.
     */
    void NewNetworksDetected();

    /**
     * One or more networks have been lost since the last scan.
     */
    void OldNetworksLost();

    /**
     * The used transmit power has been changed.
     * @param aPower The transmit power in mW.
     */
    void TransmitPowerChanged( TUint aPower );

    /**
     * Connection status has changed.
     *
     * @since S60 v5.2
     * @param aNewState specifies the new connection state.
     */
    void ConnectionStateChanged( TWlanConnectionMode aNewState );

    /**
     * Received signal strength level has been changed.
     *
     * @since S60 v5.2
     * @param aRssClass specifies the current class of the received signal.
     * @param aRss RSS level in absolute dBm values.
     */
    void RssChanged( TWlanRssClass aRssClass, TUint aRss );

private:
    // Constructor
    CWlanEvent( CWlanSupport* aWlanSupport, CConnMonServer* aServer );

private: // Data
    // Pointer to WLAN support object
    CWlanSupport* iWlanSupport;

    // Pointer to server object
    CConnMonServer* iServer;

    // WLAN bearer availability information. Valid only after
    // 'iHasAvailabilityKnowledge' has been set to true.
    TBool iWlanAvailable;

    // When this flag is true, it means the 'iWlanAvailable' flag has a correct
    // value. The flag is set to true after the first NewNetworksDetected or
    // OldNetworksLost event is received.
    TBool iHasAvailabilityKnowledge;

    // The used transmit power in mW.
    TUint iTxPower;

    // The RSS class (unknown/weak/normal) for the current WLAN connection.
    TInt iRssClass;

    // The RSS (in absolute dBm) for the current WLAN connection. Updated by
    // events, so not always accurate (depends on event thresholds).
    TUint iRss;
    };


/**
* CWlanSession
* @since 5.0
*/
NONSHARABLE_CLASS( CWlanSession ) : public CBase
    {
public:
    static CWlanSession* NewL( CConnMonSession* aSessionId, CWlanSupport* aWlanSupport );
    virtual ~CWlanSession();

public:
    /**
     * Get client WLAN session ID.
     * @return CConnMonSession*.
     */
    inline CConnMonSession* GetSessionId() { return iSessionId; }

    /**
     * Set client WLAN session ID.
     * @param aSessionId The session ID of the client.
     */
    inline void SetSessionId( CConnMonSession* aSessionId ) { iSessionId = aSessionId; }

    /**
     * Get client WLAN scan max delay.
     * @return iWlanScanMaxDelay.
     */
    inline TUint GetWlanScanMaxDelay() { return iWlanScanMaxDelay; }

    /**
     * Set client WLAN scan max delay.
     * @param aWlanScanMaxDelay Client WLAN scan max delay.
     */
    inline void SetWlanScanMaxDelay( const TUint& aWlanScanMaxDelay )
        {
        iWlanScanMaxDelay = aWlanScanMaxDelay;
        }

    /**
     * Get client WLAN scan cache lifetime.
     * @return iWlanScanCacheLifetime.
     */
    inline TInt GetWlanScanCacheLifetime() { return iWlanScanCacheLifetime; }

    /**
     * Set client WLAN scan cache lifetime. This is used if KWlanScanMaxDelay is set to 0.
     * Any negative value will be changed to -1. This means WLAN engine will use the default
     * device setting for cache lifetime value.
     * @param aWlanScanCacheLifetime Client WLAN scan cache lifetime.
     */
    void SetWlanScanCacheLifetime( const TInt& aWlanScanCacheLifetime );

    /**
     * Get client WLAN SSID.
     * @param aWlanSsid On return contais client SSID.
     */
    inline void GetWlanSsid( TBuf8<CConnMonWlanNetwork::KMaxNameLength>& aWlanSsid )
        {
        aWlanSsid = iWlanSsid;
        }

    /**
     * Set client WLAN SSID.
     * @param aWlanSsid Client WLAN SSID to be set.
     */
    inline void SetWlanSsid( const TBuf8<CConnMonWlanNetwork::KMaxNameLength>& aWlanSsid )
        {
        iWlanSsid = aWlanSsid;
        }

    /**
     * Get WLAN scan result from WLAN engine.
     * @param aMessage Client request message.
     */
    void GetScanResultsL( const RMessage2& aMessage );

    /**
     * Get WLAN SSID scan result from WLAN engine.
     * @param aMessage Client request message.
     */
    TInt GetSsidScanResultsL( const RMessage2& aMessage );

    /**
     * Complete all pending asynch requests to client.
     * @param aStatus Status of the scan.
     * @param aScanInfo Object containing scan results.
     * @param aSsidScan Flag telling if it was SSID scan.
     */
    void CompleteActiveRequestsL(
            const TInt aStatus,
            CWlanScanInfo* aScanInfo,
            const TBool aSsidScan = EFalse );

    /**
     * Complete GetBearerAvailabilityRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetBearerAvailabilityRequest(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Complete GetSignalStrengthRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetSignalStrengthRequest(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Complete GetNetworkNamesRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetNetworkNamesRequestL(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Complete GetWlanNetworksRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetWlanNetworksRequestL(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Complete GetWlanSsidNetworksRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetWlanSsidNetworksRequestL(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Complete GetWlanProbeRawBuffersRequest to client.
     * @param aScanInfo Object containing scan results.
     * @param aIndex Index of the pending message table.
     */
    void CompleteGetWlanProbeRawBuffersRequestL(
            CWlanScanInfo* aScanInfo,
            const TUint aIndex );

    /**
     * Cancel active asynch requests.
     * @param aReqType Request type to be cancelled
     */
    void CancelActiveRequests( const TInt aReqType );

    /**
     * Completes all wlan requests of certain type with the given error code.
     * @since S60 5.2
     * @param aErrorCode The error code.
     * @param aSsidScan Flag to distinguish between SSID scan based requests
     * and normal scan based requests.
     */
    void CompleteActiveRequestsWithError(
            const TInt aErrorCode,
            const TBool aSsidScan = EFalse );

private:
    // Constructor
    CWlanSession( CConnMonSession* aSessionId, CWlanSupport* aWlanSupport );

private: // Data
    // Session ID
    CConnMonSession* iSessionId;

    // Pointer to WLAN support object
    CWlanSupport* iWlanSupport;

    // GetScanResults AO
    CWlanGetScanResults* iGetScanResultsAO;

    // GetSsidScanResults AO
    CWlanGetSsidScanResults* iGetSsidScanResultsAO;

    // WLAN scan max delay
    TUint iWlanScanMaxDelay;

    // WLAN scan cache lifetime
    TInt iWlanScanCacheLifetime;

    // WLAN SSID descriptor
    TBuf8<CConnMonWlanNetwork::KMaxNameLength> iWlanSsid;

    // Table for pending async. requests
    RArray<RMessage2> iWlanRequestsArray;
    };


/**
* CWlanGetScanResults
* @since 5.0
*/
NONSHARABLE_CLASS( CWlanGetScanResults ) : public CActive
    {
public:
    static CWlanGetScanResults* NewL( CWlanSession* aWlanSession );
    ~CWlanGetScanResults();

public:
    /**
     * Requests a new event (scan done) from WLAN.
     * @since 5.0
     */
    void StartL();

private:
    // Constructor
    CWlanGetScanResults( CWlanSession* aWlanSession );

    // Second phase constructor
    void ConstructL();

private:
    /**
     * From CActive Cancels the asyncronous request from WLAN engine
     * @since 5.0
     */
    void DoCancel();

    /**
     * From CActive Handles the the event that has arrived from WLAN.
     * @since 5.0
     */
    void RunL();

    /**
     * From CActive, handles leaves from RunL.
     * @since S60 5.2
     * @param aLeaveCode The leave code.
     */
    TInt RunError( TInt aLeaveCode );

private: // Data
    // WlanSession pointer
    CWlanSession* iWlanSession;

    // WLAN Management API handle
    CWlanMgmtClient* iWlanMgmt;

    // Scan info
    CWlanScanInfo* iScanInfo;
    };


/**
* CWlanGetSsidScanResults
* @since 5.0
*/
NONSHARABLE_CLASS( CWlanGetSsidScanResults ) : public CActive
    {
public:
    static CWlanGetSsidScanResults* NewL( CWlanSession* aWlanSession );
    ~CWlanGetSsidScanResults();

public:
    /**
     * Requests a new event (scan done) from WLAN.
     * @since 5.0
     * @param
     */
    void StartL( TBuf8<CConnMonWlanNetwork::KMaxNameLength>& aWlanSsid );

private:
    // Constructor
    CWlanGetSsidScanResults( CWlanSession* aWlanSession );

    // Second phase constructor
    void ConstructL();

private:
    /**
     * From CActive Cancels the asyncronous request from WLAN engine
     * @since 5.0
     */
    void DoCancel();

    /**
     * From CActive Handles the the event that has arrived from WLAN.
     * @since 5.0
     */
    void RunL();

    /**
     * From CActive, handles leaves from RunL.
     * @since S60 5.2
     * @param aLeaveCode The leave code.
     */
    TInt RunError( TInt aLeaveCode );

private: // Data
    // WlanSession pointer
    CWlanSession* iWlanSession;

    // WLAN Management API handle
    CWlanMgmtClient* iWlanMgmt;

    // Scan info
    CWlanScanInfo* iScanInfo;
    };

#endif // CONNMONCWLANSUPPORT_H

// End-of-file
