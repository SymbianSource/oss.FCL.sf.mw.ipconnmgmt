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
* Description: S60 IP tier manager factory
*
*/

/**
@file s60tiermanagerfactory.h
S60 IP TM factory
*/

#ifndef S60TIERMANAGERFACTORY_H
#define S60TIERMANAGERFACTORY_H

#include <comms-infras/ss_tiermanager.h>
#include <in_sock.h>

/**
 * Factory for TM.
 */
class CS60TierManagerFactory : public ESock::CTierManagerFactoryBase
    {
public:

	enum {
		iTypeId = KAfInet ,
		iUid = 0x101F6D4A };
    /** 
     * Factory function for the factory which manages ip level meta connection providers.
     * This function also acts as the single ECom entry point into this object.
     * @param aParentContainer the parent factory container which owns this factory
     * @return factory for IP level meta connection providers
     */
    static CS60TierManagerFactory* NewL( TAny* aParentContainer );
protected:
    /** 
     * Constructor for Ip level meta connection providers.
     * @param aFactoryId the ID which this factory can be looked up by
     * @param aParentContainer the parent factory container which owns this factory
     */
	CS60TierManagerFactory( TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer );
    /**
     * DoCreateObjectL
     */
    virtual ESock::ACommsFactoryNodeId* DoCreateObjectL( ESock::TFactoryQueryBase& aQuery );
    };

#endif //S60TIERMANAGERFACTORY_H

