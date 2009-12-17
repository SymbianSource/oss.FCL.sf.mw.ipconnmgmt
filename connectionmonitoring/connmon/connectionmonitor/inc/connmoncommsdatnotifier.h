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
* Description:  Listens for changes in one CommsDat table through CenRep.
*
*/

#ifndef CONNMONCOMMSDATNOTIFIER_H
#define CONNMONCOMMSDATNOTIFIER_H

class CConnMonAvailabilityManager;
class CRepository;

// Stop requesting new notifications after this many consecutive errors
const TInt KConnMonCenRepErrorThreshold = 80;

NONSHARABLE_CLASS( CConnMonCommsDatNotifier ) : public CActive
    {
public:
    static CConnMonCommsDatNotifier* NewL(
            CConnMonAvailabilityManager* aAvailabilityManager,
            TUint32 aTableId );

    static CConnMonCommsDatNotifier* NewLC(
            CConnMonAvailabilityManager* aAvailabilityManager,
            TUint32 aTableId );

    virtual ~CConnMonCommsDatNotifier();

private:
    CConnMonCommsDatNotifier(
            CConnMonAvailabilityManager* aAvailabilityManager,
            TUint32 aTableId );

    void ConstructL();
    TInt RequestNotifications();

private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    CConnMonAvailabilityManager* iAvailabilityManager;
    TUint32 iTableId;
    CRepository* iRepository;
    TInt iErrorCounter;
    };

#endif // CONNMONCOMMSDATNOTIFIER_H

// End-of-file
