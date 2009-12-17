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
* Description:  RAU manager
*
*/


#ifndef C_CAORAUMANAGER_H
#define C_CAORAUMANAGER_H

// INCLUDE FILES
#include <e32base.h>
#include "maoraumanager.h"
#include "alwaysonconfig.hrh"

// FORWARD DECLARATIONS
class MAORAUManagerObserver;
class CAORAUManagerImpl;
#ifdef __ALWAYS_ON_CUSTOM_API
class RMmCustomAPI;
#endif

/**
*  RAU manager. RAU manager listens RAU events (Routing Area Update)
*  and notifies them forwards.
*  Succesfull RAU events are notified through MAORAUManagerObserver
*  interface.
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAORAUManager ):
    public CBase,
    public MAORAUManager
    {
public: // Constructors & destructors

#ifdef __ALWAYS_ON_CUSTOM_API

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @param aCustomAPI Object via send notifications to TSY.
     * @return a pointer to the created instance of CAORAUManager.
     */
    static CAORAUManager* NewL( MAORAUManagerObserver& aObserver,
                                RMmCustomAPI& aCustomAPI );
#else

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @return a pointer to the created instance of CAORAUManager.
     */
    static CAORAUManager* NewL( MAORAUManagerObserver& aObserver );

#endif // __ALWAYS_ON_CUSTOM_API

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAORAUManager();
    
private: // New methods

    /**
     * Constructor.
     *
     * @since S60 v3.1
     */
    CAORAUManager();

#ifdef __ALWAYS_ON_CUSTOM_API

    /**
     * Second-phase constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @param aCustomAPI Object via send notifications to TSY.
     * @return a pointer to the created instance of CAORAUManager.
     */
    void ConstructL( MAORAUManagerObserver& aObserver,
                     RMmCustomAPI& aCustomAPI );
#else

    /**
     * Second-phase constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @return a pointer to the created instance of CAORAUManager.
     */
    void ConstructL( MAORAUManagerObserver& aObserver );

#endif // __ALWAYS_ON_CUSTOM_API

private: // Data

    /** RAU manager implementation
     *
     * Own
     */
    CAORAUManagerImpl* iRAUManagerImpl;
    };

#endif // C_CAORAUMANAGER_H
