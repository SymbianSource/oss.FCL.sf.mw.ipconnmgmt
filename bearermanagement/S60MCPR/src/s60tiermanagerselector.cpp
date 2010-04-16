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
* Description: S60 IP TM selector implementation
*
*/

/**
@file s60tiermanagerselector.cpp
S60 IP TM selector implementation
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <commsdattypesv1_1.h> // CommsDat
#include <es_connpref.h>    //TConnIdList
#include <commdbconnpref.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypeinfov1_1_internal.h>
#include <commsdattypesv1_1_partner.h> 
#endif

#include "s60tiermanagerselector.h"
#include "s60tiermanagerfactory.h"
#include "s60mcpr.h"

using namespace ESock;
using namespace CommsDat;

// -----------------------------------------------------------------------------
// IpProto Tier Manager's UID.
// Since meaning of the selection policy id changes with
// the legacy flavor it is difficult to derive the next layer
// tier's id dynamically when supporting all of the legacy scenarios.
// At the same time it is known that for all of these legacy
// setups this is and always will be the only next layer tier manager.
// -----------------------------------------------------------------------------
static const TUid KLegacyNextLayerTierId = { 0x10281DF0 }; //CIPProtoTierManagerFactory::iUid

#ifdef __CFLOG_ACTIVE
#define KS60TierMgrTag KESockMetaConnectionTag
_LIT8(KS60TierMgrSubTag, "netmcpr");
#endif

//Panic codes
#ifdef _DEBUG
_LIT(KS60SelectorPanic, "S60TMSelector");
enum TS60SelectorPanic
    {
    EExpectedAccessPointAwareSystem = 0,     //
    EUnExpectedSelectionPreferences = 1,
    };
#endif // _DEBUG

// -----------------------------------------------------------------------------
// CSimpleS60SelectorBase::CSimpleS60SelectorBase
// -----------------------------------------------------------------------------
//
CSimpleS60SelectorBase::CSimpleS60SelectorBase( const Meta::SMetaData& aSelectionPreferences )
    :    ASimpleSelectorBase( aSelectionPreferences )
    {
    }

// -----------------------------------------------------------------------------
// CSimpleS60SelectorBase::~CSimpleS60SelectorBase
// -----------------------------------------------------------------------------
//
CSimpleS60SelectorBase::~CSimpleS60SelectorBase()
    {
    }
    
// -----------------------------------------------------------------------------
// CSimpleS60SelectorBase::SelectL
// -----------------------------------------------------------------------------
//
void CSimpleS60SelectorBase::SelectL( ISelectionNotify& aSelectionNotify )
    {
    ASSERT(iDbs);
    ASSERT(iTierRecord);

    // Default access point is required as it will point the correct node UIDs for the next layer.
    iDefaultAp = iTierRecord->iDefaultAccessPoint;
    //Since we do not understand the preferences, we create a provider based on a default Access Point,
    //like in the implicit case, but we also pass the preferences down.
    __CFLOG_VAR((KS60TierMgrTag, KS60TierMgrSubTag, _L8("S60MCPR::CSimpleS60SelectorBase %08x::\tSelectL() Using Default AP:%d"),this,iDefaultAp));

    //Legacy Attach -case handling is in NetMCPR and IPProtoTM
    // 399 Attach is not supported
    const TConnPref& prefs = iSelectionPrefs.Prefs();
    iSubSessionUniqueId = iSelectionPrefs.SubSessionUniqueId();
    
    switch ( prefs.ExtensionId() )
        {
        case TConnPref::EConnPrefCommDbMulti:
            {
            // Sanity check.
            TCommDbMultiConnPref& multiPrefs = TCommDbMultiConnPref::Cast( prefs );
            TCommDbConnPref connpref;
            TInt error = multiPrefs.GetPreference( 1, connpref );
            // if bad connpref
            if ( error != KErrNone )
                {
                User::Leave( error );
                }
            //else ok. MPM handles the TConnPref parsing.
            break;
            }
        case TConnPref::EConnPrefUnknown: // same as IsEmpty() == ETrue
        case TConnPref::EConnPrefCommDb:
        case TConnPref::EConnPrefSnap:
            {
            // Supported. MPM handles the parsing.
            break;
            }
        case TConnPref::EConnPrefIdList:
        case TConnPref::EConnPrefSip:
        case TConnPref::EConnPrefProviderInfo:
        default:
            {
            // Unsupported
            User::Leave( KErrNotSupported );
            break;
            }
        }
    // We use default AP always. If some variation is needed in APs, then
    // this must be changed to use specific APs instead of the default one.
    aSelectionNotify.SelectComplete( this, FindOrCreateProviderL( iDefaultAp ));
    aSelectionNotify.SelectComplete( this, NULL );
    }
    
// -----------------------------------------------------------------------------
// CSimpleS60SelectorBase::FindOrCreateProviderL
// -----------------------------------------------------------------------------
//
CMetaConnectionProviderBase* CSimpleS60SelectorBase::FindOrCreateProviderL( TUint aAccessPoint )
    {
    ASSERT(aAccessPoint); //Should not be 0 now.

    //Find factory
    TUid mCprUid = TierManagerUtils::ReadMCprUidL( aAccessPoint, *iDbs );
    CMetaConnectionProviderFactoryBase* factory = 
        static_cast<CMetaConnectionProviderFactoryBase*>(iMetaContainer.FindOrCreateFactoryL( mCprUid ));

    //Create the provider
    TUid tierId = TUid::Uid( iTierRecord->iRecordTag );
	TProviderInfo providerInfo( tierId, aAccessPoint );
	TUid tierImplUid = TierManagerUtils::MapTierIdtoTierImplIdL(tierId,*iDbs);
    TMetaConnectionFactoryQuery query( providerInfo, tierImplUid );
    
    //We are not looking for an existing provider, we always create a new one.
    //This is the legacy selection (can also be the legacy attach, one layer deep selection)
    CMetaConnectionProviderBase* provider = static_cast<CMetaConnectionProviderBase*>( factory->CreateObjectL( query ));
    CS60MetaConnectionProvider* prov = static_cast<CS60MetaConnectionProvider*>( provider );
    
    CleanupStack::PushL( provider );
    
    // Provision selection preferences to S60 NetMCPR.
    prov->SetSelectionPrefs( iSelectionPrefs );
    prov->SetSubSessionUniqueId( iSubSessionUniqueId );

    // Change the provider info so that this provider is never found by other selections
    const TProviderInfo& pi = provider->ProviderInfo();
    provider->SetProviderInfo( TProviderInfo( pi.TierId(), pi.APId(), provider )); //This provider will never be found
    
    // Override the selection policy as we always do for legacy prefs
    ASSERT(provider->AccessPointConfig().FindExtension(TOverridenSelectionPrefsExt::TypeId())==NULL);

	RMetaExtensionContainer mec;
	mec.Open(provider->AccessPointConfig());
	CleanupClosePushL(mec);
	
    TOverridenSelectionPrefsExt* ext = new (ELeave) TOverridenSelectionPrefsExt( KLegacyNextLayerTierId, iSelectionPrefs );
    CleanupStack::PushL( ext );
    mec.AppendExtensionL( ext ); //The ownership of the extension goes to AccessPointConfig now.
    CleanupStack::Pop( ext );

	provider->AccessPointConfig().Close();
	provider->AccessPointConfig().Open(mec);
	CleanupStack::PopAndDestroy(&mec);
	
    CleanupStack::Pop( provider );
    provider->IncrementBlockingDestroy();
    return provider;
    }

// -----------------------------------------------------------------------------
// TS60ProviderSelectorFactory::NewSelectorL
// -----------------------------------------------------------------------------
//
MProviderSelector* TS60ProviderSelectorFactory::NewSelectorL(const Meta::SMetaData& aSelectionPreferences)
    {
    __CFLOG_VAR((KS60TierMgrTag, KS60TierMgrSubTag, _L8("S60MCPR::CS60ProviderSelectorBase::\tNewSelectorL()")));
    ASSERT(aSelectionPreferences.IsTypeOf(TSelectionPrefs::TypeId()));
    //const ESock::TSelectionPrefs& sp = static_cast<const ESock::TSelectionPrefs&>(aSelectionPreferences);
    ASimpleSelectorBase* self = NULL;
    CBase* selfCleanupItem = NULL;
    CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);

    // Reveal hidden or private IAP records if a licensee has chosen to protect a record
    // using one of these flags - the API to do this is public so internal components
    // have to support the use of such records.
    dbs->SetAttributeMask( ECDHidden | ECDPrivate );

    // In all cases with S60 IP TM we want to use the S60 selector.
    // This means that it's pointless to allow any other selector to run besides CSimpleS60SelectorBase
    __CFLOG_VAR((KS60TierMgrTag, KS60TierMgrSubTag, _L8("S60MCPR::TIpProviderSelectorFactory::\tNewSelectorL() - Using S60 Simple Selector")));
    CSimpleS60SelectorBase* ipSelSelf = new (ELeave) CSimpleS60SelectorBase( aSelectionPreferences );
    selfCleanupItem = ipSelSelf;
    self = ipSelSelf;

    CleanupStack::PushL( selfCleanupItem );
    self->iTierRecord = TierManagerUtils::LoadTierRecordL( TUid::Uid(KAfInet), *dbs );
    __ASSERT_DEBUG(static_cast<TUint32>(self->iTierRecord->iDefaultAccessPoint)!=0,User::Panic(KS60SelectorPanic,EExpectedAccessPointAwareSystem));

    CleanupStack::Pop( selfCleanupItem );
    CleanupStack::Pop( dbs );
    ASSERT(self->iDbs==NULL);
    self->iDbs = dbs;
    return self;
    }

