/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CCmPluginEmbDestination
*
*/

#include <ecom/ecom.h>      // For REComSession
#include <datamobilitycommsdattypes.h>

#include "cmpluginembdestination.h"
#include "cmmserverdefs.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmpluginembdestinationTraces.h"
#endif

using namespace CMManager;
using namespace CommsDat;

// ---------------------------------------------------------
// CCmPluginEmbDestination::NewL()
// ---------------------------------------------------------
//
CCmPluginEmbDestination* CCmPluginEmbDestination::NewL(
        TCmPluginInitParam* aInitParam )
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_NEWL_ENTRY );

    CCmPluginEmbDestination* self = new( ELeave ) CCmPluginEmbDestination( aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginEmbDestination::CreateInstanceL(
        TCmPluginInitParam& aInitParam ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_CREATEINSTANCEL_ENTRY );

    CCmPluginEmbDestination* self = new( ELeave ) CCmPluginEmbDestination( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_CREATEINSTANCEL_EXIT );
    return self;
    }

// ---------------------------------------------------------
// CCmPluginEmbDestination::~CCmPluginEmbDestination()
// ---------------------------------------------------------
//
CCmPluginEmbDestination::~CCmPluginEmbDestination()
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CCmPluginEmbDestination()
// ---------------------------------------------------------------------------
//
CCmPluginEmbDestination::CCmPluginEmbDestination(
        TCmPluginInitParam* aInitParam )
        :
        CCmPluginBaseEng( aInitParam )
	{
	iBearerType = KUidEmbeddedDestination;
	iDestMetadataTableId = 0;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::ConstructL()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::ConstructL()
	{
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_CONSTRUCTL_ENTRY );

    CCmPluginBaseEng::ConstructL();

    TRAPD( err, iDestMetadataTableId = CCDSNAPMetadataRecord::TableIdL( iSession ) );
    if ( err == KErrNotFound )
        {
        iDestMetadataTableId = CCDSNAPMetadataRecord::CreateTableL( iSession );
        }
    else
        {
        User::LeaveIfError( err );
        }

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_CONSTRUCTL_EXIT );
	}


