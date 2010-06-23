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
*      Implementation of VPN CmManager Plugin interface implementation 
*      
*
*/

 
// INCLUDE FILES
#include <commsdattypesv1_1.h>
#include <datamobilitycommsdattypes.h>
#include <cmconnectionmethoddef.h>
#include <cmpluginvpndef.h>
#include <featmgr.h>
#include <publicruntimeids.hrh>

#include "cmpluginvpn.h"
#include "cmmserverdefs.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmpluginvpnTraces.h"
#endif

using namespace CommsDat;
using namespace CMManager;



_LIT( KVpnVirtualBearerName, "vpnbearer" );
_LIT( KVpnVirtualBearerAgent, "vpnconnagt.agt" );
_LIT( KVpnVirtualBearerNif, "tunnelnif" );

static const TInt KVirtualIAPNextLayerRecordIndex = 0;

// constants needed for the virtualbearer table
static const TInt KNoTimeout = -1;
static const TUint32 KLastSessionTimeout = 3;

// ---------------------------------------------------------------------------
// CCmPluginVpn::NewL
// ---------------------------------------------------------------------------
//
CCmPluginVpn* CCmPluginVpn::NewL( TCmPluginInitParam* aInitParam )
	{	
    OstTraceFunctionEntry0( CCMPLUGINVPN_NEWL_ENTRY );
    
	CCmPluginVpn* self = new( ELeave ) CCmPluginVpn( aInitParam );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	OstTraceFunctionExit0( CCMPLUGINVPN_NEWL_EXIT );
	return self;		
	}

// ---------------------------------------------------------------------------
// CCmPluginVpn::CreateInstanceL
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginVpn::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
	{
    OstTraceFunctionEntry0( CCMPLUGINVPN_CREATEINSTANCEL_ENTRY );

    CCmPluginVpn* self = new( ELeave ) CCmPluginVpn( &aInitParam );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	OstTraceFunctionExit0( CCMPLUGINVPN_CREATEINSTANCEL_EXIT );
	return self;				
	}

// ---------------------------------------------------------------------------
// CCmPluginVpn::~CCmPluginVpn
// ---------------------------------------------------------------------------
//
CCmPluginVpn::~CCmPluginVpn()
	{
    OstTraceFunctionEntry0( CCMPLUGINVPN_CCMPLUGINVPN_ENTRY );
    
    ResetBearerRecords();
    
	OstTraceFunctionExit0( CCMPLUGINVPN_CCMPLUGINVPN_EXIT );
	}

// ---------------------------------------------------------------------------
// CCmPluginVpn::CCmPluginVpn
// ---------------------------------------------------------------------------
//
CCmPluginVpn::CCmPluginVpn( TCmPluginInitParam* aInitParam )
	: CCmPluginBaseEng( aInitParam ), iVirtualTableId(0), 
	  iBearerPriorityTableId(0), iVirtualIapNextLayerRecord(NULL)
	{
    OstTraceFunctionEntry0( DUP1_CCMPLUGINVPN_CCMPLUGINVPN_ENTRY );
    
    iBearerType = KPluginVPNBearerTypeUid;
	
    OstTraceFunctionExit0( DUP1_CCMPLUGINVPN_CCMPLUGINVPN_EXIT );
	}

