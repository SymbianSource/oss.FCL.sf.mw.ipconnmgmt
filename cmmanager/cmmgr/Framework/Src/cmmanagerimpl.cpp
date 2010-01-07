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
* Description:  Implementation of CCmManagerImpl.
*
*/

#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include <cmdestination.h>
#include <cmdestinationext.h>
#include "cmdestinationimpl.h"
#include "cmtransactionhandler.h"
#include "cmlogger.h"
#include "cmmanager.hrh"
#include <cmcommonconstants.h>
#include <cmmanagerext.h>
#include <cmpluginembdestinationdef.h>
#include "cmobjectpool.h"
#include "cmcommsdatnotifier.h"

#include <sysutil.h>
#include <featmgr.h>
#include <wlancontainer.h>  // CCDWlanServiceRecord
#include <StringLoader.h>
#include <commsdattypesv1_1.h>
#include <e32cmn.h>
#include <bautils.h>        // BaflUtils
#include <cmmanager.rsg>    // resource
#include <data_caging_path_literals.hrh>
#include <ecom/ecom.h>        // For REComSession
#include <cmmanagerext.h> 

#include <cmmanager.mbg>   // icons
#include <AknsUtils.h>

#include <commsdattypesv1_1.h>
#include <rconnmon.h>

using namespace CMManager;
using namespace CommsDat;

// CONSTANTS

const TInt KResFileGranularity = 5;
const TInt KPluginGranularity = 3;
// if a CmId is grater than KMaxIapId, than it has no iap record
// Otherwise cmId is the same as the IapId
const TUint32 KMaxIapID = 255;
// Safety margin when checking disk space
const TUint32 KBytesToWrite = 8000; 

const TUint32 KEndOfArray = KMaxTUint;
/**
* This array lists all the bearerspecific attributes.
* The bearerspecific attributes can be queried via both 
* the manager interface's and connection method interface's ::Get functions.
*/
static const TUint32 KBearerSpecificAttributes[]=
    {
    /**
    */
    ECmCoverage,                        
    ECmDefaultPriority,                        
    ECmDestination,     
    ECmBearerHasUi,   
    ECmIPv6Supported,           
    ECmDefaultUiPriority,
    ECmBearerIcon,
    ECmBearerAvailableIcon,
    ECmBearerAvailableName,
    ECmBearerSupportedName,
    ECmBearerAvailableText,                        
    ECmBearerNamePopupNote,
    ECmCommsDBBearerType,   
    ECmBearerSettingName,
    ECmVirtual,
    ECmExtensionLevel,
    ECmAddToAvailableList,
    KEndOfArray
    };

// -----------------------------------------------------------------------------
// Test functions
// -----------------------------------------------------------------------------
//
/*#ifdef _DEBUG
void DumIapTableL( CMDBSession& aSession )
    {
    CMDBRecordSet<CCDIAPRecord>*  ptrRecordSet = 
                    new (ELeave) CMDBRecordSet<CCDIAPRecord>( KCDTIdIAPRecord );
    CleanupStack::PushL( ptrRecordSet );
    
    TRAP_IGNORE( ptrRecordSet->LoadL( aSession ) );
    
    TInt iapRecords = ptrRecordSet->iRecords.Count();
    CLOG_WRITE_1_PTR( NULL, "Dumping IAP Table\nNo. of IAPs: [%d]", iapRecords );
    for ( TInt i = 0; i < iapRecords; i++ )
        {
        CCDIAPRecord* iapRecord = (*ptrRecordSet)[i];

        CLOG_WRITE_4_PTR( NULL, "Rec id:[%d] - Name: [%S] - Service Type: [%S] - Bearer Type: [%S]",
                                iapRecord->RecordId(),
                                &FIELD_TO_TDESC( iapRecord->iRecordName ),
                                &FIELD_TO_TDESC( iapRecord->iServiceType ),
                                &FIELD_TO_TDESC( iapRecord->iBearerType )
                                );
        }

    CleanupStack::PopAndDestroy( ptrRecordSet );
    }
#endif */ // _DEBUG

//=============================================================================
// CONSTRUCTION / DESTRUCTION API
//=============================================================================
//

