/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CAOTimer class
*
*/


#ifndef C_CAOTIMER_H
#define C_CAOTIMER_H

// INCLUDE FILES
#include <e32base.h>
#include "maotimer.h"

// FORWARD DECLARATIONS
class CPeriodic;
class MAOTimerObserver;
class MAOSettings;

/**
*  Timer provides possibility to start and stop retry
*  and connection timers.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAOTimer ):
    public CBase,
    public MAOTimer
    {
public:    // Constructors & destructors

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver observer for timer events
     * @param aSettings settings for the timer
     * @return pointer to the created CAOTimer object
     */
    static CAOTimer* NewL( MAOTimerObserver& aObserver,
                           MAOSettings& aSettings );

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAOTimer();
    
public: // New methods

    /**
     * Handles settings changes.
     *
     * @since S60 v3.1
     */
    void HandleSettingsChangedL();
    
public: // From base class MAOTimer
        // Allow Always-On Server access

    /**
     * From MAOTimer
     * Starts retry timer.
     *
     * @since S60 v3.1
     */
    void StartRetryTimer();

    /**
     * From MAOTimer
     * Starts connection timer.
     *
     * @since S60 v3.1
     */
    void StartConnectionTimer();

    /**
     * From MAOTimer
     * Stops retry timer.
     *
     * @since S60 v3.1
     */
    void StopRetryTimer();

    /**
     * From MAOTimer
     * Stops connection timer.
     *
     * @since S60 v3.1
     */
    void StopConnectionTimer();

    /**
     * Starts unconnect timer.
     *
     * @since 3.0
     */
    void StartUnconnectTimer();

    /**
     * Stops unconnect timer.
     *
     * @since 3.0
     */
    void StopUnconnectTimer();
    
    /**
     * From MAOTimer
     * Checks if retry timer is set to infinite (disabled).
     *
     * @since S60 v3.1
     */
    TBool RetryTimerDisabled() const;
    
    /**
     * Checks if unconnect timer is set to zero (disabled).
     *
     * @since 3.0
     * @return ETrue if set to infinite
     */
    TBool UnconnectTimerDisabled() const;
    
private: // New methods

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aObserver observer for timer events
     * @param aSettings settings for the timer
     */
    CAOTimer( MAOTimerObserver& aObserver,
              MAOSettings& aSettings );

    /**
     * Default Symbian 2nd-phase constructor
     *
     * @since S60 v3.1
     */
    void ConstructL();

private: // New metohds

    /**
     * Retry timer callback
     *
     * @since S60 v3.1
     * @param aSelf the caller
     * @return error code in case of error else KErrNone
     */
    static TInt RetryTimerCallBack( TAny* aSelf );
    
    /**
     * Connection timer callback
     *
     * @since S60 v3.1
     * @param aSelf the caller
     * @return error code in case of error else KErrNone
     */
    static TInt ConnectionTimerCallBack( TAny* aSelf );
    
    /**
     * Handles retry timer callback
     *
     * @since S60 v3.1
     */
    void HandleRetryTimerCallBackL();
        
    /**
     * unconnected timer call back
     *
     * @since S60 v3.1
     * @param aSelf ?
     */ 
    static TInt UnconnectTimerCallBack( TAny* aSelf );
    
    /**
     * Configures interval for timer
     * Return value is internal in micro seconds
     *
     * @since S60 v3.1
     * @param aInternalInSec ?
     */ 
    TInt ConfigureInterval( const TInt& aIntervalInSec );

private: // Data

    /**
     * Timer observer
     */
    MAOTimerObserver& iObserver;
    
    /**
     * Settings
     */
    MAOSettings& iSettings;

    /**
     * Own: Retry timer
     */
    CPeriodic* iRetryTimer;
    
    /**
     * Retry timer value
     */
    TInt iRetryTimerCount;

    /**
     * Currently used retry timer interval
     */
    TInt iCurrentRetryTimerInterval;
    
    /**
     * Own: Connection timer
     */
    CPeriodic* iConnectionTimer;
        
    /**
     * Own: Unconnect timer
     */
    CPeriodic* iUnconnectTimer;
    };

#endif // C_CAOTIMER_H