// ---------------------------------------------------------------------------
// CCmPluginVpn::ConstructL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::ConstructL()
	{    
    OstTraceFunctionEntry0( CCMPLUGINVPN_CONSTRUCTL_ENTRY );
    
    if (!FeatureManager::FeatureSupported( KFeatureIdFfVpnClient ) )
        {   
        User::Leave( KErrNotSupported );
        }
    
    TRAP_IGNORE( iVirtualTableId = 
                        CCDVirtualIAPNextLayerRecord::TableIdL( iSession ) );       
    
    if( !iVirtualTableId )
        {
        iVirtualTableId = CCDVirtualIAPNextLayerRecord::CreateTableL( iSession );
        }
            
    TRAP_IGNORE( iBearerPriorityTableId =
            CCDGlobalBearerTypePriorizationRecord::TableIdL( iSession ) );

    if ( !iBearerPriorityTableId )
        {
        iBearerPriorityTableId =
                CCDGlobalBearerTypePriorizationRecord::CreateTableL( iSession );
        }
    
	CCmPluginBaseEng::ConstructL();
	    
	OstTraceFunctionExit0( CCMPLUGINVPN_CONSTRUCTL_EXIT );
	}

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::GetBearerInfoIntL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERINFOINTL_ENTRY );

    TUint32 retVal( 0 );
    switch ( aAttribute )
        {
        case ECmSeamlessnessLevel:
            {
            retVal = ESeamlessnessShowprogress;
            }
            break;
        case ECmBearerType:
            {             
            retVal = iBearerType;
            }
            break;
        case ECmCommsDBBearerType:
            {                         
            retVal = KCommDbBearerVirtual;
            }
            break;
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        case ECmDefaultUiPriority: //falls through
        case ECmDefaultPriority:
            {
            retVal = GetDefPriorityL( aAttribute );
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmPluginVpn::GetBearerInfoBoolL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERINFOBOOLL_ENTRY );
    
    TBool retVal( EFalse );

    switch ( aAttribute )
        {
        case ECmCoverage:
            {
            //This parameter is not actually in use for the VPN.
            //ConnMon component can make a proper coverage discovery for the VPN,
            //but CmManager just returns the default value.
            retVal = EFalse;
            }
            break;
        case ECmDestination:
            {
            retVal = EFalse;
            }
            break;
        case ECmBearerHasUi:
            {                
            retVal = EFalse;
            }
            break;
        case ECmIPv6Supported:
            {            
            retVal = EFalse;
            }
            break;
        case ECmVirtual:
            {
            retVal = ETrue;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }    
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginVpn::GetBearerInfoStringL( TUint32 /*aAttribute*/ ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERINFOSTRINGL_ENTRY );
    
    User::Leave( KErrNotSupported );
     
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERINFOSTRINGL_EXIT );
    
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerInfoString8L
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginVpn::GetBearerInfoString8L( TUint32 /*aAttribute*/ ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERINFOSTRING8L_ENTRY );

    User::Leave( KErrNotSupported );

    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERINFOSTRING8L_EXIT );
 
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmPluginVpn::CanHandleIapIdL( TUint32 aIapId ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_CANHANDLEIAPIDL_ENTRY );

    TBool retVal( EFalse );
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                            ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        
    CleanupStack::PushL( iapRecord );
    iapRecord->SetRecordId( aIapId );
    
    iapRecord->LoadL( iSession );
    retVal = CanHandleIapIdL( iapRecord );
    
    CleanupStack::PopAndDestroy( iapRecord );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }
 
