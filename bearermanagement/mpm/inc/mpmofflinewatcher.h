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
* Description: Listens for offline mode changes in central repository.
*
*/

#ifndef MPMOFFLINEWATCHER_H
#define MPMOFFLINEWATCHER_H

//  INCLUDES
#include <e32base.h>

class CRepository;
class CMPMServer;

// Stop after this many consecutive central repository errors.
const TInt KMpmOfflineWatcherCenRepErrorThreshold = 80;

/**
 *  Class for accessing central repository.
 *  Follows KCoreAppUIsNetworkConnectionAllowed key in central repository.
 *  @since 5.2
 */
class CMpmOfflineWatcher : public CActive
    {

public:

    /**
    * New for calling the two-phased constructor.
    */
    static CMpmOfflineWatcher* NewL( CMPMServer* aServer );

    /**
    * Destructor.
    */
    virtual ~CMpmOfflineWatcher();

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
    CMpmOfflineWatcher( CMPMServer* aServer );

    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Request for notifications.
    */
    TInt RequestNotifications();
    
    /**
    * Get current offline mode value.
    */
    TInt GetCurrentOfflineValue();


private: // data

    /**
     * Is offline feature supported.
     */
    TBool iOfflineFeatureSupported;

    /**
     * Central repository handle.
     * Own.
     */
    CRepository* iRepository;

    /**
     * Offline mode activity value.
     */
    TInt iOfflineMode;     // type: TCoreAppUIsNetworkConnectionAllowed

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

#endif // MPMOFFLINEWATCHER_H
