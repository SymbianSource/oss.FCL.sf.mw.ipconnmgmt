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
* Description:  Implementation of RCmConnectionMethod
*
*/

// INCLUDES
#include "cmmanagerimpl.h"
#include <cmdestination.h>
#include <cmpluginbase.h>
#include <cmpluginbaseeng.h>
#include <cmconnectionmethod.h>
#include "cmdestinationimpl.h"

using namespace CMManager;
// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// RCmConnectionMethod::RCmConnectionMethod()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::RCmConnectionMethod(
                                const RCmConnectionMethod& aItem)
    : iImplementation( aItem.iImplementation )
    {
    iImplementation->IncrementRefCounter();
    }


// -----------------------------------------------------------------------------
// RCmConnectionMethod::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::Close()
    {
    if( !iImplementation )
        {
        return;
        }

    TRAP_IGNORE( iImplementation->CmMgr().RemoveCMFromPoolL(iImplementation) );
    iImplementation = NULL;
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::~RCmConnectionMethod()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::~RCmConnectionMethod()
    {
    Close();
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::GetIntAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 
            RCmConnectionMethod::GetIntAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetIntAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::GetBoolAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool 
            RCmConnectionMethod::GetBoolAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetBoolAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::GetStringAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* 
            RCmConnectionMethod::GetStringAttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetStringAttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::GetString8AttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RCmConnectionMethod::GetString8AttributeL( TUint32 aAttribute ) const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    return iImplementation->GetString8AttributeL( aAttribute );
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::SetIntAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetIntAttributeL( TUint32 aAttribute, 
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
// RCmConnectionMethod::SetBoolAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetBoolAttributeL( TUint32 aAttribute, 
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
// RCmConnectionMethod::SetStringAttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetStringAttributeL( TUint32 aAttribute, 
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
// RCmConnectionMethod::SetString8AttributeL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::SetString8AttributeL( TUint32 aAttribute, 
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
// RCmConnectionMethod::UpdateL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCmConnectionMethod::UpdateL()
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }
    
    iImplementation->UpdateL();
    }

// -----------------------------------------------------------------------------
// RCmConnectionMethod::DeleteL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::DeleteL()
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
EXPORT_C RCmDestination RCmConnectionMethod::DestinationL() const
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }        
    if (!iImplementation->Destination())
        {
        User::Leave(KErrNotSupported);
        }
    
    RCmDestination dest;
    dest.iDestinatonData = 
                iImplementation->Destination()->GetData();
    dest.iDestinatonData->IncrementRefCounter();
    return dest;
    }
// --------------------------------------------------------------------------
// RCmConnectionMethod::RCmConnectionMethod
// --------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod::RCmConnectionMethod()
    : iImplementation( NULL )
    {
    }


// -----------------------------------------------------------------------------
// RCmConnectionMethod::operator=()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod& RCmConnectionMethod::operator=(
                                const RCmConnectionMethod& aConnMethod)
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
EXPORT_C TBool RCmConnectionMethod::operator==( 
                                    RCmConnectionMethod& aConnMethod ) const
    {
    return ( iImplementation == aConnMethod.iImplementation );
    };


// --------------------------------------------------------------------------
// RCmConnectionMethod::operator!=
// --------------------------------------------------------------------------
//
EXPORT_C TBool RCmConnectionMethod::operator!=( 
                                    RCmConnectionMethod& aConnMethod ) const
    {
    return !( iImplementation == aConnMethod.iImplementation );
    };

// -----------------------------------------------------------------------------
// RCmConnectionMethod::CreateCopyL()
// -----------------------------------------------------------------------------
//
EXPORT_C RCmConnectionMethod RCmConnectionMethod::CreateCopyL()
    {
    if (!iImplementation)
        {
        User::Leave(KErrBadHandle);
        }

    RCmConnectionMethod cm;

    cm.iImplementation = iImplementation->CreateCopyL();
    cm.iImplementation->CmMgr().InsertConnectionMethodL(cm.iImplementation);

    return cm;
    }
