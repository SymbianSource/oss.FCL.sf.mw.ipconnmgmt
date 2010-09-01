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
* Description:  Implementation of RCmDestination
*
*/

#include <cmdestination.h>
#include "cmdestinationimpl.h"
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>

using namespace CMManager;

// -----------------------------------------------------------------------------
// RCmDestination::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::Close()
    {
    if( !iDestinatonData )
        {
        return;
        }

    iDestinatonData->Cmmgr()->RemoveDestFromPool(iDestinatonData);     
    iDestinatonData = NULL;
    }
    

// -----------------------------------------------------------------------------
// RCmDestination::~RCmDestination()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::~RCmDestination()
    {
    Close();
    }


// -----------------------------------------------------------------------------
// RCmDestination::RCmDestination( RCmDestination& )
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::RCmDestination(
                                const RCmDestination& aCmDestination)    
    : iDestinatonData( aCmDestination.iDestinatonData )
    {
    iDestinatonData->IncrementRefCounter();
    }



// -----------------------------------------------------------------------------
// RCmDestination::RCmDestination::operator=()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestination& RCmDestination::operator=(
                                const RCmDestination& aCmDestination)
    {
    if (this != &aCmDestination) 
        {
        delete iDestinatonData;
        iDestinatonData = aCmDestination.iDestinatonData;        
        iDestinatonData->IncrementRefCounter();
        }
    return *this;
    }

// -----------------------------------------------------------------------------
// RCmDestination::ConnectionMethodCount()
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt RCmDestination::ConnectionMethodCount() const    
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), KErrBadHandle ) );

    return iDestinatonData->ConnectionMethodCount();
    };
        
// -----------------------------------------------------------------------------
// RCmDestination::ConnectionMethodL
// -----------------------------------------------------------------------------
//     
EXPORT_C RCmConnectionMethod 
                            RCmDestination::ConnectionMethodL( TInt anIndex ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethod cm;
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    cm.iImplementation = 
        aImplementation->GetConnectionMethodL( anIndex )->Plugin();
    //CCmDestinationImpl::GetConnectionMethodL doesn't give ownership
    cm.iImplementation->IncrementRefCounter();
    CleanupStack::PopAndDestroy(aImplementation);
    
    return cm;
    }

// -----------------------------------------------------------------------------
// RCmDestination::ConnectionMethodByIDL
// -----------------------------------------------------------------------------
//     
EXPORT_C RCmConnectionMethod 
                        RCmDestination::ConnectionMethodByIDL( TInt aIapId ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethod cm;
    
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    cm.iImplementation = 
        aImplementation->ConnectionMethodByIDL( aIapId )->Plugin();
    //CCmDestinationImpl::ConnectionMethodByIDL doesn't give ownership
    cm.iImplementation->IncrementRefCounter();
    CleanupStack::PopAndDestroy(aImplementation);
    
    return cm;
    }

// -----------------------------------------------------------------------------
// RCmDestination::PriorityL()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint RCmDestination::PriorityL(const RCmConnectionMethod& aCCMItem ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    TUint retValue = aImplementation->PriorityL( *aCCMItem.iImplementation );
    CleanupStack::PopAndDestroy(aImplementation);
    return retValue;
    }

// -----------------------------------------------------------------------------
// RCmDestination::NameLC()
// -----------------------------------------------------------------------------
//        
EXPORT_C HBufC* RCmDestination::NameLC ( ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    HBufC* retValue = aImplementation->NameLC();
    CleanupStack::Pop(retValue);
    CleanupStack::PopAndDestroy(aImplementation);
    CleanupStack::PushL(retValue);
    return retValue;
    };

// -----------------------------------------------------------------------------
// RCmDestination::Id()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint32 RCmDestination::Id() const
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );
    
    return iDestinatonData->Id();
    }

// -----------------------------------------------------------------------------
// RCmDestination::ElementId()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint32 RCmDestination::ElementId() const
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->ElementId();
    }

