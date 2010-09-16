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
* Database cache manager.
*
*/


#include <e32base.h>
#include <cmpluginbaseeng.h>
#include <cmdefconnvalues.h>
#include <cmpluginembdestinationdef.h>

#include <es_sock.h>    // RSocketServ, RConnection
#include <in_sock.h>    // KAfInet
#include <es_enum.h>    // TConnectionInfo

#include "cmmdestinationstruct.h"
#include "cmmlistenermanager.h"
#include "cmminstancemapping.h"

#include "cmmcache.h"

#include "cmmdestinationinstance.h"
#include "cmmconnmethodinstance.h"

#include "cmmtransactionhandler.h"
#include "cmmanagerimpl.h"
#include "cmmbearerprioritycache.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcacheTraces.h"
#endif


// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmCache* CCmmCache::NewL(
        CCmManagerImpl* aCmManagerImpl,
        CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins )
    {
    OstTraceFunctionEntry0( CCMMCACHE_NEWL_ENTRY );

    CCmmCache* self = CCmmCache::NewLC( aCmManagerImpl, aPlugins );
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMCACHE_NEWL_EXIT );

    return self;
    }

// ---------------------------------------------------------------------------
// Two phased construction.
// ---------------------------------------------------------------------------
//
CCmmCache* CCmmCache::NewLC(
        CCmManagerImpl* aCmManagerImpl,
        CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins )
    {
    OstTraceFunctionEntry0( CCMMCACHE_NEWLC_ENTRY );

    CCmmCache* self = new( ELeave ) CCmmCache( aCmManagerImpl, aPlugins );
    CleanupStack::PushL( self );
    self->ConstructL();

    OstTraceFunctionExit0( CCMMCACHE_NEWLC_EXIT );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CCmmCache::~CCmmCache()
    {
    OstTraceFunctionEntry0( CCMMCACHE_CCMMCACHE_ENTRY );

    delete iBearerPriorityCache;
    delete iListenerManager;
    delete iInstanceMapping;

    iConnMethodArray.ResetAndDestroy();
    iDestinationArray.ResetAndDestroy();

    iDeletedConnMethods.Close();
    iUpdatedConnMethods.Close();
    iUpdatedConnMethods2.Close(); // Does not own contents.
    iUpdatedDestinations.Close();
    iUpdatedDestinations2.Close(); // Does not own contents.

    OstTraceFunctionExit0( CCMMCACHE_CCMMCACHE_EXIT );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CCmmCache::CCmmCache(
        CCmManagerImpl* aCmManagerImpl,
        CArrayPtrFlat<const CCmPluginBaseEng>* aPlugins )
        :
        iPlugins( aPlugins ),
        iCmManagerImpl( aCmManagerImpl )
    {
    OstTraceFunctionEntry0( DUP1_CCMMCACHE_CCMMCACHE_ENTRY );

    iListenerManager = NULL;
    iInstanceMapping = NULL;
    iTrans = NULL;
    iBearerPriorityCache = NULL;
    iCurrentTemporaryId = KTemporaryIdCounterStart;

    iSnapTableId = 0;
    iSnapMetadataTableId = 0;

    OstTraceFunctionExit0( DUP1_CCMMCACHE_CCMMCACHE_EXIT );
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCmmCache::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONSTRUCTL_ENTRY );

    iTrans = iCmManagerImpl->GetTransactionHandler();

    iBearerPriorityCache = CCmmBearerPriorityCache::NewL(
            iTrans,
            iCmManagerImpl->TableId( ECmmDbBearerPriorityRecord ) );
    iListenerManager = CCmmListenerManager::NewL( this );
    iInstanceMapping = CCmmInstanceMapping::NewL( *this );

    // Create CommsDat listeners to detect changes to the database from external sources.
    RArray<TUint32> tableIdArray;
    CleanupClosePushL( tableIdArray );

    iSnapTableId = iCmManagerImpl->TableId( ECmmDbSnapRecord );
    iSnapMetadataTableId = iCmManagerImpl->TableId( ECmmDestMetadataRecord );

    // Instancemapping needs notifications on following tables.
    tableIdArray.Append( CommsDat::KCDTIdIAPRecord );
    tableIdArray.Append( CommsDat::KCDTIdVPNServiceRecord );
    tableIdArray.Append( iSnapTableId );
    tableIdArray.Append( iSnapMetadataTableId );

    // Destinations need notifications on following tables.
    tableIdArray.Append( CommsDat::KCDTIdNetworkRecord );
    tableIdArray.Append( CommsDat::KCDTIdAccessPointRecord );
    // Destination metadata table was already added.

    // Connection methods need notifications on following tables.
    TInt pluginCount( iPlugins->Count() );
    if ( pluginCount )
        {
        ( *iPlugins )[0]->GetGenericTableIdsToBeObservedL( tableIdArray );
        for( TInt i = 0; i < pluginCount; i++ )
            {
            ( *iPlugins )[i]->GetBearerTableIdsToBeObservedL( tableIdArray );
            }
        }

    for ( TInt i = 0; i < tableIdArray.Count(); i++ )
        {
        iListenerManager->AddListenerL( tableIdArray[i] );
        }
    CleanupStack::PopAndDestroy( &tableIdArray );

    OstTraceFunctionExit0( CCMMCACHE_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------
// Opens a destination with given ID (if not already opened), then copies the
// relevant data to the session instance. Checks that destination ID is valid.
// ---------------------------------------------------------------------------
//
void CCmmCache::OpenDestinationL(
        CCmmDestinationInstance& aDestinationInstance,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_OPENDESTINATIONL_ENTRY );

    if ( !iInstanceMapping->ValidDestinationId( aDestinationId ) )
        {
        User::Leave( KErrNotFound );
        }

    TInt index = FindDestinationFromCache( aDestinationId );
    if ( index == KErrNotFound )
        {
        // Cache does not have a handle open to this destination. A new handle
        // needs to be created first.
        CCmmDestinationStruct* destination = CCmmDestinationStruct::NewLC(
                this,
                iTrans,
                aDestinationId );

        // Now that cache has a handle on this destination, copy the relevant
        // data to the session instance.
        destination->CreateDestinationInstanceL( aDestinationInstance );

        iDestinationArray.AppendL( destination );
        CleanupStack::Pop( destination );
        }
    else
        {
        // Cache already has a handle on this destination. Copy the relevant
        // data to the session instance.
        iDestinationArray[index]->CreateDestinationInstanceL( aDestinationInstance );
        }

    // Add list of currently contained connection methods.
    iInstanceMapping->GetConnMethodsFromDestinationL(
            aDestinationId,
            aDestinationInstance.iConnMethodItemArray );

    OstTraceFunctionExit0( CCMMCACHE_OPENDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Refresh the data contained in aDestinationInstance. This means reloading
// the data from database if necessary. After this call the contents of
// aDestinationInstance will reflect the current state in the database. 
// ---------------------------------------------------------------------------
//
void CCmmCache::RefreshDestinationL( CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REFRESHDESTINATIONL_ENTRY );

    TInt index = FindDestinationFromCache( aDestinationInstance.GetId() );
    if ( index == KErrNotFound )
        {
        index = FindNotSavedDestinationFromCacheL( aDestinationInstance.GetDestinationNameL(), 0 );
        if ( index == KErrNotFound )
            {
            User::Leave( KErrNotFound );
            }
        }

    // If this destination is a newly created one that doesn't yet exist in
    // database, just return silently.
    if ( iDestinationArray[index]->GetStatus() == ECmmDestinationStatusNotSaved )
        {
        OstTraceFunctionExit0( CCMMCACHE_REFRESHDESTINATIONL_EXIT );
        return;
        }

    iDestinationArray[index]->RefreshDestinationInstanceL( aDestinationInstance );

    OstTraceFunctionExit0( DUP1_CCMMCACHE_REFRESHDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Create a new destination into cache (with name and ID) and copy the data
// into session side handle.
// ---------------------------------------------------------------------------
//
void CCmmCache::CreateDestinationL(
        CCmmDestinationInstance& aDestinationInstance,
        const TDesC& aName,
        const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CREATEDESTINATIONL_ENTRY );

    // Create a new destination with given name.
    CCmmDestinationStruct* destination = CCmmDestinationStruct::NewLC( this, iTrans, aName, aId );

    destination->CreateDestinationInstanceL( aDestinationInstance );

    iDestinationArray.AppendL( destination );
    CleanupStack::Pop( destination );

    OstTraceFunctionExit0( CCMMCACHE_CREATEDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Opens a connection method with given ID (if not already opened), then
// creates and passes an instance of it to the caller.
// If aDestinationInstance is not NULL, connection method is opened from
// destination.
// ---------------------------------------------------------------------------
//
void CCmmCache::OpenConnMethodL(
        CCmmConnMethodInstance& aConnMethodInstance,
        CCmmDestinationInstance* aDestinationInstance,
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_OPENCONNMETHODL_ENTRY );

    // Check connection method ID.
    TBool validAttributes( EFalse );
    if ( !aDestinationInstance )
        {
        // Check connection method exists in database.
        // Embedded destinations not included.
        validAttributes = iInstanceMapping->ValidConnMethodId( aConnMethodId );
        }
    else
        {
        // Check connection method is inside the destination.
        if ( aDestinationInstance->
                ValidConnMethodIdInDestinationIncludeEmbedded( aConnMethodId ) )
            {
            // Check connection method (can be embedded destination too)
            // exists in database.
            if ( iInstanceMapping->ValidConnMethodId( aConnMethodId ) ||
                    iInstanceMapping->ValidDestinationId( aConnMethodId ) )
                {
                validAttributes = ETrue;
                }
            }
       }
    if ( !validAttributes )
        {
        User::Leave( KErrNotFound );
        }

    // Create the connection method instance.

    // Check if connection method is already opened in cache.
    TInt index = FindConnMethodFromCache( aConnMethodId );
    if ( index != KErrNotFound )
        {
        // Update data from commsdat if necessary.
        if ( iConnMethodArray[index]->GetRecordStatus() == ECmmRecordStatusExpired )
            {
            iConnMethodArray[index]->ReloadPluginDataIfNeededL();
            // CopyDataL() will set the internal state of aConnMethodInstance.
            }

        // Already open in cache. Copy the connection method data to session
        // instance.
        // Will increase reference counter.
        aConnMethodInstance.CopyDataL( iConnMethodArray[index] );
        }
    else
        {
        // Not yet open in cache, open now.
        OpenConnectionMethodInstanceL( aConnMethodInstance, aConnMethodId );
        }

    OstTraceFunctionExit0( CCMMCACHE_OPENCONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Refresh the data contained in aConnMethodInstance. This means reloading the
// data from database if necessary. After this call the contents of
// aConnMethodInstance will reflect the current state in the database. 
// ---------------------------------------------------------------------------
//
void CCmmCache::RefreshConnMethodL( CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REFRESHCONNMETHODL_ENTRY );

    // If embedded destination --> refresh through destination API.
    if ( aConnMethodInstance.IsEmbeddedDestination() )
        {
        return;
        }

    TInt index = FindConnMethodFromCache( aConnMethodInstance.GetId() );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }

    // If this connection method is a newly created one that doesn't yet exist
    // in database, just return silently.
    if ( iConnMethodArray[index]->GetStatus() == ECmmConnMethodStatusNotSaved )
        {
        OstTraceFunctionExit0( CCMMCACHE_REFRESHCONNMETHODL_EXIT );
        return;
        }

    iConnMethodArray[index]->ReloadPluginDataIfNeededL();
    if ( iConnMethodArray[index]->GetStatus() == ECmmConnMethodStatusValid
            || iConnMethodArray[index]->GetStatus() == ECmmConnMethodStatusToBeDeleted )
        {
        iConnMethodArray[index]->GetPlugin()->GetPluginDataL( 
                aConnMethodInstance.GetPluginDataInstance() );
        }
    // Internal state need to be set to the same state as after a successfull update.
    aConnMethodInstance.UpdateSuccessful();

    OstTraceFunctionExit0( DUP1_CCMMCACHE_REFRESHCONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Creates a new connection method (not embedded destinations) into database
// with the given bearer type and optionally ID, then creates and passes an
// instance of it to the caller.
// If destination instance is provided (not NULL), connection method is
// created in that destination.
// If connection method ID is provided, it's availability is verified.
// ---------------------------------------------------------------------------
//
void CCmmCache::CreateConnMethodL(
        CCmmConnMethodInstance& aConnMethodInstance,
        CCmmDestinationInstance* aDestinationInstance,
        const TUint32 aBearerType,
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CREATECONNMETHODL_ENTRY );

    // Check that the bearer type is not embedded destination.
    if ( aBearerType == KUidEmbeddedDestination )
        {
        User::Leave( KErrArgument );
        }

    TUint32 connMethodId( aConnMethodId );
    if ( aConnMethodId )
        {
        // Check if a connection method with given ID exists (or is already
        // created but not saved).
        if ( iInstanceMapping->ValidConnMethodId( aConnMethodId ) ||
                ConnMethodOpenWithId( aConnMethodId ) )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    else
        {
        // Use a temporary ID until a real one is received from database.
        connMethodId = NextFreeTemporaryId();
        }


    // Check bearer type support and create plugin instance.
    CCmPluginBaseEng* plugin = NULL;
    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == aBearerType )
            {
            TCmPluginInitParam pluginParams( Session() );
            plugin = ( *iPlugins )[i]->CreateInstanceL( pluginParams );
            CleanupStack::PushL( plugin );
            plugin->CreateNewL( aConnMethodId ); // Use aConnMethodId here, so ID is either a real ID or 0.
            break;
            }
        }
    if ( !plugin )
        {
        TInt index = RefreshPluginL( aBearerType );
        if ( index < 0 || index >= iPlugins->Count() )
            {
            User::Leave( KErrArgument );
            }
        else
            {
            TCmPluginInitParam pluginParams( Session() );
            plugin = ( *iPlugins )[index]->CreateInstanceL( pluginParams );
            CleanupStack::PushL( plugin );
            plugin->CreateNewL( aConnMethodId );
            }
        }


    // Store the connection method into cache.
    // Use connMethodId here, so ID is either a real ID or a temporary ID.
    CCmmConnMethodStruct* connMethodStruct = CCmmConnMethodStruct::NewL( connMethodId );
    connMethodStruct->SetPlugin( plugin, aBearerType, ECmmConnMethodStatusNotSaved );
    CleanupStack::Pop( plugin );
    CleanupStack::PushL( connMethodStruct );
    iConnMethodArray.AppendL( connMethodStruct );
    CleanupStack::Pop( connMethodStruct );

    // Copy the connection method data to session instance.
    aConnMethodInstance.CopyDataL( connMethodStruct ); // Will increase reference counter.


    if ( aDestinationInstance )
        {
        // Add connection method to destination.
        aDestinationInstance->AddConnMethodL( aConnMethodInstance );
        }

    OstTraceFunctionExit0( CCMMCACHE_CREATECONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Creates a new connection method into cache as a copy of an existing
// connection method (exists in cache, not necessarily in database), and opens
// a client side handle to it. The new connection method will get a new ID when
// updated to database.
// ---------------------------------------------------------------------------
//
void CCmmCache::CreateCopyOfConnMethodL(
        CCmmConnMethodInstance& aNewConnMethodInstance,
        CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CREATECOPYOFCONNMETHODL_ENTRY );

    // Check bearer type support and create plugin instance.
    TInt index = FindConnMethodFromCache( aConnMethodInstance.GetId() );
    if ( index == KErrNotFound )
        {
        User::Leave( index );
        }

    CCmPluginBaseEng* plugin = iConnMethodArray[index]->GetPlugin();
    if ( !plugin )
        {
        User::Leave( KErrNotFound );
        }

    CCmPluginBaseEng* pluginCopy =
            plugin->CreateCopyL( aConnMethodInstance.GetPluginDataInstance() );
    CleanupStack::PushL( pluginCopy );

    // Store the connection method into cache.
    CCmmConnMethodStruct* connMethodStruct =
            CCmmConnMethodStruct::NewL( NextFreeTemporaryId() );
    connMethodStruct->SetPlugin(
            pluginCopy,
            aConnMethodInstance.GetBearerType(),
            ECmmConnMethodStatusNotSaved );
    CleanupStack::Pop( pluginCopy );
    CleanupStack::PushL( connMethodStruct );
    iConnMethodArray.AppendL( connMethodStruct );
    CleanupStack::Pop( connMethodStruct );

    // Copy the connection method data to session instance.
    aNewConnMethodInstance.CopyDataL( connMethodStruct ); // Will increase reference counter.

    OstTraceFunctionExit0( CCMMCACHE_CREATECOPYOFCONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Reloads a destination record if needed and copies the latest version to
// the session instance given as parameter.
// ---------------------------------------------------------------------------
//
void CCmmCache::LoadDestinationRecordL(
        CCmmDestinationInstance& aDestinationInstance,
        TCmmDbRecords aDestRecordType )
    {
    OstTraceFunctionEntry0( CCMMCACHE_LOADDESTINATIONRECORDL_ENTRY );

    TUint32 id = aDestinationInstance.GetId();
    // If ID is not in the valid range, it means the destination is a newly
    // created one, and doesn't yet exist in database. Thus, record data exists
    // only in session side and can't be loaded from cache. So this is an error.
    if ( id >= KTemporaryIdCounterStart )
        {
        User::Leave( KErrCorrupt );
        }
    TInt index = FindDestinationFromCache( id );
    if ( index < 0 )
        {
        User::Leave( index );
        }

    iDestinationArray[index]->LoadRecordL( aDestinationInstance, aDestRecordType );

    OstTraceFunctionExit0( CCMMCACHE_LOADDESTINATIONRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Saves the modifications in aDestinationInstance into the database. Also all
// connection methods inside this destination are updated (including any
// embedded destination).
// ---------------------------------------------------------------------------
//
void CCmmCache::UpdateDestinationL( CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_UPDATEDESTINATIONL_ENTRY );

    // Arrays to temporarily store information of updated destinations and
    // connection methods. Used to update status and ID information after
    // successful commit to database.
    if ( iTrans->GetReferenceCount() == 0 )
        {
        iDeletedConnMethods.Reset();
        iUpdatedConnMethods.Reset();
        iUpdatedConnMethods2.Reset();
        iUpdatedDestinations.Reset();
        iUpdatedDestinations2.Reset();
        }

    TInt index = FindDestinationFromCache( aDestinationInstance.GetId() );
    if ( index < 0 )
        {
        User::Leave( index );
        }

    iTrans->OpenTransactionLC();

    // Check that the connection methods marked for deletion can be deleted.
    for ( TInt i = 0; i < aDestinationInstance.iConnMethodsToBeDeleted.Count(); i++ )
        {
        TUint32 id = aDestinationInstance.iConnMethodsToBeDeleted[i];

        // Remove connection method from delete list if ID is not valid or if
        // referenced from any other destination (in database or in any other
        // client-side destination handles).
        if ( !iInstanceMapping->ValidConnMethodId( id ) ||
                iInstanceMapping->ConnMethodInOtherDestination(
                        id,
                        aDestinationInstance.GetId() ) ||
                aDestinationInstance.ConnMethodInOtherDestinationInSession( id, 0 ) )
            {
            aDestinationInstance.iConnMethodsToBeDeleted.Remove( i );
            i--; // Adjust array index counter.
            }
        else if ( iInstanceMapping->ConnMethodPointedToByVirtualIap( id ) )
            {
            aDestinationInstance.iConnMethodsToBeDeleted.Remove( i );
            User::Leave( KErrLocked );
            }
        else if ( CheckIfCmConnected( id ) )
            {
            User::Leave( KErrInUse );
            }
        }
    // Delete connection methods marked for deletion.
    for ( TInt i = 0; i < aDestinationInstance.iConnMethodsToBeDeleted.Count(); i++ )
        {
        DeleteConnMethodAsPartOfDestinationUpdateL(
                aDestinationInstance.iConnMethodsToBeDeleted[i] );
        }

    // Update the connection methods inside this destination.
    for ( TInt i = 0; i < aDestinationInstance.iConnMethodItemArray.Count(); i++ )
        {
        if ( aDestinationInstance.iConnMethodItemArray[i].IsEmbedded() )
            {
            // Embedded destination.

            //TODO, Maybe check other restrictions on embedded destination.
            // - Only one embedded destination per destination. Check.
            // - No embedded destinations in embedded destination. Check.

            TUint32 id( aDestinationInstance.iConnMethodItemArray[i].iId );
            CCmmDestinationInstance* destinationInstance =
                    aDestinationInstance.FindDestinationInstanceFromSessionById( id );

            if ( destinationInstance )
                {
                // Client has a handle open to this destination, update it.
                destinationInstance->UpdateL();
                }
            else
                {
                // TODO: Or, should we update the embedded destination if the
                // client has an open handle to any of the embedded
                // destinations connection methods.
                //
                // Skip update since client doesn't have an open handle to this
                // destination.
                if ( id >= KTemporaryIdCounterStart ||
                        !iInstanceMapping->ValidDestinationId( id ) )
                    {
                    // Remove destination item from array if:
                    // - New destination, but client has already closed the handle for it.
                    // - Destination ID was valid before, but it does not exist anymore.
                    aDestinationInstance.iConnMethodItemArray.Remove( i );
                    i--; // Adjust array index counter.
                    }
                }
            }
        else
            {
            TBool temporaryConnMethodInstance( EFalse );
            TBool connMethodProtectionMustBeSet( EFalse );
            TBool cmProtected( EFalse );

            TUint32 id( aDestinationInstance.iConnMethodItemArray[i].iId );
            CCmmConnMethodInstance* connMethodInstance =
                    aDestinationInstance.FindConnMethodInstanceFromSessionById( id );

            if ( !connMethodInstance )
                {
                // Remove connection method item from the array if it is a new
                // connection method but client has already closed the handle
                // for it, or the connection method ID was valid before but
                // does not exist anymore.
                if ( id >= KTemporaryIdCounterStart || !iInstanceMapping->ValidConnMethodId( id ) )
                    {
                    aDestinationInstance.iConnMethodItemArray.Remove( i );
                    i--; // Adjust array index counter.
                    continue; // Jump to next connection method.
                    }
                }

            if ( aDestinationInstance.ProtectionChanged() )
                {
                // Check if the connection method protection level needs to be set.
                switch ( aDestinationInstance.CurrentProtectionLevelL() )
                    {
                    case CMManager::EProtLevel0:
                    case CMManager::EProtLevel2:
                        {
                        if ( aDestinationInstance.LastProtectionLevel() ==
                                CMManager::EProtLevel1 ||
                                aDestinationInstance.LastProtectionLevel() ==
                                CMManager::EProtLevel3 )
                            {
                            connMethodProtectionMustBeSet = ETrue;
                            cmProtected = EFalse;
                            }
                        }
                        break;
                    case CMManager::EProtLevel1:
                    case CMManager::EProtLevel3:
                        {
                        connMethodProtectionMustBeSet = ETrue;
                        cmProtected = ETrue;
                        }
                        break;
                    default:
                        break;
                    }
                }

            if ( connMethodProtectionMustBeSet && !connMethodInstance )
                {
                // Client doesn't have an open handle to this connection method,
                // but it still needs to be updated because the destination's
                // protection level has been changed in such a way that
                // requires a change in all contained connection methods also.
                temporaryConnMethodInstance = ETrue;
                connMethodInstance = CCmmConnMethodInstance::NewLC( NULL, this );
                OpenConnMethodL( *connMethodInstance, NULL, id );

                // Read current protection level.
                TBool current = connMethodInstance->GetBoolAttributeL( CMManager::ECmProtected );
                if ( cmProtected == current )
                    {
                    // If the connection method already has the correct
                    // protection setting, skip the unnecessary update.
                    connMethodProtectionMustBeSet = EFalse;
                    temporaryConnMethodInstance = EFalse;
                    CleanupStack::PopAndDestroy( connMethodInstance );
                    connMethodInstance = NULL;
                    }
                }

            // Update the connection method.
            if ( connMethodInstance )
                {
                if ( connMethodProtectionMustBeSet )
                    {
                    connMethodInstance->SetBoolAttributeL( CMManager::ECmProtected, cmProtected );
                    }
                connMethodInstance->UpdateL( temporaryConnMethodInstance );
                }

            // Cleanup connection method handle, if it was temporary.
            if ( temporaryConnMethodInstance )
                {
                CleanupStack::PopAndDestroy( connMethodInstance );
                }
            connMethodInstance = NULL;
            }
        }
    // All connection methods requiring update in the destination should have
    // been updated to database now.

    iDestinationArray[index]->UpdateL( aDestinationInstance, this );
    iTrans->CommitTransactionL();

    TCmmIdStruct idStruct( iDestinationArray[index]->GetRealId(), 0 );
    if ( aDestinationInstance.GetId() >= KTemporaryIdCounterStart )
        {
        idStruct.iTemporaryId = aDestinationInstance.GetId();
        }
    iUpdatedDestinations.AppendL( idStruct );
    iUpdatedDestinations2.AppendL( &aDestinationInstance );

    // Check transaction handler reference count.
    if ( iTrans->GetReferenceCount() == 0 )
        {
        // Successful commit to database. Refresh instance mapping and all
        // necessary status information.
        for ( TInt i = 0; i < iUpdatedConnMethods.Count(); i++ )
            {
            aDestinationInstance.RefreshHandlesForAllSessions( iUpdatedConnMethods[i] );
            }
        for ( TInt i = 0; i < iUpdatedConnMethods2.Count(); i++ )
            {
            iUpdatedConnMethods2[i]->UpdateSuccessful();

            TInt cacheIndex = FindConnMethodFromCache( iUpdatedConnMethods2[i]->GetId() );
            if ( cacheIndex >= 0 )
                {
                iConnMethodArray[cacheIndex]->UpdateSuccessful();
                }
            }
        for ( TInt i = 0; i < iDeletedConnMethods.Count(); i++ )
            {
            TInt cacheIndex = FindConnMethodFromCache( iDeletedConnMethods[i].iRealId );
            if ( cacheIndex >= 0 )
                {
                aDestinationInstance.RefreshHandlesForAllSessions( iDeletedConnMethods[i] );
                iConnMethodArray[cacheIndex]->SetStatus( ECmmConnMethodStatusToBeDeleted );
                iInstanceMapping->AddConnMethodToDeletedListL( iDeletedConnMethods[i].iRealId );
                }
            }
        for ( TInt i = 0; i < iUpdatedDestinations.Count(); i++ )
            {
            aDestinationInstance.RefreshHandlesForAllSessions( iUpdatedDestinations[i] );
            }
        for ( TInt i = 0; i < iUpdatedDestinations2.Count(); i++ )
            {
            iUpdatedDestinations2[i]->UpdateSuccessful();

            TInt cacheIndex = FindDestinationFromCache( iUpdatedDestinations2[i]->GetId() );
            if ( cacheIndex >= 0 )
                {
                iDestinationArray[cacheIndex]->UpdateSuccessful();
                }
            }
        iDeletedConnMethods.Reset();
        iUpdatedConnMethods.Reset();
        iUpdatedConnMethods2.Reset();
        iUpdatedDestinations.Reset();
        iUpdatedDestinations2.Reset();

        iInstanceMapping->RefreshL();
        }

    OstTraceFunctionExit0( CCMMCACHE_UPDATEDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Saves the modifications in aConnMethodInstance into database.
// The second attribute aTemporaryHandle must be true if the connection method
// instance is only a temporary one created for the duration of this update
// process only.
// ---------------------------------------------------------------------------
//
void CCmmCache::UpdateConnMethodL(
        CCmmConnMethodInstance& aConnMethodInstance,
        TBool aTemporaryHandle )
    {
    OstTraceFunctionEntry0( CCMMCACHE_UPDATECONNMETHODL_ENTRY );

    // Embedded destinatios cannot be updated as connection methods.
    if ( aConnMethodInstance.IsEmbeddedDestination() )
        {
        User::Leave( KErrNotSupported );
        }

    // Find connection method from cache.
    TUint32 connMethodId( aConnMethodInstance.GetId() );
    TInt index = FindConnMethodFromCache( connMethodId );
    if ( index < 0 )
        {
        // Should never end up here.
        User::Leave( index );
        }

    iTrans->OpenTransactionLC();
    iConnMethodArray[index]->GetPlugin()->UpdateL( aConnMethodInstance.GetPluginDataInstance() );
    iTrans->CommitTransactionL();

    TCmmIdStruct idStruct( aConnMethodInstance.GetPluginDataInstance()->iIapId, 0 );
    if ( connMethodId >= KTemporaryIdCounterStart )
        {
        idStruct.iTemporaryId = connMethodId;
        }

    if ( iTrans->GetReferenceCount() == 0 )
        {
        // Writing to database is completed, refresh instance mapping and all
        // necessary status information.
        iInstanceMapping->RefreshL();
        aConnMethodInstance.RefreshHandlesForAllSessions( idStruct );
        aConnMethodInstance.UpdateSuccessful();
        iConnMethodArray[index]->UpdateSuccessful();
        }
    else
        {
        // Update request came from destination update. Add connection method
        // information to temporary arrays so it's status information can be
        // updated after successful commit to database.
        iUpdatedConnMethods.AppendL( idStruct );
        if ( !aTemporaryHandle )
            {
            // A temporary handle will be destroyed after update, and so won't
            // need any status updates after commit.
            iUpdatedConnMethods2.AppendL( &aConnMethodInstance );
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_UPDATECONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Deletes the destination refered by aDestinationInstance from the database.
// In case other sessions have open handles to the same destination, the status
// is updated to deleted state, but it is not yet removed from database.
// (ID must be kept reserved in commsdat until all handles are closed). The
// same is done for the connection methods inside this destination.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeleteDestinationL(
        CCmmDestinationInstance& aDestinationInstance,
        TBool aForced ) //TODO, comment on aForced param?
    {
    OstTraceFunctionEntry0( CCMMCACHE_DELETEDESTINATIONL_ENTRY );

    TUint32 destinationId( aDestinationInstance.GetId() );

    // Array to temporarily store deleted connection method IDs. Status
    // information for these connection methods is updated after successful
    // commit to database.
    iDeletedConnMethods.Reset();

    // Find destination from cache.
    TInt index = FindDestinationFromCache( destinationId );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrBadHandle );
        }

    // If not forced, check if the destination is already marked to be deleted.
    if ( !aForced && iDestinationArray[index]->GetStatus() == ECmmDestinationStatusToBeDeleted )
        {
        OstTraceFunctionExit0( CCMMCACHE_DELETEDESTINATIONL_EXIT );
        return;
        }

    // Check if there are any additional handles open. If yes, can't delete
    // detination from database just yet.
    if ( iDestinationArray[index]->GetReferenceCount() <= 1 )
        {
        // No other client handles open to this destination.

        // Get the connection methods in this destination, and then go through
        // them removing those that can't be deleted.
        RArray<TCmmConnMethodItem> connMethodArray;
        CleanupClosePushL( connMethodArray );
        if ( !aForced )
            {
            iInstanceMapping->GetConnMethodsFromDestinationL( destinationId, connMethodArray );
            }
        else
            {
            for ( TInt i = 0; i < aDestinationInstance.iConnMethodItemArray.Count() ; i++ )
                {
                connMethodArray.AppendL( aDestinationInstance.iConnMethodItemArray[i] );
                }
            }

        // Remove embedded destination from list if found.
        TInt count( connMethodArray.Count() );
        if ( count )
            {
            if ( connMethodArray[count - 1].IsEmbedded() )
                {
                connMethodArray.Remove( count - 1 );
                }
            }
        // Remove any connection method that belongs to any other destination.
        for ( TInt i = 0; i < connMethodArray.Count(); i++ )
            {
            if ( iInstanceMapping->ConnMethodInOtherDestination(
                    connMethodArray[i].iId,
                    destinationId ) )
                {
                connMethodArray.Remove( i );
                i--;
                }
            }
        // Remove any connection method that has a virtual IAP pointing to it.
        for ( TInt i = 0; i < connMethodArray.Count(); i++ )
            {
            if ( iInstanceMapping->ConnMethodPointedToByVirtualIap( connMethodArray[i].iId ) )
                {
                connMethodArray.Remove( i );
                i--;
                }
            }

        iTrans->OpenTransactionLC();

        // Delete each connection method inside this destination.
        for ( TInt i = 0; i < connMethodArray.Count(); i++ )
            {
            DeleteConnMethodAsPartOfDestinationDeleteL( connMethodArray[i].iId );
            }

        // Delete the destination.
        iDestinationArray[index]->DeleteL();

        iTrans->CommitTransactionL();
        CleanupStack::PopAndDestroy( &connMethodArray );

        for ( TInt i = 0; i < iDeletedConnMethods.Count(); i++ )
            {
            TInt index = FindConnMethodFromCache( iDeletedConnMethods[i].iRealId );
            if ( index == KErrNotFound )
                {
                // No handles open to this deleted connection method, so it was
                // removed from database. Remove it from instance mapping
                // structures. Refreshing instance mapping would do the same,
                // but more slowly.
                iInstanceMapping->RemoveConnMethod( iDeletedConnMethods[i].iRealId );
                }
            else
                {
                // There is at least one handle open to this deleted connection
                // method, so it still exists in database for now. Remove it
                // from instance mapping structures and add it to the instance
                // mapping's deleted list, so the connection method is ignored
                // if refreshing structures from database. Also mark the
                // connection method handle on cache side as 'to be deleted'.
                //
                // If the connection method is updated from another existing
                // handle after this, the connection method is restored as
                // uncategorized.
                iInstanceMapping->AddConnMethodToDeletedListL( iDeletedConnMethods[i].iRealId );
                aDestinationInstance.RefreshHandlesForAllSessions( iDeletedConnMethods[i] );
                iConnMethodArray[index]->SetStatus( ECmmConnMethodStatusToBeDeleted );
                }
            }
        iInstanceMapping->RemoveDestination( destinationId );
        }
    else
        {
        // There are additional client handles open to this destination. Mark
        // the destination as 'to be deleted'. When the other handles are
        // closed and reference count goes to 0, the destination will be
        // removed from database.
        // If the destination is updated from another existing handle after
        // this, the destination is restored to normal.
        iDestinationArray[index]->SetStatus( ECmmDestinationStatusToBeDeleted );
        iInstanceMapping->AddDestinationToDeletedListL( destinationId );
        }

    // Update status for ALL related destination handles on client side to
    // ECmmDestinationStatusChanged.
    TCmmIdStruct idStruct( destinationId, 0 );
    aDestinationInstance.RefreshHandlesForAllSessions( idStruct );

    iDeletedConnMethods.Reset();
    OstTraceFunctionExit0( DUP1_CCMMCACHE_DELETEDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Deletes a connection method as part of destination delete operation. It is
// already checked that the connection method given as parameter can be deleted
// and a transaction is already open.
// If there are client side handles open to the connection method, the
// connection method is marked as deleted, but the actual database removal will
// be done after the last handle is closed. Updating a connection method marked
// as deleted (through an already open handle) will restore it.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeleteConnMethodAsPartOfDestinationDeleteL( const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONDELETEL_ENTRY );

    // Find connection method from cache side.
    TInt index = FindConnMethodFromCache( aConnMethodId );
    if ( index == KErrNotFound )
        {
        // There is no open handles to this connection method. Open a temporary
        // handle and delete it.

        // Find out the connection method bearer type.
        TUint32 bearerType( 0 );
        TInt err = iInstanceMapping->GetConnMethodBearerType( aConnMethodId, bearerType );
        if ( err || bearerType == KUidEmbeddedDestination )
            {
            // If this is an embedded destination, or the bearer is not
            // supported, skip. The connection method is simply removed from
            // destination.
            OstTraceFunctionExit0( CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONDELETEL_EXIT );
            return;
            }

        // Check bearer type support, create plugin instance and delete the
        // connection method.
        CCmPluginBaseEng* plugin = NULL;
        for ( TInt i = 0; i < iPlugins->Count(); i++ )
            {
            if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == bearerType )
                {
                TCmPluginInitParam pluginParams( Session() );
                plugin = ( *iPlugins )[i]->CreateInstanceL( pluginParams );
                CleanupStack::PushL( plugin );

                // Transaction is already open.
                plugin->LoadL( aConnMethodId );
                plugin->DeleteL();

                CleanupStack::Pop( plugin );
                break;
                }
            }
        if ( !plugin )
            {
            // Skip, if bearer is unsupported. Connection method is simply
            // removed from destination.
            OstTraceFunctionExit0( DUP1_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONDELETEL_EXIT );
            return;
            }
        delete plugin;
        plugin = NULL;
        }
    else
        {
        // If the connection method is already open in cache side, we can't
        // delete it from database just yet. It will only be marked as 'to be
        // deleted' after a successful commit. When the other handles are
        // closed and reference count goes to 0, the connection method will be
        // removed from database.

        // Check destination status in cache.
        switch ( iConnMethodArray[index]->GetStatus() )
            {
            case ECmmConnMethodStatusNotSaved:
                {
                // The connection methods that are deleted with the destination
                // are retrieved through instance mapping, so there can't be
                // any newly created unsaved connection methods among them.
                User::Leave( KErrCorrupt );
                }
                break;
            case ECmmConnMethodStatusValid:
                // Proceed.
                break;
            case ECmmConnMethodStatusToBeDeleted:
                // Connection method has already been deleted.
                OstTraceFunctionExit0( DUP2_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONDELETEL_EXIT );
                return;
            case ECmmConnMethodStatusChanged:
            default:
                {
                ASSERT( 0 ); // Error, illegal status.
                User::Leave( KErrCorrupt );
                }
                break;
            }
        }

    // Add connection method ID to temporary array so it's status information
    // can be updated after successful commit to database.
    TCmmIdStruct idStruct( aConnMethodId, 0 );
    iDeletedConnMethods.AppendL( idStruct );

    OstTraceFunctionExit0( DUP3_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONDELETEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Deletes a connection method as part of destination update operation. It is
// already checked that the connection method given as parameter can be deleted
// and a transaction is already open.
// If there are client side handles open to the connection method, the
// connection method is marked as deleted, but the actual database removal will
// be done after the last handle is closed. Updating a connection method marked
// as deleted (through an already open handle) will restore it.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeleteConnMethodAsPartOfDestinationUpdateL(
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONUPDATEL_ENTRY );

    // Find connection method from cache side. If not found, open a temporary handle.
    TInt index = FindConnMethodFromCache( aConnMethodId );
    if ( index == KErrNotFound )
        {
        // There are no open handles to this connection method. Open a
        // temporary handle and delete it.

        // Find out the connection method bearer type.
        TUint32 bearerType( 0 );
        TInt err = iInstanceMapping->GetConnMethodBearerType( aConnMethodId, bearerType );
        if ( err || bearerType == KUidEmbeddedDestination )
            {
            // If this is an embedded destination, or the bearer is not
            // supported, skip. The connection method is simply removed from
            // destination.
            OstTraceFunctionExit0( CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONUPDATEL_EXIT );
            return;
            }

        // Check bearer type support, create plugin instance and delete the connection method.
        CCmPluginBaseEng* plugin = NULL;
        for ( TInt i = 0; i < iPlugins->Count(); i++ )
            {
            if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == bearerType )
                {
                TCmPluginInitParam pluginParams( Session() );
                plugin = ( *iPlugins )[i]->CreateInstanceL( pluginParams );
                CleanupStack::PushL( plugin );
                iTrans->OpenTransactionLC();

                plugin->LoadL( aConnMethodId );
                plugin->DeleteL();

                iTrans->CommitTransactionL();
                CleanupStack::Pop( plugin );
                break;
                }
            }
        if ( !plugin )
            {
            // Skip, if bearer is unsupported. Connection method is simply
            // removed from destination.
            OstTraceFunctionExit0( DUP1_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONUPDATEL_EXIT );
            return;
            }
        delete plugin;
        plugin = NULL;

        // Destination update will refresh instance mapping anyway at the end,
        // so no need to add this connection method to the temporary
        // iDeletedConnMethods-array here.
        }
    else
        {
        // There is one or more open handles to this connection method. We
        // can't delete it from database just yet. It will only be marked as
        // 'to be deleted' after a successful commit. When the other handles
        // are closed and reference count goes to 0, the connection method will
        // be removed from database.

        // Check connection method status.
        switch ( iConnMethodArray[index]->GetStatus() )
            {
            case ECmmConnMethodStatusNotSaved:
                {
                // Destination update will not attempt to delete any non-valid
                // connection methods.
                User::Leave( KErrCorrupt );
                }
                break;
            case ECmmConnMethodStatusValid:
                // Proceed.
                break;
            case ECmmConnMethodStatusToBeDeleted:
                // Connection method has already been deleted.
                OstTraceFunctionExit0( DUP2_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONUPDATEL_EXIT );
                return;
            case ECmmConnMethodStatusChanged:
            default:
                {
                ASSERT( 0 ); // Error, illegal status.
                User::Leave( KErrCorrupt );
                }
                break;
            }
        // Add connection method ID to temporary array so it's status
        // information can be updated after successful commit to database.
        TCmmIdStruct idStruct( aConnMethodId, 0 );
        iDeletedConnMethods.AppendL( idStruct );
        }

    OstTraceFunctionExit0( DUP3_CCMMCACHE_DELETECONNMETHODASPARTOFDESTINATIONUPDATEL_EXIT );
    }

// ---------------------------------------------------------------------------
// Basic connection method delete. Removes a connection method from any
// destination it might belong to, and then deletes it.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeleteConnMethodL( CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( DUP1_CCMMCACHE_DELETECONNMETHODL_ENTRY );

    TUint32 cmId = aConnMethodInstance.GetId();

    // Find connection method from cache.
    TInt index = FindConnMethodFromCache( cmId );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrBadHandle );
        }

    // Check if the connection method is already deleted.
    if ( iConnMethodArray[index]->GetStatus() == ECmmConnMethodStatusToBeDeleted )
        {
        OstTraceFunctionExit0( DUP1_CCMMCACHE_DELETECONNMETHODL_EXIT );
        return;
        }

    // Check if there are any additional handles open. If yes, can't delete
    // connection method from database just yet.
    if ( iConnMethodArray[index]->GetReferenceCounter() <= 1 )
        {
        // No other client handles open to this connection method.

        // Remove connection method from any destination in database and then delete it.
        iTrans->OpenTransactionLC();
        RemoveAllReferencesToConnMethodL( aConnMethodInstance );
        iConnMethodArray[index]->GetPlugin()->DeleteL();
        iTrans->CommitTransactionL();

        // Update instance mapping to reflect the current database state, and
        // notify any possible client handles for the changed destinations.
        RArray<TUint32> changedDestinations;
        iInstanceMapping->RemoveConnMethod( cmId, changedDestinations );
        for ( TInt i = 0; i < changedDestinations.Count(); i++ )
            {
            TCmmIdStruct idStruct( changedDestinations[i], 0 );
            aConnMethodInstance.RefreshHandlesForAllSessions( idStruct );
            }
        changedDestinations.Close();

        // Set status for cache and client handles.
        TUint32 newSecondaryId( NextFreeTemporaryId() );
        aConnMethodInstance.DeleteSuccessful( newSecondaryId );
        iConnMethodArray[index]->DeleteSuccessful( newSecondaryId );
        }
    else
        {
        // There are additional client handles open to this connection method.
        // Mark the connection method as 'to be deleted' and remove it from any
        // destinations in database. When the other handles are closed and
        // reference count goes to 0, the connection method will be removed
        // from database.
        RemoveAllReferencesToConnMethodL( aConnMethodInstance );
        iInstanceMapping->AddConnMethodToDeletedListL( cmId );
        TCmmIdStruct idStruct( cmId, 0 );
        aConnMethodInstance.RefreshHandlesForAllSessions( idStruct );
        iConnMethodArray[index]->SetStatus( ECmmConnMethodStatusToBeDeleted );
        }

    OstTraceFunctionExit0( DUP2_CCMMCACHE_DELETECONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given ID belongs to a valid existing destination. Attribute
// aId needs to be in the current valid range (0x1001 - 0x10FF atm).
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationExistsWithId( const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONEXISTSWITHID_ENTRY );

    TBool exists = iInstanceMapping->ValidDestinationId( aId );

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONEXISTSWITHID_EXIT );
    return exists;
    }

// ---------------------------------------------------------------------------
// Check if the given name belongs to an existing destination.
// If a destination ID is given, that destination is skipped.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationExistsWithNameL( const TDesC& aName, const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONEXISTSWITHNAMEL_ENTRY );

    TBool found( EFalse );

    // Check AccessPoint-table.
    CommsDat::CCDAccessPointRecord* destApRecord = static_cast<CommsDat::CCDAccessPointRecord*>(
            CommsDat::CCDRecordBase::RecordFactoryL( CommsDat::KCDTIdAccessPointRecord ) );
    CleanupStack::PushL( destApRecord );
    destApRecord->iRecordName.SetL( aName );
    if ( destApRecord->FindL( iTrans->Session() ) ) // Names should be unique.
        {
        if ( aDestinationId )
            {
            // Check the ID is different.
            destApRecord->LoadL( iTrans->Session() );
            if ( aDestinationId != destApRecord->iRecordTag )
                {
                found = ETrue;
                }
            }
        else
            {
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( destApRecord );

    // Check also DataMobilitySelectionPolicy-table.
    if ( !found )
        {
        CCDDataMobilitySelectionPolicyRecord* snapRecord =
                new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                        iCmManagerImpl->TableId( ECmmDbSnapRecord ) );
        CleanupStack::PushL( snapRecord );
        snapRecord->iRecordName.SetL( aName );
        if ( snapRecord->FindL( iTrans->Session() ) ) // Names should be unique.
            {
            if ( aDestinationId )
                {
                // Check the ID is different.
                snapRecord->LoadL( iTrans->Session() );
                if ( aDestinationId != snapRecord->iSNAP )
                    {
                    found = ETrue;
                    }
                }
            else
                {
                found = ETrue;
                }
            }
        CleanupStack::PopAndDestroy( snapRecord );
        }

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONEXISTSWITHNAMEL_EXIT );
    return found;
    }

// ---------------------------------------------------------------------------
// Check if the given ID belongs to a destination that the cache has an open
// handle on. This will include any destinations created by any client, that
// are not yet saved to database.
// With this check, we can prevent two clients from creating a new destination
// with the same ID (The UpdateL() operation would fail for one of them anyway).
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationOpenWithId( const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONOPENWITHID_ENTRY );

    TBool result( EFalse );
    TInt index = FindDestinationFromCache( aId );
    if ( index != KErrNotFound )
        {
        result = ETrue;
        }

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONOPENWITHID_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Check if the given name belongs to any new unsaved destinations in the
// cache. This includes any destinations created byany client, that are not
// yet saved to database.
// If a destination ID is provided, the search will exclude the relevant
// destination.
// With this check, we can prevent two clients from creating a new destination
// with the same name (The UpdateL() operation would fail for one of them
// anyway).
// ---------------------------------------------------------------------------
//
TBool CCmmCache::NotSavedDestinationOpenWithNameL(
        const TDesC& aName,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_NOTSAVEDDESTINATIONOPENWITHNAMEL_ENTRY );

    TBool res( EFalse );
    TInt index = FindNotSavedDestinationFromCacheL( aName, aDestinationId );
    if ( index != KErrNotFound )
        {
        res = ETrue;
        }

    OstTraceFunctionExit0( CCMMCACHE_NOTSAVEDDESTINATIONOPENWITHNAMEL_EXIT );
    return res;
    }

// ---------------------------------------------------------------------------
// Check if the given ID belongs to a valid existing connection method.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::ConnMethodExistsWithId( const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODEXISTSWITHID_ENTRY );

    TBool exists = iInstanceMapping->ValidConnMethodId( aConnMethodId );

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODEXISTSWITHID_EXIT );
    return exists;
    }

// ---------------------------------------------------------------------------
// Check if the given ID belongs to a connection method that the cache has an
// open handle on. This will include any connection methods created by any
// client, that are not yet saved to database.
// With this check, we can prevent two clients from creating a new connection
// method with the same ID (The UpdateL() operation would fail for one of them
// anyway).
// ---------------------------------------------------------------------------
//
TBool CCmmCache::ConnMethodOpenWithId( const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODOPENWITHID_ENTRY );

    TBool result( EFalse );
    TInt index = FindConnMethodFromCache( aConnMethodId );
    if ( index != KErrNotFound )
        {
        result = ETrue;
        }

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODOPENWITHID_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Checks if there are any destinations with metadata localization
// (ESnapMetadataDestinationIsLocalised) set to aValue. Also checks the
// relevant metadata purpose (ESnapMetadataPurpose) values.
// The destination connected to parameter aDestinationInstance is skipped.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationExistsWithMetadataLocalizedL(
        CCmmDestinationInstance& aDestinationInstance,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONEXISTSWITHMETADATALOCALIZEDL_ENTRY );

    TBool result( EFalse );

    if ( aValue != CMManager::ENotLocalisedDest )
        {
        // Check there is no destination in database that already has the same
        // localization metadata value. Also checks metadata purpose.
        CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet =
                new( ELeave )CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>(
                        iCmManagerImpl->TableId( ECmmDestMetadataRecord ) );
        CleanupStack::PushL( metaSet );
        TRAP_IGNORE( metaSet->LoadL( iTrans->Session() ) );
        for ( TInt i = 0; i < metaSet->iRecords.Count(); i++ )
            {
            TInt id = ( ( CCDSNAPMetadataRecord* )metaSet->iRecords[i] )->iSNAP;
            TUint32 metadata = ( ( CCDSNAPMetadataRecord* )metaSet->iRecords[i] )->iMetadata;
            TUint32 localizationValue =
                    ( metadata & CMManager::ESnapMetadataDestinationIsLocalised ) >> 4;
            TUint32 purposeValue = ( metadata & CMManager::ESnapMetadataPurpose ) >> 8;

            if ( aDestinationInstance.GetId() != id )
                {
                if ( aValue == localizationValue )
                    {
                    result = ETrue;
                    }
                switch ( aValue )
                    {
                    case CMManager::ELocalisedDestInternet:
                        {
                        if ( purposeValue == CMManager::ESnapPurposeInternet ||
                                ( metadata & CMManager::ESnapMetadataInternet ) )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ELocalisedDestWap:
                        {
                        if ( purposeValue == CMManager::ESnapPurposeOperator )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ELocalisedDestMMS:
                        {
                        if ( purposeValue == CMManager::ESnapPurposeMMS )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ELocalisedDestIntranet:
                        {
                        if ( purposeValue == CMManager::ESnapPurposeIntranet )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( metaSet );
        }

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONEXISTSWITHMETADATALOCALIZEDL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Checks if there are any destinations with metadata purpose
// (ESnapMetadataPurpose) set to aValue. Also checks the relevant metadata
// localization (ESnapMetadataDestinationIsLocalised) values.
// The destination connected to parameter aDestinationInstance is skipped.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationExistsWithMetadataPurposeL(
        CCmmDestinationInstance& aDestinationInstance,
        const TUint32 aValue )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONEXISTSWITHMETADATAPURPOSEL_ENTRY );

    TBool result( EFalse );

    if ( aValue != CMManager::ESnapPurposeUnknown )
        {
        // Check there is no destination in database that already has the same
        // metadata purpose value. Also checks localization metadata.
        CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet =
                new( ELeave )CommsDat::CMDBRecordSet<CCDSNAPMetadataRecord>(
                        iCmManagerImpl->TableId( ECmmDestMetadataRecord ) );
        CleanupStack::PushL( metaSet );
        TRAP_IGNORE( metaSet->LoadL( iTrans->Session() ) );
        for ( TInt i = 0; i < metaSet->iRecords.Count(); i++ )
            {
            TInt id = ( ( CCDSNAPMetadataRecord* )metaSet->iRecords[i] )->iSNAP;
            TUint32 metadata = ( ( CCDSNAPMetadataRecord* )metaSet->iRecords[i] )->iMetadata;
            TUint32 localizationValue =
                    ( metadata & CMManager::ESnapMetadataDestinationIsLocalised ) >> 4;
            TUint32 purposeValue = ( metadata & CMManager::ESnapMetadataPurpose ) >> 8;

            if ( aDestinationInstance.GetId() != id )
                {
                if ( aValue == purposeValue )
                    {
                    result = ETrue;
                    }
                switch ( aValue )
                    {
                    case CMManager::ESnapPurposeInternet:
                        {
                        if ( localizationValue == CMManager::ELocalisedDestInternet ||
                                ( metadata & CMManager::ESnapMetadataInternet ) )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ESnapPurposeOperator:
                        {
                        if ( localizationValue == CMManager::ELocalisedDestWap )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ESnapPurposeMMS:
                        {
                        if ( localizationValue == CMManager::ELocalisedDestMMS )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    case CMManager::ESnapPurposeIntranet:
                        {
                        if ( localizationValue == CMManager::ELocalisedDestIntranet )
                            {
                            result = ETrue;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( metaSet );
        }

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONEXISTSWITHMETADATAPURPOSEL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Tells the cache that a database table has changed. If the SNAP/IAP structure
// has possibly changed, the cache will refresh that information immediately.
// For other database tables, the tables are flagged and will be refreshed when
// needed.
// ---------------------------------------------------------------------------
//
void CCmmCache::DbChangeDetectedL( const TUint32 aTableId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DBCHANGEDETECTED_ENTRY );

    if ( aTableId == iSnapMetadataTableId )
        {
        for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
            {
            iDestinationArray[i]->NotifyRecordChange( ECmmDestMetadataRecord );
            }
        }
    else if ( aTableId == CommsDat::KCDTIdNetworkRecord )
        {
        // Affects destinations.
        for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
            {
            iDestinationArray[i]->NotifyRecordChange( ECmmDestNetworkRecord );
            }
        }
    else if ( aTableId == CommsDat::KCDTIdAccessPointRecord )
        {
        // Affects destinations.
        for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
            {
            iDestinationArray[i]->NotifyRecordChange( ECmmDestApRecord );
            }
        }

    // Notify Connection Methods about the table changes in CommsDat.
    NotifyPluginsForTableChangesL( aTableId );

    // Update instancemapping.
    iInstanceMapping->RefreshL();

    OstTraceFunctionExit0( CCMMCACHE_DBCHANGEDETECTED_EXIT );
    }

// ---------------------------------------------------------------------------
// Informs all the loaded iaps if something related to their tables
// changed in commsdat.
// ---------------------------------------------------------------------------
//
void CCmmCache::NotifyPluginsForTableChangesL( const TUint32 aTableId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_NOTIFYPLUGINSFORTABLECHANGESL_ENTRY );

    if ( iConnMethodArray.Count() )
        {
        RArray<TUint32> tableIdArray;
        CleanupClosePushL( tableIdArray );

        // Check if change concerns some table generic for all iaps
        ( *iPlugins )[0]->GetGenericTableIdsToBeObservedL( tableIdArray );
        TBool generic( EFalse );
        for( TInt i = 0; i < tableIdArray.Count(); i++ )
            {
            if ( aTableId == tableIdArray[i] )
                {
                generic = ETrue;
                break;
                }
            }

        if ( generic )
            {
            // generic-->Notify all iaps
            for( TInt i = 0; i < iConnMethodArray.Count(); i++ )
                {
                iConnMethodArray[i]->NotifyRecordChange( aTableId );
                }
            }
        else
            {
            // Not generic: Check bearer specific tables
            RArray<TUint32> affectedBearersArray;
            CleanupClosePushL( affectedBearersArray );
            for( TInt i = 0; i < iPlugins->Count(); i++ )
                {
                tableIdArray.Reset();
                ( *iPlugins )[i]->GetBearerTableIdsToBeObservedL( tableIdArray );
                TInt idCount = tableIdArray.Count();
                for( TInt j = 0; j < idCount; j++ )
                    {
                    if ( aTableId == tableIdArray[j] )
                        {
                        // Save the bearer type id which is affected
                        affectedBearersArray.AppendL(
                                ( *iPlugins )[i]->GetBearerInfoIntL(
                                        CMManager::ECmBearerType ) );
                        }
                    }
                }

            // Go through all the loaded iaps and notify all the iaps
            // which have the same bearer type saved above
            for( TInt i = 0; i < iConnMethodArray.Count(); i++ )
                {
                for( TInt j = 0; j < affectedBearersArray.Count(); j++ )
                    {
                    if ( iConnMethodArray[i]->GetBearerType() == affectedBearersArray[j] )
                        {
                        iConnMethodArray[i]->NotifyRecordChange( aTableId );
                        break;
                        }
                    }
                }
            CleanupStack::PopAndDestroy( &affectedBearersArray );
            }
        CleanupStack::PopAndDestroy( &tableIdArray );
        }
    OstTraceFunctionExit0( CCMMCACHE_NOTIFYPLUGINSFORTABLECHANGESL_EXIT );
    }

// ---------------------------------------------------------------------------
// Try to load unknown plugin dynamically during running-time. Some plugin
// (e.g., VPN) might be installed after CmManager starts up. So, try to load
// it. Only one plugin can be possibly loaded in a time.
// ---------------------------------------------------------------------------
//
TInt CCmmCache::RefreshPluginL( const TUint32 aBearerType )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REFRESHPLUGIN_ENTRY );

    TInt ret( KErrNotFound );
    
    // Get a list of all the bearer types.
    RPointerArray<CImplementationInformation> implArray;
    CmmCleanupResetAndDestroyPushL( implArray );
    REComSession::ListImplementationsL( TUid::Uid( KCMPluginInterfaceUid ), implArray );

    CCmPluginBaseEng* plugin = NULL;
    for ( TInt i = 0; i < implArray.Count(); i++ )
        {
        TUid uid = ( implArray )[i]->ImplementationUid();
        
        if ( uid.iUid == aBearerType )
            {
            TCmPluginInitParam params( iTrans->Session() );
            
            TRAPD( err, plugin = STATIC_CAST( CCmPluginBaseEng*, REComSession::CreateImplementationL(
                    uid,
                    _FOFF( CCmPluginBaseEng, iDtor_ID_Key ),
                    ( TAny* )&params ) ) );
            
            if ( !err )
                {
                CleanupStack::PushL( plugin );
                
                // We may not think the priority position in the plugin array for this newly loaded plugin
                // and just simply append it in the end of the plugin array
                iPlugins->AppendL( plugin );
                
                ret = iPlugins->Count() - 1;
                
                CleanupStack::Pop( plugin );
                }
 
            // Don't need to go further in the for loop
            break;
            }
        }

    CleanupStack::PopAndDestroy( &implArray );

    OstTraceFunctionExit0( CCMMCACHE_REFRESHPLUGIN_EXIT );
    return ret;
    }

// ---------------------------------------------------------------------------
// Tells the cache that an error has occured with a database listener. Any
// reads to this table need go through the database, since cache can't know if
// it has up-to-date information.
// ---------------------------------------------------------------------------
//
void CCmmCache::DbChangeError( const TUint32 aTableId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DBCHANGEERROR_ENTRY );

    (void)aTableId; //TODO
    // Flag the table as: permanently not up-to-date

    //TODO, How to do this?
    // Implement some 'status locked' flags that are always checked before changing status back to 'loaded' after reading database?
    // Or move record status info to CCmmCache-class?
    // What about plugins?
	// Or just ignore errors with notifiers?

    // For now, just ignore errors.

    OstTraceFunctionExit0( CCMMCACHE_DBCHANGEERROR_EXIT );
    }

// ---------------------------------------------------------------------------
// Tells the cache that a hadle to a destination was closed. The cache will
// decrement the related reference counter and perform any cleanup if necessary.
// This should be called automatically from the destructor of
// CCmmDestinationInstance-class.
// ---------------------------------------------------------------------------
//
void CCmmCache::CloseDestination( CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CLOSEDESTINATION_ENTRY );

    TInt index = FindDestinationFromCache( aDestinationInstance.GetId() );

    if ( index >= 0 )
        {
        TInt remainingSessionInstances = iDestinationArray[index]->DestinationInstanceClosed();
        if ( remainingSessionInstances <= 0)
            {
            // If status is 'to be deleted', then the last handle keeping this
            // destination 'alive' was closed and it can now be deleted from
            // database.
            if ( iDestinationArray[index]->GetStatus() == ECmmDestinationStatusToBeDeleted )
                {
                // Delete the destination unless an active connection is using
                // one of it's connection methods.
                TRAP_IGNORE( DeleteDestinationForcedL( aDestinationInstance ) );
                // Destination is now removed from database. Tell instance
                // mapping to stop ignoring the related ID.
                iInstanceMapping->RemoveDestinationFromDeletedList( aDestinationInstance.GetId() );
                }

            // No more references, no client has an open handle to this, delete it.
            delete iDestinationArray[index];
            iDestinationArray.Remove( index );
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_CLOSEDESTINATION_EXIT );
    }

// ---------------------------------------------------------------------------
// Tells the cache that a hadle to a connection method was closed. The cache
// will decrement the related reference counter and perform any cleanup if
// necessary.
// This should be called automatically from the destructor of
// CCmmConnMethodInstance-class.
// ---------------------------------------------------------------------------
//
void CCmmCache::CloseConnMethod( CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CLOSECONNMETHOD_ENTRY );

    TInt index = FindConnMethodFromCache( aConnMethodInstance.GetId() );

    if ( index >= 0 )
        {
        TInt remainingSessionInstances = iConnMethodArray[index]->ConnMethodInstanceClosed();
        if ( remainingSessionInstances <= 0 )
            {
            // If status is 'to be deleted', then the last handle keeping this
            // connection method 'alive' was closed and it can now be deleted
            // from database.
            if ( iConnMethodArray[index]->GetStatus() == ECmmConnMethodStatusToBeDeleted )
                {
                // Can't remove a connection method that is in use.
                if ( !CheckIfCmConnected( aConnMethodInstance.GetId() ) )
                    {
                    // Can't leave here.
                    TRAP_IGNORE( DeletePluginL( *( iConnMethodArray[index] ) ) );
                    }

                // Connection method is now removed from database. Tell
                // instance mapping to stop ignoring the related ID.
                iInstanceMapping->RemoveConnMethodFromDeletedList( aConnMethodInstance.GetId() );
                }

            // No more references, no client has an open handle to this, delete it.
            delete iConnMethodArray[index];
            iConnMethodArray.Remove( index );
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_CLOSECONNMETHOD_EXIT );
    }

//-----------------------------------------------------------------------------
// Returns a reference to the CommsDat session.
//-----------------------------------------------------------------------------
//
CommsDat::CMDBSession& CCmmCache::Session() const
    {
    // No traces.
    return iTrans->Session();
    }

//-----------------------------------------------------------------------------
// Finds out the bearer type and priority of the service type from given IAP
// record. performs LoadL()-call on the provided IAP record.
//-----------------------------------------------------------------------------
//
void CCmmCache::BearerInfoFromIapRecordL(
        CommsDat::CCDIAPRecord* aIapRecord,
        TUint32& aBearerType,
        TUint& aBearerPriority ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_BEARERINFOFROMIAPRECORDL_ENTRY );

    // Load the IAP record from IAP table. This is an optimization so that
    // plugins don't have to do it every time the CanHandleIapIdL() is called.
    aIapRecord->LoadL( Session() );
    BearerPriorityFromIapRecordL( aIapRecord, aBearerPriority );
    BearerTypeFromIapRecordL( aIapRecord, aBearerType );

    OstTraceFunctionExit0( CCMMCACHE_BEARERINFOFROMIAPRECORDL_EXIT );
    }

//-----------------------------------------------------------------------------
// Finds out the priority of the service type in given IAP record.
//-----------------------------------------------------------------------------
//
void CCmmCache::BearerPriorityFromIapRecordL(
        CommsDat::CCDIAPRecord* aIapRecord,
        TUint& aBearerPriority ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_BEARERPRIORITYFROMIAPRECORDL_ENTRY );

    aBearerPriority = CMManager::KDataMobilitySelectionPolicyPriorityWildCard;
    if ( !aIapRecord->iServiceType.IsNull() )
        {
        aBearerPriority = iBearerPriorityCache->GetPriority( aIapRecord->iServiceType.GetL() );
        }

    OstTraceFunctionExit0( CCMMCACHE_BEARERPRIORITYFROMIAPRECORDL_EXIT );
    }

//-----------------------------------------------------------------------------
// Finds out the bearer type of a connection method from given IAP record.
//-----------------------------------------------------------------------------
//
void CCmmCache::BearerTypeFromIapRecordL(
        CommsDat::CCDIAPRecord* aIapRecord,
        TUint32& aBearerType ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_BEARERTYPEFROMIAPRECORDL_ENTRY );

    TInt err( KErrNone );
    TUint32 extLevel( 0 );
    TBool canHandle( EFalse );

    TInt foundIndex( KErrNotFound );

    // Check which bearer handles the given IAP ID.
    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        TRAP( err, canHandle = (*iPlugins)[i]->CanHandleIapIdL( aIapRecord ) );
        if ( !err && canHandle )
            {
            TUint32 thisExtLevel = (*iPlugins)[i]->GetBearerInfoIntL( ECmExtensionLevel );
            if ( extLevel < thisExtLevel )
                {
                extLevel = thisExtLevel;
                foundIndex = i;
                }
            }
        }
    if ( foundIndex == KErrNotFound )
        {
        // No supporting plugin found.
        User::Leave( KErrNotSupported );
        }
    aBearerType = (*iPlugins)[foundIndex]->GetBearerInfoIntL( CMManager::ECmBearerType );

    OstTraceFunctionExit0( CCMMCACHE_BEARERTYPEFROMIAPRECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Find and return a copy of a connection method item matching the given ID.
// Returns KErrNotFound, if the connection method is not found.
// ---------------------------------------------------------------------------
//
TInt CCmmCache::GetConnMethodItem(
        const TUint32 aConnMethodId,
        TCmmConnMethodItem& aConnMethodItem ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNMETHODITEM_ENTRY );

    TInt result = iInstanceMapping->GetConnMethodItem( aConnMethodId, aConnMethodItem );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNMETHODITEM_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// Returns all conenction method IDs. Unsupported connection methods are
// included if aCheckBearerType is set to EFalse.
//-----------------------------------------------------------------------------
//
void CCmmCache::GetAllConnMethodsL(
        RArray<TUint32>& aConnMethodArray,
        TBool aCheckBearerType ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETALLCONNMETHODSL_ENTRY );

    iInstanceMapping->GetAllConnMethodsL( aConnMethodArray, aCheckBearerType );

    OstTraceFunctionExit0( CCMMCACHE_GETALLCONNMETHODSL_EXIT );
    }

//-----------------------------------------------------------------------------
// Returns the number of destinations the provided connection method belongs to.
//-----------------------------------------------------------------------------
//
TInt CCmmCache::DestinationsContainingConnMethod( const TUint32 aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODREFERENCECOUNT_ENTRY );

    TInt count = iInstanceMapping->DestinationsContainingConnMethod( aConnMethodId );

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODREFERENCECOUNT_EXIT );
    return count;
    }

//-----------------------------------------------------------------------------
// Return the EasyWLAN IAP ID, zero if not found or WLAN not supported.
//-----------------------------------------------------------------------------
//
TUint32 CCmmCache::EasyWlanIdL() const
    {
    OstTraceFunctionEntry0( CCMMCACHE_EASYWLANIDL_ENTRY );

    TUint32 easyWlanId = iInstanceMapping->EasyWlanIdL();

    OstTraceFunctionExit0( CCMMCACHE_EASYWLANIDL_EXIT );
    return easyWlanId;
    }

//-----------------------------------------------------------------------------
// Check if WLAN is supported on phone.
//-----------------------------------------------------------------------------
//
TBool CCmmCache::WlanSupported() const
    {
    OstTraceFunctionEntry0( CCMMCACHE_WLANSUPPORTED_ENTRY );

    TBool supported = iCmManagerImpl->WlanSupported();

    OstTraceFunctionExit0( CCMMCACHE_WLANSUPPORTED_EXIT );
    return supported;
    }

//-----------------------------------------------------------------------------
// Find out the internet destination ID. Set to 0 if not found.
//-----------------------------------------------------------------------------
//
void CCmmCache::InternetDestinationIdL( TUint& aInternetDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_INTERNETDESTINATIONIDL_ENTRY );

    iInstanceMapping->InternetDestinationIdL( aInternetDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_INTERNETDESTINATIONIDL_EXIT );
    }

//-----------------------------------------------------------------------------
// Return the number of destinations in database.
//-----------------------------------------------------------------------------
//
TInt CCmmCache::GetDestinationCount() const
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETDESTINATIONCOUNT_ENTRY );

    TInt count = iInstanceMapping->GetDestinationCount();

    OstTraceFunctionExit0( CCMMCACHE_GETDESTINATIONCOUNT_EXIT );
    return count;
    }

//-----------------------------------------------------------------------------
// Return an array containing all destination IDs.
//-----------------------------------------------------------------------------
//
void CCmmCache::GetDestinationsL( RArray<TUint32>& aDestinationArray ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETDESTINATIONSL_ENTRY );

    iInstanceMapping->GetDestinationsL( aDestinationArray );

    OstTraceFunctionExit0( CCMMCACHE_GETDESTINATIONSL_EXIT );
    }

//-----------------------------------------------------------------------------
// Returns all the valid connection methods under given destination.
//-----------------------------------------------------------------------------
//
void CCmmCache::GetConnMethodsFromDestinationL(
        const TUint32 aDestinationId,
        RArray<TCmmConnMethodItem>& aConnMethodArray ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNMETHODSFROMDESTINATIONL_ENTRY );

    iInstanceMapping->GetConnMethodsFromDestinationL(
            aDestinationId, aConnMethodArray );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNMETHODSFROMDESTINATIONL_EXIT );
    }

