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
* Description:  RAU manager stub for testing
*
*/


#ifndef C_CAORAUMANAGERIMPLSTUB_H
#define C_CAORAUMANAGERIMPLSTUB_H

// INCLUDE FILES
#include <e32base.h>
#include <e32property.h>

#include "alwaysonconfig.hrh"
#include "caoasyncwrapper.h"

// FORWARD DECLARATIONS
class MAORAUManagerObserver;
#ifdef __ALWAYS_ON_CUSTOM_API
class RMmCustomAPI;
#endif // __ALWAYS_ON_CUSTOM_API

/**
*  RAU manager implementation.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
class CAORAUManagerImpl : public CBase
    {
public: // Constructors & destructors

#ifdef __ALWAYS_ON_CUSTOM_API

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @param aCustomAPI Object via send notifications to TSY.
     * @return a pointer to the created instance of CAORAUManagerImpl.
     */
    static CAORAUManagerImpl* NewL( MAORAUManagerObserver& aObserver,
                                    RMmCustomAPI& /*aCustomAPI*/ );
#else

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @return a pointer to the created instance of CAORAUManagerImpl.
     */
    static CAORAUManagerImpl* NewL( MAORAUManagerObserver& aObserver );

#endif// __ALWAYS_ON_CUSTOM_API

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAORAUManagerImpl();
    
private: // RAU impl stub methods

    /**
     * RAU event async wrapper method
     *
     * @since S60 v3.1
     */
    void SubscribeRAUEvents();

    /**
     * RAU event async wrapper method
     *
     * @since S60 v3.1
     * @param aStatus TRequestStatus for async call
     */
    void RAUEventSubscribeIssueRequest( TRequestStatus& aStatus );

    /**
     * RAU event async wrapper method
     *
     * @since S60 v3.1
     * @param aStatus returned status from async call
     */
    void RAUEventSubscribeRunL( TInt aStatus );

    /**
     * RAU event async wrapper method
     *
     * @since S60 v3.1
     */
    void RAUEventSubscribeDoCancel();
            
private: // New methods

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     */
    CAORAUManagerImpl( MAORAUManagerObserver& aObserver );

    /**
     * Symbian 2nd-phase constructor.
     *
     * @since S60 v3.1
     */
    void ConstructL();

private: // Data

    /**
     * RAU manager observer
     */
    MAORAUManagerObserver& iObserver;
    
    /**
     * RAU Event property
     */
    RProperty iRAUEventProperty;
    
    /**
     * RAU Event subscriber
     * Own
     */
    CAOAsyncWrapper<CAORAUManagerImpl>* iRAUEventSubscriber;
    };

#endif // C_CAORAUMANAGERIMPLSTUB_H
