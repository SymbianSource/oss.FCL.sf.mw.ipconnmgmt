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
#include <extendedconnpref.h>

#include "s60extendedtmselector.h"
#include "s60tiermanagerfactory.h"
#include "s60mcpr.h"

#include <comms-infras/esock_params_internal.h>


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

// -----------------------------------------------------------------------------
// CExtendedS60SelectorBase::CExtendedS60SelectorBase
// -----------------------------------------------------------------------------
//
CExtendedS60SelectorBase::CExtendedS60SelectorBase(  TUid aTierId, const ESock::RConnPrefList& aConnPrefList )
    :    ASimpleSelectorBase( ),
         iDefaultAp( 0 ),
         iConnPrefList( aConnPrefList ),
         iTierId( aTierId ),
         iSubSessionUniqueId( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CExtendedS60SelectorBase::~CExtendedS60SelectorBase
// -----------------------------------------------------------------------------
//
CExtendedS60SelectorBase::~CExtendedS60SelectorBase()
    {
    }
        
// -----------------------------------------------------------------------------
// CExtendedS60SelectorBase::SelectL
// -----------------------------------------------------------------------------
//
void CExtendedS60SelectorBase::SelectL(ISelectionNotify& aSelectionNotify)
    {
    //This code doesn't need to be in a seperate selector. It could be moved to
    //a transition if the method FindOrCreateProviderL was re-created in
    //another file (it is currently inherited from ASimpleSelectorBase)

    CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_2);

    iTierRecord = TierManagerUtils::LoadTierRecordL(iTierId,*dbs);
    CleanupStack::Pop();
    iDbs = dbs;

    //__ASSERT_DEBUG(iDbs, User::Panic(KSpecAssert_ESockCrPrvTMSelC, 1));
    //__ASSERT_DEBUG(iTierRecord, User::Panic(KSpecAssert_ESockCrPrvTMSelC, 2));

    TUint accessPoint = 0;
    CCDAccessPointRecord* apRec;
    TUid tierId;
    TUint selected = 0;

    // Get sub session unique id
    ESock::RConnPrefList::TIter<TConnCSRPref> iterCSR = iConnPrefList.getIter<TConnCSRPref>();

    // If no prefs in list return error
    if (iterCSR.IsEnd())
        {
        User::Leave(KErrArgument);
        }
    
    //This iterator runs through all the instances of TConnCSRPref
    while(!iterCSR.IsEnd())
        {
        iSubSessionUniqueId = iterCSR->SubSessionUniqueId();
        iterCSR++;
        }
    
    //This iterator links to all the elements of TExtendedConnPref that exist in the list
    ESock::RConnPrefList::TIter<TExtendedConnPref> iterAP = iConnPrefList.getIter<TExtendedConnPref>();

    // If no prefs in list return error
    if (iterAP.IsEnd())
        {
        User::Leave(KErrArgument);
        }
    
    //This iterator runs through all the instances of TExtendedConnPref
    while(!iterAP.IsEnd())
        {
        accessPoint = iTierRecord->iDefaultAccessPoint;
        //The access point record that that access point represents is retrived
        apRec = TierManagerUtils::LoadAccessPointRecordL(accessPoint,*iDbs);
        tierId = TierManagerUtils::MapElementIdToTagId(apRec->iTier, *iDbs);

        delete apRec;
        // If the access point is related to this tier manager the provider
        // is created
        if(iTierId == tierId)
            {
            aSelectionNotify.SelectComplete(this, FindOrCreateProviderL(accessPoint));
            ++selected;
            iterAP++;
            // For now we accept only one instance of TExtendedConnPref 
            break;
            }
        else
            {
            LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t AP %d not in tier %08x"), this, accessPoint, iTierId) );
            iterAP++;
            }
        }

    if (selected == 0)
        {
        LOG_DETAILED( ESockLog::Printf(KCoreTMgrSelSupSubTag, _L("CCoreProviderSelectorSuper %08x:\t No APs selected [Tier: %08x]"), this, iTierId) );
        User::Leave(KErrArgument);
        }
    
    aSelectionNotify.SelectComplete(this, NULL);
    return;
    }

// -----------------------------------------------------------------------------
// CExtendedS60SelectorBase::FindOrCreateProviderL
// -----------------------------------------------------------------------------
//
CMetaConnectionProviderBase* CExtendedS60SelectorBase::FindOrCreateProviderL( TUint aAccessPoint )
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
    prov->SetConnPrefList( iConnPrefList );
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


