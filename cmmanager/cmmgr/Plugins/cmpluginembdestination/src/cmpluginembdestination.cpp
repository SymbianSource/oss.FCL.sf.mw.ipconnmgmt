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
* Description:  Implementation of CCmPluginEmbDestination
*
*/

#include "cmdestinationimpl.h"
#include "cmpluginembdestination.h"
#include "datamobilitycommsdattypes.h"
#include "cmmanagerimpl.h"
#include "cmlogger.h"
#include <ecom/ecom.h>		// For REComSession
#include <StringLoader.h>
#include <cmpluginembdestinationui.rsg>
#include <data_caging_path_literals.hrh>
#include <cmmanager.rsg>
#include <bautils.h>

using namespace CMManager;
using namespace CommsDat;

_LIT( KPluginEmbDestinationDataResDirAndFileName, 
      "z:cmpluginembdestinationui.rsc" );

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::NewL()
// ---------------------------------------------------------------------------
//
CCmPluginEmbDestination* CCmPluginEmbDestination::NewL( 
                                                TCmPluginInitParam* aInitParam )
	{
	CCmPluginEmbDestination* self = 
	                        new( ELeave ) CCmPluginEmbDestination( aInitParam );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	return self;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::~CCmPluginEmbDestination()
// ---------------------------------------------------------------------------
//
CCmPluginEmbDestination::~CCmPluginEmbDestination()
	{
	RemoveResourceFile( KPluginEmbDestinationDataResDirAndFileName );
	
    if(iDestination)
        {
        delete iDestination;
        iDestination = NULL;
        }	
	
	CLOG_CLOSE;
	}

// ----------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateInstanceL
// ----------------------------------------------------------------------------
//
CCmPluginBaseEng* 
    CCmPluginEmbDestination::CreateInstanceL( TCmPluginInitParam& aInitParam ) const
    {
    CCmPluginEmbDestination* self = new( ELeave ) CCmPluginEmbDestination( &aInitParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CCmPluginEmbDestination()
// ---------------------------------------------------------------------------
//
CCmPluginEmbDestination::CCmPluginEmbDestination( 
                                                TCmPluginInitParam* aInitParam )
    : CCmPluginBaseEng( aInitParam )
	{
	CLOG_CREATE;
	
	iBearerType = KUidEmbeddedDestination;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::ConstructL()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::ConstructL()
	{
	// Nothing is needed from CCmPluginBaseEng.
	AddResourceFileL( KPluginEmbDestinationDataResDirAndFileName );
	}
	
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetIntAttributeL()
// ---------------------------------------------------------------------------
//
TUint32 
    CCmPluginEmbDestination::GetIntAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::GetIntAttributeL" );

	TUint32 retVal( 0 );
	
    switch( aAttribute )
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
        
        case ECmIapId:
            {
            User::Leave(KErrNotSupported);
            }
            break;
        
        case ECmBearerIcon:
            {
            retVal = (TUint32)iDestination->IconL();    
            }
            break;
            
        case ECmDefaultPriority:
        case ECmDefaultUiPriority:
            {
            retVal = KDataMobilitySelectionPolicyPriorityWildCard;
            }
            break;
            
        case ECmExtensionLevel:
            {
            retVal = KExtensionBaseLevel;
            }
            break;
            
        case ECmInvalidAttribute:
            {
            retVal = 0;
            }
            break;

        case ECmLoadResult:
            {
            retVal = iLoadResult;
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
    
    return retVal;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetBoolAttributeL()
// ---------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::GetBoolAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::GetBoolAttributeL" );

	TBool retVal( EFalse );

    switch( aAttribute )
        {
        case ECmDestination:
            {
            retVal = ETrue;
            }
            break;
            
        case ECmCoverage:
            {
            retVal = EFalse;
            }
            break;
            
        case ECmProtected:
            {
            if( iDestination->ProtectionLevel() == EProtLevel1 ||
                iDestination->ProtectionLevel() == EProtLevel2 )
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
            retVal = iDestination->IsHidden();
            }
            break;
            
        case ECmBearerHasUi:
        case ECmAddToAvailableList:
            {
            retVal = EFalse;
            }
            break;

        case ECmVirtual:
            {
            retVal = ETrue;
            }
            break;

        case ECmIPv6Supported:
            {
            retVal = CCmPluginBaseEng::GetBoolAttributeL( aAttribute );
            }
            break;

        case ECmIsLinked:
            {
            retVal = EFalse;
            }
            break;
                        
        case ECmConnected:
            {
            retVal = iDestination->IsConnectedL();
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
        
    return retVal;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetStringAttributeL()
// ---------------------------------------------------------------------------
//
HBufC* 
  CCmPluginEmbDestination::GetStringAttributeL( const TUint32 aAttribute ) const
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::GetStringAttributeL" );

    HBufC* retVal = NULL;

    switch( aAttribute )
        {
        case ECmName:
    	    {
            RFs fs;
            CleanupClosePushL<RFs>( fs );
            User::LeaveIfError( fs.Connect() );

            // There may not be a UI context yet....
            TFileName resourceFile;
            _LIT( KDriveZ, "z:" );
            resourceFile.Append( KDriveZ );
            resourceFile.Append( KDC_RESOURCE_FILES_DIR );
            _LIT( KCmmResFileName, "cmmanager.rsc" );
            resourceFile.Append( KCmmResFileName );
            BaflUtils::NearestLanguageFile( fs, resourceFile );

            RResourceFile rf;
            CleanupClosePushL<RResourceFile>( rf );
            rf.OpenL( fs, resourceFile );
            rf.ConfirmSignatureL( 0 );
            HBufC8* readBuffer = rf.AllocReadLC( R_QTN_NETW_CONSET_EMBEDDED_DEST );
            const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                         ( readBuffer->Length() + 1 ) >> 1 );
            HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );
            CleanupStack::PushL( textBuffer );
            *textBuffer=ptrReadBuffer;
    
            TPtr format = textBuffer->Des();
            _LIT(KPercentS,"%S");
            _LIT(KPercentU,"%U");
            TInt puPos = format.Find(KPercentU);
            if ( puPos >= 0 )
                {
                format.Replace( puPos, 
                    static_cast<TDesC>(KPercentS).Length(), KPercentS );                
                }
            HBufC* desName = iDestination->NameLC();
            TPtr16 destName = desName->Des();
            retVal = HBufC::NewL( format.Length() + destName.Length() );
            retVal->Des().Format(format,  &destName);

            CleanupStack::PopAndDestroy( desName ); 
            CleanupStack::PopAndDestroy( textBuffer ); 
            CleanupStack::PopAndDestroy( readBuffer ); 
            CleanupStack::PopAndDestroy(); //rf
            CleanupStack::PopAndDestroy(); //fs
            }
    	    break;
    	    
    	default:
    	    {
    	    User::Leave( KErrNotSupported );
    	    }
        }
    
    return retVal;
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::GetString8AttributeL
// ---------------------------------------------------------------------------
//
HBufC8* 
    CCmPluginEmbDestination::GetString8AttributeL( const TUint32 aAttribute ) const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::GetString8AttributeL" );

    switch( aAttribute )
        {
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }
        
    // dead code
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::SetBoolAttributeL()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetBoolAttributeL( const TUint32 aAttribute, 
                                                 TBool /*aValue*/ )
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::SetBoolAttributeL" );

    switch( aAttribute )
        {
        case ECmProtected:
        case ECmHidden:
            break;
                    
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::SetStringAttributeL()
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::SetStringAttributeL( const TUint32 aAttribute, 
                                                   const TDesC16&  )
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::SetStringAttributeL" );

    switch( aAttribute )
        {
        case ECmName:
            {
            User::Leave( KErrNotSupported );
            }
            break;
            
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
	}

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CanHandleIapIdL()
// ---------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::CanHandleIapIdL( TUint32 /*aIapId*/ ) const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CanHandleIapIdL" );

    // Embedded Destination cannot handle any IAP id.
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CanHandleIapIdL()
// ---------------------------------------------------------------------------
//
TBool 
  CCmPluginEmbDestination::CanHandleIapIdL( CCDIAPRecord* /*aIapRecord*/ ) const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CanHandleIapIdL" );

    // Embedded Destination cannot handle any IAP id.
    return EFalse;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CCmDestinationImpl* CCmPluginEmbDestination::Destination() const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::Destination" );

    return iDestination;
    }
    
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::UpdateL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::UpdateL" );

    iDestination->UpdateL();
    }

// ---------------------------------------------------------------------------
// Delete embedded destination.
// ---------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::DeleteL( TBool aForced, 
                                        TBool aOneRefAllowed )
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::DeleteL" );
    
    (void)aForced;
    (void)aOneRefAllowed;
    
    // Embedded destination cannot be deleted.
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::LoadL( TUint32 aCmId)
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::LoadL" );

    if (!iDestination)
        {
        iCmId = aCmId;
        iDestination = iCmMgr.DestinationL( aCmId );        
        }
   }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::CreateNewL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CreateNewL" );
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::RunSettingsL()
// ---------------------------------------------------------------------------
//        
TInt CCmPluginEmbDestination::RunSettingsL()
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::RunSettingsL" );

    return 0;
	}
        
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::InitializeWithUiL()
// ---------------------------------------------------------------------------
//        
TBool CCmPluginEmbDestination::InitializeWithUiL( 
                                            TBool /*aManuallyConfigure*/ )
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::InitializeWithUiL" );

    // Has no UI
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::IsMultipleReferencedL()
// ---------------------------------------------------------------------------
//        
TBool CCmPluginEmbDestination::IsMultipleReferencedL()
	{
    LOGGER_ENTERFN( "CCmPluginEmbDestination::IsMultipleReferencedL" );

	return EFalse;
	}
	
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::LoadServiceSettingL()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::LoadServiceSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::LoadServiceSettingL" );
    
    User::Leave( KErrNotSupported );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::LoadBearerRecordL()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::LoadBearerSettingL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::LoadBearerSettingL" );
    
    User::Leave( KErrNotSupported );
    }
	
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateNewServiceRecordL()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::CreateNewServiceRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CreateNewServiceRecordL" );
    
    User::Leave( KErrNotSupported );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateNewBearerRecordL()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::CreateNewBearerRecordL()
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CreateNewBearerRecordL" );

    User::Leave( KErrNotSupported );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::ServiceRecordIdLC()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::ServiceRecordIdLC( HBufC* &/*aBearerName*/, 
                                                TUint32& /*aRecordId*/)
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::ServiceRecordIdLC" );

    User::Leave( KErrNotSupported );
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::BearerRecordIdLC()
// ---------------------------------------------------------------------------
//        
void CCmPluginEmbDestination::BearerRecordIdLC( HBufC* &/*aBearerName*/, 
                                                TUint32& /*aRecordId*/)
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::BearerRecordIdLC" );

    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CopyAdditionalDataL
// ---------------------------------------------------------------------------
//
void CCmPluginEmbDestination::CopyAdditionalDataL( CCmPluginBaseEng* /*aDestInst*/ ) const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CopyAdditionalDataL" );

    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::CreateCopyL
// ---------------------------------------------------------------------------
//
CCmPluginBaseEng* CCmPluginEmbDestination::CreateCopyL() const
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::CreateCopyL" );

    User::Leave(KErrNotSupported);
    return NULL;    
    }
    
// ---------------------------------------------------------------------------
// CCmPluginEmbDestination::IsLinkedToSnap
// ---------------------------------------------------------------------------
//
TBool CCmPluginEmbDestination::IsLinkedToSnap( TUint32 aSnapId )
    {
    LOGGER_ENTERFN( "CCmPluginEmbDestination::IsLinkedToSnap" );

    return ( aSnapId == iDestination->Id() );
    }
