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
* Description: Interface for receiving ConnMon events in MPM.
*
*/

/**
@file mpmconnmonevents.h
Interface for receiving ConnMon events in MPM.
*/

#ifndef MPMCONNMONEVENTS_H
#define MPMCONNMONEVENTS_H

//  INCLUDES
#include <e32base.h>
#include <rconnmon.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat.h>
#else
#include <commsdat.h>
#include <commsdat_partner.h>
#endif
#include <nifvar.h>
#include "mpmserver.h"

// FORWARD DECLARATIONS
class CMPMConnMonReqs;

// CONSTANTS

// CLASS DECLARATION
/**
*  TConnInfo
*  Record for connection information storage
*  @lib MPMServer.exe
*  @since 3.1
*/
class TConnInfo
    { 
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        TConnInfo();

    public: // Data
        // Connection Id
        TUint iConnId;

        // IAP Id
        TUint32 iIapId;

        // Presumed IAP Id which connection thinks it's using. 
        // The actual IAP Id ( iIapId ) maybe different because 
        // of SSID sharing or PDP context management.  
        TUint32 iPresumedIapId;

        // Network Id
        TUint32 iNetId;
        
        // Access point name
        TBuf<CommsDat::KMaxMedTextLength> iAPN;

        // SSID of WLAN
        TWlanSsid iSSID;

        // Connection type
        TConnectionType iConnType; 
    };

