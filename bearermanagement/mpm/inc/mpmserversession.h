/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: MPM server session class definitions
*
*/

/**
@file mpmserversession.h
Mobility Policy Manager server session class definitions.
*/

#ifndef MPMSERVERSESSION_H
#define MPMSERVERSESSION_H

//  INCLUDES
#include <e32base.h>
#include <commdbconnpref.h> // for TCommDbConnPref
#include <connpref.h>       // for TConnPref
#include <rconnmon.h>       // for TConnMonIapInfo
#include <ConnectionUiUtilities.h>
#include "rmpm.h"
#include "mpmserver.h"

// CONSTANTS
const TUint32 KLowestPriority       = 2147483647; // 0x7FFFFFFF
const TInt    KFirstArgument        = 0;
const TInt    KSecondArgument       = 1;
const TInt    KThirdArgument        = 2;
const TInt    KFourthArgument       = 3;
const TInt    KShift8               = 8;
const TInt    KSmaller              = -1;
const TInt    KEqual                = 0;
const TInt    KBigger               = 1;
const TUint32 KUidAlwaysOnlineStarter  = 0x101F85EE;
const TUint32 KUidSimApplicationToolkit  = 0x1000A833;
const TUint32 KUidBrowser           = 0x10008D39;
const TUint32 KUidDVBH  = 0x10208413;
const TUint32 KSortUncategorisedIaps  = 0;
const TUint   KMPMNrWlansOne        = 1;
const TUint   KMPMNrWlansTwo        = 2;
const TInt    KMaxGetIntSettingLength = KCommsDbSvrMaxColumnNameLength * 2;
_LIT( KIapProxyServiceSetting, "IAP\\IAPService" );

// FORWARD DECLARATIONS
class CMPMConfirmDlgRoaming;
class CMPMCommsDatAccess;
class CMPMIapSelection;

// CLASS DECLARATION
/**
*  TNetIap
*  Record for commdb IAP table search storage
*  @lib MPMServer.exe
*  @since 3.0
*/
class TNetIap
    { 
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        TNetIap();

    public: // New methods

        /**
        * Ordering method of class objects.
        * @since 3.2
        * @param aFirst First object for comparison
        * @param aSecond Second object for comparison
        * @return -1 if aFirst.iRanking >  aSecond.iRanking
        *          1 if aFirst.iRanking <  aSecond.iRanking
        *          0 if aFirst.iRanking == aSecond.iRanking
        */
        static TInt CompareRanking(const TNetIap& aFirst, const TNetIap& aSecond);


        /**
        * Ordering method of class objects. Compares iGlobalPriority and if those values 
        * are equal return value is determined based on iRanking by CompareRanking function.
        * @since 3.2
        * @param aFirst First object for comparison
        * @param aSecond Second object for comparison
        * @return -1 if aFirst is higher
        *          1 if aFirst is lower
        *          0 if argument objects are equal
        */
        static TInt CompareGlobalAndLocalPriority(const TNetIap& aFirst, const TNetIap& aSecond);

    public: // Data
        // SNAP
        TUint32 iSnap;
        
        // Embedded SNAP
        TUint32 iEmbeddedSnap;
        
        // IAP Id
        TUint32 iIapId;

        // Priority
        TUint32 iRanking;

        // Global bearer priority
        TUint32 iGlobalPriority;
    };

/**
* Stores data of whether Preferred IAP Notifications may 
* be sent immediately and stores IAP information for later handling.
* 
* @lib MPMServer.exe
* @since 3.2
*/
class TStoredIapInfo
    {
    public:
        /**
        * Constructor.
        */
        TStoredIapInfo();
        
        /**
        * Checks if Preferred IAP notification can 
        * be sent immediately or not
        *
        * @return ETrue if preferred IAP notification 
        * can't be sent immediately
        */
        TBool HoldPrefIapNotif() const;

        /**
        * Sets iHoldPrefIapNotif to true, so that notifications 
        * won't be sent.
        *
        */
        void SetHoldPrefIapNotif();

        /**
        * Checks if there is stored IAP info to be handled.
        * Sets iHoldPrefIapNotif to false so notifications can be sent. 
        *
        * @param aStoredIapInfo If stored IAP info exists it is 
        * written to this argument.
        * @return ETrue if IAP info to be handled exists
        */
        TBool HandleIapInfoWaiting( TConnMonIapInfo& aStoredIapInfo );

        /**
        * Sets iStoredIapInfo 
        *
        * @param aStoredIapInfo IAP info to be stored
        */
        void SetStoredIapInfo( const TConnMonIapInfo& aStoredIapInfo );

        /**
        * Clears data so that Pref IAP notifs can be sent 
        * and no IAP info is stored.
        *
        */
        void ResetStoredIapInfo();

    private:
    
        // True if Preferred IAP Notifications 
	    // should be stored for later handling
	    TBool iHoldPrefIapNotif;
	    
	    // True if there is an IAP info waiting 
	    // for handling
	    TBool iIapInfoWaiting;
	    
	    // Stored IAP info
	    TConnMonIapInfo iStoredIapInfo;
    };

