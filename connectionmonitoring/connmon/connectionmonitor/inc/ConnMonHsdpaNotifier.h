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
* Description:  Active object to listen for HSxPA notifications.
*
*/

#ifndef CONNMONHSDPANOTIFIER_H
#define CONNMONHSDPANOTIFIER_H

#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>

// Forward declarations
class CConnMonServer;

/**
 * CHsxpaAvailabilityNotifier
 */
NONSHARABLE_CLASS( CConnMonHsdpaNotifier ) : public CActive
    {
public:
    static CConnMonHsdpaNotifier* NewL(
            CConnMonBearerNotifier* aObserver,
            CConnMonServer* aServer,
            RMobilePhone& aMobilePhone );
    virtual ~CConnMonHsdpaNotifier();

public:
    /**
     * Requests a new event (bearer (PSD) availability changed) from ETel.
     * @param aKickOff If aKickOff is ETrue initial bearer status will be obtained from ETel.
     */
    void Receive();

    /**
     * Get the current activity status of HSDPA
     * @return Returns ETrue if HSDPA is active and EFalse if not.
     */
    TBool HsdpaActive();

    /**
     * Get the current activity status of HSUPA
     * @since MCL 9.1
     * @return Returns ETrue if HSUPA is active and EFalse if not.
     */
    TBool HsupaActive();

    /**
     * Returns the current HSxPA status.
     * @since MCL 9.1
     * @return EBearerInfoWCDMA if no high speed features in use, or
     * EBearerInfoHSDPA / EBearerInfoHSUPA / EBearerInfoHSxPA.
     */
    TInt HsxpaStatus();

private:
    /**
     * From CActive Cancels the asyncronous request
     */
    void DoCancel();

    /**
     * From CActive Handles the the event that has arrived from ETel.
     */
    void RunL();

private:
    CConnMonHsdpaNotifier(
            CConnMonBearerNotifier* aObserver,
            CConnMonServer* aServer,
            RMobilePhone& aMobilePhone );
    void Construct();


private: // Data
    CConnMonBearerNotifier* iObserver;
    CConnMonServer* iServer;
    RMobilePhone& iMobilePhone;
    RPacketService iPacketService;
    RPacketService::TDynamicCapsFlags iDynCaps;
    TUint8 iPacketServLoaded; // 1 = packet services are loaded, 0 = not loaded
    TBool iHsdpaActive;
    TBool iHsupaActive;
    TInt iHsxpaStatus;
    TInt iPreviousHsxpaStatus;
    };

#endif // CONNMONHSDPANOTIFIER_H

// End-of-file
