/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of plugin base class
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <cmmanager.rsg>
#include <es_sock.h>    // RSocketServ, RConnection
#include <in_sock.h>    // KAfInet
#include <es_enum.h>    // TConnectionInfo
#include <StringLoader.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <sysutil.h> 
#include <cmpluginbase.h>

#include <cmmanager.h>
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include <cmpluginbase.h>
#include <cmpsettingsconsts.h>
#include "cmlogger.h"
#include <cmcommonui.h>

using namespace CMManager;
using namespace CommsDat;


// ---------------------------------------------------------------------------
// CCmPluginBase::CCmPluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBase::CCmPluginBase( CCmPluginBaseEng* plugin )
    : iPlugin( plugin )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::~CCmPluginBase
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBase::~CCmPluginBase()
    {
    if (iPlugin)
        {
        TRAP_IGNORE( CmMgr().RemoveCMFromPoolL(this) );
        }
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::ConstructL()
    {
    iPlugin->ConstructL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::ConstructL
// ---------------------------------------------------------------------------
//
 CCmPluginBase* CCmPluginBase::CreateInstanceL( TCmPluginInitParam& aInitParam )
    {
    CCmPluginBaseEng* plugin = iPlugin->CreateInstanceL(aInitParam);
	CleanupStack::PushL( plugin );
    CCmPluginBase* connMethod = new (ELeave) CCmPluginBase(plugin);     
    CleanupStack::Pop( plugin );  // plugin        
    return connMethod;
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::RunSettingsL
// ---------------------------------------------------------------------------
//
TInt CCmPluginBase::RunSettingsL()
    {
    return iPlugin->RunSettingsL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::InitializeWithUiL
// ---------------------------------------------------------------------------
//
TInt CCmPluginBase::InitializeWithUiL(TBool aManuallyConfigure)
    {
    return iPlugin->InitializeWithUiL(aManuallyConfigure);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmPluginBase::CanHandleIapIdL(TUint32 aIapId) const
    {
    return iPlugin->CanHandleIapIdL(aIapId);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmPluginBase::CanHandleIapIdL(CommsDat::CCDIAPRecord* aIapRecord) const
    {
    return iPlugin->CanHandleIapIdL(aIapRecord);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::AdditionalReset
// ---------------------------------------------------------------------------
//
void CCmPluginBase::AdditionalReset()
    {
    iPlugin->AdditionalReset();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmPluginBase::ServiceRecordIdLC(HBufC* &aServiceName, 
                                       TUint32& aRecordId)
    {
    iPlugin->ServiceRecordIdLC(aServiceName, aRecordId);
    }


// ---------------------------------------------------------------------------
// CCmPluginBase::BearerRecordIdLC
// ---------------------------------------------------------------------------
//
void CCmPluginBase::BearerRecordIdLC(HBufC* &aBearerName, 
                                       TUint32& aRecordId)
    {
    iPlugin->BearerRecordIdLC(aBearerName, aRecordId);
    }


// ---------------------------------------------------------------------------
// CCmPluginBase::PrepareToDeleteRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::PrepareToDeleteRecordsL()
    {
    iPlugin->PrepareToDeleteRecordsL();
    }


// ---------------------------------------------------------------------------
// CCmPluginBase::DeleteAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::DeleteAdditionalRecordsL()
    {
    iPlugin->DeleteAdditionalRecordsL();
    }


// ---------------------------------------------------------------------------
// CCmPluginBase::CreateNewServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::CreateNewServiceRecordL()
    {
    iPlugin->CreateNewServiceRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::
// ---------------------------------------------------------------------------
//
void CCmPluginBase::CreateAdditionalRecordsL()
    {
    iPlugin->CreateAdditionalRecordsL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::PrepareToCopyDataL(CCmPluginBaseEng& aDestInst) const
    {
    iPlugin->PrepareToCopyDataL(aDestInst);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CopyAdditionalDataL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::CopyAdditionalDataL(CCmPluginBaseEng& aDestInst) const
    {
    iPlugin->CopyAdditionalDataL(aDestInst);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadAdditionalRecordsL()
    {
    iPlugin->LoadAdditionalRecordsL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateAdditionalRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateAdditionalRecordsL()
    {
    iPlugin->UpdateAdditionalRecordsL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::IsLinkedToIap
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmPluginBase::IsLinkedToIap( TUint32 aIapId )
    {
    return iPlugin->IsLinkedToIap( aIapId );
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::IsLinkedToIap
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmPluginBase::IsLinkedToSnap( TUint32 aSnapId )
    {
    return iPlugin->IsLinkedToSnap( aSnapId );
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::GetIntAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 
            CCmPluginBase::GetIntAttributeL( const TUint32 aAttribute ) const
    {
    return iPlugin->GetIntAttributeL(aAttribute);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::GetBoolAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool 
        CCmPluginBase::GetBoolAttributeL( const TUint32 aAttribute ) const
    {
    return iPlugin->GetBoolAttributeL(aAttribute);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::GetStringAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* 
    CCmPluginBase::GetStringAttributeL( const TUint32 aAttribute ) const
    {
    return iPlugin->GetStringAttributeL(aAttribute);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::GetString8AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* 
    CCmPluginBase::GetString8AttributeL( const TUint32 aAttribute ) const
    {
    return iPlugin->GetString8AttributeL(aAttribute);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::SetIntAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::SetIntAttributeL( const TUint32 aAttribute, 
                                                  TUint32 aValue )
    {
    iPlugin->SetIntAttributeL(aAttribute, aValue);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::SetBoolAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::SetBoolAttributeL( const TUint32 aAttribute, 
                                                   TBool aValue )
    {
    iPlugin->SetBoolAttributeL(aAttribute, aValue);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::SetStringAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::SetStringAttributeL( const TUint32 aAttribute, 
                                                     const TDesC16& aValue )
    {
    iPlugin->SetStringAttributeL(aAttribute, aValue);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::SetString8AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::SetString8AttributeL( const TUint32 aAttribute, 
                                                      const TDesC8& aValue )
    {
    iPlugin->SetString8AttributeL(aAttribute, aValue);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::RestoreAttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::RestoreAttributeL( const TUint32 aAttribute )
    {
    iPlugin->RestoreAttributeL(aAttribute);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::Destination
// ---------------------------------------------------------------------------
//
EXPORT_C CCmDestinationImpl* CCmPluginBase::Destination() const
    {
    return iPlugin->Destination();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::UpdateL()
    {
    iPlugin->UpdateL();
    iPlugin->SetIdValidity(ETrue);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::NumOfConnMethodReferencesL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmPluginBase::NumOfConnMethodReferencesL()
    {
    return iPlugin->NumOfConnMethodReferencesL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::Session
// ---------------------------------------------------------------------------
//
EXPORT_C CMDBSession& CCmPluginBase::Session()const
    {
    return iPlugin->Session();
    }

// ---------------------------------------------------------------------------
// Delete this connection methond.
// Record ids are checked to be sure that we delete only records
// that were loaded before.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCmPluginBase::DeleteL( TBool aForced,
                                          TBool aOneRefAllowed )
    {
    return iPlugin->DeleteL(aForced, aOneRefAllowed);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::LoadL( TUint32 aIapId )
    {
    iPlugin->LoadL(aIapId);
    CmMgr().ConnMethodUpdatedL( this->Plugin() );
    iPlugin->SetIdValidity(ETrue);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::ReLoadL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::ReLoadL()
    {
    TUint32 cmID = iPlugin->GetIntAttributeL( ECmId );    
    TUint32 refCounter = iPlugin->iRefCounter;
    
    iPlugin->iRefCounter = 1;
    CCmManagerImpl* mgr = &CmMgr();
    mgr->RemoveCMFromPoolL(iPlugin);
    iPlugin = NULL;
    CCmPluginBase* cm = mgr->GetConnectionMethodL( cmID );
    iPlugin = cm->iPlugin;
    cm->iPlugin = NULL;
    delete cm;
    iPlugin->SetIdValidity(ETrue);
    iPlugin->iRefCounter = refCounter;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::CreateNewL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::CreateNewL()
    {
    iPlugin->CreateNewL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CreateCopyL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmPluginBase* CCmPluginBase::CreateCopyL() const
    {
    CCmPluginBaseEng* plugin = iPlugin->CreateCopyL();
	CleanupStack::PushL( plugin );    
    CCmPluginBase* connMethod = CmMgr().CreateConnectionMethodL(plugin);
    CleanupStack::Pop( plugin );      
    return connMethod;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::AddResourceFileL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::AddResourceFileL( const TDesC& aName )
    {
    iPlugin->AddResourceFileL(aName);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::RemoveResourceFile
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::RemoveResourceFile( const TDesC& aName )
    {
    iPlugin->RemoveResourceFile(aName);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::AllocReadL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CCmPluginBase::AllocReadL( TInt aResourceId ) const
    {
    return iPlugin->AllocReadL( aResourceId );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::OpenTransactionLC
// ---------------------------------------------------------------------------
//
void CCmPluginBase::OpenTransactionLC()
    {
    iPlugin->OpenTransactionLC();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::CommitTransactionL( TInt aError )
    {
    iPlugin->CommitTransactionL(aError);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::PrepareToLoadRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::DoLoadL( TUint32 aIapId )
    {
    iPlugin->CommitTransactionL(aIapId);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::PrepareToLoadRecordsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::PrepareToLoadRecordsL()
    {
    iPlugin->PrepareToLoadRecordsL();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::LoadIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadIAPRecordL( TUint32 aIapId )
    {
    iPlugin->LoadIAPRecordL(aIapId);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadProxySettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadProxySettingL()
    {
    iPlugin->LoadProxySettingL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadServiceSettingL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::LoadServiceSettingL()
    {
    iPlugin->LoadServiceSettingL();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::LoadChargeCardSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadChargeCardSettingL( TUint32 aRecordId )
    {
    iPlugin->LoadChargeCardSettingL(aRecordId);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadNetworkSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadNetworkSettingL()
    {
    iPlugin->LoadNetworkSettingL();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::LoadLocationSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadLocationSettingL()
    {
    iPlugin->LoadLocationSettingL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::PrepareToUpdateRecordsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::PrepareToUpdateRecordsL()
    {
    iPlugin->PrepareToUpdateRecordsL();
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateIAPRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateIAPRecordL()
    {
    iPlugin->UpdateIAPRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateProxyRecordL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::UpdateProxyRecordL()
    {
    iPlugin->UpdateProxyRecordL();
    }


// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateSeamlessnessRecordL()
    {
    iPlugin->UpdateSeamlessnessRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateChargeCardRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateChargeCardRecordL()
    {
    iPlugin->UpdateChargeCardRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::UpdateServiceRecordL()
    {
    iPlugin->UpdateServiceRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateNetworkRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateNetworkRecordL()
    {
    iPlugin->UpdateNetworkRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateLocationRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateLocationRecordL()
    {
    iPlugin->UpdateLocationRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateConnPrefSettingL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateConnPrefSettingL()
    {
    iPlugin->UpdateConnPrefSettingL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::EnableProxyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::EnableProxyL( TBool aEnable )
    {
    iPlugin->EnableProxyL(aEnable);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::EnableChargeCardL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::EnableChargeCardL( TBool aEnable )
    {
    iPlugin->EnableChargeCardL(aEnable);
    }
    
// ---------------------------------------------------------------------------
// CCmPluginBase::EnableLocationL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmPluginBase::EnableLocationL( TBool aEnable )
    {
    iPlugin->EnableLocationL(aEnable);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadWapRecordL()
    {
    iPlugin->LoadWapRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::LoadSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::LoadSeamlessnessRecordL()
    {
    iPlugin->LoadSeamlessnessRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::UpdateWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::UpdateWapRecordL()
    {
    iPlugin->UpdateWapRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::NewWapRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginBase::NewWapRecordL()
    {
    iPlugin->NewWapRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::NewSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBase::NewSeamlessnessRecordL( TBool aSetDef )
    {
    return iPlugin->NewSeamlessnessRecordL(aSetDef);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::SetAttribute
// ---------------------------------------------------------------------------
//
void CCmPluginBase::SetAttribute( CCDRecordBase* aRecord, 
                                     TUint32 aAttribute, 
                                     TBool aSet )
    {
    iPlugin->SetAttribute(aRecord, aAttribute, aSet);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CopyAttributes
// ---------------------------------------------------------------------------
//
void CCmPluginBase::CopyAttributes( CCDRecordBase* aSrcRecord, 
                                       CCDRecordBase* aDstRecord )
    {
    iPlugin->CopyAttributes(aSrcRecord, aDstRecord);
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::FindWAPRecordL
// ---------------------------------------------------------------------------
//
CCDWAPIPBearerRecord* CCmPluginBase::FindWAPRecordL()
    {
    return iPlugin->FindWAPRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::FindSeamlessnessRecordL
// ---------------------------------------------------------------------------
//
CCDIAPMetadataRecord* CCmPluginBase::FindSeamlessnessRecordL()
    {
    return iPlugin->FindSeamlessnessRecordL();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::IAPRecordElementId
// ---------------------------------------------------------------------------
TMDBElementId CCmPluginBase::IAPRecordElementId() const
    {
    return iPlugin->IAPRecordElementId();
    }
// ---------------------------------------------------------------------------
// CCmPluginBase::ParentDestination
// ---------------------------------------------------------------------------
EXPORT_C CCmDestinationImpl* CCmPluginBase::ParentDestination() const 
    { 
    return iPlugin->ParentDestination();
    }

// ---------------------------------------------------------------------------
// CCmPluginBase::CmMgr
// ---------------------------------------------------------------------------
EXPORT_C CCmManagerImpl& CCmPluginBase::CmMgr() const 
    { 
    return iPlugin->CmMgr();
    };


// ---------------------------------------------------------------------------
// CCmPluginBase::Plugin
// ---------------------------------------------------------------------------
void CCmPluginBase::SetPlugin(CCmPluginBaseEng* aPlugin) 
    { 
    iPlugin = aPlugin;
    };

// ---------------------------------------------------------------------------
// CCmPluginBase::Plugin
// ---------------------------------------------------------------------------
EXPORT_C CCmPluginBaseEng* CCmPluginBase::Plugin() const 
    { 
    return iPlugin;
    };
    
// -----------------------------------------------------------------------------
// CCmPluginBase::IncrementRefCounter
// -----------------------------------------------------------------------------
void CCmPluginBase::IncrementRefCounter()
    {
    iPlugin->IncrementRefCounter();
    }
// -----------------------------------------------------------------------------
// CCmPluginBase::DecrementRefCounter
// -----------------------------------------------------------------------------
void CCmPluginBase::DecrementRefCounter()
    {
    iPlugin->DecrementRefCounter();
    }
// -----------------------------------------------------------------------------
// CCmPluginBase::GetRefCounter
// -----------------------------------------------------------------------------
TInt CCmPluginBase::GetRefCounter()
    {
    return iPlugin->GetRefCounter();
    }
                                                                    
// -----------------------------------------------------------------------------
// CCmPluginBase::GetAdditionalUids
// -----------------------------------------------------------------------------
EXPORT_C void CCmPluginBase::GetAdditionalUids( RArray<TUint32>& aIapIds )
    {
    iPlugin->GetAdditionalUids( aIapIds );
    }

//-----------------------------------------------------------------------------
// CCmPluginBase::FilterPossibleParentsL
// -----------------------------------------------------------------------------
void CCmPluginBase::FilterPossibleParentsL( RArray<TUint32>& aDests )
    {
    iPlugin->FilterPossibleParentsL( aDests );
    }
