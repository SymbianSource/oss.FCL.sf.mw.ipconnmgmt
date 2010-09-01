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
* Description:  Object pool for storing destinations and connection
*                methods.
*
*/

#ifndef CMOBJECTPOOL_H
#define CMOBJECTPOOL_H

class CCmPluginBaseEng;
class CCmPluginBase;
class CCmManagerImpl;

// User includes
#include "cmdestinationimpl.h"
#include <e32def.h>

/**
 *  Object pool to  hold destination implementations and 
 *  connetion method implementations
 *
 *  The get and remove functions must be used in pairs
 */
NONSHARABLE_CLASS( CCMObjectPool ) : public CBase
	{

	public:
		CCMObjectPool(CCmManagerImpl& aCmManagerImpl);
		virtual ~CCMObjectPool();
        /**
        * If the destinaton impl is in the pool returns a pointer to it. 
        * If the destinaton impl is not in the pool creates it.
        *
        * @param aDestinationId - ID of the destination requested
        * @return pointer to the destination - ownership passed
        */         
        CCmDestinationImpl* GetDestinationL( TUint32 aDestinationId );
        /**
        * If the destinaton impl is in the pool returns a pointer to it. 
        * If the destinaton impl is not in the pool creates it.
        *
        * @param aName - name of the destination requested
        * @return pointer to the destination - ownership passed
        */         
        CCmDestinationImpl* GetDestinationL( const TDesC& aName );

        /**
        * Creates a destination if it does not exist.
        * 
        * @param aName - name of the destination requested
        * @param aDestId - predefined id for the destination
        * @return pointer to the destination - ownership passed
        */         
        CCmDestinationImpl* CreateDestinationL( const TDesC& aName, TUint32 aDestId );

        /**
        * Returns a pointer to an allready existing connecton method
        *
        * @param aCmId - ID of the CM requested
        * @param aParentDest - parent destination
        * @return plugin instance - ownership passed
        */         
        CCmPluginBase* GetConnectionMethodL( TUint32             aCmId, 
                                             CCmDestinationImpl* aParentDest );
        /**
        * Creates cm and places it into the pool.
        *
        * @param aImplementationUid - Bearer Type
        * @param aParams Init params for the plugin
        * @return plugin instance - ownership passed
        */         
        CCmPluginBase* CreateConnectionMethodL( TUint32 aImplementationUid, 
                                 TCmPluginInitParam& aParams );

        /**
        * Creates cm and places it into the pool.
        *
        * @param aImplementationUid - Bearer Type
        * @param aParams Init params for the plugin
        * @param aConnMethodId Predefined id for the connection method.
        * @return plugin instance - ownership passed
        */         
        CCmPluginBase* CreateConnectionMethodL( TUint32 aImplementationUid, 
                                                TCmPluginInitParam& aParams,
                                                TUint32 aConnMethodId );

        /**
        * Creates cm and places it into the pool.
        *
        * @param pluginBaseEng - Object to put in the pool
        * @return plugin instance - ownership passed
        */         
        CCmPluginBase* CreateConnectionMethodL( CCmPluginBaseEng* pluginBaseEng );

        /**
        * Insert cm and places it into the pool.
        * API doesn't use CCmPluginBase. So, nothing returns
        *
        * @param pluginBaseEng - Object to put in the pool
        */         
        void InsertConnectionMethodL( CCmPluginBaseEng* pluginBaseEng );

        /**
        * Removes a destination impl from the object pool. 
        *
        * @param aDestination - the destination to remove
        */         
        void RemoveDestination( CCmDestinationImpl* aDestination );
        
        /**
        * Removes a destination data from the object pool. 
        *
        * @param aDestination - the destination to remove
        */         
        void RemoveDestination( CCmDestinationData* aDestination );
        
        /**
        * Removes a connection method impl from the object pool. 
        *
        * @param aDestinationId - ID of the connection method to remove
        */         
        void RemoveConnectionMethodL( CCmPluginBase* aCM );
        
        /**
        * Removes a connection method impl from the object pool. 
        *
        * @param aDestinationId - ID of the connection method to remove
        */         
        void RemoveConnectionMethodL( CCmPluginBaseEng* aCM );

        /**
        * This is called after the new destination have been updated 
        * This function updates the iDestinationId 
        * field in the TObjectPoolDestinationItem
        *
        * @param aDestination - the destination to change
        */         
        void DestinationUpdated( CCmDestinationImpl* aDestination );

        /**
        * This is called after the new cm have been updated 
        * This function updates the Id 
        * field
        *
        * @param aCM - the CM to change
        */         
        void ConnMethodUpdatedL( CCmPluginBaseEng* aCM );
        
    private:
		class TObjectPoolDestinationItem  
		    {
		    public:
                TObjectPoolDestinationItem(
                        CCmDestinationData* aDestinationItem, 
                        TUint32             aDestinationId);  
    		    TInt                iDestinationId;
    		    CCmDestinationData* iDestinationItem;
		    };
		class TObjectPoolCMItem  
		    {
		    public:
                TObjectPoolCMItem(
                        CCmPluginBaseEng* iConnectionMethodItem, 
                        TUint32           iConnectionMethodId); 
    		    TInt                iConnectionMethodId;
    		    CCmPluginBaseEng*   iConnectionMethodItem;
		    };
	private:
	    CCmManagerImpl&                     iCmManagerImpl;
	    // object pool for holding destination implementations
	    RArray<TObjectPoolDestinationItem>  iDestinationArray;
	    // object pool for holding connetion method implementations
	    RArray<TObjectPoolCMItem>           iConnectionMethodArray;
};

#endif // CMOBJECTPOOL_H