//-----------------------------------------------------------------------------
// Copies the bearer priority array's contents to aArray.
//-----------------------------------------------------------------------------
//
void CCmmCache::CopyBearerPriorityArrayL( RPointerArray<CCmmBearerPriority>& aArray ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_COPYBEARERPRIORITYARRAYL_ENTRY );

    iBearerPriorityCache->CopyL( aArray );

    OstTraceFunctionExit0( CCMMCACHE_COPYBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
// Updates the bearer priority array with the contents of aArray.
//-----------------------------------------------------------------------------
//
void CCmmCache::UpdateBearerPriorityArrayL( const RPointerArray<CCmmBearerPriority>& aArray )
    {
    OstTraceFunctionEntry0( CCMMCACHE_UPDATEBEARERPRIORITYARRAYL_ENTRY );

    iBearerPriorityCache->UpdateL( aArray );

    OstTraceFunctionExit0( CCMMCACHE_UPDATEBEARERPRIORITYARRAYL_EXIT );
    }

//-----------------------------------------------------------------------------
// Check from database if the given connection method belongs to any other
// destination than the one provided.
//-----------------------------------------------------------------------------
//
TBool CCmmCache::ConnMethodInOtherDestination(
        const TUint32 aConnMethodId,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODINOTHERDESTINATION_ENTRY );

    TBool cmInOtherDestination = iInstanceMapping->
            ConnMethodInOtherDestination( aConnMethodId, aDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODINOTHERDESTINATION_EXIT );
    return cmInOtherDestination;
    }