// CLASS DECLARATION
/**
*  MPM ConnMon interface class definition.
*
*  @lib MPMServer.exe
*  @since 3.0
*/
class CMPMConnMonEvents : public CBase, public MConnectionMonitorObserver
    {

    public:    // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CMPMConnMonEvents* NewL( CMPMServer& aServer );

        /**
        * Destructor.
        */
        virtual ~CMPMConnMonEvents();
        
    public: // New methods
    
        /**
        * Remove pointer from the array.
        * @since 3.1
        * @param aReq
        */                
        void RemoveReqPtr( const CMPMConnMonReqs* aReq );

        /**
        * Appending connection info.
        * @since 3.2
        * @param aConnId Connection Id.
        */                
        void AppendConnInfo( const TUint& aConnId );

        /**
        * Removing connection info.
        * @since 3.2
        * @param aConnId Connection Id.
        */                
        void RemoveConnInfo( const TUint& aConnId );

        /**
        * Setting connection info.
        * @since 3.2
        * @param aConnId Connection Id
        * @param aIapId IAP Id
        * @param aPresumedIapId The IAP Id that application presumes to use
        * @param aNetId Network Id
        * @param aAPN Access Point Name
        */                
        void SetConnInfo( const TUint& aConnId, 
                          const TUint& aIapId, 
                          const TUint& aPresumedIapId, 
                          const TUint& aNetId, 
                          const TDesC& aAPN );

        /**
        * Setting connection info in case of SSID, 
        * in other words WLAN IAP.
        * @since 3.2
        * @param aConnId Connection Id
        * @param aIapId IAP id
        * @param aPresumedIapId The IAP Id that application presumes to use
        * @param aNetId Network id
        * @param aSSID the id of selected wlan network
        */
        void SetConnInfo( const TUint&      aConnId, 
                          const TUint&      aIapId, 
                          const TUint&      aPresumedIapId, 
                          const TUint&      aNetId, 
                          const TWlanSsid&   aSSID );

        /**
        * Setting SSID for certain iap
        * @since 3.2
        * @param aIapId IAP id
        * @param aSSID the id of selected wlan network
        */
        void SetConnInfoIapSsid( const TUint&       aIapId, 
                                 const TWlanSsid&    aSSID  );

        /**
        * Reset connection info.
        * @since 3.2
        * @param aConnId Connection Id
        */
        void ResetConnInfo( const TUint& aConnId );

        /**
        * Reset connection info for all connections using given IAP.
        * @since 3.2
        * @param aIapId IAP id
        */
        void ResetIapConnInfo( const TUint32 aIapId );

        /**
        * Getting IDs of available IAPs.
        * @since 3.1
        * @return IDs of available IAPs 
        */                                              
        const TConnMonIapInfo& GetAvailableIAPs();
        
        /**
        * Setting IDs of available IAPs.
        * @since 3.1
        * @param aAvailableIAPs IDs of available IAPs
        */                                              
        void SetAvailableIAPs( const TConnMonIapInfo& aAvailableIAPs );
        
        /**
        * Find if connection to APN already exists
        * @since 3.1
        * @param aDes APN to be checked
        * @param aIapId On return contains the changed IAP Id
        * @param aNetId On return contains the changed Network Id
        */        
        void FindAPN( const TDesC& aDes, TUint32& aIapId, TUint32& aNetId );

        /**
        * Find if connection to SSID already exists
        * @since 3.2
        * @param aSSID the id of selected wlan network
        * @param aIapId On return contains the changed IAP Id
        * @param aNetId On return contains the changed Network Id
        */        
        void FindSSID( const TWlanSsid&  aSSID, 
                       TUint32&         aIapId, 
                       TUint32&         aNetId );

        /**
        * Checks if there is active WLAN connection.
        * @since 3.2
        * @param aSSID the id of active wlan network
        * @param aIapId the IAP Id of active wlan network
        */        
        TBool ActiveWlanConnection( TWlanSsid&  aSSID, 
                                    TUint32&    aIapId );

        /**
        * Starts WLANNetworks request towards Connection monitor.
        * @since 3.2
        * @param aSession MPM server session requesting the wlan scan, Null if none.
        * @param aId ConnectionId.
        * @param aCallback Callback type to use after WLAN scan completed
        * @param aForceRefreshIntervalSeconds if the last interval is executed withing this time
        * the scan is not executed. 0 to do available iap query always.  
        */
        void ScanWLANNetworksL( CMPMServerSession* aSession,
                                const TConnectionId& aId,
                                TWlanScanCallback aCallback,
                                TInt aForceRefreshIntervalSeconds = 0 );

        /**
        * Cancels WLANNetworks request towards Connection monitor.
        * @param aSession Pointer to session
        * @since 3.1
        */
        void CancelScanL( CMPMServerSession* aSession );

        /**
        * Gets the presumed Iap Id of Connection Id.
        * @since 3.2
        * @param aConnId Connection Id
        * @param aIapId IAP id
        */
        TUint32 PresumedIapId( const TUint& aConnId, 
                               const TUint& aIapId );

        /**
        * Updates the internal IAP refresh time stamp.
        * @since 3.2
        */
        void UpdateIAPRefreshTime();
        
        /**
        * Gets the internal IAP refresh time stamp.
        * @since 3.2
        * @return TTime latest refresh time
        */
        TTime& GetIAPRefreshTime();

        /**
        * Sets connection count.
        * @since 3.2
        * @param aValue Connection count value.
        */
        void SetConnectionCounter( const TUint aValue ); 

        /**
        * Enables discarding availability notifications from Connection Monitor.
        * @since 3.2
        */
        inline void EnableDiscarding(); 

        /**
        * Disables discarding availability notifications from Connection Monitor.
        * @since 3.2
        */
        inline void DisableDiscarding(); 

        /**
        * Returns the setting for discarding availability notifications. 
        * @since 3.2
        * @return ETrue if availability notifications are discarded. 
        */
        inline TBool DiscardAvailabilityNotification(); 
        
        /**
         * Creates preferred carrier available notification when needed.
         * @since 5.1
         * @param aCaller Identifies the calling context
         */
        void IapAvailabilityChange( const TPrefIAPNotifCaller aCaller );
       
    public: // Functions from base classes
    
        /**
        * From MConnectionMonitorObserver. Event from Connection Monitor.
        * @since 3.1
        * @param aConnMonEvent
        */                
        void EventL( const CConnMonEventBase& aConnMonEvent );
        
    private: // New methods

        /**
        * C++ default constructor.
        */
        CMPMConnMonEvents( CMPMServer& aServer );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Data
        // Handle to connection monitor
        RConnectionMonitor              iConnMon;

        // Server object reference
        CMPMServer&                     iMyServer;
        
        // Request object pointer array
        RPointerArray<CMPMConnMonReqs>  iReqPtrs;
        
        // Info about connections
        RArray<TConnInfo>               iConnInfoArray;
        
        // Available IAPs
        TConnMonIapInfo                 iAvailableIAPs;
        
        // Time stamp of last IAP availability check
        TTime                           iLastIAPRefreshTime;
        
        // Discard availability notifications from Connection Monitor 
        // until MPM initiated WLAN scan request has completed. 
        TBool iDiscardAvailabilityNotification;

#ifdef _DEBUG 
        TBool iFilterEvents; // by default off. (CBase)
    public:
        void FilterAvailableIAPEvents( const TBool aVal ) { iFilterEvents = aVal; }
#endif //_DEBUG

    };

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::EnableDiscarding
// -----------------------------------------------------------------------------
//
inline void CMPMConnMonEvents::EnableDiscarding()
    {
    iDiscardAvailabilityNotification = ETrue;
    }

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::DisableDiscarding
// -----------------------------------------------------------------------------
//
inline void CMPMConnMonEvents::DisableDiscarding()
    {
    iDiscardAvailabilityNotification = EFalse;
    }

// -----------------------------------------------------------------------------
// CMPMConnMonEvents::DiscardAvailabilityNotification
// -----------------------------------------------------------------------------
//
inline TBool CMPMConnMonEvents::DiscardAvailabilityNotification()
    {
    return iDiscardAvailabilityNotification;
    }

#endif // MPMCONNMONEVENTS_H

// End of file
