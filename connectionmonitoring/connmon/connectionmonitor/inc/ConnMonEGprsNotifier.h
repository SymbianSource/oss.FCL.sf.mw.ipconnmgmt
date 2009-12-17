/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to listen for EGPRS notifications.
*
*/

#ifndef CONNMONEGPRSNOTIFIER_H
#define CONNMONEGPRSNOTIFIER_H

#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>
#include <rmmcustomapi.h>

// Forward declarations
class CConnMonBearerNotifier;

/**
* CConnMonEGprsNotifier
*/
NONSHARABLE_CLASS( CConnMonEGprsNotifier ) : public CActive
    {
public:
    static CConnMonEGprsNotifier* NewL(
            CConnMonBearerNotifier* aObserver,
            RMobilePhone& aMobilePhone );
    virtual ~CConnMonEGprsNotifier();

public:
    /**
     * Requests a new event (bearer (PSD) availability changed) from ETel.
     * @param aKickOff If aKickOff is ETrue initial bearer status will be obtained from ETel.
     */
    void Receive();

    TBool EGprsActive();

private:
    /**
     * From CActive Cancels the asyncronous request.
     */
    void DoCancel();

    /**
     * From CActive Handles the the event that has arrived from ETel.
     */
    void RunL();

private:
    CConnMonEGprsNotifier(
            CConnMonBearerNotifier* aObserver,
            RMobilePhone& aMobilePhone );
    void ConstructL();

private: // Data
    CConnMonBearerNotifier* iObserver;
    RMobilePhone& iMobilePhone;
    RMmCustomAPI::TGprsInformation iGprsInfo;
    RMmCustomAPI::TGprsInformationPckg iGprsInfoPckg;
    RMmCustomAPI iCustomApi;
    TBool iEGprsActive;
    };

#endif // CONNMONEGPRSNOTIFIER_H

// End-of-file