//-----------------------------------------------------------------------------
// Get the next free temporary ID.
//-----------------------------------------------------------------------------
//
TUint32 CCmmCache::NextFreeTemporaryId()
    {
    OstTraceFunctionEntry0( CCMMCACHE_NEXTFREETEMPORARYID_ENTRY );

    // Zero is not a valid ID.
    if ( iCurrentTemporaryId == KMaxTUint32 )
        {
        //TODO, add flag to indicate a rollover has occured. after that need to check if temp ID is free before giving it out.

        iCurrentTemporaryId = KTemporaryIdCounterStart;
        }

    iCurrentTemporaryId++;

    OstTraceFunctionExit0( CCMMCACHE_NEXTFREETEMPORARYID_EXIT );
    return iCurrentTemporaryId;
    }

//-----------------------------------------------------------------------------
// Find an open destination matching the give ID from cache.
// Returns either a valid array index or KErrNotFound.
//-----------------------------------------------------------------------------
//
TInt CCmmCache::FindDestinationFromCache( const TUint32 aId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_FINDDESTINATIONFROMCACHE_ENTRY );

    TInt result( KErrNotFound );

    // Skip if ID is 0.
    if ( aId )
        {
        for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
            {
            if ( aId == iDestinationArray[i]->GetId() )
                {
                result = i;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_FINDDESTINATIONFROMCACHE_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// Find an open destination matching the given name from cache. If a destination
// ID is provided, the search will exclude the relevant destination.
// Returns either a valid array index or KErrNotFound.
//-----------------------------------------------------------------------------
//
TInt CCmmCache::FindNotSavedDestinationFromCacheL(
        const TDesC& aName,
        const TUint32 aDestinationId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_FINDNOTSAVEDDESTINATIONFROMCACHEL_ENTRY );

    TInt result( KErrNotFound );

    // Go through destination array.
    for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
        {
        // Only check new destinations that are not yet in database.
        if ( iDestinationArray[i]->GetStatus() == ECmmDestinationStatusNotSaved )
            {
            if ( aDestinationId )
                {
                if ( aDestinationId != iDestinationArray[i]->GetId() )
                    {
                    if ( !aName.Compare( iDestinationArray[i]->GetDestinationNameL() ) )
                        {
                        result = i;
                        break;
                        }
                    }
                }
            else
                {
                if ( !aName.Compare( iDestinationArray[i]->GetDestinationNameL() ) )
                    {
                    result = i;
                    break;
                    }
                }
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_FINDNOTSAVEDDESTINATIONFROMCACHEL_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// Find an open connection method matching the given ID from cache.
// Returns either a valid array index or KErrNotFound.
//-----------------------------------------------------------------------------
//
TInt CCmmCache::FindConnMethodFromCache( const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_FINDCONNMETHODFROMCACHE_ENTRY );

    TInt result( KErrNotFound );

    // Skip if ID is 0.
    if ( aConnMethodId )
        {
        for ( TInt i = 0; i < iConnMethodArray.Count(); i++ )
            {
            if ( aConnMethodId == iConnMethodArray[i]->GetId() )
                {
                result = i;
                break;
                }
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_FINDCONNMETHODFROMCACHE_EXIT );
    return result;
    }

//-----------------------------------------------------------------------------
// Loads and initializes a connection method instance.
//-----------------------------------------------------------------------------
//
void CCmmCache::OpenConnectionMethodInstanceL(
        CCmmConnMethodInstance& aConnMethodInstance,
        const TUint32 aConnMethodId )
    {
    OstTraceFunctionEntry0( CCMMCACHE_OPENCONNECTIONMETHODINSTANCEL_ENTRY );

    // Find out the connection method bearer type.
    TUint32 bearerType( 0 );
    User::LeaveIfError( iInstanceMapping->
            GetConnMethodBearerType( aConnMethodId, bearerType ) );

    // Check bearer type support and create plugin instance.
    CCmPluginBaseEng* plugin = NULL;
    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL(
                CMManager::ECmBearerType ) == bearerType )
            {
            TCmPluginInitParam pluginParams( Session() );
            plugin = ( *iPlugins )[i]->CreateInstanceL( pluginParams );
            CleanupStack::PushL( plugin );
            plugin->LoadL( aConnMethodId );
            break;
            }
        }
    if ( !plugin )
        {
        User::Leave( KErrNotSupported );
        }

    // Store the connection method into cache.
    CCmmConnMethodStruct* connMethodStruct = CCmmConnMethodStruct::NewL( aConnMethodId );
    connMethodStruct->SetPlugin( plugin, bearerType, ECmmConnMethodStatusValid );
    CleanupStack::Pop( plugin );
    CleanupStack::PushL( connMethodStruct );
    iConnMethodArray.AppendL( connMethodStruct );
    CleanupStack::Pop( connMethodStruct );

    // Copy the connection method data to session instance.
    aConnMethodInstance.CopyDataL( connMethodStruct ); // Will increase reference counter.

    OstTraceFunctionExit0( CCMMCACHE_OPENCONNECTIONMETHODINSTANCEL_EXIT );
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetConnectionMethodInfoIntL
//-----------------------------------------------------------------------------
//
TUint32 CCmmCache::GetConnectionMethodInfoIntL(
        const TUint32 aCmId,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNECTIONMETHODINFOINTL_ENTRY );

    TUint32 retVal( 0 );

    CCmmConnMethodInstance* cmInstance = CCmmConnMethodInstance::NewLC( NULL, this );

    // Check if connection method is already opened in cache.
    TInt index = FindConnMethodFromCache( aCmId );
    if ( index != KErrNotFound )
        {
        // Already open in cache. Copy the connection method to session instance.
        // Will increase reference counter.
        cmInstance->CopyDataL( iConnMethodArray[index] );
        }
    else
        {
        OpenConnectionMethodInstanceL( *cmInstance, aCmId );
        }

    retVal = cmInstance->GetIntAttributeL( aAttribute );

    CleanupStack::PopAndDestroy( cmInstance );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNECTIONMETHODINFOINTL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetConnectionMethodInfoBoolL
//-----------------------------------------------------------------------------
//
TBool CCmmCache::GetConnectionMethodInfoBoolL(
        const TUint32 aCmId,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNECTIONMETHODINFOBOOLL_ENTRY );

    TBool retVal( EFalse );

    CCmmConnMethodInstance* cmInstance = CCmmConnMethodInstance::NewLC( NULL, this );

    // Check if connection method is already opened in cache.
    TInt index = FindConnMethodFromCache( aCmId );
    if ( index != KErrNotFound )
        {
        // Already open in cache. Copy the connection method to session instance.
        // Will increase reference counter.
        cmInstance->CopyDataL( iConnMethodArray[index] );
        }
    else
        {
        OpenConnectionMethodInstanceL( *cmInstance, aCmId );
        }

    retVal = cmInstance->GetBoolAttributeL( aAttribute );

    CleanupStack::PopAndDestroy( cmInstance );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNECTIONMETHODINFOBOOLL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetConnectionMethodInfoStringL
//-----------------------------------------------------------------------------
//
HBufC* CCmmCache::GetConnectionMethodInfoStringL(
        const TUint32 aCmId,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNECTIONMETHODINFOSTRINGL_ENTRY );

    HBufC* retVal( NULL );

    CCmmConnMethodInstance* cmInstance = CCmmConnMethodInstance::NewLC( NULL, this );

    // Check if connection method is already opened in cache.
    TInt index = FindConnMethodFromCache( aCmId );
    if ( index != KErrNotFound )
        {
        // Already open in cache. Copy the connection method to session instance.
        // Will increase reference counter.
        cmInstance->CopyDataL( iConnMethodArray[index] );
        }
    else
        {
        OpenConnectionMethodInstanceL( *cmInstance, aCmId );
        }

    retVal = cmInstance->GetStringAttributeL( aAttribute );

    CleanupStack::PopAndDestroy( cmInstance );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNECTIONMETHODINFOSTRINGL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetConnectionMethodInfoString8L
//-----------------------------------------------------------------------------
//
HBufC8* CCmmCache::GetConnectionMethodInfoString8L(
        const TUint32 aCmId,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETCONNECTIONMETHODINFOSTRING8L_ENTRY );

    HBufC8* retVal( NULL );

    CCmmConnMethodInstance* cmInstance = CCmmConnMethodInstance::NewLC( NULL, this );

    // Check if connection method is already opened in cache.
    TInt index = FindConnMethodFromCache( aCmId );
    if ( index != KErrNotFound )
        {
        // Already open in cache. Copy the connection method to session instance.
        cmInstance->CopyDataL( iConnMethodArray[index] ); // Will increase reference counter.
        }
    else
        {
        OpenConnectionMethodInstanceL( *cmInstance, aCmId );
        }

    retVal = cmInstance->GetString8AttributeL( aAttribute );

    CleanupStack::PopAndDestroy( cmInstance );

    OstTraceFunctionExit0( CCMMCACHE_GETCONNECTIONMETHODINFOSTRING8L_EXIT );
    return retVal;
    }


//-----------------------------------------------------------------------------
// CCmmCache::GetBearerInfoIntL
//-----------------------------------------------------------------------------
//
TUint32 CCmmCache::GetBearerInfoIntL(
        const TUint32 aBearerType,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETBEARERINFOINTL_ENTRY );

    TUint32 retVal( 0 );
    TBool found( EFalse );

    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == aBearerType )
            {
            found = ETrue;
            retVal = ( *iPlugins )[i]->GetBearerInfoIntL( aAttribute );
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrArgument );
        }

    OstTraceFunctionExit0( CCMMCACHE_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetBearerInfoBoolL
//-----------------------------------------------------------------------------
//
TBool CCmmCache::GetBearerInfoBoolL(
        const TUint32 aBearerType,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETBEARERINFOBOOLL_ENTRY );

    TBool retVal( EFalse );
    TBool found( EFalse );

    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == aBearerType )
            {
            found = ETrue;
            retVal = ( *iPlugins )[i]->GetBearerInfoBoolL( aAttribute );
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrArgument );
        }

    OstTraceFunctionExit0( CCMMCACHE_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetBearerInfoStringL
//-----------------------------------------------------------------------------
//
HBufC* CCmmCache::GetBearerInfoStringL(
        const TUint32 aBearerType,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETBEARERINFOSTRINGL_ENTRY );

    HBufC* retVal( NULL );
    TBool found( EFalse );

    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == aBearerType )
            {
            found = ETrue;
            retVal = ( *iPlugins )[i]->GetBearerInfoStringL( aAttribute );
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrArgument );
        }

    OstTraceFunctionExit0( CCMMCACHE_GETBEARERINFOSTRINGL_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// CCmmCache::GetBearerInfoString8L
//-----------------------------------------------------------------------------
//
HBufC8* CCmmCache::GetBearerInfoString8L(
        const TUint32 aBearerType,
        const TUint32 aAttribute )
    {
    OstTraceFunctionEntry0( CCMMCACHE_GETBEARERINFOSTRING8L_ENTRY );

    HBufC8* retVal( NULL );
    TBool found( EFalse );

    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        if ( ( *iPlugins )[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) == aBearerType )
            {
            found = ETrue;
            retVal = ( *iPlugins )[i]->GetBearerInfoString8L( aAttribute );
            break;
            }
        }
    if ( !found )
        {
        User::Leave( KErrArgument );
        }

    OstTraceFunctionExit0( CCMMCACHE_GETBEARERINFOSTRING8L_EXIT );
    return retVal;
    }

