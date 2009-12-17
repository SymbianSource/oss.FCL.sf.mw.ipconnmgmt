/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: S60 MCPR's factory
*
*/

/**
@file s60mcprfactory.h
S60 MCPR's factory.
*/


#ifndef S60MCPRFACTORY_H
#define S60MCPRFACTORY_H

#include <comms-infras/ss_metaconnprov.h>

/**
 * CS60MetaConnectionProviderFactory factory class.
 */
class CS60MetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
    {
public:
    enum { iUid = 0x101F6D4B };
    /** 
     * Factory function for the factory which manages network level meta connection providers.
     * This function also acts as the single ECom entry point into this object.
     * @param aParentContainer the parent factory container which owns this factory
     * @return factory for IP level meta connection providers
     */
    static CS60MetaConnectionProviderFactory* NewL(TAny* aParentContainer);
protected:
    /** 
     * Constructor for Network level meta connection providers.
     * @param aFactoryId the ID which this factory can be looked up by
     * @param aParentContainer the parent factory container which owns this factory
     */
    CS60MetaConnectionProviderFactory(TUid aFactoryUid, ESock::CMetaConnectionFactoryContainer& aParentContainer);
    /** 
     * Constructor for Network level meta connection provider.
     * @return pointer to new S60 MCPR.
     */
    ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
    };

#endif //S60MCPRFACTORY_H
