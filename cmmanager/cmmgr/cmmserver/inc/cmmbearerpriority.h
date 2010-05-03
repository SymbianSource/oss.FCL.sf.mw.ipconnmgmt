/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Implementation of bearer priority structure.
*
*/


#ifndef CMMBEARERPRIORITY_H_
#define CMMBEARERPRIORITY_H_

#include <e32base.h>


NONSHARABLE_CLASS( CCmmBearerPriority ) : public CBase
    {
public:
    /**
     * Two phased constuction.
     */
    static CCmmBearerPriority* NewL(
            const TDesC& aServiceType,
            const TUint& aPriority,
            const TUint& aUiPriority );

    /**
     * Two phased constuction.
     */
    static CCmmBearerPriority* NewLC(
            const TDesC& aServiceType,
            const TUint& aPriority,
            const TUint& aUiPriority );

    /**
     * Destructor.
     */
    ~CCmmBearerPriority();

private:
    /**
     * Constructor.
     */
    CCmmBearerPriority( const TUint& aPriority, const TUint& aUiPriority );

    /**
     * 2nd phase constructor.
     */
    void ConstructL( const TDesC& aServiceType );

public:
    /**
     * Get the service type.
     */
    HBufC* ServiceType() const;

    /**
     * Get the priority.
     */
    TUint Priority() const;

    /**
     * Get the UI priority.
     */
    TUint UiPriority() const;

private:
    HBufC* iServiceType; // Owned.
    TUint iPriority;
    TUint iUiPriority;
    };

#endif // CMMBEARERPRIORITY_H_

// End of file