//-----------------------------------------------------------------------------
// Read general connection settings from database.
//-----------------------------------------------------------------------------
//
void CCmmCache::ReadGenConnSettingsL( TCmGenConnSettings& aGenConnSettings ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_READGENCONNSETTINGSL_ENTRY );

    CommsDat::CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDDefConnRecord>(
                    iCmManagerImpl->TableId( ECmmDbDefConnRecord ) );
    CleanupStack::PushL( defConnRecordSet );

    CCDDefConnRecord* defConnRecord =
            new( ELeave ) CCDDefConnRecord(
                    iCmManagerImpl->TableId( ECmmDbDefConnRecord ) );
    CleanupStack::PushL( defConnRecord );

    TRAP_IGNORE( defConnRecordSet->LoadL( Session() ) );
    if ( defConnRecordSet->iRecords.Count() > 0 )
        {
        defConnRecord->SetElementId( defConnRecordSet->iRecords[0]->ElementId() );
        defConnRecord->LoadL( Session() );

        aGenConnSettings.iUsageOfWlan =
                TCmUsageOfWlan( ( TInt )defConnRecord->iUsageOfWlan );
        aGenConnSettings.iCellularDataUsageHome =
                TCmCellularDataUsage( ( TInt )defConnRecord->iCellularDataUsageHome );
        aGenConnSettings.iCellularDataUsageVisitor =
                TCmCellularDataUsage( ( TInt )defConnRecord->iCellularDataUsageVisitor );
        }
    else
        {
        iTrans->OpenTransactionLC();

        aGenConnSettings.iUsageOfWlan = ECmUsageOfWlanKnown;
        aGenConnSettings.iCellularDataUsageHome = ECmCellularDataUsageAutomatic;
        aGenConnSettings.iCellularDataUsageVisitor = ECmCellularDataUsageConfirm;

        defConnRecord->SetRecordId( KCDNewRecordRequest );
        defConnRecord->iUsageOfWlan = ( TUint )aGenConnSettings.iUsageOfWlan;
        defConnRecord->iCellularDataUsageHome =
                ( TUint )aGenConnSettings.iCellularDataUsageHome;
        defConnRecord->iCellularDataUsageVisitor =
                ( TUint )aGenConnSettings.iCellularDataUsageVisitor;
        defConnRecord->StoreL( Session() );

        iTrans->CommitTransactionL();
        }

    CleanupStack::PopAndDestroy( defConnRecord );
    CleanupStack::PopAndDestroy( defConnRecordSet );

    OstTraceFunctionExit0( CCMMCACHE_READGENCONNSETTINGSL_EXIT );
    }

