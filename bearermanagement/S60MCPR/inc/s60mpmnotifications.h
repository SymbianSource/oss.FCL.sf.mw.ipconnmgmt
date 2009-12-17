/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Interface to Mobility Policy Manager notifications.
*
*/

/**
@file s60mpmnotifications.h
Interface to Mobility Policy Manager notifications.
*/

#ifndef S60MCPR_NOTIFICATIONS_H
#define S60MCPR_NOTIFICATIONS_H

// System includes
#include <e32base.h>

// User includes
#include "s60mpmpolicyinterfaces.h"
#include "s60mcprmpmlog.h"

// Forward declarations
class RMPM;

/**
*  MPM interface notification management Class
*/
NONSHARABLE_CLASS( CMPMPolicyNotifications ) : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        * 
        * @param aUser reference to class implementing the callback interface.
        * @param aMpm MPM session to be used.
        */
        static CMPMPolicyNotifications *NewL( MMPMPolicyNotificationUser& aUser, RMPM& aMpm );
        /**
        * Asynchronous Destructor.
        * After calling this function the object takes responsibility to destroy itself.
        */
        void CancelRequestAndDestroyD( TRequestStatus* aStatus = NULL );
        /**
        * Activates notifications.
        */
        void Activate();
    private:
        /**
        * C++ default constructor.
        * 
        * @param aUser reference to class implementing the callback interface.
        * @param aMpm MPM session to be used.
        */
        CMPMPolicyNotifications( MMPMPolicyNotificationUser& aUser, RMPM& aMpm );
        /**
        * Destructor.
        */
        ~CMPMPolicyNotifications();
    private: // from CActive
        /**
        * Active Object RunL.
        */
        void RunL();
        /**
        * Active Object DoCancel.
        */
        void DoCancel();
    private:
        MMPMPolicyNotificationUser& iNotificationUser;
        RMPM&                       iMpm;
        TMPMMessageBuffer           iMsgBuffer;
        TBool                       iCommitedToDeleteItself;
        TRequestStatus*             iDeletedStatus;
    };

#endif // S60MCPR_NOTIFICATIONS_H

