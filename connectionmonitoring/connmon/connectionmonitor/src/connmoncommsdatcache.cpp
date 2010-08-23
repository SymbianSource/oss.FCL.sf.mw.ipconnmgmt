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
* Description:  Provides cached information on IAPs and SNAPs in CommsDat.
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <datamobilitycommsdattypes.h>

#include "ConnMonServ.h"
#include "CEventQueue.h"
#include "ConnMonIAP.h"
#include "ConnMonBearer.h"
#include "connmoncommsdatcache.h"
#include "log.h"

using namespace CommsDat;

TConnMonSnapEntry::TConnMonSnapEntry()
    {
    iId = 0;
    iNextLayerIapId = 0;
    iNextLayerSnapId = 0;
    iAvailability = 0;
    iNextLayerIndex = KErrNotFound;
    }

TConnMonSnapEntry::TConnMonSnapEntry(
        TUint aId,
        TUint aNextLayerIapId,
        TUint aNextLayerSnapId )
        :
        iId( aId ),
        iNextLayerIapId( aNextLayerIapId ),
        iNextLayerSnapId( aNextLayerSnapId )
    {
    iAvailability = 0;
    iNextLayerIndex = KErrNotFound;
    }

TInt TConnMonSnapEntry::Compare(
        const TConnMonSnapEntry& aFirst,
        const TConnMonSnapEntry& aSecond )
    {
    // Availability info is ignored
    if ( aFirst.iId < aSecond.iId ) return -1;
    if ( aFirst.iId > aSecond.iId ) return 1;
    if ( aFirst.iNextLayerSnapId < aSecond.iNextLayerSnapId ) return -1;
    if ( aFirst.iNextLayerSnapId > aSecond.iNextLayerSnapId ) return 1;
    if ( aFirst.iNextLayerIapId < aSecond.iNextLayerIapId ) return -1;
    if ( aFirst.iNextLayerIapId > aSecond.iNextLayerIapId ) return 1;
    return 0;
    }

TInt TConnMonSnapEntry::FindCompare(
        const TInt* aKey,
        const TConnMonSnapEntry& aEntry )
    {
    // Zero if match, negative if first is smaller, positive otherwise
    return ( *aKey ) - aEntry.iId;
    }

TBool TConnMonSnapEntry::Match(
        const TConnMonSnapEntry& aFirst,
        const TConnMonSnapEntry& aSecond )
    {
    // Availability info is ignored
    if ( ( aFirst.iId == aSecond.iId ) &&
            ( aFirst.iNextLayerIapId == aSecond.iNextLayerIapId ) &&
            ( aFirst.iNextLayerSnapId == aSecond.iNextLayerSnapId ) )
        {
        return ETrue;
        }
    return EFalse;
    }


TConnMonIapEntry::TConnMonIapEntry()
    {
    iId = 0;
    iBearerType = 0;
    iServiceType = 0;
    iAvailability = 0;
    iNextLayerIapId = 0;
    iNextLayerSnapId = 0;
    iNextLayerIndex = KErrNotFound;
    }

TConnMonIapEntry::TConnMonIapEntry(
        TUint aId,
        TUint aBearerType,
        TUint aServiceType )
        :
        iId( aId ),
        iBearerType( aBearerType ),
        iServiceType( aServiceType )
    {
    iAvailability = 0;
    iNextLayerIndex = KErrNotFound;
    iNextLayerIapId = 0;
    iNextLayerSnapId = 0;
    }

TInt TConnMonIapEntry::Compare(
        const TConnMonIapEntry& aFirst,
        const TConnMonIapEntry& aSecond )
    {
    // Zero if match, negative if first is smaller, positive otherwise
    // Availability info is ignored
    return aFirst.iId - aSecond.iId;
    }

TInt TConnMonIapEntry::FindCompare(
        const TInt* aKey,
        const TConnMonIapEntry& aEntry )
    {
    // Zero if match, negative if first is smaller, positive otherwise
    return ( *aKey ) - aEntry.iId;
    }

TBool TConnMonIapEntry::Match(
        const TConnMonIapEntry& aFirst,
        const TConnMonIapEntry& aSecond )
    {
    // Availability info is ignored
    if ( ( aFirst.iId == aSecond.iId ) &&
            ( aFirst.iBearerType == aSecond.iBearerType ) &&
            ( aFirst.iServiceType == aSecond.iServiceType ) )
        {
        return ETrue;
        }
    return EFalse;
    }


TConnMonVirtualIapEntry::TConnMonVirtualIapEntry()
    {
    iId = 0;
    iNextLayerIapId = 0;
    iNextLayerSnapId = 0;
    }

TConnMonVirtualIapEntry::TConnMonVirtualIapEntry(
        TUint aId,
        TUint aNextLayerIapId,
        TUint aNextLayerSnapId )
        :
        iId( aId ),
        iNextLayerIapId( aNextLayerIapId ),
        iNextLayerSnapId( aNextLayerSnapId )
    {
    }

TInt TConnMonVirtualIapEntry::Compare(
        const TConnMonVirtualIapEntry& aFirst,
        const TConnMonVirtualIapEntry& aSecond )
    {
    // Zero if match, negative if first is smaller, positive otherwise
    return aFirst.iId - aSecond.iId;
    }

TInt TConnMonVirtualIapEntry::FindCompare(
        const TInt* aKey,
        const TConnMonVirtualIapEntry& aEntry )
    {
    // Zero if match, negative if first is smaller, positive otherwise
    return ( *aKey ) - aEntry.iId;
    }

