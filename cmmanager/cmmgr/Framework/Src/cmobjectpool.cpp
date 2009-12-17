/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CCMObjectPool.
*
*/


#include "cmobjectpool.h"
#include "cmmanagerimpl.h"
#include <cmpluginbaseeng.h>
#include <cmpluginbase.h>
#include <cmconnectionmethoddef.h>

#include <e32def.h>
#include <e32std.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const TInt KDestGranularity     = 20;
const TInt KCMethodGranularity  = 20;
CCMObjectPool::CCMObjectPool(CCmManagerImpl& aCmManagerImpl) :
        iCmManagerImpl(aCmManagerImpl),    
        iDestinationArray(KDestGranularity, 
            _FOFF(TObjectPoolDestinationItem,iDestinationId)),
        iConnectionMethodArray(KCMethodGranularity, 
            _FOFF(TObjectPoolCMItem,iConnectionMethodId))
    {
    }

CCMObjectPool::~CCMObjectPool()
    {
    // The get and remove functions must be used in pairs
    // If the arrays have any element here than the client 
    // of this class made a programming error
    for (TInt index = 0; index < iDestinationArray.Count(); index++)
        {
        delete iDestinationArray[index].iDestinationItem;
        }    
    for (TInt index = 0; index < iConnectionMethodArray.Count(); index++)
        {
        delete iConnectionMethodArray[index].iConnectionMethodItem;
        }
    iDestinationArray.Close();
    iConnectionMethodArray.Close();
    }

CCMObjectPool::TObjectPoolDestinationItem::TObjectPoolDestinationItem(
                        CCmDestinationData*   aDestinationItem, 
                        TUint32               aDestinationId) : 
                            iDestinationId(aDestinationId),
                            iDestinationItem(aDestinationItem)
    {
    }
    
CCMObjectPool::TObjectPoolCMItem::TObjectPoolCMItem(
                        CCmPluginBaseEng* aConnectionMethodItem, 
                        TUint32           aConnectionMethodId) : 
                            iConnectionMethodId(aConnectionMethodId),
                            iConnectionMethodItem(aConnectionMethodItem)
    {
    }

CCmDestinationImpl* CCMObjectPool::GetDestinationL( TUint32 aDestinationId )
    {
    TUint32 destinationId( aDestinationId );
    if ( destinationId > 0 && destinationId < 255 )
        {
        destinationId += KCmDefaultDestinationAPTagId;
        }
    TObjectPoolDestinationItem findD( NULL, destinationId );
    TInt index = iDestinationArray.FindInUnsignedKeyOrder(findD);
    if (index == KErrNotFound)
        {
        // If the destinaton is not in the pool create it than return it.
        CCmDestinationImpl* dest = CCmDestinationImpl::NewL( iCmManagerImpl, 
                                                             destinationId );
        TObjectPoolDestinationItem aDest( 
            dest->GetData(),
            destinationId );
        User::LeaveIfError(
            iDestinationArray.InsertInUnsignedKeyOrder(aDest));
        dest->IncrementRefCounter();
        return dest;
        }
    else
        {
        // If the destination is allready in the pool than return it.
        CCmDestinationImpl* dest = CCmDestinationImpl::NewL( iCmManagerImpl, 
                                iDestinationArray[index].iDestinationItem );
        iDestinationArray[index].iDestinationItem->IncrementRefCounter();
        return dest;    
        }
    }

CCmPluginBase* CCMObjectPool::GetConnectionMethodL( TUint32 aCmId, 
                                        CCmDestinationImpl* aParentDest )
    {
    CCmPluginBase* connMethod = NULL;
    TObjectPoolCMItem findCM( NULL, aCmId );
    TInt index = iConnectionMethodArray.FindInUnsignedKeyOrder(findCM);
    if (index == KErrNotFound)
        {
        CCmPluginBaseEng* pluginBaseEng = iCmManagerImpl.DoFindConnMethL(
                                                        aCmId, aParentDest);        
        if (!pluginBaseEng) User::Leave(KErrNotFound); //There is no such plugin
        CleanupStack::PushL( pluginBaseEng );        
        connMethod = new (ELeave) CCmPluginBase(pluginBaseEng); 
        CleanupStack::PushL( connMethod );        
        TObjectPoolCMItem aCM( pluginBaseEng, aCmId );
        User::LeaveIfError(
            iConnectionMethodArray.InsertInUnsignedKeyOrder(aCM));
        pluginBaseEng->IncrementRefCounter();
        CleanupStack::Pop( connMethod );
        CleanupStack::Pop( pluginBaseEng );
        }
    else
        {
        // If the cm is allready in the pool than return it.
        iConnectionMethodArray[index].iConnectionMethodItem->IncrementRefCounter();
        connMethod = new (ELeave) CCmPluginBase(
                iConnectionMethodArray[index].iConnectionMethodItem); 
        }
    return connMethod;            
    }

