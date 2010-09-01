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
* Description:  Implementation of connection method manager
*
*/

#ifndef CMMANAGERIMPL_INL
#define CMMANAGERIMPL_INL

public:
        /**
        * The cm have been updated if it was a new cm
        * than the zero id must be changed to the new one 
        *
        * @param aCM - the cm to change
        */         
        inline void ConnMethodUpdatedL( CCmPluginBaseEng* aCM )
            {
            iObjectPool->ConnMethodUpdatedL( aCM );
            iPluginImpl->ResetIfInMemory( aCM );
            }

public:
        template<typename TArrayToCleanup>
        /*
        * Pushes an array to the CleanupStack, calling it's ResetAndDestroy
        * when destroying it.
        */
        static void CleanupResetAndDestroyPushL(TArrayToCleanup& aPtr)
            {
            void (*p)(TAny*) = CCmManagerImpl::ResetAndDestroy<TArrayToCleanup>;
            CleanupStack::PushL( TCleanupItem(p, (void*)(&aPtr)) );
            }
private:
        /**
        * A cleanup function for TCleanupItem construction for
        * cleanup of pointers in an RPointerArray
        * usage: 
        * CleanupStack::PushL( TCleanupItem( CCmManagerImpl::DestroyRPointerArray<ItemTypeName>, (void*)(PointerToArray)) )
        * @return TInt
        */        
        template<typename TArrayToCleanup>
        static void ResetAndDestroy(TAny* aPtr)
        {
        TArrayToCleanup *p = static_cast <TArrayToCleanup*> (aPtr);
        p->ResetAndDestroy();
        }   
#endif // CMMANAGERIMPL_H