TBool TConnMonVirtualIapEntry::Match(
        const TConnMonVirtualIapEntry& aFirst,
        const TConnMonVirtualIapEntry& aSecond )
    {
    if ( ( aFirst.iId == aSecond.iId ) &&
            ( aFirst.iNextLayerIapId == aSecond.iNextLayerIapId ) &&
            ( aFirst.iNextLayerSnapId == aSecond.iNextLayerSnapId ) )
        {
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CConnMonCommsDatCache* CConnMonCommsDatCache::NewL()
    {
    //LOGENTRFN("CConnMonCommsDatCache::NewL()")

    CConnMonCommsDatCache* self = CConnMonCommsDatCache::NewLC();
    CleanupStack::Pop( self );

    //LOGEXITFN("CConnMonCommsDatCache::NewL()")
    return self;
    }

// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CConnMonCommsDatCache* CConnMonCommsDatCache::NewLC()
    {
    //LOGENTRFN("CConnMonCommsDatCache::NewLC()")

    CConnMonCommsDatCache* self = new( ELeave ) CConnMonCommsDatCache;
    CleanupStack::PushL( self );
    self->ConstructL();

    //LOGEXITFN("CConnMonCommsDatCache::NewLC()")
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CConnMonCommsDatCache::~CConnMonCommsDatCache()
    {
    LOGENTRFN("CConnMonCommsDatCache::~CConnMonCommsDatCache()")

    if ( iSnapCache )
        {
        iSnapCache->Close();
        delete iSnapCache;
        iSnapCache = NULL;
        }
    if ( iIapCache )
        {
        iIapCache->Close();
        delete iIapCache;
        iIapCache = NULL;
        }
    if ( iVirtualIapCache )
        {
        iVirtualIapCache->Close();
        delete iVirtualIapCache;
        iVirtualIapCache = NULL;
        }

    iWlanIapIdCache.Close();
    iIapIdCache.Close();
    iSnapIdCache.Close();

    LOGEXITFN("CConnMonCommsDatCache::~CConnMonCommsDatCache()")
    }

// ---------------------------------------------------------------------------
// Reads all IAP and SNAP information from CommsDat and initializes the cache
// with it.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::Init(
        CConnMonServer* aServer,
        CConnMonIAP* aConnMonIap,
        RPointerArray<TConnMonBearer>* aBearers )
    {
    LOGENTRFN("CConnMonCommsDatCache::Init()")

    iServer = aServer;
    iIap = aConnMonIap;
    iBearers = aBearers;

    if ( iInitStatus != EConnMonCacheInitNotStarted )
        {
        LOGIT("Init: ERROR, ConnMon CommsDat cache init called too many times")
        return;
        }
    iInitStatus = EConnMonCacheInitInProgress;

    // Check if WLAN bearer is available
    for ( TInt i = 0; i < iBearers->Count(); i++ )
        {
        if ( (*iBearers)[i]->BearerId() == EBearerIdWLAN )
            {
            iWlanSupportEnabled = ETrue;
            break;
            }
        }

    TRAPD( leaveCode, InitCommsDatCacheL() );
    if ( leaveCode )
        {
        LOGIT1("Init: ERROR (Ok if empty), LEAVE in ConnMon CommsDat cache init <%d>", leaveCode)
        }

    RefreshAvailabilityInfo( EFalse ); // Never send events in Init phase
    iInitStatus = EConnMonCacheInitCompleted;

    LOGEXITFN("CConnMonCommsDatCache::Init()")
    }

// ---------------------------------------------------------------------------
// Calls the correct method to read the CommsDat table, refered to with
// parameter aTableId, to cache. This should be called when a change is
// detected in CommsDat through central repository events.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::RefreshCommsDatCacheL( const TUint32 aTableId )
    {
    //LOGENTRFN("CConnMonCommsDatCache::RefreshCommsDatCacheL()")

    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    if ( aTableId == iIapRecordTableId )
        {
        LOGIT("RefreshCommsDatCacheL: IAP table change event")
        RefreshCommsDatIapCacheL( *db );
        }
    else if ( aTableId == iSnapRecordTableId )
        {
        LOGIT("RefreshCommsDatCacheL: SNAP table change event")
        RefreshCommsDatSnapCacheL( *db );
        }
    else if ( aTableId == iVirtualRecordTableId )
        {
        LOGIT("RefreshCommsDatCacheL: Virtual record table change event")
        RefreshCommsDatVirtualIapCacheL( *db );
        }
    else if ( aTableId == 0 )
        {
        // This option is not currently used, but provided to support the
        // possibility to read CommsDat information again when client is
        // asking for IAP/SNAP availability information as a request.
        // This would be needed in the case that CenRep change events
        // become unreliable for some reason, and thus the CommsDat cache
        // would not be reliably up to date.
        LOGIT("RefreshCommsDatCacheL: Reading all commsdat tables")
        RefreshCommsDatIapCacheL( *db );
        RefreshCommsDatSnapCacheL( *db );
        RefreshCommsDatVirtualIapCacheL( *db );
        }

    CleanupStack::PopAndDestroy( db );

    //LOGEXITFN("CConnMonCommsDatCache::RefreshCommsDatCacheL()")
    }

// ---------------------------------------------------------------------------
// Solves IAP and SNAP availability. The availability information in the cache
// tables will be up to date after a call to this method.
// If parameter aCanSendEvents is true, availability changed events will be
// sent to clients if any changes from previous availability state is detected.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::RefreshAvailabilityInfo( const TBool aCanSendEvents )
    {
    LOGENTRFN("CConnMonCommsDatCache::RefreshAvailabilityInfo()")

    // If any relevant commsdat table changed.
    //
    // These flags are set to true only when CommsDat change event is received,
    // CommsDat information is read into ConnMon cache, and that information
    // has changed.
    if ( iIapsChanged || iSnapsChanged || iVirtualIapsChanged )
        {
        UpdateSnapAndVirtualIapLinks();
        iIapsChanged = EFalse;
        iSnapsChanged = EFalse;
        iVirtualIapsChanged = EFalse;
        }

    ResetAllAvailabilityInfo();
    for ( TInt i = 0; i < iBearers->Count(); i++ )
        {
        (*iBearers)[i]->FlagAvailableIaps();
        }
    SolveSnapAndVirtualIapAvailability();

    LOGIT(".")
    TBool availableIapsChanged = UpdateAvailableIaps();

    #ifdef _DEBUG
    // Print available IAP IDs to log
    for ( TInt j = 0; j < iIapCache->Count(); j++ )
        {
        if ( (*iIapCache)[j].iAvailability == EConnMonAvailabilityAvailable )
            {
            TUint currentId = (*iIapCache)[j].iId;
            if ( (*iIapCache)[j].iBearerType == EConnMonCacheBearerTypeVirtual )
                {
                switch ( (*iIapCache)[j].iServiceType )
                    {
                    case EConnMonCacheServiceTypeCsd:
                        LOGIT1("  %3d: CSD, virtual", currentId) break;
                    case EConnMonCacheServiceTypeGprs:
                        LOGIT1("  %3d: GPRS, virtual", currentId) break;
                    case EConnMonCacheServiceTypeLan:
                        LOGIT1("  %3d: LAN, virtual", currentId) break;
                    case EConnMonCacheServiceTypeWlan:
                        LOGIT1("  %3d: WLAN, virtual", currentId) break;
                    default:
                        LOGIT1("  %3d: Unknown, virtual", currentId) break;
                    }
                }
            else
                {
                switch ( (*iIapCache)[j].iServiceType )
                    {
                    case EConnMonCacheServiceTypeCsd:
                        LOGIT1("  %3d: CSD", currentId) break;
                    case EConnMonCacheServiceTypeGprs:
                        LOGIT1("  %3d: GPRS", currentId) break;
                    case EConnMonCacheServiceTypeLan:
                        LOGIT1("  %3d: LAN", currentId) break;
                    case EConnMonCacheServiceTypeWlan:
                        LOGIT1("  %3d: WLAN", currentId) break;
                    default:
                        LOGIT1("  %3d: Unknown", currentId) break;
                    }
                }
            }
        }
    #endif // _DEBUG

    TBool availableSnapsChanged = UpdateAvailableSnaps();

    // Print available SNAP IDs to log
    #ifdef _DEBUG
    TUint lastSnapId( 0 );
    for ( TInt k = 0; k < iSnapCache->Count(); k++ )
        {
        TUint currentId = (*iSnapCache)[k].iId;
        if ( currentId != lastSnapId )
            {
            lastSnapId = currentId;
            if ( (*iSnapCache)[k].iAvailability == EConnMonAvailabilityAvailable )
                {
                LOGIT1("  %4d", currentId)
                }
            }
        }
    LOGIT(".")
    #endif // _DEBUG

    if ( aCanSendEvents )
        {
        if ( availableIapsChanged )
            {
            LOGIT("RefreshAvailabilityInfo: sending IAP availability event")
            SendIapAvailabilityEvent();
            }
        if ( availableSnapsChanged )
            {
            LOGIT("RefreshAvailabilityInfo: sending SNAP availability event")
            SendSnapAvailabilityEvent();
            }
        }

    LOGEXITFN("CConnMonCommsDatCache::RefreshAvailabilityInfo()")
    }

// ---------------------------------------------------------------------------
// Set as available all IAPs which correspond to the given bearer ID
// (converted to service type).
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::SetAvailableIapsWithBearerId( const TUint aBearerId )
    {
    //LOGENTRFN("CConnMonCommsDatCache::SetAvailableIapsWithBearerId()")
    TInt err( KErrNone );

    TUint serviceType;
    err = ConvertBearerIdToServiceType( aBearerId, serviceType );

    if ( !err )
        {
        const TInt iapCount = iIapCache->Count();
        for ( TInt i = 0; i < iapCount; i++ )
            {
            if ( (*iIapCache)[i].iServiceType == serviceType )
                {
                (*iIapCache)[i].iAvailability = EConnMonAvailabilityAvailable;
                }
            }
        }

    //LOGEXITFN("CConnMonCommsDatCache::SetAvailableIapsWithBearerId()")
    }

// ---------------------------------------------------------------------------
// Set as available the IAP with matching ID.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::SetAvailableIapWithId( const TUint aId )
    {
    //LOGENTRFN("CConnMonCommsDatCache::SetAvailableIapWithId()")

    TInt index = iIapCache->FindInOrder<TInt>( aId, TConnMonIapEntry::FindCompare );
    if ( index >= 0)
        {
        (*iIapCache)[index].iAvailability = EConnMonAvailabilityAvailable;
        }

    //LOGEXITFN("CConnMonCommsDatCache::SetAvailableIapWithId()")
    }

// ---------------------------------------------------------------------------
// Get available IAP IDs for the requested bearer ID. IAP availability is
// re-solved first.
// Maximum number of IAP IDs is limited by KConnMonMaxIAPCount.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::GetAvailableIaps(
        const TUint aBearerId,
        TConnMonIapInfo& aIapInfo )
    {
    LOGENTRFN("CConnMonCommsDatCache::GetAvailableIaps()")
    TInt err( KErrNone );

    TBool availabilityEventsEnabled( EFalse );
    if ( ( aBearerId == EBearerIdAll ) || 
         ( aBearerId == EBearerIdVirtualVPN ) ||
         ( aBearerId == EBearerIdWLAN ) )
        {
        // If WLAN background scanning is on, and client is asking IAP
        // availability for all- or virtual IAPs, send IAP availability changed
        // events if any changes detected.
        TRAPD( traperr, availabilityEventsEnabled = IsWlanBackgroundScanningEnabledL() );
        if ( traperr )
            {
            // If error here, continue as WLAN background scanning is disabled.
            availabilityEventsEnabled = EFalse;
            LOGIT1("ERROR, WLAN background scan discovery failed with <%d>", traperr)
            }
        }

    RefreshAvailabilityInfo( availabilityEventsEnabled );

    TInt iapCount( 0 );
    TUint bearerType( 0 );
    TUint serviceType( 0 );
    TBool done( EFalse );

    switch ( aBearerId )
        {
        case EBearerIdAll:
            {
            iapCount = iIapIdCache.Count();
            if ( iapCount > KConnMonMaxIAPCount )
                {
                iapCount = KConnMonMaxIAPCount;
                }
            aIapInfo.iCount = iapCount;
            for ( TInt i = 0; i < iapCount; i++ )
                {
                aIapInfo.iIap[i].iIapId = iIapIdCache[i];
                }
            done = ETrue;
            }
            break;
        case EBearerIdGPRS:
        case EBearerIdWCDMA:
            serviceType = EConnMonCacheServiceTypeGprs;
            break;
        case EBearerIdWLAN:
            serviceType = EConnMonCacheServiceTypeWlan;
            break;
        case EBearerIdCSD:
        case EBearerIdWcdmaCSD:
            serviceType = EConnMonCacheServiceTypeCsd;
            break;
        case EBearerIdVirtualVPN:
            bearerType = EConnMonCacheBearerTypeVirtual;
            break;
        case EBearerIdLAN:
            serviceType = EConnMonCacheServiceTypeLan;
            break;
        default:
            err = KErrArgument;
            done = ETrue;
            break;
        }

    if ( !done )
        {
        TInt totalCount = iIapCache->Count();
        for ( TInt i = 0; i < totalCount && iapCount < KConnMonMaxIAPCount; i++ )
            {
            if ( serviceType )
                {
                if ( (*iIapCache)[i].iAvailability == EConnMonAvailabilityAvailable &&
                        (*iIapCache)[i].iServiceType == serviceType )
                    {
                    aIapInfo.iIap[iapCount].iIapId = (*iIapCache)[i].iId;
                    iapCount++;
                    }
                }
            else if ( bearerType )
                {
                if ( (*iIapCache)[i].iAvailability == EConnMonAvailabilityAvailable &&
                        (*iIapCache)[i].iBearerType == bearerType )
                    {
                    aIapInfo.iIap[iapCount].iIapId = (*iIapCache)[i].iId;
                    iapCount++;
                    }
                }
            }
        aIapInfo.iCount = iapCount;
        }

    LOGEXITFN1("CConnMonCommsDatCache::GetAvailableIaps()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Get available SNAP IDs. SNAP availability is re-solved first.
// Maximum number of SNAP IDs is limited by KConnMonMaxSNAPsCount.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::GetAvailableSnaps( TConnMonSNAPInfo& aSnapInfo )
    {
    LOGENTRFN("CConnMonCommsDatCache::GetAvailableSnaps()")

    TBool availabilityEventsEnabled( EFalse );
    // If WLAN background scanning is on, and client is asking SNAP
    // availability, send SNAP availability changed events if any changes
    // detected.
    TRAPD( traperr, availabilityEventsEnabled = IsWlanBackgroundScanningEnabledL() );
    if ( traperr )
        {
        // If error here, continue as WLAN background scanning is disabled.
        availabilityEventsEnabled = EFalse;
        LOGIT1("ERROR, WLAN background scan discovery failed: <%d>", traperr)
        }

    RefreshAvailabilityInfo( availabilityEventsEnabled );

    TInt snapCount = iSnapIdCache.Count();
    if ( snapCount > KConnMonMaxSNAPsCount )
        {
        snapCount = KConnMonMaxSNAPsCount;
        }
    aSnapInfo.iCount = snapCount;
    for ( TInt i = 0; i < snapCount; i++ )
        {
        aSnapInfo.iSNAP[i].iSNAPId = iSnapIdCache[i];
        }

    LOGEXITFN("CConnMonCommsDatCache::GetAvailableSnaps()")
    }

// ---------------------------------------------------------------------------
// Get available SNAP IDs. SNAP availability is re-solved first.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::GetAvailableSnaps( RArray<TConnMonId>& aSnapIds )
    {
    LOGENTRFN("CConnMonCommsDatCache::GetAvailableSnaps()")

    TBool availabilityEventsEnabled( EFalse );
    // If WLAN background scanning is on, and client is asking SNAP
    // availability, send SNAP availability changed events if any changes
    // detected.
    TRAPD( traperr, availabilityEventsEnabled = IsWlanBackgroundScanningEnabledL() );
    if ( traperr )
        {
        // If error here, continue as WLAN background scanning is disabled.
        availabilityEventsEnabled = EFalse;
        LOGIT1("ERROR, WLAN background scan discovery failed: <%d>", traperr)
        }

    RefreshAvailabilityInfo( availabilityEventsEnabled );

    // Return KErrNoMemory only if RArray fails to allocate memory, KErrNone otherwise
    TInt err( KErrNone );
    err = aSnapIds.Reserve( iSnapIdCache.Count() );
    if ( !err )
        {
        for ( TInt i = 0; i < iSnapIdCache.Count(); i++ )
            {
            aSnapIds.Append( TConnMonId( iSnapIdCache[i] ) );
            }
        }

    LOGEXITFN1("CConnMonCommsDatCache::GetAvailableSnaps()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CConnMonCommsDatCache::CConnMonCommsDatCache()
    {
    //LOGENTRFN("CConnMonCommsDatCache::CConnMonCommsDatCache()")
    //LOGEXITFN("CConnMonCommsDatCache::CConnMonCommsDatCache()")
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ConstructL()
    {
    //LOGENTRFN("CConnMonCommsDatCache::ConstructL()")

    iIap = NULL;
    iInitStatus = EConnMonCacheInitNotStarted;
    iVirtualIapCount = 0;
    iWlanSupportEnabled = EFalse; // Set in Init()-method

    iIapsChanged = EFalse;
    iSnapsChanged = EFalse;
    iVirtualIapsChanged = EFalse;

    iIapRecordTableId = KCDTIdIAPRecord;
    iSnapRecordTableId = 0;     // Read from CommsDat when needed for first time
    iVirtualRecordTableId = 0;  // Read from CommsDat when needed for first time

    iIapCache = new( ELeave ) RArray<TConnMonIapEntry>();
    iSnapCache = new( ELeave ) RArray<TConnMonSnapEntry>();
    iVirtualIapCache = new( ELeave ) RArray<TConnMonVirtualIapEntry>();

    //LOGEXITFN("CConnMonCommsDatCache::ConstructL()")
    }

// ---------------------------------------------------------------------------
// Reads all relevant CommsDat information to cache and initializes the SNAP
// and virtual IAP table IDs.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::InitCommsDatCacheL()
    {
    //LOGENTRFN("CConnMonCommsDatCache::InitCommsDatCacheL()")

    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    // Find out the table IDs for CCDDataMobilitySelectionPolicyRecord and
    // CCDVirtualIAPNextLayerRecord, only need to do this once.
    iVirtualRecordTableId = CCDVirtualIAPNextLayerRecord::TableIdL( *db );
    iSnapRecordTableId = CCDDataMobilitySelectionPolicyRecord::TableIdL( *db );

    LOGIT("InitCommsDatCacheL: reading commsdat tables")
    RefreshCommsDatIapCacheL( *db );
    RefreshCommsDatSnapCacheL( *db );
    RefreshCommsDatVirtualIapCacheL( *db );

    CleanupStack::PopAndDestroy( db );

    //LOGEXITFN("CConnMonCommsDatCache::InitCommsDatCacheL()")
    }

// ---------------------------------------------------------------------------
// Reads all IAP information from CommsDat and updates the cache if necessary.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::RefreshCommsDatIapCacheL( CMDBSession& aDb )
    {
    LOGENTRFN("CConnMonCommsDatCache::RefreshCommsDatIapCacheL()")

    // Will only leave if device runs out of memory or LoadL fails on IAP
    // record set. This will result in an empty IAP cache and thus no IAP
    // will be reported available through ConnMon API.

    RArray<TUint> currentWlanIapIds;
    CleanupClosePushL( currentWlanIapIds );

    RArray<TConnMonIapEntry>* currentIapData;
    if ( iInitStatus == EConnMonCacheInitCompleted )
        {
        // Array to read current CommsDat info into.
        currentIapData = new( ELeave ) RArray<TConnMonIapEntry>; // Heap used by design
        CleanupClosePushL( *currentIapData );
        }
    else
        {
        // Init-phase, read current CommsDat info directly into cache array,
        // since it is still empty. Then stop (don't send any events).
        iWlanIapIdCache.Reset();
        currentIapData = iIapCache;
        }
    currentIapData->Reset();

    ReadCommsDatIapTableL( aDb, *currentIapData, currentWlanIapIds );

    if ( iInitStatus != EConnMonCacheInitCompleted )
        {
        // This is Init pass. CommsDat has been read and stored in cache. return now.
        // Note, currentIapData not in cleanup stack

        // If no WLAN, this will just be an empty array
        DeepCopy( currentWlanIapIds, iWlanIapIdCache );
        CleanupStack::Pop( &currentWlanIapIds );
        currentWlanIapIds.Close();

        iIapsChanged = ETrue;
        LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatIapCache()", iIapsChanged)
        return iIapsChanged;
        }

    CleanupStack::Pop( currentIapData );
    iIapsChanged = EFalse;

    // WLAN IAPs are most likely to change. Check those first for changes.
    if ( iWlanSupportEnabled )
        {
        iIapsChanged = CompareSortedArrays( currentWlanIapIds, iWlanIapIdCache );
        if ( iIapsChanged )
            {
            if ( iIap )
                {
                iIap->EnableWlanScan(); // Important
                }
            DeepCopy( currentWlanIapIds, iWlanIapIdCache );
            }
        }
    CleanupStack::Pop( &currentWlanIapIds );
    currentWlanIapIds.Close();

    // If WLAN IAPs didn't change, check the rest of the IAPs for changes.
    if ( !iIapsChanged )
        {
        iIapsChanged = CompareSortedArrays( *currentIapData, *iIapCache );
        }

    // Delete the obsolete IAP cache table. Either the new table that was just
    // read, or the old table if the new one is different.
    if ( iIapsChanged )
        {
        iIapCache->Close();
        delete iIapCache;
        iIapCache = currentIapData;
        LOGIT("RefreshCommsDatIapCacheL: updated IAP cache")
        }
    else
        {
        currentIapData->Close();
        delete currentIapData;
        currentIapData = NULL;
        LOGIT("RefreshCommsDatIapCacheL: IAP cache did not change")
        }

    LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatIapCacheL()", iIapsChanged)
    return iIapsChanged;
    }

// ---------------------------------------------------------------------------
// Reads all IAP table information from CommsDat into an array.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ReadCommsDatIapTableL(
        CMDBSession& aDb,
        RArray<TConnMonIapEntry>& aCurrentIapData,
        RArray<TUint>& aCurrentWlanIapIds )
    {
    LOGENTRFN("CConnMonCommsDatCache::ReadCommsDatIapTableL()")

    // Explicitly build a TLinearOrder<class>. Used as parameter to RArray::InsertInOrder().
    TLinearOrder<TConnMonIapEntry> iapEntryOrderingLogic( TConnMonIapEntry::Compare );

    CMDBRecordSet<CCDIAPRecord>* ptrIapRecordSet = new( ELeave )
            CMDBRecordSet<CCDIAPRecord>( iIapRecordTableId );
    CleanupStack::PushL( ptrIapRecordSet );
    ptrIapRecordSet->LoadL( aDb );

    // Check LAN bearer table for WLAN bearer entries and store results in array.
    // This information is used to separate LAN and WLAN IAPs from each other.
    RArray<TLanBearerEntry> lanBearerTableCache;
    CleanupClosePushL( lanBearerTableCache );
    TRAPD( leaveCode, ReadCommsDatLanBearerTableL( aDb, lanBearerTableCache ) );
    if ( leaveCode )
        {
        LOGIT1("ERROR reading LAN bearer table, LEAVE with <%d>", leaveCode)
        lanBearerTableCache.Reset();
        }

    iVirtualIapCount = 0;
    TInt iapRecordCount( ptrIapRecordSet->iRecords.Count() );
    LOGIT1("ReadCommsDatIapTableL: IAP record count %d (commsdat)", iapRecordCount)

    TUint iapId( 0 );
    TUint bearerType( 0 );
    TUint serviceType( 0 );

    for ( TInt i = 0; i < iapRecordCount; i++ )
        {
        TRAP( leaveCode, ReadCommsDatIapEntryL(
                (CCDIAPRecord*)ptrIapRecordSet->iRecords[i],
                lanBearerTableCache,
                bearerType,
                serviceType ) );

        if ( leaveCode )
            {
            // Skip this IAP and continue with next one
            LOGIT1("ERROR reading IAP entry, LEAVE with <%d>", leaveCode)
            break;
            }

        iapId = ptrIapRecordSet->iRecords[i]->RecordId();

        if ( serviceType == EConnMonCacheServiceTypeWlan && iWlanSupportEnabled )
            {
            // Add WLAN IAP ID to WLAN IAP cache
            TInt err = aCurrentWlanIapIds.InsertInOrder( iapId );
            if ( err )
                {
                LOGIT1("ERROR inserting WLAN IAP ID to WLAN cache <%d>", err)
                serviceType = 0;
                }
            }

        // Unknown IAP or error while reading it
        if ( !serviceType && !bearerType )
            {
            LOGIT1("WARNING, unknown IAP in CommsDat IAP table with id %d", iapId)
            iapId = 0; // ID to zero so this IAP will be ignored
            }

        if ( iapId )
            {
            // Adding IAP to cache
            TConnMonIapEntry iapEntry( iapId, bearerType, serviceType );

            TInt err = aCurrentIapData.InsertInOrder( iapEntry , iapEntryOrderingLogic );
            if ( err )
                {
                LOGIT1("ERROR inserting IAP to current IAP data <%d>", err)
                }
            }
        }

    CleanupStack::Pop( &lanBearerTableCache );
    lanBearerTableCache.Close();
    CleanupStack::PopAndDestroy( ptrIapRecordSet );
    LOGIT1("ReadCommsDatIapTableL: IAP record count %d (cache)", aCurrentIapData.Count())

    LOGEXITFN("CConnMonCommsDatCache::ReadCommsDatIapTableL()")
    }

// ---------------------------------------------------------------------------
// Reads one record from IAP table and finds out the service and bearer types.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ReadCommsDatIapEntryL(
        CCDIAPRecord* aIapEntry,
        RArray<TLanBearerEntry>& aLanBearerTableCache,
        TUint& aBearerType,
        TUint& aServiceType )
    {
    //LOGENTRFN("CConnMonCommsDatCache::ReadCommsDatIapEntryL()")

    aBearerType = 0;
    aServiceType = 0;
    TBuf<KMaxTextLength> bearerTypeName( aIapEntry->iBearerType.GetL() );
    TBuf<KMaxTextLength> serviceTypeName( aIapEntry->iServiceType.GetL() );

    if ( serviceTypeName == TPtrC( KCDTypeNameOutgoingWCDMA ) ||
            serviceTypeName == TPtrC( KCDTypeNameIncomingWCDMA ) )
        {
        aServiceType = EConnMonCacheServiceTypeGprs; // GPRS IAP
        }
    else if ( serviceTypeName == TPtrC( KCDTypeNameLANService ) )
        {
        // LAN or WLAN IAP
        TUint32 bearerId( aIapEntry->iBearer );
        if ( bearerTypeName == TPtrC( KCDTypeNameLANBearer ) &&
                HasWlanBearer( bearerId, aLanBearerTableCache ) )
            {
            aServiceType = EConnMonCacheServiceTypeWlan; // WLAN IAP
            }
        else
            {
            aServiceType = EConnMonCacheServiceTypeLan; // LAN IAP
            }
        }
    else if ( ( serviceTypeName == TPtrC( KCDTypeNameDialOutISP ) ) ||
            ( serviceTypeName == TPtrC( KCDTypeNameDialInISP ) ) )
        {
        aServiceType = EConnMonCacheServiceTypeCsd; // CSD IAP
        }

    if ( bearerTypeName == TPtrC( KCDTypeNameVirtualBearer ) )
        {
        iVirtualIapCount++;
        aBearerType = EConnMonCacheBearerTypeVirtual; // Virtual IAP
        }

    //LOGEXITFN("CConnMonCommsDatCache::ReadCommsDatIapEntryL()")
    }

// ---------------------------------------------------------------------------
// Reads all records from the LAN bearer table and checks if they are WLAN
// bearer type. Results are stored in an array.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ReadCommsDatLanBearerTableL(
        CMDBSession& aDb,
        RArray<TLanBearerEntry>& aLanBearerTableCache )
    {
    LOGENTRFN("CConnMonCommsDatCache::ReadCommsDatLanBearerTableL()")

    CMDBRecordSet<CCDLANBearerRecord>* ptrLanBearerRecordSet = new( ELeave )
            CMDBRecordSet<CCDLANBearerRecord>( KCDTIdLANBearerRecord );
    CleanupStack::PushL( ptrLanBearerRecordSet );
    ptrLanBearerRecordSet->LoadL( aDb );

    TInt lanBearerRecordCount( ptrLanBearerRecordSet->iRecords.Count() );
    for ( TInt i = 0; i < lanBearerRecordCount; i++ )
        {
        TLanBearerEntry lanBearerEntry;
        lanBearerEntry.iId = ptrLanBearerRecordSet->iRecords[i]->RecordId();

        TBuf<KMaxTextLength> bearerRecordName(
                ( (CCDLANBearerRecord*)ptrLanBearerRecordSet->iRecords[i] )->iRecordName.GetL() );

        if ( bearerRecordName == TPtrC( KWlanBearerRecordName ) )
            {
            lanBearerEntry.iWlanBearer = ETrue;
            }
        else
            {
            lanBearerEntry.iWlanBearer = EFalse;
            }
        aLanBearerTableCache.Append( lanBearerEntry );
        }
    CleanupStack::PopAndDestroy( ptrLanBearerRecordSet );
    LOGIT2("LAN bearer record count %d/%d", aLanBearerTableCache.Count(), lanBearerRecordCount)

    LOGEXITFN("CConnMonCommsDatCache::ReadCommsDatLanBearerTableL()")
    }

// ---------------------------------------------------------------------------
// Reads all SNAP information from CommsDat and updates the cache if necessary.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::RefreshCommsDatSnapCacheL( CMDBSession& aDb )
    {
    LOGENTRFN("CConnMonCommsDatCache::RefreshCommsDatSnapCacheL()")

    RArray<TConnMonSnapEntry>* currentSnapData;
    if ( iInitStatus == EConnMonCacheInitCompleted )
        {
        // Array to read current CommsDat info into.
        currentSnapData = new( ELeave ) RArray<TConnMonSnapEntry>; // Heap used by design
        CleanupClosePushL( *currentSnapData );
        }
    else
        {
        // Init phase. Read CommsDat and store to cache. Then stop. (no events)
        currentSnapData = iSnapCache; // Cache is still empty at init-phase.
        }
    currentSnapData->Reset();

    ReadCommsDatSnapTableL( aDb, *currentSnapData );

    if ( iInitStatus != EConnMonCacheInitCompleted )
        {
        // This is Init pass. CommsDat SNAPs have been read and stored in cache. return now.
        // Note, currentSnapData not in cleanup stack
        iSnapsChanged = ETrue;
        LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatSnapCache()", iSnapsChanged)
        return iSnapsChanged;
        }

    CleanupStack::Pop( currentSnapData );
    iSnapsChanged = CompareSortedArrays( *currentSnapData, *iSnapCache );

    // Delete the obsolete SNAP cache table. Either the new table that was just
    // read, or the old table if the new one is different.
    if ( iSnapsChanged )
        {
        iSnapCache->Close();
        delete iSnapCache;
        iSnapCache = currentSnapData;
        LOGIT("RefreshCommsDatSnapCacheL: updated SNAP cache")
        }
    else
        {
        currentSnapData->Close();
        delete currentSnapData;
        currentSnapData = NULL;
        }

    LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatSnapCacheL()", iSnapsChanged)
    return iSnapsChanged;
    }

// ---------------------------------------------------------------------------
// Reads all SNAP information from CommsDat to an array.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ReadCommsDatSnapTableL(
        CommsDat::CMDBSession& aDb,
        RArray<TConnMonSnapEntry>& aCurrentSnapData )
    {
    LOGENTRFN("CConnMonCommsDatCache::ReadCommsDatSnapTableL()")
    // Explicitly build a TLinearOrder<class>. Used as parameter to RArray::InsertInOrder().
    TLinearOrder<TConnMonSnapEntry> snapEntryOrderingLogic( TConnMonSnapEntry::Compare );

    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* ptrSnapRecordSet = new( ELeave )
            CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( iSnapRecordTableId );
    CleanupStack::PushL( ptrSnapRecordSet );

    CCDDataMobilitySelectionPolicyRecord* ptrSnapRecord = new( ELeave )
            CCDDataMobilitySelectionPolicyRecord( iSnapRecordTableId );
    CleanupStack::PushL( ptrSnapRecord );

    ptrSnapRecordSet->LoadL( aDb );
    TInt snapRecordCount( ptrSnapRecordSet->iRecords.Count() );
    LOGIT1("ReadCommsDatSnapTableL: SNAP record count %d (commsdat)", snapRecordCount)

    TUint snapId( 0 );
    TUint includedIapId( 0 );
    TUint embeddedSnapId( 0 );
    TInt tempSnapId( 0 );
    TInt tempEmbeddedSnapId( 0 );

    for ( TInt i = 0; i < snapRecordCount; i++ )
        {
        ptrSnapRecord->SetElementId( ptrSnapRecordSet->iRecords[i]->ElementId() );
        ptrSnapRecord->LoadL( aDb );

        // SNAP ID records in CCDDataMobilitySelectionPolicyRecord-table were changed
        // from record links to TInt type, as part of new CommsInfra changes.
        tempSnapId = ptrSnapRecord->iSNAP;
        if ( tempSnapId < 0 )
            {
            tempSnapId = 0;
            }
        snapId = (TUint) tempSnapId;

        includedIapId = ( ptrSnapRecord->iIAP & KCDMaskShowRecordId ) >> KBitsInByte;

        tempEmbeddedSnapId = ptrSnapRecord->iEmbeddedSNAP;
        if ( tempEmbeddedSnapId < 0 )
            {
            tempEmbeddedSnapId = 0;
            }
        embeddedSnapId = (TUint) tempEmbeddedSnapId;

        // Empty SNAPs are not included in cache
        if ( includedIapId || embeddedSnapId )
            {
            TConnMonSnapEntry snapEntry( snapId, includedIapId, embeddedSnapId );
            TInt err = aCurrentSnapData.InsertInOrder( snapEntry , snapEntryOrderingLogic );
            if ( err )
                {
                LOGIT1("ERROR inserting SNAP record to current SNAP data <%d>", err)
                }
            }
        }
    CleanupStack::PopAndDestroy( ptrSnapRecord );
    CleanupStack::PopAndDestroy( ptrSnapRecordSet );
    LOGIT1("ReadCommsDatSnapTableL: SNAP record count %d (cache)", aCurrentSnapData.Count())
    LOGEXITFN("CConnMonCommsDatCache::ReadCommsDatSnapTableL()")
    }

// ---------------------------------------------------------------------------
// Reads all virtual IAP link information from CommsDat and updates the cache
// if necessary. CommsDat is not read if IAP table did not contain any virtual
// IAPs. Returns ETrue if cache information was changed, EFalse otherwise.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::RefreshCommsDatVirtualIapCacheL( CMDBSession& aDb )
    {
    LOGENTRFN("CConnMonCommsDatCache::RefreshCommsDatVirtualIapCacheL()")

    RArray<TConnMonVirtualIapEntry>* currentVirtualIapData;
    if ( iInitStatus == EConnMonCacheInitCompleted )
        {
        // Array to read current CommsDat info into.
        currentVirtualIapData = new( ELeave ) RArray<TConnMonVirtualIapEntry>; // Heap used by design
        CleanupClosePushL( *currentVirtualIapData );
        }
    else
        {
        // Init-phase, read current CommsDat info directly into cache array,
        // since it is still empty. Then stop (don't send any events.)
        currentVirtualIapData = iVirtualIapCache;
        }
    currentVirtualIapData->Reset();

    // If there is no virtual IAPs, there is no need to read virtual IAP link
    // information, and virtual IAP cache will be left empty.
    if ( iVirtualIapCount )
        {
        TRAPD( leaveCode, ReadCommsDatVirtualIapTableL( aDb, *currentVirtualIapData ) );
        if ( leaveCode )
            {
            LOGIT1("ERROR reading virtual IAP table, LEAVE with <%d>", leaveCode)
            }
        }
    else
        {
        LOGIT("RefreshCommsDatVirtualIapCacheL: no virtual IAPs, skipping")
        }

    if ( iInitStatus != EConnMonCacheInitCompleted )
        {
        // This is Init pass. CommsDat has been read and stored in cache. return now.
        // Note, currentVirtualIapData not in cleanup stack
        iVirtualIapsChanged = ETrue;
        LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatVirtualIapCacheL()", iVirtualIapsChanged)
        return iVirtualIapsChanged;
        }

    CleanupStack::Pop( currentVirtualIapData );
    iVirtualIapsChanged = CompareSortedArrays( *currentVirtualIapData, *iVirtualIapCache );

    // Delete the obsolete virtual IAP cache table. Either the new table that
    // was just read, or the old table if the new one is different.
    if ( iVirtualIapsChanged )
        {
        iVirtualIapCache->Close();
        delete iVirtualIapCache;
        iVirtualIapCache = currentVirtualIapData;
        LOGIT("RefreshCommsDatVirtualIapCacheL: updated virtual IAP cache")
        }
    else
        {
        currentVirtualIapData->Close();
        delete currentVirtualIapData;
        currentVirtualIapData = NULL;
        }

    LOGEXITFN1("CConnMonCommsDatCache::RefreshCommsDatVirtualIapCacheL()", iVirtualIapsChanged)
    return iVirtualIapsChanged;
    }

// ---------------------------------------------------------------------------
// Reads all virtual IAP link information from CommsDat to an array.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ReadCommsDatVirtualIapTableL(
        CommsDat::CMDBSession& aDb,
        RArray<TConnMonVirtualIapEntry>& aCurrentVirtualIapData )
    {
    LOGENTRFN("CConnMonCommsDatCache::ReadCommsDatVirtualIapTableL()")

    // Explicitly build a TLinearOrder<class>. Used as parameter to RArray::InsertInOrder().
    TLinearOrder<TConnMonVirtualIapEntry> virtualEntryOrderingLogic(
            TConnMonVirtualIapEntry::Compare );

    CMDBRecordSet<CCDVirtualIAPNextLayerRecord>* ptrVirtualRecordSet = new( ELeave )
            CMDBRecordSet<CCDVirtualIAPNextLayerRecord>( iVirtualRecordTableId );
    CleanupStack::PushL( ptrVirtualRecordSet );

    CCDVirtualIAPNextLayerRecord* ptrVirtualRecord = new( ELeave )
            CCDVirtualIAPNextLayerRecord( iVirtualRecordTableId );
    CleanupStack::PushL( ptrVirtualRecord );

    // Load and loop through the records.
    ptrVirtualRecordSet->LoadL( aDb );
    TInt virtualRecordCount( ptrVirtualRecordSet->iRecords.Count() );
    LOGIT1("ReadCommsDatVirtualIapTableL: virtual record count %d (commsdat)", virtualRecordCount)

    TUint iapId( 0 );
    TUint nextLayerSnap( 0 );
    TUint nextLayerIap( 0 );

    for ( TInt i = 0; i < virtualRecordCount; i++ )
        {
        ptrVirtualRecord->SetElementId( ptrVirtualRecordSet->iRecords[i]->ElementId() );
        ptrVirtualRecord->LoadL( aDb );

        iapId = ptrVirtualRecord->iIAP;
        nextLayerSnap = ptrVirtualRecord->iNextLayerSNAP;
        nextLayerIap = ptrVirtualRecord->iNextLayerIAP;

        // Either iNextLayerSNAP or iNextLayerIAP must be NULL. Only 1 link.
        if ( iapId && ( ( nextLayerSnap && !nextLayerIap ) ||
                ( nextLayerIap && !nextLayerSnap ) ) )
            {
            TConnMonVirtualIapEntry virtualEntry( iapId, nextLayerIap, nextLayerSnap );
            TInt err = aCurrentVirtualIapData.InsertInOrder(
                    virtualEntry,
                    virtualEntryOrderingLogic );
            if ( err )
                {
                LOGIT1("ERROR inserting virtual IAP link record to cache <%d>", err)
                }
            }
        else
            {
            LOGIT2("WARNING, invalid virtual record, iap: %d, snap: %d", nextLayerIap, nextLayerSnap )
            }
        }
    CleanupStack::PopAndDestroy( ptrVirtualRecord );
    CleanupStack::PopAndDestroy( ptrVirtualRecordSet );

    LOGIT1("ReadCommsDatVirtualIapTableL: virtual record count %d (cache)", aCurrentVirtualIapData.Count())
    LOGEXITFN("CConnMonCommsDatCache::ReadCommsDatVirtualIapTableL()")
    }

// ---------------------------------------------------------------------------
// Compares two sorted RArray<TUint> arrays. Returns ETrue if the arrays are
// not identical.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::CompareSortedArrays(
        const RArray<TUint>& aFirstArray,
        const RArray<TUint>& aSecondArray )
    {
    //LOGENTRFN("CConnMonCommsDatCache::CompareSortedArrays()")
    TBool arraysDiffer( EFalse );

    const TInt firstCount = aFirstArray.Count();
    if ( firstCount != aSecondArray.Count() )
        {
        arraysDiffer = ETrue;
        }
    else
        {
        for ( TInt i = 0; i < firstCount; i++ )
            {
            if ( aFirstArray[i] != aSecondArray[i] )
                {
                arraysDiffer = ETrue;
                break; // No need to continue
                }
            }
        }

    //LOGEXITFN1("CConnMonCommsDatCache::CompareSortedArrays()", arraysDiffer)
    return arraysDiffer;
    }

