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
* Description:  Implementation of RCmManager.
*
*/

#include <cmmanager.h>
#include <cmconnectionmethod.h>
#include <cmdestination.h>
#include "cmlogger.h"
#include "cmmanagerimpl.h"
#include "cmdestinationimpl.h"
#include "cmmanager.hrh"
#include <cmcommonconstants.h>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>

//-----------------------------------------------------------------------------
//  RCmManager::Close()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::Close()
    {
    CLOG_WRITE( "RCmManagerExt::Close()" );
    delete iImplementation;
    iImplementation = NULL;
    
    CLOG_CLOSE;
    }

//-----------------------------------------------------------------------------
//  RCmManager::OpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::OpenL()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("Manager_0x%x"), this );
    iImplementation = CCmManagerImpl::NewL(EFalse);
    CLOG_ATTACH( iImplementation, this );
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::CreateTablesAndOpenL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::CreateTablesAndOpenL()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("Manager_0x%x"), this );
    iImplementation = CCmManagerImpl::NewL();
    CLOG_ATTACH( iImplementation, this );
    }

//-----------------------------------------------------------------------------
//  RCmManager::OpenLC()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::OpenLC()
    {
    CLOG_CREATE;
    CLOG_NAME_1( _L("Manager_0x%x"), this );
    CleanupClosePushL(*this);
    iImplementation = NULL;
    iImplementation = CCmManagerImpl::NewL();
    CLOG_ATTACH( iImplementation, this );
    }

//-----------------------------------------------------------------------------
//  RCmManager::ConnectionMethodL( TUint aId )
//-----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmManager::ConnectionMethodL( 
                                                    TUint32 aIapId ) const
    {
    RCmConnectionMethod cm;
    CCmPluginBase* cM =  iImplementation->GetConnectionMethodL( aIapId );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;
    
    return cm;
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoIntL()
//-----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManager::GetBearerInfoIntL( TUint32 aBearerType,
                                                TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoIntL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoBoolL()
//-----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManager::GetBearerInfoBoolL( TUint32 aBearerType,
                                               TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoBoolL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManager::GetBearerInfoStringL( TUint32 aBearerType,
                                                  TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoStringL( aBearerType,
                                                       aAttribute );
    }

//-----------------------------------------------------------------------------
//  RCmManager::GetBearerInfoStringL()
//-----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManager::GetBearerInfoString8L( TUint32 aBearerType,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetBearerInfoString8L( aBearerType,
                                                       aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoIntL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 RCmManager::GetConnectionMethodInfoIntL( 
                                                    TUint32 aIapId,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoIntL( aIapId, aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoBoolL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmManager::GetConnectionMethodInfoBoolL( 
                                                    TUint32 aIapId,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoBoolL( aIapId, aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoStringL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* RCmManager::GetConnectionMethodInfoStringL( 
                                                    TUint32 aIapId,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoStringL( aIapId, aAttribute );
    }
    
// -----------------------------------------------------------------------------
// RCmManager::GetConnectionMethodInfoString8L()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmManager::GetConnectionMethodInfoString8L( 
                                                    TUint32 aIapId,
                                                    TUint32 aAttribute ) const
    {
    return iImplementation->GetConnectionMethodInfoString8L( aIapId, aAttribute );
    }
    
//=============================================================================
// CREATION / DELETION API
//=============================================================================
//

//-----------------------------------------------------------------------------
//  RCmManager::CreateDestinationL( const TDesC& aName )
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmManager::CreateDestinationL( const TDesC& aName )
    {
    LOGGER_ENTERFN( "RCmManager::CreateDestinationL" );

    RCmDestination dest;
    
    CCmDestinationImpl* destImp = iImplementation->CreateDestinationL( aName );
    dest.iDestinatonData = destImp->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    delete destImp;
    
    return dest;
    }

// -----------------------------------------------------------------------------
// RCmManager::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethod RCmManager::CreateConnectionMethodL(
    TUint32 aImplementationUid )
    {
    LOGGER_ENTERFN( "RCmManager::CreateConnectionMethodL" );

    RCmConnectionMethod cm;
        
    CCmPluginBase* cM =  
        iImplementation->CreateConnectionMethodL( aImplementationUid );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;

    return cm;
    }

//-----------------------------------------------------------------------------
// RCmManager::CopyL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManager::CopyConnectionMethodL(
    RCmDestination& aTargetDestination,
    RCmConnectionMethod& aConnectionMethod )
    {
    LOGGER_ENTERFN( "RCmManager::CopyConnectionMethodL" );

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
// RCmManager::MoveConnMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C TInt RCmManager::MoveConnectionMethodL( 
    RCmDestination& aSourceDestination,
    RCmDestination& aTargetDestination,
    RCmConnectionMethod& aConnectionMethod )
    {
    LOGGER_ENTERFN( "RCmManager::MoveConnectionMethodL" );

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
// RCmManager::RemoveConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::RemoveConnectionMethodL( 
    RCmDestination& aDestination,
    RCmConnectionMethod& aConnectionMethod )
    {
    LOGGER_ENTERFN( "RCmManager::RemoveConnectionMethodL" );

    CCmDestinationImpl* aDestImpl = 
            CCmDestinationImpl::NewLC(aDestination.iDestinatonData);
    aDestImpl->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        

    iImplementation->RemoveConnectionMethodL( *aDestImpl, *connMethod );

    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aDestImpl);
    }

//-----------------------------------------------------------------------------
// RCmManager::RemoveAllReferencesL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::RemoveAllReferencesL( 
    RCmConnectionMethod& aConnectionMethod )
    {
    LOGGER_ENTERFN( "RCmManager::RemoveAllReferencesL" );

    iImplementation->RemoveAllReferencesL( *aConnectionMethod.iImplementation );
    }