// States for migrating
//
enum TMigrateToCarrierState
    {
    EMigrateNone,
    EMigrateUserConfirmation,
    EMigrateOfflineConfirmation
    };
    
// CLASS DECLARATION
/**
*  Mobility Policy Manager server session.
*  An instance of class CMPMServerSession is created for each client
*  @lib MPMServer.exe
*  @since 3.0
*/
class CMPMServerSession : public CSession2
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPMServerSession* NewL(CMPMServer& aServer);

        /**
        * Destructor.
        */
        virtual ~CMPMServerSession();

    public: // New methods

        /**
        * Sets iDisconnectDlg pointer to NULL.
        * @since 3.1
        */
        inline void SetDisconnectDlgPtrNull();

        /**
        * Sets iConfirmDlgRoaming pointer to NULL.
        * @since 3.2
        */
        inline void SetConfirmDlgRoamingPtrNull();

        /**
        * Gets the value of iMyServer.
        * @since 3.1
        * @return Value of iMyServer
        */
        inline CMPMServer& MyServer();

        /**
        * Returns iIapSelection.
        *
        * @since 3.2
        * @return Value of iIapSelection
        */
        CMPMIapSelection* IapSelectionL();

        /**
        * Gets the value of iConnId.
        * @since 3.1
        * @return Value of iConnId
        */
        inline TConnectionId ConnectionId() const;

        /**
        * Checks the need for preferred IAP notification.
        * @since 3.1
        * @param aIapInfo Info about available IAPs
        * @param aCaller Info about component which initiated this call
        */
        void PrefIAPNotificationL( const TConnMonIapInfo&    aIapInfo,
                                   const TPrefIAPNotifCaller aCaller );

        /**
        * Starts sending preferred IAP notification.
        * @since 3.2
        * @param aIapId IAP Id 
        */
        void StartIAPNotificationL( const TUint32 aIapId );

        /**
        * Sends Stop IAP notification.
        * @param aIapId IAP Id 
        */
        void StopIAPNotificationL( TInt aIapId = 0 );

        /**
        * Sends mobility error notification.
        * @since 5.2
        * @param aError Error code
        */
        void MobilityErrorNotificationL( TInt aError );
        
        /**
        * Sends client error notification.
        * @since 5.2
        * @param aError Error code
        */
        void ClientErrorNotificationL( TInt aError );

        /**
        * Returns the list of available IAPs, where the blacklisted IAPs 
        * have already been removed.
        * @since 3.2
        * @param aAvailableIAPs List of IAPs 
        * @param aConnId Connection Id
        */
        void AvailableUnblacklistedIapsL( RAvailableIAPList&  aAvailableIAPs,
                                          const TConnectionId aConnId );

        /**
        * Completing sorting request.
        * @since 3.2
        */
        void CompleteServerSortSNAP();

        /**
        * Completing process error call after WLAN scan.
        * @since 3.2
        */
        void ProcessErrorWlanScanCompletedL();

        /**
        * Sort Snap functionality
        *
        * @since 3.2
        * @param aSnapId Snap to be sorted
        * @param aSortedIaps Buffer which will be set to 
        * contain the sorted iaps
        */
        void SortSnapL( const TUint32   aSnapId, 
                        TMpmSnapBuffer& aSortedIaps );


        /**
        * Returns a reference to iStoredIapInfo. 
        *
        * @since 3.2
        * @return Reference to iStoredIapInfo
        */        
        inline TStoredIapInfo& StoredIapInfo();

        /**
        * Completes carrier rejected request. Attempts to select
        * new IAP to use and if successful sends a preferred iap
        * notification. If not successful, sends error notification. 
        *
        * @since 3.2
        */        
        void CompleteCarrierRejected();

        /**
        * Handling of whether it is required to display 
        * a confirmation dialog or not.
        * @since 3.2
        * @param aIapId IAP Id of the connection.
        */
        TBool IsConfirmFirstL( const TUint32 aIapId );

        /**
        * Completing choose iap request. Status and resulting
        * preferences (if preferences exist) are passed back to client.
        * @since 3.2
        * @param aError Message status
        * @param aPolicyPref Policy preference to be returned, NULL if no 
        * preference should be passed
        */
        void ChooseIapComplete( TInt                aError,
                                const TMpmConnPref* aPolicyPref );

        /**
        * Completing process error request. Error value and needed action
        * are passed back to client.
        * @since 3.2
        * @param aError Message status
        * @param aErrorReturned Error value to be returned, NULL if no 
        * value should be passed
        * @param aNeededAction Needed action to be returned, NULL if no 
        * value should be passed
        */
        void ProcessErrorComplete( TInt             aError, 
                                   TInt*            aErrorReturned,
                                   TBMNeededAction* aNeededAction );

        /**
        * Callback for handling migrate state transitions.
        *
        * @param aError Error code
        */
        void MigrateCallbackL( TInt aError );

        /**
        * Callback for roaming user confirmation.
        *
        * @param aError Error code
        * @param aResponse User response
        * @param aReconnect True if confirmation was a reconnect dialog
        */
        void RoamingConfirmationCompletedL( TInt aError, TMsgQueryLinkedResults aResponse, TBool aReconnect );
        
        /**
         * Set user connection flag
         * @since 5.1
         */
        inline void SetUserConnection();
        
        /**
         * Set when this session has user connection flag set
         * @since 5.1
         * @return ETrue if user connection is active in this session.
         */
        inline TBool UserConnection() const;
        
        /**
         * Clear user connection flag
         * @since 5.1
         */        
        inline void ClearUserConnection();

        /**
         * Use user connection preferences
         * @since 5.1
         */
        TBool UseUserConnPref();
        
        /**
         * Returns VPN user connection usage status.
         * @return ETrue if VPN user connection is used in this session.
         */
        inline TBool VpnUserConnectionUsed() const;

        /**
         * Sets VPN user connection usage status.
         * @param aEnabled Informs if VPN user connection is used.
         */
        void SetVpnUserConnectionUsed( const TBool aEnabled );
        
        /**
        * Returns id of the client.
        *
        * @since 5.0
        * @return Id of the client
        */
        inline TUint32 AppUid() const;

        /**
        * To determine whether choose best iap is called for this ession..
        *
        * @since 5.2
        * @return True is choose best iap is called for this session
        */
        inline TBool ChooseBestIapCalled() const;