// ---------------------------------------------------------------------------
// Compares two sorted RArray<TConnMonIapEntry> arrays. Returns ETrue if the
// arrays are not identical.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::CompareSortedArrays(
        const RArray<TConnMonIapEntry>& aFirstArray,
        const RArray<TConnMonIapEntry>& aSecondArray )
    {
    //LOGENTRFN("CConnMonCommsDatCache::CompareSortedArrays()")
    TBool arraysDiffer( EFalse );

    const TInt firstCount = aFirstArray.Count();
    if ( firstCount != aSecondArray.Count() )
        {
        arraysDiffer = ETrue;
        }
    else
        {
        for ( TInt i = 0; i < firstCount; i++ )
            {
            if ( !TConnMonIapEntry::Match( aFirstArray[i], aSecondArray[i] ) )
                {
                arraysDiffer = ETrue;
                break; // No need to continue
                }
            }
        }

    //LOGEXITFN1("CConnMonCommsDatCache::CompareSortedArrays()", arraysDiffer)
    return arraysDiffer;
    }

// ---------------------------------------------------------------------------
// Compares two sorted RArray<TConnMonSnapEntry> arrays. Returns ETrue if the
// arrays are not identical.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::CompareSortedArrays(
        const RArray<TConnMonSnapEntry>& aFirstArray,
        const RArray<TConnMonSnapEntry>& aSecondArray )
    {
    //LOGENTRFN("CConnMonCommsDatCache::CompareSortedArrays()")
    TBool arraysDiffer( EFalse );

    const TInt firstCount = aFirstArray.Count();
    if ( firstCount != aSecondArray.Count() )
        {
        arraysDiffer = ETrue;
        }
    else
        {
        for ( TInt i = 0; i < firstCount; i++ )
            {
            if ( !TConnMonSnapEntry::Match( aFirstArray[i], aSecondArray[i] ) )
                {
                arraysDiffer = ETrue;
                break; // No need to continue
                }
            }
        }

    //LOGEXITFN1("CConnMonCommsDatCache::CompareSortedArrays()", arraysDiffer)
    return arraysDiffer;
    }