//-----------------------------------------------------------------------------
//  RCmManager::ConnectionMethodL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::ConnectionMethodL( RArray<TUint32>& aCMArray,
                                             TBool aCheckBearerType,
                                             TBool aLegacyOnly,
                                             TBool aEasyWlan ) const
    {
    LOGGER_ENTERFN( "RCmManager::ConnectionMethodL" );

    return iImplementation->ConnectionMethodL( aCMArray, 
                                               aCheckBearerType, 
                                               aLegacyOnly,
                                               aEasyWlan );
    }

//-----------------------------------------------------------------------------
//  RCmManager::AllDestinationsL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::AllDestinationsL( RArray<TUint32>& aDestArray ) const
    {
    LOGGER_ENTERFN( "RCmManager::AllDestinationsL" );

    return iImplementation->AllDestinationsL( aDestArray );
    }


//-----------------------------------------------------------------------------
//  RCmManager::DestinationL()
//-----------------------------------------------------------------------------
//  
EXPORT_C RCmDestination RCmManager::DestinationL( TUint32 aId ) const
    {
    LOGGER_ENTERFN( "RCmManager::DestinationL" );

    RCmDestination dest;
    
    CCmDestinationImpl* aDestImpl = iImplementation->DestinationL( aId );
    dest.iDestinatonData = aDestImpl->GetData( );
    dest.iDestinatonData->IncrementRefCounter();
    delete aDestImpl;
    
    return dest;
    }

//-----------------------------------------------------------------------------
//  RCmManager::BearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::BearerPriorityArrayL( 
                                        RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "RCmManager::BearerPriorityArray" );

    iImplementation->BearerPriorityArrayL( aArray ); 
    }

//-----------------------------------------------------------------------------
//  RCmManager::UpdateBearerPriorityArrayL()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::UpdateBearerPriorityArrayL( 
    const RArray<TBearerPriority>& aArray )
    {
    LOGGER_ENTERFN( "RCmManager::UpdateBearerPriorityArray" );    

    iImplementation->UpdateBearerPriorityArrayL( aArray );
    }

//-----------------------------------------------------------------------------
//  RCmManager::CleanupGlobalPriorityArray()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::CleanupGlobalPriorityArray( 
    RArray<TBearerPriority>& aArray ) const
    {
    LOGGER_ENTERFN( "RCmManager::CleanupGlobalPriorityArray" );

    iImplementation->CleanupGlobalPriorityArray( aArray ); 
    }