//-----------------------------------------------------------------------------
// Write general connection settings to database.
//-----------------------------------------------------------------------------
//
void CCmmCache::WriteGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( CCMMCACHE_WRITEGENCONNSETTINGSL_ENTRY );

    iTrans->OpenTransactionLC();

    ReplaceGenConnSettingsL( aGenConnSettings );

    iTrans->CommitTransactionL();

    OstTraceFunctionExit0( CCMMCACHE_WRITEGENCONNSETTINGSL_EXIT );
    }

//-----------------------------------------------------------------------------
//  CCmmCache::ReplaceGenConnSettingsL()
//-----------------------------------------------------------------------------
//
void CCmmCache::ReplaceGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REPLACEGENCONNSETTINGSL_ENTRY );
    TBool oldRecordExists( EFalse );

    CommsDat::CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDDefConnRecord>(
                    iCmManagerImpl->TableId( ECmmDbDefConnRecord ) );
    CleanupStack::PushL( defConnRecordSet );

    CCDDefConnRecord* defConnRecord =
            new( ELeave ) CCDDefConnRecord(
                    iCmManagerImpl->TableId( ECmmDbDefConnRecord ) );
    CleanupStack::PushL( defConnRecord );

    TRAPD( err, defConnRecordSet->LoadL( Session() ) );
    if ( err == KErrNone )
        {
        defConnRecord->SetElementId( defConnRecordSet->iRecords[0]->ElementId() );
        defConnRecord->LoadL( Session() );
        oldRecordExists = ETrue;
        }
    else if ( err == KErrNotFound )
        {
        defConnRecord->SetRecordId( KCDNewRecordRequest );
        }
    else
        {
        User::Leave( err );
        }

    defConnRecord->iUsageOfWlan = ( TUint )aGenConnSettings.iUsageOfWlan;
    defConnRecord->iCellularDataUsageHome = ( TUint )aGenConnSettings.iCellularDataUsageHome;
    defConnRecord->iCellularDataUsageVisitor = ( TUint )aGenConnSettings.iCellularDataUsageVisitor;

    if ( oldRecordExists )
        {
        defConnRecord->ModifyL( Session() );
        }
    else
        {
        defConnRecord->StoreL( Session() );
        }

    CleanupStack::PopAndDestroy( defConnRecord );
    CleanupStack::PopAndDestroy( defConnRecordSet );

    OstTraceFunctionExit0( CCMMCACHE_REPLACEGENCONNSETTINGSL_EXIT );
    }

