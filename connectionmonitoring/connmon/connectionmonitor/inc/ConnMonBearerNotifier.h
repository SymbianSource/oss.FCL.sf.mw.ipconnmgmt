/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to listen for EGPRS and HSxPA notifications.
*
*/

#ifndef CONNMONBEARERNOTIFIER_H
#define CONNMONBEARERNOTIFIER_H

#include <rconnmon.h>

#include "ConnMonDef.h"

class CConnMonServer;
class RMobilePhone;
class CConnMonEGprsNotifier;
class CConnMonHsdpaNotifier;

/**
* CBearerNotifier
*/
NONSHARABLE_CLASS( CConnMonBearerNotifier )
    {
public:
    static CConnMonBearerNotifier* NewL(
            CConnMonServer* aServer,
            RMobilePhone& aMobilePhone,
            TUint& aConnectionId );
    virtual ~CConnMonBearerNotifier();

public:
    void Listen();
    void CancelListen();
    TBool Listening();
    void SendBearerGroupEvent();
    void SendBearerGroupEvent( TConnMonBearerInfo& aBearerInfo );

    void SendBearerInfoEvent();

    /**
     * Handles the events from the edge and hsdpa notifiers.
     */
    void HandleEvent();

private:
    CConnMonBearerNotifier(
            CConnMonServer* aServer,
            RMobilePhone& aMobilePhone,
            TUint& aConnectionId );
    void ConstructL();

private:
    CConnMonServer* iServer;
    RMobilePhone& iMobilePhone;
    CConnMonEGprsNotifier* iEGprsNotifier;
    CConnMonHsdpaNotifier* iHsdpaNotifier;
    TEventInfo iEventInfo;
    TUint iConnectionId;
    };

#endif // CONNMONBEARERNOTIFIER_H

// End-of-file