//-----------------------------------------------------------------------------
//  RCmManager::SupportedBearers()
//-----------------------------------------------------------------------------
//
EXPORT_C void RCmManager::SupportedBearersL( RArray<TUint32>& aArray ) const
    {
    LOGGER_ENTERFN( "RCmManager::SupportedBearersL" );

    return iImplementation->SupportedBearersL( aArray );        
    }

//-----------------------------------------------------------------------------
//  RCmManagerExt::UncategorizedIconL()
//-----------------------------------------------------------------------------
//    
EXPORT_C CGulIcon* RCmManager::UncategorizedIconL() const
    {
    LOGGER_ENTERFN( "RCmManager::UncategorizedIconL" );

    return iImplementation->UncategorizedIconL();        
    }

//-----------------------------------------------------------------------------
//  RCmManager::EasyWlanIdL()
//-----------------------------------------------------------------------------
//    
EXPORT_C TUint32 RCmManager::EasyWlanIdL() const
    {
    LOGGER_ENTERFN( "RCmManager::EasyWlanIdL" );

    return iImplementation->EasyWlanIdL();        
    }
    
//-----------------------------------------------------------------------------
//  RCmManagerExt::ReadDefConnL()
//-----------------------------------------------------------------------------
//    
EXPORT_C void RCmManager::ReadDefConnL( TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "RCmManager::ReadDefConnL" );

    return iImplementation->ReadDefConnL( aDCSetting );        
    }
    
//-----------------------------------------------------------------------------
//  RCmManager::WriteDefConnL()
//-----------------------------------------------------------------------------
//    
EXPORT_C void RCmManager::WriteDefConnL( const  TCmDefConnValue& aDCSetting )
    {
    LOGGER_ENTERFN( "RCmManager::WriteDefConnL" );

    return iImplementation->WriteDefConnL( aDCSetting );        
    }

//-----------------------------------------------------------------------------
//  RCmManager::ReadGenConnSettingsL()
//-----------------------------------------------------------------------------
//  
EXPORT_C void RCmManager::ReadGenConnSettingsL( TCmGenConnSettings& aGenConnSettings )
{
    LOGGER_ENTERFN( "RCmManager::ReadGenConnSettingsL" );

    return iImplementation->ReadGenConnSettingsL( aGenConnSettings ); 
}

//-----------------------------------------------------------------------------
//  RCmManager::WriteGenConnSettingsL()
//-----------------------------------------------------------------------------
//  
EXPORT_C void RCmManager::WriteGenConnSettingsL( const TCmGenConnSettings& aGenConnSettings )
{
    LOGGER_ENTERFN( "RCmManager::WriteGenConnSettingsL" );

    return iImplementation->WriteGenConnSettingsL( aGenConnSettings ); 
}

//-----------------------------------------------------------------------------
//  RCmManager::CreateDestinationL( const TDesC& aName,
//                                     TUint32 aDestId )
//-----------------------------------------------------------------------------
//
EXPORT_C RCmDestination RCmManager::CreateDestinationL(
    const TDesC& aName,
    TUint32 aDestId )
    {
    LOGGER_ENTERFN( "RCmManager::CreateDestinationL" );

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

    RCmDestination dest;

    CCmDestinationImpl* destImp =
                        iImplementation->CreateDestinationL( aName,
                                                             aDestId );
    dest.iDestinatonData = destImp->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    delete destImp;

    return dest;
    }

// -----------------------------------------------------------------------------
// RCmManager::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethod RCmManager::CreateConnectionMethodL(
    TUint32 aImplementationUid,
    TUint32 aConnMethodId )
    {
    LOGGER_ENTERFN( "RCmManager::CreateConnectionMethodL" );

    if ( aConnMethodId == 0 )
        {
        return CreateConnectionMethodL( aImplementationUid );
        }

    RCmConnectionMethod cm;

    CCmPluginBase* cM =
                   iImplementation->CreateConnectionMethodL( aImplementationUid,
                                                             aConnMethodId );
    cm.iImplementation = cM->Plugin();
    cm.iImplementation->IncrementRefCounter();
    delete cM;

    return cm;
    }