// ---------------------------------------------------------------------------
// Opens a transaction and deletes the given connection method.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeletePluginL( CCmmConnMethodStruct& aConnMethodStruct )
    {
    iTrans->OpenTransactionLC();
    aConnMethodStruct.GetPlugin()->DeleteL();
    iTrans->CommitTransactionL();
    }

// ---------------------------------------------------------------------------
// Enumerates connections and checks if the given connection method is
// connected.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::CheckIfCmConnected( const TUint32 aCmId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_CHECKIFCMCONNECTED_ENTRY );

    TBool result( EFalse );
    RSocketServ ss;
    RConnection connection;
    TUint connectionCount( 0 );

    if ( ss.Connect() == KErrNone )
        {
        if ( connection.Open( ss, KAfInet ) == KErrNone )
            {
            if ( connection.EnumerateConnections( connectionCount ) == KErrNone )
                {
                TPckgBuf<TConnectionInfo> connInfo;

                for ( TInt i = 1; i <= connectionCount; i++ )
                    {
                    connection.GetConnectionInfo( i, connInfo );

                    if ( connInfo().iIapId == aCmId )
                        {
                        result = ETrue;
                        break;
                        }
                    }

                }
            connection.Close();
            }
        ss.Close();
        }

    OstTraceFunctionExit0( CCMMCACHE_CHECKIFCMCONNECTED_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Enumerates connections and checks if any of the connection methods in the
// given destination is connected.
// If pointer to destination instance is given, then information about
// relevant connection methods is retrieved from that. Otherwise the
// information is retrieved from instance mapping using the given ID.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationConnectedL(
        const TUint32 aDestinationId,
        CCmmDestinationInstance* aDestinationInstance ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONCONNECTEDL_ENTRY );

    TBool result( EFalse );
    RSocketServ ss;
    RConnection connection;
    TUint connectionCount( 0 );

    if ( ss.Connect() == KErrNone )
        {
        if ( connection.Open( ss, KAfInet ) == KErrNone )
            {
            if ( connection.EnumerateConnections( connectionCount ) == KErrNone )
                {
                RArray<TCmmConnMethodItem> connMethodArray;
                CleanupClosePushL( connMethodArray );

                // If destination instance provided, take the destination's
                // connection methods from there. Otherwise get the connection
                // methods from instance mapping. (If the destination is marked
                // to be deleted, instance mapping won't have information about
                // it.)
                if ( aDestinationInstance )
                    {
                    for ( TInt i = 0; i < aDestinationInstance->iConnMethodItemArray.Count(); i++ )
                        {
                        connMethodArray.AppendL( aDestinationInstance->iConnMethodItemArray[i] );
                        }
                    }
                else
                    {
                    iInstanceMapping->GetConnMethodsFromDestinationL(
                            aDestinationId,
                            connMethodArray );
                    }

                // Iterate through all connections.
                TPckgBuf<TConnectionInfo> connInfo;
                for ( TUint i = 1; i <= connectionCount; i++ )
                    {
                    connection.GetConnectionInfo( i, connInfo );

                    // Iterate through all connection methods in destinations.
                    for ( TInt j = 0; j < connMethodArray.Count(); j++ )
                        {
                        if ( connInfo().iIapId == connMethodArray[j].iId )
                            {
                            result = ETrue;
                            break;
                            }
                        }
                    if ( result )
                        {
                        break;
                        }
                    }

                CleanupStack::PopAndDestroy( &connMethodArray );
                }
            connection.Close();
            }
        ss.Close();
        }

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONCONNECTEDL_EXIT );
    return result;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination is an embedded destination in