//-----------------------------------------------------------------------------
//  CCmManagerImpl::NewL()
//-----------------------------------------------------------------------------
//
CCmManagerImpl* CCmManagerImpl::NewL( TBool aCreateTables )
    {
    CCmManagerImpl* self = new (ELeave) CCmManagerImpl();
    CleanupStack::PushL( self );
    self->iCreateTables = aCreateTables;
    self->ConstructL();
    CleanupStack::Pop( self );   // self
    
    return self;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CCmManagerImpl()
//-----------------------------------------------------------------------------
//
CCmManagerImpl::CCmManagerImpl()
:iDefConnSupported(EFalse),iCreateTables(ETrue)
    {
    CLOG_CREATE;

    Dll::SetTls( this );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CCmManagerImpl()
//-----------------------------------------------------------------------------
//
CCmManagerImpl::~CCmManagerImpl()
    {
    CLOG_WRITE( "CCmManagerImpl::~CCmManagerImpl" );
    
    if ( iBearerPriorityArray )
        {
        CleanupGlobalPriorityArray( *iBearerPriorityArray );
        delete iBearerPriorityArray;
        }

    if ( iPlugins )
        {
        iPlugins->ResetAndDestroy();
        delete iPlugins;
        }
    
    
    if ( iResourceFiles )
        {
        TInt resCount = iResourceFiles->Count();
        for ( TInt i = 0; i < resCount; ++i )
            {
            // close the resource file
            (*iResourceFiles)[0]->iFile.Close();
            // delete the structure
            delete (*iResourceFiles)[0];
            // delete array item
            iResourceFiles->Delete( 0 );
            }
            
        iResourceFiles->Reset();        
        delete iResourceFiles;
        }
    
    iFs.Close();
    
    delete iPluginImpl;

    delete &Session();
    delete iTrans;

    if( iIsFeatureManagerInitialised )
        {
        FeatureManager::UnInitializeLib();
        }

    REComSession::FinalClose();
    
    delete iObjectPool;
    
    delete iCommsDatIapNotifier;
    delete iCommsDatSnapNotifier;

    CLOG_CLOSE;
    }

      
// -----------------------------------------------------------------------------
// CCmManagerImpl::CheckTablesL()
// -----------------------------------------------------------------------------
//     
void CCmManagerImpl::CheckTablesL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CheckDestinationNetworkTableL" );
    
    TInt err( 0 );

    CLOG_WRITE( "Destination network" );
    if (iCreateTables)
        {
        TRAP( err, iDestinationTableId = 
                        CCDDataMobilitySelectionPolicyRecord::TableIdL( Session() ));
        CLOG_WRITE_1( "err: [%d]", err );
        if( err == KErrNotFound )
            {
            iDestinationTableId = 
                       CCDDataMobilitySelectionPolicyRecord::CreateTableL( Session() );
            }
        else
            {
            User::LeaveIfError( err );
            }        
        }
    else
        {
        iDestinationTableId = 
                        CCDDataMobilitySelectionPolicyRecord::TableIdL( Session() );        
        }
        
    CLOG_WRITE( "Global Bearer Type Priorization Table" );
    TRAP( err, iBearerTypePriorizationTableId = 
                CCDGlobalBearerTypePriorizationRecord::TableIdL( Session() ) );
    CLOG_WRITE_1( "err: [%d]", err );
    if( err == KErrNotFound )
        {
        iBearerTypePriorizationTableId = 
               CCDGlobalBearerTypePriorizationRecord::CreateTableL( Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    CLOG_WRITE( "Destination network - Icon table" );        
    TRAP( err, iIconTableId = CCDSNAPMetadataRecord::TableIdL( Session() ) );
    if( err == KErrNotFound )
        {
        iIconTableId = CCDSNAPMetadataRecord::CreateTableL( Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }
        
    CLOG_WRITE( "Seamlessness table" );
    TRAP( err, iSeamlessnessTableId = CCDIAPMetadataRecord::TableIdL( Session() ) );
    CLOG_WRITE_1( "err: [%d]", err );
    if( err == KErrNotFound )
        {
        iSeamlessnessTableId = CCDIAPMetadataRecord::CreateTableL( Session() );
        }
    else
        {
        User::LeaveIfError( err );
        }
    if ( IsDefConnSupported() )
        {
        CLOG_WRITE( "Default connection table" );
        TRAP( err, iDefConnTableId = CCDDefConnRecord::TableIdL( Session() ) );
        CLOG_WRITE_1( "err: [%d]", err );
        if( err == KErrNotFound )
            {
            iDefConnTableId = CCDDefConnRecord::CreateTableL( Session() );
            }
        else
            {
            User::LeaveIfError( err );
            }   
        }
    }
     
//-----------------------------------------------------------------------------
//  CCmManagerImpl::CreateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::CreateBearerPriorityArrayL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl:CreateBearerPriorityArray" );

    iBearerPriorityArray = 
                new ( ELeave ) RArray<TBearerPriority>( KCmArrayBigGranularity );

    CMDBRecordSet<CCmBearerPriRec>* bpRS = 
                new (ELeave) CMDBRecordSet<CCmBearerPriRec>( 
                                               iBearerTypePriorizationTableId );
    CleanupStack::PushL( bpRS );
        
    OpenTransactionLC();        
    TRAP_IGNORE( bpRS->LoadL( Session() ) );
    RollbackTransaction();
    
    TInt bpCount = bpRS->iRecords.Count();
    TInt i( 0 );
    
    while ( i < bpCount ) // loop the Bearer Priority table
        {
        // Bearer Priority record ID
        HBufC* bpServiceType = QUERY_HBUFC_FIELD( (*bpRS)[i], 
                                                  KCDTIdGlobalServiceType );
        
        TInt bpPriority = ( QUERY_INT_FIELD( (*bpRS)[i], 
                                             KCDTIdGlobalServicePriority ) );
        TInt uiPriority = ( QUERY_INT_FIELD( (*bpRS)[i], 
                                             KCDTIdGlobalServiceUIPriority ) );                                    
        CLOG_WRITE( "Bearer Priority Table Values" );
        CLOG_WRITE_1( "Service Type: [%S]", bpServiceType );
        CLOG_WRITE_1( "Bearer Priority: [%d]", bpPriority );
        CLOG_WRITE_1( "UI Priority: [%d]", uiPriority );                          
        TBearerPriority bp;
        bp.iServiceType = bpServiceType->AllocL();
        bp.iPriority = bpPriority;
        bp.iUIPriority = uiPriority;
        iBearerPriorityArray->AppendL( bp );
        i++;
        }
    
    CleanupStack::PopAndDestroy( bpRS );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::BearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::BearerPriorityArrayL( RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl:BearerPriorityArray" );
    TRAPD( err, CopyBearerPriorityArrayL( aArray ) );
    if( err )
        // in case of any problem we clean up this array
        {
        User::Leave( err );
        }
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CopyBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::CopyBearerPriorityArrayL( 
                                        RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl:BearerPriorityArray" );
    for ( TInt i = 0; i < iBearerPriorityArray->Count(); i++ )
        {
        TBearerPriority item;
        
        TBearerPriority* orgItem = &(*iBearerPriorityArray)[i];
        
        item.iServiceType = HBufC::NewLC( orgItem->iServiceType->Length() );
        item.iServiceType->Des().Copy( *orgItem->iServiceType );
        item.iPriority = orgItem->iPriority;
        item.iUIPriority = orgItem->iUIPriority;
                          
        User::LeaveIfError( aArray.Append( item ) );
        
        CleanupStack::Pop(); // item->iServiceType
        }
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::UpdateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::UpdateBearerPriorityArrayL( 
                                        const RArray<TBearerPriority>& aArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::UpdateBearerPriorityArray" );    
    
    OpenTransactionLC();

    CMDBRecordSet<CCmBearerPriRec>* bpRS = new (ELeave) 
            CMDBRecordSet<CCmBearerPriRec>( iBearerTypePriorizationTableId );
    CleanupStack::PushL( bpRS ); // 1
    TRAP_IGNORE( bpRS->LoadL( Session() ) );
    
    // Delete the old record
    for ( TInt i = 0; i < bpRS->iRecords.Count(); ++i )
        {
        bpRS->iRecords[i]->DeleteL( Session() );
        }
    
    CleanupGlobalPriorityArray( *iBearerPriorityArray );
            
    CleanupStack::PopAndDestroy( bpRS );
    bpRS = NULL;
    
    // Add the new content
    CCmBearerPriRec* bpRcd = 
            new (ELeave) CCmBearerPriRec( iBearerTypePriorizationTableId );
    CleanupStack::PushL( bpRcd ); // 2

    CLOG_WRITE( "Store bearer info to table" );
    for ( TInt i = 0; i < aArray.Count(); ++i )
        {        
        bpRcd->SetRecordId( KCDNewRecordRequest );
        
        HBufC* serviceTypeVal = aArray[i].iServiceType;
        TInt priorityVal = aArray[i].iPriority;
        TInt uiPriorityVal = aArray[i].iUIPriority;
        
        CLOG_WRITE_FORMAT( "Set service type: [%S   ]", serviceTypeVal );
        SET_HBUFC_FIELD(    bpRcd, 
                            KCDTIdGlobalServiceType, 
                            serviceTypeVal );
        CLOG_WRITE_1( "Set priority: [%d]", priorityVal );
        SET_INT_FIELD(  bpRcd, 
                        KCDTIdGlobalServicePriority, 
                        priorityVal );
        CLOG_WRITE_1( "Set uipriority: [%d]", uiPriorityVal );
        SET_INT_FIELD(  bpRcd, 
                        KCDTIdGlobalServiceUIPriority, 
                        uiPriorityVal );                        
        bpRcd->StoreL( Session() );
        
        TBearerPriority item;
        
        item.iServiceType = serviceTypeVal->AllocLC();
        item.iPriority = priorityVal;
        item.iUIPriority = uiPriorityVal;
        iBearerPriorityArray->AppendL( item );
        CleanupStack::Pop( item.iServiceType );
        }

    CleanupStack::PopAndDestroy( bpRcd );

    CommitTransactionL( KErrNone );
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::GlobalBearerPriority()
//-----------------------------------------------------------------------------
//
TInt CCmManagerImpl::GlobalBearerPriority( TBool aUiPriority, 
                                           const TDesC& aServiceType ) const
    {
    for ( TInt i = 0; i < iBearerPriorityArray->Count(); ++i )
        {
        if ( !aServiceType.CompareF( *(*iBearerPriorityArray)[i].iServiceType ) )
            {
            if( aUiPriority )
                {
                return (*iBearerPriorityArray)[i].iUIPriority;
                }
            else
                {
                return (*iBearerPriorityArray)[i].iPriority;
                }
            }
        }
        
    return KDataMobilitySelectionPolicyPriorityWildCard;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CleanupGlobalPriorityArray()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::CleanupGlobalPriorityArray(
                                        RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CleanupGlobalPriorityArray" );

    for ( TInt i = 0; i < aArray.Count(); ++i )
        {
        delete aArray[i].iServiceType;
        }
        
    aArray.Reset();
    aArray.Close();
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::AddResourceFileL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::AddResourceFileL( TFileName& aName )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AddResourceFileL" );
    
    TInt index = FindResourceFile( aName );
    
    if ( index != KErrNotFound )
        {
        ++(*iResourceFiles)[index]->iRefs;
        return;
        }
        
    TResourceFiles* newRes = new (ELeave) TResourceFiles;
    CleanupStack::PushL( newRes );

    RResourceFile resFile;
    resFile.OpenL( iFs, aName );
    CleanupClosePushL( resFile );
    
    resFile.ConfirmSignatureL(0); 
    
    newRes->iFile = resFile;
    newRes->iFName.Copy( aName );
    newRes->iRefs = 1;  // 1 reference to this resource file
    
    iResourceFiles->AppendL( newRes );
    
    CleanupStack::Pop( 2 ); // resFile, newRes
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::RemoveResourceFile()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::RemoveResourceFile( TFileName& aName )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveResourceFile" );

    TInt index = FindResourceFile( aName );
    
    if ( index == KErrNotFound )
        {
        return;
        }
        
    if ( !--(*iResourceFiles)[index]->iRefs )
        // it was the last reference
        {
        (*iResourceFiles)[index]->iFile.Close();
        delete (*iResourceFiles)[index];
        
        iResourceFiles->Delete( index );
        }
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::FindResourceFile()
//-----------------------------------------------------------------------------
//
TInt CCmManagerImpl::FindResourceFile( TFileName& aName ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::FindResourceFile" );

    BaflUtils::NearestLanguageFile( iFs, aName );
    
    for ( TInt i = 0; i < iResourceFiles->Count(); ++i )
        {
        if ( !(*iResourceFiles)[i]->iFName.CompareF( aName ) )
            {
            return i;
            }
        }
    
    return KErrNotFound;
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::AllocReadL()
//-----------------------------------------------------------------------------
//
HBufC* CCmManagerImpl::AllocReadL( TInt aResourceId ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AllocReadL" );

    HBufC* retVal = NULL;
    
    // Scan all resource files to find owner
    for (TInt i=0; i < iResourceFiles->Count(); i++)
        { 
        RResourceFile& file=(*iResourceFiles)[i]->iFile;
        if ( file.OwnsResourceId(aResourceId) )
            {
            HBufC8* readBuffer = file.AllocReadLC( aResourceId );
            // as we are expecting HBufC16...
            const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                         readBuffer->Length() >> 1 );
            retVal=HBufC16::NewL( ptrReadBuffer.Length() );
            retVal->Des().Copy( ptrReadBuffer );
            CleanupStack::PopAndDestroy( readBuffer ); // readBuffer
            
            // exit from loop
            break;
            }
        }

    return retVal;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::ConstructL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::ConstructL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ConstructL" );
    CLOG_NAME_1( _L("ManagerImpl_0x%x"), this );
    
    iObjectPool = new (ELeave) CCMObjectPool(*this);
            
    FeatureManager::InitializeLibL();
    iIsFeatureManagerInitialised = ETrue;
    iHelp = FeatureManager::FeatureSupported( KFeatureIdHelp ); 
    iDefConnSupported = ETrue;
    //iDefConnSupported = FeatureManager::FeatureSupported( KFeatureIdDefaultConnection );    
    iWLan = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );

    CMDBSession* db = CMDBSession::NewL( KCDVersion1_2 );
    
    iTrans = CCmTransactionHandler::NewL( *db );
    CLOG_ATTACH( iTrans, this );
    
    User::LeaveIfError( iFs.Connect() );
  
    iResourceFiles = new (ELeave) CArrayPtrFlat<TResourceFiles>( 
                                                          KResFileGranularity );
    
    TParse fp;
    TInt err = fp.Set( KACMManagerResDirAndFileName, 
                        &KDC_RESOURCE_FILES_DIR, 
                        NULL ); 
    CLOG_WRITE_1( "Set: [%d]", err );
    if ( err != KErrNone)
        {
        User::Leave( err );
        }
        
    TFileName fName;
    
    fName.Copy( fp.FullName() );
    
    AddResourceFileL( fName );
        
    CheckTablesL();
    CreateBearerPriorityArrayL();
        
    BuildPluginArrayL();
        
    TCmPluginInitParam params( *this );
    
    params.iParentDest = NULL;
    iPluginImpl = CCmConnectionMethodInfo::NewL( &params );
    CLOG_ATTACH( iPluginImpl, this );
    }

// ---------------------------------------------------------------------------
// CCmManagerImpl::OpenTransactionLC
// ---------------------------------------------------------------------------
//
void CCmManagerImpl::OpenTransactionLC( TBool aSetAttribs )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::OpenTransactionL" );

    iTrans->OpenTransactionLC(  aSetAttribs );
    }

// ---------------------------------------------------------------------------
// CCmDestinationImpl::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmManagerImpl::CommitTransactionL( TInt aError )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CommitTransactionL" );

    iTrans->CommitTransactionL( aError );
    }

// ---------------------------------------------------------------------------
// CCmDestinationImpl::CommitTransactionL
// ---------------------------------------------------------------------------
//
void CCmManagerImpl::RollbackTransaction()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RollbackTransaction" );
    CleanupStack::Pop( iTrans );
    iTrans->Close();
    }

// ---------------------------------------------------------------------------
// CCmDestinationImpl::StartCommsDatNotifier
// ---------------------------------------------------------------------------
//
void CCmManagerImpl::StartCommsDatNotifierL()
    {
    // Two instances of class CCmCommsDatNotifier are created here and
    // referred by all watchers in CmManager. One is to watch change in Iap Table
    // and the other is to watch change in Snap Table.
    iCommsDatIapNotifier = CCmCommsDatNotifier::NewL( KCDTIdIAPRecord );
    
    TUint32 snapTableId = GetSnapTableIdL();
    iCommsDatSnapNotifier = CCmCommsDatNotifier::NewL( snapTableId );
    }

//=============================================================================
// CREATION / DELETION API
//=============================================================================
//

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CreateDestinationL( const TDesC& aName )
//-----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmManagerImpl::CreateDestinationL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateDestinationL" );
    
    CheckAvailableSpaceL();
    
    if ( aName.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }
    
    return iObjectPool->GetDestinationL( aName );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::CreateDestinationL( const TDesC& aName, TUint32
//  aDestId )
//-----------------------------------------------------------------------------
//
CCmDestinationImpl* CCmManagerImpl::CreateDestinationL(
    const TDesC& aName,
    TUint32 aDestId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateDestinationL" );

    CheckAvailableSpaceL();
    
    if ( aName.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }

    if ( aDestId == 0 )
        {
        return CreateDestinationL( aName );
        }

    return iObjectPool->CreateDestinationL( aName, aDestId );
    }


//-----------------------------------------------------------------------------
//  CCmManagerImpl::DestinationUpdated( CCmDestinationImpl* aDestination )
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::DestinationUpdated( CCmDestinationImpl* aDestination )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::DestinationUpdatedL" );
    iObjectPool->DestinationUpdated( aDestination );
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::CreateConnectionMethodL()
// --------------------------------------------------------------------------
//  
CCmPluginBase* 
        CCmManagerImpl::CreateConnectionMethodL( CCmPluginBaseEng* pluginBaseEng )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateConnectionMethodL 2" );

    CheckAvailableSpaceL();        

    return iObjectPool->CreateConnectionMethodL( pluginBaseEng );
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::InsertConnectionMethodL()
// --------------------------------------------------------------------------
//
void CCmManagerImpl::InsertConnectionMethodL( CCmPluginBaseEng* pluginBaseEng )
    {
    iObjectPool->InsertConnectionMethodL( pluginBaseEng );
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::CreateConnectionMethodL()
// --------------------------------------------------------------------------
//  
CCmPluginBase* 
        CCmManagerImpl::CreateConnectionMethodL( TUint32 aImplementationUid,
                                    CCmDestinationImpl* aParentDestination )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateConnectionMethodL" );
    
    CheckAvailableSpaceL();
    
    TCmPluginInitParam params( *this );
    if (aParentDestination)
        {
        params.iParentDest = aParentDestination->Id();        
        }

    return iObjectPool->CreateConnectionMethodL( aImplementationUid, params );
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::CreateConnectionMethodL()
// --------------------------------------------------------------------------
//  
CCmPluginBase* 
        CCmManagerImpl::CreateConnectionMethodL( TUint32 aImplementationUid,
                                    TUint32 aConnMethodId,
                                    CCmDestinationImpl* aParentDestination )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateConnectionMethodL" );
    
    CheckAvailableSpaceL();
         
    TCmPluginInitParam params( *this );
    if ( aParentDestination )
        {
        params.iParentDest = aParentDestination->Id();        
        }

    return iObjectPool->CreateConnectionMethodL( aImplementationUid,
    		                                     params,
    		                                     aConnMethodId);
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::DoCreateConnectionMethodL()
// --------------------------------------------------------------------------
//  
CCmPluginBaseEng* 
        CCmManagerImpl::DoCreateConnectionMethodL( TUint32 aImplementationUid,
                                     TCmPluginInitParam& aParams )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::DoCreateConnectionMethodL" );

    const TUid KMCMmImplementationUid = { aImplementationUid };    
    TAny* ptr = REComSession::CreateImplementationL( KMCMmImplementationUid, 
                                                     _FOFF( CCmPluginBaseEng, 
                                                            iDtor_ID_Key ), 
                                                     (TAny*)&aParams );

    CCmPluginBaseEng* plugin = STATIC_CAST( CCmPluginBaseEng*, ptr );
    CleanupStack::PushL( plugin );

    plugin->CreateNewL(); 
    plugin->SetIdValidity(EFalse);   
    CleanupStack::Pop( plugin );  // plugin
        
    return plugin;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::DoCreateConnectionMethodL()
// --------------------------------------------------------------------------
//  
CCmPluginBaseEng* 
   CCmManagerImpl::DoCreateConnectionMethodL( TUint32 aImplementationUid,
                                              TCmPluginInitParam& aParams,
                                              TUint32 aConnMethodId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::DoCreateConnectionMethodL" );

    const TUid KMCMmImplementationUid = { aImplementationUid };    
    TAny* ptr = REComSession::CreateImplementationL( KMCMmImplementationUid, 
                                                    _FOFF( CCmPluginBaseEng, 
                                                           iDtor_ID_Key ), 
                                                    (TAny*)&aParams );

    CCmPluginBaseEng* plugin = STATIC_CAST( CCmPluginBaseEng*, ptr );
    CleanupStack::PushL( plugin );

    plugin->CreateNewL(); 
    plugin->SetIdValidity( EFalse );
    plugin->SetPreDefinedId( aConnMethodId );
    CleanupStack::Pop( plugin );  // plugin

    return plugin;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::ConnectionMethodL( TUint aId )
//-----------------------------------------------------------------------------
//
EXPORT_C CCmPluginBaseEng* 
                      CCmManagerImpl::ConnectionMethodL( TUint32 /*aCmId*/ )
    {
    LOGGER_ENTERFN( "Error: Obsolete function CCmManagerImpl::ConnectionMethodL used" );
    User::Panic( _L("CMManager"), KErrNotSupported );
    return NULL;    
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::GetConnectionMethodL( TUint aId )
//-----------------------------------------------------------------------------
//
EXPORT_C CCmPluginBase* 
                      CCmManagerImpl::GetConnectionMethodL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetConnectionMethodL" );
    
    CCmPluginBase* plugin = DoFindConnMethodL( aCmId, NULL );
                                      
    return plugin;
    }


//-----------------------------------------------------------------------------
//  CCmManagerImpl::AppendUncatCmListL
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::AppendUncatCmListL( RPointerArray<CCmPluginBase>& aCMArray,
                                        TBool aCheckBearerType,
                                        TBool aLegacyOnly,
                                        TBool aEasyWlan,
                                        TBool aSortByBearer )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AppendUncatCmListL" );

    RArray<TUint32> cmUidArray;
    CleanupClosePushL(cmUidArray);
    ConnectionMethodL(cmUidArray, aCheckBearerType, aLegacyOnly, aEasyWlan, aSortByBearer);
    AppendCmListFromUidsL( cmUidArray, aCMArray );

    CleanupStack::PopAndDestroy(&cmUidArray);
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::AppendUncatCmListLC
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::AppendUncatCmListLC( RPointerArray<CCmPluginBase>& aCMArray,
                                        TBool aCheckBearerType,
                                        TBool aLegacyOnly,
                                        TBool aEasyWlan,
                                        TBool aSortByBearer )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AppendUncatCmListLC" );

    AppendUncatCmListL( aCMArray, aCheckBearerType, aLegacyOnly, aEasyWlan, aSortByBearer);
    
    CleanupResetAndDestroyPushL< RPointerArray<CCmPluginBase> >( aCMArray );
    }
        
// -----------------------------------------------------------------------------
// CCmDestinationImpl::AppendCmListLC
// -----------------------------------------------------------------------------
//     
void CCmManagerImpl::AppendCmListLC( TUint32 aDestintaionId, RPointerArray<CCmPluginBase>& aCMArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AppendCmListLC" );

    AppendCmListL( aDestintaionId, aCMArray );
    CCmManagerImpl::CleanupResetAndDestroyPushL< RPointerArray<CCmPluginBase> >( aCMArray );
    }
         
// -----------------------------------------------------------------------------
// CCmDestinationImpl::AppendCmListL
// -----------------------------------------------------------------------------
//     
void CCmManagerImpl::AppendCmListL( TUint32 aDestintaionId, RPointerArray<CCmPluginBase>& aCMArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AppendCmListL" );

    CCmDestinationImpl* dest = DestinationL( aDestintaionId );
    CleanupStack::PushL(dest); 
    RArray<TUint32> cmIdList;
    CleanupClosePushL(cmIdList);
    dest->ConnectMethodIdArrayL( cmIdList );
    AppendCmListFromUidsL( cmIdList, aCMArray );
    CleanupStack::PopAndDestroy(&cmIdList);  
    CleanupStack::PopAndDestroy(dest);  
    }
// -----------------------------------------------------------------------------
// CCmDestinationImpl::AppendCmListFromUidsL
// -----------------------------------------------------------------------------
//     
void CCmManagerImpl::AppendCmListFromUidsL( RArray<TUint32>& cmIdList, RPointerArray<CCmPluginBase>& aCMArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AppendCmListFromUidsL" );

    TInt count = cmIdList.Count();
    for ( TInt i = 0; i< count; i++ )
        {
        CCmPluginBase* cm = NULL;
        TRAPD( err,cm = GetConnectionMethodL( cmIdList[i] ));
        if (err == KErrNone)
            {
            aCMArray.AppendL( cm );
            }
        }
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::ConnectionMethodL
//-----------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::ConnectionMethodL( RArray<TUint32>& aCMArray,
                                        TBool aCheckBearerType,
                                        TBool aLegacyOnly,
                                        TBool aEasyWlan,
                                        TBool aSortByBearer )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ConnectionMethodL" );

    OpenTransactionLC();
    
    CMDBRecordSet<CCDIAPRecord>* ptrRecordSet = AllIapsL();
    CleanupStack::PushL( ptrRecordSet );
    
    TInt iapRecords = ptrRecordSet->iRecords.Count();
    CLOG_WRITE_1_PTR( NULL, "Record num [%d]", iapRecords );
    
    // Time optimization to load the entire destination table
    TInt destTblId = DestinationTableId();
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>*  ptrDestSet = 
            new (ELeave) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                                                                    destTblId );
    CleanupStack::PushL( ptrDestSet );
    TRAP_IGNORE( ptrDestSet->LoadL( Session() ) );
    
    for ( TInt i = 0; i < iapRecords; i++ )
        {
        TUint32 iapId = (*ptrRecordSet)[i]->RecordId();
        CLOG_WRITE_1_PTR( NULL, "IAP ID [%d]", iapId );
        if ( aCheckBearerType )
            {
            TUint32 bearerType( 0 );
            TRAPD( err, 
                   bearerType = BearerTypeFromIapRecordL( (*ptrRecordSet)[i] ));
            if ( !err && bearerType )
                {
                // append the IAP id - if aLegacyOnly is ETrue, only for legacy IAPs
                if ( !aLegacyOnly || 
                     !NumOfConnMethodReferencesL( iapId, ptrDestSet ) )
                    {
                    if( !aEasyWlan && IsEasyWLanL( (*ptrRecordSet)[i] ) )
                        {
                        continue;
                        }
                        
                    aCMArray.Append( iapId );
                    }
                }
            }
        else
            {
            // append the IAP id - if aLegacyOnly is ETrue, only for legacy IAPs
            if ( !aLegacyOnly ||
                 !NumOfConnMethodReferencesL( iapId, ptrDestSet ) )
                {
                if( !aEasyWlan && IsEasyWLanL( (*ptrRecordSet)[i] ) )
                    {
                    continue;
                    }
                    
                aCMArray.Append( iapId );
                }
            }
        }
    
    // Re-order the array according to the global bearer ordering rules
    if ( aSortByBearer )
        {        
        RArray<TUint32> bearers( KCmArrayMediumGranularity );
        CleanupClosePushL( bearers );
        SupportedBearersL( bearers );
        
        TInt prevInsertedPos = -1;
        for ( TInt i = 0; i < bearers.Count(); i++ )
            {
            for ( TInt j = prevInsertedPos + 1; j < aCMArray.Count(); j++ )
                {
                // Check if CM belongs to this bearer
                TUint cmBearerId = GetConnectionMethodInfoIntL( aCMArray[j],
                                                             ECmBearerType );
                if ( cmBearerId == bearers[i] )
                    {
                    // copy the cm id to the next 'sorted position' in the array
                    // the item removed will always be after the insertion position
                    TUint cmId = aCMArray[j];
                    aCMArray.Remove(j);                    
                    aCMArray.Insert( cmId, ++prevInsertedPos );
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( &bearers );
        }
    
    CleanupStack::PopAndDestroy( 2, ptrRecordSet );
    
    RollbackTransaction();
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::DoFindConnMethodL
//-----------------------------------------------------------------------------
//
CCmPluginBase* CCmManagerImpl::DoFindConnMethodL( TUint32 aCmId, 
                                      CCmDestinationImpl* aParentDest )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::DoFindConnMethodL" );
    return iObjectPool->GetConnectionMethodL(aCmId, aParentDest);        
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::FindConnMethodL
//-----------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmManagerImpl::DoFindConnMethL( TUint32 aCmId, 
                                       CCmDestinationImpl* aParentDest )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::FindConnMethodL" );

    // Step through each bearer to find which the CMId belongs to
    CCmPluginBaseEng* plugin = NULL;
    
    TCmPluginInitParam params( *this );
    if (aParentDest)
        {
        params.iParentDest = aParentDest->Id();        
        }
    else
        {
        params.iParentDest = ParentDestinationL( aCmId );        
        }
        
    TUint32 bearerType = BearerTypeFromCmIdL( aCmId );

    for ( TInt i=0; i<iPlugins->Count(); i++ )
        {
        if( (*iPlugins)[i]->GetIntAttributeL( ECmBearerType ) == bearerType )
            {
            plugin = (*iPlugins)[i]->CreateInstanceL( params );
            break;
            }
        }    
        
    if ( !plugin )
        {
        return plugin;
        }
    
    CleanupStack::PushL( plugin );
    
    plugin->LoadL( aCmId );
                    
    CleanupStack::Pop( plugin ); // 1
    plugin->SetIdValidity(ETrue);
        
    return plugin;
    }
 
//-----------------------------------------------------------------------------
//  CCmManagerImpl::BearerTypeFromIapL( TUint aId )
//-----------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::BearerTypeFromCmIdL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::BearerTypeFromIapL" );
    TUint32 bearerType;
    if ( aCmId > KMaxIapID )
        {
        bearerType = KUidEmbeddedDestination;  
        }
    else
        {//aCmId is an IapId
         // Load this IAP record from the IAP table
        // This is an optimization that plugins doesn't have
        // to do it every time the CanHandleIapIdL() is called.    
        CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord *>
                                   (CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));     
        CleanupStack::PushL( iapRecord ); // 1
        iapRecord->SetRecordId( aCmId );

        OpenTransactionLC();
        iapRecord->LoadL( Session() );
        RollbackTransaction();
        
        bearerType = BearerTypeFromIapRecordL( iapRecord );

        CleanupStack::PopAndDestroy( iapRecord ); // 0        
        }    
    return bearerType;
    }
   
//-----------------------------------------------------------------------------
//  CCmManagerImpl::BearerTypeFromIapRecordL( CCDIAPRecord* aIapRecord )
//-----------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::BearerTypeFromIapRecordL( CCDIAPRecord* aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::BearerTypeFromIapRecordL" );

    CLOG_WRITE_1( "Plugin count: [%d]", iPlugins->Count() );
    
    TInt err( KErrNone );
    TUint32 extLevel( 0 );
    TInt bearerType( 0 );
    TBool canHandle( EFalse );
    
    // Check which bearer handles the given IAPid
    for ( TInt i=0; i<iPlugins->Count(); i++ )
        {
        // a defective, or a wrong AP can't cause any trouble.
        TRAP( err, canHandle = (*iPlugins)[i]->CanHandleIapIdL( aIapRecord ) );
        CLOG_WRITE_3( "plugin [%d] returned with error %d, CanH: %d", 
                       i, err, TUint32(canHandle) );
        if ( !err && canHandle )
            {
            if ( extLevel < 
                (*iPlugins)[i]->GetIntAttributeL( ECmExtensionLevel ) )
                {
                extLevel = (*iPlugins)[i]->GetIntAttributeL(ECmExtensionLevel);
                bearerType = (*iPlugins)[i]->GetIntAttributeL( ECmBearerType );
                }
            }
        else if ( err == KErrNoMemory )
            {
            User::Leave( err );
            }
        }
        
    if ( !bearerType )
        // No supporting plugin found.
        {
        CLOG_WRITE( "Plugin not found, Leaving." );
        User::Leave( KErrNotSupported );
        }
    CLOG_WRITE_1( "Returning bearertype: [%d]", bearerType );
    return bearerType;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::AllDestinationsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::AllDestinationsL( RArray<TUint32>& aDestArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AllDestinationsL" );

    OpenTransactionLC();

    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* destRecordSet = 
           new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                                                             iDestinationTableId );
    CleanupStack::PushL( destRecordSet );

    TRAP_IGNORE( destRecordSet->LoadL( Session() ) );
    
    iDestinationCount = destRecordSet->iRecords.Count();

    //=========================================================================
    // Step through every item in the NW Table and check whether it is linked
    // in the DN_IAP table
    //
    for ( TInt i = 0; i < iDestinationCount; ++i )
        {
        TInt nwId = QUERY_INT_FIELD( destRecordSet->iRecords[i], 
                                     KCDTIdDataMobilitySelectionPolicyNetwork );
        TBool found( EFalse );
        
        for ( TInt j = 0; j < aDestArray.Count(); ++j )
            {
            if ( aDestArray[j] == nwId )
                {
                found = ETrue;
                break;
                }
            }
            
        if ( !found )
            {
            aDestArray.AppendL( nwId );
            CLOG_WRITE_2( "Dest(%d): [%d]", aDestArray.Count(), nwId );
            }
        }
    
    CleanupStack::PopAndDestroy( destRecordSet );
    
    RollbackTransaction();
   
    CLOG_WRITE_1( "No. of dests found: [%d] ", aDestArray.Count() );
    }
        
//-----------------------------------------------------------------------------
//  CCmManagerImpl::DestinationL()
//-----------------------------------------------------------------------------
//  
EXPORT_C CCmDestinationImpl* CCmManagerImpl::DestinationL( TUint32 aId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::DestinationL" );
        
    // Create destination object
    return iObjectPool->GetDestinationL( aId );
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::RemoveDestFromPool
//-----------------------------------------------------------------------------
//  
EXPORT_C void CCmManagerImpl::RemoveDestFromPool( CCmDestinationImpl* aDestination )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveDestFromPool" );
    iObjectPool->RemoveDestination(aDestination);
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::RemoveDestFromPool
//-----------------------------------------------------------------------------
//  
void CCmManagerImpl::RemoveDestFromPool( CCmDestinationData* aDestination )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveDestFromPool" );
    iObjectPool->RemoveDestination(aDestination);
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::RemoveCMFromPool
//-----------------------------------------------------------------------------
//  
void CCmManagerImpl::RemoveCMFromPoolL( CCmPluginBaseEng* aCM )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveCMFromPool" );
    iObjectPool->RemoveConnectionMethodL( aCM );
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::RemoveCMFromPool
//-----------------------------------------------------------------------------
//  
void CCmManagerImpl::RemoveCMFromPoolL( CCmPluginBase* aCM )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveCMFromPool" );
    iObjectPool->RemoveConnectionMethodL( aCM );
    }

//-----------------------------------------------------------------------------
// CCmManagerImpl::CopyConnectionMethodL()
//-----------------------------------------------------------------------------
//
TInt CCmManagerImpl::CopyConnectionMethodL( 
                                    CCmDestinationImpl& aTargetDestination,
                                    CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CopyConnectionMethodL" );    
    
    OpenTransactionLC();
    TInt index = aTargetDestination.AddConnectionMethodL( aConnectionMethod );
    aTargetDestination.UpdateL();
    CommitTransactionL( 0 );
    
    return index;
    }    
    
//-----------------------------------------------------------------------------
// CCmManagerImpl::MoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
TInt CCmManagerImpl::MoveConnectionMethodL( 
                                    CCmDestinationImpl& aSourceDestination,
                                    CCmDestinationImpl& aTargetDestination,
                                    CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::MoveConnectionMethodL" );
    
    OpenTransactionLC();
    
    TInt index = CopyConnectionMethodL( aTargetDestination, aConnectionMethod );
    aSourceDestination.RemoveConnectionMethodL( aConnectionMethod );
    aSourceDestination.UpdateL();
    
    CommitTransactionL( KErrNone );
    
    return index;
    }
    
//-----------------------------------------------------------------------------
// CCmManagerImpl::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::RemoveConnectionMethodL( 
                                         CCmDestinationImpl& aDestination,
                                         CCmPluginBase& aConnectionMethod )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveConnectionMethodL" );    
    
    OpenTransactionLC();
    aDestination.RemoveConnectionMethodL( aConnectionMethod );
    aDestination.UpdateL();
    CommitTransactionL( 0 );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::SupportedBearers()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::SupportedBearersL" );

    for ( TInt i = 0; i < iPlugins->Count(); i++ )
        {
        TUint32 bearerType( (*iPlugins)[i]->GetIntAttributeL(ECmBearerType) );
        
        if ( KUidEmbeddedDestination != bearerType )
            {
            aArray.AppendL( (*iPlugins)[i]->GetIntAttributeL(ECmBearerType) );
            }
        }       
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::NumOfConnMethodReferencesL()
// -----------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::NumOfConnMethodReferencesL( 
        TUint32 aId,
        CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* aRecSet )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::NumOfConnMethodReferencesL" );

    OpenTransactionLC();
    
    TUint32 retVal( 0 );
   
    if ( aRecSet )
        {
        TMDBElementId id( IAP_ELEMENT_ID(aId) );
        
        for ( TInt i = 0; i < aRecSet->iRecords.Count(); ++i )
            {
            if ( QUERY_UINT32_FIELD( (*aRecSet)[i], 
                                    KCDTIdDataMobilitySelectionPolicyIAP ) == 
                id )
                {
                ++retVal;
                }
            }
        }
    else
        {
        // Just to test how many record we have
        TInt destTblId = DestinationTableId();
        CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>*  ptrRecordSet =
             new (ELeave) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                                                                    destTblId );
        CleanupStack::PushL( ptrRecordSet );

        // Prime record
        CCDDataMobilitySelectionPolicyRecord* record = 
                              new (ELeave) CCDDataMobilitySelectionPolicyRecord( 
                                                            DestinationTableId() );
        CleanupStack::PushL( record );

        record->iIAP = IAP_ELEMENT_ID( aId );
        ptrRecordSet->iRecords.AppendL( record );
        
        CleanupStack::Pop( record );
        record = NULL;
            
        if ( ptrRecordSet->FindL(Session()) )
            {
            // we have a match
            retVal = ptrRecordSet->iRecords.Count();
            CLOG_WRITE_1( "Found: [%d]", retVal );
            }
        else
            {
            CLOG_WRITE( "Nothing" );
            }
            
        CleanupStack::PopAndDestroy( ); //  ptrRecordSet
        }
            
    RollbackTransaction();
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::HasUnprotectedDestinationsL
// -----------------------------------------------------------------------------
//
TBool CCmManagerImpl::HasUnprotectedDestinationsL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::HasUnprotectedDestinationsL" );

    TBool hasUnprotectedDests( EFalse );
    
    RArray<TUint32> destIdArray( KCmArrayBigGranularity );
    AllDestinationsL( destIdArray );
    CleanupClosePushL( destIdArray );
    
    CCmDestinationImpl* dest = NULL;
    for ( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        dest = DestinationL( destIdArray[i] );
        CleanupStack::PushL( dest );
        if ( dest->ProtectionLevel() != EProtLevel1 )
            {
            hasUnprotectedDests = ETrue;
            CleanupStack::PopAndDestroy( dest );
            break;
            }            
        CleanupStack::PopAndDestroy( dest );
        }
        
    CleanupStack::PopAndDestroy( &destIdArray );
    
    return hasUnprotectedDests;
    }