// ---------------------------------------------------------
// CCmPluginEmbDestination::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginEmbDestination::CanHandleIapIdL(
        TUint32 /*aIapId*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CCmPluginEmbDestination::CanHandleIapIdL()
// ---------------------------------------------------------
//
TBool CCmPluginEmbDestination::CanHandleIapIdL(
        CCDIAPRecord* /*aIapRecord*/ ) const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginEmbDestination::PreparePluginToLoadRecordsL()
    {
    }

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::PreparePluginToUpdateRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginEmbDestination::PreparePluginToUpdateRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::CreateServiceRecordL()
    {
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::LoadServiceRecordL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::LoadServiceRecordL()
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::PrepareToCopyDataL
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::PrepareToCopyDataL(
        CCmPluginBaseEng* /*aCopyInstance*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CopyServiceRecordL
// ---------------------------------------------------------------------------
//
CommsDat::CCDRecordBase* CCmPluginEmbDestination::CopyServiceRecordL()
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::UpdateServiceRecordL()
// ----------------------------------------------------------------------------
//
void CCmPluginEmbDestination::UpdateServiceRecordL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::CreateBearerRecordsL()
    {
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::LoadBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::LoadBearerRecordsL()
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::ServiceRecordIdLC
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginEmbDestination::ServiceRecordId() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::ServiceRecordNameLC
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::ServiceRecordNameLC( HBufC*& /*aName*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::BearerRecordIdL()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::BearerRecordIdL( TUint32& /*aRecordId*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::BearerRecordNameLC()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::BearerRecordNameLC( HBufC*& /*aBearerName*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerSpecificRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::GetBearerSpecificRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aRecordArray*/ )
    {
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::DeleteBearerRecordsL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::DeleteBearerRecordsL()
    {
    }

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::UpdateBearerRecordsL()
// ----------------------------------------------------------------------------
//
void CCmPluginEmbDestination::UpdateBearerRecordsL(
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CCmPluginEmbDestination::ResetBearerRecords()
// ---------------------------------------------------------
//
void CCmPluginEmbDestination::ResetBearerRecords()
    {
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerIntAttributeL
// --------------------------------------------------------------------------
//
TUint32 CCmPluginEmbDestination::GetBearerIntAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_GETBEARERINTATTRIBUTEL_ENTRY );

    TUint32 retVal( 0 );
    switch ( aAttribute )
        {
        case ECmBearerType:
            {
            retVal = iBearerType;
            }
            break;
        case ECmId:
            {
            retVal = iCmId;
            }
            break;
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        case ECmDefaultPriority:
        case ECmDefaultUiPriority:
            {
            retVal = KDataMobilitySelectionPolicyPriorityWildCard;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_GETBEARERINTATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::GetBearerBoolAttributeL(
        TUint32 aAttribute,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_GETBEARERBOOLATTRIBUTEL_ENTRY );

    TBool retVal( 0 );
    switch ( aAttribute )
        {
        case ECmCoverage:
            {
            retVal = EFalse;
            }
            break;
        case ECmDestination:
            {
            retVal = ETrue;
            }
            break;
        case ECmProtected:
            {
            TUint32 protlevel( CMManager::EProtLevel0 );
            ProtectionLevelL( protlevel );
            if ( protlevel == CMManager::EProtLevel1 || protlevel == CMManager::EProtLevel2 )
                {
                retVal = ETrue;
                }
            else
                {
                retVal = EFalse;
                }
            }
            break;
        case ECmHidden:
            {
            IsHiddenL( retVal );
            }
            break;
        case ECmBearerHasUi:
            {
            retVal = EFalse;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_GETBEARERBOOLATTRIBUTEL_EXIT );
    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::ProtectionLevelL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::ProtectionLevelL( TUint32& aProtLevel )
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_PROTECTIONLEVELL_ENTRY );

    CCDSNAPMetadataRecord* metadataRecord; // Owned.
    metadataRecord = new( ELeave ) CCDSNAPMetadataRecord( iDestMetadataTableId );
    CleanupStack::PushL( metadataRecord );

    metadataRecord->iSNAP = iCmId;
    metadataRecord->LoadL( iSession );

    aProtLevel = ( metadataRecord->iMetadata & KDestProtectionLevelMask )
            >> KBitsToShiftDestProtectionLevel;

    CleanupStack::PopAndDestroy( metadataRecord );

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_PROTECTIONLEVELL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::IsHiddenL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::IsHiddenL( TBool& aHidden )
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_ISHIDDENL_ENTRY );

    aHidden = EFalse;

    CCDSNAPMetadataRecord* metadataRecord; // Owned.
    metadataRecord = new( ELeave ) CCDSNAPMetadataRecord( iDestMetadataTableId );
    CleanupStack::PushL( metadataRecord );

    metadataRecord->iSNAP = iCmId;
    metadataRecord->LoadL( iSession );

    if( ( TUint32 )( metadataRecord->iMetadata ) & CMManager::ESnapMetadataHiddenAgent )
        {
        aHidden = ETrue;
        }

    CleanupStack::PopAndDestroy( metadataRecord );

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_ISHIDDENL_EXIT );
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerStringAttributeL
// --------------------------------------------------------------------------
//
HBufC* CCmPluginEmbDestination::GetBearerStringAttributeL(
        TUint32 /*aAttribute*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    HBufC* retVal = NULL;

    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerString8AttributeL
// --------------------------------------------------------------------------
//
HBufC8* CCmPluginEmbDestination::GetBearerString8AttributeL(
        TUint32 /*aAttribute*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    HBufC8* retVal( NULL );

    return retVal;
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::SetBearerIntAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetBearerIntAttributeL(
        TUint32 /*aAttribute*/,
        TUint32 /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::SetBearerBoolAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetBearerBoolAttributeL(
        TUint32 /*aAttribute*/,
        TBool /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::SetBearerStringAttributeL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetBearerStringAttributeL(
        TUint32 /*aAttribute*/,
        const TDesC16& /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CCmPluginEmbDestination::SetBearerString8AttributeL
// --------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetBearerString8AttributeL(
        TUint32 /*aAttribute*/,
        const TDesC8& /*aValue*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aGenRecordArray*/,
        RPointerArray<CommsDat::CCDRecordBase>& /*aBearerSpecRecordArray*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerInfoIntL
// ---------------------------------------------------------------------------
//
TUint32 CCmPluginEmbDestination::GetBearerInfoIntL(
        TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_GETBEARERINFOINTL_ENTRY );

    TUint32 retVal( 0 );
    switch ( aAttribute )
        {
        case ECmBearerType:
            {
            retVal = iBearerType;
            }
            break;
        case ECmDefaultUiPriority:
        case ECmDefaultPriority:
            {
            retVal = KDataMobilitySelectionPolicyPriorityWildCard;
            }
            break;
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_GETBEARERINFOINTL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerInfoBoolL
// ---------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::GetBearerInfoBoolL( TUint32 aAttribute ) const
    {
    OstTraceFunctionEntry0( CCMPLUGINEMBDESTINATION_GETBEARERINFOBOOLL_ENTRY );

    TBool retVal( EFalse );
    switch ( aAttribute )
        {
        case ECmCoverage:
            {
            retVal = EFalse;
            }
            break;
        case ECmDestination:
            {
            retVal = ETrue;
            }
            break;
        case ECmBearerHasUi:
            {
            retVal = EFalse;
            }
            break;
        case ECmVirtual:
            {
            retVal = EFalse;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    OstTraceFunctionExit0( CCMPLUGINEMBDESTINATION_GETBEARERINFOBOOLL_EXIT );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerInfoStringL
// ---------------------------------------------------------------------------
//
HBufC* CCmPluginEmbDestination::GetBearerInfoStringL(
        TUint32 /*aAttribute*/ ) const
    {
    HBufC* retVal( NULL );

    User::Leave( KErrNotSupported );

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerInfoString8L
// ---------------------------------------------------------------------------
//
HBufC8* CCmPluginEmbDestination::GetBearerInfoString8L(
        TUint32 /*aAttribute*/ ) const
    {
    HBufC8* retVal( NULL );

    User::Leave( KErrNotSupported );

    return retVal;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CopyBearerRecordsL
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::CopyBearerRecordsL(
        CCmPluginBaseEng* /*aCopyInstance*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBearerTableIdsToBeObserved
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::GetBearerTableIdsToBeObservedL(
        RArray<TUint32>& /*aTableIdArray*/ ) const
    {
    }

// End of file
