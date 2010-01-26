/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listens for changes in a Central Repository key.
*
*/

#ifndef AOCENREPOBSERVER_H
#define AOCENREPOBSERVER_H

// FORWARD DECLARATIONS
class CRepository;
class MAOConnectionObserver;

// Stop requesting new notifications after this many consecutive errors
const TInt KCenRepErrorRetryCount = 80;

/**
* MAOCenRepObserver
*
* Client application must implement the AOCenRepObserver interface in
* order to receive a notification when Central Repository key 
* KCRUidCmManager/ KCurrentCellularDataUsage changes.
*
* @lib PDPCONTEXTMANAGER2.LIB
*/
class MAOCenRepObserver
    {
public:
    /**
    * This method is called every time when Central Repository key 
    * KCRUidCmManager/ KCurrentCellularDataUsage changes.
    *
    * @param aValue The new value for KCurrentCellularDataUsage.
    */
    virtual void CurrentCellularDataUsageChangedL( const TInt aValue ) = 0;
    };

/**
* CAOCenRepObserver
*
* @lib DPCONTEXTMANAGER2.LIB
*/
NONSHARABLE_CLASS( CAOCenRepObserver ) : public CActive
    {
public:
    static CAOCenRepObserver* NewL( MAOCenRepObserver& aObserver );

    static CAOCenRepObserver* NewLC( MAOCenRepObserver& aObserver );

    virtual ~CAOCenRepObserver();

private:
    CAOCenRepObserver( MAOCenRepObserver& aObserver );

    void ConstructL();
    void RequestNotifications();
 
private: // Methods from base class
    void DoCancel();
    void RunL();

private:
    MAOCenRepObserver& iObserver;
    CRepository*       iRepository;
    TInt               iErrorCounter;
    };

#endif // AOCENREPOBSERVER_H

// End-of-file
