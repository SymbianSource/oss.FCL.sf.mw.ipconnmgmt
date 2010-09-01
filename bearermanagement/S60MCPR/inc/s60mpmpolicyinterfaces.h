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
* Description: This header defines inteface needed by MCPR to use MPM.
*
*/

/**
@file s60mpmpolicyinterfaces.h
This header defines inteface needed by MCPR to use MPM.
*/
#ifndef S60MPMPOLICYINTERFACES_H
#define S60MPMPOLICYINTERFACES_H

// Reusing MPM function code definitions
#include <rmpm.h>

// FORWARD DECLARATIONS
class PolicyRequest;
class CMPMPolicyRequests;

/**
 * Notication interface of S60MCPR
 */
class MMPMPolicyNotificationUser
    {
    public:
        /**
        * Notification
        * 
        * @param aNotification notification buffer from MPM.
        */
        virtual void PolicyNotification( TMpmNotification& aNotification ) = 0;
    };

/**
 *  Callback class.
 *
 *  @since xx
 */
class MMPMPolicyRequestsUser
    {
    friend class PolicyRequest;
public:
    virtual void PolicyResponse( PolicyRequest& aCompletedRequest ) = 0;
protected:
    virtual void Delete() = 0;
    };

/**
 * Lists the supported MPM operations of the MPM API wrapper
 * In order to simplify cancellation, match the cancellable requests to MPM's AsyncRequests. 
 */
namespace S60MCPRMPMOperations
    {
    enum TPolicyServerOperations
        {
        EPolicyRequestChooseBestIAP = EMPMChooseBestIAP,
        EPolicyRequestReselectBestIAP = EMPMReselectBestIAP,
        EPolicyRequestProcessError = EMPMProcessError,
        EPolicyRequestWaitNotification = EMPMWaitNotification,
        EPolicyRequestIAPConnectionStarted = 200,
        EPolicyRequestIAPConnectionStopped,
        EPolicyRequestApplicationJoinsConnection,
        EPolicyRequestApplicationLeavesConnection,
        EPolicyRequestApplicationConnectionEnds,
        EPolicyRequestConnect,
        EPolicyRequestRegisterPrefIAPNotif,
        EPolicyRequestUnregisterPrefIAPNotif,
        EPolicyRequestApplicationMigratesToCarrier,
        EPolicyRequestApplicationIgnoredTheCarrier,
        EPolicyRequestApplicationAcceptedTheCarrier,
        EPolicyRequestApplicationRejectedTheCarrier,
        };
    }

/**
 *  Request container class.
 */
class PolicyRequest
    {
    public:
        /**
         * c++ constructor 
         */
        PolicyRequest()
            :   iConnPref(NULL),
                iIapId(0),
                iConnId(0),
                iAppUid(KNullUid),
                iError(KErrNone),
                iPolicyPref(NULL),
                iStatus(KErrNone),
                iUser(NULL)
            {}

        /**
         * Deletes heap data. 
         */
        void Cleanup()
            {
            delete iConnPref;
            delete iPolicyPref;
            // below is type specific destructor of iUser that handles the cleanup of derived object
            if( iUser )
                iUser->Delete(); 
            }
        
        /**
         * Does not actually cancel MPM request but prevents
         * PolicyResponse() -calls to request originator. 
         */
        void Cancel()
            {
            if( iUser )
                iUser->Delete();
            iUser = NULL;
            }

    public:
        // Information for MPM  
        S60MCPRMPMOperations::TPolicyServerOperations iRequestType;
        TConnPref*              iConnPref;
        TUint                   iIapId;
        TConnectionId           iConnId;
        TUid                    iAppUid;
        TInt                    iError;
        // Information from MPM
        TConnPref*              iPolicyPref;
        TBMNeededAction         iNeededAction;
        TInt                    iStatus;
        // Interface back to requestor.
        MMPMPolicyRequestsUser* iUser;
    };

#endif  // S60MPMPOLICYINTERFACES_H

// End of File

