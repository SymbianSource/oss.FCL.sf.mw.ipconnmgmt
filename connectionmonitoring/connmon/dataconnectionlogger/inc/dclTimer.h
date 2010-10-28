/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer active object needed in logging data in timer mode.
*
*/

#ifndef CDCLTIMERAO_H
#define CDCLTIMERAO_H

#include <e32base.h>
#include <e32std.h>

#include "dcl.h"

class CEngine;

/**
*  CDclTimerAO
*  Timer that logs data periodically in timer mode.
*  @lib
*  @since
*/
class CDclTimerAO : public CActive
    {
    public: // Constructors and destructor
        CDclTimerAO( CEngine* aEngine, TInt aTimerInterval );
        void ConstructL();
        virtual ~CDclTimerAO();

    public: // New methods
        /**
        * Starts the timer mode logging for a connection.
        * @param aConnectionId Id of the connection.
        * @since
        * @return KErrNone if successful, otherwise one of the system-wide
        * error codes.
        */
        TInt Add( const TUint& aConnectionId );

        /**
        * Stops the timer mode logging for a connection.
        * @param aConnectionId Id of the connection.
        * @since
        * @return void.
        */
        void Remove( const TUint& aConnectionId );

    private: // New methods
        /**
        * Starts the timer.
        * @since
        * @return void.
        */
        void NextTimerAfter();

    private: // Methods from base classes
        /**
        * From CActive Cancels the timer.
        * @since
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Writes to the log databases.
        * @since
        * @return void
        */
        void RunL();

    private: // Data
        RTimer iTimer;
        CEngine* iEngine;
        RArray<TUint> iConnectionIds;
        TInt iTimerInterval;
    };

#endif // CDCLTIMERAO_H

// end-of-file
