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
* Description:  Implementation of the availablity manager.
*
*/

#include "ConnMonAvailabilityManager.h"
#include "connmoncommsdatcache.h"
#include "ConnMonServ.h"
#include "log.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CConnMonAvailabilityManager* CConnMonAvailabilityManager::NewL(
        CConnMonCommsDatCache* aCommsDatCache,
        CConnMonServer* aConnMonServer )
    {
    //LOGENTRFN("CConnMonAvailabilityManager::NewL()")

    CConnMonAvailabilityManager* self = new( ELeave ) CConnMonAvailabilityManager(
            aCommsDatCache,
            aConnMonServer );

    //LOGEXITFN("CConnMonAvailabilityManager::NewL()")
    return self;
    }


// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------
//
CConnMonAvailabilityManager::CConnMonAvailabilityManager(
        CConnMonCommsDatCache* aCommsDatCache,
        CConnMonServer* aConnMonServer )
        :
        iCommsDatCache( aCommsDatCache ),
        iConnMonServer( aConnMonServer )
    {
    //LOGENTRFN("CConnMonAvailabilityManager::CConnMonAvailabilityManager()")
    //LOGEXITFN("CConnMonAvailabilityManager::CConnMonAvailabilityManager()")
    }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CConnMonAvailabilityManager::~CConnMonAvailabilityManager()
    {
    //LOGENTRFN("CConnMonAvailabilityManager::~CConnMonAvailabilityManager()")
    //LOGEXITFN("CConnMonAvailabilityManager::~CConnMonAvailabilityManager()")
    }

// ---------------------------------------------------------------------------
// Handles availability change
// ---------------------------------------------------------------------------
//
void CConnMonAvailabilityManager::HandleAvailabilityChange()
    {
    LOGENTRFN("CConnMonAvailabilityManager::HandleAvailabilityChange()")
    LOGTIMINGSTART("## CConnMonAvailabilityManager::HandleAvailabilityChange()")

    iCommsDatCache->RefreshAvailabilityInfo( ETrue );

    LOGTIMINGEND("## CConnMonAvailabilityManager::HandleAvailabilityChange()")
    LOGEXITFN("CConnMonAvailabilityManager::HandleAvailabilityChange()")
    }


// ---------------------------------------------------------------------------
// Called when any CenRep CommsDat table listener receives a notification
// ---------------------------------------------------------------------------
//
void CConnMonAvailabilityManager::CommsDatEventReceivedL( const TUint32 aTableId )
    {
    LOGENTRFN("CConnMonAvailabilityManager::CommsDatEventReceivedL()")

    // Caller (CConnMonCommsDatNotifier::RunL) is trapping this
    iCommsDatCache->RefreshCommsDatCacheL( aTableId );
    HandleAvailabilityChange();

    LOGEXITFN("CConnMonAvailabilityManager::CommsDatEventReceivedL()")
    }

// End-of-file
