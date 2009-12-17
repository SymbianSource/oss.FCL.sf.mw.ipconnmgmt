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
* Description:  Implementation of RCmManagerExt
*
*/

#include <cmmanagerext.h>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include <cmdestinationext.h>
#include "cmlogger.h"
#include "cmmanager.hrh"
#include <cmcommonconstants.h>
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include "cmmanager.hrh"
#include <data_caging_path_literals.hrh>

#include <StringLoader.h>
#include <e32cmn.h>
#include <cmmanager.rsg>    // resource

//-----------------------------------------------------------------------------
//  RCmManagerExt::~RCmManagerExt()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::Close()
    {
    CLOG_WRITE( "RCmManagerExt::Close()" );
    delete iImplementation;
    iImplementation = NULL;
    
    CLOG_CLOSE;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConstructL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::OpenL()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("ManagerExt_0x%x"), this );
    iImplementation = CCmManagerImpl::NewL(EFalse);
    CLOG_ATTACH( iImplementation, this );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConstructL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::CreateTablesAndOpenL()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("ManagerExt_0x%x"), this );
    iImplementation = CCmManagerImpl::NewL();
    CLOG_ATTACH( iImplementation, this );
    }
    
//-----------------------------------------------------------------------------
//  RCmManagerExt::OpenLC()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::OpenLC()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("Manager_0x%x"), this );
    CleanupClosePushL(*this);
    iImplementation = NULL;
    iImplementation = CCmManagerImpl::NewL();
    CLOG_ATTACH( iImplementation, this );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManagerExt::GetBearerInfoIntL( TUint32 aBearerType,
                                                   TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoIntL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManagerExt::GetBearerInfoBoolL( TUint32 aBearerType,
                                                  TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoBoolL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManagerExt::GetBearerInfoStringL( TUint32 aBearerType,
                                                     TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoStringL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::GetBearerInfoString8L()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* 
                RCmManagerExt::GetBearerInfoString8L( TUint32 aBearerType,
                                                      TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoString8L( aBearerType,
                                                   aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmManagerExt::GetConnectionMethodInfoIntL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManagerExt::GetConnectionMethodInfoIntL( TUint32 aIapId,
                                     TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoIntL( aIapId, aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManagerExt::GetConnectionMethodInfoBoolL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManagerExt::GetConnectionMethodInfoBoolL( TUint32 aIapId,
                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoBoolL( aIapId, aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManagerExt::GetConnectionMethodInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManagerExt::GetConnectionMethodInfoStringL( TUint32 aIapId,
                                       TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoStringL( aIapId, 
                                                            aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManagerExt::GetConnectionMethodInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* 
    RCmManagerExt::GetConnectionMethodInfoString8L( TUint32 aIapId,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoString8L( aIapId, aAttribute );
    }
    
//=============================================================================
// CREATION / DELETION API
//=============================================================================
//

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateDestinationL( const TDesC& aName )
//-----------------------------------------------------------------------------
//
EXPORT_C 
    RCmDestinationExt RCmManagerExt::CreateDestinationL( const TDesC& aName )
    {
    RCmDestinationExt dest;
    
    CCmDestinationImpl* destImp = iImplementation->CreateDestinationL( aName );
    dest.iDestinatonData = destImp->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    delete destImp;
    
    return dest;
    }

// -----------------------------------------------------------------------------
// RCmManagerExt::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethodExt 
        RCmManagerExt::CreateConnectionMethodL( TUint32 aImplementationUid )
    {
    RCmConnectionMethodExt cm;
        
    CCmPluginBase* cM =  
        iImplementation->CreateConnectionMethodL( aImplementationUid );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;

    return cm;
    }

//-----------------------------------------------------------------------------
// RCmManagerExt::CopyL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManagerExt::CopyConnectionMethodL( 
                                      RCmDestinationExt& aTargetDestination,
                                      RCmConnectionMethodExt& aConnectionMethod )
    {
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(aTargetDestination.iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    aImplementation->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        
    TInt retVal = iImplementation->CopyConnectionMethodL( 
                                           *aImplementation, 
                                           *connMethod );
    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aImplementation);
    return retVal;
    }

//-----------------------------------------------------------------------------
// RCmManagerExt::MoveConnMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManagerExt::MoveConnectionMethodL( 
                                      RCmDestinationExt& aSourceDestination,
                                      RCmDestinationExt& aTargetDestination,
                                      RCmConnectionMethodExt& aConnectionMethod )
    {
    CCmDestinationImpl* aSourceImpl = 
            CCmDestinationImpl::NewLC(aSourceDestination.iDestinatonData);
    aSourceImpl->IncrementRefCounter();
    CCmDestinationImpl* aTargetImpl = 
            CCmDestinationImpl::NewLC(aTargetDestination.iDestinatonData);
    aTargetImpl->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        
    TInt retVal = iImplementation->MoveConnectionMethodL( 
                    *aSourceImpl, 
                    *aTargetImpl, 
                    *connMethod );
    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aTargetImpl);
    CleanupStack::PopAndDestroy(aSourceImpl);
    return retVal;
    }

//-----------------------------------------------------------------------------
// RCmManagerExt::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::RemoveConnectionMethodL( 
                                      RCmDestinationExt& aDestination,
                                      RCmConnectionMethodExt& aConnectionMethod )
    {
    CCmDestinationImpl* aDestImpl = 
            CCmDestinationImpl::NewLC(aDestination.iDestinatonData);
    aDestImpl->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        
    iImplementation->RemoveConnectionMethodL( 
                    *aDestImpl, 
                    *connMethod );
    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aDestImpl);
    }

//-----------------------------------------------------------------------------
// RCmManagerExt::RemoveAllReferencesL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::RemoveAllReferencesL( 
                                    RCmConnectionMethodExt& aConnectionMethod )
    {
    iImplementation->RemoveAllReferencesL( *aConnectionMethod.iImplementation );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConnectionMethodExtL()
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt 
                              RCmManagerExt::ConnectionMethodL( TUint32 aIapId )
    {
    RCmConnectionMethodExt cm;
    CCmPluginBase* cM =  iImplementation->GetConnectionMethodL( aIapId );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;
        
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::ConnectionMethodL( RArray<TUint32>& aCMArray,
                                                TBool aCheckBearerType,
                                                TBool aLegacyOnly,
                                                TBool aEasyWlan )
    {
    iImplementation->ConnectionMethodL( aCMArray, 
                                               aCheckBearerType,
                                               aLegacyOnly,
                                               aEasyWlan );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::DestinationL()
//-----------------------------------------------------------------------------
//  
EXPORT_C RCmDestinationExt RCmManagerExt::DestinationL( TUint32 aId )
    {
    RCmDestinationExt dest;
    
    CCmDestinationImpl* aDestImpl = iImplementation->DestinationL( aId );
    dest.iDestinatonData = aDestImpl->GetData( );
    dest.iDestinatonData->IncrementRefCounter();
    delete aDestImpl;
    
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::AllDestinationsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::AllDestinationsL( RArray<TUint32>& aDestArray )
    {   
    return iImplementation->AllDestinationsL( aDestArray );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::BearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::BearerPriorityArrayL( 
                                        RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "RCmManagerExt:BearerPriorityArray" );
    iImplementation->BearerPriorityArrayL( aArray ); 
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::UpdateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::UpdateBearerPriorityArrayL( 
                                        const RArray<TBearerPriority>& aArray )
    {
    LOGGER_ENTERFN( "RCmManagerExt::UpdateBearerPriorityArray" );    
    iImplementation->UpdateBearerPriorityArrayL( aArray );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CleanupGlobalPriorityArray()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::CleanupGlobalPriorityArray( 
                                        RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "RCmManagerExt:CleanupGlobalPriorityArray" );
    iImplementation->CleanupGlobalPriorityArray( aArray ); 
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::SupportedBearers()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManagerExt::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    return iImplementation->SupportedBearersL( aArray );        
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::UncategorizedIconL()
//-----------------------------------------------------------------------------
//    
EXPORT_C CGulIcon* RCmManagerExt::UncategorizedIconL() const
    {
    return iImplementation->UncategorizedIconL();        
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::EasyWlanIdL()
//-----------------------------------------------------------------------------
//    
EXPORT_C TUint32 RCmManagerExt::EasyWlanIdL()
    {
    return iImplementation->EasyWlanIdL();        
    }
    
//-----------------------------------------------------------------------------
//  RCmManagerExt::ReadDefConnL()
//-----------------------------------------------------------------------------
//    
EXPORT_C void RCmManagerExt::ReadDefConnL( TCmDefConnValue& aDCSetting )
    {
    return iImplementation->ReadDefConnL( aDCSetting );        
    }
    
//-----------------------------------------------------------------------------
//  RCmManagerExt::WriteDefConnL()
//-----------------------------------------------------------------------------
//    
EXPORT_C void RCmManagerExt::WriteDefConnL( const  TCmDefConnValue& aDCSetting )
    {
    return iImplementation->WriteDefConnL( aDCSetting );        
    }

 
//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateDestinationL( const TDesC& aName,
//                                     TUint32 aDestId )
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmManagerExt::CreateDestinationL(
    const TDesC& aName,
    TUint32 aDestId )
    {
    if ( aDestId == 0 )
        {
        return CreateDestinationL( aName );
        }

    // Check if there already exists a destination with the same Id
    RArray<TUint32> destIdArray = RArray<TUint32>( KCmArrayBigGranularity );
    iImplementation->AllDestinationsL( destIdArray );
    for ( TInt i = 0; i < destIdArray.Count(); i++ )
        {
        if ( aDestId > 0 && aDestId < 255 )
            {
            aDestId = aDestId + KCmDefaultDestinationAPTagId;
            }
        else if ( aDestId < KCmDefaultDestinationAPTagId+1 
                  && ( aDestId > ( KCmDefaultDestinationAPTagId+255 ) ) )
            {
            User::Leave( KErrArgument );
            }
        if ( aDestId == destIdArray[i] )
            {
            destIdArray.Close();
            User::Leave( KErrAlreadyExists );
            }
        }
    destIdArray.Close();

    RCmDestinationExt dest;

    CCmDestinationImpl* destImp =
                        iImplementation->CreateDestinationL( aName,
                                                             aDestId );
    dest.iDestinatonData = destImp->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    delete destImp;

    return dest;
    }

// -----------------------------------------------------------------------------
// RCmManagerExt::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethodExt RCmManagerExt::CreateConnectionMethodL(
    TUint32 aImplementationUid,
    TUint32 aConnMethodId )
    {
    if ( aConnMethodId == 0 )
        {
        return CreateConnectionMethodL( aImplementationUid );
        }

    RCmConnectionMethodExt cm;

    CCmPluginBase* cM =
                   iImplementation->CreateConnectionMethodL( aImplementationUid,
                                                             aConnMethodId );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;

    return cm;
    }