// ---------------------------------------------------------------------------
// Compares two sorted RArray<TConnMonVirtualIapEntry> arrays. Returns ETrue
// if the arrays are not identical.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::CompareSortedArrays(
        const RArray<TConnMonVirtualIapEntry>& aFirstArray,
        const RArray<TConnMonVirtualIapEntry>& aSecondArray )
    {
    //LOGENTRFN("CConnMonCommsDatCache::CompareSortedArrays()")
    TBool arraysDiffer( EFalse );

    const TInt firstCount = aFirstArray.Count();
    if ( firstCount != aSecondArray.Count() )
        {
        arraysDiffer = ETrue;
        }
    else
        {
        for ( TInt i = 0; i < firstCount; i++ )
            {
            if ( !TConnMonVirtualIapEntry::Match( aFirstArray[i], aSecondArray[i] ) )
                {
                arraysDiffer = ETrue;
                break; // No need to continue
                }
            }
        }

    //LOGEXITFN1("CConnMonCommsDatCache::CompareSortedArrays()", arraysDiffer)
    return arraysDiffer;
    }

// ---------------------------------------------------------------------------
// Copy new data into a RArray<TUint> array.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::DeepCopy(
        const RArray<TUint>& aSourceArray,
        RArray<TUint>& aTargetArray )
    {
    //LOGENTRFN("CConnMonCommsDatCache::DeepCopy()")

    const TInt count = aSourceArray.Count();
    aTargetArray.Reset();
    TInt err = aTargetArray.Reserve( count );
    if ( KErrNone == err )
        {
        for ( TInt i = 0; i < count; i++ )
            {
            aTargetArray.Append( aSourceArray[i] );
            }
        }

    //LOGEXITFN("CConnMonCommsDatCache::DeepCopy()")
    }

