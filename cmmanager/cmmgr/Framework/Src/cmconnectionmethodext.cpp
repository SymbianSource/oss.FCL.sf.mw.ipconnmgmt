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
* Description:  Implementation of RCmConnectionMethodExt
*
*/

// INCLUDES
#include "cmmanagerimpl.h"
#include <cmdestinationext.h>
#include <cmpluginbase.h>
#include <cmconnectionmethodext.h>
#include "cmdestinationimpl.h"

using namespace CMManager;
// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::RCmConnectionMethodExt()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::RCmConnectionMethodExt(
                                const RCmConnectionMethodExt& aItem)
    : iImplementation( aItem.iImplementation )
    {
    iImplementation->IncrementRefCounter();
    }


// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::Close()
    {
    if( !iImplementation )
        {
        return;
        }

    TRAP_IGNORE( iImplementation->CmMgr().RemoveCMFromPoolL(iImplementation) );
    iImplementation = NULL;
    }


// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::CreateCopyL()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt RCmConnectionMethodExt::CreateCopyL()
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    RCmConnectionMethodExt cm;
    
    cm.iImplementation = iImplementation->CreateCopyL();

    cm.iImplementation->CmMgr().InsertConnectionMethodL(cm.iImplementation);
    
    return cm;
    }
    

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::GetIntAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 
            RCmConnectionMethodExt::GetIntAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetIntAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::GetBoolAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool 
           RCmConnectionMethodExt::GetBoolAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }

    
    return iImplementation->GetBoolAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::GetStringAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* 
        RCmConnectionMethodExt::GetStringAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }

    
    return iImplementation->GetStringAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::GetString8AttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* 
        RCmConnectionMethodExt::GetString8AttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetString8AttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::SetIntAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetIntAttributeL( TUint32 aAttribute, 
                                                        TUint32 aValue )
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    if( iImplementation->GetBoolAttributeL( ECmProtected ) )
        {
        // Only clients with NetworkControl capability can modify
        // protection setting!
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }

    iImplementation->SetIntAttributeL( aAttribute, aValue );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::SetBoolAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::SetBoolAttributeL( TUint32 aAttribute, 
                                                         TBool aValue )
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }

    if( iImplementation->GetBoolAttributeL( ECmProtected ) )
        {
        // Only clients with NetworkControl capability can modify
        // protection setting!
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    iImplementation->SetBoolAttributeL( aAttribute, aValue );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::SetStringAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void 
            RCmConnectionMethodExt::SetStringAttributeL( TUint32 aAttribute, 
                                                         const TDesC16& aValue )
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    if( iImplementation->GetBoolAttributeL( ECmProtected ) )
        {
        // Only clients with NetworkControl capability can modify
        // protection setting!
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }

    iImplementation->SetStringAttributeL( aAttribute, aValue );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::SetString8AttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void 
            RCmConnectionMethodExt::SetString8AttributeL( TUint32 aAttribute, 
                                                          const TDesC8& aValue )
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    if( iImplementation->GetBoolAttributeL( ECmProtected ) )
        {
        // Only clients with NetworkControl capability can modify
        // protection setting!
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }
        
    iImplementation->SetString8AttributeL( aAttribute, aValue );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::UpdateL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethodExt::UpdateL()
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    iImplementation->UpdateL();
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::DeleteL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::DeleteL()
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }

    if( iImplementation->GetBoolAttributeL( ECmProtected ) )
        {
        // Only clients with NetworkControl capability can delete
        // this destination
        CCmManagerImpl::HasCapabilityL( ECapabilityNetworkControl );
        }

    // Check it's not connected
    if ( iImplementation->GetBoolAttributeL( ECmConnected ) )
        {
        User::Leave( KErrInUse );
        }

    // Check it's not referenced by Virtual CM
    if( iImplementation->GetBoolAttributeL( ECmIsLinked ) )
        {
        User::Leave( KErrLocked );
        }

    // Check that it's not the last CM in a destination which is
    // referenced by Virtual CM
    CCmDestinationImpl* destImpl = iImplementation->ParentDestination();
    TUint32 destId ( 0 );
    TUint32 count( 0 ); 
    
    if ( destImpl )
        {
        destId = destImpl->Id();
        count = destImpl->ConnectionMethodCount();
        }
    delete destImpl;
    destImpl = NULL;

    if ( count  == 1 )
        {
        // The IAP is in destination and it's the last one
            // for each IAP in CM manager
            //   1. check if it is virtual
            //      if not => goto 1.
            //      if yes:
            //      2. check if it links to the destination of this CM
            //         if yes => carryOn = EFalse, ERROR
            //         if not: carryOn = ETrue
        TBool carryOn = ETrue;
        
        CommsDat::CMDBRecordSet<CommsDat::CCDIAPRecord>* iaps = iImplementation->CmMgr().AllIapsL();
        CleanupStack::PushL( iaps );

        // for each IAP in CM manager
        for ( TInt i = KCmInitIndex; carryOn && i < iaps->iRecords.Count(); ++i )
            {
            CommsDat::CCDIAPRecord* rec = (*iaps)[i];
            TUint32 bearerType = 0;

            TRAP_IGNORE( bearerType = iImplementation->CmMgr().BearerTypeFromIapRecordL( rec ) );
            if ( !bearerType )
                {
                continue;
                }

            // check if it is virtual
            if ( iImplementation->CmMgr().GetBearerInfoBoolL( bearerType, ECmVirtual ) )
                {
                // check if it links to the current destination
                CCmPluginBase* plugin = NULL;            
                TRAP_IGNORE( plugin = iImplementation->CmMgr().GetConnectionMethodL( rec->RecordId() ) );

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

    // ETrue = delete it. It doesn't matter how many referencies there are.
    TBool deleteRes = iImplementation->DeleteL( ETrue );
    if( deleteRes )
        {
        CCmDestinationImpl* dest = iImplementation->ParentDestination();
        if( dest )
            {
            CCmPluginBase* connMethod = new (ELeave) 
                                    CCmPluginBase(iImplementation);
            CleanupStack::PushL( connMethod );        
            dest->ConnectionMethodDeletedL( *connMethod );
            CleanupStack::PopAndDestroy(connMethod);//deletes iImplementation
            iImplementation = NULL;
            }
        delete dest;
        }
        
    return deleteRes;
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::Destination()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmDestinationExt RCmConnectionMethodExt::DestinationL() const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }        
    if (!iImplementation->Destination())
        {
        User::Leave(KErrNotSupported);
        }
    
    RCmDestinationExt dest;
    dest.iDestinatonData = 
                iImplementation->Destination()->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    return dest;
    }
// --------------------------------------------------------------------------
// RCmConnectionMethod::RCmConnectionMethod
// --------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::RCmConnectionMethodExt()
    : iImplementation( NULL )
    {
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethodExt::~RCmConnectionMethodExt()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt::~RCmConnectionMethodExt()
    {
    Close();
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::operator=()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethodExt& RCmConnectionMethodExt::operator=(
                                const RCmConnectionMethodExt& aConnMethod)
    {
    if (this != &aConnMethod) 
        {
        Close();
        iImplementation = aConnMethod.iImplementation;
        iImplementation->IncrementRefCounter();
        }
    return *this;
    }

// --------------------------------------------------------------------------
// RCmConnectionMethod::operator==
// --------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::operator==( 
                                    RCmConnectionMethodExt& aConnMethod ) const
    {
    return ( iImplementation == aConnMethod.iImplementation );
    };


// --------------------------------------------------------------------------
// RCmConnectionMethod::operator!=
// --------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethodExt::operator!=( 
                                    RCmConnectionMethodExt& aConnMethod ) const
    {
    return !( iImplementation == aConnMethod.iImplementation );
    };