#ifdef _DEBUG
        /**
         * Forces MPM to send StopIAPNotification in DEBUG MODE only
         */
        void HandleDebugGenerateStopIAPNotification( const RMessage2& aMessage );
        /**
         * Forces MPM to filter connmon events
         */
        void HandleDebugFilterConnMonEvents( const RMessage2& aMessage, const TBool aVal );       
        /**
         * Forces MPM to shutdown
         */
        void HandleDebugShutdown( const RMessage2& aMessage, const TBool aVal );
#endif //_DEBUG

        /**
         * Stops connection
         */                
        TInt StopConnection();
        
        /**
        * Returns true if preferred iap notification is requested.
        *
        * @since 5.2
        * @return Ture or false depending on state.
        */
        inline TBool PreferredIapRequested() const;
        
        /**
        * To determine whether only WLAN is allowed in current network
        *
        * @since 5.2
        * @param aNewWlansAllowed ETrue if new WLANs can be prompted to the user by MPM
        * @return ETrue if only WLAN is allowed 
        */
        TBool IsWlanOnlyL( TBool& aNewWlansAllowed );
        
        /**
        * Returns custom preference's forced roaming value
        * @since 5.2
        * @return forced roaming state
        */
        TBool ForcedRoaming();

        /**
         * Get available iaps
         * @since 5.2
         * @return Returns list of available iaps.
         */
        TConnMonIapInfo GetAvailableIAPs();

        /**
         * Is bearer accepted
         * @since 5.2
         * @param aBearerType Bearer type to be checked (WLAN or packet data)
         * @return Returns true if given bearer is accepted by this session.
         */
        TBool IsBearerAccepted( TMPMBearerType aBearerType );
        
        /**
        * To determine whether IAP is used for MMS
        *
        * @since 5.2
        * @param aIap Iap to be checked
        * @return ETrue if Iap is used for MMS 
        */
        TBool IsMMSIap( TUint32 aIap );
        
    public: // Functions from base classes

        /**
        * From CSession2. Service dispathcer.
        * @since 3.0
        * @param aMessage Message from the client
        */
        void ServiceL(const RMessage2& aMessage);

        /**
        * From CSession2. Service error handling.
        * @since 3.2
        * @param aMessage Message from the client
        * @param aError Error code
        */
        void ServiceError(const RMessage2& aMessage, TInt aError);

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CMPMServerSession(CMPMServer& aServer);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL() ;

    private: // New methods

        /**
        * Handling of IAP selection.
        * @since 3.0
        * @param aMessage message from client
        */
        void HandleServerChooseIapL(const RMessage2& aMessage);

        /**
        * Handling of request canceling.
        * @since 3.0
        * @param aMessage message from client
        */
        void HandleServerCancelRequest(const RMessage2& aMessage);

        /**
        * Handling of BM application specific connection addition.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationJoinsConnection( 
                const RMessage2& aMessage);

        /**
        * Handling of BM application specific connection removal.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerApplicationLeavesConnection(
                const RMessage2& aMessage);

        /**
        * Handling of total BM IAP connection addition.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerIapConnectionActivatedL(const RMessage2& aMessage);

        /**
        * Handling of total BM IAP connection addition.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerIapConnectionStartedL(const RMessage2& aMessage);

        /**
        * Handling of total BM IAP connection removal.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerIapConnectionStopped( const RMessage2& aMessage );

        /**
        * Handling of application connection ending.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationConnectionEnds(
                                            const RMessage2& aMessage );
        /**
        * Handling of error processing.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerProcessErrorL(const RMessage2& aMessage);
        
        /**
        * Handling of prefered IAP notification registration.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerRegisterPrefIAPNotifL(const RMessage2& aMessage);

        /**
        * Handling of preferred IAP notification unregistration.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerUnregisterPrefIAPNotif(const RMessage2& aMessage);

        /**
        * Handling of notification request.
        * @since 3.1
        * @param aMessage message from client
        */
        void HandleServerWaitNotificationL(const RMessage2& aMessage);

        /**
        * Handling of sorting request.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerSortSNAPL(const RMessage2& aMessage);

        /**
        * Handling of application migrating to carrier.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationMigratesToCarrierL(
                                                const RMessage2& aMessage);

        /**
        * Handling of application ignoring the carrier.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationIgnoredTheCarrier(
                                                const RMessage2& aMessage);

        /**
        * Handling of application accepting the carrier.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationAcceptedTheCarrier(
                                                const RMessage2& aMessage);

        /**
        * Handling of application rejecting the carrier.
        * @since 3.2
        * @param aMessage message from client
        */
        void HandleServerApplicationRejectedTheCarrierL(
                                                const RMessage2& aMessage);

        /**
        * Handling of ReselectBestIap call.
        * @since 9.1
        * @param aMessage message from client
        */
        void HandleServerReselectIapL( const RMessage2& aMessage);

        /**
        * Extracts connection preferences into format suitable for MPM
        * and validates that connection preferences are valid.
        * All different type of connection preferences are mapped and
        * converted into TMpmConnPref.
        * Only a a single TMpmConnPref is extracted from the given TConnPref.
        * That is, only one connection attempt is tried and thus only the
        * first preferences are taken if TCommDbMultiConnPref is provided.
        * This function also sets connection type which MPM uses later to 
        * select correct IAP in the requested manner.
        * @since 3.0
        * @param aBasePref Connection preferences to be extracted.
        * @param aMpmConnPref Extracted connection preferences.
        * @return KErrNone if successful, otherwise one of the
        * system-wide error codes
        */
        TInt ExtractConnPref(
            const TConnPref& aBasePref,
            TMpmConnPref& aMpmConnPref ) const;

        /**
        * Extracts TCommDBConnPref connection preferences into format suitable
        * for MPM and validates that connection preferences are valid.
        * @since 3.0
        * @param aBasePref TCommDBConnPref connection preferences.
        * @param aMpmConnPref Extracted connection preferences.
        * @return KErrNone if successful, otherwise KErrArgument.
        */
        TInt ExtractConnPrefCommDb(
            const TConnPref& aBasePref,
            TMpmConnPref& aMpmConnPref ) const;

        /**
        * Extracts TCommDbMultiConnPref connection preferences into format
        * suitable for MPM and validates that connection preferences are
        * valid.
        * @since 3.0
        * @param aBasePref TCommDbMultiConnPref connection preferences.
        * @param aMpmConnPref Extracted connection preferences.
        * @return KErrNone if successful, otherwise KErrArgument.
        */
        TInt ExtractConnPrefCommDbMulti(
            const TConnPref& aBasePref,
            TMpmConnPref& aMpmConnPref ) const;

        /**
        * Extracts TConnSnapPref connection preferences into format suitable
        * for MPM and validates that connection preferences are valid.
        * @since 3.0
        * @param aBasePref TConnSnapPref connection preferences.
        * @param aMpmConnPref Extracted connection preferences.
        * @return KErrNone if successful, otherwise one of the
        *         system-wide error codes.
        */
        TInt ExtractConnPrefSnap(
            const TConnPref& aBasePref,
            TMpmConnPref& aMpmConnPref ) const;

        /**
        * Extracts TMpmConnPref connection preferences into format suitable
        * for MPM and validates that connection preferences are valid.
        * It's obvious that aBasePref is already in right format.
        * This function, however, fills in some of the extra field not
        * existing in TExtendedConnPref and validates preferences.
        * @since 5.2
        * @param aBasePref TMpmConnPref connection preferences.
        * @param aMpmConnPref MPM connection preferences.
        * @param KErrNone in success, otherwise one of the
        *         system-wide error codes.
        */
        TInt ExtractConnPrefMpm(
            const TConnPref& aBasePref,
            TMpmConnPref& aMpmConnPref ) const;

        /**
        * Validates extended connection preferences.
        * Although the input for this function is TMpmConnPref,
        * the purpose of this function is to verify TExtendedConnPref
        * into which TMpmConnPref object's first fields match exactly.
        * @since 5.2
        * @param aMpmConnPref Custom preferences to be validated.
        * @return KErrNone if extended connection preferences are valid.
        *         Otherwise KErrArgument.
        */
        TInt ValidateExtendedConnPref( TMpmConnPref& aMpmConnPref ) const;

        /**
        * Resolves connection type based on the other attributes
        * in MPM connection preferences.
        * @since 3.0
        * @param aMpmConnPref Extracted connection preferences.
        */
        void ResolveConnType( TMpmConnPref& aMpmConnPref ) const;

        /**
        * Resolves original Service Id.
        * @since 5.2
        * @return The found service id. KErrNotFound if not ok.
        */
        TInt GetServiceIdSettingL();

        /**
        * Returns the list of available IAPs, where the blacklisted IAPs 
        * have already been removed.
        * @since 3.2
        * @param aAvailableIAPs List of IAPs 
        * @param aIapInfo Info about available IAPs
        * @param aConnId Connection Id
        */
        void AvailableUnblacklistedIapsL( RAvailableIAPList&        aAvailableIAPs,
                                          const TConnMonIapInfo&    aIapInfo, 
                                          const TConnectionId       aConnId );

        /**
        * Checks if the given Iap Id is an upgrade or downgrade.
        * @since 3.2
        * @param aIapId Given Iap Id 
        * @param aMigrateIapId Iap Id where application is roaming 
        * @param aAvailableIAPs List of IAPs 
        * return ETrue if upgrade, otherwise EFalse.
        */
        TBool IsUpgrade( const TUint32            aIapId, 
                         const TUint32            aMigrateIapId,
                         const RAvailableIAPList& aAvailableIAPs ) const;

        /**
        * Checks if there is a need to send preferred IAP notification.
        * @since 3.2
        * @param aCurrentIap Current Iap Id of connection
        * @param aLastNotifiedIap Last notified Iap Id
        * @param aValidatedIap Iap validated by MPM for roaming
        * @return ETrue if necessary, otherwise EFalse.
        */
        TBool CheckNotifNeedL( const TUint32       aCurrentIap,
                               const TUint32       aLastNotifiedIap,
                               const TUint32       aValidatedIap );

        /**
        * Returns the list of unavailable IAPs.
        * @since 3.2
        * @param aAvailableIAPs Array of available IAPs
        * @param aUnavailableIAPs Array of unavailable IAPs
        */
        void UnavailableIAPsL( const RAvailableIAPList aAvailableIAPs, 
                               RAvailableIAPList&      aUnavailableIAPs );

        /**
        * Removes unavailable IAP Id.
        * @since 3.2
        * @param aIapInfo Info about available IAPs 
        * @param aIapId IAP Id to be removed
        */
        void RemoveUnavailableIap( TConnMonIapInfo& aIapInfo, 
                                   const TUint32    aIapId );

        /**
        * Determines whether the process uid belongs 
        * to a background application.
        * @since 3.2
        * @param aUid Application Uid
        */
        TBool IsBackgroundApplication( TUint32 aUid ) const;
        
        /**
        * Checks if disconnect dialog should be displayed for this error
        * @since 3.2
        * @param aError Error code
        */
        TBool DisconnectDlgErrorCode( TInt aError ) const;
        
        /**
        * Gets presumed Iap Id of this connection
        *
        * @since 3.2
        * @return Presumed Iap Id
        */
        TUint32 GetPresumedIap();

        /**
        * Finshing migration. If no error, updates BM pref iap structure 
        * and sends start iap notification.
        *
        * @param aError KErrNone if migration was succesful
        */
        void MigrateDoneL( TInt aError );
        
        /**
        * Sends error notification.
        * @since 5.2
        * @param aNotificationType Type of the notification
        * @param aError Error code
        */
        void ErrorNotificationL( TInt aError,
                                 TMpmNotifications aNotificationType );
        
        /**
         * Removes iaps not according to bearer set
         * @since 5.2
         * @param aAvailableIAPs List of available iaps.
         */
        void RemoveIapsAccordingToBearerSetL( TConnMonIapInfo& aAvailableIAPs );
		
        /**
         * Checks whether preferences allow using WLAN connection
         * @since 5.2
         * @return ETrue if preferences allow WLAN otherwise EFalse
         */
        TBool PrefsAllowWlan();

    private: // Data
    
        // Server class reference
        CMPMServer& iMyServer;

        // Pointer to the roaming confirmation dialog active object
        CMPMConfirmDlgRoaming* iConfirmDlgRoaming;

        // Connection Id
        TConnectionId iConnId;

        // Flag for notification request activity
        TBool iNotifRequested;
        
        // Flag for indicating mobility API support.
        TBool iPreferredIAPRequested;

        // Notification message
        RMessage2 iNotifMessage;

        // If offline feature is supported then this is ETrue
        TBool iOfflineFeatureSupported;

        // Iap Id of Easy WLAN
        TUint32 iEasyWlanIap;

        // Uid of the application
        TUint32 iAppUid;

        // Choose iap request message to be completed when 
        // once MPM has received the asyncronous services from 
        // other servers
        // 
        RMessage2 iChooseIapMessage;

        // Process error request message to be completed when 
        // MPM has received the asyncronous services from 
        // other servers
        //
        // These messages are stored separately 
        // 
        RMessage2 iProcessErrorMessage;

        // ServerSortSNAPL() request message to be completed  
        // when MPM has received the asyncronous services 
        // from other servers
        //
        RMessage2 iServerSortSNAPMessage;
      
        // Contains state info whether PrefIapnotifs can be sent 
        // and saved IAP info structure.
        //
        TStoredIapInfo iStoredIapInfo;
            
        // Iap selection object
        //    
        CMPMIapSelection* iIapSelection;

        // Stored state of migrating to carrier
        // 
        TMigrateToCarrierState iMigrateState;

        // Last Iap notified using PreferredIap-notification
        //
        TUint32 iLastNotifiedIap;
    
        // Iap to which connection is migrating
        //
        TUint32 iMigrateIap;

        // Set when this session is user connection
        //
        TBool iUserConnection;

        // Set when this session uses VPN user connection
        //
        TBool iVpnUserConnectionUsed;        

    };

#include "mpmserversession.inl"
    
#endif // MPMSERVERSESSION_H

// End of File