// ---------------------------------------------------------------------------
// CCmPluginVpn::CanHandleIapIdL
// ---------------------------------------------------------------------------
//
TBool CCmPluginVpn::CanHandleIapIdL( CommsDat::CCDIAPRecord* aIapRecord ) const
    {
    OstTraceFunctionEntry0( DUP1_CCMPLUGINVPN_CANHANDLEIAPIDL_ENTRY );
    TBool retVal( EFalse );
        
    if( TPtrC(aIapRecord->iServiceType) == TPtrC(KCDTypeNameVPNService) ||
        TPtrC(aIapRecord->iBearerType) == TPtrC(KCDTypeNameVirtualBearer) )
        {
        // Further comparision is to find exact info that the IAP can handle by this plugin
        CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
                new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>
                                                   (KCDTIdVirtualBearerRecord);
        CleanupStack::PushL( bearersRS );

        CCDVirtualBearerRecord* bearerRecord = 
                static_cast<CCDVirtualBearerRecord *>
                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));

        CleanupStack::PushL( bearerRecord );
    
        // Find entries used "vpnconnagt.agt" as agent from Table VirtualBearer
        bearerRecord->iBearerAgent.SetL( KVpnVirtualBearerAgent );
    
        bearersRS->iRecords.AppendL( bearerRecord );

        CleanupStack::Pop( bearerRecord );
        bearerRecord = NULL;
        
        if ( bearersRS->FindL( iSession ) )
            {
            TUint32 recordId = (*bearersRS)[0]->RecordId();
            TPtrC bearerName( (*bearersRS)[0]->iRecordName.GetL() );
        
            // Further comparing record ID referred to by this VPN IAP with entry ID in table VirtualBearer
            // And also comparing bear name with our expected one "vpnbearer"
            if( recordId == aIapRecord->iBearer && 
                bearerName == TPtrC( KVpnVirtualBearerName ) )
                {
                retVal = ETrue;
                }
            }        
        CleanupStack::PopAndDestroy( bearersRS );
        }
    
    OstTraceFunctionExit0( DUP1_CCMPLUGINVPN_CANHANDLEIAPIDL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerTableIdsToBeObservedL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::GetBearerTableIdsToBeObservedL( RArray<TUint32>& aTableIdArray ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERTABLEIDSTOBEOBSERVEDL_ENTRY );
    
    aTableIdArray.AppendL( KCDTIdVirtualBearerRecord );
    aTableIdArray.AppendL( iVirtualTableId );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERTABLEIDSTOBEOBSERVEDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerSpecificRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::GetBearerSpecificRecordsL(RPointerArray<CommsDat::CCDRecordBase>& aRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERSPECIFICRECORDSL_ENTRY );
       
    __ASSERT_DEBUG( iVirtualIapNextLayerRecord != NULL, User::Invariant() );  
            
    CCDVirtualIAPNextLayerRecord* virtualIapNextLayerRecordCopy = 
                new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    CleanupStack::PushL( virtualIapNextLayerRecordCopy );
    
    TUint value = iVirtualIapNextLayerRecord->iIAP;
    virtualIapNextLayerRecordCopy->iIAP = value;
    
    value = iVirtualIapNextLayerRecord->iNextLayerSNAP;
    virtualIapNextLayerRecordCopy->iNextLayerSNAP = value;
    
    value = iVirtualIapNextLayerRecord->iNextLayerIAP;
    virtualIapNextLayerRecordCopy->iNextLayerIAP = value;
    
    virtualIapNextLayerRecordCopy->SetElementId( iVirtualIapNextLayerRecord->ElementId() );
    User::LeaveIfError( aRecordArray.Append( virtualIapNextLayerRecordCopy ) );
    CleanupStack::Pop( virtualIapNextLayerRecordCopy );    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERSPECIFICRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerIntAttributeL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::GetBearerIntAttributeL( 
        TUint32 aAttribute, 
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERINTATTRIBUTEL_ENTRY );
    
    CCDVPNServiceRecord* serviceRecord = static_cast<CCDVPNServiceRecord *>                
                                                        ( aGenRecordArray[KServiceRecordIndex] );
        
    TUint32 retVal( 0 );
    switch( aAttribute )
        {
        case ECmExtensionLevel:   
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        case ECmCommsDBBearerType:
            {
            TCommsDBBearerTechnology bearer = KCommDbBearerVirtual; 
            retVal = bearer;
            }
            break;
        case ECmNextLayerIapId:
        case EVpnIapId:
            {            
            TUint recordId = serviceRecord->iServiceIAP;//TODO, can't use this straight? link containd the ID.
            if (recordId != 0)
                {
                CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                                        ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
                
                CleanupStack::PushL( iapRecord );
                iapRecord->SetRecordId( recordId );
                iapRecord->LoadL( iSession );
                retVal = iapRecord->RecordId();
                CleanupStack::PopAndDestroy( iapRecord );                             
                }
            }
            break;
        case EVpnNetworkId:
        case ECmNextLayerSNAPId:
            {
            TUint recordId = serviceRecord->iServiceSNAP;
            if ( recordId != 0 )
                {
                CCDAccessPointRecord* accessPointRecord = static_cast<CCDAccessPointRecord*>(
                        CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
                CleanupStack::PushL( accessPointRecord );
                accessPointRecord->SetRecordId( recordId );
                accessPointRecord->LoadL( iSession );
                retVal = accessPointRecord->iRecordTag;
                CleanupStack::PopAndDestroy( accessPointRecord );

                if ( aAttribute == EVpnNetworkId )
                    {
                    __ASSERT_DEBUG( retVal >= KCmmDestIdIntervalMin, User::Invariant() );//TODO, replace with iCustomSelectionPolicy
                    retVal -= KCmmDestIdIntervalMin;
                    }
                }
            }
            break;
        default:
            {
            retVal = GetBearerInfoIntL( aAttribute );
            }
            break;
        }
    
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERINTATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerBoolAttributeL
// ---------------------------------------------------------------------------
//
TBool CCmPluginVpn::GetBearerBoolAttributeL( 
        TUint32 aAttribute, 
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERBOOLATTRIBUTEL_ENTRY );
    //We don't have IAP specific bool attributes, but the bearer specific
    //attributes may also be asked this way...
    TBool retVal = GetBearerInfoBoolL( aAttribute );              
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERBOOLATTRIBUTEL_EXIT );
    return retVal;
    }
  
// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerStringAttributeL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginVpn::GetBearerStringAttributeL( 
        TUint32 aAttribute, 
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERSTRINGATTRIBUTEL_ENTRY );
    
    CCDVPNServiceRecord* serviceRecord = static_cast<CCDVPNServiceRecord *>                
                                                        ( aGenRecordArray[KServiceRecordIndex] );

    HBufC* retVal =  NULL;
    switch( aAttribute )
        {
        case EVpnServicePolicy:
            {
            retVal = TPtrC( serviceRecord->iServicePolicy ).AllocL();
            }
            break;
        default:
            {
            retVal = GetBearerInfoStringL( aAttribute );
            }
            break;
        }
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERSTRINGATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetBearerString8AttributeL
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginVpn::GetBearerString8AttributeL( 
        TUint32 aAttribute, 
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETBEARERSTRING8ATTRIBUTEL_ENTRY );
    
    HBufC8* retVal = GetBearerInfoString8L( aAttribute );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETBEARERSTRING8ATTRIBUTEL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::SetBearerIntAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::SetBearerIntAttributeL( 
        TUint32 aAttribute, 
        TUint32 aValue, 
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_SETBEARERINTATTRIBUTEL_ENTRY );

    CCDVPNServiceRecord* serviceRecord = static_cast<CCDVPNServiceRecord *>                
                                                    (aGenRecordArray[KServiceRecordIndex]);

    CCDVirtualIAPNextLayerRecord* virtualIapNextLayerRecord = static_cast<CCDVirtualIAPNextLayerRecord*>
                                                        (aBearerSpecRecordArray[KVirtualIAPNextLayerRecordIndex]);

    
    switch(aAttribute)
        {
        case ECmNextLayerIapId: //falls through
        case EVpnIapId:
            {
            if( aValue == 0 || aValue >= (KCmmConnMethodIdIntervalMax - 2) )
                {
                User::Leave( KErrArgument );
                }            
            
            //Tries to find corresponding IAP record.
            CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                                    ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
                
            CleanupStack::PushL( iapRecord );
            iapRecord->SetRecordId( aValue );            
            TRAPD(err, iapRecord->LoadL( iSession ));
            if ( err == KErrNotFound )
                {
                User::Leave( KErrArgument );
                }
            User::LeaveIfError( err );
            
            serviceRecord->iServiceIAP = iapRecord->RecordId();
            serviceRecord->iServiceNetwork = 0;
            serviceRecord->iServiceSNAP = 0;
            virtualIapNextLayerRecord->iNextLayerIAP.SetL( aValue );
            virtualIapNextLayerRecord->iNextLayerSNAP = 0;
            CleanupStack::PopAndDestroy( iapRecord );
            }
            break;
        case EVpnNetworkId: //falls through
            {
            if ( aValue == 0 || aValue > (KCmmDestIdIntervalLegacyMax - 2) )
                {
                User::Leave( KErrArgument );
                }
            }
            // Fallthrough intended.
        case ECmNextLayerSNAPId:
            {
            if ( aValue <= ( KCmmDestIdIntervalLegacyMax - 2 ) )
                {
                aValue += KCmmDestIdIntervalMin;
                }

            if ( aValue <= KCmmDestIdIntervalMin ||
                    aValue > KCmmDestIdIntervalMax - 2 )
                {
                User::Leave( KErrArgument );
                }

            CCDAccessPointRecord* accessPointRecord = static_cast<CCDAccessPointRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
            CleanupStack::PushL( accessPointRecord );
            accessPointRecord->iRecordTag = aValue;
            if ( accessPointRecord->FindL( iSession ) )
                {
                serviceRecord->iServiceIAP = 0;
                serviceRecord->iServiceNetwork = 0;
                serviceRecord->iServiceSNAP = accessPointRecord->RecordId();
                virtualIapNextLayerRecord->iNextLayerIAP = 0;
                virtualIapNextLayerRecord->iNextLayerSNAP.SetL( aValue );
                }
            else
                {
                User::Leave( KErrArgument );
                }
            CleanupStack::PopAndDestroy( accessPointRecord );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SETBEARERINTATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::SetBearerBoolAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::SetBearerBoolAttributeL( 
        TUint32 /*aAttribute*/, 
        TBool /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_SETBEARERBOOLATTRIBUTEL_ENTRY );
    
    User::Leave( KErrNotSupported );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SETBEARERBOOLATTRIBUTEL_EXIT );
    }
  