// -----------------------------------------------------------------------------
// CCmManagerImpl::BuildPluginArrayL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::BuildPluginArrayL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::BuildPluginArrayL" );
    iPlugins = new (ELeave) CArrayPtrFlat<const CCmPluginBaseEng>( 
                                                           KPluginGranularity );

    // Get a list of all the bearer types
    RImplInfoPtrArray implArray;
    REComSession::ListImplementationsL( TUid::Uid( KCMPluginInterfaceUid ), 
                                        implArray );

    CleanupClosePushL( implArray ); // 1

    CCmPluginBaseEng* plugin = NULL;
    for ( TInt i=0; i<implArray.Count(); i++ )
        {
        TCmPluginInitParam params( *this );
        params.iParentDest = NULL;

        // This is the Util implementation        
        // Trap it to be able to work even if there's a faulty plugin installed
        // in the phone.
        TRAPD( err, plugin = STATIC_CAST( CCmPluginBaseEng*, 
                                        REComSession::CreateImplementationL( 
                                        (implArray)[i]->ImplementationUid(),
                                        _FOFF( CCmPluginBaseEng, 
                                               iDtor_ID_Key ),
                                        (TAny*)&params )) );

        if ( !err )
            {
            CLOG_ATTACH( plugin, this );
            
            CleanupStack::PushL( plugin );
            
            TBool inserted( EFalse );
            TInt defaultPriority( KDataMobilitySelectionPolicyPriorityWildCard );
            
            // No UI priority -> wildcard
            TRAP_IGNORE( defaultPriority = plugin->GetIntAttributeL( ECmDefaultUiPriority ) );
            
            for( TInt j( 0 ); j < iPlugins->Count(); ++j )
                {
                if( defaultPriority > (*iPlugins)[j]->GetIntAttributeL( ECmDefaultUiPriority ) )
                    {
                    continue;
                    }
                else if( defaultPriority == (*iPlugins)[j]->GetIntAttributeL( ECmDefaultUiPriority ) )
                    {
                    if( plugin->GetIntAttributeL( ECmExtensionLevel ) > 
                        (*iPlugins)[j]->GetIntAttributeL( ECmExtensionLevel ) )
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
            
            if( !inserted )
                {
                iPlugins->AppendL( plugin );
                }

            CleanupStack::Pop( plugin );
            }
        }
    
    implArray.ResetAndDestroy();
        
    CleanupStack::PopAndDestroy();  // implArray;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetBearerInfoIntL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmManagerImpl::GetBearerInfoIntL( TUint32 aBearerType,
                                           TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetBearerInfoIntL" );
    if( !IsBearerSpecific( aAttribute ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    for ( TInt i = 0; i < iPlugins->Count(); ++i )
        {
        if ( aBearerType == (*iPlugins)[i]->GetIntAttributeL( ECmBearerType ) )
            {
            return (*iPlugins)[i]->GetIntAttributeL( aAttribute );
            }
        }
        
    User::Leave( KErrNotSupported );
    
    return 0;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetBearerInfoBoolL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCmManagerImpl::GetBearerInfoBoolL( TUint32 aBearerType,
                                          TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetBearerInfoBoolL" );
    if( !IsBearerSpecific( aAttribute ) )
        {
        User::Leave( KErrNotSupported );
        }
        
    for ( TInt i = 0; i < iPlugins->Count(); ++i )
        {
        if ( aBearerType == (*iPlugins)[i]->GetIntAttributeL( ECmBearerType ) )
            {
            return (*iPlugins)[i]->GetBoolAttributeL( aAttribute );
            }
        }
        
    User::Leave( KErrNotSupported );
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetBearerInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCmManagerImpl::GetBearerInfoStringL( TUint32 aBearerType,
                                             TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetBearerInfoStringL" );
    if( !IsBearerSpecific( aAttribute ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    for ( TInt i = 0; i < iPlugins->Count(); ++i )
        {
        if ( aBearerType == (*iPlugins)[i]->GetIntAttributeL( ECmBearerType ) )
            {
            return (*iPlugins)[i]->GetStringAttributeL( aAttribute );
            }
        }
        
    User::Leave( KErrNotSupported );
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetBearerInfoString8L()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CCmManagerImpl::GetBearerInfoString8L( TUint32 aBearerType,
                                               TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetBearerInfoStringL" );
    if( !IsBearerSpecific( aAttribute ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    for ( TInt i = 0; i < iPlugins->Count(); ++i )
        {
        if ( aBearerType == (*iPlugins)[i]->GetIntAttributeL( ECmBearerType ) )
            {
            return (*iPlugins)[i]->GetString8AttributeL( aAttribute );
            }
        }
        
    User::Leave( KErrNotSupported );
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetConnectionMethodInfoIntL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmManagerImpl::GetConnectionMethodInfoIntL( TUint32 aCmId,
                                                     TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetConnectionMethodInfoIntL" );

    if ( aAttribute == ECmBearerType )
        {
        return const_cast<CCmManagerImpl*>(this)->BearerTypeFromCmIdL( aCmId );
        }
    else
        {
        return iPluginImpl->GetConnectionInfoIntL( aCmId, aAttribute );
        }
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::GetConnectionMethodInfoBoolL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCmManagerImpl::GetConnectionMethodInfoBoolL( TUint32 aCmId,
                                                    TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetConnectionMethodInfoBoolL" );

    TBool retval = EFalse;
    if ( aAttribute == ECmVirtual )
        {
        TUint32 b = GetConnectionMethodInfoIntL( aCmId, ECmBearerType );
        retval = GetBearerInfoBoolL( b, ECmVirtual );
        }
    else
        {
        retval = iPluginImpl->GetConnectionInfoBoolL( aCmId, aAttribute );
        }
    return retval;
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::GetConnectionMethodInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCmManagerImpl::GetConnectionMethodInfoStringL( 
                                                    TUint32 aCmId,
                                                    TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetConnectionMethodInfoStringL" );

    return iPluginImpl->GetConnectionInfoStringL( aCmId, aAttribute );
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::GetConnectionMethodInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CCmManagerImpl::GetConnectionMethodInfoString8L( 
                                                     TUint32 aCmId,
                                                     TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::GetConnectionMethodInfoString8L" );

    return iPluginImpl->GetConnectionInfoString8L( aCmId, aAttribute );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::UncategorizedIconL()
//-----------------------------------------------------------------------------
//    
CGulIcon* CCmManagerImpl::UncategorizedIconL() const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::UncategorizedIconL" );
    
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
           
    TParse mbmFile;
    User::LeaveIfError( mbmFile.Set( KManagerIconFilename, 
                                     &KDC_BITMAP_DIR, 
                                     NULL ) );
           
    CGulIcon* icon = AknsUtils::CreateGulIconL( 
                            skinInstance, 
                            KAknsIIDQgnPropSetConnDestUncategorized,
                            mbmFile.FullName(), 
                            EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized, 
                            EMbmCmmanagerQgn_prop_set_conn_dest_uncategorized_mask );
                            
    return icon;            
    }    

//-----------------------------------------------------------------------------
//  CCmManagerImpl::UncategorizedIconL()
//-----------------------------------------------------------------------------
//    
CMDBSession& CCmManagerImpl::Session() const
    { 
    return iTrans->Session(); 
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::HasCapabilityL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::HasCapabilityL( TCapability aCapability )
    {
    TSecurityInfo info;

    info.SetToCurrentInfo();
    
    if( !info.iCaps.HasCapability( aCapability ) )
        {
        User::Leave( KErrPermissionDenied );
        }
    }

// ---------------------------------------------------------------------------
// CCmManagerImpl::FeatureSupported
// ---------------------------------------------------------------------------
TBool CCmManagerImpl::FeatureSupported( TInt aFeature )
    {
    return FeatureManager::FeatureSupported( aFeature );
    }

// ---------------------------------------------------------------------------
// CCmManagerImpl::IsEasyWLan
//
// We can't have any dependecy to WLan plugin, but
// in some cases we need to filter out EasyWLan access points.
// ---------------------------------------------------------------------------
TBool CCmManagerImpl::IsEasyWLanL( CCDIAPRecord* aIapRecord ) const
    {
    LOGGER_ENTERFN( "CCmManagerImpl::IsEasyWLanL" );
    if (!iWLan)
        {
        return EFalse;    
        }
    if( (TPtrC(aIapRecord->iServiceType) != TPtrC(KCDTypeNameLANService) ) ||
        TPtrC(aIapRecord->iBearerType) != TPtrC(KCDTypeNameLANBearer) )
        {
        return EFalse;
        }
        
    TBool retVal( EFalse );
    TUint32 serviceId = aIapRecord->iService;
    
    CCDWlanServiceRecord* wLanServ = 
            new (ELeave) CCDWlanServiceRecord( CCDWlanServiceRecord::TableIdL( Session() ) );

    CleanupStack::PushL( wLanServ );
                
    wLanServ->iWlanServiceId.SetL( serviceId );
    
    if( wLanServ->FindL( Session() ) )
        {
        wLanServ->LoadL( Session() );

        if( wLanServ->iWLanSSID.IsNull() ||
            !TPtrC(wLanServ->iWLanSSID).Compare( KNullDesC ) )
            {
            retVal = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy( wLanServ );
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::EasyWlanIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmManagerImpl::EasyWlanIdL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::EasyWlanId" );

    OpenTransactionLC();
    
    TUint32 easyId( 0 );

    CMDBRecordSet<CCDIAPRecord>*  ptrRecordSet = AllIapsL();
    CleanupStack::PushL( ptrRecordSet );
    TInt iapRecords = ptrRecordSet->iRecords.Count();
    CLOG_WRITE_1_PTR( NULL, "Record num [%d]", iapRecords );

    for ( TInt i = 0; i < iapRecords; i++ )
        {
        CCDIAPRecord* iapRecord = (*ptrRecordSet)[i];
        
        if( (TPtrC(iapRecord->iServiceType) == TPtrC(KCDTypeNameLANService) ) &&
            TPtrC(iapRecord->iBearerType) == TPtrC(KCDTypeNameLANBearer) )
            {
            if( IsEasyWLanL( iapRecord ) )
                {
                easyId = (*ptrRecordSet)[i]->RecordId();
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( ptrRecordSet );

    RollbackTransaction();
            
    return easyId;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::AllIapsL()
// -----------------------------------------------------------------------------
//
CMDBRecordSet<CCDIAPRecord>* CCmManagerImpl::AllIapsL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AllIapsL" );

    OpenTransactionLC();

    CMDBRecordSet<CCDIAPRecord>*  ptrRecordSet = 
                    new (ELeave) CMDBRecordSet<CCDIAPRecord>( KCDTIdIAPRecord );
    
    TRAP_IGNORE( ptrRecordSet->LoadL( Session() ) );
    
    RollbackTransaction();
    
    return ptrRecordSet;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::AllSNAPRecordsL()
// -----------------------------------------------------------------------------
//
CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* 
                    CCmManagerImpl::AllSNAPRecordsL( TUint32 aSnapElementId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::AllSNAPRecordsL" );

    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>*  ptrRecordSet = 
              new (ELeave) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>(  
                                                        DestinationTableId() );
    CleanupStack::PushL( ptrRecordSet );

    OpenTransactionLC();    
    if( aSnapElementId )
        {
        // Prime record
        CCDSnapRecord* record = SNAPRecordL( 0 );
        CleanupStack::PushL( record );
        record->iEmbeddedSNAP = aSnapElementId;
        ptrRecordSet->iRecords.AppendL( record );

        CleanupStack::Pop( record );
        record = NULL;
            
        ptrRecordSet->FindL( Session() );
        }
    else
        {
        ptrRecordSet->LoadL( Session() );
        }
        
    RollbackTransaction();
    
    return ptrRecordSet;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::IsIapLinkedL()
// -----------------------------------------------------------------------------
//
TBool CCmManagerImpl::IsIapLinkedL( const CCmPluginBaseEng& aPlugin )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::IsIapLinkedL" );
    
    TUint32 iapId = aPlugin.GetIntAttributeL( ECmId );
    CMDBRecordSet<CCDIAPRecord>*  ptrRecordSet = AllIapsL();
    CleanupStack::PushL( ptrRecordSet );

    TInt iapRecords = ptrRecordSet->iRecords.Count();
    CLOG_WRITE_1_PTR( NULL, "Record num [%d]", iapRecords );

    TBool linked( EFalse );
    for ( TInt i = 0; !linked && i < iapRecords; i++ )
        {
        CCDIAPRecord* iapRecord = (*ptrRecordSet)[i];
        
        if( iapRecord->RecordId() == iapId )
            {
            continue;
            }
            
        TUint32 bearerType( 0 );
        
        TRAP_IGNORE( bearerType = BearerTypeFromIapRecordL( iapRecord ) );
        if( bearerType )
            {
            if( GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                {
                CCmPluginBase* plugin = NULL;
                
                TRAP_IGNORE( plugin = GetConnectionMethodL( iapRecord->RecordId() ) );
                if( plugin )
                    {
                    linked = plugin->IsLinkedToIap( iapId );
                    }
                    
                delete plugin;
                }
            }
        }
    CleanupStack::PopAndDestroy( ptrRecordSet );
    
    return linked;
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::RemoveAllReferencesL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::RemoveAllReferencesL( const CCmPluginBaseEng& aPlugin )
    {
    RArray<TUint32> dests;
    CleanupClosePushL( dests );    
    OpenTransactionLC();

    AllDestinationsL( dests );
    
    for( TInt i = 0; i < dests.Count(); ++i )
        {
        CCmDestinationImpl* dest = DestinationL( dests[i] );
        CleanupStack::PushL( dest );
        TRAPD( err, dest->RemoveConnectionMethodL( aPlugin ) );
        
        if( err != KErrNotFound )
            {
            User::LeaveIfError( err );

            dest->UpdateL();
            }
        
        CleanupStack::PopAndDestroy( dest );
        }
    
    CommitTransactionL( 0 );
    CleanupStack::PopAndDestroy( &dests );   
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::RemoveAllReferencesLWoTransL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::RemoveAllReferencesWoTransL( const CCmPluginBaseEng& aPlugin )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::RemoveAllReferencesWoTransL" );

    RArray<TUint32> dests;
    CleanupClosePushL( dests );    

    AllDestinationsL( dests );

    for( TInt i = 0; i < dests.Count(); ++i )
        {
        CCmDestinationImpl* dest = DestinationL( dests[i] );
        CleanupStack::PushL( dest );
        TRAPD( err, dest->RemoveConnectionMethodL( aPlugin ) );

        if( err != KErrNotFound )
            {
            User::LeaveIfError( err );

            dest->UpdateL();
            }

        CleanupStack::PopAndDestroy( dest );
        }
    CleanupStack::PopAndDestroy( &dests );   
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::HandleDefConnDeletedL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::HandleDefConnDeletedL( TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::HandleDefConnDeletedL" );

    if ( IsDefConnSupported() )
        {
        TCmDefConnValue defConn;
        ReadDefConnWoTransL(defConn);
        if ( defConn == aDCSetting )
            {
            // Default connection deleted, setting it to default value.
            TUint32 destId = GetInternetDestinationIdL();

            if ( destId && ( destId != aDCSetting.iId ) )
                {
                defConn.iType = ECmDefConnDestination;
                defConn.iId = destId;                
                }
            else
                {
                // Internet destination not found. Initialise to zero.
                defConn.iType = TCmDefConnType ( 0 );
                defConn.iId = 0;
                }    

            WriteDefConnWoTransL( defConn );
            }
        } // do nothing if default connection is not supported
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::ReadDefConnWoTransL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::ReadDefConnWoTransL( TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ReadDefConnWoTransL" );
    if ( IsDefConnSupported() )
        {
        CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
            new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iDefConnTableId );
        CleanupStack::PushL( defConnRecordSet );

        TRAPD( err,  defConnRecordSet->LoadL( Session() ) );
        if (err == KErrNone)
            {
            TInt mode = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                                         KCDTIdDefConnType );
            aDCSetting.iType= TCmDefConnType (mode);

            aDCSetting.iId = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                                              KCDTIdDefConnUid );
            CLOG_WRITE( "Default Connection read OK " );
            }
        else
            {
            //deleting old record, writing Always Ask (default value)
            //aDCSetting.iType = ECmDefConnAlwaysAsk;
            //aDCSetting.iId = 0;
            TUint32 destId = GetInternetDestinationIdL();

            if ( destId )
                {
                aDCSetting.iType = ECmDefConnDestination;
                aDCSetting.iId = destId;                
                }
            else
                {                
                // Internet destination not found. Initialise to zero.
                aDCSetting.iType = TCmDefConnType ( 0 );
                aDCSetting.iId = 0;
                }    

            CLOG_WRITE( "Default Connection read FAILED, storing Always Ask! " );
            ReplaceDefConnRecordL( aDCSetting );
            }
        CleanupStack::PopAndDestroy( defConnRecordSet );
        }
    else
        {
        CLOG_WRITE( "Default Connection NOT supported, leaving with KErrNotSupported" );
        User::Leave(KErrNotSupported);
        }
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::WriteDefConnWoTransL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::WriteDefConnWoTransL( const TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::WriteDefConnWoTransL" );

    if ( IsDefConnSupported() )
        {
        ReplaceDefConnRecordL( aDCSetting );
        }
    else
        {
        CLOG_WRITE( "Default Connection NOT supported, leaving with KErrNotSupported" );
        User::Leave(KErrNotSupported);
        }
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::ReadDefConnL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::ReadDefConnL( TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ReadDefConn" );

    if ( IsDefConnSupported() )
        {
        OpenTransactionLC();
        CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
               new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iDefConnTableId );
        CleanupStack::PushL( defConnRecordSet );
     
        TRAPD( err,  defConnRecordSet->LoadL( Session() ) );
        if (err == KErrNone)
            {
             TInt mode = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                                             KCDTIdDefConnType );
              aDCSetting.iType= TCmDefConnType ( mode );
            
            aDCSetting.iId = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], 
                                             KCDTIdDefConnUid );
            CleanupStack::PopAndDestroy( defConnRecordSet );
            RollbackTransaction();                                 
            }
        else
            {
            //deleting old record, writing Always Ask (default value)
            //aDCSetting.iType = ECmDefConnAlwaysAsk;
            //aDCSetting.iId = 0;
            
            TUint32 destId = GetInternetDestinationIdL();

            if ( destId )
                {
                aDCSetting.iType = ECmDefConnDestination;
                aDCSetting.iId = destId;                
                }
            else
                {                
                // Internet destination not found. Initialise to zero.
                aDCSetting.iType = TCmDefConnType ( 0 );
                aDCSetting.iId = 0;
                }    

            CLOG_WRITE( "Default Connection read FAILED, storing Always Ask! " );
            ReplaceDefConnRecordL( aDCSetting );
            
            CleanupStack::PopAndDestroy( defConnRecordSet );
            CommitTransactionL( KErrNone );
            }
        }
    else
        {
        CLOG_WRITE( "Default Connection NOT supported" );
        User::Leave(KErrNotSupported);
        }
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::ReplaceDefConnRecordL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::ReplaceDefConnRecordL( const TCmDefConnValue aDCSetting )
    {
    	
    LOGGER_ENTERFN( "CCmManagerImpl::ReplaceDefConnRecordL" );	
    // Check the Id first (that it exists)
    if ( aDCSetting.iType == ECmDefConnConnectionMethod )
        {
        CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord *>
                                  ( CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
        CleanupStack::PushL( iapRecord );
        iapRecord->SetRecordId( aDCSetting.iId );
        TRAPD( err, iapRecord->LoadL( Session() ) );
        if ( err == KErrNotFound )
            {
            User::Leave( KErrArgument );
            }
        CleanupStack::PopAndDestroy( iapRecord );
        
        // Check that Connection Method is not hidden
        if ( GetConnectionMethodInfoBoolL( aDCSetting.iId, ECmHidden ) )
            {
            User::Leave( KErrArgument );
            }
        }
    else if ( aDCSetting.iType == ECmDefConnDestination )
        {
        TUint32 destId = GetInternetDestinationIdL();
        
        // Allow empty Internet destination, others must have some content
        if ( destId != aDCSetting.iId )
            {
            CCDAccessPointRecord* destAPRecord = static_cast<CCDAccessPointRecord *>(
                           CCDRecordBase::RecordFactoryL( KCDTIdAccessPointRecord ) );
            CleanupStack::PushL( destAPRecord );
            
            destAPRecord->iRecordTag = aDCSetting.iId;
            if ( !destAPRecord->FindL( Session() ) )
                {
                User::Leave( KErrArgument );
                }
            CleanupStack::PopAndDestroy( destAPRecord );
         		}
        }

    TInt ret = KErrNone;
    TCmGenConnSettings genConnSettings;
    
    SetGenConnSettingsToDefault( genConnSettings );

    CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
           new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iDefConnTableId );
    CleanupStack::PushL( defConnRecordSet );
    
    TRAP( ret, defConnRecordSet->LoadL( Session() ) );
    
    if (ret == KErrNone)
        {
        TInt value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdUsageOfWlan );
        genConnSettings.iUsageOfWlan = TCmUsageOfWlan( value );

        value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdSeamlessnessHome );
        genConnSettings.iSeamlessnessHome = TCmSeamlessnessValue( value );

        value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdSeamlessnessVisitor );
        genConnSettings.iSeamlessnessVisitor = TCmSeamlessnessValue( value );
        }

    TInt typeVal = aDCSetting.iType;
    TInt uidVal = aDCSetting.iId;

    // Delete the old record
    for ( TInt i = 0; i < defConnRecordSet->iRecords.Count(); ++i )
        {
        defConnRecordSet->iRecords[i]->DeleteL( Session() );
        }
    CleanupStack::PopAndDestroy( defConnRecordSet );

    CCDDefConnRecord* dcRcd = 
        new (ELeave) CCDDefConnRecord( iDefConnTableId );
    CleanupStack::PushL( dcRcd ); // 2
            
    dcRcd->SetRecordId( KCDNewRecordRequest );

    SET_INT_FIELD(dcRcd, KCDTIdUsageOfWlan, genConnSettings.iUsageOfWlan );
    SET_INT_FIELD(dcRcd, KCDTIdSeamlessnessHome, genConnSettings.iSeamlessnessHome );
    SET_INT_FIELD(dcRcd, KCDTIdSeamlessnessVisitor, genConnSettings.iSeamlessnessVisitor );

    CLOG_WRITE_FORMAT( "Set default connection type: [%d]", typeVal );
    SET_INT_FIELD(    dcRcd, 
                        KCDTIdDefConnType, 
                        typeVal );
    CLOG_WRITE_1( "Set uid: [%d]", uidVal );
    SET_INT_FIELD(  dcRcd, 
                    KCDTIdDefConnUid, 
                    uidVal );
    dcRcd->StoreL( Session() );    
    CleanupStack::PopAndDestroy( dcRcd );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::WriteDefConnL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::WriteDefConnL( const TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::WriteDefConnL" );
    
    if ( ( aDCSetting.iType == ECmDefConnAlwaysAsk ) ||
         ( aDCSetting.iType == ECmDefConnAskOnce ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    if ( IsDefConnSupported() )
        {
        OpenTransactionLC();

        ReplaceDefConnRecordL( aDCSetting );
        CommitTransactionL( KErrNone );
        }
    else
        {
        CLOG_WRITE( "Default Connection NOT supported, leaving with KErrNotSupported" );
        User::Leave(KErrNotSupported);
        }
    }
    
//-----------------------------------------------------------------------------
//  CCmManagerImpl::IsDefConnSupported()
//-----------------------------------------------------------------------------
//
TBool CCmManagerImpl::IsDefConnSupported(  )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::IsDefConnSupported" );

    return iDefConnSupported;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::ReadGenConneSettingsL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::ReadGenConnSettingsL( TCmGenConnSettings& aGenConnSettings )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ReadGenConnSettingsL" );

    OpenTransactionLC();
    CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
        new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iDefConnTableId );
    CleanupStack::PushL( defConnRecordSet );
     
    defConnRecordSet->LoadL( Session() );

    TInt value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdUsageOfWlan );
    aGenConnSettings.iUsageOfWlan = TCmUsageOfWlan( value );

    value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdSeamlessnessHome );
    aGenConnSettings.iSeamlessnessHome = TCmSeamlessnessValue( value );

    value = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdSeamlessnessVisitor );
    aGenConnSettings.iSeamlessnessVisitor = TCmSeamlessnessValue( value );
            
    CleanupStack::PopAndDestroy( defConnRecordSet );
    RollbackTransaction();
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::ReplaceGenConnSettingsL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::ReplaceGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings )
    {
    
    LOGGER_ENTERFN( "CCmManagerImpl::ReplaceGenConnSettingsL" );	
    	
    TInt ret = KErrNone;
    TCmDefConnValue defConn;

    defConn.iType = TCmDefConnType ( 0 );
    defConn.iId = 0;
    
    CMDBRecordSet<CCDDefConnRecord>* defConnRecordSet = 
           new ( ELeave ) CMDBRecordSet<CCDDefConnRecord>( iDefConnTableId );
    CleanupStack::PushL( defConnRecordSet );
    
    TRAP(ret, defConnRecordSet->LoadL(Session()));
    
    if (ret == KErrNone)
        {
        TInt type = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdDefConnType);
        defConn.iType = TCmDefConnType( type );
        TInt id = QUERY_INT_FIELD( defConnRecordSet->iRecords[0], KCDTIdDefConnUid);
        defConn.iId = id;
        }
    
    TInt usageOfNewWlan = aGenConnSettings.iUsageOfWlan;
    TInt seamlessnessHome    = aGenConnSettings.iSeamlessnessHome;
    TInt seamlessnessVisitor = aGenConnSettings.iSeamlessnessVisitor;
    
    // Delete the old record
    for ( TInt i = 0; i < defConnRecordSet->iRecords.Count(); ++i )
        {
        defConnRecordSet->iRecords[i]->DeleteL( Session() );
        }
    CleanupStack::PopAndDestroy( defConnRecordSet );

    CCDDefConnRecord* dcRcd = 
        new (ELeave) CCDDefConnRecord( iDefConnTableId );
    CleanupStack::PushL( dcRcd ); // 2
            
    dcRcd->SetRecordId( KCDNewRecordRequest );

    SET_INT_FIELD(dcRcd, KCDTIdDefConnType, defConn.iType);
    SET_INT_FIELD(dcRcd, KCDTIdDefConnUid, defConn.iId);

    CLOG_WRITE_1( "Set wlan usage: [%d]", usageOfNewWlan );
    SET_INT_FIELD(dcRcd, KCDTIdUsageOfWlan, usageOfNewWlan );

    CLOG_WRITE_1( "Set seamlessnessHome: [%d]", seamlessnessHome );
    SET_INT_FIELD(dcRcd, KCDTIdSeamlessnessHome, seamlessnessHome );

    CLOG_WRITE_1( "Set seamlessnessVisitor: [%d]", seamlessnessVisitor );
    SET_INT_FIELD(dcRcd, KCDTIdSeamlessnessVisitor, seamlessnessVisitor );

    dcRcd->StoreL( Session() );    
    CleanupStack::PopAndDestroy( dcRcd );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::WriteGenConnSettingsL()
//-----------------------------------------------------------------------------
//
void CCmManagerImpl::WriteGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::WriteGenConnSettingsL" );    

    OpenTransactionLC();

    ReplaceGenConnSettingsL( aGenConnSettings );
    CommitTransactionL( KErrNone );
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::IsMemoryLow()
//-----------------------------------------------------------------------------
//
TBool CCmManagerImpl::IsMemoryLow()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::IsMemoryLow" );

    TBool ret( EFalse );
    // Any plugin can be used here, because CheckSpaceBelowCriticalLevelL
    // is defined in their father class CCmPluginBaseEng
    TRAPD( err, ret = (*iPlugins)[0]->CheckSpaceBelowCriticalLevelL() );
    if( err != KErrNone )
        {
        ret = ETrue;
        return ret;
        }
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CCmManagerImpl::SNAPRecordL
// -----------------------------------------------------------------------------
CCDSnapRecord* CCmManagerImpl::SNAPRecordL( TUint32 aRecordId ) const
    {
    CCDSnapRecord* record = new (ELeave) CCDSnapRecord( 
                                 DestinationTableId() );

    if( aRecordId )
        {
        record->SetRecordId( aRecordId );
        
        CleanupStack::PushL( record );
        record->LoadL( Session() );
        CleanupStack::Pop( record );
        }
    
    return record;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::SNAPRecordL
// -----------------------------------------------------------------------------
CCDSnapRecord* CCmManagerImpl::CreateSNAPRecordL( TUint32 aRecordId ) const
    {
    CCDSnapRecord* record = new (ELeave) CCDSnapRecord( 
        DestinationTableId() );

    if( aRecordId )
        {
        record->SetRecordId( aRecordId );
        }

    return record;
    }

//-----------------------------------------------------------------------------
//  CCmManagerImpl::WrapCmManager()
//-----------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::WrapCmManager( RCmManagerExt& aCmManagerExt )
    {
    aCmManagerExt.iImplementation = this;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::FilterOutVirtualsL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::FilterOutVirtualsL( RPointerArray<CCmPluginBase>& aCmDataArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::FilterOutVirtualsL" );

    TInt count = aCmDataArray.Count();
    
    for ( TInt i = 0; i < aCmDataArray.Count(); i++ )
        {
        if ( aCmDataArray[i]->GetBoolAttributeL( ECmVirtual ) )
            {
            delete aCmDataArray[i];
            aCmDataArray.Remove(i);
            i--;
            }
        }
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::CreateFlatCMListLC
// --------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::CreateFlatCMListLC( TUint32 aDestinationId , 
                                                RPointerArray<CCmPluginBase>& aCmArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateFlatCMListLC" );

    CreateFlatCMListL(aDestinationId, aCmArray);
    CleanupResetAndDestroyPushL< RPointerArray<CCmPluginBase> >(aCmArray);
    }
    
// --------------------------------------------------------------------------
// CCmManagerImpl::CreateFlatCMListL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::CreateFlatCMListL( TUint32 aDestinationId , 
                                                RPointerArray<CCmPluginBase>& aCmArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CreateFlatCMListL" );
    // get the uncategorised connection methods
    if ( aDestinationId == KDestItemUncategorized ) 
        {
        AppendUncatCmListLC( aCmArray);
        }
    else
        {
        // Get the CM ids from the destination
        AppendCmListLC( aDestinationId, aCmArray );
        }
    
    // get the possible embedded destinations to be able to list their content, too
    // we only have to check in cmArray
    // put them in a sep. array and get their data into another.    
    TInt cmCount = aCmArray.Count();
    RArray<TUint32> cmEmbDestArray ( KCmArrayMediumGranularity );
    CleanupClosePushL( cmEmbDestArray );
    for ( TInt i = 0; i < cmCount; i++ )
        {
        if ( aCmArray[i]->GetBoolAttributeL( ECmDestination ) )
            {
            cmEmbDestArray.AppendL( aCmArray[i]->GetIntAttributeL( ECmId ) );
            //remove embedded destination right now
            delete aCmArray[i];
            aCmArray.Remove(i);
           // i--;
            }
        }
    // now we stored the needed emb.dest, 
    // we can filter out all virtual ones
    //iCmManager.FilterOutVirtualsL( cmArray ); //no filtering here, we can do it later
    
    // now we have the list of emb.dest. inside current destination, 
    // now get their CM's data appended to one flat list
    RPointerArray<CCmPluginBase> cmLinkedDataArray ( KCmArrayMediumGranularity );
    CleanupResetAndDestroyPushL< RPointerArray<CCmPluginBase> >( cmLinkedDataArray );
    TInt embdestcount = cmEmbDestArray.Count();
    for ( TInt i = 0; i < embdestcount; i++ )
        {
        // get the list of CM's
        CCmPluginBase* emdestcm = 
                    GetConnectionMethodL( cmEmbDestArray[i] );
        CleanupStack::PushL( emdestcm );
        TUint destUid = emdestcm->Destination()->Id();
        AppendCmListL( destUid, cmLinkedDataArray );
        CleanupStack::PopAndDestroy( emdestcm );    
        }
    // filter out virtuals
    //iCmManager.FilterOutVirtualsL( cmLinkedDataArray ); //still no filtering here

    // now we have the original, sorted list and an unsorted cmLinkedDataArray,
    // add each element to original array with sorting

    CombineArraysForPriorityOrderL( aCmArray, cmLinkedDataArray );

    // now we do no longer need the linked array
    CleanupStack::Pop( &cmLinkedDataArray );   // don't destroy the cm objects!
    
    cmLinkedDataArray.Close();                 // they are appended to aCmArray!!!
    CleanupStack::PopAndDestroy( &cmEmbDestArray ); 
    
    CleanupStack::Pop(&aCmArray);     
    }
    
// --------------------------------------------------------------------------
// CCmManagerImpl::CombineCmListsL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::CombineArraysForPriorityOrderL( 
                                        RPointerArray<CCmPluginBase>& aCmArray , 
                                        RPointerArray<CCmPluginBase>& aCmLinkedArray )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CombineArraysForPriorityOrderL" );

    // now we have two arrays, the first is sorted as needed,
    // the second is not.
    // we have to insert the items from the second to the first into their 
    // correct position based on the bearer type priorities
    
    // so loop on the second array, and for ech element, find it's place
    TInt count = aCmLinkedArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TUint32 linkedpriority( aCmLinkedArray[i]->GetIntAttributeL( ECmDefaultPriority ) );
        // InsertItemL( aCmArray, aCmLinkedArray[i]);
        // higher priority means smaller number
        // if same priority elements exist, then 
        // - we need to insert our element inside the elements 
        //   of the same priority according to 
        //   bearetypes and extension levels
        // or before the first element having higher priority numbers.
        TInt cc = aCmArray.Count();
        if ( cc )
            {
            TInt ii = 0;
            for ( ; ii < cc; ii++ )
                {
                if ( linkedpriority < aCmArray[ii]->GetIntAttributeL( ECmDefaultPriority ) )
                    {
                    // the next element already has larger number, 
                    // we must insert it HERE
                    break;
                    }
                else
                    {
                    // priorities either same, then we need to check bearer, 
                    // ext.level, etc
                    // or we simply still need to go to next element
                    if ( linkedpriority == aCmArray[ii]->GetIntAttributeL( ECmDefaultPriority ))
                        {
                        // check if the same bearer
                        if ( aCmArray[ii]->GetIntAttributeL( ECmBearerType ) == 
                                    aCmLinkedArray[i]->GetIntAttributeL( ECmBearerType ) )
                            {
                            // we need to find the last one of these bearers, 
                            // and insert after the last one
                            do
                                {
                                ii++;
                                }while ( ( ii < cc )
                                        &&  (aCmArray[ii]->GetIntAttributeL( ECmBearerType ) == 
                                            aCmLinkedArray[i]->GetIntAttributeL( ECmBearerType )) );
                            // now we either found a correct place or 
                            // we are at the end of the list
                            break;
                            }
                        else
                            {
                            // diff. bearers, check Ext.level
                            if ( aCmArray[ii]->GetIntAttributeL( ECmExtensionLevel ) < aCmLinkedArray[i]->GetIntAttributeL( ECmExtensionLevel ) )
                                {
                                break;
                                }
                            // if not, then go for the next one...
                            }
                        }
                    }
                }
                // now we either found a correct place or 
                // we are at the end of the list
                // ( we break-ed for cycle when found the correct place )
                if ( ii == cc )
                    {
                    // end of the list, append
                    aCmArray.AppendL( aCmLinkedArray[i]);
                    }
                else
                    {
                    // we found the place to be inserted
                    aCmArray.InsertL( aCmLinkedArray[i], ii );
                    }   
            }
        else
            {
            // no elements in original array yet, 
            // so simply append
            aCmArray.AppendL( aCmLinkedArray[i]);
            }
        }
    }


// --------------------------------------------------------------------------
// CCmManagerImpl::ParentDestinationL
// --------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::ParentDestinationL( TUint32 aCmId )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::ParentDestinationL" );

    TUint32 retval(0);
    
    TUint32 destTblId = DestinationTableId();
    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>*  ptrRecordSet =
         new (ELeave) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                                                                destTblId );
    CleanupStack::PushL( ptrRecordSet );

    // Prime record
    CCDDataMobilitySelectionPolicyRecord* record = 
                          new (ELeave) CCDDataMobilitySelectionPolicyRecord( 
                                                        DestinationTableId() );
    CleanupStack::PushL( record );
    
    record->iIAP = IAP_ELEMENT_ID( aCmId );
    ptrRecordSet->iRecords.AppendL( record );
    
    CleanupStack::Pop( record );
    record = NULL;
            
    if ( ptrRecordSet->FindL(Session()) )
        {
        // we have a match
        retval = QUERY_INT_FIELD( (*ptrRecordSet)[0],
                             KCDTIdDataMobilitySelectionPolicyNetwork );
        
        }
    CleanupStack::PopAndDestroy( ptrRecordSet );        

    return retval;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::GetInternetDestinationIdL
// --------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::GetInternetDestinationIdL()
    {
    
    LOGGER_ENTERFN( "CCmManagerImpl::GetInternetDestinationIdL" );
   
    TUint32 destId = 0;
    
    // Get all destinations
    RArray<TUint32> destIdArray( KCmArrayBigGranularity );
    AllDestinationsL( destIdArray );
    CleanupClosePushL( destIdArray );
    
    CCmDestinationImpl* dest = NULL;
    
    for ( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        dest = DestinationL( destIdArray[i] );
        CleanupStack::PushL( dest );
        
        // Check if destination is internet
        if ( dest->MetadataL(ESnapMetadataInternet) )
            {
            destId = destIdArray[i];
            }

        CleanupStack::PopAndDestroy( dest );

        if ( destId )
            {
            break;
            }
        }
        
    CleanupStack::PopAndDestroy( &destIdArray );    
    
    return destId;    
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::SetGenConnSettingsToDefault()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::SetGenConnSettingsToDefault( TCmGenConnSettings& aGenConnSettings )
    {
    LOGGER_ENTERFN( "CCmManagerImpl::SetGenConnSettingsToDefault" );	
    	
    aGenConnSettings.iUsageOfWlan = ECmUsageOfWlanKnown;
    aGenConnSettings.iSeamlessnessHome    = ECmSeamlessnessConfirmFirst;
    aGenConnSettings.iSeamlessnessVisitor = ECmSeamlessnessConfirmFirst;
    }


// -----------------------------------------------------------------------------
// CCmManagerImpl::CheckAvailableSpaceL()
// -----------------------------------------------------------------------------
//
void CCmManagerImpl::CheckAvailableSpaceL()
    {
    LOGGER_ENTERFN( "CCmManagerImpl::CheckAvailableSpaceL" );
    TBool belowCritical = SysUtil::FFSSpaceBelowCriticalLevelL( &(this->iFs), KBytesToWrite );
    CLOG_WRITE_1( "belowCritical: [%d]", belowCritical );
    if ( belowCritical )
        {
        User::Leave( KErrDiskFull );
        }
    }


// --------------------------------------------------------------------------
// CCmManagerImpl::WatcherRegisterL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmManagerImpl::WatcherRegisterL( MCmCommsDatWatcher* aWatcher )
    {
    iCommsDatIapNotifier->WatcherRegisterL( aWatcher );
    iCommsDatSnapNotifier->WatcherRegisterL( aWatcher );
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::WatcherUnRegister
// --------------------------------------------------------------------------
//    
EXPORT_C void CCmManagerImpl::WatcherUnRegister()
    {
    iCommsDatIapNotifier->WatcherUnRegister();
    iCommsDatSnapNotifier->WatcherUnRegister();
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::DestinationStillExistedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CCmManagerImpl::DestinationStillExistedL( CCmDestinationImpl* aDest )
    {
    TUint32 destId = aDest->Id();
    TBool found( EFalse );
    TInt count;
    
    OpenTransactionLC();

    CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>* destRecordSet = 
           new ( ELeave ) CMDBRecordSet<CCDDataMobilitySelectionPolicyRecord>( 
                                                             iDestinationTableId );
    CleanupStack::PushL( destRecordSet );

    TRAP_IGNORE( destRecordSet->LoadL( Session() ) );
    
    count = destRecordSet->iRecords.Count();
    
    for ( TInt i = 0; i < count; ++i )
        {
        TInt nwId = QUERY_INT_FIELD( destRecordSet->iRecords[i], 
                                     KCDTIdDataMobilitySelectionPolicyNetwork );
        //nwId = RECORD_FROM_ELEMENT_ID( nwId );
        
        if(  nwId == destId )
            {
            found = ETrue;
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( destRecordSet );
    
    RollbackTransaction();
    
    return found;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::IapStillExistedL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CCmManagerImpl::IapStillExistedL( const CCmPluginBaseEng& aPlugin )
    {
    TUint32 iapId = aPlugin.GetIntAttributeL( ECmId );
    CMDBRecordSet<CCDIAPRecord>*  ptrRecordSet = AllIapsL();
    CleanupStack::PushL( ptrRecordSet );

    TInt iapRecords = ptrRecordSet->iRecords.Count();

    TBool found( EFalse );
    for ( TInt i = 0; !found && i < iapRecords; i++ )
        {
        CCDIAPRecord* iapRecord = (*ptrRecordSet)[i];
        
        if( iapRecord->RecordId() == iapId )
            {
            found = ETrue;
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( ptrRecordSet );
    
    return found;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::IsIapStillInDestL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CCmManagerImpl::IsIapStillInDestL( 
        CCmDestinationImpl* aDest, 
        const CCmPluginBaseEng& aPlugin )
    {
    TUint32 iapId = aPlugin.GetIntAttributeL( ECmId );
    
    aDest->ReLoadConnectionMethodsL();
    RArray<TUint32> cmIds;
    aDest->ConnectMethodIdArrayL( cmIds );
    CleanupClosePushL( cmIds );

    TBool found( EFalse );
    for ( TInt i = 0; i < cmIds.Count(); i++ )
        {
        if( cmIds[i] == iapId )
            {
            found = ETrue;
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( &cmIds );
    
    return found;
    }

// --------------------------------------------------------------------------
// CCmManagerImpl::GetSnapTableIdL
// --------------------------------------------------------------------------
//
TUint32 CCmManagerImpl::GetSnapTableIdL()
    {
    TUint32 snapTableId;
    
    CMDBSession* db = CMDBSession::NewLC( CMDBSession::LatestVersion() );
    db->SetAttributeMask( ECDHidden | ECDProtectedWrite );

    snapTableId = CCDDataMobilitySelectionPolicyRecord::TableIdL( *db );

    CleanupStack::PopAndDestroy( db );
    
    return snapTableId;
    }

// -----------------------------------------------------------------------------
// CCmManagerImpl::IsBearerSpecific()
// -----------------------------------------------------------------------------
//
TBool CCmManagerImpl::IsBearerSpecific( TUint32 aAttribute ) const
    {
    TUint32 i = 0;
    while( KBearerSpecificAttributes[i] != KEndOfArray )
        {
        if( KBearerSpecificAttributes[i] == aAttribute )
            {
            return ETrue;
            }
        i++;
        }        
    return EFalse;
    }
