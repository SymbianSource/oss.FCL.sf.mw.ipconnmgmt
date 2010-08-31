/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Implementation of the bearer priority array cache.
*
*/


#include <datamobilitycommsdattypes.h>
#include <cmmanagerdef.h>

#include "cmmbearerprioritycache.h"
#include "cmmbearerpriority.h"
#include "cmmtransactionhandler.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmbearerprioritycacheTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmBearerPriorityCache* CCmmBearerPriorityCache::NewL(
        CCmmTransactionHandler* aTrans,
        CommsDat::TMDBElementId aBearerPriorityTableId )
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_NEWL_ENTRY );

    CCmmBearerPriorityCache* self =
            CCmmBearerPriorityCache::NewLC( aTrans, aBearerPriorityTableId );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmBearerPriorityCache* CCmmBearerPriorityCache::NewLC(
        CCmmTransactionHandler* aTrans,
        CommsDat::TMDBElementId aBearerPriorityTableId )
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_NEWLC_ENTRY );

    CCmmBearerPriorityCache* self =
            new( ELeave ) CCmmBearerPriorityCache( aTrans, aBearerPriorityTableId );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_NEWLC_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmBearerPriorityCache::~CCmmBearerPriorityCache()
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_CCMMBEARERPRIORITYCACHE_ENTRY );
    iArray.ResetAndDestroy();
    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_CCMMBEARERPRIORITYCACHE_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmBearerPriorityCache::CCmmBearerPriorityCache(
        CCmmTransactionHandler* aTrans,
        CommsDat::TMDBElementId aBearerPriorityTableId )
        :
        iTrans( aTrans ),
        iArray(),
        iBearerPriorityTableId( aBearerPriorityTableId )
    {
    OstTraceFunctionEntry0( DUP1_CCMMBEARERPRIORITYCACHE_CCMMBEARERPRIORITYCACHE_ENTRY );
    OstTraceFunctionExit0( DUP1_CCMMBEARERPRIORITYCACHE_CCMMBEARERPRIORITYCACHE_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmBearerPriorityCache::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_CONSTRUCTL_ENTRY );

    // Read initial bearer priority array from commsdat.
    ReadL();

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Updates the bearer priority array with the contents of aArray.
// ---------------------------------------------------------------------------
//
void CCmmBearerPriorityCache::UpdateL( const RPointerArray<CCmmBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_UPDATEL_ENTRY );

    iTrans->OpenTransactionLC();

    CommsDat::CMDBRecordSet<CCDGlobalBearerTypePriorizationRecord>* bpRS =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDGlobalBearerTypePriorizationRecord>(
                    iBearerPriorityTableId );
    CleanupStack::PushL( bpRS );

    // Delete the old records.
    TRAP_IGNORE( bpRS->LoadL( iTrans->Session() ) );
    for ( TInt i = 0; i < bpRS->iRecords.Count(); i++ )
        {
        bpRS->iRecords[i]->DeleteL( iTrans->Session() );
        }
    CleanupStack::PopAndDestroy( bpRS );
    bpRS = NULL;

    iArray.ResetAndDestroy();

    // Add new content.
    const TInt count( aArray.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( aArray[i]->ServiceType() && aArray[i]->ServiceType()->Length() > 0 )
            {
            CCDGlobalBearerTypePriorizationRecord* bpRcd =
                    new( ELeave ) CCDGlobalBearerTypePriorizationRecord(
                            iBearerPriorityTableId );
            CleanupStack::PushL( bpRcd );

            HBufC* serviceType = aArray[i]->ServiceType()->AllocLC();
            TUint priority = aArray[i]->Priority();
            TUint uiPriority = aArray[i]->UiPriority();

            bpRcd->SetRecordId( KCDNewRecordRequest );
            bpRcd->iServiceType.SetL( *serviceType );
            bpRcd->iPriority.SetL( priority );
            bpRcd->iUIPriority.SetL( uiPriority );
            bpRcd->StoreL( iTrans->Session() );

            TPtrC tempServiceType( serviceType->Des() );
            CCmmBearerPriority* item = CCmmBearerPriority::NewLC(
                    tempServiceType,
                    priority,
                    uiPriority );
            iArray.AppendL( item ); // Ownership transferred.
            CleanupStack::Pop( item );
            CleanupStack::PopAndDestroy( serviceType );
            CleanupStack::PopAndDestroy( bpRcd );
            }
        }
    iTrans->CommitTransactionL();

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_UPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CopyL()
// ---------------------------------------------------------------------------
//
void CCmmBearerPriorityCache::CopyL( RPointerArray<CCmmBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_COPYL_ENTRY );

    const TInt count( iArray.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iArray[i]->ServiceType() )
            {
            TPtrC tempServiceType( iArray[i]->ServiceType()->Des() );
            if ( tempServiceType.Length() > 0 )
                {
                CCmmBearerPriority* item = CCmmBearerPriority::NewLC(
                        tempServiceType,
                        iArray[i]->Priority(),
                        iArray[i]->UiPriority() );
                aArray.AppendL( item );
                CleanupStack::Pop( item );
                }
            }
        }

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_COPYL_EXIT );
    }

