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
* Description:  RAU manager implementation
*
*/


#ifndef C_CAORAUMANAGERIMPL_H
#define C_CAORAUMANAGERIMPL_H

// INCLUDE FILES
#include <e32base.h>

#include "alwaysonconfig.hrh"

#ifdef __ALWAYS_ON_CUSTOM_API
#include <RMmCustomAPI.h>
#endif // __ALWAYS_ON_CUSTOM_API

// FORWARD DECLARATIONS
class MAORAUManagerObserver;

/**
*  RAU manager implementation.
*  @see CAORAUManager
*
*  @lib PDPContextManager2.lib
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CAORAUManagerImpl ): public CActive
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
                                    RMmCustomAPI& aCustomAPI );
#else
 
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @return a pointer to the created instance of CAORAUManagerImpl.
     */
   static CAORAUManagerImpl* NewL( MAORAUManagerObserver& aObserver );

#endif // __ALWAYS_ON_CUSTOM_API

    /**
     * Destructor
     *
     * @since S60 v3.1
     */
    virtual ~CAORAUManagerImpl();
    
private: // New methods

#ifdef __ALWAYS_ON_CUSTOM_API

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     * @param aCustomAPI Object via send notifications to TSY.
     */
    CAORAUManagerImpl( MAORAUManagerObserver& aObserver,
                       RMmCustomAPI& aCustomAPI );
#else

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aObserver  Interface via receive notifications from RAU.
     */
    CAORAUManagerImpl( MAORAUManagerObserver& aObserver );

#endif // __ALWAYS_ON_CUSTOM_API

    /**
     * Symbian 2nd-phase constructor.
     *
     * @since S60 v3.1
     */
    void ConstructL();
    
private: // New methods

    //
    void IssueRAUNotifications();

private: // From base class CActive

    /**
     * From CActive
     * Process the completed request.
     *
     * @since S60 v3.1
     */
    void RunL();
    
    /**
     * From CActive
     * Cancel outstanding request
     *
     * @since S60 v3.1
     */
    void DoCancel();
    
    /**
     * From CActive
     * Handle a leave occurred in the RunL().
     * Note: If RunError function pointer is not given in construction,
     * the leave error is to be propagated back to the active scheduler!
     *
     * @since S60 v3.1
     * @param aError the error code of leave
     */
    TInt RunError( TInt aError );
    
private: // Data

    /**
     * Ref: RAU manager observer
     */
    MAORAUManagerObserver& iObserver;
    
#ifdef __ALWAYS_ON_CUSTOM_API
    
    /** 
     * Ref: Custom API
     */
    RMmCustomAPI& iCustomAPI;
    
    /**
     * RAU status
     */
    RMmCustomAPI::TRauEventStatus iRAUStatus;
    
#endif // __ALWAYS_ON_CUSTOM_API
    };

#endif // C_CAORAUMANAGERIMPL_H
