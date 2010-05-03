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
* Description: Listen cellular data usage key changes in central repository.
*
*/

#ifndef MPMDATAUSAGEWATCHER_H
#define MPMDATAUSAGEWATCHER_H

//  INCLUDES
#include <e32base.h>

class CRepository;
class CMPMServer;

// Stop after this many consecutive central repository errors.
const TInt KMpmDataUsageWatcherCenRepErrorThreshold = 80;

/**
 *  Class for accessing central repository.
 *  Follows KCurrentCellularDataUsage key in central repository.
 *  @since 5.2
 */
class CMpmDataUsageWatcher : public CActive
    {

public:

    /**
    * New for calling the two-phased constructor.
    */
    static CMpmDataUsageWatcher* NewL( CMPMServer* aServer );

    /**
    * Destructor.
    */
    virtual ~CMpmDataUsageWatcher();

    /**
     * Start to listen for events.
     */
    void StartL();
    
    /**
    * Active object's RunL.
    */
    void RunL();

    /**
    * Active object's DoCancel.
    */
    void DoCancel();

private:

    /**
    * C++ default constructor.
    */
    CMpmDataUsageWatcher( CMPMServer* aServer );

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Request for notifications.
    */
    TInt RequestNotifications();
    
    /**
    * Get current cellular data usage value.
    */
    TInt GetCurrentDataUsageValue();


private: // data

    /**
     * Central repository handle.
     * Own.
     */
    CRepository* iRepository;

    /**
     * Data usage value.
     */
    TInt iCellularDataUsage;     // type: TCmCellularDataUsage

    /**
     * Pointer to the MPM Server object.
     * Not own.
     */
    CMPMServer* iServer;

    /**
     * Error counter.
     */
    TUint iErrorCounter;

    };

#endif // MPMDATAUSAGEWATCHER_H
