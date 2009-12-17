/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares bearer abstraction class.
*
*/

#ifndef CONNMONBEARER_H
#define CONNMONBEARER_H

#include <e32def.h>

class CConnMonIAP;
class CConnMonCommsDatCache;

/**
 * Abstraction for Bearer
 * Class declaration for bearer abstraction
 */
class TConnMonBearer
    {
public:
    /**
     * Constructor.
     * @param aConnMonIAP Link to IAP services
     */
    TConnMonBearer( CConnMonIAP* aConnMonIAP, CConnMonCommsDatCache* aCommsDatCache );

    /**
     * Is the bearer available
     * @param aAvailable ETrue if bearer is available
     * @return errorcode
     */
    virtual TInt GetAvailability( TBool& aAvailable ) const = 0;

    /**
     * Bearer ID getter
     *
     * @return bearer ID
     */
    virtual TUint BearerId() const = 0;

    /**
     * Invariant - validates object
     *
     * @return ETrue if bearer is valid (internal state is correct)
     */
    virtual TBool Valid() const = 0;

    /**
     * Checks IAPs belonging to this bearer and sets the availability flag of
     * those IAPs in CommsDat cache.
     */
    virtual void FlagAvailableIaps() const = 0;

protected: // Data
    /**
     * Link to IAP services
     */
    CConnMonIAP* iConnMonIAP;

    /**
     * Link to CommsDat cache
     */
    CConnMonCommsDatCache* iCommsDatCache;
    };

#endif // CONNMONBEARER_H

// End-of-file