// any other destination.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationIsEmbedded( const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONISEMBEDDED_ENTRY );

    TBool isEmbedded = iInstanceMapping->DestinationIsEmbedded( aDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONISEMBEDDED_EXIT );
    return isEmbedded;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination has an embedded destination.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationHasEmbedded( const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONHASEMBEDDED_ENTRY );

    TBool hasEmbedded = iInstanceMapping->DestinationHasEmbedded( aDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONHASEMBEDDED_EXIT );
    return hasEmbedded;
    }

// ---------------------------------------------------------------------------
// Check from database if the given destination is pointed to by any virtual
// IAP.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::DestinationPointedToByVirtualIap( const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_DESTINATIONPOINTEDTOBYVIRTUALIAP_ENTRY );

    TBool pointedByVirtual =
            iInstanceMapping->DestinationPointedToByVirtualIap( aDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_DESTINATIONPOINTEDTOBYVIRTUALIAP_EXIT );
    return pointedByVirtual;
    }

// ---------------------------------------------------------------------------
// Check from database if the given connection method is pointed to by any
// virtual IAP.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::ConnMethodPointedToByVirtualIap( const TUint32 aConnMethodId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODPOINTEDTOBYVIRTUALIAP_ENTRY );

    TBool pointedToByVirtual =
            iInstanceMapping->ConnMethodPointedToByVirtualIap( aConnMethodId );

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODPOINTEDTOBYVIRTUALIAP_EXIT );
    return pointedToByVirtual;
    }