// ---------------------------------------------------------------------------
// CCmPluginVpn::SetBearerStringAttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::SetBearerStringAttributeL( 
        TUint32 aAttribute, 
        const TDesC16& aValue,
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_SETBEARERSTRINGATTRIBUTEL_ENTRY );

    CCDVPNServiceRecord* serviceRecord = static_cast<CCDVPNServiceRecord *>                
                                                    ( aGenRecordArray[KServiceRecordIndex] );

    switch(aAttribute)
        {
        case EVpnServicePolicyName:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        case EVpnServicePolicy:
            {
            serviceRecord->iServicePolicy.SetL( aValue );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }    
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SETBEARERSTRINGATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::SetBearerString8AttributeL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::SetBearerString8AttributeL( 
        TUint32 /*aAttribute*/, 
        const TDesC8& /*aValue*/,                                                    
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_SETBEARERSTRING8ATTRIBUTEL_ENTRY );

    User::Leave( KErrNotSupported );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SETBEARERSTRING8ATTRIBUTEL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::PreparePluginToLoadRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::PreparePluginToLoadRecordsL()
    {    
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::PrepareToCopyDataL( CCmPluginBaseEng* /*aCopyInstance*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::PreparePluginToUpdateRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::PreparePluginToUpdateRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::PrepareToDeleteRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::PrepareToDeleteRecordsL()
    {    
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::LoadServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::LoadServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_LOADSERVICERECORDL_ENTRY );
    
    __ASSERT_DEBUG( iServiceRecord == NULL, User::Invariant() );
    
    if( TPtrC( KCDTypeNameVPNService ) == iIapRecord->iServiceType )
        {
        iServiceRecord = static_cast<CCDVPNServiceRecord *>
                    ( CCDRecordBase::RecordFactoryL(KCDTIdVPNServiceRecord) );
        iServiceRecord->SetRecordId( iIapRecord->iService );
        iServiceRecord->LoadL( iSession );        
        }
    else
        // this IAP service is not supported by this plugin.
        {
        User::Leave( KErrNotSupported );
        }    
    OstTraceFunctionExit0( CCMPLUGINVPN_LOADSERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CreateServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::CreateServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_CREATESERVICERECORDL_ENTRY );
    
    delete iServiceRecord; 
    iServiceRecord = NULL;
    
    iServiceRecord = static_cast<CCDVPNServiceRecord *>
                    ( CCDRecordBase::RecordFactoryL( KCDTIdVPNServiceRecord ) );
    
    ServiceRecord().iServicePolicy.SetL( KNullDesC );
    ServiceRecord().iServiceIAP = 0;
    ServiceRecord().iServiceNetwork = 0;
    ServiceRecord().iServiceSNAP = 0;
    
    OstTraceFunctionExit0( CCMPLUGINVPN_CREATESERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::UpdateServiceRecordL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::UpdateServiceRecordL( 
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_UPDATESERVICERECORDL_ENTRY );
        
    // Delete the original record and create a copy from the parameter
    delete iServiceRecord;
    iServiceRecord = NULL;

    CCDVPNServiceRecord* vpnServiceRecordFrom =
            static_cast<CCDVPNServiceRecord *>( aGenRecordArray[KServiceRecordIndex] );

    
    iServiceRecord = static_cast<CCDVPNServiceRecord*>
                                  ( CCDRecordBase::CreateCopyRecordL( *vpnServiceRecordFrom ) );
    iServiceRecord->SetElementId( vpnServiceRecordFrom->ElementId() );
    
    if ( !iServiceRecord->RecordId() )
        {
        iServiceRecord->SetRecordId( KCDNewRecordRequest );
        iServiceRecord->StoreL( iSession );
        
        vpnServiceRecordFrom->SetElementId( ServiceRecord().ElementId() );
        }
    else
        {
        iServiceRecord->ModifyL( iSession );
        }    
    
    OstTraceFunctionExit0( CCMPLUGINVPN_UPDATESERVICERECORDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CopyServiceRecordL
// ---------------------------------------------------------------------------
//
CommsDat::CCDRecordBase* CCmPluginVpn::CopyServiceRecordL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_COPYSERVICERECORDL_ENTRY );
    
    __ASSERT_DEBUG( iServiceRecord != NULL, User::Invariant());    
    CCDRecordBase* serviceRecord = static_cast<CCDVPNServiceRecord*>
                                  ( CCDRecordBase::CreateCopyRecordL( *iServiceRecord ) );     
    OstTraceFunctionExit0( CCMPLUGINVPN_COPYSERVICERECORDL_EXIT );
    return serviceRecord;    
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecordId
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::ServiceRecordId() const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_SERVICERECORDID_ENTRY );
    
    TUint32 retVal = ServiceRecord().RecordId();
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SERVICERECORDID_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecordNameLC
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::ServiceRecordNameLC( HBufC* &aServiceName )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_SERVICERECORDNAMELC_ENTRY );    
    aServiceName = TPtrC( KCDTypeNameVPNService ).AllocLC();      
    OstTraceFunctionExit0( CCMPLUGINVPN_SERVICERECORDNAMELC_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::LoadBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::LoadBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_LOADBEARERRECORDSL_ENTRY );
    
    delete iVirtualIapNextLayerRecord;
    iVirtualIapNextLayerRecord = NULL;
            
    CMDBRecordSet<CCDVirtualIAPNextLayerRecord>* virtualRS = 
              new(ELeave) CMDBRecordSet<CCDVirtualIAPNextLayerRecord>( iVirtualTableId );
    CleanupStack::PushL( virtualRS );
    
    CCDVirtualIAPNextLayerRecord* record = 
                            new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    
    record->iIAP = iIapRecord->RecordId();
    CleanupStack::PushL( record );
    virtualRS->iRecords.AppendL( record );
    CleanupStack::Pop( record );
    record = NULL;
    
    if( virtualRS->FindL( iSession ) )
        {
        iVirtualIapNextLayerRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
        iVirtualIapNextLayerRecord->SetRecordId( (*virtualRS)[0]->RecordId() );        
        iVirtualIapNextLayerRecord->LoadL( iSession );        
        }
    else
        {        
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( virtualRS );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_LOADBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CreateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::CreateBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_CREATEBEARERRECORDSL_ENTRY );

    delete iVirtualIapNextLayerRecord;
    iVirtualIapNextLayerRecord = NULL;
    
    iVirtualIapNextLayerRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );

    OstTraceFunctionExit0( CCMPLUGINVPN_CREATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::UpdateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::UpdateBearerRecordsL( 
        RPointerArray<CommsDat::CCDRecordBase>& aGenRecordArray,
        RPointerArray<CommsDat::CCDRecordBase>& aBearerSpecRecordArray )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_UPDATEBEARERRECORDSL_ENTRY );
        
    delete iVirtualIapNextLayerRecord;
    iVirtualIapNextLayerRecord = NULL;
    
    iVirtualIapNextLayerRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    
    CCDVirtualIAPNextLayerRecord* virtualIAPNextLayerRecordFrom = 
            static_cast<CCDVirtualIAPNextLayerRecord*>(aBearerSpecRecordArray[KVirtualIAPNextLayerRecordIndex]);
    
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                            ( aGenRecordArray[KIapRecordIndex] );

    
    TUint value = iapRecord->RecordId();
    iVirtualIapNextLayerRecord->iIAP = value;
    value = virtualIAPNextLayerRecordFrom->iNextLayerSNAP;
    iVirtualIapNextLayerRecord->iNextLayerSNAP = value;
    value = virtualIAPNextLayerRecordFrom->iNextLayerIAP;
    iVirtualIapNextLayerRecord->iNextLayerIAP = value;    
    iVirtualIapNextLayerRecord->SetElementId(virtualIAPNextLayerRecordFrom->ElementId());
    
    if ( !iVirtualIapNextLayerRecord->RecordId() )
        {
        iVirtualIapNextLayerRecord->SetRecordId( KCDNewRecordRequest );
        iVirtualIapNextLayerRecord->StoreL( iSession );
        virtualIAPNextLayerRecordFrom->SetElementId( iVirtualIapNextLayerRecord->ElementId() );
        }
    else
        {
        iVirtualIapNextLayerRecord->ModifyL( iSession );
        }

    OstTraceFunctionExit0( CCMPLUGINVPN_UPDATEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::DeleteBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::DeleteBearerRecordsL()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_DELETEBEARERRECORDSL_ENTRY );    
    iVirtualIapNextLayerRecord->DeleteL( iSession );
    OstTraceFunctionExit0( CCMPLUGINVPN_DELETEBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::ResetBearerRecords
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::ResetBearerRecords()
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_RESETBEARERRECORDS_ENTRY );
    
    delete iVirtualIapNextLayerRecord;
    iVirtualIapNextLayerRecord = NULL;    
    OstTraceFunctionExit0( CCMPLUGINVPN_RESETBEARERRECORDS_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::CopyBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::CopyBearerRecordsL( CCmPluginBaseEng* aCopyInstance ) 
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_COPYBEARERRECORDSL_ENTRY );
    
    __ASSERT_DEBUG(iVirtualIapNextLayerRecord != NULL, User::Invariant());
           
    CCmPluginVpn* plugin = static_cast<CCmPluginVpn*>( aCopyInstance );

    __ASSERT_DEBUG(plugin->iVirtualIapNextLayerRecord == NULL, User::Invariant());
        
    plugin->iVirtualIapNextLayerRecord = new (ELeave) CCDVirtualIAPNextLayerRecord( iVirtualTableId );
    TUint value = iVirtualIapNextLayerRecord->iIAP;
    plugin->iVirtualIapNextLayerRecord->iIAP = value;
    value = iVirtualIapNextLayerRecord->iNextLayerSNAP;
    plugin->iVirtualIapNextLayerRecord->iNextLayerSNAP = value;
    value = iVirtualIapNextLayerRecord->iNextLayerIAP;
    plugin->iVirtualIapNextLayerRecord->iNextLayerIAP = value;    

    OstTraceFunctionExit0( CCMPLUGINVPN_COPYBEARERRECORDSL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::BearerRecordIdL
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::BearerRecordIdL( TUint32& aRecordId )
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_BEARERRECORDIDL_ENTRY );
    
    CCDVirtualBearerRecord* bearerRecord = GetVirtualBearerRecordLC();
    aRecordId = bearerRecord->RecordId();
    CleanupStack::PopAndDestroy(bearerRecord);

    OstTraceFunctionExit0( CCMPLUGINVPN_BEARERRECORDIDL_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::BearerRecordNameLC
// ---------------------------------------------------------------------------
//
void CCmPluginVpn::BearerRecordNameLC( HBufC* &aBearerName )
    {   
    OstTraceFunctionEntry0( CCMPLUGINVPN_BEARERRECORDNAMELC_ENTRY );
    
    aBearerName = TPtrC( KCDTypeNameVirtualBearer ).AllocLC();
    
    OstTraceFunctionExit0( CCMPLUGINVPN_BEARERRECORDNAMELC_EXIT );
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::ServiceRecord
// ---------------------------------------------------------------------------
//
CCDVPNServiceRecord& CCmPluginVpn::ServiceRecord() const
    {    
    OstTraceFunctionEntry0( CCMPLUGINVPN_SERVICERECORD_ENTRY );
    
    CCDVPNServiceRecord& serviceRecord = *static_cast<CCDVPNServiceRecord*>( iServiceRecord );
    
    OstTraceFunctionExit0( CCMPLUGINVPN_SERVICERECORD_EXIT );
    return serviceRecord;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetVirtualBearerRecordLC
// ---------------------------------------------------------------------------
//
CCDVirtualBearerRecord* CCmPluginVpn::GetVirtualBearerRecordLC() const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETVIRTUALBEARERRECORDLC_ENTRY );
        
    //First tries to load the virtual bearer record.
    //If the loading fails creates a new one.
    CMDBRecordSet<CCDVirtualBearerRecord>* bearersRS = 
            new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>
                                               (KCDTIdVirtualBearerRecord);
    CleanupStack::PushL( bearersRS );

    CCDVirtualBearerRecord* bearerRecord = 
            static_cast<CCDVirtualBearerRecord *>
                (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));

    CleanupStack::PushL( bearerRecord );

    // Find entries used "vpnconnagt.agt" as agent from Table VirtualBearer
    bearerRecord->iBearerAgent.SetL( KVpnVirtualBearerAgent );
    bearersRS->iRecords.AppendL( bearerRecord );

    CleanupStack::Pop( bearerRecord );
    bearerRecord = NULL;
    
    CCDVirtualBearerRecord* retVal = NULL;
    if ( bearersRS->FindL( iSession ) )
        {
        for (TInt i = 0; i < bearersRS->iRecords.Count(); ++i)
            {            
            TPtrC bearerName( (*bearersRS)[i]->iRecordName.GetL() );
    
            // Further comparing record ID referred to by this VPN IAP with entry ID in table VirtualBearer
            // And also comparing bear name with our expected one "vpnbearer"
            if( bearerName == TPtrC( KVpnVirtualBearerName ) )
                {
                TUint32 recordId = (*bearersRS)[i]->RecordId();
                retVal = static_cast<CCDVirtualBearerRecord *>
                                            (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
                CleanupStack::PushL(retVal);
                retVal->SetRecordId( recordId );
                retVal->LoadL( iSession );
                CleanupStack::Pop(retVal);
                break;
                }
            }
        }         
    CleanupStack::PopAndDestroy( bearersRS );

    //If iVirtualBearerRecord is still NULL the loading has failed we try to create a new 
    //entry to the table.
    if (retVal == NULL)
        {
        retVal = static_cast<CCDVirtualBearerRecord *>
                                    (CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
        CleanupStack::PushL(retVal);
        retVal->iRecordName.SetL( KVpnVirtualBearerName );
        retVal->iBearerAgent.SetL( KVpnVirtualBearerAgent );
        retVal->iVirtualBearerNifName.SetL(KVpnVirtualBearerNif);
        retVal->iLastSocketActivityTimeout = (TUint32)KNoTimeout;
        retVal->iLastSessionClosedTimeout = KLastSessionTimeout;
        retVal->iLastSocketClosedTimeout = (TUint32)KNoTimeout;
        retVal->iBearerTechnology = KCommDbBearerVirtual;
        retVal->SetRecordId( KCDNewRecordRequest );
        
        retVal->StoreL( iSession );
        }
    else
        {
        CleanupStack::PushL(retVal);
        }
    
    OstTraceFunctionExit0( CCMPLUGINVPN_GETVIRTUALBEARERRECORDLC_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginVpn::GetDefPriorityL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginVpn::GetDefPriorityL( const TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINVPN_GETDEFPRIORITYL_ENTRY );
    
    TUint32 retVal( KDataMobilitySelectionPolicyPriorityWildCard );

    CCDGlobalBearerTypePriorizationRecord* priorityRecord =
            new( ELeave ) CCDGlobalBearerTypePriorizationRecord( iBearerPriorityTableId );
    CleanupStack::PushL( priorityRecord );

    priorityRecord->iServiceType.SetL( TPtrC( KCDTypeNameVPNService ) );

    if ( priorityRecord->FindL( iSession ) )
        {
        priorityRecord->LoadL( iSession );

        switch ( aAttribute )
            {
            case ECmDefaultPriority:
                {
                retVal = priorityRecord->iPriority;
                }
                break;
            case ECmDefaultUiPriority:
                {
                retVal = priorityRecord->iUIPriority;
                }
                break;
            default:
                break;
            }
        }
    CleanupStack::PopAndDestroy( priorityRecord );

    OstTraceFunctionExit0( CCMPLUGINVPN_GETDEFPRIORITYL_EXIT );
    return retVal;
    }
