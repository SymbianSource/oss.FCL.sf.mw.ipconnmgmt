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
* Description:  Implements the CAORAUManager class
*
*/


// INCLUDE FILES
#include "caoraumanager.h"
#include "logger.h"
#include "caoraumanagerimpl.h"

//    METHODS

// Two-phased constructor.
#ifdef __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManager::NewL
// ---------------------------------------------------------------------------
//
CAORAUManager* CAORAUManager::NewL(
    MAORAUManagerObserver& aObserver,
    RMmCustomAPI& aCustomAPI )
    {
    LOG_1( _L("CAORAUManager::NewL") );
    
    CAORAUManager* self = new( ELeave ) CAORAUManager;
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aCustomAPI );
    CleanupStack::Pop( self );
    
    return self;
    }
#else

// ---------------------------------------------------------------------------
// CAORAUManager::NewL
// ---------------------------------------------------------------------------
//
CAORAUManager* CAORAUManager::NewL( MAORAUManagerObserver& aObserver )
    {
    LOG_1( _L("CAORAUManager::NewL") );
    
    CAORAUManager* self = new( ELeave ) CAORAUManager;
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }
    
#endif // __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManager::~CAORAUManager
// ---------------------------------------------------------------------------
//
CAORAUManager::~CAORAUManager()
    {
    LOG_1( _L("CAORAUManager::~CAORAUManager") );
    
    delete iRAUManagerImpl;
    }

// ---------------------------------------------------------------------------
// CAORAUManager::CAORAUManager
// ---------------------------------------------------------------------------
//
CAORAUManager::CAORAUManager():
    iRAUManagerImpl( NULL )
    {
    LOG_1( _L("CAORAUManager::CAORAUManager") );
    }

// Symbian 2nd phase constructor
#ifdef __ALWAYS_ON_CUSTOM_API

// ---------------------------------------------------------------------------
// CAORAUManager::ConstructL
// ---------------------------------------------------------------------------
//
void CAORAUManager::ConstructL( MAORAUManagerObserver& aObserver,
                                RMmCustomAPI& aCustomAPI )
    {
    LOG_1( _L("CAORAUManager::ConstructL") );
    
    iRAUManagerImpl = CAORAUManagerImpl::NewL( aObserver, aCustomAPI );
    }
#else

// ---------------------------------------------------------------------------
// CAORAUManager::ConstructL
// ---------------------------------------------------------------------------
//
void CAORAUManager::ConstructL( MAORAUManagerObserver& aObserver )
    {
    LOG_1( _L("CAORAUManager::ConstructL") );
    
    iRAUManagerImpl = CAORAUManagerImpl::NewL( aObserver );
    }
    
#endif // __ALWAYS_ON_CUSTOM_API

// End of file
