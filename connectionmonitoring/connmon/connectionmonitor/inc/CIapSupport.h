/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides IAP availability information.
*
*/

#ifndef __CIAPSUPPORT_H
#define __CIAPSUPPORT_H

// To initialize with an invalid value
const TUint KIapSupportUnknown = static_cast<TUint>( -1 );

class TFullIapInfo;

/**
 * CIapSupport
 * @lib CONNMON.LIB
 */
NONSHARABLE_CLASS( CIapSupport ) : public CBase
    {
public: // Constructors and destructor
    static CIapSupport* NewL();
    virtual ~CIapSupport();

public: // New methods
    /**
     * Gets (reads) IAPs (iapID) from commDB.
     * @since
     * @param aBearerId The beared ID. Identifies the bearer.
     * @param aIapBuf On return, contains IAPs matching the bearer ID.
     * @return void.
     */
    void GetIapsFromCommDbL(
            const TUint& aBearerId,
            TFullIapInfo& aIapBuf ) const;

private: // New methods
    /**
     * Appends IAPs (iapID and netID) from commDB to aIapBuf.
     * @since
     * @param aBearerId The beared ID. Identifies the bearer.
     * @param aIapBuf On return, contains IAPs matching the bearer ID.
     * @return void.
     */
    void AppendIapsFromCommDbL(
            const TUint& aBearerId,
            TFullIapInfo& aIapBuf ) const;

private:
    CIapSupport();
    };

#endif // __CIAPSUPPORT_H

// End-of-file
