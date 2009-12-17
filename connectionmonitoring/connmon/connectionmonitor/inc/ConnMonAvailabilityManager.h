/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides declarations for the availability manager.
*
*/

#ifndef CONNMONAVAILABILITYMANAGER_H
#define CONNMONAVAILABILITYMANAGER_H

#include <e32base.h>

class CConnMonCommsDatCache;
class CConnMonServer;

/**
 * Availability manager
 * Availability manager class declaration
 */
class CConnMonAvailabilityManager : public CBase
    {
public:
    /**
     * Two phase constructor.
     * @param aIapsAvailabilityManager IapsAvailabilityManager
     * @param aCommsDataNotifier CommsDataNotifier
     */
    static CConnMonAvailabilityManager* NewL(
            CConnMonCommsDatCache* aCommsDatCache,
            CConnMonServer* aConnMonServer );

    /**
     * Destructor.
     */
    virtual ~CConnMonAvailabilityManager();

    /**
     * Handles availability change.
     */
    void HandleAvailabilityChange();

    /**
     * Called when any CenRep CommsDat table listener receives a notification
     */
    void CommsDatEventReceivedL( const TUint32 aTableId );

private:
    /**
     * Constructor.
     * @param aIapsAvailabilityManager IapsAvailabilityManager
     * @param aCommsDataNotifier CommsDataNotifier
     */
    CConnMonAvailabilityManager(
            CConnMonCommsDatCache* aCommsDatCache,
            CConnMonServer* aConnMonServer );

private: // Data
    /**
     * CommsDat cache
     * Not own
     */
    CConnMonCommsDatCache* iCommsDatCache;

    /**
     * Link to Server and IAP services and data
     * Not own
     */
    CConnMonServer* iConnMonServer;
    };

#endif // CONNMONAVAILABILITYMANAGER_H

// End-of-file