// ---------------------------------------------------------------------------
// Get the priority value for a service type name from the bearer priority
// cache.
// ---------------------------------------------------------------------------
//
TUint CCmmBearerPriorityCache::GetPriority( const TDesC& aServiceType ) const
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_GETPRIORITY_ENTRY );

    TUint priority = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;

    if ( aServiceType.Length() > 0 )
        {
        for ( TInt i = 0; i < iArray.Count(); i++ )
            {
            if ( iArray[i]->ServiceType()->Compare( aServiceType ) == 0 )
                {
                priority = iArray[i]->Priority();
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_GETPRIORITY_EXIT );
    return priority;
    }

// ---------------------------------------------------------------------------
// Reads the bearer priority array from CommsDat.
// ---------------------------------------------------------------------------
//
void CCmmBearerPriorityCache::ReadL()
    {
    OstTraceFunctionEntry0( CCMMBEARERPRIORITYCACHE_READL_ENTRY );

    CommsDat::CMDBRecordSet<CCDGlobalBearerTypePriorizationRecord>* bpRS =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDGlobalBearerTypePriorizationRecord>(
                    iBearerPriorityTableId );
    CleanupStack::PushL( bpRS );

    CCDGlobalBearerTypePriorizationRecord* bpRcd =
            new( ELeave ) CCDGlobalBearerTypePriorizationRecord(
                    iBearerPriorityTableId );
    CleanupStack::PushL( bpRcd );

    TRAP_IGNORE( bpRS->LoadL( iTrans->Session() ) );

    // Read every record and store to cache.
    const TInt bpCount = bpRS->iRecords.Count();
    for ( TInt i = 0; i < bpCount; i++ )
        {
        bpRcd->SetElementId( bpRS->iRecords[i]->ElementId() );
        bpRcd->LoadL( iTrans->Session() );

        if ( !bpRcd->iServiceType.IsNull() )
            {
            TPtrC serviceType( bpRcd->iServiceType );
            TUint priority = bpRcd->iPriority;
            TUint uiPriority = bpRcd->iUIPriority;

            if ( serviceType.Length() > 0 )
                {
                CCmmBearerPriority* item = CCmmBearerPriority::NewLC(
                        serviceType,
                        priority,
                        uiPriority );
                iArray.AppendL( item );
                CleanupStack::Pop( item );
                }
            }
        }
    CleanupStack::PopAndDestroy( bpRcd );
    CleanupStack::PopAndDestroy( bpRS );

    OstTraceFunctionExit0( CCMMBEARERPRIORITYCACHE_READL_EXIT );
    }

// End of file