// ---------------------------------------------------------------------------
// Updates the index links for SNAPs and virtual IAPs.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::UpdateSnapAndVirtualIapLinks()
    {
    LOGENTRFN("CConnMonCommsDatCache::UpdateSnapAndVirtualIapLinks()")

    TInt count = iSnapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TConnMonSnapEntry* pSnap = &(*iSnapCache)[i];
        if ( pSnap->iNextLayerIapId )
            {
            pSnap->iNextLayerIndex = FindIapIndex( pSnap->iNextLayerIapId );
            }
        else if ( pSnap->iNextLayerSnapId )
            {
            pSnap->iNextLayerIndex = FindSnapIndex( pSnap->iNextLayerSnapId );
            }
        }

    count = iIapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TConnMonIapEntry* pIap = &(*iIapCache)[i];
        // Only virtual IAPs can have links
        if ( pIap->iBearerType == EConnMonCacheBearerTypeVirtual )
            {
            TInt index = iVirtualIapCache->FindInOrder<TInt>(
                    pIap->iId,
                    TConnMonVirtualIapEntry::FindCompare );

            if ( index < 0)
                {
                pIap->iNextLayerIapId = 0;
                pIap->iNextLayerSnapId = 0;
                pIap->iNextLayerIndex = KErrNotFound;
                }
            else
                {
                pIap->iNextLayerIapId = (*iVirtualIapCache)[index].iNextLayerIapId;
                pIap->iNextLayerSnapId = (*iVirtualIapCache)[index].iNextLayerSnapId;

                // Either iNextLayerIapId or iNextLayerSnapId is 0. (already checked)
                // Link to SNAP is more likely than link to IAP
                if ( pIap->iNextLayerSnapId )
                    {
                    // Returns KErrNotFound if not found
                    pIap->iNextLayerIndex = FindSnapIndex( pIap->iNextLayerSnapId );
                    }
                else if ( pIap->iNextLayerIapId )
                    {
                    // Returns KErrNotFound if not found
                    pIap->iNextLayerIndex = FindIapIndex( pIap->iNextLayerIapId );
                    }
                }
            }
        }

    LOGEXITFN("CConnMonCommsDatCache::UpdateSnapAndVirtualIapLinks()")
    }