CCmPluginBase* CCMObjectPool::CreateConnectionMethodL( CCmPluginBaseEng* pluginBaseEng )
    {
    CCmPluginBase* connMethod = new (ELeave) CCmPluginBase(pluginBaseEng); 
    CleanupStack::PushL( connMethod );        
    TObjectPoolCMItem aCM( pluginBaseEng, 0 );//The new id is zero till update
    User::LeaveIfError(
        iConnectionMethodArray.InsertInUnsignedKeyOrderAllowRepeats(aCM));
    pluginBaseEng->IncrementRefCounter();
    CleanupStack::Pop( connMethod );
    return connMethod;            
    }

void CCMObjectPool::InsertConnectionMethodL( CCmPluginBaseEng* pluginBaseEng )
    {       
    TObjectPoolCMItem aCM( pluginBaseEng, 0 );//The new id is zero till update
    User::LeaveIfError(
        iConnectionMethodArray.InsertInUnsignedKeyOrderAllowRepeats(aCM));
    pluginBaseEng->IncrementRefCounter();
    }

CCmPluginBase* CCMObjectPool::CreateConnectionMethodL( TUint32 aImplementationUid, 
                                        TCmPluginInitParam& aParams )
    {
    CCmPluginBaseEng* pluginBaseEng = iCmManagerImpl.DoCreateConnectionMethodL(
                                            aImplementationUid, aParams);
    CleanupStack::PushL( pluginBaseEng );        
    CCmPluginBase* connMethod = new (ELeave) CCmPluginBase(pluginBaseEng); 
    CleanupStack::PushL( connMethod );        
    TObjectPoolCMItem aCM( pluginBaseEng, 0 );//The new id is zero till update
    User::LeaveIfError(
        iConnectionMethodArray.InsertInUnsignedKeyOrderAllowRepeats(aCM));
    pluginBaseEng->IncrementRefCounter();
    CleanupStack::Pop( connMethod );
    CleanupStack::Pop( pluginBaseEng );
    return connMethod;            
    }

CCmPluginBase* CCMObjectPool::CreateConnectionMethodL( TUint32 aImplementationUid, 
                                                       TCmPluginInitParam& aParams,
                                                       TUint32 aConnMethodId )
	{
    CCmPluginBaseEng* pluginBaseEng = iCmManagerImpl.DoCreateConnectionMethodL(
                                            aImplementationUid, 
                                            aParams,
                                            aConnMethodId);
    CleanupStack::PushL( pluginBaseEng );
    CCmPluginBase* connMethod = new (ELeave) CCmPluginBase(pluginBaseEng); 
    CleanupStack::PushL( connMethod );        
    TObjectPoolCMItem aCM( pluginBaseEng, 0 );//The new id is zero till update
    User::LeaveIfError(
        iConnectionMethodArray.InsertInUnsignedKeyOrderAllowRepeats(aCM));
    pluginBaseEng->IncrementRefCounter();
    CleanupStack::Pop( connMethod );
    CleanupStack::Pop( pluginBaseEng );
    return connMethod;            
	}

CCmDestinationImpl* CCMObjectPool::GetDestinationL( const TDesC& aName )
    {
    // CmManagerImpl.GetDestinationL will leave with KErrAlreadyExists 
    // if the destination already exists
    CCmDestinationImpl* destImpl = CCmDestinationImpl::NewL( iCmManagerImpl,
         aName );
    // Yet destImpl->Id() == 0
    TObjectPoolDestinationItem aDest( destImpl->GetData(), destImpl->Id() );
    // InsertInUnsignedKeyOrderAllowRepeats is used because all
    // new destinations' Id is zero   
    User::LeaveIfError(
        iDestinationArray.InsertInUnsignedKeyOrderAllowRepeats(aDest) );
    destImpl->IncrementRefCounter();
    return destImpl;            
    }

