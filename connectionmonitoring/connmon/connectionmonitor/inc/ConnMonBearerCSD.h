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
* Description:  Declaration for bearer CSD.
*
*/

#ifndef CONNMONBEARERCSD_H
#define CONNMONBEARERCSD_H

#include "ConnMonBearer.h"

/**
 * TConnMonBearerCSD
 * Class declaration for CSD bearer
 */
class TConnMonBearerCSD : public TConnMonBearer
    {
public:
    /**
     * Constructor.
     * @param aConnMonIAP Link to IAP services
     */
    TConnMonBearerCSD( CConnMonIAP* aConnMonIAP, CConnMonCommsDatCache* aCommsDatCache );

    // From TConnMonBearer

    /**
     * Is the bearer available
     * @param aAvailable ETrue if bearer is available
     * @return errorcode
     */
    TInt GetAvailability( TBool& aAvailable ) const;

    /**
     * Bearer ID getter
     *
     * @return bearer ID
     */
    TUint BearerId() const;

    /**
     * Invariant - validates bearer
     *
     * @return ETrue if bearer is valid (internal state is correct)
     */
    TBool Valid() const;

    /**
     * Sets available IAPs of this type as available in CommsDat cache
     */
    void FlagAvailableIaps() const;
    };

#endif // CONNMONBEARERCSD_H

// End-of-file
