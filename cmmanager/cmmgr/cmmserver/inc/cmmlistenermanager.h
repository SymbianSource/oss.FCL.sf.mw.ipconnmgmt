/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Manages all CommsDat table specific CenRep listeners.
*
*/


#ifndef CMMLISTENERMANAGER_H_
#define CMMLISTENERMANAGER_H_

#include <e32base.h>

#include "cmmdbchangelistener.h"

class CCmmCache;

// Repository for CommsDat
const TUid KCDCommsRepositoryId = { 0xCCCCCC00 };


NONSHARABLE_CLASS( CCmmListenerManager ) : public CBase
    {
public:
    /**
     * NewL.
     */
    static CCmmListenerManager* NewL( CCmmCache* aCache );

    /**
     * NewLC.
     */
    static CCmmListenerManager* NewLC( CCmmCache* aCache );

    /**
     * Destructor.
     */
    ~CCmmListenerManager();

private:
    /**
     * Constructor.
     */
    CCmmListenerManager( CCmmCache* aCache );

    /**
     * Second phase constructor.
     */
    void ConstructL();

public:
    void AddListenerL( TUint32 aIdentifier );
    TInt RemoveListener( TUint32 aIdentifier );
    void DbChangeDetected( TUint32 aIdentifier );
    void DbChangeError( TUint32 aIdentifier );

private:
    // The database cache.
    CCmmCache* iCache; // Not owned

    // Array for listener active objects.
    RPointerArray<CCmmDbChangeListener> iListeners;
    };

#endif // CMMLISTENERMANAGER_H_

// End of file