CCmDestinationImpl* CCMObjectPool::CreateDestinationL( const TDesC& aName,
                                                       TUint32 aDestId )
    {
    // CmManagerImpl.GetDestinationL will leave with KErrAlreadyExists 
    // if the destination already exists
    CCmDestinationImpl* destImpl = CCmDestinationImpl::NewL( iCmManagerImpl,
                                                             aName,
                                                             aDestId);
    // Yet destImpl->Id() == 0
    TObjectPoolDestinationItem aDest( destImpl->GetData(), destImpl->Id() );
    // InsertInUnsignedKeyOrderAllowRepeats is used because all
    // new destinations' Id is zero   
    User::LeaveIfError(
                   iDestinationArray.InsertInUnsignedKeyOrderAllowRepeats(aDest) );
    destImpl->IncrementRefCounter();
    return destImpl;            
    }


void CCMObjectPool::RemoveDestination( CCmDestinationData* aDestination )
    {
    if (aDestination->IdIsValid())
        {
        aDestination->DecrementRefCounter();
        if (!aDestination->GetRefCounter())
            {// this was the last reference -> must delete        
            TObjectPoolDestinationItem findD( NULL, aDestination->Id() );
            TInt index = iDestinationArray.FindInUnsignedKeyOrder(findD);
            iDestinationArray.Remove(index);
            delete aDestination;
            }        
        }
    else
        { // this is a new destination which hasn't been updated yet
        // there may be more than one dest with id 0 -> must search by 
        // iDestinationItem        
        for (TInt index = 0; index < iDestinationArray.Count(); index++)
            {
            if ( iDestinationArray[index].iDestinationItem == 
                 aDestination )
                {
                aDestination->DecrementRefCounter();
                if (!aDestination->GetRefCounter())
                    {// this was the last reference -> must delete        
                    iDestinationArray.Remove(index);
                    delete aDestination;
                    }
                break;    
                }
            }                
        }
    }

void CCMObjectPool::RemoveDestination( CCmDestinationImpl* aDestination )
    {
    RemoveDestination(aDestination->GetData());
    aDestination->SetData(NULL);
    }

void CCMObjectPool::DestinationUpdated( CCmDestinationImpl* aDestination )
    {
    // there may be more than one dest with id 0 -> must search by 
    // iDestinationItem        
    for (TInt index = 0; index < iDestinationArray.Count(); index++)
        {
        if ( iDestinationArray[index].iDestinationItem == 
                aDestination->GetData() )
            {
            if (aDestination->IdIsValid()) break; //allready updated
            iDestinationArray[index].iDestinationId = aDestination->Id();
            iDestinationArray.SortUnsigned();
            break;    
            }
        }        
    }

void CCMObjectPool::ConnMethodUpdatedL( CCmPluginBaseEng* aCM )
    {
    // there may be more than one dest with id 0 -> must search by 
    // iDestinationItem        
    for (TInt index = 0; index < iConnectionMethodArray.Count(); index++)
        {
        if ( iConnectionMethodArray[index].iConnectionMethodItem == 
                aCM )
            {
            TInt cmId = aCM->GetIntAttributeL(CMManager::ECmId);
            iConnectionMethodArray[index].iConnectionMethodId = cmId;
            iConnectionMethodArray.SortUnsigned();
            break;    
            }
        }        
    }

void CCMObjectPool::RemoveConnectionMethodL( CCmPluginBase* aCM)
    {
    RemoveConnectionMethodL(aCM->Plugin());
    aCM->SetPlugin(NULL);
    }

void CCMObjectPool::RemoveConnectionMethodL( CCmPluginBaseEng* aCM)
    {
    if (aCM->IdIsValid())
        {
        aCM->DecrementRefCounter();
        if (!aCM->GetRefCounter())
            {// this was the last reference -> must delete        
            TUint32 cmId = aCM->GetIntAttributeL(CMManager::ECmId);
            TObjectPoolCMItem findD( NULL, cmId );
            TInt index = iConnectionMethodArray.FindInUnsignedKeyOrder(findD);
            iConnectionMethodArray.Remove(index);
            delete aCM;
            }        
        }
    else
        { // this is a new cm which hasn't been updated yet
        // there may be more than one cm with id 0 -> must search by pointer
        for (TInt index = 0; index < iConnectionMethodArray.Count(); index++)
            {
            if ( iConnectionMethodArray[index].iConnectionMethodItem == aCM )
                {
                aCM->DecrementRefCounter();
                if (!aCM->GetRefCounter())
                    {// this was the last reference -> must delete        
                    iConnectionMethodArray.Remove(index);
                    delete aCM;
                    }
                break;    
                }
            }
        }
    }

