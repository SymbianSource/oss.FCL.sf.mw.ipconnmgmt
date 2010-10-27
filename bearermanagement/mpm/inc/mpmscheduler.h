/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Active scheduler for MPM server.
*
*/

#ifndef MPMSCHEDULER_H
#define MPMSCHEDULER_H

#include <e32base.h>

/**
 * Mobility Policy Manager active scheduler. Required to override the default
 * error handling from CActiveScheduler.
 */
NONSHARABLE_CLASS( CMpmScheduler ) : public CActiveScheduler
    {
public:
    /**
     * NewL.
     */
    static CMpmScheduler* NewL();

    /**
     * NewLC.
     */
    static CMpmScheduler* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMpmScheduler();

private:
    /**
     * Constructor.
     */
    CMpmScheduler();

    /**
     * 2nd level constructor.
     */
    void ConstructL();

public:
    /**
     * Set the MPM server.
     * @param aMpmServer Pointer to the MPM server.
     */
    void SetMpmServer( CServer2* aMpmServer );

    /**
     * Error handling for the active scheduler.
     * Handles any leave occuring in an active object's RunL() function that
     * hasn't been properly handled in the active object's own RunError()
     * function.
     * @param aError Leave code from active object.
     */
    void Error( TInt aError ) const;

private:
    /**
     * Pointer to the MPM server.
     */
    CServer2* iMpmServer; // Not owned
    };

#endif // MPMSCHEDULER_H

// End of file
