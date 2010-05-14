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
* Connection method manager server.
*
*/


#ifndef CMMSERVER_H_
#define CMMSERVER_H_

#include <e32base.h>

#include "cmmserverdefs.h"
#include "cmmanagerimpl.h"

class CCmmDelayedShutdown;


/**
 * The main server class
 */
NONSHARABLE_CLASS( CCmmServer ) : public CPolicyServer
    {
public:

    /**
     * Two phased construction.
     */
    static CCmmServer* NewL();

    /**
     * Two phased construction.
     */
    static CCmmServer* NewLC();

    /**
     * Destroy the object and release all memory objects
     */
    ~CCmmServer();

    /**
     * Increments the count of active sessions for this server.
     */
    void IncrementSessions();

    /**
     * Decrements the count of the active sessions for this server.
     * If no more sessions are in use the server terminates.
     */
    void DecrementSessions();

     /**
     * Produces object containers (needed by subsessions) with unique ids within the server.
     * @return A object container.
     */
    CObjectCon* NewContainerL();

    /**
     * Removes from index and deletes the object container.
     * @param aContainer The container to be deleted.
     * @return A object container.
     */
    void RemoveContainer( CObjectCon* aContainer );

    CCmManagerImpl* CmManager();

    /**
     * Check for restrictions for adding an embedded destination from destination
     * instances of all clients.
     * @param aDestinationId The ID of the destination where a destination is
     * beeing embedded.
     * @param aEmbeddedDestinationId The ID of the destination that is beeing
     * embedded.
     */
    TBool EmbeddedDestinationConflictsFromAllSessions(
            const TUint32& aDestinationId,
            const TUint32& aEmbeddedDestinationId );

    /**
     * Notify each session about a destination/connection method that has been
     * updated to, or deleted from, database.
     */
    void RefreshHandlesForAllSessions( const TUint32& aId );
    
    /**
     * Custom security checking for IPCs marked with TSpecialCase::ECustomCheck.
     * @param aMsg The IPC message from the client.
     * @return The result of the security check (TCustomResult::EPass/TCustomResult::EFail).
     */
    TCustomResult CustomSecurityCheckL(
            const RMessage2& aMsg,
            TInt& aAction,
            TSecurityInfo& aMissing );

    /**
     * Checks the client's capabilities in the middle of request handling. Some
     * checks cannot be done with CustomSecurityCheckL() at the start phase of
     * the operation.
     * @param aMsg The IPC message from the client.
     * @return Returns ETrue if client has the needed capabilities. Otherwise
     * returns EFalse.
     */
    CPolicyServer::TCustomResult CapabilityCheckWithProtection( const RMessage2& aMessage );

protected: // From CActive
    /**
     * Process any errors
     *
     * @param aError the leave code reported
     */
    TInt RunError( TInt aError );

private:

    /**
     * Perform the first phase of two phase construction
     *
     * @param Priority for this thread
     */
    CCmmServer( TInt aPriority );

    /**
     * Perform the second phase construction
     */
    void ConstructL() ;

private: // From CServer

    /**
     * Create a time server session, and return a pointer to the
     * created object
     *
     * @param Version
     *
     * @result Pointer to the new session
     */
    CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;

private:
    /** The number of client-server sessions */
    TInt iSessionCount;

    static const TUint iCmmRangeCount;
    static const TInt iCmmRanges[];
    static const TUint8 iCmmElementIndex[];

    static const CPolicyServer::TPolicyElement iCmmElements[];
    static const CPolicyServer::TPolicy iCmmPolicy;

    CCmManagerImpl* iCmManager;

    // Object container index
    CObjectConIx* iContainerIndex;

    // Pointer to the shutdown timer object
    CCmmDelayedShutdown* iShutdown;
    };

/**
* CCmmDelayedShutdown
* Shutdown timer that closes Connection Method Manager server after a period.
*/
NONSHARABLE_CLASS( CCmmDelayedShutdown ) : public CActive
    {
public:
    CCmmDelayedShutdown( CCmmServer* aServer );
    void ConstructL();
    ~CCmmDelayedShutdown();

public:
    /**
     * Starts shutdown timer.
     * @return KErrNone if OK, otherwise a system wide error code.
     */
    void Start();

private:
    /**
     * From CActive Cancels the shutdown timer.
     */
    void DoCancel();

    /**
     * From CActive Shuts the server down.
     */
    void RunL();

private: // Data
    RTimer iTimer;
    CCmmServer* iServer;
    };


#endif // CMMSERVER_H_

// End of file