// ---------------------------------------------------------------------------
// Check if the given connection method is the only connection method in the
// given destination and if a virtual IAP points to that destination.
// ---------------------------------------------------------------------------
//
TBool CCmmCache::ConnMethodInDestinationButLocked(
        const TUint32 aConnMethodId,
        const TUint32 aDestinationId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_CONNMETHODINDESTINATIONBUTLOCKED_ENTRY );

    TBool inAndlocked = iInstanceMapping->
            ConnMethodInDestinationButLocked( aConnMethodId, aDestinationId );

    OstTraceFunctionExit0( CCMMCACHE_CONNMETHODINDESTINATIONBUTLOCKED_EXIT );
    return inAndlocked;
    }

// ---------------------------------------------------------------------------
// Remove all references to the given connection method from the datamobility
// selection policy records. Then update instance mapping to reflect the
// current database state, and notify any possible client handles for the
// changed destinations. Also removes the connection method from any
// destination handles the client has open.
// ---------------------------------------------------------------------------
//
void CCmmCache::RemoveAllReferencesToConnMethodL(
        CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REMOVEALLREFERENCESTOCONNMETHODL_ENTRY );

    TUint32 connMethodId( aConnMethodInstance.GetId() );

    iTrans->OpenTransactionLC();

    // Create DataMobilitySelectionPolicy-record set.
    CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* snapRecordSet =
            new( ELeave ) CommsDat::CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>(
                    iCmManagerImpl->TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecordSet );

    // Create a DataMobilitySelectionPolicy-record.
    CCDDataMobilitySelectionPolicyRecord* snapRecord =
            new( ELeave ) CCDDataMobilitySelectionPolicyRecord(
                    iCmManagerImpl->TableId( ECmmDbSnapRecord ) );
    CleanupStack::PushL( snapRecord );

    TRAP_IGNORE( snapRecordSet->LoadL( iTrans->Session() ) );

    // Read IAP ID from each record and delete any that match the connection
    // method we are removing all references for.
    TUint32 connMethodIdInRecord( 0 );
    TInt snapRecordCount( snapRecordSet->iRecords.Count() );
    for ( TInt i = 0; i < snapRecordCount; i++ )
        {
        snapRecord->SetElementId( snapRecordSet->iRecords[i]->ElementId() );
        snapRecord->LoadL( iTrans->Session() );

        connMethodIdInRecord = ( snapRecord->iIAP & KCDMaskShowRecordId ) >> 8;
        if ( connMethodIdInRecord == connMethodId )
            {
            snapRecord->DeleteL( iTrans->Session() );
            }
        }
    CleanupStack::PopAndDestroy( snapRecord );
    CleanupStack::PopAndDestroy( snapRecordSet );

    iTrans->CommitTransactionL();

    // Reference count will be zero if this method call is not part of some
    // bigger operation (e.g. connection method delete).
    if ( iTrans->GetReferenceCount() == 0 )
        {
        // Update instance mapping to reflect the current database state, and
        // notify any possible client handles for the changed destinations.
        RArray<TUint32> changedDestinations;
        iInstanceMapping->RemoveConnMethodFromDestinations( connMethodId, changedDestinations );
        for ( TInt i = 0; i < changedDestinations.Count(); i++ )
            {
            TCmmIdStruct idStruct( changedDestinations[i], 0 );
            aConnMethodInstance.RefreshHandlesForAllSessions( idStruct );
            }
        changedDestinations.Close();

        // Remove the connection method from any destination handles the client
        // has open.
        aConnMethodInstance.RemoveConnMethodFromSessionDestinationHandles( connMethodId );
        }

    OstTraceFunctionExit0( CCMMCACHE_REMOVEALLREFERENCESTOCONNMETHODL_EXIT );
    }

// ---------------------------------------------------------------------------
// Update the ID of a new destination from temporary ID to real ID after a
// successful update to database.
// ---------------------------------------------------------------------------
//
void CCmmCache::RefreshDestinationId( const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REFRESHDESTINATIONID_ENTRY );

    // Iterate destinations in cache and update the ID if match found.
    for ( TInt i = 0; i < iDestinationArray.Count(); i++ )
        {
        if ( iDestinationArray[i]->GetId() == aIdStruct.iTemporaryId )
            {
            iDestinationArray[i]->SetId( aIdStruct.iRealId );
            break;
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_REFRESHDESTINATIONID_EXIT );
    }

// ---------------------------------------------------------------------------
// Update the ID of a new connection method from temporary ID to real ID after
// a successful update to database.
// ---------------------------------------------------------------------------
//
void CCmmCache::RefreshConnMethodId( const TCmmIdStruct& aIdStruct )
    {
    OstTraceFunctionEntry0( CCMMCACHE_REFRESHCONNMETHODID_ENTRY );

    // Iterate connection methods in cache and update the ID if match found.
    for ( TInt i = 0; i < iConnMethodArray.Count(); i++ )
        {
        if ( iConnMethodArray[i]->GetId() == aIdStruct.iTemporaryId )
            {
            iConnMethodArray[i]->SetId( aIdStruct.iRealId );
            break; // Can only be 1 match.
            }
        }

    OstTraceFunctionExit0( CCMMCACHE_REFRESHCONNMETHODID_EXIT );
    }

// ---------------------------------------------------------------------------
// During destination update, after updating connection methods inside the
// destination, the database records containing the information of what
// connection methods are inside the destination need to be updated with real
// IDs for any newly created connection methods. These real IDs are held in the
// temporary array iUpdatedConnMethods until successful commit to database.
// This method is used to find out those real IDs before that.
// ---------------------------------------------------------------------------
//
void CCmmCache::TranslateTemporaryId( const TUint32 aTemporaryId, TUint32& aRealId ) const
    {
    OstTraceFunctionEntry0( CCMMCACHE_TRANSLATETEMPORARYID_ENTRY );

    aRealId = 0;

    for ( TInt i = 0; i < iUpdatedConnMethods.Count(); i++ )
        {
        if ( iUpdatedConnMethods[i].iTemporaryId == aTemporaryId )
            {
            aRealId = iUpdatedConnMethods[i].iRealId;
            break;
            }
        }
    if ( !aRealId )
        {
        ASSERT( 0 );
        }

    OstTraceFunctionExit0( CCMMCACHE_TRANSLATETEMPORARYID_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given connection method can have all references removed and
// made into an uncategorized connection method.
// ---------------------------------------------------------------------------
//
void CCmmCache::CheckIfConnMethodReferencesCanBeRemovedL(
        const CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CHECKIFCONNMETHODREFERENCESCANBEREMOVEDL_ENTRY );

    TUint32 connMethodId( aConnMethodInstance.GetId() );

    // Check that connection method exists in database.
    if ( !iInstanceMapping->ValidConnMethodId( connMethodId ) )
        {
        User::Leave( KErrNotFound );
        }

    // Can't remove an embedded destination this way.
    if ( aConnMethodInstance.IsEmbeddedDestination() )
        {
        User::Leave( KErrArgument );
        }

    // Iterate all destinations in database and check if possible reference can
    // be removed.
    RArray<TUint32> dbDestinations;
    CleanupClosePushL( dbDestinations );
    iInstanceMapping->GetDestinationsL( dbDestinations );
    for ( TInt i = 0; i < dbDestinations.Count(); i++ )
        {
        if ( iInstanceMapping->ConnMethodInDestinationButLocked(
                connMethodId,
                dbDestinations[i] ) )
            {
            User::Leave( KErrLocked );
            }
        }
    CleanupStack::PopAndDestroy( &dbDestinations );

    // Can't remove a connection method that is in use.
    if ( CheckIfCmConnected( connMethodId ) )
        {
        User::Leave( KErrInUse );
        }

    OstTraceFunctionExit0( CCMMCACHE_CHECKIFCONNMETHODREFERENCESCANBEREMOVEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if given connection method is referenced from any protected destination.
// ---------------------------------------------------------------------------
//
void CCmmCache::CheckIfConnMethodBelongsToProtectedDestinationL(
        const CCmmConnMethodInstance& aConnMethodInstance,
        TBool& aBelongsToProtectedDestination )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CHECKIFCONNMETHODBELONGSTOPROTECTEDDESTINATIONL_ENTRY );

    TUint32 connMethodId( aConnMethodInstance.GetId() );

    // Check that connection method exists in database.
    if ( !iInstanceMapping->ValidConnMethodId( connMethodId ) )
        {
        User::Leave( KErrNotFound );
        }

    aBelongsToProtectedDestination = EFalse;
    // Get destinations which have references to connection method passed as parameter.
    RArray<TUint32> dbDestinations;
    CleanupClosePushL( dbDestinations );
    iInstanceMapping->DestinationsContainingConnMethodL( connMethodId, dbDestinations );
    TUint32 metadata( 0 );
    for ( TInt i = 0; i < dbDestinations.Count(); i++ )
        {
        // Check if any of destinations is protected.
        metadata = iInstanceMapping->DestinationMetadata( dbDestinations[i] );
        TUint32 protlevel =
                ( metadata & KDestProtectionLevelMask ) >> KBitsToShiftDestProtectionLevel;
        if ( protlevel == CMManager::EProtLevel1 || protlevel == CMManager::EProtLevel3 )
            {
            aBelongsToProtectedDestination = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( &dbDestinations );

    OstTraceFunctionExit0( CCMMCACHE_CHECKIFCONNMETHODBELONGSTOPROTECTEDDESTINATIONL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given connection method can be deleted.
// ---------------------------------------------------------------------------
//
void CCmmCache::CheckIfConnMethodCanBeDeletedL(
        const CCmmConnMethodInstance& aConnMethodInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CHECKIFCONNMETHODCANBEDELETEDL_ENTRY );

    TUint32 connMethodId( aConnMethodInstance.GetId() );

    // Find connection method from cache.
    TInt index = FindConnMethodFromCache( connMethodId );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrBadHandle );
        }

    // Check connection method status at cache side.
    switch ( iConnMethodArray[index]->GetStatus() )
        {
        case ECmmConnMethodStatusNotSaved:
            {
            // Connection method is not in database, nothing to delete.
            User::Leave( KErrNotFound );
            }
            break;
        case ECmmConnMethodStatusValid:
            // Proceed.
            break;
        case ECmmConnMethodStatusToBeDeleted:
            // Connection method has already been deleted.
            return;
        case ECmmConnMethodStatusChanged:
        default:
            {
            ASSERT( 0 ); // Error, illegal status.
            User::Leave( KErrCorrupt );
            }
            break;
        }

    // Removing all references is part of deleting a connection method. Check
    // if can do that.
    CheckIfConnMethodReferencesCanBeRemovedL( aConnMethodInstance );

    // Check that no virtual IAP points to this connection method.
    if ( iInstanceMapping->ConnMethodPointedToByVirtualIap( connMethodId ) )
        {
        User::Leave( KErrLocked );
        }

    OstTraceFunctionExit0( CCMMCACHE_CHECKIFCONNMETHODCANBEDELETEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Check if the given destination can be deleted.
// ---------------------------------------------------------------------------
//
void CCmmCache::CheckIfDestinationCanBeDeletedL(
        const CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_CHECKIFDESTINATIONCANBEDELETEDL_ENTRY );

    TUint32 destinationId( aDestinationInstance.GetId() );

    // Find destination from cache.
    TInt index = FindDestinationFromCache( destinationId );
    if ( index == KErrNotFound )
        {
        User::Leave( KErrBadHandle );
        }

    // Check destination status in cache.
    switch ( iDestinationArray[index]->GetStatus() )
        {
        case ECmmDestinationStatusNotSaved:
            {
            User::Leave( KErrNotFound );
            }
            break;
        case ECmmDestinationStatusValid:
            // Proceed.
            break;
        case ECmmDestinationStatusToBeDeleted:
            // Destination has already been deleted.
            return;
        case ECmmDestinationStatusChanged:
        default:
            {
            ASSERT( 0 ); // Error, illegal status.
            User::Leave( KErrCorrupt );
            }
            break;
        }

    // Check destination exists in database.
    if ( !iInstanceMapping->ValidDestinationId( destinationId ) )
        {
        User::Leave( KErrNotFound );
        }

    // Check if any virtual IAP points to this destination. Don't check session side.
    if ( iInstanceMapping->DestinationPointedToByVirtualIap( destinationId ) )
        {
        User::Leave( KErrLocked );
        }

    // Check if any of the connection methods in this destination are currently in use.
    if ( DestinationConnectedL( destinationId ) )
        {
        User::Leave( KErrInUse );
        }

    OstTraceFunctionExit0( CCMMCACHE_CHECKIFDESTINATIONCANBEDELETEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// Return the requested table ID.
// ---------------------------------------------------------------------------
//
CommsDat::TMDBElementId CCmmCache::TableId( TCmmDbRecords aRecord )
    {
    return iCmManagerImpl->TableId( aRecord );
    }

// ---------------------------------------------------------------------------
// Initiate the deletion of given destination if none of the connection
// methods inside it are connected.
// ---------------------------------------------------------------------------
//
void CCmmCache::DeleteDestinationForcedL( CCmmDestinationInstance& aDestinationInstance )
    {
    OstTraceFunctionEntry0( CCMMCACHE_DELETEDESTINATIONFORCEDL_ENTRY );

    if ( !DestinationConnectedL( 0, &aDestinationInstance ) )
        {
        DeleteDestinationL( aDestinationInstance, ETrue );
        }

    OstTraceFunctionExit0( CCMMCACHE_DELETEDESTINATIONFORCEDL_EXIT );
    }

// End of file
