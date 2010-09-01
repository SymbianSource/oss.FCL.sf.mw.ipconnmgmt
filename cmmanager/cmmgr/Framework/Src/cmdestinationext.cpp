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
* Description:  Implementation of RCmDestinationExt
*
*/

#include <cmdestinationext.h>
#include "cmdestinationimpl.h"
#include <cmconnectionmethodext.h>
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include "cmmanagerimpl.h"

using namespace CMManager;

// -----------------------------------------------------------------------------
// RCmDestinationExt::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::Close()
    {
    if( !iDestinatonData )
        {
        return;
        }

    iDestinatonData->Cmmgr()->RemoveDestFromPool(iDestinatonData);     
    iDestinatonData = NULL;
    }
    

// -----------------------------------------------------------------------------
// RCmDestinationExt::~RCmDestinationExt()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::~RCmDestinationExt()
    {
    Close();
    }


// -----------------------------------------------------------------------------
// RCmDestinationExt::RCmDestinationExt( RCmDestinationExt& )
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::RCmDestinationExt(
                                const RCmDestinationExt& aCmDestination)    
    : iDestinatonData( aCmDestination.iDestinatonData )
    {
    iDestinatonData->IncrementRefCounter();
    }



// -----------------------------------------------------------------------------
// RCmDestinationExt::RCmDestinationExt::operator=()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt& RCmDestinationExt::operator=(
                                const RCmDestinationExt& aCmDestination)
    {
    if (this != &aCmDestination) 
        {
        Close();
        iDestinatonData = aCmDestination.iDestinatonData;        
        iDestinatonData->IncrementRefCounter();
        }
    return *this;
    }
    

// -----------------------------------------------------------------------------
// RCmDestinationExt::ConnectionMethodCount()
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt RCmDestinationExt::ConnectionMethodCount()    
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->ConnectionMethodCount();
    };
    
// -----------------------------------------------------------------------------
// RCmDestinationExt::ConnectionMethodL
// -----------------------------------------------------------------------------
//     
EXPORT_C RCmConnectionMethodExt 
                            RCmDestinationExt::ConnectionMethodL( TInt anIndex )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethodExt cm;
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
// RCmDestinationExt::ConnectionMethodByIDL
// -----------------------------------------------------------------------------
//     
EXPORT_C RCmConnectionMethodExt 
                        RCmDestinationExt::ConnectionMethodByIDL( TInt aIapId )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethodExt cm;
    
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
// RCmDestinationExt::PriorityL()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint RCmDestinationExt::PriorityL(const RCmConnectionMethodExt& aCCMItem )
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
// RCmDestinationExt::NameLC()
// -----------------------------------------------------------------------------
//        
EXPORT_C HBufC* RCmDestinationExt::NameLC ( )
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
// RCmDestinationExt::Id()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint32 RCmDestinationExt::Id() 
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->Id();
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::ElementId()
// -----------------------------------------------------------------------------
//        
EXPORT_C TUint32 RCmDestinationExt::ElementId() 
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->ElementId();
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::IconL()
// -----------------------------------------------------------------------------
//      
EXPORT_C CGulIcon* RCmDestinationExt::IconL() 
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
// RCmDestinationExt::MetadataL()
// -----------------------------------------------------------------------------
//      
EXPORT_C 
    TUint32 RCmDestinationExt::MetadataL( TSnapMetadataField aMetaField ) const
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }
    return iDestinatonData->MetadataL( aMetaField );
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::ProtectionLevel()
// -----------------------------------------------------------------------------
//  
EXPORT_C TProtectionLevel RCmDestinationExt::ProtectionLevel()
    {
    __ASSERT_ALWAYS( iDestinatonData, User::Panic(_L( "CMManager" ), 
                                                  KErrBadHandle ) );

    return iDestinatonData->ProtectionLevel();
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::IsHidden()
// -----------------------------------------------------------------------------
//          
EXPORT_C TBool RCmDestinationExt::IsHidden()
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
// RCmDestinationExt::CreateConnectionMethodL()
// -----------------------------------------------------------------------------
//  
EXPORT_C RCmConnectionMethodExt 
        RCmDestinationExt::CreateConnectionMethodL( TUint32 aImplementationUid )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethodExt cm;
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
EXPORT_C RCmConnectionMethodExt RCmDestinationExt::CreateConnectionMethodL( 
    TUint32 aImplementationUid,
    TUint32  aConnMethId )
    {
    if (!iDestinatonData)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethodExt cm;
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
// RCmDestinationExt::AddConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt RCmDestinationExt::AddConnectionMethodL( 
                                      RCmConnectionMethodExt aConnectionMethod )
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
// RCmDestinationExt::AddEmbeddedDestinationL()
// -----------------------------------------------------------------------------
// 
EXPORT_C TInt RCmDestinationExt::AddEmbeddedDestinationL( 
                                           const RCmDestinationExt& aDestination )
    {
    if ( !iDestinatonData )
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
// RCmDestinationExt::DeleteConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C void RCmDestinationExt::DeleteConnectionMethodL( 
                                      RCmConnectionMethodExt& aConnectionMethod )
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
// RCmDestinationExt::RemoveConnectionMethodL()
// -----------------------------------------------------------------------------
// 
EXPORT_C void RCmDestinationExt::RemoveConnectionMethodL( 
                                      RCmConnectionMethodExt aConnectionMethod )
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
// RCmDestinationExt::ModifyPriorityL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestinationExt::ModifyPriorityL( 
                                                RCmConnectionMethodExt& aCCmItem, 
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
// RCmDestinationExt::SetNameL()
// -----------------------------------------------------------------------------
//        
EXPORT_C void RCmDestinationExt::SetNameL( const TDesC& aName )
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
// RCmDestinationExt::SetIconL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestinationExt::SetIconL( TUint32 anIndex )
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
// RCmDestinationExt::SetMetadataL()
// -----------------------------------------------------------------------------
//      
EXPORT_C void RCmDestinationExt::SetMetadataL( TSnapMetadataField aMetaField, 
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
// RCmDestinationExt::SetProtectionL()
// -----------------------------------------------------------------------------
//  
EXPORT_C void RCmDestinationExt::SetProtectionL( TProtectionLevel aProtLevel )
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
// RCmDestinationExt::SetHiddenL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void RCmDestinationExt::SetHiddenL( TBool aHidden )
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
// RCmDestinationExt::UpdateL()
// -----------------------------------------------------------------------------
//          
EXPORT_C void RCmDestinationExt::UpdateL()
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
// RCmDestinationExt::DeleteLD()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmDestinationExt::DeleteLD()
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
// RCmDestinationExt::IsConnectedL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::IsConnectedL() const 
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
// RCmDestinationExt::RCmDestinationExt
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt::RCmDestinationExt()    
    : iDestinatonData( NULL )
    {
    }

// -----------------------------------------------------------------------------
// RCmDestinationExt::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::operator==(const RCmDestinationExt& aDestination ) const
    {
    return ( iDestinatonData == aDestination.iDestinatonData );
    };

// -----------------------------------------------------------------------------
// RCmDestinationExt::operator!=
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmDestinationExt::operator!=(const RCmDestinationExt& aDestination ) const
    {
    return !( iDestinatonData == aDestination.iDestinatonData );
    };
    
// End of file