// -----------------------------------------------------------------------------
// RCmDestination::IconL()
// -----------------------------------------------------------------------------
//      
EXPORT_C CGulIcon* RCmDestination::IconL() const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    CGulIcon* retValue = aImplementation->IconL();
    CleanupStack::PopAndDestroy(aImplementation);
    return retValue;
    }


// -----------------------------------------------------------------------------
// RCmDestination::MetadataL()
// -----------------------------------------------------------------------------
//      
EXPORT_C 
    TUint32 RCmDestination::MetadataL( TSnapMetadataField aMetaField ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }
    return iDestinatonData->MetadataL( aMetaField );
    }

// -----------------------------------------------------------------------------
// RCmDestination::ProtectionLevel()
// -----------------------------------------------------------------------------
//  
EXPORT_C TProtectionLevel RCmDestination::ProtectionLevel() const
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->ProtectionLevel();
    }

// -----------------------------------------------------------------------------
// RCmDestination::IsHidden()
// -----------------------------------------------------------------------------
//          
EXPORT_C TBool RCmDestination::IsHidden() const
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    TBool hidden( EFalse );
    TRAPD( err, hidden = iDestinatonData->IsHiddenL() );
    __ASSERT_ALWAYS( err == KErrNone, User::Panic(_L( "CMManager" ), 
                                                  KErrArgument ) );
                                                  
    return hidden;
    }

// -----------------------------------------------------------------------------
// RCmDestination::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethod RCmDestination::CreateConnectionMethodL
   ( TUint32 aImplementationUid )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethod cm;
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    CCmPluginBase* imp = aImplementation->CreateConnectionMethodL( 
                                        aImplementationUid );
    imp->IncrementRefCounter();
    CleanupStack::PushL(imp);                                    
    cm.iImplementation = imp->Plugin();
    CleanupStack::PopAndDestroy(imp);
    
    CleanupStack::PopAndDestroy(aImplementation);
    return cm;
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethod RCmDestination::CreateConnectionMethodL( 
    TUint32 aImplementationUid,
    TUint32  aConnMethId )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethod cm;
    CCmDestinationImpl* implementation = 
                                     CCmDestinationImpl::NewLC( iDestinatonData );
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    CCmPluginBase* imp = implementation->CreateConnectionMethodL( 
                                                            aImplementationUid,
                                                            aConnMethId );
    imp->IncrementRefCounter();
    CleanupStack::PushL(imp);                                    
    cm.iImplementation = imp->Plugin();
    CleanupStack::PopAndDestroy(imp);

    CleanupStack::PopAndDestroy(implementation);
    return cm;
    }

// -----------------------------------------------------------------------------
// RCmDestination::AddConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt RCmDestination::AddConnectionMethodL( 
    RCmConnectionMethod aConnectionMethod )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }
        
    CCmDestinationImpl* implementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();

    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        

    // Check if the given connection method exists already in this destination
    if ( aConnectionMethod.iImplementation->IdIsValid() &&
         implementation->CheckIfCMExistsL( *connMethod ) )
        {
        CleanupStack::PopAndDestroy( connMethod );
        CleanupStack::PopAndDestroy( implementation );
        User::Leave( KErrAlreadyExists );
        }

    TInt retValue =  implementation->AddConnectionMethodL( *connMethod );
    CleanupStack::PopAndDestroy( connMethod );
    CleanupStack::PopAndDestroy( implementation );
    return retValue;
    }

// -----------------------------------------------------------------------------
// RCmDestination::AddEmbeddedDestinationL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt RCmDestination::AddEmbeddedDestinationL( 
    const RCmDestination& aDestination )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }
    
    if ( this == &aDestination )
        {
        User::Leave( KErrArgument );
        }
    
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting will decrement the ref counter
    aImplementation->IncrementRefCounter();
    CCmDestinationImpl* aDestToAdd = 
            CCmDestinationImpl::NewLC(aDestination.iDestinatonData);
    // Deleting will decrement the ref counter
    aDestToAdd->IncrementRefCounter();
        
    TInt retValue =  aImplementation->AddEmbeddedDestinationL( 
                                                *aDestToAdd );        
    CleanupStack::PopAndDestroy(aDestToAdd);
    CleanupStack::PopAndDestroy(aImplementation);
    return retValue;
    }

