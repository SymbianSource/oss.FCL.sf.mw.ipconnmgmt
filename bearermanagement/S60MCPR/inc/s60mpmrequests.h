/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Interface to Mobility Policy Manager definitions.
*
*/

/**
@file s60mpmrequests.h
Interface to Mobility Policy Manager definitions.
*/

#ifndef S60MPMREQUESTS_H
#define S60MPMREQUESTS_H

// System includes
#include <e32base.h>
//#include <ss_log.h>
#include <rmpm.h>

// User includes
#include "s60mpmpolicyinterfaces.h"
#include "s60mcprmpmlog.h"

// Namespaces
using namespace S60MCPRMPMOperations;

// Forward declaration
class CMPMPolicyNotifications;

// Initial reconnect timeout is 100 000 microseconds (0.1 sec), 
// which is doubled upon issuing the first timer request
const TInt KMPMReconnectTimeoutMultiplier = 2;
const TInt KMPMInitialReconnectTimeout = 100000 / KMPMReconnectTimeoutMultiplier;
const TInt KMPMMaxReconnectTimeout = 800000;

/**
*  S60 NetMCPR MPM interface management Class inherits from CActive
*/
NONSHARABLE_CLASS( CMPMPolicyRequests ) 
    :   public CActive, 
        public MMPMPolicyNotificationUser
    {
    public:
        /**
         * Two-phased constructor.
         * @param aUser reference to class implementing MMPMPolicyNotificationsUser interface  
         */
        static CMPMPolicyRequests *NewL( MMPMPolicyNotificationUser& aUser );

        /**
         * Asynchronous destructor.
         * At the point of return the object may or may not be deleted
         * but MUST NOT be used by the owner anymore.
         */
        void RequestDelete();
        
        /**
         * Notifies MPM a certain IAP started.
         * forward the call to MPM
         *
         * @param aIapId IAP Id of the connection.
         */
        void IAPConnectionStartedL( const TUint32 aIapId );

        /**
        * Notifies MPM a certain IAP stopped.
        * forward the call to MPM
        * 
        * @param aIapId IAP Id of the connection.
        */
        void IAPConnectionStoppedL( const TUint32 aIapId );

        /**
        * Notifies MPM of new RConnection joining to connection
        * forward the call to MPM
        * 
        * @param aIapId IAP Id of the connection.
        */
        void ApplicationJoinsConnectionL( const TUint32 aIapId );

        /**
        * Notifies MPM that RConnection is leaving the connection provider
        * forward the call to MPM
        * 
        * @param aIapId IAP Id of the connection.
        */
        void ApplicationLeavesConnectionL( const TUint32 aIapId );
        
        /**
        * Notifies MPM that RConnection is closing down
        * forward the call to MPM
        */
        void ApplicationConnectionEndsL();
        
        /**
         * Register to Preferred IAP Notifications which are delivered through 
         * the CMPMPolicyNotification -class's PolicyNotification -call.
         */
        void RegisterPreferredIAPNotificationL();

        /**
         * Unregister from Preferred IAP Notifications. 
         */
        void UnregisterPreferredIAPNotificationL();

        /**
         * Informs MPM that application want to ignore the carrier.
         * @param aIapId Ignored IAP 
         */
        void ApplicationIgnoredTheCarrierL( const TUint32 aIapId );
        
        /**
         * Informs MPM that application wants to migrate to carrier. 
         * @param aIapId IAP to migrate to 
         */
        void ApplicationMigratesToCarrierL( const TUint32 aIapId);
        
        /**
         * Informs MPM that application rejected the carrier
         * @param aIapId Rejected IAP 
         */
        void ApplicationRejectedTheCarrierL( const TUint32 aIapId );
        
        /**
         * Informs MPM that application accepted the carrier
         * @param aIapId Accepted IAP  
         */
        void ApplicationAcceptedTheCarrierL( const TUint32 aIapId);

        /**
        * Request for best iap from MPM
        * 
        * @param aConnPref connection preference chosen by user
        * @param aConnId ID of a connection
        * @param aAppUid application uid of the application using the connection
        * @param aPolicyPref connection preference chosen by MPM
        * @param aUser object requesting the answer
        */
        void ChooseBestIAPL( TConnPref*            aConnPref,
                            const TUid&           aAppUid,
                            TConnPref*            aPolicyPref,
                            MMPMPolicyRequestsUser* aUser );

        /**
        * Cancel Request for best iap from MPM
        */
        void CancelReselectBestIAP();

        /**
        * Cancel Request for best iap from MPM
        */
        void CancelChooseBestIAPL();

        /**
        * Cancel ProcessError request.
        */
        void CancelProcessError();
        
        /**
         * Request MPM to choose another IAP
         * 
         * @param aPolicyPref connection preference chosen by MPM
         * @param aUser object requesting the answer.
         */
        void ReselectIAPL( TConnPref*              aPolicyPref,
                           MMPMPolicyRequestsUser* aUser );

        /**
        * Request MPM to process the error
        *
        * @param anError error occured at connection
        * @param aUser object requesting the answer
        */
        void ProcessErrorL( TInt&                   anError,
                            MMPMPolicyRequestsUser* aUser );

        /**
        * Destructor.
        */
        virtual ~CMPMPolicyRequests();
        
    public: // from MMPMPolicyNotificationUser
        /**
        * Called by AO listening MPM notifications.
        * @param aNotification Generic nsotification structure class
        */
        virtual void PolicyNotification( TMpmNotification& aNotification );

    private:
        /**
        * C++ default constructor.
        */
        CMPMPolicyRequests();

        /**
        * 2nd phase constructor.
        * 
        * @param aUser reference to class implementing MMPMPolicyNotificationsUser interface  
        */
        void ConstructL( MMPMPolicyNotificationUser& aUser );

    private: // from CActive

        /**
        * Active Object RunL.
        * @since 5.2
        */
        void RunL();

        /**
        * Active Object DoCancel. 
        * CActive::Cancel() must not be called.
        * CMPMPolicyRequests::DoCancel() must not be called.
        * CMPMPolicyRequests::CancelRequest( TMpmAsynchCodes ) is ok.
        * @since 5.2
        */
        void DoCancel();
        
        /**
        * Active Object RunError
        * @since 5.2
        * @param aError Error Leave code from RunL()
        */
        TInt RunError( TInt aError );

    private: // private methods
        
        /**
        * Cancels asynchronous request.
        * @param aRequestCode asynchronous request to be cancelled.
        * @since 5.2
        */
        void CancelRequest( TMpmAsynchCodes aRequestCode );

        /**
         * Creates a request with iapid and connid and takes it forward.
         * @param TMpmAsynchCodes a request type
         * @param aIapId IAP
         * @since 5.2
         */
        void CreatePolicyRequestL( TPolicyServerOperations aRequestCode, 
                                   const TUint32& aIapId );

        /**
        * Issue the first request in the policy data list
        * @since 5.2
        */
        
        void IssueRequest();
        /**
        * Delete object asynchronously
        * @since 5.2
        */
        void IssueDeleteRequest();
        
    private:
        // policy request list
        RArray<PolicyRequest>       iPolicyRequests;
        // client handle to mpm
        RMPM                        iMpm;
        TConnectionId               iConnId;
        // handle MPM notifications
        //CBearerManPolicyNotif*      iBearerManPolicyNotif;
        TBool                       iCommitedToDeleteItself;
        CMPMPolicyNotifications*    iPolicyNotifications;
        MMPMPolicyNotificationUser* iPolicyNotificationsUser;
        
        // Reconnect timer related variables
        RTimer                      iReconnectTimer;
        TBool                       iReconnectTimerRunning;
        TInt                        iReconnectTimeout;
    };

#endif // S60MPMREQUESTS_H

