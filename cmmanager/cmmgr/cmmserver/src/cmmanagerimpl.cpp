/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of CCmManagerImpl.
*
*/


#include <cmpluginbaseeng.h>
#include <featmgr.h>
#include <cmpluginembdestinationdef.h> // KUidEmbeddedDestination

#include "cmmanagerimpl.h"
#include "cmmtransactionhandler.h"
#include "cmmcache.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmanagerimplTraces.h"
#endif

const TInt KPluginGranularity = 3;


//-----------------------------------------------------------------------------
//  CCmManagerImpl::NewL()
//-----------------------------------------------------------------------------
//
CCmManagerImpl* CCmManagerImpl::NewL()
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_NEWL_ENTRY );

    CCmManagerImpl* self = new( ELeave ) CCmManagerImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMMANAGERIMPL_NEWL_EXIT );
    return self;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CCmManagerImpl()
//-----------------------------------------------------------------------------
//
CCmManagerImpl::~CCmManagerImpl()
    {
    OstTraceFunctionEntry0( DUP1_CCMMANAGERIMPL_CCMMANAGERIMPL_ENTRY );

    delete iCache;

    if ( iPlugins )
        {
        iPlugins->ResetAndDestroy();
        delete iPlugins;
        }

    delete iTrans;

    if ( iIsFeatureManagerInitialised )
        {
        // Feature manager must not be uninitialized earlier. Plugins must be
        // able to use it.
        FeatureManager::UnInitializeLib();
        }

    REComSession::FinalClose();

    OstTraceFunctionExit0( DUP1_CCMMANAGERIMPL_CCMMANAGERIMPL_EXIT );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CCmManagerImpl()
//-----------------------------------------------------------------------------
//
CCmManagerImpl::CCmManagerImpl()
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_CCMMANAGERIMPL_ENTRY );

    iCache = NULL;
    iPlugins = NULL;
    iTrans = NULL;
    iSnapTableId = 0;
    iBearerPriorityTableId = 0;
    iDestMetadataTableId = 0;
    iDefConnTableId = 0;
    iIsFeatureManagerInitialised = EFalse;
    iWLanSupport = ETrue;

    OstTraceFunctionExit0( CCMMANAGERIMPL_CCMMANAGERIMPL_EXIT );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::ConstructL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::ConstructL()
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_CONSTRUCTL_ENTRY );

    // Feature Manager is initialized here, and it can be used by plugins also.
    // It is uninitialized in destructor.
    FeatureManager::InitializeLibL();
    iIsFeatureManagerInitialised = ETrue;
    iWLanSupport = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );

    CommsDat::CMDBSession* db = CommsDat::CMDBSession::NewLC( KCDVersion1_2 );
    iTrans = CCmmTransactionHandler::NewL( *db ); // Ownership of db transferred.
    CleanupStack::Pop( db );

    CheckTablesL();
    BuildPluginArrayL();

    iCache = CCmmCache::NewL( this, iPlugins );

    OstTraceFunctionExit0( CCMMANAGERIMPL_CONSTRUCTL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::CheckTablesL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::CheckTablesL()
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_CHECKTABLESL_ENTRY );

    TInt err( 0 );

    TRAP( err, iBearerPriorityTableId =
            CCDGlobalBearerTypePriorizationRecord::TableIdL( iTrans->Session() ) );
    if ( err == KErrNotFound )
        {
        iBearerPriorityTableId =
               CCDGlobalBearerTypePriorizationRecord::CreateTableL( iTrans->Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }

    TRAP( err, iSnapTableId = CCDDataMobilitySelectionPolicyRecord::TableIdL( iTrans->Session() ));
    if ( err == KErrNotFound )
        {
        iSnapTableId = CCDDataMobilitySelectionPolicyRecord::CreateTableL( iTrans->Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }

    TRAP( err, iDestMetadataTableId = CCDSNAPMetadataRecord::TableIdL( iTrans->Session() ) );
    if ( err == KErrNotFound )
        {
        iDestMetadataTableId = CCDSNAPMetadataRecord::CreateTableL( iTrans->Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }

    TRAP( err, iDefConnTableId = CCDDefConnRecord::TableIdL( iTrans->Session() ) );
    if ( err == KErrNotFound )
        {
        iDefConnTableId = CCDDefConnRecord::CreateTableL( iTrans->Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }

    OstTraceFunctionExit0( CCMMANAGERIMPL_CHECKTABLESL_EXIT );
    }

//-----------------------------------------------------------------------------
// Return an array containing the supported bearer types (embedded destination
// excluded).
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_SUPPORTEDBEARERSL_ENTRY );

    aArray.Reset();
    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        TUint32 bearerType( (*iPlugins)[i]->GetBearerInfoIntL( CMManager::ECmBearerType ) );
        if ( KUidEmbeddedDestination != bearerType )
            {
            aArray.AppendL( bearerType );
            }
        }
    OstTraceFunctionExit0( CCMMANAGERIMPL_SUPPORTEDBEARERSL_EXIT );
    }