// ---------------------------------------------------------------------------
// Resets the availability status for all SNAPs and IAPs. SNAPs and virtual
// IAPs are set to unknown, because their availability depends on real IAPs
// and needs to be solved using recursive algorithms. Real IAPs are set to
// unavailable. The next step should be to find the real IAPs that are
// available.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::ResetAllAvailabilityInfo()
    {
    //LOGENTRFN("CConnMonCommsDatCache::ResetAllAvailabilityInfo()")

    TInt count = iIapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( (*iIapCache)[i].iBearerType == EConnMonCacheBearerTypeVirtual )
            {
            (*iIapCache)[i].iAvailability = EConnMonAvailabilityUnknown;
            }
        else
            {
            (*iIapCache)[i].iAvailability = EConnMonAvailabilityUnavailable;
            }
        }

    count = iSnapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        (*iSnapCache)[i].iAvailability = EConnMonAvailabilityUnknown;
        }

    //LOGEXITFN("CConnMonCommsDatCache::ResetAllAvailabilityInfo()")
    }

// ---------------------------------------------------------------------------
// Solve SNAP and virtual IAP availability. The results depend on real IAP
// availability, so those need to be set when this method is called.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::SolveSnapAndVirtualIapAvailability()
    {
    LOGENTRFN("CConnMonCommsDatCache::SolveSnapAndVirtualIapAvailability()")

    TInt count = iSnapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Check SNAP availability recursively at index i
        CheckSnapAvailability( i );
        }

    count = iIapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( (*iIapCache)[i].iBearerType == EConnMonCacheBearerTypeVirtual )
            {
            // Check IAP availability recursively at index i
            CheckIapAvailability( i );
            }
        }

    LOGEXITFN("CConnMonCommsDatCache::SolveSnapAndVirtualIapAvailability()")
    }

