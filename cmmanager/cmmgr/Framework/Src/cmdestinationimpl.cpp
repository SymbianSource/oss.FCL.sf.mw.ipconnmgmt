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
* Description:  Implementation of CCmDestinationImpl
*
*/

#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include <cmmanager.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypeinfov1_1_internal.h>
#include <commsdattypesv1_1_partner.h> 
#endif
#include "cmlogger.h"
#include "cmmanager.hrh"
#include <cmcommonconstants.h>
#include "datamobilitycommsdattypes.h"
#include <cmpluginembdestinationdef.h>
#include <cmmanagerdef.h>
#include <data_caging_path_literals.hrh>
#include <cmmanager.rsg>
#include <cmmanager.mbg>   // icons
#include <AknsUtils.h>
#include <e32cmn.h>

// System includes
#include <ecom/ecom.h>        // For REComSession
#include <commsdattypesv1_1.h>

#include <in_sock.h>

using namespace CMManager;
using namespace CommsDat;

const TUint KDefaultTierManagerTagId = KAfInet;

const TInt KDestinationIconConvTable[][4] = {
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnIndiSettProtectedAdd,
      EMbmCmmanagerQgn_prop_set_conn_dest_internet, 
      EMbmCmmanagerQgn_prop_set_conn_dest_internet_mask },
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_wap, 
      EMbmCmmanagerQgn_prop_set_conn_dest_wap_mask },                   
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_mms, 
      EMbmCmmanagerQgn_prop_set_conn_dest_mms_mask },
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_intranet, 
      EMbmCmmanagerQgn_prop_set_conn_dest_intranet_mask },          
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_operator, 
      EMbmCmmanagerQgn_prop_set_conn_dest_operator_mask },
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon1, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon1_mask },     
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon2, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon2_mask },
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon3, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon3_mask },            
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon4, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon4_mask },       

    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon5, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon5_mask }, 
             
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_icon6, 
      EMbmCmmanagerQgn_prop_set_conn_dest_icon6_mask }, 
      
    { EAknsMajorGeneric, 
      EAknsMinorGenericQgnPropWmlGprs,
      EMbmCmmanagerQgn_prop_set_conn_dest_default, 
      EMbmCmmanagerQgn_prop_set_conn_dest_default_mask },            
    {0, 0, 0, 0}
    };

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCmDestinationImpl::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CCmDestinationImpl* 
                           CCmDestinationImpl::NewL( CCmManagerImpl& aCmMgr,
                                                     const TDesC& aName )
    {
    CCmDestinationImpl* dest = 
                    new (ELeave) CCmDestinationImpl( aCmMgr );
    CleanupStack::PushL( dest );
    
    dest->ConstructL( aName );
    
    CleanupStack::Pop( dest );
    return dest;        
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CCmDestinationImpl* 
                        CCmDestinationImpl::NewL( CCmManagerImpl& aCmMgr,
                                                  TUint aDestinationId )
    {
    CCmDestinationImpl* dest = 
                    new (ELeave) CCmDestinationImpl( aCmMgr );
    CleanupStack::PushL( dest );
    
    dest->ConstructL( aDestinationId );
    
    CleanupStack::Pop( dest );
    return dest;        
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::NewL()
// -----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmDestinationImpl::NewL( CCmManagerImpl& aCmMgr,
                                              CCmDestinationData* aData )
    {
    CCmDestinationImpl* dest = 
                    new (ELeave) CCmDestinationImpl( aCmMgr );
    dest->iData = aData;
    return dest;        
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::NewL()
// -----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmDestinationImpl::NewL( CCmManagerImpl& aCmMgr,
                                              const TDesC& aName,
                                              TUint32 aDestId)
    {
    CCmDestinationImpl* dest = 
                              new (ELeave) CCmDestinationImpl( aCmMgr );
    CleanupStack::PushL( dest );

    dest->ConstructL( aName, aDestId );

    CleanupStack::Pop( dest );
    return dest;        
    }


// -----------------------------------------------------------------------------
// CCmDestinationImpl::NewLC()
// -----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmDestinationImpl::NewLC( CCmDestinationData* aData )
    {
    CCmDestinationImpl* dest = 
                    new (ELeave) CCmDestinationImpl( aData->iCmMgr );
    CleanupStack::PushL( dest );
    dest->iData = aData;
    return dest;        
    }


// -----------------------------------------------------------------------------
// CCmDestinationImpl::CCmDestinationImpl()
// -----------------------------------------------------------------------------
//
CCmDestinationImpl::CCmDestinationImpl( CCmManagerImpl& aCmMgr )
    : iCmMgr( aCmMgr )
    {
    }

// -----------------------------------------------------------------------------
// CCmDestinationData::CCmDestinationData()
// -----------------------------------------------------------------------------
//
CCmDestinationData::CCmDestinationData( CCmManagerImpl& aCmMgr )
    : iCmMgr( aCmMgr ),
    iNewWithId ( EFalse )
    {
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationData::~CCmDestinationData()
// -----------------------------------------------------------------------------
//
CCmDestinationData::~CCmDestinationData()
    {
    delete iMetaDataRec;
    delete iNetworkRecord;
    delete iDestAPRecord;

    if (iConnMethodArray)
        {
        ResetAndDestroyArray( iConnMethodArray, ETrue );        
        }
    if (iDeleteCmArray)
        {
        ResetAndDestroyArray( iDeleteCmArray, ETrue );        
        }
    
    CLOG_CLOSE;
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::~CCmDestinationImpl()
// -----------------------------------------------------------------------------
//
CCmDestinationImpl::~CCmDestinationImpl()
    {
    if (iData)
        {
        if (GetRefCounter())
            {
            iCmMgr.RemoveDestFromPool(this);        
            }
        else //if leaves during construction refcounter is 0
            {
            delete iData; 
            }        
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConstructL()
// -----------------------------------------------------------------------------
//    
void CCmDestinationImpl::ConstructL( const TDesC& aName )
    {
    CLOG_CREATE;
    CLOG_NAME_2( _L("Destination_0x%x_%S"), this, &aName );
    
    iData =  new (ELeave) CCmDestinationData(iCmMgr);
    iData->iNetworkRecord = static_cast<CCDNetworkRecord *>(
                         CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );
    
    iData->iConnMethodArray = new (ELeave) CCmArray( KCmArrayBigGranularity );
    iData->iDeleteCmArray = new (ELeave) CCmArray( KCmArrayBigGranularity );
    
    iData->iMetaDataRec = new (ELeave) CCDSNAPMetadataRecord( 
                                       iCmMgr.IconTableId() );

    iData->iDestAPRecord = static_cast<CCDAccessPointRecord *>(
                    CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );

    SetNameL( aName );

    InitializeDestAPRecordL();

    CPluginItem* item = new (ELeave) CPluginItem;
    CleanupStack::PushL( item );

    // create the new record
    item->iDNRecord = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( item->iDNRecord );
    
    AddToArrayL( item );
    
    CleanupStack::Pop( 2, item );  // iDNRecord, item
    iData->iIdIsValid = EFalse;// the Id is still zero here
    } 

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConstructL()
// -----------------------------------------------------------------------------
//    
void CCmDestinationImpl::ConstructL( const TDesC& aName, TUint32 aDestId )
    {
    CLOG_CREATE;
    CLOG_NAME_2( _L("Destination_0x%x_%S"), this, &aName );

    iData =  new (ELeave) CCmDestinationData(iCmMgr);
    iData->iNetworkRecord = static_cast<CCDNetworkRecord *>(
                    CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );

    iData->iConnMethodArray = new (ELeave) CCmArray( KCmArrayBigGranularity );
    iData->iDeleteCmArray = new (ELeave) CCmArray( KCmArrayBigGranularity );

    iData->iMetaDataRec = new (ELeave) CCDSNAPMetadataRecord( iCmMgr.IconTableId() );

    iData->iDestAPRecord = static_cast<CCDAccessPointRecord *>(
                    CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );

    SetNameL( aName );
    
    TInt correctAPId = aDestId;
    TInt correctNetworkId = aDestId;
	
    if ( aDestId > 0 && aDestId < 255 )
        {
        correctAPId = aDestId + KCmDefaultDestinationAPTagId;
        }
    else if ( aDestId > KCmDefaultDestinationAPTagId 
              && ( aDestId < ( KCmDefaultDestinationAPTagId+255 ) ) )
        {
        correctNetworkId = aDestId - KCmDefaultDestinationAPTagId;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    SetIdL( correctAPId );

    InitializeDestAPRecordL();

    CPluginItem* item = new (ELeave) CPluginItem;
    CleanupStack::PushL( item );

    // create the new record
    iData->iNetworkRecord->SetRecordId( correctNetworkId );
    iData->iNewWithId = ETrue;
    item->iDNRecord = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( item->iDNRecord );

    AddToArrayL( item );

    CleanupStack::Pop( 2, item );  // iDNRecord, item
    iData->iIdIsValid = EFalse;// the Id is still zero here
    } 

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetIdL()
// -----------------------------------------------------------------------------
//        
void CCmDestinationImpl::SetIdL( TInt aDestId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetIdL" );

    TInt correctAPId( aDestId );
    if ( aDestId > 0 && aDestId < 255 )
        {
        correctAPId = aDestId + KCmDefaultDestinationAPTagId;
        }
    else if ( aDestId < KCmDefaultDestinationAPTagId + 1
             || aDestId > KCmDefaultDestinationAPTagId + 255 )
        {
        User::Leave( KErrArgument );
        }
        
    CheckIfDestinationIdExistsL( correctAPId );

    iData->iDestAPRecord->iRecordTag = correctAPId;
    };

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfDestinationIdExistsL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::CheckIfDestinationIdExistsL( TUint32 aDestId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfDestinationNameExistL" );

    CMDBRecordSet<CCDAccessPointRecord>*  ptrRecordSet = 
          new (ELeave) CMDBRecordSet<CCDAccessPointRecord>( KCDTIdAccessPointRecord );
    CleanupStack::PushL( ptrRecordSet );    

    // Prime record
    CCDAccessPointRecord* record = static_cast<CCDAccessPointRecord *>
                                   ( CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
    CleanupStack::PushL( record );

    record->iRecordTag = ( TInt )aDestId;

    ptrRecordSet->iRecords.AppendL( record );

    CleanupStack::Pop( record );
    record = NULL;

    if ( ptrRecordSet->FindL( Session() ) )
        {
        User::Leave( KErrAlreadyExists );      
        }

    ptrRecordSet->iRecords.ResetAndDestroy();
    CleanupStack::PopAndDestroy( ptrRecordSet );

    iData->iDestAPRecord->iRecordTag = aDestId;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfDestinationNameExistL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::CheckIfDestinationNameExistL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfDestinationNameExistL" );
    
    CMDBRecordSet<CCDSnapRecord>*  ptrRecordSet = 
                            new (ELeave) CMDBRecordSet<CCDSnapRecord>( 
                                 iCmMgr.DestinationTableId() );
    CleanupStack::PushL( ptrRecordSet );    
    
    // Prime record
    CCDSnapRecord* record = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( record );
    
    record->iRecordName.SetL( aName );
    
    ptrRecordSet->iRecords.AppendL( record );

    CleanupStack::Pop( record );
    record = NULL;
                
    if ( ptrRecordSet->FindL( Session() ) )
        {
        User::Leave( KErrAlreadyExists );          
        }
        
    ptrRecordSet->iRecords.ResetAndDestroy();
    CleanupStack::PopAndDestroy( ptrRecordSet );            
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConstructL()
// -----------------------------------------------------------------------------
//    
void CCmDestinationImpl::ConstructL( TUint aDestinationId )
    {
    CLOG_CREATE;
    CLOG_NAME_2( _L("Destination_0x%x_%d"), this, aDestinationId );
    iData =  new (ELeave) CCmDestinationData(iCmMgr);
    OpenTransactionLC();
    
    iData->iNetworkRecord = static_cast<CCDNetworkRecord *>(
                         CCDRecordBase::RecordFactoryL( KCDTIdNetworkRecord ) );

    iData->iDestAPRecord = static_cast<CCDAccessPointRecord *>(
                         CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
    
    iData->iConnMethodArray = new (ELeave) CCmArray( KCmArrayBigGranularity );
    iData->iDeleteCmArray = new (ELeave) CCmArray( KCmArrayBigGranularity );

    TInt correctAPId = aDestinationId;
    TInt correctNetworkId = aDestinationId;
	
    if ( aDestinationId > 0 && aDestinationId < 255 )
        {
        correctAPId = aDestinationId + KCmDefaultDestinationAPTagId;
        }
    else if ( aDestinationId > KCmDefaultDestinationAPTagId 
              && ( aDestinationId < ( KCmDefaultDestinationAPTagId+255 ) ) )
        {
        correctNetworkId = aDestinationId - KCmDefaultDestinationAPTagId;
        }
    else
        {
        User::Leave( KErrArgument );
        }
        
    LoadDestAPRecordL( correctAPId );
    
    LoadNetworkRecordL( correctNetworkId );

    LoadConnectionMethodsL();
    LoadSNAPMetadataTableL();
    
    RollbackTransaction();
    
    iData->iIdIsValid = ETrue;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::Session()
// -----------------------------------------------------------------------------
//        
CMDBSession& CCmDestinationImpl::Session()const
    {
    return iCmMgr.Session();
    }
    
// ---------------------------------------------------------------------------
// CCmDestinationImpl::OpenTransactionLC
// ---------------------------------------------------------------------------
//
void CCmDestinationImpl::OpenTransactionLC()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::OpenTransactionL" );
    // Do NOT call OpenTransactionLC() more then once!
    iCmMgr.OpenTransactionLC( ETrue );
    }

// ---------------------------------------------------------------------------
// CCmDestinationImpl::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmDestinationImpl::CommitTransactionL( TInt aError )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CommitTransactionL" );
    
    iCmMgr.CommitTransactionL( aError );
    }

// ---------------------------------------------------------------------------
// CCmDestinationImpl::RollbackTransaction
// ---------------------------------------------------------------------------
//
void CCmDestinationImpl::RollbackTransaction()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::RollbackTransaction" );
    
    iCmMgr.RollbackTransaction();
    }

    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::NameLC()
// -----------------------------------------------------------------------------
//        
EXPORT_C HBufC* CCmDestinationImpl::NameLC( )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::NameLC" );

    HBufC* name = NULL;
    const TUint32 KSnapMetadataNameMask = 0x0000000F;

    TUint32 val = TUint32(iData->iMetaDataRec->iMetadata) & KSnapMetadataNameMask;
    TUint32 val2 = (TUint32(iData->iMetaDataRec->iMetadata) & ESnapMetadataDestinationIsLocalised)
                     >> 4;                   
    
    if( (val == ESnapMetadataInternet) ||
        (val2 == ELocalisedDestInternet) )
        {
        name = iCmMgr.AllocReadL( R_CMMANAGERUI_DEST_INTERNET );
        }
    else if ( val2 == ELocalisedDestWap )
        {
        name = iCmMgr.AllocReadL( R_CMMANAGERUI_DEST_WAP );
        }
    else if ( val2 == ELocalisedDestMMS )
        {
        name = iCmMgr.AllocReadL( R_CMMANAGERUI_DEST_MMS );
        }
    else if ( val2 == ELocalisedDestIntranet )
        {
        name = iCmMgr.AllocReadL( R_CMMANAGERUI_DEST_INTRANET );
        }
    else
        {
        name = TPtrC(iData->iNetworkRecord->iRecordName).AllocL();
        }
        
    CleanupStack::PushL( name );
        
    return name;
    };

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetNameL()
// -----------------------------------------------------------------------------
//        
EXPORT_C void CCmDestinationImpl::SetNameL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetNameL" );

    // Destination cannot be renamed if it's
    // - protected
    // - Internet    
    if ( ProtectionLevel() == EProtLevel1 ||
         ProtectionLevel() == EProtLevel2 ||
         MetadataL( ESnapMetadataInternet ) )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }

    OpenTransactionLC();
    CheckIfDestinationNameExistL( aName );
    CommitTransactionL( KErrNone );        

    iData->iNetworkRecord->iRecordName.SetL( aName );
    iData->iDestAPRecord->iRecordName.SetL( aName );
    };
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodCount()
// -----------------------------------------------------------------------------
//        
TInt CCmDestinationData::ConnectionMethodCount()
    {    
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodCount" );

    return iConnMethodArray->Count() - KCmInitIndex;
    };
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodCount()
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CCmDestinationImpl::ConnectionMethodCount()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodCount" );
    
    return iData->ConnectionMethodCount();
    };
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodL
// -----------------------------------------------------------------------------
//     
EXPORT_C CCmPluginBaseEng* CCmDestinationImpl::ConnectionMethodL( TInt /*aIndex*/ )
    {
    LOGGER_ENTERFN( "Error: Obsolete function CCmDestinationImpl::ConnectionMethodL used" );
    User::Panic( _L("CMManager"), KErrNotSupported );
    return NULL;    
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodL
// -----------------------------------------------------------------------------
//     
EXPORT_C CCmPluginBase* CCmDestinationImpl::GetConnectionMethodL( TInt aIndex )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodL" );
    // Initial entry!
   
    CCmPluginBase* retVal = NULL;    
    TInt index = KCmInitIndex + aIndex;
    CPluginItem* item = (*(iData->iConnMethodArray))[index];

    LoadPluginL( *item );
    retVal = item->iPlugin;
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodByIDL
// -----------------------------------------------------------------------------
//     
CCmPluginBase* CCmDestinationImpl::ConnectionMethodByIDL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodByIDL" );

    TInt found = ConnectionMethodIndex( aCmId );

    if( found == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
        
    CPluginItem* item = (*(iData->iConnMethodArray))[found];

    LoadPluginL( *item );
    
    return item->iPlugin;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectMethodIdArrayL
// -----------------------------------------------------------------------------
//     
void CCmDestinationImpl::ConnectMethodIdArrayL( RArray<TUint32>& aCmIds )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectMethodIdArrayL" );

    TInt count = iData->iConnMethodArray->Count();    

    for( TInt i = KCmInitIndex; i < count; ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];

        TUint32 elementId = item->iDNRecord->iIAP ?
                          RECORD_FROM_ELEMENT_ID( TUint32(item->iDNRecord->iIAP) ) :
                          TUint32( item->iDNRecord->iEmbeddedSNAP );
                          
        User::LeaveIfError( aCmIds.Append( elementId ) );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::AddToArrayL
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationImpl::AddToArrayL( CPluginItem* aItem )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::AddToArrayL" );

    TInt count = iData->iConnMethodArray->Count();    
    if ( 0 == count  )
        {
        iData->iConnMethodArray->AppendL( aItem );
        CLOG_WRITE_1( "inserted: [%d]", count );
        return count;
        }
        
    // Fill in SNAP record with IAP/EmbSNAP/SNAP values    
    TMDBElementId iapElemetid( 0 );
    TInt embDestId( 0 );

    // No plugin means this is the template record.
    TBool isDestination = aItem->iPlugin->GetBoolAttributeL( ECmDestination );
    if ( isDestination )
        {
        embDestId = aItem->iPlugin->Destination()->Id();    
        }
    else
        {
        iapElemetid = aItem->iPlugin->IAPRecordElementId();   
        }
        
    aItem->iDNRecord->iIAP = iapElemetid;
    aItem->iDNRecord->iEmbeddedSNAP = embDestId;

    TUint32 prio = 0;
    TUint32 bearerType = 0;
    TUint32 extLevel = 0;
    
    TBool isVirtual( EFalse );
    
    if ( count == KCmInitIndex )
        {
        if( !aItem->iDNRecord->iIAP && !aItem->iDNRecord->iEmbeddedSNAP )
            {
            iData->iConnMethodArray->AppendL( aItem );
            }
        else
            {
            isVirtual = PrioDataL( aItem, prio, bearerType, extLevel );
            aItem->iDNRecord->iPriority = prio;
            iData->iConnMethodArray->AppendL( aItem );
            }
        return count;
        }
    
    TInt insertedAt( -1 );

    if ( !isDestination )
        {
        // This function returns the info necessary for determining the priority
        // It is the cm data in none VPN case 
        // It is the data of the underlying cm in case of VPN over cm
        // It is wildcard prio in case of VPN over destination
        isVirtual = PrioDataL( aItem, prio, bearerType, extLevel );

        CLOG_WRITE_3( "inserted item prio: [%d], bearer: [%x] ext: [%d]", 
                       prio, bearerType, extLevel );

        // wildcard priority goes to back
        if ( KDataMobilitySelectionPolicyPriorityWildCard == prio ) 
            {
            // Set the default priority of a wildcard bearer
            aItem->iDNRecord->iPriority = prio;
            }   
        else 
            {
            insertedAt = InsertSameMethodL( bearerType, isVirtual );
            if( insertedAt == -1 )
                {
                insertedAt = InsertPriorityMethodL( prio, extLevel );
                }
 
            } // if wildcard
        } // if !destination
   
    if( insertedAt == -1 )
        {
        //Add to the end of the list        
        iData->iConnMethodArray->AppendL( aItem );
        insertedAt = iData->iConnMethodArray->Count()-1;
        CLOG_WRITE_1( "inserted as last: [%d]", insertedAt );
        }
    else
        {
        iData->iConnMethodArray->InsertL( insertedAt, aItem );
        }    

    CLOG_WRITE_1( "inserted at: [%d]", insertedAt );
        
    return insertedAt;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::PrioDataL
// -----------------------------------------------------------------------------
// 
TBool CCmDestinationImpl::PrioDataL( CPluginItem* aItem,  TUint32& aRetPrio, 
                                     TUint32& aRetBearer, TUint32& aRetExtLevel )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::PrioDataL" );

    // Gets the data affecting the priority of the plugin in the list.
    // It means the VPN priory based on the barer priority of the underlying cm
    // or "wildcard" if VPN points to a destination      
    aRetPrio   = aItem->iPlugin->GetIntAttributeL( ECmDefaultPriority );
    aRetBearer = aItem->iPlugin->GetIntAttributeL( ECmBearerType );
    aRetExtLevel = aItem->iPlugin->GetIntAttributeL( ECmExtensionLevel );
    
    TBool isVirtual = aItem->iPlugin->GetBoolAttributeL( ECmVirtual );
    if( isVirtual )
        {
        // Embeded destination is also virtual but further checking should
        // not be done. 
        TBool isDestination = aItem->iPlugin->GetBoolAttributeL( ECmDestination );
        if( !isDestination )
            {
            TUint32 underlying = aItem->iPlugin->GetIntAttributeL( 
                                                           ECmNextLayerIapId );
            if ( underlying != 0 )
                {
                // Gets the bearer of the underlying cm
                aRetBearer = iCmMgr.BearerTypeFromCmIdL( underlying );
                // Gets the prio of the bearer
                aRetPrio = 
                iCmMgr.GetBearerInfoIntL( aRetBearer, ECmDefaultPriority );
                aRetExtLevel = 
                iCmMgr.GetBearerInfoIntL( aRetBearer, ECmExtensionLevel );
                }
            else
                {
                // VPN points to a destination so its priority is "wildcard"
                aRetPrio = KDataMobilitySelectionPolicyPriorityWildCard;
                }    
            }
        }

    return isVirtual;
    }


// -----------------------------------------------------------------------------
// CCmDestinationImpl::InsertSameMethodL
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationImpl::InsertSameMethodL( TUint32 aBearerType, TBool aIsVirtual )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::InsertSameMethodL" );

    TInt insertedAt( -1 );
    TUint32 bearerInList( 0 );
    TUint32 prioInList( 0 );
    TUint32 extLevelInList( 0 ); 
    TBool isVirtualInList( EFalse );
     
    TInt count = iData->iConnMethodArray->Count();    
    // Loops throught the items in the list
    for ( TInt i = count-1; ( i >= KCmInitIndex ) && ( insertedAt == -1) ; i-- )
        {
        // Loads the plugin
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        LoadPluginL( *item );
        
        // Gets the data of the list item    
        isVirtualInList = PrioDataL( item, 
                                    prioInList, bearerInList, extLevelInList );

        CLOG_WRITE_4( "actual item [%d] prio: [%d], bearer: [%x] ext: [%d]", 
                            i, prioInList, bearerInList, extLevelInList );
          
        // Bearer type the same
        if( aBearerType == bearerInList )
            {
            // puts Virt/Virt or !Virt/!Virt or !Virt/Virt
            if( ( aIsVirtual == isVirtualInList ) || ( aIsVirtual && !isVirtualInList ))
                {
                insertedAt = i+1;
                CLOG_WRITE_1( "inserted same bearer: [%d]", insertedAt );
                }
            } // if bearer
        } // for
        return insertedAt;
    }
    
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::InsertPriorityMethodL
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationImpl::InsertPriorityMethodL( TUint32 aPrio, TUint32 aExtLevel )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::InsertPriorityMethodL" );

    TInt insertedAt = -1;
    TUint32 bearerInList( 0 );
    TUint32 prioInList( 0 );
    TUint32 extLevelInList( 0 );

    TInt count = iData->iConnMethodArray->Count();    
    // Loops throught the items in the list
    ////for ( TInt i = KCmInitIndex; ( i < count ) && ( insertedAt == -1) ; i++ )
    TInt i ( count-1 );
    for ( ; ( i >= KCmInitIndex ) && ( insertedAt == -1) ; i-- )
        {
        // Loads the plugin
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        LoadPluginL( *item );

        // Gets the data of the list item    
        PrioDataL( item, prioInList, bearerInList, extLevelInList );

        CLOG_WRITE_4( "actual item [%d] prio: [%d], bearer: [%x] ext: [%d]", 
                        i, prioInList, bearerInList, extLevelInList );
        
        // If the priority is the same then it should be inserted near here
        if ( aPrio == prioInList )
            {
            if( aExtLevel <= extLevelInList )
                {
                insertedAt = i+1;
                CLOG_WRITE_1( "inserted same bearer non virtual: [%d]", insertedAt );
                }
            }
        // Insert in front of the higher priority (prio 1 is the highest ...)    
        else if ( aPrio > prioInList )
            {
            insertedAt = i+1;
            CLOG_WRITE_1( "inserted higher prio: [%d]", insertedAt );
            } // if same prio
            
        } // for
        
    // This is the highest priority one. It should be inserted at the first pos. 
    if( -1 == insertedAt )
        {
        insertedAt = KCmInitIndex;
        CLOG_WRITE_1( "inserted first position: [%d]", insertedAt );
        } // if same prio

    return insertedAt;
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationData::ProtectionLevel()
// -----------------------------------------------------------------------------
//  
TProtectionLevel CCmDestinationData::ProtectionLevel()
    {
    TInt protection = TUint32(iMetaDataRec->iMetadata) & 
                      KSnapMetadataProtecionLevelMask;
    return TProtectionLevel(protection >> 28);
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::ProtectionLevel()
// -----------------------------------------------------------------------------
//  
EXPORT_C 
    TProtectionLevel CCmDestinationImpl::ProtectionLevel()
    {
    return iData->ProtectionLevel();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetAttributeL()
// -----------------------------------------------------------------------------
//  
EXPORT_C void CCmDestinationImpl::SetProtectionL( TProtectionLevel aProtectionLevel )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetProtectionL" );
    
    CLOG_WRITE_1( "Level: [%d]", aProtectionLevel );

    CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
    
    // Make a destination protected.
    TUint32 mask = ~KSnapMetadataProtecionLevelMask;
    TUint32 temp = TUint32(iData->iMetaDataRec->iMetadata) & mask;    
    iData->iMetaDataRec->iMetadata = aProtectionLevel << 28;
    iData->iMetaDataRec->iMetadata = temp | TUint32(iData->iMetaDataRec->iMetadata);
    
    iData->iProtectionChanged = ETrue;
    }
        
// -----------------------------------------------------------------------------
// CCmDestinationImpl::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
CCmPluginBase* CCmDestinationImpl::CreateConnectionMethodL( 
                                                   TUint32 aImplementationUid )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CreateConnectionMethodL" );

    CCmPluginBase* retval = NULL;
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
    retval = iCmMgr.CreateConnectionMethodL(aImplementationUid, this);
    CleanupStack::PushL(retval); 
    AddConnectionMethodL(*retval);
    CleanupStack::Pop(retval);
    return retval;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
CCmPluginBase* CCmDestinationImpl::CreateConnectionMethodL( 
    TUint32 aImplementationUid,
    TUint32 aConnMthId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CreateConnectionMethodL" );

    CCmPluginBase* retval = NULL;
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }

    retval = iCmMgr.CreateConnectionMethodL( aImplementationUid,
                                             aConnMthId,
                                             this );
    CleanupStack::PushL(retval);
    AddConnectionMethodL(*retval);
    CleanupStack::Pop(retval);
    return retval;
    }


// -----------------------------------------------------------------------------
// CCmDestinationImpl::AddConnectionMethodL()
// -----------------------------------------------------------------------------
//
TInt CCmDestinationImpl::AddConnectionMethodL( 
                                           CCmPluginBase& aConnectionMethod )
    {    
    LOGGER_ENTERFN( "CCmDestinationImpl::AddConnectionMethodL" );

    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    TUint cmId = aConnectionMethod.GetIntAttributeL( ECmId );
    
    CPluginItem* item = new (ELeave) CPluginItem;
    CleanupStack::PushL( item ); // 1
    
    // get the plugin
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.Plugin());
    CleanupStack::PushL( connMethod );// 2           
    connMethod->IncrementRefCounter();    
    item->iPlugin = connMethod;

    // create the new record
    item->iDNRecord = iCmMgr.SNAPRecordL( 0 );

    CleanupStack::PushL( item->iDNRecord ); // 3
    
    // if the destination is protection level 1, the connection method must be protected
    if ( ProtectionLevel() == EProtLevel1 )
        {
        item->iPlugin->SetBoolAttributeL( ECmProtected, ETrue );
        }
    
    TInt index = AddToArrayL( item );
    index = index - KCmInitIndex; //the array contains one initial item, so subtract KCmInitIndex from the index to get cm index!
    
    CleanupStack::Pop( 3, item ); // item, connMethod, iDNRecord
    
    return index;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::AddEmbeddedDestinationL()
// -----------------------------------------------------------------------------
//
TInt CCmDestinationImpl::AddEmbeddedDestinationL( 
                                             CCmDestinationImpl& aDestination )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::AddEmbeddedDestinationL" );
    
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    //Check to avoid multiple levels of embedding
    CheckIfEmbeddedL( Id() );
    
    //Check to avoid multiple levels of embedding
    if ( aDestination.HasEmbeddedL() )
        {
        User::Leave( KErrNotSupported );
        }
        
    // Only one embedded destination per destination is allowed
    if ( HasEmbeddedL() )
        {
        User::Leave( KErrNotSupported );
        }
           
    CPluginItem* item = new (ELeave) CPluginItem;
    CleanupStack::PushL( item ); // 1

    CreateEmbeddedDestinationPluginL( *item, aDestination.Id() );
     // create the new record
    item->iDNRecord = iCmMgr.SNAPRecordL( 0 );
    item->iDNRecord->iPriority = KDataMobilitySelectionPolicyPriorityWildCard;

    CleanupStack::PushL( item->iDNRecord ); // 2    
    TInt index = AddToArrayL( item );
    index = index - KCmInitIndex; //the array contains one initial item, so subtract KCmInitIndex from the index to get cm index!
    
    CleanupStack::Pop( 2, item ); // item, iDNRecord
    
    return index;   
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::DeleteConnectionMethodL()
// -----------------------------------------------------------------------------
// 
void CCmDestinationImpl::DeleteConnectionMethodL( 
                                        CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteConnectionMethodL" );
    
    if( ProtectionLevel() == EProtLevel1 ||
        ProtectionLevel() == EProtLevel3 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    if ( aConnectionMethod.GetBoolAttributeL( ECmConnected ) )
        {
        User::Leave( KErrInUse );
        }
        
    TInt index = FindConnectionMethodL( aConnectionMethod );

    // move this method to the 'to be deleted' array         
    CPluginItem* item = (*(iData->iConnMethodArray))[index];
    
    LoadPluginL( *item );
    
    if( item->iPlugin->GetBoolAttributeL( ECmIsLinked ) )
        {
        User::Leave( KErrLocked );
        }

    if ( ConnectionMethodCount() == 1 )
        {
        // Check if this destination is referenced by Virtual CM 
        CheckIfReferencedByVirtualCML();
        }

    iData->iDeleteCmArray->AppendL( item );
    iData->iConnMethodArray->Delete( index );
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::RemoveConnectionMethodL()
// -----------------------------------------------------------------------------
// 
void CCmDestinationImpl::RemoveConnectionMethodL( 
                                     const CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteConnectionMethodL" );

    if( ProtectionLevel() == EProtLevel1 ||
        ProtectionLevel() == EProtLevel3 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
    
    if ( ConnectionMethodCount() == 1 )
        {
        // Check if this destination is referenced by Virtual CM 
        CheckIfReferencedByVirtualCML();
        }
        
    TInt index = FindConnectionMethodL( aConnectionMethod );

    // move this method to the 'to be deleted' array, but
    // set remove only from, not to delete it on update.
    CPluginItem* item = (*(iData->iConnMethodArray))[index];
    
    item->iFlags |= CPluginItem::ERemoveCm;
   
    iData->iDeleteCmArray->AppendL( item );
    iData->iConnMethodArray->Delete( index );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfReferencedByVirtualCML()
// -----------------------------------------------------------------------------
// 
void CCmDestinationImpl::CheckIfReferencedByVirtualCML()
    {
    // for each IAP in CM manager
    //   1. check if it is virtual
    //      if not => goto 1.
    //      if yes:
    //      2. check if it links to the destination of this CM
    //         if yes => carryOn = EFalse, ERROR
    //         if not: carryOn = ETrue
    CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* iaps = iCmMgr.AllIapsL();
    CleanupStack::PushL( iaps );

    TBool carryOn = ETrue;
    TUint32 destId = Id();

    // for each IAP in CM manager
    for ( TInt i = KCmInitIndex; carryOn && i < iaps->iRecords.Count(); ++i )
        {
        CommsDat::CCDIAPRecord* rec = (*iaps)[i];
        TUint32 bearerType = 0;

        TRAP_IGNORE( bearerType = iCmMgr.BearerTypeFromIapRecordL( rec ) );
        if ( !bearerType )
            {
            continue;
            }

        // check if it is virtual
        if ( iCmMgr.GetBearerInfoBoolL( bearerType, ECmVirtual ) )
            {
            // check if it links to the current destination
            CCmPluginBase* plugin = NULL;            
            TRAP_IGNORE( plugin = iCmMgr.GetConnectionMethodL( rec->RecordId() ) );

            if ( !plugin )
                {
                continue;
                }

            if ( plugin->IsLinkedToSnap( destId ) )
                {
                // the CM links to this destination, deletion not allowed
                carryOn = EFalse;
                }

            delete plugin;
            }
        }

    CleanupStack::PopAndDestroy( iaps );

    if ( !carryOn )
        {
        User::Leave( KErrLocked );
        }
    }
// -----------------------------------------------------------------------------
// CCmDestinationImpl::RemoveConnectionMethodL()
// -----------------------------------------------------------------------------
// 
void CCmDestinationImpl::RemoveConnectionMethodL( 
                                     const CCmPluginBaseEng& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteConnectionMethodL" );

    if( ProtectionLevel() == EProtLevel1 ||
        ProtectionLevel() == EProtLevel3 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    TInt index = FindConnectionMethodL( aConnectionMethod );

    // move this method to the 'to be deleted' array, but
    // set remove only from, not to delete it on update.
    CPluginItem* item = (*(iData->iConnMethodArray))[index];
    
    item->iFlags |= CPluginItem::ERemoveCm;
   
    iData->iDeleteCmArray->AppendL( item );
    iData->iConnMethodArray->Delete( index );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodDeletedL()
// -----------------------------------------------------------------------------
// 
void CCmDestinationImpl::ConnectionMethodDeletedL( 
                                    const CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodDeletedL" );

    TInt found( KErrNotFound );
    TUint32 aCmId = aConnectionMethod.GetIntAttributeL( ECmId );

    found = ConnectionMethodIndex( aCmId );

    if( found != KErrNotFound )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[found];
        
        (*(iData->iConnMethodArray))[found] = NULL;
        iData->iConnMethodArray->Delete( found );
        iData->iConnMethodArray->Compress();
        
        // The iPlugin is deleted in plugin interface function
        // that called this function.
        delete item->iDNRecord;
        delete item;
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::FindConnectionMethodL()
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationImpl::FindConnectionMethodL( 
                                     const CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::FindConnectionMethodL" );

    TInt cmId = aConnectionMethod.GetIntAttributeL( ECmId );
    TBool embeded = aConnectionMethod.GetBoolAttributeL( ECmDestination ) ?
                      ETrue :
                      EFalse;

    CLOG_WRITE_2( "ID: [%d], embDest: [%d]", 
                  cmId, 
                  aConnectionMethod.GetBoolAttributeL( ECmDestination ) );
    
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        if( embeded )
            {
            if( item->iDNRecord->iEmbeddedSNAP == cmId )
                {
                return i;
                }        
            }
        else
            {
            if( RECORD_FROM_ELEMENT_ID(item->iDNRecord->iIAP) == cmId )
                {
                return i;
                }
            }
        }
        
    CLOG_WRITE( "Not found" );
    User::Leave( KErrNotFound );
    
    // dummy return value. never used.
    return -1;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::FindConnectionMethodL()
// -----------------------------------------------------------------------------
// 
TInt CCmDestinationImpl::FindConnectionMethodL( 
                                     const CCmPluginBaseEng& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::FindConnectionMethodL" );

    TInt cmId = aConnectionMethod.GetIntAttributeL( ECmId );
    TBool embeded = aConnectionMethod.GetBoolAttributeL( ECmDestination ) ?
                      ETrue :
                      EFalse;

    CLOG_WRITE_2( "ID: [%d], embDest: [%d]", 
                  cmId, 
                  aConnectionMethod.GetBoolAttributeL( ECmDestination ) );
    
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        if( embeded )
            {
            if( item->iDNRecord->iEmbeddedSNAP == cmId )
                {
                return i;
                }        
            }
        else
            {
            if( RECORD_FROM_ELEMENT_ID(item->iDNRecord->iIAP) == cmId )
                {
                return i;
                }
            }
        }
        
    CLOG_WRITE( "Not found" );
    User::Leave( KErrNotFound );
    
    // dummy return value. never used.
    return -1;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ModifyPriorityL()
// -----------------------------------------------------------------------------
//      
void CCmDestinationImpl::ModifyPriorityL( CCmPluginBase& aCCMItem, 
                                          TUint aIndex )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ModifyPriorityL" );
    
    if (aCCMItem.GetBoolAttributeL(ECmDestination) ||
        (aCCMItem.GetBoolAttributeL(ECmVirtual) &&
         aCCMItem.GetIntAttributeL(ECmNextLayerSNAPId)))
        {
        // Priority can not be changed for this cm
        return;
        }
        
    if( ProtectionLevel() == EProtLevel1 ||
        ProtectionLevel() == EProtLevel3 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    TBool found( EFalse );
    TInt cm2 = aCCMItem.GetIntAttributeL( ECmId );
    
    if ( aIndex >= ( iData->iConnMethodArray->Count() - KCmInitIndex ) )
        {
        User::Leave( KErrArgument );
        }
    
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); i++ )
        {
        LoadPluginL( *(*(iData->iConnMethodArray))[i] );
            
        // Compare the IAP IDs
        TInt cm1 = (*(iData->iConnMethodArray))[i]->iPlugin->GetIntAttributeL( ECmId );        
        
        if ( cm1 == cm2 )
            {
            CPluginItem* item = (*(iData->iConnMethodArray))[i];
            
            TInt pri = (*(iData->iConnMethodArray))[i]->iDNRecord->iPriority;
            if ( pri == KDataMobilitySelectionPolicyPriorityWildCard )
                {
                TUint32 prio = 0;
                TUint32 bearerType = 0;
                TUint32 extLevel = 0;
                
                TBool isVirtual = PrioDataL( item, prio, bearerType, extLevel );
                if (prio != KDataMobilitySelectionPolicyPriorityWildCard)
                    {
                    // The priority will be changed later when UpdateL is called
                    // So, the priority will simply be chnaged from
                    // KDataMobilitySelectionPolicyPriorityWildCard to value i
                    (*(iData->iConnMethodArray))[i]->iDNRecord->iPriority = i; 
                    }
                }
            
            (*(iData->iConnMethodArray))[i] = NULL;
            
            // remove from array
            iData->iConnMethodArray->Delete( i );
            iData->iConnMethodArray->Compress();
            iData->iConnMethodArray->InsertL( KCmInitIndex + aIndex, item );
            
            found = ETrue;           
            break;
            }
        }
        
    if ( !found )
        {
        User::Leave( KErrArgument );
        }
        
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::PriorityL()
// -----------------------------------------------------------------------------
//          
TUint CCmDestinationImpl::PriorityL( CCmPluginBaseEng& aCCMItem )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::PriorityL" );

    TInt index = FindConnectionMethodL( aCCMItem );
    
    return (*(iData->iConnMethodArray))[index]->iDNRecord->iPriority;
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::PriorityL()
// -----------------------------------------------------------------------------
//          
TUint CCmDestinationImpl::PriorityL( CCmPluginBase& aCCMItem )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::PriorityL" );

    TInt index = FindConnectionMethodL( aCCMItem );
    
    return (*(iData->iConnMethodArray))[index]->iDNRecord->iPriority;
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void CCmDestinationImpl::UpdateL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateL" );

    // Connection methods and destination records are updated in 
    // one transaction.
    OpenTransactionLC();
    
    DeleteConnectionMethodsL();

    // Update protection level of connection methods if it's changed in 
    // the destination
    UpdateProtectionL();
    // Updates all modified connection methods.
    UpdateConnectionMethodsL();
    UpdateNetworkRecordL();
    UpdateDestAPRecordL();
    UpdateSNAPTableL();
    UpdateSNAPMetadataTableL();
        
    CommitTransactionL( KErrNone );
    
    // set the last updated protection level of the destination
    iData->iLastProtectionLevel = ProtectionLevel();

    // Clear the modified flag
    iData->iProtectionChanged = EFalse;

    // empty the array, but do not delete it
    iData->ResetAndDestroyArray( iData->iDeleteCmArray, EFalse );
        
    iCmMgr.DestinationUpdated(this);
    iData->iIdIsValid = ETrue;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::DeleteLD()
// -----------------------------------------------------------------------------
//
EXPORT_C void CCmDestinationImpl::DeleteLD()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteLD" );

    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    TInt count = iData->iConnMethodArray->Count();

    if( IsConnectedL() )
        {
        User::Leave( KErrInUse );
        }

    // Check first that any of the CMs in this Destination is not
    // referenced by Virtual CM
    TInt i( 0 );
    
    for( i = KCmInitIndex; i < count; ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        
        LoadPluginL( *item );
            
        if( item->iPlugin->GetBoolAttributeL( ECmIsLinked ) )
            {
            User::Leave( KErrLocked );
            }
        }
    // Check that this Destination is not referenced by any Virtual CM
    if ( ConnectionMethodCount() > 0 )
        {
        CheckIfReferencedByVirtualCML();
        }

    OpenTransactionLC();
    TRAPD( err, DeleteRelatedRecordsL( count ) );
    if ( err )
        {
        RollbackTransaction();
        User::Leave( err );
        }
    CommitTransactionL(KErrNone);

    delete this;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::DeleteRelatedRecordsL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::DeleteRelatedRecordsL( TInt aCount )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteRelatedRecordsL" );

    CleanUpEmbeddedL( Id() );

    TCmDefConnValue deletedItem;
    deletedItem.iType = ECmDefConnDestination;
    deletedItem.iId = Id();
    iCmMgr.HandleDefConnDeletedL( deletedItem );       
    CMDBRecordBase* dnIapRecord;
    TInt i;
    
    // Here we don't use KCmInitIndex becouse initial entry should be deleted
    // too!
    for ( i = 0; i < aCount; i++ )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        
        // Delete the plugin
        if ( i >= KCmInitIndex )
            {
            if ( !item->iPlugin->Plugin()->IdIsValid() )
                {
                // Delete incomplete plugin 
                delete item->iPlugin;
                item->iPlugin = NULL;
                }
            else
                {
                TRAPD( err, item->iPlugin->DeleteL( EFalse ) );
                
                if( err != KErrLocked &&
                    err != KErrNotReady )
                    {
                    User::LeaveIfError( err );
                    }
                    
                delete item->iPlugin;
                item->iPlugin = NULL;
                }
            }  
        
        // Delete the DN-IAP record
        dnIapRecord = item->iDNRecord;
        
        if ( dnIapRecord->RecordId() )
            // If we have record Id, this record is already in CommsDat
            {
            dnIapRecord->DeleteL( Session() );
            }   
        delete item->iDNRecord;
        item->iDNRecord = NULL;
        }

    iData->iDestAPRecord->DeleteL( Session() ); // deletes a record
    iData->iNetworkRecord->DeleteL( Session() ); // deletes a record
    
    // Delete SNAPMetadata table    
    CMDBRecordSet<CCDSNAPMetadataRecord>*  ptrRecordSet = 
                    new (ELeave) CMDBRecordSet<CCDSNAPMetadataRecord>( 
                                                 iCmMgr.IconTableId() );
    CleanupStack::PushL( ptrRecordSet );

    // Prime record
    CCDSNAPMetadataRecord* record = 
        new (ELeave) CCDSNAPMetadataRecord( iCmMgr.IconTableId() );
    CleanupStack::PushL( record );
    record->iSNAP = Id();
    ptrRecordSet->iRecords.AppendL( record );

    CleanupStack::Pop( record );
    record = NULL;
                
    if ( ptrRecordSet->FindL( Session() ) )
        {
        (*ptrRecordSet)[0]->DeleteL( Session() );          
        }
        
    ptrRecordSet->iRecords.ResetAndDestroy();
    CleanupStack::PopAndDestroy( ptrRecordSet );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfEmbeddedL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::CheckIfEmbeddedL( TInt aId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfEmbeddedL" );
    
    CMDBRecordSet<CCDSnapRecord>*  ptrRecordSet = 
                    new (ELeave) CMDBRecordSet<CCDSnapRecord>( 
                                          iCmMgr.DestinationTableId() );
    CleanupStack::PushL( ptrRecordSet );

    // Prime record
    CCDSnapRecord* record = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( record );
    record->iEmbeddedSNAP = aId;
    ptrRecordSet->iRecords.AppendL( record );

    CleanupStack::Pop( record );
    record = NULL;
        
    OpenTransactionLC();    
    if( ptrRecordSet->FindL( Session() ) )
        // Match found - it's embedded
        {
        User::Leave( KErrNotSupported );          
        }
    RollbackTransaction();

    ptrRecordSet->iRecords.ResetAndDestroy();
    
    CleanupStack::PopAndDestroy( ptrRecordSet );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::HasEmbeddedL
// -----------------------------------------------------------------------------
//
TBool CCmDestinationImpl::HasEmbeddedL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::HasEmbeddedL" );

    return ( EmbeddedL() != NULL )? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::EmbeddedL
// -----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmDestinationImpl::EmbeddedL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::EmbeddedL" );

    CCmDestinationImpl* retVal = NULL;
    // Check if the destination contains embedded destinations
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); i++ )
        {
        LoadPluginL( *(*(iData->iConnMethodArray))[i] );
        // It's an embedded destination ( this is also true for VPN CMs )
        retVal = (*(iData->iConnMethodArray))[i]->iPlugin->Destination();
        if ( retVal )
            {
            // as there can be only one
            // embedded destination, the first one is enough
            break;
            }
        }
    
    return retVal;
    }
 
 // -----------------------------------------------------------------------------
// CCmDestinationImpl::CanUseToConnectL
// -----------------------------------------------------------------------------
//
TBool CCmDestinationImpl::CanUseToConnectL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CanUseToConnectL" );

    const TInt KZeroItem = 0;
    const TInt KOneItem = 1;
    TInt cmCount = ConnectionMethodCount(); //without KCmInitIndex !!!
    switch ( cmCount )
        {
        case KZeroItem:// no items at all
            {
            return EFalse;
            }
        case KOneItem:// One item, a possible empty embedded destination!
            {
            CCmDestinationImpl* embeddedDest = EmbeddedL();
            if ( embeddedDest )
                {
                // If embeddedDest->ConnectionMethodCount() is not zero,
                // we have something in the embedded destination
                // which cannot be another embedded destination, 
                // since multiple embedding is not allowed.
                return embeddedDest->ConnectionMethodCount();
                }
             else
                {
                return ETrue; //not an embedded destination
                }
            }
        default: // more than one item, only one can be an empty embedded
                 // destination, so we surely contain a valid cm
            {
            return ETrue;
            }
            
        }
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateNetworkRecordL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateNetworkRecordL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateNetworkRecordL" );

    if ( !iData->iNetworkRecord->RecordId() || iData->iNewWithId )
        {
        CLOG_WRITE( "New Destination, create it!" );
        //Create a network record, set the host name and add it to the database    

        if ( !iData->iNewWithId )
        	{
        	iData->iNetworkRecord->SetRecordId(KCDNewRecordRequest);
        	}

        SetAttribute( iData->iNetworkRecord, 
                      ECDProtectedWrite, 
                      ProtectionLevel() == EProtLevel1 );

        iData->iNetworkRecord->StoreL( Session() ); //adds a new record
        iData->iNewWithId = EFalse;
        }
    else
        {
        SetAttribute( iData->iNetworkRecord, 
                      ECDProtectedWrite, 
                      ProtectionLevel() == EProtLevel1 );

        iData->iNetworkRecord->ModifyL( Session() );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateDestAPRecordL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateDestAPRecordL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateDestAPRecordL" );

    if ( !iData->iDestAPRecord->RecordId() )
        {
        CLOG_WRITE( "New Destination, create AP for it!" );
        //Create an AP record, set the selection policy(network id)
        //and add it to the database
        iData->iDestAPRecord->SetRecordId(KCDNewRecordRequest);
        iData->iDestAPRecord->iRecordTag = CreateDestinationAPTagIdL();
        iData->iDestAPRecord->iCustomSelectionPolicy = iData->iNetworkRecord->RecordId();
        iData->iDestAPRecord->iRecordName.SetL(iData->iNetworkRecord->iRecordName);
        iData->iDestAPRecord->StoreL( Session() ); //adds a new record
        }
    else
        {
        iData->iDestAPRecord->iCustomSelectionPolicy = iData->iNetworkRecord->RecordId();
        iData->iDestAPRecord->iRecordName.SetL(iData->iNetworkRecord->iRecordName);
        iData->iDestAPRecord->ModifyL( Session() ); //modifies the existing
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CreateDestinationAPTagId()
// -----------------------------------------------------------------------------
//
TUint32 CCmDestinationImpl::CreateDestinationAPTagIdL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CreateDestinationAPTagId" );

    return KCmDefaultDestinationAPTagId + iData->iNetworkRecord->RecordId();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::GetDestinationAPTagId()
// -----------------------------------------------------------------------------
//
TUint32 CCmDestinationImpl::GetDestinationAPTagId( )
    {
    return iData->iNetworkRecord->RecordId() + KCmDefaultDestinationAPTagId;
    }


// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateProtectionL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateProtectionL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateProtectionL" );

    if( !iData->iProtectionChanged )
        // protection level not changed -> nothing to update here
        {
        return;
        }
        
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); i++ )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        
        LoadPluginL( *item );
        
        switch ( ProtectionLevel() ) 
            {
            case EProtLevel0:
            case EProtLevel2:
                {
                // from the protection level 0 to 2 and vice versa 
                // the ECmProtected value of the CM is not changed                
                if( iData->iLastProtectionLevel == EProtLevel1 ||
                    iData->iLastProtectionLevel == EProtLevel3 )
                    {
                    item->iPlugin->SetBoolAttributeL( ECmProtected, EFalse );
                    }
                }
              break;
            case EProtLevel1:
            case EProtLevel3:
                {
                item->iPlugin->SetBoolAttributeL( ECmProtected, ETrue );
                }
              break;
            default:
                {
                //
                }
              break;              
             }                      
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateConnectionMethodsL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateConnectionMethodsL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateConnectionMethodsL" );

    for( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        
        // If the plugin is not loaded, it means that this a old one, or nothing
        // modified in it -> there's no need to update it.
        if( item->iPlugin )
            {
            item->iPlugin->UpdateL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetAttribute()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::SetAttribute( CMDBRecordBase* aRecord, 
                                       TUint32 aAttribute, TBool aSet )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetAttribute" );
    
    if ( aSet && !aRecord->IsSetAttribute( aAttribute ) )
        {
        aRecord->SetAttributes( aAttribute );
        }
    else if( !aSet && aRecord->IsSetAttribute( aAttribute ) )
        {
        aRecord->ClearAttributes( aAttribute );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateSNAPTableL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateSNAPTableL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateSNAPTableL" );
    
    TInt i;
    CCDSnapRecord* dnIapRecord;

    // Now we have only records of which are new or needs to be updated
    for ( i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        TInt pri = (*(iData->iConnMethodArray))[i]->iDNRecord->iPriority;

        if ( pri != i &&
             pri != KDataMobilitySelectionPolicyPriorityWildCard )
            // wildcard priority CM have to be the last one in the array.
            // In theory they cannot be re-prioritized.
            {
            (*(iData->iConnMethodArray))[i]->iDNRecord->iPriority = i;
            }
        }

    // Here we don't use KCmInitIndex becouse initial entry should be updated 
    // too!
    for ( i = 0; i < iData->iConnMethodArray->Count(); ++i )
        {
        dnIapRecord = (*(iData->iConnMethodArray))[i]->iDNRecord;
        
        if( dnIapRecord->iIAP )
            {
            if( !RECORD_FROM_ELEMENT_ID( dnIapRecord->iIAP ) )
                {
                dnIapRecord->iIAP = (*(iData->iConnMethodArray))[i]->iPlugin->IAPRecordElementId();
                }
            }
        
        // Set protection on connection methods 
        SetAttribute( dnIapRecord, 
                      ECDProtectedWrite, 
                      ProtectionLevel() == EProtLevel1 ? ETrue : EFalse );
        // Metadata is used instead of ECDHidden Attribute                      
        //SetAttribute( dnIapRecord, ECDHidden, iData->iNetworkRecord->Attributes() & ECDHidden );
        CheckIfNameModifiedL( dnIapRecord );
             
        if ( dnIapRecord->RecordId() )
            {           
            dnIapRecord->ModifyL( Session() );
            }
        else
            // this is a new record
            {
            dnIapRecord->SetRecordId(KCDNewRecordRequest);
            dnIapRecord->iSNAP = Id();
            dnIapRecord->StoreL( Session() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::UpdateSNAPMetadataTableL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::UpdateSNAPMetadataTableL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::UpdateSNAPMetadataTableL" );
    
    if( !iData->iMetaDataRec->RecordId() )
        // new record
        {
        iData->iMetaDataRec->iSNAP = Id();
        iData->iMetaDataRec->SetRecordId( KCDNewRecordRequest );
        iData->iMetaDataRec->StoreL( Session() );
        }
    else
        {
        iData->iMetaDataRec->ModifyL( Session() );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadSNAPMetadataTableL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::LoadSNAPMetadataTableL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadSNAPMetadataTableL" );

    iData->iMetaDataRec = new (ELeave) CCDSNAPMetadataRecord( 
                                            iCmMgr.IconTableId() );

    iData->iMetaDataRec->iSNAP = Id();
                
    if ( !iData->iMetaDataRec->FindL( Session() ) )
        // Not found -> fill in with default values
        {
        iData->iMetaDataRec->iSNAP = Id();
        iData->iMetaDataRec->iIcon = 0;
        iData->iMetaDataRec->iMetadata = 0;
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadNetworkRecordL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::LoadNetworkRecordL( TUint32 /*aDestinationId*/ )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadNetworkRecordL" );

    // set the record ID
    iData->iNetworkRecord->SetRecordId( iData->iDestAPRecord->iCustomSelectionPolicy );
  
    // load the record ID
    iData->iNetworkRecord->LoadL( Session() );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadDestAPRecordL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::LoadDestAPRecordL( TUint32 aDestinationId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadDestAPRecordL" );

    if ( aDestinationId > KCmDefaultDestinationAPTagId )
        {
        // set the record ID
        iData->iDestAPRecord->iRecordTag = aDestinationId;
        // load the record ID
        if ( iData->iDestAPRecord->FindL( Session() ) )
            {
            iData->iDestAPRecord->LoadL( Session() );
            }
        else
            {
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadConnectionMethodsL()
// -----------------------------------------------------------------------------
//    
void CCmDestinationImpl::LoadConnectionMethodsL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadConnectionMethodsL" );
    
    CMDBRecordSet<CCDSnapRecord>*  ptrRecordSet = 
                        new (ELeave) CMDBRecordSet<CCDSnapRecord>( 
                                          iCmMgr.DestinationTableId() );
    CleanupStack::PushL( ptrRecordSet );

    // Prime record
    CCDSnapRecord* snapRec = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( snapRec );
    snapRec->iSNAP = Id();
    ptrRecordSet->iRecords.AppendL( snapRec );

    CleanupStack::Pop( snapRec );
    snapRec = NULL;

    OpenTransactionLC();            
    if ( !ptrRecordSet->FindL(Session()) )
        // no conn method is to this destination -> it was not us who made it
        {
        CLOG_WRITE( "Empty destination" );
        User::Leave( KErrUnderflow );
        }

    TLinearOrder<CMDBRecordBase> orderbyId( 
                                    CCmDestinationImpl::SortRecordsByPriority );
    ptrRecordSet->iRecords.Sort(orderbyId);

    TInt count = ptrRecordSet->iRecords.Count();
    
    //Load IAP records to be used to check if the IAPs are existing
    CMDBRecordSet<CCDIAPRecord>*  ptrIapSet = 
                    new (ELeave) CMDBRecordSet<CCDIAPRecord>( KCDTIdIAPRecord );
    CleanupStack::PushL( ptrIapSet );
    
    TRAPD( err, ptrIapSet->LoadL( Session() ) );

    if( !err )
        {
        CLOG_WRITE_1_PTR( NULL, "Record num [%d]", ptrIapSet->iRecords.Count() );
        
        // Here we don't use CM_INIT_INDEX because initial entry should be 
        // loaded too!
        for ( TInt i = 0; i < count; i++ )
            {
            snapRec = (*ptrRecordSet)[i];
            TBool exist( EFalse );
            
            // Check if CM really exists, if not 
            // it should be cleared from selection policy table
            TInt32 embedded = QUERY_INT_FIELD( 
                                snapRec, 
                                KCDTIdDataMobilitySelectionPolicyEmbeddedSNAP );
            TInt32 iap = RECORD_FROM_ELEMENT_ID( 
                            QUERY_INT_FIELD( 
                                       snapRec, 
                                       KCDTIdDataMobilitySelectionPolicyIAP ) );
            // Item could be embedded destination or iap
            if ( embedded )
                {
                CCDAccessPointRecord * destAPRecord = static_cast<CCDAccessPointRecord *>(
                                CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
                CleanupStack::PushL( destAPRecord );
                destAPRecord->iRecordTag = embedded;
                if ( !destAPRecord->FindL( Session() ) )
                    {
                    CleanUpSnapMetadataTableL( embedded );
                    (*ptrRecordSet)[i]->DeleteL( Session() );
                    }
                else
                    {
                    exist = ETrue;
                    }
                    
                CleanupStack::PopAndDestroy( destAPRecord );
                }                
            else if ( iap )
                // This is an AP
                {
                for ( TInt iapInd = 0; iapInd < ptrIapSet->iRecords.Count(); ++iapInd )
                    {
                    CMDBRecordBase* iapRec = (*ptrIapSet)[iapInd];

                    if ( iap == iapRec->RecordId() )
                        {
                        exist = ETrue;
                        delete iapRec;
                        // this way we can filter out duplicated CMs
                        ptrIapSet->iRecords.Remove( iapInd );
                        break;
                        }
                    }
                if ( !exist )
                    {
                    DeleteIAPRecordsL( snapRec );
                    }
                }
            
            // The first record is a dummy one where iap and snap are zero
            if ( exist || ( !embedded && !iap ) )
                {
                // The item may have to be loaded at this stage so that the supported can be checked 
                // before it is put into iData->iConnMethodArray for the item may be unsupported.
                TInt err( 0 );
                
                if ( i != 0 )
                    {
                    if ( embedded )
                        {
                        TRAP( err, TUint32 bearerType = iCmMgr.BearerTypeFromCmIdL( embedded ) );
                        }
                    else if ( iap )
                        {
                        TRAP( err, TUint32 bearerType = iCmMgr.BearerTypeFromCmIdL( iap ) );
                        }
                    }
               
                if ( !err )
                    {
                    CPluginItem* item = new (ELeave) CPluginItem;
                    CleanupStack::PushL( item );
                    
                    item->iDNRecord = iCmMgr.SNAPRecordL( snapRec->RecordId() );
                    item->iPlugin = NULL;

                    iData->iConnMethodArray->AppendL( item );
                
                    CleanupStack::Pop( item );
                    }
                }
            }
        }
    else
        // There's no connection method in the phone
        {
        CPluginItem* item = new (ELeave) CPluginItem;
        
        CleanupStack::PushL( item );
        
        item->iDNRecord = iCmMgr.SNAPRecordL( (*ptrRecordSet)[0]->RecordId() );

        iData->iConnMethodArray->AppendL( item );       
        
        CleanupStack::Pop( item );                 
        }
        
    CleanupStack::PopAndDestroy( ptrIapSet );
    RollbackTransaction();
        
    ptrRecordSet->iRecords.ResetAndDestroy();
    CleanupStack::PopAndDestroy( ptrRecordSet );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ReLoadConnectionMethodsL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::ReLoadConnectionMethodsL()
    {
    TInt count = iData->iConnMethodArray->Count();
    //TInt index = KCmInitIndex;
    TInt index = 0;
    
    while ( index < count )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[index];
        item->iFlags |= CPluginItem::ERemoveCm;
        
        iData->iDeleteCmArray->AppendL( item );
        iData->iConnMethodArray->Delete( index );
        iData->iConnMethodArray->Compress();
        
        count = iData->iConnMethodArray->Count();
        }
    
    // Empty the array, but do not delete it
    iData->ResetAndDestroyArray( iData->iDeleteCmArray, EFalse );
    
    LoadConnectionMethodsL();
    }
    
// ---------------------------------------------------------
// CCmDestinationImpl::CleanUpEmbeddedL
// ---------------------------------------------------------
//
void CCmDestinationImpl::CleanUpEmbeddedL(TUint32 aDestId)
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CleanUpEmbeddedL" );

    TBool found = EFalse;
    CMDBRecordSet<CCDSnapRecord>*  ptrRecordSet = 
                        new (ELeave) CMDBRecordSet<CCDSnapRecord>( 
                                          iCmMgr.DestinationTableId() );
    CleanupStack::PushL( ptrRecordSet );
    // Prime record
    CCDSnapRecord* snapRec = iCmMgr.SNAPRecordL( 0 );
    CleanupStack::PushL( snapRec );
    snapRec->iEmbeddedSNAP = aDestId;
    ptrRecordSet->iRecords.AppendL( snapRec );
    CleanupStack::Pop( snapRec );
    snapRec = NULL;

    if ( ptrRecordSet->FindL(Session()) )
        {
        for ( TInt i = 0; i <ptrRecordSet->iRecords.Count(); i++ )
            {
            (*ptrRecordSet)[i]->DeleteL( Session() );
            }
        found = ETrue;
        }
    CleanupStack::PopAndDestroy( ptrRecordSet );
    if (found)
        {
        CleanUpSnapMetadataTableL(aDestId);    
        }
    }
// ---------------------------------------------------------
// CCmDestinationImpl::CleanUpSnapMetadataTableL
// ---------------------------------------------------------
//
void CCmDestinationImpl::CleanUpSnapMetadataTableL( TUint32 aSnap )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CleanUpSnapMetadataTableL" );
    
    // Just to test how many record we have
    CMDBRecordSet<CCDSNAPMetadataRecord>*  ptrRecordSet 
                = new (ELeave) CMDBRecordSet<CCDSNAPMetadataRecord>( 
                                                iCmMgr.IconTableId() );
    CleanupStack::PushL( ptrRecordSet );
        
    // Prime record
    CCDSNAPMetadataRecord* record = 
            new (ELeave) CCDSNAPMetadataRecord( iCmMgr.IconTableId() );
    CleanupStack::PushL( record );
    
    record->iSNAP = aSnap;
    ptrRecordSet->iRecords.AppendL( record );
    
    CleanupStack::Pop( record );
    record = NULL;
    
    if ( ptrRecordSet->FindL( Session() ) )
        {
        for ( TInt i = 0; i <ptrRecordSet->iRecords.Count(); i++ )
            {
            (*ptrRecordSet)[i]->DeleteL( Session() );
            }
        }
        
    CleanupStack::PopAndDestroy( ptrRecordSet );    //ptrRecordSet 
    }

// ---------------------------------------------------------
// CCmDestinationImpl::CleanUpSeamlessnessTableL
// ---------------------------------------------------------
//
void CCmDestinationImpl::CleanUpSeamlessnessTableL( TUint32 aIapId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CleanUpSeamlessnesstableL" );
    
    // Just to test how many record we have
    CMDBRecordSet<CCDIAPMetadataRecord>*  ptrRecordSet 
        = new (ELeave) CMDBRecordSet<CCDIAPMetadataRecord>( 
                                        iCmMgr.SeamlessnessTableId() );
    CleanupStack::PushL( ptrRecordSet );
        
    // Prime record
    CCDIAPMetadataRecord* record = 
      new (ELeave) CCDIAPMetadataRecord( iCmMgr.SeamlessnessTableId() );
    CleanupStack::PushL( record );
    
    TMDBElementId elementid = iCmMgr.SeamlessnessTableId() & 
                              KCDTIdIAPMetadataIAP & 
                              aIapId;
    record->iSeamlessness = elementid;
    ptrRecordSet->iRecords.AppendL( record );
    
    CleanupStack::Pop( record );
    record = NULL;
    
    if ( ptrRecordSet->FindL( Session() ) )
        {
        for ( TInt i = 0; i <ptrRecordSet->iRecords.Count(); i++ )
            {
            (*ptrRecordSet)[i]->DeleteL( Session() );
            }
        }
        
    CleanupStack::PopAndDestroy( ptrRecordSet );
    }
    
// ---------------------------------------------------------
// CCmDestinationImpl::SortRecordsByPriority
// ---------------------------------------------------------
//
TInt CCmDestinationImpl::SortRecordsByPriority( const CMDBRecordBase& aLeft, 
                                                const CMDBRecordBase& aRight )
    {
    TUint left = QUERY_INT_FIELD( const_cast<CMDBRecordBase*>(&aLeft), 
                                  KCDTIdDataMobilitySelectionPolicyPriority );
    TUint right = QUERY_INT_FIELD( const_cast<CMDBRecordBase*>(&aRight),
                                   KCDTIdDataMobilitySelectionPolicyPriority );

    /*
    *   - negative value, if aLeft is less than aRight;
    *   - 0,              if aLeft equals to aRight;
    *   - positive value, if aLeft is greater than aRight.
    */
    if ( left == right )
        {
        return 0;
        }
    
    return ( left < right ) ? -1 : 1;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::IconL()
// -----------------------------------------------------------------------------
//      
EXPORT_C CGulIcon* CCmDestinationImpl::IconL() 
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::IconL" );

    return IconL( iData->iMetaDataRec->iIcon );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetIconL()
// -----------------------------------------------------------------------------
//      
void CCmDestinationImpl::SetIconL( TUint32 anIndex )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetIconL" )
    
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    iData->iMetaDataRec->iIcon = anIndex;
    }

// -----------------------------------------------------------------------------
// CCmDestinationData::MetadataL()
// -----------------------------------------------------------------------------
//
TUint32 CCmDestinationData::MetadataL( TSnapMetadataField aMetaField ) const
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::MetadataL" )

    TUint retVal( 0 );

    switch( aMetaField )
        {
        case ESnapMetadataInternet:
            {
            retVal = TUint32(iMetaDataRec->iMetadata) & 
                     ESnapMetadataInternet;
            }
            break;
            
        case ESnapMetadataHighlight:
            {
            retVal = TUint32(iMetaDataRec->iMetadata) & 
                     ESnapMetadataHighlight;
            }
            break;
            
        case ESnapMetadataHiddenAgent:
            {
            retVal = TUint32(iMetaDataRec->iMetadata) &
                     ESnapMetadataHiddenAgent;
            }
            break;

        case ESnapMetadataDestinationIsLocalised:
            {
            retVal = (TUint32(iMetaDataRec->iMetadata) &
                      ESnapMetadataDestinationIsLocalised) >> 4;
            }
            break;
            
        case ESnapMetadataPurpose:
            {
            retVal = (TUint32(iMetaDataRec->iMetadata) &
                     ESnapMetadataPurpose) >> 8;
            }
            break;

        default:
            {
            User::Leave( KErrArgument );
            }
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::MetadataL()
// -----------------------------------------------------------------------------
//
TUint32 CCmDestinationImpl::MetadataL( TSnapMetadataField aMetaField ) const
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::MetadataL" )
    return iData->MetadataL(aMetaField);
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetMetadataL()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::SetMetadataL( TSnapMetadataField aMetaField, 
		                               TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetMetadata" );
    
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    switch( aMetaField )
        {
        case ESnapMetadataInternet:
            {
            if( aValue )
                {
                CheckIfInternetExistsL();
                
                iData->iMetaDataRec->iMetadata = 
                        ESnapMetadataInternet | iData->iMetaDataRec->iMetadata;

                // Set the new way, too!!!
                SetMetadataL( ESnapMetadataDestinationIsLocalised, 
                              ELocalisedDestInternet );
                }
            else
                {
                iData->iMetaDataRec->iMetadata =
                        ~ESnapMetadataInternet & iData->iMetaDataRec->iMetadata;
                // Clear the new way, too!!!
                SetMetadataL( ESnapMetadataDestinationIsLocalised, 
                              ENotLocalisedDest );                
                }
            }
            break;

        case ESnapMetadataDestinationIsLocalised:
            {
            // Check that aValue is within valid range.
            if ( aValue > ELocalisedDestIntranet )
                {
                User::Leave( KErrArgument );
                }
            
            TUint32 oldVal = TUint32(iData->iMetaDataRec->iMetadata) & 
                             ESnapMetadataInternet;
            if ( aValue != ENotLocalisedDest )
                {
                // check if already exists...
                CheckIfLocalisedDestExistL( aValue );
                // does not exists, first clear original value
                // if old one was Internet, clear that, too:
                if ( oldVal )
                    {
                    // copied here to avoid recursivity!!!
                    // SetMetadataL(ESnapMetadataInternet, 0 );
                    iData->iMetaDataRec->iMetadata =
                        ~ESnapMetadataInternet & iData->iMetaDataRec->iMetadata;
                    }
                // clearing
                iData->iMetaDataRec->iMetadata =
                                                ~ESnapMetadataDestinationIsLocalised &
                                                iData->iMetaDataRec->iMetadata;
                // set new value
                TUint32 mask = aValue << 4; 
                iData->iMetaDataRec->iMetadata = 
                                                mask | 
                                                iData->iMetaDataRec->iMetadata;
                // for backward compatibility, if it is internet, 
                // set it the old way, too
                if ( aValue == ELocalisedDestInternet )
                    {
                    // copied here to avoid recursivity!!!
                    iData->iMetaDataRec->iMetadata = 
                        ESnapMetadataInternet | iData->iMetaDataRec->iMetadata;
                    // SetMetadataL(ESnapMetadataInternet, 1 );
                    }
                }
            else
                {
                // not localised, set to ENotLocalisedDest = 0
                // for backward compatibility, if it was internet, 
                // set it the old way, too
                if ( oldVal )
                    {
                    SetMetadataL(ESnapMetadataInternet, 0 );
                    }
                // clear the new way...
                iData->iMetaDataRec->iMetadata =
                                                ~ESnapMetadataDestinationIsLocalised &
                                                iData->iMetaDataRec->iMetadata;
                }

            break;
            }

        case ESnapMetadataPurpose:
            {
            // Check that aValue is within valid range.
            if ( aValue > ESnapPurposeIntranet )
                {
                User::Leave( KErrArgument );
                }
            
            // Check if destination with the given purpose already
            // exists
            CheckIfPurposeExistL( aValue );
            
            // ESnapPurposeInternet and ESnapPurposeMMS need special
            // handling
            switch( aValue )
                {
                case ESnapPurposeUnknown:
                    {
                    // This also clears ESnapMetadataInternet
                    SetMetadataL( ESnapMetadataDestinationIsLocalised, ENotLocalisedDest );
                    break;
                    }
                case ESnapPurposeInternet:
                    {
                    // This sets ESnapMetadataInternet and
                    // ELocalisedDestInternet
                    SetMetadataL( ESnapMetadataInternet, 1 );
                    }
                    break;
                case ESnapPurposeOperator:
                    {
                    SetMetadataL( ESnapMetadataDestinationIsLocalised, 
                                  ELocalisedDestWap );
                    }
                	break;
                case ESnapPurposeMMS:
                    {
                    SetMetadataL( ESnapMetadataDestinationIsLocalised, 
                                  ELocalisedDestMMS );
                    iData->iMetaDataRec->iMetadata = 
                        ESnapMetadataHiddenAgent | iData->iMetaDataRec->iMetadata;
                    }
                    break;
                case ESnapPurposeIntranet:
                    {
                    SetMetadataL( ESnapMetadataDestinationIsLocalised,
                                  ELocalisedDestIntranet );
                    }
                    break;
                default:
                    break;
                }
            
            TUint32 value = aValue << 8;
            // reset the purpose bit
            iData->iMetaDataRec->iMetadata = ~ESnapMetadataPurpose & iData->iMetaDataRec->iMetadata;
            // set the purpose bit
            iData->iMetaDataRec->iMetadata = value | iData->iMetaDataRec->iMetadata;
            }
            break;

        case ESnapMetadataHighlight:
            {
            if( aValue )
                {
                iData->iMetaDataRec->iMetadata = 
                        ESnapMetadataHighlight | iData->iMetaDataRec->iMetadata;
                }
            else
                {
                iData->iMetaDataRec->iMetadata =
                        ~ESnapMetadataHighlight & iData->iMetaDataRec->iMetadata;
                }
            }
            break;
            
        case ESnapMetadataHiddenAgent:
            {
            TUint32 internet = iData->iMetaDataRec->iMetadata & ESnapMetadataInternet;
            TUint32 internetLocal = ( iData->iMetaDataRec->iMetadata & ESnapMetadataDestinationIsLocalised ) >> 4;
            TUint32 internetPurpose = ( iData->iMetaDataRec->iMetadata & ESnapMetadataPurpose ) >> 8;
            if( aValue )
                {
                if ( internet
                     || ( internetLocal == ELocalisedDestInternet )
                     || ( internetPurpose == ESnapPurposeInternet ) )
                    {
                    User::Leave( KErrArgument );
                    }

                iData->iMetaDataRec->iMetadata = 
                        ESnapMetadataHiddenAgent | iData->iMetaDataRec->iMetadata;
                }
            else
                {
                iData->iMetaDataRec->iMetadata = 
                        ~ESnapMetadataHiddenAgent & iData->iMetaDataRec->iMetadata;
                }
            }
            break;
            
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    }
// -----------------------------------------------------------------------------
// CCmDestinationImpl::CreateEmbeddedDestinationPluginL()
// -----------------------------------------------------------------------------
//      
void CCmDestinationImpl::CreateEmbeddedDestinationPluginL( 
                                                        CPluginItem& aItem, 
                                                        TInt aDestinationId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CreateEmbeddedDestinationPluginL" );

    aItem.iPlugin = iCmMgr.CreateConnectionMethodL(KUidEmbeddedDestination, this); 
    CleanupStack::PushL( aItem.iPlugin );
    // Load this destination
    aItem.iPlugin->LoadL( aDestinationId );
    CleanupStack::Pop( aItem.iPlugin );        
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadEmbeddedDestinationPluginL()
// -----------------------------------------------------------------------------
//      
void CCmDestinationImpl::LoadEmbeddedDestinationPluginL( 
                                                        CPluginItem& aItem )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadEmbeddedDestinationPluginL" );

    CreateEmbeddedDestinationPluginL(aItem, aItem.iDNRecord->ElementId());
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::LoadPluginL()
// -----------------------------------------------------------------------------
//      
void CCmDestinationImpl::LoadPluginL( CPluginItem& aItem )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::LoadPluginL" );
    
    if( aItem.iPlugin )
        {
        return;
        }
        
    TUint32 cmId;
    if ( aItem.iDNRecord->iEmbeddedSNAP )
        // This is an embedded destination
        {
        cmId = aItem.iDNRecord->iEmbeddedSNAP;
        
        }
    else
        // Normal IAP id
        {
        cmId = RECORD_FROM_ELEMENT_ID( aItem.iDNRecord->iIAP );
        }

    aItem.iPlugin = iCmMgr.DoFindConnMethodL( 
                        cmId, 
                        this );
                        
    CLOG_ATTACH( aItem.iPlugin, this );
    }    

// -----------------------------------------------------------------------------
// CCmDestinationImpl::PluginClosed()
// -----------------------------------------------------------------------------
//
void CCmDestinationImpl::PluginClosed( CCmPluginBase* aPlugin )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::PluginClosed" );

    if (iData->iConnMethodArray)
        {
        for ( TInt i = 0; i < iData->iConnMethodArray->Count(); ++i )
            {
            if ( (*(iData->iConnMethodArray))[i]->iPlugin == aPlugin )
                {
                // Must NOT delete it here. Simply forget that
                // we had this plugin instance.
                (*(iData->iConnMethodArray))[i]->iPlugin = NULL;
                
                break;
                }
            }        
        }
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationData::IsHiddenL()
// -----------------------------------------------------------------------------
//          
TBool CCmDestinationData::IsHiddenL()
    {
    TSnapMetadataField hiddenValue = ( TSnapMetadataField )MetadataL( ESnapMetadataHiddenAgent );
    if ( hiddenValue == ESnapMetadataHiddenAgent )
        {
    	return ETrue;
        }
    else
        {
    	return EFalse;
        }
    }    
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::IsHiddenL()
// -----------------------------------------------------------------------------
//          
EXPORT_C TBool CCmDestinationImpl::IsHidden()
    {
    TBool hidden( EFalse );
    TRAPD( err, hidden = iData->IsHiddenL() );
    __ASSERT_ALWAYS( err == KErrNone, User::Panic(_L( "CMManager" ), err ) );
    
    return hidden;
    }    

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetHiddenL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void CCmDestinationImpl::SetHiddenL( TBool aHidden )
    {
    if( ProtectionLevel() == EProtLevel1 )
        {
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
    SetMetadataL( ESnapMetadataHiddenAgent, aHidden );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::DeleteConnectionMethodsL
//
// Can be called only during opened transaction. 
// Array is emptied only after transaction commited!
//
// -----------------------------------------------------------------------------
//          
void CCmDestinationImpl::DeleteConnectionMethodsL()
    {
    LOGGER_ENTERFN( "DeleteConnectionMethodsL" );
    
    TInt i;
    
    for ( i = 0; i < iData->iDeleteCmArray->Count(); ++i )
        {
        CPluginItem* item = (*iData->iDeleteCmArray)[i];
        
        LoadPluginL( *item );   

        if( !(item->iFlags & CPluginItem::ERemoveCm) )
            {
            item->iPlugin->DeleteL( EFalse );
            }
            
        item->iDNRecord->DeleteL( Session() );
        }
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationData::ResetAndDestroyArray
// -----------------------------------------------------------------------------
//          
void CCmDestinationData::ResetAndDestroyArray( CCmArray* &aArray, TBool aDestroy )
    {
    if( aArray )
        {
        for ( TInt i = 0; i < aArray->Count(); ++i )
            {
            CPluginItem* item = (*aArray)[i];
            
            delete item->iPlugin;
            delete item->iDNRecord;
            }
            
        aArray->ResetAndDestroy();
        
        if( aDestroy )
            {
            delete aArray; aArray = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::DeleteIAPRecordsL
// -----------------------------------------------------------------------------
//          
void CCmDestinationImpl::DeleteIAPRecordsL( CMDBRecordBase* aSnapRecord )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::DeleteIAPRecordsL" );

    // First delete the metadata record
    CCDIAPMetadataRecord* metaRec = new (ELeave) CCDIAPMetadataRecord( 
                                        iCmMgr.SeamlessnessTableId() );
                                        
    CleanupStack::PushL( metaRec );

    metaRec->iIAP = RECORD_FROM_ELEMENT_ID( 
                                QUERY_INT_FIELD( 
                                       aSnapRecord, 
                                       KCDTIdDataMobilitySelectionPolicyIAP ) );

    if( metaRec->FindL( Session() ) )
        {
        metaRec->DeleteL( Session() );
        }
        
    CleanupStack::PopAndDestroy( metaRec );
    
    // and now delete the SNAP record
    aSnapRecord->DeleteL( Session() );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfNameModifiedL
// -----------------------------------------------------------------------------
void CCmDestinationImpl::CheckIfNameModifiedL( CCDSnapRecord* aRecord )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfNameModifiedL" );
            
    if ( !TPtrC(iData->iDestAPRecord->iRecordName).CompareF( TPtrC(aRecord->iRecordName) ) )
        // names matches
        {
        return;
        }
    
    aRecord->iRecordName.SetL( TPtrC(iData->iDestAPRecord->iRecordName) );
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::IsConnectedL
// -----------------------------------------------------------------------------
EXPORT_C TBool CCmDestinationImpl::IsConnectedL() const
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::IsConnectedL" );

    CCmDestinationImpl* myThis = const_cast<CCmDestinationImpl*>( this );
    TBool retVal( EFalse );
    TInt i;
    
    for( i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        
        myThis->LoadPluginL( *item );
            
        if( item->iPlugin->GetBoolAttributeL( ECmConnected ) )
            {
            retVal = ETrue;
            break;
            }
        }
        
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfInternetExistsL
//
// There can be only one SNAP with Internet metadata field set.
// -----------------------------------------------------------------------------
void CCmDestinationImpl::CheckIfInternetExistsL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfInternetExistsL" );

    OpenTransactionLC();

    CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet = 
           new ( ELeave ) CMDBRecordSet<CCDSNAPMetadataRecord>( 
                                                 iCmMgr.IconTableId() );
    CleanupStack::PushL( metaSet );

    TRAP_IGNORE( metaSet->LoadL( Session() ) );

    for ( TInt i = 0; i < metaSet->iRecords.Count(); ++i )
        {
        TInt meta = QUERY_INT_FIELD( metaSet->iRecords[i], 
                                     KCDTIdSNAPMetadataMetadata );

        if( meta & ESnapMetadataInternet )
            {
            if( Id() != QUERY_INT_FIELD( metaSet->iRecords[i], 
                                                KCDTIdSNAPMetadataSNAP ) )
                {
                User::Leave( KErrAlreadyExists );
                }
            }
        }
    
    CleanupStack::PopAndDestroy( metaSet );
    
    RollbackTransaction();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ConnectionMethodIndex
// -----------------------------------------------------------------------------
TInt CCmDestinationImpl::ConnectionMethodIndex( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::ConnectionMethodIndex" );

    TInt found( KErrNotFound );
    
    for ( TInt i = KCmInitIndex; i < iData->iConnMethodArray->Count(); ++i )
        {
        CPluginItem* item = (*(iData->iConnMethodArray))[i];
        if ( RECORD_FROM_ELEMENT_ID(item->iDNRecord->iIAP) == aCmId ||
             (item->iDNRecord->iEmbeddedSNAP == aCmId && aCmId != 0) )
            {
            found = i;
            break;
            }        
        }
        
    return found;
    }        

// -----------------------------------------------------------------------------
// CCmDestinationImpl::IconIdL
// -----------------------------------------------------------------------------
CGulIcon* CCmDestinationImpl::IconIdL( CCmManagerImpl& aCmMgr,
                                       TMDBElementId aId )
    {
    CGulIcon* retVal = NULL;
    CCDSNAPMetadataRecord* metaDataRec = new (ELeave) CCDSNAPMetadataRecord( 
                                                        aCmMgr.IconTableId() );
    CleanupStack::PushL( metaDataRec );
    metaDataRec->iSNAP = aId;
                
    if ( metaDataRec->FindL( aCmMgr.Session() ) )
        // Not found -> fill in with default values
        {
        retVal = IconL( metaDataRec->iIcon );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( metaDataRec );
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ProtectionLevelL
// -----------------------------------------------------------------------------
TProtectionLevel CCmDestinationImpl::ProtectionLevelL( CCmManagerImpl& aCmMgr, 
                                                       TMDBElementId aId ) 
    {
    TProtectionLevel retVal( EProtLevel0 );
    CCDSNAPMetadataRecord* metaDataRec = new (ELeave) CCDSNAPMetadataRecord( 
                                                        aCmMgr.IconTableId() );
    CleanupStack::PushL( metaDataRec );
    metaDataRec->iSNAP = aId;
                
    if ( metaDataRec->FindL( aCmMgr.Session() ) )
        // Not found -> fill in with default values
        {
        retVal = ProtectionLevel( metaDataRec );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PopAndDestroy( metaDataRec );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ProtectionLevel
// -----------------------------------------------------------------------------
TProtectionLevel CCmDestinationImpl::ProtectionLevel( 
                                           CCDSNAPMetadataRecord* aMetaDataRec )
    {
    TInt protection = TUint32(aMetaDataRec->iMetadata) & 
                      KSnapMetadataProtecionLevelMask;
    return TProtectionLevel(protection >> 28);
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::ProtectionLevelL
// -----------------------------------------------------------------------------
CGulIcon* CCmDestinationImpl::IconL( TInt aIconIndex )
    {
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
           
    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                                     &KDC_BITMAP_DIR, 
                                     NULL ) );
                                     
    TAknsItemID aknItemID;
    aknItemID.Set( KDestinationIconConvTable[aIconIndex][0],
                   KDestinationIconConvTable[aIconIndex][1] );
           
    CGulIcon* icon = AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            aknItemID,
                            mbmFile.FullName(), 
                            KDestinationIconConvTable[aIconIndex][2], 
                            KDestinationIconConvTable[aIconIndex][3] );
                            
    return icon;
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::RemoveFromPool
// -----------------------------------------------------------------------------
EXPORT_C void CCmDestinationImpl::RemoveFromPool()
    {
    iCmMgr.RemoveDestFromPool(this);            
    }
    
// -----------------------------------------------------------------------------
// CCmDestinationImpl::IncrementRefCounter
// -----------------------------------------------------------------------------
void CCmDestinationImpl::IncrementRefCounter()
    {
    iData->IncrementRefCounter();
    }
// -----------------------------------------------------------------------------
// CCmDestinationImpl::DecrementRefCounter
// -----------------------------------------------------------------------------
void CCmDestinationImpl::DecrementRefCounter()
    {
    iData->DecrementRefCounter();
    }
// -----------------------------------------------------------------------------
// CCmDestinationData::IncrementRefCounter
// -----------------------------------------------------------------------------
void CCmDestinationData::IncrementRefCounter()
    {
    iRefCounter++;
    }
// -----------------------------------------------------------------------------
// CCmDestinationData::DecrementRefCounter
// -----------------------------------------------------------------------------
void CCmDestinationData::DecrementRefCounter()
    {
    iRefCounter--;
    }

// -----------------------------------------------------------------------------
// CCmDestinationData::GetRefCounter
// -----------------------------------------------------------------------------
TInt CCmDestinationData::GetRefCounter()
    {
    return iRefCounter;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::GetRefCounter
// -----------------------------------------------------------------------------
TInt CCmDestinationImpl::GetRefCounter()
    {
    return iData->iRefCounter;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::GetData
// -----------------------------------------------------------------------------
CCmDestinationData* CCmDestinationImpl::GetData()
    {
    return iData;
    }
// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetData
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetData(CCmDestinationData* aData)
    {
    iData = aData;
    }

// -----------------------------------------------------------------------------
// CCmDestinationData::IdIsValid
// -----------------------------------------------------------------------------
TBool CCmDestinationData::IdIsValid()
    {
    return iIdIsValid;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::IdIsValid
// -----------------------------------------------------------------------------
TBool CCmDestinationImpl::IdIsValid()
    {
    return iData->iIdIsValid;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::Cmmgr
// -----------------------------------------------------------------------------
CCmManagerImpl* CCmDestinationData::Cmmgr()
    {
    return &iCmMgr;    
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::Cmmgr
// -----------------------------------------------------------------------------
CCmManagerImpl* CCmDestinationImpl::Cmmgr()
    {
    return &iCmMgr;    
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfLocalisedDestExistL
//
// There can be only one SNAP with a certain localised metadata field set.
// -----------------------------------------------------------------------------
void CCmDestinationImpl::CheckIfLocalisedDestExistL( TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfLocalisedDestExistL" );

    OpenTransactionLC();

    CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet = 
                new ( ELeave ) CMDBRecordSet<CCDSNAPMetadataRecord>( iCmMgr.IconTableId() );
    CleanupStack::PushL( metaSet );

    TRAP_IGNORE( metaSet->LoadL( Session() ) );

    for ( TInt i = 0; i < metaSet->iRecords.Count(); ++i )
        {
        TInt meta = QUERY_INT_FIELD( metaSet->iRecords[i], 
                                     KCDTIdSNAPMetadataMetadata );

        if ( meta & ESnapMetadataDestinationIsLocalised )
            {
            TInt locval = (meta & ESnapMetadataDestinationIsLocalised) >> 4;
            if ( aValue == locval )
                {
                if( Id() != QUERY_INT_FIELD( metaSet->iRecords[i], KCDTIdSNAPMetadataSNAP ) )
                    {
                    User::Leave( KErrAlreadyExists );
                    }
                }
            if ( aValue ==  ELocalisedDestInternet )
                { // internet might also be set the 'old way', check it!
                if( meta & ESnapMetadataInternet )
                    {
                    if( Id() != QUERY_INT_FIELD( metaSet->iRecords[i], KCDTIdSNAPMetadataSNAP ) )
                        {
                        User::Leave( KErrAlreadyExists );
                        }
                    }                
                }
            }            
        }

    CleanupStack::PopAndDestroy( metaSet );

    RollbackTransaction();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfPurposeExistL
//
// There can be only one SNAP with a certain purpose metadata field set.
// -----------------------------------------------------------------------------
void CCmDestinationImpl::CheckIfPurposeExistL( TUint32 aValue )
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::CheckIfPurposeExistL" );

    OpenTransactionLC();

    CMDBRecordSet<CCDSNAPMetadataRecord>* metaSet = 
                new ( ELeave ) CMDBRecordSet<CCDSNAPMetadataRecord>( iCmMgr.IconTableId() );
    CleanupStack::PushL( metaSet );

    TRAP_IGNORE( metaSet->LoadL( Session() ) );

    for ( TInt i = 0; i < metaSet->iRecords.Count(); ++i )
        {
        TInt meta = QUERY_INT_FIELD( metaSet->iRecords[i], 
                                     KCDTIdSNAPMetadataMetadata );

        if ( meta & ESnapMetadataPurpose )
            {
            TInt purVal = ( meta & ESnapMetadataPurpose ) >> 8;
            if ( aValue == purVal )
                {
                if( Id() != QUERY_INT_FIELD( metaSet->iRecords[i], 
                                        KCDTIdSNAPMetadataSNAP ) )
                    {
                    User::Leave( KErrAlreadyExists );
                    }
                }
            }            
        }

    CleanupStack::PopAndDestroy( metaSet );

    RollbackTransaction();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::InitializeDestAPRecordL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::InitializeDestAPRecordL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::InitializeDestAPRecordL" );

    SetDefaultTierManagerL();
    SetDefaultMCprL();
    SetDefaultCprL();
    SetDefaultSCprL();
    SetDefaultProtocolL();
    iData->iDestAPRecord->iCustomSelectionPolicy = 0;
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetDefaultTierManagerL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetDefaultTierManagerL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetDefaultTierManagerL" );

    CCDTierRecord* tierRec = static_cast<CCDTierRecord *>
                               ( CCDRecordBase::RecordFactoryL( KCDTIdTierRecord ) );
    CleanupStack::PushL( tierRec );

    tierRec->iRecordTag = KDefaultTierManagerTagId;

    if ( tierRec->FindL( Session() ) )
        {
        iData->iDestAPRecord->iTier = tierRec->ElementId();
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetDefaultMCprL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetDefaultMCprL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetDefaultMCprL" );

    CCDMCprRecord* mcprRec = static_cast<CCDMCprRecord *>
                             ( CCDRecordBase::RecordFactoryL( KCDTIdMCprRecord ) );
    CleanupStack::PushL( mcprRec );

    mcprRec->SetRecordId( 1 );

    mcprRec->LoadL( Session() );
	
	iData->iDestAPRecord->iMCpr = mcprRec->ElementId();

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetDefaultCprL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetDefaultCprL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetDefaultCprL" );

    CCDCprRecord* cprRec = static_cast<CCDCprRecord *>
                             ( CCDRecordBase::RecordFactoryL( KCDTIdCprRecord ) );
    CleanupStack::PushL( cprRec );

    cprRec->SetRecordId( 1 );

    cprRec->LoadL( Session() );
	iData->iDestAPRecord->iCpr = cprRec->ElementId();

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetDefaultSCprL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetDefaultSCprL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetDefaultSCprL" );

    CCDSCprRecord* scprRec = static_cast<CCDSCprRecord *>
                             ( CCDRecordBase::RecordFactoryL( KCDTIdSCprRecord ) );
    CleanupStack::PushL( scprRec );

    scprRec->SetRecordId( 1 );

    scprRec->LoadL( Session() );
	iData->iDestAPRecord->iSCpr = scprRec->ElementId();

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::SetDefaultProtocolL
//
// 
// -----------------------------------------------------------------------------
void CCmDestinationImpl::SetDefaultProtocolL()
    {
    LOGGER_ENTERFN( "CCmDestinationImpl::SetDefaultProtocolL" );

    CCDProtocolRecord* protocolRec = static_cast<CCDProtocolRecord *>
                             ( CCDRecordBase::RecordFactoryL( KCDTIdProtocolRecord ) );
    CleanupStack::PushL( protocolRec );

    protocolRec->SetRecordId( 1 );

    protocolRec->LoadL( Session() );
	iData->iDestAPRecord->iProtocol = protocolRec->ElementId();

    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCmDestinationImpl::CheckIfCMExists
//
// 
// -----------------------------------------------------------------------------
TBool CCmDestinationImpl::CheckIfCMExistsL( CCmPluginBase& aConnectionMethod )
    {
    TRAPD( err, (void)FindConnectionMethodL( aConnectionMethod ) );
    if ( err )
        {
        if ( err == KErrNotFound )
            {
            return EFalse;
            }
        else
            {
            User::Leave( err );
            }
        }

    return ETrue;
    }
