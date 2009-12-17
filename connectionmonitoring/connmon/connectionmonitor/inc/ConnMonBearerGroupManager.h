/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines the mappings and services between bearers and
*                bearer groups.
*
*/

#ifndef CONNMONBEARERGROUPMANAGER_H
#define CONNMONBEARERGROUPMANAGER_H

#include <e32std.h>
#include <rconnmon.h>

/**
 * Bearer group mappings and services.
 */
NONSHARABLE_CLASS( CConnMonBearerGroupManager )
    {
public:
    /**
     * Constructor.
     */
    static CConnMonBearerGroupManager* NewL();

    /**
     * Destructor.
     */
    ~CConnMonBearerGroupManager();

    /**
    * Returns the bearer group masks related to given bearer.
    *
    * @param aBearer Detailed bearer to identify the groups wanted.
    * @param aBearerGroups1 On return, the bearer groups masked to integer.
    * @param aBearerGroups2 On return, the bearer groups masked to integer.
    * @return System-wide error code is returned if no success. Otherwise KErrNone.
    */
    TInt GetBearerGroups(
            const TConnMonBearerInfo aBearer,
            TUint& aBearerGroups1,
            TUint& aBearerGroups2 );

private:
    /**
     * Constructor.
     */
    CConnMonBearerGroupManager();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

private: // Data
    RArray<TUint> iBearerGroupMasks1;
    RArray<TUint> iBearerGroupMasks2;
    };

#endif // CONNMONBEARERGROUPMANAGER_H

// End-of-file