//-----------------------------------------------------------------------------
// Return the requested table ID.
//-----------------------------------------------------------------------------
//
CommsDat::TMDBElementId CCmManagerImpl::TableId( TCmmDbRecords aRecord )
    {
    CommsDat::TMDBElementId tableId( 0 );
    switch ( aRecord )
        {
        case ECmmDestNetworkRecord:
            tableId = CommsDat::KCDTIdNetworkRecord;
            break;
        case ECmmDestApRecord:
            tableId = CommsDat::KCDTIdAccessPointRecord;
            break;
        case ECmmDestMetadataRecord:
            tableId = iDestMetadataTableId;
            break;
        case ECmmDbSnapRecord:
            tableId = iSnapTableId;
            break;
        case ECmmDbBearerPriorityRecord:
            tableId = iBearerPriorityTableId;
            break;
        case ECmmDbDefConnRecord:
            tableId = iDefConnTableId;
            break;
        default:
            break;
        }
    return tableId;
    }

// -----------------------------------------------------------------------------
// Get a handle to the database cache manager.
// -----------------------------------------------------------------------------
//
CCmmCache& CCmManagerImpl::Cache()
    {
    return *iCache;
    }

//-----------------------------------------------------------------------------
// Get a handle to the database transaction handler.
//-----------------------------------------------------------------------------
//
CCmmTransactionHandler* CCmManagerImpl::GetTransactionHandler()
    {
    return iTrans;
    }

//-----------------------------------------------------------------------------
// Get a handle to database session.
//-----------------------------------------------------------------------------
//
CommsDat::CMDBSession& CCmManagerImpl::Session() const
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_SESSION_ENTRY );
    return iTrans->Session();
    }

// ---------------------------------------------------------------------------
// Return true is WLAN is supported on phone.
// ---------------------------------------------------------------------------
//
TBool CCmManagerImpl::WlanSupported() const
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_WLANSUPPORTED_ENTRY );
    OstTraceFunctionExit0( CCMMANAGERIMPL_WLANSUPPORTED_EXIT );
    return iWLanSupport;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::BuildPluginArrayL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::BuildPluginArrayL()
    {
    OstTraceFunctionEntry0( CCMMANAGERIMPL_BUILDPLUGINARRAYL_ENTRY );

    iPlugins = new( ELeave ) CArrayPtrFlat<const CCmPluginBaseEng>( KPluginGranularity );

    // Get a list of all the bearer types.
    RPointerArray<CImplementationInformation> implArray;
    CmmCleanupResetAndDestroyPushL( implArray );
    REComSession::ListImplementationsL( TUid::Uid( KCMPluginInterfaceUid ), implArray );

    CCmPluginBaseEng* plugin = NULL;
    for ( TInt i = 0; i < implArray.Count(); i++ )
        {
        TCmPluginInitParam params( iTrans->Session() );

        // This is the Util implementation.
        // Trap it to be able to work even if there's a faulty plugin installed
        // in the phone.
        TRAPD( err, plugin = STATIC_CAST( CCmPluginBaseEng*, REComSession::CreateImplementationL(
                ( implArray )[i]->ImplementationUid(),
                _FOFF( CCmPluginBaseEng, iDtor_ID_Key ),
                ( TAny* )&params ) ) );

        if ( !err )
            {
            CleanupStack::PushL( plugin );

            TBool inserted( EFalse );

            TInt defaultPriority( CMManager::KDataMobilitySelectionPolicyPriorityWildCard );

            // No priority -> wildcard
            TRAP_IGNORE( defaultPriority = plugin->GetBearerInfoIntL(
                    CMManager::ECmDefaultPriority ) );

            for ( TInt j = 0; j < iPlugins->Count(); j++ )
                {
                TUint32 bearerToCompare = (*iPlugins)[j]->GetBearerInfoIntL(
                        CMManager::ECmDefaultPriority );
                if ( defaultPriority > bearerToCompare )
                    {
                    continue;
                    }
                else if ( defaultPriority == bearerToCompare )
                    {
                    if ( plugin->GetBearerInfoIntL( ECmExtensionLevel ) >
                            (*iPlugins)[j]->GetBearerInfoIntL( ECmExtensionLevel ) )
                        {
                        iPlugins->InsertL( j, plugin );
                        inserted = ETrue;
                        break;
                        }
                    }
                else
                    {
                    iPlugins->InsertL( j, plugin );
                    inserted = ETrue;
                    break;
                    }
                }

            if ( !inserted )
                {
                iPlugins->AppendL( plugin );
                }
            CleanupStack::Pop( plugin );
            }
        }

    CleanupStack::PopAndDestroy( &implArray );

    OstTraceFunctionExit0( CCMMANAGERIMPL_BUILDPLUGINARRAYL_EXIT );
    }

// Enf of file