// -----------------------------------------------------------------------------
// RCmDestination::DeleteConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C void RCmDestination::DeleteConnectionMethodL( 
    RCmConnectionMethod& aConnectionMethod )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aConnectionMethod.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        
    aImplementation->DeleteConnectionMethodL( 
                                           *connMethod );
    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aImplementation);
    }
    
// -----------------------------------------------------------------------------
// RCmDestination::RemoveConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C void RCmDestination::RemoveConnectionMethodL( 
    RCmConnectionMethod aConnectionMethod )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->RemoveConnectionMethodL( 
                                           *aConnectionMethod.iImplementation );
    CleanupStack::PopAndDestroy(aImplementation);
    }
    
// -----------------------------------------------------------------------------
// RCmDestination::ModifyPriorityL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestination::ModifyPriorityL( 
    RCmConnectionMethod& aCCmItem, 
    TUint aIndex )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    CCmPluginBase* connMethod = new (ELeave) 
                            CCmPluginBase(aCCmItem.iImplementation);
    connMethod->IncrementRefCounter();    
    CleanupStack::PushL( connMethod );        
    aImplementation->ModifyPriorityL( *connMethod, aIndex );
    CleanupStack::PopAndDestroy(connMethod);
    CleanupStack::PopAndDestroy(aImplementation);
    }
    
// -----------------------------------------------------------------------------
// RCmDestination::SetNameL()
// -----------------------------------------------------------------------------
//        
EXPORT_C void RCmDestination::SetNameL( const TDesC& aName )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->SetNameL( aName );
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::SetIconL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestination::SetIconL( TUint32 anIndex )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    if ( anIndex > KNumOfIcons )
        {
        User::Leave( KErrArgument );
        }
        
    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->SetIconL( anIndex );        
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::SetMetadataL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestination::SetMetadataL( TSnapMetadataField aMetaField, 
                                            TUint32 aValue ) 
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->SetMetadataL( aMetaField, aValue );
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::SetProtectionL()
// -----------------------------------------------------------------------------
//  
EXPORT_C void RCmDestination::SetProtectionL( TProtectionLevel aProtLevel )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->SetProtectionL( aProtLevel );
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::SetHiddenL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void RCmDestination::SetHiddenL( TBool aHidden )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->SetHiddenL( aHidden );         
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::UpdateL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void RCmDestination::UpdateL()
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    aImplementation->UpdateL();
    CleanupStack::PopAndDestroy(aImplementation);
    }

// -----------------------------------------------------------------------------
// RCmDestination::DeleteLD()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmDestination::DeleteLD()
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    iDestinatonData->IncrementRefCounter();
    aImplementation->DeleteLD();  
    CleanupStack::Pop(aImplementation);
    iDestinatonData = NULL;
    }

// -----------------------------------------------------------------------------
// RCmDestination::IsConnectedL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::IsConnectedL() const 
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    CCmDestinationImpl* aImplementation = 
            CCmDestinationImpl::NewLC(iDestinatonData);
    // Deleting aImplementation will decrement the ref counter
    iDestinatonData->IncrementRefCounter();
    TBool retValue = aImplementation->IsConnectedL();    
    CleanupStack::PopAndDestroy(aImplementation);
    return retValue;
    }
    
// -----------------------------------------------------------------------------
// RCmDestination::RCmDestination
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestination::RCmDestination()    
    : iDestinatonData( NULL )
    {
    }

// -----------------------------------------------------------------------------
// RCmDestination::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::operator==(const RCmDestination& aDestination ) const
    {
    return ( iDestinatonData == aDestination.iDestinatonData );
    };

// -----------------------------------------------------------------------------
// RCmDestination::operator!=
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestination::operator!=(const RCmDestination& aDestination ) const
    {
    return !( iDestinatonData == aDestination.iDestinatonData );
    };
    
// End of file