// ---------------------------------------------------------------------------
// Create a EConnMonIapAvailabilityChange event and add it to the event
// queue to be sent to clients.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::SendIapAvailabilityEvent()
    {
    LOGENTRFN("CConnMonCommsDatCache::SendIapAvailabilityEvent()")
    TInt err( KErrNone );

    TInt iapCount = iIapIdCache.Count();
    if ( iapCount > KConnMonMaxIAPCount )
        {
        // If too many IAPs available, clip the extras away from event.
        LOGIT1("WARNING, too many IAPs (%d), all did not fit into event", iapCount)
        iapCount = KConnMonMaxIAPCount;
        }

    TEventInfo info;
    info.Reset();
    info.iEventType = EConnMonIapAvailabilityChange;
    info.iConnectionId = EBearerIdAll;
    info.iData = iapCount;

    // Create event data area. Leave left out intentionally, check for NULL instead
    TConnMonIapInfo* eventIaps = new TConnMonIapInfo; // No (ELeave)
    if ( !eventIaps )
        {
        err = KErrNoMemory;
        }
    else
        {
        eventIaps->iCount = iapCount;
        TInt i = 0;
        for ( ; i < iapCount; i++ )
            {
            eventIaps->iIap[i].iIapId = iIapIdCache[i];
            }
        for ( ; i < KConnMonMaxIAPCount; i++ )
            {
            eventIaps->iIap[i].iIapId = 0;
            }

        // EventQueue will finally destroy the memory area pointed by eventIaps.
        // However, if all the clients can't receive the event right away the
        // event queue will keep it until the last client has received it.
        iServer->EventQueue()->Add(
                info,
                reinterpret_cast<const TUint8*>( eventIaps ),
                sizeof( TConnMonIapInfo ) );
        }

    LOGEXITFN1("CConnMonCommsDatCache::SendIapAvailabilityEvent()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Create a EConnMonSNAPsAvailabilityChange event and add it to the event
// queue to be sent to clients.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::SendSnapAvailabilityEvent()
    {
    LOGENTRFN("CConnMonCommsDatCache::SendSnapAvailabilityEvent()")
    TInt err( KErrNone );

    TInt snapCount = iSnapIdCache.Count();

    TEventInfo info;
    info.Reset();
    info.iEventType = EConnMonSNAPsAvailabilityChange;
    info.iConnectionId = EBearerIdAll;
    info.iData = snapCount; // Total amount of available SNAPs

    if ( snapCount > KConnMonMaxSNAPsCount )
        {
        // If too many SNAPs available, clip the extras away from event.
        LOGIT1("WARNING, too many SNAPs (%d), all did not fit into event", snapCount)
        snapCount = KConnMonMaxSNAPsCount;
        }

    // Create event data area. Leave left out intentionally, check for NULL instead
    TConnMonSNAPInfo* eventSnaps = new TConnMonSNAPInfo; // No (ELeave)
    if ( !eventSnaps )
        {
        err = KErrNoMemory;
        }
    else
        {
        eventSnaps->iCount = snapCount;
        TInt i = 0;
        for ( ; i < snapCount; i++ )
            {
            eventSnaps->iSNAP[i].iSNAPId = iSnapIdCache[i];
            }
        for ( ; i < KConnMonMaxSNAPsCount; i++ )
            {
            eventSnaps->iSNAP[i].iSNAPId = 0;
            }

        // EventQueue will finally destroy the memory area pointed by eventSnaps.
        // However, if all the clients can't receive the event right away the
        // event queue will keep it until the last client has received it.
        iServer->EventQueue()->Add(
                info,
                reinterpret_cast<const TUint8*>( eventSnaps ),
                sizeof( TConnMonSNAPInfo ) );
        }

    LOGEXITFN1("CConnMonCommsDatCache::SendSnapAvailabilityEvent()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Finds the index in cache for IAP with id aId. Returns KErrNotFound (-1) if
// no match is found.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::FindIapIndex( const TUint aId )
    {
    TInt index = iIapCache->FindInOrder<TInt>( aId, TConnMonIapEntry::FindCompare );
    return index;
    }

// ---------------------------------------------------------------------------
// Finds the first index in cache for SNAP with ID aId. Returns KErrNotFound
// (-1) if no match is found.
// The SNAP cache is sorted by ID, so the first index is found by traversing
// decrementally from the initial index returned by RArray's FindInOrder.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::FindSnapIndex( const TUint aId )
    {
    TInt index  = iSnapCache->FindInOrder<TInt>( aId, TConnMonSnapEntry::FindCompare );
    for ( ; index > 0; index-- )
        {
        if ( (*iSnapCache)[index-1].iId != aId )
            {
            return index;
            }
        }
    return index;
    }

// ---------------------------------------------------------------------------
// Takes a SNAP cache index and reads the ID from that entry. Then adjusts the
// availability for each cache entry related to that ID.
// ---------------------------------------------------------------------------
//
void CConnMonCommsDatCache::SetSnapAvailabilityAtIndex( TUint aIndex, TInt aAvailability )
    {
    //LOGENTRFN("CConnMonCommsDatCache::SetSnapAvailabilityAtIndex()")

    TUint id = (*iSnapCache)[aIndex].iId;
    (*iSnapCache)[aIndex].iAvailability = aAvailability;

    TInt count = iSnapCache->Count();
    for ( TInt i = aIndex+1; i < count; i++ )
        {
        if ( (*iSnapCache)[i].iId != id )
            {
            return;
            }
        (*iSnapCache)[i].iAvailability = aAvailability;
        }

    //LOGEXITFN("CConnMonCommsDatCache::SetSnapAvailabilityAtIndex()")
    }

// ---------------------------------------------------------------------------
// Check IAP availability by index. In case of virtual IAP, keeps checking
// recursively until something is found available or all link chains are
// checked.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::CheckIapAvailability( TUint aIndex )
    {
    TConnMonIapEntry* pIap = &(*iIapCache)[aIndex];
    TInt currentAvailability = pIap->iAvailability;
    switch ( currentAvailability )
        {
        // Real IAPs should have availability status of either available or unavailable at this
        // point. Unprocessed virtual IAPs and SNAPs should have availability status unknown.
        //
        case EConnMonAvailabilityProcessing: // Loop. Fallthrough intended
        case EConnMonAvailabilityUnavailable:
            return EConnMonAvailabilityUnavailable;
        case EConnMonAvailabilityAvailable:
            return EConnMonAvailabilityAvailable;
        case EConnMonAvailabilityUnknown:
            {
            if ( pIap->iNextLayerIndex >= 0 )
                {
                // Set availability status to processing (to detect/prevent loops)
                pIap->iAvailability = EConnMonAvailabilityProcessing;
                if ( pIap->iNextLayerSnapId )
                    {
                    pIap->iAvailability = CheckSnapAvailability( pIap->iNextLayerIndex );
                    }
                else
                    {
                    pIap->iAvailability = CheckIapAvailability( pIap->iNextLayerIndex );
                    }
                }
            else
                {
                // Virtual IAP that does not point anywhere
                pIap->iAvailability = EConnMonAvailabilityUnavailable;
                }
            }
            break;
        default:
            LOGIT1("ERROR, IAP entry contained invalid availability value: %d", currentAvailability)
            return EConnMonAvailabilityUnavailable;
        }
    return pIap->iAvailability;
    }

// ---------------------------------------------------------------------------
// Check SNAP availability by index. Keeps checking recursively until something
// is found available inside or all contents of this SNAP have are checked.
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::CheckSnapAvailability( TUint aIndex )
    {
    TInt currentAvailability = (*iSnapCache)[aIndex].iAvailability;
    switch ( currentAvailability )
        {
        case EConnMonAvailabilityProcessing: // Loop. Fallthrough intended
        case EConnMonAvailabilityUnavailable:
            return EConnMonAvailabilityUnavailable;
        case EConnMonAvailabilityAvailable:
            return EConnMonAvailabilityAvailable;
        case EConnMonAvailabilityUnknown:
            {
            // Set availability status to processing (to detect/prevent loops)
            SetSnapAvailabilityAtIndex( aIndex, EConnMonAvailabilityProcessing );

            const TUint id = (*iSnapCache)[aIndex].iId;
            const TInt count = iSnapCache->Count();

            currentAvailability = EConnMonAvailabilityUnavailable;
            for ( TInt i = aIndex; i < count; i++ )
                {
                if ( (*iSnapCache)[i].iId != id )
                    {
                    // This line belongs to next SNAP, finish here
                    break;
                    }
                TInt link = (*iSnapCache)[i].iNextLayerIndex;
                if ( link >= 0 )
                    {
                    if ( (*iSnapCache)[i].iNextLayerIapId )
                        {
                        currentAvailability = CheckIapAvailability( link );
                        }
                    else
                        {
                        currentAvailability = CheckSnapAvailability( link );
                        }
                    }
                if ( currentAvailability == EConnMonAvailabilityAvailable )
                    {
                    // Something inside this SNAP is available, no need to continue
                    break;
                    }
                }
            // Set this SNAP as available (all lines in cache that belong to this SNAP)
            SetSnapAvailabilityAtIndex( aIndex, currentAvailability );
            }
            break;
        default:
            LOGIT("ERROR, SNAP entry contained invalid availability value")
            return EConnMonAvailabilityUnavailable;
        }
    return currentAvailability;
    }

// ---------------------------------------------------------------------------
// Updates the internal list of available IAP IDs and returns ETrue if it has
// changed.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::UpdateAvailableIaps()
    {
    //LOGENTRFN("CConnMonCommsDatCache::UpdateAvailableIaps()")

    RArray<TUint> currentIapIds;

    // Read available IAP IDs from IAP cache
    const TInt count = iIapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( (*iIapCache)[i].iAvailability == EConnMonAvailabilityAvailable )
            {
            TInt err = currentIapIds.InsertInOrder( (*iIapCache)[i].iId );
            if ( KErrNone != err )
                {
                LOGIT1("ERROR inserting IAP ID to available IAPs cache <%d>", err)
                break;
                }
            }
        }

    // Check for changes
    TBool arrayChanged = CompareSortedArrays( currentIapIds, iIapIdCache );
    if ( arrayChanged )
        {
        LOGIT1("Available IAP count: %d (changed)", currentIapIds.Count())
        DeepCopy( currentIapIds, iIapIdCache );
        }
    else
        {
        LOGIT1("Available IAP count: %d", currentIapIds.Count())
        }
    currentIapIds.Close();

    //LOGEXITFN1("CConnMonCommsDatCache::UpdateAvailableIaps()", arrayChanged)
    return arrayChanged;
    }

// ---------------------------------------------------------------------------
// Updates the internal list of available SNAP IDs and returns ETrue if it has
// changed.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::UpdateAvailableSnaps()
    {
    //LOGENTRFN("CConnMonCommsDatCache::UpdateAvailableSnaps()")

    TUint lastSnapId( 0 );
    RArray<TUint> currentSnapIds;

    // Read available SNAP IDs from SNAP cache.
    // If there is n items inside a SNAP, then SNAP cache will contain n entries for that SNAP
    const TInt count = iSnapCache->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TUint currentId = (*iSnapCache)[i].iId;
        if ( currentId != lastSnapId )
            {
            lastSnapId = currentId;
            if ( (*iSnapCache)[i].iAvailability == EConnMonAvailabilityAvailable )
                {
                TInt err = currentSnapIds.InsertInOrder( currentId );
                if ( KErrNone != err )
                    {
                    LOGIT1("ERROR inserting SNAP ID to available SNAPs cache <%d>", err)
                    break;
                    }
                }
            }
        }

    // Check for changes
    TBool arrayChanged = CompareSortedArrays( currentSnapIds, iSnapIdCache );
    if ( arrayChanged )
        {
        LOGIT1("Available SNAP count: %d (changed)", currentSnapIds.Count())
        DeepCopy( currentSnapIds, iSnapIdCache );
        }
    else
        {
        LOGIT1("Available SNAP count: %d", currentSnapIds.Count())
        }
    currentSnapIds.Close();

    //LOGEXITFN1("CConnMonCommsDatCache::UpdateAvailableSnaps()", arrayChanged)
    return arrayChanged;
    }

// ---------------------------------------------------------------------------
// Checks from 'LAN bearer table' array if given ID has a WLAN bearer
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::HasWlanBearer(
        TUint32 aId,
        RArray<TLanBearerEntry>& aLanBearerTableCache )
    {
    //LOGENTRFN("CConnMonCommsDatCache::HasWlanBearer()")
    TBool wlanBearer( EFalse );

    for ( TInt i = 0; i < aLanBearerTableCache.Count(); i++ )
        {
        if ( aLanBearerTableCache[i].iId == aId )
            {
            wlanBearer = aLanBearerTableCache[i].iWlanBearer;
            break;
            }
        }

    //LOGEXITFN1("CConnMonCommsDatCache::HasWlanBearer()", wlanBearer)
    return wlanBearer;
    }

// ---------------------------------------------------------------------------
// Convert a bearer ID to (cache internal) service type
// ---------------------------------------------------------------------------
//
TInt CConnMonCommsDatCache::ConvertBearerIdToServiceType(
        const TUint aBearerId,
        TUint& aServiceType )
    {
    //LOGENTRFN("CConnMonCommsDatCache::ConvertBearerIdToServiceType()")
    TInt err( KErrNone );

    switch ( aBearerId )
        {
        case EBearerIdGPRS:
        case EBearerIdWCDMA:
            aServiceType = EConnMonCacheServiceTypeGprs;
            break;
        case EBearerIdCSD:
            aServiceType = EConnMonCacheServiceTypeCsd;
            break;
        case EBearerIdWLAN:
            aServiceType = EConnMonCacheServiceTypeWlan;
            break;
        case EBearerIdLAN:
            aServiceType = EConnMonCacheServiceTypeLan;
            break;
        default:
            LOGIT1("WARNING, ConvertBearerIdToServiceType() called with bad bearerId: %d", aBearerId)
            aServiceType = EConnMonCacheServiceTypeUnknown;
            err = KErrNotFound;
        }

    //LOGEXITFN1("CConnMonCommsDatCache::ConvertBearerIdToServiceType()", err)
    return err;
    }

// ---------------------------------------------------------------------------
// Checks from CommsDat if WLAN background scans are enabled.
// ---------------------------------------------------------------------------
//
TBool CConnMonCommsDatCache::IsWlanBackgroundScanningEnabledL()
    {
    LOGENTRFN("CConnMonCommsDatCache::IsWlanBackgroundScanningEnabledL()")

    TBool bgScanEnabled( EFalse );
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );

    CCDWlanDeviceSettingsRecord* ptrWlanDevSettingsRecord = new( ELeave )
    CCDWlanDeviceSettingsRecord( CCDWlanDeviceSettingsRecord::TableIdL( *db ) );
    CleanupStack::PushL( ptrWlanDevSettingsRecord );

    ptrWlanDevSettingsRecord->iWlanDeviceSettingsType = KWlanUserSettings;
    if( ptrWlanDevSettingsRecord->FindL( *db ) )
        {
        ptrWlanDevSettingsRecord->LoadL( *db );
        if ( ptrWlanDevSettingsRecord->iBgScanInterval > 0 )
            {
            bgScanEnabled = ETrue;
            LOGIT1("WLAN background scans are enabled, interval: %d", (TUint)ptrWlanDevSettingsRecord->iBgScanInterval)
            }
        }
    CleanupStack::PopAndDestroy( ptrWlanDevSettingsRecord );
    CleanupStack::PopAndDestroy( db );

    LOGEXITFN1("CConnMonCommsDatCache::IsWlanBackgroundScanningEnabledL()", bgScanEnabled)
    return bgScanEnabled;
    }

// End-of-file
